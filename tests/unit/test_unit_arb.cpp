// 단위 테스트(SWE.4): CMP-ARB(ecl_arb). 우선순위 표 P1~P11과 MC/DC 100% 목표.
#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_arb.h"
}

namespace {
struct Ctx {
    ecl_inputs_t in;
    ecl_cmd_result_t cmd;
    ecl_ovr_result_t ovr;
    ecl_lock_t prev[2];
    Ctx() : in(), cmd(), ovr(), prev{ECL_RELEASE, ECL_RELEASE} {}
    void driver(int valid, ecl_side_t side, ecl_action_t action)
    {
        cmd.present = 1;
        cmd.valid = valid;
        cmd.rejected = (valid == 0) ? 1 : 0;
        cmd.side = side;
        cmd.action = action;
    }
    ecl_decision_t decide()
    {
        ecl_decision_t out = {};
        EXPECT_EQ(RET_OK, ecl_arb_decide(&in, &cmd, &ovr, prev, &out));
        return out;
    }
};
}  // namespace

/**
 * @brief   [목적] 모든 인자가 NULL이면 각각 RET_ERR_NULL을 반환한다(인자별 독립).
 * @tracing SRQ-QUA-REL-001
 * @par Input   in, cmd, ovr, prev, out을 하나씩 nullptr
 * @par Procedure  1. 각 호출
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, NullArgumentsReturnErrNull)
{
    Ctx c;
    ecl_decision_t out = {};
    EXPECT_EQ(RET_ERR_NULL, ecl_arb_decide(nullptr, &c.cmd, &c.ovr, c.prev, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_arb_decide(&c.in, nullptr, &c.ovr, c.prev, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_arb_decide(&c.in, &c.cmd, nullptr, c.prev, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_arb_decide(&c.in, &c.cmd, &c.ovr, nullptr, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_arb_decide(&c.in, &c.cmd, &c.ovr, c.prev, nullptr));
}

/**
 * @brief   [목적] P1 충돌 CONFIRMED는 sensor_fault, 접근위험, 운전자 명령보다 우선하여 두 도어를 RELEASE한다.
 * @tracing SSR-001, SSR-002, SSR-027
 * @par Input   crash_confirmed=1, hold_reason=SENSOR_FAULT, approach 양쪽, prev LOCK/LOCK, lock 명령
 * @par Procedure  1. decide
 * @par Expected   lock=RELEASE/RELEASE, reason=RC_CRASH_CONFIRMED
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, CrashConfirmedReleasesBothAboveAll)
{
    Ctx c;
    c.in.crash_confirmed = 1;
    c.in.hold_reason = RC_SENSOR_FAULT;
    c.in.approach[0] = c.in.approach[1] = 1;
    c.prev[0] = c.prev[1] = ECL_LOCK;
    c.driver(1, ECL_SIDE_ALL, ECL_ACTION_LOCK);
    const ecl_decision_t d = c.decide();
    EXPECT_EQ(ECL_RELEASE, d.lock[0]);
    EXPECT_EQ(ECL_RELEASE, d.lock[1]);
    EXPECT_EQ(RC_CRASH_CONFIRMED, d.reason_code);
}

/**
 * @brief   [목적] P2~P4 유지: 직전 출력을 유지하고 hold_reason을 이유코드로 하되 접근위험 도어는 LOCK이다.
 * @tracing SSR-003, SSR-010, SSR-011, SSR-016, SSR-017, SSR-024
 * @par Input   hold_reason ∈ {SENSOR_FAULT, CRASH_PENDING_HOLD, INPUT_STALE, INPUT_INVALID}, prev RELEASE/LOCK, approach[RIGHT]=1 또는 0
 * @par Procedure  1. 각 hold_reason으로 decide
 * @par Expected   approach 없음: lock=prev. approach[RIGHT]=1이면 RIGHT=LOCK. reason=hold_reason
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, HoldKeepsPreviousExceptApproachDoorLocks)
{
    const ecl_reason_t holds[4] = {RC_SENSOR_FAULT, RC_CRASH_PENDING_HOLD, RC_INPUT_STALE, RC_INPUT_INVALID};
    for (int i = 0; i < 4; i++) {
        Ctx c;
        c.in.hold_reason = holds[i];
        c.prev[0] = ECL_RELEASE;
        c.prev[1] = ECL_LOCK;
        c.driver(1, ECL_SIDE_ALL, ECL_ACTION_UNLOCK);
        ecl_decision_t d = c.decide();
        EXPECT_EQ(ECL_RELEASE, d.lock[0]);
        EXPECT_EQ(ECL_LOCK, d.lock[1]);
        EXPECT_EQ(holds[i], d.reason_code);
        c.prev[1] = ECL_RELEASE;
        c.in.approach[1] = 1;
        d = c.decide();
        EXPECT_EQ(ECL_RELEASE, d.lock[0]);
        EXPECT_EQ(ECL_LOCK, d.lock[1]);
    }
}

/**
 * @brief   [목적] P5 접근위험: 해당 도어만 LOCK, 해제 억제, 이유코드 RC_APPROACH_RISK(override ack이면 RC_OVERRIDE_ACK).
 * @tracing SSR-011, SSR-012, SSR-013, SSR-014, SRQ-FUN-033
 * @par Input   approach[LEFT]=1, unlock(ALL) 명령, ack 0/1
 * @par Procedure  1. decide
 * @par Expected   LEFT=LOCK, RIGHT=RELEASE(명령). reason: ack 0→RC_APPROACH_RISK, ack 1→RC_OVERRIDE_ACK
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, ApproachRiskLocksOnlyThatDoor)
{
    Ctx c;
    c.in.approach[0] = 1;
    c.prev[0] = ECL_LOCK;
    c.prev[1] = ECL_LOCK;
    c.driver(1, ECL_SIDE_ALL, ECL_ACTION_UNLOCK);
    ecl_decision_t d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(ECL_RELEASE, d.lock[1]);
    EXPECT_EQ(RC_APPROACH_RISK, d.reason_code);
    c.ovr.ack[0] = 1;
    d = c.decide();
    EXPECT_EQ(RC_OVERRIDE_ACK, d.reason_code);
}

/**
 * @brief   [목적] P5(접근위험)가 P6(강제 해제)보다 우선하고, P6는 화재·과온·성인 각각 단독으로 RELEASE하며 코드 우선순위는 fire > overtemp > adult이다.
 * @tracing SRQ-FUN-010, SRQ-FUN-011, SRQ-FUN-032, SSR-027
 * @par Input   fire/overtemp/adult 단독 및 fire+adult, approach 유무
 * @par Procedure  1. 각 입력 조합으로 decide
 * @par Expected   단독: RELEASE와 RC_FIRE/RC_OVERTEMP/RC_ADULT_PRESENT. fire+adult→RC_FIRE. approach[LEFT]=1+fire → LEFT=LOCK, RIGHT=RELEASE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, ForcedReleaseReasonsAndApproachPrecedence)
{
    Ctx c;
    c.prev[0] = c.prev[1] = ECL_LOCK;
    c.in.fire = 1;
    ecl_decision_t d = c.decide();
    EXPECT_EQ(ECL_RELEASE, d.lock[0]);
    EXPECT_EQ(ECL_RELEASE, d.lock[1]);
    EXPECT_EQ(RC_FIRE, d.reason_code);
    c.in.adult = 1;
    EXPECT_EQ(RC_FIRE, c.decide().reason_code);
    c.in.fire = 0;
    EXPECT_EQ(RC_ADULT_PRESENT, c.decide().reason_code);
    c.in.adult = 0;
    c.in.overtemp = 1;
    EXPECT_EQ(RC_OVERTEMP, c.decide().reason_code);
    c.in.overtemp = 0;
    c.in.adult = 1;
    EXPECT_EQ(RC_ADULT_PRESENT, c.decide().reason_code);
    c.in.fire = 1;
    c.in.adult = 0;
    c.in.approach[0] = 1;
    d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(ECL_RELEASE, d.lock[1]);
    EXPECT_EQ(RC_APPROACH_RISK, d.reason_code);
}

/**
 * @brief   [목적] P7 ISOFIX는 TRUE인 쪽만 LOCK하고(이유코드 RC_ISOFIX) 반대쪽은 변경하지 않는다.
 * @tracing SRQ-FUN-012, SRQ-FUN-013
 * @par Input   isofix[LEFT]=1, prev RELEASE/LOCK
 * @par Procedure  1. decide
 * @par Expected   LEFT=LOCK, RIGHT=LOCK(직전 유지), reason=RC_ISOFIX. prev RELEASE이면 RIGHT=RELEASE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, IsofixLocksOnlyThatDoor)
{
    Ctx c;
    c.in.isofix[0] = 1;
    c.prev[1] = ECL_LOCK;
    ecl_decision_t d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(ECL_LOCK, d.lock[1]);
    EXPECT_EQ(RC_ISOFIX, d.reason_code);
    c.prev[1] = ECL_RELEASE;
    EXPECT_EQ(ECL_RELEASE, c.decide().lock[1]);
}

/**
 * @brief   [목적] P8 이동 중 LOCK과 unlock 거부 이유코드: 유효 unlock이면 RC_UNLOCK_DENIED_MOVING, 유효 lock/무효 명령/명령 없음이면 RC_SPEED_AUTO_LOCK.
 * @tracing SRQ-FUN-004, SRQ-FUN-005, SRQ-FUN-009
 * @par Input   speed_moving=1과 명령 4종(unlock 유효, lock 유효, unlock 무효, 없음)
 * @par Procedure  1. 각 명령으로 decide
 * @par Expected   lock=LOCK/LOCK. reason: DENIED / AUTO_LOCK / AUTO_LOCK / AUTO_LOCK
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, MovingLocksAndDeniesUnlock)
{
    Ctx c;
    c.in.speed_moving = 1;
    c.driver(1, ECL_SIDE_ALL, ECL_ACTION_UNLOCK);
    ecl_decision_t d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(ECL_LOCK, d.lock[1]);
    EXPECT_EQ(RC_UNLOCK_DENIED_MOVING, d.reason_code);
    c.driver(1, ECL_SIDE_ALL, ECL_ACTION_LOCK);
    EXPECT_EQ(RC_SPEED_AUTO_LOCK, c.decide().reason_code);
    c.driver(0, ECL_SIDE_ALL, ECL_ACTION_UNLOCK);
    EXPECT_EQ(RC_SPEED_AUTO_LOCK, c.decide().reason_code);
    c = Ctx();
    c.in.speed_moving = 1;
    EXPECT_EQ(RC_SPEED_AUTO_LOCK, c.decide().reason_code);
}

/**
 * @brief   [목적] P8(속도 LOCK)이 P9(시동 OFF 해제)보다 우선하고, P9는 상위 규칙이 없을 때 두 도어를 RELEASE한다.
 * @tracing SRQ-FUN-014, SRQ-FUN-016, SSR-027
 * @par Input   ignition_off=1 (speed_moving 0/1)
 * @par Procedure  1. decide
 * @par Expected   speed 0: RELEASE, RC_IGNITION_OFF. speed 1: LOCK, RC_SPEED_AUTO_LOCK
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, IgnitionOffReleasesUnlessMoving)
{
    Ctx c;
    c.prev[0] = c.prev[1] = ECL_LOCK;
    c.in.ignition_off = 1;
    ecl_decision_t d = c.decide();
    EXPECT_EQ(ECL_RELEASE, d.lock[0]);
    EXPECT_EQ(ECL_RELEASE, d.lock[1]);
    EXPECT_EQ(RC_IGNITION_OFF, d.reason_code);
    c.in.speed_moving = 1;
    d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(RC_SPEED_AUTO_LOCK, d.reason_code);
}

/**
 * @brief   [목적] P10 운전자 명령: side(LEFT/RIGHT/ALL)와 action(lock/unlock)대로 선택 도어에만 적용하고 무효 명령은 적용하지 않는다.
 * @tracing SRQ-FUN-002, SRQ-FUN-003, SRQ-FUN-006
 * @par Input   lock/unlock × LEFT/RIGHT/ALL, 무효 명령, 명령 없음
 * @par Procedure  1. decide
 * @par Expected   선택 도어만 변경, 나머지는 prev 유지. reason=RC_DRIVER_LOCK/RC_DRIVER_UNLOCK. 무효: 변경 없음
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, DriverCommandAppliesToSelectedDoorsOnly)
{
    Ctx c;
    c.driver(1, ECL_SIDE_LEFT, ECL_ACTION_LOCK);
    ecl_decision_t d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(ECL_RELEASE, d.lock[1]);
    EXPECT_EQ(RC_DRIVER_LOCK, d.reason_code);
    c.driver(1, ECL_SIDE_RIGHT, ECL_ACTION_LOCK);
    d = c.decide();
    EXPECT_EQ(ECL_RELEASE, d.lock[0]);
    EXPECT_EQ(ECL_LOCK, d.lock[1]);
    c.driver(1, ECL_SIDE_ALL, ECL_ACTION_LOCK);
    d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(ECL_LOCK, d.lock[1]);
    c.prev[0] = c.prev[1] = ECL_LOCK;
    c.driver(1, ECL_SIDE_RIGHT, ECL_ACTION_UNLOCK);
    d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(ECL_RELEASE, d.lock[1]);
    EXPECT_EQ(RC_DRIVER_UNLOCK, d.reason_code);
    c.driver(0, ECL_SIDE_ALL, ECL_ACTION_UNLOCK);
    d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(ECL_LOCK, d.lock[1]);
    c = Ctx();
    EXPECT_EQ(RC_NONE, c.decide().reason_code);
}

/**
 * @brief   [목적] 명령 거절 시 command_rejected=1이며, 상위 규칙이 없을 때만 이유코드가 RC_COMMAND_REJECTED이다.
 * @tracing SRQ-FUN-008, SRQ-FUN-038
 * @par Input   rejected 명령 + (없음 / ignition_off / crash_confirmed)
 * @par Procedure  1. decide
 * @par Expected   없음: RC_COMMAND_REJECTED. 그 외: 상위 규칙 코드, command_rejected=1은 유지
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, RejectedCommandReasonOnlyWhenNoHigherRule)
{
    Ctx c;
    c.driver(0, ECL_SIDE_ALL, ECL_ACTION_LOCK);
    ecl_decision_t d = c.decide();
    EXPECT_EQ(1, d.command_rejected);
    EXPECT_EQ(RC_COMMAND_REJECTED, d.reason_code);
    c.in.ignition_off = 1;
    d = c.decide();
    EXPECT_EQ(1, d.command_rejected);
    EXPECT_EQ(RC_IGNITION_OFF, d.reason_code);
    c.in.crash_confirmed = 1;
    d = c.decide();
    EXPECT_EQ(1, d.command_rejected);
    EXPECT_EQ(RC_CRASH_CONFIRMED, d.reason_code);
    c = Ctx();
    EXPECT_EQ(0, c.decide().command_rejected);
}

/**
 * @brief   [목적] PENDING 타임아웃 첫 판정 평가는 이유코드 RC_CRASH_PENDING_TIMEOUT이 P5~P10 규칙 코드보다 우선한다(출력은 하위 규칙대로).
 * @tracing SSR-031, SSR-004
 * @par Input   pending_timeout_first=1 + approach[LEFT]=1 또는 규칙 없음
 * @par Procedure  1. decide
 * @par Expected   reason=RC_CRASH_PENDING_TIMEOUT. approach 있으면 LEFT=LOCK
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, PendingTimeoutReasonBeatsLowerRules)
{
    Ctx c;
    c.in.pending_timeout_first = 1;
    EXPECT_EQ(RC_CRASH_PENDING_TIMEOUT, c.decide().reason_code);
    c.in.approach[0] = 1;
    const ecl_decision_t d = c.decide();
    EXPECT_EQ(ECL_LOCK, d.lock[0]);
    EXPECT_EQ(RC_CRASH_PENDING_TIMEOUT, d.reason_code);
}

/**
 * @brief   [목적] 두 도어의 이유코드 병합: 순위가 높은 코드를 택하고, 같은 순위면 값이 작은 코드를 택한다(MC/DC: 3조건 독립).
 * @tracing SSR-027
 * @par Input   (LEFT ISOFIX, RIGHT 접근위험), (LEFT 접근위험, RIGHT ISOFIX), (LEFT APPROACH, RIGHT OVERRIDE_ACK), 반대, 동일 코드
 * @par Procedure  1. decide
 * @par Expected   (ISOFIX,접근)→APPROACH, (접근,ISOFIX)→APPROACH, 같은 순위→값이 작은 RC_APPROACH_RISK, 동일→동일
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ArbUnit, MergesReasonsByRankThenValue)
{
    Ctx c;
    c.in.isofix[0] = 1;
    c.in.approach[1] = 1;
    EXPECT_EQ(RC_APPROACH_RISK, c.decide().reason_code);
    c = Ctx();
    c.in.approach[0] = 1;
    c.in.isofix[1] = 1;
    EXPECT_EQ(RC_APPROACH_RISK, c.decide().reason_code);
    c = Ctx();
    c.in.approach[0] = c.in.approach[1] = 1;
    c.ovr.ack[1] = 1;
    EXPECT_EQ(RC_APPROACH_RISK, c.decide().reason_code);
    c = Ctx();
    c.in.approach[0] = c.in.approach[1] = 1;
    c.ovr.ack[0] = 1;
    EXPECT_EQ(RC_APPROACH_RISK, c.decide().reason_code);
    c = Ctx();
    c.in.approach[0] = c.in.approach[1] = 1;
    EXPECT_EQ(RC_APPROACH_RISK, c.decide().reason_code);
}
