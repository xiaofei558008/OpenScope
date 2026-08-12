#!/usr/bin/env python3
"""OpenScope 干净环境构建入口。

本机环境存在 PATH/Path 大小写重复，直接运行 build.bat 会触发
MSB6001（CL.exe 命令行开关无效）。此脚本构造只含单个 Path 键的
环境块后调用 build.bat，并把完整输出写入 build_last.log。

用法: python build.py [--quiet]
"""
import os
import re
import subprocess
import sys


def clean_env():
    env = dict(os.environ)
    pathval = env.get("Path") or env.get("PATH") or ""
    for k in [k for k in env if k.lower() == "path"]:
        del env[k]
    env["Path"] = pathval
    return env


def gen_version_h(root):
    """从 code/src/version.rc（唯一版本来源）解析 FILEVERSION，生成 code/src/version.h。
    内容不变则不重写（避免 mtime 变化触发全量重编译）。"""
    rc_path = os.path.join(root, "code", "src", "version.rc")
    with open(rc_path, "r", encoding="utf-8") as f:
        rc = f.read()
    m = re.search(r"FILEVERSION\s+(\d+),(\d+),(\d+),(\d+)", rc)
    if not m:
        print("[build.py] ERROR: version.rc 缺少 FILEVERSION")
        return 1
    major, minor, patch = int(m.group(1)), int(m.group(2)), int(m.group(3))
    content = (
        "/* 自动生成：build.py 从 version.rc 解析（唯一版本来源），请勿手改。\n"
        " * 启动日志/关于框/各显示处统一引用 OS_VERSION_STR/WIDE，杜绝版本号失配。 */\n"
        "#ifndef OS_VERSION_H\n"
        "#define OS_VERSION_H\n"
        "#define OS_VERSION_MAJOR %d\n"
        "#define OS_VERSION_MINOR %d\n"
        "#define OS_VERSION_PATCH %d\n"
        "#define OS_VERSION_STR  \"%d.%d.%d\"\n"
        "#define OS_VERSION_WIDE L\"%d.%d.%d\"\n"
        "#endif\n"
        % (major, minor, patch, major, minor, patch, major, minor, patch)
    )
    h_path = os.path.join(root, "code", "src", "version.h")
    old = None
    if os.path.exists(h_path):
        with open(h_path, "r", encoding="utf-8") as f:
            old = f.read()
    if old != content:
        with open(h_path, "w", encoding="utf-8", newline="\n") as f:
            f.write(content)
        print("[build.py] version.h 已更新: %d.%d.%d" % (major, minor, patch))
    else:
        print("[build.py] version.h 无需更新: %d.%d.%d" % (major, minor, patch))
    return 0


def main():
    quiet = "--quiet" in sys.argv[1:]
    root = os.path.dirname(os.path.abspath(__file__))
    if gen_version_h(root) != 0:
        return 1
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
