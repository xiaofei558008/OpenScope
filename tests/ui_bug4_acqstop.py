# -*- coding: utf-8 -*-
"""Bug4（续）：停止采集时 UI 最多等 ~200 ms（OS_DS_STOP_WAIT_MS），采集线程卡在驱动读里也不例外；
残留线程退出前拒绝重新开始，退出后可以重新开始。
slowdrv 每次 READ_MEM 阻塞 SLOWDRV_READ_MS（默认 1500 ms）。无鼠标/键盘输入：只用 RPC + SendMessageTimeout。
用法: python tests\\ui_bug4_acqstop.py [OpenScope.exe]
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
shutil.copy(os.path.join(ROOT, 'tests', 'bin', 'test_layout.ini'), LAY)
READ_MS = 1500
env = dict(os.environ, OPENSCOPE_LAYOUT=LAY, OPENSCOPE_REMOTE_DISABLE='1', SLOWDRV_MS='0',
           SLOWDRV_MODE='again', SLOWDRV_READ_MS=str(READ_MS))
env.pop('SLOWDRV_STATS', None)
u = ctypes.windll.user32
PORT = 18782
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
        try: r = c.call(m, a or {})
        except RuntimeError as e: return {'ok': False, 'error': str(e)}   # RPC 错误（如 acq start failed）
        return r if isinstance(r, dict) else json.loads(r)
    CB = ctypes.WINFUNCTYPE(ctypes.c_bool, wintypes.HWND, wintypes.LPARAM)
    found = []
    def f(h, l):
        q = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(q))
        b = ctypes.create_unicode_buffer(64); u.GetClassNameW(h, b, 64)
        if q.value == p.pid and b.value == 'OpenScopeMain': found.append(h)
        return True
    u.EnumWindows(CB(f), 0)
    main = found[0]
    call('drivers.select', {'index': 0})
    r = call('connect', {'device': 'X', 'iface': 0, 'speed_khz': 1000, 'probe_index': 0, 'attach': 0})
    check(bool(r.get('ok')), 'connect ok')
    r = call('elf.load', {'path': os.path.join(ROOT, 'tests', 'elf_sample.out')})
    check(bool(r.get('ok')), 'elf.load ok')
    vl = call('vars.list', {'limit': 50})
    leaves = [v for v in vl.get('vars', vl.get('items', [])) if v.get('size', 0) in (1, 2, 4)]
    check(bool(leaves), 'found a scalar variable')
    r = call('vars.watch', {'id': leaves[0]['id'], 'on': 1})
    check(r.get('watch_count', 0) >= 1, 'watch added')
    r = call('acq.start')
    check(bool(r.get('ok')), 'acq.start ok')
    time.sleep(READ_MS / 1000.0 / 3)   # 采集线程此时卡在一次 1.5 s 的 READ_MEM 里
    # 停止期间同时测 UI 线程延迟
    worst = [0.0]; stop_flag = [False]
    def probe():
        res = ctypes.c_size_t()
        while not stop_flag[0]:
            t0 = time.time()
            u.SendMessageTimeoutW(main, 0, 0, 0, 0x0002, 5000, ctypes.byref(res))
            worst[0] = max(worst[0], time.time() - t0)
            time.sleep(0.01)
    th = threading.Thread(target=probe); th.start()
    t0 = time.time(); r = call('acq.stop'); dt = time.time() - t0
    time.sleep(0.3); stop_flag[0] = True; th.join()
    print('acq.stop %.0f ms, UI worst %.0f ms' % (dt * 1000, worst[0] * 1000))
    check(dt < 0.35, '停止采集 RPC 返回 %.0f ms（<= ~200 ms 等待 + RPC 开销）' % (dt * 1000))
    check(worst[0] < 0.35, '停止时 UI 线程最坏 %.0f ms' % (worst[0] * 1000))
    st = call('acq.state'); check(st.get('acq_state') == 0, '状态已为停止')
    r = call('acq.start')
    check(not r.get('ok'), '残留采集线程未退出时拒绝重新开始')
    time.sleep(READ_MS / 1000.0 + 0.5)
    r = call('acq.start')
    check(bool(r.get('ok')), '残留线程退出后可重新开始')
    time.sleep(0.2)
    call('acq.stop')
    check(not u.IsHungAppWindow(main), '窗口未被系统判定为未响应')
    check(p.poll() is None, '进程未退出')
finally:
    p.terminate()
    try: p.wait(10)
    except Exception: pass
print('ALL PASS' if fails == 0 else 'FAILURES=%d' % fails)
sys.exit(fails)
