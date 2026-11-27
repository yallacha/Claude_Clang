/**
 * @file    ecl_log.c
 * @brief   이벤트 기록 컴포넌트(CMP-LOG) 구현 (고정 크기 순환 버퍼, 동적 할당 없음).
 * @tracing SRQ-FUN-027, SRQ-FUN-028, SRQ-FUN-030, SRQ-FUN-037, SRQ-FUN-039, SRQ-LIM-001, SRQ-LIM-003
 *
 * @@HITL@@
 */
#include "ecl_log.h"

#include <string.h>

/**
 * @brief  이벤트를 저장할 버퍼 인덱스를 정한다(가득 차면 가장 오래된 것을 덮어쓰도록 head를 전진).
 * @tracing SRQ-LIM-003
 * @@HITL@@
 */
static size_t claim_slot(ecl_log_t *log)
{
    size_t idx = 0U;

    if (log->count < ECL_EVENT_CAPACITY) {
        idx = (log->head + log->count) % ECL_EVENT_CAPACITY;
        log->count++;
    } else {
        idx = log->head;
        log->head = (log->head + 1U) % ECL_EVENT_CAPACITY;
    }

    return idx;
}

/**
 * @brief  이력을 비운다.
 * @tracing SRQ-FUN-028, SRQ-LIM-001
 * @@HITL@@
 */
ecl_ret_t ecl_log_init(ecl_log_t *log)
{
    ecl_ret_t ret = RET_OK;

    if (log == NULL) {
        ret = RET_ERR_NULL;
    } else {
        (void)memset(log, 0, sizeof(*log));
        log->next_id = 1U;
    }

    return ret;
}

/**
 * @brief  이벤트를 기록하고 ID를 부여한다. ID가 최댓값에 도달한 뒤에는 기록하지 않고 *assigned_id를 0으로 한다.
 * @tracing SRQ-FUN-027, SRQ-FUN-028, SRQ-FUN-037, SRQ-LIM-001, SRQ-LIM-003
 * @@HITL@@
 */
ecl_ret_t ecl_log_append(ecl_log_t *log, const ecl_event_t *event, uint64_t *assigned_id)
{
    ecl_ret_t ret = RET_OK;

    if ((log == NULL) || (event == NULL) || (assigned_id == NULL)) {
        ret = RET_ERR_NULL;
    } else if (log->exhausted != 0) {
        *assigned_id = 0U;
    } else {
        const uint64_t id = log->next_id;
        const size_t idx = claim_slot(log);
        log->events[idx] = *event;
        log->events[idx].event_id = id;
        if (id == UINT64_MAX) {
            log->exhausted = 1;
        } else {
            log->next_id = id + 1U;
        }
        *assigned_id = id;
    }

    return ret;
}

/**
 * @brief  보존된 이벤트 중 최신 최대 capacity건을 event_id 오름차순으로 복사한다.
 * @tracing SRQ-FUN-030, SRQ-LIM-003
 * @@HITL@@
 */
ecl_ret_t ecl_log_query(const ecl_log_t *log, ecl_event_t *out, size_t capacity, size_t *count)
{
    ecl_ret_t ret = RET_OK;

    if ((log == NULL) || (out == NULL) || (count == NULL)) {
        ret = RET_ERR_NULL;
    } else {
        const size_t n = (log->count < capacity) ? log->count : capacity;
        const size_t start = (log->head + log->count - n) % ECL_EVENT_CAPACITY;
        for (size_t i = 0U; i < n; i++) {
            out[i] = log->events[(start + i) % ECL_EVENT_CAPACITY];
        }
        *count = n;
    }

    return ret;
}

#ifdef ECL_TEST_BUILD
/**
 * @brief  시험 빌드 전용: 다음 이벤트 ID가 value+1이 되도록 카운터를 설정한다.
 * @tracing SRQ-FUN-039
 * @@HITL@@
 */
ecl_ret_t ecl_log_test_set_counter(ecl_log_t *log, uint64_t value)
{
    ecl_ret_t ret = RET_OK;

    if (log == NULL) {
        ret = RET_ERR_NULL;
    } else if (value == UINT64_MAX) {
        log->exhausted = 1;
    } else {
        log->next_id = value + 1U;
        log->exhausted = 0;
    }

    return ret;
}
#endif
