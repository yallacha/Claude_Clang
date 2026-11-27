/**
 * @file    ecl_val.c
 * @brief   입력 조건화 컴포넌트(CMP-VAL) 구현: 형식·범위·freshness 검사, 안전 측 대체, PENDING 추적.
 * @tracing SSR-005, SSR-006, SSR-007, SSR-008, SSR-020, SSR-021, SSR-022, SSR-023, SSR-025, SSR-029, SSR-032, SSR-004, SSR-031
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#include "ecl_val.h"

#include <math.h>
#include <string.h>

/** boolean 입력 9종의 신호 인덱스 (check_bool_signals 순서) */
static const ecl_signal_t BOOL_SIGS[9] = {
    ECL_SIG_APPROACH_L, ECL_SIG_APPROACH_R, ECL_SIG_FIRE, ECL_SIG_OVERTEMP, ECL_SIG_ADULT,
    ECL_SIG_ISOFIX_L, ECL_SIG_ISOFIX_R, ECL_SIG_IGNITION, ECL_SIG_FAULT
};

/**
 * @brief  v가 boolean(0 또는 1)인지 판정한다.
 * @tracing SSR-005
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
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
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
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
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
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
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
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
 * @brief  boolean 입력 9종의 형식을 검사하여 validity를 채운다.
 * @tracing SSR-005, SSR-006
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int check_bool_signals(const ecl_snapshot_t *snap, ecl_validity_t validity[ECL_SIG_COUNT])
{
    const int values[9] = {
        snap->rear_left_approach_risk, snap->rear_right_approach_risk, snap->fire_detected,
        snap->overtemperature_detected, snap->adult_present, snap->isofix_left, snap->isofix_right,
        snap->ignition_on, snap->sensor_fault
    };
    int i = 0;

    for (i = 0; i < 9; i++) {
        validity[BOOL_SIGS[i]] = (is_bool(values[i]) != 0) ? ECL_VALID : ECL_INVALID;
    }

    return 0;
}

/**
 * @brief  속도, 기어, source_timestamp_s, crash_status의 형식·범위를 검사한다.
 * @tracing SSR-005, SSR-006
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int check_other_signals(const ecl_snapshot_t *snap, ecl_validity_t validity[ECL_SIG_COUNT])
{
    validity[ECL_SIG_SPEED] = (speed_ok(snap->vehicle_speed_kph) != 0) ? ECL_VALID : ECL_INVALID;
    validity[ECL_SIG_GEAR] = (in_enum(snap->gear, (int)ECL_GEAR_P, (int)ECL_GEAR_R) != 0) ? ECL_VALID : ECL_INVALID;
    validity[ECL_SIG_SRC_TS] = (src_ts_ok(snap->source_timestamp_s, snap->timestamp_s) != 0) ? ECL_VALID : ECL_INVALID;
    validity[ECL_SIG_CRASH] =
        (in_enum(snap->crash_status, (int)ECL_CRASH_NONE, (int)ECL_CRASH_CONFIRMED) != 0) ? ECL_VALID : ECL_INVALID;

    return 0;
}

/**
 * @brief  STALE 여부를 판정한다: source_timestamp_s INVALID(SSR-029) 또는 timestamp_s − source_timestamp_s가 0.200 s 초과.
 * 틱은 마지막 스냅샷의 source_timestamp_s를 재사용하므로 같은 규칙이 "마지막 수신 후 경과 0.200 s 초과"(SSR-008)를 포함한다
 * (source_timestamp_s <= 수신 시각이므로 경과 기준이 더 늦게 성립하는 일이 없다).
 * @tracing SSR-007, SSR-008, SSR-029
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int judge_stale(const ecl_snapshot_t *snap, const ecl_validity_t validity[ECL_SIG_COUNT])
{
    int stale = 0;
    double limit = ECL_STALE_LIMIT_S + ECL_TIME_EPS_S;

    if (validity[ECL_SIG_SRC_TS] == ECL_INVALID) {
        stale = 1;
    } else if ((snap->timestamp_s - snap->source_timestamp_s) > limit) {
        stale = 1;
    } else {
        stale = 0;
    }

    return stale;
}

/**
 * @brief  STALE이면 VALID였던 신호를 모두 STALE로 바꾼다.
 * @tracing SSR-007, SSR-008
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int mark_stale(ecl_validity_t validity[ECL_SIG_COUNT])
{
    int i = 0;

    for (i = 0; i < ECL_SIG_COUNT; i++) {
        if (validity[i] == ECL_VALID) {
            validity[i] = ECL_STALE;
        }
    }

    return 0;
}

/**
 * @brief  boolean 입력의 효과 값: VALID는 값, INVALID는 invalid_default, STALE은 0.
 * @tracing SSR-020, SSR-021, SSR-022, SSR-025
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int bool_effective(int value, ecl_validity_t validity, int invalid_default)
{
    int eff = 0;

    if (validity == ECL_VALID) {
        eff = value;
    } else if (validity == ECL_INVALID) {
        eff = invalid_default;
    } else {
        eff = 0;
    }

    return eff;
}

/**
 * @brief  입력별 안전 측 대체를 적용하여 효과 값을 채운다(SSR-020~023, 025).
 * @tracing SSR-020, SSR-021, SSR-022, SSR-023, SSR-025
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int apply_substitution(const ecl_snapshot_t *snap, const ecl_validity_t validity[ECL_SIG_COUNT], ecl_inputs_t *in)
{
    in->approach[ECL_DOOR_LEFT] = bool_effective(snap->rear_left_approach_risk, validity[ECL_SIG_APPROACH_L], 1);
    in->approach[ECL_DOOR_RIGHT] = bool_effective(snap->rear_right_approach_risk, validity[ECL_SIG_APPROACH_R], 1);
    in->isofix[ECL_DOOR_LEFT] = bool_effective(snap->isofix_left, validity[ECL_SIG_ISOFIX_L], 1);
    in->isofix[ECL_DOOR_RIGHT] = bool_effective(snap->isofix_right, validity[ECL_SIG_ISOFIX_R], 1);
    in->sensor_fault = bool_effective(snap->sensor_fault, validity[ECL_SIG_FAULT], 1);
    in->fire = bool_effective(snap->fire_detected, validity[ECL_SIG_FIRE], 0);
    in->overtemp = bool_effective(snap->overtemperature_detected, validity[ECL_SIG_OVERTEMP], 0);
    in->adult = bool_effective(snap->adult_present, validity[ECL_SIG_ADULT], 0);
    in->ignition_off = ((validity[ECL_SIG_IGNITION] == ECL_VALID) && (snap->ignition_on == 0)) ? 1 : 0;
    in->crash_confirmed =
        ((validity[ECL_SIG_CRASH] == ECL_VALID) && (snap->crash_status == (int)ECL_CRASH_CONFIRMED)) ? 1 : 0;
    if (validity[ECL_SIG_SPEED] == ECL_INVALID) {
        in->speed_moving = 1;
    } else {
        in->speed_moving =
            ((validity[ECL_SIG_SPEED] == ECL_VALID) && (snap->vehicle_speed_kph >= ECL_SPEED_MOVING_KPH)) ? 1 : 0;
    }

    return 0;
}

/**
 * @brief  INVALID 대체가 있는지(SSR-026, 기어 제외) 판정한다.
 * @tracing SSR-026
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int has_substitution(const ecl_validity_t validity[ECL_SIG_COUNT])
{
    int found = 0;
    int i = 0;

    for (i = 0; i < ECL_SIG_COUNT; i++) {
        if ((validity[i] == ECL_INVALID) && (i != (int)ECL_SIG_GEAR)) {
            found = 1;
        }
    }

    return found;
}

/**
 * @brief  PENDING 지속 시간을 추적하고 유지 여부와 타임아웃 첫 판정을 계산한다.
 * @tracing SSR-003, SSR-004, SSR-031
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int track_pending(ecl_val_t *val, const ecl_snapshot_t *snap, ecl_validity_t crash_validity, ecl_inputs_t *in,
                         int *pending_hold)
{
    int is_pending = ((crash_validity == ECL_VALID) && (snap->crash_status == (int)ECL_CRASH_PENDING)) ? 1 : 0;

    *pending_hold = 0;
    if (is_pending != 0) {
        if (val->pending_active == 0) {
            val->pending_active = 1;
            val->pending_start_s = snap->timestamp_s;
            val->pending_reported = 0;
        }
        const double elapsed = snap->timestamp_s - val->pending_start_s;
        if (elapsed <= (ECL_PENDING_LIMIT_S + ECL_TIME_EPS_S)) {
            *pending_hold = 1;
        } else if (val->pending_reported == 0) {
            val->pending_reported = 1;
            in->pending_timeout_first = 1;
        }
    } else if (crash_validity == ECL_VALID) {
        val->pending_active = 0;
        val->pending_reported = 0;
    }

    return 0;
}

/**
 * @brief  P2~P4 유지 조건의 이유코드를 결정한다(sensor_fault > PENDING > STALE > crash/ignition INVALID).
 * @tracing SSR-027, SSR-024
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
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
    } else {
        hold = RC_NONE;
    }

    return hold;
}

/**
 * @brief  스냅샷으로부터 조건화된 입력을 만든다(상태 갱신 없음 제외: PENDING 추적만 갱신).
 * @tracing SSR-005, SSR-006, SSR-007, SSR-008, SSR-029, SSR-032
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int build_inputs(ecl_val_t *val, const ecl_snapshot_t *snap, ecl_inputs_t *out)
{
    int pending_hold = 0;

    (void)memset(out, 0, sizeof(*out));
    (void)check_bool_signals(snap, out->validity);
    (void)check_other_signals(snap, out->validity);
    out->stale = judge_stale(snap, out->validity);
    out->substituted = has_substitution(out->validity);
    if (out->stale != 0) {
        (void)mark_stale(out->validity);
    }
    (void)apply_substitution(snap, out->validity, out);
    (void)track_pending(val, snap, out->validity[ECL_SIG_CRASH], out, &pending_hold);
    out->hold_reason = derive_hold(out, pending_hold);
    out->command = snap->command;

    return 0;
}

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

ecl_ret_t ecl_val_condition(ecl_val_t *val, const ecl_snapshot_t *snap, ecl_inputs_t *out)
{
    ecl_ret_t ret = RET_OK;

    if ((val == NULL) || (snap == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        (void)build_inputs(val, snap, out);
        val->rx_valid = 1;
        val->last_rx_s = snap->timestamp_s;
        val->last_snap = *snap;
        val->last_snap.command.present = 0;
    }

    return ret;
}

ecl_ret_t ecl_val_condition_tick(ecl_val_t *val, double now_s, ecl_inputs_t *out)
{
    ecl_ret_t ret = RET_OK;
    ecl_snapshot_t snap;

    if ((val == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else if (val->rx_valid != 0) {
        snap = val->last_snap;
        snap.timestamp_s = now_s;
        (void)build_inputs(val, &snap, out);
    } else {
        (void)memset(out, 0, sizeof(*out));
        out->stale = ((now_s - val->last_rx_s) > (ECL_STALE_LIMIT_S + ECL_TIME_EPS_S)) ? 1 : 0;
        out->hold_reason = (out->stale != 0) ? RC_INPUT_STALE : RC_NONE;
        if (out->stale != 0) {
            (void)mark_stale(out->validity);
        }
    }

    return ret;
}
