---
name: integration-test
description: A-SPICE SWE.5 소프트웨어 통합 및 통합 테스트. Google Test/GMock 기반으로 컴포넌트 간 인터페이스 연동을 검증하고 함수 커버리지 100%, 테스트 성공률 100%를 달성할 때 사용한다.
---

# 통합 테스트 (A-SPICE SWE.5)

## 품질 기준 (`CLAUDE.md` 2장, 예외 없음)
| 항목 | 기준 |
|---|---|
| **함수 커버리지** | **100%** (LLVM으로 측정) |
| **테스트 성공률** | **100%** (실패·스킵·비활성 테스트 없음) |

- 측정 대상은 **통합 대상 컴포넌트의 프로덕션 함수 전체**이며, 테스트 코드·gtest/gmock·시스템 헤더는 제외한다.
- 커버리지는 **`tests/integration/`의 통합 테스트만**으로 측정한다. 단위 테스트(`tests/unit/`)나 TDD 테스트(`tests/tdd/`)의 실행은 합산하지 않는다.
- 100%가 될 때까지 측정 → 미커버 함수 분석 → 테스트 추가를 **반복**한다.

## 원칙
1. **인터페이스 중심**: 아키텍처에 정의된 컴포넌트 인터페이스를 통해 컴포넌트 간 연동이 올바른지 확인하는 테스트 위주로 수행한다. 컴포넌트 내부 로직 검증은 `unit-test` 범위이다. 함수 커버리지는 이 연동 테스트가 호출 경로를 통해 컴포넌트 내부 함수까지 실행시키며 달성한다.
2. **의미 없는 테스트 금지**: 커버리지 수치만을 위한 케이스를 추가하지 않는다. 호출되지 않는 함수는 테스트를 억지로 만들지 말고 **불필요한 코드(설계 결함)**인지 먼저 판단한다.
3. **Stub/Mock은 필요한 경우에만** 사용하며 과의존하지 않는다. 가능한 한 실제 컴포넌트를 연결해 검증하고, 아직 없는 컴포넌트·하드웨어·외부 시스템에 한해 대체한다. (GMock 사용)
4. 통합 순서/전략(상향·하향, 빅뱅 지양)을 정하고 기록한다.
5. 테스트 케이스는 아키텍처 인터페이스/요구사항과 **양방향 추적**한다. 테스트 코드는 **Google Test**로 작성한다.

## 검증 대상 (인터페이스별)
- 정상 호출 시퀀스와 데이터 전달 (타입·범위·단위)
- 오류 코드 전파와 예외/실패 경로
- 사전·사후 조건, 호출 순서, 타이밍
- 인터페이스 우회 접근이 없는지 (아키텍처 위반 확인)

## 설계 기법 (적극 활용)
`unit-test` 스킬과 동일하게 **경계값 분석, 동치 클래스, PICT 조합**을 활용한다. 인터페이스 파라미터 조합과 컴포넌트 간 상태 조합에 특히 PICT를 사용한다. (PICT 사용 절차는 `unit-test` 스킬 참고)

## Doxygen 주석 (모든 테스트 함수 필수)
목적·입력값·절차·예상 출력값과 **Human in the Loop 태그**를 모두 포함한다.
```cpp
/**
 * @brief   [테스트 목적] 컴포넌트 A가 IB_Read()로 B의 센서값을 요청하면 유효 범위 값이 전달된다.
 * @tracing SWAD-IF-004, SWRS-FUN-012
 *
 * @par Input
 *   센서 원시값 = 1023 (최대 경계값)
 * @par Procedure
 *   1. B 컴포넌트를 초기화하고 입력을 설정한다.
 *   2. A에서 IB_Read()를 호출한다.
 * @par Expected
 *   리턴값 == RET_OK, 전달된 값 == 100.0 [%]
 *
 * @par AI Authorship
 *   @authored_by  Claude Sonnet 5.5 (claude-sonnet-5-5)
 *   @authored_at  2026-11-27
 *   @requested_by Heechang Jang
 * @par Human Review
 *   @review_status  Not Reviewed
 *   @reviewed_by    -
 *   @reviewed_at    -
 */
TEST(CompAReadsCompBTest, ReturnsFullScaleWhenSensorAtMax)
{
    /* ... */
}
```
- `@review_status`는 `Not Reviewed`로 시작하고, **사용자가 리뷰 완료를 알려줄 때만** `Reviewed`로 바꾼다. 사람이 수정하면 되돌린다.
- `TEST()` 본문은 `void`이며 `implementation` 스킬의 리턴 규칙 예외이다.

## 커버리지 측정 (LLVM)
`unit-test/scripts/coverage.sh`를 사용한다. 리포트의 **Functions / Missed Functions** 열로 함수 커버리지를 확인한다.
```
.claude/skills/unit-test/scripts/coverage.sh "<프로덕션 소스...>" "<통합 테스트 소스...>" [출력디렉터리]
```
환경변수(`CLANG`, `EXTRA_FLAGS`, `LIBS`, `IGNORE_REGEX` 등)와 종료 코드(테스트 실패 시 0이 아님)는 `unit-test` 스킬과 같다. `ClaudeCLangBuild` Docker 이미지 안에서 실행한다.

## 완료 조건
- 함수 커버리지 100%, 테스트 성공률 100%
- 모든 인터페이스와 요구사항(해당 통합 수준)에 테스트 존재
- 모든 테스트 함수에 Doxygen 주석(목적·입력·절차·기대값 + Human in the Loop 태그) 존재
- 추적성 갱신, `reviewer` 서브에이전트 리뷰 `Approved`
