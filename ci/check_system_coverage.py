#!/usr/bin/env python3
"""시스템 테스트 요구사항 커버리지 게이트 (CLAUDE.md 2장: 요구사항 커버리지 100%).

docs/SRS.md, docs/safety-requirements.md 의 모든 요구사항 ID가 docs/system-test/system_tests.json 에서
(a) 일반(비 [제안]) 테스트 케이스로 덮이거나 (b) 시스템 테스트 대상이 아님을 사유와 함께 open_issues 로 기록했는지 검사한다.
(b)는 이터레이션 3 범위의 표시 계층 요구사항뿐이어야 한다(INS/BLD 요구사항은 ci/inspection.sh 로 검사하는 TC-I 케이스로 덮는다). 사용법: ci/check_system_coverage.py [최소 %(기본 100)]
"""
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
minimum = float(sys.argv[1]) if len(sys.argv) > 1 else 100.0
ids = []
for f in ("docs/SRS.md", "docs/safety-requirements.md"):
    for line in (ROOT / f).read_text(encoding="utf-8").splitlines():
        m = re.match(r"\| ((?:SRQ|SSR)-[A-Z0-9-]+) \|", line)
        if m:
            ids.append(m.group(1))
data = json.loads((ROOT / "docs/system-test/system_tests.json").read_text(encoding="utf-8"))
covered = set()
for tc in data["test_cases"]:
    if not tc.get("proposed"):
        covered.update(tc["basis_ids"])
excluded = {o["basis_id"] for o in data["open_issues"]}
bad_excluded = sorted(o["basis_id"] for o in data["open_issues"] if "이터레이션 3" not in o["question"])
missing = sorted(i for i in ids if i not in covered and i not in excluded)
unknown = sorted(c for c in covered if c not in ids)
testable = [i for i in ids if i not in excluded]
cov = 100.0 * sum(1 for i in testable if i in covered) / len(testable)
print(f"요구사항 {len(ids)}건, 시스템 테스트 대상 {len(testable)}건, 케이스 {len(data['test_cases'])}건")
print(f"대상 요구사항 커버리지: {cov:.2f}% (제외 {len(excluded)}건: 이터레이션 3 이연 — 표시 계층, 사유는 system_tests.json open_issues)")
if missing:
    print("테스트 케이스 없는 요구사항:", ", ".join(missing))
if unknown:
    print("요구사항에 없는 ID를 참조하는 케이스:", ", ".join(unknown))
if bad_excluded:
    print("이터레이션 3 이연 사유가 없는 제외 요구사항:", ", ".join(bad_excluded))
ok = cov >= minimum and not missing and not unknown and not bad_excluded
sys.exit(0 if ok else 1)
