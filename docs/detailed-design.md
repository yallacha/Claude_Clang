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

내부 상태 `ecl_val_t`(private): `rx_valid`, `last_rx_s`, `pending_active`, `pending_start_s`, `pending_reported`, `last_inputs`(틱용, `ecl_inputs_t`).

| 함수 | 종류 | 설계 |
|---|---|---|
| `ecl_val_init` | 공개 | 모든 상태를 0으로, `last_rx_s`=0.0, `rx_valid`=0, `last_inputs`를 "입력 없음"(모든 효과값 0, validity VALID, hold_reason RC_NONE)으로 설정. NULL→`RET_ERR_NULL` |
| `ecl_val_condition` | 공개 | `check_signals` → `judge_stale` → `apply_substitution` → `track_pending` → `derive_hold` → 상태 갱신(`last_rx_s`=시각, `rx_valid`=1, `last_inputs`=결과의 명령 제거 복사) |
| `ecl_val_condition_tick` | 공개 | `rx_valid`가 0이면 `now_s − 0.0`, 아니면 `now_s − last_rx_s`가 0.200 초과일 때만 `stale`로 판정하고, 이전 `last_inputs`에 `stale`/hold만 갱신하여 반환(명령 없음). `last_rx_s`는 바꾸지 않음 |
| `check_signals` | static | 신호별 규칙(SSR-005 표)을 적용하여 `validity[]`(VALID/INVALID)를 채운다. 헬퍼: `is_bool(v)`(v==0\|\|v==1), `in_enum(v,lo,hi)`, `speed_ok(v)`(!isnan && 0.0≤v≤300.0), `src_ts_ok(src, now)`(유한 && 0≤src≤now). 신호 13종을 `validate_bool_signals`(booleans 9종)와 `validate_other_signals`(speed, gear, crash, src_ts)로 나누어 CCN을 낮춘다 |
| `judge_stale` | static | `stale = (src_ts INVALID(SSR-029)) \|\| (now − src_ts > 0.200+eps) \|\| (rx_valid && now − last_rx_s > 0.200+eps)`. STALE이면 VALID였던 신호를 STALE로 바꾼다(INVALID는 유지) |
| `apply_substitution` | static | SSR-020~025: approach/isofix/sensor_fault INVALID→1, speed INVALID→`speed_moving`=1, fire/overtemp/adult INVALID→0, crash/ignition INVALID은 별도 `hold_invalid` 플래그(내부)로 표시. VALID 신호만 값을 효과값으로 복사하고 STALE 신호의 효과값은 0(미사용). `speed_moving` = 속도 VALID && v ≥ 3.0 \|\| 속도 INVALID. `ignition_off` = ignition VALID && ==0. `crash_confirmed` = crash VALID && CONFIRMED. `substituted` = INVALID 신호 존재 |
| `track_pending` | static | crash가 VALID이고 PENDING이면: 시작 안 됐으면 `pending_start_s`=now, `pending_active`=1, `pending_reported`=0. `elapsed = now − pending_start_s`. `pending_hold` = elapsed ≤ 1.0+eps. 초과 && !`pending_reported`이면 `pending_timeout_first`=1, `pending_reported`=1. crash가 VALID이고 PENDING이 아니면 시작 상태 초기화. INVALID/STALE이면 유지 |
| `derive_hold` | static | `hold_reason` 결정: sensor_fault → `RC_SENSOR_FAULT`; 아니면 pending_hold → `RC_CRASH_PENDING_HOLD`; 아니면 stale → `RC_INPUT_STALE`; 아니면 hold_invalid → `RC_INPUT_INVALID`; 그 외 `RC_NONE`. 단 crash_confirmed이면 hold_reason은 계산하되 ARB가 P1을 먼저 적용한다 |

### 3.2 ecl_cmd (CMP-CMD)

| 함수 | 설계 |
|---|---|
| `ecl_cmd_interpret`(공개) | `present`=raw->present(0/1만 인정, 그 외 0). present이면 `side∈[LEFT..ALL]`, `action∈[LOCK_CMD..UNLOCK_CMD]`, `source∈[PHYSICAL_BUTTON..MOBILE_APP]`이고 `ECL_ABSENT`가 아니면 `valid`=1, 아니면 `rejected`=1. NULL→`RET_ERR_NULL` |
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
| `ecl_arb_decide`(공개) | NULL 검사 → `crash_confirmed`이면 `decide_crash`; 아니면 도어별 `decide_door`(0, 1) → `merge_reason` → 거절 보조 코드 `apply_reject` |
| `decide_crash`(static) | lock[0..1]=RELEASE, reason=`RC_CRASH_CONFIRMED`, rejected=cmd.rejected |
| `decide_door`(static) | `hold_reason != RC_NONE`이면 `decide_hold`, 아니면 `decide_rules` |
| `decide_hold`(static) | `approach[d]` → LOCK이고 reason=hold_reason; 그 외 lock=prev[d], reason=hold_reason (SSR-011, 003, 010, 017, 024) |
| `decide_rules`(static) | P5~P10 순차 판정(첫 번째 성립 규칙이 선택). P5: approach → LOCK, reason = `ovr->ack[d]`? `RC_OVERRIDE_ACK` : `RC_APPROACH_RISK`. P6: (fire\|\|overtemp\|\|adult) → RELEASE, reason = `fire_reason`. P7: isofix → LOCK `RC_ISOFIX`. P8: speed_moving → LOCK, reason = 유효 unlock 명령이 있으면 `RC_UNLOCK_DENIED_MOVING` 아니면 `RC_SPEED_AUTO_LOCK`. P9: ignition_off → RELEASE `RC_IGNITION_OFF`. P10: `driver_applies(cmd, d)` → 명령 action에 따라 LOCK/`RC_DRIVER_LOCK` 또는 RELEASE/`RC_DRIVER_UNLOCK`. 없음: lock=prev[d], reason=`RC_NONE`. 마지막에 `pending_timeout_first`이면 reason을 `RC_CRASH_PENDING_TIMEOUT`으로 덮어쓴다(P4 > P5~P10) |
| `fire_reason`(static) | fire → `RC_FIRE`; overtemp → `RC_OVERTEMP`; adult → `RC_ADULT_PRESENT` (SRQ-FUN-032 순서) |
| `driver_applies`(static) | `cmd.valid && (side==ALL \|\| side==door)` |
| `merge_reason`(static) | 두 도어 reason 중 `P_RANK`가 작은 쪽, 같으면 enum 값이 작은 쪽 |
| `apply_reject`(static) | `cmd.rejected`이면 `command_rejected`=1이고 reason이 `RC_NONE`일 때 `RC_COMMAND_REJECTED` |
| `reason_rank`(static) | `P_RANK[reason]`(범위 밖 값은 99) |

`P_RANK`: CRASH_CONFIRMED=1, SENSOR_FAULT=2, CRASH_PENDING_HOLD=3, CRASH_PENDING_TIMEOUT=4, INPUT_STALE=4, INPUT_INVALID=4, APPROACH_RISK=5, OVERRIDE_ACK=5, FIRE=6, OVERTEMP=6, ADULT_PRESENT=6, ISOFIX=7, SPEED_AUTO_LOCK=8, UNLOCK_DENIED_MOVING=8, IGNITION_OFF=9, DRIVER_LOCK=10, DRIVER_UNLOCK=10, COMMAND_REJECTED=11, NONE=99.

### 3.5 ecl_sta (CMP-STA)

`ecl_sta_decide`: `sensor_fault` → FAULT; 아니면 `stale || substituted` → DEGRADED; 아니면 `ignition_off` → OFF; 아니면 NORMAL. (NULL→`RET_ERR_NULL`)

### 3.6 ecl_log (CMP-LOG)

상태 `ecl_log_t`(private): `events[100]`, `head`(가장 오래된 인덱스), `count`, `next_id`(uint64), `exhausted`.
- `ecl_log_init`: head=count=0, next_id=1, exhausted=0.
- `ecl_log_append`: exhausted이면 `*assigned_id`=0. 아니면 id=next_id, 이벤트 저장(`store_event`: count<100이면 (head+count)%100에 추가, 가득 차면 head 위치를 덮어쓰고 head 전진), id==UINT64_MAX이면 exhausted=1 아니면 next_id++, `*assigned_id`=id.
- `ecl_log_query`: n = min(count, capacity). 최신 n건을 오름차순으로 복사(시작 인덱스 = (head + count − n) % 100), `*count`=n.
- `ecl_log_test_set_counter`(시험 빌드 `ECL_TEST_BUILD`에서만 컴파일): value==UINT64_MAX이면 exhausted=1, 아니면 next_id=value+1, exhausted=0.

### 3.7 ecl_ser (CMP-SER)

고정 순서 문자열 생성(SAD §4.8). 지역 버퍼 `tmp[640]`에 `append_pair(tmp, &pos, name, value)`로 `name=value;`를 이어 붙이고, 전체 길이+1이 `capacity` 이하이면 `memcpy`하고 `*length`=길이, 아니면 `RET_ERR_SERIALIZE`(`buf` 불변). 필드: `lock_left`, `lock_right`, `state`, `reason_code`, `override_left`, `override_right`, `command_rejected`, `validity_speed`, `validity_gear`, `validity_source_ts`, `validity_crash`, `validity_approach_left`, `validity_approach_right`, `validity_fire`, `validity_overtemp`, `validity_adult`, `validity_isofix_left`, `validity_isofix_right`, `validity_ignition`, `validity_sensor_fault`. 문자열 표는 `static const char *const`. 열거값이 표 범위 밖이면 `"?"`.

### 3.8 ecl_core (CMP-CORE)

상태 `ecl_core_t`(private): `val`, `ovr`, `log`, `status`(직전 확정 결과, 초기 RELEASE/RELEASE/OFF/RC_NONE/validity 모두 VALID/override 0/rejected 0).

| 함수 | 설계 |
|---|---|
| `ecl_core_init` | 각 하위 `*_init` 호출, status 초기화 |
| `ecl_core_evaluate` | NULL→`RET_ERR_NULL`; `ecl_val_condition(&val, snap, snap->timestamp_s, &inputs)` 후 `run_pipeline(core, &inputs, snap->timestamp_s)` |
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
| SSR-005, 006, 032 | CMP-VAL | ecl_val | `check_signals`, `ecl_val_condition` |
| SSR-007, 008, 029 | CMP-VAL | ecl_val | `judge_stale`, `ecl_val_condition_tick` |
| SSR-020~023, 025 | CMP-VAL | ecl_val | `apply_substitution` |
| SSR-004, 031, 034 | CMP-VAL | ecl_val | `track_pending`, `derive_hold` |
| SRQ-FUN-001, 007, 008, 031 | CMP-CMD | ecl_cmd | `ecl_cmd_interpret` |
| SRQ-FUN-017, 019, 020, 033~035, SSR-015, 035 | CMP-OVR | ecl_ovr | `update_door`, `apply_request` |
| SSR-001, 002 | CMP-ARB | ecl_arb | `decide_crash` |
| SSR-003, 009(유지), 010, 016, 017, 024, 036, 037, 033 | CMP-ARB | ecl_arb | `decide_hold`, 이유코드 선택 |
| SSR-011~014, 019 | CMP-ARB | ecl_arb | `decide_rules`(P5), `decide_hold` |
| SSR-027 | CMP-ARB | ecl_arb | `ecl_arb_decide`, `decide_rules`, `merge_reason` |
| SRQ-FUN-002~006, 009~014, 016, 018, 021, 032, 038, SSR-021(ISOFIX LOCK), 013 | CMP-ARB | ecl_arb | `decide_rules`, `fire_reason`, `apply_reject` |
| SSR-009, 018, 026, 028, 030, SRQ-FUN-015 | CMP-STA | ecl_sta | `ecl_sta_decide` |
| SRQ-FUN-027, 028, 030, 037, 039, SRQ-LIM-001, 003 | CMP-LOG | ecl_log | `ecl_log_*` |
| SRQ-FUN-023, SRQ-QUA-FUN-001(형식) | CMP-SER | ecl_ser | `ecl_ser_status` |
| SRQ-FUN-021, 022, 026, 036, SRQ-QUA-REL-001, SRQ-LIM-002, 004, SRQ-CON-002, 004, 005, SSR-008(틱) | CMP-CORE | ecl_core | `ecl_core_*`, `run_pipeline` |
| SRQ-FUN-022, 029, SRQ-CON-003, 006, 007 | CMP-TYP | ecl_types.h | 구조체 정의 |

## 6. 설계상 가정 (구현에서 유지)
1. STALE일 때 입력 효과값은 모두 0이며 직전 출력(접근위험 도어는 직전 값)을 유지한다. STALE 중에는 접근위험 TRUE 도어 LOCK을 새로 만들 수 없다(직전 출력이 LOCK이면 유지). 이 한계는 `OI-004`, `OI-013`에 포함된다.
2. 틱 호출은 새 스냅샷이 없으면 마지막 스냅샷의 조건화 결과를 재사용하며 명령은 없다.
3. 시간 비교 허용 오차 `ECL_TIME_EPS_S`=1e-9 s는 부동소수점 표현 오차 흡수용이며 요구사항 경계(0.200, 1.0, 10.0)는 이 오차 안에서 포함/초과를 판정한다.
