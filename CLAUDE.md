# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project status

This repository (`Claude_Clang`, remote `https://github.com/yallacha/Claude_Clang.git`) currently holds no product source code; only the process (skills, agents), build image, CI and hook infrastructure exist. Production code is **C** (`src/*.c`, headers in `include/` or `src/`), tests are **C++ with Google Test/GMock** (`tests/{tdd,unit,integration,system}/`).

Common commands (run inside the build image on the host, see section 4):
```
docker build -t claudeclangbuild:latest -f docker/build/Dockerfile docker/build   # build image (once)
docker run --rm -v "$(pwd -W):/work" -w /work claudeclangbuild:latest ci/quality.sh all   # all quality gates
docker run --rm -v "$(pwd -W):/work" -w /work claudeclangbuild:latest ci/quality.sh unit  # one gate: cppcheck|lizard|cloc|tdd|unit|integration
```
Run a single gtest case: build with `.claude/skills/unit-test/scripts/coverage.sh "<src>" "<test>" out` and run `out/test_bin --gtest_filter=Suite.Case`.

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

- **Lizard 검사는 구현이 끝날 때 Hook으로 실행**한다. (`.claude/settings.json`의 `Stop` hook → `.claude/hooks/lizard-check.sh`. 위반 시 응답 종료를 막고 수정하게 한다. 소스가 없거나 Docker/이미지가 없으면 건너뛴다. 최종 게이트는 CI)
- 주석률은 CLOC으로 측정한다.
- 모든 검사는 `ci/quality.sh`가 실행하며 개발과 CI가 같은 스크립트를 쓴다. 대상 소스가 없으면 `SKIPPED`로 통과하고, 소스가 있는데 테스트가 없으면 실패한다.
- 함수 리턴 규칙(모든 함수 리턴값, return 한 번)은 **프로덕션 코드** 대상이며 gtest `TEST()` 본문은 예외이다. (`implementation` 스킬)

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
- 위 태그는 커스텀 태그이므로 Doxyfile의 `ALIASES`에 등록한다. (Doxyfile은 아직 없음 — 첫 소스 작성 시 함께 만든다)
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

개발 빌드와 CI 빌드는 **동일한 Docker 이미지 `ClaudeCLangBuild`**를 사용한다. Dockerfile은 `docker/build/Dockerfile`이며, **Docker 이미지 이름은 소문자만 허용되므로 실제 이름은 `claudeclangbuild:latest`**이다. 로컬에서 직접 설치한 도구로 빌드·검증한 결과는 인정하지 않는다.

| 용도 | 도구 |
|---|---|
| 컴파일러 | Clang 20 (apt.llvm.org, 이미지의 `LLVM_VERSION` 인자로 변경. 18 이상 필수) |
| 빌드 도구 | **CMake + Ninja** (`CMakeLists.txt`, `compile_commands.json` 생성). 품질 게이트의 테스트·커버리지 빌드는 `ci/quality.sh`가 수행 |
| 정적 분석 | CppCheck 2.13 |
| 테스트 프레임워크 | Google Test / GMock (이미지의 Ubuntu 패키지 `libgtest-dev`, `libgmock-dev`) |
| 커버리지 | LLVM (`llvm-cov`, `llvm-profdata`, MC/DC는 `-fcoverage-mcdc`, 스크립트: `.claude/skills/unit-test/scripts/`) |
| 순환복잡도/함수 라인수 | Lizard |
| 주석률 | CLOC |
| 테스트 조합 생성 | PICT |
| 문서화 | Doxygen |
| 포맷/보조 분석 | clang-format, clang-tidy (CppCheck 기준과 별개의 보조 도구) |

- 호스트는 Windows이고 빌드·테스트는 Docker 컨테이너(Linux) 안에서 수행한다. 쉘 스크립트는 LF를 유지한다(`.gitattributes`).

## 5. CI 환경

- CI는 **Jenkins 안정화(LTS)**(`jenkins/jenkins:lts-jdk21`)를 이 PC의 Docker 위에서 실행한다. 포트 **8080**, `127.0.0.1`에만 바인딩(이 PC에서만 접속). 시작: `docker compose up -d --build`, 중지: `docker compose down`.
- 관리자 계정은 `admin` / `admin`이다. 로컬 전용이며, 변경하려면 `JENKINS_ADMIN_PASSWORD` 환경변수로 컨테이너를 다시 시작한다.
- 설정은 코드로 관리한다: `docker/jenkins/{Dockerfile,plugins.txt,casc.yaml}` (JCasC로 계정과 멀티브랜치 잡 `Claude_Clang` 자동 생성). GitHub 저장소는 공개이므로 자격 증명 없이 **2분 폴링**으로 브랜치/PR 변경을 감지한다. 비공개로 바꾸면 자격 증명을 추가해야 한다.
- 설치된 플러그인(`plugins.txt`): Pipeline, Docker Pipeline, Git/GitHub Branch Source, Configuration as Code, Job DSL, JUnit(테스트 결과), Warnings Next Generation(CppCheck), Coverage(Cobertura), HTML Publisher 등.
- `Jenkinsfile`은 `claudeclangbuild` 이미지 안에서 Build → CppCheck → Lizard → TDD → Unit(MC/DC 100%) → Integration(함수 100%) → CLOC을 실행하고, 기준 미달 시 실패 처리한다. 결과는 JUnit·Warnings·Coverage 화면과 아티팩트(`build/reports/`)에서 확인한다.
- Jenkins는 호스트 Docker 데몬(`/var/run/docker.sock`)으로 빌드 컨테이너를 띄운다. 빌드 이미지는 호스트에서 먼저 빌드해 두어야 한다.
