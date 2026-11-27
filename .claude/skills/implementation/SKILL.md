---
name: implementation
description: A-SPICE 기준 소프트웨어 구현 규칙(TDD 연계, 함수 리턴 규칙, 정적 분석/메트릭 기준, Doxygen·Human-in-the-Loop 주석 포함). 소스 코드를 작성하거나 수정할 때 사용한다.
---

# 구현 (A-SPICE SWE.3 구현 활동)

## TDD 연계
구현은 **`test-driven-development` 스킬로 TDD 방식**으로 진행한다. (RED → GREEN → REFACTOR)
- TDD로 만든 테스트는 **단위 테스트와 분리**하여 `tests/tdd/`에 둔다. 단위 테스트(`tests/unit/`)는 `unit-test` 스킬로 별도 작성한다.
- TDD 테스트는 **별도 결과물**(별도 실행 결과)로 내고 **커버리지도 별도로 측정**한다. 단위 테스트 커버리지(MC/DC)에 합산하지 않는다.

## 함수 리턴 규칙 (필수, 프로덕션 코드 대상)
1. **모든 함수는 반드시 리턴값을 가진다.**
2. **`return`은 함수의 마지막에 단 한 번만** 호출한다. (단일 종료점) 조기 return, 중간 return 금지.
3. 의미 있는 리턴값이 없으면 `void` 대신 **정상 실행 여부를 판단할 수 있는 값**(상태 코드, `bool`, 열거형 등)을 리턴한다.
4. 호출자는 이 리턴값을 확인하고 처리한다.

**적용 범위와 예외**
- 이 규칙은 **프로덕션 코드**에 적용한다.
- 테스트 프레임워크가 `void`로 정의하는 **Google Test 매크로 본문(`TEST`, `TEST_F`, `TEST_P`, `SetUp`/`TearDown`)**은 예외이며, `ASSERT_*`의 암묵적 조기 return도 허용한다. 이 외의 **테스트 헬퍼 함수**는 규칙을 따른다.
- GMock이 요구하는 시그니처(`MOCK_METHOD`)는 대상 인터페이스의 시그니처를 따른다.

## 단일 종료점 작성 패턴
```c
typedef enum { RET_OK = 0, RET_ERR_NULL, RET_ERR_RANGE } ret_t;

ret_t process(const input_t *in, output_t *out)
{
    ret_t ret = RET_OK;

    if ((in == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else if (in->value > MAX_VALUE) {
        ret = RET_ERR_RANGE;
    } else {
        out->result = compute(in->value);
    }

    return ret;
}
```
- 조기 탈출 대신 `if/else` 또는 결과 변수로 흐름을 제어한다. 중첩이 깊어지면 함수를 분리한다 (상세 설계의 SRP).
- 결과 변수는 선언 시 초기화한다.

## 품질 기준 (`CLAUDE.md` 2장, 예외 없음)
- **CppCheck**: 룰 100% 만족. 위반 0건이며 억제(`cppcheck-suppress`, 억제 파일) 사용 금지.
- **Lizard**: 함수당 순환복잡도 10 이하, 함수 라인수 80 이하. 구현 종료 시 Hook으로 검사한다. 초과하면 함수를 분리한다.
- 컴파일러 경고 0 (`-Wall -Wextra`), `clang-tidy`/`clang-format` 권장.

## 주석 (Doxygen + Human in the Loop)
모든 소스 파일/함수 주석은 Doxygen 형식이며 다음 태그를 **반드시** 포함한다.
```c
/**
 * @brief  입력값을 검증하고 결과를 계산한다.
 * @tracing SWDD-MOD-012
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
```
- `@authored_by`/`@authored_at`/`@requested_by`는 실제 모델, 작성 일시, 요청한 사용자로 기록한다.
- `@review_status`는 `Not Reviewed`로 시작하며, **사용자가 리뷰 완료를 알려줄 때만** `Reviewed`와 리뷰어·일시로 바꾼다. 사람이 코드를 고치면 다시 `Not Reviewed`로 되돌린다.
- `@tracing`, `@authored_by` 등 커스텀 태그는 Doxyfile의 `ALIASES`에 등록한다.

## 준수 사항
- 상세 설계 및 인터페이스 명세와 일치하게 구현하고, 설계에 없는 동작을 추가하지 않는다.
- 모듈 내부 함수는 `static`; 공개 헤더에는 인터페이스만 둔다.
- 구현 후 소스 ↔ 상세 설계 **추적성**을 갱신하고 `reviewer` 서브에이전트로 리뷰한다.
