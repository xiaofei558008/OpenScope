#!/usr/bin/env python3
"""OpenScope 一键构建+测试+打包。

步骤：
  0. 版本号增长（可选 --bump/--bump-minor/--bump-major）
  1. 构建（build.py，生成 version.h + 编译）
  2. 单元测试（tests/build_tests.bat，编译并运行全部冒烟测试）
  3. 打包（packaging/make_setup.py，产出 dist/OpenScope-Setup-<版本>.exe）

用法：
  python auto.py                    一键完整流程
  python auto.py --bump             补丁号 +1（1.17.0 → 1.17.1）后完整流程
  python auto.py --bump-minor       次版本号 +1（1.17.0 → 1.18.0）
  python auto.py --bump-major       主版本号 +1（1.17.0 → 2.0.0）
  python auto.py --skip-tests       跳过单元测试
  python auto.py --skip-package     跳过打包（仅构建+测试）
  python auto.py --publish          打包后自动发布到 www.opendebugger.com
  python auto.py --quiet            安静模式（不输出编译日志）
"""

import argparse
import os
import re
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.abspath(__file__))


def step(name, cmd, cwd=None):
    """运行一个步骤，打印耗时。失败抛出 SystemExit。"""
    t0 = time.time()
    print(f"\n{'='*60}")
    print(f"  {name}")
    print(f"{'='*60}")
    print(f"  $ {' '.join(cmd)}")
    r = subprocess.run(cmd, cwd=cwd or ROOT)
    dt = time.time() - t0
    if r.returncode != 0:
        print(f"\n[auto] FAIL ({dt:.1f}s) -> {name}")
        raise SystemExit(1)
    print(f"\n[auto] OK ({dt:.1f}s) -> {name}")
    return r


def bump_version(part):
    """版本号增长：以 code/src/version.rc 为唯一来源，同步三处：
      1. version.rc（FILEVERSION/PRODUCTVERSION 逗号形式 + FileVersion/ProductVersion 字符串）
      2. packaging/openscope.iss（AppVersion/AppVerName/OutputBaseFilename/
         VersionInfoVersion/VersionInfoProductVersion/VersionInfoOriginalFileName）
      3. module/jlink/jlink.c（模块版本串 "%s", "X.Y.Z"）
    part: 'major' | 'minor' | 'patch'。返回新版本号 "X.Y.Z"。"""
    rc_path = os.path.join(ROOT, "code", "src", "version.rc")
    iss_path = os.path.join(ROOT, "packaging", "openscope.iss")
    jl_path = os.path.join(ROOT, "module", "jlink", "jlink.c")
    rc = open(rc_path, encoding="utf-8").read()
    m = re.search(r"FILEVERSION\s+(\d+),(\d+),(\d+),(\d+)", rc)
    if not m:
        print("[auto] ERROR: version.rc 缺少 FILEVERSION")
        return None
    major, minor, patch, rev = map(int, m.groups())
    old_comma = "%d,%d,%d,%d" % (major, minor, patch, rev)
    old_dot = "%d.%d.%d.%d" % (major, minor, patch, rev)
    old_short = "%d.%d.%d" % (major, minor, patch)
    if part == "major":
        major += 1
        minor = 0
        patch = 0
    elif part == "minor":
        minor += 1
        patch = 0
    else:
        patch += 1
    new_comma = "%d,%d,%d,%d" % (major, minor, patch, rev)
    new_dot = "%d.%d.%d.%d" % (major, minor, patch, rev)
    new_short = "%d.%d.%d" % (major, minor, patch)
    # 1) version.rc
    rc2 = rc.replace("FILEVERSION %s" % old_comma, "FILEVERSION %s" % new_comma)
    rc2 = rc2.replace("PRODUCTVERSION %s" % old_comma, "PRODUCTVERSION %s" % new_comma)
    rc2 = rc2.replace('"FileVersion", "%s"' % old_dot, '"FileVersion", "%s"' % new_dot)
    rc2 = rc2.replace('"ProductVersion", "%s"' % old_dot, '"ProductVersion", "%s"' % new_dot)
    if rc2 == rc:
        print("[auto] ERROR: version.rc 中未找到旧版本串 %s" % old_dot)
        return None
    with open(rc_path, "w", encoding="utf-8", newline="\n") as f:
        f.write(rc2)
    # 2) openscope.iss
    iss = open(iss_path, encoding="utf-8").read()
    iss2 = iss.replace("AppVersion=%s" % old_dot, "AppVersion=%s" % new_dot)
    iss2 = iss2.replace("AppVerName=OpenScope %s" % old_short,
                        "AppVerName=OpenScope %s" % new_short)
    iss2 = iss2.replace("OutputBaseFilename=OpenScope-Setup-%s" % old_short,
                        "OutputBaseFilename=OpenScope-Setup-%s" % new_short)
    iss2 = iss2.replace("VersionInfoVersion=%s" % old_dot, "VersionInfoVersion=%s" % new_dot)
    iss2 = iss2.replace("VersionInfoProductVersion=%s" % old_dot,
                        "VersionInfoProductVersion=%s" % new_dot)
    iss2 = iss2.replace("VersionInfoOriginalFileName=OpenScope-Setup-%s.exe" % old_short,
                        "VersionInfoOriginalFileName=OpenScope-Setup-%s.exe" % new_short)
    if iss2 == iss:
        print("[auto] ERROR: openscope.iss 中未找到旧版本串 %s" % old_dot)
        return None
    with open(iss_path, "w", encoding="utf-8", newline="\n") as f:
        f.write(iss2)
    # 3) module/jlink/jlink.c（两处："%s", "1.17.0" 与模块表 "1.17.0"）
    jl = open(jl_path, encoding="utf-8").read()
    jl2 = jl.replace('"%s"' % old_short, '"%s"' % new_short)
    if jl2 == jl:
        print("[auto] WARNING: jlink.c 未找到模块版本串 %s（跳过，仅影响模块信息显示）"
              % old_short)
    else:
        with open(jl_path, "w", encoding="utf-8", newline="\n") as f:
            f.write(jl2)
    print("[auto] 版本号: %s -> %s（version.rc / openscope.iss / jlink.c 已同步）"
          % (old_short, new_short))
    return new_short


def main():
    ap = argparse.ArgumentParser(
        description="OpenScope 一键构建+测试+打包",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python auto.py                        一键完整流程（构建→测试→打包）
  python auto.py --bump                 补丁号 +1 后完整流程（打包新版本）
  python auto.py --bump-minor           次版本号 +1
  python auto.py --bump-major           主版本号 +1
  python auto.py --skip-tests           跳过单元测试
  python auto.py --skip-package         仅构建+测试，不打包
  python auto.py --publish --quiet      安静构建并自动发布
        """,
    )
    ap.add_argument("--skip-tests", action="store_true", help="跳过单元测试")
    ap.add_argument("--skip-package", action="store_true", help="跳过打包")
    ap.add_argument("--bump", action="store_true", help="补丁号 +1（1.17.0 → 1.17.1）")
    ap.add_argument("--bump-minor", action="store_true", help="次版本号 +1（1.17.0 → 1.18.0）")
    ap.add_argument("--bump-major", action="store_true", help="主版本号 +1（1.17.0 → 2.0.0）")
    ap.add_argument("--publish", action="store_true",
                    help="打包后自动发布到 www.opendebugger.com")
    ap.add_argument("--quiet", action="store_true", help="安静模式")
    args = ap.parse_args()

    # ---- 环境检查 ----
    build_py = os.path.join(ROOT, "build.py")
    build_bat = os.path.join(ROOT, "build.bat")
    test_bat = os.path.join(ROOT, "tests", "build_tests.bat")
    setup_py = os.path.join(ROOT, "packaging", "make_setup.py")

    for f, desc in [(build_py, "build.py"),
                     (build_bat, "build.bat"),
                     (setup_py, "packaging/make_setup.py")]:
        if not os.path.isfile(f):
            print(f"[auto] MISSING: {f}")
            return 2

    if not args.skip_tests and not os.path.isfile(test_bat):
        print(f"[auto] WARNING: tests/build_tests.bat not found, skipping")
        args.skip_tests = True

    # ---- 步骤 0：版本号增长（可选，构建前执行——version.h 随 build.py 重新生成） ----
    if args.bump or args.bump_minor or args.bump_major:
        part = "major" if args.bump_major else ("minor" if args.bump_minor else "patch")
        new_ver = bump_version(part)
        if not new_ver:
            return 2

    # ---- 步骤 1：构建 ----
    build_cmd = [sys.executable, build_py]
    if args.quiet:
        build_cmd.append("--quiet")
    step("1/3 构建（build.py）", build_cmd)

    # ---- 步骤 2：单元测试 ----
    if not args.skip_tests:
        # build_tests.bat 内部使用相对路径（tests\bin\、code\src\），必须从项目根运行
        step("2/3 单元测试（build_tests.bat）",
             ["cmd", "/c", test_bat],
             cwd=ROOT)
    else:
        print("\n  SKIP tests (--skip-tests)")

    # ---- 步骤 3：打包 ----
    if not args.skip_package:
        pkg_cmd = [sys.executable, setup_py]
        if args.publish:
            pkg_cmd.append("--publish")
        step("3/3 打包（make_setup.py）", pkg_cmd)
    else:
        print("\n  SKIP package (--skip-package)")

    # ---- 汇总 ----
    dist_dir = os.path.join(ROOT, "dist")
    setups = []
    if os.path.isdir(dist_dir):
        for fn in os.listdir(dist_dir):
            if fn.lower().endswith(".exe"):
                fp = os.path.join(dist_dir, fn)
                setups.append((fn, os.path.getsize(fp)))
    print(f"\n{'='*60}")
    print(f"  ALL DONE")
    if setups:
        for name, size in setups:
            print(f"  Setup: dist\\{name}  ({size:,} bytes)")
    print(f"{'='*60}\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
