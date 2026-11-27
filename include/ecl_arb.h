/**
 * @file    ecl_arb.h
 * @brief   우선순위 중재 컴포넌트(CMP-ARB) 인터페이스.
 * @tracing SSR-027, SSR-001, SSR-002, SSR-003, SSR-011, SSR-012, SSR-013, SSR-014
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_ARB_H
#define ECL_ARB_H

#include "ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  도어별로 우선순위 규칙 P1~P11을 적용하여 출력과 이유코드를 결정한다(순수 함수).
 * @param  prev 직전 확정 출력(두 원소, ECL_RELEASE 또는 ECL_LOCK).
 * @tracing SSR-027, SSR-001, SSR-002, SSR-003, SSR-010, SSR-011, SSR-012, SSR-013, SSR-014, SSR-016, SSR-017, SSR-024, SRQ-FUN-021
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_arb_decide(const ecl_inputs_t *in, const ecl_cmd_result_t *cmd, const ecl_ovr_result_t *ovr,
                         const ecl_lock_t prev[ECL_DOOR_COUNT], ecl_decision_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_ARB_H */
