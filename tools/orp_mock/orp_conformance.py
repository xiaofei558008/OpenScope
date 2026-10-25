#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""ORP v1 协议一致性测试（mock 与 remote.dll 共用）。

  python tools/orp_mock/orp_conformance.py --url ws://127.0.0.1:18766 --code 123456 [--sim]
--sim: 额外校验 mock 的模拟波形（正弦）数值；真机测试不加。
--write-var: 用于写值测试的变量名（默认 g_cfg.gain）
依赖: pip install websocket-client
"""
import argparse, json, socket, struct, sys, time, itertools
import websocket

HDR = struct.Struct("<BBHIII")
A = None
RESULTS = []
CASES = []
_ids = itertools.count(1)


class Cli:
    def __init__(self, url):
        self.ws = websocket.create_connection(url, timeout=5, subprotocols=["orp.v1"])
        self.notes, self.bins = [], []

    def raw(self):
        op, data = self.ws.recv_data()
        if op == websocket.ABNF.OPCODE_BINARY:
            self.bins.append(data); return None
        return json.loads(data.decode("utf-8"))

    def call(self, method, params=None, timeout=5):
        rid = next(_ids)
        self.ws.send(json.dumps({"jsonrpc": "2.0", "id": rid, "method": method, "params": params or {}}))
        end = time.time() + timeout
        while time.time() < end:
            m = self.raw()
            if m is None: continue
            if m.get("id") == rid: return m
            if "method" in m: self.notes.append(m)
        raise TimeoutError(method)

    def ok(self, method, params=None):
        m = self.call(method, params)
        if "error" in m: raise AssertionError(f"{method} -> {m['error']}")
        return m["result"]

    def err(self, method, params, code):
        m = self.call(method, params)
        e = m.get("error")
        if not e or e.get("code") != code:
            raise AssertionError(f"{method} expect error {code}, got {m}")
        return e

    def collect(self, seconds):
        self.bins.clear(); end = time.time() + seconds
        self.ws.settimeout(0.2)
        while time.time() < end:
            try:
                m = self.raw()
                if m and "method" in m: self.notes.append(m)
            except websocket.WebSocketTimeoutException:
                pass
        self.ws.settimeout(5)
        return list(self.bins)

    def close(self):
        try: self.ws.close()
        except Exception: pass


def parse(frame, vt="f32"):
    ver, kind, sid, seq, hi, lo = HDR.unpack_from(frame, 0)
    n, = struct.unpack_from("<H", frame, 16)
    vf = "f" if vt == "f32" else "d"
    rec = struct.Struct("<HI" + (vf if kind == 1 else vf * 3))
    assert len(frame) == 18 + n * rec.size, f"frame len {len(frame)} != 18+{n}*{rec.size}"
    assert len(frame) <= 65536
    t0 = (hi << 32) | lo
    out = [rec.unpack_from(frame, 18 + i * rec.size) for i in range(n)]
    return {"ver": ver, "kind": kind, "sid": sid, "seq": seq, "t0": t0, "recs": out}


def case(name):
    def deco(f):
        def run():
            t = time.time()
            try:
                f(); RESULTS.append((name, True, "")); print(f"PASS {name} ({time.time()-t:.2f}s)", flush=True)
            except Exception as e:
                RESULTS.append((name, False, repr(e))); print(f"FAIL {name}: {e!r}", flush=True)
        CASES.append(run)
        return run
    return deco


def hello(c, tok, client="web", accept=(1, 2), **kw):
    return c.ok("session.hello", dict({"token": tok, "client": client, "accept_kinds": list(accept), "orp": 1}, **kw))


def main():
    global A
    ap = argparse.ArgumentParser()
    ap.add_argument("--url", default="ws://127.0.0.1:18766")
    ap.add_argument("--code", required=True)
    ap.add_argument("--sim", action="store_true")
    ap.add_argument("--write-var", default="g_cfg.gain")
    ap.add_argument("--write-value", type=float, default=1.5, help="写入测试值（整型变量请给整数，如 4660）")
    ap.add_argument("--stream-vars", default="", help="逗号分隔变量名，默认取 vars.list 前 2 个")
    A = ap.parse_args()
    st = {}

    def pair(client="web"):
        c = Cli(A.url)
        r = c.ok("pair.claim", {"code": A.code, "client": client, "name": "conformance"})
        c.close(); return r["token"]

    @case("ping_before_hello")
    def _():
        c = Cli(A.url); r = c.ok("ping"); assert r["pong"]; c.close()

    @case("unauth_before_hello")
    def _():
        c = Cli(A.url); c.err("vars.list", {}, -32002); c.close()

    @case("bad_pair_code")
    def _():
        c = Cli(A.url); bad = "000000" if A.code != "000000" else "111111"
        c.err("pair.claim", {"code": bad, "client": "web"}, -32002); c.close()

    @case("bad_token")
    def _():
        c = Cli(A.url); c.err("session.hello", {"token": "nope", "client": "web", "accept_kinds": [1, 2]}, -32002); c.close()

    @case("pair_and_hello")
    def _():
        st["tok"] = pair()
        c = Cli(A.url); r = hello(c, st["tok"])
        assert r["session_id"] and r["role"] == "viewer" and r["resumed"] is False
        assert set(r["control"]) >= {"held_by_me", "holder", "since"} and r["control"]["held_by_me"] is False
        assert r["caps"].get("heartbeat_s") == 10
        assert set(r["accept_kinds"]) <= {1, 2}
        st["c"] = c; st["sid"] = r["session_id"]

    @case("protocol_errors")
    def _():
        c = st["c"]
        c.err("no.such.method", {}, -32601)
        c.err("vars.read", {"name": "__nope__"}, -32602)
        c.ws.send("{bad json"); m = c.raw()
        while m is None: m = c.raw()
        assert m["error"]["code"] == -32700
        c.err("stream.subscribe", {"vars": []}, -32602)
        c.err("flash.download", {}, -32003)

    @case("request_too_large")
    def _():
        c = st["c"]; rid = next(_ids)
        c.ws.send(json.dumps({"jsonrpc": "2.0", "id": rid, "method": "ping", "params": {"pad": "x" * 300000}}))
        m = c.raw()
        while m is None or "method" in m: m = c.raw()
        assert m["error"]["code"] == -32600 and "too large" in m["error"]["message"] and m.get("id") == rid, m
        assert c.ok("ping")["pong"]

    @case("vars_list")
    def _():
        r = st["c"].ok("vars.list", {"limit": 50})
        assert r["total"] >= 1 and r["vars"] and {"id", "name", "addr", "size", "type", "writable"} <= set(r["vars"][0])
        p2 = st["c"].ok("vars.list", {"offset": 1, "limit": 1}); assert len(p2["vars"]) == 1 and p2["vars"][0]["id"] == r["vars"][1]["id"] and p2["total"] == r["total"]
        st["vars"] = r["vars"]
        r2 = st["c"].ok("vars.list", {"needle": A.write_var, "limit": 5})
        assert any(v["name"] == A.write_var for v in r2["vars"]), "write var not found"

    @case("write_needs_operator")
    def _():
        st["c"].err("vars.write", {"name": A.write_var, "value": 1.0}, -32003)
        st["c"].err("control.acquire", {}, -32003)

    @case("elevate_and_control")
    def _():
        c = st["c"]
        assert c.ok("auth.elevate", {"role": "operator"})["role"] == "operator"
        c.err("vars.write", {"name": A.write_var, "value": 1.0}, -32004)
        ci = c.ok("control.acquire"); assert ci["held_by_me"] is True and ci["holder"]["client"] == "web" and ci["holder"]["session_id"] == st["sid"] and ci["since"] > 0, ci
        t2 = pair(); c2 = Cli(A.url); hello(c2, t2)
        c2.ok("auth.elevate", {"role": "operator"})
        e = c2.err("control.acquire", {}, -32004); assert (e.get("data") or {}).get("holder", {}).get("session_id") == st["sid"], e
        c2.err("auth.elevate", {"role": "admin"}, -32003)
        c2.err("vars.write", {"name": A.write_var, "value": 2.0}, -32004)
        c2.close()

    @case("write_and_readback_idempotent")
    def _():
        try:
            _write_case()
        finally:
            try: st["c"].ok("vars.write", {"name": A.write_var, "value": st.get("orig", 1.0)})  # 复位
            except Exception: pass

    def _write_case():
        c = st["c"]
        o = c.ok("vars.read", {"name": A.write_var})
        if o.get("has_sample"): st["orig"] = o["value"]
        wv = A.write_value
        r = c.ok("vars.write", {"name": A.write_var, "value": wv, "req_id": "w-1"})
        assert r["duplicate"] is False and r["req_id"] == "w-1" and isinstance(r["id"], int) and abs(r["value"] - wv) < 1e-6, r
        r2 = c.ok("vars.write", {"name": A.write_var, "value": wv, "req_id": "w-1"})
        assert r2.get("duplicate") is True and r2.get("req_id") == "w-1" and r2["id"] == r["id"]
        time.sleep(0.3)
        rd = c.ok("vars.read", {"name": A.write_var})
        assert rd["has_sample"] == 1 and abs(rd["value"] - wv) < 1e-3, rd
        c.ok("vars.write", {"name": A.write_var, "value": st.get("orig", 1.0), "req_id": "w-2"})

    def SN(n):
        """流测试变量名：--stream-vars 优先（真机用），否则取 vars.list 前 n 个"""
        return (A.stream_vars.split(",") if A.stream_vars else [v["name"] for v in st["vars"]])[:n]

    def sub_check(mode, rate, kind, vt="f32", secs=1.2):
        c = st["c"]
        names = SN(2)
        c.notes.clear()
        r = c.ok("stream.subscribe", {"vars": names, "mode": mode, "rate_hz": rate, "value_type": vt})
        r["_t_sub"] = time.time()
        assert r["kind"] == kind and all(isinstance(v, int) for v in r["vars"]), r
        frames = [parse(f, vt) for f in c.collect(secs)]
        c.ok("stream.unsubscribe", {"stream_id": r["stream_id"]})
        meta = [n for n in c.notes if n["method"] == "stream.meta" and n["params"]["stream_id"] == r["stream_id"]]
        assert meta and meta[0]["params"]["vars"][0]["value_type"] == vt
        fr = [f for f in frames if f["sid"] == r["stream_id"]]
        assert fr, "no frames"
        for f in fr:
            assert f["ver"] == 1 and f["kind"] == kind
            assert abs(f["t0"] / 1e6 - time.time()) < 30, "t0 not unix-us"
        seqs = [f["seq"] for f in fr]
        assert all(b > a for a, b in zip(seqs, seqs[1:])), "seq not increasing"
        return r, fr

    @case("meta_before_frames")
    def _():
        c = st["c"]; seen_meta = set(); order_ok = True
        names = SN(2)
        rid = next(_ids)
        c.ws.send(json.dumps({"jsonrpc": "2.0", "id": rid, "method": "stream.subscribe", "params": {"vars": names, "mode": "latest", "rate_hz": 50}}))
        sid = None; end = time.time() + 1.5
        while time.time() < end:
            op, data = c.ws.recv_data()
            if op == websocket.ABNF.OPCODE_BINARY:
                fsid = HDR.unpack_from(data, 0)[2]
                if fsid != 0 and fsid not in seen_meta: order_ok = False
            else:
                m = json.loads(data)
                if m.get("method") == "stream.meta": seen_meta.add(m["params"]["stream_id"])
                if m.get("id") == rid: sid = m["result"]["stream_id"]
        c.ok("stream.unsubscribe", {})
        assert sid in seen_meta and order_ok

    @case("stream_latest_f32")
    def _():
        r, fr = sub_check("latest", 20, 1)
        assert 10 <= len(fr) <= 40, f"latest@20Hz frames={len(fr)}"
        if A.sim:
            # 跳过订阅前 100ms 内（上一用例改 gain 之前）的样本
            tmin = r["_t_sub"] + 0.1
            vid = r["vars"][0]; vals = [x[2] for f in fr for x in f["recs"] if x[0] == vid and (f["t0"] + x[1]) / 1e6 > tmin]
            assert max(vals) - min(vals) > 0.5 and all(abs(v) <= 1.01 for v in vals), ("sine range", min(vals), max(vals))

    @case("stream_raw_f64")
    def _():
        r, fr = sub_check("raw", 20, 1, "f64")
        ts = sorted({f["t0"] + x[1] for f in fr for x in f["recs"] if x[0] == r["vars"][0]})
        assert len(ts) >= 20, f"raw samples={len(ts)}"
        assert all(b > a for a, b in zip(ts, ts[1:]))

    @case("stream_minmax")
    def _():
        r, fr = sub_check("minmax", 10, 2)
        for f in fr:
            for (vid, dt, mn, mx, last) in f["recs"]:
                assert mn <= last + 1e-6 and last <= mx + 1e-6, (mn, mx, last)

    @case("mp_clamp_and_accept_kinds")
    def _():
        t = pair("mp"); c = Cli(A.url)
        h = hello(c, t, client="mp", accept=(1, 2, 3))
        names = SN(6) if not A.stream_vars else (SN(6) + [v["name"] for v in st["vars"]])[:6]
        r = c.ok("stream.subscribe", {"vars": names, "rate_hz": 100})
        assert r["rate_hz"] <= 20 and len(r["vars"]) <= 4 and r["mode"] == "minmax", r
        frames = c.collect(1.0)
        assert frames and all(parse(f)["kind"] in h["accept_kinds"] for f in frames)
        c.close()
        c = Cli(A.url); hello(c, t, client="mp", accept=(1,))
        c.err("stream.subscribe", {"vars": names[:1], "mode": "minmax"}, -32602); c.close()

    @case("history_fetch")
    def _():
        c = st["c"]; hn = SN(1)[0]
        vid = next(v["id"] for v in c.ok("vars.list", {"needle": hn, "limit": 50})["vars"] if v["name"] == hn)
        c.bins.clear()
        r = c.ok("history.fetch", {"vars": [hn], "mode": "minmax", "max_points": 100,
                                   "t0": int((time.time() - 2) * 1e6), "t1": int(time.time() * 1e6)})
        assert r["frames"] >= 1 and 0 < r["records"] <= 120, r
        assert r["value_type"] == "f32" and r["vars"][0]["var_id"] == vid and r["vars"][0]["name"] == hn, r
        pre = list(c.bins)
        fr = [parse(f) for f in pre + c.collect(0.3)]
        assert any(f["sid"] == 0 and f["kind"] == 2 for f in fr)

    @case("disconnect_resume")
    def _():
        c = st["c"]
        r = c.ok("stream.subscribe", {"vars": SN(1), "mode": "latest", "rate_hz": 10})
        c.ws.sock.shutdown(socket.SHUT_RDWR); c.close()
        time.sleep(1.0)
        c = Cli(A.url)
        h = hello(c, st["tok"], resume_session_id=st["sid"], last_seq=0)
        assert h["resumed"] is True and h["session_id"] == st["sid"] and h["control"]["held_by_me"] is True and h["role"] == "operator", h
        fr = [parse(f) for f in c.collect(1.0)]
        assert any(f["sid"] == r["stream_id"] for f in fr), "stream not restored"
        assert any(n["method"] == "stream.meta" for n in c.notes)
        c.ok("stream.unsubscribe", {})
        st["c"] = c
        c2 = Cli(A.url); c2.err("session.hello", {"token": st["tok"], "client": "web", "accept_kinds": [1],
                                                  "resume_session_id": "deadbeefdeadbeef"}, -32006); c2.close()

    @case("control_release")
    def _():
        c = st["c"]; c.ok("control.release")
        c.err("vars.write", {"name": A.write_var, "value": 1.0}, -32004)
        t2 = pair(); c2 = Cli(A.url); hello(c2, t2); c2.ok("auth.elevate", {"role": "operator"})
        c2.ok("control.acquire"); c2.ok("control.release"); c2.close()

    @case("too_many_clients")
    def _():
        time.sleep(1.0)  # 等前面用例关闭的连接在服务端释放名额
        cs, got = [], False
        for i in range(12):
            try:
                x = Cli(A.url); cs.append(x)
                x.ws.settimeout(0.5)
                try:
                    m = x.raw()
                    if m and (m.get("error") or {}).get("code") == -32001: got = True; break
                except Exception:
                    pass
            except Exception:
                pass
        for x in cs: x.close()
        assert got, "no -32001"
        time.sleep(0.5); x = Cli(A.url); assert x.ok("ping")["pong"]; x.close()

    for run in CASES:
        run()
    try: st["c"].call("session.bye", {}, timeout=2); st["c"].close()
    except Exception: pass
    bad = [r for r in RESULTS if not r[1]]
    print(f"\n== ORP conformance: {len(RESULTS)-len(bad)}/{len(RESULTS)} passed ==")
    for n, _, e in bad: print(f"  FAIL {n}: {e}")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
