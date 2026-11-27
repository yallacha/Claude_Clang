#!/usr/bin/env python3
"""시스템 테스트 케이스 생성기 (SWE.6, `sw-system-test-cases` 스킬).

입력: docs/SRS.md, docs/safety-requirements.md (요구사항), PICT 모델.
출력:
  docs/system-test/system_tests.json       build_workbook.py 입력
  docs/system-test/pict_model_arb.txt      PICT 모델 (SSR-027 중재 조합)
  docs/system-test/pict_result_arb.tsv     PICT 결과
  tests/system/system_cases.inc            자동 실행용 케이스 표 (자동 생성, 수정하지 말 것)
기대값은 요구사항(우선순위 표 SSR-027 등)에서 도출한 독립 오라클/손 계산이며 구현 코드를 참조하지 않는다.
실행: 이미지 안에서  python3 tools/gen_system_tests.py
"""
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "docs" / "system-test"
OUT.mkdir(parents=True, exist_ok=True)
(ROOT / "tests" / "system").mkdir(parents=True, exist_ok=True)

ABSENT = "ABSENT"
NAN = "NaN"
NOCMD = None
SRC = {"PHYSICAL_BUTTON": 0, "AVN": 1, "VOICE": 2, "MOBILE_APP": 3}
SIDE = {"LEFT": 0, "RIGHT": 1, "ALL": 2}
ACT = {"LOCK": 0, "UNLOCK": 1}
ENV = "PC/SIL (ecl_core 공개 API 직접 호출, 시각 주입)"


# --------------------------------------------------------------------------- 요구사항 추출
def load_requirements():
    reqs = {}
    for f in ("docs/SRS.md", "docs/safety-requirements.md"):
        for line in (ROOT / f).read_text(encoding="utf-8").splitlines():
            m = re.match(r"\| ((?:SRQ|SSR)-[A-Z0-9-]+) \| (.*)", line)
            if not m:
                continue
            cells = [c.strip() for c in line.strip().strip("|").split("|")]
            rid = cells[0]
            if rid.startswith("SRQ-QUA") or rid.startswith("SRQ-LIM"):
                text = cells[2] if rid.startswith("SRQ-QUA") else cells[1]
            elif rid.startswith("SRQ-CON"):
                text = cells[1]
            else:
                text = cells[1]
            reqs[rid] = text
    return reqs


# --------------------------------------------------------------------------- 단계(step) DSL
def S(exp=None, t=None, tick=False, speed=0.0, gear=0, age=0.0, crash=0, rl=0, rr=0, fire=0, ot=0, adult=0,
      ifl=0, ifr=0, ign=1, fault=0, cmd=NOCMD):
    return dict(exp=exp, t=t, tick=tick, speed=speed, gear=gear, age=age, crash=crash, rl=rl, rr=rr, fire=fire,
                ot=ot, adult=adult, ifl=ifl, ifr=ifr, ign=ign, fault=fault, cmd=cmd)


def E(l, r, state="NORMAL", reason="NONE", rej=0, ovl=-1, ovr=-1):
    return dict(L=l, R=r, state=state, reason=reason, rej=rej, ovl=ovl, ovr=ovr)


def C(side, action, src="AVN"):
    return (side, action, src)


def describe_step(i, s):
    parts = []
    if s["tick"]:
        return f"S{i}: tick t={s['t']:.2f} s"
    base = S()
    if s["t"] is not None:
        parts.append(f"t={s['t']:.2f} s")
    else:
        parts.append("t=직전+0.010 s")
    names = dict(speed="vehicle_speed_kph", gear="gear(0=P,1=N,2=D,3=R)", age="timestamp_s−source_timestamp_s",
                 crash="crash_status(0=NONE,1=PENDING,2=CONFIRMED)", rl="rear_left_approach_risk(boolean)",
                 rr="rear_right_approach_risk(boolean)", fire="fire_detected(boolean)",
                 ot="overtemperature_detected(boolean)", adult="adult_present(boolean)", ifl="isofix_left(boolean)",
                 ifr="isofix_right(boolean)", ign="ignition_on(boolean)", fault="sensor_fault(boolean)")
    units = dict(speed=" km/h", age=" s")
    for k, n in names.items():
        if s[k] != base[k]:
            parts.append(f"{n}={s[k]}{units.get(k, '')}")
    if s["cmd"] is not None:
        parts.append("cmd=" + "/".join(str(x) for x in s["cmd"]))
    return f"S{i}: " + (", ".join(parts) if parts else "정상 입력")


def describe_exp(i, e):
    if e is None:
        return None
    t = f"S{i}: lock_left={e['L']}, lock_right={e['R']}, state={e['state']}, reason=RC_{e['reason']}"
    if e["rej"]:
        t += ", command_rejected=1"
    if e["ovl"] >= 0:
        t += f", override_left={e['ovl']}"
    if e["ovr"] >= 0:
        t += f", override_right={e['ovr']}"
    return t


# --------------------------------------------------------------------------- 케이스 정의
cases = []  # 자동 실행 + Excel
special = []  # 수작업 C++ 시험(Excel만)


def add_case(tc_id, basis, category, purpose, steps, techniques, pre="초기화 직후(ecl_core_init)", priority="High",
             initial=None, remarks="", env=ENV, auto=True):
    cases.append(dict(id=tc_id, basis=basis, category=category, purpose=purpose, steps=steps,
                      techniques=techniques, pre=pre, priority=priority, initial=initial, remarks=remarks,
                      env=env, auto=auto))


RESET = lambda: S(cmd=C("ALL", "UNLOCK"), exp=E("RELEASE", "RELEASE", reason="DRIVER_UNLOCK"))

# TC-F-001 / 002: source x side x action (4 x 3 x 2 = 24 전 조합)
st = []
for src in SRC:
    for side in SIDE:
        st.append(RESET())
        l = "LOCK" if side in ("LEFT", "ALL") else "RELEASE"
        r = "LOCK" if side in ("RIGHT", "ALL") else "RELEASE"
        st.append(S(cmd=C(side, "LOCK", src), exp=E(l, r, reason="DRIVER_LOCK")))
add_case("TC-F-001", ["SRQ-FUN-001", "SRQ-FUN-002", "SRQ-FUN-006", "SRQ-FUN-021"], "functional",
         "정지·정상 입력에서 source 4종 × side 3종의 lock 명령이 선택된 도어에만 적용되고 반대쪽 도어는 변하지 않는다(24개 중 lock 12조합, 전 조합 완전 열거)",
         st, ["동치 분할", "요구사항 기반"], pre="정지(speed 0.0), 정상 입력, 직전 출력 RELEASE/RELEASE")
st = []
for src in SRC:
    for side in SIDE:
        st.append(S(cmd=C("ALL", "LOCK", src), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")))
        l = "RELEASE" if side in ("LEFT", "ALL") else "LOCK"
        r = "RELEASE" if side in ("RIGHT", "ALL") else "LOCK"
        st.append(S(cmd=C(side, "UNLOCK", src), exp=E(l, r, reason="DRIVER_UNLOCK")))
add_case("TC-F-002", ["SRQ-FUN-001", "SRQ-FUN-003", "SRQ-FUN-006"], "functional",
         "정지·정상 입력에서 source 4종 × side 3종의 unlock 명령이 선택된 도어만 RELEASE하고 반대쪽 도어는 변하지 않는다(24개 중 unlock 12조합)",
         st, ["동치 분할", "요구사항 기반"], pre="정지(speed 0.0), 정상 입력")

add_case("TC-F-003", ["SRQ-FUN-004", "SRQ-FUN-005", "SRQ-FUN-009"], "functional",
         "속도 경계 3.0 km/h(2.9, 3.0, 300.0)에서 자동 LOCK과 이동 중 unlock 거부(이유코드 RC_UNLOCK_DENIED_MOVING)를 확인한다",
         [S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(speed=2.9, cmd=C("ALL", "UNLOCK"), exp=E("RELEASE", "RELEASE", reason="DRIVER_UNLOCK")),
          S(speed=3.0, exp=E("LOCK", "LOCK", reason="SPEED_AUTO_LOCK")),
          S(speed=3.0, cmd=C("ALL", "UNLOCK"), exp=E("LOCK", "LOCK", reason="UNLOCK_DENIED_MOVING")),
          S(speed=300.0, exp=E("LOCK", "LOCK", reason="SPEED_AUTO_LOCK")),
          S(speed=2.9, exp=E("LOCK", "LOCK", reason="NONE")),
          S(speed=2.9, cmd=C("ALL", "UNLOCK"), exp=E("RELEASE", "RELEASE", reason="DRIVER_UNLOCK"))],
         ["경계값 분석", "동치 분할", "요구사항 기반"], pre="IGN ON, 기어 P/D, 정상 입력")

bad = []
for f, vals in (("side", [ABSENT, -1, 3]), ("action", [ABSENT, -1, 2]), ("source", [ABSENT, -1, 4])):
    for v in vals:
        c = [0, 0, 0]
        c[("side", "action", "source").index(f)] = v
        bad.append((f, v, c))
for n, (f, v, c) in enumerate(bad, start=1):
    cmd = (c[0] if f == "side" else "ALL", c[1] if f == "action" else "UNLOCK", c[2] if f == "source" else "AVN")
    st = [S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(cmd=cmd, exp=E("LOCK", "LOCK", reason="COMMAND_REJECTED", rej=1)),
          S(exp=E("LOCK", "LOCK", reason="NONE"))]
    add_case(f"TC-F-004-{n:02d}", ["SRQ-FUN-007", "SRQ-FUN-008", "SRQ-FUN-031", "SRQ-FUN-038"], "functional",
             f"명령 필드 {f}만 {v}(누락/범위 밖/미등록, 케이스당 무효 클래스 1개)로 주입하면 거절되어 출력이 유지되고 command_rejected=1, 이유코드 RC_COMMAND_REJECTED이다",
             st, ["동치 분할", "경계값 분석", "요구사항 기반"], pre="직전 출력 LOCK/LOCK(첫 단계로 설정), 정지·정상 입력")

add_case("TC-F-005", ["SRQ-FUN-010", "SRQ-FUN-011", "SRQ-FUN-032"], "functional",
         "화재, 과온, 성인 탑승 각각 단독 및 동시 TRUE일 때 두 도어가 RELEASE되고 이유코드가 입력별(동시 시 fire > overtemp > adult)로 기록된다",
         [S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(fire=1, exp=E("RELEASE", "RELEASE", reason="FIRE")),
          S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(ot=1, exp=E("RELEASE", "RELEASE", reason="OVERTEMP")),
          S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(adult=1, exp=E("RELEASE", "RELEASE", reason="ADULT_PRESENT")),
          S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(fire=1, adult=1, exp=E("RELEASE", "RELEASE", reason="FIRE")),
          S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(ot=1, adult=1, exp=E("RELEASE", "RELEASE", reason="OVERTEMP"))],
         ["동치 분할", "요구사항 기반"], pre="정상 입력, 정지, 직전 출력 LOCK/LOCK(단계 설정)")

add_case("TC-F-006", ["SRQ-FUN-012", "SRQ-FUN-013"], "functional",
         "ISOFIX가 TRUE인 쪽만 LOCK되고 FALSE인 쪽은 ISOFIX 규칙으로 변경되지 않으며 ISOFIX 도어는 운전자 unlock보다 우선한다",
         [S(cmd=C("ALL", "UNLOCK"), exp=E("RELEASE", "RELEASE", reason="DRIVER_UNLOCK")),
          S(ifl=1, exp=E("LOCK", "RELEASE", reason="ISOFIX")),
          S(ifl=1, ifr=1, exp=E("LOCK", "LOCK", reason="ISOFIX")),
          S(ifl=0, ifr=1, exp=E("LOCK", "LOCK", reason="ISOFIX")),
          S(ifl=1, ifr=0, cmd=C("ALL", "UNLOCK"), exp=E("LOCK", "RELEASE", reason="ISOFIX"))],
         ["동치 분할", "요구사항 기반"], pre="정지, 정상 입력")

add_case("TC-F-007", ["SRQ-FUN-014", "SRQ-FUN-015", "SRQ-FUN-016", "SRQ-FUN-026", "SRQ-FUN-036"], "functional",
         "초기 출력/state(RELEASE/RELEASE/OFF) 확인 후, ignition_on=FALSE이면 두 도어 RELEASE, state=OFF, 이유코드 RC_IGNITION_OFF이다(속도 3.0 이상이면 LOCK 우선)",
         [S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(ign=0, exp=E("RELEASE", "RELEASE", state="OFF", reason="IGNITION_OFF")),
          S(ign=1, exp=E("RELEASE", "RELEASE", reason="NONE")),
          S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(ign=0, speed=3.0, exp=E("LOCK", "LOCK", state="OFF", reason="SPEED_AUTO_LOCK"))],
         ["동치 분할", "요구사항 기반"], pre="초기화 직후", initial=E("RELEASE", "RELEASE", state="OFF", reason="NONE"))

add_case("TC-F-008", ["SSR-011", "SSR-012", "SSR-013", "SSR-014", "SSR-015", "SSR-035", "SRQ-FUN-017", "SRQ-FUN-018",
                      "SRQ-FUN-019", "SRQ-FUN-020", "SRQ-FUN-033", "SRQ-FUN-034", "SRQ-FUN-035"], "functional",
         "접근위험 도어는 LOCK·해제 억제(좌우 독립), 같은 source의 10.0 s 이내 재입력은 override(LOCK 유지), 10.01 s 초과는 새 억제, 접근위험 해소 시 override 해제를 확인한다",
         [S(t=1.00, cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(t=2.00, rl=1, cmd=C("ALL", "UNLOCK"), exp=E("LOCK", "RELEASE", reason="APPROACH_RISK", ovl=0)),
          S(t=12.00, rl=1, cmd=C("ALL", "UNLOCK"), exp=E("LOCK", "RELEASE", reason="OVERRIDE_ACK", ovl=1, ovr=0)),
          S(t=12.01, rl=0, exp=E("LOCK", "RELEASE", reason="NONE", ovl=0)),
          S(t=20.00, rl=1, cmd=C("LEFT", "UNLOCK"), exp=E("LOCK", "RELEASE", reason="APPROACH_RISK", ovl=0)),
          S(t=30.01, rl=1, cmd=C("LEFT", "UNLOCK"), exp=E("LOCK", "RELEASE", reason="APPROACH_RISK", ovl=0)),
          S(t=30.02, rl=1, cmd=C("LEFT", "UNLOCK"), exp=E("LOCK", "RELEASE", reason="OVERRIDE_ACK", ovl=1)),
          S(t=30.03, rl=1, cmd=C("LEFT", "UNLOCK", "VOICE"), exp=E("LOCK", "RELEASE", reason="APPROACH_RISK", ovl=1)),
          S(t=45.00, rl=1, exp=E("LOCK", "RELEASE", reason="APPROACH_RISK", ovl=0))],
         ["경계값 분석", "동치 분할", "요구사항 기반"], pre="정지, 정상 입력(우 도어 접근위험 없음)")

add_case("TC-F-009", ["SSR-001", "SSR-002", "SSR-003", "SSR-004", "SSR-031", "SSR-036"], "functional",
         "충돌 CONFIRMED는 모든 명령·접근위험보다 우선해 RELEASE를 유지하고, PENDING은 1.000 s까지 출력 유지(RC_CRASH_PENDING_HOLD)·1.010 s에 RC_CRASH_PENDING_TIMEOUT을 한 번만 기록한다",
         [S(t=1.00, cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(crash=2, exp=E("RELEASE", "RELEASE", reason="CRASH_CONFIRMED")),
          S(crash=2, cmd=C("ALL", "LOCK"), exp=E("RELEASE", "RELEASE", reason="CRASH_CONFIRMED")),
          S(crash=0, exp=E("RELEASE", "RELEASE", reason="NONE")),
          S(cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(t=5.00, crash=1, exp=E("LOCK", "LOCK", reason="CRASH_PENDING_HOLD")),
          S(t=6.00, crash=1, exp=E("LOCK", "LOCK", reason="CRASH_PENDING_HOLD")),
          S(t=6.01, crash=1, exp=E("LOCK", "LOCK", reason="CRASH_PENDING_TIMEOUT")),
          S(t=6.02, crash=1, exp=E("LOCK", "LOCK", reason="NONE")),
          S(t=6.03, crash=0, exp=E("LOCK", "LOCK", reason="NONE")),
          S(t=7.00, crash=2, rl=1, exp=E("RELEASE", "RELEASE", reason="CRASH_CONFIRMED"))],
         ["경계값 분석", "동치 분할", "요구사항 기반"], pre="정지, 정상 입력")

R0 = lambda: RESET()
inv = []
inv.append(("approach_left 누락", dict(rl=ABSENT), E("LOCK", "RELEASE", "DEGRADED", "APPROACH_RISK")))
inv.append(("approach_right 범위 밖(7)", dict(rr=7), E("RELEASE", "LOCK", "DEGRADED", "APPROACH_RISK")))
inv.append(("isofix_left 범위 밖(-1)", dict(ifl=-1), E("LOCK", "RELEASE", "DEGRADED", "ISOFIX")))
inv.append(("isofix_right 누락", dict(ifr=ABSENT), E("RELEASE", "LOCK", "DEGRADED", "ISOFIX")))
inv.append(("fire 범위 밖(2)", dict(fire=2), E("RELEASE", "RELEASE", "DEGRADED", "NONE")))
inv.append(("overtemp 범위 밖(2)", dict(ot=2), E("RELEASE", "RELEASE", "DEGRADED", "NONE")))
inv.append(("adult 누락", dict(adult=ABSENT), E("RELEASE", "RELEASE", "DEGRADED", "NONE")))
inv.append(("sensor_fault 범위 밖(2)", dict(fault=2), E("RELEASE", "RELEASE", "FAULT", "SENSOR_FAULT")))
inv.append(("speed NaN", dict(speed=NAN), E("LOCK", "LOCK", "DEGRADED", "SPEED_AUTO_LOCK")))
inv.append(("speed -0.1", dict(speed=-0.1), E("LOCK", "LOCK", "DEGRADED", "SPEED_AUTO_LOCK")))
inv.append(("speed 300.1", dict(speed=300.1), E("LOCK", "LOCK", "DEGRADED", "SPEED_AUTO_LOCK")))
inv.append(("speed 300.0(유효 상한)", dict(speed=300.0), E("LOCK", "LOCK", "NORMAL", "SPEED_AUTO_LOCK")))
inv.append(("gear 범위 밖(9)", dict(gear=9), E("RELEASE", "RELEASE", "NORMAL", "NONE")))
inv.append(("crash_status 범위 밖(5)", dict(crash=5), E("RELEASE", "RELEASE", "DEGRADED", "INPUT_INVALID")))
inv.append(("ignition_on 범위 밖(2)", dict(ign=2), E("RELEASE", "RELEASE", "DEGRADED", "INPUT_INVALID")))
inv.append(("source_timestamp 미래(age -0.1)", dict(age=-0.1), E("RELEASE", "RELEASE", "DEGRADED", "INPUT_STALE")))
inv.append(("source_timestamp NaN", dict(age=NAN), E("RELEASE", "RELEASE", "DEGRADED", "INPUT_STALE")))
INV_BASIS = {  # 입력별 근거 요구사항
    "approach_left": ["SSR-005", "SSR-006", "SSR-020", "SSR-026", "SSR-032"],
    "approach_right": ["SSR-005", "SSR-006", "SSR-020", "SSR-026", "SSR-032"],
    "isofix": ["SSR-005", "SSR-006", "SSR-021", "SSR-026", "SSR-032"],
    "fire/overtemp/adult": ["SSR-005", "SSR-006", "SSR-025", "SSR-026", "SSR-032"],
    "sensor_fault": ["SSR-005", "SSR-006", "SSR-022", "SSR-032"],
    "speed": ["SSR-005", "SSR-006", "SSR-023", "SSR-026", "SSR-032"],
    "gear": ["SSR-005", "SSR-006"],
    "crash/ignition": ["SSR-005", "SSR-006", "SSR-024", "SSR-037", "SSR-026", "SSR-032"],
    "source_timestamp": ["SSR-005", "SSR-006", "SSR-029", "SSR-032"],
}
for n, (desc, kw, e) in enumerate(inv, start=1):
    key = next(k for k in INV_BASIS if any(p in desc for p in k.split("/")))
    add_case(f"TC-F-010-{n:02d}", INV_BASIS[key], "functional",
             f"무효 클래스 1개({desc})만 주입하면 안전 측 대체 값 규칙대로 출력·state·이유코드가 결정된다",
             [RESET(), S(exp=e, **kw)], ["동치 분할", "경계값 분석", "오류 추정", "요구사항 기반"],
             pre="직전 출력 RELEASE/RELEASE(reset 단계로 설정), 정지, 나머지 입력 정상")

add_case("TC-F-011", ["SSR-007", "SSR-008", "SSR-009", "SSR-010", "SSR-033", "SSR-034"], "functional",
         "freshness 경계(age 0.200 fresh, 0.210 STALE)와 틱·스냅샷 간격(마지막 스냅샷 후 0.200 s까지 정상, 0.210 s에 STALE; 0.22 s 만에 도착한 스냅샷도 STALE, 다음 격자 스냅샷에서 복구)을 확인한다",
         [S(t=1.00, cmd=C("ALL", "LOCK"), exp=E("LOCK", "LOCK", reason="DRIVER_LOCK")),
          S(t=1.01, age=0.2, exp=E("LOCK", "LOCK", reason="NONE")),
          S(t=1.02, age=0.21, exp=E("LOCK", "LOCK", "DEGRADED", "INPUT_STALE")),
          S(t=1.03, exp=E("LOCK", "LOCK", "NORMAL", "NONE")),
          S(t=2.00, exp=E("LOCK", "LOCK", "NORMAL", "NONE")),
          S(t=2.20, tick=True, exp=E("LOCK", "LOCK", "NORMAL", "NONE")),
          S(t=2.21, tick=True, exp=E("LOCK", "LOCK", "DEGRADED", "INPUT_STALE")),
          S(t=2.22, exp=E("LOCK", "LOCK", "DEGRADED", "INPUT_STALE")),
          S(t=2.23, exp=E("LOCK", "LOCK", "NORMAL", "NONE"))],
         ["경계값 분석", "동치 분할", "요구사항 기반"], pre="정지, 정상 입력")

add_case("TC-F-012", ["SSR-016", "SSR-017", "SSR-018", "SSR-019"], "functional",
         "sensor_fault=TRUE이면 새 명령을 적용하지 않고 직전 출력을 유지하며(접근위험 도어는 LOCK) state=FAULT, 경고코드 RC_SENSOR_FAULT를 제공한다",
         [S(cmd=C("LEFT", "LOCK"), exp=E("LOCK", "RELEASE", reason="DRIVER_LOCK")),
          S(fault=1, cmd=C("ALL", "UNLOCK"), exp=E("LOCK", "RELEASE", "FAULT", "SENSOR_FAULT")),
          S(fault=1, rr=1, exp=E("LOCK", "LOCK", "FAULT", "SENSOR_FAULT")),
          S(fault=0, exp=E("LOCK", "LOCK", "NORMAL", "NONE"))],
         ["동치 분할", "고장 주입", "요구사항 기반"], pre="정지, 정상 입력")

add_case("TC-F-019", ["SSR-001", "SSR-027", "SSR-028", "SSR-020", "SSR-022", "SSR-005"], "functional",
         "WoW(Worst-of-Worst): 충돌+sensor_fault+접근위험+화재+ISOFIX+속도 300+시동 OFF+lock 명령을 동시에 주입(W1), 충돌 없이 같은 조합+unlock(W2), 모든 입력과 명령을 동시에 무효(W3)",
         [S(cmd=C("ALL", "UNLOCK"), exp=E("RELEASE", "RELEASE", reason="DRIVER_UNLOCK")),
          S(crash=2, fault=1, rl=1, rr=1, fire=1, ifl=1, ifr=1, speed=300.0, ign=0, cmd=C("ALL", "LOCK"),
            exp=E("RELEASE", "RELEASE", "FAULT", "CRASH_CONFIRMED")),
          S(cmd=C("ALL", "UNLOCK"), exp=E("RELEASE", "RELEASE", reason="DRIVER_UNLOCK")),
          S(crash=0, fault=1, rl=1, rr=1, fire=1, ifl=1, ifr=1, speed=300.0, ign=0, cmd=C("ALL", "UNLOCK"),
            exp=E("LOCK", "LOCK", "FAULT", "SENSOR_FAULT")),
          S(cmd=C("ALL", "UNLOCK"), exp=E("RELEASE", "RELEASE", reason="DRIVER_UNLOCK")),
          S(speed=NAN, gear=ABSENT, age=NAN, crash=ABSENT, rl=ABSENT, rr=ABSENT, fire=ABSENT, ot=ABSENT, adult=ABSENT,
            ifl=ABSENT, ifr=ABSENT, ign=ABSENT, fault=ABSENT, cmd=(ABSENT, ABSENT, ABSENT),
            exp=E("LOCK", "LOCK", "FAULT", "SENSOR_FAULT", rej=1))],
         ["WoW(Worst-of-Worst)", "오류 추정", "고장 주입"], pre="정지, 직전 출력 RELEASE/RELEASE(단계 설정)")

# ---- PICT: 중재 조합(SSR-027, SSR-028, SSR-030)
model = """prev: RR, LL, LR, RL
crash: 0, 1, 2
fault: 0, 1
approach_left: 0, 1
fire: 0, 1
isofix_left: 0, 1
speed: 0.0, 3.0
ign: 1, 0
cmd: none, unlock, lock
"""
(OUT / "pict_model_arb.txt").write_text(model, encoding="utf-8")
pict_script = ROOT / ".claude" / "skills" / "sw-system-test-cases" / "scripts" / "run_pict.py"
res = subprocess.run([sys.executable, str(pict_script), str(OUT / "pict_model_arb.txt"), "--out",
                      str(OUT / "pict_result_arb.tsv"), "--order", "2"], capture_output=True, text=True)
if res.returncode != 0:
    sys.exit("PICT 실패: " + res.stdout + res.stderr)
pict_summary = json.loads(res.stdout)
rows = [l.split("\t") for l in (OUT / "pict_result_arb.tsv").read_text(encoding="utf-8").splitlines()]
hdr, rows = rows[0], rows[1:]

RANK = {"CRASH_CONFIRMED": 1, "SENSOR_FAULT": 2, "CRASH_PENDING_HOLD": 3, "APPROACH_RISK": 5, "FIRE": 6, "ISOFIX": 7,
        "SPEED_AUTO_LOCK": 8, "UNLOCK_DENIED_MOVING": 8, "IGNITION_OFF": 9, "DRIVER_LOCK": 10, "DRIVER_UNLOCK": 10,
        "NONE": 99}
ORDER = ["NONE", "CRASH_CONFIRMED", "SENSOR_FAULT", "CRASH_PENDING_HOLD", "CRASH_PENDING_TIMEOUT", "INPUT_STALE",
         "INPUT_INVALID", "FIRE", "OVERTEMP", "ADULT_PRESENT", "APPROACH_RISK", "OVERRIDE_ACK", "ISOFIX",
         "IGNITION_OFF", "SPEED_AUTO_LOCK", "UNLOCK_DENIED_MOVING", "DRIVER_LOCK", "DRIVER_UNLOCK",
         "COMMAND_REJECTED"]


def oracle(row):
    """SSR-027 우선순위 표(P1~P11)와 SSR-030 state 조건표에서 직접 도출한 기대값. 구현 코드와 무관."""
    prev = {"L": "LOCK" if row["prev"][0] == "L" else "RELEASE", "R": "LOCK" if row["prev"][1] == "L" else "RELEASE"}
    crash, fault, ap_l = int(row["crash"]), int(row["fault"]), int(row["approach_left"])
    fire, if_l = int(row["fire"]), int(row["isofix_left"])
    speed, ign, cmd = float(row["speed"]), int(row["ign"]), row["cmd"]
    out, reasons = {}, {}
    if crash == 2:
        out = {"L": "RELEASE", "R": "RELEASE"}
        reason = "CRASH_CONFIRMED"
    else:
        hold = "SENSOR_FAULT" if fault else ("CRASH_PENDING_HOLD" if crash == 1 else None)
        for d in ("L", "R"):
            ap = ap_l if d == "L" else 0
            isf = if_l if d == "L" else 0
            if hold:
                out[d] = "LOCK" if ap else prev[d]
                reasons[d] = hold
            elif ap:
                out[d], reasons[d] = "LOCK", "APPROACH_RISK"
            elif fire:
                out[d], reasons[d] = "RELEASE", "FIRE"
            elif isf:
                out[d], reasons[d] = "LOCK", "ISOFIX"
            elif speed >= 3.0:
                out[d] = "LOCK"
                reasons[d] = "UNLOCK_DENIED_MOVING" if cmd == "unlock" else "SPEED_AUTO_LOCK"
            elif ign == 0:
                out[d], reasons[d] = "RELEASE", "IGNITION_OFF"
            elif cmd != "none":
                out[d] = "LOCK" if cmd == "lock" else "RELEASE"
                reasons[d] = "DRIVER_LOCK" if cmd == "lock" else "DRIVER_UNLOCK"
            else:
                out[d], reasons[d] = prev[d], "NONE"
        a, b = reasons["L"], reasons["R"]
        ra, rb = RANK[a], RANK[b]
        reason = b if (rb < ra or (rb == ra and ORDER.index(b) < ORDER.index(a))) else a
    state = "FAULT" if fault else ("OFF" if ign == 0 else "NORMAL")
    return E(out["L"], out["R"], state, reason)


pict_tcs = []
for n, vals in enumerate(rows, start=1):
    row = dict(zip(hdr, vals))
    steps = [S(cmd=C("ALL", "UNLOCK"), exp=E("RELEASE", "RELEASE", reason="DRIVER_UNLOCK"))]
    prev = row["prev"]
    if prev[0] == "L":
        steps.append(S(cmd=C("LEFT", "LOCK"), exp=E("LOCK", "RELEASE", reason="DRIVER_LOCK")))
    if prev[1] == "L":
        l_now = "LOCK" if prev[0] == "L" else "RELEASE"
        steps.append(S(cmd=C("RIGHT", "LOCK"), exp=E(l_now, "LOCK", reason="DRIVER_LOCK")))
    cmd = {"none": NOCMD, "unlock": C("ALL", "UNLOCK"), "lock": C("ALL", "LOCK")}[row["cmd"]]
    steps.append(S(crash=int(row["crash"]), fault=int(row["fault"]), rl=int(row["approach_left"]),
                   fire=int(row["fire"]), ifl=int(row["isofix_left"]), speed=float(row["speed"]),
                   ign=int(row["ign"]), cmd=cmd, exp=oracle(row)))
    tc_id = f"TC-F-013-{n:02d}"
    desc = ", ".join(f"{k}={v}" for k, v in row.items())
    add_case(tc_id, ["SSR-027", "SSR-028", "SSR-030", "SRQ-FUN-021"], "functional",
             f"중재 우선순위 페어와이즈 조합 #{n}: {desc}", steps, ["페어와이즈(PICT)", "요구사항 기반"],
             pre="직전 출력 설정 단계 후(prev=" + row["prev"] + ")", priority="High")
    pict_tcs.append(tc_id)

# --------------------------------------------------------------------------- 수작업 시험(특수) — tests/system/test_system_special.cpp
def add_special(tc_id, basis, category, purpose, pre, inputs, procedure, expected, passc, techniques, env=ENV,
                priority="High", remarks=""):
    special.append(dict(id=tc_id, basis=basis, category=category, purpose=purpose, pre=pre, inputs=inputs,
                        procedure=procedure, expected=expected, passc=passc, techniques=techniques, env=env,
                        priority=priority, remarks=remarks))


add_special("TC-F-014", ["SRQ-FUN-022", "SRQ-FUN-023", "SRQ-FUN-029"], "functional",
            "상태 조회/직렬화에 좌·우 출력, state, 입력별 validity, 이유코드, override, command_rejected가 모두 포함되고 직렬화 버퍼 경계(필요 길이+1 성공, 길이 부족 RET_ERR_SERIALIZE)를 확인한다",
            "init 후 정상 평가 1회",
            "상태 직렬화 버퍼 capacity = 필요 길이+1, 필요 길이, 4 바이트; 이벤트 조회",
            "1. 정상 입력을 평가한다.\n2. ecl_core_serialize_status를 세 가지 capacity로 호출한다.\n3. 결과 문자열에서 필드명을 찾는다.\n4. ecl_core_query_events로 이벤트 필드를 읽는다.",
            "문자열에 lock_left, lock_right, state, reason_code, override_left, override_right, command_rejected, validity_<신호 13종> 포함. capacity 필요 길이+1: RET_OK, 필요 길이: RET_ERR_SERIALIZE(버퍼 불변), 4: RET_ERR_SERIALIZE. 이벤트에 event_id, timestamp_s, lock, state, reason_code, command_rejected, validity 필드가 존재",
            "필드 누락 0건, 반환 코드 일치", ["경계값 분석", "동치 분할", "요구사항 기반"])
add_special("TC-F-015", ["SRQ-FUN-027", "SRQ-FUN-028", "SRQ-FUN-030", "SRQ-LIM-001", "SRQ-LIM-003"], "functional",
            "이벤트 이력: 변화 평가마다 정확히 1건, ID 1부터 연속 증가, 101건 기록 후 최신 100건(ID 2~101)이 오름차순으로 조회되고 프로세스 재기동(재초기화) 후 0건이다",
            "init 직후",
            "lock/unlock 명령을 번갈아 99, 100, 101회; 변화 없는 평가 3회; 재초기화",
            "1. 99/100/101회 변화 후 각각 query한다.\n2. 변화 없는 평가를 3회 수행하고 건수를 확인한다.\n3. ecl_core_init로 재초기화 후 query한다.",
            "99회: 건수 99(첫 평가가 OFF→NORMAL 전이를 포함하면 +1), 101회: 100건(첫 ID 2 이상, 마지막 ID가 마지막 이벤트), 오름차순, 변화 없는 평가는 건수 불변, 재초기화 후 0건",
            "건수·ID 일치", ["경계값 분석", "요구사항 기반"])
add_special("TC-F-016", ["SRQ-FUN-037", "SRQ-FUN-039"], "functional",
            "시험 빌드의 카운터 설정 접근자로 이벤트 ID를 최댓값 직전으로 설정하면 ID 2^64−1인 이벤트 1건만 기록되고 이후 이벤트는 기록되지 않으며 제어는 계속된다",
            "시험 빌드(ECL_TEST_BUILD), init 직후",
            "카운터 = 2^64−2 설정 후 변화 평가 2회",
            "1. ecl_log_test_set_counter(&core.log, UINT64_MAX-1)\n2. 변화 평가 2회\n3. query_events",
            "이벤트 1건(ID=2^64−1), 두 번째 변화 평가의 status는 갱신됨, 두 번째 이벤트는 없음",
            "건수 1, 출력 갱신", ["경계값 분석", "요구사항 기반"], priority="Medium")
add_special("TC-NF-001", ["SRQ-QUA-FUN-001"], "non-functional",
            "시험 벡터 V를 고정 시각으로 1,000회 재생(매번 init)했을 때 평가마다 상태 응답 직렬화를 이어 붙인 바이트열의 SHA-256이 모두 같다",
            "docs/test-vectors/vector-v.csv(23행), 평가 간격 0.010 s",
            "벡터 V 23행 × 1,000회 재생",
            "1. CSV를 읽는다.\n2. 1,000회 반복: init → 행마다 evaluate → serialize → SHA-256 누적.\n3. 해시를 비교한다.",
            "1,000개 해시가 모두 동일",
            "해시 불일치 0건", ["요구사항 기반", "오류 추정"])
add_special("TC-NF-002", ["SRQ-LIM-002", "SRQ-LIM-004"], "non-functional",
            "시험 벡터 V의 평가 호출 간격이 0.010 s이고(호출 계약), 평가 1회의 최악 소요 시간이 0.010 s 미만이다(PC 벽시계, 벡터 10,000회 반복 중 최댓값)",
            "docs/test-vectors/vector-v.csv, 단일 스레드, 최적화 -O0",
            "벡터 V 23행 × 10,000회, 호출 간격 검사",
            "1. CSV의 연속 행 시각 차이를 확인한다.\n2. std::chrono::steady_clock으로 evaluate 1회 시간을 측정하고 최댓값을 구한다.",
            "모든 시각 차이 = 0.010 s(±1e-9), 최악 평가 시간 < 0.010 s",
            "최댓값 < 10 ms", ["경계값 분석", "요구사항 기반"], priority="Medium")
add_special("TC-NF-003", ["SRQ-QUA-REL-001"], "non-functional",
            "공개 함수(ecl_core_init/evaluate/tick/get_status/serialize_status/query_events) 각각에 NULL 포인터 인자를 하나씩 주면 RET_ERR_NULL(-1)을 반환하고 출력 인자를 바꾸지 않는다",
            "init 후",
            "각 함수의 포인터 인자를 하나씩 nullptr",
            "1. 출력 버퍼를 센티널로 채운다.\n2. 각 함수를 NULL 인자로 호출한다.\n3. 반환값과 버퍼를 확인한다.",
            "모든 호출이 -1 반환, 출력 인자 불변", "예외 0건", ["동치 분할", "오류 추정"], priority="Medium")
DEFERRED_TO_ITERATION_3 = ["SRQ-FUN-024", "SRQ-FUN-025", "SRQ-CON-008", "SRQ-CON-009"]  # 표시 계층(CMP-WEB, OI-017): 이터레이션 3에서 설계·시험

# ---- 검사(INS/BLD) 케이스: ci/inspection.sh 가 자동 실행한다(TC-I-xxx)
INSPECT = [
    ("TC-I-001", ["SRQ-CON-001"], "제품 코어가 C11이다: src 아래 C 소스만 있고 -std=c11 -Wall -Wextra -Wpedantic -Werror 컴파일 경고가 0이다", "[PASS] SRQ-CON-001 출력"),
    ("TC-I-002", ["SRQ-CON-002", "SRQ-CON-005"], "SW가 시스템 시계를 읽지 않는다: src/include에 time, clock, gettimeofday, clock_gettime, timespec_get, localtime, gmtime, difftime 호출이 없다", "[PASS] SRQ-CON-002, SRQ-CON-005 출력"),
    ("TC-I-003", ["SRQ-CON-003"], "입력 구조체에 영상·음성·개인 식별자 필드가 없다", "[PASS] SRQ-CON-003 출력"),
    ("TC-I-004", ["SRQ-CON-006"], "저장 구조체(ecl_event_t)의 필드가 event_id, timestamp_s, lock, state, reason_code, command_rejected, validity뿐이다", "[PASS] SRQ-CON-006 출력"),
    ("TC-I-005", ["SRQ-CON-007"], "성인 존재가 boolean(int) 단일 필드 adult_present로만 정의된다", "[PASS] SRQ-CON-007 출력"),
    ("TC-I-006", ["SRQ-CON-004"], "SW가 도어 래치/구동기 HW를 직접 구동하지 않는다: 장치·시스템 헤더, 인라인 어셈블리, volatile 접근이 없다", "[PASS] SRQ-CON-004 출력"),
    ("TC-I-007", ["SRQ-FUN-039"], "시험용 접근자가 제품 빌드(ECL_TEST_BUILD 미정의)의 목적 파일에 없다: nm 심볼 목록에 ecl_log_test_set_counter, ecl_core_test_get_log가 없다", "[PASS] SRQ-FUN-039 출력"),
]
for tc_id, basis, purpose, expect in INSPECT:
    add_special(tc_id, basis, "non-functional", purpose, "소스 트리와 claudeclangbuild 이미지",
                "대상: src/*.c, include/*.h (제품 빌드)", "1. 이미지 안에서 ci/inspection.sh를 실행한다.\n2. build/reports/inspect/inspect.txt를 확인한다.",
                expect, "스크립트 종료 코드 0, 해당 항목 [PASS]", ["오류 추정", "요구사항 기반"], env="검사(INS) — ci/inspection.sh (이미지)",
                priority="Medium")

# --------------------------------------------------------------------------- 요구사항 목록과 설계
reqs_text = load_requirements()
INS_ONLY = {rid: "표시 계층(CMP-WEB)은 이터레이션 3 범위(OI-017). 설계(아키텍처 §4.10)만 완료하고 시험 케이스는 이터레이션 3에서 작성" for rid in DEFERRED_TO_ITERATION_3}
tc_map = {}
for c in cases:
    for b in c["basis"]:
        tc_map.setdefault(b, []).append(c["id"])
for c in special:
    for b in c["basis"]:
        tc_map.setdefault(b, []).append(c["id"])
# PICT 케이스는 SSR-027 중재 쌍 단위 설계 항목으로 분리 추적
for tc in pict_tcs:
    pass

requirements = []
open_issues = []
design = []
RANGE_REQS = {"SRQ-FUN-004", "SRQ-FUN-009", "SRQ-FUN-017", "SRQ-FUN-020", "SRQ-FUN-034", "SRQ-FUN-035", "SSR-004",
              "SSR-005", "SSR-007", "SSR-008", "SSR-031", "SRQ-LIM-001", "SRQ-LIM-002", "SRQ-LIM-003", "SRQ-LIM-004",
              "SRQ-FUN-030", "SRQ-FUN-037", "SSR-003", "SSR-036", "SRQ-FUN-028"}
NOINPUT = {"SRQ-QUA-REL-001": True}
oi = 0
for rid, text in reqs_text.items():
    category = "functional" if (rid.startswith("SSR") or rid.startswith("SRQ-FUN")) else "non-functional"
    testable = rid not in INS_ONLY
    tcs = tc_map.get(rid, [])
    has_in = testable and not rid.startswith("SRQ-LIM") and rid not in ("SRQ-QUA-FUN-001",)
    has_rng = testable and rid in RANGE_REQS
    title = text[:50]
    requirements.append(dict(id=rid, title=title, text=text, category=category, asil="B" if rid.startswith("SSR") else "QM",
                             testable=testable, has_inputs=has_in, has_ranges=has_rng))
    if not testable:
        oi += 1
        open_issues.append(dict(id=f"Q-{oi:02d}", basis_id=rid, question=INS_ONLY[rid],
                                impact="시스템 테스트 케이스를 만들지 않음. 해당 검증 방법(INS/BLD)으로 별도 확인"))
        continue
    if not tcs:
        sys.exit(f"요구사항 {rid}에 테스트 케이스가 없음")
    if has_in:
        design.append(dict(id=f"EP-{rid}", basis_id=rid, technique="동치 분할", item="유효/무효 입력 클래스와 상태 클래스",
                           values="유효: 정상 입력 및 요구사항이 정한 조건 성립 / 무효: 범위 밖·누락 입력, 조건 미성립(TC 입력 값 참조)",
                           covered_by=tcs, applicable=True, note=""))
    if has_rng:
        design.append(dict(id=f"BV-{rid}", basis_id=rid, technique="경계값 분석", item="수치·시간·건수 경계",
                           values="경계 직전/경계/경계 직후(속도 2.9/3.0, 시간 0.200/0.210·1.000/1.010·10.00/10.01, 건수 99/100/101 등 TC 입력 값 참조)",
                           covered_by=tcs, applicable=True, note=""))
    elif has_in:
        design.append(dict(id=f"BV-NA-{rid}", basis_id=rid, technique="경계값 분석", item="적용 검토", values="",
                           covered_by=[], applicable=False,
                           note="입력이 boolean·열거형·상태 조건이라 순서 있는 수치·시간·건수 범위가 없다. 경계는 열거 범위 밖 값(동치 분할의 무효 클래스)으로 다룬다."))
# 페어와이즈 적용 검토 기록(미적용 요구사항: 사유 기록)
PW_NOTES = {
    "SRQ-FUN-001": "독립 파라미터 3개(source 4 × side 3 × action 2)이나 전 조합이 24개뿐이라 TC-F-001/002에서 전수 열거한다(전수는 페어와이즈보다 엄격). 조합 수가 작아 PICT를 쓰지 않는다.",
    "SRQ-FUN-010": "독립 boolean 3개(fire, overtemp, adult)로 전 조합 8개 중 단독 3 + 이중 2를 TC-F-005에서 직접 열거했고 우선순위 상호작용은 SSR-027 PICT에서 다룬다.",
    "SRQ-FUN-012": "isofix 좌우 2개 파라미터로 전 조합 4개를 TC-F-006에서 열거한다(2개 이하).",
}
for r in requirements:
    if r["testable"] and r["id"] != "SSR-027":
        design.append(dict(id=f"PW-NA-{r['id']}", basis_id=r["id"], technique="페어와이즈(PICT)", item="적용 검토",
                           values="", covered_by=[], applicable=False,
                           note=PW_NOTES.get(r["id"], "이 요구사항 자체의 독립 입력 파라미터는 1~2개이거나 단일 조건이다(전수 시험 가능). 다른 입력과의 상호작용은 SSR-027 PICT 모델(pict_model_arb.txt)에서 다룬다.")))
# 페어와이즈 설계 항목(PICT)
for tc in pict_tcs:
    design.append(dict(id=f"PW-SSR-027-{tc[-2:]}", basis_id="SSR-027", technique="페어와이즈(PICT)",
                       item="중재 조합 " + tc, values="pict_result_arb.tsv의 해당 행", covered_by=[tc], applicable=True, note=""))
design.append(dict(id="WOW-SSR-001", basis_id="SSR-001", technique="WoW(Worst-of-Worst)", item="최악 값 동시 주입",
                   values="TC-F-019 W1~W3", covered_by=["TC-F-019"], applicable=True, note=""))

test_cases = []
for c in cases:
    inputs = "\n".join(describe_step(i + 1, s) for i, s in enumerate(c["steps"]))
    expected = "\n".join(x for x in (describe_exp(i + 1, s["exp"]) for i, s in enumerate(c["steps"])) if x)
    if c["initial"] is not None:
        expected = "초기: " + describe_exp(0, c["initial"])[4:] + "\n" + expected
    proc = ("1. ecl_core_init로 초기화한다.\n2. 각 단계(S1..Sn)의 입력으로 ecl_core_evaluate(또는 tick)를 호출한다(시각은 단계별 t, 생략 시 직전+0.010 s).\n"
            "3. 각 호출 직후 ecl_core_get_status 값을 기대값과 비교한다.")
    test_cases.append(dict(id=c["id"], basis_ids=c["basis"], category=c["category"], source="basis", proposed=False,
                           purpose=c["purpose"], preconditions=c["pre"], inputs=inputs, procedure=proc, expected=expected,
                           pass_criteria="모든 단계의 lock_left, lock_right, state, reason_code(및 지정 시 command_rejected/override)가 기대값과 일치",
                           environment=c["env"], techniques=c["techniques"], priority=c["priority"], remarks=c["remarks"]))
for c in special:
    test_cases.append(dict(id=c["id"], basis_ids=c["basis"], category=c["category"], source="basis", proposed=False,
                           purpose=c["purpose"], preconditions=c["pre"], inputs=c["inputs"], procedure=c["procedure"],
                           expected=c["expected"], pass_criteria=c["passc"], environment=c["env"], techniques=c["techniques"],
                           priority=c["priority"], remarks=c["remarks"]))

doc = dict(title="전자식 차일드락 제어 SW 시스템 테스트 명세 (SWE.6)", basis_document="docs/SRS.md v0.3, docs/safety-requirements.md v0.3 (SRS-ECL-001, SSR-ECL-001), OEM-SWR-001 Rev 1.0 입력",
           requirements=requirements, test_cases=test_cases, design=design,
           pict_models=[dict(basis_id="SSR-027", model_file="pict_model_arb.txt", result_file="pict_result_arb.tsv",
                             order=2, rows=len(rows))],
           open_issues=open_issues)
(OUT / "system_tests.json").write_text(json.dumps(doc, ensure_ascii=False, indent=2), encoding="utf-8")

# --------------------------------------------------------------------------- C++ 케이스 표
def cval(v, absent="ECL_ABSENT", nan="NAN"):
    if v == ABSENT:
        return absent
    if v == NAN:
        return nan
    if isinstance(v, float):
        return repr(v)
    return str(v)


def cxx_step(s, prev_t):
    t = s["t"] if s["t"] is not None else round(prev_t + 0.010, 6)
    if s["cmd"] is None:
        cs, ca, cx = "-2", "0", "0"
    else:
        side, act, src = s["cmd"]
        cs = cval(SIDE[side] if side in SIDE else side)
        ca = cval(ACT[act] if act in ACT else act)
        cx = cval(SRC[src] if src in SRC else src)
    age = "NAN" if s["age"] == NAN else repr(float(s["age"]))
    e = s["exp"]
    if e is None:
        ex = "0, 0, 0, 0, 0, 0, 0"
    else:
        ex = f"1, ECL_{e['L']}, ECL_{e['R']}, ECL_STATE_{e['state']}, RC_{e['reason']}, {e['rej']}, {e['ovl']}"
        ex += f", {e['ovr']}"
    if e is None:
        ex += ", 0"
    fields = [str(1 if s["tick"] else 0), repr(float(t)), cval(s["speed"]), cval(s["gear"]), age, cval(s["crash"]),
              cval(s["rl"]), cval(s["rr"]), cval(s["fire"]), cval(s["ot"]), cval(s["adult"]), cval(s["ifl"]),
              cval(s["ifr"]), cval(s["ign"]), cval(s["fault"]), cs, ca, cx]
    return "    {" + ", ".join(fields) + ", " + ex + "},", t


lines = ["// 자동 생성 파일: tools/gen_system_tests.py 가 docs/SRS.md, docs/safety-requirements.md, PICT 결과로부터 생성한다. 수정하지 말 것.",
         "// StStep 필드: tick,t,speed,gear,age,crash,rl,rr,fire,ot,adult,ifl,ifr,ign,fault,cmd_side,cmd_action,cmd_source,has_exp,L,R,state,reason,rej,ovl,ovr"]
names = []
for c in cases:
    var = "k" + c["id"].replace("-", "_")
    lines.append(f"static const StStep {var}[] = {{")
    t = 0.0
    for s in c["steps"]:
        text, t = cxx_step(s, t)
        lines.append(text)
    lines.append("};")
    init = c["initial"]
    if init is None:
        initv = "{0, ECL_RELEASE, ECL_RELEASE, ECL_STATE_OFF, RC_NONE, 0, 0, 0}"
    else:
        initv = f"{{1, ECL_{init['L']}, ECL_{init['R']}, ECL_STATE_{init['state']}, RC_{init['reason']}, 0, 0, 0}}"
    names.append(f'    {{"{c["id"]}", {var}, sizeof({var}) / sizeof({var}[0]), {initv}}},')
lines.append("static const StCase kCases[] = {")
lines.extend(names)
lines.append("};")
(ROOT / "tests" / "system" / "system_cases.inc").write_text("\n".join(lines) + "\n", encoding="utf-8")
print(json.dumps(dict(requirements=len(requirements), testable=sum(1 for r in requirements if r["testable"]),
                      cases=len(test_cases), auto_cases=len(cases), special=len(special), pict_rows=len(rows),
                      pict=pict_summary), ensure_ascii=False))
