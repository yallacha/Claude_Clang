// 단위 테스트(SWE.4): CMP-VAL(ecl_val). 입력 검사·freshness·대체·PENDING 추적, MC/DC 100% 목표.
#include <cmath>

#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
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
 * @brief   [목적] NULL 인자에 RET_ERR_NULL을 반환한다(인자별 독립).
 * @tracing SRQ-QUA-REL-001
 * @par Input   init/condition/tick의 각 인자를 nullptr
 * @par Procedure  1. 각 호출
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, NullArgumentsReturnErrNull)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ecl_inputs_t in = {};
    EXPECT_EQ(RET_ERR_NULL, ecl_val_init(nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_val_condition(nullptr, &s, &in));
    EXPECT_EQ(RET_ERR_NULL, ecl_val_condition(&v, nullptr, &in));
    EXPECT_EQ(RET_ERR_NULL, ecl_val_condition(&v, &s, nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_val_condition_tick(nullptr, 1.0, &in));
    EXPECT_EQ(RET_ERR_NULL, ecl_val_condition_tick(&v, 1.0, nullptr));
}

/**
 * @brief   [목적] boolean 입력은 0/1이 유효하고 2, -1, 누락은 INVALID이다(경계값).
 * @tracing SSR-005, SSR-006
 * @par Input   rear_left_approach_risk = 0, 1, 2, -1, ECL_ABSENT
 * @par Procedure  1. 각 값으로 condition
 * @par Expected   0,1: VALID. 2, -1, ABSENT: INVALID
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, BooleanInputRange)
{
    const int values[5] = {0, 1, 2, -1, ECL_ABSENT};
    const ecl_validity_t expect[5] = {ECL_VALID, ECL_VALID, ECL_INVALID, ECL_INVALID, ECL_INVALID};
    for (int i = 0; i < 5; i++) {
        ecl_val_t v = fresh_val();
        ecl_snapshot_t s = ecl_normal_snapshot(1.0);
        s.rear_left_approach_risk = values[i];
        EXPECT_EQ(expect[i], condition(&v, s).validity[ECL_SIG_APPROACH_L]);
    }
}

/**
 * @brief   [목적] 모든 boolean 신호 9종의 INVALID/VALID가 각 신호에 반영된다.
 * @tracing SSR-005
 * @par Input   각 boolean 신호만 2로 설정
 * @par Procedure  1. 신호별로 condition
 * @par Expected   해당 신호만 INVALID
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, EachBooleanSignalValidatedIndependently)
{
    int ecl_snapshot_t::*const fields[9] = {
        &ecl_snapshot_t::rear_left_approach_risk, &ecl_snapshot_t::rear_right_approach_risk,
        &ecl_snapshot_t::fire_detected, &ecl_snapshot_t::overtemperature_detected,
        &ecl_snapshot_t::adult_present, &ecl_snapshot_t::isofix_left, &ecl_snapshot_t::isofix_right,
        &ecl_snapshot_t::ignition_on, &ecl_snapshot_t::sensor_fault};
    const ecl_signal_t sigs[9] = {ECL_SIG_APPROACH_L, ECL_SIG_APPROACH_R, ECL_SIG_FIRE, ECL_SIG_OVERTEMP,
                                  ECL_SIG_ADULT, ECL_SIG_ISOFIX_L, ECL_SIG_ISOFIX_R, ECL_SIG_IGNITION, ECL_SIG_FAULT};
    for (int i = 0; i < 9; i++) {
        ecl_val_t v = fresh_val();
        ecl_snapshot_t s = ecl_normal_snapshot(1.0);
        s.*fields[i] = 2;
        const ecl_inputs_t in = condition(&v, s);
        for (int k = 0; k < 9; k++) {
            EXPECT_EQ((k == i) ? ECL_INVALID : ECL_VALID, in.validity[sigs[k]]);
        }
    }
}

/**
 * @brief   [목적] 속도 경계: 0.0, 300.0은 유효, -0.1, 300.1, NaN, 무한대는 INVALID이며 3.0 이상은 이동 중이다.
 * @tracing SSR-005, SSR-023, SRQ-FUN-009
 * @par Input   speed = 0.0, 2.9, 3.0, 300.0, 300.1, -0.1, NaN, +inf
 * @par Procedure  1. 각 값으로 condition
 * @par Expected   유효: 0.0, 2.9, 3.0, 300.0 / INVALID: 나머지(이동 중으로 간주). speed_moving: 3.0, 300.0, INVALID는 1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, SpeedRangeBoundariesAndMoving)
{
    const double speeds[8] = {0.0, 2.9, 3.0, 300.0, 300.1, -0.1, NAN, INFINITY};
    const int valid[8] = {1, 1, 1, 1, 0, 0, 0, 0};
    const int moving[8] = {0, 0, 1, 1, 1, 1, 1, 1};
    for (int i = 0; i < 8; i++) {
        ecl_val_t v = fresh_val();
        ecl_snapshot_t s = ecl_normal_snapshot(1.0);
        s.vehicle_speed_kph = speeds[i];
        const ecl_inputs_t in = condition(&v, s);
        EXPECT_EQ(valid[i] != 0 ? ECL_VALID : ECL_INVALID, in.validity[ECL_SIG_SPEED]) << i;
        EXPECT_EQ(moving[i], in.speed_moving) << i;
    }
}

/**
 * @brief   [목적] 기어와 crash_status 열거 범위: 하한 미만, 범위 내, 상한 초과를 판정한다.
 * @tracing SSR-005
 * @par Input   gear = -1, 0(P), 3(R), 4 / crash_status = -1, 0, 2, 3
 * @par Procedure  1. 각 값으로 condition
 * @par Expected   -1, 4/3은 INVALID, 0..3 / 0..2는 VALID
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, GearAndCrashEnumRanges)
{
    const int gears[4] = {-1, 0, 3, 4};
    const int crashes[4] = {-1, 0, 2, 3};
    const int ok_g[4] = {0, 1, 1, 0};
    const int ok_c[4] = {0, 1, 1, 0};
    for (int i = 0; i < 4; i++) {
        ecl_val_t v = fresh_val();
        ecl_snapshot_t s = ecl_normal_snapshot(1.0);
        s.gear = gears[i];
        s.crash_status = crashes[i];
        const ecl_inputs_t in = condition(&v, s);
        EXPECT_EQ(ok_g[i] != 0 ? ECL_VALID : ECL_INVALID, in.validity[ECL_SIG_GEAR]);
        EXPECT_EQ(ok_c[i] != 0 ? ECL_VALID : ECL_INVALID, in.validity[ECL_SIG_CRASH]);
    }
}

/**
 * @brief   [목적] source_timestamp_s 유효 범위(0 ≤ src ≤ timestamp_s)와 INVALID는 STALE로 간주된다(SSR-029).
 * @tracing SSR-005, SSR-029
 * @par Input   src = NaN, -0.1, now+0.1, 0.0(now=0.0)
 * @par Procedure  1. 각 값으로 condition
 * @par Expected   NaN/-0.1/now+0.1: INVALID이고 stale=1. src=0,now=0: VALID, stale=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, SourceTimestampRangeAndInvalidMeansStale)
{
    const double srcs[4] = {NAN, -0.1, 1.1, 0.0};
    for (int i = 0; i < 4; i++) {
        ecl_val_t v = fresh_val();
        ecl_snapshot_t s = ecl_normal_snapshot((i == 3) ? 0.0 : 1.0);
        s.source_timestamp_s = srcs[i];
        const ecl_inputs_t in = condition(&v, s);
        EXPECT_EQ((i == 3) ? ECL_VALID : ECL_INVALID, in.validity[ECL_SIG_SRC_TS]) << i;
        EXPECT_EQ((i == 3) ? 0 : 1, in.stale) << i;
    }
}

/**
 * @brief   [목적] freshness 경계: timestamp_s − source_timestamp_s가 0.200이면 fresh, 0.210이면 STALE이며 STALE이면 VALID 신호가 STALE로 바뀐다.
 * @tracing SSR-007, SSR-006
 * @par Input   t=1.0, src=0.8 (0.200) / src=0.79 (0.210)
 * @par Procedure  1. condition
 * @par Expected   0.200: stale=0, validity VALID. 0.210: stale=1, validity STALE, hold_reason=RC_INPUT_STALE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, StaleBoundaryAtPointTwoHundred)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    s.source_timestamp_s = 0.8;
    ecl_inputs_t in = condition(&v, s);
    EXPECT_EQ(0, in.stale);
    EXPECT_EQ(ECL_VALID, in.validity[ECL_SIG_SPEED]);
    s.timestamp_s = 2.0;
    s.source_timestamp_s = 1.79;
    in = condition(&v, s);
    EXPECT_EQ(1, in.stale);
    EXPECT_EQ(ECL_STALE, in.validity[ECL_SIG_SPEED]);
    EXPECT_EQ(RC_INPUT_STALE, in.hold_reason);
}

/**
 * @brief   [목적] STALE이어도 이미 INVALID인 신호는 INVALID로 유지된다.
 * @tracing SSR-006, SSR-007
 * @par Input   stale 스냅샷에서 fire_detected=2
 * @par Procedure  1. condition
 * @par Expected   fire validity=INVALID, 나머지 VALID였던 것은 STALE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, InvalidStaysInvalidWhenStale)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(2.0);
    s.source_timestamp_s = 1.0;
    s.fire_detected = 2;
    const ecl_inputs_t in = condition(&v, s);
    EXPECT_EQ(ECL_INVALID, in.validity[ECL_SIG_FIRE]);
    EXPECT_EQ(ECL_STALE, in.validity[ECL_SIG_SPEED]);
}

/**
 * @brief   [목적] INVALID 대체: approach/isofix/sensor_fault는 TRUE, 화재·과온·성인은 FALSE, 속도는 이동, crash/ignition은 hold로 취급한다.
 * @tracing SSR-020, SSR-021, SSR-022, SSR-023, SSR-024, SSR-025, SSR-032
 * @par Input   각 신호를 INVALID로 하나씩 설정
 * @par Procedure  1. condition
 * @par Expected   approach/isofix=1, sensor_fault=1(hold SENSOR_FAULT), fire/overtemp/adult=0, speed_moving=1, crash/ignition INVALID→hold RC_INPUT_INVALID, substituted=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, InvalidSignalsAreSubstitutedSafely)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    s.rear_left_approach_risk = 9;
    s.rear_right_approach_risk = 9;
    s.isofix_left = 9;
    s.isofix_right = 9;
    s.fire_detected = 9;
    s.overtemperature_detected = 9;
    s.adult_present = 9;
    ecl_inputs_t in = condition(&v, s);
    EXPECT_EQ(1, in.approach[0]);
    EXPECT_EQ(1, in.approach[1]);
    EXPECT_EQ(1, in.isofix[0]);
    EXPECT_EQ(1, in.isofix[1]);
    EXPECT_EQ(0, in.fire);
    EXPECT_EQ(0, in.overtemp);
    EXPECT_EQ(0, in.adult);
    EXPECT_EQ(1, in.substituted);
    EXPECT_EQ(RC_NONE, in.hold_reason);

    v = fresh_val();
    s = ecl_normal_snapshot(1.0);
    s.sensor_fault = 9;
    in = condition(&v, s);
    EXPECT_EQ(1, in.sensor_fault);
    EXPECT_EQ(RC_SENSOR_FAULT, in.hold_reason);

    v = fresh_val();
    s = ecl_normal_snapshot(1.0);
    s.crash_status = 9;
    in = condition(&v, s);
    EXPECT_EQ(0, in.crash_confirmed);
    EXPECT_EQ(RC_INPUT_INVALID, in.hold_reason);

    v = fresh_val();
    s = ecl_normal_snapshot(1.0);
    s.ignition_on = 9;
    in = condition(&v, s);
    EXPECT_EQ(0, in.ignition_off);
    EXPECT_EQ(RC_INPUT_INVALID, in.hold_reason);
}

/**
 * @brief   [목적] gear INVALID는 대체 대상이 아니므로 substituted로 보지 않는다(결정에 쓰이지 않음).
 * @tracing SSR-026, OI-012
 * @par Input   gear = 9
 * @par Procedure  1. condition
 * @par Expected   validity[GEAR]=INVALID, substituted=0, hold_reason=RC_NONE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, GearInvalidIsNotSubstituted)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    s.gear = 9;
    const ecl_inputs_t in = condition(&v, s);
    EXPECT_EQ(ECL_INVALID, in.validity[ECL_SIG_GEAR]);
    EXPECT_EQ(0, in.substituted);
    EXPECT_EQ(RC_NONE, in.hold_reason);
}

/**
 * @brief   [목적] 유효 입력의 효과 값: ignition FALSE→ignition_off, crash CONFIRMED→crash_confirmed, TRUE 입력은 그대로 반영한다.
 * @tracing SSR-001, SRQ-FUN-005, SRQ-FUN-010, SRQ-FUN-012, SRQ-FUN-014
 * @par Input   ignition_on=0, crash=CONFIRMED, fire/overtemp/adult/approach/isofix=1
 * @par Procedure  1. condition
 * @par Expected   ignition_off=1, crash_confirmed=1, fire/overtemp/adult/approach/isofix=1, substituted=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, ValidInputsAreReflected)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    s.ignition_on = 0;
    s.crash_status = ECL_CRASH_CONFIRMED;
    s.fire_detected = s.overtemperature_detected = s.adult_present = 1;
    s.rear_left_approach_risk = s.rear_right_approach_risk = 1;
    s.isofix_left = s.isofix_right = 1;
    const ecl_inputs_t in = condition(&v, s);
    EXPECT_EQ(1, in.ignition_off);
    EXPECT_EQ(1, in.crash_confirmed);
    EXPECT_EQ(1, in.fire + in.overtemp + in.adult - 2);
    EXPECT_EQ(1, in.approach[0] & in.approach[1]);
    EXPECT_EQ(1, in.isofix[0] & in.isofix[1]);
    EXPECT_EQ(0, in.substituted);
}

/**
 * @brief   [목적] STALE 입력은 효과 값에 반영되지 않는다(0).
 * @tracing SSR-007
 * @par Input   stale 스냅샷에서 ignition_on=0, crash=CONFIRMED, speed=50
 * @par Procedure  1. condition
 * @par Expected   ignition_off=0, crash_confirmed=0, speed_moving=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, StaleInputsAreNotReflected)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(2.0);
    s.source_timestamp_s = 1.0;
    s.ignition_on = 0;
    s.crash_status = ECL_CRASH_CONFIRMED;
    s.vehicle_speed_kph = 50.0;
    const ecl_inputs_t in = condition(&v, s);
    EXPECT_EQ(0, in.ignition_off);
    EXPECT_EQ(0, in.crash_confirmed);
    EXPECT_EQ(0, in.speed_moving);
}

/**
 * @brief   [목적] PENDING 추적: 1.000 s까지 유지, 1.010 s에서 타임아웃을 한 번만 알리고, 다른 crash 값이 오면 초기화한다.
 * @tracing SSR-003, SSR-004, SSR-031
 * @par Input   t=1.0 PENDING 시작, 2.0(1.000 s), 2.01(1.010 s), 2.02, NONE, 다시 PENDING
 * @par Procedure  1. 순차 condition
 * @par Expected   2.0: hold=RC_CRASH_PENDING_HOLD. 2.01: hold=NONE, timeout_first=1. 2.02: timeout_first=0. NONE 후 PENDING은 새로 시작
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, PendingHoldTimeoutOnceAndReset)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    s.crash_status = ECL_CRASH_PENDING;
    EXPECT_EQ(RC_CRASH_PENDING_HOLD, condition(&v, s).hold_reason);
    s = ecl_normal_snapshot(2.0);
    s.crash_status = ECL_CRASH_PENDING;
    ecl_inputs_t in = condition(&v, s);
    EXPECT_EQ(RC_CRASH_PENDING_HOLD, in.hold_reason);
    EXPECT_EQ(0, in.pending_timeout_first);
    s = ecl_normal_snapshot(2.01);
    s.crash_status = ECL_CRASH_PENDING;
    in = condition(&v, s);
    EXPECT_EQ(RC_NONE, in.hold_reason);
    EXPECT_EQ(1, in.pending_timeout_first);
    s = ecl_normal_snapshot(2.02);
    s.crash_status = ECL_CRASH_PENDING;
    EXPECT_EQ(0, condition(&v, s).pending_timeout_first);
    s = ecl_normal_snapshot(2.03);
    EXPECT_EQ(RC_NONE, condition(&v, s).hold_reason);
    s = ecl_normal_snapshot(3.0);
    s.crash_status = ECL_CRASH_PENDING;
    in = condition(&v, s);
    EXPECT_EQ(RC_CRASH_PENDING_HOLD, in.hold_reason);
    EXPECT_EQ(0, in.pending_timeout_first);
}

/**
 * @brief   [목적] crash가 INVALID/STALE인 동안에는 PENDING 추적 상태를 바꾸지 않는다.
 * @tracing SSR-004
 * @par Input   PENDING 시작 후 crash=INVALID 입력, 다시 PENDING
 * @par Procedure  1. t=1.0 PENDING 2. t=1.5 crash=9 3. t=2.01 PENDING
 * @par Expected   t=2.01에서 시작 시각이 유지되어 타임아웃 판정(timeout_first=1)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, PendingStateKeptWhileCrashInvalid)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    s.crash_status = ECL_CRASH_PENDING;
    (void)condition(&v, s);
    s = ecl_normal_snapshot(1.5);
    s.crash_status = 9;
    EXPECT_EQ(RC_INPUT_INVALID, condition(&v, s).hold_reason);
    s = ecl_normal_snapshot(2.01);
    s.crash_status = ECL_CRASH_PENDING;
    EXPECT_EQ(1, condition(&v, s).pending_timeout_first);
}

/**
 * @brief   [목적] 유지 조건 이유코드 우선순위: sensor_fault > PENDING > STALE > crash/ignition INVALID.
 * @tracing SSR-027
 * @par Input   조건 조합
 * @par Procedure  1. condition
 * @par Expected   조합별 hold_reason이 우선순위대로
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, HoldReasonPriority)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    s.sensor_fault = 1;
    s.crash_status = ECL_CRASH_PENDING;
    EXPECT_EQ(RC_SENSOR_FAULT, condition(&v, s).hold_reason);
    v = fresh_val();
    s = ecl_normal_snapshot(1.0);
    s.crash_status = ECL_CRASH_PENDING;
    s.ignition_on = 9;
    EXPECT_EQ(RC_CRASH_PENDING_HOLD, condition(&v, s).hold_reason);
    v = fresh_val();
    s = ecl_normal_snapshot(2.0);
    s.source_timestamp_s = 1.0;
    s.ignition_on = 9;
    EXPECT_EQ(RC_INPUT_STALE, condition(&v, s).hold_reason);
}

/**
 * @brief   [목적] 틱 경계: 마지막 스냅샷 후 0.200 s는 fresh, 0.210 s는 STALE(DEGRADED 근거)이며 마지막 스냅샷의 입력을 재사용한다(명령 제외).
 * @tracing SSR-008
 * @par Input   t=1.0 스냅샷(ignition off, 명령 포함) 후 tick(1.2), tick(1.21)
 * @par Procedure  1. condition 2. tick
 * @par Expected   tick(1.2): stale=0, ignition_off=1, command.present=0. tick(1.21): stale=1, hold=RC_INPUT_STALE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, TickReusesLastSnapshotAndGoesStale)
{
    ecl_val_t v = fresh_val();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ecl_inputs_t in = {};
    s.ignition_on = 0;
    ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN);
    (void)condition(&v, s);
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&v, 1.2, &in));
    EXPECT_EQ(0, in.stale);
    EXPECT_EQ(1, in.ignition_off);
    EXPECT_EQ(0, in.command.present);
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&v, 1.21, &in));
    EXPECT_EQ(1, in.stale);
    EXPECT_EQ(RC_INPUT_STALE, in.hold_reason);
}

/**
 * @brief   [목적] 첫 스냅샷 전 틱: 경과 0.200 s 이하면 입력 없음(정상), 초과하면 STALE이다.
 * @tracing SSR-008
 * @par Input   init 직후 tick(0.2), tick(0.21)
 * @par Procedure  1. tick
 * @par Expected   0.2: stale=0, hold=NONE. 0.21: stale=1, hold=RC_INPUT_STALE, validity STALE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(ValUnit, TickBeforeFirstSnapshot)
{
    ecl_val_t v = fresh_val();
    ecl_inputs_t in = {};
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&v, 0.2, &in));
    EXPECT_EQ(0, in.stale);
    EXPECT_EQ(RC_NONE, in.hold_reason);
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&v, 0.21, &in));
    EXPECT_EQ(1, in.stale);
    EXPECT_EQ(RC_INPUT_STALE, in.hold_reason);
    EXPECT_EQ(ECL_STALE, in.validity[ECL_SIG_SPEED]);
}
