/**
 * @file    ecl_core.h
 * @brief   제어 코어(CMP-CORE) 공개 API. 평가 호출 간격은 0.010 s 계약이다(SRQ-LIM-002).
 * @tracing SRQ-FUN-021, SRQ-FUN-022, SRQ-FUN-026, SRQ-FUN-036, SRQ-QUA-REL-001, SRQ-CON-002, SRQ-CON-004, SRQ-CON-005
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_CORE_H
#define ECL_CORE_H

#include "ecl_log.h"
#include "ecl_ovr.h"
#include "ecl_types.h"
#include "ecl_val.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 코어 컨텍스트. 모든 필드는 private이며 호출자가 소유한다(전역 상태 없음). */
typedef struct {
    ecl_val_t val;
    ecl_ovr_t ovr;
    ecl_log_t log;
    ecl_status_t status;
} ecl_core_t;

/**
 * @brief  초기 상태(RELEASE/RELEASE/OFF/RC_NONE, 이벤트 0건)로 초기화한다.
 * @tracing SRQ-FUN-026, SRQ-FUN-036
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_core_init(ecl_core_t *core);

/**
 * @brief  스냅샷 1건으로 평가 1회를 수행한다. 같은 호출의 반환 시점에 출력·state·이벤트가 확정된다.
 * @tracing SRQ-FUN-021, SRQ-LIM-004, SRQ-CON-002, SRQ-CON-005, SRQ-QUA-REL-001
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_core_evaluate(ecl_core_t *core, const ecl_snapshot_t *snap);

/**
 * @brief  스냅샷 없는 평가(시간 경과 처리)를 수행한다.
 * @tracing SSR-008, SRQ-FUN-021
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_core_tick(ecl_core_t *core, double now_s);

/**
 * @brief  마지막 평가의 상태를 조회한다.
 * @tracing SRQ-FUN-022
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_core_get_status(const ecl_core_t *core, ecl_status_t *out);

/**
 * @brief  마지막 평가의 상태를 직렬화한다.
 * @tracing SRQ-FUN-022, SRQ-FUN-023, SRQ-QUA-FUN-001
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_core_serialize_status(const ecl_core_t *core, char *buf, size_t capacity, size_t *length);

/**
 * @brief  이벤트를 최신 최대 capacity건, ID 오름차순으로 조회한다.
 * @tracing SRQ-FUN-030
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_core_query_events(const ecl_core_t *core, ecl_event_t *out, size_t capacity, size_t *count);

#ifdef __cplusplus
}
#endif

#endif /* ECL_CORE_H */
