// 단위 테스트(SWE.4): CMP-SER(ecl_ser), CMP-CORE(ecl_core). MC/DC 100% 목표.
#include <cstring>
#include <string>

#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_ser.h"
}

namespace {
std::string serialize(const ecl_status_t &st)
{
    char buf[1024];
    size_t len = 0;
    EXPECT_EQ(RET_OK, ecl_ser_status(&st, buf, sizeof(buf), &len));
    EXPECT_EQ(std::strlen(buf), len);
    return std::string(buf);
}

ecl_status_t base_status()
{
    ecl_status_t st = {};
    st.state = ECL_STATE_NORMAL;
    return st;
}

ecl_core_t fresh_core()
{
    ecl_core_t core;
    EXPECT_EQ(RET_OK, ecl_core_init(&core));
    return core;
}

ecl_status_t status_of(const ecl_core_t &core)
{
    ecl_status_t st = {};
    EXPECT_EQ(RET_OK, ecl_core_get_status(&core, &st));
    return st;
}

size_t event_count(const ecl_core_t &core)
{
    static ecl_event_t ev[ECL_EVENT_CAPACITY];
    size_t n = 0;
    EXPECT_EQ(RET_OK, ecl_core_query_events(&core, ev, ECL_EVENT_CAPACITY, &n));
    return n;
}
}  // namespace

/**
 * @brief   [목적] 직렬화 형식: 고정 순서 `필드=값;` ASCII이며 길이가 strlen과 같다.
 * @tracing SRQ-QUA-FUN-001, SRQ-FUN-022
 * @par Input   lock_left=LOCK, state=NORMAL, reason=RC_NONE, 나머지 기본
 * @par Procedure  1. ecl_ser_status 호출
 * @par Expected   정확한 문자열(아래 EXPECT)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SerUnit, SerializesExactFormat)
{
    ecl_status_t st = base_status();
    st.lock[ECL_DOOR_LEFT] = ECL_LOCK;
    const std::string expected =
        "lock_left=LOCK;lock_right=RELEASE;state=NORMAL;reason_code=RC_NONE;override_left=0;override_right=0;"
        "command_rejected=0;validity_speed=VALID;validity_gear=VALID;validity_source_ts=VALID;validity_crash=VALID;"
        "validity_approach_left=VALID;validity_approach_right=VALID;validity_fire=VALID;validity_overtemp=VALID;"
        "validity_adult=VALID;validity_isofix_left=VALID;validity_isofix_right=VALID;validity_ignition=VALID;"
        "validity_sensor_fault=VALID;";
    EXPECT_EQ(expected, serialize(st));
}

/**
 * @brief   [목적] override/command_rejected 플래그 값(1)과 validity INVALID/STALE, 이유코드가 문자열에 반영된다.
 * @tracing SRQ-FUN-022
 * @par Input   override 양쪽 1, command_rejected=1, reason=RC_COMMAND_REJECTED, validity[0]=INVALID, [1]=STALE
 * @par Procedure  1. 직렬화
 * @par Expected   해당 부분 문자열 포함
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SerUnit, FlagsAndValidityNamesAreSerialized)
{
    ecl_status_t st = base_status();
    st.override[0] = st.override[1] = 1;
    st.command_rejected = 1;
    st.reason_code = RC_COMMAND_REJECTED;
    st.validity[0] = ECL_INVALID;
    st.validity[1] = ECL_STALE;
    st.state = ECL_STATE_FAULT;
    const std::string s = serialize(st);
    EXPECT_NE(std::string::npos, s.find("override_left=1;override_right=1;command_rejected=1;"));
    EXPECT_NE(std::string::npos, s.find("reason_code=RC_COMMAND_REJECTED;"));
    EXPECT_NE(std::string::npos, s.find("validity_speed=INVALID;validity_gear=STALE;"));
    EXPECT_NE(std::string::npos, s.find("state=FAULT;"));
}

/**
 * @brief   [목적] 범위 밖 열거값(음수, 상한 초과)은 이름 "?"로 직렬화된다.
 * @tracing SRQ-FUN-023
 * @par Input   lock=5, state=-1, reason=19, validity=3
 * @par Procedure  1. 직렬화
 * @par Expected   lock_left=?; state=?; reason_code=?; validity_speed=?
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SerUnit, OutOfRangeEnumsBecomeQuestionMark)
{
    ecl_status_t st = base_status();
    st.lock[0] = static_cast<ecl_lock_t>(5);
    st.state = static_cast<ecl_state_t>(-1);
    st.reason_code = static_cast<ecl_reason_t>(19);
    st.validity[0] = static_cast<ecl_validity_t>(3);
    const std::string s = serialize(st);
    EXPECT_NE(std::string::npos, s.find("lock_left=?;"));
    EXPECT_NE(std::string::npos, s.find("state=?;"));
    EXPECT_NE(std::string::npos, s.find("reason_code=?;"));
    EXPECT_NE(std::string::npos, s.find("validity_speed=?;"));
}

/**
 * @brief   [목적] 버퍼 경계: 필요 길이+1이면 성공, 1바이트 부족하면 RET_ERR_SERIALIZE이며 버퍼를 바꾸지 않는다.
 * @tracing SRQ-FUN-023
 * @par Input   capacity = len+1, len
 * @par Procedure  1. 정상 길이를 구한 뒤 경계 capacity로 호출
 * @par Expected   len+1: RET_OK, len: RET_ERR_SERIALIZE(버퍼 'x' 유지)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SerUnit, BufferBoundary)
{
    const ecl_status_t st = base_status();
    const size_t need = serialize(st).size();
    char buf[1024];
    size_t len = 0;
    std::memset(buf, 'x', sizeof(buf));
    EXPECT_EQ(RET_OK, ecl_ser_status(&st, buf, need + 1U, &len));
    EXPECT_EQ(need, len);
    std::memset(buf, 'x', sizeof(buf));
    EXPECT_EQ(RET_ERR_SERIALIZE, ecl_ser_status(&st, buf, need, &len));
    EXPECT_EQ('x', buf[0]);
    EXPECT_EQ('x', buf[need - 1U]);
}

/**
 * @brief   [목적] NULL 인자에 RET_ERR_NULL을 반환한다(인자별 독립).
 * @tracing SRQ-QUA-REL-001
 * @par Input   status/buf/length 각각 nullptr
 * @par Procedure  1. 호출
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SerUnit, NullArgumentsReturnErrNull)
{
    ecl_status_t st = base_status();
    char buf[16];
    size_t len = 0;
    EXPECT_EQ(RET_ERR_NULL, ecl_ser_status(nullptr, buf, sizeof(buf), &len));
    EXPECT_EQ(RET_ERR_NULL, ecl_ser_status(&st, nullptr, sizeof(buf), &len));
    EXPECT_EQ(RET_ERR_NULL, ecl_ser_status(&st, buf, sizeof(buf), nullptr));
}

/**
 * @brief   [목적] 초기 상태는 RELEASE/RELEASE/OFF/RC_NONE이고 이벤트가 없다.
 * @tracing SRQ-FUN-026, SRQ-FUN-036
 * @par Input   init 직후
 * @par Procedure  1. get_status, query_events
 * @par Expected   lock RELEASE 둘, state=OFF, reason=RC_NONE, 이벤트 0건
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, InitialState)
{
    const ecl_core_t core = fresh_core();
    const ecl_status_t st = status_of(core);
    EXPECT_EQ(ECL_RELEASE, st.lock[0]);
    EXPECT_EQ(ECL_RELEASE, st.lock[1]);
    EXPECT_EQ(ECL_STATE_OFF, st.state);
    EXPECT_EQ(RC_NONE, st.reason_code);
    EXPECT_EQ(0U, event_count(core));
}

/**
 * @brief   [목적] 이벤트 기록 조건(MC/DC): 좌 출력만, 우 출력만, state만, 이유코드만, 거절만 변해도 1건 기록하고 변화가 없으면 기록하지 않는다.
 * @tracing SRQ-FUN-027
 * @par Input   평가 시퀀스(아래 단계)
 * @par Procedure  각 단계 후 이벤트 수를 확인한다.
 * @par Expected   1)state 변화 2)변화 없음 3)LEFT만 4)RIGHT만 5)state만(fire INVALID→DEGRADED) 6)reason만(unlock 정지) 7)거절 8)거절 반복(거절만)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, EventRecordedExactlyOnChange)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));      // OFF->NORMAL
    EXPECT_EQ(1U, event_count(core));
    s = ecl_normal_snapshot(0.020);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));      // 변화 없음
    EXPECT_EQ(1U, event_count(core));
    s = ecl_normal_snapshot(0.030);
    ecl_set_command(&s, ECL_SIDE_RIGHT, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));      // RIGHT만 LOCK + reason 변화
    EXPECT_EQ(2U, event_count(core));
    s = ecl_normal_snapshot(0.040);
    ecl_set_command(&s, ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));      // LEFT만 LOCK (reason 동일)
    EXPECT_EQ(3U, event_count(core));
    s = ecl_normal_snapshot(0.050);
    ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));      // 변화 없음(이미 LOCK, reason 동일)
    EXPECT_EQ(3U, event_count(core));
}

/**
 * @brief   [목적] state만 변하는 평가(화재 입력 INVALID로 NORMAL→DEGRADED)와 reason만 변하는 평가(정지 중 unlock으로 RC_DRIVER_UNLOCK)가 각각 1건 기록된다.
 * @tracing SRQ-FUN-027
 * @par Input   정상 평가 후 fire INVALID 스냅샷, 이어서 unlock 명령 스냅샷
 * @par Procedure  이벤트 수 확인
 * @par Expected   state만 변화: +1건. reason만 변화: +1건(출력 RELEASE 유지)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, StateOnlyAndReasonOnlyChanges)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ASSERT_EQ(1U, event_count(core));
    s = ecl_normal_snapshot(0.020);
    s.fire_detected = 9;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(ECL_STATE_DEGRADED, status_of(core).state);
    EXPECT_EQ(2U, event_count(core));
    s = ecl_normal_snapshot(0.030);
    s.fire_detected = 9;
    ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(RC_DRIVER_UNLOCK, status_of(core).reason_code);
    EXPECT_EQ(3U, event_count(core));
    EXPECT_EQ(ECL_RELEASE, status_of(core).lock[0]);
}

/**
 * @brief   [목적] 거절된 명령은 이벤트를 남기며, 같은 거절이 반복되어도(출력·state·이유코드 동일) 매번 기록된다.
 * @tracing SRQ-FUN-008, SRQ-FUN-027, SRQ-FUN-038
 * @par Input   정상 평가 후 side=3 명령 2회
 * @par Procedure  이벤트 수와 status 확인
 * @par Expected   첫 거절: +1건, reason=RC_COMMAND_REJECTED, command_rejected=1. 두 번째 거절: +1건
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, RejectedCommandAlwaysRecorded)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    s = ecl_normal_snapshot(0.020);
    ecl_set_command(&s, 3, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(RC_COMMAND_REJECTED, status_of(core).reason_code);
    EXPECT_EQ(1, status_of(core).command_rejected);
    EXPECT_EQ(2U, event_count(core));
    s.timestamp_s = s.source_timestamp_s = 0.030;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(3U, event_count(core));
}

/**
 * @brief   [목적] 평가 흐름 통합: 충돌 CONFIRMED→RELEASE 유지, 접근위험 override 상태 보고, 타임아웃 이벤트 시간 기록.
 * @tracing SSR-001, SSR-002, SRQ-FUN-019, SRQ-FUN-022
 * @par Input   LOCK 후 crash CONFIRMED, NONE; 접근위험 + unlock 두 번
 * @par Procedure  각 평가 후 status 확인
 * @par Expected   CONFIRMED 동안 RELEASE/RELEASE. 접근위험 unlock 두 번째에 override=1, reason=RC_OVERRIDE_ACK, LOCK 유지
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, CrashReleaseAndOverrideReporting)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(ECL_LOCK, status_of(core).lock[0]);
    s = ecl_normal_snapshot(0.020);
    s.crash_status = ECL_CRASH_CONFIRMED;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(ECL_RELEASE, status_of(core).lock[0]);
    s = ecl_normal_snapshot(0.030);
    s.crash_status = ECL_CRASH_CONFIRMED;
    ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(ECL_RELEASE, status_of(core).lock[1]);
    s = ecl_normal_snapshot(0.040);
    s.rear_left_approach_risk = 1;
    ecl_set_command(&s, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_VOICE);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(RC_APPROACH_RISK, status_of(core).reason_code);
    s.timestamp_s = s.source_timestamp_s = 0.050;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(RC_OVERRIDE_ACK, status_of(core).reason_code);
    EXPECT_EQ(1, status_of(core).override[ECL_DOOR_LEFT]);
    EXPECT_EQ(ECL_LOCK, status_of(core).lock[ECL_DOOR_LEFT]);
}

/**
 * @brief   [목적] 틱: 마지막 스냅샷 후 0.200 s까지는 상태가 유지되고 0.210 s에서 DEGRADED로 전이한다.
 * @tracing SSR-008, SSR-009
 * @par Input   t=1.0 평가 후 tick(1.2), tick(1.21)
 * @par Procedure  status 확인
 * @par Expected   1.2: NORMAL, 1.21: DEGRADED/RC_INPUT_STALE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, TickDegradesAfterPointTwoHundred)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ASSERT_EQ(RET_OK, ecl_core_tick(&core, 1.2));
    EXPECT_EQ(ECL_STATE_NORMAL, status_of(core).state);
    ASSERT_EQ(RET_OK, ecl_core_tick(&core, 1.21));
    EXPECT_EQ(ECL_STATE_DEGRADED, status_of(core).state);
    EXPECT_EQ(RC_INPUT_STALE, status_of(core).reason_code);
}

/**
 * @brief   [목적] 코어 직렬화와 이벤트 조회가 하위 컴포넌트에 위임된다(RET_ERR_SERIALIZE 전파 포함).
 * @tracing SRQ-FUN-022, SRQ-FUN-023, SRQ-FUN-030
 * @par Input   init 직후, 작은 버퍼
 * @par Procedure  serialize_status 호출
 * @par Expected   충분한 버퍼: RET_OK와 길이>0. 4바이트 버퍼: RET_ERR_SERIALIZE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, SerializeDelegatesAndPropagatesError)
{
    const ecl_core_t core = fresh_core();
    char buf[1024];
    char tiny[4];
    size_t len = 0;
    EXPECT_EQ(RET_OK, ecl_core_serialize_status(&core, buf, sizeof(buf), &len));
    EXPECT_GT(len, 0U);
    EXPECT_EQ(RET_ERR_SERIALIZE, ecl_core_serialize_status(&core, tiny, sizeof(tiny), &len));
}

/**
 * @brief   [목적] 모든 공개 함수가 NULL 인자에 RET_ERR_NULL을 반환한다(인자별 독립).
 * @tracing SRQ-QUA-REL-001
 * @par Input   각 인자를 하나씩 nullptr
 * @par Procedure  호출
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, NullArgumentsReturnErrNull)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    ecl_status_t st = {};
    ecl_event_t ev[1] = {};
    char buf[16];
    size_t n = 0;
    EXPECT_EQ(RET_ERR_NULL, ecl_core_init(nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_evaluate(nullptr, &s));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_evaluate(&core, nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_tick(nullptr, 1.0));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_get_status(nullptr, &st));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_get_status(&core, nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_serialize_status(nullptr, buf, sizeof(buf), &n));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_serialize_status(&core, nullptr, sizeof(buf), &n));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_query_events(nullptr, ev, 1, &n));
    EXPECT_EQ(RET_ERR_NULL, ecl_core_query_events(&core, nullptr, 1, &n));
}

/**
 * @brief   [목적] 우 출력만 변하는 평가(이유코드·state 동일)도 이벤트를 1건 기록한다.
 * @tracing SRQ-FUN-027
 * @par Input   정상 평가 → LEFT lock → RIGHT lock
 * @par Procedure  이벤트 수 확인
 * @par Expected   각 단계 +1건, 마지막 단계는 우 출력만 변화(reason=RC_DRIVER_LOCK 동일)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, RightOutputOnlyChangeIsRecorded)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    s = ecl_normal_snapshot(0.020);
    ecl_set_command(&s, ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ASSERT_EQ(2U, event_count(core));
    s = ecl_normal_snapshot(0.030);
    ecl_set_command(&s, ECL_SIDE_RIGHT, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(3U, event_count(core));
    EXPECT_EQ(ECL_LOCK, status_of(core).lock[ECL_DOOR_RIGHT]);
}

/**
 * @brief   [목적] 기록된 이벤트가 정의된 필드(event_id, timestamp_s, lock, state, reason_code, command_rejected, validity)를 평가 결과대로 담는다.
 * @tracing SRQ-FUN-029, SRQ-FUN-027
 * @par Input   t=0.010 정상 평가 후 t=0.020 LEFT lock 명령(속도 입력 INVALID로 validity 확인)
 * @par Procedure  이벤트 조회 후 필드를 비교한다.
 * @par Expected   두 번째 이벤트: event_id=2, timestamp_s=0.020, lock=LOCK/RELEASE, state=DEGRADED, reason=RC_DRIVER_LOCK, validity[SPEED]=INVALID
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreUnit, EventContainsDefinedFields)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    s = ecl_normal_snapshot(0.020);
    s.vehicle_speed_kph = -1.0;
    s.command.present = 0;
    ecl_set_command(&s, ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_AVN);
    s.vehicle_speed_kph = 0.0;
    s.adult_present = 9;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ecl_event_t ev[4] = {};
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_core_query_events(&core, ev, 4, &n));
    ASSERT_EQ(2U, n);
    EXPECT_EQ(2U, ev[1].event_id);
    EXPECT_DOUBLE_EQ(0.020, ev[1].timestamp_s);
    EXPECT_EQ(ECL_LOCK, ev[1].lock[ECL_DOOR_LEFT]);
    EXPECT_EQ(ECL_RELEASE, ev[1].lock[ECL_DOOR_RIGHT]);
    EXPECT_EQ(ECL_STATE_DEGRADED, ev[1].state);
    EXPECT_EQ(RC_DRIVER_LOCK, ev[1].reason_code);
    EXPECT_EQ(0, ev[1].command_rejected);
    EXPECT_EQ(ECL_INVALID, ev[1].validity[ECL_SIG_ADULT]);
}
