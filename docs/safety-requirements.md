# 소프트웨어 안전 요구사항 — 전자식 차일드락 제어 SW

| 항목 | 값 |
|---|---|
| 문서 ID / 버전 | SSR-ECL-001 / 0.3 (Draft, 재리뷰 대기) |
| 입력 | OEM-A가 ASIL B로 할당한 OEM-SR-001~004 (HARA/ASIL 근거는 재구성하지 않는다) |
| 관련 문서 | `docs/SRS.md`(용어집·부록 A 이유코드 포함), `docs/open-issues.md`, `docs/traceability.md` |

**범위 경계**: 교육용 가상 OEM 입력이다. ISO 26262 준수·적합성·인증·ASIL 달성을 주장하지 않는다. 용어는 SRS §0 용어집을 따른다. **이 문서의 모든 요구사항은 우선순위 H, 상태 Draft이며 ASIL B(OEM 할당 표기)이다**(우선/상태 열 참조. SSR-027, 028, 030과 §5의 가정 요구사항은 파생이며 ASIL은 OEM 입력 근거를 따른다). ID는 불변이다(SRS 머리글).

**안전 상태와 FTTI(가정, `OI-013`)**: OEM은 안전 상태와 FTTI를 정의하지 않았다. 가정: 안전 상태 = 충돌 CONFIRMED는 RELEASE, 접근위험 TRUE 도어는 LOCK, 그 외 결함 시에는 직전 확정 출력 유지. 시간 허용치는 OEM 수용 기준 — 충돌 해제 0.300 s, stale 기준 0.200 s 초과 후 DEGRADED 전이 0.100 s. 평가주기 0.010 s에서는 충돌 해제가 입력을 처리한 같은 평가(≤ 0.010 s)에 이루어진다. **잔여 위험**: 속도가 STALE인 채 DEGRADED가 되면 이동 중에도 직전 출력(RELEASE일 수 있음)을 유지한다. 사용자 승인 가정이며 OEM 안전 담당 확인 대기(DEC-003)이다.

## 1. 충돌 (OEM-SR-001)

| ID | 요구사항 | 수용 기준 | 근거 | 검증 | 상위(OEM) | OEM 계획 ID | 우선/상태 |
|---|---|---|---|---|---|---|---|
| SSR-001 | SW는 유효한 crash_status가 CONFIRMED이면 다른 모든 명령(운전자 명령 포함)보다 우선하여 좌, 우 출력을 RELEASE로 해야 한다. | CONFIRMED를 처리한 평가에서 두 출력이 RELEASE이고 입력 처리부터 출력까지 0.300 s 이내이다(평가주기 0.010 s, `OI-003`). | OEM-SR-001, `OI-003` | UT, ST | OEM-SR-001 | SWR-007 | H/Draft |
| SSR-002 | SW는 crash_status가 CONFIRMED인 동안 이후 모든 평가에서 좌, 우 출력을 RELEASE로 유지해야 한다. | CONFIRMED가 N회 연속인 평가 모두에서 RELEASE. NONE으로 복귀하면 일반 중재(SSR-027)로 돌아간다. | OEM-SR-001 수용기준("이후 입력 주기에도 유지"), `OI-007` | UT, ST | OEM-SR-001 | SWR-007 | H/Draft |
| SSR-003 | SW는 crash_status가 PENDING(지속 ≤ 1.0 s)이면 SSR-027의 P3에 따라 출력을 유지해야 한다. | PENDING인 평가에서 접근위험 도어 외의 출력이 직전 값과 같다. | OEM 추적성(SWR-008 "PENDING 처리"), `OI-007` | UT, ST | OEM-SR-001 | SWR-008 | H/Draft |
| SSR-036 | SW는 PENDING 유지(SSR-003)가 적용된 평가에서 이유코드를 `RC_CRASH_PENDING_HOLD`로 기록해야 한다. | reason_code=`RC_CRASH_PENDING_HOLD`(상위 P1, P2 미성립 시). | OEM-SR-001, SSR-027, `OI-007` | UT | OEM-SR-001 | SWR-008 | H/Draft |
| SSR-004 | SW는 PENDING 지속이 1.0 s를 초과하면 SSR-003의 유지를 적용하지 않아야 한다(PENDING 지속의 시작은 SRS 용어집). | 지속 1.000 s까지는 유지, 1.010 s에서는 유지하지 않는다. | 파생(PENDING 무한 유지 방지), `OI-007` | UT | OEM-SR-001 | SWR-008 | H/Draft |
| SSR-031 | SW는 PENDING 지속이 1.0 s를 초과했음을 처음 판정한 평가에서만 이유코드를 `RC_CRASH_PENDING_TIMEOUT`으로 기록해야 한다. | 1.010 s 평가에서만 해당 코드, 이후 평가는 일반 규칙의 코드. | 파생, `OI-007` | UT | OEM-SR-001 | SWR-008 | H/Draft |

## 2. 입력 유효성과 freshness (OEM-SR-003)

| ID | 요구사항 | 수용 기준 | 근거 | 검증 | 상위(OEM) | OEM 계획 ID | 우선/상태 |
|---|---|---|---|---|---|---|---|
| SSR-005 | SW는 모든 필수 입력의 형식과 범위를 평가 전에 검사하여 아래 입력 유효 범위 표를 벗어나거나 누락된 입력을 INVALID로 판정해야 한다. | 표의 경계값(최소, 최대, 최소−ε, 최대+ε, 누락)에서 판정이 표와 같다. | OEM-SR-003 | UT, ST | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-006 | SW는 INVALID로 판정한 입력의 input_validity를 INVALID로 보고해야 한다. | INVALID 입력 주입 시 상태 조회의 해당 input_validity가 INVALID. | OEM-SR-003 | UT, ST | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-032 | SW는 INVALID로 판정한 입력의 원래 값을 결정에 사용하지 않아야 한다(대체 값은 §5의 SSR-020~025, 029가 정한다). | INVALID 입력의 값을 바꿔도 출력 결과가 같다. | OEM-SR-003("평가 전 거절"), `OI-008` | UT | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-007 | SW는 평가 호출의 스냅샷에서 `timestamp_s − source_timestamp_s`가 0.200 s를 초과하면 입력을 STALE로 판정해야 한다. | 0.200 s는 fresh, 0.210 s는 STALE. | OEM-SR-003, OEM-IF-001(source_timestamp_s), `OI-004` | UT, ST | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-008 | SW는 평가 호출의 `timestamp_s − 마지막으로 스냅샷을 수신한 평가의 timestamp_s`가 0.200 s를 초과하면(스냅샷이 없는 호출 포함) 입력을 STALE로 판정해야 한다. | 마지막 스냅샷 후 0.200 s 호출은 fresh, 0.210 s 호출은 STALE. | OEM-SR-003, `OI-004` | UT, ST | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-009 | SW는 STALE을 판정한 평가에서 state를 DEGRADED로 해야 한다(판정 시점부터 0.100 s 이내 = 같은 평가, `OI-003`). | STALE 판정 평가에서 state=DEGRADED(SSR-030 우선순위 적용). | OEM-SR-003, `OI-003`, `OI-013` | UT, ST | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-033 | SW는 STALE을 판정한 평가에서 이유코드를 `RC_INPUT_STALE`로 기록해야 한다(SSR-027의 상위 규칙이 없는 경우). | 정상 평가 중 STALE 주입 시 reason_code=`RC_INPUT_STALE`. | OEM-SR-003, SSR-027 | UT | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-010 | SW는 DEGRADED 동안 출력을 직전 확정 출력으로 유지해야 한다(SSR-027 P4). | DEGRADED 평가에서 접근위험 도어 외의 출력이 직전 값. | OEM-SR-003 ("정의된 방식"), `OI-013` | UT, ST | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-034 | SW는 fresh이고 유효한 입력의 평가에서 DEGRADED 조건이 해소되면 즉시 해소된 상태로 복귀해야 한다(SSR-030). | 복구 입력의 첫 평가에서 state=NORMAL. | OEM-SR-003, `OI-013` | UT, ST | OEM-SR-003 | SWR-013 | H/Draft |
| SSR-029 | SW는 source_timestamp_s가 INVALID이면 STALE로 간주해야 한다. | INVALID 주입 시 SSR-009 동작이 나타난다. | OEM-SR-003, `OI-008` | UT | OEM-SR-003 | SWR-013 | H/Draft |

**입력 유효 범위 표 (SSR-005)** — 필수 입력은 SRS 용어집의 목록이다.

| 입력 | 유효 범위 | INVALID 조건 |
|---|---|---|
| vehicle_speed_kph | 0.0 ≤ v ≤ 300.0, 유한한 수 | 범위 밖, NaN/무한, 누락 |
| gear | P, N, D, R | 그 외 값, 누락 |
| source_timestamp_s | 0 ≤ 값 ≤ timestamp_s, 유한한 수 | 범위 밖, 누락 |
| crash_status | NONE, PENDING, CONFIRMED | 그 외 값, 누락 |
| rear_left/right_approach_risk, fire_detected, overtemperature_detected, adult_present, isofix_left/right, ignition_on, sensor_fault | boolean(0 또는 1) | 그 외 값, 누락 |

## 3. 후측방 접근위험 (OEM-SR-002)

| ID | 요구사항 | 수용 기준 | 근거 | 검증 | 상위(OEM) | OEM 계획 ID | 우선/상태 |
|---|---|---|---|---|---|---|---|
| SSR-011 | SW는 후측방 접근위험이 유효한 TRUE인 도어의 출력을, 충돌 CONFIRMED(SSR-027 P1)가 아닌 모든 평가에서 LOCK으로 해야 한다(P2~P4의 유지 규칙과 P6 강제 해제보다 우선). | rear_left_approach_risk=TRUE이면 P1 미성립 시 lock_left=LOCK. sensor_fault·STALE·PENDING·화재 중에도 LOCK. | OEM-SR-002(무예외), `OI-006` | UT, ST | OEM-SR-002 | SWR-005 | H/Draft |
| SSR-012 | SW는 접근위험 TRUE인 도어의 해제 요청(운전자 unlock, 시동 OFF 해제)을 억제해야 한다. | TRUE 상태에서 unlock 명령 후에도 해당 도어가 LOCK. | OEM-SR-002 | UT, ST | OEM-SR-002 | SWR-005 | H/Draft |
| SSR-013 | SW는 접근위험으로 LOCK 또는 해제 억제를 한 평가에서 이유코드를 `RC_APPROACH_RISK`로 기록해야 한다(SSR-027의 상위 규칙이 없는 경우). | 해당 평가 reason_code가 `RC_APPROACH_RISK`. | OEM-SR-002 ("원인이 기록") | UT, ST | OEM-SR-002 | SWR-005 | H/Draft |
| SSR-014 | SW는 접근위험을 좌, 우 도어별로 독립적으로 처리해야 한다. | left만 TRUE이면 right 출력은 접근위험 영향을 받지 않는다. | OEM-SR-002 | UT, ST | OEM-SR-002 | SWR-009 | H/Draft |
| SSR-015 | SW는 접근위험 TRUE인 도어의 첫 억제 시각을 기록해야 한다. | 억제 평가에서 도어별 시각이 기록된다(SRQ-FUN-017 입력). | OEM-FR-003 (SWR-006), `OI-011`, `OI-015` | UT | OEM-SR-002, OEM-FR-003 | SWR-006 | H/Draft |
| SSR-035 | SW는 접근위험이 유효한 FALSE가 되면 해당 도어의 첫 억제 시각을 삭제해야 한다. | FALSE 전환 평가 후 기록이 없다. | OEM-FR-003, `OI-011` | UT | OEM-SR-002, OEM-FR-003 | SWR-006 | H/Draft |

## 4. sensor_fault (OEM-SR-004)

| ID | 요구사항 | 수용 기준 | 근거 | 검증 | 상위(OEM) | OEM 계획 ID | 우선/상태 |
|---|---|---|---|---|---|---|---|
| SSR-016 | SW는 sensor_fault가 TRUE(SSR-022에 따라 INVALID 포함)이면 새 운전자 명령을 적용하지 않아야 한다. | TRUE인 평가에서 lock/unlock 명령이 출력에 반영되지 않는다. | OEM-SR-004 | UT, ST | OEM-SR-004 | SWR-021 | H/Draft |
| SSR-017 | SW는 sensor_fault가 TRUE인 평가에서 좌, 우 출력을 직전 확정 출력으로 유지해야 한다(SSR-027 P2). OEM 원문의 "직전 확정 출력 유지"를 그대로 따른다. | TRUE인 첫 평가에서 접근위험 도어 외의 출력이 직전 값. | OEM-SR-004, `OI-006` | UT, ST | OEM-SR-004 | SWR-021 | H/Draft |
| SSR-018 | SW는 sensor_fault가 TRUE인 첫 평가에서 state를 FAULT로 해야 한다. | 첫 평가의 state=FAULT. | OEM-SR-004 | UT, ST | OEM-SR-004 | SWR-021 | H/Draft |
| SSR-019 | SW는 sensor_fault가 TRUE인 평가에서 경고코드 `RC_SENSOR_FAULT`를 제공해야 한다. | reason_code(경고코드)=`RC_SENSOR_FAULT`(상위 P1 미성립 시). | OEM-SR-004 | UT, ST | OEM-SR-004 | SWR-021 | H/Draft |

## 5. INVALID 입력의 안전 측 대체 값 (가정, `OI-008`)

OEM은 신호별 INVALID 결과를 정의하지 않았다. 사용자 승인 가정(DEC-003)이며 OEM 안전 담당 확인 대기이다.

| ID | 요구사항 | 수용 기준 | 근거 | 검증 | 상위(OEM) | 우선/상태 |
|---|---|---|---|---|---|---|
| SSR-020 | SW는 rear_left/right_approach_risk가 INVALID이면 해당 도어를 접근위험 TRUE로 간주해야 한다. | INVALID 주입 시 해당 도어 LOCK, 해제 억제. | OEM-SR-002, OEM-SR-003, `OI-008` | UT | OEM-SR-002, OEM-SR-003 | H/Draft |
| SSR-021 | SW는 isofix_left/right가 INVALID이면 해당 도어를 ISOFIX TRUE로 간주해야 한다. | INVALID 주입 시 해당 도어 LOCK. | OEM-FR-006, OEM-SR-003, `OI-008` | UT | OEM-SR-003 | H/Draft |
| SSR-022 | SW는 sensor_fault가 INVALID이면 TRUE로 간주해야 한다. | INVALID 주입 시 SSR-016~019 동작. | OEM-SR-004, OEM-SR-003, `OI-008` | UT | OEM-SR-003, OEM-SR-004 | H/Draft |
| SSR-023 | SW는 vehicle_speed_kph가 INVALID이면 이동 중으로 간주해야 한다. | INVALID 주입 시 unlock 거부, 자동 잠금 LOCK. | OEM-FR-002, OEM-SR-003, `OI-008` | UT | OEM-SR-003 | H/Draft |
| SSR-024 | SW는 crash_status 또는 ignition_on이 INVALID이면 SSR-027의 P4에 따라 출력을 유지해야 한다. | INVALID 주입 시 접근위험 도어 외의 출력이 직전 값. | OEM-SR-003, `OI-008` | UT | OEM-SR-003 | H/Draft |
| SSR-037 | SW는 crash_status 또는 ignition_on INVALID로 출력을 유지한 평가에서 이유코드를 `RC_INPUT_INVALID`로 기록해야 한다. | reason_code=`RC_INPUT_INVALID`(상위 P1~P3 미성립 시). | OEM-SR-003, SSR-027, `OI-008` | UT | OEM-SR-003 | H/Draft |
| SSR-025 | SW는 fire_detected, overtemperature_detected, adult_present가 INVALID이면 해당 입력을 FALSE로 간주해야 한다. | INVALID 주입 시 강제 해제가 일어나지 않는다. | OEM-FR-005, OEM-SR-003, `OI-008` | UT | OEM-SR-003 | H/Draft |
| SSR-026 | SW는 SSR-020~025, 029로 대체한 INVALID 입력이 하나라도 있으면 state를 DEGRADED로 해야 한다(SSR-030 우선순위 적용). | sensor_fault가 TRUE·INVALID가 아닌 입력에서 INVALID 주입 평가의 state=DEGRADED(FAULT가 있으면 FAULT, SSR-030). | OEM-SR-003, `OI-008` | UT, ST | OEM-SR-003 | H/Draft |

## 6. 중재와 상태 (파생, `OI-006`, `OI-005`)

| ID | 요구사항 | 수용 기준 | 근거 | 검증 | 상위(OEM) | 우선/상태 |
|---|---|---|---|---|---|---|
| SSR-027 | SW는 도어별로 아래 우선순위 표에서 성립한 가장 높은 우선순위 규칙의 출력을 선택해야 한다. | 표의 모든 규칙 쌍(상위·하위 동시 성립)에서 상위 규칙의 출력과 이유코드가 선택된다. | OEM-SR-001("다른 명령보다 우선") 외 정의 없음, `OI-006` | UT, IT, ST | OEM-SR-001~004, OEM-FR-001~007 | H/Draft |
| SSR-028 | SW는 state를 FAULT > DEGRADED > OFF > NORMAL의 우선순위로 정해야 한다. | sensor_fault+ignition OFF 동시 입력에서 FAULT, STALE+ignition OFF에서 DEGRADED. | 파생, `OI-005` | UT | OEM-SR-004, OEM-FR-007 | H/Draft |
| SSR-030 | SW는 매 평가에서 state를 아래 조건표로 이전 state와 무관하게 다시 정해야 한다(조건 해소 시 즉시 복귀). | 조건표의 모든 조합에서 state가 표와 같고, FAULT→NORMAL, DEGRADED→NORMAL, OFF→NORMAL 전이가 조건 해소 평가에서 일어난다. | 파생, `OI-005` | UT | OEM-SR-004, OEM-FR-007 | H/Draft |

**state 조건표 (SSR-030)**: FAULT = sensor_fault TRUE 또는 INVALID. DEGRADED = FAULT가 아니고 STALE 또는 SSR-026의 INVALID 대체 입력 존재. OFF = FAULT, DEGRADED가 아니고 ignition_on 유효 FALSE. NORMAL = 그 외. 초기 state는 OFF(SRQ-FUN-036).

**우선순위 표 (SSR-027, 높음→낮음)**: "유지"는 직전 확정 출력 유지이며, **P2~P4의 유지는 접근위험 TRUE(또는 INVALID로 간주) 도어를 LOCK으로 한다**(SSR-011, 안전 상태 정의와 일치). 범위가 "해당 도어"인 규칙은 도어별로, "두 도어"인 규칙은 P5에서 LOCK이 선택되지 않은 도어에 적용한다.

| 순위 | 규칙 | 범위 | 선택되는 출력 | 이유코드 |
|---|---|---|---|---|
| P1 | crash_status=CONFIRMED | 두 도어 | RELEASE | RC_CRASH_CONFIRMED |
| P2 | sensor_fault TRUE(또는 INVALID) | 두 도어 | 유지 | RC_SENSOR_FAULT |
| P3 | crash_status=PENDING(≤1.0 s) | 두 도어 | 유지 | RC_CRASH_PENDING_HOLD |
| P4 | STALE, 또는 crash/ignition INVALID | 두 도어 | 유지 | RC_INPUT_STALE / RC_INPUT_INVALID (PENDING 초과 첫 판정 평가는 RC_CRASH_PENDING_TIMEOUT) |
| P5 | 접근위험 TRUE(또는 INVALID) | 해당 도어 | LOCK(해제 억제) | RC_APPROACH_RISK (override 시 RC_OVERRIDE_ACK) |
| P6 | fire/overtemperature/adult 유효 TRUE | P5에서 LOCK이 선택되지 않은 도어 | RELEASE | RC_FIRE / RC_OVERTEMP / RC_ADULT_PRESENT |
| P7 | ISOFIX TRUE(또는 INVALID) | 해당 도어 | LOCK | RC_ISOFIX |
| P8 | 속도 3.0 이상(또는 INVALID) | 두 도어 | LOCK | RC_SPEED_AUTO_LOCK, 그 평가에 유효한 unlock 명령이 있으면 RC_UNLOCK_DENIED_MOVING |
| P9 | ignition_on 유효 FALSE | 두 도어 | RELEASE | RC_IGNITION_OFF |
| P10 | 운전자 명령 | 선택 도어 | lock→LOCK, unlock→RELEASE(정지일 때) | RC_DRIVER_LOCK / RC_DRIVER_UNLOCK |
| P11 | 명령 거절(보조) | - | 출력 영향 없음 | RC_COMMAND_REJECTED (P1~P9가 코드를 선택하지 않았을 때) |

규칙이 성립하지 않은 도어는 직전 확정 출력을 유지한다.

**우선순위 근거와 안전 영향 (`OI-006`, 사용자 승인 가정, OEM 안전 담당 확인 대기)**:
1. **P5(접근위험 LOCK) > P6(강제 해제)**: OEM-SR-002는 접근위험 TRUE 도어의 LOCK에 예외를 두지 않으므로 문언대로 따랐다. 안전 영향: 화재·과온·성인 탑승 중에도 접근위험이 있는 도어는 열리지 않는다. 양쪽 도어에 접근위험이 동시에 TRUE이면 두 도어 모두 LOCK되어 화재 탈출이 막힐 수 있다(충돌 CONFIRMED는 P1이 해제하므로 예외). 이 상충을 어느 쪽으로 해소할지는 OEM 안전 담당이 정해야 한다.
2. **P6 > P7, P6 > P8**: OEM-FR-005는 강제 해제에 속도·ISOFIX 조건을 두지 않으므로 문언대로 따랐다. 안전 영향: 주행 중에도 adult_present, fire, overtemperature가 TRUE이면 ISOFIX 도어를 포함해 두 도어가 RELEASE되어 어린이가 이동 중 문을 열 수 있다. adult_present가 오검출이면 위험이 현실화한다.
3. **P2~P4(유지) > P6, P7, P8**: OEM-SR-003/004의 "직전 확정 출력 유지"를 따랐다. 안전 영향: sensor_fault, PENDING, STALE, INVALID 중에는 화재 해제가 일어나지 않고, 직전 출력이 RELEASE였다면 주행 중에도 RELEASE가 유지된다(접근위험 도어만 LOCK 보장).
4. **P8(속도 LOCK) > P9(시동 OFF 해제)**: OEM-FR-002와 FR-007이 각각 무조건 문구이며 동시 성립(주행 중 시동 OFF) 시 어린이 보호 측(LOCK)을 선택했다.
