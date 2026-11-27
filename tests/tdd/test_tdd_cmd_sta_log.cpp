// TDD 테스트: CMD, STA, LOG (RED → GREEN 로 작성한 구현 주도 테스트, 단위 테스트와 분리)
#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_cmd.h"
#include "ecl_log.h"
#include "ecl_sta.h"
}

/**
 * @brief   [목적] 모든 필드가 등록 범위인 명령을 유효로 판정한다.
 * @tracing SRQ-FUN-001
 * @par Input   present=1, side=LEFT, action=LOCK, source=AVN
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   valid=1, rejected=0, side=LEFT
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddCmd, AcceptsFullyValidCommand)
{
    ecl_command_t raw = {1, ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_AVN};
    ecl_cmd_result_t out = {};
    ASSERT_EQ(RET_OK, ecl_cmd_interpret(&raw, &out));
    EXPECT_EQ(1, out.present);
    EXPECT_EQ(1, out.valid);
    EXPECT_EQ(0, out.rejected);
    EXPECT_EQ(ECL_SIDE_LEFT, out.side);
}

/**
 * @brief   [목적] 누락·범위 밖 필드가 있는 명령을 거절한다.
 * @tracing SRQ-FUN-007
 * @par Input   side 누락, side=3, source=4
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   rejected=1, valid=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddCmd, RejectsMissingAndUnregisteredFields)
{
    ecl_cmd_result_t out = {};
    ecl_command_t missing = {1, ECL_ABSENT, ECL_ACTION_LOCK, ECL_SRC_AVN};
    ecl_command_t badside = {1, 3, ECL_ACTION_LOCK, ECL_SRC_AVN};
    ecl_command_t badsrc = {1, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, 4};
    ASSERT_EQ(RET_OK, ecl_cmd_interpret(&missing, &out));
    EXPECT_EQ(1, out.rejected);
    EXPECT_EQ(0, out.valid);
    ASSERT_EQ(RET_OK, ecl_cmd_interpret(&badside, &out));
    EXPECT_EQ(1, out.rejected);
    ASSERT_EQ(RET_OK, ecl_cmd_interpret(&badsrc, &out));
    EXPECT_EQ(1, out.rejected);
}

/**
 * @brief   [목적] 명령 없음(present=0)은 수락도 거절도 아니다.
 * @tracing SRQ-FUN-007
 * @par Input   present=0
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   present=0, valid=0, rejected=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddCmd, AbsentCommandIsNotPresent)
{
    ecl_command_t raw = {0, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT};
    ecl_cmd_result_t out = {};
    ASSERT_EQ(RET_OK, ecl_cmd_interpret(&raw, &out));
    EXPECT_EQ(0, out.present);
    EXPECT_EQ(0, out.rejected);
    EXPECT_EQ(0, out.valid);
}

/**
 * @brief   [목적] NULL 인자는 RET_ERR_NULL이다.
 * @tracing SRQ-QUA-REL-001
 * @par Input   raw=nullptr / out=nullptr
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddCmd, NullArgumentsReturnErrNull)
{
    ecl_command_t raw = {};
    ecl_cmd_result_t out = {};
    EXPECT_EQ(RET_ERR_NULL, ecl_cmd_interpret(nullptr, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_cmd_interpret(&raw, nullptr));
}

/**
 * @brief   [목적] state 우선순위 FAULT > DEGRADED > OFF > NORMAL.
 * @tracing SSR-028, SSR-030
 * @par Input   ignition_off, stale, sensor_fault를 차례로 설정
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   NORMAL, OFF, DEGRADED, FAULT 순으로 바뀐다
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddSta, PriorityFaultDegradedOffNormal)
{
    ecl_inputs_t in = {};
    ecl_state_t st = ECL_STATE_OFF;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_NORMAL, st);
    in.ignition_off = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_OFF, st);
    in.stale = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_DEGRADED, st);
    in.sensor_fault = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_FAULT, st);
}

/**
 * @brief   [목적] 이벤트 ID가 1부터 증가하고 오름차순으로 조회된다.
 * @tracing SRQ-FUN-028, SRQ-FUN-030
 * @par Input   이벤트 2건 기록
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   ID 1, 2 오름차순
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddLog, AssignsIncreasingIdsAndQueriesAscending)
{
    ecl_log_t log;
    ecl_event_t e = {};
    ecl_event_t out[4] = {};
    uint64_t id = 99;
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(1U, id);
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(2U, id);
    ASSERT_EQ(RET_OK, ecl_log_query(&log, out, 4, &n));
    ASSERT_EQ(2U, n);
    EXPECT_EQ(1U, out[0].event_id);
    EXPECT_EQ(2U, out[1].event_id);
}

/**
 * @brief   [목적] 101건 기록 후 최신 100건만 보존된다.
 * @tracing SRQ-LIM-003
 * @par Input   101건 기록
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   100건, 첫 ID 2, 마지막 ID 101
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddLog, KeepsLatestHundredOldestDropped)
{
    ecl_log_t log;
    ecl_event_t e = {};
    static ecl_event_t out[ECL_EVENT_CAPACITY];
    uint64_t id = 0;
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    for (int i = 0; i < 101; i++) {
        ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    }
    ASSERT_EQ(RET_OK, ecl_log_query(&log, out, ECL_EVENT_CAPACITY, &n));
    ASSERT_EQ(100U, n);
    EXPECT_EQ(2U, out[0].event_id);
    EXPECT_EQ(101U, out[99].event_id);
}

/**
 * @brief   [목적] ID가 최댓값에 도달하면 이후 기록하지 않는다.
 * @tracing SRQ-FUN-037, SRQ-FUN-039
 * @par Input   카운터를 UINT64_MAX-1로 설정 후 2건
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   첫 ID=UINT64_MAX, 두 번째 ID=0, 보존 1건
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddLog, StopsRecordingAfterMaxId)
{
    ecl_log_t log;
    ecl_event_t e = {};
    ecl_event_t out[4] = {};
    uint64_t id = 0;
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    ASSERT_EQ(RET_OK, ecl_log_test_set_counter(&log, UINT64_MAX - 1U));
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(UINT64_MAX, id);
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(0U, id);
    ASSERT_EQ(RET_OK, ecl_log_query(&log, out, 4, &n));
    EXPECT_EQ(1U, n);
}
