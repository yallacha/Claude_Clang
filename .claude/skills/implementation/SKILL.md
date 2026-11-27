---
name: implementation
description: A-SPICE 기준 소프트웨어 구현 규칙(함수 리턴 규칙 포함). 소스 코드를 작성하거나 수정할 때 사용한다.
---

# 구현 (A-SPICE SWE.3 구현 활동)

## 함수 리턴 규칙 (필수)
1. **모든 함수는 반드시 리턴값을 가진다.**
2. **`return`은 함수의 마지막에 단 한 번만** 호출한다. (단일 종료점) 조기 return, 중간 return 금지.
3. 의미 있는 리턴값이 없으면 `void` 대신 **정상 실행 여부를 판단할 수 있는 값**(상태 코드, `bool`, 열거형 등)을 리턴한다.
4. 호출자는 이 리턴값을 확인하고 처리한다.

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

## 준수 사항
- 상세 설계 및 인터페이스 명세와 일치하게 구현하고, 설계에 없는 동작을 추가하지 않는다.
- 모듈 내부 함수는 `static`; 공개 헤더에는 인터페이스만 둔다.
- 컴파일러 경고를 0으로 유지한다 (`clang -Wall -Wextra`). 정적 분석(`clang-tidy`) 사용을 권장한다.
- 구현 후 소스 ↔ 상세 설계 **추적성**을 갱신하고 `review` 스킬로 리뷰한다.
