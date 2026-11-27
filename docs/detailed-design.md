# 소프트웨어 상세 설계 (SWE.3) — 전자식 차일드락 제어 SW 코어

| 항목 | 값 |
|---|---|
| 문서 ID / 버전 | SDD-ECL-001 / 0.1 (Draft) |
| 입력 | `docs/architecture.md`(SAD-ECL-001 v0.1), `docs/SRS.md`, `docs/safety-requirements.md` |
| 스킬 | `detailed-design`, 구현 규칙은 `implementation` 스킬 |
| 리뷰 상태 | 미리뷰 (DEC-008) |
| 범위 | 코어 컴포넌트 CMP-TYP, VAL, CMD, OVR, ARB, STA, LOG, SER, CORE. CMP-WEB은 이터레이션 3 |

**모듈 인터페이스 정의 기준 (DEC-009)**: 다른 모듈이 호출하거나 컴포넌트 외부로 노출되는 함수를 가진 모듈(= `include/`에 헤더가 있는 모듈)은 인터페이스를 반드시 정의한다. 모듈 내부 `static` 함수는 인터페이스를 정의하지 않고 함수 설계만 기술한다.

**구현 규칙 요약**: 모든 함수는 `ecl_ret_t`(또는 값)를 반환하며 `return`은 마지막에 한 번. 내부 함수는 `static`. 순환복잡도 ≤ 10, 함수 ≤ 80라인. 매직 넘버 금지(상수는 `ecl_types.h`). 시간 비교는 `ECL_TIME_EPS_S`(1e-9 s)를 허용 오차로 쓴다(`a > limit + eps`가 "초과", `a <= limit + eps`가 "이하"). 전역 변수 없음.

## 1. 모듈 구성

| 모듈(소스/헤더) | 컴포넌트 | 책임 | 공개 함수(인터페이스) |
|---|---|---|---|
| `include/ecl_types.h` | CMP-TYP | 공통 타입·상수 정의(코드 없음) | - |
| `src/ecl_val.c` / `include/ecl_val.h` | CMP-VAL | 입력 조건화 | `ecl_val_init`, `ecl_val_condition`, `ecl_val_condition_tick` |
| `src/ecl_cmd.c` / `include/ecl_cmd.h` | CMP-CMD | 명령 해석 | `ecl_cmd_interpret` |
| `src/ecl_ovr.c` / `include/ecl_ovr.h` | CMP-OVR | override 추적 | `ecl_ovr_init`, `ecl_ovr_update` |
| `src/ecl_arb.c` / `include/ecl_arb.h` | CMP-ARB | 중재 | `ecl_arb_decide` |
| `src/ecl_sta.c` / `include/ecl_sta.h` | CMP-STA | state 결정 | `ecl_sta_decide` |
| `src/ecl_log.c` / `include/ecl_log.h` | CMP-LOG | 이벤트 기록 | `ecl_log_init`, `ecl_log_append`, `ecl_log_query`, `ecl_log_test_set_counter` |
| `src/ecl_ser.c` / `include/ecl_ser.h` | CMP-SER | 직렬화 | `ecl_ser_status` |
| `src/ecl_core.c` / `include/ecl_core.h` | CMP-CORE | 평가 조정, 공개 API | `ecl_core_init`, `ecl_core_evaluate`, `ecl_core_tick`, `ecl_core_get_status`, `ecl_core_serialize_status`, `ecl_core_query_events` |

공개 인터페이스의 시그니처, 사전/사후 조건, 오류는 SAD §4가 정의하며 본 문서는 그와 일치한다. 아래는 모듈 내부 설계(구조체, 알고리즘, 내부 함수)이다. `ecl_types.h` 추가 정의: `ECL_TIME_EPS_S`=1e-9, `ECL_SIG_COUNT`=13, 구조체 `ecl_inputs_t`(아래 §2), `ecl_cmd_result_t`, `ecl_ovr_result_t`, `ecl_decision_t`.

## 2. ecl_types.h — 데이터 정의

`ecl_inputs_t`(VAL 출력, 모든 하위 모듈의 입력): `validity[13]`(ecl_validity_t); `speed_moving`(1: 유효 속도 ≥ 3.0 또는 속도 INVALID); `crash_confirmed`(유효 CONFIRMED이고 STALE 아님); `hold_reason`(ecl_reason_t, P2~P4 유지 조건: 높은 순서로 `RC_SENSOR_FAULT`, `RC_CRASH_PENDING_HOLD`, `RC_INPUT_STALE`, `RC_INPUT_INVALID`, 없으면 `RC_NONE`); `pending_timeout_first`; `approach[2]`(INVALID→1); `isofix[2]`(INVALID→1); `fire`, `overtemp`, `adult`(유효 TRUE만 1); `ignition_off`(유효 FALSE); `sensor_fault`(TRUE 또는 INVALID); `stale`; `substituted`(INVALID 입력이 하나라도 있음, SSR-026); `command`(원 명령 복사).
`ecl_cmd_result_t`: `present`, `rejected`, `valid`, `side`, `action`, `source`. `ecl_ovr_result_t`: `suppress[2]`, `override[2]`, `ack[2]`. `ecl_decision_t`: `lock[2]`, `reason_code`, `command_rejected`.
`ecl_reason_t` 값은 SRS 부록 A(0~18), 우선순위 순위는 ARB 내부 표 `P_RANK[]`(P1=1 … P11=11, RC_NONE=99).

## 3. 모듈별 상세 설계

### 3.1 ecl_val (CMP-VAL)

내부 상태 `ecl_val_t`(비공개 관례): `rx_valid`, `last_rx_s`, `pending_active`, `pending_start_s`, `pending_reported`, `last_snap`(마지막 수신 스냅샷, 틱 재사용용).

| 함수 | 종류 | 설계 |
|---|---|---|
| `ecl_val_init` | 공개 | 모든 상태를 0으로 설정. NULL→`RET_ERR_NULL` |
| `ecl_val_condition` | 공개 | `build_inputs` 후 `rx_valid`=1, `last_rx_s`=스냅샷 시각, `last_snap`=스냅샷(명령 제외) |
| `ecl_val_condition_tick` | 공개 | 수신 이력이 있으면 `last_snap`을 시각만 `now_s`로 바꾸고 **수신 당시의 지연(timestamp_s − source_timestamp_s)을 유지**하도록 `source_timestamp_s`를 조정해 `build_inputs`로 재조건화한다(명령 없음). 수신 이력이 없으면 입력 없음 상태로 `now_s − 0.0 > 0.200`일 때만 STALE로 한다 |
| `build_inputs` | static | `check_bool_signals` → `check_other_signals` → `evaluate_staleness` → `apply_substitution` → `track_pending` → `derive_hold` 순으로 `ecl_inputs_t`를 채우고 `ecl_ret_t`로 결과를 전파한다 |
| `check_bool_signals`, `check_other_signals` | static | SSR-005 표로 `validity[]`를 채우고 INVALID 개수를 누적한다(기어는 대체 대상이 아니므로 개수에서 제외, SSR-026). 헬퍼 `is_bool`, `in_enum`, `speed_ok`, `src_ts_ok` |
| `evaluate_staleness` | static | STALE = source_timestamp_s INVALID(SSR-029) 또는 `timestamp_s − source_timestamp_s > 0.200+eps`(SSR-007) 또는 `rx_valid && timestamp_s − last_rx_s > 0.200+eps`(SSR-008: 스냅샷이 있는 호출과 틱 모두에 적용, 따라서 0.200 s를 넘겨 도착한 스냅샷도 STALE). STALE이면 VALID였던 신호를 STALE로 바꾸고 판정값을 반환한다 |
| `apply_substitution` | static | 효과 값 계산: approach는 VALID/STALE이면 입력 값 그대로, INVALID이면 TRUE(SSR-011, 020); isofix, sensor_fault는 INVALID→TRUE(SSR-021, 022); fire, overtemp, adult는 INVALID와 STALE→0(SSR-025); speed는 INVALID→이동(SSR-023); ignition_off, crash_confirmed는 VALID일 때만. 헬퍼 `bool_effective`, `approach_effective` |
| `track_pending` | static | PENDING 지속 시간 추적: 시작 기록, `elapsed ≤ 1.0+eps`이면 유지, 초과의 첫 판정에서 `pending_timeout_first`. 다른 유효 crash 값이면 초기화, INVALID/STALE이면 상태 유지 |
| `derive_hold` | static | `hold_reason`: sensor_fault > PENDING 유지 > STALE > crash/ignition INVALID > 없음 |

### 3.2 ecl_cmd (CMP-CMD)

| 함수 | 설계 |
|---|---|
| `ecl_cmd_interpret`(공개) | `present`가 0이면 명령 없음(present=0, valid=0, rejected=0). 0이 아니면 `present`=1로 보고, raw `present`가 1이고 `side∈[LEFT..ALL]`, `action∈[LOCK..UNLOCK]`, `source∈[PHYSICAL_BUTTON..MOBILE_APP]`이며 `ECL_ABSENT`가 아니면 `valid`=1, 아니면 `rejected`=1(raw `present`가 0/1 밖이면 형식 오류로 거절, DEC-013). NULL→`RET_ERR_NULL` |
| `field_ok`(static) | `(v != ECL_ABSENT) && (v >= lo) && (v <= hi)` |

### 3.3 ecl_ovr (CMP-OVR)

상태 `ecl_ovr_t`(private): `first_valid[2]`, `first_s[2]`, `first_source[2]`, `override[2]`.

| 함수 | 설계 |
|---|---|
| `ecl_ovr_init`(공개) | 0으로 초기화 |
| `ecl_ovr_update`(공개) | 도어 0,1에 `update_door`를 적용하고 결과를 `out`에 복사 |
| `update_door`(static) | 입력: 접근위험 `ap`, `req`(그 도어에 대한 유효 unlock 명령 && 명령 차단 아님), 시각, source. `ap`==0이면 `first_valid`=0, `override`=0, 종료. `ap`==1이면: `req`이면 `suppress`=1 이후 `apply_request`; `req`가 아니면 `first_valid`이고 10.0 s 초과 시 `override`=0 |
| `apply_request`(static) | `first_valid`==0 → 기록(시각, source). 그 외 `dt = now − first_s`: `dt ≤ 10.0+eps`이고 같은 source → `override`=1, `ack`=1. `dt > 10.0+eps` → 새 첫 억제 기록(시각, source), `override`=0. 그 외(다른 source) → 변경 없음 |
| `door_requested`(static) | `cmd.valid && cmd.action==UNLOCK && (cmd.side==ALL \|\| cmd.side==door)` |
명령 차단 조건: `in->crash_confirmed \|\| in->hold_reason != RC_NONE`(그 평가에서는 명령이 적용되지 않으므로 억제·override를 기록하지 않음).

### 3.4 ecl_arb (CMP-ARB)

| 함수 | 설계 |
|---|---|
| `ecl_arb_decide`(공개) | NULL 검사 → `crash_confirmed`이면 두 도어 RELEASE/`RC_CRASH_CONFIRMED`(P1), 아니면 `decide_both` → 거절 보조 코드(`command_rejected`이고 reason이 `RC_NONE`이면 `RC_COMMAND_REJECTED`, P11) |
| `decide_both`(static) | `decide_door`를 좌·우에 적용하고 `merge_reason`으로 이유코드를 병합한다 |
| `decide_door`(static) | `hold_reason != RC_NONE`이면 `decide_hold`, 아니면 `decide_rules` |
| `decide_hold`(static) | P2~P4: 접근위험 도어는 LOCK, 그 외 `prev`, reason=`hold_reason` |
| `decide_rules`(static) | P5~P9는 `apply_input_rules`, 성립하지 않으면 P10 `apply_driver`, 아무 규칙도 없으면 `prev`/`RC_NONE`. `pending_timeout_first`이면 reason을 `RC_CRASH_PENDING_TIMEOUT`(P4)으로 덮어쓴다 |
| `apply_input_rules`(static) | 접근위험(`ovr->ack`이면 `RC_OVERRIDE_ACK`) → 화재/과온/성인(`fire_reason`) → ISOFIX → 이동(`speed_reason`) → 시동 OFF 순서로 첫 성립 규칙을 적용 |
| `apply_driver`(static) | `driver_applies`이면 lock→LOCK/`RC_DRIVER_LOCK`, unlock→RELEASE/`RC_DRIVER_UNLOCK` |
| `driver_applies`, `speed_reason`, `fire_reason`(static) | 명령의 도어 적용 판정 / 이동 중 이유코드(유효 unlock이면 `RC_UNLOCK_DENIED_MOVING`) / fire > overtemp > adult |
| `merge_reason`, `reason_rank`(static) | 순위가 높은(작은) 코드, 같으면 값이 작은 코드. 순위는 `P_RANK[]`(`ECL_RANK_P1`~`P11`, `ECL_RANK_NONE`) |

(`decide_crash`, `apply_reject`는 위 구조로 통합하여 별도 함수가 없다.)

### 3.5 ecl_sta (CMP-STA)

`ecl_sta_decide`: `sensor_fault` → FAULT; 아니면 `stale || substituted` → DEGRADED; 아니면 `ignition_off` → OFF; 아니면 NORMAL. (NULL→`RET_ERR_NULL`)

### 3.6 ecl_log (CMP-LOG)

상태 `ecl_log_t`(private): `events[100]`, `head`(가장 오래된 인덱스), `count`, `next_id`(uint64), `exhausted`.
- `ecl_log_init`: head=count=0, next_id=1, exhausted=0.
- `ecl_log_append`: exhausted이면 `*assigned_id`=0. 아니면 id=next_id, 이벤트 저장(`store_event`: count<100이면 (head+count)%100에 추가, 가득 차면 head 위치를 덮어쓰고 head 전진), id==UINT64_MAX이면 exhausted=1 아니면 next_id++, `*assigned_id`=id.
- `ecl_log_query`: n = min(count, capacity). 최신 n건을 오름차순으로 복사(시작 인덱스 = (head + count − n) % 100), `*count`=n.
- `ecl_log_test_set_counter`(시험 빌드 `ECL_TEST_BUILD`에서만 컴파일): value==UINT64_MAX이면 exhausted=1, 아니면 next_id=value+1, exhausted=0.

### 3.7 ecl_ser (CMP-SER)

고정 순서 문자열 생성(SAD §4.8). 먼저 `buf`가 NULL인 쓰기 상태(`ser_writer_t`)로 **건식 실행하여 길이를 계산**하고, `길이+1 > capacity`이면 `RET_ERR_SERIALIZE`(버퍼 불변), 아니면 같은 `write_all`을 `buf`에 실제로 쓴다(`write_pair`가 `name=value;`를 쓰고 길이를 누적). 필드 순서: `lock_left`, `lock_right`, `state`, `reason_code`, `override_left`, `override_right`, `command_rejected`, 이어서 신호 13종의 `validity_<신호>`(speed, gear, source_ts, crash, approach_left, approach_right, fire, overtemp, adult, isofix_left, isofix_right, ignition, sensor_fault). 이름 표는 `static const char *const`이며 크기는 `ecl_types.h`의 `ECL_*_COUNT` 상수다. 열거값이 표 범위 밖이면 `"?"`.

### 3.8 ecl_core (CMP-CORE)

상태 `ecl_core_t`(private): `val`, `ovr`, `log`, `status`(직전 확정 결과, 초기 RELEASE/RELEASE/OFF/RC_NONE/validity 모두 VALID/override 0/rejected 0).

| 함수 | 설계 |
|---|---|
| `ecl_core_init` | 각 하위 `*_init` 호출, status 초기화 |
| `ecl_core_evaluate` | NULL→`RET_ERR_NULL`; `ecl_val_condition(&val, snap, &inputs)` 후 `run_pipeline(core, &inputs, snap->timestamp_s)` |
| `ecl_core_tick` | `ecl_val_condition_tick` 후 `run_pipeline` |
| `run_pipeline`(static) | `ecl_cmd_interpret` → `ecl_ovr_update` → `ecl_arb_decide`(prev=`status.lock`) → `ecl_sta_decide` → `build_status` → `log_if_changed` → `status`=새 상태 |
| `build_status`(static) | decision, state, inputs.validity, ovr.override, decision.command_rejected를 `ecl_status_t`로 조립 |
| `log_if_changed`(static) | lock/state/reason이 직전 status와 다르거나 `command_rejected`==1이면 `ecl_event_t`를 만들어 `ecl_log_append` |
| `ecl_core_get_status` | `*out`=`status`. NULL→`RET_ERR_NULL` |
| `ecl_core_serialize_status` | `ecl_ser_status(&core->status, ...)` |
| `ecl_core_query_events` | `ecl_log_query(&core->log, ...)` |

## 4. 오류 처리와 자원

공개 함수는 모두 NULL 인자에 `RET_ERR_NULL`. 내부 `static` 함수는 호출자가 NULL이 아님을 보장하므로 검사하지 않는다(공개 함수가 검사). 직렬화 실패만 `RET_ERR_SERIALIZE`. 동적 할당 없음. 최악 실행 시간: 상수 시간(신호 13종, 도어 2개, 규칙 6개).

## 5. 요구사항 ↔ 컴포넌트 ↔ 모듈 ↔ 함수 추적

| 요구사항 | 컴포넌트 | 모듈 | 함수 |
|---|---|---|---|
| SSR-005, 006, 032 | CMP-VAL | ecl_val | `check_bool_signals`, `check_other_signals`, `build_inputs`, `ecl_val_condition` |
| SSR-007, 008, 029 | CMP-VAL | ecl_val | `evaluate_staleness`, `ecl_val_condition_tick` |
| SSR-001(STALE 중 CONFIRMED 인정), SSR-011, SSR-020~023, 025 | CMP-VAL | ecl_val | `compute_effective`, `approach_effective`, `bool_effective`, `speed_moving_of` |
| SSR-004, 031, 034 | CMP-VAL | ecl_val | `track_pending`, `derive_hold` |
| SRQ-FUN-001, 007, 008, 031 | CMP-CMD | ecl_cmd | `ecl_cmd_interpret` |
| SRQ-FUN-017, 019, 020, 033~035, SSR-015, 035 | CMP-OVR | ecl_ovr | `update_door`, `apply_request` |
| SSR-001, 002 | CMP-ARB | ecl_arb | `ecl_arb_decide`(P1 분기) |
| SSR-003, 009(유지), 010, 016, 017, 024, 036, 037, 033 | CMP-ARB | ecl_arb | `decide_hold`, 이유코드 선택 |
| SSR-011~014, 019 | CMP-ARB | ecl_arb | `decide_rules`(P5), `decide_hold` |
| SSR-027 | CMP-ARB | ecl_arb | `ecl_arb_decide`, `decide_rules`, `merge_reason` |
| SRQ-FUN-002~006, 009~014, 016, 018, 021, 032, 038, SSR-021(ISOFIX LOCK), 013 | CMP-ARB | ecl_arb | `decide_rules`, `apply_input_rules`, `apply_driver`, `fire_reason`, `ecl_arb_decide`(P11 보조 코드) |
| SSR-009, 018, 026, 028, 030, SRQ-FUN-015 | CMP-STA | ecl_sta | `ecl_sta_decide` |
| SRQ-FUN-027, 028, 030, 037, 039, SRQ-LIM-001, 003 | CMP-LOG | ecl_log | `ecl_log_*` |
| SRQ-FUN-023, SRQ-QUA-FUN-001(형식) | CMP-SER | ecl_ser | `ecl_ser_status` |
| SRQ-FUN-021, 022, 026, 036, SRQ-QUA-REL-001, SRQ-LIM-002, 004, SRQ-CON-002, 004, 005, SSR-008(틱) | CMP-CORE | ecl_core | `ecl_core_*`, `run_pipeline` |
| SRQ-FUN-022, 029, SRQ-CON-003, 006, 007 | CMP-TYP | ecl_types.h | 구조체 정의 |

## 6. 설계상 가정 (구현에서 유지)
1. STALE일 때 효과 값은 접근위험(입력 값 그대로)을 제외하고 모두 0이며, 출력은 직전 확정 출력을 유지하되 **접근위험 TRUE 도어는 LOCK**이다(SSR-011). STALE 중 접근위험 FALSE 입력은 최신 값으로 믿을 수 없으나 새 LOCK 근거가 없으므로 직전 출력을 유지한다.
2. 틱은 새 스냅샷이 없을 때 마지막 스냅샷의 조건화 결과를 재사용하며 명령은 없다. STALE 판정은 마지막 수신 평가 이후 경과(SSR-008)이다. 스냅샷이 있는 호출도 마지막 수신 후 0.200 s를 넘겨 도착하면 STALE이며(SSR-008의 "스냅샷이 없는 호출 포함" 해석), 다음 평가(0.010 s 후)에서 복구된다.
3. 시간 비교 허용 오차 `ECL_TIME_EPS_S`=1e-9 s는 부동소수점 표현 오차 흡수용이며 경계(0.200, 1.0, 10.0)는 이 오차 안에서 포함/초과를 판정한다(SRS "시각 비교 규칙").
4. `ecl_cmd_interpret`는 `present`가 0이면 명령 없음, 1이면 필드 검사, 그 밖의 값은 형식 오류로 거절한다(DEC-013).
5. 모듈 내부 함수가 `ecl_ret_t`를 반환하고 호출자가 `RET_OK`를 확인해 전파하는 것은 방어적 설계이며, 현재 내부 호출은 NULL이 아닌 인자만 받으므로 오류 분기는 도달하지 않는다(단위 시험은 분기가 아닌 MC/DC 대상 조건으로 측정).
6. 컨텍스트 구조체의 필드는 비공개 관례이며 시험은 `ecl_core_test_get_log`(시험 빌드 전용)로만 하위 컨텍스트에 접근한다. 제품 빌드에는 시험용 접근자가 없다(INS: `ci/inspection.sh` SRQ-FUN-039).
7. `door_requested`(OVR)와 `driver_applies`(ARB)는 판정 대상이 다르다(전자는 unlock만, 후자는 lock/unlock 모두). 같은 `cmd` 구조를 읽지만 책임이 달라 공통화하지 않았다.
8. **틱에서도 PENDING 타임아웃을 판정한다**: 틱은 마지막 스냅샷의 crash_status가 계속된다고 보고 같은 `build_inputs`를 거치므로, PENDING 1.0 s 초과의 첫 판정이 틱 평가에서 일어날 수 있고 그 평가에서 `RC_CRASH_PENDING_TIMEOUT`을 기록한다(SSR-031).
9. **crash_status CONFIRMED는 freshness와 무관하게 형식이 유효하면(VALID 또는 STALE) 인정**하여 두 도어를 RELEASE한다(요구사항 트랙 DEC-011 (1), SSR-001). 다른 신호의 오래됨이 충돌 해제를 막지 않는다.
10. **첫 수신 전 틱**: 입력이 한 번도 없었으므로 `now_s − 0.0 > 0.200 s`이면 STALE이고 13개 신호를 모두 STALE로 보고한다(수신한 값이 없는 상태에서 "유효였던 신호"가 없으므로 SRS 정의의 예외이다. SRS 보완 필요 시 `OI-004`와 함께 처리).
11. **틱 STALE 기준의 합집합**: 틱은 수신 당시의 지연(timestamp_s − source_timestamp_s)을 유지한 채 마지막 수신 후 경과를 본다. 따라서 수신 시점에 이미 STALE이던 스냅샷은 다음 틱에서도 STALE이고, 마지막 수신 후 0.200 s를 넘기면 STALE이다(두 기준의 합집합). SSR-008 본문에 이 합집합을 반영할지는 사용자 결정 대기이다(요구사항 트랙 DEC-011 (5)).
12. 프로덕션 코드의 `(void)memset/memcpy/strcpy/strcat`은 표준 라이브러리가 돌려주는 포인터를 쓰지 않아서이며 반환값으로 판단할 정보가 없다. `ecl_ser.c`의 `strcpy/strcat`은 `key[ECL_SER_KEY_MAX]`(40)에 접두어 9자와 신호 이름(최대 13자)만 쓰므로 길이가 컴파일 시점에 22자 이하로 정해져 있다.
