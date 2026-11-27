/**
 * @file    ecl_ser.h
 * @brief   상태 직렬화 컴포넌트(CMP-SER) 인터페이스.
 * @tracing SRQ-FUN-023, SRQ-QUA-FUN-001
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_SER_H
#define ECL_SER_H

#include "ecl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  상태를 `필드=값;` ASCII로 직렬화한다. 버퍼가 부족하면 RET_ERR_SERIALIZE이며 buf는 변경하지 않는다.
 * @param  capacity buf 크기(널 종료 포함).
 * @tracing SRQ-FUN-023, SRQ-QUA-FUN-001
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
ecl_ret_t ecl_ser_status(const ecl_status_t *status, char *buf, size_t capacity, size_t *length);

#ifdef __cplusplus
}
#endif

#endif /* ECL_SER_H */
