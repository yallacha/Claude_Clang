// 시스템 테스트(SWE.6): 특수 시험 케이스(TC-F-014~016, TC-NF-001~003). 자동 생성 표로 표현할 수 없는 관측(직렬화, 이력, 해시, 시간)을 수행한다.
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "ecl_test_util.h"
#include "sha256.h"

extern "C" {
#include "ecl_log.h"
}

namespace {
struct CsvRow {
    double t, speed;
    int gear;
    double src;
    int crash, rl, rr, fire, ot, adult, ifl, ifr, ign, fault;
    int has_cmd, side, action, source;
};

int parse_side(const std::string &s)
{
    int v = 9;  // 미등록(예: "middle")은 범위 밖 값으로 주입한다.
    if (s == "left") {
        v = ECL_SIDE_LEFT;
    } else if (s == "right") {
        v = ECL_SIDE_RIGHT;
    } else if (s == "all") {
        v = ECL_SIDE_ALL;
    }
    return v;
}

int parse_action(const std::string &s)
{
    return (s == "lock") ? static_cast<int>(ECL_ACTION_LOCK) : static_cast<int>(ECL_ACTION_UNLOCK);
}

int parse_source(const std::string &s)
{
    int v = ECL_SRC_PHYSICAL_BUTTON;
    if (s == "avn") {
        v = ECL_SRC_AVN;
    } else if (s == "voice") {
        v = ECL_SRC_VOICE;
    } else if (s == "mobile_app") {
        v = ECL_SRC_MOBILE_APP;
    }
    return v;
}

int crash_of(const std::string &s)
{
    int v = ECL_CRASH_NONE;
    if (s == "PENDING") {
        v = ECL_CRASH_PENDING;
    } else if (s == "CONFIRMED") {
        v = ECL_CRASH_CONFIRMED;
    }
    return v;
}

int gear_of(const std::string &s)
{
    int v = ECL_GEAR_P;
    if (s == "N") {
        v = ECL_GEAR_N;
    } else if (s == "D") {
        v = ECL_GEAR_D;
    } else if (s == "R") {
        v = ECL_GEAR_R;
    }
    return v;
}

std::vector<CsvRow> load_vector()
{
    std::vector<CsvRow> rows;
    std::ifstream in("docs/test-vectors/vector-v.csv");
    std::string line;
    EXPECT_TRUE(in.is_open());
    std::getline(in, line);  // 머리글
    while (std::getline(in, line)) {
        std::vector<std::string> f;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            f.push_back(cell);
        }
        while (f.size() < 17) {
            f.push_back("");
        }
        CsvRow r = {};
        r.t = std::stod(f[0]);
        r.speed = std::stod(f[1]);
        r.gear = gear_of(f[2]);
        r.src = std::stod(f[3]);
        r.crash = crash_of(f[4]);
        r.rl = std::stoi(f[5]);
        r.rr = std::stoi(f[6]);
        r.fire = std::stoi(f[7]);
        r.ot = std::stoi(f[8]);
        r.adult = std::stoi(f[9]);
        r.ifl = std::stoi(f[10]);
        r.ifr = std::stoi(f[11]);
        r.ign = std::stoi(f[12]);
        r.fault = std::stoi(f[13]);
        r.has_cmd = f[14].empty() ? 0 : 1;
        r.side = parse_side(f[14]);
        r.action = parse_action(f[15]);
        r.source = parse_source(f[16]);
        rows.push_back(r);
    }
    return rows;
}

ecl_snapshot_t to_snapshot(const CsvRow &r)
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
    if (r.has_cmd != 0) {
        ecl_set_command(&s, r.side, r.action, r.source);
    }
    return s;
}

std::string replay_sha256(const std::vector<CsvRow> &rows)
{
    ecl_core_t core;
    Sha256 sha;
    char buf[1024];
    size_t len = 0;
    EXPECT_EQ(RET_OK, ecl_core_init(&core));
    for (const CsvRow &r : rows) {
        const ecl_snapshot_t s = to_snapshot(r);
        EXPECT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
        EXPECT_EQ(RET_OK, ecl_core_serialize_status(&core, buf, sizeof(buf), &len));
        sha.update(buf, len);
    }
    return sha.hex();
}

size_t count_events(const ecl_core_t &core, ecl_event_t *buf, size_t cap)
{
    size_t n = 0;
    EXPECT_EQ(RET_OK, ecl_core_query_events(&core, buf, cap, &n));
    return n;
}
}  // namespace

/**
 * @brief   [목적] TC-F-014: 상태 직렬화에 모든 필드가 포함되고 버퍼 경계(필요 길이+1 성공, 길이 부족 RET_ERR_SERIALIZE)가 지켜지며 이벤트에 정의된 필드가 존재한다.
 * @tracing SRQ-FUN-022, SRQ-FUN-023, SRQ-FUN-029
 * @par Input   정상 평가 1회 후 capacity = 필요 길이+1, 필요 길이, 4
 * @par Procedure  1. evaluate 2. serialize 3. 필드명 확인 4. query_events
 * @par Expected   필드 전부 존재, RET_OK/RET_ERR_SERIALIZE/RET_ERR_SERIALIZE, 이벤트 필드 존재
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SystemSpecial, TcF014StatusSerializationAndFields)
{
    ecl_core_t core;
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    const ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    char buf[1024];
    size_t len = 0;
    ASSERT_EQ(RET_OK, ecl_core_serialize_status(&core, buf, sizeof(buf), &len));
    const std::string text(buf);
    const char *fields[] = {"lock_left=", "lock_right=", "state=", "reason_code=", "override_left=", "override_right=",
                            "command_rejected=", "validity_speed=", "validity_gear=", "validity_source_ts=",
                            "validity_crash=", "validity_approach_left=", "validity_approach_right=", "validity_fire=",
                            "validity_overtemp=", "validity_adult=", "validity_isofix_left=", "validity_isofix_right=",
                            "validity_ignition=", "validity_sensor_fault="};
    for (const char *f : fields) {
        EXPECT_NE(std::string::npos, text.find(f)) << f;
    }
    char exact[1024];
    std::memset(exact, 'x', sizeof(exact));
    EXPECT_EQ(RET_OK, ecl_core_serialize_status(&core, exact, len + 1U, &len));
    std::memset(exact, 'x', sizeof(exact));
    EXPECT_EQ(RET_ERR_SERIALIZE, ecl_core_serialize_status(&core, exact, len, &len));
    EXPECT_EQ('x', exact[0]);
    char tiny[4];
    EXPECT_EQ(RET_ERR_SERIALIZE, ecl_core_serialize_status(&core, tiny, sizeof(tiny), &len));
    ecl_event_t ev[4] = {};
    ASSERT_GE(count_events(core, ev, 4), 1U);
    EXPECT_EQ(1U, ev[0].event_id);
    EXPECT_DOUBLE_EQ(0.010, ev[0].timestamp_s);
    EXPECT_EQ(ECL_STATE_NORMAL, ev[0].state);
    EXPECT_EQ(ECL_VALID, ev[0].validity[ECL_SIG_SPEED]);
}

/**
 * @brief   [목적] TC-F-015: 이벤트 이력 — 변화 평가마다 1건, ID 연속 증가, 99/100/101건 경계에서 최신 100건 오름차순, 변화 없는 평가는 기록하지 않고 재초기화 후 0건이다.
 * @tracing SRQ-FUN-027, SRQ-FUN-028, SRQ-FUN-030, SRQ-LIM-001, SRQ-LIM-003
 * @par Input   lock/unlock 명령을 번갈아 입력(정지), 변화 없는 평가 3회, 재초기화
 * @par Procedure  1. 첫 평가(OFF→NORMAL, 1건) 2. 변화 평가 반복하며 98/99/100건째에 query 3. 변화 없는 평가 4. init
 * @par Expected   건수 99, 100, 100(101번째 후 첫 ID 2), ID 연속·오름차순, 변화 없는 평가 후 건수 불변, 재초기화 후 0건
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SystemSpecial, TcF015EventHistory)
{
    ecl_core_t core;
    static ecl_event_t ev[ECL_EVENT_CAPACITY];
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    double t = 0.010;
    ecl_snapshot_t s = ecl_normal_snapshot(t);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));  // 이벤트 1: OFF→NORMAL
    for (int i = 2; i <= 101; i++) {
        t += 0.010;
        s = ecl_normal_snapshot(t);
        ecl_set_command(&s, ECL_SIDE_ALL, (i % 2 == 0) ? ECL_ACTION_LOCK : ECL_ACTION_UNLOCK, ECL_SRC_AVN);
        ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
        if (i == 99) {
            EXPECT_EQ(99U, count_events(core, ev, ECL_EVENT_CAPACITY));
        }
        if (i == 100) {
            EXPECT_EQ(100U, count_events(core, ev, ECL_EVENT_CAPACITY));
            EXPECT_EQ(1U, ev[0].event_id);
        }
    }
    const size_t n = count_events(core, ev, ECL_EVENT_CAPACITY);
    ASSERT_EQ(100U, n);
    EXPECT_EQ(2U, ev[0].event_id);
    EXPECT_EQ(101U, ev[n - 1].event_id);
    for (size_t k = 1; k < n; k++) {
        EXPECT_EQ(ev[k - 1].event_id + 1U, ev[k].event_id);
    }
    for (int i = 0; i < 3; i++) {
        t += 0.010;
        s = ecl_normal_snapshot(t);
        ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_AVN);  // 이미 RELEASE+RC_DRIVER_UNLOCK: 변화 없음
        ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    }
    EXPECT_EQ(100U, count_events(core, ev, ECL_EVENT_CAPACITY));
    EXPECT_EQ(101U, ev[99].event_id);
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    EXPECT_EQ(0U, count_events(core, ev, ECL_EVENT_CAPACITY));
}

/**
 * @brief   [목적] TC-F-016: 시험용 카운터 설정으로 이벤트 ID를 2^64−2로 만든 뒤 변화 2회 — ID 2^64−1인 이벤트 1건만 기록되고 제어는 계속된다.
 * @tracing SRQ-FUN-037, SRQ-FUN-039
 * @par Input   ecl_log_test_set_counter(UINT64_MAX-1) 후 lock, unlock 명령
 * @par Procedure  1. 설정 2. 변화 평가 2회 3. query
 * @par Expected   이벤트 1건(ID=UINT64_MAX), 두 번째 평가 후 lock_left=RELEASE(출력 갱신)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SystemSpecial, TcF016EventIdSaturation)
{
    ecl_core_t core;
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    ASSERT_EQ(RET_OK, ecl_log_test_set_counter(&core.log, UINT64_MAX - 1U));
    ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    s = ecl_normal_snapshot(0.020);
    ecl_set_command(&s, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &s));
    ecl_status_t st = {};
    ASSERT_EQ(RET_OK, ecl_core_get_status(&core, &st));
    EXPECT_EQ(ECL_RELEASE, st.lock[ECL_DOOR_LEFT]);
    ecl_event_t ev[4] = {};
    ASSERT_EQ(1U, count_events(core, ev, 4));
    EXPECT_EQ(UINT64_MAX, ev[0].event_id);
}

/**
 * @brief   [목적] TC-NF-001: 시험 벡터 V를 1,000회 재생(매번 init)한 평가별 상태 직렬화 이어붙임의 SHA-256이 모두 같다(결정론).
 * @tracing SRQ-QUA-FUN-001
 * @par Input   docs/test-vectors/vector-v.csv 23행 × 1,000회
 * @par Procedure  1. CSV 로드 2. 1,000회 replay_sha256 3. 비교
 * @par Expected   1,000개 해시가 모두 같다(64자리 hex)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SystemSpecial, TcNf001DeterministicReplay)
{
    const std::vector<CsvRow> rows = load_vector();
    ASSERT_EQ(23U, rows.size());
    const std::string first = replay_sha256(rows);
    EXPECT_EQ(64U, first.size());
    for (int i = 1; i < 1000; i++) {
        ASSERT_EQ(first, replay_sha256(rows)) << i;
    }
}

/**
 * @brief   [목적] TC-NF-002: 시험 벡터의 평가 호출 간격이 0.010 s이고 평가 1회의 최악 소요 시간이 0.010 s 미만이다.
 * @tracing SRQ-LIM-002, SRQ-LIM-004
 * @par Input   벡터 V 23행 × 10,000회
 * @par Procedure  1. 행 간 시각 차 확인 2. steady_clock으로 evaluate 시간 측정, 최댓값
 * @par Expected   모든 간격 0.010 s(오차 1e-9), 최댓값 < 0.010 s
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SystemSpecial, TcNf002CallIntervalAndWorstCaseTime)
{
    const std::vector<CsvRow> rows = load_vector();
    ASSERT_EQ(23U, rows.size());
    for (size_t i = 1; i < rows.size(); i++) {
        EXPECT_NEAR(0.010, rows[i].t - rows[i - 1].t, 1e-9);
    }
    double worst = 0.0;
    ecl_core_t core;
    for (int rep = 0; rep < 10000; rep++) {
        ASSERT_EQ(RET_OK, ecl_core_init(&core));
        for (const CsvRow &r : rows) {
            const ecl_snapshot_t s = to_snapshot(r);
            const auto t0 = std::chrono::steady_clock::now();
            const ecl_ret_t ret = ecl_core_evaluate(&core, &s);
            const auto t1 = std::chrono::steady_clock::now();
            ASSERT_EQ(RET_OK, ret);
            const double dt = std::chrono::duration<double>(t1 - t0).count();
            worst = (dt > worst) ? dt : worst;
        }
    }
    EXPECT_LT(worst, 0.010);
}

/**
 * @brief   [목적] TC-NF-003: 공개 함수에 NULL 인자를 하나씩 주면 RET_ERR_NULL(-1)을 반환하고 출력 인자를 바꾸지 않는다.
 * @tracing SRQ-QUA-REL-001
 * @par Input   각 인자 nullptr
 * @par Procedure  1. 출력 버퍼를 센티널로 채움 2. 호출
 * @par Expected   모두 -1, 출력 불변
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SystemSpecial, TcNf003NullArgumentsOnPublicApi)
{
    ecl_core_t core;
    ASSERT_EQ(RET_OK, ecl_core_init(&core));
    const ecl_snapshot_t s = ecl_normal_snapshot(0.010);
    ecl_status_t st;
    std::memset(&st, 0x5A, sizeof(st));
    char buf[16];
    std::memset(buf, 'x', sizeof(buf));
    size_t n = 77;
    ecl_event_t ev[1];
    EXPECT_EQ(-1, static_cast<int>(ecl_core_init(nullptr)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_evaluate(nullptr, &s)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_evaluate(&core, nullptr)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_tick(nullptr, 0.0)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_get_status(nullptr, &st)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_get_status(&core, nullptr)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_serialize_status(nullptr, buf, sizeof(buf), &n)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_serialize_status(&core, nullptr, sizeof(buf), &n)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_serialize_status(&core, buf, sizeof(buf), nullptr)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_query_events(nullptr, ev, 1, &n)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_query_events(&core, nullptr, 1, &n)));
    EXPECT_EQ(-1, static_cast<int>(ecl_core_query_events(&core, ev, 1, nullptr)));
    EXPECT_EQ('x', buf[0]);
    EXPECT_EQ(77U, n);
    EXPECT_EQ(0x5A5A5A5A, static_cast<int>(*reinterpret_cast<const unsigned int *>(&st)));
}
