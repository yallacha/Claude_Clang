# Use Case (OEM G1 인도 증거)

액터: 운전자(Driver), 차량(Vehicle), 표시 계층(Display). 범위는 PC/SIL 논리 제어이다. 요구사항 ID는 `docs/SRS.md`, `docs/safety-requirements.md`.

| UC | 이름 | 액터 | 주 흐름 | 관련 요구사항 |
|---|---|---|---|---|
| UC-01 | 정지 중 잠금/해제 | Driver | 정지 상태에서 4개 경로 중 하나로 좌/우/전체 lock·unlock을 입력하면 선택 도어에만 반영된다. 부정(누락/미등록)은 거절된다. | SRQ-FUN-001~008, 031, 038, SSR-027 |
| UC-02 | 이동 시작 자동 잠금 | Vehicle | 속도 3.0 km/h 이상이 되면 두 도어가 LOCK이 되고 이동 중 unlock은 거부된다. | SRQ-FUN-004, 005, 009 |
| UC-03 | 충돌 긴급 해제 | Vehicle | crash_status=CONFIRMED이면 어떤 명령보다 우선하여 두 도어가 RELEASE이고 유지된다. PENDING은 출력 유지(최대 1.0 s). | SSR-001~004, 031, 036 |
| UC-04 | 접근위험 해제 억제와 override | Vehicle, Driver | 접근위험 도어는 LOCK이고 해제가 억제된다. 10.0 s 이내 같은 해제 재입력은 override로 표시되나 LOCK은 유지된다. | SSR-011~015, 035, SRQ-FUN-017~020, 033~035 |
| UC-05 | 화재·과온·성인 탑승 해제 | Vehicle | 입력이 TRUE이면 두 도어가 RELEASE이고 입력별 이유코드가 기록된다. | SRQ-FUN-010, 011, 032 |
| UC-06 | ISOFIX 강제 잠금 | Vehicle | ISOFIX 연결된 쪽 도어를 LOCK으로 한다. | SRQ-FUN-012, 013 |
| UC-07 | 시동 OFF 초기화 | Vehicle | ignition_on=FALSE이면 두 도어가 RELEASE이고 state=OFF이다(상위 규칙 없을 때). | SRQ-FUN-014~016, 036 |
| UC-08 | 입력 이상 처리 | Vehicle | 형식·범위 오류는 INVALID, 0.200 s 초과 미갱신은 DEGRADED(출력 유지), sensor_fault는 FAULT(출력 유지). | SSR-005~010, 016~026, 029, 030, 032~034, 037 |
| UC-09 | 상태 조회와 표시 | Display | 상태 응답이 출력, state, 입력 유효성, 이유코드, override를 제공한다. 직렬화 실패는 HTTP 500이다. | SRQ-FUN-022~025 |
| UC-10 | 이벤트 이력 조회 | Display | 최근 100건이 ID 오름차순으로 조회되고 재기동 후 0건이다. | SRQ-FUN-027~030, 037, 039, SRQ-LIM-001, 003 |
