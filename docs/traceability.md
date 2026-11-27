# 양방향 추적성 매트릭스

위치 결정: 이 문서(`docs/traceability.md`)를 추적성의 기준으로 한다. 시스템 테스트 Excel(`sw-system-test-cases`)의 추적성 시트는 이 문서의 요구사항 ID를 참조한다.
뒤 단계 열(아키텍처/상세 설계/소스/테스트)은 각 단계가 끝날 때 채우며, `-`는 해당 단계 미수행이다. ID 체계: `OEM-…`(입력), `SRQ-…`(SW 요구사항), `SSR-…`(안전 요구사항), `SWR-…`(OEM 계획 ID, 참고), `UC-…`(Use Case). v0.1→v0.2 ID 대응은 `docs/decisions.md`.

## 1. OEM 입력 → SW 요구사항 (SWE.1)

| OEM 요구 | 분류 | 파생 SW 요구사항 | OEM 계획 SWR |
|---|---|---|---|
| OEM-SR-001 | ASIL B | SRQ-LIM-002, SRQ-LIM-004, SSR-001, SSR-002, SSR-003, SSR-004, SSR-027, SSR-031, SSR-036 | SWR-007, 008 |
| OEM-SR-002 | ASIL B | SRQ-FUN-018, SSR-011, SSR-012, SSR-013, SSR-014, SSR-015, SSR-020, SSR-027, SSR-035 | SWR-005, 006, 009 |
| OEM-SR-003 | ASIL B | SRQ-LIM-002, SRQ-LIM-004, SSR-005, SSR-006, SSR-007, SSR-008, SSR-009, SSR-010, SSR-020, SSR-021, SSR-022, SSR-023, SSR-024, SSR-025, SSR-026, SSR-027, SSR-029, SSR-032, SSR-033, SSR-034, SSR-037 | SWR-013 |
| OEM-SR-004 | ASIL B | SRQ-FUN-026, SSR-016, SSR-017, SSR-018, SSR-019, SSR-022, SSR-027, SSR-028, SSR-030 | SWR-021 |
| OEM-FR-001 | QM | SRQ-FUN-001, SRQ-FUN-002, SRQ-FUN-003, SRQ-FUN-004, SRQ-FUN-005, SRQ-FUN-006, SRQ-FUN-007, SRQ-FUN-008, SRQ-FUN-021, SRQ-FUN-031, SRQ-FUN-038, SSR-027 | SWR-001, 002, 004 |
| OEM-FR-002 | QM | SRQ-FUN-004, SRQ-FUN-009, SRQ-FUN-021, SSR-027 | SWR-003 |
| OEM-FR-003 | QM | SRQ-FUN-017, SRQ-FUN-018, SRQ-FUN-019, SRQ-FUN-020, SRQ-FUN-021, SRQ-FUN-033, SRQ-FUN-034, SRQ-FUN-035, SSR-015, SSR-027, SSR-035 | SWR-006 |
| OEM-FR-004 | QM | SRQ-FUN-021, SRQ-FUN-022, SRQ-FUN-023, SRQ-FUN-024, SRQ-FUN-025, SSR-027 | SWR-014, 015 |
| OEM-FR-005 | QM | SRQ-FUN-010, SRQ-FUN-011, SRQ-FUN-021, SRQ-FUN-032, SSR-027 | SWR-017 |
| OEM-FR-006 | QM | SRQ-FUN-012, SRQ-FUN-013, SRQ-FUN-021, SSR-027 | SWR-018 |
| OEM-FR-007 | QM | SRQ-FUN-014, SRQ-FUN-015, SRQ-FUN-016, SRQ-FUN-021, SRQ-FUN-036, SSR-027, SSR-028, SSR-030 | SWR-020 |
| OEM-NFR-001 | QM | SRQ-CON-002, SRQ-CON-005, SRQ-QUA-FUN-001 | SWR-016 |
| OEM-NFR-002 | QM | SRQ-CON-003, SRQ-CON-006, SRQ-CON-007, SRQ-FUN-027, SRQ-FUN-028, SRQ-FUN-029, SRQ-FUN-030, SRQ-FUN-037, SRQ-FUN-039, SRQ-LIM-001, SRQ-LIM-003 | SWR-010, 011, 012 |
| OEM-IF-001 | 인터페이스 | SSR-005, 007, 008, 023, 029, SRQ-FUN-003, 004, 009 (SRS §3) | - |
| OEM-IF-002 | 인터페이스 | SSR-001~004, 024 | - |
| OEM-IF-003 | 인터페이스 | SSR-011~015, 020 | - |
| OEM-IF-004 | 인터페이스 | SRQ-FUN-001~008, 031, 038, SRQ-CON-002 | - |
| OEM-IF-005 | 인터페이스 | SRQ-FUN-021, SRQ-CON-004 | - |
| OEM-IF-006 | 인터페이스 | SRQ-FUN-022~025 | - |
| OEM-IF-007 | 인터페이스 | SRQ-FUN-010, 011, 032, SSR-025 | - |
| OEM-IF-008 | 인터페이스 | SRQ-FUN-012, 013, SSR-021 | - |
| OEM-IF-009 | 인터페이스 | SRQ-FUN-014~016, SSR-016~019, 022, 024 | - |
| OEM §3 실행환경 | 제약(변경) | SRQ-CON-001, 008, 009 (CR-001, DEC-007) | - |
| OEM §3 제외/제한 | 제약 | SRQ-CON-004 | - |
| (OEM 입력에 없음) 이벤트 조회 | 인터페이스 | SRQ-FUN-030 (`OI-021`) | - |
| (파생: 프로젝트 코딩 규칙) | 품질 | SRQ-QUA-REL-001 | - |

## 2. SW 요구사항 → 후속 산출물 (자동 생성: `tools/gen_traceability.py`)

컴포넌트는 `docs/architecture.md` §7, 소스 모듈은 `docs/detailed-design.md`, 단위/통합 테스트는 테스트 함수 주석의 `@tracing`(예: `ArbUnit.CrashConfirmedReleasesBothAboveAll`), 시스템 테스트는 `docs/system-test/ST-ECL-001.xlsx`의 케이스 ID이다. TDD 테스트(`tests/tdd`)는 구현 주도 테스트이며 별도 결과물이라 이 표에 포함하지 않는다.

| 요구사항 | 컴포넌트 | 소스 모듈 | 단위 테스트 | 통합 테스트 | 시스템 테스트 |
|---|---|---|---|---|---|
| SRQ-FUN-001 | CMP-CMD | ecl_cmd | CmdUnit.AllTwentyFourCombinationsAreValid, CmdUnit.ValidCommandIsAccepted | CoreIntegration.VectorVProducesExpectedStatusPerRow | TC-F-001, TC-F-002 |
| SRQ-FUN-002 | CMP-ARB | ecl_arb | ArbUnit.DriverCommandAppliesToSelectedDoorsOnly | - | TC-F-001 |
| SRQ-FUN-003 | CMP-ARB | ecl_arb | ArbUnit.DriverCommandAppliesToSelectedDoorsOnly | - | TC-F-002 |
| SRQ-FUN-004 | CMP-ARB | ecl_arb | ArbUnit.MovingLocksAndDeniesUnlock | - | TC-F-003 |
| SRQ-FUN-005 | CMP-ARB | ecl_arb | ArbUnit.MovingLocksAndDeniesUnlock, ValUnit.ValidInputsAreReflected | - | TC-F-003 |
| SRQ-FUN-006 | CMP-ARB | ecl_arb | ArbUnit.DriverCommandAppliesToSelectedDoorsOnly | - | TC-F-001, TC-F-002 |
| SRQ-FUN-007 | CMP-CMD | ecl_cmd | CmdUnit.AbsentCommandIsNeitherValidNorRejected, CmdUnit.RejectsInvalidAction, CmdUnit.RejectsInvalidSide, CmdUnit.RejectsInvalidSource | - | TC-F-004-01, TC-F-004-02, TC-F-004-03, TC-F-004-04, TC-F-004-05, TC-F-004-06, TC-F-004-07, TC-F-004-08, TC-F-004-09 |
| SRQ-FUN-031 | CMP-CMD | ecl_cmd | CmdUnit.RejectsInvalidSide | - | TC-F-004-01, TC-F-004-02, TC-F-004-03, TC-F-004-04, TC-F-004-05, TC-F-004-06, TC-F-004-07, TC-F-004-08, TC-F-004-09 |
| SRQ-FUN-008 | CMP-CMD | ecl_cmd | ArbUnit.RejectedCommandReasonOnlyWhenNoHigherRule, CoreUnit.RejectedCommandAlwaysRecorded | - | TC-F-004-01, TC-F-004-02, TC-F-004-03, TC-F-004-04, TC-F-004-05, TC-F-004-06, TC-F-004-07, TC-F-004-08, TC-F-004-09 |
| SRQ-FUN-038 | CMP-ARB | ecl_arb | ArbUnit.RejectedCommandReasonOnlyWhenNoHigherRule, CoreUnit.RejectedCommandAlwaysRecorded | - | TC-F-004-01, TC-F-004-02, TC-F-004-03, TC-F-004-04, TC-F-004-05, TC-F-004-06, TC-F-004-07, TC-F-004-08, TC-F-004-09 |
| SRQ-FUN-009 | CMP-ARB | ecl_arb | ArbUnit.MovingLocksAndDeniesUnlock, ValUnit.SpeedRangeBoundariesAndMoving | CoreIntegration.VectorVProducesExpectedStatusPerRow | TC-F-003 |
| SRQ-FUN-010 | CMP-ARB | ecl_arb | ArbUnit.ForcedReleaseReasonsAndApproachPrecedence, ValUnit.ValidInputsAreReflected | - | TC-F-005 |
| SRQ-FUN-011 | CMP-ARB | ecl_arb | ArbUnit.ForcedReleaseReasonsAndApproachPrecedence | - | TC-F-005 |
| SRQ-FUN-032 | CMP-ARB | ecl_arb | ArbUnit.ForcedReleaseReasonsAndApproachPrecedence | - | TC-F-005 |
| SRQ-FUN-012 | CMP-ARB | ecl_arb | ArbUnit.IsofixLocksOnlyThatDoor, ValUnit.ValidInputsAreReflected | - | TC-F-006 |
| SRQ-FUN-013 | CMP-ARB | ecl_arb | ArbUnit.IsofixLocksOnlyThatDoor | - | TC-F-006 |
| SRQ-FUN-014 | CMP-ARB | ecl_arb | ArbUnit.IgnitionOffReleasesUnlessMoving, ValUnit.ValidInputsAreReflected | - | TC-F-007 |
| SRQ-FUN-015 | CMP-STA | ecl_sta | StaUnit.DecidesStateByPriorityTable | - | TC-F-007 |
| SRQ-FUN-016 | CMP-ARB | ecl_arb | ArbUnit.IgnitionOffReleasesUnlessMoving | - | TC-F-007 |
| SRQ-FUN-017 | CMP-OVR | ecl_ovr | OvrUnit.DifferentSourceIsNotOverride, OvrUnit.FirstSuppressionThenOverrideAtBoundary, OvrUnit.NonRequestsDoNotSuppress, StaleUnit.ApproachResolveDeletesFirstSuppressionTime | CoreIntegration.VectorVProducesExpectedStatusPerRow | TC-F-008 |
| SRQ-FUN-018 | CMP-ARB | ecl_arb | ArbUnit.ApproachRiskLocksOnlyThatDoor | - | TC-F-008 |
| SRQ-FUN-019 | CMP-OVR | ecl_ovr | CoreUnit.CrashReleaseAndOverrideReporting, OvrUnit.FirstSuppressionThenOverrideAtBoundary | - | TC-F-008 |
| SRQ-FUN-033 | CMP-OVR | ecl_ovr | ArbUnit.ApproachRiskLocksOnlyThatDoor, OvrUnit.FirstSuppressionThenOverrideAtBoundary | - | TC-F-008 |
| SRQ-FUN-034 | CMP-OVR | ecl_ovr | OvrUnit.OverrideExpiresOnResolveOrTimeout | - | TC-F-008 |
| SRQ-FUN-020 | CMP-OVR | ecl_ovr | OvrUnit.ReinputAfterWindowStartsNewSuppression | - | TC-F-008 |
| SRQ-FUN-035 | CMP-OVR | ecl_ovr | OvrUnit.ReinputAfterWindowStartsNewSuppression | - | TC-F-008 |
| SRQ-FUN-021 | CMP-ARB, CMP-CORE | ecl_arb, ecl_core | ArbUnit.CrashConfirmedReleasesBothAboveAll | CoreIntegration.VectorVProducesExpectedStatusPerRow | TC-F-001, TC-F-013-01~14 |
| SRQ-FUN-022 | CMP-CORE, CMP-TYP | ecl_core, include/ecl_types.h | CoreUnit.CrashReleaseAndOverrideReporting, CoreUnit.SerializeDelegatesAndPropagatesError, SerUnit.FlagsAndValidityNamesAreSerialized, SerUnit.SerializesExactFormat | CoreIntegration.InvalidInputsAreSafelySubstitutedAndReported | TC-F-014 |
| SRQ-FUN-023 | CMP-SER | ecl_ser | CoreUnit.SerializeDelegatesAndPropagatesError, SerUnit.BufferBoundary, SerUnit.OutOfRangeEnumsBecomeQuestionMark | CoreIntegration.SerializeErrorPropagates | TC-F-014 |
| SRQ-FUN-024 | CMP-WEB | (이터레이션 3) | - | - | - |
| SRQ-FUN-025 | CMP-WEB | (이터레이션 3) | - | - | - |
| SRQ-FUN-026 | CMP-CORE | ecl_core | CoreUnit.InitialState | - | TC-F-007 |
| SRQ-FUN-036 | CMP-CORE, CMP-STA | ecl_core, ecl_sta | CoreUnit.InitialState | - | TC-F-007 |
| SRQ-FUN-027 | CMP-LOG | ecl_log | CoreUnit.EventContainsDefinedFields, CoreUnit.EventRecordedExactlyOnChange, CoreUnit.RejectedCommandAlwaysRecorded, CoreUnit.RightOutputOnlyChangeIsRecorded, CoreUnit.StateOnlyAndReasonOnlyChanges | CoreIntegration.EventHistoryFollowsEvaluations | TC-F-015 |
| SRQ-FUN-028 | CMP-LOG | ecl_log | LogUnit.AssignsSequentialIdsAndQueriesAscending | CoreIntegration.EventHistoryFollowsEvaluations | TC-F-015 |
| SRQ-FUN-037 | CMP-LOG | ecl_log | LogUnit.SaturatesAtMaxId | CoreIntegration.EventIdSaturationDoesNotStopControl | TC-F-016 |
| SRQ-FUN-039 | CMP-LOG | ecl_log | LogUnit.SaturatesAtMaxId, LogUnit.TestSetCounterMaxAndRelease, StaleUnit.TestLogAccessorChecksArgumentsAndReturnsLog | CoreIntegration.EventIdSaturationDoesNotStopControl | TC-F-016, TC-I-007 |
| SRQ-FUN-029 | CMP-TYP | include/ecl_types.h | CoreUnit.EventContainsDefinedFields | CoreIntegration.EventHistoryFollowsEvaluations | TC-F-014 |
| SRQ-FUN-030 | CMP-LOG | ecl_log | CoreUnit.SerializeDelegatesAndPropagatesError, LogUnit.AssignsSequentialIdsAndQueriesAscending, LogUnit.KeepsLatestHundredAtBoundary, LogUnit.QueryReturnsLatestCapacityItems | CoreIntegration.EventCapacityHundredThroughCore, CoreIntegration.EventHistoryFollowsEvaluations | TC-F-015 |
| SRQ-QUA-FUN-001 | CMP-CORE, CMP-SER | ecl_core, ecl_ser | SerUnit.SerializesExactFormat | CoreIntegration.ReplayIsDeterministic | TC-NF-001 |
| SRQ-QUA-REL-001 | CMP-CORE | ecl_core | ArbUnit.NullArgumentsReturnErrNull, CmdUnit.NullArgumentsReturnErrNull, CoreUnit.NullArgumentsReturnErrNull, LogUnit.NullArgumentsReturnErrNull, OvrUnit.NullArgumentsReturnErrNull, SerUnit.NullArgumentsReturnErrNull, StaUnit.NullArgumentsReturnErrNull, StaleUnit.TestLogAccessorChecksArgumentsAndReturnsLog, ValUnit.NullArgumentsReturnErrNull | - | TC-NF-003 |
| SRQ-LIM-001 | CMP-LOG | ecl_log | LogUnit.KeepsLatestHundredAtBoundary | CoreIntegration.EventCapacityHundredThroughCore | TC-F-015 |
| SRQ-LIM-003 | CMP-LOG | ecl_log | LogUnit.KeepsLatestHundredAtBoundary | CoreIntegration.EventCapacityHundredThroughCore | TC-F-015 |
| SRQ-LIM-002 | CMP-CORE | ecl_core | - | - | TC-NF-002 |
| SRQ-LIM-004 | CMP-CORE | ecl_core | - | - | TC-NF-002 |
| SRQ-CON-001 | 전체(횡단) | 전체 | - | - | TC-I-001 |
| SRQ-CON-008 | CMP-WEB | (이터레이션 3) | - | - | - |
| SRQ-CON-009 | CMP-WEB | (이터레이션 3) | - | - | - |
| SRQ-CON-002 | CMP-CORE | ecl_core | - | CoreIntegration.ReplayIsDeterministic | TC-I-002 |
| SRQ-CON-005 | CMP-CORE | ecl_core | - | CoreIntegration.ReplayIsDeterministic | TC-I-002 |
| SRQ-CON-003 | CMP-TYP | include/ecl_types.h | - | - | TC-I-003 |
| SRQ-CON-006 | CMP-TYP | include/ecl_types.h | - | - | TC-I-004 |
| SRQ-CON-007 | CMP-TYP | include/ecl_types.h | - | - | TC-I-005 |
| SRQ-CON-004 | CMP-CORE | ecl_core | - | - | TC-I-006 |
| SSR-001 | CMP-ARB | ecl_arb | ArbUnit.CrashConfirmedReleasesBothAboveAll, CoreUnit.CrashReleaseAndOverrideReporting, ValUnit.ValidInputsAreReflected | CoreIntegration.VectorVProducesExpectedStatusPerRow | TC-F-009, TC-F-019 |
| SSR-002 | CMP-ARB | ecl_arb | ArbUnit.CrashConfirmedReleasesBothAboveAll, CoreUnit.CrashReleaseAndOverrideReporting | - | TC-F-009 |
| SSR-003 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, ValUnit.PendingHoldTimeoutOnceAndReset | CoreIntegration.PendingTimeoutFlow | TC-F-009 |
| SSR-036 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, ValUnit.HoldReasonPriority | CoreIntegration.PendingTimeoutFlow | TC-F-009 |
| SSR-004 | CMP-VAL | ecl_val | ArbUnit.PendingTimeoutReasonBeatsLowerRules, ValUnit.PendingHoldTimeoutOnceAndReset, ValUnit.PendingStateKeptWhileCrashInvalid | CoreIntegration.PendingTimeoutFlow | TC-F-009 |
| SSR-031 | CMP-VAL | ecl_val | ArbUnit.PendingTimeoutReasonBeatsLowerRules, ValUnit.PendingHoldTimeoutOnceAndReset | CoreIntegration.PendingTimeoutFlow | TC-F-009 |
| SSR-005 | CMP-VAL | ecl_val | ValUnit.BooleanInputRange, ValUnit.EachBooleanSignalValidatedIndependently, ValUnit.GearAndCrashEnumRanges, ValUnit.SourceTimestampRangeAndInvalidMeansStale, ValUnit.SpeedRangeBoundariesAndMoving | CoreIntegration.InvalidInputsAreSafelySubstitutedAndReported | TC-F-010-01, TC-F-010-02, TC-F-010-03, TC-F-010-04, TC-F-010-05, TC-F-010-06, TC-F-010-07, TC-F-010-08, TC-F-010-09, TC-F-010-10, TC-F-010-11, TC-F-010-12, TC-F-010-13, TC-F-010-14, TC-F-010-15, TC-F-010-16, TC-F-010-17, TC-F-019 |
| SSR-006 | CMP-VAL | ecl_val | ValUnit.BooleanInputRange, ValUnit.InvalidStaysInvalidWhenStale, ValUnit.StaleBoundaryAtPointTwoHundred | CoreIntegration.InvalidInputsAreSafelySubstitutedAndReported | TC-F-010-01, TC-F-010-02, TC-F-010-03, TC-F-010-04, TC-F-010-05, TC-F-010-06, TC-F-010-07, TC-F-010-08, TC-F-010-09, TC-F-010-10, TC-F-010-11, TC-F-010-12, TC-F-010-13, TC-F-010-14, TC-F-010-15, TC-F-010-16, TC-F-010-17 |
| SSR-032 | CMP-VAL | ecl_val | ValUnit.InvalidSignalsAreSubstitutedSafely | - | TC-F-010-01, TC-F-010-02, TC-F-010-03, TC-F-010-04, TC-F-010-05, TC-F-010-06, TC-F-010-07, TC-F-010-08, TC-F-010-09, TC-F-010-10, TC-F-010-11, TC-F-010-12, TC-F-010-14, TC-F-010-15, TC-F-010-16, TC-F-010-17 |
| SSR-007 | CMP-VAL | ecl_val | StaleUnit.StaleKeepsApproachRiskTrueOnly, ValUnit.InvalidStaysInvalidWhenStale, ValUnit.StaleBoundaryAtPointTwoHundred, ValUnit.StaleInputsAreNotReflected | CoreIntegration.VectorVProducesExpectedStatusPerRow | TC-F-011 |
| SSR-008 | CMP-CORE, CMP-VAL | ecl_core, ecl_val | CoreUnit.TickDegradesAfterPointTwoHundred, StaleUnit.SnapshotGapAbovePointTwoHundredIsStale, StaleUnit.TickKeepsInvalidSourceTimestamp, StaleUnit.TickUsesElapsedSinceLastReception, ValUnit.TickBeforeFirstSnapshot, ValUnit.TickReusesLastSnapshotAndGoesStale | CoreIntegration.TickStaleThenRecover | TC-F-011 |
| SSR-009 | CMP-ARB | ecl_arb | CoreUnit.TickDegradesAfterPointTwoHundred, StaleUnit.CoreLocksApproachDoorWhileStale, StaleUnit.SnapshotGapAbovePointTwoHundredIsStale | CoreIntegration.TickStaleThenRecover | TC-F-011 |
| SSR-033 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, ValUnit.StaleBoundaryAtPointTwoHundred | - | TC-F-011 |
| SSR-010 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, StaleUnit.CoreLocksApproachDoorWhileStale | CoreIntegration.TickStaleThenRecover | TC-F-011 |
| SSR-034 | CMP-VAL | ecl_val | StaUnit.DecidesStateByPriorityTable | CoreIntegration.TickStaleThenRecover | TC-F-011 |
| SSR-029 | CMP-VAL | ecl_val | StaleUnit.TickKeepsInvalidSourceTimestamp, ValUnit.SourceTimestampRangeAndInvalidMeansStale | - | TC-F-010-16, TC-F-010-17 |
| SSR-011 | CMP-ARB | ecl_arb | ArbUnit.ApproachRiskLocksOnlyThatDoor, ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, StaleUnit.CoreLocksApproachDoorWhileStale, StaleUnit.StaleKeepsApproachRiskTrueOnly | - | TC-F-008 |
| SSR-012 | CMP-ARB | ecl_arb | ArbUnit.ApproachRiskLocksOnlyThatDoor | - | TC-F-008 |
| SSR-013 | CMP-ARB | ecl_arb | ArbUnit.ApproachRiskLocksOnlyThatDoor | - | TC-F-008 |
| SSR-014 | CMP-ARB | ecl_arb | ArbUnit.ApproachRiskLocksOnlyThatDoor | - | TC-F-008 |
| SSR-015 | CMP-OVR | ecl_ovr | OvrUnit.FirstSuppressionThenOverrideAtBoundary, OvrUnit.NonRequestsDoNotSuppress, StaleUnit.ApproachResolveDeletesFirstSuppressionTime | - | TC-F-008 |
| SSR-035 | CMP-OVR | ecl_ovr | OvrUnit.OverrideExpiresOnResolveOrTimeout, StaleUnit.ApproachResolveDeletesFirstSuppressionTime | - | TC-F-008 |
| SSR-016 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, OvrUnit.BlockedCommandsAreNotRecorded | - | TC-F-012 |
| SSR-017 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks | - | TC-F-012 |
| SSR-018 | CMP-STA | ecl_sta | StaUnit.DecidesStateByPriorityTable | - | TC-F-012 |
| SSR-019 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, ValUnit.HoldReasonPriority | - | TC-F-012 |
| SSR-020 | CMP-VAL | ecl_val | ValUnit.InvalidSignalsAreSubstitutedSafely | CoreIntegration.InvalidInputsAreSafelySubstitutedAndReported | TC-F-010-01, TC-F-010-02, TC-F-019 |
| SSR-021 | CMP-ARB, CMP-VAL | ecl_arb, ecl_val | ValUnit.InvalidSignalsAreSubstitutedSafely | - | TC-F-010-03, TC-F-010-04 |
| SSR-022 | CMP-VAL | ecl_val | ValUnit.InvalidSignalsAreSubstitutedSafely | CoreIntegration.InvalidInputsAreSafelySubstitutedAndReported | TC-F-010-08, TC-F-019 |
| SSR-023 | CMP-VAL | ecl_val | ValUnit.InvalidSignalsAreSubstitutedSafely, ValUnit.SpeedRangeBoundariesAndMoving | CoreIntegration.InvalidInputsAreSafelySubstitutedAndReported | TC-F-010-09, TC-F-010-10, TC-F-010-11, TC-F-010-12 |
| SSR-024 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, ValUnit.InvalidSignalsAreSubstitutedSafely | - | TC-F-010-14, TC-F-010-15 |
| SSR-037 | CMP-ARB | ecl_arb | ArbUnit.HoldKeepsPreviousExceptApproachDoorLocks, ValUnit.HoldReasonPriority | - | TC-F-010-14, TC-F-010-15 |
| SSR-025 | CMP-VAL | ecl_val | ValUnit.InvalidSignalsAreSubstitutedSafely | - | TC-F-010-05, TC-F-010-06, TC-F-010-07 |
| SSR-026 | CMP-ARB, CMP-STA | ecl_arb, ecl_sta | StaUnit.DecidesStateByPriorityTable, ValUnit.GearInvalidIsNotSubstituted | - | TC-F-010-01, TC-F-010-02, TC-F-010-03, TC-F-010-04, TC-F-010-05, TC-F-010-06, TC-F-010-07, TC-F-010-09, TC-F-010-10, TC-F-010-11, TC-F-010-12, TC-F-010-14, TC-F-010-15 |
| SSR-027 | CMP-ARB | ecl_arb | ArbUnit.CrashConfirmedReleasesBothAboveAll, ArbUnit.ForcedReleaseReasonsAndApproachPrecedence, ArbUnit.IgnitionOffReleasesUnlessMoving, ArbUnit.MergesReasonsByRankThenValue, ValUnit.HoldReasonPriority | CoreIntegration.VectorVProducesExpectedStatusPerRow | TC-F-019, TC-F-013-01~14 |
| SSR-028 | CMP-STA | ecl_sta | StaUnit.DecidesStateByPriorityTable | - | TC-F-019, TC-F-013-01~14 |
| SSR-030 | CMP-STA | ecl_sta | StaUnit.DecidesStateByPriorityTable | - | TC-F-013-01~14 |

- **시스템 테스트 미추적 요구사항**: SRQ-FUN-024, SRQ-FUN-025, SRQ-CON-008, SRQ-CON-009 (검증 방법이 INS/BLD로 정의된 제약. 사유: 시스템 테스트 `미결 사항` 시트 Q-01~Q-08).
- **단위 테스트 @tracing 없는 요구사항**: SRQ-FUN-024, SRQ-FUN-025, SRQ-LIM-002, SRQ-LIM-004, SRQ-CON-001, SRQ-CON-008, SRQ-CON-009, SRQ-CON-002, SRQ-CON-005, SRQ-CON-003, SRQ-CON-006, SRQ-CON-007, SRQ-CON-004 (UT가 검증 방법이 아니거나 통합/시스템에서 검증).

## 3. Use Case ↔ 요구사항 (양방향, G1 증거)

| UC | 요구사항 | 
|---|---|
| UC-01 | SRQ-FUN-001~008, 031, 038, SSR-027 |
| UC-02 | SRQ-FUN-004, 005, 009 |
| UC-03 | SSR-001~004, 031, 036 |
| UC-04 | SSR-011~015, 035, SRQ-FUN-017~020, 033~035 |
| UC-05 | SRQ-FUN-010, 011, 032 |
| UC-06 | SRQ-FUN-012, 013 |
| UC-07 | SRQ-FUN-014~016, 036 |
| UC-08 | SSR-005~010, 016~026, 029, 030, 032~034, 037 |
| UC-09 | SRQ-FUN-022~025 |
| UC-10 | SRQ-FUN-027~030, 037, 039, SRQ-LIM-001, 003 |

역방향(요구사항 → UC): 위 표의 각 요구사항은 표시된 UC에 속한다. UC에 속하지 않는 요구사항은 비기능/제약(SRQ-QUA-*, SRQ-LIM-*, SRQ-CON-*)과 SRQ-FUN-021, 026, SSR-028이다. SSR-027(중재 규칙)은 UC-01 행에 두었으며 UC-02~08에도 공통으로 적용되는 규칙이라 별도 UC를 두지 않는다. SSR-030은 UC-08에 속한다. UC에 속하지 않는 요구사항은 요구사항 기반 시스템 테스트(OEM §5 검증 수준)로 검증한다.

## 4. 점검 결과 (고아/미추적)

- **미추적 요구사항(하위 산출물 없음)**: 전 요구사항 — 아키텍처 설계(SWE.2) 이전이므로 정상.
- **OEM 요구 → SW 요구사항**: OEM-SR-001~004, FR-001~007, NFR-001~002, IF-001~009 모두 §1에서 하나 이상의 SW 요구사항으로 파생되었다(와일드카드 추적 없음).
- **SW 요구사항의 OEM 상위 열**: SRS/안전 문서의 "상위(OEM)" 열에서 §1의 OEM→SW 행을 스크립트로 생성했다(요구 상위 열과 §1이 일치). OEM 상위가 없는 요구사항은 SRQ-QUA-REL-001뿐이며 근거는 `implementation` 스킬이다.
- **OEM 입력 중 결정에 쓰이지 않는 항목**: gear(형식 검사·보고만, `OI-012`).
- **OEM 계획 SWR-019**: 결번(`OI-018`).
