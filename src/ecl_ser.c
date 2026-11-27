/**
 * @file    ecl_ser.c
 * @brief   상태 직렬화 컴포넌트(CMP-SER) 구현: `필드=값;` ASCII.
 * @tracing SRQ-FUN-023, SRQ-QUA-FUN-001
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#include "ecl_ser.h"

#include <string.h>

#define SER_TMP_SIZE (1024U)

static const char *const LOCK_NAMES[2] = {"RELEASE", "LOCK"};
static const char *const STATE_NAMES[4] = {"OFF", "NORMAL", "DEGRADED", "FAULT"};
static const char *const VALIDITY_NAMES[3] = {"VALID", "INVALID", "STALE"};
static const char *const REASON_NAMES[19] = {
    "RC_NONE", "RC_CRASH_CONFIRMED", "RC_SENSOR_FAULT", "RC_CRASH_PENDING_HOLD", "RC_CRASH_PENDING_TIMEOUT",
    "RC_INPUT_STALE", "RC_INPUT_INVALID", "RC_FIRE", "RC_OVERTEMP", "RC_ADULT_PRESENT", "RC_APPROACH_RISK",
    "RC_OVERRIDE_ACK", "RC_ISOFIX", "RC_IGNITION_OFF", "RC_SPEED_AUTO_LOCK", "RC_UNLOCK_DENIED_MOVING",
    "RC_DRIVER_LOCK", "RC_DRIVER_UNLOCK", "RC_COMMAND_REJECTED"
};
static const char *const SIGNAL_NAMES[ECL_SIG_COUNT] = {
    "speed", "gear", "source_ts", "crash", "approach_left", "approach_right", "fire", "overtemp", "adult",
    "isofix_left", "isofix_right", "ignition", "sensor_fault"
};

/** 쓰기 상태 */
typedef struct {
    char buf[SER_TMP_SIZE];
    size_t pos;
    int overflow;
} ser_writer_t;

/**
 * @brief  열거값에 대응하는 이름을 반환한다(범위 밖은 "?").
 * @tracing SRQ-FUN-023
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static const char *name_of(const char *const *table, int count, int idx)
{
    const char *name = "?";

    if ((idx >= 0) && (idx < count)) {
        name = table[idx];
    }

    return name;
}

/**
 * @brief  `name=value;`를 버퍼 끝에 붙인다(넘치면 overflow 표시).
 * @tracing SRQ-FUN-023
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int write_pair(ser_writer_t *w, const char *name, const char *value)
{
    size_t need = strlen(name) + strlen(value) + 2U;

    if ((w->overflow == 0) && ((w->pos + need) < SER_TMP_SIZE)) {
        (void)memcpy(&w->buf[w->pos], name, strlen(name));
        w->pos += strlen(name);
        w->buf[w->pos] = '=';
        w->pos++;
        (void)memcpy(&w->buf[w->pos], value, strlen(value));
        w->pos += strlen(value);
        w->buf[w->pos] = ';';
        w->pos++;
        w->buf[w->pos] = '\0';
    } else {
        w->overflow = 1;
    }

    return w->overflow;
}

/**
 * @brief  출력, state, 이유코드, override, command_rejected 필드를 쓴다.
 * @tracing SRQ-FUN-022, SRQ-FUN-023
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int write_header(ser_writer_t *w, const ecl_status_t *st)
{
    (void)write_pair(w, "lock_left", name_of(LOCK_NAMES, 2, (int)st->lock[ECL_DOOR_LEFT]));
    (void)write_pair(w, "lock_right", name_of(LOCK_NAMES, 2, (int)st->lock[ECL_DOOR_RIGHT]));
    (void)write_pair(w, "state", name_of(STATE_NAMES, 4, (int)st->state));
    (void)write_pair(w, "reason_code", name_of(REASON_NAMES, 19, (int)st->reason_code));
    (void)write_pair(w, "override_left", (st->override[ECL_DOOR_LEFT] != 0) ? "1" : "0");
    (void)write_pair(w, "override_right", (st->override[ECL_DOOR_RIGHT] != 0) ? "1" : "0");
    (void)write_pair(w, "command_rejected", (st->command_rejected != 0) ? "1" : "0");

    return w->overflow;
}

/**
 * @brief  입력별 validity 필드(`validity_<신호>`)를 쓴다.
 * @tracing SRQ-FUN-022
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
static int write_validity(ser_writer_t *w, const ecl_status_t *st)
{
    char key[40];
    int i = 0;

    for (i = 0; i < ECL_SIG_COUNT; i++) {
        (void)strcpy(key, "validity_");
        (void)strcat(key, SIGNAL_NAMES[i]);
        (void)write_pair(w, key, name_of(VALIDITY_NAMES, 3, (int)st->validity[i]));
    }

    return w->overflow;
}

ecl_ret_t ecl_ser_status(const ecl_status_t *status, char *buf, size_t capacity, size_t *length)
{
    ecl_ret_t ret = RET_OK;
    ser_writer_t w;

    if ((status == NULL) || (buf == NULL) || (length == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        w.pos = 0U;
        w.overflow = 0;
        w.buf[0] = '\0';
        (void)write_header(&w, status);
        (void)write_validity(&w, status);
        if ((w.overflow != 0) || ((w.pos + 1U) > capacity)) {
            ret = RET_ERR_SERIALIZE;
        } else {
            (void)memcpy(buf, w.buf, w.pos + 1U);
            *length = w.pos;
        }
    }

    return ret;
}
