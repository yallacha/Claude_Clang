/**
 * @file    ecl_sta.h
 * @brief   state 결정 컴포넌트(CMP-STA) 인터페이스.
 * @tracing SSR-009, SSR-018, SSR-026, SSR-028, SSR-030, SRQ-FUN-015
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_STA_H
#define ECL_STA_H

#include "ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  조건표(FAULT > DEGRADED > OFF > NORMAL)로 state를 결정한다(순수 함수).
 * @tracing SSR-009, SSR-018, SSR-026, SSR-028, SSR-030, SRQ-FUN-015
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_sta_decide(const ecl_inputs_t *in, ecl_state_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_STA_H */
