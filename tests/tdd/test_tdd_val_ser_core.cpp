// TDD 테스트: VAL, SER, CORE
#include <cmath>
#include <cstring>

#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_ser.h"
#include "ecl_val.h"
}

TEST(TddVal, NormalSnapshotIsValidAndNotStale)
{
    ecl_val_t val;
    ecl_inputs_t in = {};
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ASSERT_EQ(RET_OK, ecl_val_init(&val));
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(0, in.stale);
    EXPECT_EQ(ECL_VALID, in.validity[ECL_SIG_SPEED]);
    EXPECT_EQ(RC_NONE, in.hold_reason);
    EXPECT_EQ(0, in.speed_moving);
}

TEST(TddVal, StaleBoundaryAtPointTwoHundred)
{
    ecl_val_t val;
    ecl_inputs_t in = {};
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ASSERT_EQ(RET_OK, ecl_val_init(&val));
    s.source_timestamp_s = 1.0 - 0.200;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(0, in.stale);
    s = ecl_normal_snapshot(2.0);
    s.source_timestamp_s = 2.0 - 0.210;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(1, in.stale);
    EXPECT_EQ(RC_INPUT_STALE, in.hold_reason);
}

TEST(TddVal, InvalidSignalsAreSubstitutedTowardLock)
{
    ecl_val_t val;
    ecl_inputs_t in = {};
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ASSERT_EQ(RET_OK, ecl_val_init(&val));
    s.rear_left_approach_risk = ECL_ABSENT;
    s.isofix_right = 7;
    s.vehicle_speed_kph = 301.0;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(1, in.approach[ECL_DOOR_LEFT]);
    EXPECT_EQ(1, in.isofix[ECL_DOOR_RIGHT]);
    EXPECT_EQ(1, in.speed_moving);
    EXPECT_EQ(1, in.substituted);
    EXPECT_EQ(ECL_INVALID, in.validity[ECL_SIG_SPEED]);
}

TEST(TddVal, SensorFaultInvalidTreatedAsTrueAndNanSpeedInvalid)
{
    ecl_val_t val;
    ecl_inputs_t in = {};
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ASSERT_EQ(RET_OK, ecl_val_init(&val));
    s.sensor_fault = ECL_ABSENT;
    s.vehicle_speed_kph = NAN;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(1, in.sensor_fault);
    EXPECT_EQ(RC_SENSOR_FAULT, in.hold_reason);
    EXPECT_EQ(ECL_INVALID, in.validity[ECL_SIG_SPEED]);
}

TEST(TddVal, PendingHoldsUpToOneSecondThenTimesOutOnce)
{
    ecl_val_t val;
    ecl_inputs_t in = {};
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ASSERT_EQ(RET_OK, ecl_val_init(&val));
    s.crash_status = ECL_CRASH_PENDING;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(RC_CRASH_PENDING_HOLD, in.hold_reason);
    s = ecl_normal_snapshot(2.0);
    s.crash_status = ECL_CRASH_PENDING;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(RC_CRASH_PENDING_HOLD, in.hold_reason);
    EXPECT_EQ(0, in.pending_timeout_first);
    s = ecl_normal_snapshot(2.01);
    s.crash_status = ECL_CRASH_PENDING;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(RC_NONE, in.hold_reason);
    EXPECT_EQ(1, in.pending_timeout_first);
    s = ecl_normal_snapshot(2.02);
    s.crash_status = ECL_CRASH_PENDING;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(0, in.pending_timeout_first);
}

TEST(TddVal, TickBecomesStaleAfterPointTwoHundred)
{
    ecl_val_t val;
    ecl_inputs_t in = {};
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ASSERT_EQ(RET_OK, ecl_val_init(&val));
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&val, 1.2, &in));
    EXPECT_EQ(0, in.stale);
    ASSERT_EQ(RET_OK, ecl_val_condition_tick(&val, 1.21, &in));
    EXPECT_EQ(1, in.stale);
}

TEST(TddSer, SerializesInFixedOrderAndRejectsSmallBuffer)
{
    ecl_status_t st = {};
    char buf[1024];
    char small[8];
    size_t len = 0;
    st.lock[ECL_DOOR_LEFT] = ECL_LOCK;
    st.state = ECL_STATE_NORMAL;
    st.reason_code = RC_NONE;
    ASSERT_EQ(RET_OK, ecl_ser_status(&st, buf, sizeof(buf), &len));
    EXPECT_EQ(0, std::strncmp(buf, "lock_left=LOCK;lock_right=RELEASE;state=NORMAL;reason_code=RC_NONE;", 66));
    EXPECT_EQ(std::strlen(buf), len);
    std::memset(small, 'x', sizeof(small));
    EXPECT_EQ(RET_ERR_SERIALIZE, ecl_ser_status(&st, small, sizeof(small), &len));
    EXPECT_EQ('x', small[0]);
}

TEST(TddCore, InitialStatusAndNoEvents)
{
    ecl_core_t core;
    ecl_status_t st = {};
    ecl_event_t ev[2] = {};
    size_t n = 5;
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    ASSERT_EQ(RET_OK, ecl_core_get_status(&core, &st));
    EXPECT_EQ(ECL_RELEASE, st.lock[0]);
    EXPECT_EQ(ECL_RELEASE, st.lock[1]);
    EXPECT_EQ(ECL_STATE_OFF, st.state);
    ASSERT_EQ(RET_OK, ecl_core_query_events(&core, ev, 2, &n));
    EXPECT_EQ(0U, n);
}

TEST(TddCore, DriverLockThenEventRecordedOnlyOnChange)
{
    ecl_core_t core;
    ecl_status_t st = {};
    ecl_event_t ev[4] = {};
    size_t n = 0;
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_PHYSICAL_BUTTON);
    s.timestamp_s = s.source_timestamp_s = 0.020;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ASSERT_EQ(RET_OK, ecl_core_get_status(&core, &st));
    EXPECT_EQ(ECL_LOCK, st.lock[0]);
    EXPECT_EQ(ECL_LOCK, st.lock[1]);
    EXPECT_EQ(RC_DRIVER_LOCK, st.reason_code);
    s = ecl_normal_snapshot(0.030);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ASSERT_EQ(RET_OK, ecl_core_query_events(&core, ev, 4, &n));
    EXPECT_EQ(3U, n);  // 1) OFF→NORMAL, 2) 잠금과 이유코드 변화, 3) 이유코드가 RC_DRIVER_LOCK→RC_NONE으로 변화
}

TEST(TddCore, NullArgumentsReturnErrNull)
{
    ecl_core_t core;
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ecl_status_t st = {};
    EXPECT_EQ(RET_ERR_NULL, ecl_core_init(nullptr));
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_evaluate(nullptr, &s));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_evaluate(&core, nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_get_status(&core, nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_tick(nullptr, 1.0));
    (void)st;
}
