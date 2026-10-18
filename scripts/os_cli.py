#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""OpenScope CLI — 通过本机 WebSocket JSON-RPC 调用 OpenScope（aiapi.dll）。

用法:
  python tools/os_cli.py help
  python tools/os_cli.py status
  python tools/os_cli.py tools.list
  python tools/os_cli.py connect '{"device":"Cortex-M4","speed_khz":4000}'
  python tools/os_cli.py elf.load '{"path":"D:/fw.elf"}'
  python tools/os_cli.py vars.watch '{"name":"g_counter","on":1}'
  python tools/os_cli.py acq.start

环境变量 OPENSCOPE_AI_URL 默认 ws://127.0.0.1:18765
"""
from __future__ import annotations

import argparse
import json
import os
import sys
import threading
from typing import Any, Optional

try:
    import websocket  # websocket-client
except ImportError:
    print("需要: pip install websocket-client", file=sys.stderr)
    sys.exit(2)


DEFAULT_URL = os.environ.get("OPENSCOPE_AI_URL", "ws://127.0.0.1:18765")


class RpcClient:
    def __init__(self, url: str, timeout: float = 30.0):
        self.url = url
        self.timeout = timeout
        self._id = 0
        self._lock = threading.Lock()
        self._ws = websocket.create_connection(url, timeout=timeout)

    def close(self) -> None:
        try:
            self._ws.close()
        except Exception:
            pass

    def call(self, method: str, params: Optional[dict] = None) -> Any:
        with self._lock:
            self._id += 1
            req_id = self._id
            msg = {
                "jsonrpc": "2.0",
                "id": req_id,
                "method": method,
                "params": params or {},
            }
            self._ws.send(json.dumps(msg, ensure_ascii=False))
            while True:
                raw = self._ws.recv()
                if isinstance(raw, bytes):
                    raw = raw.decode("utf-8", errors="replace")
                data = json.loads(raw)
                if data.get("id") != req_id:
                    continue
                if "error" in data:
                    err = data["error"]
                    raise RuntimeError(f"RPC error {err.get('code')}: {err.get('message')}")
                return data.get("result")


def main() -> int:
    ap = argparse.ArgumentParser(description="OpenScope AI CLI (WebSocket JSON-RPC)")
    ap.add_argument("method", help="方法名，如 help / status / connect / tools.list")
    ap.add_argument("params", nargs="?", default="{}", help="JSON 参数对象")
    ap.add_argument("--url", default=DEFAULT_URL, help="WebSocket URL")
    ap.add_argument("--pretty", action="store_true", help="美化输出")
    args = ap.parse_args()

    try:
        params = json.loads(args.params) if args.params else {}
        if not isinstance(params, dict):
            print("params 必须是 JSON 对象", file=sys.stderr)
            return 2
    except json.JSONDecodeError as e:
        print(f"params JSON 无效: {e}", file=sys.stderr)
        return 2

    try:
        cli = RpcClient(args.url)
    except Exception as e:
        print(f"连接失败 {args.url}: {e}", file=sys.stderr)
        print("请先启动 OpenScope（需加载 aiapi.dll）", file=sys.stderr)
        return 1

    try:
        result = cli.call(args.method, params)
    except Exception as e:
        print(str(e), file=sys.stderr)
        cli.close()
        return 1
    finally:
        cli.close()

    if args.pretty:
        print(json.dumps(result, ensure_ascii=False, indent=2))
    else:
        print(json.dumps(result, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main())
