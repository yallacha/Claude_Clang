// TDD 테스트: CMD, STA, LOG (RED → GREEN 로 작성한 구현 주도 테스트, 단위 테스트와 분리)
#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_cmd.h"
#include "ecl_log.h"
#include "ecl_sta.h"
}

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

TEST(TddCmd, AbsentCommandIsNotPresent)
{
    ecl_command_t raw = {0, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT};
    ecl_cmd_result_t out = {};
    ASSERT_EQ(RET_OK, ecl_cmd_interpret(&raw, &out));
    EXPECT_EQ(0, out.present);
    EXPECT_EQ(0, out.rejected);
    EXPECT_EQ(0, out.valid);
}

TEST(TddCmd, NullArgumentsReturnErrNull)
{
    ecl_command_t raw = {};
    ecl_cmd_result_t out = {};
    EXPECT_EQ(RET_ERR_NULL, ecl_cmd_interpret(nullptr, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_cmd_interpret(&raw, nullptr));
}

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
