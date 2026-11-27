---
name: unit-test
description: A-SPICE SWE.4 소프트웨어 단위 검증. Google Test/GMock 단위 테스트 작성, MC/DC 100%와 테스트 성공률 100% 달성, 경계값/동치 클래스/PICT 적용 시 사용한다.
---

# 단위 테스트 (A-SPICE SWE.4)

## 품질 기준 (`CLAUDE.md` 2장, 예외 없음)
| 항목 | 기준 |
|---|---|
| **MC/DC 커버리지** | **100%** (LLVM으로 측정) |
| **테스트 성공률** | **100%** (실패·스킵·비활성 테스트 없음) |

- 측정 대상은 **프로덕션 소스**이며, 테스트 코드·gtest/gmock·시스템 헤더는 제외한다.
- 커버리지는 **`tests/unit/`의 단위 테스트만**으로 측정한다. 구현 단계의 TDD 테스트(`tests/tdd/`)는 별도 결과·별도 커버리지이며 **합산하지 않는다.**
- 100%가 될 때까지 측정 → 미커버 분석 → 테스트 추가를 **반복**한다.

## 원칙
1. **의미 없는 테스트 금지**: 단언 없이 호출만 하거나 커버리지 수치만 올리는 테스트를 추가하지 않는다. 모든 케이스는 상세 설계/요구사항의 기대 동작을 검증해야 한다. 도달 불가능한 코드는 테스트로 억지로 채우지 말고 **설계/코드 결함**으로 보고하고 코드를 바꾼다. (예외 제외는 허용되지 않는다.)
2. **Stub/Mock은 필요할 때만** 사용하고 과의존하지 않는다. 실제 모듈로 검증 가능하면 실제 모듈을 쓰고, 하드웨어·외부 의존·오류 주입처럼 격리가 불가피한 경우에 한정한다. (GMock 사용)
3. 테스트 케이스는 상세 설계 모듈/함수 및 요구사항과 **양방향 추적**한다.
4. 테스트 코드는 **Google Test** 로 작성한다. 테스트 이름은 동작을 설명한다.

## Doxygen 주석 (모든 테스트 함수 필수)
목적·입력값·절차·예상 출력값과 **Human in the Loop 태그**를 모두 포함한다.
```cpp
/**
 * @brief   [테스트 목적] 입력이 최대값을 초과하면 RET_ERR_RANGE를 리턴한다.
 * @tracing SWDD-MOD-012, SWRS-FUN-007
 *
 * @par Input
 *   in.value = MAX_VALUE + 1, out = &result
 * @par Procedure
 *   1. 입력 구조체를 경계값 초과로 설정한다.
 *   2. process()를 호출한다.
 * @par Expected
 *   리턴값 == RET_ERR_RANGE, result 변경 없음
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
TEST(ProcessTest, ReturnsRangeErrorWhenValueAboveMax)
{
    input_t  in     = { MAX_VALUE + 1 };
    output_t result = { 0 };

    EXPECT_EQ(RET_ERR_RANGE, process(&in, &result));
    EXPECT_EQ(0, result.result);
}
```
- `@review_status`는 `Not Reviewed`로 시작하고, **사용자가 리뷰 완료를 알려줄 때만** `Reviewed`로 바꾼다. 사람이 수정하면 되돌린다.
- `TEST()` 본문은 `void`이며 `implementation` 스킬의 리턴 규칙 예외이다. 테스트 헬퍼 함수는 규칙을 따른다.
- Doxygen이 `TEST` 매크로를 인식하고 커스텀 태그(`@tracing`, `@authored_by` 등)를 처리하도록 Doxyfile(`PREDEFINED`, `ALIASES`)에 설정한다.

## 테스트 설계 기법 (적극 활용)
- **동치 클래스 분할**: 유효/무효 클래스별 대표값 1개 이상.
- **경계값 분석**: min-1, min, min+1, max-1, max, max+1.
- **PICT 조합 테스트**: 다중 파라미터 입력은 PICT로 조합을 생성한다.
  1. 모델 파일 작성 (`param: v1, v2, ...`, 필요 시 제약조건)
  2. `pict model.txt /o:2 > cases.tsv` (페어와이즈, 필요 시 `/o:3`)
  3. 생성된 조합을 테스트 케이스로 변환하고 기대값은 설계에서 도출한다. (PICT 출력은 입력 조합일 뿐 기대값이 아니다.) gtest의 `INSTANTIATE_TEST_SUITE_P`로 조합을 파라미터화할 수 있다.

## 커버리지 측정 (LLVM)
스크립트는 이 스킬의 `scripts/`에 있다. 두 스크립트 모두 같은 인자를 받는다.
```
scripts/mcdc.sh      "<프로덕션 소스...>" "<테스트 소스...>" [출력디렉터리]   # MC/DC (필수 기준)
scripts/coverage.sh  "<프로덕션 소스...>" "<테스트 소스...>" [출력디렉터리]   # 구문/분기/함수
```
- 환경변수: `CLANG`(기본 `clang++`), `EXTRA_FLAGS`(예: `-std=c++17 -Iinclude`), `LIBS`(기본 `-lgtest -lgtest_main -lgmock -lpthread`), `IGNORE_REGEX`(리포트 제외 경로), `LLVM_PROFDATA`, `LLVM_COV`.
- 스크립트는 테스트가 **실패하더라도 리포트는 생성**하고, 마지막에 **테스트 실패 시 0이 아닌 종료 코드**로 끝낸다. (성공률 100% 판정)
- MC/DC 리포트(`mcdc.txt`)에서 독립성(independence pair)이 충족되지 않은 조건을 찾아, 그 조건이 결과에 독립적으로 영향을 주는 케이스 쌍을 설계해 추가한다. 100%가 될 때까지 반복한다.
- LLVM 18+ 필요. LLVM MC/DC는 하나의 결정에 포함된 조건 수에 제한(기본 6개)이 있으므로, 초과하면 조건식을 분리하도록 코드를 바꾼다.
- Windows 호스트가 아니라 **`ClaudeCLangBuild` Docker 이미지 안에서** 실행한다.

## 완료 조건
- MC/DC 100%, 테스트 성공률 100%
- 모든 테스트 함수에 Doxygen 주석(목적·입력·절차·기대값 + Human in the Loop 태그) 존재
- 추적성 갱신, `reviewer` 서브에이전트 리뷰 `Approved`
