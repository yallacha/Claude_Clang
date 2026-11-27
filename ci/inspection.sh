#!/usr/bin/env bash
# 검사(INS)/빌드(BLD)로 검증하는 요구사항의 자동 검사 (시스템 테스트 케이스 TC-I-001~, docs/system-test).
# 결과: build/reports/inspect/inspect.txt . 하나라도 실패하면 종료 코드 1.
set -uo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT" || exit 2
OUT="${REPORTS_DIR:-build/reports}/inspect"
mkdir -p "$OUT"
LOG="$OUT/inspect.txt"
: > "$LOG"
FAILED=0

report() { # <상태> <요구사항> <설명>
    echo "[$1] $2: $3" | tee -a "$LOG"
    [ "$1" = "PASS" ] || FAILED=1
}

# SRQ-CON-001: 제품 코어는 C11 (src 아래 C 소스만, 컴파일 -std=c11 -pedantic 경고 0)
if [ -z "$(find src -type f \( -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' \) 2>/dev/null)" ] && \
   grep -q "C_STANDARD 11" CMakeLists.txt; then
    bad=0
    for f in src/*.c; do
        clang -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -c "$f" -o /dev/null 2>>"$OUT/warnings.txt" || bad=1
    done
    [ "$bad" -eq 0 ] && report PASS SRQ-CON-001 "src는 C 소스만이며 -std=c11 -Wall -Wextra -Wpedantic -Werror 컴파일 경고 0" \
                      || report FAIL SRQ-CON-001 "C11 컴파일 경고/오류 (build/reports/inspect/warnings.txt)"
else
    report FAIL SRQ-CON-001 "src에 C++ 소스가 있거나 CMakeLists.txt에 C_STANDARD 11이 없음"
fi

# SRQ-CON-002, SRQ-CON-005: 시스템 시계를 읽지 않는다 (시각은 timestamp_s 주입)
if grep -rEn '\b(time|clock|gettimeofday|clock_gettime|timespec_get|localtime|gmtime|difftime)[[:space:]]*\(' src include >"$OUT/clock_calls.txt"; then
    report FAIL SRQ-CON-002 "시스템 시계 호출 발견 (build/reports/inspect/clock_calls.txt)"
    report FAIL SRQ-CON-005 "시스템 시계 호출 발견"
else
    report PASS SRQ-CON-002 "시간 값은 timestamp_s/source_timestamp_s 입력뿐(시계 함수 호출 없음)"
    report PASS SRQ-CON-005 "src/include에 time/clock/gettimeofday/clock_gettime 등 시스템 시계 호출 없음"
fi

# SRQ-CON-003, SRQ-CON-006: 영상/음성/개인 식별자 필드가 입력·저장 구조체에 없음, 이벤트 필드 집합 고정
if grep -Ein '\b(face|image|video|audio|voice_data|name|vin|plate|person|user_id|driver_id|phone|email)\b[[:space:]]*(;|\[)' include/ecl_types.h >"$OUT/pii_fields.txt"; then
    report FAIL SRQ-CON-003 "금지 필드 후보 발견 (build/reports/inspect/pii_fields.txt)"
else
    report PASS SRQ-CON-003 "입력 구조체(ecl_snapshot_t)에 영상·음성·개인 식별자 필드 없음"
fi
event_fields="$(awk '/typedef struct \{/{buf=""} {buf=buf $0 "\n"} /\} ecl_event_t;/{print buf}' include/ecl_types.h \
    | grep -E '^[[:space:]]+[a-z_0-9]+[[:space:]]+[a-z_0-9]+(\[[A-Z_]+\])?;' | sed -E 's/\[[A-Z_]+\]//; s/;//' | awk '{print $2}' | sort | tr '\n' ' ')"
expected="command_rejected event_id lock reason_code state timestamp_s validity "
if [ "$event_fields" = "$expected" ]; then
    report PASS SRQ-CON-006 "저장 구조체 ecl_event_t 필드 = {event_id, timestamp_s, lock, state, reason_code, command_rejected, validity}"
else
    report FAIL SRQ-CON-006 "ecl_event_t 필드 집합 불일치: $event_fields"
fi

# SRQ-CON-007: 성인 존재는 boolean 1개 필드
if [ "$(grep -Ec '\badult_present\b' include/ecl_types.h)" -eq 1 ] && grep -Eq '^[[:space:]]+int[[:space:]]+adult_present;' include/ecl_types.h; then
    report PASS SRQ-CON-007 "adult_present는 int(boolean) 단일 필드로만 정의"
else
    report FAIL SRQ-CON-007 "adult_present 정의가 단일 boolean 필드가 아님"
fi

# SRQ-CON-004: 도어 래치/구동기 HW 직접 구동 없음 (장치·시스템 헤더, 인라인 어셈블리, volatile 레지스터 접근 없음)
if grep -rEn '#include[[:space:]]*<(sys/|linux/|windows|dev/|asm/)|__asm__|\basm[[:space:]]*\(|\bvolatile\b' src include >"$OUT/hw_access.txt"; then
    report FAIL SRQ-CON-004 "HW 접근 후보 발견 (build/reports/inspect/hw_access.txt)"
else
    report PASS SRQ-CON-004 "장치/시스템 헤더, 인라인 어셈블리, volatile 접근 없음(논리 출력만 생성)"
fi

# SRQ-FUN-039: 제품 빌드 목적 파일에 시험용 접근자가 없다
nm_ok=1
for f in ecl_log ecl_core; do
    clang -std=c11 -Iinclude -c "src/$f.c" -o "$OUT/$f.product.o" || nm_ok=0
done
if [ "$nm_ok" -eq 1 ] && ! nm "$OUT/ecl_log.product.o" "$OUT/ecl_core.product.o" | grep -Eq 'ecl_log_test_set_counter|ecl_core_test_get_log'; then
    report PASS SRQ-FUN-039 "ECL_TEST_BUILD 미정의 제품 빌드의 목적 파일 심볼에 시험용 접근자 없음"
else
    report FAIL SRQ-FUN-039 "제품 빌드 목적 파일에 시험용 접근자 심볼이 있거나 컴파일 실패"
fi

# HITL 태그: 전개되지 않은 자리표시자가 소스/시험에 남아 있지 않다 (CLAUDE.md 2장 주석 규칙)
if grep -rln "@@HITL@@" src include tests >"$OUT/hitl_placeholders.txt"; then
    report FAIL HITL "Human-in-the-Loop 태그 자리표시자(@@HITL@@) 미전개 파일: $(tr "
" " " < "$OUT/hitl_placeholders.txt")"
else
    report PASS HITL "모든 소스/시험 주석의 AI 작성·사용자 리뷰 태그가 전개되어 있음"
fi

exit "$FAILED"
