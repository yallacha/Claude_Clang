/**
 * @file    ecl_ovr.c
 * @brief   override 추적 컴포넌트(CMP-OVR) 구현.
 * @tracing SRQ-FUN-017, SRQ-FUN-019, SRQ-FUN-020, SRQ-FUN-033, SRQ-FUN-034, SRQ-FUN-035, SSR-015, SSR-035
 *
 * @@HITL@@
 */
#include "ecl_ovr.h"

#include <string.h>

/**
 * @brief  해당 도어에 대한 유효 unlock 명령이면 1을 반환한다.
 *         (ecl_arb.c의 driver_applies는 lock/unlock 모두를 대상으로 하므로 판정 대상이 달라 공통화하지 않는다.)
 * @tracing SSR-015
 * @@HITL@@
 */
static int door_requested(const ecl_cmd_result_t *cmd, int door)
{
    int req = 0;

    if ((cmd->valid != 0) && (cmd->action == ECL_ACTION_UNLOCK) &&
        ((cmd->side == ECL_SIDE_ALL) || ((int)cmd->side == door))) {
        req = 1;
    }

    return req;
}

/**
 * @brief  접근위험 도어에 대한 해제 요청을 처리한다(첫 억제 기록, override 판정, 창 만료). 이번 평가에서 override로
 *         판정했으면 1(ack), 아니면 0을 반환한다.
 * @tracing SRQ-FUN-017, SRQ-FUN-019, SRQ-FUN-020, SRQ-FUN-033, SRQ-FUN-035
 * @@HITL@@
 */
static int apply_request(ecl_ovr_t *ovr, int d, double now_s, const ecl_cmd_result_t *cmd)
{
    int ack = 0;

    if (ovr->first_valid[d] == 0) {
        ovr->first_valid[d] = 1;
        ovr->first_s[d] = now_s;
        ovr->first_source[d] = cmd->source;
    } else {
        const double dt = now_s - ovr->first_s[d];
        if (dt > (ECL_OVERRIDE_WINDOW_S + ECL_TIME_EPS_S)) {
            ovr->first_s[d] = now_s;
            ovr->first_source[d] = cmd->source;
            ovr->override[d] = 0;
        } else if (ovr->first_source[d] == cmd->source) {
            ovr->override[d] = 1;
            ack = 1;
        }
    }

    return ack;
}

/**
 * @brief  한 도어의 override 상태를 갱신하고 결과(suppress, ack)를 out에 쓴 뒤 갱신된 override 상태를 반환한다.
 * @tracing SRQ-FUN-034, SSR-015, SSR-035
 * @@HITL@@
 */
static int update_door(ecl_ovr_t *ovr, int d, double now_s, const ecl_inputs_t *in,
                       const ecl_cmd_result_t *cmd, int blocked, ecl_ovr_result_t *out)
{
    if (in->approach[d] == 0) {
        ovr->first_valid[d] = 0;
        ovr->override[d] = 0;
    } else {
        const int req = (blocked == 0) && door_requested(cmd, d);
        if (req != 0) {
            out->suppress[d] = 1;
            out->ack[d] = apply_request(ovr, d, now_s, cmd);
        } else if ((ovr->first_valid[d] != 0) && ((now_s - ovr->first_s[d]) > (ECL_OVERRIDE_WINDOW_S + ECL_TIME_EPS_S))) {
            ovr->override[d] = 0;
        }
    }

    return ovr->override[d];
}

/**
 * @brief  컨텍스트를 초기화한다.
 * @tracing SSR-015
 * @@HITL@@
 */
ecl_ret_t ecl_ovr_init(ecl_ovr_t *ovr)
{
    ecl_ret_t ret = RET_OK;

    if (ovr == NULL) {
        ret = RET_ERR_NULL;
    } else {
        (void)memset(ovr, 0, sizeof(*ovr));
    }

    return ret;
}

/**
 * @brief  도어별 해제 억제·override 상태를 갱신한다.
 * @tracing SRQ-FUN-017, SRQ-FUN-019, SRQ-FUN-020, SRQ-FUN-033, SRQ-FUN-034, SRQ-FUN-035, SSR-015, SSR-035
 * @@HITL@@
 */
ecl_ret_t ecl_ovr_update(ecl_ovr_t *ovr, double now_s, const ecl_inputs_t *in,
                         const ecl_cmd_result_t *cmd, ecl_ovr_result_t *out)
{
    ecl_ret_t ret = RET_OK;

    if ((ovr == NULL) || (in == NULL) || (cmd == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        const int blocked = (in->crash_confirmed != 0) || (in->hold_reason != RC_NONE);
        (void)memset(out, 0, sizeof(*out));
        out->override[ECL_DOOR_LEFT] = update_door(ovr, (int)ECL_DOOR_LEFT, now_s, in, cmd, blocked, out);
        out->override[ECL_DOOR_RIGHT] = update_door(ovr, (int)ECL_DOOR_RIGHT, now_s, in, cmd, blocked, out);
    }

    return ret;
}
