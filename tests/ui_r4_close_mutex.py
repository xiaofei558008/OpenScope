# -*- coding: utf-8 -*-
"""第四轮：(1) 0/1/2/3 个 RPC 客户端保持连接时关闭 OpenScope 的耗时（目标 < 1 s，原先 8~9 s）；
(2) UI 连接与 AI 连接互斥：只有一方拿到，另一方 AGAIN / 被忽略；UI 断开取消 AI 连接 → CANCELED。
slowdrv（SLOWDRV_CONNECT_MS）。无鼠标/键盘输入：按钮用 PostMessage(WM_COMMAND) 投给本测试实例，其余走 RPC；只结束自己启动的进程。
用法: python tests\\ui_r4_close_mutex.py [OpenScope.exe]
"""
import os, sys, time, json, shutil, subprocess, ctypes, threading
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
u.PostMessageW.argtypes = [wintypes.HWND, ctypes.c_uint, wintypes.WPARAM, wintypes.LPARAM]
WM_COMMAND, WM_CLOSE = 0x0111, 0x0010
IDC_BTN_CONNECT, IDC_BTN_DISCON = 2002, 2003
fails = 0
def check(ok, what):
    global fails
    print(('PASS ' if ok else 'FAIL ') + what, flush=True)
    if not ok: fails += 1

def rpc(port, tmo=30):
    for i in range(40):
        try:
            c = RpcClient('ws://127.0.0.1:%d' % port, timeout=tmo); c.call('status'); return c
        except Exception: time.sleep(.5)
    raise RuntimeError('no rpc')

def launch(port, connect_ms):
    shutil.copy(os.path.join(ROOT, 'tests', 'bin', 'test_layout.ini'), LAY)
    env = dict(os.environ, OPENSCOPE_LAYOUT=LAY, OPENSCOPE_REMOTE_DISABLE='1', OPENSCOPE_DOCK='1',
               SLOWDRV_MS='0', SLOWDRV_MODE='again', SLOWDRV_CONNECT_MS=str(connect_ms))
    for k in ('SLOWDRV_STATS', 'SLOWDRV_READ_MS'): env.pop(k, None)
    p = subprocess.Popen([os.path.join(B, 'OpenScope.exe'), '--ai-port=%d' % port], env=env, cwd=B)
    c = rpc(port)
    CB = ctypes.WINFUNCTYPE(ctypes.c_bool, wintypes.HWND, wintypes.LPARAM)
    found = []
    def f(h, l):
        q = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(q))
        b = ctypes.create_unicode_buffer(64); u.GetClassNameW(h, b, 64)
        if q.value == p.pid and b.value == 'OpenScopeMain': found.append(h)
        return True
    u.EnumWindows(CB(f), 0)
    return p, c, found[0]

def call(c, m, a=None):
    try: r = c.call(m, a or {})
    except RuntimeError as e: return {'ok': False, 'error': str(e)}
    except Exception as e: return {'ok': False, 'error': repr(e)}
    return r if isinstance(r, dict) else json.loads(r)

class Log:
    def __init__(s, c): s.c, s.since, s.lines = c, 0, []
    def pull(s):
        r = call(s.c, 'log.read', {'since': s.since, 'limit': 500})
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

def stop(p):
    if p.poll() is None:
        p.terminate()
        try: p.wait(10)
        except Exception: pass

res = {}
# ---------- (1) 关闭耗时 ----------
for n in (0, 1, 2, 3):
    port = 18810 + n
    p, c, main = launch(port, 300)
    try:
        cl = [c] + [rpc(port) for _ in range(max(0, n - 1))]
        if n == 0:
            try: c._ws.close()
            except Exception: pass
            cl = []
        for x in cl: call(x, 'status')          # 都已握手、空闲在 recv
        time.sleep(0.3)
        t0 = time.time(); u.PostMessageW(main, WM_CLOSE, 0, 0)
        try: p.wait(10)
        except Exception: pass
        dt = time.time() - t0
        res['close_%d' % n] = dt
        check(p.poll() is not None and dt < 1.0, '%d 个 RPC 客户端保持连接时关闭：%.0f ms 内退出（< 1000 ms），退出码 %s' % (n, dt * 1000, p.poll()))
    finally:
        stop(p)

# ---------- (2) UI / AI 连接互斥 ----------
p, c, main = launch(18820, 1500)
try:
    L = Log(c)
    call(c, 'ui.silent', {'on': 1, 'yesno_default': 1})
    ai = rpc(18820)
    # A) UI 连接在途 → AI connect 立即 AGAIN
    u.PostMessageW(main, WM_COMMAND, IDC_BTN_CONNECT, 0)
    time.sleep(0.4)
    t0 = time.time(); r = call(ai, 'connect', {'device': 'TC375', 'attach': 1}); dt = time.time() - t0
    print('A ai reply', r, '%.0f ms' % (dt * 1000))
    s = json.dumps(r, ensure_ascii=False)
    check(not r.get('ok') and 'already in progress' in s and '(UI)' in s and dt < 0.3, 'UI 连接中 AI connect → AGAIN "connect already in progress (UI)"（%.0f ms）' % (dt * 1000))
    check(L.wait('已连接: ', 6) is not None, 'UI 连接仍正常完成（只有 UI 一方拿到）')
    st = call(c, 'status'); print('status', st)
    call(c, 'disconnect'); time.sleep(0.3)
    # B) AI 连接在途 → UI 点连接被忽略（日志“AI 客户端正在连接”），AI 成功
    box = {}
    th = threading.Thread(target=lambda: box.update(r=call(ai, 'connect', {'device': 'TC375', 'attach': 1}))); th.start()
    time.sleep(0.4)
    u.PostMessageW(main, WM_COMMAND, IDC_BTN_CONNECT, 0)
    check(L.wait('AI 客户端正在连接', 2) is not None, 'AI 连接中 UI 点连接 → 被忽略，日志“AI 客户端正在连接…”')
    th.join(10)
    print('B ai reply', box.get('r'))
    check(bool(box.get('r', {}).get('ok')), 'AI connect 成功（只有 AI 一方拿到）')
    nconn = sum(1 for x in L.pull() if '已连接' in x and 'slowdrv' in x)
    print('connected lines so far', nconn)
    call(c, 'disconnect'); time.sleep(0.3)
    # C) AI 连接在途 → UI 断开 → AI 收到 CANCELED
    box = {}
    th = threading.Thread(target=lambda: box.update(r=call(ai, 'connect', {'device': 'TC375', 'attach': 1}))); th.start()
    time.sleep(0.5)
    t0 = time.time(); u.PostMessageW(main, WM_COMMAND, IDC_BTN_DISCON, 0)
    th.join(5); dt = time.time() - t0
    print('C ai reply', box.get('r'), '%.0f ms' % (dt * 1000))
    s = json.dumps(box.get('r', {}), ensure_ascii=False)
    check(not box.get('r', {}).get('ok') and 'canceled' in s.lower(), 'AI 连接中 UI 断开 → AI 收到 CANCELED（%.0f ms）' % (dt * 1000))
    # D) 同时起跑 10 轮：每轮恰好一方连上，另一方 AGAIN/忽略
    bad = 0
    for i in range(10):
        call(c, 'disconnect'); time.sleep(0.2)
        box = {}
        th = threading.Thread(target=lambda: box.update(r=call(ai, 'connect', {'device': 'TC375', 'attach': 1})))
        u.PostMessageW(main, WM_COMMAND, IDC_BTN_CONNECT, 0); th.start()
        th.join(10)
        L.wait('已连接: ', 4)
        r = box.get('r', {}); s = json.dumps(r, ensure_ascii=False)
        ai_ok = bool(r.get('ok')); ai_again = 'already in progress' in s
        ui_ignored = any('AI 客户端正在连接' in x for x in L.lines[-50:])
        if not (ai_ok or ai_again): bad += 1; print('  round', i, r)
        L.lines.clear()
    check(bad == 0, '同时起跑 10 轮：每轮 AI 要么连上、要么 AGAIN（异常 %d 轮）' % bad)
    try: ai._ws.close()
    except Exception: pass
finally:
    stop(p)
print('RESULTS', json.dumps(res))
print('ALL PASS' if fails == 0 else 'FAILURES=%d' % fails)
sys.exit(fails)
