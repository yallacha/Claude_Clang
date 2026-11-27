#!/usr/bin/env bash
# MC/DC 커버리지 측정 (LLVM 18+ -fcoverage-mcdc, Google Test)
# 사용법: mcdc.sh "<프로덕션 소스...>" "<테스트 소스...>" [출력디렉터리]
#   소스가 여러 개면 따옴표로 묶어 공백으로 구분한다. 환경변수는 _common.sh 참고.
# 종료 코드: 테스트 실패 시 0이 아님(테스트 성공률 100% 판정), 빌드 실패 시 2.
#            리포트는 테스트 실패와 무관하게 생성한다.
# 참고: LLVM MC/DC 는 하나의 결정(decision)에 포함되는 조건 수에 제한이 있다(기본 6개).
set -uo pipefail
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=_common.sh
. "$HERE/_common.sh"

SRC="${1:?프로덕션 소스 필요}"
TEST="${2:?테스트 소스 필요}"
OUT="${3:-mcdc-out}"

VER="$("$CXX" --version | sed -n 's/.*version \([0-9][0-9]*\).*/\1/p' | head -n1)"
if [ -n "$VER" ] && [ "$VER" -lt 18 ]; then
    echo "오류: MC/DC 는 LLVM 18 이상이 필요합니다 (현재 $VER)" >&2
    exit 2
fi

TEST_RC=0
build_and_measure "$OUT" "$SRC" "$TEST" \
    "-fprofile-instr-generate -fcoverage-mapping -fcoverage-mcdc" || TEST_RC=$?
if [ "$TEST_RC" -eq 2 ]; then echo "오류: 빌드/프로파일 병합 실패" >&2; exit 2; fi

"$COV" report "$OUT/test_bin" -instr-profile="$OUT/test.profdata" \
    -ignore-filename-regex="$IGNORE_REGEX" -show-mcdc-summary | tee "$OUT/mcdc-report.txt"
"$COV" show "$OUT/test_bin" -instr-profile="$OUT/test.profdata" \
    -ignore-filename-regex="$IGNORE_REGEX" -show-mcdc > "$OUT/mcdc.txt"
export_summaries "$OUT"

echo "MC/DC 상세 리포트(미충족 조건 확인): $OUT/mcdc.txt, 요약: $OUT/summary.json, 테스트 결과(gtest XML): $OUT/test-results.xml"
[ "$TEST_RC" -eq 0 ] || echo "실패: 테스트 실패 (종료 코드 $TEST_RC)" >&2
exit "$TEST_RC"
