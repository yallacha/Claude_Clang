/**
 * @file    ecl_cmd.h
 * @brief   명령 해석 컴포넌트(CMP-CMD) 인터페이스.
 * @tracing SRQ-FUN-001, SRQ-FUN-007, SRQ-FUN-008, SRQ-FUN-031
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_CMD_H
#define ECL_CMD_H

#include "ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  원시 명령을 검사하여 수락/거절을 판정한다.
 * @tracing SRQ-FUN-001, SRQ-FUN-007, SRQ-FUN-008
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_cmd_interpret(const ecl_command_t *raw, ecl_cmd_result_t *out);

#ifdef __cplusplus
}
#endif

#endif /* ECL_CMD_H */
