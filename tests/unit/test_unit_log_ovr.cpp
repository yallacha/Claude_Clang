// 단위 테스트(SWE.4): CMP-LOG(ecl_log), CMP-OVR(ecl_ovr). MC/DC 100% 목표.
#include <gtest/gtest.h>

#include "ecl_test_util.h"

extern "C" {
#include "ecl_log.h"
#include "ecl_ovr.h"
}

namespace {
ecl_cmd_result_t cmd_of(int valid, ecl_side_t side, ecl_action_t action, ecl_source_t src)
{
    ecl_cmd_result_t c = {};
    c.present = valid;
    c.valid = valid;
    c.side = side;
    c.action = action;
    c.source = src;
    return c;
}
}  // namespace

/**
 * @brief   [목적] 이벤트 ID를 1부터 1씩 증가시키고 오름차순으로 조회한다.
 * @tracing SRQ-FUN-028, SRQ-FUN-030
 * @par Input   이벤트 3건 기록
 * @par Procedure  1. 초기화 2. 3건 append 3. query(capacity 10)
 * @par Expected   ID 1,2,3 오름차순, count=3
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(LogUnit, AssignsSequentialIdsAndQueriesAscending)
{
    ecl_log_t log;
    ecl_event_t e = {};
    ecl_event_t out[10] = {};
    uint64_t id = 0;
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    for (uint64_t k = 1; k <= 3; k++) {
        ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
        EXPECT_EQ(k, id);
    }
    ASSERT_EQ(RET_OK, ecl_log_query(&log, out, 10, &n));
    ASSERT_EQ(3U, n);
    EXPECT_EQ(1U, out[0].event_id);
    EXPECT_EQ(3U, out[2].event_id);
}

/**
 * @brief   [목적] 100건 한도: 100건까지 보존, 101건째에 가장 오래된 1건을 삭제한다(경계 99/100/101).
 * @tracing SRQ-LIM-001, SRQ-LIM-003, SRQ-FUN-030
 * @par Input   99, 100, 101건 기록
 * @par Procedure  1. 각 시점에 query 2. 최신 100건 확인
 * @par Expected   99건→99, 100건→100(ID 1~100), 101건→100(ID 2~101), 오름차순
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(LogUnit, KeepsLatestHundredAtBoundary)
{
    ecl_log_t log;
    ecl_event_t e = {};
    static ecl_event_t out[ECL_EVENT_CAPACITY];
    uint64_t id = 0;
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    for (int i = 1; i <= 101; i++) {
        ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
        if ((i == 99) || (i == 100) || (i == 101)) {
            ASSERT_EQ(RET_OK, ecl_log_query(&log, out, ECL_EVENT_CAPACITY, &n));
            EXPECT_EQ((i == 101) ? 100U : static_cast<size_t>(i), n);
        }
    }
    EXPECT_EQ(2U, out[0].event_id);
    EXPECT_EQ(101U, out[99].event_id);
    for (size_t k = 1; k < n; k++) {
        EXPECT_LT(out[k - 1].event_id, out[k].event_id);
    }
}

/**
 * @brief   [목적] capacity가 보존 건수보다 작으면 최신 capacity건을 반환하고, 0이면 0건이다.
 * @tracing SRQ-FUN-030
 * @par Input   5건 기록 후 capacity=2, 0
 * @par Procedure  1. query(2) 2. query(0)
 * @par Expected   capacity 2→ID 4,5. capacity 0→count 0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(LogUnit, QueryReturnsLatestCapacityItems)
{
    ecl_log_t log;
    ecl_event_t e = {};
    ecl_event_t out[2] = {};
    uint64_t id = 0;
    size_t n = 9;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    }
    ASSERT_EQ(RET_OK, ecl_log_query(&log, out, 2, &n));
    ASSERT_EQ(2U, n);
    EXPECT_EQ(4U, out[0].event_id);
    EXPECT_EQ(5U, out[1].event_id);
    ASSERT_EQ(RET_OK, ecl_log_query(&log, out, 0, &n));
    EXPECT_EQ(0U, n);
}

/**
 * @brief   [목적] ID가 UINT64_MAX인 이벤트를 기록한 뒤에는 새 이벤트를 기록하지 않는다(포화).
 * @tracing SRQ-FUN-037, SRQ-FUN-039
 * @par Input   카운터를 UINT64_MAX-2로 설정 후 3건 append
 * @par Procedure  1. test_set_counter 2. 3건 append 3. query
 * @par Expected   ID = MAX-1, MAX, 0(미기록), 보존 2건
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(LogUnit, SaturatesAtMaxId)
{
    ecl_log_t log;
    ecl_event_t e = {};
    ecl_event_t out[4] = {};
    uint64_t id = 7;
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    ASSERT_EQ(RET_OK, ecl_log_test_set_counter(&log, UINT64_MAX - 2U));
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(UINT64_MAX - 1U, id);
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(UINT64_MAX, id);
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(0U, id);
    ASSERT_EQ(RET_OK, ecl_log_query(&log, out, 4, &n));
    EXPECT_EQ(2U, n);
}

/**
 * @brief   [목적] 시험용 카운터를 UINT64_MAX로 설정하면 즉시 포화 상태가 되고, 낮은 값으로 설정하면 해제된다.
 * @tracing SRQ-FUN-039
 * @par Input   set_counter(UINT64_MAX), set_counter(5)
 * @par Procedure  1. MAX 설정 후 append 2. 5 설정 후 append
 * @par Expected   첫 append ID=0(미기록), 두 번째 ID=6
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(LogUnit, TestSetCounterMaxAndRelease)
{
    ecl_log_t log;
    ecl_event_t e = {};
    uint64_t id = 7;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    ASSERT_EQ(RET_OK, ecl_log_test_set_counter(&log, UINT64_MAX));
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(0U, id);
    ASSERT_EQ(RET_OK, ecl_log_test_set_counter(&log, 5U));
    ASSERT_EQ(RET_OK, ecl_log_append(&log, &e, &id));
    EXPECT_EQ(6U, id);
}

/**
 * @brief   [목적] 모든 공개 함수가 NULL 인자에 RET_ERR_NULL을 반환한다(인자별 독립).
 * @tracing SRQ-QUA-REL-001
 * @par Input   각 인자를 하나씩 nullptr
 * @par Procedure  1. init/append/query/test_set_counter를 호출한다.
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(LogUnit, NullArgumentsReturnErrNull)
{
    ecl_log_t log;
    ecl_event_t e = {};
    ecl_event_t out[1] = {};
    uint64_t id = 0;
    size_t n = 0;
    ASSERT_EQ(RET_OK, ecl_log_init(&log));
    EXPECT_EQ(RET_ERR_NULL, ecl_log_init(nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_log_append(nullptr, &e, &id));
    EXPECT_EQ(RET_ERR_NULL, ecl_log_append(&log, nullptr, &id));
    EXPECT_EQ(RET_ERR_NULL, ecl_log_append(&log, &e, nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_log_query(nullptr, out, 1, &n));
    EXPECT_EQ(RET_ERR_NULL, ecl_log_query(&log, nullptr, 1, &n));
    EXPECT_EQ(RET_ERR_NULL, ecl_log_query(&log, out, 1, nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_log_test_set_counter(nullptr, 1U));
}

/**
 * @brief   [목적] 접근위험 도어에 unlock 명령이 오면 첫 억제를 기록하고(suppress=1, ack=0), 10.0 s 이내 같은 명령은 override로 판정한다.
 * @tracing SRQ-FUN-017, SRQ-FUN-019, SRQ-FUN-033, SSR-015
 * @par Input   approach[LEFT]=1, unlock(LEFT, AVN)을 t=1.0, 11.0(경계)
 * @par Procedure  1. 첫 입력 2. 10.0 s 후 같은 입력
 * @par Expected   t=1.0: suppress=1, ack=0, override=0. t=11.0: ack=1, override=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(OvrUnit, FirstSuppressionThenOverrideAtBoundary)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    const ecl_cmd_result_t cmd = cmd_of(1, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    EXPECT_EQ(1, out.suppress[ECL_DOOR_LEFT]);
    EXPECT_EQ(0, out.ack[ECL_DOOR_LEFT]);
    EXPECT_EQ(0, out.override[ECL_DOOR_LEFT]);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 11.0, &in, &cmd, &out));
    EXPECT_EQ(1, out.ack[ECL_DOOR_LEFT]);
    EXPECT_EQ(1, out.override[ECL_DOOR_LEFT]);
    EXPECT_EQ(0, out.suppress[ECL_DOOR_RIGHT]);
}

/**
 * @brief   [목적] 10.0 s 초과(10.01 s) 재입력은 override가 아니며 새 첫 억제로 기록된다(이후 같은 명령은 override).
 * @tracing SRQ-FUN-020, SRQ-FUN-035
 * @par Input   unlock을 t=1.0, 11.01, 11.02
 * @par Procedure  1~3 순차 입력
 * @par Expected   11.01: ack=0, override=0. 11.02: ack=1
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(OvrUnit, ReinputAfterWindowStartsNewSuppression)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    const ecl_cmd_result_t cmd = cmd_of(1, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_VOICE);
    in.approach[ECL_DOOR_RIGHT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 11.01, &in, &cmd, &out));
    EXPECT_EQ(0, out.ack[ECL_DOOR_RIGHT]);
    EXPECT_EQ(0, out.override[ECL_DOOR_RIGHT]);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 11.02, &in, &cmd, &out));
    EXPECT_EQ(1, out.ack[ECL_DOOR_RIGHT]);
}

/**
 * @brief   [목적] 다른 source의 재입력은 override로 판정하지 않으며 첫 억제 기록을 유지한다.
 * @tracing SRQ-FUN-017
 * @par Input   unlock(AVN) t=1.0, unlock(VOICE) t=2.0, unlock(AVN) t=3.0
 * @par Procedure  순차 입력
 * @par Expected   t=2.0: ack=0. t=3.0: ack=1(첫 억제 source와 같음)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(OvrUnit, DifferentSourceIsNotOverride)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    const ecl_cmd_result_t a = cmd_of(1, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    const ecl_cmd_result_t v = cmd_of(1, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_VOICE);
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &a, &out));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 2.0, &in, &v, &out));
    EXPECT_EQ(0, out.ack[ECL_DOOR_LEFT]);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 3.0, &in, &a, &out));
    EXPECT_EQ(1, out.ack[ECL_DOOR_LEFT]);
}

/**
 * @brief   [목적] override 상태는 접근위험 해소 또는 첫 억제 후 10.0 s 초과 시 FALSE가 된다(명령 없이도).
 * @tracing SRQ-FUN-034, SSR-035
 * @par Input   override 성립 후 (a) 접근위험 FALSE, (b) t=11.01 명령 없음
 * @par Procedure  (a), (b)를 별도 컨텍스트로 수행
 * @par Expected   두 경우 모두 override=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(OvrUnit, OverrideExpiresOnResolveOrTimeout)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    const ecl_cmd_result_t cmd = cmd_of(1, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    const ecl_cmd_result_t none = cmd_of(0, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    in.approach[ECL_DOOR_LEFT] = 1;

    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 2.0, &in, &cmd, &out));
    ASSERT_EQ(1, out.override[ECL_DOOR_LEFT]);
    in.approach[ECL_DOOR_LEFT] = 0;
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 3.0, &in, &none, &out));
    EXPECT_EQ(0, out.override[ECL_DOOR_LEFT]);

    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 2.0, &in, &cmd, &out));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 11.0, &in, &none, &out));
    EXPECT_EQ(1, out.override[ECL_DOOR_LEFT]);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 11.01, &in, &none, &out));
    EXPECT_EQ(0, out.override[ECL_DOOR_LEFT]);
}

/**
 * @brief   [목적] 유효하지 않은 명령, lock 명령, 다른 도어 명령은 해제 요청이 아니다. 접근위험 없는 도어에는 억제가 없다.
 * @tracing SRQ-FUN-017, SSR-015
 * @par Input   invalid unlock, valid lock, unlock(RIGHT) (approach LEFT만 1), approach 없음 + unlock
 * @par Procedure  각각 호출
 * @par Expected   suppress 모두 0(LEFT)
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(OvrUnit, NonRequestsDoNotSuppress)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    ecl_cmd_result_t c = cmd_of(0, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &c, &out));
    EXPECT_EQ(0, out.suppress[ECL_DOOR_LEFT]);
    c = cmd_of(1, ECL_SIDE_LEFT, ECL_ACTION_LOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &c, &out));
    EXPECT_EQ(0, out.suppress[ECL_DOOR_LEFT]);
    c = cmd_of(1, ECL_SIDE_RIGHT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &c, &out));
    EXPECT_EQ(0, out.suppress[ECL_DOOR_LEFT]);
    in.approach[ECL_DOOR_LEFT] = 0;
    c = cmd_of(1, ECL_SIDE_ALL, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &c, &out));
    EXPECT_EQ(0, out.suppress[ECL_DOOR_LEFT]);
}

/**
 * @brief   [목적] 충돌 CONFIRMED 또는 유지 조건(hold_reason)이면 명령이 적용되지 않으므로 억제·override를 기록하지 않는다.
 * @tracing SSR-016
 * @par Input   approach=1, unlock 명령, (a) crash_confirmed=1 (b) hold_reason=RC_SENSOR_FAULT
 * @par Procedure  각각 호출
 * @par Expected   suppress=0
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(OvrUnit, BlockedCommandsAreNotRecorded)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_ovr_result_t out = {};
    const ecl_cmd_result_t cmd = cmd_of(1, ECL_SIDE_LEFT, ECL_ACTION_UNLOCK, ECL_SRC_AVN);
    in.approach[ECL_DOOR_LEFT] = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    in.crash_confirmed = 1;
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    EXPECT_EQ(0, out.suppress[ECL_DOOR_LEFT]);
    in.crash_confirmed = 0;
    in.hold_reason = RC_SENSOR_FAULT;
    ASSERT_EQ(RET_OK, ecl_ovr_update(&ovr, 1.0, &in, &cmd, &out));
    EXPECT_EQ(0, out.suppress[ECL_DOOR_LEFT]);
}

/**
 * @brief   [목적] NULL 인자에 RET_ERR_NULL을 반환한다(인자별 독립).
 * @tracing SRQ-QUA-REL-001
 * @par Input   각 인자 nullptr
 * @par Procedure  init, update 호출
 * @par Expected   RET_ERR_NULL
 * @authored_by   Claude Sonnet 5.5 (claude-sonnet-5-5)
 * @authored_at   2026-11-27
 * @requested_by  Heechang Jang
 * @review_status Not Reviewed
 * @reviewed_by   -
 * @reviewed_at   -
 */
TEST(OvrUnit, NullArgumentsReturnErrNull)
{
    ecl_ovr_t ovr;
    ecl_inputs_t in = {};
    ecl_cmd_result_t cmd = {};
    ecl_ovr_result_t out = {};
    ASSERT_EQ(RET_OK, ecl_ovr_init(&ovr));
    EXPECT_EQ(RET_ERR_NULL, ecl_ovr_init(nullptr));
    EXPECT_EQ(RET_ERR_NULL, ecl_ovr_update(nullptr, 1.0, &in, &cmd, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_ovr_update(&ovr, 1.0, nullptr, &cmd, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_ovr_update(&ovr, 1.0, &in, nullptr, &out));
    EXPECT_EQ(RET_ERR_NULL, ecl_ovr_update(&ovr, 1.0, &in, &cmd, nullptr));
}
