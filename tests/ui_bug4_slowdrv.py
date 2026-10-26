# -*- coding: utf-8 -*-
"""Bug4 回归：探针驱动调用阻塞时 UI 线程必须保持响应（不再在 UI 线程同步调 GET_CPU_STATE）。
无鼠标/键盘输入：只用 RPC + SendMessageTimeout + SWP_ASYNCWINDOWPOS，不干扰用户桌面。
用法: python tests\\ui_bug4_slowdrv.py [OpenScope.exe] [--block]
  默认（again）：slowdrv 模拟第二轮驱动——后台重连窗口内 CONNECT / GET_CPU_STATE / BP_LIST 返回 OS_ERR_AGAIN；
                 断言 RPC connect 等重连结束后成功、日志无“连接失败”、UI 走过 AGAIN 分支且之后恢复。
  --block：旧驱动行为，GET_CPU_STATE / BP_LIST 每次阻塞 3 s（测 UI 线程不同步等驱动）。
"""
import os, sys, time, json, shutil, subprocess, ctypes
from ctypes import wintypes
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'scripts'))
from os_cli import RpcClient
BLOCK = '--block' in sys.argv
sys.argv = [a for a in sys.argv if a != '--block']
exe = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, 'bin', 'Release', 'OpenScope.exe')
B = os.path.join(ROOT, 'tests', 'bin', 'slowdrv')
assert os.path.exists(os.path.join(B, 'dll', 'slowdrv.dll')), 'run tests\\build_slowdrv.bat first'
shutil.copy(exe, os.path.join(B, 'OpenScope.exe'))
for d in ('aiapi.dll',):
    src = os.path.join(ROOT, 'dll', d)
    if os.path.exists(src): shutil.copy(src, os.path.join(B, 'dll', d))
LAY = os.path.join(B, 'layout.ini')
shutil.copy(os.path.join(ROOT, 'tests', 'bin', 'test_layout.ini'), LAY)
STATS = os.path.join(B, 'slowdrv_stats.txt')
LOG = os.path.join(B, 'openscope.log')
for f in (STATS, LOG):
    if os.path.exists(f): os.remove(f)
env = dict(os.environ, OPENSCOPE_LAYOUT=LAY, OPENSCOPE_REMOTE_DISABLE='1', SLOWDRV_MS='3000',
           SLOWDRV_MODE='block' if BLOCK else 'again', SLOWDRV_STATS=STATS)
print('mode', 'block' if BLOCK else 'again')
u = ctypes.windll.user32
PORT = 18781
p = subprocess.Popen([os.path.join(B, 'OpenScope.exe'), '--ai-port=%d' % PORT], env=env, cwd=B)
fails = 0
def check(ok, what):
    global fails
    print(('PASS ' if ok else 'FAIL ') + what)
    if not ok: fails += 1
try:
    c = None
    for i in range(40):
        try:
            c = RpcClient('ws://127.0.0.1:%d' % PORT, timeout=30); c.call('ui.dock.state'); break
        except Exception: time.sleep(.5)
    def call(m, a=None):
        r = c.call(m, a or {}); return r if isinstance(r, dict) else json.loads(r)
    CB = ctypes.WINFUNCTYPE(ctypes.c_bool, wintypes.HWND, wintypes.LPARAM)
    found = []
    def f(h, l):
        q = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(q))
        b = ctypes.create_unicode_buffer(64); u.GetClassNameW(h, b, 64)
        if q.value == p.pid and b.value == 'OpenScopeMain': found.append(h)
        return True
    u.EnumWindows(CB(f), 0)
    main = found[0]
    dl = call('drivers.list'); print('drivers', json.dumps(dl, ensure_ascii=False)[:300])
    idx = None
    for k, d in enumerate(dl.get('drivers', dl.get('items', []))):
        if 'slow' in json.dumps(d).lower(): idx = d.get('index', k)
    check(idx is not None, 'slowdrv 已加载')
    call('drivers.select', {'index': idx})
    t0 = time.time()
    r = call('connect', {'device': 'X', 'iface': 0, 'speed_khz': 1000, 'probe_index': 0, 'attach': 1})
    print('connect', r, '%.0f ms' % ((time.time() - t0) * 1000))
    check(bool(r.get('ok')) and r.get('connected') == 1, 'connect 成功（again 模式下等后台重连结束后完成）')
    call('ui.win.create', {'type': 'locals'})   # 打开局部变量窗 → 500 ms 定时器查询 CPU 状态
    res = ctypes.c_size_t()
    worst = 0.0; hung = 0
    t_end = time.time() + 8
    k = 0
    r0 = wintypes.RECT(); u.GetWindowRect(main, ctypes.byref(r0))
    while time.time() < t_end:
        k += 1
        # 触发 layout()（WM_SIZE）：异步改窗口大小，等价拖动/调整边框
        u.SetWindowPos(main, 0, 0, 0, 1000 + (k % 2) * 40, 700, 0x4000 | 0x0002 | 0x0004 | 0x0010)
        t0 = time.time()
        ok = u.SendMessageTimeoutW(main, 0, 0, 0, 0x0002, 5000, ctypes.byref(res))
        dt = time.time() - t0
        worst = max(worst, dt)
        if not ok or dt > 1.0: hung += 1
        time.sleep(0.1)
    print('UI worst WM_NULL latency %.0f ms, slow replies %d' % (worst * 1000, hung))
    check(worst < 0.5, 'UI 线程在探针阻塞 3 s 时仍响应（最坏 %.0f ms < 500 ms）' % (worst * 1000))
    check(not u.IsHungAppWindow(main), '窗口未被系统判定为未响应')
    check(p.poll() is None, '进程未退出')
    if not BLOCK:
        st = [0] * 6
        for _ in range(20):   # slowdrv 随时在重写该文件：读到完整 6 个数为止
            try:
                v = [int(x) for x in open(STATS).read().split()]
                if len(v) == 6: st = v; break
            except (OSError, ValueError): pass
            time.sleep(0.05)
        print('slowdrv stats connect_again/ok cpu_again/ok bp_again/ok =', st)
        check(st[0] > 0 and st[1] == 1, 'CONNECT 先收到 AGAIN，重试后成功一次')
        check(st[2] > 0, 'UI 收到过 GET_CPU_STATE AGAIN（忙/重连中分支）')
        check(st[3] > 0, '重连窗口结束后 GET_CPU_STATE 自动恢复')
        lg = open(LOG, 'rb').read() if os.path.exists(LOG) else b''
        bad = [w for w in ('连接失败', 'connect failed') for enc in ('utf-8', 'gbk') if w.encode(enc) in lg]
        check(not bad, '日志无误导性的连接失败 %s' % bad)
finally:
    t0 = time.time(); p.terminate()
    try: p.wait(10)
    except Exception: pass
print('ALL PASS' if fails == 0 else 'FAILURES=%d' % fails)
sys.exit(fails)
