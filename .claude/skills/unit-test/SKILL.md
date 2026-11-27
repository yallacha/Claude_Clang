---
name: unit-test
description: A-SPICE SWE.4 소프트웨어 단위 검증. 단위 테스트 케이스 작성, 커버리지 100% 달성, 경계값/동치 클래스/PICT, MC/DC(LLVM) 적용 시 사용한다.
---

# 단위 테스트 (A-SPICE SWE.4)

## 원칙
1. 프로젝트에서 정한 커버리지 목표(구문/분기/MC/DC 등)를 **100% 달성할 때까지** 측정 → 미커버 분석 → 테스트 추가를 **반복**한다. 목표가 정해져 있지 않으면 사용자에게 확인한다.
2. **의미 없는 테스트 금지**: 단언(assert) 없이 코드를 호출만 하거나, 커버리지 수치만 올리는 테스트를 추가하지 않는다. 모든 케이스는 상세 설계/요구사항의 기대 동작을 검증해야 한다. 도달 불가능한 코드는 테스트로 억지로 채우지 말고 **설계/코드 결함 또는 사유가 있는 제외**로 보고한다.
3. **Stub/Mock은 필요할 때만** 사용하고 과의존하지 않는다. 실제 모듈로 검증 가능하면 실제 모듈을 쓰고, 하드웨어·외부 의존·오류 주입처럼 격리가 불가피한 경우에 한정한다.
4. 테스트 케이스는 상세 설계 모듈/함수 및 요구사항과 **양방향 추적**한다.

## Doxygen 주석 (모든 테스트 함수 필수)
```c
/**
 * @brief   [테스트 목적] 입력이 최대값을 초과하면 RET_ERR_RANGE를 리턴한다.
 * @tracing SWDD-MOD-012, SWRS-FUN-007
 *
 * @par Input
 *   in->value = MAX_VALUE + 1, out = &result
 * @par Procedure
 *   1. 입력 구조체를 경계값 초과로 설정한다.
 *   2. process()를 호출한다.
 * @par Expected
 *   리턴값 == RET_ERR_RANGE, result 변경 없음
 */
ret_t test_process_value_above_max(void);
```
목적·입력값·절차·예상 출력값 네 항목이 모두 있어야 한다. (테스트 함수도 `implementation` 스킬의 리턴 규칙을 따른다.)

## 테스트 설계 기법 (적극 활용)
- **동치 클래스 분할**: 유효/무효 클래스별 대표값 1개 이상.
- **경계값 분석**: min-1, min, min+1, max-1, max, max+1.
- **PICT 조합 테스트**: 다중 파라미터 입력은 PICT로 조합을 생성한다.
  1. 모델 파일 작성 (`param: v1, v2, ...`, 필요 시 제약조건)
  2. `pict model.txt /o:2 > cases.tsv` (페어와이즈, 필요 시 `/o:3`)
  3. 생성된 조합을 테스트 케이스로 변환하고 기대값은 설계에서 도출한다. (PICT 출력은 입력 조합일 뿐 기대값이 아니다.)

## 커버리지 측정 (LLVM)
- 구문/분기 커버리지: `scripts/coverage.sh <소스> <테스트소스> [출력디렉터리]`
  (clang `-fprofile-instr-generate -fcoverage-mapping` → `llvm-profdata` → `llvm-cov`)
- **MC/DC 목표가 수립된 경우**: `scripts/mcdc.sh <소스> <테스트소스> [출력디렉터리]`를 사용한다. (LLVM 18+ 의 `-fcoverage-mcdc`)
  1. 스크립트로 MC/DC 리포트를 생성한다.
  2. 리포트에서 독립성(independence pair)이 충족되지 않은 조건을 확인한다.
  3. 해당 조건이 결과에 독립적으로 영향을 주는 케이스 쌍을 설계하여 테스트를 추가한다.
  4. 100%가 될 때까지 반복한다.
- 스크립트는 이 스킬 폴더의 `scripts/` 안에 있다. 도구 경로는 환경변수 `CLANG`, `LLVM_PROFDATA`, `LLVM_COV`로 재정의할 수 있다. (Windows에서는 Git Bash로 실행)

## 완료 조건
- 커버리지 목표 100% (또는 사유가 승인된 제외 항목만 남음)
- 모든 테스트 함수에 Doxygen 주석 4항목 존재
- 추적성 갱신, `review` 스킬로 테스트 체크리스트 리뷰 완료
