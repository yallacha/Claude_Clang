// 통합 테스트(SWE.5): 공개 API(ecl_core_*)를 통해 VAL/CMD/OVR/ARB/STA/LOG/SER 컴포넌트 연동을 검증한다.
// 통합 전략(하향식, Mock 없음): (1) 최상위 CMP-CORE 공개 인터페이스를 실제 하위 컴포넌트 전체와 연결한다.
// (2) 단계 1: 시험 벡터 V 전체 재생으로 VAL→CMD→OVR→ARB→STA→CORE 데이터 흐름과 결정론을 확인한다.
// (3) 단계 2: CORE↔LOG(이력 한도, ID 포화), CORE↔SER(오류 전파), VAL↔STA↔ARB(틱·STALE·PENDING 타임아웃) 시나리오로 인터페이스별 연동을 확인한다.
// 컴포넌트를 하나씩 추가하는 상향식 단계는 두지 않았다: 각 컴포넌트가 순수 함수 또는 단일 컨텍스트라 CORE 연결 시 오류 원인 식별이 가능하다(근거).
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_log.h"
}

namespace {
/** 시험 벡터 V(docs/test-vectors/vector-v.csv)의 한 행과 기대 결과 */
struct Row {
    double t, speed;
    int gear;
    double src;
    int crash, rl, rr, fire, ot, adult, ifl, ifr, ign, fault;
    int side, action, source;  // side == -2: 명령 없음
    ecl_lock_t exp_l, exp_r;
    ecl_state_t exp_state;
    ecl_reason_t exp_reason;
};

const int kNoCmd = -2;

const Row kVectorV[23] = {
    {0.010, 0.0, 0, 0.010, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_NONE},
    {0.020, 0.0, 0, 0.020, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_PHYSICAL_BUTTON, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK},
    {0.030, 0.0, 0, 0.030, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK},
    {0.040, 2.9, 2, 0.040, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE},
    {0.050, 3.0, 2, 0.050, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_SPEED_AUTO_LOCK},
    {0.060, 3.0, 2, 0.060, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_VOICE, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_UNLOCK_DENIED_MOVING},
    {0.070, 0.0, 0, 0.070, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_APPROACH_RISK},
    {0.080, 0.0, 0, 0.080, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_MOBILE_APP, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_APPROACH_RISK},
    {0.090, 0.0, 0, 0.090, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_MOBILE_APP, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_OVERRIDE_ACK},
    {0.100, 0.0, 0, 0.100, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE},
    {0.110, 0.0, 0, 0.110, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_FIRE},
    {0.120, 0.0, 0, 0.120, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, kNoCmd, 0, 0, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_ISOFIX},
    {0.130, 0.0, 0, 0.130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_IGNITION_OFF},
    {0.140, 0.0, 0, 0.140, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_PENDING_HOLD},
    {0.150, 0.0, 0, 0.150, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_CONFIRMED},
    {0.160, 0.0, 0, 0.160, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_PHYSICAL_BUTTON, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_CONFIRMED},
    {0.170, 0.0, 0, 0.170, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_NONE},
    {0.180, 0.0, 0, 0.180, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN, ECL_RELEASE, ECL_RELEASE, ECL_STATE_FAULT, RC_SENSOR_FAULT},
    {0.190, 0.0, 0, 0.190, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_NONE},
    {0.200, 0.0, 0, 0.000, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_NONE},
    {0.210, 0.0, 0, 0.000, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_DEGRADED, RC_INPUT_STALE},
    {0.220, 301.0, 0, 0.220, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, kNoCmd, 0, 0, ECL_LOCK, ECL_LOCK, ECL_STATE_DEGRADED, RC_SPEED_AUTO_LOCK},
    {0.230, 0.0, 0, 0.230, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 9, ECL_ACTION_LOCK, ECL_SRC_AVN, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED},
};

ecl_snapshot_t to_snapshot(const Row &r)
{
    ecl_snapshot_t s = ecl_normal_snapshot(r.t);
    s.source_timestamp_s = r.src;
    s.vehicle_speed_kph = r.speed;
    s.gear = r.gear;
    s.crash_status = r.crash;
    s.rear_left_approach_risk = r.rl;
    s.rear_right_approach_risk = r.rr;
    s.fire_detected = r.fire;
    s.overtemperature_detected = r.ot;
    s.adult_present = r.adult;
    s.isofix_left = r.ifl;
    s.isofix_right = r.ifr;
    s.ignition_on = r.ign;
    s.sensor_fault = r.fault;
    if (r.side != kNoCmd) {
        EXPECT_EQ(RET_OK, ecl_set_command(&s, r.side, r.action, r.source));
    }
    return s;
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

uint64_t fnv1a(uint64_t h, const std::string &s)
{
    for (unsigned char c : s) {
        h ^= c;
        h *= 1099511628211ULL;
    }
    return h;
}

uint64_t replay_hash()
{
    ecl_core_t core = fresh_core();
    uint64_t h = 1469598103934665603ULL;
    char buf[1024];
    size_t len = 0;
    for (const Row &r : kVectorV) {
        const ecl_snapshot_t s = to_snapshot(r);
        EXPECT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
        EXPECT_EQ(RET_OK, ecl_core_serialize_status(&core, buf, sizeof(buf), &len));
        h = fnv1a(h, std::string(buf, len));
    }
    return h;
}
}  // namespace

/**
 * @brief   [목적] 시험 벡터 V 23행을 코어에 순차 입력하면 행마다 기대 출력·state·이유코드가 나온다(VAL→CMD→OVR→ARB→STA→CORE 연동).
 * @tracing SRQ-FUN-001, SRQ-FUN-009, SRQ-FUN-017, SRQ-FUN-021, SSR-001, SSR-007, SSR-027
 * @par Input   docs/test-vectors/vector-v.csv 23행(평가 간격 0.010 s)
 * @par Procedure  1. init 2. 행마다 ecl_core_evaluate 3. get_status와 기대값 비교
 * @par Expected   각 행의 exp_l, exp_r, exp_state, exp_reason (테스트 표)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, VectorVProducesExpectedStatusPerRow)
{
    ecl_core_t core = fresh_core();
    int idx = 0;
    for (const Row &r : kVectorV) {
        const ecl_snapshot_t s = to_snapshot(r);
        ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s)) << idx;
        const ecl_status_t st = status_of(core);
        EXPECT_EQ(r.exp_l, st.lock[ECL_DOOR_LEFT]) << "row " << idx;
        EXPECT_EQ(r.exp_r, st.lock[ECL_DOOR_RIGHT]) << "row " << idx;
        EXPECT_EQ(r.exp_state, st.state) << "row " << idx;
        EXPECT_EQ(r.exp_reason, st.reason_code) << "row " << idx;
        idx++;
    }
}

/**
 * @brief   [목적] 같은 벡터를 1,000회 재생(매번 초기화)하면 공개 결과 해시가 모두 같다(결정론).
 * @tracing SRQ-QUA-FUN-001, SRQ-CON-002, SRQ-CON-005
 * @par Input   벡터 V, 고정 timestamp 주입, 직렬화 결과 FNV-1a 해시
 * @par Procedure  1. 1,000회 replay_hash 2. 해시 비교
 * @par Expected   1,000개 해시가 모두 같다
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, ReplayIsDeterministic)
{
    const uint64_t first = replay_hash();
    for (int i = 1; i < 1000; i++) {
        ASSERT_EQ(first, replay_hash()) << i;
    }
}

/**
 * @brief   [목적] 벡터 V 재생 후 이벤트 이력이 변화 평가마다 기록되며 ID가 오름차순이고 reason/lock이 상태와 일치한다.
 * @tracing SRQ-FUN-027, SRQ-FUN-028, SRQ-FUN-029, SRQ-FUN-030
 * @par Input   벡터 V 재생
 * @par Procedure  1. 재생 2. query_events(100)
 * @par Expected   건수 > 0이고 ≤ 23, ID 1..n 연속, 마지막 이벤트가 마지막 상태(RC_COMMAND_REJECTED)와 같다
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, EventHistoryFollowsEvaluations)
{
    ecl_core_t core = fresh_core();
    for (const Row &r : kVectorV) {
        const ecl_snapshot_t s = to_snapshot(r);
        ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    }
    static ecl_event_t ev[ECL_EVENT_CAPACITY];
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_core_query_events(&core, ev, ECL_EVENT_CAPACITY, &n));
    ASSERT_GT(n, 0U);
    ASSERT_LE(n, 23U);
    for (size_t i = 0; i < n; i++) {
        EXPECT_EQ(i + 1U, ev[i].event_id);
    }
    EXPECT_EQ(RC_COMMAND_REJECTED, ev[n - 1].reason_code);
    EXPECT_EQ(1, ev[n - 1].command_rejected);
    EXPECT_DOUBLE_EQ(0.230, ev[n - 1].timestamp_s);
}

/**
 * @brief   [목적] 101회 변화를 일으키면 최신 100건만 오름차순으로 조회된다(CORE→LOG 연동, 용량 경계).
 * @tracing SRQ-FUN-030, SRQ-LIM-001, SRQ-LIM-003
 * @par Input   lock/unlock 명령을 번갈아 101회(정지)
 * @par Procedure  1. 명령 입력 반복 2. query_events
 * @par Expected   100건, 첫 ID=2, 마지막 ID=101 (첫 평가 NORMAL 전이 포함 시 ID 오프셋 반영)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, EventCapacityHundredThroughCore)
{
    ecl_core_t core = fresh_core();
    double t = 0.010;
    for (int i = 0; i < 101; i++) {
        ecl_snapshot_t s = ecl_normal_snapshot(t);
        EXPECT_EQ(RET_OK, ecl_set_command(&s, ECL_SIDE_ALL, (i % 2 == 0) ? ECL_ACTION_LOCK : ECL_ACTION_UNLOCK, ECL_SRC_AVN));
        ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
        t += 0.010;
    }
    static ecl_event_t ev[ECL_EVENT_CAPACITY];
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_core_query_events(&core, ev, ECL_EVENT_CAPACITY, &n));
    EXPECT_EQ(100U, n);
    for (size_t i = 1; i < n; i++) {
        EXPECT_EQ(ev[i - 1].event_id + 1U, ev[i].event_id);
    }
    EXPECT_EQ(101U, ev[n - 1].event_id);
}

/**
 * @brief   [목적] 이벤트 ID가 최댓값에 도달하면(시험용 카운터) 이후 변화는 기록되지 않지만 제어 결과는 계속 갱신된다.
 * @tracing SRQ-FUN-037, SRQ-FUN-039
 * @par Input   core.log 카운터를 UINT64_MAX-1로 설정 후 변화 2회
 * @par Procedure  1. test_set_counter 2. 두 번 평가
 * @par Expected   이벤트 1건(ID=UINT64_MAX), 두 번째 변화는 기록되지 않으나 status는 갱신
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, EventIdSaturationDoesNotStopControl)
{
    ecl_core_t core = fresh_core();
    ecl_log_t *log = nullptr;
    ASSERT_EQ(RET_OK, ecl_core_test_get_log(&core, &log));
    ASSERT_EQ(RET_OK, ecl_log_test_set_counter(log, UINT64_MAX - 1U));
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    EXPECT_EQ(RET_OK, ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN));
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    s = ecl_normal_snapshot(0.020);
    EXPECT_EQ(RET_OK, ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_AVN));
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(ECL_RELEASE, status_of(core).lock[0]);
    ecl_event_t ev[4] = {};
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_core_query_events(&core, ev, 4, &n));
    ASSERT_EQ(1U, n);
    EXPECT_EQ(UINT64_MAX, ev[0].event_id);
}

/**
 * @brief   [목적] 마지막 스냅샷 후 틱을 0.010 s 간격으로 호출해 STALE이 된 뒤 새 스냅샷이 마지막 수신 후 0.200 s를 넘겨 도착하면 그 평가도 STALE(SSR-008)이고, 다음 격자 스냅샷에서 복구된다.
 * @tracing SSR-008, SSR-009, SSR-010, SSR-034
 * @par Input   t=1.00 s LOCK, tick 1.21 s, 스냅샷 1.22 s, 스냅샷 1.23 s
 * @par Procedure  1. LOCK 평가 2. tick 3. 스냅샷 2회
 * @par Expected   tick 1.21: DEGRADED, LOCK 유지(RC_INPUT_STALE). 1.22(마지막 수신 후 0.22 s): DEGRADED. 1.23(0.01 s): NORMAL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, TickStaleThenRecover)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    EXPECT_EQ(RET_OK, ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN));
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    for (int k = 1; k <= 21; k++) {  // 호출 계약(0.010 s 격자): 1.01~1.21 s 틱
        ASSERT_EQ(RET_OK, ecl_core_tick(&core, 1.0 + 0.010 * k));
    }
    EXPECT_EQ(ECL_STATE_DEGRADED, status_of(core).state);
    EXPECT_EQ(ECL_LOCK, status_of(core).lock[0]);
    EXPECT_EQ(RC_INPUT_STALE, status_of(core).reason_code);
    s = ecl_normal_snapshot(1.22);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(ECL_STATE_DEGRADED, status_of(core).state);
    s = ecl_normal_snapshot(1.23);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(ECL_STATE_NORMAL, status_of(core).state);
}

/**
 * @brief   [목적] 충돌 PENDING 1.0 s 유지 후 타임아웃에서 일반 중재로 복귀하며 타임아웃 코드를 한 번만 기록한다(VAL→ARB→LOG, 0.010 s 격자).
 * @tracing SSR-003, SSR-004, SSR-031, SSR-036
 * @par Input   LOCK 상태에서 crash=PENDING을 t=1.00 s부터 0.010 s 간격으로 2.01 s까지
 * @par Procedure  1. LOCK 2. PENDING 입력 반복 3. 경계 평가
 * @par Expected   2.00 s까지: LOCK 유지/RC_CRASH_PENDING_HOLD, 2.01 s: RC_CRASH_PENDING_TIMEOUT(출력 LOCK), 2.02 s: RC_NONE
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, PendingTimeoutFlow)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(0.99);
    EXPECT_EQ(RET_OK, ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN));
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    for (int i = 0; i <= 100; i++) {
        s = ecl_normal_snapshot(1.0 + 0.010 * i);
        s.crash_status = ECL_CRASH_PENDING;
        ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
        EXPECT_EQ(RC_CRASH_PENDING_HOLD, status_of(core).reason_code) << i;
        EXPECT_EQ(ECL_LOCK, status_of(core).lock[0]) << i;
    }
    s = ecl_normal_snapshot(2.01);
    s.crash_status = ECL_CRASH_PENDING;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(RC_CRASH_PENDING_TIMEOUT, status_of(core).reason_code);
    EXPECT_EQ(ECL_LOCK, status_of(core).lock[0]);
    s = ecl_normal_snapshot(2.02);
    s.crash_status = ECL_CRASH_PENDING;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    EXPECT_EQ(RC_NONE, status_of(core).reason_code);
}

/**
 * @brief   [목적] 잘못된 입력(속도 NaN, 접근위험 누락, sensor_fault 범위 밖)이 대체 값 규칙으로 안전 측(LOCK, FAULT)으로 처리되고 입력 유효성이 상태 조회/직렬화에 반영된다.
 * @tracing SSR-005, SSR-006, SSR-020, SSR-022, SSR-023, SRQ-FUN-022
 * @par Input   speed=NaN, rear_left_approach_risk=ECL_ABSENT, sensor_fault=7
 * @par Procedure  1. 평가 2. 상태 조회 3. 직렬화
 * @par Expected   state=FAULT, reason=RC_SENSOR_FAULT, validity speed/approach_left/sensor_fault=INVALID, 문자열에 validity_speed=INVALID 포함
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, InvalidInputsAreSafelySubstitutedAndReported)
{
    ecl_core_t core = fresh_core();
    ecl_snapshot_t s = ecl_normal_snapshot(1.0);
    s.vehicle_speed_kph = NAN;
    s.rear_left_approach_risk = ECL_ABSENT;
    s.sensor_fault = 7;
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    const ecl_status_t st = status_of(core);
    EXPECT_EQ(ECL_STATE_FAULT, st.state);
    EXPECT_EQ(RC_SENSOR_FAULT, st.reason_code);
    EXPECT_EQ(ECL_INVALID, st.validity[ECL_SIG_SPEED]);
    EXPECT_EQ(ECL_INVALID, st.validity[ECL_SIG_APPROACH_L]);
    EXPECT_EQ(ECL_LOCK, st.lock[ECL_DOOR_LEFT]);
    char buf[1024];
    size_t len = 0;
    ASSERT_EQ(RET_OK, ecl_core_serialize_status(&core, buf, sizeof(buf), &len));
    EXPECT_NE(std::string::npos, std::string(buf).find("validity_speed=INVALID;"));
}

/**
 * @brief   [목적] 직렬화 오류 전파: 버퍼 부족이면 CORE→SER의 RET_ERR_SERIALIZE가 호출자에게 전달된다.
 * @tracing SRQ-FUN-023
 * @par Input   16바이트 버퍼
 * @par Procedure  1. serialize_status 호출
 * @par Expected   RET_ERR_SERIALIZE, 버퍼 변경 없음
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(CoreIntegration, SerializeErrorPropagates)
{
    const ecl_core_t core = fresh_core();
    char buf[16];
    size_t len = 0;
    std::memset(buf, 'x', sizeof(buf));
    EXPECT_EQ(RET_ERR_SERIALIZE, ecl_core_serialize_status(&core, buf, sizeof(buf), &len));
    EXPECT_EQ('x', buf[0]);
}
