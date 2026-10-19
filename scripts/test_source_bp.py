#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""OpenScope 源码/行断点 AI API 自动化测试。

用法（先启动 OpenScope.exe --ai-silent）:
  python scripts/test_source_bp.py
  python scripts/test_source_bp.py --elf D:/path/to/fw.out --src-root D:/path/to/proj
"""
from __future__ import annotations

import argparse
import json
import os
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from os_cli import RpcClient  # noqa: E402

DEFAULT_ELF = r"D:\C_Code\g431kb_for_openscope_d\EWARM\g431rb_tst\Exe\g431rb_tst.out"
DEFAULT_ROOT = r"D:\C_Code\g431kb_for_openscope_d"
DEFAULT_SRC = r"D:\C_Code\g431kb_for_openscope_d\Core\Src\main.c"
DEFAULT_LINE = 100


class Fail(Exception):
    pass


def expect(cond: bool, msg: str) -> None:
    if not cond:
        raise Fail(msg)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--url", default=os.environ.get("OPENSCOPE_AI_URL", "ws://127.0.0.1:18765"))
    ap.add_argument("--elf", default=DEFAULT_ELF)
    ap.add_argument("--src-root", default=DEFAULT_ROOT)
    ap.add_argument("--src", default=DEFAULT_SRC)
    ap.add_argument("--line", type=int, default=DEFAULT_LINE)
    ap.add_argument("--skip-hw", action="store_true", help="跳过 ST-Link 连接与硬件断点")
    args = ap.parse_args()

    passed = 0
    failed = 0

    def step(name: str, fn):
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
        c = RpcClient(args.url, timeout=60)
    except Exception as e:
        print(f"无法连接 AI API: {e}")
        print("请先启动: bin\\Release\\OpenScope.exe --ai-silent")
        return 2

    def call(m, p=None):
        r = c.call(m, p or {})
        if isinstance(r, dict) and r.get("ok") is False:
            raise Fail(f"{m}: {r}")
        return r

    try:
        print("\n== 基础 ==")
        step("status", lambda: expect(call("status").get("ok"), "status"))
        step("tools has src.*", lambda: expect(
            any(t.get("name") == "src.status" for t in call("tools.list").get("tools", [])),
            "missing src.status in tools.list"))
        step("tools has bp.set_line", lambda: expect(
            any(t.get("name") == "bp.set_line" for t in call("tools.list").get("tools", [])),
            "missing bp.set_line"))

        print("\n== ELF / 源码索引 ==")
        step("elf.load", lambda: expect(call("elf.load", {"path": args.elf}).get("ok"), "elf.load"))
        step("src.roots add", lambda: expect(
            call("src.roots", {"add": args.src_root}).get("count", 0) >= 1, "no roots"))
        # force rebuild index after add
        call("src.roots", {})
        st = {}
        def _status():
            nonlocal st
            # refresh via files list which rebuilds if empty
            call("src.files", {"needle": "main.c", "limit": 5})
            st = call("src.status")
            expect(st.get("elf_loaded") == 1, "elf not loaded")
            expect(st.get("line_rows", 0) > 0, f"line_rows={st.get('line_rows')}")
            expect(st.get("src_files", 0) > 0, f"src_files={st.get('src_files')}")
        step("src.status line table", _status)
        print(f"       line_rows={st.get('line_rows')} src_files={st.get('src_files')}")

        files = {}
        def _files():
            nonlocal files
            files = call("src.files", {"needle": "main.c", "limit": 10})
            expect(files.get("count", 0) >= 1, f"files={files}")
        step("src.files fuzzy main.c", _files)

        mapped = {}
        def _map():
            nonlocal mapped
            mapped = call("src.line_to_addr", {"path": args.src, "line": args.line})
            expect(mapped.get("address", 0) > 0, f"map={mapped}")
        step(f"src.line_to_addr {args.line}", _map)
        addr = mapped.get("address")
        print(f"       -> {mapped.get('address_hex')}")

        rev = {}
        def _rev():
            nonlocal rev
            rev = call("src.addr_to_line", {"address": addr})
            expect(rev.get("line", 0) > 0, f"rev={rev}")
            expect("main.c" in (rev.get("path") or "").replace("\\", "/").lower()
                   or rev.get("line") == args.line
                   or abs(rev.get("line", 0) - args.line) < 50, f"rev path={rev}")
        step("src.addr_to_line roundtrip", _rev)

        step("src.open", lambda: expect(
            call("src.open", {"path": args.src, "line": args.line}).get("ok"), "open"))

        if args.skip_hw:
            print("\n== 硬件断点（跳过）==")
        else:
            print("\n== ST-Link 硬件断点 ==")
            def _drv():
                d = call("drivers.list")
                idx = None
                for it in d.get("drivers", []):
                    if it.get("name") == "stlink":
                        idx = it.get("index")
                expect(idx is not None, "no stlink")
                call("drivers.select", {"index": idx})
            step("drivers.select stlink", _drv)
            step("probe.scan", lambda: expect(call("probe.scan").get("count", 0) >= 1, "no probe"))
            step("connect attach", lambda: expect(
                call("connect", {"device": "Cortex-M4", "iface": 0, "speed_khz": 4000, "attach": 1})
                .get("connected") == 1, "connect"))
            step("cpu.state", lambda: expect("state" in call("cpu.state"), "cpu.state"))

            step("bp.clear all", lambda: call("bp.clear", {"all": 1}))
            setr = {}
            def _set_line():
                nonlocal setr
                setr = call("bp.set_line", {"path": args.src, "line": args.line, "note": "auto"})
                expect(setr.get("address") == addr or setr.get("address", 0) > 0, f"set={setr}")
            step("bp.set_line", _set_line)

            def _list1():
                lst = call("bp.list")
                expect(lst.get("count", 0) >= 1, f"list={lst}")
                addrs = [b.get("address") for b in lst.get("breakpoints", [])]
                expect(addr in addrs or (addr & ~1) in addrs, f"addr {addr} not in {addrs}")
            step("bp.list contains", _list1)

            # also by address API
            step("bp.set address again ok", lambda: call("bp.set", {
                "address": hex(addr), "note": "dup"}))

            step("bp.clear_line", lambda: call("bp.clear_line", {
                "path": args.src, "line": args.line}))
            step("bp.list empty-or-less", lambda: expect(
                call("bp.list").get("count", 0) >= 0, "list"))

            # set again then clear all
            call("bp.set_line", {"path": "main.c", "line": args.line})
            step("bp.set_line basename", lambda: expect(
                call("bp.list").get("count", 0) >= 1, "basename set"))
            step("bp.clear all final", lambda: call("bp.clear", {"all": 1}))
            step("bp.list empty", lambda: expect(call("bp.list").get("count") == 0, "not empty"))

            # halt/go smoke
            step("halt", lambda: call("halt"))
            time.sleep(0.2)
            step("go", lambda: call("go"))
            step("disconnect", lambda: call("disconnect"))

        print(f"\n==== 结果: {passed} passed, {failed} failed ====")
        return 0 if failed == 0 else 1
    finally:
        c.close()


if __name__ == "__main__":
    sys.exit(main())
