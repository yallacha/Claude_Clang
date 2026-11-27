/**
 * @file    ecl_log.h
 * @brief   이벤트 기록 컴포넌트(CMP-LOG) 인터페이스.
 * @tracing SRQ-FUN-027, SRQ-FUN-028, SRQ-FUN-030, SRQ-FUN-037, SRQ-FUN-039, SRQ-LIM-001, SRQ-LIM-003
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_LOG_H
#define ECL_LOG_H

#include "ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** LOG 컨텍스트. 모든 필드는 private이다. */
typedef struct {
    ecl_event_t events[ECL_EVENT_CAPACITY];
    size_t head;
    size_t count;
    uint64_t next_id;
    int exhausted;
} ecl_log_t;

/**
 * @brief  이력을 비운다.
 * @tracing SRQ-FUN-028, SRQ-LIM-001
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_log_init(ecl_log_t *log);

/**
 * @brief  이벤트를 기록하고 ID를 부여한다. ID가 최댓값에 도달한 뒤에는 기록하지 않고 *assigned_id를 0으로 한다.
 * @tracing SRQ-FUN-027, SRQ-FUN-028, SRQ-FUN-037, SRQ-LIM-001, SRQ-LIM-003
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_log_append(ecl_log_t *log, const ecl_event_t *event, uint64_t *assigned_id);

/**
 * @brief  보존된 이벤트 중 최신 최대 capacity건을 event_id 오름차순으로 복사한다.
 * @tracing SRQ-FUN-030, SRQ-LIM-003
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_log_query(const ecl_log_t *log, ecl_event_t *out, size_t capacity, size_t *count);

#ifdef ECL_TEST_BUILD
/**
 * @brief  시험 빌드 전용: 다음 이벤트 ID가 value+1이 되도록 카운터를 설정한다.
 * @tracing SRQ-FUN-039
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_log_test_set_counter(ecl_log_t *log, uint64_t value);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ECL_LOG_H */
