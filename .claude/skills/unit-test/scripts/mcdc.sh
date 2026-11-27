#!/usr/bin/env bash
# MC/DC 커버리지 측정 (LLVM 18+ -fcoverage-mcdc)
# 사용법: mcdc.sh <소스.c> <테스트.c> [출력디렉터리]
#   테스트.c 는 main()을 포함해야 한다. 소스가 여러 개면 따옴표로 묶어 전달한다.
# 참고: LLVM MC/DC 는 하나의 결정(decision)에 포함되는 조건 수에 제한이 있다(기본 6개).
set -euo pipefail

SRC="${1:?소스 파일 필요}"
TEST="${2:?테스트 파일 필요}"
OUT="${3:-mcdc-out}"
CLANG="${CLANG:-clang}"
PROFDATA="${LLVM_PROFDATA:-llvm-profdata}"
COV="${LLVM_COV:-llvm-cov}"

VER="$("$CLANG" --version | sed -n 's/.*version \([0-9][0-9]*\).*/\1/p' | head -n1)"
if [ -n "$VER" ] && [ "$VER" -lt 18 ]; then
    echo "오류: MC/DC 는 LLVM 18 이상이 필요합니다 (현재 $VER)" >&2
    exit 2
fi

mkdir -p "$OUT"
# shellcheck disable=SC2086
"$CLANG" -g -O0 -fprofile-instr-generate -fcoverage-mapping -fcoverage-mcdc \
    $SRC "$TEST" -o "$OUT/test_bin"
LLVM_PROFILE_FILE="$OUT/test.profraw" "$OUT/test_bin"
"$PROFDATA" merge -sparse "$OUT/test.profraw" -o "$OUT/test.profdata"

"$COV" report "$OUT/test_bin" -instr-profile="$OUT/test.profdata" -show-mcdc-summary
"$COV" show   "$OUT/test_bin" -instr-profile="$OUT/test.profdata" \
    -show-mcdc > "$OUT/mcdc.txt"
echo "MC/DC 상세 리포트(미충족 조건 확인): $OUT/mcdc.txt"
