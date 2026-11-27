/**
 * @file    ecl_arb.c
 * @brief   우선순위 중재 컴포넌트(CMP-ARB) 구현 (SSR-027 우선순위 표 P1~P11).
 * @tracing SSR-027, SSR-001, SSR-002, SSR-003, SSR-010, SSR-011, SSR-012, SSR-013, SSR-014, SSR-016, SSR-024
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#include "ecl_arb.h"

/** 이유코드별 우선순위 순위(작을수록 높음). 인덱스는 ecl_reason_t 값(SRS 부록 A). */
static const int P_RANK[ECL_REASON_COUNT] = {
    ECL_RANK_NONE,  /* RC_NONE */
    ECL_RANK_P1,    /* RC_CRASH_CONFIRMED */
    ECL_RANK_P2,    /* RC_SENSOR_FAULT */
    ECL_RANK_P3,    /* RC_CRASH_PENDING_HOLD */
    ECL_RANK_P4,    /* RC_CRASH_PENDING_TIMEOUT */
    ECL_RANK_P4,    /* RC_INPUT_STALE */
    ECL_RANK_P4,    /* RC_INPUT_INVALID */
    ECL_RANK_P6,    /* RC_FIRE */
    ECL_RANK_P6,    /* RC_OVERTEMP */
    ECL_RANK_P6,    /* RC_ADULT_PRESENT */
    ECL_RANK_P5,    /* RC_APPROACH_RISK */
    ECL_RANK_P5,    /* RC_OVERRIDE_ACK */
    ECL_RANK_P7,    /* RC_ISOFIX */
    ECL_RANK_P9,    /* RC_IGNITION_OFF */
    ECL_RANK_P8,    /* RC_SPEED_AUTO_LOCK */
    ECL_RANK_P8,    /* RC_UNLOCK_DENIED_MOVING */
    ECL_RANK_P10,   /* RC_DRIVER_LOCK */
    ECL_RANK_P10,   /* RC_DRIVER_UNLOCK */
    ECL_RANK_P11    /* RC_COMMAND_REJECTED */
};

/** 한 도어의 결정 */
typedef struct {
    ecl_lock_t lock;
    ecl_reason_t reason;
} door_result_t;

/**
 * @brief  이유코드의 우선순위 순위를 반환한다(내부 호출만 있어 reason은 항상 유효 열거값).
 * @tracing SSR-027
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int reason_rank(ecl_reason_t reason)
{
    return P_RANK[(int)reason];
}

/**
 * @brief  화재·과온·성인 입력의 이유코드를 fire, overtemperature, adult 순서로 고른다.
 * @tracing SRQ-FUN-011, SRQ-FUN-032
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static ecl_reason_t fire_reason(const ecl_inputs_t *in)
{
    ecl_reason_t reason = RC_ADULT_PRESENT;

    if (in->fire != 0) {
        reason = RC_FIRE;
    } else if (in->overtemp != 0) {
        reason = RC_OVERTEMP;
    }

    return reason;
}

/**
 * @brief  운전자 명령이 해당 도어에 적용되는지 판정한다(lock/unlock 모두).
 * @tracing SRQ-FUN-002, SRQ-FUN-003, SRQ-FUN-006
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int driver_applies(const ecl_cmd_result_t *cmd, int door)
{
    int applies = 0;

    if ((cmd->valid != 0) && ((cmd->side == ECL_SIDE_ALL) || ((int)cmd->side == door))) {
        applies = 1;
    }

    return applies;
}

/**
 * @brief  P8: 이동 중 LOCK의 이유코드(유효 unlock 명령이 있으면 거부 코드)를 고른다.
 * @tracing SRQ-FUN-004, SRQ-FUN-005, SRQ-FUN-009
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static ecl_reason_t speed_reason(const ecl_cmd_result_t *cmd)
{
    ecl_reason_t reason = RC_SPEED_AUTO_LOCK;

    if ((cmd->valid != 0) && (cmd->action == ECL_ACTION_UNLOCK)) {
        reason = RC_UNLOCK_DENIED_MOVING;
    }

    return reason;
}

/**
 * @brief  P10: 운전자 명령이 적용되면 결과를 설정하고 1을 반환한다.
 * @tracing SRQ-FUN-002, SRQ-FUN-003
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int apply_driver(const ecl_cmd_result_t *cmd, int d, door_result_t *res)
{
    int applied = 0;

    if (driver_applies(cmd, d) != 0) {
        applied = 1;
        if (cmd->action == ECL_ACTION_LOCK) {
            res->lock = ECL_LOCK;
            res->reason = RC_DRIVER_LOCK;
        } else {
            res->lock = ECL_RELEASE;
            res->reason = RC_DRIVER_UNLOCK;
        }
    }

    return applied;
}

/**
 * @brief  P5~P9(입력 조건 규칙) 중 첫 성립 규칙을 적용하고 1을 반환한다.
 * @tracing SSR-011, SSR-012, SSR-013, SSR-014, SRQ-FUN-009, SRQ-FUN-010, SRQ-FUN-012, SRQ-FUN-014
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int apply_input_rules(const ecl_inputs_t *in, const ecl_cmd_result_t *cmd,
                             const ecl_ovr_result_t *ovr, int d, door_result_t *res)
{
    int applied = 1;

    if (in->approach[d] != 0) {
        res->lock = ECL_LOCK;
        res->reason = (ovr->ack[d] != 0) ? RC_OVERRIDE_ACK : RC_APPROACH_RISK;
    } else if ((in->fire != 0) || (in->overtemp != 0) || (in->adult != 0)) {
        res->lock = ECL_RELEASE;
        res->reason = fire_reason(in);
    } else if (in->isofix[d] != 0) {
        res->lock = ECL_LOCK;
        res->reason = RC_ISOFIX;
    } else if (in->speed_moving != 0) {
        res->lock = ECL_LOCK;
        res->reason = speed_reason(cmd);
    } else if (in->ignition_off != 0) {
        res->lock = ECL_RELEASE;
        res->reason = RC_IGNITION_OFF;
    } else {
        applied = 0;
    }

    return applied;
}

/**
 * @brief  P2~P4 유지: 직전 출력을 유지하되 접근위험 도어는 LOCK이다.
 * @tracing SSR-003, SSR-010, SSR-011, SSR-016, SSR-017, SSR-024
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static door_result_t decide_hold(const ecl_inputs_t *in, const ecl_lock_t prev[ECL_DOOR_COUNT], int d)
{
    door_result_t res;

    res.lock = (in->approach[d] != 0) ? ECL_LOCK : prev[d];
    res.reason = in->hold_reason;

    return res;
}

/**
 * @brief  P5~P10 규칙을 적용한다(성립 규칙이 없으면 직전 출력 유지). PENDING 타임아웃 판정 평가는 코드를 P4로 덮어쓴다.
 * @tracing SSR-027, SSR-031
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static door_result_t decide_rules(const ecl_inputs_t *in, const ecl_cmd_result_t *cmd, const ecl_ovr_result_t *ovr,
                                  const ecl_lock_t prev[ECL_DOOR_COUNT], int d)
{
    door_result_t res;

    res.lock = prev[d];
    res.reason = RC_NONE;
    if (apply_input_rules(in, cmd, ovr, d, &res) == 0) {
        if (apply_driver(cmd, d, &res) == 0) {
            res.reason = RC_NONE;
        }
    }
    if (in->pending_timeout_first != 0) {
        res.reason = RC_CRASH_PENDING_TIMEOUT;
    }

    return res;
}

/**
 * @brief  한 도어의 결정을 만든다(P2~P4 유지 또는 P5~P10).
 * @tracing SSR-027
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static door_result_t decide_door(const ecl_inputs_t *in, const ecl_cmd_result_t *cmd, const ecl_ovr_result_t *ovr,
                                 const ecl_lock_t prev[ECL_DOOR_COUNT], int d)
{
    door_result_t res;

    if (in->hold_reason != RC_NONE) {
        res = decide_hold(in, prev, d);
    } else {
        res = decide_rules(in, cmd, ovr, prev, d);
    }

    return res;
}

/**
 * @brief  두 도어의 이유코드 중 순위가 높은(같으면 값이 작은) 것을 고른다.
 * @tracing SSR-027
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static ecl_reason_t merge_reason(ecl_reason_t a, ecl_reason_t b)
{
    ecl_reason_t merged = a;
    const int rank_a = reason_rank(a);
    const int rank_b = reason_rank(b);

    if ((rank_b < rank_a) || ((rank_b == rank_a) && ((int)b < (int)a))) {
        merged = b;
    }

    return merged;
}

/**
 * @brief  충돌 CONFIRMED(P1)이 아닌 평가에서 두 도어를 각각 결정해 출력을 out에 채우고 병합한 이유코드를 반환한다.
 * @tracing SSR-027, SSR-014
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static ecl_reason_t decide_both(const ecl_inputs_t *in, const ecl_cmd_result_t *cmd, const ecl_ovr_result_t *ovr,
                                const ecl_lock_t prev[ECL_DOOR_COUNT], ecl_decision_t *out)
{
    const door_result_t left = decide_door(in, cmd, ovr, prev, (int)ECL_DOOR_LEFT);
    const door_result_t right = decide_door(in, cmd, ovr, prev, (int)ECL_DOOR_RIGHT);

    out->lock[ECL_DOOR_LEFT] = left.lock;
    out->lock[ECL_DOOR_RIGHT] = right.lock;

    return merge_reason(left.reason, right.reason);
}

/**
 * @brief  도어별로 우선순위 규칙 P1~P11을 적용하여 출력과 이유코드를 결정한다(순수 함수).
 * @tracing SSR-027, SSR-001, SSR-002, SSR-003, SSR-010, SSR-011, SSR-012, SSR-013, SSR-014, SSR-016, SSR-024, SRQ-FUN-021
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_arb_decide(const ecl_inputs_t *in, const ecl_cmd_result_t *cmd, const ecl_ovr_result_t *ovr,
                         const ecl_lock_t prev[ECL_DOOR_COUNT], ecl_decision_t *out)
{
    ecl_ret_t ret = RET_OK;

    if ((in == NULL) || (cmd == NULL) || (ovr == NULL) || (prev == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        out->command_rejected = (cmd->rejected != 0) ? 1 : 0;
        if (in->crash_confirmed != 0) {
            out->lock[ECL_DOOR_LEFT] = ECL_RELEASE;
            out->lock[ECL_DOOR_RIGHT] = ECL_RELEASE;
            out->reason_code = RC_CRASH_CONFIRMED;
        } else {
            out->reason_code = decide_both(in, cmd, ovr, prev, out);
        }
        if ((out->command_rejected != 0) && (out->reason_code == RC_NONE)) {
            out->reason_code = RC_COMMAND_REJECTED;
        }
    }

    return ret;
}
