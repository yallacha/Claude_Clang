/**
 * @file    ecl_val.c
 * @brief   입력 조건화 컴포넌트(CMP-VAL) 구현: 형식·범위·freshness 검사, 안전 측 대체, PENDING 추적.
 * @tracing SSR-005, SSR-006, SSR-007, SSR-008, SSR-020, SSR-021, SSR-022, SSR-023, SSR-025, SSR-029, SSR-032, SSR-004, SSR-031
 *
 * @@HITL@@
 */
#include "ecl_val.h"

#include <math.h>
#include <string.h>

/** boolean 입력 9종의 신호 인덱스 (check_bool_signals 순서) */
static const ecl_signal_t BOOL_SIGS[ECL_BOOL_SIG_COUNT] = {
    ECL_SIG_APPROACH_L, ECL_SIG_APPROACH_R, ECL_SIG_FIRE, ECL_SIG_OVERTEMP, ECL_SIG_ADULT,
    ECL_SIG_ISOFIX_L, ECL_SIG_ISOFIX_R, ECL_SIG_IGNITION, ECL_SIG_FAULT
};

/** 안전 측 대체를 적용한 효과 값 묶음 */
typedef struct {
    int approach[ECL_DOOR_COUNT];
    int isofix[ECL_DOOR_COUNT];
    int sensor_fault;
    int fire;
    int overtemp;
    int adult;
    int ignition_off;
    int crash_confirmed;
    int speed_moving;
} effective_t;

/**
 * @brief  v가 boolean(0 또는 1)인지 판정한다.
 * @tracing SSR-005
 * @@HITL@@
 */
static int is_bool(int v)
{
    int ok = 0;

    if ((v == 0) || (v == 1)) {
        ok = 1;
    }

    return ok;
}

/**
 * @brief  v가 [lo, hi] 열거 범위인지 판정한다(누락 센티널은 범위 밖).
 * @tracing SSR-005
 * @@HITL@@
 */
static int in_enum(int v, int lo, int hi)
{
    int ok = 0;

    if ((v >= lo) && (v <= hi)) {
        ok = 1;
    }

    return ok;
}

/**
 * @brief  속도가 유한하고 0.0 이상 300.0 이하인지 판정한다.
 * @tracing SSR-005
 * @@HITL@@
 */
static int speed_ok(double v)
{
    int ok = 0;

    if ((isfinite(v) != 0) && (v >= 0.0) && (v <= ECL_SPEED_MAX_KPH)) {
        ok = 1;
    }

    return ok;
}

/**
 * @brief  source_timestamp_s가 유한하고 0 이상 평가 시각 이하인지 판정한다.
 * @tracing SSR-005
 * @@HITL@@
 */
static int src_ts_ok(double src, double now)
{
    int ok = 0;

    if ((isfinite(src) != 0) && (src >= 0.0) && (src <= (now + ECL_TIME_EPS_S))) {
        ok = 1;
    }

    return ok;
}

/**
 * @brief  boolean 입력 9종의 형식을 검사하여 validity를 채우고 INVALID 개수를 반환한다.
 * @tracing SSR-005, SSR-006
 * @@HITL@@
 */
static int check_bool_signals(const ecl_snapshot_t *snap, ecl_validity_t validity[ECL_SIG_COUNT])
{
    const int values[ECL_BOOL_SIG_COUNT] = {
        snap->rear_left_approach_risk, snap->rear_right_approach_risk, snap->fire_detected,
        snap->overtemperature_detected, snap->adult_present, snap->isofix_left, snap->isofix_right,
        snap->ignition_on, snap->sensor_fault
    };
    int invalid_count = 0;

    for (int i = 0; i < ECL_BOOL_SIG_COUNT; i++) {
        if (is_bool(values[i]) != 0) {
            validity[BOOL_SIGS[i]] = ECL_VALID;
        } else {
            validity[BOOL_SIGS[i]] = ECL_INVALID;
            invalid_count++;
        }
    }

    return invalid_count;
}

/**
 * @brief  속도, 기어, source_timestamp_s, crash_status의 형식·범위를 검사한다.
 *         대체 대상이 아닌 기어(SSR-026)를 제외한 INVALID 개수를 반환한다.
 * @tracing SSR-005, SSR-006
 * @@HITL@@
 */
static int check_other_signals(const ecl_snapshot_t *snap, ecl_validity_t validity[ECL_SIG_COUNT])
{
    validity[ECL_SIG_SPEED] = (speed_ok(snap->vehicle_speed_kph) != 0) ? ECL_VALID : ECL_INVALID;
    validity[ECL_SIG_GEAR] = (in_enum(snap->gear, (int)ECL_GEAR_P, (int)ECL_GEAR_R) != 0) ? ECL_VALID : ECL_INVALID;
    validity[ECL_SIG_SRC_TS] = (src_ts_ok(snap->source_timestamp_s, snap->timestamp_s) != 0) ? ECL_VALID : ECL_INVALID;
    validity[ECL_SIG_CRASH] =
        (in_enum(snap->crash_status, (int)ECL_CRASH_NONE, (int)ECL_CRASH_CONFIRMED) != 0) ? ECL_VALID : ECL_INVALID;

    return ((validity[ECL_SIG_SPEED] == ECL_INVALID) ? 1 : 0) + ((validity[ECL_SIG_SRC_TS] == ECL_INVALID) ? 1 : 0) +
           ((validity[ECL_SIG_CRASH] == ECL_INVALID) ? 1 : 0);
}

/**
 * @brief  STALE 여부를 판정하고 STALE이면 VALID였던 신호를 STALE로 바꾼다.
 *         STALE = source_timestamp_s INVALID(SSR-029), timestamp_s − source_timestamp_s > 0.200 s(SSR-007),
 *         또는 마지막 수신 평가 이후 경과가 0.200 s 초과(SSR-008, 스냅샷이 없는 호출 포함).
 * @tracing SSR-007, SSR-008, SSR-029
 * @@HITL@@
 */
static int evaluate_staleness(const ecl_val_t *val, const ecl_snapshot_t *snap, ecl_validity_t validity[ECL_SIG_COUNT])
{
    int stale = 0;
    const double limit = ECL_STALE_LIMIT_S + ECL_TIME_EPS_S;

    if (validity[ECL_SIG_SRC_TS] == ECL_INVALID) {
        stale = 1;
    } else if ((snap->timestamp_s - snap->source_timestamp_s) > limit) {
        stale = 1;
    } else if ((val->rx_valid != 0) && ((snap->timestamp_s - val->last_rx_s) > limit)) {
        stale = 1;
    }
    if (stale != 0) {
        for (int i = 0; i < ECL_SIG_COUNT; i++) {
            if (validity[i] == ECL_VALID) {
                validity[i] = ECL_STALE;
            }
        }
    }

    return stale;
}

/**
 * @brief  boolean 입력의 효과 값: VALID는 값, INVALID는 invalid_default, STALE은 0.
 * @tracing SSR-020, SSR-021, SSR-022, SSR-025
 * @@HITL@@
 */
static int bool_effective(int value, ecl_validity_t validity, int invalid_default)
{
    int eff = ECL_FLAG_CLEAR;

    if (validity == ECL_VALID) {
        eff = value;
    } else if (validity == ECL_INVALID) {
        eff = invalid_default;
    }

    return eff;
}

/**
 * @brief  접근위험 입력의 효과 값: VALID와 STALE은 입력 값 그대로(STALE이어도 TRUE이면 LOCK 유지, SSR-011), INVALID는 TRUE(SSR-020).
 * @tracing SSR-011, SSR-020
 * @@HITL@@
 */
static int approach_effective(int value, ecl_validity_t validity)
{
    int eff = value;

    if (validity == ECL_INVALID) {
        eff = ECL_FLAG_SET;
    }

    return eff;
}

/**
 * @brief  속도 입력의 효과 값: INVALID는 이동(SSR-023), VALID는 3.0 km/h 이상일 때 이동, STALE은 0.
 * @tracing SSR-023, SRQ-FUN-009
 * @@HITL@@
 */
static int speed_moving_of(const ecl_snapshot_t *snap, ecl_validity_t validity)
{
    int moving = ECL_FLAG_CLEAR;

    if (validity == ECL_INVALID) {
        moving = ECL_FLAG_SET;
    } else if ((validity == ECL_VALID) && (snap->vehicle_speed_kph >= ECL_SPEED_MOVING_KPH)) {
        moving = ECL_FLAG_SET;
    }

    return moving;
}

/**
 * @brief  입력별 안전 측 대체를 적용한 효과 값을 계산한다(SSR-020~023, 025).
 * @tracing SSR-011, SSR-020, SSR-021, SSR-022, SSR-023, SSR-025
 * @@HITL@@
 */
static effective_t compute_effective(const ecl_snapshot_t *snap, const ecl_validity_t validity[ECL_SIG_COUNT])
{
    effective_t eff;

    eff.approach[ECL_DOOR_LEFT] = approach_effective(snap->rear_left_approach_risk, validity[ECL_SIG_APPROACH_L]);
    eff.approach[ECL_DOOR_RIGHT] = approach_effective(snap->rear_right_approach_risk, validity[ECL_SIG_APPROACH_R]);
    eff.isofix[ECL_DOOR_LEFT] = bool_effective(snap->isofix_left, validity[ECL_SIG_ISOFIX_L], ECL_FLAG_SET);
    eff.isofix[ECL_DOOR_RIGHT] = bool_effective(snap->isofix_right, validity[ECL_SIG_ISOFIX_R], ECL_FLAG_SET);
    eff.sensor_fault = bool_effective(snap->sensor_fault, validity[ECL_SIG_FAULT], ECL_FLAG_SET);
    eff.fire = bool_effective(snap->fire_detected, validity[ECL_SIG_FIRE], ECL_FLAG_CLEAR);
    eff.overtemp = bool_effective(snap->overtemperature_detected, validity[ECL_SIG_OVERTEMP], ECL_FLAG_CLEAR);
    eff.adult = bool_effective(snap->adult_present, validity[ECL_SIG_ADULT], ECL_FLAG_CLEAR);
    eff.ignition_off = ((validity[ECL_SIG_IGNITION] == ECL_VALID) && (snap->ignition_on == 0)) ? ECL_FLAG_SET : ECL_FLAG_CLEAR;
    eff.crash_confirmed = ((validity[ECL_SIG_CRASH] == ECL_VALID) && (snap->crash_status == (int)ECL_CRASH_CONFIRMED))
                              ? ECL_FLAG_SET : ECL_FLAG_CLEAR;
    eff.speed_moving = speed_moving_of(snap, validity[ECL_SIG_SPEED]);

    return eff;
}

/**
 * @brief  PENDING 지속 시간을 추적하고 유지 여부(1: 유지)를 반환한다. 1.0 s 초과의 첫 판정에서는 in->pending_timeout_first를 설정한다.
 * @tracing SSR-003, SSR-004, SSR-031
 * @@HITL@@
 */
static int track_pending(ecl_val_t *val, const ecl_snapshot_t *snap, ecl_validity_t crash_validity, ecl_inputs_t *in)
{
    int pending_hold = 0;
    const int is_pending = ((crash_validity == ECL_VALID) && (snap->crash_status == (int)ECL_CRASH_PENDING)) ? 1 : 0;

    if (is_pending != 0) {
        if (val->pending_active == 0) {
            val->pending_active = 1;
            val->pending_start_s = snap->timestamp_s;
            val->pending_reported = 0;
        }
        const double elapsed = snap->timestamp_s - val->pending_start_s;
        if (elapsed <= (ECL_PENDING_LIMIT_S + ECL_TIME_EPS_S)) {
            pending_hold = 1;
        } else if (val->pending_reported == 0) {
            val->pending_reported = 1;
            in->pending_timeout_first = 1;
        }
    } else if (crash_validity == ECL_VALID) {
        val->pending_active = 0;
        val->pending_reported = 0;
    }

    return pending_hold;
}

/**
 * @brief  P2~P4 유지 조건의 이유코드를 결정한다(sensor_fault > PENDING > STALE > crash/ignition INVALID).
 * @tracing SSR-027, SSR-024
 * @@HITL@@
 */
static ecl_reason_t derive_hold(const ecl_inputs_t *in, int pending_hold)
{
    ecl_reason_t hold = RC_NONE;

    if (in->sensor_fault != 0) {
        hold = RC_SENSOR_FAULT;
    } else if (pending_hold != 0) {
        hold = RC_CRASH_PENDING_HOLD;
    } else if (in->stale != 0) {
        hold = RC_INPUT_STALE;
    } else if ((in->validity[ECL_SIG_CRASH] == ECL_INVALID) || (in->validity[ECL_SIG_IGNITION] == ECL_INVALID)) {
        hold = RC_INPUT_INVALID;
    }

    return hold;
}

/**
 * @brief  스냅샷으로부터 조건화된 입력을 만들어 반환한다(PENDING 추적 상태만 갱신한다).
 * @tracing SSR-005, SSR-006, SSR-007, SSR-008, SSR-029, SSR-032
 * @@HITL@@
 */
static ecl_inputs_t build_inputs(ecl_val_t *val, const ecl_snapshot_t *snap)
{
    ecl_inputs_t out;
    int pending_hold = 0;
    effective_t eff;

    (void)memset(&out, 0, sizeof(out));
    out.substituted = ((check_bool_signals(snap, out.validity) + check_other_signals(snap, out.validity)) > 0) ? 1 : 0;
    out.stale = evaluate_staleness(val, snap, out.validity);
    eff = compute_effective(snap, out.validity);
    (void)memcpy(out.approach, eff.approach, sizeof(out.approach));
    (void)memcpy(out.isofix, eff.isofix, sizeof(out.isofix));
    out.sensor_fault = eff.sensor_fault;
    out.fire = eff.fire;
    out.overtemp = eff.overtemp;
    out.adult = eff.adult;
    out.ignition_off = eff.ignition_off;
    out.crash_confirmed = eff.crash_confirmed;
    out.speed_moving = eff.speed_moving;
    pending_hold = track_pending(val, snap, out.validity[ECL_SIG_CRASH], &out);
    out.hold_reason = derive_hold(&out, pending_hold);
    out.command = snap->command;

    return out;
}

/**
 * @brief  컨텍스트를 초기화한다.
 * @tracing SSR-007
 * @@HITL@@
 */
ecl_ret_t ecl_val_init(ecl_val_t *val)
{
    ecl_ret_t ret = RET_OK;

    if (val == NULL) {
        ret = RET_ERR_NULL;
    } else {
        (void)memset(val, 0, sizeof(*val));
    }

    return ret;
}

/**
 * @brief  스냅샷을 검사·대체하여 조건화된 입력을 만든다. 평가 시각은 snap->timestamp_s이다.
 * @tracing SSR-005, SSR-006, SSR-007, SSR-008, SSR-020, SSR-021, SSR-022, SSR-023, SSR-025, SSR-029, SSR-032, SSR-004, SSR-031
 * @@HITL@@
 */
ecl_ret_t ecl_val_condition(ecl_val_t *val, const ecl_snapshot_t *snap, ecl_inputs_t *out)
{
    ecl_ret_t ret = RET_OK;

    if ((val == NULL) || (snap == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        *out = build_inputs(val, snap);
        val->rx_valid = 1;
        val->last_rx_s = snap->timestamp_s;
        val->last_snap = *snap;
        val->last_snap.command.present = 0;
    }

    return ret;
}

/**
 * @brief  스냅샷 없는 평가(틱)의 조건화된 입력을 만든다. 마지막 스냅샷을 수신 당시의 지연(timestamp_s − source_timestamp_s)을
 *         유지한 채 재사용하며, 마지막 수신 후 경과 0.200 s 초과이면 STALE이다(SSR-008).
 * @tracing SSR-008
 * @@HITL@@
 */
ecl_ret_t ecl_val_condition_tick(ecl_val_t *val, double now_s, ecl_inputs_t *out)
{
    ecl_ret_t ret = RET_OK;

    if ((val == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else if (val->rx_valid != 0) {
        ecl_snapshot_t snap = val->last_snap;
        snap.source_timestamp_s = now_s - (val->last_snap.timestamp_s - val->last_snap.source_timestamp_s);
        snap.timestamp_s = now_s;
        *out = build_inputs(val, &snap);
    } else {
        (void)memset(out, 0, sizeof(*out));
        out->stale = ((now_s - val->last_rx_s) > (ECL_STALE_LIMIT_S + ECL_TIME_EPS_S)) ? 1 : 0;
        out->hold_reason = (out->stale != 0) ? RC_INPUT_STALE : RC_NONE;
        if (out->stale != 0) {
            for (int i = 0; i < ECL_SIG_COUNT; i++) {
                out->validity[i] = ECL_STALE;
            }
        }
    }

    return ret;
}
