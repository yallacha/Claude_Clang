#!/usr/bin/env bash
# 구문/분기/함수 커버리지 측정 (LLVM source-based coverage)
# 사용법: coverage.sh <소스.c> <테스트.c> [출력디렉터리]
#   테스트.c 는 main()을 포함해야 한다. 소스가 여러 개면 따옴표로 묶어 전달한다.
set -euo pipefail

SRC="${1:?소스 파일 필요}"
TEST="${2:?테스트 파일 필요}"
OUT="${3:-coverage-out}"
CLANG="${CLANG:-clang}"
PROFDATA="${LLVM_PROFDATA:-llvm-profdata}"
COV="${LLVM_COV:-llvm-cov}"

mkdir -p "$OUT"
# shellcheck disable=SC2086
"$CLANG" -g -O0 -fprofile-instr-generate -fcoverage-mapping $SRC "$TEST" -o "$OUT/test_bin"
LLVM_PROFILE_FILE="$OUT/test.profraw" "$OUT/test_bin"
"$PROFDATA" merge -sparse "$OUT/test.profraw" -o "$OUT/test.profdata"

"$COV" report "$OUT/test_bin" -instr-profile="$OUT/test.profdata"
"$COV" show   "$OUT/test_bin" -instr-profile="$OUT/test.profdata" \
    -show-branches=count -show-line-counts-or-regions > "$OUT/coverage.txt"
echo "상세 리포트: $OUT/coverage.txt"
