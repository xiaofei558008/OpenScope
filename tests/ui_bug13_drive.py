# -*- coding: utf-8 -*-
r"""Bug1 + Bug3 回归（无鼠标/键盘输入，PrintWindow 截图）：
 Bug1: 同一源文件用 D:/..、D:\.\..、大小写/混合斜杠打开 → 只有 1 个源码窗口
 Bug3: 暗色主题下变量树/源码/右侧区无 WS_EX_CLIENTEDGE，停靠区上沿无整行浅色条
用法: python tests\ui_bug13_drive.py [OpenScope.exe] [shot_prefix]
"""
import os, sys, time, json, shutil, subprocess, ctypes
from ctypes import wintypes
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
from os_cli import RpcClient
from PIL import Image
exe = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, 'bin', 'Release', 'OpenScope.exe')
shot = sys.argv[2] if len(sys.argv) > 2 else ''
OUT = os.path.join(ROOT, 'tests', 'out'); os.makedirs(OUT, exist_ok=True)
LAY = os.path.join(ROOT, 'tests', 'bin', 'bug13_layout.ini')
_t = open(os.path.join(ROOT, 'tests', 'bin', 'test_layout.ini'), encoding='utf-8', errors='ignore').read()
import re
open(LAY, 'w', encoding='utf-8').write(re.sub(r'(?m)^theme=\d', 'theme=1', _t))   # 强制暗色
SRC_DIR = os.path.join(ROOT, 'tests', 'bin', 'bug13src'); os.makedirs(SRC_DIR, exist_ok=True)
SRC = os.path.join(SRC_DIR, 'main.c')
open(SRC, 'w').write('\n'.join('int line%d;' % i for i in range(200)))
env = dict(os.environ, OPENSCOPE_LAYOUT=LAY, OPENSCOPE_REMOTE_DISABLE='1')
u = ctypes.windll.user32; g = ctypes.windll.gdi32
try: u.SetProcessDPIAware()
except Exception: pass
PORT = 18782
p = subprocess.Popen([exe, '--ai-port=%d' % PORT], env=env)
fails = 0
def check(ok, what):
    global fails
    print(('PASS ' if ok else 'FAIL ') + what); fails += 0 if ok else 1
CB = ctypes.WINFUNCTYPE(ctypes.c_bool, wintypes.HWND, wintypes.LPARAM)
def cls(h):
    b = ctypes.create_unicode_buffer(64); u.GetClassNameW(h, b, 64); return b.value
def children(parent):
    out = []
    def f(h, l): out.append(h); return True
    u.EnumChildWindows(parent, CB(f), 0); return out
def grab(h):
    r = wintypes.RECT(); u.GetWindowRect(h, ctypes.byref(r)); w, hh = r.right - r.left, r.bottom - r.top
    hdc = u.GetDC(0); mdc = g.CreateCompatibleDC(hdc); bmp = g.CreateCompatibleBitmap(hdc, w, hh); g.SelectObject(mdc, bmp)
    u.PrintWindow(h, mdc, 2)
    class BI(ctypes.Structure):
        _fields_ = [('a', ctypes.c_uint32), ('w', ctypes.c_int32), ('h', ctypes.c_int32), ('p', ctypes.c_uint16), ('b', ctypes.c_uint16),
                    ('c', ctypes.c_uint32), ('s', ctypes.c_uint32), ('x', ctypes.c_int32), ('y', ctypes.c_int32), ('u', ctypes.c_uint32), ('i', ctypes.c_uint32)]
    bi = BI(40, w, -hh, 1, 32, 0, 0, 0, 0, 0, 0); buf = ctypes.create_string_buffer(w * hh * 4)
    g.GetDIBits(mdc, bmp, 0, hh, buf, ctypes.byref(bi), 0)
    im = Image.frombuffer('RGBA', (w, hh), buf, 'raw', 'BGRA', 0, 1).convert('RGB')
    g.DeleteObject(bmp); g.DeleteDC(mdc); u.ReleaseDC(0, hdc); return im, r
try:
    c = None
    for i in range(40):
        try: c = RpcClient('ws://127.0.0.1:%d' % PORT, timeout=15); c.call('ui.dock.state'); break
        except Exception: time.sleep(.5)
    def call(m, a=None):
        r = c.call(m, a or {}); return r if isinstance(r, dict) else json.loads(r)
    call('src.open', {'path': SRC, 'line': 10})
    call('src.open', {'path': SRC.replace('\\', '/'), 'line': 20})
    call('src.open', {'path': os.path.join(SRC_DIR, '.', 'main.c'), 'line': 30})
    call('src.open', {'path': SRC.lower().replace('\\main.c', '/MAIN.C'), 'line': 40})
    n = len(call('ui.src.state').get('src', []))
    check(n == 1, 'Bug1 混合路径打开同一文件只有 1 个源码窗口（实际 %d）' % n)
    time.sleep(.8)
    mains = []
    def f(h, l):
        q = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(q))
        if q.value == p.pid and cls(h) == 'OpenScopeMain': mains.append(h)
        return True
    u.EnumWindows(CB(f), 0)
    main = mains[0]
    edged = [cls(h) for h in children(main) if u.IsWindowVisible(h) and (u.GetWindowLongW(h, -20) & 0x200)
             and cls(h) in ('OSRightPanel', 'SysTreeView32', 'OSSrcWin', 'SysListView32')]
    check(not edged, 'Bug3 暗色下主要面板无 WS_EX_CLIENTEDGE（残留: %s）' % edged)
    im, r = grab(main)
    if shot: im.save(os.path.join(OUT, shot + '_dark.png'))
    host = [h for h in children(main) if cls(h) == 'OSRightPanel']
    hr = wintypes.RECT(); u.GetWindowRect(host[0], ctypes.byref(hr))
    y0 = hr.top - r.top; W = im.size[0]
    light = []
    for y in range(max(0, y0 - 2), y0 + 4):
        xs = range(20, W - 20, 8)
        if sum(1 for x in xs if sum(im.getpixel((x, y))) > 360) > 0.7 * len(xs): light.append(y)
    check(not light, 'Bug3 停靠区上沿无整行浅色条（行 %s）' % light)
    check(p.poll() is None, '进程未退出')
finally:
    p.terminate()
    try: p.wait(10)
    except Exception: pass
print('ALL PASS' if fails == 0 else 'FAILURES=%d' % fails)
sys.exit(fails)
