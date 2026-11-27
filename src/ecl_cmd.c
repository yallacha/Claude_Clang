/**
 * @file    ecl_cmd.c
 * @brief   명령 해석 컴포넌트(CMP-CMD) 구현.
 * @tracing SRQ-FUN-001, SRQ-FUN-007, SRQ-FUN-008, SRQ-FUN-031
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#include "ecl_cmd.h"

#include <string.h>

/**
 * @brief  v가 누락이 아니고 [lo, hi] 범위인지 판정한다.
 * @tracing SRQ-FUN-007
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int field_ok(int v, int lo, int hi)
{
    int ok = 0;

    if ((v != ECL_ABSENT) && (v >= lo) && (v <= hi)) {
        ok = 1;
    }

    return ok;
}

/**
 * @brief  원시 명령을 검사하여 수락/거절을 판정한다. present가 0이면 명령 없음, 1이면 필드를 검사하며,
 *         0/1이 아닌 present는 형식 오류로 거절한다(DEC-013).
 * @tracing SRQ-FUN-001, SRQ-FUN-007, SRQ-FUN-008, SRQ-FUN-031
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_cmd_interpret(const ecl_command_t *raw, ecl_cmd_result_t *out)
{
    ecl_ret_t ret = RET_OK;

    if ((raw == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        (void)memset(out, 0, sizeof(*out));
        if (raw->present != 0) {
            const int ok = (raw->present == 1) && field_ok(raw->side, (int)ECL_SIDE_LEFT, (int)ECL_SIDE_ALL) &&
                           field_ok(raw->action, (int)ECL_ACTION_LOCK, (int)ECL_ACTION_UNLOCK) &&
                           field_ok(raw->source, (int)ECL_SRC_PHYSICAL_BUTTON, (int)ECL_SRC_MOBILE_APP);
            out->present = 1;
            out->valid = ok;
            out->rejected = !ok;
            if (ok != 0) {
                out->side = (ecl_side_t)raw->side;
                out->action = (ecl_action_t)raw->action;
                out->source = (ecl_source_t)raw->source;
            }
        }
    }

    return ret;
}
