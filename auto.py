#!/usr/bin/env python3
"""OpenScope 一键构建+测试+打包。

步骤：
  1. 构建（build.py，生成 version.h + 编译）
  2. 单元测试（tests/build_tests.bat，编译并运行全部冒烟测试）
  3. 打包（packaging/make_setup.py，产出 dist/OpenScope-Setup-<版本>.exe）

用法：
  python auto.py                    一键完整流程
  python auto.py --skip-tests       跳过单元测试
  python auto.py --skip-package     跳过打包（仅构建+测试）
  python auto.py --publish          打包后自动发布到 www.opendebugger.com
  python auto.py --quiet            安静模式（不输出编译日志）
"""

import argparse
import os
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


def main():
    ap = argparse.ArgumentParser(
        description="OpenScope 一键构建+测试+打包",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python auto.py                        一键完整流程（构建→测试→打包）
  python auto.py --skip-tests           跳过单元测试
  python auto.py --skip-package         仅构建+测试，不打包
  python auto.py --publish --quiet      安静构建并自动发布
        """,
    )
    ap.add_argument("--skip-tests", action="store_true", help="跳过单元测试")
    ap.add_argument("--skip-package", action="store_true", help="跳过打包")
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
