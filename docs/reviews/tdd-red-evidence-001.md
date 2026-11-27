# TDD RED 증적 (이터레이션 1, 통합 리뷰 R1-08 조치)

`test-driven-development` 스킬은 구현 코드 전에 실패하는 테스트를 확인하도록 요구한다. 이번 이터레이션은 모듈 단위로 헤더(인터페이스)와 TDD 테스트 28건을 먼저 작성한 뒤 구현했으며, RED 확인은 **전체 테스트 묶음에 대해 1회** 수행했다(기능별 개별 RED 로그는 남기지 않았다 — 프로세스 편차).

## 기록된 RED 실행 (구현 소스 없음)

명령(`claudeclangbuild:latest` 이미지 안):
```
clang++ -std=c++17 -Iinclude -Itests/common -DECL_TEST_BUILD tests/tdd/*.cpp -lgtest -lgtest_main -lpthread -o /tmp/red
```
결과(링크 단계 실패, 구현 함수 미정의):
```
undefined reference to `ecl_arb_decide'
undefined reference to `ecl_cmd_interpret'
undefined reference to `ecl_core_evaluate'
undefined reference to `ecl_core_get_status'
undefined reference to `ecl_core_init'
undefined reference to `ecl_core_query_events'
```
(이 외 ecl_val_*, ecl_ovr_*, ecl_sta_decide, ecl_log_*, ecl_ser_status, ecl_core_* 동일 형식)

이어서 `ci/quality.sh tdd`가 구현 후 첫 실행에서 27/28 통과, 실패 1건(`TddCore.DriverLockThenEventRecordedOnlyOnChange`의 기대 건수 2 ≠ 실제 3)을 보였고, **테스트의 기대값이 잘못된 것**임을 설계(변화 판정 기준)로 확인해 테스트를 고쳐 28/28을 얻었다.

## 편차와 조치
- 기능(테스트)별 RED 로그가 없는 것은 프로세스 편차이며 사후에 만들 수 없다.
- 이후 이터레이션부터는 테스트마다 `ci/quality.sh tdd`의 실패 출력을 `build/reports/tdd/`에 보관하고, 신규 TDD 테스트의 RED 로그를 `docs/reviews/`에 첨부한다.
