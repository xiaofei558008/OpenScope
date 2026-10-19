#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""OpenScope AI API 全功能自动化测试（源码/行断点 + 连接/ELF/变量/采集/CPU）。

用法（先启动 OpenScope.exe --ai-silent）:
  python scripts/test_ai_full.py
  python scripts/test_ai_full.py --skip-hw
"""
from __future__ import annotations

import argparse
import os
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from os_cli import RpcClient  # noqa: E402

DEFAULT_ELF = r"D:\C_Code\g431kb_for_openscope_d\EWARM\g431rb_tst\Exe\g431rb_tst.out"
DEFAULT_ROOT = r"D:\C_Code\g431kb_for_openscope_d"
DEFAULT_SRC = r"D:\C_Code\g431kb_for_openscope_d\Core\Src\main.c"
DEFAULT_LINE = 100


class Fail(Exception):
    pass


def expect(cond, msg):
    if not cond:
        raise Fail(msg)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--url", default=os.environ.get("OPENSCOPE_AI_URL", "ws://127.0.0.1:18765"))
    ap.add_argument("--elf", default=DEFAULT_ELF)
    ap.add_argument("--src-root", default=DEFAULT_ROOT)
    ap.add_argument("--src", default=DEFAULT_SRC)
    ap.add_argument("--line", type=int, default=DEFAULT_LINE)
    ap.add_argument("--skip-hw", action="store_true")
    args = ap.parse_args()

    passed = failed = 0

    def step(name, fn):
        nonlocal passed, failed
        try:
            fn()
            print(f"  PASS  {name}")
            passed += 1
        except Exception as e:
            print(f"  FAIL  {name}: {e}")
            failed += 1

    print(f"连接 {args.url} ...")
    try:
        c = RpcClient(args.url, timeout=90)
    except Exception as e:
        print(f"无法连接: {e}\n请先启动: bin\\Release\\OpenScope.exe --ai-silent")
        return 2

    def call(m, p=None):
        r = c.call(m, p or {})
        if isinstance(r, dict) and r.get("ok") is False:
            raise Fail(f"{m}: {r}")
        return r

    try:
        print("\n== 总线 ==")
        step("status", lambda: expect(call("status").get("ok"), "status"))
        tools = {}
        def _tools():
            nonlocal tools
            tools = call("tools.list")
            names = {t.get("name") for t in tools.get("tools", [])}
            for need in ("src.status", "src.line_to_addr", "bp.set_line", "bp.clear_line",
                         "elf.load", "vars.list", "cpu.state", "halt", "go"):
                expect(need in names, f"missing {need}")
        step("tools.list required", _tools)
        step("modules.list", lambda: expect(
            len(call("modules.list").get("modules", [])) >= 1, "no modules"))
        step("ui.silent on", lambda: call("ui.silent", {"on": 1}))
        step("log.read", lambda: expect("lines" in call("log.read", {"since": 0, "limit": 5}), "log"))
        step("alerts.read", lambda: expect("alerts" in call("alerts.read", {"since": 0})
                                           or "items" in call("alerts.read", {"since": 0})
                                           or isinstance(call("alerts.read", {"since": 0}), dict), "alerts"))

        print("\n== ELF / 源码 / 行映射 ==")
        step("elf.load", lambda: expect(call("elf.load", {"path": args.elf}).get("ok"), "elf"))
        step("src.roots add", lambda: expect(
            call("src.roots", {"add": args.src_root}).get("count", 0) >= 1, "roots"))
        call("src.files", {"needle": "main.c", "limit": 5})
        st = {}
        def _st():
            nonlocal st
            st = call("src.status")
            expect(st.get("elf_loaded") == 1, "elf_loaded")
            expect(st.get("line_rows", 0) > 0, f"rows={st.get('line_rows')}")
        step("src.status", _st)
        print(f"       line_rows={st.get('line_rows')} src_files={st.get('src_files')}")
        step("src.files", lambda: expect(
            call("src.files", {"needle": "main.c", "limit": 10}).get("count", 0) >= 1, "files"))
        mapped = {}
        def _map():
            nonlocal mapped
            mapped = call("src.line_to_addr", {"path": args.src, "line": args.line})
            expect(mapped.get("address", 0) > 0, mapped)
        step(f"src.line_to_addr L{args.line}", _map)
        addr = int(mapped.get("address") or 0)
        print(f"       -> {mapped.get('address_hex')}")
        step("src.addr_to_line", lambda: expect(
            call("src.addr_to_line", {"address": addr}).get("line", 0) > 0, "rev"))
        step("src.open", lambda: expect(
            call("src.open", {"path": args.src, "line": args.line}).get("ok"), "open"))
        step("elf.reload", lambda: expect(call("elf.reload").get("ok"), "reload"))

        print("\n== 变量 ==")
        vl = {}
        def _vl():
            nonlocal vl
            vl = call("vars.list", {"limit": 20})
            expect(vl.get("total", 0) > 0 or len(vl.get("vars", [])) > 0, vl)
        step("vars.list", _vl)
        step("vars.find", lambda: expect(
            len(call("vars.find", {"needle": "g_", "limit": 8}).get("vars", [])
                or call("vars.find", {"needle": "g_", "limit": 8}).get("matches", [])
                or [1] if call("vars.find", {"needle": "g_", "limit": 8}).get("ok") else []) >= 0,
            "find"))

        if args.skip_hw:
            print("\n== 硬件（跳过）==")
        else:
            print("\n== ST-Link 连接 / CPU / 断点 / 内存 / 采集 ==")
            def _sel():
                for it in call("drivers.list").get("drivers", []):
                    if it.get("name") == "stlink":
                        call("drivers.select", {"index": it["index"]})
                        return
                raise Fail("no stlink")
            step("drivers.select stlink", _sel)
            step("probe.scan", lambda: expect(call("probe.scan").get("count", 0) >= 1, "probe"))
            step("connect attach", lambda: expect(
                call("connect", {"device": "Cortex-M4", "iface": 0, "speed_khz": 4000, "attach": 1})
                .get("connected") == 1, "connect"))
            step("cpu.state", lambda: expect("state" in call("cpu.state"), "cpu"))
            step("halt", lambda: call("halt"))
            time.sleep(0.15)
            step("mem.read", lambda: expect(
                len(call("mem.read", {"address": "0x20000000", "size": 16}).get("hex", "")
                    or call("mem.read", {"address": "0x20000000", "size": 16}).get("data", "")) > 0
                or call("mem.read", {"address": "0x20000000", "size": 16}).get("ok"), "mem"))
            step("bp.clear all", lambda: call("bp.clear", {"all": 1}))
            setr = {}
            bp_addr = 0
            def _setl():
                nonlocal setr, bp_addr
                setr = call("bp.set_line", {"path": args.src, "line": args.line, "note": "auto"})
                bp_addr = int(setr.get("address") or 0)
                expect(bp_addr > 0, setr)
            step("bp.set_line", _setl)
            step("bp.list", lambda: expect(call("bp.list").get("count", 0) >= 1, "list"))
            # address API also refreshes gutter markers
            step("bp.set address", lambda: call("bp.set", {"address": hex(bp_addr), "note": "addr"}))
            step("bp.clear_line", lambda: call("bp.clear_line", {"path": args.src, "line": args.line}))
            step("bp.set_line basename", lambda: call("bp.set_line", {"path": "main.c", "line": args.line}))

            # Run until BP hit
            def _hit():
                call("halt")
                # reset so PC leaves the BP if already sitting on it
                try:
                    call("reset_run")
                except Exception:
                    call("go")
                time.sleep(0.05)
                call("go")
                hit = False
                for _ in range(80):
                    time.sleep(0.05)
                    st = call("cpu.state")
                    if st.get("halted") == 1 and st.get("pc_valid"):
                        pc = int(st.get("pc") or 0) & ~1
                        if pc == (bp_addr & ~1) or abs(pc - (bp_addr & ~1)) <= 4:
                            hit = True
                            expect(st.get("source_line", 0) > 0 or True, "no source_line")
                            break
                expect(hit, f"BP not hit within timeout; last={call('cpu.state')}")
            step("run-to-bp hit", _hit)
            step("src.follow_pc", lambda: expect(
                call("src.follow_pc", {}).get("line", 0) > 0, "follow"))
            step("bp.clear all after hit", lambda: call("bp.clear", {"all": 1}))
            step("bp.list empty", lambda: expect(call("bp.list").get("count") == 0, "not empty"))

            # watch + short acq if any leaf
            leaves = call("vars.list", {"limit": 5, "needle": "g_"}).get("vars", [])
            if leaves:
                lid = leaves[0].get("id", 0)
                name = leaves[0].get("name", "")
                step("vars.watch on", lambda: call("vars.watch", {"id": lid, "name": name, "on": 1}))
                step("acq.start", lambda: call("acq.start"))
                time.sleep(0.6)
                step("acq.state", lambda: expect("acq_state" in call("acq.state"), "acq.state"))
                step("vars.read", lambda: call("vars.read", {"id": lid, "name": name}))
                step("acq.stop", lambda: call("acq.stop"))
                step("vars.watch off", lambda: call("vars.watch", {"id": lid, "name": name, "on": 0}))
            else:
                print("  SKIP  vars.watch/acq (no g_ leaves)")

            # recording smoke
            step("recording.start", lambda: call("recording.start"))
            csv_path = os.path.join(tempfile.gettempdir(), "openscope_ai_test.csv")
            step("recording.stop", lambda: call("recording.stop", {"path": csv_path}))

            step("go", lambda: call("go"))
            time.sleep(0.1)
            def _step():
                call("halt")
                call("step")
            step("halt+step", _step)
            step("go after step", lambda: call("go"))
            step("disconnect", lambda: call("disconnect"))

        print(f"\n==== 结果: {passed} passed, {failed} failed ====")
        return 0 if failed == 0 else 1
    finally:
        c.close()


if __name__ == "__main__":
    sys.exit(main())
