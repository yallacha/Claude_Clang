/**
 * @file    ecl_types.h
 * @brief   전자식 차일드락 제어 SW 공통 타입과 상수 (CMP-TYP).
 * @tracing SRQ-FUN-022, SRQ-FUN-029, SRQ-CON-003, SRQ-CON-006, SRQ-CON-007
 *
 * 교육용 가상 OEM 입력 기반이며 준수·인증·ASIL 달성을 주장하지 않는다.
 *
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_TYPES_H
#define ECL_TYPES_H

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 오류코드(SRS 부록 A) */
typedef enum {
    RET_OK = 0,
    RET_ERR_NULL = -1,
    RET_ERR_SERIALIZE = -2
} ecl_ret_t;

/** 정수 입력의 누락 센티널 (속도 누락은 NaN) */
#define ECL_ABSENT (INT_MIN)

/** 평가/시험 상수 */
#define ECL_EVENT_CAPACITY (100U)
#define ECL_STALE_LIMIT_S (0.200)
#define ECL_PENDING_LIMIT_S (1.0)
#define ECL_OVERRIDE_WINDOW_S (10.0)
#define ECL_SPEED_MOVING_KPH (3.0)
#define ECL_SPEED_MAX_KPH (300.0)
#define ECL_TIME_EPS_S (1e-9)
#define ECL_DOOR_COUNT (2)
#define ECL_SIG_COUNT (13)
#define ECL_BOOL_SIG_COUNT (9)
#define ECL_LOCK_NAME_COUNT (2)
#define ECL_STATE_NAME_COUNT (4)
#define ECL_VALIDITY_NAME_COUNT (3)
#define ECL_REASON_COUNT (19)
#define ECL_SER_KEY_MAX (40U)
#define ECL_SER_EQ_LEN (1U)
#define ECL_SER_END_LEN (1U)
#define ECL_SER_NUL_LEN (1U)
#define ECL_FLAG_SET (1)
#define ECL_FLAG_CLEAR (0)

/** 우선순위 표(SSR-027)의 순위. 값이 작을수록 높다. */
#define ECL_RANK_P1 (1)
#define ECL_RANK_P2 (2)
#define ECL_RANK_P3 (3)
#define ECL_RANK_P4 (4)
#define ECL_RANK_P5 (5)
#define ECL_RANK_P6 (6)
#define ECL_RANK_P7 (7)
#define ECL_RANK_P8 (8)
#define ECL_RANK_P9 (9)
#define ECL_RANK_P10 (10)
#define ECL_RANK_P11 (11)
#define ECL_RANK_NONE (99)

typedef enum { ECL_RELEASE = 0, ECL_LOCK = 1 } ecl_lock_t;
typedef enum { ECL_STATE_OFF = 0, ECL_STATE_NORMAL, ECL_STATE_DEGRADED, ECL_STATE_FAULT } ecl_state_t;
typedef enum { ECL_VALID = 0, ECL_INVALID, ECL_STALE } ecl_validity_t;
typedef enum { ECL_CRASH_NONE = 0, ECL_CRASH_PENDING, ECL_CRASH_CONFIRMED } ecl_crash_t;
typedef enum { ECL_GEAR_P = 0, ECL_GEAR_N, ECL_GEAR_D, ECL_GEAR_R } ecl_gear_t;
typedef enum { ECL_SIDE_LEFT = 0, ECL_SIDE_RIGHT, ECL_SIDE_ALL } ecl_side_t;
typedef enum { ECL_ACTION_LOCK = 0, ECL_ACTION_UNLOCK } ecl_action_t;
typedef enum { ECL_SRC_PHYSICAL_BUTTON = 0, ECL_SRC_AVN, ECL_SRC_VOICE, ECL_SRC_MOBILE_APP } ecl_source_t;
typedef enum { ECL_DOOR_LEFT = 0, ECL_DOOR_RIGHT = 1 } ecl_door_t;

/** 입력 신호 인덱스 (validity 배열) */
typedef enum {
    ECL_SIG_SPEED = 0, ECL_SIG_GEAR, ECL_SIG_SRC_TS, ECL_SIG_CRASH,
    ECL_SIG_APPROACH_L, ECL_SIG_APPROACH_R, ECL_SIG_FIRE, ECL_SIG_OVERTEMP, ECL_SIG_ADULT,
    ECL_SIG_ISOFIX_L, ECL_SIG_ISOFIX_R, ECL_SIG_IGNITION, ECL_SIG_FAULT
} ecl_signal_t;

/** 이유코드 (SRS 부록 A, reason_code = priority_reason) */
typedef enum {
    RC_NONE = 0, RC_CRASH_CONFIRMED, RC_SENSOR_FAULT, RC_CRASH_PENDING_HOLD,
    RC_CRASH_PENDING_TIMEOUT, RC_INPUT_STALE, RC_INPUT_INVALID, RC_FIRE, RC_OVERTEMP,
    RC_ADULT_PRESENT, RC_APPROACH_RISK, RC_OVERRIDE_ACK, RC_ISOFIX, RC_IGNITION_OFF,
    RC_SPEED_AUTO_LOCK, RC_UNLOCK_DENIED_MOVING, RC_DRIVER_LOCK, RC_DRIVER_UNLOCK,
    RC_COMMAND_REJECTED
} ecl_reason_t;

/** 운전자 명령 원시 입력 (OEM-IF-004). 필드 누락은 ECL_ABSENT. */
typedef struct {
    int present;
    int side;
    int action;
    int source;
} ecl_command_t;

/** 스냅샷 원시 입력 (OEM-IF-001~004, 007~009). 누락: 정수 ECL_ABSENT, 속도 NaN. */
typedef struct {
    double timestamp_s;
    double source_timestamp_s;
    double vehicle_speed_kph;
    int gear;
    int crash_status;
    int rear_left_approach_risk;
    int rear_right_approach_risk;
    int fire_detected;
    int overtemperature_detected;
    int adult_present;
    int isofix_left;
    int isofix_right;
    int ignition_on;
    int sensor_fault;
    ecl_command_t command;
} ecl_snapshot_t;

/** 조건화된 입력 (VAL 출력). 효과 값은 SSR-020~025, 029에 따른 대체 값을 포함한다. */
typedef struct {
    ecl_validity_t validity[ECL_SIG_COUNT];
    int speed_moving;
    int crash_confirmed;
    ecl_reason_t hold_reason;
    int pending_timeout_first;
    int approach[ECL_DOOR_COUNT];
    int isofix[ECL_DOOR_COUNT];
    int fire;
    int overtemp;
    int adult;
    int ignition_off;
    int sensor_fault;
    int stale;
    int substituted;
    ecl_command_t command;
} ecl_inputs_t;

/** 명령 해석 결과 (CMD 출력) */
typedef struct {
    int present;
    int rejected;
    int valid;
    ecl_side_t side;
    ecl_action_t action;
    ecl_source_t source;
} ecl_cmd_result_t;

/** override 추적 결과 (OVR 출력) */
typedef struct {
    int suppress[ECL_DOOR_COUNT];
    int override[ECL_DOOR_COUNT];
    int ack[ECL_DOOR_COUNT];
} ecl_ovr_result_t;

/** 중재 결과 (ARB 출력) */
typedef struct {
    ecl_lock_t lock[ECL_DOOR_COUNT];
    ecl_reason_t reason_code;
    int command_rejected;
} ecl_decision_t;

/** 상태 응답 (OEM-IF-005, 006) */
typedef struct {
    ecl_lock_t lock[ECL_DOOR_COUNT];
    ecl_state_t state;
    ecl_reason_t reason_code;
    ecl_validity_t validity[ECL_SIG_COUNT];
    int override[ECL_DOOR_COUNT];
    int command_rejected;
} ecl_status_t;

/** 이벤트 (SRQ-FUN-029) */
typedef struct {
    uint64_t event_id;
    double timestamp_s;
    ecl_lock_t lock[ECL_DOOR_COUNT];
    ecl_state_t state;
    ecl_reason_t reason_code;
    int command_rejected;
    ecl_validity_t validity[ECL_SIG_COUNT];
} ecl_event_t;

#ifdef __cplusplus
}
#endif

#endif /* ECL_TYPES_H */
