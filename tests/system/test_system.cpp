// 시스템 테스트(SWE.6): 요구사항 기반 시험 케이스의 자동 실행기.
// 케이스 표(system_cases.inc)는 tools/gen_system_tests.py가 요구사항과 PICT 결과로부터 생성하며,
// 같은 표가 docs/system-test/ST-ECL-001.xlsx의 테스트 케이스(TC-F-001~019, TC-F-013-xx)와 1:1이다.
// 시험 대상은 ecl_core 공개 API뿐이다(PC/SIL, 시각 주입).
#include <cmath>
#include <string>

#include <gtest/gtest.h>

#include "ecl_test_util.h"

namespace {
struct StExp {
    int has;
    ecl_lock_t l, r;
    ecl_state_t state;
    ecl_reason_t reason;
    int rej, ovl, ovr;
};

struct StStep {
    int tick;
    double t, speed;
    int gear;
    double age;
    int crash, rl, rr, fire, ot, adult, ifl, ifr, ign, fault;
    int cs, ca, cx;
    StExp exp;
};

struct StCase {
    const char *id;
    const StStep *steps;
    size_t count;
    StExp initial;
};

#include "system_cases.inc"

ecl_snapshot_t make_snapshot(const StStep &s)
{
    ecl_snapshot_t snap = ecl_normal_snapshot(s.t);
    snap.source_timestamp_s = std::isnan(s.age) ? NAN : s.t - s.age;
    snap.vehicle_speed_kph = s.speed;
    snap.gear = s.gear;
    snap.crash_status = s.crash;
    snap.rear_left_approach_risk = s.rl;
    snap.rear_right_approach_risk = s.rr;
    snap.fire_detected = s.fire;
    snap.overtemperature_detected = s.ot;
    snap.adult_present = s.adult;
    snap.isofix_left = s.ifl;
    snap.isofix_right = s.ifr;
    snap.ignition_on = s.ign;
    snap.sensor_fault = s.fault;
    if (s.cs != -2) {
        EXPECT_EQ(RET_OK, ecl_set_command(&snap, s.cs, s.ca, s.cx));
    }
    return snap;
}

void expect_status(const ecl_core_t &core, const StExp &e, const std::string &where)
{
    ecl_status_t st = {};
    ASSERT_EQ(RET_OK, ecl_core_get_status(&core, &st)) << where;
    EXPECT_EQ(e.l, st.lock[ECL_DOOR_LEFT]) << where << " lock_left";
    EXPECT_EQ(e.r, st.lock[ECL_DOOR_RIGHT]) << where << " lock_right";
    EXPECT_EQ(e.state, st.state) << where << " state";
    EXPECT_EQ(e.reason, st.reason_code) << where << " reason_code";
    EXPECT_EQ(e.rej, st.command_rejected) << where << " command_rejected";
    if (e.ovl >= 0) {
        EXPECT_EQ(e.ovl, st.override[ECL_DOOR_LEFT]) << where << " override_left";
    }
    if (e.ovr >= 0) {
        EXPECT_EQ(e.ovr, st.override[ECL_DOOR_RIGHT]) << where << " override_right";
    }
}
}  // namespace

/**
 * @brief   [목적] 생성된 시스템 테스트 케이스 전체(TC-F-001~012, 019, TC-F-013-xx)를 실행하고 단계별 기대값과 비교한다.
 * @tracing SSR-001~037, SRQ-FUN-001~038 (docs/traceability.md 시스템 테스트 열, ST-ECL-001.xlsx)
 * @par Input   system_cases.inc의 케이스별 단계 입력(시각 주입, 스냅샷/틱, 명령)
 * @par Procedure  1. 케이스마다 ecl_core_init 2. 단계마다 evaluate/tick 3. get_status와 기대값 비교
 * @par Expected   모든 단계에서 lock_left, lock_right, state, reason_code, command_rejected(, override)가 기대값과 같다
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SystemTest, AllGeneratedCasesPass)
{
    for (const StCase &c : kCases) {
        ecl_core_t core;
        ASSERT_EQ(RET_OK, ecl_core_init(&core)) << c.id;
        expect_status(core, c.initial, std::string(c.id) + " initial");
        const StStep *prev = nullptr;
        for (size_t i = 0; i < c.count; i++) {
            const StStep &s = c.steps[i];
            if ((prev != nullptr) && (prev->tick == 0) && (s.tick == 0)) {
                // 호출 계약(0.010 s 격자): 단계 사이에 시간 공백이 있으면 직전 입력(명령 제외)을 0.010 s 간격으로 반복 주입한다.
                for (int k = 1; prev->t + 0.010 * k < s.t - 0.005; k++) {
                    StStep filler = *prev;
                    filler.t = prev->t + 0.010 * k;
                    filler.cs = -2;
                    const ecl_snapshot_t fill_snap = make_snapshot(filler);
                    ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &fill_snap)) << c.id << " fill before step " << i + 1;
                }
            }
            prev = &s;
            if (s.tick != 0) {
                ASSERT_EQ(RET_OK, ecl_core_tick(&core, s.t)) << c.id << " step " << i + 1;
            } else {
                const ecl_snapshot_t snap = make_snapshot(s);
                ASSERT_EQ(RET_OK, ecl_core_evaluate(&core, &snap)) << c.id << " step " << i + 1;
            }
            if (s.exp.has != 0) {
                expect_status(core, s.exp, std::string(c.id) + " step " + std::to_string(i + 1));
            }
        }
    }
}

/**
 * @brief   [목적] 케이스 표의 개수와 ID 형식을 확인한다(생성 누락 방지: 기능 12 + WoW 1 + PICT 조합 ≥ 10).
 * @tracing SSR-027
 * @par Input   kCases
 * @par Procedure  1. 건수와 첫/마지막 ID 확인
 * @par Expected   TC-F-001이 첫 케이스, 총 케이스 ≥ 23
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(SystemTest, CaseTableIsComplete)
{
    EXPECT_STREQ("TC-F-001", kCases[0].id);
    EXPECT_GE(sizeof(kCases) / sizeof(kCases[0]), 23U);
}
