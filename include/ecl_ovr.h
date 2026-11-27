/**
 * @file    ecl_ovr.h
 * @brief   override 추적 컴포넌트(CMP-OVR) 인터페이스.
 * @tracing SRQ-FUN-017, SRQ-FUN-019, SRQ-FUN-020, SRQ-FUN-033, SRQ-FUN-034, SRQ-FUN-035, SSR-015, SSR-035
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_OVR_H
#define ECL_OVR_H

#include "ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** OVR 컨텍스트. 필드는 비공개 관례이며 아래 함수로만 접근한다. */
typedef struct {
    int first_valid[ECL_DOOR_COUNT];
    double first_s[ECL_DOOR_COUNT];
    ecl_source_t first_source[ECL_DOOR_COUNT];
    int override[ECL_DOOR_COUNT];
} ecl_ovr_t;

/**
 * @brief  컨텍스트를 초기화한다.
 * @tracing SSR-015
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_ovr_init(ecl_ovr_t *ovr);

/**
 * @brief  도어별 해제 억제·override 상태를 갱신한다.
 * @tracing SRQ-FUN-017, SRQ-FUN-019, SRQ-FUN-020, SRQ-FUN-033, SRQ-FUN-034, SRQ-FUN-035, SSR-015, SSR-035
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_ovr_update(ecl_ovr_t *ovr, double now_s, const ecl_inputs_t *in,
                         const ecl_cmd_result_t *cmd, ecl_ovr_result_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_OVR_H */
