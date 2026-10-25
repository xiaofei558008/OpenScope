# ORP mock 服务端（M0）

按 `docs/remote_debug_design.md` v0.3 实现的 OpenScope Remote Protocol 模拟服务端，内部用正弦/余弦/锯齿/方波/噪声/计数器模拟 9 个变量（1 kHz 采样），供网页、小程序、远端 OpenScope 客户端在没有板子时联调。

## 依赖
    pip install websockets websocket-client

## 启动
    python tools\orp_mock\orp_mock.py                         # ws://127.0.0.1:18766
    python tools\orp_mock\orp_mock.py --host 0.0.0.0          # 局域网（小程序真机预览）
    python tools\orp_mock\orp_mock.py --fixed-code 123456     # 固定配对码，便于自动化测试
    参数: --host  --port（默认 18766）  --fixed-code（或环境变量 ORP_MOCK_CODE）

## 配对流程
1. 控制台打印 `[pair] 配对码 XXXXXX ... 二维码内容: orp://pair?d=<device_id>&c=<code>&r=<ws地址>`（5 分钟有效，用一次即换新码）。
2. 客户端发 `pair.claim {code, client:"web|mp|os"}` 得到 `token`（viewer）。
3. 之后每次连接 `session.hello {token, client, accept_kinds:[1,2]}`；断线后带 `resume_session_id` 恢复。
4. 要写值：`auth.elevate {role:"operator"}` → `control.acquire` → `vars.write {name:"g_cfg.gain", value:2, req_id:"..."}`。

可写变量：`g_counter`、`g_cfg.gain`、`g_cfg.freq_hz`、`g_cfg.offset`（改 gain/freq/offset 会实时改变波形）。

## 一致性测试
    python tools\orp_mock\orp_conformance.py --url ws://127.0.0.1:18766 --code 123456 --sim
    # 真机（remote.dll 默认 18770；开发期约定 mock 18766 / 真机 18770）：
    python tools\orp_mock\orp_conformance.py --url ws://127.0.0.1:18770 --code 123456 --write-var g_mta --write-value 4660 --stream-vars "asin_data[3],acos_data[2]"
remote.dll（M1）用同一脚本，去掉 `--sim`，并用 `--write-var/--stream-vars` 指定真实变量。
