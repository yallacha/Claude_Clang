// TDD 테스트: VAL, SER, CORE
#include <cmath>
#include <cstring>

#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_ser.h"
#include "ecl_val.h"
}

/**
 * @brief   [목적] 정상 스냅샷은 유효하며 STALE이 아니다.
 * @tracing SSR-005, SSR-007
 * @par Input   정상 스냅샷 t=1.0
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   stale=0, validity VALID, hold=RC_NONE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 지연 0.200 s는 fresh, 0.210 s는 STALE이다.
 * @tracing SSR-007
 * @par Input   age 0.200 s, 0.210 s
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   stale=0, stale=1/RC_INPUT_STALE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] INVALID 입력이 LOCK 측으로 대체된다.
 * @tracing SSR-020, SSR-021, SSR-023
 * @par Input   approach 누락, isofix=7, speed=301
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   approach=1, isofix=1, speed_moving=1, substituted=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] sensor_fault INVALID는 TRUE로, NaN 속도는 INVALID로 처리한다.
 * @tracing SSR-022, SSR-005
 * @par Input   sensor_fault=ABSENT, speed=NaN
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   sensor_fault=1, RC_SENSOR_FAULT, speed INVALID
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] PENDING은 1.000 s까지 유지하고 1.010 s에 타임아웃을 한 번만 알린다.
 * @tracing SSR-003, SSR-004, SSR-031
 * @par Input   0.010 s 격자로 1.00~2.00 s PENDING, 2.01, 2.02
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   2.00 s까지 RC_CRASH_PENDING_HOLD, 2.01 s timeout_first=1, 2.02 s 0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddVal, PendingHoldsUpToOneSecondThenTimesOutOnce)
{
    ecl_val_t val;
    ecl_inputs_t in = {};
    ASSERT_EQ(RET_OK, ecl_val_init(&val));
    for (int i = 0; i <= 100; i++) {  // t = 1.00 .. 2.00 s, 0.010 s 격자(호출 계약)
        ecl_snapshot_t s = ecl_normal_snapshot(1.0 + 0.010 * i);
        s.crash_status = ECL_CRASH_PENDING;
        ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
        EXPECT_EQ(RC_CRASH_PENDING_HOLD, in.hold_reason) << i;
        EXPECT_EQ(0, in.pending_timeout_first) << i;
    }
    ecl_snapshot_t s = ecl_normal_snapshot(2.01);
    s.crash_status = ECL_CRASH_PENDING;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(RC_NONE, in.hold_reason);
    EXPECT_EQ(1, in.pending_timeout_first);
    s = ecl_normal_snapshot(2.02);
    s.crash_status = ECL_CRASH_PENDING;
    ASSERT_EQ(RET_OK, ecl_val_condition(&val, &s, &in));
    EXPECT_EQ(0, in.pending_timeout_first);
}

/**
 * @brief   [목적] 틱이 마지막 수신 후 0.200 s까지 fresh, 0.210 s에서 STALE이다.
 * @tracing SSR-008
 * @par Input   t=1.0 수신 후 tick 1.2, 1.21
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   stale=0, stale=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 직렬화가 고정 순서이고 버퍼 부족 시 RET_ERR_SERIALIZE이다.
 * @tracing SRQ-FUN-023, SRQ-QUA-FUN-001
 * @par Input   lock_left=LOCK 상태, 8바이트 버퍼
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   문자열 접두어 일치, RET_ERR_SERIALIZE, 버퍼 불변
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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
    const char *const prefix = "lock_left=LOCK;lock_right=RELEASE;state=NORMAL;reason_code=RC_NONE;";
    EXPECT_EQ(0, std::strncmp(buf, prefix, std::strlen(prefix)));
    EXPECT_EQ(std::strlen(buf), len);
    std::memset(small, 'x', sizeof(small));
    EXPECT_EQ(RET_ERR_SERIALIZE, ecl_ser_status(&st, small, sizeof(small), &len));
    EXPECT_EQ('x', small[0]);
}

/**
 * @brief   [목적] 초기 상태는 RELEASE/RELEASE/OFF이고 이벤트가 없다.
 * @tracing SRQ-FUN-026, SRQ-FUN-036
 * @par Input   init 직후
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   RELEASE/RELEASE/OFF, 이벤트 0건
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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

/**
 * @brief   [목적] 변화 평가마다 이벤트가 기록된다.
 * @tracing SRQ-FUN-027
 * @par Input   정상 평가, lock ALL, 정상 평가
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   이벤트 3건(OFF→NORMAL, 잠금 변화, 이유코드 복귀)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(TddCore, DriverLockThenEventRecordedOnlyOnChange)
{
    ecl_core_t core;
    ecl_status_t st = {};
    ecl_event_t ev[4] = {};
    size_t n = 0;
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(RET_OK, ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_PHYSICAL_BUTTON));
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

/**
 * @brief   [목적] NULL 인자는 RET_ERR_NULL이다.
 * @tracing SRQ-QUA-REL-001
 * @par Input   core/snap/out nullptr
 * @par Procedure  TDD(RED→GREEN)로 작성한 구현 주도 테스트다. 입력을 구성해 대상 함수를 호출하고 결과를 확인한다.
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
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
