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
SIDEBAR_BMP = os.path.join(ROOT, "packaging", "wizard_sidebar.bmp")    # 第 1 张（iso1/iso0）
SIDEBAR_BMP2 = os.path.join(ROOT, "packaging", "wizard_sidebar2.bmp")  # 第 2 张（iso2）
SIDEBAR_BMP3 = os.path.join(ROOT, "packaging", "wizard_sidebar3.bmp")  # 第 3 张（iso3）


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
    """生成安装向导左侧广告栏图片（每张 164x314，单图等比缩放居中、白底）。
    三张产品图分别对应三个安装步骤：
      wizard_sidebar.bmp  = iso1（缺则 iso0）——编译期嵌入，欢迎/完成页
      wizard_sidebar2.bmp = iso2                  ——选择目录页/安装中页
      wizard_sidebar3.bmp = iso3                  ——选择任务页
    缺失的图生成空白横幅占位；三张都缺失时退回 isolator.bmp/jpg 单图裁剪。
    每次打包重新生成，替换图片后下次打包即生效。"""
    try:
        from PIL import Image
    except ImportError:
        raise SystemExit("[make_setup] 生成广告栏图片需要 Pillow，请先执行: pip install pillow")
    w, h = 164, 314

    def _fit(src_path):
        """单图等比缩放居中放入 164x314 白底横幅"""
        im = Image.open(src_path).convert("RGB")
        k = min(w / im.width, h / im.height)
        im = im.resize((max(1, round(im.width * k)), max(1, round(im.height * k))),
                       Image.Resampling.LANCZOS)
        canvas = Image.new("RGB", (w, h), (255, 255, 255))
        canvas.paste(im, ((w - im.width) // 2, (h - im.height) // 2))
        return canvas

    iso1 = os.path.join(ROOT, "icon", "iso1.bmp")
    iso0 = os.path.join(ROOT, "icon", "iso0.bmp")
    iso2 = os.path.join(ROOT, "icon", "iso2.bmp")
    iso3 = os.path.join(ROOT, "icon", "iso3.bmp")
    first = iso1 if os.path.isfile(iso1) else iso0
    slots = [(first, SIDEBAR_BMP), (iso2, SIDEBAR_BMP2), (iso3, SIDEBAR_BMP3)]
    found = 0
    for src, out in slots:
        if os.path.isfile(src):
            _fit(src).save(out, "BMP")
            found += 1
            print(f"[make_setup] 广告栏: {os.path.relpath(src, ROOT)} "
                  f"-> {os.path.relpath(out, ROOT)} ({w}x{h} BMP)")
        else:
            Image.new("RGB", (w, h), (255, 255, 255)).save(out, "BMP")
            print(f"[make_setup] 广告栏: {os.path.relpath(src, ROOT)} 缺失，"
                  f"{os.path.relpath(out, ROOT)} 用空白横幅占位")
    if found:
        return
    # 兜底：isolator 单图裁剪
    src_file = SIDEBAR_SRC_BMP if os.path.isfile(SIDEBAR_SRC_BMP) else SIDEBAR_JPG
    if not os.path.isfile(src_file):
        raise SystemExit(f"[make_setup] 广告栏图片缺失: {src_file}（icon/iso*.bmp 或 isolator.*）")
    src = Image.open(src_file).convert("RGB")
    sw, sh = src.size
    ratio = w / h
    if sw / sh > ratio:
        cw = int(round(sh * ratio))
        box = ((sw - cw) // 2, 0, (sw + cw) // 2, sh)
    else:
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
