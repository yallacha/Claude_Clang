# 소프트웨어 요구사항 명세 (SRS) — 전자식 차일드락 제어 SW

| 항목 | 값 |
|---|---|
| 문서 ID / 버전 | SRS-ECL-001 / 0.3 (Draft, 재리뷰 대기) |
| 베이스라인 | BL-SWR-1.0 — 이터레이션 게이트에서 사용자가 승인할 때 부여(`docs/decisions.md` DEC-006) |
| 입력(상위) 문서 | `OEM/OEM-SWR-001_OEM SW 요구사항 사양서.docx` (Rev 1.0, BL-OEM-1.0) — 저장소에는 포함하지 않는다(DEC-005) |
| 프로세스 | A-SPICE SWE.1 (`requirements-analysis` 스킬) |
| 안전 요구사항 | `docs/safety-requirements.md` (SSR-xxx). 본 문서는 ID 참조만 한다 |
| 미결 사항·가정 | `docs/open-issues.md` (OI-xxx, 사용자 승인 가정 DEC-003) |
| 결정 기록 / Use Case / 추적성 / 시험 벡터 | `docs/decisions.md` / `docs/use-cases.md` / `docs/traceability.md` / `docs/test-vectors/vector-v.csv` |

**ID 체계**: 일반 SW 요구사항은 `SRQ-…`, 안전 요구사항은 `SSR-…`이다(OEM 입력 `OEM-…`, OEM 계획 ID `SWR-…`). v0.1의 `SWRS/SWSR` ID는 v0.2에서 번호를 다시 부여했다(사용자 승인 예외 DEC-004, 대응표는 `docs/decisions.md`). **v0.2 이후 ID는 불변**이며 분리·추가는 새 번호로 한다(번호가 순서대로가 아닐 수 있다).

**문서 지위**: 교육용 가상 OEM 입력에 기반하며 실제 OEM 승인, A-SPICE 능력수준, ISO 26262 준수·적합성·인증·ASIL 달성을 주장하지 않는다.

**범위**: 후석 좌·우 차일드락의 PC/SIL 논리 제어까지이다. 도어 래치·구동기 HW, ECU, HIL, 실차, HARA·ASIL 결정 근거의 재구성(OEM이 ASIL B를 입력으로 할당), 공식 심사·인증은 범위 밖이다(OEM §1.3, §3). 개발 환경 전제(Clang 18 이상, Docker 이미지 `claudeclangbuild`, `CLAUDE.md` 2장 품질 기준)는 `CLAUDE.md`에서 관리하는 프로세스 조건이며 제품 요구사항이 아니다.

**표기**: 우선순위 H/M/L, 상태 `Draft`는 `우선/상태` 열. 검증 방법: UT(단위 테스트), IT(통합 테스트), ST(시스템 테스트), INS(검사/검토), BLD(빌드). OEM 검증 수준 "PC/SIL/Web"은 ST에 해당하며 "Web"은 표시 계층(`OI-017`)이다. 근거 열의 `OI-xxx`는 사용자 승인 가정을 가리킨다.

**단위**: 시각과 기간은 초(s)로 쓴다(OEM 원문의 300 ms = 0.300 s, 200 ms = 0.200 s, 100 ms = 0.100 s, 10 ms = 0.010 s). 속도는 km/h.

## 0. 용어집

| 용어 | 정의 |
|---|---|
| 평가 호출 | 입력으로 `timestamp_s`(호출 시각, 호출자 주입)를 받고 선택적으로 스냅샷을 동반하는 동기 호출. 스냅샷 없이 `timestamp_s`만 가진 호출(tick)을 허용한다. |
| 스냅샷 | OEM-IF-001~003, 007~009 신호와 `source_timestamp_s`, 선택적 운전자 명령(IF-004)을 가진 입력 묶음. |
| 평가주기 / 평가 | 0.010 s(`OI-003`). 평가 = 평가 호출 1회이며 같은 호출의 반환 시점에 출력·상태·이벤트를 확정한다. OEM의 "첫/다음 평가주기"는 입력을 처음 처리하는 평가이다. |
| 필수 입력 | IF-001(vehicle_speed_kph, gear, source_timestamp_s), IF-002(crash_status), IF-003(두 접근위험), IF-007(fire, overtemperature, adult), IF-008(두 ISOFIX), IF-009(ignition_on, sensor_fault)의 모든 신호. |
| 유효 / INVALID / STALE | 유효 = 형식·범위 검사(SSR-005) 통과 + fresh. INVALID = 형식·범위 위반 또는 누락. STALE = freshness 기준 초과(SSR-007, 008). |
| 직전 확정 출력 | 직전 평가가 확정한 lock_left, lock_right. 첫 평가 전에는 RELEASE(SRQ-FUN-026). |
| 정지 / 이동 | 정지 = 유효한 vehicle_speed_kph < 3.0. 이동 = 유효한 vehicle_speed_kph ≥ 3.0. (속도 INVALID는 SSR-023에 따라 이동으로 처리) |
| 정상 입력 | 필수 입력이 모두 유효이고 ignition_on=TRUE, crash_status=NONE, sensor_fault=FALSE, 접근위험·ISOFIX·fire·overtemperature·adult가 모두 FALSE인 입력(속도와 운전자 명령은 시험이 지정). |
| input_validity 값 | 입력별 `VALID`, `INVALID`, `STALE` 중 하나. |
| override 상태 | 도어별 boolean 상태 필드 `override`(state 열거와 별개). |
| 유효한 명령 | SRQ-FUN-007에서 거절되지 않은 운전자 명령. |
| 적용 | 우선순위 표(SSR-027)에서 해당 규칙이 도어의 출력으로 선택됨. |
| 같은 해제 명령 | 접근위험으로 해제가 억제된 도어를 side가 포함하고 action=unlock인 명령. source는 첫 억제 때와 같아야 한다(`OI-011`). |
| 첫 억제 시각 | 도어별로 접근위험 TRUE 구간에서 해제가 처음 억제된 평가의 `timestamp_s`. 접근위험이 유효한 FALSE가 되면 삭제. |
| PENDING 지속 | crash_status가 PENDING으로 처음 유효하게 입력된 평가의 `timestamp_s`부터 PENDING이 계속되는 기간. |
| 공개 함수 | `include/` 헤더에 선언된 모든 함수. |
| 이유코드(reason_code) | 부록 A의 열거값. OEM-IF-006의 `priority_reason`은 `reason_code`와 같은 값. |
| 경고코드 | FAULT 상태에서 제공하는 이유코드 `RC_SENSOR_FAULT`. |
| 최악 평가 시간 | ST 시험 벡터 전체에서 평가 호출 1회에 걸린 시간의 최댓값(벽시계, PC 환경). |

## 1. 기능 요구사항

수용 기준은 시험 판정이 가능한 구체 값으로 쓴다. 모든 출력 결정은 `SSR-027`(우선순위 표)을 따른다. "정상 입력"에서 시험하는 기준은 상위 규칙이 성립하지 않는 조건이다.

### 1.1 운전자 명령 (OEM-FR-001, OEM-IF-004)

| ID | 요구사항 | 수용 기준 | 근거 | 우선/상태 | 검증 | 상위(OEM) |
|---|---|---|---|---|---|---|
| SRQ-FUN-001 | SW는 side(left, right, all), action(lock, unlock), source(physical_button, avn, voice, mobile_app)로 구성된 운전자 명령을 수신해야 한다. | 4 source × 3 side × 2 action = 24개 조합이 모두 수신 처리된다. | OEM-FR-001, `OI-014` | M/Draft | UT, ST | OEM-FR-001 |
| SRQ-FUN-002 | SW는 lock 명령이 적용되면 선택된 side의 출력을 LOCK으로 해야 한다. | 정상 입력·정지에서 left 선택 시 lock_left=LOCK, all 선택 시 둘 다 LOCK. | OEM-FR-001, SSR-027 | M/Draft | UT, ST | OEM-FR-001 |
| SRQ-FUN-003 | SW는 정지 상태에서 unlock 명령이 적용되면 선택된 side의 출력을 RELEASE로 해야 한다. | 정상 입력·정지에서 source 4종 각각의 unlock이 선택 side 출력만 RELEASE로 만든다. | OEM-FR-001, SSR-027, `OI-010`, `OI-012` | M/Draft | UT, ST | OEM-FR-001 |
| SRQ-FUN-004 | SW는 이동 중(속도 3.0 이상 또는 속도 INVALID)에는 unlock 명령으로 출력을 RELEASE로 바꾸지 않아야 한다. | 직전 출력 RELEASE, 정상 입력, 속도 3.0에서 unlock 명령을 단독 주입한 평가의 출력이 LOCK(P8). 같은 조건에서 속도 2.9이면 RELEASE. | 파생(OEM-FR-002와 일관), SSR-027, SSR-023, `OI-010`, `OI-012` | M/Draft | UT, ST | OEM-FR-001, OEM-FR-002 |
| SRQ-FUN-005 | SW는 이동 중 유효한 unlock 명령을 받은 평가에서 우선순위 P8이 선택되면 이유코드를 `RC_UNLOCK_DENIED_MOVING`으로 기록해야 한다. | 정상 입력·속도 3.0·unlock 명령 단독 주입 평가의 reason_code=`RC_UNLOCK_DENIED_MOVING`. | 파생, SSR-027, `OI-010` | L/Draft | UT | OEM-FR-001 |
| SRQ-FUN-006 | SW는 운전자 명령이 선택하지 않은 side의 출력을 그 명령으로 변경하지 않아야 한다. | 직전 lock_left=lock_right=RELEASE, 정상 입력, 정지에서 left lock 명령을 단독 주입한 평가의 lock_right=RELEASE. | OEM-FR-001 | M/Draft | UT, ST | OEM-FR-001 |
| SRQ-FUN-007 | SW는 side, action, source 중 하나라도 누락되었거나 형식이 틀렸거나 미등록 값인 명령을 거절해야 한다. | 부록 B-1의 거절 벡터 전체가 거절된다. | OEM-IF-004 | M/Draft | UT | OEM-FR-001 |
| SRQ-FUN-031 | SW는 거절한 명령으로 출력을 변경하지 않아야 한다. | 거절 평가의 출력이 명령 없이 평가한 결과와 같다. | OEM-IF-004 | M/Draft | UT | OEM-FR-001 |
| SRQ-FUN-008 | SW는 명령을 거절한 평가에서 command_rejected를 TRUE로 해야 한다. | 거절 평가의 상태 조회 command_rejected=TRUE, 그 외 평가는 FALSE. | OEM-IF-004, 파생 | L/Draft | UT | OEM-FR-001 |
| SRQ-FUN-038 | SW는 명령을 거절한 평가에서 우선순위 P1~P9가 이유코드를 선택하지 않았으면 reason_code를 `RC_COMMAND_REJECTED`로 기록해야 한다. | 정상 입력에서 거절 평가의 reason_code=`RC_COMMAND_REJECTED`. 상위 규칙이 있으면 그 코드이다. | 파생, SSR-027 | L/Draft | UT | OEM-FR-001 |

### 1.2 자동 잠금, 강제 해제, ISOFIX, 시동 OFF

| ID | 요구사항 | 수용 기준 | 근거 | 우선/상태 | 검증 | 상위(OEM) |
|---|---|---|---|---|---|---|
| SRQ-FUN-009 | SW는 유효한 vehicle_speed_kph가 3.0 이상인 평가에서 자동 잠금이 적용되면 좌, 우 출력을 LOCK으로 해야 한다. | 속도 2.9 → 3.0으로 바뀐 평가에서 두 출력이 LOCK이다(3.0 포함). | OEM-FR-002, SSR-027, `OI-006` | M/Draft | UT, ST | OEM-FR-002 |
| SRQ-FUN-010 | SW는 fire_detected, overtemperature_detected, adult_present 중 하나가 유효한 TRUE이면 강제 해제가 적용될 때 접근위험으로 LOCK이 선택되지 않은 도어의 출력을 RELEASE로 해야 한다. | 정상 입력(접근위험 없음)에서 세 입력 각각을 단독 주입한 평가에서 두 출력이 RELEASE. 접근위험이 있는 도어는 LOCK 유지(SSR-027 P5 > P6). | OEM-FR-005, SSR-027, `OI-006` | M/Draft | UT, ST | OEM-FR-005 |
| SRQ-FUN-011 | SW는 강제 해제 시 이유코드를 입력별로 `RC_FIRE`, `RC_OVERTEMP`, `RC_ADULT_PRESENT`로 기록해야 한다. | 단독 주입 시 해당 코드가 reason_code. | OEM-FR-005 | M/Draft | UT, ST | OEM-FR-005 |
| SRQ-FUN-032 | SW는 fire_detected, overtemperature_detected, adult_present 중 둘 이상이 동시에 TRUE이면 fire, overtemperature, adult 순서로 하나의 이유코드를 기록해야 한다. | fire+adult 동시 주입 시 `RC_FIRE`. | 파생, `OI-016` | L/Draft | UT | OEM-FR-005 |
| SRQ-FUN-012 | SW는 isofix_left 또는 isofix_right가 유효한 TRUE이면 ISOFIX 잠금이 적용될 때 TRUE인 쪽 출력을 LOCK으로 해야 한다(둘 다 TRUE이면 둘 다 LOCK). | 정상 입력에서 left만 TRUE이면 lock_left=LOCK, 둘 다 TRUE이면 둘 다 LOCK. | OEM-FR-006, SSR-027, SSR-021, `OI-006` | M/Draft | UT, ST | OEM-FR-006 |
| SRQ-FUN-013 | SW는 FALSE인 쪽 출력을 ISOFIX 규칙으로 변경하지 않아야 한다. | 직전 lock_left=lock_right=RELEASE, 정상 입력에서 isofix_left만 단독 TRUE 주입한 평가의 lock_right=RELEASE. | OEM-FR-006 | M/Draft | UT, ST | OEM-FR-006 |
| SRQ-FUN-014 | SW는 ignition_on이 유효한 FALSE이면 시동 OFF 규칙이 적용될 때 좌, 우 출력을 RELEASE로 해야 한다. | P1~P8이 성립하지 않는 입력에서 첫 평가에 두 출력이 RELEASE. | OEM-FR-007, SSR-027, `OI-006` | M/Draft | UT, ST | OEM-FR-007 |
| SRQ-FUN-015 | SW는 시동 OFF 규칙이 성립할 때 state를 OFF로 해야 한다(상태 결정은 SSR-028, SSR-030). | 다른 state 조건이 없는 입력에서 첫 평가에 state=OFF. | OEM-FR-007, SSR-028, SSR-030, `OI-005` | M/Draft | UT, ST | OEM-FR-007 |
| SRQ-FUN-016 | SW는 시동 OFF 규칙이 적용된 평가에서 이유코드를 `RC_IGNITION_OFF`로 기록해야 한다. | P1~P8이 성립하지 않는 입력에서 reason_code=`RC_IGNITION_OFF`. | OEM-FR-007, SSR-027 | M/Draft | UT, ST | OEM-FR-007 |

### 1.3 override (OEM-FR-003)

| ID | 요구사항 | 수용 기준 | 근거 | 우선/상태 | 검증 | 상위(OEM) |
|---|---|---|---|---|---|---|
| SRQ-FUN-017 | SW는 접근위험으로 해제가 억제된 도어에 같은 해제 명령이 첫 억제 시각부터 10.0 s 이하(경계 포함)에 재입력되면 override로 판정해야 한다. | 경과 10.0 s 입력은 override, 10.01 s 입력은 override가 아니다. | OEM-FR-003, SSR-015, `OI-011`, `OI-015` | M/Draft | UT, ST | OEM-FR-003 |
| SRQ-FUN-018 | SW는 override로 판정되어도 접근위험 TRUE인 도어의 출력을 LOCK으로 유지해야 한다. | override 판정 평가에서 해당 도어 출력이 LOCK이다. | SSR-012와 일관, `OI-011` (OEM은 효과 미정의) | H/Draft | UT, ST | OEM-FR-003, OEM-SR-002 |
| SRQ-FUN-019 | SW는 override 판정 시 해당 도어의 override 상태를 TRUE로 해야 한다. | override 판정 평가에서 상태 조회의 도어별 override=TRUE. | OEM-FR-003, `OI-011` | M/Draft | UT, ST | OEM-FR-003 |
| SRQ-FUN-033 | SW는 override 판정 시 이유코드를 `RC_OVERRIDE_ACK`로 기록해야 한다. | override 판정 평가의 reason_code=`RC_OVERRIDE_ACK`. | OEM-FR-003 | M/Draft | UT, ST | OEM-FR-003 |
| SRQ-FUN-034 | SW는 도어의 override 상태를 접근위험이 유효한 FALSE가 되었거나 첫 억제 시각부터 10.0 s를 초과한 평가에서 FALSE로 되돌려야 한다. | override=TRUE 후 접근위험 FALSE 평가에서 override=FALSE. 10.01 s 경과 평가에서도 FALSE. | 파생(상태 수명 정의), `OI-011` | M/Draft | UT | OEM-FR-003 |
| SRQ-FUN-020 | SW는 첫 억제 시각부터 10.0 s를 초과하여 재입력된 해제 명령을 일반 억제된 해제 요청으로 처리해야 한다. | 10.01 s 입력은 override가 아니고 해당 도어가 LOCK 유지. | OEM-FR-003, `OI-011` | L/Draft | UT | OEM-FR-003 |
| SRQ-FUN-035 | SW는 10.0 s 초과 후 억제된 해제 요청이 발생하면 첫 억제 시각을 그 평가의 `timestamp_s`로 갱신해야 한다. | 10.01 s 입력 평가 후 첫 억제 시각 = 그 평가의 timestamp_s. | 파생, `OI-011` | L/Draft | UT | OEM-FR-003 |

### 1.4 출력, 상태 조회, 표시 (OEM-FR-004, OEM-IF-005, OEM-IF-006)

| ID | 요구사항 | 수용 기준 | 근거 | 우선/상태 | 검증 | 상위(OEM) |
|---|---|---|---|---|---|---|
| SRQ-FUN-021 | SW는 매 평가마다 lock_left와 lock_right를 각각 LOCK 또는 RELEASE로 출력해야 한다. | 모든 평가 결과에서 두 출력이 두 값 중 하나. | OEM-IF-005 | H/Draft | UT, IT | OEM-FR-001~007 |
| SRQ-FUN-022 | SW는 상태 조회에 lock_left, lock_right, state, 입력별 input_validity(VALID, INVALID, STALE), reason_code(= priority_reason), 도어별 override, command_rejected를 포함해 응답해야 한다. | 응답에 모든 필드가 존재하고 값이 평가 결과와 같다. | OEM-FR-004, OEM-IF-006, `OI-005` | M/Draft | IT, ST | OEM-FR-004 |
| SRQ-FUN-023 | SW는 상태 응답 직렬화에 실패하면 오류코드 `RET_ERR_SERIALIZE`(-2)를 반환해야 한다. | 출력 버퍼 크기가 응답보다 작은 호출에서 -2가 반환된다. | OEM-IF-006, `OI-017` | L/Draft | UT, IT | OEM-FR-004 |
| SRQ-FUN-024 | 표시 계층(Web)은 코어가 `RET_ERR_SERIALIZE`를 반환하면 HTTP 500으로 응답해야 한다. | 직렬화 실패 주입 시 응답 상태 500. | OEM-IF-006, `OI-017` | L/Draft | ST(Web) | OEM-FR-004 |
| SRQ-FUN-025 | 표시 계층(Web)은 상태 응답의 출력, state, 입력 유효성, 이유코드를 사용자에게 표시해야 한다. | 시험 입력별로 표시값이 상태 응답과 일치. | OEM-FR-004, OEM §3 표시/조작, `OI-017` | M/Draft | ST(Web) | OEM-FR-004 |
| SRQ-FUN-026 | SW는 첫 평가 전의 초기 출력을 두 도어 모두 RELEASE로 해야 한다. | 초기화 직후 상태 조회가 RELEASE/RELEASE. | 파생(직전 확정 출력 정의) | M/Draft | UT | OEM-SR-004 |
| SRQ-FUN-036 | SW는 첫 평가 전의 초기 state를 OFF로 해야 한다. | 초기화 직후 상태 조회 state=OFF. | 파생, `OI-005` | M/Draft | UT | OEM-FR-007 |

### 1.5 이벤트 기록 (OEM-NFR-002)

| ID | 요구사항 | 수용 기준 | 근거 | 우선/상태 | 검증 | 상위(OEM) |
|---|---|---|---|---|---|---|
| SRQ-FUN-027 | SW는 평가 결과의 lock_left, lock_right, state, reason_code 중 하나라도 직전 평가와 다르거나 command_rejected가 TRUE인 평가마다 이벤트를 1건 기록해야 한다. | 직전 평가의 기준값은 첫 평가에서 초기값(RELEASE, RELEASE, OFF, RC_NONE)이다. 변화 없는 평가는 기록하지 않고 변화가 있는 평가는 정확히 1건 기록. | OEM-NFR-002, `OI-016`, `OI-021` | M/Draft | UT, IT | OEM-NFR-002 |
| SRQ-FUN-028 | SW는 이벤트 ID를 1부터 시작하여 기록마다 1씩 증가하는 64비트 부호 없는 정수로 부여해야 한다(최댓값 2^64−1 이후는 SRQ-FUN-037). | n번째 이벤트의 ID가 n이다. | OEM-NFR-002 | M/Draft | UT | OEM-NFR-002 |
| SRQ-FUN-037 | SW는 이벤트 ID가 2^64−1인 이벤트를 기록한 뒤에는 새 이벤트를 기록하지 않아야 한다(ID 유일성 유지). | 시험용 접근자(SRQ-FUN-039)로 카운터를 2^64−2로 설정한 뒤 이벤트 2건을 발생시키면 ID 2^64−1인 1건만 보존되고 두 번째는 기록되지 않는다. | 파생(경계 정의), SRQ-FUN-028, SRQ-FUN-030 | L/Draft | UT | OEM-NFR-002 |
| SRQ-FUN-039 | SW는 시험 빌드에서 이벤트 ID 카운터의 값을 설정하는 시험용 접근자를 제공해야 한다. | 시험 빌드에서 카운터 설정 후 다음 이벤트 ID가 설정값+1이다. 제품 빌드에는 접근자가 없다(INS). | 파생(SRQ-FUN-037 시험 가능성) | L/Draft | UT, INS | OEM-NFR-002 |
| SRQ-FUN-029 | SW는 이벤트에 event_id, timestamp_s, lock_left, lock_right, state, reason_code, command_rejected, 입력별 input_validity 필드만 포함해야 한다. | 이벤트 구조체의 필드가 이 목록과 정확히 같다. | OEM-NFR-002, `OI-016` | M/Draft | UT, INS | OEM-NFR-002 |
| SRQ-FUN-030 | SW는 이벤트 조회 시 보존된 최신 최대 100건을 event_id 오름차순으로 반환해야 한다. | 101건 기록 후 조회 결과가 ID 2~101의 100건이고 오름차순. | OEM-NFR-002 수용기준, `OI-021` | M/Draft | UT, IT, ST | OEM-NFR-002 |

## 2. 비기능 요구사항

### 2.1 품질 (ISO/IEC 25010)

| ID | 특성/부특성 | 요구사항 | 측정 기준 | 근거 | 우선/상태 | 검증 | 상위(OEM) |
|---|---|---|---|---|---|---|---|
| SRQ-QUA-FUN-001 | 기능 적합성/기능 정확성 | SW는 동일한 입력 순서와 초기 상태에 대해 동일한 공개 결과를 생성해야 한다. | `docs/test-vectors/vector-v.csv`의 시험 벡터 V를 `timestamp_s` 주입으로 1,000회 재생한다. 공개 결과 = 평가마다 상태 응답(SRQ-FUN-022)을 `필드=값;` ASCII로 직렬화하여 이어 붙인 바이트열, 해시 = SHA-256. 1,000개 해시가 모두 같다. | OEM-NFR-001, `OI-009` | H/Draft | ST | OEM-NFR-001 |
| SRQ-QUA-REL-001 | 신뢰성/성숙성(결함 허용성) | SW의 공개 함수는 NULL 포인터 인자를 받으면 오류코드 `RET_ERR_NULL`(-1)을 반환해야 한다. | 모든 공개 함수에 대해 NULL 인자 호출이 -1을 반환한다(함수별 시험 존재). | 파생(`implementation` 스킬 반환 규칙). OEM 상위 없음 | M/Draft | UT | (파생) |

### 2.2 제한 (Limitation)

| ID | 요구사항 | 기준 | 근거 | 우선/상태 | 검증 | 상위(OEM) |
|---|---|---|---|---|---|---|
| SRQ-LIM-001 | SW는 이벤트를 프로세스 메모리에만 보존해야 한다. | 프로세스 재기동 후 이벤트 조회 결과가 0건. | OEM-NFR-002 | M/Draft | UT, IT, ST | OEM-NFR-002 |
| SRQ-LIM-003 | SW는 이벤트가 100건을 넘으면 가장 오래된 1건을 삭제해야 한다. | 101번째 기록 후 보존 건수 100, 가장 오래된 ID가 2. | OEM-NFR-002 | M/Draft | UT, IT | OEM-NFR-002 |
| SRQ-LIM-002 | SW는 평가 호출이 0.010 s 간격으로 이루어진다는 호출 계약을 전제로 SSR의 시간 규칙을 평가해야 한다. | 모든 시험 벡터의 평가 호출 간격이 0.010 s이다. | 파생(OEM-SR-001 0.300 s, OEM-SR-003 0.100 s 충족), `OI-003` | H/Draft | INS, ST | OEM-SR-001, OEM-SR-003 |
| SRQ-LIM-004 | SW는 평가 호출 1회를 0.010 s 미만에 완료해야 한다. | ST 시험 벡터 전체에서 최악 평가 시간이 0.010 s 미만. | 파생, `OI-003` | H/Draft | ST | OEM-SR-001, OEM-SR-003 |

### 2.3 제약사항 (Constraint)

| ID | 요구사항 | 근거 | 우선/상태 | 검증 | 상위(OEM) |
|---|---|---|---|---|---|
| SRQ-CON-001 | 제품 코어는 C 언어(C11)로 `src/`에 구현해야 한다. **가정: OEM 편차 확인 대기** | 사용자 확정 요구사항 변경(`docs/decisions.md` CR-001), OEM §3 실행 환경(Python 3.12)에서 변경, `OI-002` | H/Draft | BLD, INS | OEM §3 (변경) |
| SRQ-CON-008 | 표시 계층 서버는 C 언어(C11)로 `src/`에 구현해야 한다. | CR-001, `OI-017` | M/Draft | BLD, INS | OEM §3 표시/조작 |
| SRQ-CON-009 | 표시 계층은 HTML을 C 코드로 생성하여 응답하고 클라이언트 측 스크립트(JavaScript)를 포함하지 않아야 한다. | OEM §3 표시/조작, `OI-017`, DEC-007 | M/Draft | INS, ST(Web) | OEM §3 표시/조작 |
| SRQ-CON-002 | SW는 시각을 호출자가 주입한 `timestamp_s`로만 얻어야 한다. | OEM-NFR-001, OEM-IF-004(요청 시각 = 스냅샷 timestamp_s), `OI-003` | H/Draft | INS, UT | OEM-NFR-001, OEM-IF-004 |
| SRQ-CON-005 | SW는 시스템 시계를 읽지 않아야 한다. | OEM-NFR-001(결정론) | H/Draft | INS | OEM-NFR-001 |
| SRQ-CON-003 | SW는 영상, 음성 원본과 개인 식별자를 입력으로 받지 않아야 한다. | OEM §2 프로젝트 제약 | M/Draft | INS, UT | OEM-NFR-002 |
| SRQ-CON-006 | SW는 영상, 음성 원본과 개인 식별자를 저장하지 않아야 한다. | OEM §2, OEM-NFR-002 | M/Draft | INS, UT | OEM-NFR-002 |
| SRQ-CON-007 | SW는 성인 존재를 boolean 1개 필드로만 표현해야 한다. | OEM §2 | M/Draft | INS, UT | OEM-NFR-002 |
| SRQ-CON-004 | SW는 도어 래치/구동기 HW를 직접 구동하지 않아야 한다. | OEM §3 제외/제한, OEM-IF-005 | M/Draft | INS | OEM-IF-005 |

## 3. 외부 인터페이스 매핑

| OEM IF | 방향 | 데이터 | 반영 요구사항 |
|---|---|---|---|
| OEM-IF-001 | Vehicle→SW | vehicle_speed_kph, gear, source_timestamp_s | SSR-005, 007, 008, 023, 029, SRQ-FUN-003, 004, 009 (gear는 형식 검사·보고만, `OI-012`) |
| OEM-IF-002 | Vehicle→SW | crash_status | SSR-001~004, 024 |
| OEM-IF-003 | Vehicle→SW | rear_left/right_approach_risk | SSR-011~015, 020 |
| OEM-IF-004 | Driver→SW | side, action, source | SRQ-FUN-001~008, 031, 038, SRQ-CON-002 |
| OEM-IF-005 | SW→Actuator model | lock_left/right | SRQ-FUN-021, SRQ-CON-004 |
| OEM-IF-006 | SW→Display | state, priority_reason, reason_code, input_validity | SRQ-FUN-022~025 |
| OEM-IF-007 | Vehicle→SW | fire/overtemperature/adult | SRQ-FUN-010, 011, 032, SSR-025 |
| OEM-IF-008 | Vehicle→SW | isofix_left/right | SRQ-FUN-012, 013, SSR-021 |
| OEM-IF-009 | Vehicle→SW | ignition_on, sensor_fault | SRQ-FUN-014~016, SSR-016~019, 022, 024 |
| (OEM 입력에 없음) 이벤트 조회 | Display/Test→SW | 이벤트 목록(최대 100건) | SRQ-FUN-030 — 조회 방식의 OEM 범위 확인 `OI-021` |

## 부록 A. 이유코드 (reason_code = priority_reason)

한 평가에는 하나의 reason_code를 기록한다. 선택은 SSR-027 우선순위 표에서 출력을 선택한 규칙의 코드이며(P1이 가장 높음), 같은 규칙의 다중 코드는 표의 값 오름차순이다. `RC_CRASH_PENDING_TIMEOUT`은 타임아웃을 처음 판정한 평가에서만 기록한다.

| 코드 | 값 | 발생 조건 | 선택 우선순위(SSR-027) |
|---|---|---|---|
| RC_NONE | 0 | 어떤 규칙도 출력을 바꾸지 않은 평가 | - |
| RC_CRASH_CONFIRMED | 1 | crash_status=CONFIRMED | P1 |
| RC_SENSOR_FAULT | 2 | sensor_fault TRUE 또는 INVALID(경고코드) | P2 |
| RC_CRASH_PENDING_HOLD | 3 | crash_status=PENDING 유지(≤1.0 s) | P3 |
| RC_CRASH_PENDING_TIMEOUT | 4 | PENDING 1.0 s 초과를 처음 판정한 평가 | P4 |
| RC_INPUT_STALE | 5 | STALE | P4 |
| RC_INPUT_INVALID | 6 | crash/ignition INVALID | P4 |
| RC_FIRE | 7 | fire_detected TRUE | P6 |
| RC_OVERTEMP | 8 | overtemperature_detected TRUE | P6 |
| RC_ADULT_PRESENT | 9 | adult_present TRUE | P6 |
| RC_APPROACH_RISK | 10 | 접근위험 TRUE(또는 INVALID)로 LOCK/억제 | P5 |
| RC_OVERRIDE_ACK | 11 | override 판정 | P5 |
| RC_ISOFIX | 12 | ISOFIX 잠금 | P7 |
| RC_IGNITION_OFF | 13 | 시동 OFF 해제 | P9 |
| RC_SPEED_AUTO_LOCK | 14 | 이동으로 인한 LOCK(해당 평가에 유효한 unlock 명령 없음) | P8 |
| RC_UNLOCK_DENIED_MOVING | 15 | 이동으로 인한 LOCK + 그 평가에 유효한 unlock 명령 있음 | P8 |
| RC_DRIVER_LOCK | 16 | 운전자 lock 적용 | P10 |
| RC_DRIVER_UNLOCK | 17 | 운전자 unlock 적용 | P10 |
| RC_COMMAND_REJECTED | 18 | 명령 거절(P1~P9가 코드를 선택하지 않은 경우) | P11 |

오류코드(반환값): `RET_OK`=0, `RET_ERR_NULL`=-1, `RET_ERR_SERIALIZE`=-2.

## 부록 B. 시험 입력 벡터 기준

- **B-1 거절 명령 벡터**: side/action/source 각각에 대해 누락, 범위 밖 값, 미등록 enum, 잘못된 형식(문자열 대신 숫자 등)을 단일 결함으로 주입한 전 조합(정상 나머지 필드).
- **B-2 결정론 벡터 V**: `docs/test-vectors/vector-v.csv` (23행, 평가 간격 0.010 s). 정지·정상, 이동 시작과 해제 거부, 접근위험 억제·override, 화재, ISOFIX, 시동 OFF/ON, 충돌 PENDING→CONFIRMED→NONE, sensor_fault, stale 경계(0.200/0.210), 범위 밖 속도, 명령 거절을 포함한다. 기준 해시값은 입력이 아니라 출력이므로 고정하지 않고 "1,000회 해시가 모두 같음"으로 판정한다.
