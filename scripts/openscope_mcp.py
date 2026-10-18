#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""OpenScope MCP stdio 桥：把 MCP tools/list、tools/call 转到本机 OpenScope WebSocket。

在 Cursor / Claude Desktop 等 MCP 宿主中配置:
  {
    "mcpServers": {
      "openscope": {
        "command": "python",
        "args": ["D:/OpenScope/scripts/openscope_mcp.py"],
        "env": { "OPENSCOPE_AI_URL": "ws://127.0.0.1:18765" }
      }
    }
  }

前提：OpenScope 已启动且 aiapi.dll 在监听。
依赖：pip install websocket-client
"""
from __future__ import annotations

import json
import os
import sys
import threading
from typing import Any, Dict, Optional

try:
    import websocket
except ImportError:
    sys.stderr.write("需要: pip install websocket-client\n")
    sys.exit(2)

URL = os.environ.get("OPENSCOPE_AI_URL", "ws://127.0.0.1:18765")


class Bridge:
    def __init__(self) -> None:
        self._ws: Optional[Any] = None
        self._id = 0
        self._lock = threading.Lock()

    def _ensure(self) -> None:
        if self._ws is None:
            self._ws = websocket.create_connection(URL, timeout=30)

    def call(self, method: str, params: Optional[dict] = None) -> Any:
        with self._lock:
            self._ensure()
            assert self._ws is not None
            self._id += 1
            req_id = self._id
            self._ws.send(
                json.dumps(
                    {
                        "jsonrpc": "2.0",
                        "id": req_id,
                        "method": method,
                        "params": params or {},
                    },
                    ensure_ascii=False,
                )
            )
            while True:
                raw = self._ws.recv()
                if isinstance(raw, bytes):
                    raw = raw.decode("utf-8", errors="replace")
                data = json.loads(raw)
                if data.get("id") != req_id:
                    continue
                if "error" in data:
                    raise RuntimeError(str(data["error"]))
                return data.get("result")


bridge = Bridge()


def mcp_reply(req_id: Any, result: Any) -> None:
    sys.stdout.write(json.dumps({"jsonrpc": "2.0", "id": req_id, "result": result}) + "\n")
    sys.stdout.flush()


def mcp_error(req_id: Any, code: int, message: str) -> None:
    sys.stdout.write(
        json.dumps(
            {
                "jsonrpc": "2.0",
                "id": req_id,
                "error": {"code": code, "message": message},
            }
        )
        + "\n"
    )
    sys.stdout.flush()


def tools_from_openscope() -> list:
    raw = bridge.call("tools.list", {})
    tools = []
    for t in raw.get("tools") or []:
        name = t.get("name")
        if not name:
            continue
        tools.append(
            {
                "name": name,
                "description": t.get("description") or name,
                "inputSchema": t.get("inputSchema")
                or {"type": "object", "properties": {}},
            }
        )
    return tools


def handle(msg: Dict[str, Any]) -> None:
    method = msg.get("method")
    req_id = msg.get("id")
    params = msg.get("params") or {}

    if method == "initialize":
        mcp_reply(
            req_id,
            {
                "protocolVersion": "2024-11-05",
                "capabilities": {"tools": {}},
                "serverInfo": {"name": "openscope", "version": "1.25.0"},
            },
        )
        return
    if method == "notifications/initialized" or method == "initialized":
        return
    if method == "ping":
        mcp_reply(req_id, {})
        return
    if method == "tools/list":
        try:
            mcp_reply(req_id, {"tools": tools_from_openscope()})
        except Exception as e:
            mcp_error(req_id, -32000, f"OpenScope 未就绪: {e}")
        return
    if method == "tools/call":
        name = params.get("name")
        arguments = params.get("arguments") or {}
        try:
            result = bridge.call(name, arguments if isinstance(arguments, dict) else {})
            text = json.dumps(result, ensure_ascii=False)
            mcp_reply(
                req_id,
                {
                    "content": [{"type": "text", "text": text}],
                    "structuredContent": result if isinstance(result, dict) else {"value": result},
                    "isError": bool(isinstance(result, dict) and result.get("ok") is False),
                },
            )
        except Exception as e:
            mcp_error(req_id, -32000, str(e))
        return

    # 直通：允许宿主发 OpenScope 原生方法名
    if method and req_id is not None and not str(method).startswith("notifications/"):
        try:
            result = bridge.call(method, params if isinstance(params, dict) else {})
            mcp_reply(req_id, result)
        except Exception as e:
            mcp_error(req_id, -32000, str(e))
        return


def main() -> None:
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            msg = json.loads(line)
        except json.JSONDecodeError:
            continue
        handle(msg)


if __name__ == "__main__":
    main()
