# 요구사항 리뷰 기록 requirements-review-001

## 리뷰 기록
- 대상: SWE.1 소프트웨어 요구사항 명세 (`docs/SRS.md`, `docs/safety-requirements.md`, `docs/open-issues.md`, `docs/traceability.md`) / 버전: SRS-ECL-001 0.1, SSR-ECL-001 0.1, traceability 무버전 (전부 Draft) / 체크리스트: `.claude/skills/review/references/checklists.md` 1장 SWE.1 (무버전 기본 틀) + `requirements-analysis` 스킬의 분류·작성·추적 규칙 (프로젝트 전용 체크리스트 없음)
- 상위 입력: `OEM/OEM-SWR-001_OEM SW 요구사항 사양서.docx` (Rev 1.0, BL-OEM-1.0). 텍스트는 `/tmp/swr.txt`와 docx의 `word/document.xml`을 대조하여 핵심 수치(3 km/h, 10초, 300/200/100 ms, 1,000회, 100건, Python 3.12, 정지, 요청 시각, source_timestamp_s, Use Case, HTTP 500)가 일치함을 확인했다. 숨은 변경 이력·주석은 없다.
- 결과: **Rejected**
- 다음 단계 진행: **불가** (열린 결함 27건: Major 19건, Minor 8건. 전부 Open)
- 재리뷰 이력: 최초 리뷰이므로 이전 결함 closure 대상 없음.

### 판정 원칙 적용 메모
- OI-XXX 가정(Open)은 가정 자체로 결함을 면제하지 않는다. 가정이 요구사항 본문과 충돌하거나, 안전 측 실패를 낳거나, 이 문서가 기대 결과를 정의하지 못하게 하면 결함이다.
- 상위 입력 원문과 다르게 바뀐 내용(왜곡)은 Open 가정으로 표시돼 있어도 결함으로 기록했다.

## 1. 체크리스트 판정 (checklists.md 1장 SWE.1 전 항목 + requirements-analysis 규칙)

| # | 체크 항목 | 판정 | 근거/비고 |
|---|---|---|---|
| 1 | 기능/비기능 요구사항이 구분되어 있는가 | Pass | SRS.md §1(FUN-001~013), §2.1 품질, §2.2 제한, §2.3 제약으로 분리됨. (CON-003 문장 형식 결함은 R-23) |
| 2 | 기능 안전 요구사항이 별도 문서에서 관리되고 참조만 되어 있는가 | Fail | 안전 요구는 safety-requirements.md로 분리되어 있으나, 안전 중재가 SRS 본문에 남아 있음(FUN-010 충돌 CONFIRMED 최우선·sensor_fault 유지, FUN-004 override가 SWSR-005의 억제를 우회). SRS 본문 행에는 SWSR ID 참조가 없고 §3 매핑 표에만 있음. SRS.md:20-32, 40-42, 64-74 / R-06, R-03 |
| 3 | 비기능 요구사항이 품질/제한/제약으로 구분되어 있는가 | Fail | QUA-SEC-001(SRS.md:41)은 OEM §2의 "프로젝트 제약"인데 품질로 분류. CON-004(:58), CON-006(:60)은 공급자 프로세스 문장이며 SW 요구로 검증 불가. / R-23 |
| 4 | 품질 요구가 ISO/IEC 25010으로 분류되고 측정 가능한 기준이 있는가 | Fail | QUA-REL-001(:42)은 "정의된 오류코드"를 요구하나 오류코드 목록이 없고 "공개 함수"도 정의되지 않음. QUA-FUN-001(:40)은 OI-009(Open)에 의존해 해시 대상이 미정이므로 측정 기준이 실행 불가. / R-14, R-15 |
| 5 | 각 요구사항이 단일하고 모호하지 않으며 검증 가능한가 | Fail | 1의무 위반 13건(R-11), 미정의 용어와 모호 표현(R-12), 경계·조건 누락(R-13). FUN-004는 기대 출력 자체가 없어 검증 불가(R-02). FUN-009(:28)는 Web 계층 미결정(OI-017) 상태에서 IT로 검증한다고 명시(R-10). |
| 6 | 요구사항 간 충돌·중복이 없는가 | Fail | FUN-001(:20) "선택된 side에 적용"과 FUN-002(:21) "unlock은 P·3.0 미만에서만"이 직접 충돌(R-01). FUN-003/005/006/007과 SWSR-005/007 사이에 충돌이 있고 우선순위는 Open 가정(OI-006)에만 있음(R-03). |
| 7 | 필수 속성(ID, 근거, 우선순위, 검증 방법, 상태)이 있는가 | Fail | ID·근거·우선순위·검증·상위는 FUN/LIM/QUA 표에 있음. CON 표(:53-60)에는 우선순위·상위 열이 없고, safety 표(safety-requirements.md:11-19)에는 우선순위 열이 없음. "상태"는 요구별 열이 없고 SRS.md:14, safety:9에 "모두 Draft"로만 기재됨. / R-21 |
| 8 | 시스템 요구사항과 양방향 추적이 되며 고아 항목이 없는가 | Fail | 상위(OEM) 열은 전 항목에 있으나 다음이 깨짐: source_timestamp_s(IF-001)를 쓰는 요구가 없음(고아 입력, R-05). QUA-REL-001의 상위 OEM-SR-003은 원문(docx 161행)에 NULL 인자 내용이 없어 허위 추적(R-14). NFR-002 수용기준 "조회"에 대응 요구가 없음(R-08). traceability §3의 "고아 없음" 주장과 불일치(R-19). |
| 9 | 요구사항이 아키텍처/테스트와 양방향 추적 가능한 상태인가 | Pass | traceability.md §2에 아키텍처·상세설계·소스·UT·IT·ST 열이 존재하며, 미수행 단계는 "-"로 표시됨. SWE.1 단계에서 하위 산출물이 없는 것은 정상이다. 단 하위 단계의 ID 체계는 아직 정의되지 않음(확인 범위 명시). |
| R1 | (skill 분류 규칙 2) 안전 요구사항 분리와 ID 참조 | Fail | R-06. §3 매핑 표 외에는 SWSR 참조가 없고, 충돌하는 QM 요구에 참조가 없다. |
| R2 | (skill 분류 규칙 3) 품질 특성·제한·제약 분류 | Fail | R-23, R-14, R-15. LIM-002(:49)는 성능 한계로 분류 적합. |
| R3 | (skill 작성 규칙) shall 문장 1개 1의무 | Fail | R-11. CON-003(:57)은 "범위 밖이다"로 shall 문장이 아님(R-23). |
| R4 | (skill 작성 규칙) 모호 표현 금지, 수치·조건·단위 명시 | Fail | R-12, R-13. "유효한", "정지", "등"(CON-006), "정의된 오류코드", "최악 평가 시간" 등. |
| R5 | (skill 작성 규칙) ID 불변·비재사용 | Pass | FUN-001~013, QUA-*, LIM-*, CON-001~006, SWSR-001~007 모두 연속 번호이며 재사용 흔적 없음. OEM SWR-019 결번은 OI-018에 기록됨. |
| R6 | (skill 추적성) 추적 매트릭스 존재와 고아·미추적 점검 | Fail | 매트릭스는 존재하나 §3 점검 결과가 사실과 다름(R-19). 이슈↔요구사항 역링크 누락(R-18). |
| R7 | (skill 완료 조건) 일관성·모호성·검증 가능성 점검 완료 | Fail | 본 리뷰에서 다수 위반 확인. 작성자의 완료 주장은 사실과 맞지 않는다. |
| R8 | (skill 분류/추적) OEM 14개 요구의 누락·왜곡 | Fail | 아래 §2 표 참조. 13개 항목 중 왜곡·누락이 있는 항목이 다수. |
| R9 | (skill 추적) IF-001~009 매핑 | Fail | 아래 §2 표 참조. IF-001(source_timestamp_s 고아), IF-005(와일드카드 추적), IF-006(priority_reason 누락) 등. |
| R10 | 임시 가정(open-issues) 타당성 | Fail | 아래 §3 표 참조. 가정 다수가 요구 본문과 충돌하거나 안전 측이 아니다. |
| R11 | (OEM §8 G1 인도 증거) Use Case, 베이스라인 ID | Fail | docx 297-298행은 G1 증거로 "SW 요구사항, Use Case, 검토기록, 양방향 추적"과 베이스라인 BL-SWR-1.0을 요구. Use Case 산출물 없음, BL-SWR-1.0 미기재(R-26). |
| R12 | (저작권/공개 증거) OEM 원문 재배포 근거 | Fail | SRS.md:12는 "저장소 공개는 저작권자로부터 승인받았다(사용자 확인)"라고만 쓴다. docx 21행은 과정 밖 공개에 Synetics 사전 서면 승인을 요구한다. 서면 증거가 산출물에 없다(R-17). |

## 2. OEM 요구 14개 및 IF-001~009 대조

| OEM 항목 | SRS 대응 (위치) | 판정 | 누락·왜곡 내용 |
|---|---|---|---|
| OEM-SR-001 (충돌 CONFIRMED 우선 해제, 300 ms, 유지) | SWSR-001(safety:13), SWSR-002(:14), FUN-010(SRS:29) | 결함 | 우선순위는 "다른 명령보다 우선"으로 옮겨짐. 그러나 FUN-010 "한 개의 출력 결정"은 좌·우 독립 처리(SWSR-005)와 맞지 않음. PENDING 지속 상한 없음(OI-007). R-03, R-13 |
| OEM-SR-002 (접근위험 LOCK, 해제 억제, 기록) | SWSR-005(:17), SWSR-006(:18), FUN-004(SRS:23) | 결함 | OI-006 ③이 화재·과온·성인 강제해제를 접근위험 LOCK보다 위에 두어 SWSR-005의 "무예외 LOCK"과 충돌. FUN-004 override의 출력 효과 미정의(R-02, R-03). |
| OEM-SR-003 (freshness 200/100 ms, 형식·범위 거절) | SWSR-003(:15), SWSR-004(:16), LIM-002(SRS:49), QUA-REL-001(:42) | 결함 | INVALID 결과 미정의, fail-unsafe 가정(R-04). freshness를 호출자 스냅샷 시각으로 바꿔 stuck 값을 탐지 못함(R-05). QUA-REL-001의 상위 추적 허위(R-14). |
| OEM-SR-004 (sensor_fault 시 새 명령 미적용, 출력 유지, FAULT, 경고코드) | SWSR-007(:19) | 결함 | OEM 원문은 "새 명령을 적용하지 않고"인데 OI-006 ②는 모든 출력 유지로 범위를 넓힘(화재·ISOFIX 포함). "새 명령", "정규화 입력", "경고코드" 정의 없음(R-03, R-12). 다중 의무(R-11). |
| OEM-FR-001 (4개 source, L/R/ALL, 정지·정상입력에서 선택 출력만 변경) | FUN-001(SRS:20), FUN-002(:21), FUN-011(:30) | 결함 | FUN-002가 해제를 P·3.0 미만으로 제한하여, 원문 수용기준 "정지, 정상입력에서 각 source의 RELEASE"가 충족되지 않을 수 있다. "정지" 미정의. OI-010은 원문에 없는 조건을 추가하며 근거를 "SWR-004"로만 댄다(R-01). |
| OEM-FR-002 (3 km/h 이상 첫 주기 LOCK) | FUN-003(SRS:22) | 결함 | 3.0 기준은 원문과 일치. 그러나 FUN-007(시동 OFF RELEASE), FUN-005(화재 RELEASE)와 충돌하고 우선순위가 요구 본문에 없다(R-03). |
| OEM-FR-003 (10초 이하 재입력 시 명시적 override) | FUN-004(SRS:23), SWSR-006(:18) | 결함 | (a) 기준 시점을 원문 "접근위험 경고 후"에서 "첫 억제 시각"으로 바꿨으나 OI-011에 기록되지 않음. (b) 출력 효과, "override 상태"(OI-005 상태 열거에 없음), "같은 해제 명령"의 동일성, 10초 경과 후 동작이 없음. (c) 접근위험 해제는 QM 분류인데 ASIL B SWSR-005를 우회(R-02, R-06). |
| OEM-FR-004 (잠금·상태·입력 유효성·이유 제공) | FUN-008(SRS:27), FUN-009(:28) | 결함 | priority_reason 항목이 요구에서 빠짐(IF-006은 reason_code로도 매핑하라고 함). FUN-009의 HTTP 500은 Web 계층 결정(OI-017) 전이므로 IT 검증 불가(R-07, R-10). |
| OEM-FR-005 (화재·과온·성인 강제 해제) | FUN-005(SRS:24) | 결함 | 본문은 원문과 일치. 그러나 OI-006의 ②(sensor_fault 유지)가 이를 막을 수 있고 SWSR-007과 충돌한다. 안전 문서 참조 없음(R-03, R-06). 이유코드 목록 없음(R-07). |
| OEM-FR-006 (ISOFIX 한쪽 TRUE 시 해당 쪽만 LOCK) | FUN-006(SRS:25) | 결함 | 양쪽 TRUE 동작 미정의(R-13). ISOFIX와 시동 OFF 충돌은 OI-006에만 있고 FUN-007에 예외가 없음(R-03). |
| OEM-FR-007 (시동 OFF 시 초기 해제 상태 전환) | FUN-007(SRS:26) | 결함 | "유효한"이 없음(R-13). 시동 OFF 시 state=OFF인데 ISOFIX LOCK이면 출력이 LOCK이 되어 FUN-007 본문과 모순. 예외가 본문에 없음(R-03). |
| OEM-NFR-001 (결정론, 1,000회 해시 동일) | QUA-FUN-001(SRS:40) | 결함 | 해시 대상 필드·직렬화가 OI-009(Open)에 의존해 측정 기준이 실행 불가(R-15). |
| OEM-NFR-002 (100건 보존, 101건째 삭제, 조회, 허용 필드만, 재기동 시 소거) | FUN-012(:31), FUN-013(:32), LIM-001(:48), QUA-SEC-001(:41) | 결함 | 수용기준의 "최신 100건을 순서대로 조회"에 대응하는 요구와 인터페이스가 없음(R-08). FUN-012 "제어 결정이 발생할 때"의 트리거 미정의(R-13). |
| OEM-IF-001 (speed 0.0~300.0, gear P/N/D/R, source_timestamp_s) | SRS §3 행(SRS:66), SWSR-003 | 결함 | 범위·열거값이 요구 본문에 없음(R-13). source_timestamp_s를 쓰는 요구가 없음(R-05). |
| OEM-IF-002 (crash_status NONE/PENDING/CONFIRMED) | SWSR-001, 002 (§3 :67) | 결함 | 미정 값의 INVALID 결과가 없음(R-04). |
| OEM-IF-003 (approach_risk boolean) | SWSR-005, FUN-004 (§3 :68) | 결함 | INVALID를 "성립 아님"으로 처리하면 LOCK이 해제되어 fail-unsafe(R-04). |
| OEM-IF-004 (side, action, source, 요청 시각 = timestamp_s) | FUN-001, FUN-011 (§3 :69), CON-005 | 결함 | 거절 시 이유코드 없음(R-07). CON-005의 상위 근거가 traceability §1에서는 "사용자/CLAUDE.md"로 기재되어 불일치(R-27). |
| OEM-IF-005 (lock_left/right LOCK/RELEASE, 논리 출력) | §3 :70 "모든 출력 결정 요구사항" | 결함 | 와일드카드 추적. 출력 신호명·값을 정의하는 요구가 없음(R-19). |
| OEM-IF-006 (state, priority_reason, reason_code, input_validity; 직렬화 실패 HTTP 500) | FUN-008, FUN-009 (§3 :71) | 결함 | priority_reason 매핑 누락(R-07). HTTP 500은 Web 계층 전제 필요(R-10). |
| OEM-IF-007 (fire, overtemp, adult boolean) | FUN-005, SWSR-003 (§3 :72) | 결함 | INVALID 결과가 OI-008에 따라 "성립 아님"이라서 화재 강제해제가 실행되지 않는다(R-04). |
| OEM-IF-008 (isofix boolean) | FUN-006 (§3 :73) | 결함 | INVALID 결과 미정의(R-04). 양쪽 TRUE 미정의(R-13). |
| OEM-IF-009 (ignition_on, sensor_fault boolean) | FUN-007, SWSR-007 (§3 :74) | 결함 | ignition_on INVALID 처리 미정의(R-04). |

## 3. 임시 가정(open-issues.md) 타당성 판정

| OI | 쟁점 | 판정 | 근거 |
|---|---|---|---|
| OI-001 | 문서 지위 경계 | 보완 | 내용은 타당. 상태 Closed인데 확인 주체가 "-"이다(R-25). |
| OI-002 | 구현 언어 C11 전환 | 반려 | OEM 원문 docx 113행의 실행 환경은 Python 3.12 PC/SIL이다. 공급자 내부 "사용자 확정"만으로 Closed 처리하고 OEM 확인을 받지 않았다. OEM 편차이므로 Open이어야 한다(R-09). |
| OI-003 | 평가주기 10 ms | 보완 | 근거 없이 임의로 정했다. 요구 본문에 주기 요구가 없고 LIM-002에만 쓰인다(R-13). |
| OI-004 | freshness를 스냅샷 시각으로 판단 | 반려 | 호출자가 스냅샷마다 timestamp를 새로 부여하면 값이 고정된 센서도 항상 fresh가 된다. OEM IF-001이 source_timestamp_s를 준 취지를 무효화한다(R-05). |
| OI-005 | 상태 4종 (OFF/NORMAL/DEGRADED/FAULT) | 보완 | 4종 자체는 타당. 그러나 FUN-004의 "override 상태"가 목록에 없다(R-02). NORMAL 전이 조건과 OI-005 역링크 누락(R-18). |
| OI-006 | 동시 조건 우선순위 | 반려 | 요구 본문 간 충돌을 해소하는 규칙인데 본문에 반영되지 않았고 Open이다. ③ > ④는 SWSR-005의 ASIL B 요구를 QM 요구가 덮는 구조다. ② > ③은 FR-005를 막는다. 예외와 안전 근거를 요구 본문에 기재하고 OEM 확인을 받아야 한다(R-03). |
| OI-007 | PENDING 유지, CONFIRMED 후 래치 없음 | 보완 | CONFIRMED 해제 후 래치 없이 정상 중재로 복귀하는 정책은 안전 관점에서 근거가 필요하다. PENDING 지속 상한이 없다(R-13). |
| OI-008 | INVALID 신호는 조건 성립으로 보지 않음 | 반려 | 접근위험 INVALID에서 LOCK이 걸리지 않아 해제가 허용된다. 안전 측 실패다. SWSR-004의 stale 규칙을 INVALID에 연결한 것도 본문과 맞지 않는다(R-04). |
| OI-009 | 해시 대상·방식 | 보완 | 해시 대상 필드를 정해 두긴 했으나 측정 기준이 여전히 Open 항목에 의존한다(R-15). 역링크 없음(R-18). |
| OI-010 | 일반 해제 허용조건 P & 3.0 미만 | 반려 | 원문에 없는 조건을 추가했다. 근거로 든 "SWR-004"는 원문 기재가 없다. 원문 수용기준의 "정지, 정상입력" 해제와 충돌할 수 있다(R-01). |
| OI-011 | override 10.0초 포함, 해소 시 삭제 | 반려 | 기준 시점을 원문 "경고 후"에서 "첫 억제 시각"으로 바꿨는데 이를 기록하지 않았다. 출력 효과 미정의(R-02). |
| OI-012 | gear 사용처 | 타당 | OI-010 가정 안에서만 쓰인다. 다만 OI-010이 반려되므로 함께 재검토한다. 역링크 없음(R-18). |
| OI-013 | DEGRADED 시 직전 출력 유지, 복구 시 NORMAL | 반려 | 원문 SR-003은 "정의된 방식으로 처리"만 요구하며 출력 유지를 요구하지 않는다. 주행 중 속도 stale이면 RELEASE 유지 가능성이 있다. 안전 상태와 FTTI가 정의되지 않았다(R-16). |
| OI-014 | source enum만 처리, 채널 미구현 | 보완 | 근거 오독. docx 141행의 §4 금지는 Tesla 참고 대상에 대한 "모바일 앱·음성 제어 가능 주장"의 금지이며, OEM 자체 FR-001의 처리 요구를 금지하지 않는다(R-20). |
| OI-015 | SWSR-006(안전)과 FUN-004(QM) 분리 | 반려 | 억제 시각 기록을 안전으로 분리한 것은 타당하다. 그러나 FUN-004 override(접근위험 해제)를 QM으로 둔 것은 ASIL B 억제를 우회하는 결정이므로 타당하지 않다(R-06). |
| OI-016 | 이유코드 목록 "SRS 상세에서 확정" | 반려 | SRS에 "상세" 절이 없다. 이유코드 목록이 어디에도 없어 기대값을 판정할 수 없다(R-07). |
| OI-017 | Web 계층은 품질 기준 제외 | 반려 | CLAUDE.md §2 "모든 기준에 예외는 없다"에 위배된다. 예외 정책은 이 프로젝트에서 허용되지 않는다(R-10). |
| OI-018 | OEM SWR-019 결번 | 타당 | OEM에 확인을 요청하는 Open 항목으로 적절하다. 다만 결번이 누락된 요구일 가능성이 있으므로 OEM 답변 전까지 완전성 판단을 유보한다. |
| OI-019 | 법규 후보는 요구로 만들지 않음 | 타당 | 차종 확정 전 적용성을 확정하지 않는 원칙과 일치한다. |

## 4. 결함 목록

| ID | 심각도 | 위치 | 설명 | 조치 요청 | 상태 |
|---|---|---|---|---|---|
| R-01 | Major | `docs/SRS.md:20`, `:21`, `:30`; `docs/open-issues.md:16`; OEM 원문 169-172행 | FUN-001은 lock/unlock을 선택 출력에 무조건 적용하고, FUN-002는 unlock을 P·3.0 미만에서만 허용한다. 두 요구가 직접 충돌한다. "정지"가 미정의이고 OI-010의 조건이 원문 수용기준 "정지, 정상입력에서 각 source RELEASE"와 충돌할 수 있다. | FUN-001에 해제 조건을 명시하거나 FUN-002를 예외로 연결한다. "정지"를 정의하고 OI-010을 OEM에 확인받는다. 원문 수용기준과의 차이를 기록한다. | Open |
| R-02 | Major | `docs/SRS.md:23`; `docs/open-issues.md:17`, `:11`; OEM 원문 179-182행 | FUN-004의 override 결과(좌우 출력이 RELEASE인지 LOCK 유지인지)가 없어 기대값이 없다. "override 상태"는 OI-005의 상태 열거(OFF/NORMAL/DEGRADED/FAULT)에 없다. "같은 해제 명령"의 동일성(source·side)과 10초 경과 후 재입력 동작이 없다. 기준 시점을 원문 "경고 후"에서 "첫 억제 시각"으로 바꿨는데 OI-011에 기록되지 않았다. | 출력 효과, 상태, 기준 시각을 요구에 명시한다. "경고" 시각을 정의하거나 원문과의 차이를 OEM에 확인한다. 재입력 조건을 정의한다. | Open |
| R-03 | Major | `docs/SRS.md:20-26`, `:29`; `docs/safety-requirements.md:17` (SWSR-005), `:19` (SWSR-007); `docs/open-issues.md:12` (OI-006) | 요구 본문 간 충돌이 다수 있고, 우선순위는 Open 가정(OI-006)에만 있다. (a) SWSR-005는 접근위험 TRUE 시 무예외 LOCK인데 OI-006 ③은 화재·과온·성인 RELEASE를 ④ 접근위험 LOCK보다 위에 둔다. (b) FUN-005 RELEASE와 SWSR-007 출력 유지, OI-006 ②가 충돌한다. (c) FUN-003 LOCK(≥3.0), FUN-007 RELEASE(시동 OFF), FUN-006 LOCK(ISOFIX)이 충돌하고 FUN-007은 state=OFF인데 LOCK 출력이 가능하다. (d) SWSR-007은 원문대로 "새 명령 미적용"인데 OI-006 ②는 모든 출력 유지로 범위를 넓혔다. | 충돌하는 요구 본문마다 예외와 우선순위를 명시한다. 우선순위 표를 FUN-010 또는 안전 요구로 승격한다. OI-006을 OEM과 안전 담당에게 확인받는다. SWSR-007의 범위를 원문과 일치시킨다. 확인 전까지 상태를 Open으로 유지한다. | Open |
| R-04 | Major | `docs/safety-requirements.md:15`, `:16`; `docs/open-issues.md:14` (OI-008); `docs/SRS.md:24`, `:26`, `:25`, `:73` | INVALID 입력의 결과가 요구 본문에 없다. OI-008은 INVALID를 "조건 성립 아님"으로 처리하여 approach_risk INVALID 시 LOCK이 걸리지 않고 해제가 허용된다. 이는 안전 측 실패다. OI-008은 필수 입력 INVALID 지속을 SWSR-004(갱신 지연 규칙)에 연결하지만 SWSR-004 본문은 갱신 지연만 다룬다. fire, adult, isofix, ignition_on INVALID도 같은 문제가 있다. | INVALID 입력별 안전 측 출력을 안전 요구로 정의한다(접근위험 INVALID의 LOCK 등). INVALID 지속 조건을 SWSR-004 본문에 편입하거나 별도 요구를 만든다. 안전 분석 근거를 기록한다. | Open |
| R-05 | Major | `docs/open-issues.md:10` (OI-004); `docs/SRS.md:66` (IF-001 매핑); `docs/safety-requirements.md:16` (SWSR-004); OEM 원문 223행 | OI-004는 freshness 기준을 호출자가 스냅샷마다 부여하는 timestamp_s로 바꾼다. 값이 고정된 센서도 새 timestamp가 붙으면 항상 fresh로 판정되어 SWSR-004의 stale 탐지가 무력화된다. OEM IF-001의 source_timestamp_s는 어떤 요구 본문에서도 쓰이지 않는 고아 입력이다. | source_timestamp_s 기반 freshness 요구를 추가하거나, 가정의 한계(값 고정 미탐지)를 안전 영향과 함께 기록하고 OEM 확인을 받는다. | Open |
| R-06 | Major | `docs/SRS.md:21-29`(FUN-002~010 전반), `:23`(FUN-004), `:24`(FUN-005), `:29`(FUN-010); `docs/open-issues.md:21` (OI-015) | 안전 요구사항 분리 위반 소지. (a) FUN-010(우선순위 H)은 충돌 CONFIRMED 최우선과 sensor_fault 유지 등 안전 중재를 포함하는데 안전 문서에 없다. (b) FUN-004 override는 접근위험 도어의 해제 결정으로 SWSR-005의 ASIL B 억제를 우회하는데 QM으로 분류했다. (c) SRS 본문 행에 SWSR ID 참조가 없어 충돌하는 요구 쌍을 추적할 수 없다. | 안전 중재 규칙을 safety-requirements.md로 이관하거나, 안전 요구임을 인정하고 ASIL 분류와 간섭 방지 근거를 기록한다. 관련 FUN 행마다 SWSR ID를 참조한다. FUN-004의 분류를 재검토한다. | Open |
| R-07 | Major | `docs/SRS.md:21`, `:24`, `:26`, `:27`, `:30`; `docs/safety-requirements.md:18`, `:19`; `docs/open-issues.md:22` (OI-016) | 이유코드·경고코드 목록이 없다. OI-016은 "SRS 상세에서 확정"이라 했으나 SRS에 해당 절이 없다. OEM-IF-006의 priority_reason을 reason_code로 매핑하라는 요구가 빠졌다. FUN-011 거절에 이유코드가 없다. override 상태도 목록에 없다. 기대 출력값을 판정할 수 없다. | 이유코드 목록(ID, 값, 발생 조건, 우선순위)을 요구 또는 부속 표로 추가한다. priority_reason→reason_code 매핑을 명시한다. FUN-011 거절 시 이유코드를 정의한다. OI-016 상태를 정정한다. | Open |
| R-08 | Major | OEM 원문 209-212행(NFR-002 수용기준 "최신 100건만 순서대로 조회"); `docs/SRS.md:31`, `:32`, `:48` | 이벤트 조회 요구와 조회 인터페이스가 없다. LIM-001 수용기준의 "시간순 조회"가 검증 불가다. §3 인터페이스 매핑에도 이벤트 조회가 없다. | 이벤트 조회 기능 요구와 인터페이스 항목을 추가한다. 조회 방식이 OEM 범위인지 확인한다. | Open |
| R-09 | Major | `docs/open-issues.md:8` (OI-002 Closed); `docs/SRS.md:55` (CON-001); OEM 원문 113행 | OEM 입력의 실행 환경은 "Python 3.12 PC/SIL"이다. 공급자가 "사용자 확정"만으로 C11로 바꾸고 OI-002를 Closed로 처리했다. OEM 승인 증거가 없고 편차가 OEM에 전달되지 않았다. | OI-002를 Open으로 되돌리고 OEM에 편차를 확인받는다. 확인 전까지 CON-001의 상태를 가정으로 표시한다. OEM 승인 결과를 기록한다. | Open |
| R-10 | Major | `docs/open-issues.md:23` (OI-017); `docs/SRS.md:27`, `:28`; `CLAUDE.md` §2("모든 기준에 예외는 없다") | Web 계층을 품질 기준에서 제외하는 가정은 CLAUDE.md 예외 불가 원칙에 위배된다. FUN-008/009는 Web 계층 결정 전에 IT로 검증한다고 명시했으나 검증 대상이 없다. FUN-009의 "HTTP 500"은 C 코어 요구에 Web 계층 개념을 섞은 것이다. | Web 계층에도 품질 기준을 적용하거나 CLAUDE.md 변경 절차를 거친다. FUN-008/009를 코어 요구(직렬화 실패 시 오류코드 반환)와 Web 요구로 분리하고 검증 방법을 확정한다. | Open |
| R-11 | Major | `docs/SRS.md:20`(FUN-001), `:21`(FUN-002), `:24`(FUN-005), `:26`(FUN-007), `:30`(FUN-011), `:31`(FUN-012), `:42`(QUA-REL-001), `:48`(LIM-001), `:56`(CON-002); `docs/safety-requirements.md:13`(SWSR-001), `:16`(SWSR-004), `:17`(SWSR-005), `:18`(SWSR-006), `:19`(SWSR-007) | "1요구사항 1의무" 위반. 예: FUN-007(RELEASE + state OFF + 이유코드 3의무), SWSR-005(LOCK + 억제 + 기록 3의무), SWSR-007(미적용 + 유지 + FAULT + 경고코드 4의무), FUN-002(조건부 반영 + 거절 + 이유코드). 테스트 케이스 하나가 여러 의무를 나눠 검증할 수 없다. | 복합 요구를 의무 단위 ID로 분리한다(예: SWSR-007a~d). 분리가 불가능하다면 근거를 기록한다. | Open |
| R-12 | Major | `docs/SRS.md:22`, `:24`, `:25`, `:26`, `:29`, `:42`, `:49`, `:60`; `docs/safety-requirements.md:13`, `:14`, `:16`, `:19`; `docs/SRS.md:59`; OEM 원문 171행 | 정의되지 않은 용어와 모호 표현. "유효한"(SWRS-FUN-003/005/006/007, SWSR-001), "정규화 입력"(SWSR-007), "직전 확정 출력"(SWSR-002/004/007), "VehicleSnapshot"(CON-005), "정지"(OEM FR-001), "제어 결정이 발생할 때"(FUN-012), "정의된 오류코드"와 "공개 함수"(QUA-REL-001), "최악 평가 시간"(LIM-002), "같은 해제 명령"과 "override 상태"(FUN-004), "경고코드"(SWSR-007), "접근위험 해소"(OI-011), "등"(CON-006), "시각 단위는 초(s)"와 ms 혼용(SRS:14). 용어집이 없다. | 용어집을 만들고 각 용어를 정의한다. "등"을 제거한다. 시각과 기간의 단위를 구분한다. | Open |
| R-13 | Major | `docs/SRS.md:25`(FUN-006 양쪽 TRUE), `:26`(FUN-007 "유효한" 누락), `:31`(FUN-012 트리거·오버플로), `:49`(LIM-002 평가주기 요구 부재); `docs/safety-requirements.md:14`(SWSR-002 PENDING 지속 상한); `docs/SRS.md:66`(IF-001 범위 0.0~300.0 등) | 경계와 조건이 요구 본문에 없다. FUN-006 양쪽 ISOFIX TRUE 동작이 없다. FUN-007 ignition_on의 유효성이 없다. speed 범위와 gear 열거값이 요구 본문에 없어 경계값 분석이 불가하다. 평가주기 10 ms가 요구로 존재하지 않는다(OI-003). PENDING 지속 상한이 없다. ID 카운터 오버플로 동작이 없다. | 각 요구에 경계·조건·예외를 명시한다. 평가주기를 독립 요구로 정의하고 근거를 기록한다. 범위와 열거값을 요구 본문 수용기준에 포함한다. | Open |
| R-14 | Major | `docs/SRS.md:42` (QUA-REL-001); OEM 원문 161행(OEM-SR-003); `docs/traceability.md:12` | QUA-REL-001의 상위(OEM) 추적 OEM-SR-003은 입력 유효성과 freshness에 관한 것으로 NULL 인자 처리 내용이 없다. "파생(코딩 규칙)"이라 하면서 OEM 상위를 붙인 허위 추적이다. "정의된 오류코드"의 목록도 없다. | 상위 근거를 정정한다(CLAUDE.md 또는 프로젝트 코딩 규칙, 또는 OEM 확인). 오류코드 목록을 정의하고 "공개 함수" 목록을 정한다. | Open |
| R-15 | Major | `docs/SRS.md:40` (QUA-FUN-001); `docs/open-issues.md:15` (OI-009) | 측정 기준이 실행 불가다. 해시 대상 필드와 해시 방식이 Open(OI-009)이고 측정 기준 본문에는 "공개 결과 해시"만 있다. 검증 방법 ST만으로는 판정 기준을 재현할 수 없다. | 해시 대상 필드, 직렬화 규칙, 해시 알고리즘, 고정 시계 정의를 측정 기준에 직접 기재한다. OI-009 역링크를 추가한다. | Open |
| R-16 | Major | `docs/safety-requirements.md:16` (SWSR-004), `:9`; `docs/open-issues.md:19` (OI-013); OEM 원문 161-162행 | DEGRADED에서 직전 확정 출력을 유지하는 동작은 OEM 원문(SR-003: "정의된 방식으로 처리")에 없다. OI-013 가정이 Open인 채로 안전 요구 "shall"이 되었다. 주행 중 속도 stale 시 RELEASE 유지 가능성이 있다. 안전 상태(safe state), FTTI, 결함 시 동작 정의가 없다. | 안전 상태와 FTTI를 정의하고 근거를 기록한다. DEGRADED 출력 정책을 OEM 안전 담당에게 확인받는다. 확인 전까지 상태를 Open 가정으로 명시한다. | Open |
| R-17 | Major | `docs/SRS.md:12`; OEM 원문 21행(과정 밖 배포·공개는 Synetics 사전 서면 승인 필요); `D:\Repo\Claude_Clang\OEM\` 폴더 | "저장소 공개는 저작권자로부터 승인받았다(사용자 확인)"는 구두 확인에 기댄다. 서면 증거가 산출물에 없다. 공개 저장소에 OEM 원문 docx가 포함되는지도 확인되지 않았다. | 서면 승인 증거를 저장소 내 지정 위치에 보관하고 경로를 기재한다. 승인이 없으면 OEM 원문을 비공개 처리하고 재리뷰를 받는다. | Open |
| R-18 | Major | `docs/open-issues.md:7-25` "영향 요구사항"; `docs/SRS.md` 본문 | 이슈 → 요구사항 역링크가 불완전하다. OI-002(CON-001), OI-005(FUN-007, SWSR-004/007), OI-008(SWSR-003/004), OI-009(QUA-FUN-001), OI-012(FUN-002), OI-014(FUN-001), OI-017(FUN-008/009)은 해당 요구 본문이나 근거 열에 OI 참조가 없다. 가정이 바뀌었을 때 영향 분석이 불완전하다. | 영향받는 모든 요구의 근거 열에 OI ID를 기재하고 역링크를 맞춘다. | Open |
| R-19 | Major | `docs/traceability.md:37-39` (§3 점검 결과), `:24` (CON 행), `:12-13`; `docs/SRS.md:70` (IF-005) | §3 점검 결과 "고아 산출물: 없음"은 사실과 다르다(source_timestamp_s 고아, QUA-REL-001 허위 추적). IF-005의 "모든 출력 결정 요구사항"은 와일드카드라서 실제 추적이 아니다. §1 CON 행이 "(사용자/CLAUDE.md)"로 기재되었으나 CON-005는 OEM-IF-004/NFR-001이 근거다. | §3 점검을 다시 수행하고 결과를 정정한다. IF-005의 출력 정의 요구를 추가하여 구체 추적을 한다. CON-005 행을 SRS 근거와 맞춘다. | Open |
| R-20 | Minor | `docs/open-issues.md:20` (OI-014) | 근거 오독. docx 141행은 Tesla 참고 대상에서 "모바일 앱·음성 제어 가능" 주장을 금지한 것이며, OEM의 자체 FR-001 처리 요구를 금지하지 않는다. | OI-014의 근거를 정정한다. source enum 처리와 채널 구현 제외 가정을 OEM FR-001 문언에 맞춰 다시 쓴다. | Open |
| R-21 | Minor | `docs/SRS.md:53-60` (§2.3 CON 표); `docs/safety-requirements.md:11-19` (안전 표); `docs/SRS.md:14` | 필수 속성 누락. CON 표에는 우선순위와 상위(OEM) 열이 없다. 안전 표에는 우선순위 열이 없다. 요구별 상태가 기록되지 않고 전체 문구로만 "Draft"라고 적혀 있다. | CON과 안전 표에 우선순위와 상위 열을 추가한다. 요구별 상태 열을 추가하거나 표준 문구를 각 표 머리에 적는다. | Open |
| R-22 | Minor | `docs/SRS.md:55-60` (검증 열 "빌드", "CI", "검토"); `docs/SRS.md:14` (검증 코드 UT/IT/ST); OEM 원문 170-202행(검증 수준 "PC/SIL/Web") | 검증 방법 코드가 머리글 정의(UT/IT/ST)와 맞지 않는다. OEM 검증 수준의 Web 항목이 SRS에서 빠졌다(OI-017 연계). | 검증 코드 체계를 정의하고 CON의 검증 열을 맞춘다. OEM 검증 수준을 열로 옮기거나 누락 근거를 기록한다. | Open |
| R-23 | Minor | `docs/SRS.md:57` (CON-003 "범위 밖이다", shall 아님); `:58` (CON-004); `:60` (CON-006); `:41` (QUA-SEC-001) | 분류·작성 오류. CON-003은 범위 선언이라 shall 요구가 아니다. CON-004("SW는 HARA를 재구성하지 않아야 한다")와 CON-006(프로세스 준수)은 SW 제품 요구로서 검증 불가하다. QUA-SEC-001은 OEM §2 "프로젝트 제약"인데 품질 특성으로 분류됐다. | CON-003은 문서 범위 절로 옮긴다. CON-004와 CON-006은 프로세스 문서로 옮기거나 SW가 검증 가능한 형태로 고친다. QUA-SEC-001은 제약 분류로 옮긴다. | Open |
| R-24 | Minor | `docs/SRS.md` 전반(SWRS-*) 및 `docs/safety-requirements.md` 전반(SWSR-*) | 두 ID 접두어 SWRS와 SWSR가 시각적으로 혼동된다. 이 문서들은 서로 참조하므로 오기 위험이 높다. | 접두어를 구분되는 형태로 정하고 참조를 일괄 정정한다. 변경 이력을 남긴다. | Open |
| R-25 | Minor | `docs/open-issues.md:7`(OI-001), `:8`(OI-002) | Closed 항목에 확인 주체·일자·근거가 없다(OI-001은 확인 주체 "-"). | Closed 항목에 확인자, 일자, 근거 링크를 기록한다. | Open |
| R-26 | Minor | OEM 원문 297-298행(G1: "Use Case", "BL-SWR-1.0"); `docs/SRS.md:5` (문서 ID/버전) | OEM G1 인도 증거에 Use Case가 없고 베이스라인 ID BL-SWR-1.0이 기재되지 않았다. | Use Case(또는 대응 시나리오)를 작성하거나 OEM과 대체 증거를 합의한다. 베이스라인 ID를 기재한다. | Open |
| R-27 | Minor | `docs/traceability.md:25` (CON 행 "(사용자/CLAUDE.md)"); `docs/SRS.md:59` (CON-005 근거 OEM-IF-004, OEM-NFR-001) | SRS와 추적표의 CON-005 근거가 일치하지 않는다. | 추적표와 SRS의 근거를 같은 값으로 맞춘다. | Open |

## 5. 확인 불가 항목 (Fail 처리)
- 아키텍처·상세설계·소스·테스트 추적은 해당 산출물이 없으므로 확인할 수 없다. 이 단계에서는 정상이나 하위 단계에서 ID 체계를 정의해야 한다.
- OEM 원문 외부의 고객 측 승인(서면) 증거는 확인할 수 없다(R-17).
- OEM-SWR-019 결번의 내용은 확인할 수 없다(OI-018, OEM 답변 필요).

## 6. 요구사항 리뷰 결론
- 최종 판정: **Rejected**
- 열린 결함: **27건** (Major 19, Minor 8), 종결 0건
- 다음 단계 진행: **불가** (열린 결함 27건)
- 재리뷰 조건: 위 결함 27건을 각각 조치하고 수정 내용을 문서에서 확인할 수 있어야 한다. 특히 R-01~R-06의 요구 본문 충돌과 R-08, R-17 조치 없이는 재리뷰 합격이 불가하다. OI-002, OI-004, OI-006, OI-008, OI-010, OI-011, OI-013, OI-016, OI-017은 OEM 또는 내부 담당의 확인 전까지 해당 요구 본문을 확정할 수 없다. 확인 없이 Approved로 가는 경로는 없다.

## 변경 이력
- 2026-11-27 최초 작성 (requirements-review-001, requirements-analysis 체크리스트 1장 SWE.1 적용)
