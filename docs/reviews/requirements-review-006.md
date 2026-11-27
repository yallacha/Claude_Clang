# 요구사항 리뷰 기록 requirements-review-006

## 리뷰 기록
- 대상: SWE.1 소프트웨어 요구사항 명세 6차 재리뷰 (`docs/SRS.md` v0.4, `docs/safety-requirements.md` v0.4, `docs/open-issues.md`, `docs/traceability.md`, `docs/decisions.md`(DEC-003~010), `docs/use-cases.md`, `docs/test-vectors/vector-v.csv`) / 버전: SRS-ECL-001 0.4, SSR-ECL-001 0.4, 나머지 무버전 (전부 Draft) / 체크리스트: `.claude/skills/review/references/checklists.md` 1장 SWE.1 + `requirements-analysis` 스킬 분류·작성·추적 규칙 + `review` 스킬 "외부 확인 대기 항목(DEC-003)" (프로젝트 전용 체크리스트 없음)
- 선행 기록: `docs/reviews/requirements-review-005.md`의 열린 결함 18건을 수정본에서 열어 closure를 판정했다.
- 구현 대조(읽기 전용): `src/ecl_val.c`, `src/ecl_arb.c`, `src/ecl_ovr.c`, `src/ecl_sta.c`, `src/ecl_core.c`, `src/ecl_log.c`, `include/ecl_types.h`, `include/ecl_log.h`. 코드는 수정하지 않았다.
- 상위 입력: OEM-SWR-001 Rev 1.0 원문 `C:\Users\user\AppData\Local\Temp\swr.txt`를 `grep`/`sed`로 읽어 수용기준·§7·§8을 대조했다(`Read` 도구로는 열리지 않음).
- 결과: **Rejected**
- 다음 단계 진행: **불가** (열린 결함 13건: Major 4, Minor 9)

### 판정 원칙 적용 메모
- 005 열린 결함 18건 중 **14건 Closed**, **4건 Open**(R-10 재개, N-12, N5-03, N5-04). N5-04는 심각도를 Major에서 Minor로 낮췄다(사유는 §1).
- 신규 결함은 N6-01~N6-09로 기록했다. 구현 불일치(N6-02)는 요구 문언이 아니라 코드 쪽 결함이며 코드 수정은 작성자 몫이다.
- 작성자 주장("그대로", "ASIL B 우선 삭제", 변경 이력 문구, "비-C 소스가 없다")은 증거로 쓰지 않았다. 원문 대조, 스크립트 결과, 파일 목록, 코드 읽기로만 판정했다.
- 005에서 R-10을 Major에서 Minor로 낮춘 근거는 "비-C 소스가 없다"는 전제였다. 저장소에 Python 도구가 있어 전제가 거짓이므로 R-10은 재개(Major)했다.

---

## 0. 검증 실행 기록 (스크립트 직접 실행, 보고서 주장 미사용)

임시 Python 스크립트(`python -I`)를 저장소 밖 스크래치 위치(`C:\Users\user\AppData\Local\Temp\claude\0e89c31cbdebc8c0\90b67dd4-e59d-4b12-a107-3b5bbb92bbde\scratchpad\check6b.py`)에 두고 실행했다. 출력은 `check6b_out.txt`(UTF-8)에 저장해 읽었다. 1차 실행은 헤더 없는 표 행을 누락했고(N6-05 참조), 파싱 규칙을 고쳐 2차 실행했다. 아래 표는 2차 결과다.

| 검사 | 결과 |
|---|---|
| 정의 ID | SRQ-FUN 39, SRQ-QUA 2, SRQ-LIM 4, SRQ-CON 9, SSR 37 (요구 91건). OI 22, DEC 10, UC 10. 중복 정의 0 |
| 참조 무결성 (범위 `~`, 쉼표 나열 확장 후, 정의 행의 ID 칸 제외) | 정의되지 않은 SRQ/SSR/OI/DEC/UC 참조 0건 |
| 요구 ID 고아 (다른 문서에서 참조 0건) | 0건 |
| OI 고아 (open-issues 밖 참조 0건) | OI-001(문서 지위), OI-020(프로세스 증거). 둘 다 "프로세스·문서" 항목이라 사유 있음 |
| OI 역링크 (근거 열 ↔ 영향 요구사항 열, 22건) | 전부 일치. 차이 0 (R-18 포함) |
| traceability §1 ↔ 요구 상위(OEM) 열 (OEM-SR/FR/NFR 13키) | 전부 일치. 차이 0 (N4-05 종결 유지) |
| traceability §2 커버리지 | 요구 91건 전부 행 존재 |
| traceability §3 UC 커버리지 | UC 미포함 요구 = SRQ-FUN-021, 026, SSR-028, SRQ-LIM-002/004, SRQ-QUA-*, SRQ-CON-*. 문서에 적힌 예외 목록과 일치 |
| traceability 테스트 함수명 | 참조 75건 모두 `tests/` 본문에 존재(문자열 검색) |
| `ms` 잔존 | `docs/SRS.md:21` 환산 표기만. 요구 본문·안전 문서·UC는 0건 |
| 구 수치 (1.001, 0.201, 1.01) | 0건 |
| "ASIL B 우선", "그대로" | 0건 |
| 비-C 소스 파일 (repo 전체, OEM 제외) | `tools/gen_traceability.py`, `tools/gen_system_tests.py`, `ci/check_coverage.py`, `ci/check_system_coverage.py`, `.claude/skills/sw-system-test-cases/scripts/build_workbook.py`, `.claude/skills/sw-system-test-cases/scripts/run_pict.py`, `tools/hitl.sh`. `.js`/`.html` 없음 |
| 벡터 `vector-v.csv` | 23행, t는 0.010 s 등간격(전 행), src_ts > t 0건, STALE 판정 행은 0.210 한 건, 0.200 행은 fresh(차 0.200). 기대값 열 없음 |

**부동소수점 경계 실험** (IEEE-754 double, 십진 리터럴로 만든 시각 쌍, 코드와 같은 한계값 `L + 1e-9` 사용, 4,000개 시작 시각)

| 한계 L | 시각 크기 | 잘못 초과 판정 |
|---|---|---|
| 0.200, 1.0, 10.0 | 1 ~ 8e6 s | 0건 |
| 0.200 | 1e7 s | 1,600 / 4,000 (40%) |
| 0.200 | 1e8 s | 3,200 / 4,000 (80%) |
| 1.0, 10.0 | 1e7, 1e8 s | 이 표본에서는 0건 (부호 의존) |

005의 원래 실험(작은 시각에서 106/300 등)은 허용오차 1e-9 s 규칙으로 해소되었다. 남은 문제는 `timestamp_s`의 상한이 없다는 점이다(§1, N5-04).

---

## 1. 004/005 열린 결함 18건 closure 판정

| ID | 원 심각도 | 판정 | 근거 (수정본 위치) | 잔여 사항 |
|---|---|---|---|---|
| R-03 | Major | **Closed** | SSR-017(`safety-requirements.md:65`)에서 "그대로"가 사라졌고, OEM 수용기준과 본문의 차이(접근위험 도어 예외)를 명시했다. OI-006 가정 ④(`open-issues.md:12`)에 "P5 출력 > P2~P4 유지"가 있다. | 없음 |
| R-10 | Major → Minor(005) → **Major(재개)** | **Open** | 005의 해소 근거는 "비-C 소스가 없다"(OI-017 `open-issues.md:23`, DEC-010 `decisions.md:43`)였다. 실제 저장소에 Python 파일 7개가 있다(§0 표). 따라서 SRQ-CON-009 INS(1) "비-C 소스 파일이 저장소에 없고"는 현재 저장소에서 불합격이다. CLAUDE.md §2의 "예외는 없다"(`CLAUDE.md:42`)와의 충돌이 되살아난다. INS 판정 기준도 기계 검사가 아니다: "`src=`가 스크립트를 가리키는 경우", "`on<이벤트>=`"는 대소문자 무시 문자열 검색으로 판정할 수 없다(`SRS.md:143`). | 적용 범위(저장소 전체인지, 표시 계층 소스인지)를 정하고 도구 스크립트 처리 방식을 정한다. OI-017과 DEC-010 근거를 고친다. INS 패턴을 고정 문자열 목록으로 적는다. |
| R-11 | Major | **Closed** | SSR-010(`safety-requirements.md:35`)과 SSR-017(`:65`) 본문이 접근위험 도어를 제외하고 SSR-011을 참조한다. 두 수용기준도 같은 범위다. | 새 불일치 N6-03(OI-013)는 별도 기록 |
| R-16 | Major | **Closed** | OI-006 행 상태 칸에 DEC-003 참조가 기록되었다(`open-issues.md:12`). 005 요청의 "(또는 DEC 참조)"를 충족한다. | OI-006은 N5-03 때문에 DEC-003 판정 Open(§2) |
| N-12 | Minor | **Open** | SRQ-LIM-002(`SRS.md:132`)의 주체는 "SW는 ... 판정해야 한다"로 SW다. 그러나 수용기준 (2)는 "SW 시험이 쓰는 벡터 V의 평가 호출 간격이 0.010 s"이며 시험 하네스 전제이고 이 문장이 "SW의 합격 기준은 (1)이다"라고 스스로 밝힌다. 한 요구 안에 SW 행위와 호출자 계약 검사가 섞여 있다. | 호출자 계약은 OI-003 가정 또는 별도 SRQ-CON으로 옮기고, SRQ-LIM-002 수용기준은 (1)로만 둔다. |
| N-13 | Minor | **Closed** | SRQ-FUN-009(`SRS.md:72`) 수용기준에 "정상 입력(속도 제외)" 전제가 있고, 정상 입력이 아닌 경우는 SSR-027을 따른다고 적었다. | 없음 |
| N3-01 | Major | **Closed** | OI-018(`open-issues.md:24`)에 "SWR-019가 안전 요구였다면"의 영향과 확인 전 가정이 있다. OI-019(`:25`)의 영향 열에 SSR-001, SSR-002가 있고 본문 근거가 OEM §7 별표 14 후보 파생과 일치한다(원문 `swr.txt` §7 확인). | 없음 |
| N3-02 | Major | **Closed** | OI-011 안전 영향(`open-issues.md:17`)에서 "(ASIL B 우선)"이 삭제되었고, 근거는 SRQ-FUN-018, SSR-011로 바뀌었다. 전체 문서에서 "ASIL B 우선" 0건(§0). | 없음 |
| N4-02 | Major | **Closed** | (a) OI-008 안전 영향(`open-issues.md:14`)은 sensor_fault INVALID→TRUE를 "직전 출력 유지"로 적었다(SSR-017과 일치). (b) OI-013 한계(`:19`)에 sensor_fault, PENDING, crash/ignition INVALID 유지의 이동 중 잔여 위험이 추가되었다. | OI-013 가정 칸의 새 불일치는 N6-03 |
| N4-03 | Major → Minor(005) | **Closed** | OI-003 영향 열(`open-issues.md:9`)에 SSR-004, SSR-008, SSR-031이 있다(§0 역링크 일치). | 경계 표현 잔여는 N5-04 |
| N4-06 | Minor | **Closed** | (a) INVALID 우선이 SRS §0(`SRS.md:33`)에 있다. (b) SSR-006(`safety-requirements.md:29`)은 source_timestamp_s의 보고값을 INVALID로 고정하고 SSR-029(`:37`)는 다른 신호에만 STALE을 적용한다. (c) "입력별" 대상은 SRS §0의 필수 입력 13개(`SRS.md:37`)다. | 용어 정의 불일치는 N6-04(신규) |
| N5-01 | Major | **Closed** | SSR-027 수용기준(`safety-requirements.md:88`), 표 설명 단락(`:94`), 표 P5 행(`:102`), SSR-011(`:53`), 부록 A(2)(`SRS.md:168`), OI-006 ④(`open-issues.md:12`)가 같은 규칙을 말한다. 접근위험 도어의 출력은 LOCK이고 이유코드는 P2~P4 코드다. | 없음 (P4 타임아웃 문제는 N5-03) |
| N5-02 | Major | **Closed** | SRQ-FUN-027(`SRS.md:110`)에 포화 예외가 명시되었고 SRQ-FUN-037(`:112`)과 충돌하지 않는다. 누락 표시 수단을 두지 않은 결정은 OI-022(`open-issues.md:28`)로 기록했다. | 승인 기록 부재는 N6-06 |
| N5-03 | Major | **Open** | 타임아웃 평가의 출력 규칙이 세 곳에서 다르다. (i) SSR-027 예외 (2)(`safety-requirements.md:88`)와 부록 A(2)(`SRS.md:168`)는 "P1·P2·P4가 성립하지 않을 때"라는 조건이 부록에만 있고 SSR-027 본문은 무조건 "출력은 P5~P10"이다. (ii) 표 P4 행(`:101`)은 STALE/INVALID "유지"와 함께 `RC_CRASH_PENDING_TIMEOUT`을 적는다. (iii) SSR-004(`:21`)는 "P5~P10 중재"를 무조건 요구한다. 따라서 STALE이 겹친 타임아웃 평가는 P4 "유지"인지 P5~P10인지 문서로 정해지지 않는다. 코드는 P2/P4 유지를 먼저 적용하고 타임아웃 코드는 유지가 없을 때만 적용한다(`ecl_arb.c:233-237`, `:211-213`, `ecl_val.c:335-352`). 코드는 부록 A(2)의 조건부 문언과 같다. | 표 P4 행에서 타임아웃 코드를 빼고, SSR-027 예외 (2)와 SSR-004에 "P1·P2·P4 미성립 시" 조건을 쓴다. |
| N5-04 | Major → **Minor** (심각도 조정) | **Open** | 허용오차 1e-9 s 규칙(`SRS.md:23`)은 코드와 같다(`ecl_types.h:43`, `ecl_val.c:170`, `:311`, `ecl_ovr.c:57`). 0.200/1.0/10.0 경계는 |t| ≤ 8e6 s에서 판정이 맞다(§0 실험). 그러나 `timestamp_s`의 상한이 어디에도 없다. 2^23 s(약 8.4e6 s) 이상에서 double 간격이 1e-9 s를 넘어 0.200 경계가 40% 잘못 판정된다(1e7 s). 요구 범위 밖 주장("1e-9 s 이하의 차이를 구분하는 입력은 요구 범위 밖")은 시각 상한이 없으므로 성립하지 않는다. | `timestamp_s`와 `source_timestamp_s`의 최댓값(예: 2^23 s 미만)을 SRS §0 또는 시각 비교 규칙에 쓰거나, 정수 틱으로 정의한다. |
| N5-05 | Minor | **Closed** | SRQ-FUN-039(`SRS.md:113`)에 시험 빌드 정의(`ECL_TEST_BUILD`), 설정 범위(0~2^64−1), NULL 인자(`RET_ERR_NULL`), 제품 빌드 INS가 있다. 코드와 일치한다(`ecl_log.c:98-114`, `ecl_log.h:67-79`, `ecl_log.c:105` 값 2^64−1의 처리). | 없음 |
| N5-06 | Minor | **Closed** | traceability §2에 SRQ-FUN-039 행이 있고(`traceability.md:79`), §3 UC-10 행이 SRQ-FUN-039를 포함한다(`:151`). 스크립트 §0에서 누락 0. | 없음 |
| N5-07 | Minor | **Closed** | B-2(`SRS.md:197`)가 경계 벡터의 위치(1.000/1.010 PENDING, 10.0/10.01 override는 UT 전용)와 판정 방식(해시 동일성 외 기대값은 UT/ST)을 적었다. CSV에 1.000/1.010 PENDING 경계가 없다는 사실과 일치한다. | 없음 |

**집계**: 18건 중 Closed 14건(R-03, R-11, R-16, N-13, N3-01, N3-02, N4-02, N4-03, N4-06, N5-01, N5-02, N5-05, N5-06, N5-07), Open 4건(R-10, N-12, N5-03, N5-04). 심각도 조정: R-10(Minor→Major 재개), N5-04(Major→Minor).

---

## 2. 외부 확인 대기 항목 DEC-003 판정 (review 스킬 "외부 확인 대기 항목")

조건: ① 가정, 한계, 안전 영향, 확인 주체가 open-issues에 기록. ② 사용자 승인이 decisions.md에 기록. ③ 가정이 요구 본문과 모순되지 않고 근거 열에서 역참조됨. 추가로, DEC-003은 **외부 확인을 받을 수 없는 항목**에만 적용된다. 내부 설계 선택은 DEC-003 대상이 아니다.

| OI | 쟁점 | ① | ② | ③ | 판정 (006) |
|---|---|---|---|---|---|
| OI-001 | 문서 지위 | ✔ | DEC-001 사용자 확인 | ✔ | **Closed** (사용자 확인) |
| OI-002 | 구현 언어 C (OEM 편차) | ✔ | CR-001, DEC-003 | ✔ SRQ-CON-001 | **Closed (사용자 승인 가정)** |
| OI-003 | 평가주기 0.010 s, 호출 간격 계약 | ✔ | DEC-003 | ✔ 영향 열 완비 (N4-03 해소). 경계 표현 미확정 | **Open** (N5-04) |
| OI-004 | freshness 기준 | ✔ | DEC-003 | ✔ SSR-007, 008 본문과 일치. 구현은 N6-02 | **Closed (사용자 승인 가정)** |
| OI-005 | 상태 4종, override 표현 | ✔ | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-006 | 동시 조건 우선순위 | ✔ 가정 ①~⑤, 안전 영향 | DEC-003 | ✗ SSR-027 예외 (2)와 P4 행, SSR-004가 서로 다르다 (N5-03). 가정 ③ 역참조 본문이 이 모순을 포함 | **Open** (N5-03) |
| OI-007 | PENDING 유지, 1.0 s 후 일반 중재 | ✔ | DEC-003 | ✗ 가정 "1.0 s 후 일반 중재 복귀"가 STALE 겹친 타임아웃의 P4 행과 모순 (N5-03). PENDING 중단 규칙 미정 (N6-08) | **Open** (N5-03, N6-08) |
| OI-008 | INVALID 대체 값 | ✔ | DEC-003 | ✔ SSR-020~026, 029, 032, 037 | **Closed (사용자 승인 가정)** |
| OI-009 | 해시 대상·직렬화 | ✔ | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-010 | 일반 해제 허용 (정지 3.0 km/h 미만) | ✔ | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-011 | override 기준·경계·같은 source | ✔ | DEC-003 | ✗ 유지 구간(P2~P4, crash hold)의 해제 요청을 억제로 볼지 미정. 코드는 기록하지 않음 (N6-07). "ASIL B 우선"은 삭제됨 | **Open** (N6-07) |
| OI-012 | gear 사용처 | ✔ | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-013 | DEGRADED 출력, 안전 상태, FTTI | ✔ 한계 확장 완료 | DEC-003 | ✗ 가정 "DEGRADED는 직전 출력 유지"가 SSR-010 축소·SSR-026과 모순 (N6-03). 안전 상태 "CONFIRMED는 RELEASE"가 STALE 전파로 깨지는 잔여 위험이 기록되지 않음 (N6-01) | **Open** (N6-01, N6-03) |
| OI-014 | source enum | ✔ | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-015 | SWR-006과 FUN-004 분리 | ✔ | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-016 | 이벤트 이유코드·스키마 | ✔ | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-017 | Web 구현 방식 | ✔ | DEC-003, DEC-007 | ✗ 가정의 전제 "비-C 소스가 없다"가 거짓 (R-10) | **Open** (R-10) |
| OI-018 | OEM 추적성 SWR-019 결번 | ✔ 조건부 영향 기록 | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-019 | 법규 후보 적용성 | ✔ | DEC-003 | ✔ OEM §7 일치, SSR-001/002 역참조 | **Closed (사용자 승인 가정)** |
| OI-020 | G1 증거, BL-SWR-1.0, 원문 비공개 | ✔ | DEC-005, DEC-006 | 결정 기록으로 대체 | **Closed (결정 기록 대체)** |
| OI-021 | 이벤트 해석과 조회 인터페이스 | ✔ | DEC-003 | ✔ | **Closed (사용자 승인 가정)** |
| OI-022 | 포화 후 기록 중단, 누락 표시 없음 | ✔ 영향·한계 기록 | **없음** (DEC-003 일반 규칙 외 항목별 승인 기록 없음) | ✔ | **Open** (N6-06). 내부 설계 선택이며 확인 주체를 OEM으로 적은 것도 맞지 않음 |

**결론**: Closed 15건(OI-001, 002, 004, 005, 008, 009, 010, 012, 014, 015, 016, 018, 019, 020, 021). Open 7건(OI-003, 006, 007, 011, 013, 017, 022).

**승인 기록 메모(②)**: DEC-003 승인("1번 동의")은 일반 규칙에 대한 승인이다. 승인 대상 OI 목록이 기록에 없다. 005에서 이 해석을 허용했다. 006에서도 허용하되, OI-022는 DEC-003 이후 추가된 항목이고 외부 확인 대기가 아니므로 이 허용 범위 밖으로 판정했다.

---

## 3. 신규 결함 (이번 재리뷰에서 발견)

### 3.1 N6-01 (Major): crash_status의 freshness가 CONFIRMED를 무력화한다 (안전 상태와 충돌)
- 위치: `docs/safety-requirements.md:11`(안전 상태 가정: "충돌 CONFIRMED는 RELEASE"), `:17`(SSR-001: "유효한 crash_status가 CONFIRMED이면"), `:37`(SSR-029), `docs/safety-requirements.md:31`(SSR-007 "입력을 STALE로 판정"), `docs/SRS.md:33`(§0 STALE = "freshness 기준 초과", 유효 = "fresh"). 구현: `src/ecl_val.c:167-181`(judge_stale), `:193-204`(mark_stale), `:371-375`(build_inputs), `:252-253`(crash_confirmed는 VALID일 때만), `src/ecl_arb.c:297-300`.
- 설명: crash_status에는 자체 timestamp가 없다. 코드는 속도·기어의 `source_timestamp_s` 기준 시간 초과를 **VALID였던 모든 신호**에 적용하므로 crash_status도 STALE이 된다. 그러면 P1이 성립하지 않고 P4 유지가 적용된다. 예: 평가 시각 1.000 s, source 0.700 s, crash CONFIRMED이면 출력은 직전 값(LOCK 가능)이고 OEM-SR-001("300 ms 이내 RELEASE")과 안전 상태 가정에 반한다. SRS와 SSR 문서 어디에도 "crash_status의 freshness 판정 근거"가 없고, OI-013 잔여 위험에도 이 경우가 없다. SSR-029는 source_timestamp_s가 INVALID인 경우만 명시한다.
- 조치 요청: (a) crash_status의 freshness 판정 근거를 정의하거나, (b) CONFIRMED는 freshness와 무관하게 P1을 적용한다고 쓴다. 어느 쪽이든 결과를 SSR-001, SSR-029, OI-013 잔여 위험에 기록하고 안전 상태 가정과 맞춘다. 코드 정정은 작성자 몫이다.

### 3.2 N6-02 (Major, 구현 불일치): 틱 경로의 STALE 기준이 SSR-008과 다르다
- 위치: `src/ecl_val.c:414-424`(틱: 마지막 스냅샷을 복사하고 timestamp만 `now`로 바꿈), `:172-174`(judge_stale: `now − 저장된 source_timestamp_s`). 주석 `:156-158`의 주장("source_timestamp_s ≤ 수신 시각이므로 경과 기준이 더 늦게 성립하는 일이 없다")은 반대다. source가 수신 시각보다 이르면 source 기준이 **먼저** 성립한다. 요구: `docs/safety-requirements.md:32`(SSR-008: `timestamp_s − 마지막으로 스냅샷을 수신한 평가의 timestamp_s`가 0.200 s 초과), `:31`(SSR-007은 "평가 호출의 스냅샷"에만 적용).
- 예: 스냅샷 수신 평가 timestamp 1.000 s, source 0.990 s. 틱 1.200 s에서 SSR-008은 0.200 s(fresh, 경계 포함)인데 코드는 0.210 s로 STALE이 된다.
- 조치 요청: 틱 경로의 판정을 SSR-008의 "마지막 스냅샷 수신 평가의 timestamp_s" 기준으로 정정하고, 주석 `:156-158`을 고친다. 경계 시험(0.200 fresh, 0.210 STALE)을 source 지연이 있는 입력으로 추가한다.

### 3.3 N6-03 (Minor): OI-013 가정이 SSR-010 축소·SSR-026과 모순된다
- 위치: `docs/open-issues.md:19`(OI-013 가정: "DEGRADED는 직전 확정 출력 유지·복구 시 NORMAL"), `docs/safety-requirements.md:35`(SSR-010: STALE로 DEGRADED인 동안만 유지, 괄호 안에서 "그 밖의 INVALID 대체 입력은 DEGRADED여도 유지 없이 SSR-027 중재"), `:82`(SSR-026: INVALID 대체로 DEGRADED). 구현: `src/ecl_val.c:335-352`(derive_hold: 유지 조건에 INVALID 대체 없음), `src/ecl_sta.c:23-24`(대체가 있으면 DEGRADED).
- 조치 요청: OI-013 가정을 "STALE로 인한 DEGRADED만 직전 출력을 유지하고, INVALID 대체로 인한 DEGRADED는 SSR-027 중재를 따른다"로 고친다.

### 3.4 N6-04 (Minor): STALE 정의가 SSR-029·구현과 다르다
- 위치: `docs/SRS.md:33`(§0: "STALE = freshness 기준 초과(SSR-007, 008)"), `docs/safety-requirements.md:37`(SSR-029: freshness 판정 불가 → 다른 유효 신호를 STALE로 보고). 구현: `src/ecl_val.c:193-204`(시간 초과 시 freshness와 무관한 VALID 신호도 모두 STALE).
- 조치 요청: §0 정의를 "freshness 기준 초과, 또는 SSR-029가 정하는 freshness 판정 불가 상태"로 넓혀 세 가지 원인(시간 초과, source INVALID, 그 밖)을 구분한다.

### 3.5 N6-05 (Minor, 문서 형식): 헤더 없는 표 조각으로 요구 6건과 DEC 3건이 표로 렌더링되지 않는다
- 위치: `docs/SRS.md:143`(SRQ-CON-009 검사 절차 문단) 뒤 `:144~150`(SRQ-CON-002, 005, 003, 006, 007, 004 행이 헤더·구분선 없이 놓임. GFM 표가 아니다). `docs/decisions.md:40~43`(빈 줄 뒤 DEC-008, 009, 010 행이 헤더 없이 놓임). 1차 스크립트가 이 행들을 누락해 "정의되지 않은 참조"로 잘못 보고했다.
- 설명: 요구 6건은 "검증", "상위(OEM)" 열의 의미를 표 헤더로 알 수 없다(필수 속성 체크 불가). DEC-010은 SRQ-CON-009가 참조하는데 표에서 읽히지 않는다.
- 조치 요청: SRQ-CON-002~007을 2.3 표 헤더 아래로 옮기고, SRQ-CON-009 검사 절차 문단은 표 뒤에 둔다. DEC-008~010은 DEC 표 안으로 옮긴다.

### 3.6 N6-06 (Minor): OI-022가 승인 기록 없이 DEC-003 가정으로 종결되었다
- 위치: `docs/open-issues.md:28`(OI-022, 상태 "사용자 승인 가정(DEC-003)", 확인 주체 OEM). `docs/decisions.md` DEC-003(`:8`)의 승인 범위는 외부 확인을 받을 수 없는 항목에 대한 일반 규칙이다.
- 설명: 포화 후 기록 중단과 누락 표시 없음은 내부 설계 선택이며 OEM 확인 대상이 아니다. 또한 DEC-003 이후 추가된 항목이라 사용자가 승인한 적이 없다.
- 조치 요청: 사용자 승인을 받아 별도 DEC로 기록하거나, 설계 결정으로 확정하고 확인 주체와 상태를 고친다.

### 3.7 N6-07 (Minor): 유지 구간의 접근위험 도어 해제 요청이 억제로 기록되지 않는다 (미정의)
- 위치: `docs/safety-requirements.md:57`(SSR-015: "접근위험 TRUE인 도어의 첫 억제 시각을 기록"), `docs/SRS.md:86`(SRQ-FUN-017 "해제가 억제된 도어"). 구현: `src/ecl_ovr.c:122`(`blocked = crash_confirmed || hold_reason != RC_NONE`), `:87`(blocked이면 요청으로 세지 않음).
- 설명: P2~P4 유지나 crash hold 중의 접근위험 도어 해제 요청을 "억제 평가"로 볼지 요구에 없다. 코드는 세지 않으므로 첫 억제 시각과 override 창이 시작되지 않는다. OI-011 가정에도 이 경우가 없다.
- 조치 요청: 유지 구간의 해제 요청이 억제에 해당하는지 SSR-015와 SRQ-FUN-017에 쓰고 OI-011에 기록한다.

### 3.8 N6-08 (Minor): STALE 중 PENDING 지속의 중단 규칙이 없다
- 위치: `docs/SRS.md:43`(PENDING 지속: "PENDING이 계속되는 기간"). 구현: `src/ecl_val.c:301`(is_pending은 crash가 VALID일 때만), `:317-320`(VALID가 아니면 pending 상태를 갱신하지 않음).
- 설명: crash가 STALE인 구간이 끼면 PENDING 타이머가 멈추지도 리셋되지도 않고 이어진다. 중단된 뒤 다시 PENDING이 되면 과거 시작 시각 기준으로 1.0 s를 넘어 타임아웃이 빨리 난다. 요구에 "계속되는"의 중단 처리가 없다.
- 조치 요청: STALE 구간에서 PENDING 지속을 리셋할지 정지할지 SRS §0과 SSR-003/004에 쓴다.

### 3.9 N6-09 (Minor): 첫 스냅샷 이전 틱의 동작이 정의되지 않았다
- 위치: `src/ecl_val.c:426-428`(스냅샷 수신 전 틱: `last_rx_s = 0`을 기준으로 `now − 0 > 0.200`이면 STALE이고 DEGRADED). 요구: `docs/safety-requirements.md:32`(SSR-008은 "마지막으로 스냅샷을 수신한 평가"를 기준으로 하는데 그런 평가가 없을 때를 정하지 않음).
- 조치 요청: 첫 스냅샷 이전 틱의 state, 출력, 이유코드를 정의하고 SSR-008에 추가한다.

---

## 4. 체크리스트 판정

### 4.1 checklists.md 1장 SWE.1

| # | 체크 항목 | 판정 | 근거/비고 |
|---|---|---|---|
| 1 | 기능/비기능 요구사항이 구분되어 있는가 | Pass | 기능 §1(`SRS.md:49~116`), 비기능 §2(`:117~149`). |
| 2 | 기능 안전 요구사항이 별도 문서에서 관리되고 참조만 되어 있는가 | Pass | SSR은 `safety-requirements.md`. SRS는 ID만 참조. 정의되지 않은 SSR 참조 0건. |
| 3 | 비기능 요구사항이 품질/제한/제약으로 구분되어 있는가 | Pass | QUA(`:119~124`), LIM(`:126~133`), CON(`:135~149`). |
| 4 | 품질 요구가 ISO/IEC 25010 특성으로 분류되고 측정 가능한 기준이 있는가 | Pass | SRQ-QUA-FUN-001(벡터 V, SHA-256, 1,000회), SRQ-QUA-REL-001(NULL → -1). |
| 5 | 각 요구사항이 단일하고 모호하지 않으며 검증 가능한가 | Fail | N5-03(SSR-027 예외 (2)와 P4 행), N5-04(시각 상한 없음), N-12(SRQ-LIM-002 주체 혼합), N6-04(STALE 정의), N6-07~N6-09(미정의 구간), R-10(SRQ-CON-009 INS 기계 검사 불가). |
| 6 | 요구사항 간 충돌·중복이 없는가 | Fail | N5-03(SSR-027 예외 (2) ↔ 표 P4 행 ↔ SSR-004), N6-01(CONFIRMED RELEASE ↔ STALE 전파, 안전 상태 가정), N6-03(OI-013 ↔ SSR-010/026). |
| 7 | 필수 속성(ID, 근거, 우선순위, 검증 방법, 상태)이 있는가 | Fail | N6-05: SRQ-CON-002~007은 헤더 없는 표 조각이라 "검증", "상위(OEM)" 열 의미가 표로 정해지지 않는다. 나머지 요구는 속성 열을 갖는다. |
| 8 | 시스템 요구사항과 양방향 추적이 되며 고아 항목이 없는가 | Pass | 상위(OEM) ↔ §1 13키 일치(§0). 고아 0. 정의되지 않은 참조 0(헤더 없는 행을 포함해 스캔). OI 역링크 22건 일치. |
| 9 | 요구사항이 아키텍처/테스트와 양방향 추적 가능한 상태인가 | Pass | traceability §2 91행 전부, §3 UC 예외 목록 일치, 테스트 함수명 75건 존재. |

### 4.2 requirements-analysis 규칙

| # | 체크 항목 | 판정 | 근거/비고 |
|---|---|---|---|
| R1 | (분류 2) 안전 요구 분리와 ID 참조 | Pass | §0 참조 무결성. |
| R2 | (분류 3) 품질 특성·제한·제약 분류 | Pass | QUA는 ISO 25010 특성 표기, LIM·CON 분리. |
| R3 | (작성) shall 1개 1의무 | Fail | N-12(SRQ-LIM-002 수용기준 (2)는 SW 의무가 아니다). |
| R4 | (작성) 모호 표현 금지, 수치·조건·단위 명시 | Fail | N5-04(시각 상한 없음), N6-04(STALE 정의), N6-07~09(미정의 구간), R-10(INS 기준 "가리키는 경우"). ms 표기는 해소. |
| R5 | (작성) ID 불변·비재사용 | Pass | 신규 ID 없음(v0.2 이후 ID 재사용 없음). |
| R6 | (추적) 추적 매트릭스와 고아·미추적 점검 | Pass | §0 전 항목 일치. 단 N6-05 형식 결함은 7항에서 기록. |
| R7 | (완료 조건) 일관성·모호성·검증 가능성 점검 완료 | Fail | N5-03, N6-01, N6-02(구현), N6-03. |
| R8 | (추적) OEM 14개 요구의 누락·왜곡 | Fail | N6-01: OEM-SR-001 "유효 충돌 입력 시 300 ms 이내 RELEASE"를 freshness 조건으로 바꾸는 결과를 기록하지 않음. OEM-SR-004 "그대로" 주장 해소(R-03 Closed), OEM-SR-002 ↔ FR-005는 OI-006. |
| R9 | (추적) IF-001~009 매핑 | Pass | SRS §3 표와 traceability §1 IF 행 육안 대조. 변경 없음. |
| R10 | 임시 가정(open-issues) 타당성 | Fail | §2: Open 7건(OI-003, 006, 007, 011, 013, 017, 022). |
| R11 | (OEM §8 G1 인도 증거) Use Case, 베이스라인 | Pass | UC-01~10, traceability §3 매트릭스 일치. BL-SWR-1.0은 DEC-006. |
| R12 | (저작권/공개 증거) 원문 재배포 | Pass | DEC-005, `.gitignore`의 `/OEM/`. 원문 내용 미포함. |

### 4.3 공통 확인 사항

| 항목 | 판정 | 근거/비고 |
|---|---|---|
| 양방향 추적성이 상·하위 산출물과 일치하는가 | Pass | §0 전 항목 일치(OEM 13키, OI 22건, §2 91행, §3 UC). |
| 해당 스킬의 필수 규칙을 모두 만족하는가 | Fail | R3, R4, R7, R8, R10 및 체크리스트 5, 6, 7. |
| 이전 리뷰 결함이 모두 종결되었는가 | Fail | 005 열린 결함 18건 중 14건 Closed, 4건 Open (R-10, N-12, N5-03, N5-04). |

---

## 5. 열린 결함 목록 (006 기준)

| ID | 심각도 | 위치 | 설명 | 조치 요청 | 상태 |
|---|---|---|---|---|---|
| R-10 | Major (재개) | `docs/SRS.md:143`, `docs/open-issues.md:23`(OI-017), `docs/decisions.md:43`(DEC-010) | 전제 "비-C 소스가 없다"가 거짓이다(Python 파일 7개). INS 기준이 기계 검사 불가. | 적용 범위를 정하고 도구 처리를 정한다. OI-017, DEC-010 근거를 고친다. INS 패턴을 고정 목록으로 적는다. | Open |
| N5-03 | Major | `docs/safety-requirements.md:88`(SSR-027 예외 2), `:101`(P4 행), `:21`(SSR-004), `docs/SRS.md:168`(부록 A 2) | 타임아웃 평가의 출력이 P4 유지인지 P5~P10인지 문서가 서로 다르다. 코드는 부록 A(2)와 같다. | P4 행에서 타임아웃 코드를 빼고, SSR-027 예외 (2)와 SSR-004에 "P1·P2·P4 미성립 시"를 쓴다. | Open |
| N6-01 | Major | `docs/safety-requirements.md:11, :17, :31, :37`, `docs/SRS.md:33`; 구현 `src/ecl_val.c:371-375, :252-253`, `src/ecl_arb.c:297-300` | crash_status의 freshness 근거가 없다. 속도 기준 시간 초과가 CONFIRMED를 STALE로 만들어 RELEASE를 막는다(안전 상태 가정과 충돌). | crash freshness 근거를 정하거나 CONFIRMED는 freshness와 무관하게 P1 적용을 쓴다. 결과를 SSR-001, SSR-029, OI-013에 기록한다. | Open |
| N6-02 | Major (구현) | 구현 `src/ecl_val.c:414-424, :172-174, :156-158`; 요구 `docs/safety-requirements.md:32` | 틱 STALE 기준이 source_timestamp_s 기준이라 SSR-008보다 일찍 STALE이 된다(예: 1.000/0.990 → 1.200에서 STALE). 주석의 논리가 거꾸로다. | 틱 판정을 "마지막 수신 평가 timestamp_s" 기준으로 고친다. source 지연이 있는 경계 시험을 추가한다. | Open |
| N-12 | Minor | `docs/SRS.md:132`(SRQ-LIM-002) | 수용기준 (2)가 시험 하네스 전제이며 SW 의무가 아니다. 한 요구에 주체가 섞였다. | 호출 간격 계약은 OI-003 또는 별도 SRQ-CON으로 옮기고 SRQ-LIM-002는 (1)만 둔다. | Open |
| N5-04 | Minor (하향) | `docs/SRS.md:23`, `docs/safety-requirements.md:21, :22, :31, :32`, `docs/SRS.md:84, :88, :89, :90` | 허용오차 규칙은 |t| ≤ 8e6 s에서 맞지만 시각 상한이 없다. 1e7 s에서 0.200 경계 40% 오판. | timestamp_s 상한을 쓰거나 정수 틱으로 정의한다. | Open |
| N6-03 | Minor | `docs/open-issues.md:19`(OI-013 가정) | "DEGRADED는 직전 출력 유지"가 SSR-010 축소와 SSR-026과 모순된다. | 가정을 STALE 유도 DEGRADED로 한정하고 INVALID 대체는 SSR-027을 따른다고 고친다. | Open |
| N6-04 | Minor | `docs/SRS.md:33`(§0 STALE), `docs/safety-requirements.md:37`(SSR-029), 구현 `src/ecl_val.c:193-204` | STALE 정의가 세 원인을 구분하지 않는다. | §0 정의를 세 원인으로 넓힌다. | Open |
| N6-05 | Minor | `docs/SRS.md:144-150`(SRQ-CON-002~007), `docs/decisions.md:41-43`(DEC-008~010) | 헤더 없는 표 조각이라 렌더링되지 않고 열 의미가 없다. | 표 헤더 아래로 옮긴다. | Open |
| N6-06 | Minor | `docs/open-issues.md:28`(OI-022), `docs/decisions.md:8`(DEC-003) | 내부 설계 선택을 DEC-003 외부 확인 가정으로 종결했고 항목별 승인이 없다. | 사용자 승인 DEC를 추가하거나 설계로 확정하고 확인 주체를 고친다. | Open |
| N6-07 | Minor | `docs/safety-requirements.md:57`(SSR-015), `docs/SRS.md:86`(SRQ-FUN-017), 구현 `src/ecl_ovr.c:122, :87` | 유지 구간의 해제 요청이 억제인지 미정의다. 코드는 세지 않는다. | 억제 정의를 쓰고 OI-011에 기록한다. | Open |
| N6-08 | Minor | `docs/SRS.md:43`(PENDING 지속), 구현 `src/ecl_val.c:301, :317-320` | STALE 구간에서 PENDING 타이머가 멈추지도 리셋되지도 않는다. | 중단 규칙을 SRS §0과 SSR-003/004에 쓴다. | Open |
| N6-09 | Minor | 구현 `src/ecl_val.c:426-428`; 요구 `docs/safety-requirements.md:32` | 첫 스냅샷 이전 틱의 state·출력·이유코드가 정의되지 않았다. | 규칙을 정의하고 SSR-008에 추가한다. | Open |

**Open 합계: 13건 (Major 4, Minor 9)**
- 005 이월 Open 4건: R-10(Major, 재개), N-12(Minor), N5-03(Major), N5-04(Minor, 하향)
- 006 신규 9건: N6-01(Major), N6-02(Major), N6-03~N6-09(Minor 7건)

---

## 6. 요구사항 리뷰 결론
- 최종 판정: **Rejected**
- 열린 결함: **13건** (Major 4, Minor 9)
- 다음 단계 진행: **불가** (열린 결함 13건)
- 재리뷰 조건
  1. 안전 상태 일관성 (N6-01, N6-03, N6-02): crash_status의 freshness 근거와 CONFIRMED 처리를 정하고 안전 상태 가정과 맞춘다. OI-013 가정을 SSR-010·026과 맞춘다. 틱 판정을 SSR-008 기준으로 정정한다(구현).
  2. 타임아웃·PENDING (N5-03, N6-08, N6-09): 타임아웃 평가의 출력 규칙을 표 한 곳에서 정하고 P4 행을 고친다. STALE 중 PENDING 중단과 첫 스냅샷 이전 틱을 정한다.
  3. 경계·구간 정의 (N5-04, N6-04, N6-07, N-12): timestamp_s 상한 또는 정수 틱, STALE 정의, 억제 정의, SRQ-LIM-002 주체를 고친다.
  4. 승인·형식 (N6-05, N6-06, R-10): 헤더 없는 표를 복원하고 DEC-008~010을 표 안으로 옮긴다. OI-022는 사용자 승인 DEC로 기록하거나 설계로 확정한다. R-10은 도구 범위와 INS 기준을 정하고 OI-017, DEC-010 근거를 고친다.

**참고 관찰 (결함 아님)**
- DEC-008(`decisions.md:41`)은 단계별 리뷰 게이트 생략을 사용자 지시의 해석으로 기록했다. 해석 확인은 사용자 몫이다. 이 리뷰 기록은 그 해석을 바꾸지 않는다.
- OI-001, OI-020은 요구 본문에서 참조되지 않는다. 문서 지위와 프로세스 증거 항목이라 결함으로 세지 않았다.
- 변경 이력은 SRS와 SSR 모두 v0.4에 존재한다(005 관찰 해소).

## 변경 이력
- 2026-11-27 최초 작성 (requirements-review-006, 005 열린 결함 18건 closure 판정: Closed 14, Open 4, DEC-003 조건 ①②③ OI별 재판정, 신규 N6-01~N6-09. ID 정의·참조 무결성·OI 역링크·traceability 상위 열·§2/§3 커버리지·ms 잔존·벡터 CSV·부동소수점 경계를 스크립트로 직접 검사하고, `src/ecl_val.c`·`ecl_arb.c`·`ecl_ovr.c`·`ecl_sta.c`·`ecl_log.c`와 요구 문언을 대조했다.)
