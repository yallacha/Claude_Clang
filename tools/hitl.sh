#!/usr/bin/env bash
# Doxygen HITL 태그 블록을 소스의 "@@HITL@@" 자리표시자에 전개한다 (CLAUDE.md 2장).
# 사용: tools/hitl.sh <파일...>   (이미 전개된 파일은 변경하지 않음)
for f in "$@"; do
  sed -i \
    -e 's#^\( *\)\* @@HITL@@#\1* @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)\n\1* @authored_at   2026-11-27\n\1* @requested_by  Heechang Jang\n\1* @review_status Not Reviewed\n\1* @reviewed_by   -\n\1* @reviewed_at   -#' "$f"
done
