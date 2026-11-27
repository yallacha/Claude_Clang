# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project status

This repository (`Claude_Clang`, remote `https://github.com/yallacha/Claude_Clang.git`) currently holds no source code. The build system, Docker image, Jenkins CI and hooks described below are **defined policy but not yet created**; when one is created, replace its "(예정)" mark and add the concrete commands (build, lint, test, running a single test) to this file.

## 1. 개발 절차

- 개발은 **A-SPICE**를 기준으로 한다.
- 생명주기는 **애자일**이며, **분석 → 아키텍처 설계 → 상세 설계 → 구현 → 단위/통합/시스템 테스트를 하나의 이터레이션**으로 한다.
- 개발은 기본적으로 **메인 에이전트**가 수행하며, 각 단계에 맞는 스킬(`.claude/skills/`)을 사용한다.

  | 단계 | 스킬 |
  |---|---|
  | 분석 | `requirements-analysis` |
  | 아키텍처 설계 | `architecture-design` |
  | 상세 설계 | `detailed-design` |
  | 구현 | `implementation` + `test-driven-development` |
  | 단위 테스트 | `unit-test` |
  | 통합 테스트 | `integration-test` |
  | 시스템 테스트 | `sw-system-test-cases` |

- **단계 게이트**: 각 단계가 완료되면 `reviewer` 서브에이전트(`.claude/agents/reviewer.md`)가 산출물을 리뷰한다. 부적합 사항을 **모두 조치하고 재리뷰에서 `Approved`를 받아야** 다음 단계로 진행한다.
- **이터레이션 게이트**: 한 이터레이션이 끝나면 **사용자에게 확인을 받는다.** 확인 후에 PR 절차를 진행한다. 확인 없이 PR을 만들지 않는다.
- **TDD**: 구현은 `test-driven-development` 스킬로 TDD 방식으로 한다. 이때 만든 TDD 테스트 코드는 **단위 테스트 코드와 분리**(`tests/tdd/` vs `tests/unit/`)하고, **별도 결과물**로 산출하며 **커버리지도 별도로 측정**한다. TDD 테스트는 단위 테스트 커버리지에 합산하지 않는다.
- 모든 요구사항은 이후 산출물(아키텍처, 상세 설계, 코드, 테스트)과 **양방향 추적성**을 유지한다.

## 2. 소스코드 품질 기준

모든 기준에 **예외는 없다.** 미달이면 이터레이션을 완료할 수 없다.

| 항목 | 기준 |
|---|---|
| 시스템 테스트 | 요구사항 커버리지 100% |
| 통합 테스트 | 함수 커버리지 100%, 테스트 성공률 100% |
| 단위 테스트 | **MC/DC 커버리지 100%**, 테스트 성공률 100% |
| 정적 분석 | CppCheck 룰 대상 100% 만족 (위반 0건, 예외·억제 처리 없음) |
| 순환복잡도 | 함수당 10 이하 (Lizard) |
| 함수 라인수 | 함수당 80라인 이하 (Lizard) |

- **Lizard 검사는 구현이 끝날 때 Hook으로 실행**한다. (예정: `.claude/settings.json`의 hook으로 구성)
- 주석률은 CLOC으로 측정한다.

### 주석 (Doxygen, Human in the Loop)

주석은 Doxygen 형식이며, 소스 코드에는 다음을 **반드시** 포함한다. (역시 Doxygen 태그로 작성한다.)

- 어느 AI 모델이, 언제, 누가 요청해서 작성했는지
- 사용자 리뷰가 완료된 코드인지, 완료했다면 언제 누가 했는지

```c
/**
 * @brief  센서 값을 검증한다.
 *
 * @par AI Authorship
 *   @authored_by  Claude Sonnet 5.5 (claude-sonnet-5-5)
 *   @authored_at  2026-11-27
 *   @requested_by Heechang Jang
 * @par Human Review
 *   @review_status  Reviewed        (Not Reviewed | Reviewed)
 *   @reviewed_by    Heechang Jang   (미완료 시 -)
 *   @reviewed_at    2026-11-28      (미완료 시 -)
 */
```
- 위 태그는 커스텀 태그이므로 Doxyfile의 `ALIASES`에 등록한다. (예정)
- 사용자가 리뷰하기 전에는 `Not Reviewed`로 두고, **사용자가 리뷰를 완료했다고 알려줄 때만** `Reviewed`로 바꾼다. 임의로 바꾸지 않는다.
- 사람이 코드를 수정하면 해당 주석의 리뷰 상태를 `Not Reviewed`로 되돌린다.
- 단위/통합 테스트 함수 주석(목적·입력값·절차·예상 출력값)은 각 테스트 스킬의 규칙을 따른다.

## 3. Git 브랜치 및 PR 전략

- 브랜치는 **Gitflow**를 따른다: `main`(릴리스), `develop`(통합), `feature/*`, `release/*`, `hotfix/*`.
- 이터레이션 작업은 `develop`에서 분기한 `feature/*` 브랜치에서 수행한다.
- PR은 **CI를 통과해야만 완료**한다. CI 통과 조건은 빌드 성공 + 2장의 소스코드 품질 기준 전체 충족이다.
- 이터레이션 완료에 대한 사용자 확인 후에 PR을 만든다. (1장)
- (참고: 초기 세팅 커밋은 Gitflow 도입 전에 `main`에 직접 푸시했다. 이후부터 적용한다.)

## 4. 개발 환경

개발 빌드와 CI 빌드는 **동일한 Docker 이미지 `ClaudeCLangBuild`**를 사용한다. (예정: `Dockerfile`로 빌드) 로컬에서 직접 설치한 도구로 빌드·검증한 결과는 인정하지 않는다.

| 용도 | 도구 |
|---|---|
| 컴파일러 | Clang 18 이상 (최신 안정 버전) |
| 빌드 도구 | **CMake + Ninja** (권장: 이식성, `compile_commands.json` 생성으로 CppCheck/clang-tidy 연동, CTest로 테스트 실행) |
| 정적 분석 | CppCheck |
| 테스트 프레임워크 | Google Test / GMock (CMake `FetchContent`로 버전 고정) |
| 커버리지 | LLVM (`llvm-cov`, `llvm-profdata`, MC/DC는 `-fcoverage-mcdc`, 스크립트: `.claude/skills/unit-test/scripts/`) |
| 순환복잡도/함수 라인수 | Lizard |
| 주석률 | CLOC |
| 테스트 조합 생성 | PICT |
| 문서화 | Doxygen |
| 포맷/보조 분석 | clang-format, clang-tidy (CppCheck 기준과 별개의 보조 도구) |

- 호스트는 Windows이고 빌드·테스트는 Docker 컨테이너(Linux) 안에서 수행한다. 쉘 스크립트는 LF를 유지한다(`.gitattributes`).

## 5. CI 환경

- CI는 **Jenkins 안정화(LTS) 버전**을 이 PC의 Docker 위에서 실행한다. 포트는 **8080**. (예정)
- 필요한 플러그인을 이미지에 포함해 설치 완료 상태로 제공하고, **테스트·정적 분석·커버리지·메트릭 결과를 Jenkins 플러그인 화면에서 확인**할 수 있어야 한다. (예정, 계획 플러그인: Git, Pipeline, Docker Pipeline, Warnings Next Generation(CppCheck), xUnit/JUnit, Code Coverage API/Coverage, Gitflow 브랜치용 Multibranch/GitHub Branch Source)
- Jenkins 파이프라인은 `ClaudeCLangBuild` 이미지에서 빌드 → 테스트 → 커버리지 → CppCheck → Lizard → CLOC을 실행하고, 품질 기준 미달 시 실패 처리한다.
- 에이전트 컨테이너(`ClaudeCLangBuild`)와 Jenkins 컨테이너는 분리한다.
