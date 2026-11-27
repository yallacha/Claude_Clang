/**
 * @file    ecl_sta.c
 * @brief   state 결정 컴포넌트(CMP-STA) 구현.
 * @tracing SSR-009, SSR-018, SSR-026, SSR-028, SSR-030, SRQ-FUN-015
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#include "ecl_sta.h"

/**
 * @brief  조건표(FAULT > DEGRADED > OFF > NORMAL)로 state를 결정한다(이전 state와 무관, SSR-030).
 * @tracing SSR-009, SSR-018, SSR-026, SSR-028, SSR-030, SRQ-FUN-015
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_sta_decide(const ecl_inputs_t *in, ecl_state_t *out)
{
    ecl_ret_t ret = RET_OK;

    if ((in == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else if (in->sensor_fault != 0) {
        *out = ECL_STATE_FAULT;
    } else if ((in->stale != 0) || (in->substituted != 0)) {
        *out = ECL_STATE_DEGRADED;
    } else if (in->ignition_off != 0) {
        *out = ECL_STATE_OFF;
    } else {
        *out = ECL_STATE_NORMAL;
    }

    return ret;
}
