# 리뷰 기록: 이터레이션 1 (제어 코어) 통합 리뷰

- 리뷰 방식: DEC-008에 따른 이터레이션 종료 시 통합 리뷰 (`review` 스킬, 체크리스트 `references/checklists.md` 2·3·4·5·6·7장)
- 리뷰어 판정 근거: 산출물 파일을 직접 열어 확인함. 품질 게이트(빌드/CppCheck/Lizard/tdd/unit/integration/system)는 재실행하지 않았고, 기존 `build/reports/` 산출물을 증거로 사용했다. 기존 산출물을 증거로 쓴 항목은 비고에 명시했다.
- 수정·생성 범위: 이 기록 파일 외에는 아무 파일도 수정·생성하지 않았다. 요구사항 문서(`docs/SRS.md`, `docs/safety-requirements.md`)는 상위 입력으로만 사용했다.

## 리뷰 기록
- 대상: 이터레이션 1 (제어 코어): `docs/architecture.md`, `docs/detailed-design.md`, `include/ecl_*.h`, `src/ecl_*.c`, `tests/{tdd,unit,integration,system}/`, `docs/system-test/` (system_tests.json, ST-ECL-001.xlsx, pict_*), `tools/gen_system_tests.py`, `ci/check_system_coverage.py`, `docs/traceability.md` §2 / 버전: 리뷰 시점 워킹트리 (SAD-ECL-001 v0.1, SDD-ECL-001 v0.1, SRS-ECL-001 v0.4 기준) / 체크리스트: `review` 스킬 `checklists.md` 2장·3장·4장·5장·6장·7장
- 결과: **Rejected**
- 다음 단계 진행: **불가 (열린 결함 28건: Major 12, Minor 16)**

### 체크리스트 판정

| # | 체크 항목 | 판정 | 근거/비고 |
|---|---|---|---|
| **2. 아키텍처 (SWE.2)** | | | |
| A-1 | 모든 컴포넌트가 정의되고 책임이 단일한가 | Pass | `docs/architecture.md` §3 (CMP-TYP~CMP-CORE, CMP-WEB 제외). 컴포넌트별 책임 한 문장. |
| A-2 | 모든 컴포넌트에 인터페이스(시그니처, 범위, 오류, 조건)가 정의되어 있는가 | Fail | §4.1~4.9 정의는 Pass. CMP-WEB는 §4.10에 "요구 인터페이스/제공 인터페이스" 개요만 있고 시그니처·오류·사전·사후 조건이 없다. 그런데 SRQ-FUN-024/025, SRQ-CON-008/009가 CMP-WEB에 할당되어 있다. → R1-26 |
| A-3 | 컴포넌트 간 통신이 인터페이스로만 이루어지는가 | Pass | `src/ecl_*.c`의 include는 공개 헤더만 사용. 기능 컴포넌트 간 직접 include 없음 (`ecl_core.c`만 하위 헤더를 include). |
| A-4 | 높은 응집도·낮은 결합, 순환 의존 없음 | Pass | `architecture.md` §2 의존 그래프는 DAG. 헤더 include 그래프도 순환 없음 (`ecl_core.h` → `ecl_log.h`/`ecl_ovr.h`/`ecl_val.h`/`ecl_types.h`). |
| A-5 | 모듈화·캡슐화·SOLID 준수 | Fail | §1 결정표와 `include/ecl_val.h` L22, `ecl_ovr.h` L22, `ecl_log.h` L22, `ecl_core.h` L25의 주석은 "모든 필드는 private"라고 하지만 구조체 필드가 모두 공개되어 있다. `architecture.md` §1은 "INS로 검사"를 약속했으나 검사 증적이 없다. 테스트가 `core.log`를 직접 접근한다 (`tests/integration/test_integration_core.cpp` L252, `tests/system/test_system_special.cpp` L148). → R1-23 |
| A-6 | 동적 거동과 자원 소비 목표가 기술되어 있는가 | Pass | `architecture.md` §5 (평가 시퀀스, 상태 전이, O(1), 정적 메모리 ≈10 KB). |
| A-7 | 모든 SW 요구사항이 컴포넌트에 할당되어 있는가 | Pass | `architecture.md` §7에 SRQ/SSR 91건 전부 언급됨을 스크립트로 대조 확인. |
| A-8 | 설계 결정의 근거가 기록되어 있는가 | Pass | `architecture.md` §1 결정표 (대안과 기각 사유). |
| **3. 상세 설계 (SWE.3)** | | | |
| D-1 | 모듈과 함수가 정의되고 컴포넌트와 추적되는가 | Fail | 코드에는 있으나 `detailed-design.md`에 없는 정적 함수가 다수다: `reset_status`, `build_status`, `mark_stale`, `has_substitution`, `build_inputs`, `check_bool_signals`, `check_other_signals`, `apply_input_rules`, `apply_driver`, `speed_reason`, `fire_reason`, `decide_both`, `decide_door`. 반대로 설계의 `decide_crash`, `apply_reject`는 코드에 없다 (인라인). → R1-16 |
| D-2 | 기준 이상 모듈에 인터페이스가 정의되어 있는가 | Pass | DEC-009 기준대로 `include/` 헤더 8종 모두 공개 함수 인터페이스가 있다. |
| D-3 | 인터페이스 정의 모듈 간 통신이 인터페이스로만 이루어지는가 | Pass | D-2와 동일 근거. |
| D-4 | 함수·모듈이 단일 책임 원칙을 만족하는가 | Pass | 컴포넌트 단위 SRP가 유지됨. Lizard 최대 CCN 10. |
| D-5 | 클린코드 원칙(명명, 작은 함수, 중복 없음, 매직 넘버 없음) | Fail | 매직 넘버 다수 (R1-14). 동일 목적 판정 로직 중복 (`src/ecl_arb.c` `driver_applies` ↔ `src/ecl_ovr.c` `door_requested`) (R1-25). 의미 없는 상수 반환 함수 (R1-05). |
| D-6 | 함수별 입력·출력·리턴값·오류 처리가 기술되어 있는가 | Fail | 공개 함수는 SAD §4·SDD §3에 기술됨. 정적 함수 다수가 SDD에 없어 입출력·리턴 의미가 기술되지 않았다 (D-1 참조). SDD §3.7의 `tmp[640]`/`append_pair` 설계와 코드(`write_pair`, 건식 2-pass)가 다르다. → R1-16 |
| D-7 | 아키텍처 및 요구사항과 일관되고 양방향 추적되는가 | Fail | SDD §3.1 `judge_stale` 식에 있는 "마지막 수신 후 0.200 s 초과" 항이 코드에 없다 (R1-01). SDD §6 가정 1이 SSR-011 본문과 모순된다 (R1-02). SDD §3.1의 틱 동작·`last_inputs` 설계와 코드가 다르다 (R1-06). |
| **4. 구현 (소스)** | | | |
| I-1 | 프로덕션 코드의 모든 함수가 리턴값을 가지는가 | Pass | `src/`·`include/`에 `void` 0건 (grep). lizard 기준 함수 58개 전부 리턴 타입 보유. |
| I-2 | 프로덕션 코드의 `return`이 함수 마지막에 단 한 번인가 | Pass | 함수별 `return` 개수 합계 58 = 함수 58개. 각 함수 `return`이 마지막 문장이며 중간 조기 return 없음 (`ecl_*.c` 전수 확인). |
| I-3 | 리턴값이 없는 경우 정상 실행 여부를 알 수 있는 상태 값을 리턴하는가 | Fail | 상수 `0`만 반환하는 함수 17개: `ecl_arb.c` `decide_both`(L275), `ecl_core.c` `reset_status`(L32), `build_status`(L51), `ecl_ovr.c` `apply_request`(L49), `update_door`(L80), `ecl_val.c` `check_bool_signals`(L118), `check_other_signals`(L144), `mark_stale`(L193), `apply_substitution`(L241), `track_pending`(L298), `build_inputs`(L364), `ecl_log.c` `store_event`(L27), `ecl_ser.c` `write_pair`(L68), `write_header`(L94), `write_validity`(L117), `write_all`(L141). → R1-05 |
| I-4 | 호출자가 리턴값을 확인·처리하는가 | Fail | 위 함수들의 결과를 전부 `(void)`로 버린다. `ecl_arb.c` L209의 `apply_driver`는 의미 있는 "적용 여부"를 반환하지만 버려진다. `ecl_val_condition`(L404)·`ecl_ser_status`(L160, L166)도 같다. → R1-05 |
| I-5 | 상세 설계와 일치하며 설계에 없는 동작이 없는가 | Fail | 시간 판정 설계 불일치 (R1-01, R1-03). 틱 동작 설계 불일치 (R1-06). 설계에 없는 `present` 값 처리 규칙 (R1-28). 구현 SKILL의 "설계에 없는 동작 추가 금지" 위반. |
| I-6 | 내부 함수/데이터가 `static`으로 은닉되고 인터페이스로만 접근되는가 | Pass | `.c` 파일의 비공개 함수 전부 `static`. 파일 스코프 변수는 `static const` 7개뿐이고 전역 가변 상태 없음 (`P_RANK`, `BOOL_SIGS`, 이름 테이블). |
| I-7 | 컴파일러 경고 0 | Fail | 경고 0을 증명하는 빌드 로그가 저장소·`build/reports/`에 없다. 이번 리뷰에서 재빌드하지 않았다. 증거가 없으므로 미확인 = Fail. → R1-27 |
| I-8 | CppCheck 위반 0건, 억제 주석·억제 파일 없음 | Pass | `build/reports/cppcheck/cppcheck.xml` (version 2.13.0, `<errors>` 비어 있음). `src/`·`include/`에 `cppcheck-suppress`·NOLINT 0건. (기존 보고서 기준, 재실행 안 함) |
| I-9 | Lizard: 순환복잡도 ≤ 10, 함수 라인수 ≤ 80 | Pass | `build/reports/lizard/lizard.xml` 기준 프로덕션 함수 58개, 최대 CCN 10 (`ecl_arb_decide`), 최대 NCSS 31 (`run_pipeline`). (기존 보고서 기준) |
| I-10 | 소스·함수 주석이 Doxygen이며 AI 작성 정보가 있는가 | Fail | 파일 헤더와 정적 함수 주석 6종 태그는 전부 있음. 그러나 `src/`의 공개 함수 정의 16개에는 주석 블록이 없다 (`ecl_arb.c` L288, `ecl_cmd.c` L38, `ecl_core.c` L150/170/187/204/217/230, `ecl_log.c` L44/58/80/99, `ecl_ovr.c` L100/113, `ecl_ser.c` L149, `ecl_sta.c` L15, `ecl_val.c` L384/397/414). 헤더에만 있다. → R1-13 |
| I-11 | 리뷰 상태(Human Review) 태그가 있고 사용자 리뷰 전 `Not Reviewed`인가 | Pass | 모든 파일·함수 `@review_status Not Reviewed`, `Reviewed` 표기 0건 (grep). |
| I-12 | TDD 테스트가 `tests/tdd/`에 분리되고 별도 결과·별도 커버리지로 산출되었는가 | Pass | `tests/tdd/` 분리. `build/reports/tdd/`(28건, 별도 summary·커버리지). `build/reports/unit/` 오브젝트 목록에 tdd 파일 없음. |
| I-13 | 기능별 RED(실패 선행) 증거가 있는가 | Fail | 저장소 어디에도 RED 실행 로그·실패 출력·커밋 이력 증적이 없다 (`docs/`, `build/reports/tdd/` 등 전수 검색). → R1-08 |
| I-14 | 상세 설계와의 추적성이 유지되는가 | Fail | SDD §5는 함수 단위 추적을 목표로 하나 정적 함수 다수와 `decide_crash`/`apply_reject` 불일치 (D-1). → R1-16 |
| **5. 단위 테스트 (SWE.4)** | | | |
| U-1 | 테스트가 상세 설계/요구사항과 양방향 추적되는가 | Fail | `docs/traceability.md` §2의 단위 열과 `@tracing`은 상호 일치하며 허위 ID는 없다. 그러나 (a) NULL 테스트에 무관한 SSR-019/033/036/037 추적 (R1-17), (b) SSR-035의 UT로 지정된 `OvrUnit.OverrideExpiresOnResolveOrTimeout`은 override 플래그만 보고 첫 억제 기록 삭제를 검증하지 않음 (R1-09), (c) SSR-008 스냅샷 경로 UT가 없음 (R1-01). |
| U-2 | 모든 테스트 함수에 Doxygen 주석(목적, 입력값, 절차, 예상 출력값)이 있는가 | Pass | `tests/unit/*.cpp` 66건 전부 `@brief [목적]`, `@par Input`, `@par Procedure`, `@par Expected`. (TDD 테스트는 U-2 대상 아님. R1-07에서 별도 지적) |
| U-3 | 경계값 분석·동치 클래스·PICT 조합이 적용되었는가 | Pass | 경계: 0.200/0.210 s, 3.0/2.9 km/h, 300.0/300.1, 10.0/10.01 s, 1.000/1.010 s, 0/1/2/-1/ABSENT (`test_unit_val.cpp`, `test_unit_cmd_sta.cpp`, `test_unit_log_ovr.cpp`). |
| U-4 | MC/DC 커버리지 100%를 LLVM 리포트로 직접 확인했는가 | Pass | `build/reports/unit/mcdc-report.txt`: 8개 소스 파일 모두 MC/DC Condition 100% (Missed Conditions 0). 제외 설정 없음 (`ci/quality.sh` EXTRA_FLAGS는 `-DECL_TEST_BUILD`만 추가). (기존 보고서 기준) 단 `ecl_core.c` 분기 9개 미실행 (R1-22). |
| U-5 | 테스트 성공률 100%인가 | Pass | `build/reports/unit/test-results.xml`: tests=66, failures=0, errors=0, skipped=0. (기존 보고서 기준) |
| U-6 | 커버리지가 `tests/unit/`만으로 측정되었는가 | Pass | `build/reports/unit/`은 unit 오브젝트만 포함 (tdd 미합산). |
| U-7 | 테스트 주석에 AI 작성 정보와 HITL 태그가 있는가 | Pass | 단위 테스트 66건 전부 6종 태그 확인. |
| U-8 | 의미 없는 테스트(단언 없음/커버리지용)가 없는가 | Fail | 단언은 전부 있으나 의미가 불명확하거나 문서와 불일치하는 단언이 있다: `test_unit_val.cpp` L354 `in.fire + in.overtemp + in.adult - 2` 및 L355–356 `&` 비트 단언; `test_unit_ser_core.cpp` `CoreUnit.EventContainsDefinedFields` (주석은 속도 INVALID 확인이라 쓰지만 실제 단언은 ADULT, L479 죽은 대입); TDD `SerializesInFixedOrder...` `strncmp` 길이 66이 문자열 길이 67과 다름. → R1-18 |
| U-9 | Stub/Mock이 필요한 곳에만, 과하지 않게 사용되었는가 | Pass | Mock·Stub 없음. |
| U-10 | 예상 결과가 설계/요구사항에서 도출되었고 구현 코드를 베끼지 않았는가 | Fail | 다음 테스트는 SSR-008 위배 상태(시각 1.0 s 공백 후 스냅샷을 fresh로 기대)를 정답으로 고정한다: `ValUnit.PendingHoldTimeoutOnceAndReset`, `ValUnit.PendingStateKeptWhileCrashInvalid`, `TddVal.PendingHoldsUpToOneSecondThenTimesOutOnce` (R1-04). `CmdUnit.AbsentCommandIsNeitherValidNorRejected`는 구현의 `present` 처리 결과를 기대값으로 고정한다 (R1-28). |
| U-11 | 테스트가 반복 실행 시 동일한 결과를 내는가 | Pass | 시계·난수 미사용. 결정론 반복 시험 존재 (`CoreIntegration.ReplayIsDeterministic`, `TcNf001DeterministicReplay`). |
| **6. 통합 테스트 (SWE.5)** | | | |
| T-1 | 인터페이스 중심의 컴포넌트 연동 테스트인가 | Pass | `tests/integration/test_integration_core.cpp`는 `ecl_core_*` 공개 API만 호출하고 Mock이 없다. |
| T-2 | 모든 인터페이스와 대상 요구사항에 테스트가 있고 양방향 추적되는가 | Pass | `ecl_core_*` 전 공개 API와 `ecl_log_test_set_counter` 호출. SRS에서 IT 검증으로 지정된 8건 (SRQ-FUN-021/022/023/027/030, SRQ-LIM-001/003, SSR-027) 모두 `traceability.md` IT 열에 항목 있음. |
| T-3 | 정상/오류/경계/순서·타이밍 조건이 검증되는가 | Fail | 타이밍 시나리오가 호출 계약(0.010 s 격자)을 위반하는 입력으로 짜여 있다. `CoreIntegration.PendingTimeoutFlow` (0.5 → 1.0 → 2.0 → 2.01 s 스냅샷), `CoreIntegration.TickStaleThenRecover` (1.0 수신 후 1.22 스냅샷을 NORMAL로 기대). 이 기대값은 SSR-008에 어긋난다 (R1-04). |
| T-4 | 모든 테스트 함수에 Doxygen 주석(목적, 입력값, 절차, 예상 출력값)이 있는가 | Pass | 9건 전부 `@brief [목적]`, `@par Input/Procedure/Expected` 확인. |
| T-5 | 경계값 분석·동치 클래스·PICT 조합이 적용되었는가 | Pass | 벡터 V 23행 (`docs/test-vectors/vector-v.csv`, 0.200/0.210 경계 포함, 명령 무효 행 포함). `kVectorV`와 CSV 대조 일치 (거절 명령 side 표기 "middle"만 9로 치환). |
| T-6 | 함수 커버리지 100%를 LLVM 리포트로 직접 확인했는가 | Pass | `build/reports/integration/coverage-report.txt` TOTAL Functions 58/58 = 100.00%. (기존 보고서 기준. 리전 91.68%는 게이트 아님) |
| T-7 | 테스트 성공률 100%인가 | Pass | `build/reports/integration/test-results.xml`: tests=9, failures=0. (기존 보고서 기준) |
| T-8 | 커버리지가 `tests/integration/`만으로 측정되었는가 | Pass | `build/reports/integration/` 별도 측정. |
| T-9 | 테스트 함수 주석에 AI 작성 정보와 HITL 태그가 있는가 | Pass | 9건 전부 6종 태그. |
| T-10 | 의미 없는 테스트가 없고 Stub/Mock 의존이 최소인가 | Pass | 의미 없는 테스트 미발견. Mock 없음. |
| T-11 | 통합 전략과 순서가 기록되어 있는가 | Fail | 파일 머리 2행의 "하향식, 시나리오 단위" 한 줄뿐이다. 컴포넌트 결합 순서와 근거가 문서로 없다. → R1-24 |
| **7. 시스템 테스트 (SWE.6)** | | | |
| S-1 | 모든 케이스가 존재하는 요구사항 ID를 참조하는가 | Pass | `docs/system-test/system_tests.json` 35건의 `basis_ids`가 전부 요구사항 목록 91건에 존재 (unknown 0). |
| S-2 | 요구사항 커버리지 100%: 모든 요구사항이 [제안]이 아닌 케이스로 1개 이상 덮이는가 | Fail | (a) 제약 8건(SRQ-CON-001~008)이 케이스 없이 `open_issues`로만 제외됨. CLAUDE.md 2장 "요구사항 커버리지 100%, 예외 없음"에 어긋난다 (R1-10). (b) TC-F-017/018(SRQ-FUN-024/025, SRQ-CON-009)은 실행되지 않는데 커버리지에 계산됨 (R1-11). |
| S-3 | 미결 사항(테스트 불가 요구사항)이 0건인가 | Fail | `system_tests.json` `open_issues` 8건 (Q-01~Q-08, 전부 SRQ-CON-*). `checklists.md` 7장은 남으면 100% 미달로 판정한다고 정함. → R1-10 |
| S-4 | 입력이 있는 요구사항에 동치 분할과 경계값 분석이 적용되었는가 | Fail | `has_inputs` 78건 중 62건에 "경계값 분석" 설계 항목이 없다 (`system_tests.json` design). 예: SRQ-FUN-001, SRQ-FUN-006, SRQ-FUN-010, SSR-006, SSR-020~025. → R1-12 |
| S-5 | 다중 파라미터 조합에 PICT가 적용되었는가 | Fail | PICT 모델은 SSR-027 1건뿐 (`docs/system-test/pict_model_arb.txt`). 결과 14행은 9개 파라미터 2-way 전 쌍을 커버함을 확인했다. 그러나 TC-F-001/002는 source×side×action 24 조합을 손으로 나열(S1..S24). 나머지 PICT 항목 82건은 "독립 파라미터 2개 이하"라는 사유로 N/A인데, SRQ-FUN-001(side·action·source 3개) 등은 사실과 다르다. → R1-12 |
| S-6 | 엣지/WoW 케이스 검토, 안전 메커니즘에 고장 주입 케이스 | Pass | WoW `TC-F-019`, 고장 주입 `TC-F-012`(sensor_fault), INVALID 주입 `TC-F-010`, 카운터 포화 `TC-F-016`. |
| S-7 | 각 케이스에 ID, 베이시스, 목적, 입력(값·단위), 절차, 예상 출력, 기법이 있는가 | Fail | ID·베이스·목적·절차·예상·기법·판정 기준은 전 케이스 존재. 입력 값에 단위가 없다 (`speed=2.9`, 시각 `t` 등 km/h·s 표기 0건, 35건 전부). → R1-19 |
| S-8 | 입력은 구체 값이며 기대값은 요구사항 원문에서 도출되었고 판정 기준이 있는가 | Fail | 입력은 구체 값이고 `pass_criteria`는 전 케이스에 있다. 그러나 TC-F-008, TC-F-009, TC-F-011의 기대값은 SSR-008에 위배된다. 예: TC-F-011 S5 이후 2.0 스냅샷(직전 1.03 대비 0.97 s 공백)을 NORMAL로 기대. → R1-04 |
| S-9 | 정의된 테스트 환경에서 실제로 만들 수 있는 입력인가 | Fail | 환경은 "시각 주입 PC/SIL, 호출 간격 0.010 s"(SRQ-LIM-002, OI-003)다. TC-F-008(2.0→12.0 s), TC-F-009(1.04→5.0 s, 6.03→7.0 s), TC-F-011(1.03→2.0 s)은 그 환경의 호출 계약을 벗어난다. → R1-04 |
| S-10 | 중복·무의미 케이스가 없고 무효 클래스는 케이스당 하나인가 | Fail | `techniques.md` L21: "무효 클래스는 케이스당 하나만 넣는다". TC-F-010은 12개 무효 클래스를, TC-F-004는 11개 무효 필드 주입을 한 케이스에 넣었다. → R1-20 |
| S-11 | [제안] 케이스가 커버리지 계산에서 제외되었는가 | Pass | `proposed=false` 35/35, [제안] 케이스 0건. |
| S-12 | 요구사항 ↔ 케이스 양방향 추적이 프로젝트 기준 위치(`docs/traceability.md`)와 일치하는가 | Pass | `traceability.md` §2 시스템 테스트 열의 TC ID와 `system_tests.json` 35건, `ST-ECL-001.xlsx` 내 TC ID 35건 (XML 전수 확인) 일치. 범위 표기 `TC-F-013-01~14`는 14건으로 펼쳐 일치. |
| S-13 | 실행 결과(합격/불합격)가 기록되고 불합격이 없는가 | Fail | 실행된 8개 gtest(33개 케이스 표 + 특수 6건)는 실패 0 (`build/reports/system/test-results.xml`, tests=8, failures=0). TC-F-017/018에는 실행 기록이 없다 (R1-11). 케이스별 합격/불합격 기록 열도 JSON·xlsx에 없다. |
| **공통 확인 사항** | | | |
| C-1 | 양방향 추적성이 상·하위 산출물과 일치하는가 | Fail | 요구사항→설계·코드·테스트 매핑 자체는 누락 없이 존재한다 (`traceability.md` §2 기준 요구사항 91건 전부, 테스트 함수명 전부 실존). 그러나 추적이 가리키는 동작이 틀린 항목이 있다 (R1-01, R1-04, R1-09, R1-17). |
| C-2 | 해당 스킬(architecture-design, detailed-design, implementation, unit-test, integration-test, sw-system-test-cases)의 필수 규칙을 모두 만족하는가 | Fail | 구현 SKILL의 리턴 규칙 (R1-05), 주석 필수 (R1-13, R1-07), TDD RED (R1-08), 시스템 테스트 SKILL의 경계값·PICT·무효 클래스 규칙 (R1-12, R1-20), 커버리지 100% (R1-10). |
| C-3 | 이전 리뷰 결함이 모두 종결되었는가 | N/A | 이번 이터레이션의 첫 통합 리뷰이며 이전 이터레이션 산출물 리뷰 기록이 없다 (`docs/reviews/`의 `requirements-review-001~006.md`는 요구사항 대상). |

### 결함 목록

| ID | 심각도 | 위치 | 설명 | 조치 요청 | 상태 |
|---|---|---|---|---|---|
| R1-01 | Major | `src/ecl_val.c` `judge_stale` (L167–181), `ecl_val_condition` (L397–412) | SSR-008 스냅샷 경로 미구현. 마지막 스냅샷 수신 평가의 `timestamp_s`로부터 0.200 s 초과 시 STALE이어야 하나 판정 항이 없다. 예: t=1.0 수신 후 t=1.5 스냅샷(src=1.5)은 요구상 STALE/DEGRADED인데 코드는 NORMAL. SDD §3.1 식과 OI-004 가정 본문에도 이 항이 있다. | `judge_stale`에 `rx_valid && (timestamp_s − last_rx_s > 0.200 + eps)` 항 추가. 스냅샷 경로에서 `last_rx_s`를 참조. 경계 0.200/0.210 단위 시험과 1.5 s 공백 IT를 추가. | Open |
| R1-02 | Major | `src/ecl_val.c` `bool_effective` (L216–229), `apply_substitution` (L241–262); `src/ecl_arb.c` `decide_hold` (L179–187) | SSR-011 위배 (STALE 중 접근위험 도어). STALE이면 효과 값이 0이 되어 접근위험 TRUE 도어가 LOCK되지 않고 `prev`를 유지한다. 예: 첫 평가에서 `rear_left_approach_risk=1`이고 source 지연 0.2 s 초과면 `lock_left=RELEASE`. SSR-011은 "STALE 중에도 LOCK"을 명시한다. SDD §6 가정 1과 OI-013의 "접근위험 도어만 LOCK 보장"도 실제와 다르다. | STALE 상태에서도 접근위험 TRUE(VALID 또는 STALE 원값)를 `approach[d]=1`로 유지하고 decide_hold/decide_rules에서 LOCK 적용. SDD §6과 OI-013 정정. ST/UT/IT에 STALE+접근위험 케이스 추가. | Open |
| R1-03 | Major | `src/ecl_val.c` `ecl_val_condition_tick` (L414–435, rx_valid 분기, L421–424) | 틱 경로가 SSR-007 기준(`now − source_timestamp_s > 0.200`)을 적용한다. SSR-008은 "마지막 스냅샷 수신 평가의 timestamp_s" 기준이다. 예: ts=1.00, src=0.95 수신 후 tick(1.16)은 요구상 fresh(0.16 s), 코드는 STALE(0.21 s). | 틱 경로는 `now − last_rx_s` 기준으로만 STALE 판정. 효과 값 재사용 정책을 SDD와 맞춤. 경계 1.16/1.21 시험 추가. | Open |
| R1-04 | Major | `tests/system/system_cases.inc` 케이스 TC-F-008, TC-F-009, TC-F-011 (생성원 `tools/gen_system_tests.py`의 add_case 호출부); `tests/unit/test_unit_val.cpp` `ValUnit.PendingHoldTimeoutOnceAndReset`, `ValUnit.PendingStateKeptWhileCrashInvalid`; `tests/tdd/test_tdd_val_ser_core.cpp` `TddVal.PendingHoldsUpToOneSecondThenTimesOutOnce`; `tests/integration/test_integration_core.cpp` `CoreIntegration.PendingTimeoutFlow`, `CoreIntegration.TickStaleThenRecover` | 시험 입력이 호출 계약(0.010 s 격자, SRQ-LIM-002)을 벗어나고, 그 기대값은 SSR-008을 적용하지 않은 결과다. 예: TC-F-008 2.0→12.0 s, TC-F-009 1.04→5.0 s, TC-F-011 1.03→2.0 s 및 2.0→2.22 s. 이 테스트들은 R1-01 수정 시 실패한다. 즉 기대값이 구현에서 역산되었다. | 0.010 s 격자 스냅샷(또는 신선한 source 유지)으로 입력을 재작성하고, 기대값을 SRS 원문에서 다시 도출. 생성기(`tools/gen_system_tests.py`) 수정 후 `system_cases.inc` 재생성. UT/TDD/IT의 해당 테스트도 같이 수정. | Open |
| R1-05 | Major | `src/ecl_arb.c` L209(apply_driver 결과 폐기), `decide_both` L275; `src/ecl_core.c` `reset_status` L32, `build_status` L51; `src/ecl_ovr.c` `apply_request` L49, `update_door` L80; `src/ecl_val.c` `check_bool_signals` L118, `check_other_signals` L144, `mark_stale` L193, `apply_substitution` L241, `track_pending` L298, `build_inputs` L364; `src/ecl_log.c` `store_event` L27; `src/ecl_ser.c` `write_pair` L68, `write_header` L94, `write_validity` L117, `write_all` L141 | 구현 SKILL 리턴 규칙 3·4 위반. 상수 0 또는 무의미 값을 반환하는 함수 17개를 `(void)`로 버린다. 정상 실행 여부를 알 수 없고 호출자가 확인하지 않는다. | 각 함수가 `ecl_ret_t` 또는 판정 결과(bool/상태)를 반환하고, 호출자가 `RET_OK`를 확인하여 전파. `(void)` 캐스트 제거. 단일 종료점 유지. | Open |
| R1-06 | Major | `include/ecl_val.h` L23–30 (`ecl_val_t`: `last_snap`); `src/ecl_val.c` `ecl_val_condition_tick` (L414–435); SDD §3.1 | 설계와 코드가 다르다. 설계는 `last_inputs`(조건화 결과)를 보관하고 틱은 "stale/hold만 갱신"한다고 했다. 코드는 `last_snap`을 보관하고 틱에서 전체를 재조건화하며 `track_pending`을 다시 돌려 틱에서도 `pending_timeout_first`가 발생한다. | 설계 또는 코드를 정정해 일치시키고, 틱에서 PENDING 타임아웃 첫 판정을 허용할지 명시. 설계와 코드 양쪽 근거를 SDD §3.1에 기록. | Open |
| R1-07 | Major | `tests/tdd/test_tdd_cmd_sta_log.cpp` (TEST 8건), `tests/tdd/test_tdd_ovr_arb.cpp` (10건), `tests/tdd/test_tdd_val_ser_core.cpp` (10건), 헬퍼 `unlock_cmd` | TDD 테스트 함수 28건에 Doxygen 주석과 AI/HITL 태그가 없다 (`@tracing`도 없음). 구현 SKILL "모든 소스 파일/함수 주석" 위반. | 각 TEST에 목적·입력·절차·예상 주석과 6종 태그를 추가. 헬퍼에도 동일 적용. | Open |
| R1-08 | Major | 저장소 전체 (`docs/`, `build/reports/tdd/`) | TDD 기능별 RED(실패 선행) 증거가 없다. `checklists.md` 4장 항목 충족 불가. | 기능별 RED 실행 로그(실패 출력, 실행 시각, 커밋 또는 산출물 링크)를 산출물로 남기고 재리뷰에서 제출. | Open |
| R1-09 | Major | `tests/unit/test_unit_log_ovr.cpp` `OvrUnit.OverrideExpiresOnResolveOrTimeout` (L314–339), 추적 행 SSR-035 | SSR-035("접근위험 FALSE 전환 시 첫 억제 시각 삭제")의 UT가 없다. 이 테스트는 override 플래그만 확인한다. 삭제가 없으면 재진입 후 같은 source 해제가 잘못 override 된다. 현재 유일한 검증은 시스템 TC-F-008의 12.0→20.0 s 공백 구간이고, 그 구간은 R1-04로 무효다. | approach 1→0→1 재진입 시 첫 억제 재기록(ack=0)을 검증하는 UT 추가. | Open |
| R1-10 | Major | `ci/check_system_coverage.py` (`excluded`, `testable`); `docs/system-test/system_tests.json` `open_issues` Q-01~Q-08; `docs/traceability.md` SRQ-CON 행 | CLAUDE.md 2장은 요구사항 커버리지 100%를 예외 없이 요구한다. 게이트는 SRQ-CON-001~008 8건을 open_issues로 제외해 "100%"를 계산한다. 이 8건 중 UT를 요구하는 SRQ-CON-002/003/006/007은 UT 증적이 없다 (`traceability.md` 단위 열 "-"). INS/BLD 증적(검사 기록, 빌드 로그)도 저장소에 없다. | 8건 각각에 INS/BLD/UT 증적을 산출하고(검사 기록, 테스트 케이스 ID) 추적 행을 갱신. 제외 로직 제거 후 100% 판정. 증적이 없으면 Open 유지. | Open |
| R1-11 | Major | `docs/system-test/system_tests.json` TC-F-017, TC-F-018 (SRQ-FUN-024/025, SRQ-CON-009); `tests/system/*` | 두 케이스는 실행 코드가 없다 (`AllGeneratedCasesPass`, `TcF014`~`TcNf003` 어디에도 없음). 표시 계층(CMP-WEB)은 이터레이션 3 범위인데 이번 이터레이션 커버리지 계산에 포함되어 있다. 실행 기록이 없다. | 이터레이션 범위를 명시해 Web 케이스를 이 이터레이션 커버리지에서 분리(사용자 승인 필요)하거나, 실행 결과를 기록. 결과 열 추가. | Open |
| R1-12 | Major | `docs/system-test/system_tests.json` `design` 항목 (BVA/PICT); `docs/system-test/pict_model_arb.txt`; TC-F-001, TC-F-002 | (a) 입력 요구사항 78건 중 62건에 경계값 분석이 없다. (b) PICT는 SSR-027 1건뿐이다. (c) TC-F-001/002는 24 조합을 수작업으로 나열했다. (d) PICT N/A 82건의 사유 "독립 파라미터 2개 이하"는 SRQ-FUN-001, SRQ-FUN-010 등에서 사실과 다르다. 생성된 PICT 결과 14행은 2-way 커버리지를 만족함은 확인했다(누락 쌍 0). | (a) 요구사항별 분해능 단위 경계값 설계 추가. (b)(c) side×action×source, fire×overtemp×adult, 다중 입력 요구에 PICT 모델·결과 추가 후 TC-F-001/002를 PICT 결과로 대체. (d) N/A 사유를 실제 파라미터 수로 정정. | Open |
| R1-13 | Minor | `src/ecl_arb.c` L288; `src/ecl_cmd.c` L38; `src/ecl_core.c` L150, L170, L187, L204, L217, L230; `src/ecl_log.c` L44, L58, L80, L99; `src/ecl_ovr.c` L100, L113; `src/ecl_ser.c` L149; `src/ecl_sta.c` L15; `src/ecl_val.c` L384, L397, L414 | 공개 함수 정의 16개에 Doxygen과 HITL 태그 블록이 없다 (헤더에만 있음). 구현 SKILL "모든 소스 함수" 위반. | 정의부에 동일한 주석 블록 추가. 헤더와 내용 동기화. | Open |
| R1-14 | Minor | `src/ecl_arb.c` L16 (`P_RANK[19]`, 값 99, 순위 1~11); `src/ecl_ser.c` L17–29 (배열 크기 2·4·3·19), L79 (`2U`), L96–125 (`name_of(..., 2/4/3/19)`), L119 (`key[40]`); `src/ecl_val.c` L19 (`BOOL_SIGS[9]`), L120 (`values[9]`), L127 (`i < 9`), L243–250 (`bool_effective(..., 1)` 등 대체값) | 매직 넘버. SDD "상수는 `ecl_types.h`" 위반. | `ECL_BOOL_SIGNAL_COUNT`, `ECL_REASON_COUNT`, `ECL_STATE_COUNT`, `ECL_VALIDITY_COUNT`, `ECL_RANK_NONE`, 대체값 상수 등으로 명명하여 `ecl_types.h`에 정의. | Open |
| R1-15 | Minor | `tests/common/ecl_test_util.h` L37 (`ecl_set_command`, void); `tests/unit/test_unit_arb.cpp` L17 (`Ctx::driver`, void); `tests/system/test_system.cpp` L62 (`expect_status`, void); `tests/common/sha256.h` L22, L31, L75 (`reset`, `update`, `block`, void) | 구현 SKILL은 GTest `TEST` 본문 외의 테스트 헬퍼도 리턴 규칙을 따르라고 한다. 위 헬퍼는 void다. | 상태 값을 반환하도록 바꾸고 호출부에서 `ASSERT_EQ(RET_OK, …)`로 검사. | Open |
| R1-16 | Minor | `docs/architecture.md` §4.8 vs `docs/detailed-design.md` §3.7 vs `tests/unit/test_unit_ser_core.cpp` `SerUnit.SerializesExactFormat`; `docs/detailed-design.md` §1·§3.1·§3.4·§3.7; `docs/architecture.md` §4.2·§4.4 | 설계 문서 간 및 설계와 코드의 불일치. (a) 직렬화 필드: SAD §4.8은 `input_validity.speed`, `command_rejected`를 말미에 둔다. SDD §3.7·코드·테스트는 `validity_speed`, `command_rejected`를 중간에 둔다. (b) SDD §3.4의 `decide_crash`, `apply_reject`는 코드에 없다. (c) SDD §3.7의 `tmp[640]`·`append_pair`는 코드에 없다(건식 2-pass). (d) SAD §4.2 출력 필드 `invalid_substituted`, `pending_hold`, `driver_command`, `crash_invalid_or_ignition_invalid`와 SAD §4.4 `suppress_unlock`, `override_ack`이 코드(`substituted`, `hold_reason`, `command`, `suppress`, `ack`)와 다르다. (e) 코드의 정적 함수 다수가 SDD에 없다. | 단일 기준(SDD)으로 SAD·SDD·코드를 정렬하고 각 불일치를 해소. 직렬화 형식은 SDD에서 한 번만 정의. | Open |
| R1-17 | Minor | `tests/unit/test_unit_val.cpp` `ValUnit.NullArgumentsReturnErrNull` (L29: SSR-019, SSR-033, SSR-036, SSR-037); `tests/unit/test_unit_arb.cpp` `ArbUnit.NullArgumentsReturnErrNull` (L36: SRQ-FUN-018, SRQ-FUN-021, SSR-019, SSR-033, SSR-036, SSR-037); `tests/unit/test_unit_cmd_sta.cpp` `CmdUnit.ValidCommandIsAccepted` (L24: SSR-034); 기타 `@tracing`에 `ecl_cmd_interpret` 같은 함수명 혼입 | NULL 방어 테스트가 무관한 요구사항을 추적한다. 추적이 부풀려져 있어 양방향 추적성 신뢰도를 떨어뜨린다. | 각 테스트의 `@tracing`을 실제 검증 대상 요구사항으로 한정. | Open |
| R1-18 | Minor | `tests/unit/test_unit_val.cpp` L354–356 (`ValidInputsAreReflected`); `tests/unit/test_unit_ser_core.cpp` `CoreUnit.EventContainsDefinedFields` (L473–496, 주석과 단언 불일치, L479 죽은 대입); `tests/tdd/test_tdd_val_ser_core.cpp` L122 (`strncmp` 66자, 문자열 67자); `tests/system/test_system_special.cpp` L262 (`reinterpret_cast`로 0x5A 검사); `tests/system/test_system.cpp` `SystemTest.CaseTableIsComplete` (L127–131, 개수만 확인) | 의미가 불명확하거나 문서와 다른 단언, 사실상 항상 참인 검사. | 단언을 의도대로 명확하게 쓰고 주석과 일치시킴. 죽은 대입 제거. `strncmp` 길이를 전체 문자열로 수정. | Open |
| R1-19 | Minor | `docs/system-test/system_tests.json` 35건 `inputs`; `docs/system-test/ST-ECL-001.xlsx` | 입력 값에 단위(km/h, s, 무차원 enum)가 없다. checklists 7장 "입력 값(값·단위)" 위반. | 입력 열에 단위를 명시(예: `speed=2.9 km/h`, `t=1.010 s`). | Open |
| R1-20 | Minor | `docs/system-test/system_tests.json` TC-F-004, TC-F-010 | `techniques.md` L21 "무효 클래스는 케이스당 하나만": 두 케이스가 여러 무효 클래스를 한 케이스에 넣었다. | 무효 클래스별로 케이스를 분리하거나, 예외가 필요하면 기법 규칙에 근거를 명시. | Open |
| R1-21 | Minor | `tests/common/sha256.h` (전체) | SHA-256 구현에 표준 벡터(KAT) 검증이 없다. SRQ-QUA-FUN-001의 "해시 = SHA-256" 주장을 검증할 수 없다. | `""`, `"abc"` 등 FIPS 180-4 벡터 테스트 추가. | Open |
| R1-22 | Minor | `src/ecl_core.c` `ecl_core_init`, `run_pipeline`, `ecl_core_evaluate` 등 `if (ret == RET_OK)` 분기 | 단위 리포트에서 `ecl_core.c` 분기 9개가 미실행(Branch 82%). 내부 호출이 실패할 수 없는 방어 분기이며 MC/DC 게이트는 통과하나 도달 불가 코드다. | 도달 불가 분기를 제거하거나, 도달 불가 근거를 설계에 기록. | Open |
| R1-23 | Minor | `include/ecl_val.h` L23–30, `ecl_ovr.h` L23–28, `ecl_log.h` L23–29, `ecl_core.h` L26–31; `tests/integration/test_integration_core.cpp` L252; `tests/system/test_system_special.cpp` L148 | 헤더 주석은 "private"이라 하나 구조체 필드가 전부 공개이고 테스트가 내부 필드(`core.log`)를 직접 건드린다. SAD §1의 INS 검사 증적도 없다. | 불투명 구조체 또는 시험용 접근자로 캡슐화하고 INS 검사 기록 추가. 주석을 사실에 맞게 수정. | Open |
| R1-24 | Minor | `tests/integration/test_integration_core.cpp` L2 | 통합 전략은 한 줄 설명뿐이고 컴포넌트 결합 순서와 근거가 없다 (checklists 6장 마지막 항목). | 통합 순서(예: VAL→CMD→OVR→ARB→STA→LOG→SER)와 단계별 시험 목표를 문서화. | Open |
| R1-25 | Minor | `src/ecl_arb.c` `driver_applies` (L72); `src/ecl_ovr.c` `door_requested` (L27) | 유효 명령·도어 적용 판정 로직이 두 모듈에 중복. 한쪽 수정 시 불일치 위험. | 공통 판정을 `ecl_types.h` 또는 공개 헤더 정의로 한 번만 두거나 설계에 중복 허용 근거를 기록. | Open |
| R1-26 | Minor | `docs/architecture.md` §4.10, §7 (CMP-WEB) | CMP-WEB에 SRQ-FUN-024/025, SRQ-CON-008/009를 할당했으나 인터페이스는 개요만 있다 (시그니처·오류·조건 없음). | 이터레이션 3 이연을 명시하고 인터페이스를 상세화하거나, 이번 이터레이션 커버리지에서 분리 (R1-11과 연계). | Open |
| R1-27 | Minor | 빌드 산출물 (`build/reports/`, 빌드 로그 없음) | 컴파일러 경고 0 증적(`-Wall -Wextra` 빌드 로그)이 없다. | Docker 이미지 `claudeclangbuild`에서 빌드한 경고 로그를 `build/reports/`에 남기고 재리뷰에서 제출. | Open |
| R1-28 | Minor | `src/ecl_cmd.c` L46 (`raw->present == 1`만 인정, 그 외는 명령 없음으로 처리하며 `rejected=0`); `tests/unit/test_unit_cmd_sta.cpp` `CmdUnit.AbsentCommandIsNeitherValidNorRejected` (L147–156, present=2) | SRQ-FUN-007은 "형식이 틀린 명령"을 거절하라고 한다. present가 0/1 밖인 입력을 조용히 무시하는 규칙은 요구에 근거가 없다. 테스트 기대값도 구현 결과를 역산한 것이다. 설계 SDD §3.2는 "그 외 0"이라고 적었으나 요구 근거를 남기지 않았다. | 요구 해석을 결정하고 근거를 SRS/SDD에 기록. 거절로 볼 경우 `rejected=1`로 바꾸고 기대값을 요구 원문에서 다시 도출. | Open |

### 외부 확인 대기 항목 (DEC-003) 판정

| 가정 ID | 쟁점 | 기록 위치 | 사용자 승인 (DEC-003) | 요구 본문과 모순 | 구현 상태 | 판정 |
|---|---|---|---|---|---|---|
| OI-003 | 평가주기 0.010 s, 호출 계약 | `open-issues.md` OI-003 | 있음 | 없음 | 시험 입력이 계약을 위반 (R1-04) | 가정은 Closed. 위반은 R1-04로 Open |
| OI-004 | 마지막 스냅샷 후 0.200 s 초과 STALE | `open-issues.md` OI-004 | 있음 | 없음 (SSR-008과 일치) | 스냅샷 경로 미구현 (R1-01) | 가정은 Closed. 구현 누락은 R1-01로 Open |
| OI-005 | state 열거, 우선순위 | `open-issues.md` OI-005 | 있음 | 없음 | 일치 | Closed (사용자 승인 가정) |
| OI-006 | SSR-027 동시 조건 우선순위 | `open-issues.md` OI-006 | 있음 | 없음 | P1~P11 순서 일치 (`ecl_arb.c`) | Closed (사용자 승인 가정) |
| OI-008 | INVALID 대체 방향 | `open-issues.md` OI-008 | 있음 | 없음 | 일치 (`apply_substitution`) | Closed (사용자 승인 가정) |
| OI-010 | 정지 3.0 km/h 기준, gear 미사용 | `open-issues.md` OI-010 | 있음 | 없음 | 일치 | Closed (사용자 승인 가정) |
| OI-011 | override 의미·경계·source 규칙 | `open-issues.md` OI-011 | 있음 | 없음 | 일치 (`ecl_ovr.c` `apply_request`) | Closed (사용자 승인 가정) |
| OI-012 | gear 미사용 | `open-issues.md` OI-012 | 있음 | 없음 | 일치 | Closed (사용자 승인 가정) |
| OI-013 | DEGRADED 중 직전 출력 유지, "접근위험 도어만 LOCK 보장" | `open-issues.md` OI-013 | 있음 | 가정 문장은 SSR-011과 충돌하지 않지만, 가정이 주장하는 "접근위험 도어 LOCK 보장"이 구현에서 성립하지 않음 | 위반 (R1-02) | **Open** (가정 종결 조건 ③ 불충족: 구현이 요구 본문 SSR-011과 모순) |
| OI-016, OI-021, OI-022 | 이유코드, 이벤트 기록, 포화 동작 | `open-issues.md` | 있음 | 없음 | 일치 (`ecl_core.c` `log_if_changed`, `ecl_log.c`) | Closed (사용자 승인 가정) |

### 열린 결함 요약

- 열린 결함: **28건** (Major 12: R1-01~R1-12, Minor 16: R1-13~R1-28). 전부 Open (첫 통합 리뷰).
- 다음 단계 진행: **불가** (열린 결함 28건). Approved 조건(모든 Fail 해소와 모든 결함 종결) 미충족.
- 재리뷰 요청 시 확인 우선순위: R1-01~R1-04 (시간 규칙과 시험 오라클), R1-02 (SSR-011), R1-05 (리턴 규칙), R1-10~R1-12 (시스템 테스트 커버리지).
