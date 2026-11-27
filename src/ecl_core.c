/**
 * @file    ecl_core.c
 * @brief   제어 코어(CMP-CORE) 구현: 평가 파이프라인과 공개 API.
 * @tracing SRQ-FUN-021, SRQ-FUN-022, SRQ-FUN-026, SRQ-FUN-027, SRQ-FUN-036, SRQ-QUA-REL-001, SRQ-CON-002, SRQ-CON-005
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#include "ecl_core.h"

#include <string.h>

#include "ecl_arb.h"
#include "ecl_cmd.h"
#include "ecl_ser.h"
#include "ecl_sta.h"

/**
 * @brief  초기 상태(RELEASE/RELEASE/OFF/RC_NONE)로 status를 설정한다.
 * @tracing SRQ-FUN-026, SRQ-FUN-036
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int reset_status(ecl_status_t *st)
{
    (void)memset(st, 0, sizeof(*st));
    st->state = ECL_STATE_OFF;
    st->reason_code = RC_NONE;

    return 0;
}

/**
 * @brief  중재·state·입력 결과로 새 status를 조립한다.
 * @tracing SRQ-FUN-022
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int build_status(ecl_status_t *next, const ecl_inputs_t *in, const ecl_decision_t *dec,
                        const ecl_ovr_result_t *ovr, ecl_state_t state)
{
    int i = 0;

    for (i = 0; i < ECL_DOOR_COUNT; i++) {
        next->lock[i] = dec->lock[i];
        next->override[i] = ovr->override[i];
    }
    next->state = state;
    next->reason_code = dec->reason_code;
    next->command_rejected = dec->command_rejected;
    for (i = 0; i < ECL_SIG_COUNT; i++) {
        next->validity[i] = in->validity[i];
    }

    return 0;
}

/**
 * @brief  직전 status와 달라졌거나 명령이 거절되었으면 이벤트를 1건 기록한다.
 * @tracing SRQ-FUN-027, SRQ-FUN-029
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static ecl_ret_t log_if_changed(ecl_core_t *core, const ecl_status_t *next, double now_s)
{
    ecl_ret_t ret = RET_OK;
    ecl_event_t ev;
    uint64_t id = 0U;
    int changed = 0;

    changed = (next->lock[ECL_DOOR_LEFT] != core->status.lock[ECL_DOOR_LEFT]) ||
              (next->lock[ECL_DOOR_RIGHT] != core->status.lock[ECL_DOOR_RIGHT]) ||
              (next->state != core->status.state) || (next->reason_code != core->status.reason_code) ||
              (next->command_rejected != 0);
    if (changed != 0) {
        (void)memset(&ev, 0, sizeof(ev));
        ev.timestamp_s = now_s;
        ev.lock[ECL_DOOR_LEFT] = next->lock[ECL_DOOR_LEFT];
        ev.lock[ECL_DOOR_RIGHT] = next->lock[ECL_DOOR_RIGHT];
        ev.state = next->state;
        ev.reason_code = next->reason_code;
        ev.command_rejected = next->command_rejected;
        (void)memcpy(ev.validity, next->validity, sizeof(ev.validity));
        ret = ecl_log_append(&core->log, &ev, &id);
    }

    return ret;
}

/**
 * @brief  조건화된 입력으로 명령 해석 → override → 중재 → state → 기록을 수행한다.
 * @tracing SRQ-FUN-021, SRQ-FUN-027, SSR-027
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static ecl_ret_t run_pipeline(ecl_core_t *core, const ecl_inputs_t *in, double now_s)
{
    ecl_ret_t ret = RET_OK;
    ecl_cmd_result_t cmd;
    ecl_ovr_result_t ovr;
    ecl_decision_t dec;
    ecl_state_t state = ECL_STATE_OFF;
    ecl_status_t next;

    (void)memset(&cmd, 0, sizeof(cmd));
    (void)memset(&ovr, 0, sizeof(ovr));
    (void)memset(&dec, 0, sizeof(dec));
    (void)memset(&next, 0, sizeof(next));
    ret = ecl_cmd_interpret(&in->command, &cmd);
    if (ret == RET_OK) {
        ret = ecl_ovr_update(&core->ovr, now_s, in, &cmd, &ovr);
    }
    if (ret == RET_OK) {
        ret = ecl_arb_decide(in, &cmd, &ovr, core->status.lock, &dec);
    }
    if (ret == RET_OK) {
        ret = ecl_sta_decide(in, &state);
    }
    if (ret == RET_OK) {
        (void)build_status(&next, in, &dec, &ovr, state);
        ret = log_if_changed(core, &next, now_s);
    }
    if (ret == RET_OK) {
        core->status = next;
    }

    return ret;
}

ecl_ret_t ecl_core_init(ecl_core_t *core)
{
    ecl_ret_t ret = RET_OK;

    if (core == NULL) {
        ret = RET_ERR_NULL;
    } else {
        ret = ecl_val_init(&core->val);
        if (ret == RET_OK) {
            ret = ecl_ovr_init(&core->ovr);
        }
        if (ret == RET_OK) {
            ret = ecl_log_init(&core->log);
        }
        (void)reset_status(&core->status);
    }

    return ret;
}

ecl_ret_t ecl_core_evaluate(ecl_core_t *core, const ecl_snapshot_t *snap)
{
    ecl_ret_t ret = RET_OK;
    ecl_inputs_t in;

    if ((core == NULL) || (snap == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        ret = ecl_val_condition(&core->val, snap, &in);
        if (ret == RET_OK) {
            ret = run_pipeline(core, &in, snap->timestamp_s);
        }
    }

    return ret;
}

ecl_ret_t ecl_core_tick(ecl_core_t *core, double now_s)
{
    ecl_ret_t ret = RET_OK;
    ecl_inputs_t in;

    if (core == NULL) {
        ret = RET_ERR_NULL;
    } else {
        ret = ecl_val_condition_tick(&core->val, now_s, &in);
        if (ret == RET_OK) {
            ret = run_pipeline(core, &in, now_s);
        }
    }

    return ret;
}

ecl_ret_t ecl_core_get_status(const ecl_core_t *core, ecl_status_t *out)
{
    ecl_ret_t ret = RET_OK;

    if ((core == NULL) || (out == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        *out = core->status;
    }

    return ret;
}

ecl_ret_t ecl_core_serialize_status(const ecl_core_t *core, char *buf, size_t capacity, size_t *length)
{
    ecl_ret_t ret = RET_OK;

    if (core == NULL) {
        ret = RET_ERR_NULL;
    } else {
        ret = ecl_ser_status(&core->status, buf, capacity, length);
    }

    return ret;
}

ecl_ret_t ecl_core_query_events(const ecl_core_t *core, ecl_event_t *out, size_t capacity, size_t *count)
{
    ecl_ret_t ret = RET_OK;

    if (core == NULL) {
        ret = RET_ERR_NULL;
    } else {
        ret = ecl_log_query(&core->log, out, capacity, count);
    }

    return ret;
}
