# 요구사항 리뷰 기록 requirements-review-002

## 리뷰 기록
- 대상: SWE.1 소프트웨어 요구사항 명세 재리뷰 (`docs/SRS.md` v0.2, `docs/safety-requirements.md` v0.2, `docs/open-issues.md`, `docs/traceability.md`, `docs/decisions.md`(신규), `docs/use-cases.md`(신규)) / 버전: SRS-ECL-001 0.2, SSR-ECL-001 0.2, 나머지 무버전 (전부 Draft) / 체크리스트: `.claude/skills/review/references/checklists.md` 1장 SWE.1 + `requirements-analysis` 스킬 분류·작성·추적 규칙 (프로젝트 전용 체크리스트 없음)
- 선행 기록: `docs/reviews/requirements-review-001.md` (Rejected, R-01~R-27)
- 상위 입력: `OEM/OEM-SWR-001_OEM SW 요구사항 사양서.docx` (Rev 1.0, BL-OEM-1.0). docx의 `word/document.xml`을 스크래치 위치에 풀어 본문을 직접 대조했고, `/tmp/swr.txt` 추출본과 일치함을 확인했다. OEM 원문의 수용기준 문언(§5)을 기준으로 판정했다.
- 결과: **Rejected**
- 다음 단계 진행: **불가** (열린 결함 29건: Major 25건, Minor 4건. 종결 11건)
- 판정 원칙 적용 메모: OI 가정은 가정 자체로 결함을 면제하지 않는다. 다만 에이전트가 해소할 수 없는 외부 확인(OEM·안전 담당 확인, 서면 승인, 베이스라인 부여)은 숨기지 않고 Open으로 두고 사유를 적었다.

---

## 1. 1차 결함 R-01~R-27 종결 판정

판정 기준: 수정된 산출물을 열어 결함의 원인이 제거되었는지 확인했다. 외부 확인이 필요한 부분만 남아 있으면 해당 결함은 Open이다.

| ID | 원 심각도 | 판정 | 근거 (수정본 위치 확인) | 잔여 사항 |
|---|---|---|---|---|
| R-01 | Major | Closed | FUN-001(SRS:46)은 수신만 정의. lock은 FUN-002(:47), 정지 시 unlock은 FUN-003(:48)로 분리. FUN-002의 unlock 조건과 FUN-001의 충돌 제거. "정지"는 용어집(:30)에 정의됨. | OI-010은 OEM 확인 전까지 Open (정상). 신규 N-01과 별개. |
| R-02 | Major | Closed | 용어집 "같은 해제 명령"(:31), "첫 억제 시각"(:32). override 효과는 LOCK 유지(FUN-018, :73). override 상태는 도어별 boolean(FUN-019, :74). 10.0 s 초과 재입력 처리(FUN-020, :75). | "경고 후"→"첫 억제 시각" 차이는 OI-011에 기록되어 있으나 OEM 확인 전까지 Open. 신규 N-07 참조. |
| R-03 | Major | **Open** | 충돌 규칙은 SSR-027 우선순위 표(safety:87~95)로 승격되어 (a)(b)(c)는 본문에 명시됨. 그러나 (d) SSR-016(safety:57)은 원문대로 "새 명령 미적용"인데 P2(safety:88)는 sensor_fault 시 **모든 규칙**을 직전 확정 출력으로 덮는다. 범위 확장을 본문이 그대로 가진다. | OI-006(OEM·안전 담당 확인)은 에이전트가 해소 불가하여 Open. 추가로 P2·P3 vs P5 충돌은 N-04. |
| R-04 | Major | Closed | INVALID 결과가 신호별로 정의됨(SSR-020~025, safety:68~73). 접근위험 INVALID→LOCK(SSR-020), 화재류 INVALID→FALSE(SSR-025), crash/ignition INVALID→직전 출력 유지(SSR-024). | 안전 측 가정의 OEM 확인은 OI-008 Open. 신규 N-03(SSR-006 모순), N-09(source_timestamp_s 미정의). |
| R-05 | Major | Closed | SSR-007(safety:28)이 `timestamp_s − source_timestamp_s > 0.200 s`로 source_timestamp_s를 사용. IF-001 매핑(SRS:126)에 SSR-007 반영. | 값 고정 센서 미탐지 한계는 OI-004 Open에 안전 영향과 함께 기록 (OEM 확인 필요). SSR-008 트리거 문제는 N-05. |
| R-06 | Major | **Open** | (a) FUN-010의 중재는 SSR-027로 이관됨. (b) override가 출력을 바꾸지 않게 되어 ASIL B 억제 우회 제거(FUN-018). QM 분류 근거는 OI-015에 기록. (c) **미완**: SRS 기능 행 30개 중 SSR을 참조하는 행은 SRQ-FUN-015(:65, 존재하지 않는 SSR-030 참조)와 SRQ-FUN-018(:73) 두 건뿐이다. 충돌·중재가 걸린 FUN-004(:49), FUN-010(:60), FUN-012(:62), FUN-014(:64), FUN-017(:72) 등에 SSR ID 참조가 없다. | (c) 참조 누락과 SSR-030 오기 수정 필요. |
| R-07 | Major | Closed | 부록 A 이유코드 표(SRS:136~158): ID, 값, 발생 조건, 우선순위. priority_reason=reason_code(FUN-022, :82). 거절 코드(FUN-008, :53). override 상태(FUN-019, :74). 경고코드 정의(:35). | 부록 A 자체의 우선순위 충돌은 N-01. OI-016은 OEM 확인 Open. |
| R-08 | Major | **Open** | SRQ-FUN-030(:95) 이벤트 조회 요구 추가. 그러나 §3 인터페이스 매핑(SRS:124~134)과 traceability.md에 조회 인터페이스 항목이 없다. OEM 입력에도 조회 인터페이스가 없어 조회 방식의 OEM 범위 확인 기록도 없다. | 인터페이스 행 추가 및 OEM 범위 확인 기록 필요. |
| R-09 | Major | **Open** | OI-002가 Open으로 복구되어 있고 OEM 편차로 기록됨(open-issues:8). 그러나 SRQ-CON-001(SRS:117) 행에 OI-002 역링크가 없고, "가정"으로 표시되지도 않았다. 상태는 H/Draft로 확정 요구와 같다. | CON-001에 OI-002 참조와 가정 표시 추가. OEM 승인 결과 기록은 외부 확인 (Open 유지). |
| R-10 | Major | **Open** | OI-017(open-issues:23)에서 Web 계층에도 품질 기준을 적용하도록 정정됨. FUN-008/009가 코어 오류(FUN-023, :83)와 Web HTTP 500(FUN-024, :84)으로 분리됨. **잔여**: CON-001(:117)은 "제품 코어"만 C로 제한하는데 Web 구현 언어는 미정이다. CLAUDE.md §2의 도구(CppCheck, Lizard, MC/DC)는 C/C++ 대상이므로 Web 계층에 적용 가능한 측정 기준이 정의되지 않았다. | Web 계층에 적용할 측정 기준 정의 필요. Web 구현 방식 확정 전까지 ST(Web) 검증 환경이 없다. |
| R-11 | Major | **Open** | 1요구 1의무 위반이 다수 잔존하며 분리도 근거 기록도 없다. 대표: SRQ-FUN-007(:52, 거절+출력 불변), FUN-011(:61, 기록+선택 규칙), FUN-019(:74, 상태+기록), FUN-020(:75, 처리+갱신), FUN-026(:86, 출력+상태), SSR-003(:19, 유지+기록), SSR-004(:20, 해제+복귀+기록), SSR-006(:27, 미사용+보고), SSR-009(:30, 상태+기록), SSR-010(:31, 유지+복귀), SSR-015(:51, 기록+삭제), SSR-024(:72, 유지+기록), LIM-001(:110, 보존+삭제), LIM-002(:111, 주기+완료시간), CON-002(:118), CON-003(:119), CON-004(:120). | 의무 단위 분리 또는 분리 불가 근거 기록. |
| R-12 | Major | **Open** | 용어집 추가(SRS:22~36). "VehicleSnapshot", "등" 제거. "적용"의 정의(SRS:40). **잔여**: "필수 입력"(SRS:145, safety:30)은 목록이 없다. "정상 입력"(SRS:47~48)은 정의가 없다. "경고"(OI-011 "경고 후")는 정의가 없다. PENDING "지속"의 시작 시각(SSR-004, safety:20)은 정의가 없다. 단위 혼용은 N-12. | 잔여 용어 정의 추가. |
| R-13 | Major | **Open** | FUN-006 양쪽 TRUE(SRQ-FUN-012, :62)와 FUN-007 "유효한"(:64) 반영. FUN-012 트리거는 FUN-027(:92)로 정의됨. LIM-002는 요구로 존재(:111). PENDING 상한 1.0 s(SSR-004). 범위·열거값 표(safety:35~41). **잔여**: 이벤트 ID 64비트 카운터 오버플로 동작 없음(SRQ-FUN-028, :93). | 오버플로 동작 정의. |
| R-14 | Major | **Open** | 상위 허위 추적 제거: "파생(프로젝트 코딩 규칙), OEM 상위 없음"(SRS:104, traceability:34). 근거 확인: `.claude/skills/implementation/SKILL.md:32~33`에 NULL 입력 시 `RET_ERR_NULL` 규칙이 실제로 존재함. **잔여**: 오류코드 값은 "상세 값은 설계에서 정의한다"로 이월되었다(SRS:160). RET_ERR_SERIALIZE는 값이 없어 SRQ-FUN-023(:83)의 기대값을 판정할 수 없다. | 오류코드 값을 요구 단계에서 정의하거나, 값 불필요 근거를 기록. |
| R-15 | Major | **Open** | 측정 기준 본문 명시: 직렬화 형식 `필드=값;`, SHA-256, 1,000회, 고정 timestamp_s 주입(QUA-FUN-001, SRS:103). OI-009 역링크 존재. **잔여**: 측정 입력 "시험 벡터 V(부록 B-2)"는 B-2가 시나리오 나열일 뿐이고 구체 벡터는 "시험 설계에서 파일로 확정"(SRS:165)이다. 실행 가능한 입력이 정의되지 않았다. | 벡터 V의 구체 값 또는 파일 위치와 해시 기준값 정의. |
| R-16 | Major | **Open** | 안전 상태·FTTI 가정 명시(safety:11), OEM 시간 허용치 인용, 잔여 위험 기록. **그러나** 가정한 안전 상태 "접근위험은 LOCK"(safety:11)을 SSR-027의 P2·P3 유지 규칙(safety:88~89)이 깨뜨린다. 신규 N-04. | 안전 상태 정의와 우선순위 표를 일치시킨 뒤 OEM 안전 담당 확인 (확인은 Open). |
| R-17 | Major | **Open** | `docs/approvals/` 디렉터리가 없다. DEC-002(decisions:7)는 "사용자 진술만 있음"이며 서면 승인 파일명 기재 칸이 비어 있다("미제출"). 서면 증거는 에이전트가 생성할 수 없다. | 사용자가 서면 승인 파일을 제출하고 경로를 DEC-002에 기재해야 한다. 에이전트 해소 불가로 Open 유지. |
| R-18 | Major | **Open** | 이슈→요구 역링크 누락 확인: OI-002(→CON-001, SRS:117), OI-003(→CON-002 :118, SSR-001 :17, SSR-009 :30), OI-005(→FUN-015 :65), OI-006(→FUN-012 :62), OI-011(→FUN-019 :74), OI-012(→FUN-003 :48, FUN-004 :49), OI-013(→SSR-009 :30), OI-015(→FUN-017 :72). 모두 open-issues의 "영향 요구사항" 열에는 기재되어 있으나 요구 행 근거 열에는 OI가 없다. | 누락된 OI 참조 추가. |
| R-19 | Major | **Open** | IF-005 와일드카드 제거(SRS:130, traceability:27) 확인. CON-005 불일치는 행 자체가 제거되어 해소(R-27). **잔여**: traceability §3(:49)은 "OEM 상위가 없는 파생: SRQ-FUN-005, 008, 021, 026, SSR-004, 027, 028"이라 적었으나, SRS 본문은 이 항목들에 OEM 상위를 기재한다(예: FUN-005 "OEM-FR-001", FUN-021 "OEM-FR-001~007", SSR-027 "OEM-SR-001~004, OEM-FR-001~007", SSR-004 "OEM-SR-001"). 두 문서가 서로 모순된다. | §3을 SRS 상위 열 기준으로 재작성. |
| R-20 | Minor | Closed | OI-014(open-issues:20) 근거를 FR-001 문언 기준으로 정정. §4 금지는 Tesla 참고 항목으로 한정 기재됨. | 없음. |
| R-21 | Minor | **Open** | SRS CON 표(SRS:115)에 우선/상태, 상위(OEM) 열 추가 확인. **잔여**: safety 표 6개(:15, :24, :45, :55, :66, :78)에 우선/상태 열이 없다. safety:9는 "`우선순위 열`의 우선순위는 전부 H"와 "`ASIL 열`" 표기를 말하지만 두 열 모두 존재하지 않는다. | 안전 표에 우선/상태 열 추가 또는 표준 문구를 표마다 기재하고 safety:9 문장 정정. |
| R-22 | Minor | Closed | 검증 코드 정의(SRS:20): UT, IT, ST, INS, BLD. Web은 ST(Web)로 명시. CON 행의 BLD/INS 사용과 일치. | 없음. |
| R-23 | Minor | Closed | CON-003은 shall 문장(SRS:119). CON-004는 INS로 검증 가능(:120). "범위 밖" 문장과 프로세스 조건(구 CON-004, CON-006)은 제거됨. QUA-SEC-001은 제거되어 CON-003(제약)에 편입. | 1요구1의무 위반(CON-003)은 R-11로 추적. |
| R-24 | Minor | Closed | 접두어 SRQ(일반)와 SSR(안전)로 분리(SRS:14). | 번호 재부여 문제는 N-11. |
| R-25 | Minor | Closed | OI-001 확인 주체·일자·근거 기재(open-issues:7). OI-002 확인 주체 기재(:8). | 없음. |
| R-26 | Minor | **Open** | Use Case 작성 완료(`docs/use-cases.md`, UC-01~10). 베이스라인 ID는 "승인 시 부여, 현재 미부여"(SRS:6). 이 값은 OEM 승인 전에는 부여할 수 없다. OI-020(open-issues:27) Open. | BL-SWR-1.0 부여는 승인 절차 필요 (에이전트 해소 불가). UC 추적 누락은 N-10. |
| R-27 | Minor | Closed | traceability §1에서 "(사용자/CLAUDE.md)" 기재 제거. SRQ-CON-002(SRS:118) 근거 OEM-NFR-001, OEM-IF-004가 traceability §1(:21, :26)과 일치. | 없음. |

**1차 결함 종결 집계**: Closed 11건 (R-01, R-02, R-04, R-05, R-07, R-20, R-22, R-23, R-24, R-25, R-27), Open 16건 (R-03, R-06, R-08, R-09, R-10, R-11, R-12, R-13, R-14, R-15, R-16, R-17, R-18, R-19, R-21, R-26).

---

## 2. 체크리스트 판정 (checklists.md 1장 SWE.1 + requirements-analysis 규칙)

| # | 체크 항목 | 판정 | 근거/비고 |
|---|---|---|---|
| 1 | 기능/비기능 요구사항이 구분되어 있는가 | Pass | 기능 §1(SRS:38~96), 비기능 §2(SRS:97~120), 안전 요구는 별도 문서. |
| 2 | 기능 안전 요구사항이 별도 문서에서 관리되고 참조만 되어 있는가 | Fail | 안전 요구는 분리됨(safety 문서). 그러나 SRS 기능 행의 SSR 참조는 SRQ-FUN-015(:65, 깨진 SSR-030)와 SRQ-FUN-018(:73) 두 건뿐이다. 중재가 걸린 행(:49, :60, :62, :64, :72)에 참조가 없다. R-06 Open. |
| 3 | 비기능 요구사항이 품질/제한/제약으로 구분되어 있는가 | Pass | QUA(SRS:101~104), LIM(:110~111), CON(:117~120)으로 분리. 1요구1의무 문제는 항목 5에서 판정. |
| 4 | 품질 요구가 ISO/IEC 25010으로 분류되고 측정 가능한 기준이 있는가 | Fail | QUA-REL-001(:104)은 측정 가능. QUA-FUN-001(:103)은 측정 입력 "벡터 V"가 구체 값으로 없음(SRS:165). R-15 Open. |
| 5 | 각 요구사항이 단일하고 모호하지 않으며 검증 가능한가 | Fail | 1의무 위반 다수(R-11), 잔여 미정의 용어(R-12, N-12), 수용기준 격리 조건 누락(N-13), 카운터 오버플로 미정의(R-13). |
| 6 | 요구사항 간 충돌·중복이 없는가 | Fail | 이유코드 규칙과 우선순위 표 충돌(N-01), SSR-006 vs SSR-020~026(N-03), P2·P3와 P5 충돌(N-04), SSR-016 vs P2 범위(R-03). |
| 7 | 필수 속성(ID, 근거, 우선순위, 검증 방법, 상태)이 있는가 | Fail | SRS 모든 표에 필수 속성 있음(:44~120, 우선/상태 열 포함). 안전 표 6개에는 우선/상태 열이 없음(R-21, safety:15, :24, :45, :55, :66, :78). |
| 8 | 시스템 요구사항과 양방향 추적이 되며 고아 항목이 없는가 | Fail | 상위 열은 전 항목에 있음. 그러나 역링크 누락 다수(R-18), §3 주장 불일치(R-19), 조회 인터페이스 행 없음(R-08), source_timestamp_s INVALID 미처리(N-09), UC 역추적 없음(N-10). |
| 9 | 요구사항이 아키텍처/테스트와 양방향 추적 가능한 상태인가 | Pass | traceability §2 구조 존재, 하위 단계 "-" 표기는 SWE.1 단계에서 정상. ID 재부여(N-11)는 별도 결함. |
| R1 | (분류 규칙 2) 안전 요구사항 분리와 ID 참조 | Fail | R-06. |
| R2 | (분류 규칙 3) 품질 특성·제한·제약 분류 | Pass | QUA 특성 명시, CON-003/004 재분류 확인. |
| R3 | (작성 규칙) shall 1개 1의무 | Fail | R-11. |
| R4 | (작성 규칙) 모호 표현 금지, 수치·조건·단위 명시 | Fail | R-12, N-12, N-13. |
| R5 | (작성 규칙) ID 불변·비재사용 | Fail | N-11. v0.1 번호를 재부여했고 초안 예외 근거가 스킬에 없다. |
| R6 | (추적성) 추적 매트릭스와 고아·미추적 점검 | Fail | R-18, R-19, N-10. |
| R7 | (완료 조건) 일관성·모호성·검증 가능성 점검 완료 | Fail | 본 리뷰에서 N-01~N-09 확인. 작성자의 완료 주장과 불일치. |
| R8 | (추적) OEM 14개 요구의 누락·왜곡 | Fail | 14개 모두 매핑됨. 왜곡: OEM-NFR-002 "최근 제어결정 100건"을 변화 이벤트로 바꾼 해석이 기록되지 않음(N-08). OEM-FR-003은 OI-011로 기록됨. |
| R9 | (추적) IF-001~009 매핑 | Fail | IF-001 source_timestamp_s의 INVALID 처리 미정의(N-09). IF-005 와일드카드는 해소됨. |
| R10 | 임시 가정(open-issues) 타당성 | Fail | OI-006 P2 범위 확장(R-03), OI-008 INVALID 처리와 SSR-006 모순(N-03), OI-013 안전 상태 모순(N-04, R-16). |
| R11 | (OEM §8 G1 인도 증거) Use Case, 베이스라인 ID | Fail | Use Case 문서는 존재. 양방향 추적 없음(N-10). BL-SWR-1.0 미부여(R-26, OI-020). |
| R12 | (저작권/공개 증거) 원문 재배포 근거 | Fail | 서면 승인 파일 없음(R-17). 확인 불가 항목. |
| 공통 | 양방향 추적성이 상·하위 산출물과 일치하는가 | Fail | 항목 8, R6과 동일. |
| 공통 | 해당 스킬의 필수 규칙을 모두 만족하는가 | Fail | R1, R3, R4, R5, R6, R7. |
| 공통 | 이전 리뷰 결함이 모두 종결되었는가 | Fail | 11건 종결, 16건 Open. |

---

## 3. 추적성 실측 검증

### 3.1 OI 역링크 (OI 표 "영향 요구사항" ↔ 요구 행 근거 열)
| OI | open-issues 영향 열 | 요구 행에 OI 참조가 없는 항목 |
|---|---|---|
| OI-002 | SRQ-CON-001 | SRQ-CON-001 (SRS:117) |
| OI-003 | SRQ-LIM-002, SRQ-CON-002, SSR-001, SSR-009 | SRQ-CON-002 (:118), SSR-001 (safety:17), SSR-009 (safety:30) |
| OI-005 | SRQ-FUN-015, 022, 026, SSR-028 | SRQ-FUN-015 (:65) |
| OI-006 | SSR-027, SRQ-FUN-010, 012, 014 | SRQ-FUN-012 (:62) |
| OI-011 | SRQ-FUN-017~020, SSR-015 | SRQ-FUN-019 (:74) |
| OI-012 | SRQ-FUN-003, 004 | SRQ-FUN-003 (:48), SRQ-FUN-004 (:49) |
| OI-013 | SSR-009, 010 | SSR-009 (safety:30) |
| OI-015 | SSR-015, SRQ-FUN-017 | SRQ-FUN-017 (:72) |

### 3.2 ID 참조 무결성
- SRQ-FUN-015 (SRS:65)가 "상태 우선순위는 SSR-030"을 참조하나 SSR-030은 존재하지 않는다 (safety 문서는 SSR-001~028). **깨진 참조.**
- 기타 SSR/SRQ 참조는 존재하는 ID를 가리킨다.

### 3.3 traceability §3 주장과 SRS 상위 열
| 요구사항 | SRS 상위 열 (SRS/safety) | traceability §3 주장 |
|---|---|---|
| SRQ-FUN-005 | OEM-FR-001 (:50) | OEM 상위 없음 |
| SRQ-FUN-008 | OEM-FR-001 (:53) | OEM 상위 없음 |
| SRQ-FUN-021 | OEM-FR-001~007 (:81) | OEM 상위 없음 |
| SRQ-FUN-026 | OEM-SR-004 (:86) | OEM 상위 없음 |
| SSR-004 | OEM-SR-001 (safety:20) | OEM 상위 없음 |
| SSR-027 | OEM-SR-001~004, OEM-FR-001~007 (safety:80) | OEM 상위 없음 |
| SSR-028 | OEM-SR-004, OEM-FR-007 (safety:81) | OEM 상위 없음 |

### 3.4 UC 추적
- use-cases.md UC-01~10은 SRQ/SSR을 가리킨다. SRS와 safety 문서의 어느 행도 UC를 역참조하지 않고, traceability.md에도 UC 항목이 없다. 양방향이 아니다.

### 3.5 이벤트 기록 정책의 원문 대조
- OEM-NFR-002 원문: "최근 제어결정 100건을 메모리 내 저장소에 보존". SRQ-FUN-027(SRS:92)은 변화 또는 거절 평가에만 기록한다. 이 해석은 OI에 기록되지 않았다.

---

## 4. 신규 결함 (재리뷰 중 발견)

### N-01 (Major) 우선순위 표와 이유코드 요구 충돌: RC_UNLOCK_DENIED_MOVING 도달 불가, RC_COMMAND_REJECTED 우선순위 미정
- 위치: SRS:50 (SRQ-FUN-005), :53 (SRQ-FUN-008), 부록 A (:157~158, 규칙 :160), safety:94~95 (P8, P9)
- 설명: P8(속도 3.0 이상 또는 INVALID → LOCK)이 P9(운전자 명령)보다 상위다. 이동 중 unlock 요청이 있는 평가에서 성립하는 최고 규칙은 P8이며, 한 평가의 reason_code는 최고 규칙의 값(:160)이다. 따라서 기록되는 값은 RC_SPEED_AUTO_LOCK이다. RC_UNLOCK_DENIED_MOVING은 P9 행에 있으나, P9의 unlock은 정지일 때만 적용되므로 선택되지 않는다. SRQ-FUN-005의 수용기준(거부된 평가의 reason_code가 RC_UNLOCK_DENIED_MOVING)은 설계상 도달 불가다. RC_COMMAND_REJECTED는 우선순위가 "-"(:158)이라 거절 명령이 P1~P8 규칙과 같은 평가에서 성립하면 기록 코드가 정의되지 않으며, SRQ-FUN-008 수용기준과 충돌한다.
- 조치 요청: 이동 중 거부와 명령 거절의 기록 규칙을 우선순위 표 또는 이벤트 구조에서 정의한다(예: 명령 결과 코드와 상태 코드 분리 또는 RC_COMMAND_REJECTED 우선순위 명시). SRQ-FUN-005/008 수용기준을 도달 가능한 형태로 고친다.
- 상태: Open

### N-02 (Major) 존재하지 않는 요구사항 ID 참조
- 위치: SRS:65 (SRQ-FUN-015, 설명 및 수용기준의 "상태 우선순위는 SSR-030")
- 설명: safety 문서는 SSR-001~028만 정의한다. SSR-030은 없다. 상태 우선순위 규칙은 SSR-028에 있다.
- 조치 요청: SSR-028로 정정한다. 전 문서의 ID 참조를 존재 검사하는 절차를 추가한다.
- 상태: Open

### N-03 (Major) INVALID 처리 규칙 모순: "결정 근거로 쓰지 않음" vs "TRUE/이동 중으로 간주"
- 위치: safety:27 (SSR-006), :68~74 (SSR-020~025), :74 (SSR-026), :57 (SSR-016), :70 (SSR-022). OEM 원문 §5 OEM-SR-003("형식, 범위 오류는 평가 전에 거절된다").
- 설명: SSR-006은 INVALID 입력을 결정 근거로 쓰지 않는다고 정한다. 그러나 SSR-020~023은 같은 INVALID 입력을 TRUE 또는 이동 중으로 간주해 결정에 쓴다. SSR-016은 "정규화 입력(SSR-005를 통과한 입력)"으로 한정하는데, SSR-005를 통과하지 못한 INVALID sensor_fault를 SSR-022가 TRUE로 간주해 SSR-016~019를 적용하라고 한다. 원문 "거절"과 "대체값 사용"의 차이도 OI-008에 기록되지 않았다.
- 조치 요청: SSR-006을 "INVALID 입력은 SSR-020~025의 대체값 규칙으로만 결정에 쓴다"로 고치고, SSR-016의 "정규화 입력"을 대체 규칙 적용 후의 값으로 정의한다. 원문 "거절"과의 차이를 OI-008에 기록하고 OEM 확인 대상으로 둔다.
- 상태: Open

### N-04 (Major) 안전 상태 정의와 우선순위 표의 충돌: 접근위험 LOCK이 P2·P3 유지에 밀린다
- 위치: safety:11 (안전 상태 가정), :47 (SSR-011), :88 (P2), :89 (P3), :91 (P5), :97 (설명)
- 설명: 안전 상태 가정은 "접근위험은 LOCK"이라고 한다. OEM-SR-002는 접근위험 TRUE 시 LOCK을 무예외로 요구하며 ASIL B다. 그런데 P2(sensor_fault/INVALID 또는 PENDING) 및 P3(STALE, crash/ignition INVALID, PENDING 초과)은 직전 확정 출력을 유지하고 P5보다 상위다. 따라서 직전 출력이 RELEASE인 상태에서 접근위험이 TRUE가 되고 같은 평가에 sensor_fault 또는 STALE이 성립하면 RELEASE가 유지된다. 이는 SSR-011과 OEM-SR-002를 위반하며 안전 상태 정의와 모순된다. OI-013의 잔여 위험은 속도 stale만 다루고 이 경우를 다루지 않는다.
- 조치 요청: 접근위험 LOCK이 P2·P3 유지보다 우선하도록 우선순위를 바꾸거나, 예외와 안전 분석 근거를 안전 요구로 기록한다. 안전 상태 가정과 우선순위 표를 일치시키고 안전 담당 확인 대상으로 OI에 기록한다.
- 상태: Open

### N-05 (Major) freshness 트리거와 "필수 입력" 미정의
- 위치: safety:29 (SSR-008), SRS:27 (평가 정의), safety:30 (SSR-009), SRS:145 (RC_INPUT_STALE "필수 입력 STALE")
- 설명: SRS:27은 평가를 "스냅샷 N을 처리하는 1회 동기 호출"로 정의하고 스냅샷은 호출자의 timestamp_s를 가진다. 따라서 호출은 항상 새 timestamp를 갖는다. SSR-008의 "새 스냅샷이 없는 상태로 평가가 호출되면"은 호출 형식상 판정 조건이 없어 시험 입력을 만들 수 없다. 또한 "필수 입력"이 어떤 입력인지 목록이 없다(OEM-SR-003이 쓴 표현을 그대로 옮겼으나 정의되지 않음).
- 조치 요청: 신호 미갱신을 표현하는 입력 규칙(예: 신호별 마지막 갱신 시각 또는 빈 스냅샷)을 정의한다. 필수 입력 목록을 입력 유효 범위 표(safety:35~41)에 열로 추가한다.
- 상태: Open

### N-06 (Major) 상태 전이 규칙 불완전
- 위치: safety:31 (SSR-010, DEGRADED→NORMAL만 정의), :59 (SSR-018, FAULT 진입), :81 (SSR-028, 우선순위만), SRS:65 (SRQ-FUN-015, OFF 진입), SRS:86 (SRQ-FUN-026, 초기 OFF)
- 설명: 다음 전이가 정의되어 있지 않다. (1) OFF→NORMAL(ignition_on TRUE로 바뀔 때). (2) FAULT에서 sensor_fault FALSE로 복귀하는 조건. (3) NORMAL 진입의 일반 기준. state 4종 중 OFF, FAULT의 이탈 규칙이 없어 SRQ-FUN-022의 state 응답 시험 기대값을 정할 수 없다.
- 조치 요청: 상태 전이표(현재 상태 × 이벤트 → 다음 상태)를 SRS 부록 또는 SSR로 정의한다.
- 상태: Open

### N-07 (Major) override 상태의 수명 미정의, "같은 해제 명령" 정의가 OEM 수용기준과 다름
- 위치: SRS:31 (같은 해제 명령 정의), :72 (SRQ-FUN-017), :74 (SRQ-FUN-019), OEM 원문 §5 OEM-FR-003 수용기준("첫 억제 후 경과시간이 10초 이하인 같은 도어 재입력")
- 설명: (1) SRQ-FUN-019는 override 상태를 TRUE로 하지만 해제 조건이 없다(접근위험 FALSE 전환, 해제 명령 발생, 다음 평가 중 어느 것인지 불명). (2) 용어집은 "같은 해제 명령"을 source 동일까지 요구하는데, OEM 수용기준은 "같은 도어 재입력"이다. 이 차이는 OI-011에 기록되지 않았다.
- 조치 요청: override 상태의 생명주기를 정의한다. source 동일 조건을 유지한다면 OI-011에 OEM 확인 대상으로 기록하고, OEM 수용기준과의 차이를 명시한다.
- 상태: Open

### N-08 (Major) 이벤트 기록 정책이 OEM-NFR-002와 다르며 OI로 기록되지 않음
- 위치: SRS:92 (SRQ-FUN-027), :95 (SRQ-FUN-030), :110 (LIM-001). OEM 원문 §5 OEM-NFR-002("최근 제어결정 100건을 메모리 내 저장소에 보존").
- 설명: SRQ-FUN-027은 변화 또는 거절이 있는 평가에만 이벤트를 기록한다. 원문의 "제어결정"을 변화 이벤트로 좁힌 해석이다. 이 해석은 OI로 기록되지 않았다. OI-016은 스키마만 다룬다. "101건 입력" 수용기준의 실제 입력 수도 이 해석에 따라 달라진다.
- 조치 요청: 해석을 OI에 기록하고 OEM 확인 대상으로 둔다. 또는 매 평가 기록으로 정하고 SRQ-FUN-027, 030, LIM-001 수용기준을 재정의한다.
- 상태: Open

### N-09 (Major) source_timestamp_s INVALID 처리 미정의
- 위치: safety:26 (SSR-005 대상 포함), :28 (SSR-007이 source_timestamp_s로 freshness 계산), :68~74 (SSR-020~025), :74 (SSR-026), SRS:39 (입력 표)
- 설명: SSR-005는 source_timestamp_s의 INVALID를 정의하지만 SSR-020~026과 우선순위 표 어디에도 이 입력의 INVALID 결과가 없다. SSR-007의 freshness 계산도 INVALID일 때 STALE인지, INVALID인지, 무엇이 출력되는지 정의되지 않았다.
- 조치 요청: source_timestamp_s INVALID의 결과를 정의하고 OI-008에 추가한다.
- 상태: Open

### N-10 (Major) Use Case 양방향 추적 누락 (OEM §8 G1 증거)
- 위치: `docs/use-cases.md`:7~16 (UC-01~10), `docs/traceability.md` 전체, SRS와 safety 문서의 근거 열
- 설명: UC는 SRQ/SSR을 가리키지만 역방향이 없다. traceability.md에 UC 항목이 없다. OEM §8 G1은 "SW 요구사항, Use Case, 검토기록, 양방향 추적"을 인도 증거로 요구한다.
- 조치 요청: traceability.md에 UC ↔ SRQ/SSR 매트릭스를 추가하고, 요구 행의 근거 열 또는 §1에 UC ID를 기재한다.
- 상태: Open

### N-11 (Major) ID 번호 재부여: 불변 규칙 위반과 추적 단절
- 위치: SRS:14 (변경 이력: "번호를 재부여했다... ID 재사용 금지 규칙의 대상이 아니다"), `.claude/skills/requirements-analysis/SKILL.md:20` ("ID는 불변이며 재사용하지 않는다")
- 설명: 접두어 변경은 사용자 결정이지만, 번호까지 재부여했다. 스킬에는 초안 예외가 없다. v0.1 ↔ v0.2 대응표도 없어 1차 리뷰(requirements-review-001)의 ID 참조가 추적 불가능해졌다.
- 조치 요청: 번호를 v0.1에서 유지하거나 old→new 대응표를 변경 이력에 작성한다. 초안 예외가 필요하다면 사용자 승인을 받아 스킬에 명문화한다. 해당 결정은 decisions.md에 기록한다.
- 상태: Open

### N-12 (Minor) 용어·단위 불일치
- 위치: SRS:30 (정지/이동 정의), :49 (SRQ-FUN-004 "이동 중(3.0 이상 또는 INVALID)"), :47~48 ("정상 입력"), :72 ("10.0초" 및 "10.0 s"), safety:20 ("1.0초"와 "1.000 s"), OI-011 ("경고 후")
- 설명: 용어집의 "이동 = 3.0 km/h 이상"과 SRQ-FUN-004의 "INVALID 포함"이 다르다. 속도 INVALID는 정지도 이동도 아닌 상태가 된다. "정상 입력"과 "경고"는 정의가 없다. PENDING "지속"의 시작 시각이 없다. 단위는 "초"와 "s"를 같은 행에서 혼용한다.
- 조치 요청: 용어집을 고치고 누락 용어를 정의한다. PENDING 시작 시각을 정의한다. 단위 표기를 하나로 통일한다.
- 상태: Open

### N-13 (Minor) 수용기준의 격리·선행 조건 누락
- 위치: SRS:49 (SRQ-FUN-004), :63 (SRQ-FUN-013)
- 설명: SRQ-FUN-004는 "속도 3.0 km/h에서 unlock 명령 후에도 출력이 변하지 않는다"라고 하지만 명령 직전 출력이 정의되지 않았다. P8 자동 잠금 때문에 직전이 RELEASE였다면 출력이 바뀌므로 판정이 불가능하다. SRQ-FUN-013은 "left만 TRUE이면 lock_right가 직전 값과 같다"라고 하지만 다른 규칙(속도, 접근위험 등)이 동시에 성립하면 거짓 실패가 난다. 단독 주입 조건이 없다.
- 조치 요청: 수용기준에 선행 상태와 단독 주입 조건을 명시한다.
- 상태: Open

---

## 5. 결함 목록 (열린 결함 전체)

| ID | 심각도 | 위치 | 설명 | 조치 요청 | 상태 |
|---|---|---|---|---|---|
| R-03 | Major | SRS:88, :57, safety:88~89 | 충돌 우선순위 확정 전 범위 확장 (P2가 모든 규칙 덮음), OEM·안전 확인 필요 | P2 범위를 OEM 원문(새 명령 미적용)과 일치시키거나 OEM 확인 기록 | Open |
| R-06 | Major | SRS:49, :60, :62, :64, :72 | 기능 행의 SSR 참조 누락 (SSR-030 깨짐 포함) | 충돌·중재 행에 SSR ID 참조, :65의 SSR-030을 SSR-028로 정정 | Open |
| R-08 | Major | SRS §3 (:124~134), traceability §1 | 이벤트 조회 인터페이스 항목 없음 | §3 및 traceability에 조회 인터페이스 행 추가, OEM 범위 확인 기록 | Open |
| R-09 | Major | SRS:117, open-issues:8 | CON-001에 OI-002 역링크와 가정 표시 없음 | CON-001 근거에 OI-002 추가, 가정으로 표시 | Open |
| R-10 | Major | SRS:117, :83~85, open-issues:23 | Web 계층 구현 언어 미정 상태에서 C 기반 품질 도구 적용을 선언 | Web 언어·적용 측정 기준 정의, CON-001 범위 명시 | Open |
| R-11 | Major | SRS:52, :61, :74, :75, :86, :110, :111, :118~120; safety:19, :20, :27, :30, :31, :51, :72 | 1요구 1의무 위반 다수 | 의무 단위 분리 또는 분리 불가 근거 기록 | Open |
| R-12 | Major | SRS:145, :47~48; safety:20, :30; open-issues:22 | 미정의 용어 잔존 ("필수 입력", "정상 입력", "경고", PENDING 시작 시각) | 용어 정의 추가 | Open |
| R-13 | Major | SRS:93 | ID 카운터 오버플로 동작 미정의 | 오버플로 동작 정의 | Open |
| R-14 | Major | SRS:160, :83 | RET_ERR_SERIALIZE 등 오류코드 값이 설계로 이월되어 기대값 판정 불가 | 값 정의 또는 판정 불필요 근거 기록 | Open |
| R-15 | Major | SRS:103, :165 | 측정 입력 벡터 V가 구체 값으로 없음 | 벡터 V의 구체 값 또는 확정 파일 경로와 기준 해시 정의 | Open |
| R-16 | Major | safety:11, :88~89 | 안전 상태 가정 "접근위험은 LOCK"이 우선순위 표와 모순 (N-04와 연계), 안전 담당 확인 필요 | N-04 조치 후 안전 담당 확인 기록 | Open |
| R-17 | Major | decisions:7, docs/approvals/ (없음) | 서면 공개 승인 증거 미제출 | 사용자가 서면 승인 파일을 제출하고 DEC-002에 경로 기재 (에이전트 해소 불가) | Open |
| R-18 | Major | SRS:62, :65, :74, :72, :117, :118, :48~49; safety:17, :30 | OI 역링크 누락 8건 | 3.1 표의 누락 항목에 OI ID 추가 | Open |
| R-19 | Major | traceability:49, SRS:50, :53, :81, :86, safety:20, :80, :81 | traceability §3의 "OEM 상위 없음" 주장이 SRS 상위 열과 모순 | §3을 SRS 상위 열 기준으로 재작성 | Open |
| R-21 | Minor | safety:15, :24, :45, :55, :66, :78, safety:9 | 안전 표에 우선/상태 열 없음, 존재하지 않는 열을 참조하는 문장 | 열 추가 또는 표준 문구 기재, safety:9 정정 | Open |
| R-26 | Minor | SRS:6, open-issues:27 | BL-SWR-1.0 미부여 (승인 시 부여) | 승인 절차 필요 (에이전트 해소 불가) | Open |
| N-01 | Major | SRS:50, :53, :157~160; safety:94~95 | 이유코드 RC_UNLOCK_DENIED_MOVING 도달 불가, RC_COMMAND_REJECTED 우선순위 미정 | 4절 N-01 조치 참조 | Open |
| N-02 | Major | SRS:65 | 존재하지 않는 SSR-030 참조 | SSR-028로 정정 | Open |
| N-03 | Major | safety:27, :57, :70, :68~74 | INVALID 처리 규칙 모순 (SSR-006 vs SSR-020~026) | 4절 N-03 조치 참조 | Open |
| N-04 | Major | safety:11, :47, :88~89, :91 | 접근위험 LOCK이 P2·P3 유지에 밀려 ASIL B 요구 위반 가능, 안전 상태 정의와 모순 | 우선순위 재설계 또는 안전 분석 근거와 안전 담당 확인 | Open |
| N-05 | Major | safety:29~30, SRS:27, :145 | 신호 미갱신 트리거와 "필수 입력" 미정의 | 입력 규칙과 필수 입력 목록 정의 | Open |
| N-06 | Major | safety:31, :59, :81; SRS:65, :86 | 상태 전이 (OFF→NORMAL, FAULT 이탈, NORMAL 진입) 미정의 | 상태 전이표 정의 | Open |
| N-07 | Major | SRS:31, :72, :74; OEM FR-003 | override 상태 수명 미정의, "같은 해제 명령" 정의가 OEM 수용기준과 다르고 OI 미기록 | 수명 정의, OI-011에 차이 기록 | Open |
| N-08 | Major | SRS:92, :95, :110; OEM NFR-002 | 변화 시에만 기록하는 해석이 OEM 원문과 다르고 OI 미기록 | OI 기록 및 OEM 확인, 또는 정책 재정의 | Open |
| N-09 | Major | safety:26, :28, :68~74; SRS:39 | source_timestamp_s INVALID 처리 미정의 | INVALID 결과 정의, OI-008에 추가 | Open |
| N-10 | Major | use-cases:7~16, traceability 전체 | Use Case 양방향 추적 없음 (G1 증거) | UC ↔ 요구 매트릭스 추가 | Open |
| N-11 | Major | SRS:14, requirements-analysis SKILL:20 | ID 번호 재부여, 불변 규칙 위반, 대응표 없음 | 번호 유지 또는 대응표 작성, 예외는 사용자 승인 후 명문화 | Open |
| N-12 | Minor | SRS:30, :47~49, :72; safety:20; OI-011 | 용어·단위 불일치 (이동 정의, 정상 입력, 경고, 초/s/ms 혼용) | 용어집 정정 및 단위 통일 | Open |
| N-13 | Minor | SRS:49, :63 | 수용기준의 선행 상태·단독 주입 조건 누락 | 수용기준에 조건 명시 | Open |

---

## 6. 확인 불가 항목 (에이전트 해소 불가, Open 유지)

아래 항목은 OEM, 안전 담당, 사용자의 확인 또는 서면 증거가 필요하다. 수정본에 가정이 기록되어 있어도 확인 전까지 Open이다.

| 항목 | 사유 | 관련 OI/결함 |
|---|---|---|
| OEM 편차: 구현 언어 C 전환 | OEM §3은 Python 3.12다. 사용자 결정(CR-001)만 있고 OEM 확인이 없다. | OI-002, R-09 |
| 우선순위 표 P1~P9 및 QM 입력의 ASIL B 상위 배치 | OEM 및 안전 담당 확인 필요 | OI-006, R-03 |
| INVALID 입력별 안전 측 처리 | 안전 담당 확인 필요 | OI-008, N-03, N-09 |
| DEGRADED 출력, 안전 상태, FTTI | 안전 담당 확인 필요 | OI-013, R-16, N-04 |
| override 기준 시각 (경고 후 → 첫 억제) | OEM 확인 필요 | OI-011, N-07 |
| 일반 해제 허용조건 (정지 3.0 km/h 미만) | OEM 확인 필요 | OI-010 |
| 고정 신호의 freshness 한계 | OEM 신호별 timestamp 또는 liveness 신호 필요 | OI-004 |
| 이벤트 기록 해석 (변화 시 기록) | OEM 확인 필요 | N-08 |
| 서면 공개 승인 | 사용자 제출 필요 | DEC-002, R-17 |
| BL-SWR-1.0 베이스라인 부여 | 승인 절차 필요 | OI-020, R-26 |
| 차종·법규 적용성 | 차종 확정 전 확정 불가 | OI-019 |

---

## 7. 요구사항 리뷰 결론
- 최종 판정: **Rejected**
- 열린 결함: **29건** (Major 25, Minor 4), 종결 11건 (1차 결함 R-01, R-02, R-04, R-05, R-07, R-20, R-22, R-23, R-24, R-25, R-27)
- 다음 단계 진행: **불가** (열린 결함 29건)
- 재리뷰 조건: 위 Open 결함 전부를 수정 내용으로 확인할 수 있어야 한다. 특히 N-01(이유코드 도달 불가), N-03·N-04(안전 요구 모순), N-05·N-06(freshness와 상태 전이), N-10·N-11(추적성)은 에이전트가 수정할 수 있는 항목이므로 우선 조치한다. OEM·안전 담당 확인과 서면 승인 항목은 Open으로 남으며, 확인 결과가 나오기 전에는 Approved가 될 수 없다.

## 변경 이력
- 2026-11-27 최초 작성 (requirements-review-002, 재리뷰, requirements-review-001 R-01~R-27 closure 확인 및 신규 N-01~N-13 발견)
