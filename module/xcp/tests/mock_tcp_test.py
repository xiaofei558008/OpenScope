#!/usr/bin/env python3
"""端到端：启动 xcp_mock_slave（XCP-on-TCP），用最小 Python 主机验证 CONNECT、BUSY 注入、
Seed&Key、1kHz DAQ（时间戳、丢包注入可由计数器检测）。用法：mock_tcp_test.py <xcp_mock_slave 路径>"""
import socket, struct, subprocess, sys, time

exe = sys.argv[1]
_t = socket.socket(); _t.bind(("127.0.0.1", 0)); port = _t.getsockname()[1]; _t.close()
p = subprocess.Popen([exe, "--port", str(port), "--seconds", "10", "--lock", "daq",
                      "--fault", "busy:FD:1", "--fault", "drop:10"], stdout=subprocess.PIPE)
import atexit; atexit.register(lambda: (p.kill(), p.wait()))
if not p.stdout.readline().startswith(b"xcp_mock_slave listening"): sys.exit("mock failed to start")
s = socket.create_connection(("127.0.0.1", port), timeout=2)
ctr = 0
buf = b""
fails = 0

def check(ok, what):
    global fails
    print(("PASS " if ok else "FAIL ") + what)
    fails += not ok

def send(b):
    global ctr
    s.sendall(struct.pack("<HH", len(b), ctr) + bytes(b)); ctr += 1

def recv_pkt():
    global buf
    while True:
        if len(buf) >= 4:
            n, c = struct.unpack_from("<HH", buf)
            if len(buf) >= 4 + n:
                pkt = buf[4:4 + n]; buf = buf[4 + n:]
                return c, pkt
        buf += s.recv(4096)

def cmd(b):
    send(b)
    while True:
        _, r = recv_pkt()
        if r[0] in (0xFF, 0xFE):
            return r

r = cmd([0xFF, 0]); check(r[0] == 0xFF and r[3] == 64, "TCP CONNECT")
r = cmd([0xFD]); check(r[:2] == b"\xfe\x10", "注入 ERR_CMD_BUSY")
r = cmd([0xFD]); check(r[0] == 0xFF and r[2] & 0x04, "重试成功，DAQ 资源已锁")
r = cmd([0xD6]); check(r[0] == 0xFF, "FREE_DAQ")
r = cmd([0xD5, 0, 1, 0]); r = cmd([0xD4, 0, 0, 0, 1]); r = cmd([0xD3, 0, 0, 0, 0, 1])
r = cmd([0xE2, 0, 0, 0, 0, 0]); r = cmd([0xE1, 0xFF, 4, 0] + list(struct.pack("<I", 0x20000000)))
check(r[:2] == b"\xfe\x25", "DAQ 上锁时 WRITE_DAQ → ACCESS_LOCKED")
r = cmd([0xF8, 0, 0x04]); seed = r[2:2 + r[1]]
r = cmd([0xF7, len(seed)] + [x ^ 0xFF for x in seed]); check(r[0] == 0xFF and not r[1] & 0x04, "Seed&Key 解锁 DAQ")
r = cmd([0xE2, 0, 0, 0, 0, 0]); r = cmd([0xE1, 0xFF, 4, 0] + list(struct.pack("<I", 0x20000000)))
check(r[0] == 0xFF, "解锁后 WRITE_DAQ")
r = cmd([0xE0, 0x10, 0, 0, 0, 0, 1, 0])
r = cmd([0xDE, 2, 0, 0]); check(r[0] == 0xFF, "选择 DAQ0")
r = cmd([0xDD, 1]); check(r[0] == 0xFF, "START_STOP_SYNCH")
t_end = time.time() + 0.5
samples = []
last_ctr = None; ctr_gaps = 0
while time.time() < t_end:
    c, pkt = recv_pkt()
    if last_ctr is not None and c != (last_ctr + 1) & 0xFFFF: ctr_gaps += 1
    last_ctr = c
    if pkt[0] == 0:
        ts, val = struct.unpack_from("<II", pkt, 1); samples.append((ts, val))
r = cmd([0xDD, 0])
check(300 <= len(samples) <= 600, f"0.5s 收到 {len(samples)} 个 1kHz 样本")
gaps = sum(1 for a, b in zip(samples, samples[1:]) if b[1] - a[1] != 1)
check(gaps > 0, f"计数器检测到 {gaps} 处丢包（drop:10 注入）")
check(all(b[0] > a[0] for a, b in zip(samples, samples[1:])), "时间戳单调递增")
check(ctr_gaps == 0, "TCP 头 CTR 连续")
cmd([0xFE]); s.close(); p.terminate(); p.wait()
print("mock_tcp_test:", "ALL PASS" if not fails else f"{fails} FAIL")
sys.exit(1 if fails else 0)
