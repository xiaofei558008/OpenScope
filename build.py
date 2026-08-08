#!/usr/bin/env python3
"""OpenScope 干净环境构建入口。

本机环境存在 PATH/Path 大小写重复，直接运行 build.bat 会触发
MSB6001（CL.exe 命令行开关无效）。此脚本构造只含单个 Path 键的
环境块后调用 build.bat，并把完整输出写入 build_last.log。

用法: python build.py [--quiet]
"""
import os
import subprocess
import sys


def clean_env():
    env = dict(os.environ)
    pathval = env.get("Path") or env.get("PATH") or ""
    for k in [k for k in env if k.lower() == "path"]:
        del env[k]
    env["Path"] = pathval
    return env


def main():
    quiet = "--quiet" in sys.argv[1:]
    root = os.path.dirname(os.path.abspath(__file__))
    build_bat = os.path.join(root, "build.bat")
    r = subprocess.run(
        ["cmd", "/c", build_bat],
        env=clean_env(),
        cwd=root,
        capture_output=True,
    )
    out = (r.stdout or b"") + (r.stderr or b"")
    with open(os.path.join(root, "build_last.log"), "wb") as f:
        f.write(out)
    if not quiet:
        print(out.decode("utf-8", errors="replace"))
    print(f"[build.py] rc={r.returncode} log=build_last.log")
    return r.returncode


if __name__ == "__main__":
    sys.exit(main())
