# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project status

This repository (`Claude_Clang`, remote `https://github.com/yallacha/Claude_Clang.git`, default branch `main`) currently holds only a placeholder `README.md`. There is no source code, build system, linter, or test suite yet.

When the first code lands, update this file with:
- Build, lint and test commands (including how to run a single test)
- The high-level architecture that spans multiple files

## Environment

- Development happens on Windows (PowerShell is the primary shell; Git Bash is also available).

## 개발 프로세스 (A-SPICE)

단계별 스킬은 `.claude/skills/`에 있다: `requirements-analysis` → `architecture-design` → `detailed-design` → `implementation` → `unit-test` → `integration-test`.

- 각 단계가 완료될 때마다 **반드시 `reviewer` 서브에이전트(`.claude/agents/reviewer.md`)를 호출**해 산출물을 리뷰한다.
- 리뷰 부적합 사항이 **모두 조치되고 `Approved`를 받기 전에는 다음 단계로 넘어가지 않는다.** 조치 후에는 재리뷰를 받는다.
- 모든 요구사항은 이후 산출물(아키텍처, 상세 설계, 코드, 테스트)과 양방향 추적성을 유지한다.
