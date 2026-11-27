#!/usr/bin/env bash
# MC/DC 커버리지 측정 (LLVM 18+ -fcoverage-mcdc, Google Test)
# 사용법: mcdc.sh "<프로덕션 소스...>" "<테스트 소스...>" [출력디렉터리]
#   소스가 여러 개면 따옴표로 묶어 공백으로 구분한다.
# 환경변수: coverage.sh 와 동일 (CLANG, EXTRA_FLAGS, LIBS, IGNORE_REGEX, LLVM_PROFDATA, LLVM_COV)
# 종료 코드: 테스트가 하나라도 실패하면 0이 아님 (테스트 성공률 100% 판정).
#            리포트는 테스트 실패와 무관하게 생성한다.
# 참고: LLVM MC/DC 는 하나의 결정(decision)에 포함되는 조건 수에 제한이 있다(기본 6개).
set -uo pipefail

SRC="${1:?프로덕션 소스 필요}"
TEST="${2:?테스트 소스 필요}"
OUT="${3:-mcdc-out}"
CLANG="${CLANG:-clang++}"
EXTRA_FLAGS="${EXTRA_FLAGS:-}"
LIBS="${LIBS:--lgtest -lgtest_main -lgmock -lpthread}"
IGNORE_REGEX="${IGNORE_REGEX:-(/tests?/|/usr/|gtest|gmock)}"
PROFDATA="${LLVM_PROFDATA:-llvm-profdata}"
COV="${LLVM_COV:-llvm-cov}"

VER="$("$CLANG" --version | sed -n 's/.*version \([0-9][0-9]*\).*/\1/p' | head -n1)"
if [ -n "$VER" ] && [ "$VER" -lt 18 ]; then
    echo "오류: MC/DC 는 LLVM 18 이상이 필요합니다 (현재 $VER)" >&2
    exit 2
fi

mkdir -p "$OUT"
# shellcheck disable=SC2086
"$CLANG" -g -O0 -fprofile-instr-generate -fcoverage-mapping -fcoverage-mcdc $EXTRA_FLAGS \
    $SRC $TEST $LIBS -o "$OUT/test_bin" || { echo "오류: 빌드 실패" >&2; exit 2; }

TEST_RC=0
LLVM_PROFILE_FILE="$OUT/test.profraw" "$OUT/test_bin" \
    --gtest_output="xml:$OUT/test-results.xml" || TEST_RC=$?

"$PROFDATA" merge -sparse "$OUT/test.profraw" -o "$OUT/test.profdata" || exit 2

"$COV" report "$OUT/test_bin" -instr-profile="$OUT/test.profdata" \
    -ignore-filename-regex="$IGNORE_REGEX" -show-mcdc-summary | tee "$OUT/mcdc-report.txt"
"$COV" show "$OUT/test_bin" -instr-profile="$OUT/test.profdata" \
    -ignore-filename-regex="$IGNORE_REGEX" -show-mcdc > "$OUT/mcdc.txt"

echo "MC/DC 상세 리포트(미충족 조건 확인): $OUT/mcdc.txt, 테스트 결과(gtest XML): $OUT/test-results.xml"
if [ "$TEST_RC" -ne 0 ]; then
    echo "실패: 테스트 실패 (종료 코드 $TEST_RC)" >&2
fi
exit "$TEST_RC"
