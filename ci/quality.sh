#!/usr/bin/env bash
# 소스코드 품질 기준 검사 (CLAUDE.md 2장). 개발과 CI 모두 ClaudeCLangBuild 이미지 안에서 실행한다.
#
# 사용법: ci/quality.sh <cppcheck|lizard|cloc|tdd|unit|integration|system|all>
#   cppcheck     CppCheck 위반 0건 (억제 사용 금지)
#   lizard       순환복잡도 10 이하, 함수 라인수 80 이하
#   cloc         코드/주석 라인 집계 (주석률 보고)
#   tdd          tests/tdd      : 테스트 성공률 100% (별도 결과/커버리지, 게이트는 성공률)
#   unit         tests/unit     : MC/DC 100%, 테스트 성공률 100%
#   integration  tests/integration : 함수 커버리지 100%, 테스트 성공률 100%
#   system       tests/system : 테스트 성공률 100% (요구사항 커버리지 100%는 docs/system-test 의 추적성 검사로 판정)
#   all          위 전체 (하나라도 실패하면 종료 코드 1)
#
# 레이아웃: 프로덕션 src/(*.c), 헤더 include/ 또는 src/, 테스트 tests/{tdd,unit,integration,system}/(*.cpp)
# 결과: build/reports/<항목>/ (Jenkins 가 수집). 대상 소스가 없으면 SKIPPED 로 표시하고 통과한다.
set -uo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT" || exit 2
REPORTS="${REPORTS_DIR:-build/reports}"
SCRIPTS=".claude/skills/unit-test/scripts"
export EXTRA_FLAGS="${EXTRA_FLAGS:--Iinclude -Isrc -Itests/common -DECL_TEST_BUILD}"

SRC_FILES="$(find src -type f -name '*.c' 2>/dev/null | sort | tr '\n' ' ')"
FAILED=0

has_src() { [ -n "${SRC_FILES// /}" ]; }
skip()    { echo "[SKIPPED] $1: $2"; }
fail()    { echo "[FAIL] $1"; FAILED=1; }
pass()    { echo "[PASS] $1"; }

tests_in() { find "tests/$1" -type f \( -name '*.cpp' -o -name '*.cc' \) 2>/dev/null | sort | tr '\n' ' '; }

step_cppcheck() {
    if ! has_src; then skip cppcheck "src/ 에 C 소스 없음"; return; fi
    mkdir -p "$REPORTS/cppcheck"
    # 억제 주석 사용 금지 (예외 없음)
    if grep -rEn "cppcheck-suppress" src include 2>/dev/null; then fail "cppcheck: 억제 주석 사용 금지"; return; fi
    # shellcheck disable=SC2086
    cppcheck --std=c11 --language=c --enable=warning,style,performance,portability \
        --check-level=exhaustive --error-exitcode=1 -Iinclude -Isrc \
        --xml --xml-version=2 $SRC_FILES 2> "$REPORTS/cppcheck/cppcheck.xml"
    local rc=$? count
    count="$(grep -c '<error ' "$REPORTS/cppcheck/cppcheck.xml")"
    echo "CppCheck 위반 수: $count"
    if [ "$rc" -eq 0 ] && [ "$count" -eq 0 ]; then pass "cppcheck"; else fail "cppcheck: 위반 있음"; fi
}

step_lizard() {
    if ! has_src; then skip lizard "src/ 에 C 소스 없음"; return; fi
    mkdir -p "$REPORTS/lizard"
    lizard src -l c -C 10 -L 80 --xml > "$REPORTS/lizard/lizard.xml" 2>/dev/null
    # -w: 기준 초과 함수만 경고로 출력하고, 하나라도 있으면 종료 코드가 0이 아니다.
    if lizard src -l c -C 10 -L 80 -w; then pass "lizard (CCN<=10, 라인<=80)"; else fail "lizard: 기준 초과 함수 있음"; fi
}

step_cloc() {
    mkdir -p "$REPORTS/cloc"
    cloc src include tests --by-file --xml --out="$REPORTS/cloc/cloc.xml" 2>/dev/null \
        || cloc . --exclude-dir=build,.git --xml --out="$REPORTS/cloc/cloc.xml" 2>/dev/null
    cloc . --exclude-dir=build,.git,node_modules --quiet | tail -n 15
    pass "cloc (보고용, 게이트 아님)"
}

# gate_run <이름> <측정 스크립트> <테스트 디렉터리> <지표|-> [최소%]
gate_run() {
    local name="$1" script="$2" dir="$3" metric="$4" min="${5:-100}" tsrc out
    if ! has_src; then skip "$name" "src/ 에 C 소스 없음"; return; fi
    tsrc="$(tests_in "$dir")"
    if [ -z "${tsrc// /}" ]; then fail "$name: tests/$dir 에 테스트가 없음 (프로덕션 소스가 있으면 테스트가 필요)"; return; fi
    out="$REPORTS/$name"
    "$SCRIPTS/$script" "$SRC_FILES" "$tsrc" "$out"; local rc=$?
    if [ "$rc" -ne 0 ]; then fail "$name: 테스트 성공률 100% 미달 또는 빌드 실패 (rc=$rc)"; return; fi
    if [ "$metric" != "-" ]; then
        python3 ci/check_coverage.py "$out/summary.json" "$metric" "$min" || { fail "$name: $metric 커버리지 $min% 미달"; return; }
    fi
    if [ "$name" = "system" ]; then
        python3 ci/check_system_coverage.py 100 || { fail "system: 요구사항 커버리지 100% 미달"; return; }
    fi
    # Jenkins 커버리지 플러그인용 Cobertura 변환
    lcov_cobertura "$out/lcov.info" --output "$out/cobertura.xml" >/dev/null 2>&1 || true
    pass "$name"
}

case "${1:-all}" in
    cppcheck)    step_cppcheck ;;
    lizard)      step_lizard ;;
    cloc)        step_cloc ;;
    tdd)         gate_run tdd         coverage.sh tdd         -         ;;
    unit)        gate_run unit        mcdc.sh     unit        mcdc      ;;
    integration) gate_run integration coverage.sh integration functions ;;
    system)      gate_run system      coverage.sh system      -         ;;
    all)         step_cppcheck; step_lizard; step_cloc
                 gate_run tdd         coverage.sh tdd         -
                 gate_run unit        mcdc.sh     unit        mcdc
                 gate_run integration coverage.sh integration functions
                 gate_run system      coverage.sh system      - ;;
    *) echo "사용법: $0 <cppcheck|lizard|cloc|tdd|unit|integration|system|all>" >&2; exit 2 ;;
esac

exit "$FAILED"
