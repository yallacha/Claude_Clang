# 리뷰 기록: 이터레이션 1 (제어 코어) 통합 재리뷰 002

- 리뷰 방식: `review` 스킬, 체크리스트 `references/checklists.md` 2·3·4·5·6·7장과 공통 확인 사항(이전 기록 `iteration-1-review-001.md`의 항목 ID 체계를 그대로 사용)
- 조치 결정 반영: DEC-011(iteration 트랙: present 0/1 외 거절), DEC-012(표시 계층 SRQ-FUN-024/025, SRQ-CON-008/009 이터레이션 3 이연, INS/BLD 제약은 `ci/inspection.sh`의 TC-I 케이스로 덮음)은 승인 사항이므로 결함으로 세지 않았다.
- 확인 방법: 산출물 파일을 직접 열어 읽고 grep으로 대조했다. 빌드·테스트·품질 게이트는 재실행하지 않았다. 게이트 수치는 `build/reports/` 기존 보고서를 읽어 확인했고, 보고서 시각(15:15:31)이 대상 소스 시각(ecl_val.c 15:13:50, ecl_ser.c 15:12:59)보다 늦음을 확인했다.
- 수정·생성 범위: 이 기록 파일 외에는 아무 파일도 수정·생성하지 않았다. 요구사항 문서(`docs/SRS.md`, `docs/safety-requirements.md`)는 입력으로만 사용했다.

## 리뷰 기록
- 대상: 이터레이션 1 (제어 코어): `docs/architecture.md`, `docs/detailed-design.md`, `include/ecl_*.h`, `src/ecl_*.c`, `tests/{tdd,unit,integration,system}/`, `docs/system-test/`, `tools/gen_system_tests.py`, `ci/check_system_coverage.py`, `ci/inspection.sh`, `docs/traceability.md` §2, `docs/reviews/tdd-red-evidence-001.md` / 버전: 리뷰 시점 워킹트리 (SAD-ECL-001 v0.1, SDD-ECL-001 v0.1, SRS-ECL-001 v0.5, DEC-011·DEC-012 반영) / 체크리스트: `review` 스킬 `checklists.md` 2장~7장 + 공통 확인 사항
- 결과: **Rejected**
- 다음 단계 진행: **불가 (열린 결함 15건: Major 7, Minor 8)**

### 체크리스트 판정

| # | 체크 항목 | 판정 | 근거/비고 |
|---|---|---|---|
| **2. 아키텍처 (SWE.2)** | | | |
| A-1 | 모든 컴포넌트가 정의되고 책임이 단일한가 | Pass | `docs/architecture.md` §3. CMP-WEB는 DEC-012로 이연. |
| A-2 | 모든 컴포넌트에 인터페이스가 정의되어 있는가 | N/A | CMP-WEB 인터페이스 상세화는 DEC-012(이터레이션 3 이연)에 따른다. `architecture.md` L187 "이터레이션 3에서 상세화". 승인된 사유. |
| A-3 | 컴포넌트 간 통신이 인터페이스로만 이루어지는가 | Pass | `src/ecl_*.c` include는 공개 헤더만 사용. `ecl_core.c`만 하위 헤더를 include. |
| A-4 | 높은 응집도·낮은 결합, 순환 의존 없음 | Pass | 헤더 include 그래프 DAG. |
| A-5 | 모듈화·캡슐화·SOLID 준수 | Pass | R1-23 종결. 구조체 필드는 C 관례상 공개이며 `ecl_val.h`, `ecl_core.h` 주석이 "관례"로 정정됨. 시험 접근은 `ecl_core_test_get_log`(`ECL_TEST_BUILD` 가드, `ecl_core.h` L105)만 사용. 제품 빌드 접근자 없음은 TC-I-007(`inspection.sh` SRQ-FUN-039)로 확인. |
| A-6 | 동적 거동과 자원 소비 목표가 기술되어 있는가 | Pass | `architecture.md` §5. |
| A-7 | 모든 SW 요구사항이 컴포넌트에 할당되어 있는가 | Pass | `traceability.md` §2 기준 누락 없음(DEC-012 이연 4건 제외). |
| A-8 | 설계 결정의 근거가 기록되어 있는가 | Pass | `architecture.md` §1 결정표. |
| **3. 상세 설계 (SWE.3)** | | | |
| D-1 | 모듈과 함수가 정의되고 컴포넌트와 추적되는가 | Fail | SDD §5 추적 표에 존재하지 않는 함수명: `check_signals`(L131), `judge_stale`(L132), `decide_crash`(L137), `apply_reject`(L141). SDD에 없는 static 함수: `compute_effective`, `speed_moving_of`, `claim_slot`, `name_of`, `write_pair`, `write_header`, `write_validity`, `write_all`, `initial_status`. → R1-16 잔여. |
| D-2 | 기준 이상 모듈에 인터페이스가 정의되어 있는가 | Pass | DEC-009 기준, `include/` 8종 공개 함수 정의. |
| D-3 | 인터페이스 정의 모듈 간 통신이 인터페이스로만 이루어지는가 | Pass | D-2와 동일. |
| D-4 | 함수·모듈이 단일 책임 원칙을 만족하는가 | Pass | Lizard 함수 59개 최대 CCN 10, 최대 NCSS 29(`build/reports/lizard/lizard.xml`). |
| D-5 | 클린코드 원칙(명명, 작은 함수, 중복 없음, 매직 넘버 없음) | Fail | 주요 상수는 `ecl_types.h`에 정의됨(`ECL_BOOL_SIG_COUNT`, `ECL_REASON_COUNT`, `ECL_RANK_*`, `ECL_SER_KEY_MAX`). 잔여 매직 넘버: `src/ecl_ser.c` L55(`1U`), L59(`2U`), L126(`1U`). → R1-14 잔여. |
| D-6 | 함수별 입력·출력·리턴값·오류 처리가 기술되어 있는가 | Fail | SDD §3.8 L114 `ecl_val_condition(&val, snap, snap->timestamp_s, &inputs)`는 코드(`src/ecl_core.c` L199, 3인자)와 다르다. SDD §3.1 L48은 `build_inputs`가 `ecl_ret_t`를 반환하고 `apply_substitution`이 있다고 기술하나 코드는 구조체를 반환하고 `compute_effective`가 있다(`src/ecl_val.c` L227, L305). → R1-16 잔여. |
| D-7 | 아키텍처 및 요구사항과 일관되고 양방향 추적되는가 | Fail | SDD §6.1 L149 "STALE이면 효과 값 모두 0"은 SRS 정의(SRS L33 "단 crash_status의 CONFIRMED는 STALE이어도 SSR-001이 인정")와 모순. 코드도 동일하게 구현됨(N-01). SDD §6.2 L150 "STALE 판정은 마지막 수신 후 경과"는 코드(보존된 수신 시점 지연 포함)와 다르다(R1-03). |
| **4. 구현 (소스)** | | | |
| I-1 | 프로덕션 코드의 모든 함수가 리턴값을 가지는가 | Pass | `src/`·`include/`에 `void` 함수 0건(grep). |
| I-2 | 프로덕션 코드의 `return`이 함수 마지막에 단 한 번인가 | Pass | 8개 `.c` 파일의 모든 함수 단일 `return`(전수 확인). |
| I-3 | 리턴값이 없는 경우 정상 실행 여부를 알 수 있는 상태 값을 리턴하는가 | Pass | R1-05 17개 함수 모두 의미 있는 값 반환으로 변경됨: `ecl_arb.c` `decide_both`는 `reason_code`, `ecl_core.c` `initial_status`/`build_status`는 구조체, `ecl_ovr.c` `apply_request`는 ack, `update_door`는 override, `ecl_val.c` `check_bool_signals`는 INVALID 수, `track_pending`은 hold, `ecl_log.c` `claim_slot`은 인덱스, `ecl_ser.c` `write_*`는 위치. 상수 0 반환 0건. |
| I-4 | 호출자가 리턴값을 확인·처리하는가 | Fail | 프로젝트 함수 반환은 모두 확인됨. 그러나 프로덕션 코드에 `(void)` 캐스트로 반환값을 버리는 호출이 남아 있다: `src/ecl_core.c` L37, L59, L95, L102, L127–129; `src/ecl_val.c` L311, L315–316, L343, L389; `src/ecl_ser.c` L53, L55, L93–94; `src/ecl_ovr.c` L95, L115; `src/ecl_log.c` L44; `src/ecl_cmd.c` L56. 대상은 libc(`memset`, `memcpy`, `strcpy`, `strcat`)이다. → N-02. |
| I-5 | 상세 설계와 일치하며 설계에 없는 동작이 없는가 | Fail | STALE 중 CONFIRMED 무시(N-01), 틱 STALE 기준 설계 불일치(R1-03), 첫 수신 전 틱의 validity 전부 STALE(N-03), SDD §3.2 L59 `present` 기술이 DEC-011·코드와 불일치(R1-28). |
| I-6 | 내부 함수/데이터가 `static`으로 은닉되고 인터페이스로만 접근되는가 | Pass | 비공개 함수 전부 `static`. 파일 스코프 변수는 `static const`뿐. |
| I-7 | 컴파일러 경고 0 | Pass | `build/reports/inspect/warnings.txt` 0행. `inspect.txt`의 SRQ-CON-001 `clang -std=c11 -Wall -Wextra -Wpedantic -Werror` PASS. R1-27 종결. |
| I-8 | CppCheck 위반 0건, 억제 주석·억제 파일 없음 | Pass | `build/reports/cppcheck/cppcheck.xml` 오류 0건. `src`·`include`에서 `cppcheck-suppress`·NOLINT 등 억제 주석 0건(이번 grep). |
| I-9 | Lizard: 순환복잡도 ≤ 10, 함수 라인수 ≤ 80 | Pass | `build/reports/lizard/lizard.xml` Function 항목 59개: 최대 CCN 10(`ecl_arb_decide`), 최대 NCSS 29. |
| I-10 | 소스·함수 주석이 Doxygen이며 AI 작성 정보가 있는가 | Fail | `src/ecl_val.c`(18), `src/ecl_ser.c`(7), `src/ecl_ovr.c`(6), `src/ecl_log.c`(6)의 모든 주석이 `@@HITL@@` 플레이스홀더뿐이다. `@authored_by`, `@requested_by`, `@review_status` 등 6종 태그가 없다. 이전 기록 R1-13(정의부 누락)보다 악화. → R1-13. |
| I-11 | 리뷰 상태(Human Review) 태그가 있고 사용자 리뷰 전 `Not Reviewed`인가 | Fail | 위 4개 파일에는 `@review_status` 자체가 없다. 나머지 파일은 `Not Reviewed`로 일관되며 `Reviewed` 0건. → R1-13. |
| I-12 | TDD 테스트가 `tests/tdd/`에 분리되고 별도 결과·별도 커버리지로 산출되었는가 | Pass | `tests/tdd/` 3파일 28 TEST, `build/reports/tdd/test-results.xml` tests=28 failures=0, 별도 커버리지 디렉터리. |
| I-13 | 기능별 RED(실패 선행) 증거가 있는가 | Fail | `docs/reviews/tdd-red-evidence-001.md`는 전체 묶음에 대해 1회 링크 실패(`undefined reference`)만 기록한다. 링크 오류는 어서션 실패가 아니며, 기능별 RED 로그가 없음을 문서가 스스로 인정한다("프로세스 편차"). 첫 실행 27/28에서 실패한 테스트의 기대값을 수정한 기록도 있다. → R1-08. |
| I-14 | 상세 설계와의 추적성이 유지되는가 | Fail | SDD §5의 존재하지 않는 함수명과 누락된 static 함수. → R1-16. |
| **5. 단위 테스트 (SWE.4)** | | | |
| U-1 | 테스트가 상세 설계/요구사항과 양방향 추적되는가 | Pass | R1-09, R1-17 종결. `@tracing` 행과 실제 검증 대상이 일치. |
| U-2 | 모든 테스트 함수에 Doxygen 주석(목적, 입력값, 절차, 예상 출력값)이 있는가 | Pass | 단위 73건: `@par Input` 73, `@par Procedure` 73, `@par Expected` 73(grep 전수). |
| U-3 | 경계값 분석·동치 클래스·PICT 조합이 적용되었는가 | Pass | 0.200/0.210(`test_unit_stale_reentry.cpp` L42–52, L95–105), 1.000/1.010, 3.0/2.9, 10.0/10.01 등. |
| U-4 | MC/DC 커버리지 100%를 LLVM 리포트로 직접 확인했는가 | Pass | `build/reports/unit/mcdc-report.txt` TOTAL MC/DC Conditions 94, Missed 0, 100.00%. 단 Branches Missed 9(97.54%)는 R1-22 설계 근거(SDD §6.5)로 수용. |
| U-5 | 테스트 성공률 100%인가 | Pass | `build/reports/unit/test-results.xml` tests=73, failures=0, errors=0. |
| U-6 | 커버리지가 `tests/unit/`만으로 측정되었는가 | Pass | `build/reports/unit/`은 unit 오브젝트만. |
| U-7 | 테스트 주석에 AI 작성 정보와 HITL 태그가 있는가 | Pass | 단위 73건 `@authored_by` 73건(TEST 개수와 일치). |
| U-8 | 의미 없는 테스트(단언 없음/커버리지용)가 없는가 | Fail | `tests/unit/test_unit_val.cpp` L355–357 비트 `&`·합 기반 단언(의도 불명확, R1-18 잔여). `tests/unit/test_unit_ser_core.cpp` L479 죽은 대입(L482에서 덮어씀). `ValUnit.StaleInputsAreNotReflected`(L373–385)는 SSR-001 위반값을 단언(N-01). |
| U-9 | Stub/Mock이 필요한 곳에만, 과하지 않게 사용되었는가 | Pass | Mock 없음. |
| U-10 | 예상 결과가 설계/요구사항에서 도출되었고 구현 코드를 베끼지 않았는가 | Fail | `ValUnit.StaleInputsAreNotReflected`의 `crash_confirmed=0`은 DEC-011(요구사항 트랙) (4)가 "수정 필요"로 명시한 구현 결과 역산 기대값이다(N-01). R1-04의 펜딩 계열 시험은 0.010 s 격자로 재작성되어 해소. |
| U-11 | 테스트가 반복 실행 시 동일한 결과를 내는가 | Pass | 시계·난수 미사용. |
| **6. 통합 테스트 (SWE.5)** | | | |
| T-1 | 인터페이스 중심의 컴포넌트 연동 테스트인가 | Pass | `ecl_core_*` 공개 API만 호출, Mock 없음. |
| T-2 | 모든 인터페이스와 대상 요구사항에 테스트가 있고 양방향 추적되는가 | Pass | `traceability.md` §2 IT 열과 `@tracing` 일치. |
| T-3 | 정상/오류/경계/순서·타이밍 조건이 검증되는가 | Fail | `CoreIntegration.TickStaleThenRecover`(`tests/integration/test_integration_core.cpp` L284–299)는 평가 1.00 s 후 틱을 1.21 s에 호출한다(1.01–1.20 호출 없음). OI-003이 정한 시험 하네스의 0.010 s 호출 관행과 다르다. 값 자체는 격자 위에 있다. → R1-04. |
| T-4 | 모든 테스트 함수에 Doxygen 주석이 있는가 | Pass | 9건 `@par Input/Procedure/Expected`(grep 전수). |
| T-5 | 경계값 분석·동치 클래스·PICT 조합이 적용되었는가 | Pass | 벡터 V 23행 및 경계 시험. |
| T-6 | 함수 커버리지 100%를 LLVM 리포트로 직접 확인했는가 | Pass | `build/reports/integration/coverage-report.txt` TOTAL Functions 59/59 100.00%. 리전 91.03%는 게이트 아님. |
| T-7 | 테스트 성공률 100%인가 | Pass | `build/reports/integration/test-results.xml` tests=9, failures=0. |
| T-8 | 커버리지가 `tests/integration/`만으로 측정되었는가 | Pass | 별도 측정 디렉터리. |
| T-9 | 테스트 함수 주석에 AI 작성 정보와 HITL 태그가 있는가 | Pass | 9건 `@authored_by` 9건. |
| T-10 | 의미 없는 테스트가 없고 Stub/Mock 의존이 최소인가 | Pass | Mock 없음. |
| T-11 | 통합 전략과 순서가 기록되어 있는가 | Pass | R1-24 종결. `tests/integration/test_integration_core.cpp` L1–6: 하향식 단계 1(벡터 V 재생, VAL→CMD→OVR→ARB→STA→CORE), 단계 2(LOG·SER·틱/STALE/PENDING 연동), 상향식을 두지 않은 근거. |
| **7. 시스템 테스트 (SWE.6)** | | | |
| S-1 | 모든 케이스가 존재하는 요구사항 ID를 참조하는가 | Pass | `system_tests.json` `basis_ids` 전수 요구사항 목록에 존재. |
| S-2 | 요구사항 커버리지 100%: 모든 요구사항이 [제안]이 아닌 케이스로 1개 이상 덮이는가 | Pass | `ci/check_system_coverage.py`: 제외는 open_issues 4건(SRQ-FUN-024/025, SRQ-CON-008/009)뿐이고 사유가 모두 "이터레이션 3"(DEC-012). 게이트 로직 확인. TC-F-017/018은 명세에서 제거되어 미실행 케이스가 커버리지에 계산되지 않음(R1-11 종결). |
| S-3 | 미결 사항(테스트 불가 요구사항)이 0건인가 | Pass | open_issues 4건은 DEC-012로 승인된 이연이며 사유가 기록됨. |
| S-4 | 입력이 있는 요구사항에 동치 분할과 경계값 분석이 적용되었는가 | Pass | BVA 적용 20행, 나머지 66행은 "boolean·열거형이라 수치 경계 없음" 사유 기재(`system_tests.json` design). |
| S-5 | 다중 파라미터 조합에 PICT가 적용되었는가 | Fail | PICT 모델 1건(SSR-027, 결과 14행). 3파라미터 조합 TC-F-001/002는 24조합 전수 열거로 대체(사유 기재, 수용). 그러나 83건의 N/A 사유가 동일 문구("독립 파라미터 1~2개이거나 단일 조건")로 일괄 기재되어 요구사항별 검증이 불가능하다. → R1-12 잔여. |
| S-6 | 엣지/WoW 케이스 검토, 안전 메커니즘에 고장 주입 케이스 | Pass | TC-F-019(WoW), TC-F-012(sensor_fault), TC-F-010-01~17(무효 주입). |
| S-7 | 각 케이스에 ID, 베이스, 목적, 입력(값·단위), 절차, 예상 출력, 기법이 있는가 | Pass | 입력 값에 단위 표기(속도 `km/h` 13건 전부, 시각 `s`). R1-19 종결. |
| S-8 | 입력은 구체 값이며 기대값은 요구사항 원문에서 도출되었고 판정 기준이 있는가 | Fail | 명세 기대값이 SSR-008과 충돌: TC-F-008 S3(2.00→12.00 s, NORMAL 기대), TC-F-009 S6(1.04→5.00 s, NORMAL 기대), TC-F-011 S5(1.03→2.00 s, NORMAL 기대). 명세 입력만으로는 SSR-008에 따라 STALE/DEGRADED가 된다. → R1-04. |
| S-9 | 정의된 테스트 환경에서 실제로 만들 수 있는 입력인가 | Fail | 환경은 호출 간격 0.010 s(SRQ-LIM-002, OI-003)다. 명세 단계는 이 간격을 벗어난다(위 S-8). 실행기 `tests/system/test_system.cpp` L102–111은 시간 공백을 0.010 s filler 스냅샷으로 채우는데, 이 절차가 `system_tests.json`·`tools/gen_system_tests.py`·xlsx 어디에도 기록되어 있지 않다. 명세대로 수행한 시험과 실행 시험이 다르다. TC-F-011의 S6(tick 2.20)·S7(tick 2.21)은 2.00→2.20 사이 filler가 없다(tick 단계는 채움 대상 아님). → R1-04. |
| S-10 | 중복·무의미 케이스가 없고 무효 클래스는 케이스당 하나인가 | Pass | R1-20 종결. TC-F-010-01~17 단일 무효 클래스, TC-F-004-01~09 분리. |
| S-11 | [제안] 케이스가 커버리지 계산에서 제외되었는가 | Pass | `proposed=false` 전수, [제안] 0건. |
| S-12 | 요구사항 ↔ 케이스 양방향 추적이 프로젝트 기준 위치(`docs/traceability.md`)와 일치하는가 | Pass | `traceability.md` §2 TC ID와 `system_tests.json` 일치. 실행기의 non-I TC 57건 전부 `system_cases.inc` 또는 `test_system_special.cpp`에서 참조됨. |
| S-13 | 실행 결과(합격/불합격)가 기록되고 불합격이 없는가 | Pass | `build/reports/system/test-results.xml` tests=9, failures=0. 단 케이스별 결과 열은 명세에 없다(단계 기대값은 실행기에서 검사). |
| **공통 확인 사항** | | | |
| C-1 | 양방향 추적성이 상·하위 산출물과 일치하는가 | Fail | 요구사항 ↔ 케이스 ID 매핑은 존재하나, 추적이 가리키는 동작이 틀린 항목이 남아 있다: SSR-001 STALE 항목(N-01), SSR-008 명세 단계(R1-04), SDD §5 존재하지 않는 함수(R1-16). |
| C-2 | 해당 스킬의 필수 규칙을 모두 만족하는가 | Fail | 구현 스킬: 주석 AI 태그(R1-13), TDD RED(R1-08). 시험 스킬: PICT N/A 사유(R1-12). |
| C-3 | 이전 리뷰 결함이 모두 종결되었는가 | Fail | 001의 28건 중 17건 종결, 11건 열림(아래 결함 목록). |

### 외부 확인 대기 항목 (DEC-003) 판정

| 가정 ID | 쟁점 | 사용자 승인 | 요구 본문과 모순 | 구현 상태 | 판정 |
|---|---|---|---|---|---|
| OI-003 | 평가주기 0.010 s, 호출 계약 | 있음 | 없음 | 명세 입력 위반(R1-04) | 가정 Closed. 위반은 R1-04 Open |
| OI-004 | source 기준 + 마지막 스냅샷 후 0.200 s STALE | 있음 | 없음 | 스냅샷 경로 구현(R1-01 종결). 틱 기준은 DEC-011(5) 결정 대기 | 가정 Closed. 틱 기준은 R1-03 Open |
| OI-013 | DEGRADED 중 직전 출력 유지, 접근위험 도어 LOCK | 있음 | 없음 | STALE 접근위험 LOCK 구현(R1-02 종결). CONFIRMED STALE 처리는 DEC-011(1) 미구현(N-01) | 가정 Closed(본문 일치). 구현은 N-01 Open |
| OI-005, OI-006, OI-008, OI-010, OI-011, OI-012, OI-016, OI-021, OI-022 | 기존 승인 가정 | 있음 | 없음 | 일치 | Closed |

## 결함 목록

### 이전 결함(001) 종결 판정

| ID | 심각도(001) | 판정 | 종결 근거 (수정 파일 확인) |
|---|---|---|---|
| R1-01 | Major | Closed | `src/ecl_val.c` L156 스냅샷 경로 `(timestamp_s − last_rx_s) > limit`. `tests/unit/test_unit_stale_reentry.cpp` `SnapshotGapAbovePointTwoHundredIsStale`(1.2 fresh, 1.41 STALE). |
| R1-02 | Major | Closed | `src/ecl_val.c` `approach_effective`(L193–202)는 STALE 입력 값 유지. `src/ecl_arb.c` `apply_input_rules` L167–169가 접근위험을 최상위 LOCK. `StaleUnit.StaleKeepsApproachRiskTrueOnly`, `CoreLocksApproachDoorWhileStale`. SDD §6.1 정정. |
| R1-03 | Major | **Open** | 틱 STALE 기준이 여전히 "보존된 수신 시점 지연 + 마지막 수신 후 경과"이다(`src/ecl_val.c` L154–157, L385–386). R1-03 요청은 "마지막 수신 후 경과만"이었다. DEC-011(요구사항 트랙) (5)는 이 차이를 "사용자 결정 대기"로 남겼다(`docs/decisions.md` L16). 결정 전에는 종결할 수 없다. 조치: 사용자 결정(DEC-011(5)) 확보 후 (가) 틱 경로를 `now − last_rx_s`만으로 판정하도록 코드·SDD §3.1·§6.2 정정, 또는 (나) 보존 지연과의 합집합을 SSR-008에 명문화하고 SDD §6.2 정정. 어느 쪽이든 경계 시험 1.16/1.21과 "수신 시 STALE 후 틱" 시험을 추가. |
| R1-04 | Major | **Open** | 단위·TDD·IT의 PENDING 계열 시험은 0.010 s 격자로 재작성되어 해소. 그러나 명세(`tools/gen_system_tests.py` L198–210, L212–222, L264–272; `system_tests.json` TC-F-008·009·011)는 SSR-008 위반 간격을 그대로 두었고, 실행기가 이를 filler로 채운다(`tests/system/test_system.cpp` L102–111, 미문서화). `CoreIntegration.TickStaleThenRecover`(L284–299)는 1.00→tick 1.21 호출 간격 위반. |
| R1-05 | Major | Closed | 17개 함수 모두 의미 있는 반환(I-3 근거). 잔여 libc `(void)`는 N-02. |
| R1-06 | Major | **Open** (아래 R1-06 재판정 참조) | 설계-코드 불일치는 SDD §3.1 L41·L47–48에서 정정. 틱 PENDING 첫 판정 허용 명시만 누락. |
| R1-07 | Major | Closed | TDD 28 TEST에 `@authored_by` 28건(`test_tdd_*.cpp`). |
| R1-08 | Major | **Open** | 기능별 RED 로그 없음. 링크 실패 1회만 기록, 어서션 실패 0건(`docs/reviews/tdd-red-evidence-001.md`). |
| R1-09 | Major | Closed | `StaleUnit.ApproachResolveDeletesFirstSuppressionTime`(`test_unit_stale_reentry.cpp` L146–181): 1.0 억제 → 2.0 FALSE → 3.0 재진입에서 ack=0. 삭제 로직 `src/ecl_ovr.c` L67–69. |
| R1-10 | Major | Closed | SRQ-CON-001~007은 TC-I-001~005·006·007로 INS 검사(`inspection.sh`, `inspect.txt` 8 PASS). SRQ-CON-008/009는 DEC-012 이연. 제외 로직은 "이터레이션 3" 사유 검사를 포함. |
| R1-11 | Major | Closed | TC-F-017/018 명세 제거. 제외 4건은 DEC-012 승인 이연. |
| R1-12 | Major | **Open** | BVA N/A 66건은 사유 기재, PICT 3파라미터 3건은 사유 정정(TC-F-001/002 전수 열거). 그러나 83건 N/A가 동일 문구로 일괄 기재되어 요구사항별 확인 불가. → N/A 사유에 파라미터 수와 이름을 기재해야 한다. |
| R1-13 | Major(상향) | **Open** | `src/ecl_val.c`, `ecl_ser.c`, `ecl_ovr.c`, `ecl_log.c`에 `@@HITL@@` 플레이스홀더뿐이며 AI 작성 태그 전무. 001 시점보다 범위가 넓어졌다. |
| R1-14 | Minor | **Open** | 잔여 `src/ecl_ser.c` L55(`1U`), L59(`2U`), L126(`1U`). |
| R1-15 | Minor | **Open** | `tests/system/test_system.cpp` L62 `expect_status` void; `tests/common/sha256.h` L22, L31, L75 `reset`·`update`·`block` void; `tests/unit/test_unit_arb.cpp` `Ctx::driver`(L17 부근)는 항상 0을 반환; `tests/common/ecl_test_util.h` L37 `ecl_set_command`는 항상 `RET_OK`를 반환. |
| R1-16 | Minor | **Open** | SDD 잔여 불일치: §5 L131·132·137·141(존재하지 않는 함수명), §3.8 L114(3인자 호출 vs 코드 3인자 `ecl_core.c` L199 — 설계는 4인자), §3.1 L48(`apply_substitution`, `ecl_ret_t`), §3.6(`store_event` vs 코드 `claim_slot`), §3.7(`ser_writer_t`는 코드에 없음). SAD 필드명 정정(a, d)은 종결. |
| R1-17 | Minor | Closed | `ValUnit`/`ArbUnit.NullArgumentsReturnErrNull`의 `@tracing`이 SRQ-QUA-REL-001로 정정됨(`test_unit_val.cpp` L29, `test_unit_arb.cpp` L14 주석). |
| R1-18 | Minor | **Open** | (a) `tests/tdd/test_tdd_val_ser_core.cpp` L211 `strncmp(..., 66)` — 비교 문자열 `lock_left=LOCK;lock_right=RELEASE;state=NORMAL;reason_code=RC_NONE;`는 67자. (b) `test_unit_val.cpp` L355–357 비트 `&`·합 단언 미수정. (c) `test_unit_ser_core.cpp` L479 죽은 대입과 주석 불일치 미수정. (d) `tests/system/test_system_special.cpp` L403 `reinterpret_cast` 미수정. (e) `test_system.cpp` L139–143 `CaseTableIsComplete`는 개수만 확인. |
| R1-19 | Minor | Closed | 속도 입력 13건에 `km/h` 표기, 시각에 `s` 표기(`system_tests.json` inputs). |
| R1-20 | Minor | Closed | TC-F-010-01~17, TC-F-004-01~09로 분리. |
| R1-21 | Minor | Closed | `SystemSpecial` `Sha256MatchesFipsKnownAnswers`(`test_system_special.cpp` L419). |
| R1-22 | Minor | Closed | SDD §6.5 L153에 도달 불가 근거 기록(내부 호출은 NULL이 아닌 인자만 받음). Branches 9 미실행은 근거 있는 잔여로 수용. |
| R1-23 | Minor | Closed | 헤더 주석 "관례"로 정정, 시험 접근자는 `ECL_TEST_BUILD` 가드(`ecl_core.h` L105, `ecl_log.h` L67), 제품 빌드 접근자 0건(TC-I-007). |
| R1-24 | Minor | Closed | `test_integration_core.cpp` L1–6에 통합 순서와 근거. |
| R1-25 | Minor | Closed | SDD §6.7 L155, `ecl_ovr.c` L14 주석에 중복 허용 근거. |
| R1-26 | Minor | Closed | DEC-012 승인 이연. `architecture.md` L187, L251. |
| R1-27 | Minor | Closed | `build/reports/inspect/warnings.txt` 0행, `inspect.txt` PASS. |
| R1-28 | Minor | **Open** | 요구 근거(DEC-011)와 테스트·코드는 정합(`cmd` present=2 → `present=1, rejected=1`, `test_unit_cmd_sta.cpp` L147–157). 그러나 SDD §3.2 L59 "`present`=raw->present(0/1만 인정, 그 외 0)"는 §6.4 L152 및 코드와 충돌. |

#### R1-06 재판정
R1-06의 설계-코드 불일치(`last_inputs` vs `last_snap`, 틱 재조건화)는 SDD §3.1 L41·L47–48에서 정정되어 종결됐다. 단, 요구된 명시 사항 "틱에서 PENDING 타임아웃 첫 판정을 허용할지"는 SDD 어디에도 명시되지 않았다(`track_pending`은 `build_inputs`를 통해 틱에서도 실행됨: `src/ecl_val.c` L324, L385–387). 따라서 **Open**.

### 신규 결함 (iteration-1-review-002에서 발견)

| ID | 심각도 | 위치 | 설명 | 조치 요청 | 상태 |
|---|---|---|---|---|---|
| N-01 | Major | `src/ecl_val.c` `evaluate_staleness` L159–165, `compute_effective` L240–241; `tests/unit/test_unit_val.cpp` `ValUnit.StaleInputsAreNotReflected` L373–385; `docs/SRS.md` L33; `docs/safety-requirements.md` SSR-001 L17, SSR-029 L37; `docs/detailed-design.md` §3.1 L51, §6.1 L149; `docs/decisions.md` DEC-011(요구사항 트랙) (1)·(4) | **SSR-001 위반.** STALE 판정 시 `evaluate_staleness`가 CRASH 신호를 포함해 VALID를 STALE로 바꾸고, `crash_confirmed`는 `validity==VALID`일 때만 1이 된다. 따라서 STALE 평가의 CONFIRMED는 P1 RELEASE가 적용되지 않는다. SSR-001과 SRS L33은 "같은 평가가 STALE이어도 두 출력 RELEASE"를 명시한다. 시험 `StaleInputsAreNotReflected`는 이 위반값(`crash_confirmed=0`)을 기대값으로 고정한다. DEC-011(1)은 인정, (4)는 "구현 반영 필요(미완)"으로 기록되어 있다. 부수 문제: 같은 STALE에서 PENDING은 `track_pending`이 VALID만 보므로 중단되고 이유코드가 `RC_INPUT_STALE`로 바뀐다(SSR-027 P3 > P4 순위와 불일치 소지). | 효과 값에서 CRASH는 INVALID가 아닌 한 원값으로 `crash_confirmed`를 판정(STALE 보고 표시는 유지). SDD §3.1 L51·§6.1 L149를 SRS와 일치시킴. `StaleInputsAreNotReflected`의 `crash_confirmed` 기대값을 SSR-001 원문에서 재도출(CONFIRMED STALE → RELEASE). STALE+CONFIRMED의 UT·IT·ST 추가. STALE 중 PENDING 이유코드 우선순위를 SRS에 명시하거나 설계 결정 기록. | Open |
| N-02 | Minor | `src/ecl_core.c` L37, L59, L95, L102, L127–129; `src/ecl_val.c` L311, L315–316, L343, L389; `src/ecl_ser.c` L53, L55, L93–94; `src/ecl_ovr.c` L95, L115; `src/ecl_log.c` L44; `src/ecl_cmd.c` L56 | 프로덕션 코드에서 libc 반환값을 `(void)`로 폐기한다(`memset`, `memcpy`, `strcpy`, `strcat`). 리턴 규칙(I-4)과 사용자 확인 항목("(void) 폐기 제거")에 어긋난다. 또한 `ecl_ser.c` L93–94 `strcpy`/`strcat`은 길이 검사 없이 고정 버퍼(`ECL_SER_KEY_MAX`=40)에 쓴다. 현재 길이(최대 23자)로는 넘치지 않으나 상수 검증이 없다. | `(void)` 캐스트를 제거하고 반환값을 사용하거나 반환값 없는 방식으로 재작성. `strcpy`/`strcat`은 `snprintf` 또는 길이 정적 검증(`_Static_assert`)으로 대체. | Open |
| N-03 | Minor | `src/ecl_val.c` `ecl_val_condition_tick` L388–396 | 첫 스냅샷 수신 전 틱에서 `timestamp_s > 0.200`이면 13개 신호 전체를 STALE로 보고한다. `docs/SRS.md` L33은 STALE을 "그 평가에서 유효였던 모든 신호가 STALE"로 정의하며, 수신 전에는 유효였던 신호가 없다. 보고 규칙과 불일치한다. | SRS 정의를 따를지(validity를 INVALID 또는 보고 제외) 사용자 결정 후 코드와 SDD §3.1 정정. 시험 추가. | Open |
| N-04 | Minor | `docs/decisions.md` L16, L46 | 결정 ID `DEC-011`이 두 건 존재한다(요구사항 6차 재리뷰 후속 결정 / 이터레이션 `present` 거절). 추적에서 모호하다. | 요구사항 트랙 결정 ID를 재부여하고 참조(SRS 개정 이력, 리뷰 기록)를 갱신. | Open |


## 열린 결함 요약
- 열린 결함: **15건** — Major 7: R1-03, R1-04, R1-06, R1-08, R1-12, R1-13(상향), N-01 / Minor 8: R1-14, R1-15, R1-16, R1-18, R1-28, N-02, N-03, N-04.
- 종결: R1-01, R1-02, R1-05, R1-07, R1-09, R1-10, R1-11, R1-17, R1-19, R1-20, R1-21, R1-22, R1-23, R1-24, R1-25, R1-26, R1-27 (17건).
- **다음 단계 진행: 불가 (열린 결함 15건)**. Approved 조건(모든 체크 Pass 또는 정당한 N/A, 모든 결함 종결) 미충족.

## 재리뷰 조치 우선순위
1. N-01: DEC-011(1)·(4) 구현 반영. `src/ecl_val.c` CRASH 효과 값과 `StaleInputsAreNotReflected` 기대값 수정, STALE+CONFIRMED 시험 추가. SSR-001 원문과 대조.
2. R1-04: 명세(`tools/gen_system_tests.py`, `system_tests.json`, xlsx)의 SSR-008 위반 기대값 정정. 0.010 s 격자 채움(틱 구간 포함)을 명세 절차에 기록. `CoreIntegration.TickStaleThenRecover` 호출 간격 수정.
3. R1-03: 사용자 결정(DEC-011(5)) 확보 후 틱 STALE 기준 확정.
4. R1-13: `src/ecl_val.c`, `ecl_ser.c`, `ecl_ovr.c`, `ecl_log.c`의 `@@HITL@@`를 6종 AI 작성 태그 블록으로 교체.
5. R1-08: 기능별 RED를 어서션 실패 로그(실행 시각, 출력 발췌)로 남김.
6. R1-12: 83건 N/A 사유에 요구사항별 파라미터 수와 이름 기재.
7. 나머지 Minor: R1-06, R1-14, R1-15, R1-16, R1-18, R1-28, N-02, N-03, N-04.
