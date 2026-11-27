#!/usr/bin/env python3
"""docs/traceability.md §2(요구사항 → 후속 산출물)를 생성한다.
입력: docs/architecture.md §7(컴포넌트 할당), tests/{unit,integration,tdd}/*.cpp 의 @tracing, docs/system-test/system_tests.json.
실행: 이미지 안에서 python3 tools/gen_traceability.py
"""
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ID = re.compile(r"(?:SRQ|SSR)-[A-Z0-9]+(?:-[A-Z0-9]+)*")

# 요구사항 목록
reqs = []
for f in ("docs/SRS.md", "docs/safety-requirements.md"):
    for line in (ROOT / f).read_text(encoding="utf-8").splitlines():
        m = re.match(r"\| ((?:SRQ|SSR)-[A-Z0-9-]+) \|", line)
        if m:
            reqs.append(m.group(1))

# 컴포넌트 할당(아키텍처 §7)
comp = {r: set() for r in reqs}
sec = (ROOT / "docs/architecture.md").read_text(encoding="utf-8").split("## 7.")[1]
SRC = {"CMP-TYP": "include/ecl_types.h", "CMP-VAL": "ecl_val", "CMP-CMD": "ecl_cmd", "CMP-OVR": "ecl_ovr",
       "CMP-ARB": "ecl_arb", "CMP-STA": "ecl_sta", "CMP-LOG": "ecl_log", "CMP-SER": "ecl_ser", "CMP-CORE": "ecl_core",
       "CMP-WEB": "(이터레이션 3)", "전체(횡단)": "전체"}
for line in sec.splitlines():
    m = re.match(r"\| (CMP-[A-Z]+|전체\(횡단\)) \| (.*) \|", line)
    if m:
        for rid in ID.findall(m.group(2)):
            if rid in comp:
                comp[rid].add(m.group(1))

# 시험 @tracing
tests = {"unit": {r: [] for r in reqs}, "integration": {r: [] for r in reqs}, "tdd": {r: [] for r in reqs}}
for kind in tests:
    for p in sorted((ROOT / "tests" / kind).glob("*.cpp")):
        text = p.read_text(encoding="utf-8")
        for m in re.finditer(r"/\*\*(.*?)\*/\s*TEST\((\w+),\s*(\w+)\)", text, re.S):
            ids = set(ID.findall(" ".join(l for l in m.group(1).splitlines() if "@tracing" in l)))
            for rid in ids:
                if rid in tests[kind]:
                    tests[kind][rid].append(f"{m.group(2)}.{m.group(3)}")
# TDD 테스트는 @tracing이 없으므로 단위/통합과 구분된 별도 결과물로만 유지한다.

st = json.loads((ROOT / "docs/system-test/system_tests.json").read_text(encoding="utf-8"))
sys_tc = {r: [] for r in reqs}
for tc in st["test_cases"]:
    for b in tc["basis_ids"]:
        if b in sys_tc:
            sys_tc[b].append(tc["id"])


def compress(tcs):
    base = [t for t in tcs if not t.startswith("TC-F-013-")]
    pw = [t for t in tcs if t.startswith("TC-F-013-")]
    if pw:
        base.append("TC-F-013-01~%02d" % len(pw))
    return ", ".join(base)


rows = ["| 요구사항 | 컴포넌트 | 소스 모듈 | 단위 테스트 | 통합 테스트 | 시스템 테스트 |", "|---|---|---|---|---|---|"]
no_ut, no_st = [], []
for r in reqs:
    cs = sorted(comp[r])
    mods = ", ".join(sorted(SRC[c] for c in cs))
    ut = ", ".join(sorted(set(tests["unit"][r]))) or "-"
    it = ", ".join(sorted(set(tests["integration"][r]))) or "-"
    stc = compress(sys_tc[r]) or "-"
    rows.append(f"| {r} | {', '.join(cs) or '-'} | {mods or '-'} | {ut} | {it} | {stc} |")
    if ut == "-":
        no_ut.append(r)
    if stc == "-":
        no_st.append(r)

path = ROOT / "docs" / "traceability.md"
text = path.read_text(encoding="utf-8")
head, rest = text.split("## 2. SW 요구사항 → 후속 산출물", 1)
_, tail = rest.split("## 3.", 1)
new = ("## 2. SW 요구사항 → 후속 산출물 (자동 생성: `tools/gen_traceability.py`)\n\n"
       "컴포넌트는 `docs/architecture.md` §7, 소스 모듈은 `docs/detailed-design.md`, 단위/통합 테스트는 테스트 함수 주석의 `@tracing`(예: `ArbUnit.CrashConfirmedReleasesBothAboveAll`), "
       "시스템 테스트는 `docs/system-test/ST-ECL-001.xlsx`의 케이스 ID이다. TDD 테스트(`tests/tdd`)는 구현 주도 테스트이며 별도 결과물이라 이 표에 포함하지 않는다.\n\n"
       + "\n".join(rows) + "\n\n"
       f"- **시스템 테스트 미추적 요구사항**: {', '.join(no_st) or '없음'} (검증 방법이 INS/BLD로 정의된 제약. 사유: 시스템 테스트 `미결 사항` 시트 Q-01~Q-08).\n"
       f"- **단위 테스트 @tracing 없는 요구사항**: {', '.join(no_ut) or '없음'} (UT가 검증 방법이 아니거나 통합/시스템에서 검증).\n\n")
path.write_text(head + new + "## 3." + tail, encoding="utf-8")
print(f"요구사항 {len(reqs)}건, ST 미추적 {len(no_st)}건, UT 미추적 {len(no_ut)}건")
