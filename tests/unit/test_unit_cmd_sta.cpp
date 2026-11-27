// 단위 테스트(SWE.4): CMP-CMD(ecl_cmd), CMP-STA(ecl_sta). MC/DC 100% 목표, 단위 테스트만으로 측정한다.
#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_cmd.h"
#include "ecl_sta.h"
}

namespace {
ecl_cmd_result_t interpret(int present, int side, int action, int source)
{
    ecl_command_t raw = {present, side, action, source};
    ecl_cmd_result_t out;
    out.present = -1;
    EXPECT_EQ(RET_OK, ecl_cmd_interpret(&raw, &out));
    return out;
}
}  // namespace

/**
 * @brief   [목적] 모든 필드가 등록 범위이면 명령을 유효로 판정한다.
 * @tracing SRQ-FUN-001
 * @par Input   present=1, side=LEFT, action=LOCK, source=AVN
 * @par Procedure  1. 유효 필드로 ecl_cmd_interpret 호출.
 * @par Expected   valid=1, rejected=0, side/action/source가 입력과 같다.
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CmdUnit, ValidCommandIsAccepted)
{
    const ecl_cmd_result_t r = interpret(1, ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_AVN);
    EXPECT_EQ(1, r.valid);
    EXPECT_EQ(0, r.rejected);
    EXPECT_EQ(ECL_SIDE_LEFT, r.side);
    EXPECT_EQ(ECL_ACTION_LOCK, r.action);
    EXPECT_EQ(ECL_SRC_AVN, r.source);
}

/**
 * @brief   [목적] 24개 조합(4 source × 3 side × 2 action) 전부 유효로 판정한다(경계값 min/max 포함).
 * @tracing SRQ-FUN-001
 * @par Input   side 0..2, action 0..1, source 0..3 전 조합
 * @par Procedure  1. 모든 조합으로 호출한다.
 * @par Expected   모두 valid=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CmdUnit, AllTwentyFourCombinationsAreValid)
{
    for (int side = ECL_SIDE_LEFT; side <= ECL_SIDE_ALL; side++) {
        for (int action = ECL_ACTION_LOCK; action <= ECL_ACTION_UNLOCK; action++) {
            for (int src = ECL_SRC_PHYSICAL_BUTTON; src <= ECL_SRC_MOBILE_APP; src++) {
                EXPECT_EQ(1, interpret(1, side, action, src).valid);
            }
        }
    }
}

/**
 * @brief   [목적] side가 누락/하한 미만/상한 초과이면 거절한다(MC/DC: side 조건 3개 독립).
 * @tracing SRQ-FUN-007, SRQ-FUN-031
 * @par Input   side = ECL_ABSENT, -1, 3 (action, source 유효)
 * @par Procedure  1. 각 값으로 호출한다.
 * @par Expected   rejected=1, valid=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CmdUnit, RejectsInvalidSide)
{
    const int bad[3] = {ECL_ABSENT, -1, 3};
    for (int i = 0; i < 3; i++) {
        const ecl_cmd_result_t r = interpret(1, bad[i], ECL_ACTION_LOCK, ECL_SRC_AVN);
        EXPECT_EQ(1, r.rejected);
        EXPECT_EQ(0, r.valid);
    }
}

/**
 * @brief   [목적] action이 누락/범위 밖이면 거절한다(side는 유효).
 * @tracing SRQ-FUN-007
 * @par Input   action = ECL_ABSENT, -1, 2
 * @par Procedure  1. 각 값으로 호출한다.
 * @par Expected   rejected=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CmdUnit, RejectsInvalidAction)
{
    const int bad[3] = {ECL_ABSENT, -1, 2};
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(1, interpret(1, ECL_SIDE_ALL, bad[i], ECL_SRC_AVN).rejected);
    }
}

/**
 * @brief   [목적] source가 누락/미등록이면 거절한다(side, action은 유효).
 * @tracing SRQ-FUN-007
 * @par Input   source = ECL_ABSENT, -1, 4
 * @par Procedure  1. 각 값으로 호출한다.
 * @par Expected   rejected=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CmdUnit, RejectsInvalidSource)
{
    const int bad[3] = {ECL_ABSENT, -1, 4};
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(1, interpret(1, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, bad[i]).rejected);
    }
}

/**
 * @brief   [목적] 명령이 없으면(present=0) 수락도 거절도 아니고, present가 0/1이 아닌 값이면 형식 오류로 거절한다(DEC-011).
 * @tracing SRQ-FUN-007
 * @par Input   present = 0 / 2, -1 (나머지 필드 유효)
 * @par Procedure  1. 각 값으로 호출한다.
 * @par Expected   present=0: present=0, valid=0, rejected=0. present=2/-1: present=1, valid=0, rejected=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CmdUnit, AbsentCommandIsNeitherValidNorRejected)
{
    const ecl_cmd_result_t none = interpret(0, ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_AVN);
    EXPECT_EQ(0, none.present);
    EXPECT_EQ(0, none.valid);
    EXPECT_EQ(0, none.rejected);
    const int bad[2] = {2, -1};
    for (int i = 0; i < 2; i++) {
        const ecl_cmd_result_t r = interpret(bad[i], ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_AVN);
        EXPECT_EQ(1, r.present);
        EXPECT_EQ(0, r.valid);
        EXPECT_EQ(1, r.rejected);
    }
}

/**
 * @brief   [목적] NULL 인자에 RET_ERR_NULL을 반환한다(두 인자 각각 독립).
 * @tracing SRQ-QUA-REL-001
 * @par Input   (nullptr, out), (raw, nullptr)
 * @par Procedure  1. 각각 호출한다.
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CmdUnit, NullArgumentsReturnErrNull)
{
    ecl_command_t raw = {1, 0, 0, 0};
    ecl_cmd_result_t out = {};
    EXPECT_EQ(RET_ERR_NULL, ecl_cmd_interpret(nullptr, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_cmd_interpret(&raw, nullptr));
}

/**
 * @brief   [목적] state 우선순위 FAULT > DEGRADED > OFF > NORMAL을 조건표대로 결정한다.
 * @tracing SSR-028, SSR-030, SSR-018, SSR-026, SRQ-FUN-015, SSR-034
 * @par Input   sensor_fault / stale / substituted / ignition_off 조합
 * @par Procedure  1. 조건 조합을 설정하고 ecl_sta_decide 호출.
 * @par Expected   fault→FAULT, stale→DEGRADED, substituted→DEGRADED, ignition_off→OFF, 없음→NORMAL, fault+off→FAULT
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaUnit, DecidesStateByPriorityTable)
{
    ecl_state_t st = ECL_STATE_OFF;
    ecl_inputs_t in = {};

    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_NORMAL, st);

    in = {};
    in.sensor_fault = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_FAULT, st);

    in = {};
    in.stale = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_DEGRADED, st);

    in = {};
    in.substituted = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_DEGRADED, st);

    in = {};
    in.ignition_off = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_OFF, st);

    in = {};
    in.sensor_fault = 1;
    in.ignition_off = 1;
    in.stale = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_FAULT, st);

    in = {};
    in.stale = 1;
    in.ignition_off = 1;
    ASSERT_EQ(RET_OK, ecl_sta_decide(&in, &st));
    EXPECT_EQ(ECL_STATE_DEGRADED, st);
}

/**
 * @brief   [목적] NULL 인자에 RET_ERR_NULL을 반환한다.
 * @tracing SRQ-QUA-REL-001
 * @par Input   (nullptr, &st), (&in, nullptr)
 * @par Procedure  1. 각각 호출한다.
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaUnit, NullArgumentsReturnErrNull)
{
    ecl_inputs_t in = {};
    ecl_state_t st = ECL_STATE_NORMAL;
    EXPECT_EQ(RET_ERR_NULL, ecl_sta_decide(nullptr, &st));
    EXPECT_EQ(RET_ERR_NULL, ecl_sta_decide(&in, nullptr));
}
