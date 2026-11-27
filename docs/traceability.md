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

## 2. SW 요구사항 → 후속 산출물

| 요구사항 | 아키텍처 컴포넌트 | 상세 설계 모듈 | 소스 | 단위 테스트 | 통합 테스트 | 시스템 테스트 |
|---|---|---|---|---|---|---|
| SSR-001~037 | - | - | - | - | - | - |
| SRQ-FUN-001~038 | - | - | - | - | - | - |
| SRQ-QUA-*, LIM-*, CON-* | - | - | - | - | - | - |

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
| UC-10 | SRQ-FUN-027~030, 037, SRQ-LIM-001, 003 |

역방향(요구사항 → UC): 위 표의 각 요구사항은 표시된 UC에 속한다. UC에 속하지 않는 요구사항은 비기능/제약(SRQ-QUA-*, SRQ-LIM-*, SRQ-CON-*)과 SRQ-FUN-021, 026, SSR-028이다. SSR-027(중재 규칙)은 UC-01 행에 두었으며 UC-02~08에도 공통으로 적용되는 규칙이라 별도 UC를 두지 않는다. SSR-030은 UC-08에 속한다. UC에 속하지 않는 요구사항은 요구사항 기반 시스템 테스트(OEM §5 검증 수준)로 검증한다.

## 4. 점검 결과 (고아/미추적)

- **미추적 요구사항(하위 산출물 없음)**: 전 요구사항 — 아키텍처 설계(SWE.2) 이전이므로 정상.
- **OEM 요구 → SW 요구사항**: OEM-SR-001~004, FR-001~007, NFR-001~002, IF-001~009 모두 §1에서 하나 이상의 SW 요구사항으로 파생되었다(와일드카드 추적 없음).
- **SW 요구사항의 OEM 상위 열**: SRS/안전 문서의 "상위(OEM)" 열에서 §1의 OEM→SW 행을 스크립트로 생성했다(요구 상위 열과 §1이 일치). OEM 상위가 없는 요구사항은 SRQ-QUA-REL-001뿐이며 근거는 `implementation` 스킬이다.
- **OEM 입력 중 결정에 쓰이지 않는 항목**: gear(형식 검사·보고만, `OI-012`).
- **OEM 계획 SWR-019**: 결번(`OI-018`).
