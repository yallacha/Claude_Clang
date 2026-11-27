/**
 * @file    ecl_ser.c
 * @brief   상태 직렬화 컴포넌트(CMP-SER) 구현: `필드=값;` ASCII. 길이를 먼저 계산(건식)하여 버퍼가 충분할 때만 기록한다.
 * @tracing SRQ-FUN-023, SRQ-QUA-FUN-001
 *
 * @@HITL@@
 */
#include "ecl_ser.h"

#include <string.h>

static const char *const LOCK_NAMES[ECL_LOCK_NAME_COUNT] = {"RELEASE", "LOCK"};
static const char *const STATE_NAMES[ECL_STATE_NAME_COUNT] = {"OFF", "NORMAL", "DEGRADED", "FAULT"};
static const char *const VALIDITY_NAMES[ECL_VALIDITY_NAME_COUNT] = {"VALID", "INVALID", "STALE"};
static const char *const REASON_NAMES[ECL_REASON_COUNT] = {
    "RC_NONE", "RC_CRASH_CONFIRMED", "RC_SENSOR_FAULT", "RC_CRASH_PENDING_HOLD", "RC_CRASH_PENDING_TIMEOUT",
    "RC_INPUT_STALE", "RC_INPUT_INVALID", "RC_FIRE", "RC_OVERTEMP", "RC_ADULT_PRESENT", "RC_APPROACH_RISK",
    "RC_OVERRIDE_ACK", "RC_ISOFIX", "RC_IGNITION_OFF", "RC_SPEED_AUTO_LOCK", "RC_UNLOCK_DENIED_MOVING",
    "RC_DRIVER_LOCK", "RC_DRIVER_UNLOCK", "RC_COMMAND_REJECTED"
};
static const char *const SIGNAL_NAMES[ECL_SIG_COUNT] = {
    "speed", "gear", "source_ts", "crash", "approach_left", "approach_right", "fire", "overtemp", "adult",
    "isofix_left", "isofix_right", "ignition", "sensor_fault"
};

/**
 * @brief  열거값에 대응하는 이름을 반환한다(범위 밖은 "?").
 * @tracing SRQ-FUN-023
 * @@HITL@@
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
 * @brief  `name=value;`를 buf의 pos 위치에 쓰고(buf가 NULL이면 길이만 계산) 새 위치를 반환한다.
 * @tracing SRQ-FUN-023
 * @@HITL@@
 */
static size_t write_pair(char *buf, size_t pos, const char *name, const char *value)
{
    const size_t name_len = strlen(name);
    const size_t value_len = strlen(value);

    if (buf != NULL) {
        (void)memcpy(&buf[pos], name, name_len);
        buf[pos + name_len] = '=';
        (void)memcpy(&buf[pos + name_len + 1U], value, value_len);
        buf[pos + name_len + 1U + value_len] = ';';
    }

    return pos + name_len + value_len + 2U;
}

/**
 * @brief  출력, state, 이유코드, override, command_rejected 필드를 pos 위치부터 쓰고 새 위치를 반환한다.
 * @tracing SRQ-FUN-022, SRQ-FUN-023
 * @@HITL@@
 */
static size_t write_header(char *buf, size_t pos, const ecl_status_t *st)
{
    size_t p = pos;

    p = write_pair(buf, p, "lock_left", name_of(LOCK_NAMES, ECL_LOCK_NAME_COUNT, (int)st->lock[ECL_DOOR_LEFT]));
    p = write_pair(buf, p, "lock_right", name_of(LOCK_NAMES, ECL_LOCK_NAME_COUNT, (int)st->lock[ECL_DOOR_RIGHT]));
    p = write_pair(buf, p, "state", name_of(STATE_NAMES, ECL_STATE_NAME_COUNT, (int)st->state));
    p = write_pair(buf, p, "reason_code", name_of(REASON_NAMES, ECL_REASON_COUNT, (int)st->reason_code));
    p = write_pair(buf, p, "override_left", (st->override[ECL_DOOR_LEFT] != 0) ? "1" : "0");
    p = write_pair(buf, p, "override_right", (st->override[ECL_DOOR_RIGHT] != 0) ? "1" : "0");
    p = write_pair(buf, p, "command_rejected", (st->command_rejected != 0) ? "1" : "0");

    return p;
}

/**
 * @brief  입력별 validity 필드(`validity_<신호>`)를 pos 위치부터 쓰고 새 위치를 반환한다.
 * @tracing SRQ-FUN-022
 * @@HITL@@
 */
static size_t write_validity(char *buf, size_t pos, const ecl_status_t *st)
{
    size_t p = pos;
    char key[ECL_SER_KEY_MAX];

    for (int i = 0; i < ECL_SIG_COUNT; i++) {
        (void)strcpy(key, "validity_");
        (void)strcat(key, SIGNAL_NAMES[i]);
        p = write_pair(buf, p, key, name_of(VALIDITY_NAMES, ECL_VALIDITY_NAME_COUNT, (int)st->validity[i]));
    }

    return p;
}

/**
 * @brief  전체 직렬화(헤더 + validity)를 쓰고 총 길이(널 문자 제외)를 반환한다.
 * @tracing SRQ-FUN-022, SRQ-QUA-FUN-001
 * @@HITL@@
 */
static size_t write_all(char *buf, const ecl_status_t *st)
{
    const size_t after_header = write_header(buf, 0U, st);

    return write_validity(buf, after_header, st);
}

/**
 * @brief  상태를 `필드=값;` ASCII로 직렬화한다. 버퍼가 부족하면 RET_ERR_SERIALIZE이며 buf는 변경하지 않는다.
 * @tracing SRQ-FUN-023, SRQ-QUA-FUN-001
 * @@HITL@@
 */
ecl_ret_t ecl_ser_status(const ecl_status_t *status, char *buf, size_t capacity, size_t *length)
{
    ecl_ret_t ret = RET_OK;

    if ((status == NULL) || (buf == NULL) || (length == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        const size_t need = write_all(NULL, status);
        if ((need + 1U) > capacity) {
            ret = RET_ERR_SERIALIZE;
        } else {
            const size_t written = write_all(buf, status);
            buf[written] = '\0';
            *length = written;
        }
    }

    return ret;
}
