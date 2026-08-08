#!/usr/bin/env python3
"""构建 OpenScope 安装包（request.md 第 8 条）。

流程：
  1. 从 code/src/version.rc 读取版本号（唯一来源，保证与 exe 版本一致）；
  2. 若 assets/openscope.ico 缺失则用 make_icon.py 生成；
  3. 调用本地 tools/innosetup/ISCC.exe 编译 packaging/openscope.iss；
  4. 产出 dist/OpenScope-Setup-<display>.exe。

用法：python packaging/make_setup.py [--version 1.0.0.0]
"""
import argparse
import os
import re
import subprocess
import sys


ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ISCC = os.path.join(ROOT, "tools", "innosetup", "ISCC.exe")
ISS = os.path.join(ROOT, "packaging", "openscope.iss")
VERSION_RC = os.path.join(ROOT, "code", "src", "version.rc")
ICON = os.path.join(ROOT, "assets", "openscope.ico")


def read_version():
    text = open(VERSION_RC, encoding="utf-8", errors="replace").read()
    m = re.search(r"FILEVERSION\s+(\d+),(\d+),(\d+),(\d+)", text)
    if not m:
        raise SystemExit("[make_setup] version.rc: FILEVERSION not found")
    return ".".join(m.groups())


def display_version(full):
    parts = full.split(".")
    # 仅去掉 revision 段（第 4 段）为 0 的情况：1.0.0.0 -> 1.0.0
    if len(parts) == 4 and parts[3] == "0":
        parts.pop()
    return ".".join(parts)


def ensure_icon():
    if os.path.isfile(ICON):
        return
    icon_py = os.path.join(ROOT, "packaging", "make_icon.py")
    r = subprocess.run([sys.executable, icon_py], cwd=ROOT)
    if r.returncode:
        raise SystemExit("[make_setup] icon generation failed")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--version", default=None, help="完整版本号，默认读取 version.rc")
    args = ap.parse_args()
    full = args.version or read_version()
    display = display_version(full)
    iss = open(ISS, encoding="utf-8", errors="replace").read()
    if f"VersionInfoVersion={full}" not in iss or f"OpenScope-Setup-{display}" not in iss:
        raise SystemExit(
            f"[make_setup] packaging/openscope.iss 版本与 version.rc({full}) 不一致，"
            "请同步 openscope.iss 后重试。"
        )
    if not os.path.isfile(ISCC):
        raise SystemExit(f"[make_setup] ISCC not found: {ISCC}\n"
                         "请先将 Inno Setup 安装到 tools\\innosetup\\（ISCC.exe 位于该目录）。")
    os.makedirs(os.path.join(ROOT, "dist"), exist_ok=True)
    ensure_icon()
    cmd = [ISCC, ISS]
    print(f"[make_setup] ISCC {full} -> dist/OpenScope-Setup-{display}.exe")
    r = subprocess.run(cmd, cwd=ROOT)
    if r.returncode:
        raise SystemExit(f"[make_setup] ISCC failed rc={r.returncode}")
    setup = os.path.join(ROOT, "dist", f"OpenScope-Setup-{display}.exe")
    if not os.path.isfile(setup):
        raise SystemExit(f"[make_setup] output missing: {setup}")
    size = os.path.getsize(setup)
    print(f"[make_setup] OK {os.path.relpath(setup, ROOT)} ({size} bytes)")


if __name__ == "__main__":
    main()
