#!/usr/bin/env python3
"""构建 OpenScope 安装包（request.md 第 8 条）。

流程：
  1. 从 code/src/version.rc 读取版本号（唯一来源，保证与 exe 版本一致）；
  2. 生成安装界面资源：assets/openscope.ico（缺失时 make_icon.py）+ 左侧广告栏
     packaging/wizard_sidebar.bmp（icon/isolator.jpg 居中裁剪 164x314，每次重新生成）；
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
SIDEBAR_SRC_BMP = os.path.join(ROOT, "icon", "isolator.bmp")   # 优先：用户提供
SIDEBAR_JPG = os.path.join(ROOT, "icon", "isolator.jpg")        # 兜底
SIDEBAR_BMP = os.path.join(ROOT, "packaging", "wizard_sidebar.bmp")


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


def ensure_sidebar_banner():
    """生成安装向导左侧广告栏图片 packaging/wizard_sidebar.bmp。
    Inno Setup 只支持 BMP；向导图标准尺寸 164x314（96dpi，运行时随 DPI 等比拉伸）。
    图片源优先 icon/isolator.bmp（用户提供），缺失时退回 icon/isolator.jpg。
    每次打包重新生成，替换图片后下次打包即生效。"""
    src_file = SIDEBAR_SRC_BMP if os.path.isfile(SIDEBAR_SRC_BMP) else SIDEBAR_JPG
    if not os.path.isfile(src_file):
        raise SystemExit(f"[make_setup] 广告栏图片缺失: {src_file}（icon/isolator.bmp 或 isolator.jpg）")
    try:
        from PIL import Image
    except ImportError:
        raise SystemExit("[make_setup] 生成广告栏图片需要 Pillow，请先执行: pip install pillow")
    w, h = 164, 314
    src = Image.open(src_file).convert("RGB")
    sw, sh = src.size
    ratio = w / h
    if sw / sh > ratio:  # 原图偏宽：居中裁左右
        cw = int(round(sh * ratio))
        box = ((sw - cw) // 2, 0, (sw + cw) // 2, sh)
    else:                # 原图偏高：居中裁上下
        ch = int(round(sw / ratio))
        box = (0, (sh - ch) // 2, sw, (sh + ch) // 2)
    src.crop(box).resize((w, h), Image.Resampling.LANCZOS).save(SIDEBAR_BMP, "BMP")
    print(f"[make_setup] 广告栏: {os.path.relpath(src_file, ROOT)} ({sw}x{sh}) "
          f"-> {os.path.relpath(SIDEBAR_BMP, ROOT)} ({w}x{h} BMP)")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--version", default=None, help="完整版本号，默认读取 version.rc")
    ap.add_argument("--publish", action="store_true",
                    help="打包完成后自动发布到 www.opendebugger.com（调用 publish.py）")
    args = ap.parse_args()
    full = args.version or read_version()
    display = display_version(full)
    iss = open(ISS, encoding="utf-8", errors="replace").read()
    # Bug17: 校验全部版本相关字段，避免安装界面/文件属性显示旧版本。
    missing = []
    if f"VersionInfoVersion={full}" not in iss:
        missing.append(f"VersionInfoVersion={full}")
    if f"VersionInfoProductVersion={full}" not in iss:
        missing.append(f"VersionInfoProductVersion={full}")
    if f"AppVerName=OpenScope {display}" not in iss:
        missing.append(f"AppVerName=OpenScope {display}")
    if f"OpenScope-Setup-{display}" not in iss:
        missing.append(f"OpenScope-Setup-{display}")
    if missing:
        raise SystemExit(
            f"[make_setup] packaging/openscope.iss 版本与 version.rc({full}) 不一致，"
            f"缺少：{'、'.join(missing)}。请同步 openscope.iss 后重试。"
        )
    if not os.path.isfile(ISCC):
        raise SystemExit(f"[make_setup] ISCC not found: {ISCC}\n"
                         "请先将 Inno Setup 安装到 tools\\innosetup\\（ISCC.exe 位于该目录）。")
    os.makedirs(os.path.join(ROOT, "dist"), exist_ok=True)
    ensure_icon()
    ensure_sidebar_banner()
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

    if args.publish:
        print(f"[make_setup] 自动发布新版本 v{display} ...")
        publish_py = os.path.join(ROOT, "publish.py")
        if not os.path.isfile(publish_py):
            raise SystemExit(f"[make_setup] publish.py not found: {publish_py}")
        r = subprocess.run([sys.executable, publish_py], cwd=ROOT)
        if r.returncode:
            raise SystemExit(f"[make_setup] publish failed rc={r.returncode}")
        print(f"[make_setup] 发布完成 [OK] https://www.opendebugger.com/downloads/")


if __name__ == "__main__":
    main()
