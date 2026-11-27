/**
 * @file    ecl_test_util.h
 * @brief   시험 공용 헬퍼: 정상 스냅샷 생성 등 (테스트 코드 전용).
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
#ifndef ECL_TEST_UTIL_H
#define ECL_TEST_UTIL_H

extern "C" {
#include "ecl_core.h"
#include "ecl_types.h"
}

/** 정상 입력 스냅샷: 정지, 기어 P, 충돌 없음, 모든 위험 FALSE, 시동 ON, 명령 없음. */
inline ecl_snapshot_t ecl_normal_snapshot(double t)
{
    ecl_snapshot_t s = {};
    s.timestamp_s = t;
    s.source_timestamp_s = t;
    s.vehicle_speed_kph = 0.0;
    s.gear = ECL_GEAR_P;
    s.crash_status = ECL_CRASH_NONE;
    s.ignition_on = 1;
    s.command.present = 0;
    s.command.side = ECL_ABSENT;
    s.command.action = ECL_ABSENT;
    s.command.source = ECL_ABSENT;
    return s;
}

/** 명령 부착 */
inline void ecl_set_command(ecl_snapshot_t *s, int side, int action, int source)
{
    s->command.present = 1;
    s->command.side = side;
    s->command.action = action;
    s->command.source = source;
}

#endif /* ECL_TEST_UTIL_H */
