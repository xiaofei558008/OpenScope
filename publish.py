#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""OpenScope 自动发布脚本 —— 发布安装包到 www.opendebugger.com。

功能（全自动）：
  1. 扫描 dist/ 下的 OpenScope-Setup-<版本>.exe 安装包；
  2. 将缺失/更新的安装包上传到服务器 /var/www/downloads/openscope/；
  3. 自动生成下载页 HTML（含版本 / 大小 / 日期 / SHA256 / 下载按钮），上传到 /var/www/downloads/index.html；
  4. 端到端自检：curl 验证下载页与最新安装包 HTTPS 可达。

用法：
  python publish.py             # 上传缺失的新安装包 + 重新生成下载页
  python publish.py --all       # 强制上传所有安装包（覆盖同版本）
  python publish.py --dry-run   # 只打印将要执行的操作，不实际上传
  python publish.py --no-upload # 只重新生成下载页（不传安装包）

依赖：本地已配置到 xiaofei@8.133.18.102 的 SSH 免密登录（scp 可用）。
"""
import argparse
import hashlib
import os
import re
import subprocess
import sys
import time

# ---------------------------------------------------------------------------
# 配置
# ---------------------------------------------------------------------------
ROOT = os.path.dirname(os.path.abspath(__file__))
DIST_DIR = os.path.join(ROOT, "dist")
HOST = "xiaofei@8.133.18.102"
REMOTE_DIR = "/var/www/downloads/openscope/"
REMOTE_PAGE = "/var/www/downloads/index.html"
BASE_URL = "https://www.opendebugger.com"
SITE_HOME = "https://www.opendebugger.com/"
# 页面样式主色调（与主站蓝色主题一致）
COLOR_PRIMARY = "#1e3a8a"
COLOR_ACCENT = "#3b82f6"
COLOR_BG = "#f4f7fb"


def ssh_prefix():
    """返回 scp/ssh 通用参数。Windows 下批量复制。"""
    return ["scp", "-o", "BatchMode=yes", "-o", "ConnectTimeout=15"]


def run(cmd, check=True):
    print("  $ " + " ".join(cmd))
    r = subprocess.run(cmd, capture_output=True, text=True)
    if check and r.returncode != 0:
        sys.exit(f"[publish] 命令失败: {' '.join(cmd)}\n{r.stderr}")
    return r


# ---------------------------------------------------------------------------
# 安装包信息
# ---------------------------------------------------------------------------
def list_installers():
    """返回 [{version, path, size, mtime}]，按版本号从新到旧排序。"""
    items = []
    if os.path.isdir(DIST_DIR):
        for name in os.listdir(DIST_DIR):
            m = re.match(r"^OpenScope-Setup-(\d+\.\d+(?:\.\d+)?)\.exe$", name)
            if not m:
                continue
            path = os.path.join(DIST_DIR, name)
            st = os.stat(path)
            ver = m.group(1)
            items.append({
                "version": ver,
                "filename": name,
                "path": path,
                "size": st.st_size,
                "mtime": st.st_mtime,
            })
    items.sort(key=lambda x: [int(p) for p in x["version"].split(".")], reverse=True)
    return items


def sha256_of(path, chunk=1 << 20):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        while True:
            blk = f.read(chunk)
            if not blk:
                break
            h.update(blk)
    return h.hexdigest()


def fmt_size(n):
    if n >= 1 << 20:
        return f"{n / (1 << 20):.1f} MB"
    if n >= 1 << 10:
        return f"{n / (1 << 10):.1f} KB"
    return f"{n} B"


def fmt_date(ts):
    return time.strftime("%Y-%m-%d", time.localtime(ts))


# ---------------------------------------------------------------------------
# 下载页生成
# ---------------------------------------------------------------------------
def build_page(items):
    """生成下载页 HTML。items: 已含 sha256 的安装包信息（新→旧）。"""
    latest = items[0] if items else None
    rows = []
    for i, it in enumerate(items):
        url = f"{BASE_URL}/downloads/openscope/{it['filename']}"
        badge = '<span class="badge">最新版</span>' if i == 0 else ""
        rows.append(f"""
        <tr class="{'latest' if i == 0 else ''}">
          <td class="ver">{badge}<b>OpenScope {it['version']}</b></td>
          <td>{fmt_size(it['size'])}</td>
          <td>{it['date']}</td>
          <td><code class="sha" title="点击复制">{it['sha256']}</code></td>
          <td><a class="btn" href="{url}">下载</a></td>
        </tr>""")

    latest_card = ""
    if latest:
        url = f"{BASE_URL}/downloads/openscope/{latest['filename']}"
        latest_card = f"""
      <div class="latest-card">
        <div>
          <h2>最新版本 v{latest['version']}</h2>
          <p class="sub">Windows x64 · {fmt_size(latest['size'])} · {latest['date']}</p>
          <p class="desc">OpenScope —— MCU 变量采集与校准工具。支持 SWD/JTAG 接口，实时采集、记录回放、波形/数值窗口。</p>
        </div>
        <a class="btn-big" href="{url}">⬇ 下载 OpenScope v{latest['version']}</a>
      </div>"""

    return f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>OpenScope 软件下载 - 晶圆上的生物</title>
<style>
  * {{ margin: 0; padding: 0; box-sizing: border-box; }}
  body {{ font-family: -apple-system, "Segoe UI", "Microsoft YaHei", Roboto, sans-serif;
         background: {COLOR_BG}; color: #1e293b; }}
  .topbar {{ background: linear-gradient(135deg, {COLOR_PRIMARY} 0%, {COLOR_ACCENT} 100%);
            color: #fff; }}
  .topbar-inner {{ max-width: 960px; margin: 0 auto; padding: 14px 20px; display: flex;
                  justify-content: space-between; align-items: center; }}
  .topbar a {{ color: #fff; text-decoration: none; font-size: 14px; opacity: .9; }}
  .topbar a:hover {{ opacity: 1; }}
  .brand {{ font-size: 18px; font-weight: 600; }}
  .container {{ max-width: 960px; margin: 0 auto; padding: 32px 20px 60px; }}
  h1 {{ color: {COLOR_PRIMARY}; margin: 8px 0 6px; }}
  .lead {{ color: #475569; margin-bottom: 24px; line-height: 1.7; }}
  .latest-card {{ background: linear-gradient(135deg, {COLOR_PRIMARY}, {COLOR_ACCENT});
                 color: #fff; border-radius: 12px; padding: 24px 28px; margin-bottom: 28px;
                 display: flex; justify-content: space-between; align-items: center;
                 gap: 20px; flex-wrap: wrap; box-shadow: 0 8px 24px rgba(30,58,138,.25); }}
  .latest-card h2 {{ margin-bottom: 6px; }}
  .latest-card .sub {{ opacity: .85; font-size: 14px; margin-bottom: 8px; }}
  .latest-card .desc {{ opacity: .95; font-size: 14px; line-height: 1.6; }}
  .btn-big {{ background: #fff; color: {COLOR_PRIMARY}; font-weight: 700; font-size: 16px;
             padding: 14px 26px; border-radius: 8px; text-decoration: none;
             white-space: nowrap; box-shadow: 0 4px 12px rgba(0,0,0,.2); }}
  .btn-big:hover {{ background: #eef4ff; }}
  h3.section {{ color: {COLOR_PRIMARY}; margin: 24px 0 12px; }}
  table {{ width: 100%; border-collapse: collapse; background: #fff; border-radius: 10px;
          overflow: hidden; box-shadow: 0 2px 10px rgba(15,23,42,.06); }}
  th, td {{ padding: 12px 14px; text-align: left; font-size: 14px; }}
  th {{ background: {COLOR_PRIMARY}; color: #fff; font-weight: 600; }}
  tr.latest {{ background: #eef4ff; }}
  td.ver {{ white-space: nowrap; }}
  .badge {{ display: inline-block; background: #dc2626; color: #fff; font-size: 11px;
           padding: 2px 8px; border-radius: 10px; margin-right: 8px; vertical-align: 1px; }}
  code.sha {{ font-size: 11px; color: #475569; user-select: all; word-break: break-all; }}
  .btn {{ display: inline-block; background: {COLOR_ACCENT}; color: #fff; padding: 7px 18px;
         border-radius: 6px; text-decoration: none; font-weight: 600; }}
  .btn:hover {{ background: {COLOR_PRIMARY}; }}
  .notes {{ margin-top: 32px; background: #fff; border-radius: 10px; padding: 20px 24px;
           border-left: 4px solid {COLOR_ACCENT}; font-size: 14px; line-height: 1.8; color: #475569; }}
  .notes a {{ color: {COLOR_ACCENT}; }}
  .footer {{ text-align: center; color: #94a3b8; font-size: 13px; padding: 20px; }}
</style>
</head>
<body>
  <div class="topbar">
    <div class="topbar-inner">
      <span class="brand">⬢ OpenScope</span>
      <nav><a href="{SITE_HOME}">← 返回主页</a></nav>
    </div>
  </div>
  <div class="container">
    <h1>OpenScope 软件下载</h1>
    <p class="lead">OpenScope 是晶圆上的生物（www.opendebugger.com）开发的 MCU 变量采集与校准工具，配合 J-Link 隔离器使用。<br>
    支持 SWD / JTAG 接口，实时采集变量、CSV 记录与回放、波形/数值/示波器窗口，适用于嵌入式调试与电机/编码器等场景。</p>

    {latest_card}

    <h3 class="section">历史版本</h3>
    <table>
      <thead><tr><th>版本</th><th>大小</th><th>发布日期</th><th>SHA256 校验和</th><th></th></tr></thead>
      <tbody>{''.join(rows)}
      </tbody>
    </table>

    <div class="notes">
      <b>安装说明：</b>Windows 10/11 x64。安装包为 Inno Setup 安装程序，双击安装即可；如浏览器提示"未知发布者"，
      请选择"仍要下载/运行"，或右键安装包 → 属性 → 解除阻止。下载后建议核对上表 SHA256 校验和，确保文件完整。
      <br><b>源码：</b><a href="https://gitee.com/xiaofei558008/open-scope">Gitee</a> ·
      <a href="https://github.com/xiaofei558008/OpenScope">GitHub</a>
      <br><b>支持：</b>如有使用问题，请联系我们或关注公众号获取技术支持。
    </div>
  </div>
  <div class="footer">© 2026 晶圆上的生物 · OpenScope · www.opendebugger.com</div>
</body>
</html>
"""


# ---------------------------------------------------------------------------
# 主流程
# ---------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(description="OpenScope 安装包自动发布")
    ap.add_argument("--all", action="store_true", help="强制上传所有安装包（覆盖同版本）")
    ap.add_argument("--dry-run", action="store_true", help="只打印操作，不实际上传")
    ap.add_argument("--no-upload", action="store_true", help="只生成下载页，不传安装包")
    args = ap.parse_args()

    items = list_installers()
    if not items:
        sys.exit("[publish] dist/ 下未找到 OpenScope-Setup-*.exe 安装包")

    print(f"[publish] 发现 {len(items)} 个安装包:")
    for it in items:
        print(f"  - {it['filename']} ({fmt_size(it['size'])})")

    # 1) 计算校验和
    for it in items:
        it["sha256"] = sha256_of(it["path"])
        it["date"] = fmt_date(it["mtime"])

    # 2) 上传安装包
    if not args.no_upload:
        for it in items:
            remote = REMOTE_DIR + it["filename"]
            remote_file = f"{HOST}:{remote}"
            local = it["path"]
            if not args.all:
                r = run(["ssh", "-o", "BatchMode=yes", "-o", "ConnectTimeout=15", HOST,
                         f"test -f {remote} && wc -c < {remote} || echo MISSING"],
                        check=False)
                cur = r.stdout.strip()
                if cur == str(it["size"]):
                    print(f"[publish] 已存在且大小一致，跳过: {it['filename']}")
                    continue
                print(f"[publish] 上传 {it['filename']} ({fmt_size(it['size'])}) ...")
            if args.dry_run:
                print(f"[publish] [dry-run] scp {local} {remote_file}")
            else:
                run(["scp", "-o", "BatchMode=yes", "-o", "ConnectTimeout=15", local, remote_file])
    else:
        print("[publish] --no-upload，跳过安装包上传")

    # 3) 生成并上传下载页
    page = build_page(items)
    local_page = os.path.join(ROOT, "dist", "_downloads_index.html")
    with open(local_page, "w", encoding="utf-8") as f:
        f.write(page)
    if args.dry_run:
        print(f"[publish] [dry-run] 下载页已生成: {local_page}（未上传）")
    else:
        print(f"[publish] 上传下载页 {REMOTE_PAGE} ...")
        run(["scp", "-o", "BatchMode=yes", "-o", "ConnectTimeout=15", local_page, f"{HOST}:{REMOTE_PAGE}"])
        # 确保 www-data 可读
        run(["ssh", "-o", "BatchMode=yes", HOST,
             "chmod 644 /var/www/downloads/index.html"])
        os.remove(local_page)

    # 4) 端到端自检
    if args.dry_run:
        print("[publish] [dry-run] 跳过自检")
        return
    import urllib.request
    checks = [
        (f"{BASE_URL}/downloads/", "下载页"),
        (f"{BASE_URL}/downloads/openscope/{items[0]['filename']}", f"最新安装包 v{items[0]['version']}"),
    ]
    print("[publish] 自检:")
    ok = True
    for url, label in checks:
        try:
            req = urllib.request.Request(url, headers={"User-Agent": "publish-script"})
            with urllib.request.urlopen(req, timeout=30) as resp:
                status = resp.status
                size = resp.headers.get("Content-Length", "?")
            print(f"  [OK] {label}: HTTP {status} ({size} bytes)")
        except Exception as e:
            ok = False
            print(f"  [ERR] {label}: {e}")
    print()
    print(f"[publish] 完成 [OK] 下载页: {BASE_URL}/downloads/")
    if not ok:
        sys.exit("[publish] 自检发现异常，请检查！")


if __name__ == "__main__":
    main()
