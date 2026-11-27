// 단위 테스트(SWE.4): 통합 리뷰(R1-01~03, R1-09, R1-28) 조치 시험 — STALE 판정 경로, 접근위험 LOCK 하한, 첫 억제 시각 재기록.
#include <cmath>

#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_ovr.h"
#include "ecl_val.h"
}

namespace {
ecl_inputs_t condition(ecl_val_t *val, const ecl_snapshot_t &s)
{
    ecl_inputs_t in = {};
    EXPECT_EQ(RET_OK, ecl_val_condition(val, &s, &in));
    return in;
}

ecl_val_t fresh_val()
{
    ecl_val_t v;
    EXPECT_EQ(RET_OK, ecl_val_init(&v));
    return v;
}
}  // namespace

/**
 * @brief   [목적] 마지막 스냅샷 수신 후 0.200 s까지는 fresh, 0.210 s를 넘기면 새 스냅샷도 STALE이다(SSR-008, 스냅샷 경로).
 * @tracing SSR-008, SSR-009
 * @par Input   t=1.00 s 수신 후 t=1.20 s(간격 0.200), t=1.41 s(간격 0.210) 스냅샷(source_timestamp_s = t)
 * @par Procedure  1. 순차 condition
 * @par Expected   1.20 s: stale=0. 1.41 s: stale=1, validity=STALE, hold=RC_INPUT_STALE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaleUnit, SnapshotGapAbovePointTwoHundredIsStale)
{
    ecl_val_t v = fresh_val();
    (void)condition(&v, ecl_normal_snapshot(1.0));
    EXPECT_EQ(0, condition(&v, ecl_normal_snapshot(1.2)).stale);
    const ecl_inputs_t in = condition(&v, ecl_normal_snapshot(1.41));
    EXPECT_EQ(1, in.stale);
    EXPECT_EQ(ECL_STALE, in.validity[ECL_SIG_SPEED]);
    EXPECT_EQ(RC_INPUT_STALE, in.hold_reason);
}

/**
 * @brief   [목적] STALE이어도 접근위험 TRUE는 효과 값으로 유지되어 LOCK 하한이 보장되고(SSR-011), 그 외 STALE 입력(화재 등)은 반영되지 않는다.
 * @tracing SSR-011, SSR-007
 * @par Input   source 지연 0.3 s(STALE)인 스냅샷: rear_left_approach_risk=1, rear_right_approach_risk=0, fire_detected=1, isofix_left=1
 * @par Procedure  1. condition
 * @par Expected   approach=[1,0], fire=0, isofix[LEFT]=0, stale=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaleUnit, StaleKeepsApproachRiskTrueOnly)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(2.0);
    s.source_timestamp_s = 1.7;
    s.rear_left_approach_risk = 1;
    s.fire_detected = 1;
    s.isofix_left = 1;
    const ecl_inputs_t in = condition(&v, s);
    EXPECT_EQ(1, in.stale);
    EXPECT_EQ(1, in.approach[ECL_DOOR_LEFT]);
    EXPECT_EQ(0, in.approach[ECL_DOOR_RIGHT]);
    EXPECT_EQ(0, in.fire);
    EXPECT_EQ(0, in.isofix[ECL_DOOR_LEFT]);
}

/**
 * @brief   [목적] 틱의 STALE 판정은 마지막 수신 후 경과 시간 기준이며 수신 당시의 지연(timestamp_s − source_timestamp_s)을 유지한다(SSR-008).
 * @tracing SSR-008
 * @par Input   t=1.00 s, source_timestamp_s=0.95 s(지연 0.05 s) 수신 후 tick 1.16 s(경과 0.16), tick 1.21 s(경과 0.21)
 * @par Procedure  1. condition 2. tick 2회
 * @par Expected   1.16 s: stale=0(수신 후 0.16 s). 1.21 s: stale=1(수신 후 0.21 s)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaleUnit, TickUsesElapsedSinceLastReception)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ecl_inputs_t in = {};
    s.source_timestamp_s = 0.95;
    (void)condition(&v, s);
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&v, 1.16, &in));
    EXPECT_EQ(0, in.stale);
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&v, 1.21, &in));
    EXPECT_EQ(1, in.stale);
}

/**
 * @brief   [목적] 마지막 스냅샷의 source_timestamp_s가 INVALID이면 틱에서도 INVALID(STALE로 간주)로 유지된다.
 * @tracing SSR-029, SSR-008
 * @par Input   source_timestamp_s=NaN 스냅샷 수신 후 tick 1.01 s
 * @par Procedure  1. condition 2. tick
 * @par Expected   tick에서 validity[SRC_TS]=INVALID, stale=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaleUnit, TickKeepsInvalidSourceTimestamp)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ecl_inputs_t in = {};
    s.source_timestamp_s = NAN;
    (void)condition(&v, s);
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&v, 1.01, &in));
    EXPECT_EQ(ECL_INVALID, in.validity[ECL_SIG_SRC_TS]);
    EXPECT_EQ(1, in.stale);
}

/**
 * @brief   [목적] 접근위험이 해소되면 첫 억제 시각이 삭제되어, 재진입 후의 첫 해제 요청은 override가 아니라 새 첫 억제로 기록된다(SSR-035).
 * @tracing SSR-035, SSR-015, SRQ-FUN-017
 * @par Input   approach=1 + unlock(LEFT, AVN) t=1.0 → approach=0 t=2.0 → approach=1 + 같은 unlock t=3.0, 3.5
 * @par Procedure  1. 순차 ecl_ovr_update
 * @par Expected   t=1.0: ack=0(첫 억제). t=3.0: ack=0(삭제 후 새 첫 억제, 이전 기록이 남았다면 ack=1). t=3.5: ack=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaleUnit, ApproachResolveDeletesFirstSuppressionTime)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    ecl_cmd_result_t cmd = {};
    cmd.present = 1;
    cmd.valid = 1;
    cmd.side = ECL_SIDE_LEFT;
    cmd.action = ECL_ACTION_UNLOCK;
    cmd.source = ECL_SRC_AVN;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    EXPECT_EQ(0, out.ack[ECL_DOOR_LEFT]);
    in.approach[ECL_DOOR_LEFT] = 0;
    ecl_cmd_result_t none = {};
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 2.0, &in, &none, &out));
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 3.0, &in, &cmd, &out));
    EXPECT_EQ(0, out.ack[ECL_DOOR_LEFT]);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 3.5, &in, &cmd, &out));
    EXPECT_EQ(1, out.ack[ECL_DOOR_LEFT]);
}

/**
 * @brief   [목적] 코어 수준: 첫 평가부터 STALE인 스냅샷에서도 접근위험 TRUE 도어는 LOCK이고 반대쪽은 직전 출력(RELEASE)을 유지한다(SSR-011).
 * @tracing SSR-011, SSR-010, SSR-009
 * @par Input   source 지연 0.3 s, rear_left_approach_risk=1
 * @par Procedure  1. ecl_core_evaluate 2. get_status
 * @par Expected   lock_left=LOCK, lock_right=RELEASE, state=DEGRADED, reason=RC_INPUT_STALE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaleUnit, CoreLocksApproachDoorWhileStale)
{
    ecl_core_t core;
    ecl_status_t st = {};
    ecl_snapshot_t s = ecl_normal_snapshot(2.0);
    s.source_timestamp_s = 1.7;
    s.rear_left_approach_risk = 1;
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ASSERT_EQ(RET_OK, ecl_core_get_status(&core, &st));
    EXPECT_EQ(ECL_LOCK, st.lock[ECL_DOOR_LEFT]);
    EXPECT_EQ(ECL_RELEASE, st.lock[ECL_DOOR_RIGHT]);
    EXPECT_EQ(ECL_STATE_DEGRADED, st.state);
    EXPECT_EQ(RC_INPUT_STALE, st.reason_code);
}

/**
 * @brief   [목적] 시험용 로그 접근자는 NULL 인자에 RET_ERR_NULL을 반환하고 유효 인자에는 코어의 로그 컨텍스트를 돌려준다.
 * @tracing SRQ-FUN-039, SRQ-QUA-REL-001
 * @par Input   (nullptr, &log), (&core, nullptr), (&core, &log)
 * @par Procedure  1. 각 호출 2. 반환 포인터로 ID 카운터를 설정하고 다음 이벤트 ID를 확인한다.
 * @par Expected   RET_ERR_NULL 둘, RET_OK, 설정 후 다음 이벤트 ID = 설정값+1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaleUnit, TestLogAccessorChecksArgumentsAndReturnsLog)
{
    ecl_core_t core;
    ecl_log_t *log = nullptr;
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_test_get_log(nullptr, &log));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_test_get_log(&core, nullptr));
    ASSERT_EQ(RET_OK, ecl_core_test_get_log(&core, &log));
    ASSERT_NE(nullptr, log);
    ASSERT_EQ(RET_OK, ecl_log_test_set_counter(log, 41U));
    const ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ecl_event_t ev[1] = {};
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_core_query_events(&core, ev, 1, &n));
    ASSERT_EQ(1U, n);
    EXPECT_EQ(42U, ev[0].event_id);
}

/**
 * @brief   [목적] 입력이 STALE이어도 crash_status CONFIRMED는 두 도어를 RELEASE하고 유지하며 state는 DEGRADED이다(SSR-001, 충돌 해제는 다른 신호의 오래됨에 막히지 않는다).
 * @tracing SSR-001, SSR-002, SSR-009
 * @par Input   t=1.00 s LOCK, t=2.00 s source 지연 0.3 s(STALE)인 스냅샷 crash_status=CONFIRMED, 틱 2.01 s
 * @par Procedure  1. LOCK 평가 2. STALE+CONFIRMED 스냅샷 3. 틱(직전 스냅샷 재사용)
 * @par Expected   2.00 s: RELEASE/RELEASE, RC_CRASH_CONFIRMED, DEGRADED. 틱 2.01 s: 계속 RELEASE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(StaleUnit, ConfirmedCrashReleasesEvenWhenStale)
{
    ecl_core_t core;
    ecl_status_t st = {};
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    EXPECT_EQ(RET_OK, ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN));
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    s = ecl_normal_snapshot(2.0);
    s.source_timestamp_s = 1.7;
    s.crash_status = ECL_CRASH_CONFIRMED;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ASSERT_EQ(RET_OK, ecl_core_get_status(&core, &st));
    EXPECT_EQ(ECL_RELEASE, st.lock[ECL_DOOR_LEFT]);
    EXPECT_EQ(ECL_RELEASE, st.lock[ECL_DOOR_RIGHT]);
    EXPECT_EQ(RC_CRASH_CONFIRMED, st.reason_code);
    EXPECT_EQ(ECL_STATE_DEGRADED, st.state);
    ASSERT_EQ(RET_OK, ecl_core_tick(&core, 2.01));
    ASSERT_EQ(RET_OK, ecl_core_get_status(&core, &st));
    EXPECT_EQ(ECL_RELEASE, st.lock[ECL_DOOR_LEFT]);
}
