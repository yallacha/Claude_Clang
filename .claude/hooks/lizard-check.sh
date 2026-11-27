#!/usr/bin/env bash
# Stop hook: 구현이 끝났을 때(응답 종료 시) Lizard 메트릭(CCN<=10, 함수 라인<=80)을 검사한다.
# 위반이 있으면 종료 코드 2로 응답 종료를 막고, 위반 내용을 Claude 에게 전달해 수정하게 한다.
# 개발과 CI 가 같은 도구를 쓰도록 claudeclangbuild 이미지 안에서 실행한다.
cd "${CLAUDE_PROJECT_DIR:-.}" || exit 0

INPUT="$(cat 2>/dev/null || true)"
# 이미 한 번 막힌 뒤의 재시도에서는 무한 루프를 피하기 위해 경고만 남긴다 (CI 가 최종 게이트).
ACTIVE=0
echo "$INPUT" | grep -Eq '"stop_hook_active"[[:space:]]*:[[:space:]]*true' && ACTIVE=1

# 검사할 C 소스가 없으면 통과
if [ -z "$(find src -type f -name '*.c' 2>/dev/null | head -n1)" ]; then exit 0; fi

if ! command -v docker >/dev/null 2>&1 || ! docker image inspect claudeclangbuild:latest >/dev/null 2>&1; then
    echo "경고: docker 또는 claudeclangbuild 이미지가 없어 Lizard 검사를 건너뜁니다. (CI에서 검사됨)" >&2
    exit 0
fi

WORK="$(pwd -W 2>/dev/null || pwd)"
OUT="$(MSYS_NO_PATHCONV=1 docker run --rm -v "$WORK:/work" -w /work claudeclangbuild:latest ci/quality.sh lizard 2>&1)"
RC=$?
if [ "$RC" -ne 0 ]; then
    echo "Lizard 기준 위반(순환복잡도 10 이하, 함수 라인수 80 이하). 함수를 분리해 수정하세요:" >&2
    echo "$OUT" >&2
    [ "$ACTIVE" -eq 1 ] && exit 0
    exit 2
fi
exit 0
