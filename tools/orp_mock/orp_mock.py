#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""ORP v1 mock 服务端（OpenScope Remote Protocol，见 docs/remote_debug_design.md）。

用法:
  python tools/orp_mock/orp_mock.py                    # ws://127.0.0.1:18766
  python tools/orp_mock/orp_mock.py --host 0.0.0.0     # 局域网监听（强制配对，本 mock 始终要求 token）
  python tools/orp_mock/orp_mock.py --fixed-code 123456 --port 18766
依赖: pip install websockets (>=12)
"""
from __future__ import annotations

import argparse
import asyncio
import json
import math
import os
import random
import re
import secrets
import socket
import struct
import time
import urllib.parse

import websockets

ORP_VER = 1
MAX_CLIENTS = 8
MAX_REQ = 256 * 1024 - 1
MAX_FRAME = 64 * 1024
HB_INTERVAL = 10.0  # 客户端心跳间隔
HB_TIMEOUT = 3 * HB_INTERVAL
SESSION_KEEP = 120.0
CONTROL_GRACE = 15.0  # 控制权持有者断线超过该时间自动释放
SIM_HZ = 1000
HIST_SEC = 60
PAIR_TTL = 300

# ---- 错误码（与文档 3.6 / aiapi 一致）----
E_PARSE, E_INVALID_REQ, E_NO_METHOD, E_PARAMS = -32700, -32600, -32601, -32602
E_FAIL, E_TOO_MANY, E_UNAUTH, E_FORBIDDEN, E_LOCKED, E_OFFLINE, E_EXPIRED = -32000, -32001, -32002, -32003, -32004, -32005, -32006
OS_ERR_INVALID_ARG, OS_ERR_NOT_CONNECTED = -2, -4
ROLE_RANK = {"viewer": 0, "operator": 1, "admin": 2}


class RpcError(Exception):
    def __init__(self, code, message, data=None):
        super().__init__(message)
        self.code, self.message, self.data = code, message, data


def now_us() -> int:
    return int(time.time() * 1_000_000)


# ---------------- 模拟目标 ----------------
class SimTarget:
    def __init__(self):
        self.vars = [
            {"id": 0, "name": "g_sine", "addr": "0x20000000", "size": 4, "type": "float", "writable": 0},
            {"id": 1, "name": "g_cosine", "addr": "0x20000004", "size": 4, "type": "float", "writable": 0},
            {"id": 2, "name": "g_saw", "addr": "0x20000008", "size": 4, "type": "float", "writable": 0},
            {"id": 3, "name": "g_square", "addr": "0x2000000C", "size": 4, "type": "float", "writable": 0},
            {"id": 4, "name": "g_noise", "addr": "0x20000010", "size": 4, "type": "float", "writable": 0},
            {"id": 5, "name": "g_counter", "addr": "0x20000014", "size": 4, "type": "uint32", "writable": 1},
            {"id": 6, "name": "g_cfg.gain", "addr": "0x20000018", "size": 4, "type": "float", "writable": 1},
            {"id": 7, "name": "g_cfg.freq_hz", "addr": "0x2000001C", "size": 4, "type": "float", "writable": 1},
            {"id": 8, "name": "g_cfg.offset", "addr": "0x20000020", "size": 4, "type": "float", "writable": 1},
        ]
        self.params = {6: 1.0, 7: 1.0, 8: 0.0}
        self.counter = 0
        self.t0 = time.time()
        self.hist = {v["id"]: [] for v in self.vars}  # list of (ts_us, value)
        self.latest = {}
        self.connected = True

    def find(self, id_=None, name=None):
        for v in self.vars:
            if (id_ is not None and v["id"] == id_) or (name and v["name"] == name):
                return v
        return None

    def value_at(self, vid, t):
        g, f, o = self.params[6], self.params[7], self.params[8]
        ph = 2 * math.pi * f * t
        if vid == 0: return g * math.sin(ph) + o
        if vid == 1: return g * math.cos(ph) + o
        if vid == 2: return g * (2 * ((f * t) % 1.0) - 1) + o
        if vid == 3: return g * (1.0 if math.sin(ph) >= 0 else -1.0) + o
        if vid == 4: return random.gauss(0, 0.1 * g) + o
        if vid == 5: return float(self.counter)
        return float(self.params.get(vid, 0.0))

    def step(self, n):
        """生成 n 个 1kHz 样本"""
        base = now_us() - (n - 1) * 1000
        for i in range(n):
            ts = base + i * 1000
            t = ts / 1e6 - self.t0
            self.counter = (self.counter + 1) & 0xFFFFFFFF
            for v in self.vars:
                val = self.value_at(v["id"], t)
                h = self.hist[v["id"]]
                h.append((ts, val))
                self.latest[v["id"]] = (ts, val)
        lim = SIM_HZ * HIST_SEC
        for h in self.hist.values():
            if len(h) > lim + 2000:
                del h[: len(h) - lim]

    def write(self, vid, value):
        v = self.find(id_=vid)
        if not v or not v["writable"]:
            raise RpcError(E_PARAMS, "variable not writable", {"os_err": OS_ERR_INVALID_ARG})
        if vid == 5:
            self.counter = int(value) & 0xFFFFFFFF
        else:
            self.params[vid] = float(value)


# ---------------- 二进制帧 ----------------
HDR = struct.Struct("<BBHIII")  # ver kind stream_id seq t0_hi t0_lo  = 16B


def build_frames(kind, stream_id, seq0, records, vt):
    """records: kind1 -> [(var_id, ts_us, value)]；kind2 -> [(var_id, ts_us, mn, mx, last)]
    返回 (frames, next_seq)；单帧 ≤ 64KB，t0=首记录 ts，dt=uint32 µs 偏移。"""
    vfmt = "f" if vt == "f32" else "d"
    rec = struct.Struct("<HI" + vfmt) if kind == 1 else struct.Struct("<HI" + vfmt * 3)
    per = (MAX_FRAME - HDR.size - 2) // rec.size
    frames, seq = [], seq0
    i = 0
    while i < len(records):
        chunk = records[i:i + per]
        t0 = min(r[1] for r in chunk)
        # dt 必须 < 2^32
        chunk = [r for r in chunk if r[1] - t0 < 0xFFFFFFFF]
        body = bytearray(HDR.pack(ORP_VER, kind, stream_id, seq & 0xFFFFFFFF, (t0 >> 32) & 0xFFFFFFFF, t0 & 0xFFFFFFFF))
        body += struct.pack("<H", len(chunk))
        for r in chunk:
            body += rec.pack(r[0], r[1] - t0, *r[2:])
        frames.append(bytes(body))
        seq += 1
        i += per
    return frames, seq


def minmax_buckets(samples_by_var, bucket_us):
    out = []
    for vid, ss in samples_by_var.items():
        if not ss:
            continue
        b0 = ss[0][0] - (ss[0][0] % bucket_us)
        cur = None
        for ts, v in ss:
            b = ts - (ts % bucket_us)
            if cur is None or b != cur[1]:
                if cur: out.append(tuple(cur))
                cur = [vid, b, v, v, v]
            else:
                cur[2] = min(cur[2], v); cur[3] = max(cur[3], v); cur[4] = v
        if cur: out.append(tuple(cur))
    return out


# ---------------- 服务端 ----------------
class Session:
    def __init__(self, sid, token, role, client, accept):
        self.sid, self.token, self.role, self.client = sid, token, role, client
        self.accept = accept
        self.ws = None
        self.subs = {}  # stream_id -> dict
        self.last_rx = time.time()
        self.detached_at = None
        self.seq = 0
        self.req_cache = {}


class Server:
    def __init__(self, args):
        self.args = args
        self.sim = SimTarget()
        self.device_id = "OS-MOCK-" + secrets.token_hex(3).upper()
        self.tokens = {}  # token -> {role, client, name}
        self.sessions = {}
        self.control = None  # sid
        self.control_since = None
        self.n_clients = 0
        self.next_stream = 1
        self.pair_code = None
        self.pair_exp = 0
        self.new_pair_code()

    # -- 配对 --
    def lan_addr(self):
        if self.args.host in ("0.0.0.0", ""):
            ips = []
            try:
                ips = socket.gethostbyname_ex(socket.gethostname())[2]
            except OSError:
                pass
            # 优先真实局域网私网地址（跳过 198.18.x 代理 TUN、169.254 链路本地）
            pri = [i for i in ips if i.startswith(("192.168.", "10.")) or (i.startswith("172.") and 16 <= int(i.split(".")[1]) <= 31)]
            ip = (pri or ips or ["127.0.0.1"])[0]
        else:
            ip = self.args.host
        return f"ws://{ip}:{self.args.port}"

    def new_pair_code(self):
        self.pair_code = self.args.fixed_code or f"{secrets.randbelow(1000000):06d}"
        self.pair_exp = time.time() + PAIR_TTL
        uri = "orp://pair?" + urllib.parse.urlencode({"d": self.device_id, "c": self.pair_code, "r": self.lan_addr()})
        print(f"[pair] 配对码 {self.pair_code}（{PAIR_TTL // 60} 分钟有效） 二维码内容: {uri}", flush=True)

    # -- 通用 --
    async def send_json(self, ws, obj):
        try:
            await ws.send(json.dumps(obj, ensure_ascii=False))
        except Exception:
            pass

    async def broadcast_control(self, reason):
        """控制权任何变化 → 推 control.changed 给所有在线会话，held_by_me 按接收方计算"""
        for x in list(self.sessions.values()):
            if x.ws:
                try: await self.notify(x, "control.changed", dict(self.control_info(x), reason=reason))
                except Exception: pass

    def control_changed(self, reason):
        asyncio.get_event_loop().create_task(self.broadcast_control(reason))

    async def notify(self, sess, method, params):
        if sess.ws:
            await self.send_json(sess.ws, {"jsonrpc": "2.0", "method": method, "params": params})

    def need_role(self, sess, role):
        if ROLE_RANK[sess.role] < ROLE_RANK[role]:
            raise RpcError(E_FORBIDDEN, f"permission denied: need {role}, have {sess.role}", {"need": role, "role": sess.role})

    def need_control(self, sess):
        if self.control != sess.sid:
            holder = self.sessions.get(self.control)
            raise RpcError(E_LOCKED, "control locked" if holder else "control not acquired",
                           {"holder": {"session_id": holder.sid, "client": holder.client} if holder else None})

    def var_of(self, p):
        if not isinstance(p, dict):
            raise RpcError(E_PARAMS, "params must be object", {"os_err": OS_ERR_INVALID_ARG})
        v = self.sim.find(id_=p.get("id") if isinstance(p.get("id"), int) else None, name=p.get("name"))
        if not v:
            raise RpcError(E_PARAMS, "id or name required/unknown", {"os_err": OS_ERR_INVALID_ARG})
        return v

    # -- 方法 --
    async def dispatch(self, ctx, method, p):
        sess = ctx.get("sess")
        if method == "ping":
            return {"pong": True, "t_us": now_us()}
        if method == "pair.claim":
            code = str(p.get("code", ""))
            if time.time() > self.pair_exp:
                self.new_pair_code()
                raise RpcError(E_UNAUTH, "pairing code expired")
            if code != self.pair_code:
                raise RpcError(E_UNAUTH, "invalid pairing code")
            tok = secrets.token_urlsafe(24)
            self.tokens[tok] = {"role": "viewer", "client": p.get("client", "?"), "name": p.get("name", "")}
            print(f"[pair] 已配对 client={p.get('client')} name={p.get('name','')} role=viewer", flush=True)
            if not self.args.fixed_code:
                self.new_pair_code()  # 一次性
            return {"token": tok, "role": "viewer", "device_id": self.device_id}
        if method == "session.hello":
            return await self.hello(ctx, p)
        if not sess:
            raise RpcError(E_UNAUTH, "session.hello with valid token required")
        if method == "session.bye":
            await self.drop_session(sess, "release"); return {}
        if method == "status":
            return {"device_id": self.device_id, "connected": int(self.sim.connected), "role": sess.role,
                    "control": self.control_info(sess), "sessions": len(self.sessions), "orp": ORP_VER}
        if method == "auth.elevate":
            role = p.get("role")
            if role not in ROLE_RANK:
                raise RpcError(E_PARAMS, "role must be viewer|operator|admin")
            if role == "admin" or (sess.client == "mp" and ROLE_RANK[role] > 1):
                raise RpcError(E_FORBIDDEN, "admin not grantable remotely", {"os_err": -6})
            print(f"[auth] {sess.client}:{sess.sid[:6]} 申请 {role} → 自动批准（mock）", flush=True)
            sess.role = role
            self.tokens[sess.token]["role"] = role
            return {"role": role}
        if method == "control.acquire":
            self.need_role(sess, "operator")
            if self.control and self.control != sess.sid and self.control in self.sessions:
                self.need_control(sess)
            changed = self.control != sess.sid
            if changed:
                self.control_since = now_us()
            self.control = sess.sid
            if changed: self.control_changed("acquire")
            return self.control_info(sess)
        if method == "control.release":
            if self.control == sess.sid:
                self.control = None; self.control_since = None
                self.control_changed("release")
            return dict(self.control_info(sess), released=True)
        if method == "vars.list":
            needle = str(p.get("needle", "") or "")
            off, lim = int(p.get("offset", 0) or 0), int(p.get("limit", 200) or 200)
            if off < 0 or lim < 0:
                raise RpcError(E_PARAMS, "offset/limit invalid")
            vs = [v for v in self.sim.vars if needle.lower() in v["name"].lower()]
            return {"total": len(vs), "vars": vs[off:off + lim]}
        if method == "vars.read":
            v = self.var_of(p)
            lt = self.sim.latest.get(v["id"])
            if not lt:
                return {"id": v["id"], "name": v["name"], "has_sample": 0}
            return {"id": v["id"], "name": v["name"], "has_sample": 1, "value": lt[1], "ts_us": lt[0]}
        if method == "vars.write":
            self.need_role(sess, "operator")
            self.need_control(sess)
            v = self.var_of(p)
            if "value" not in p:
                raise RpcError(E_PARAMS, "value required", {"os_err": OS_ERR_INVALID_ARG})
            rid = p.get("req_id")
            if rid is not None and rid in sess.req_cache:
                return dict(sess.req_cache[rid], duplicate=True)
            if not self.sim.connected:
                raise RpcError(E_FAIL, "not connected", {"os_err": OS_ERR_NOT_CONNECTED})
            try:
                val = float(p["value"])
            except (TypeError, ValueError):
                raise RpcError(E_PARAMS, "value must be number", {"os_err": OS_ERR_INVALID_ARG})
            self.sim.write(v["id"], val)
            res = {"id": v["id"], "value": val, "req_id": rid, "duplicate": False}
            if rid is not None:
                sess.req_cache[rid] = res
            print(f"[write] {v['name']} = {val} by {sess.client}:{sess.sid[:6]}", flush=True)
            return res
        if method == "stream.subscribe":
            return await self.subscribe(sess, p)
        if method == "stream.unsubscribe":
            sid = p.get("stream_id")
            if sid is None: sess.subs.clear()
            else: sess.subs.pop(sid, None)
            return {"streams": list(sess.subs)}
        if method == "history.fetch":
            return await self.history(sess, p)
        if method in ("flash.download", "flash.erase", "mem.write"):
            raise RpcError(E_FORBIDDEN, "remote flashing/memory write disabled")
        raise RpcError(E_NO_METHOD, f"unknown method: {method}")

    async def hello(self, ctx, p):
        tok = p.get("token")
        if not tok or tok not in self.tokens:
            raise RpcError(E_UNAUTH, "unpaired or token invalid")
        client = p.get("client", "web")
        acc = [k for k in (p.get("accept_kinds") or [1, 2]) if k in (1, 2)]  # mock 不实现 kind=3
        if not acc:
            raise RpcError(E_PARAMS, "accept_kinds must include 1 or 2")
        rs = p.get("resume_session_id")
        if rs:
            old = self.sessions.get(rs)
            if not old or old.token != tok:
                raise RpcError(E_EXPIRED, "session expired; re-subscribe")
            if old.ws and old.ws is not ctx["ws"]:
                try: await old.ws.close()
                except Exception: pass
            old.accept = acc
            for st in list(old.subs.values()):  # 先在新连接上补发 meta，再挂接（推流只看 old.ws）
                await self.send_json(ctx["ws"], {"jsonrpc": "2.0", "method": "stream.meta", "params": self.meta(st)})
            old.ws, old.detached_at, old.last_rx = ctx["ws"], None, time.time()
            ctx["sess"] = old
            return {"session_id": old.sid, "device": self.device_info(), "role": old.role,
                    "accept_kinds": acc, "resumed": True, "caps": self.caps(old),
                    "control": self.control_info(old), "streams": list(old.subs)}
        sid = secrets.token_hex(8)
        s = Session(sid, tok, self.tokens[tok]["role"], client, acc)
        s.ws = ctx["ws"]
        self.sessions[sid] = s
        ctx["sess"] = s
        return {"session_id": sid, "device": self.device_info(), "role": s.role, "accept_kinds": acc,
                "resumed": False, "caps": self.caps(s), "control": self.control_info(s)}

    def control_info(self, s):
        h = self.sessions.get(self.control) if self.control else None
        return {"held_by_me": bool(h and h.sid == s.sid),
                "holder": {"session_id": h.sid, "client": h.client} if h else None,
                "since": self.control_since if h else None}

    def device_info(self):
        return {"device_id": self.device_id, "name": "OpenScope ORP mock", "probe": "SIM", "target": "SIM-Cortex-M4",
                "connected": int(self.sim.connected), "voltage_mv": 3300}

    def caps(self, s):
        c = {"max_frame": MAX_FRAME, "max_request": MAX_REQ, "modes": ["latest", "minmax", "raw"],
             "sim_hz": SIM_HZ, "flash": False, "heartbeat_s": HB_INTERVAL}
        if s.client == "mp":
            c.update({"max_rate_hz": 20, "max_vars": 4})
        return c

    def meta(self, st):
        return {"stream_id": st["id"], "mode": st["mode"], "rate_hz": st["rate_hz"], "kind": st["kind"],
                "vars": [{"var_id": v, "name": self.sim.find(id_=v)["name"], "value_type": st["vt"]} for v in st["vars"]]}

    async def subscribe(self, sess, p):
        ids = p.get("vars")
        if not isinstance(ids, list) or not ids:
            raise RpcError(E_PARAMS, "vars required", {"os_err": OS_ERR_INVALID_ARG})
        vids = []
        for x in ids:
            v = self.sim.find(id_=x if isinstance(x, int) else None, name=x if isinstance(x, str) else None)
            if not v:
                raise RpcError(E_PARAMS, f"unknown var {x}", {"os_err": OS_ERR_INVALID_ARG})
            vids.append(v["id"])
        mode = p.get("mode") or ("minmax" if sess.client == "mp" else "latest")
        if mode not in ("latest", "minmax", "raw"):
            raise RpcError(E_PARAMS, "mode must be latest|minmax|raw")
        rate = float(p.get("rate_hz") or (10 if sess.client == "mp" else 30))
        if rate <= 0 or rate > 200:
            raise RpcError(E_PARAMS, "rate_hz out of range (0,200]")
        if sess.client == "mp":
            rate = min(rate, 20.0); vids = vids[:4]
        vt = p.get("value_type") or "f32"
        if vt not in ("f32", "f64"):
            raise RpcError(E_PARAMS, "value_type must be f32|f64")
        kind = 2 if mode == "minmax" else 1
        if kind not in sess.accept:
            raise RpcError(E_PARAMS, f"mode {mode} needs kind {kind} not in accept_kinds")
        st = {"id": self.next_stream, "vars": vids, "mode": mode, "rate_hz": rate, "vt": vt, "kind": kind,
              "last_ts": now_us(), "next_t": time.time()}
        self.next_stream = (self.next_stream % 65535) + 1
        await self.notify(sess, "stream.meta", self.meta(st))  # 先 meta，再挂到推流表
        sess.subs[st["id"]] = st
        return {"stream_id": st["id"], "mode": mode, "rate_hz": rate, "vars": vids, "value_type": vt, "kind": kind}

    async def history(self, sess, p):
        ids = p.get("vars") or []
        mode = p.get("mode", "raw")
        if mode not in ("raw", "minmax") or not ids:
            raise RpcError(E_PARAMS, "vars required; mode raw|minmax")
        t1 = int(p.get("t1") or now_us()); t0 = int(p.get("t0") or (t1 - 5_000_000))
        mp = int(p.get("max_points") or 1000)
        kind = 2 if mode == "minmax" else 1
        if kind not in sess.accept:
            raise RpcError(E_PARAMS, f"kind {kind} not accepted")
        vt = p.get("value_type") or "f32"
        if vt not in ("f32", "f64"):
            raise RpcError(E_PARAMS, "value_type must be f32|f64")
        sel, metas = {}, []
        for x in ids:
            v = self.sim.find(id_=x if isinstance(x, int) else None, name=x if isinstance(x, str) else None)
            if not v:
                raise RpcError(E_PARAMS, f"unknown var {x}", {"os_err": OS_ERR_INVALID_ARG})
            sel[v["id"]] = [y for y in self.sim.hist[v["id"]] if t0 <= y[0] <= t1]
            metas.append({"var_id": v["id"], "name": v["name"], "type": v["type"], "value_type": vt})
        sid = 0  # history 帧 stream_id=0
        if kind == 2:
            span = max(1, t1 - t0)
            recs = minmax_buckets(sel, max(1000, span // max(1, mp)))
        else:
            recs = []
            for vid, ss in sel.items():
                step = max(1, len(ss) // max(1, mp))
                recs += [(vid, ts, v) for ts, v in ss[::step]]
        frames, sess.seq = build_frames(kind, sid, sess.seq, recs, vt)
        for f in frames:
            await sess.ws.send(f)
        return {"frames": len(frames), "records": len(recs), "kind": kind, "stream_id": 0, "t0": t0, "t1": t1,
                "value_type": vt, "vars": metas}

    async def drop_session(self, sess, reason="expired"):
        held = self.control == sess.sid
        if held:
            self.control = None; self.control_since = None
        self.sessions.pop(sess.sid, None)
        if held: await self.broadcast_control(reason)

    # -- 连接 --
    async def handler(self, ws):
        if self.n_clients >= MAX_CLIENTS:
            await self.send_json(ws, {"jsonrpc": "2.0", "id": None, "error": {"code": E_TOO_MANY, "message": f"too many clients (max {MAX_CLIENTS})"}})
            await ws.close(code=1013, reason="too many clients"); return
        self.n_clients += 1
        ctx = {"ws": ws, "sess": None}
        try:
            async for raw in ws:
                if ctx["sess"]:
                    ctx["sess"].last_rx = time.time()
                if isinstance(raw, bytes):
                    await self.send_json(ws, {"jsonrpc": "2.0", "id": None, "error": {"code": E_INVALID_REQ, "message": "binary requests not supported"}})
                    continue
                if len(raw.encode("utf-8")) > MAX_REQ:
                    m = re.search(r'"id"\s*:\s*(-?\d+|"(?:[^"\\]|\\.){0,128}")', raw[:4096])
                    big_id = json.loads(m.group(1)) if m else None
                    await self.send_json(ws, {"jsonrpc": "2.0", "id": big_id, "error": {"code": E_INVALID_REQ, "message": f"request too large (max {MAX_REQ} bytes)"}})
                    continue
                try:
                    req = json.loads(raw)
                except ValueError:
                    await self.send_json(ws, {"jsonrpc": "2.0", "id": None, "error": {"code": E_PARSE, "message": "parse error"}})
                    continue
                if not isinstance(req, dict) or not isinstance(req.get("method"), str):
                    await self.send_json(ws, {"jsonrpc": "2.0", "id": req.get("id") if isinstance(req, dict) else None,
                                              "error": {"code": E_INVALID_REQ, "message": "method required"}})
                    continue
                rid = req.get("id")
                p = req.get("params") or {}
                try:
                    if not isinstance(p, dict):
                        raise RpcError(E_PARAMS, "params must be object")
                    res = await self.dispatch(ctx, req["method"], p)
                    if rid is not None:
                        await self.send_json(ws, {"jsonrpc": "2.0", "id": rid, "result": res})
                except RpcError as e:
                    err = {"code": e.code, "message": e.message}
                    if e.data is not None: err["data"] = e.data
                    if rid is not None:
                        await self.send_json(ws, {"jsonrpc": "2.0", "id": rid, "error": err})
        except websockets.ConnectionClosed:
            pass
        finally:
            self.n_clients -= 1
            s = ctx["sess"]
            if s and s.ws is ws:
                s.ws = None
                s.detached_at = time.time()

    async def sim_loop(self):
        last = time.time()
        while True:
            await asyncio.sleep(0.01)
            t = time.time()
            n = int((t - last) * SIM_HZ)
            if n <= 0: continue
            last += n / SIM_HZ
            self.sim.step(min(n, 2000))

    async def push_loop(self):
        while True:
            await asyncio.sleep(0.005)
            t = time.time()
            for s in list(self.sessions.values()):
                # 心跳与会话保留
                if s.ws and t - s.last_rx > HB_TIMEOUT:
                    print(f"[hb] {s.client}:{s.sid[:6]} 心跳超时，断开（会话保留 {SESSION_KEEP:.0f}s）", flush=True)
                    try: await s.ws.close(code=4000, reason="heartbeat timeout")
                    except Exception: pass
                    s.last_rx = t  # 避免关闭完成前每轮重复关闭/刷日志
                if not s.ws:
                    if s.detached_at and self.control == s.sid and t - s.detached_at > CONTROL_GRACE:
                        print(f"[ctl] {s.client}:{s.sid[:6]} 断线超过 {CONTROL_GRACE:.0f}s，自动释放控制权", flush=True)
                        self.control = None; self.control_since = None
                        await self.broadcast_control("auto_release")
                    if s.detached_at and t - s.detached_at > SESSION_KEEP:
                        await self.drop_session(s)
                    continue
                for st in list(s.subs.values()):
                    if t < st["next_t"]: continue
                    st["next_t"] = t + 1.0 / st["rate_hz"]
                    now = now_us(); since = st["last_ts"]; st["last_ts"] = now
                    if st["mode"] == "latest":
                        recs = [(v, *self.sim.latest[v]) for v in st["vars"] if v in self.sim.latest]
                    else:
                        sel = {v: [x for x in self.sim.hist[v][-(SIM_HZ * 2):] if since < x[0] <= now] for v in st["vars"]}
                        if st["mode"] == "raw":
                            recs = [(v, ts, val) for v, ss in sel.items() for ts, val in ss]
                        else:
                            recs = minmax_buckets(sel, max(1000, int(1e6 / st["rate_hz"])))
                    if not recs: continue
                    frames, s.seq = build_frames(st["kind"], st["id"], s.seq, recs, st["vt"])
                    for f in frames:
                        try: await s.ws.send(f)
                        except Exception: break

    async def main(self):
        print(f"[orp-mock] ORP v{ORP_VER} 监听 ws://{self.args.host}:{self.args.port}  device_id={self.device_id}", flush=True)
        if self.args.host != "127.0.0.1":
            print("[orp-mock] 局域网监听：所有连接必须先配对（pair.claim）再 session.hello", flush=True)
        async with websockets.serve(self.handler, self.args.host, self.args.port, max_size=4 * 1024 * 1024,
                                    ping_interval=None, subprotocols=["orp.v1"], select_subprotocol=lambda c, s: "orp.v1" if "orp.v1" in s else None):
            asyncio.get_running_loop().create_task(self.sim_loop())
            asyncio.get_running_loop().create_task(self.push_loop())
            while True:
                await asyncio.sleep(5)
                if time.time() > self.pair_exp:
                    self.new_pair_code()


def main():
    ap = argparse.ArgumentParser(description="ORP v1 mock server")
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=18766)
    ap.add_argument("--fixed-code", default=os.environ.get("ORP_MOCK_CODE"), help="固定配对码（测试用，可重复配对）")
    args = ap.parse_args()
    if args.fixed_code and (len(args.fixed_code) != 6 or not args.fixed_code.isdigit()):
        ap.error("--fixed-code must be 6 digits")
    try:
        asyncio.run(Server(args).main())
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
