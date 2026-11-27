// 자동 생성 파일: tools/gen_system_tests.py 가 docs/SRS.md, docs/safety-requirements.md, PICT 결과로부터 생성한다. 수정하지 말 것.
// StStep 필드: tick,t,speed,gear,age,crash,rl,rr,fire,ot,adult,ifl,ifr,ign,fault,cmd_side,cmd_action,cmd_source,has_exp,L,R,state,reason,rej,ovl,ovr
static const StStep kTC_F_001[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.04, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.05, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.06, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.07, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.08, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.09, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.1, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.11, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.12, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.13, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.14, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.15, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.16, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 2, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.17, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.18, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 2, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.19, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.2, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.21, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.22, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 3, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.23, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.24, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 3, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
};
static const StStep kTC_F_002[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.04, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.05, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.06, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.07, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.08, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.09, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.1, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.11, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.12, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.13, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 2, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.14, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 2, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.15, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 2, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.16, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.17, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 2, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.18, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 2, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.19, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 3, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.2, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 3, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.21, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 3, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.22, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 3, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.23, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 3, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.24, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 3, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
};
static const StStep kTC_F_003[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 2.9, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.03, 3.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_SPEED_AUTO_LOCK, 0, -1, -1},
    {0, 0.04, 3.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_UNLOCK_DENIED_MOVING, 0, -1, -1},
    {0, 0.05, 300.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_SPEED_AUTO_LOCK, 0, -1, -1},
    {0, 0.06, 2.9, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {0, 0.07, 2.9, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
};
static const StStep kTC_F_004_01[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, ECL_ABSENT, 1, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_004_02[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -1, 1, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_004_03[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_004_04[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, ECL_ABSENT, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_004_05[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, -1, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_004_06[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 2, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_004_07[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, ECL_ABSENT, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_004_08[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, -1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_004_09[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 4, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_COMMAND_REJECTED, 1, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_005[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_FIRE, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.04, 0.0, 0, 0.0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_OVERTEMP, 0, -1, -1},
    {0, 0.05, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.06, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_ADULT_PRESENT, 0, -1, -1},
    {0, 0.07, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.08, 0.0, 0, 0.0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_FIRE, 0, -1, -1},
    {0, 0.09, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.1, 0.0, 0, 0.0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_OVERTEMP, 0, -1, -1},
};
static const StStep kTC_F_006[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_ISOFIX, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_ISOFIX, 0, -1, -1},
    {0, 0.04, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_ISOFIX, 0, -1, -1},
    {0, 0.05, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 2, 1, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_ISOFIX, 0, -1, -1},
};
static const StStep kTC_F_007[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_IGNITION_OFF, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {0, 0.04, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.05, 3.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_OFF, RC_SPEED_AUTO_LOCK, 0, -1, -1},
};
static const StStep kTC_F_008[] = {
    {0, 1.0, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 2.0, 0.0, 0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_APPROACH_RISK, 0, 0, -1},
    {0, 12.0, 0.0, 0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_OVERRIDE_ACK, 0, 1, 0},
    {0, 12.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_NONE, 0, 0, -1},
    {0, 20.0, 0.0, 0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_APPROACH_RISK, 0, 0, -1},
    {0, 30.01, 0.0, 0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_APPROACH_RISK, 0, 0, -1},
    {0, 30.02, 0.0, 0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_OVERRIDE_ACK, 0, 1, -1},
    {0, 30.03, 0.0, 0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 2, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_APPROACH_RISK, 0, 1, -1},
    {0, 45.0, 0.0, 0, 0.0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_APPROACH_RISK, 0, 0, -1},
};
static const StStep kTC_F_009[] = {
    {0, 1.0, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 1.01, 0.0, 0, 0.0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_CONFIRMED, 0, -1, -1},
    {0, 1.02, 0.0, 0, 0.0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_CONFIRMED, 0, -1, -1},
    {0, 1.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {0, 1.04, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 5.0, 0.0, 0, 0.0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_CRASH_PENDING_HOLD, 0, -1, -1},
    {0, 6.0, 0.0, 0, 0.0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_CRASH_PENDING_HOLD, 0, -1, -1},
    {0, 6.01, 0.0, 0, 0.0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_CRASH_PENDING_TIMEOUT, 0, -1, -1},
    {0, 6.02, 0.0, 0, 0.0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {0, 6.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {0, 7.0, 0.0, 0, 0.0, 2, 1, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_CONFIRMED, 0, -1, -1},
};
static const StStep kTC_F_010_01[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, ECL_ABSENT, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_DEGRADED, RC_APPROACH_RISK, 0, -1, -1},
};
static const StStep kTC_F_010_02[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 7, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_DEGRADED, RC_APPROACH_RISK, 0, -1, -1},
};
static const StStep kTC_F_010_03[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, -1, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_DEGRADED, RC_ISOFIX, 0, -1, -1},
};
static const StStep kTC_F_010_04[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, ECL_ABSENT, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_DEGRADED, RC_ISOFIX, 0, -1, -1},
};
static const StStep kTC_F_010_05[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_DEGRADED, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_010_06[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_DEGRADED, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_010_07[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, ECL_ABSENT, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_DEGRADED, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_010_08[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
};
static const StStep kTC_F_010_09[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, NAN, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_DEGRADED, RC_SPEED_AUTO_LOCK, 0, -1, -1},
};
static const StStep kTC_F_010_10[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, -0.1, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_DEGRADED, RC_SPEED_AUTO_LOCK, 0, -1, -1},
};
static const StStep kTC_F_010_11[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 300.1, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_DEGRADED, RC_SPEED_AUTO_LOCK, 0, -1, -1},
};
static const StStep kTC_F_010_12[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 300.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_SPEED_AUTO_LOCK, 0, -1, -1},
};
static const StStep kTC_F_010_13[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 9, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_010_14[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_DEGRADED, RC_INPUT_INVALID, 0, -1, -1},
};
static const StStep kTC_F_010_15[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_DEGRADED, RC_INPUT_INVALID, 0, -1, -1},
};
static const StStep kTC_F_010_16[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, -0.1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_DEGRADED, RC_INPUT_STALE, 0, -1, -1},
};
static const StStep kTC_F_010_17[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, NAN, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_DEGRADED, RC_INPUT_STALE, 0, -1, -1},
};
static const StStep kTC_F_011[] = {
    {0, 1.0, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 1.01, 0.0, 0, 0.2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {0, 1.02, 0.0, 0, 0.21, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_DEGRADED, RC_INPUT_STALE, 0, -1, -1},
    {0, 1.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {0, 2.0, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {1, 2.2, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
    {1, 2.21, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_DEGRADED, RC_INPUT_STALE, 0, -1, -1},
    {0, 2.22, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_DEGRADED, RC_INPUT_STALE, 0, -1, -1},
    {0, 2.23, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_012[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
    {0, 0.04, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_NONE, 0, -1, -1},
};
static const StStep kTC_F_019[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 300.0, 0, 0.0, 2, 1, 1, 1, 0, 0, 1, 1, 0, 1, 2, 0, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_FAULT, RC_CRASH_CONFIRMED, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.04, 300.0, 0, 0.0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 2, 1, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
    {0, 0.05, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.06, NAN, ECL_ABSENT, NAN, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, ECL_ABSENT, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_FAULT, RC_SENSOR_FAULT, 1, -1, -1},
};
static const StStep kTC_F_013_01[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 2, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
};
static const StStep kTC_F_013_02[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 3.0, 0, 0.0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 2, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_PENDING_HOLD, 0, -1, -1},
};
static const StStep kTC_F_013_03[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.04, 0.0, 0, 0.0, 2, 1, 0, 1, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_FAULT, RC_CRASH_CONFIRMED, 0, -1, -1},
};
static const StStep kTC_F_013_04[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 3.0, 0, 0.0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_CRASH_CONFIRMED, 0, -1, -1},
};
static const StStep kTC_F_013_05[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, -2, 0, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
};
static const StStep kTC_F_013_06[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 3.0, 0, 0.0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 2, 1, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_OFF, RC_APPROACH_RISK, 0, -1, -1},
};
static const StStep kTC_F_013_07[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.04, 3.0, 0, 0.0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_APPROACH_RISK, 0, -1, -1},
};
static const StStep kTC_F_013_08[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 2, 1, 1, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
};
static const StStep kTC_F_013_09[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 3.0, 0, 0.0, 2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_CRASH_CONFIRMED, 0, -1, -1},
};
static const StStep kTC_F_013_10[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.04, 3.0, 0, 0.0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 1, 2, 0, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_FAULT, RC_CRASH_CONFIRMED, 0, -1, -1},
};
static const StStep kTC_F_013_11[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 2, 1, 0, 1, 0, 0, 0, 0, 1, 0, 2, 0, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_CONFIRMED, 0, -1, -1},
};
static const StStep kTC_F_013_12[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
};
static const StStep kTC_F_013_13[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_RELEASE, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, -2, 0, 0, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_CRASH_CONFIRMED, 0, -1, -1},
};
static const StStep kTC_F_013_14[] = {
    {0, 0.01, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_UNLOCK, 0, -1, -1},
    {0, 0.02, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, ECL_LOCK, ECL_RELEASE, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.03, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_NORMAL, RC_DRIVER_LOCK, 0, -1, -1},
    {0, 0.04, 0.0, 0, 0.0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, ECL_LOCK, ECL_LOCK, ECL_STATE_FAULT, RC_SENSOR_FAULT, 0, -1, -1},
};
static const StCase kCases[] = {
    {"TC-F-001", kTC_F_001, sizeof(kTC_F_001) / sizeof(kTC_F_001[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-002", kTC_F_002, sizeof(kTC_F_002) / sizeof(kTC_F_002[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-003", kTC_F_003, sizeof(kTC_F_003) / sizeof(kTC_F_003[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-01", kTC_F_004_01, sizeof(kTC_F_004_01) / sizeof(kTC_F_004_01[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-02", kTC_F_004_02, sizeof(kTC_F_004_02) / sizeof(kTC_F_004_02[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-03", kTC_F_004_03, sizeof(kTC_F_004_03) / sizeof(kTC_F_004_03[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-04", kTC_F_004_04, sizeof(kTC_F_004_04) / sizeof(kTC_F_004_04[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-05", kTC_F_004_05, sizeof(kTC_F_004_05) / sizeof(kTC_F_004_05[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-06", kTC_F_004_06, sizeof(kTC_F_004_06) / sizeof(kTC_F_004_06[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-07", kTC_F_004_07, sizeof(kTC_F_004_07) / sizeof(kTC_F_004_07[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-08", kTC_F_004_08, sizeof(kTC_F_004_08) / sizeof(kTC_F_004_08[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-004-09", kTC_F_004_09, sizeof(kTC_F_004_09) / sizeof(kTC_F_004_09[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-005", kTC_F_005, sizeof(kTC_F_005) / sizeof(kTC_F_005[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-006", kTC_F_006, sizeof(kTC_F_006) / sizeof(kTC_F_006[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-007", kTC_F_007, sizeof(kTC_F_007) / sizeof(kTC_F_007[0]), {1, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-008", kTC_F_008, sizeof(kTC_F_008) / sizeof(kTC_F_008[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-009", kTC_F_009, sizeof(kTC_F_009) / sizeof(kTC_F_009[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-01", kTC_F_010_01, sizeof(kTC_F_010_01) / sizeof(kTC_F_010_01[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-02", kTC_F_010_02, sizeof(kTC_F_010_02) / sizeof(kTC_F_010_02[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-03", kTC_F_010_03, sizeof(kTC_F_010_03) / sizeof(kTC_F_010_03[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-04", kTC_F_010_04, sizeof(kTC_F_010_04) / sizeof(kTC_F_010_04[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-05", kTC_F_010_05, sizeof(kTC_F_010_05) / sizeof(kTC_F_010_05[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-06", kTC_F_010_06, sizeof(kTC_F_010_06) / sizeof(kTC_F_010_06[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-07", kTC_F_010_07, sizeof(kTC_F_010_07) / sizeof(kTC_F_010_07[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-08", kTC_F_010_08, sizeof(kTC_F_010_08) / sizeof(kTC_F_010_08[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-09", kTC_F_010_09, sizeof(kTC_F_010_09) / sizeof(kTC_F_010_09[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-10", kTC_F_010_10, sizeof(kTC_F_010_10) / sizeof(kTC_F_010_10[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-11", kTC_F_010_11, sizeof(kTC_F_010_11) / sizeof(kTC_F_010_11[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-12", kTC_F_010_12, sizeof(kTC_F_010_12) / sizeof(kTC_F_010_12[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-13", kTC_F_010_13, sizeof(kTC_F_010_13) / sizeof(kTC_F_010_13[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-14", kTC_F_010_14, sizeof(kTC_F_010_14) / sizeof(kTC_F_010_14[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-15", kTC_F_010_15, sizeof(kTC_F_010_15) / sizeof(kTC_F_010_15[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-16", kTC_F_010_16, sizeof(kTC_F_010_16) / sizeof(kTC_F_010_16[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-010-17", kTC_F_010_17, sizeof(kTC_F_010_17) / sizeof(kTC_F_010_17[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-011", kTC_F_011, sizeof(kTC_F_011) / sizeof(kTC_F_011[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-012", kTC_F_012, sizeof(kTC_F_012) / sizeof(kTC_F_012[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-019", kTC_F_019, sizeof(kTC_F_019) / sizeof(kTC_F_019[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-01", kTC_F_013_01, sizeof(kTC_F_013_01) / sizeof(kTC_F_013_01[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-02", kTC_F_013_02, sizeof(kTC_F_013_02) / sizeof(kTC_F_013_02[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-03", kTC_F_013_03, sizeof(kTC_F_013_03) / sizeof(kTC_F_013_03[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-04", kTC_F_013_04, sizeof(kTC_F_013_04) / sizeof(kTC_F_013_04[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-05", kTC_F_013_05, sizeof(kTC_F_013_05) / sizeof(kTC_F_013_05[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-06", kTC_F_013_06, sizeof(kTC_F_013_06) / sizeof(kTC_F_013_06[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-07", kTC_F_013_07, sizeof(kTC_F_013_07) / sizeof(kTC_F_013_07[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-08", kTC_F_013_08, sizeof(kTC_F_013_08) / sizeof(kTC_F_013_08[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-09", kTC_F_013_09, sizeof(kTC_F_013_09) / sizeof(kTC_F_013_09[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-10", kTC_F_013_10, sizeof(kTC_F_013_10) / sizeof(kTC_F_013_10[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-11", kTC_F_013_11, sizeof(kTC_F_013_11) / sizeof(kTC_F_013_11[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-12", kTC_F_013_12, sizeof(kTC_F_013_12) / sizeof(kTC_F_013_12[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-13", kTC_F_013_13, sizeof(kTC_F_013_13) / sizeof(kTC_F_013_13[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
    {"TC-F-013-14", kTC_F_013_14, sizeof(kTC_F_013_14) / sizeof(kTC_F_013_14[0]), {0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}},
};
