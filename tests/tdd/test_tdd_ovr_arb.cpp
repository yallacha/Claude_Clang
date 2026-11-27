// TDD 테스트: OVR, ARB
#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_arb.h"
#include "ecl_ovr.h"
}

namespace {
ecl_cmd_result_t unlock_cmd(ecl_side_t side, ecl_source_t src)
{
    ecl_cmd_result_t c = {};
    c.present = 1;
    c.valid = 1;
    c.side = side;
    c.action = ECL_ACTION_UNLOCK;
    c.source = src;
    return c;
}
}  // namespace

TEST(TddOvr, FirstSuppressionThenOverrideWithinWindow)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    ecl_cmd_result_t cmd = unlock_cmd(ECL_SIDE_LEFT, ECL_SRC_AVN);
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    EXPECT_EQ(1, out.suppress[ECL_DOOR_LEFT]);
    EXPECT_EQ(0, out.ack[ECL_DOOR_LEFT]);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 11.0, &in, &cmd, &out));
    EXPECT_EQ(1, out.ack[ECL_DOOR_LEFT]);
    EXPECT_EQ(1, out.override[ECL_DOOR_LEFT]);
}

TEST(TddOvr, ReinputAfterWindowIsNewSuppression)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    ecl_cmd_result_t cmd = unlock_cmd(ECL_SIDE_LEFT, ECL_SRC_AVN);
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 11.01, &in, &cmd, &out));
    EXPECT_EQ(0, out.ack[ECL_DOOR_LEFT]);
    EXPECT_EQ(0, out.override[ECL_DOOR_LEFT]);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 11.02, &in, &cmd, &out));
    EXPECT_EQ(1, out.ack[ECL_DOOR_LEFT]);
}

TEST(TddOvr, ClearsWhenApproachRiskResolves)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    ecl_cmd_result_t cmd = unlock_cmd(ECL_SIDE_ALL, ECL_SRC_VOICE);
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 2.0, &in, &cmd, &out));
    ASSERT_EQ(1, out.override[ECL_DOOR_LEFT]);
    in.approach[ECL_DOOR_LEFT] = 0;
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 3.0, &in, &cmd, &out));
    EXPECT_EQ(0, out.override[ECL_DOOR_LEFT]);
}

TEST(TddArb, CrashConfirmedReleasesBothAboveEverything)
{
    ecl_inputs_t in = {};
    ecl_cmd_result_t cmd = {};
    ecl_ovr_result_t ovr = {};
    ecl_lock_t prev[2] = {ECL_LOCK, ECL_LOCK};
    ecl_decision_t out = {};
    in.crash_confirmed = 1;
    in.approach[0] = 1;
    in.sensor_fault = 1;
    in.hold_reason = RC_SENSOR_FAULT;
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(ECL_RELEASE, out.lock[0]);
    EXPECT_EQ(ECL_RELEASE, out.lock[1]);
    EXPECT_EQ(RC_CRASH_CONFIRMED, out.reason_code);
}

TEST(TddArb, HoldKeepsPreviousButApproachDoorLocks)
{
    ecl_inputs_t in = {};
    ecl_cmd_result_t cmd = {};
    ecl_ovr_result_t ovr = {};
    ecl_lock_t prev[2] = {ECL_RELEASE, ECL_RELEASE};
    ecl_decision_t out = {};
    in.hold_reason = RC_SENSOR_FAULT;
    in.approach[ECL_DOOR_RIGHT] = 1;
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(ECL_RELEASE, out.lock[ECL_DOOR_LEFT]);
    EXPECT_EQ(ECL_LOCK, out.lock[ECL_DOOR_RIGHT]);
    EXPECT_EQ(RC_SENSOR_FAULT, out.reason_code);
}

TEST(TddArb, ApproachRiskBeatsFireRelease)
{
    ecl_inputs_t in = {};
    ecl_cmd_result_t cmd = {};
    ecl_ovr_result_t ovr = {};
    ecl_lock_t prev[2] = {ECL_RELEASE, ECL_RELEASE};
    ecl_decision_t out = {};
    in.fire = 1;
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(ECL_LOCK, out.lock[ECL_DOOR_LEFT]);
    EXPECT_EQ(ECL_RELEASE, out.lock[ECL_DOOR_RIGHT]);
    EXPECT_EQ(RC_APPROACH_RISK, out.reason_code);
}

TEST(TddArb, OverrideAckSelectsOverrideReason)
{
    ecl_inputs_t in = {};
    ecl_cmd_result_t cmd = unlock_cmd(ECL_SIDE_LEFT, ECL_SRC_AVN);
    ecl_ovr_result_t ovr = {};
    ecl_lock_t prev[2] = {ECL_LOCK, ECL_LOCK};
    ecl_decision_t out = {};
    in.approach[ECL_DOOR_LEFT] = 1;
    ovr.ack[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(ECL_LOCK, out.lock[ECL_DOOR_LEFT]);
    EXPECT_EQ(RC_OVERRIDE_ACK, out.reason_code);
}

TEST(TddArb, SpeedLocksAndDeniesUnlock)
{
    ecl_inputs_t in = {};
    ecl_cmd_result_t cmd = unlock_cmd(ECL_SIDE_ALL, ECL_SRC_VOICE);
    ecl_ovr_result_t ovr = {};
    ecl_lock_t prev[2] = {ECL_RELEASE, ECL_RELEASE};
    ecl_decision_t out = {};
    in.speed_moving = 1;
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(ECL_LOCK, out.lock[0]);
    EXPECT_EQ(ECL_LOCK, out.lock[1]);
    EXPECT_EQ(RC_UNLOCK_DENIED_MOVING, out.reason_code);
    cmd.valid = 0;
    cmd.present = 0;
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(RC_SPEED_AUTO_LOCK, out.reason_code);
}

TEST(TddArb, DriverLockUnlockAndRejectedReason)
{
    ecl_inputs_t in = {};
    ecl_cmd_result_t cmd = unlock_cmd(ECL_SIDE_LEFT, ECL_SRC_AVN);
    ecl_ovr_result_t ovr = {};
    ecl_lock_t prev[2] = {ECL_LOCK, ECL_LOCK};
    ecl_decision_t out = {};
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(ECL_RELEASE, out.lock[ECL_DOOR_LEFT]);
    EXPECT_EQ(ECL_LOCK, out.lock[ECL_DOOR_RIGHT]);
    EXPECT_EQ(RC_DRIVER_UNLOCK, out.reason_code);
    cmd.valid = 0;
    cmd.rejected = 1;
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(1, out.command_rejected);
    EXPECT_EQ(RC_COMMAND_REJECTED, out.reason_code);
}

TEST(TddArb, IsofixLocksOnlyThatDoorAndIgnitionOffReleases)
{
    ecl_inputs_t in = {};
    ecl_cmd_result_t cmd = {};
    ecl_ovr_result_t ovr = {};
    ecl_lock_t prev[2] = {ECL_RELEASE, ECL_LOCK};
    ecl_decision_t out = {};
    in.isofix[ECL_DOOR_LEFT] = 1;
    in.ignition_off = 1;
    ASSERT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
    EXPECT_EQ(ECL_LOCK, out.lock[ECL_DOOR_LEFT]);
    EXPECT_EQ(ECL_RELEASE, out.lock[ECL_DOOR_RIGHT]);
    EXPECT_EQ(RC_ISOFIX, out.reason_code);
}
