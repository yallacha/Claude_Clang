#!/usr/bin/env bash
# 구문/분기/함수 커버리지 측정 (LLVM source-based coverage, Google Test)
# 사용법: coverage.sh "<프로덕션 소스...>" "<테스트 소스...>" [출력디렉터리]
#   소스가 여러 개면 따옴표로 묶어 공백으로 구분한다.
# 환경변수:
#   CLANG          컴파일러 (기본 clang++)
#   EXTRA_FLAGS    추가 컴파일 옵션 (예: "-std=c++17 -Iinclude")
#   LIBS           링크 라이브러리 (기본 gtest/gmock/pthread; gtest_main이 main 제공)
#   IGNORE_REGEX   리포트에서 제외할 파일 경로 정규식
#   LLVM_PROFDATA, LLVM_COV
# 종료 코드: 테스트가 하나라도 실패하면 0이 아님 (테스트 성공률 100% 판정).
#            리포트는 테스트 실패와 무관하게 생성한다.
set -uo pipefail

SRC="${1:?프로덕션 소스 필요}"
TEST="${2:?테스트 소스 필요}"
OUT="${3:-coverage-out}"
CLANG="${CLANG:-clang++}"
EXTRA_FLAGS="${EXTRA_FLAGS:-}"
LIBS="${LIBS:--lgtest -lgtest_main -lgmock -lpthread}"
IGNORE_REGEX="${IGNORE_REGEX:-(/tests?/|/usr/|gtest|gmock)}"
PROFDATA="${LLVM_PROFDATA:-llvm-profdata}"
COV="${LLVM_COV:-llvm-cov}"

mkdir -p "$OUT"
# shellcheck disable=SC2086
"$CLANG" -g -O0 -fprofile-instr-generate -fcoverage-mapping $EXTRA_FLAGS \
    $SRC $TEST $LIBS -o "$OUT/test_bin" || { echo "오류: 빌드 실패" >&2; exit 2; }

TEST_RC=0
LLVM_PROFILE_FILE="$OUT/test.profraw" "$OUT/test_bin" \
    --gtest_output="xml:$OUT/test-results.xml" || TEST_RC=$?

"$PROFDATA" merge -sparse "$OUT/test.profraw" -o "$OUT/test.profdata" || exit 2

# report: Regions / Functions / Lines / Branches 열이 출력된다 (Functions = 함수 커버리지)
"$COV" report "$OUT/test_bin" -instr-profile="$OUT/test.profdata" \
    -ignore-filename-regex="$IGNORE_REGEX" | tee "$OUT/coverage-report.txt"
"$COV" show "$OUT/test_bin" -instr-profile="$OUT/test.profdata" \
    -ignore-filename-regex="$IGNORE_REGEX" \
    -show-branches=count -show-line-counts-or-regions > "$OUT/coverage.txt"

echo "상세 리포트: $OUT/coverage.txt, 테스트 결과(gtest XML):$OUT/test-results.xml"
if [ "$TEST_RC" -ne 0 ]; then
    echo "실패: 테스트 실패 (종료 코드 $TEST_RC)" >&2
fi
exit "$TEST_RC"
