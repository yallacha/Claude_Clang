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

/**
 * @brief   [목적] 첫 억제 후 10.0 s 이내 같은 명령은 override이다.
 * @tracing SRQ-FUN-017, SRQ-FUN-019
 * @par Input   approach LEFT, unlock(LEFT,AVN) t=1.0, 11.0
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   t=1.0 suppress=1/ack=0, t=11.0 ack=1/override=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 10.0 s 초과 재입력은 새 억제이다.
 * @tracing SRQ-FUN-020, SRQ-FUN-035
 * @par Input   unlock t=1.0, 11.01, 11.02
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   11.01 ack=0, 11.02 ack=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 접근위험 해소 시 override가 해제된다.
 * @tracing SRQ-FUN-034
 * @par Input   override 성립 후 approach=0
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   override=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 충돌 CONFIRMED가 다른 모든 조건보다 우선하여 두 도어를 RELEASE한다.
 * @tracing SSR-001, SSR-027
 * @par Input   crash_confirmed, approach LEFT, sensor_fault, prev LOCK/LOCK
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   RELEASE/RELEASE, RC_CRASH_CONFIRMED
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 유지 조건에서 직전 출력을 유지하되 접근위험 도어는 LOCK이다.
 * @tracing SSR-011, SSR-017
 * @par Input   hold_reason=SENSOR_FAULT, approach RIGHT, prev RELEASE/RELEASE
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   LEFT RELEASE, RIGHT LOCK
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 접근위험 LOCK이 화재 강제 해제보다 우선한다.
 * @tracing SSR-027, SRQ-FUN-010
 * @par Input   fire=1, approach LEFT
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   LEFT LOCK, RIGHT RELEASE, RC_APPROACH_RISK
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] override ack이면 이유코드가 RC_OVERRIDE_ACK이다.
 * @tracing SRQ-FUN-033
 * @par Input   approach LEFT, ack LEFT
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   LOCK 유지, RC_OVERRIDE_ACK
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 이동 중 LOCK이며 유효 unlock이면 거부 코드이다.
 * @tracing SRQ-FUN-004, SRQ-FUN-005, SRQ-FUN-009
 * @par Input   speed_moving=1, unlock ALL / 명령 없음
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   LOCK/LOCK, RC_UNLOCK_DENIED_MOVING / RC_SPEED_AUTO_LOCK
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 운전자 명령이 선택 도어에 적용되고 거절 시 RC_COMMAND_REJECTED이다.
 * @tracing SRQ-FUN-002, SRQ-FUN-003, SRQ-FUN-038
 * @par Input   unlock LEFT, 거절된 명령
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   LEFT RELEASE, RC_DRIVER_UNLOCK / command_rejected=1, RC_COMMAND_REJECTED
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] ISOFIX는 해당 도어만 LOCK하며 시동 OFF 해제보다 우선한다.
 * @tracing SRQ-FUN-012, SRQ-FUN-014
 * @par Input   isofix LEFT, ignition_off
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   LEFT LOCK, RIGHT RELEASE, RC_ISOFIX
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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
