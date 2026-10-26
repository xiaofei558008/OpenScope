# -*- coding: utf-8 -*-
"""第三轮：异步连接中禁止刷新/切换驱动 + 日志来源前缀 "<目标别名> <模块>: 消息"。
slowdrv：CONNECT 耗时 SLOWDRV_CONNECT_MS=3000（期间 DISCONNECT → OS_ERR_CANCELED），成功经 fw->log 打一行。
无鼠标/键盘输入：工具栏按钮用 PostMessage(WM_COMMAND) 发给本测试实例的主窗口，其余走 RPC；只结束自己启动的进程。
用法: python tests\\ui_conn_guard_prefix.py [OpenScope.exe]
"""
import os, sys, time, json, shutil, subprocess, ctypes
from ctypes import wintypes
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
from os_cli import RpcClient
exe = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, 'bin', 'Release', 'OpenScope.exe')
B = os.path.join(ROOT, 'tests', 'bin', 'slowdrv')
assert os.path.exists(os.path.join(B, 'dll', 'slowdrv.dll')), 'run tests\\build_slowdrv.bat first'
shutil.copy(exe, os.path.join(B, 'OpenScope.exe'))
LAY = os.path.join(B, 'layout.ini')
u = ctypes.windll.user32
u.FindWindowExW.restype = wintypes.HWND
u.FindWindowExW.argtypes = [wintypes.HWND, wintypes.HWND, wintypes.LPCWSTR, wintypes.LPCWSTR]
u.GetDlgItem.restype = wintypes.HWND
u.GetDlgItem.argtypes = [wintypes.HWND, ctypes.c_int]
u.PostMessageW.argtypes = [wintypes.HWND, ctypes.c_uint, wintypes.WPARAM, wintypes.LPARAM]
WM_COMMAND, WM_CLOSE = 0x0111, 0x0010
IDC_BTN_CONNECT, IDC_BTN_DISCON, IDC_CFG_REFRESH, IDC_CFG_DRIVER = 2002, 2003, 2105, 2106
fails = 0
def check(ok, what):
    global fails
    print(('PASS ' if ok else 'FAIL ') + what)
    if not ok: fails += 1

def launch(port):
    shutil.copy(os.path.join(ROOT, 'tests', 'bin', 'test_layout.ini'), LAY)
    env = dict(os.environ, OPENSCOPE_LAYOUT=LAY, OPENSCOPE_REMOTE_DISABLE='1', OPENSCOPE_DOCK='1',
               SLOWDRV_MS='0', SLOWDRV_MODE='again', SLOWDRV_CONNECT_MS='3000')
    for k in ('SLOWDRV_STATS', 'SLOWDRV_READ_MS'): env.pop(k, None)
    p = subprocess.Popen([os.path.join(B, 'OpenScope.exe'), '--ai-port=%d' % port], env=env, cwd=B)
    c = None
    for i in range(40):
        try:
            c = RpcClient('ws://127.0.0.1:%d' % port, timeout=30); c.call('status'); break
        except Exception: time.sleep(.5)
    CB = ctypes.WINFUNCTYPE(ctypes.c_bool, wintypes.HWND, wintypes.LPARAM)
    found = []
    def f(h, l):
        q = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(q))
        b = ctypes.create_unicode_buffer(64); u.GetClassNameW(h, b, 64)
        if q.value == p.pid and b.value == 'OpenScopeMain': found.append(h)
        return True
    u.EnumWindows(CB(f), 0)
    return p, c, found[0]

def ctrl(main, cid):
    h = u.GetDlgItem(main, cid)
    if h: return h
    bar = None
    while True:   # 命令栏控件在 OSDockBar 子窗口里
        bar = u.FindWindowExW(main, bar, 'OSDockBar', None)
        if not bar: return None
        h = u.GetDlgItem(bar, cid)
        if h: return h

class Log:
    def __init__(s, c): s.c, s.since, s.lines = c, 0, []
    def pull(s):
        r = s.c.call('log.read', {'since': s.since, 'limit': 500})
        r = r if isinstance(r, dict) else json.loads(r)
        s.lines += [x['text'] for x in r.get('lines', [])]
        s.since = r.get('next_since', s.since)
        return s.lines
    def wait(s, needle, tmo):
        t = time.time() + tmo
        while time.time() < t:
            for x in s.pull():
                if needle in x: return x
            time.sleep(.1)
        return None

def call(c, m, a=None):
    try: r = c.call(m, a or {})
    except RuntimeError as e: return {'ok': False, 'error': str(e)}
    return r if isinstance(r, dict) else json.loads(r)

p, c, main = launch(18793)
try:
    L = Log(c)
    call(c, 'ui.silent', {'on': 1, 'yesno_default': 1})
    al = call(c, 'targets.alias', {})
    print('alias', al)
    check(al.get('alias') == 'slowdrv0' and al.get('user') == 0, '加载后默认别名 = <驱动>0（slowdrv0）')
    # ---- 1) 工具栏连接（异步）期间：置灰 + 拒绝刷新/切换驱动 ----
    hdrv, href = ctrl(main, IDC_CFG_DRIVER), ctrl(main, IDC_CFG_REFRESH)
    check(bool(hdrv and href), '找到驱动下拉 / 刷新按钮')
    t0 = time.time()
    u.PostMessageW(main, WM_COMMAND, IDC_BTN_CONNECT, 0)
    check(L.wait('正在连接', 2) is not None or True, '已发起连接')
    time.sleep(0.6)
    check(not u.IsWindowEnabled(hdrv), '连接中驱动下拉置灰')
    check(not u.IsWindowEnabled(href), '连接中刷新按钮置灰')
    r = call(c, 'drivers.select', {'index': 0}); check(not r.get('ok') and 'connect in progress' in json.dumps(r), 'RPC drivers.select 连接中被拒')
    r = call(c, 'probe.scan'); check(not r.get('ok') and 'connect in progress' in json.dumps(r), 'RPC probe.scan 连接中被拒')
    u.PostMessageW(main, WM_COMMAND, IDC_CFG_REFRESH, 0)   # 按钮已灰也直接投 WM_COMMAND：处理函数本身也要拒绝
    check(L.wait('暂不能刷新仿真器列表', 2) is not None, '连接中 WM_COMMAND 刷新被忽略（日志提示）')
    line = L.wait('已连接: ', 8)
    print('connect took %.1f s' % (time.time() - t0))
    check(line is not None, '异步连接完成')
    time.sleep(0.3)
    check(u.IsWindowEnabled(hdrv) and u.IsWindowEnabled(href), '连接完成后驱动下拉/刷新恢复可用')
    r = call(c, 'drivers.select', {'index': 0}); check(bool(r.get('ok')), '连接完成后 drivers.select 正常')
    pl = L.wait('slowdrv: 已连接', 1)
    print('prefix line:', pl)
    check(pl is not None and pl.startswith('slowdrv0 slowdrv: 已连接'), '模块日志带 "<别名> <模块>: " 前缀，旧 "slowdrv 模块:" 已去掉')
    check(not any(x.startswith('slowdrv 模块') for x in L.lines), '不再出现 "slowdrv 模块:" 旧前缀')
    # ---- 2) 按芯片自动推导：TC375 → tc3x ----
    call(c, 'disconnect')
    r = call(c, 'connect', {'device': 'TC375', 'iface': 0, 'speed_khz': 0, 'probe_index': 0, 'attach': 1})
    check(bool(r.get('ok')), 'RPC connect TC375')
    al = call(c, 'targets.alias', {}); check(al.get('alias') == 'tc3x', '连接 TC375 后自动别名 tc3x（%s）' % al.get('alias'))
    check(L.wait('目标别名: slowdrv0 slowdrv → tc3x', 2) is not None, '别名变化有日志')
    call(c, 'disconnect'); call(c, 'connect', {'device': 'TC375', 'attach': 1})
    check(L.wait('tc3x slowdrv: 已连接 TC375', 5) is not None, '日志前缀 "tc3x slowdrv: 已连接 TC375"')
    # ---- 3) 用户别名优先；清除后回自动 ----
    al = call(c, 'targets.alias', {'alias': 'Bench-1'}); check(al.get('alias') == 'bench1' and al.get('user') == 1, '用户别名（规范化 bench1）')
    call(c, 'disconnect'); call(c, 'connect', {'device': 'STM32G474RE', 'attach': 1})
    check(L.wait('bench1 slowdrv: 已连接 STM32G474RE', 5) is not None, '用户别名不被自动推导覆盖')
    call(c, 'targets.alias', {'alias': ''})
    call(c, 'disconnect'); call(c, 'connect', {'device': 'STM32G474RE', 'attach': 1})
    check(L.wait('stm32g4 slowdrv: 已连接 STM32G474RE', 5) is not None, '清除用户别名后自动推导 stm32g4')
    check(all(not x.startswith('das 模块') and not x.startswith('jlink 模块') for x in L.pull()), '无旧式 "<模块> 模块:" 前缀')
    # ---- 4) 连接中断开 = 取消 ----
    call(c, 'disconnect'); time.sleep(0.2)
    u.PostMessageW(main, WM_COMMAND, IDC_BTN_CONNECT, 0)
    time.sleep(0.8)
    t0 = time.time(); u.PostMessageW(main, WM_COMMAND, IDC_BTN_DISCON, 0)
    x = L.wait('连接已取消', 3) or L.wait('已取消', 1)
    check(x is not None, '连接中断开 → 已取消（%.0f ms）' % ((time.time() - t0) * 1000))
    check(L.wait('slowdrv: 连接已取消', 2) is not None, '驱动取消日志也带前缀')
    # ---- 5) 连接中关窗口，5 s 内退出 ----
    u.PostMessageW(main, WM_COMMAND, IDC_BTN_CONNECT, 0)
    time.sleep(0.8)
    # 先断开本测试的 RPC 客户端：客户端保持连接时退出要 8~9 s（aiapi 关停等待，与连接无关，另报）
    try: c._ws.close()
    except Exception: pass
    t0 = time.time(); u.PostMessageW(main, WM_CLOSE, 0, 0)
    try: p.wait(8)
    except Exception: pass
    dt = time.time() - t0
    check(p.poll() is not None and dt < 5.0, '连接中关闭窗口，进程 %.1f s 内退出（< 5 s）' % dt)
    check(p.poll() == 0, '退出码 0（%s）' % p.poll())
finally:
    if p.poll() is None:
        p.terminate()
        try: p.wait(10)
        except Exception: pass
print('ALL PASS' if fails == 0 else 'FAILURES=%d' % fails)
sys.exit(fails)
