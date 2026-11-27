/**
 * @file    ecl_val.h
 * @brief   입력 조건화 컴포넌트(CMP-VAL) 인터페이스.
 * @tracing SSR-005, SSR-006, SSR-007, SSR-008, SSR-029, SSR-032
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_VAL_H
#define ECL_VAL_H

#include "ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** VAL 컨텍스트. 모든 필드는 private이며 아래 함수로만 접근한다. */
typedef struct {
    int rx_valid;
    double last_rx_s;
    int pending_active;
    double pending_start_s;
    int pending_reported;
    ecl_snapshot_t last_snap;
} ecl_val_t;

/**
 * @brief  컨텍스트를 초기화한다.
 * @tracing SSR-007
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_val_init(ecl_val_t *val);

/**
 * @brief  스냅샷을 검사·대체하여 조건화된 입력을 만든다.
 * @note   평가 시각은 snap->timestamp_s이다.
 * @tracing SSR-005, SSR-006, SSR-007, SSR-008, SSR-020, SSR-021, SSR-022, SSR-023, SSR-025, SSR-029, SSR-032, SSR-004, SSR-031
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_val_condition(ecl_val_t *val, const ecl_snapshot_t *snap, ecl_inputs_t *out);

/**
 * @brief  스냅샷 없는 평가(틱)의 조건화된 입력을 만든다.
 * @tracing SSR-008
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_val_condition_tick(ecl_val_t *val, double now_s, ecl_inputs_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_VAL_H */
