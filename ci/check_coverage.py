#!/usr/bin/env python3
"""llvm-cov export -summary-only JSON 에서 커버리지 지표를 읽어 기준 미달이면 실패한다.

사용법: check_coverage.py <summary.json> <functions|mcdc|branches|lines|regions> <최소 퍼센트>
커버 대상 항목이 0개이면(예: 조건식이 없음) 해당 지표는 100% 로 본다.
"""
import json
import sys


def main(argv):
    if len(argv) != 4:
        print(__doc__)
        return 2
    path, metric, threshold = argv[1], argv[2], float(argv[3])
    with open(path, encoding="utf-8") as f:
        data = json.load(f)
    totals = data["data"][0]["totals"]
    if metric not in totals:
        print(f"오류: 요약에 '{metric}' 지표가 없습니다 (가능: {', '.join(totals)})")
        return 2
    item = totals[metric]
    count = item.get("count", 0)
    percent = 100.0 if count == 0 else item.get("percent", 0.0)
    ok = percent >= threshold
    print(f"{metric}: {percent:.2f}% ({item.get('covered', 0)}/{count}) 기준 {threshold:.0f}% -> {'PASS' if ok else 'FAIL'}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
