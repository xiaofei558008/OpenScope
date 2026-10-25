# OpenScope 远程调试（网络外发）现状与方案设计

> 版本 v0.3（2026-10-25，按小微评审修订，见第 7 节修订记录），作者：程序员小程。本阶段仅调研 + 方案，未改代码。
> 讨论对象：Tom、小微（微信小程序）、网页端负责人（待定）。

## 1. 现状

### 1.1 仓库
- gitee_origin: `git@gitee.com:xiaofei558008/open-scope.git`
- github_origin: `git@github.com:xiaofei558008/OpenScope.git`
- 当前分支 `dev_llm`，HEAD `37f617f`（本地，未推送），版本 1.28.0。

### 1.2 network.dll 架构（module/network）
| 文件 | 作用 |
|---|---|
| `netws.c/.h` | 自研 RFC6455 WebSocket（明文 ws://，无 TLS，无子协议/Origin 校验）；服务端监听 + 客户端连接；二进制/文本帧 |
| `netproto.c/.h` | 传输无关帧：`magic "OSN1"(4) + type(1) + flags(1) + seq(4) + len(4) + payload`，LEB128 varint，分块/重组 |
| `netsession.c/.h` | 载荷序列化：变量表(name/addr/size)、名字列表、写值(name,text)、ACK(code,msg) |
| `netcodec.c/.h` | 样本编解码：flat（实时）、Gorilla 式 delta/XOR（历史无损）、多线程并行压缩 |
| `netbuf.c/.h` | 可增长缓冲 |
| `network.c` | 模块主体：服务端（探针侧）每客户端一线程（≤16，一对多 fan-out）；客户端（显示侧） |

消息类型（二进制）：`HELLO(1) ELF_SYNC(2) WATCH_LIST(3) SAMPLE_BATCH(4) WRITE_VAR(5) ACK(6) CHUNK(7) BYE(8) ELF_REQ(9) LOG_REQ(10)`。

已实现链路：远端 HELLO → 服务端回变量表；远端 WATCH_LIST → 服务端勾选变量并启动采集 → 采集线程约 30ms 推一次 SAMPLE_BATCH（每变量只推**最新一个样本**）；WRITE_VAR → 服务端写 MCU → ACK；LOG_REQ → 历史环缓冲压缩分块回传；ELF 变量表双向同步（远端无需本地 ELF）。IP/端口在界面可配，默认 127.0.0.1:10005。

缺失：
- **无鉴权/配对/会话 token**，任何能连上端口的人可写内存；无 TLS；无 Origin 校验。
- 协议是自定义二进制 + C 结构，网页/小程序需重写解析；无版本协商。
- 实时流只推最新值（丢中间样本），无降采样/窗口概念；无背压控制。
- 无连接/断开、扫描探针、读内存、断点/单步、CPU 状态、录制、烧录、XCP 的远程化。
- 无云端中转、无 NAT 穿透、无心跳/断线重连、无设备在线状态。
- 握手仍在监听线程同步执行（aiapi 已修，network 尚未修），半开连接可卡死监听。
- 编译警告：`send_msg` 隐式声明、`OS_Sample*`→`OS_NetSample*` 类型不兼容（network.c:132/228）。

### 1.3 ST-Link 远程写值 `write failed` 根因
E2E 测试 `tests/net_drive.ps1` 的 A 端（探针侧）只尝试通过 UI 连接 **J-Link**（等待日志 "J-Link 已连接"），本机只有 ST-Link，因此 A 端**未连接探针**。A 收到 WRITE_VAR 后调用 `write_leaf → os_ds_write_leaf`，该函数在"本机未连接"时会把写值**再次经网络转发**（设计给显示侧用），服务端没有上游，等待 ACK 超时 → `网络写入失败 (rc=-5 OS_ERR_TIMEOUT)` → ACK -1 `write failed`。
- 环境问题：测试脚本未支持 ST-Link（且测试 ELF `elf_sample.out` 与板上固件不符）。
- 设计缺陷：服务端角色不应把写值回转网络（存在回环风险），应直接返回"未连接探针"。

### 1.4 可复用资产：aiapi（AI JSON-RPC）
`aiapi.dll` 已在 `127.0.0.1:18765` 提供 WebSocket JSON-RPC 2.0，66 个方法，覆盖：status/drivers/probe.scan/connect/disconnect、elf.load/vars.list/find/read/write/watch、mem.read/write、acq.start/stop/state、recording、bp.*/halt/go/step/cpu.state、src.*、xcp.*/a2l.export、flash.download/erase、log.read/subscribe（推送）、alerts。本次已修复：握手超时、客户端上限/超大请求明确错误、采集零样本等，真机 10 轮 52 项全过。
**结论：远程协议以 aiapi 的方法集为基础最省工作量**，network.dll 的二进制 codec 用于波形大数据。

## 2. 功能清单与远程化优先级

图例：✅ 支持 ◐ 精简/只读 ✗ 不支持。P0=首版必需。

| 功能 | 优先级 | 远端 OpenScope | 网页 | 小程序 | 备注 |
|---|---|---|---|---|---|
| 设备在线/状态（探针、电压、连接） | P0 | ✅ | ✅ | ✅ | |
| 连接/断开目标（attach 非侵入） | P0 | ✅ | ✅ | ✅ | |
| 探针扫描/驱动选择/速度 | P1 | ✅ | ✅ | ◐ | 小程序只选默认 |
| ELF 变量表浏览/搜索 | P0 | ✅ | ✅ | ◐ | 小程序搜索+收藏，不展开大树 |
| 选择监视变量、启动/停止采集 | P0 | ✅ | ✅ | ✅ | |
| 实时数值显示 | P0 | ✅ | ✅ | ✅ | |
| 实时波形 | P0 | ✅ 全速 | ✅ 降采样 | ◐ 按 `rate_hz≤20`、≤4 条曲线、默认 minmax 请求 | |
| 写变量值（标定） | P0 | ✅ | ✅ | ✅（需确认） | 危险操作权限 |
| 读/写任意内存 | P2 | ✅ | ✅ | ✗ | 高危 |
| 暂停/运行/单步/复位、CPU 状态 | P1 | ✅ | ✅ | ◐ 仅暂停/运行 | |
| 断点（地址/源码行）、源码跟随 | P2 | ✅ | ◐ | ✗ | 源码需本地文件或上传 |
| 录制、历史回传、导出 CSV/MF4 | P1 | ✅ | ✅ CSV | ◐ 只看最近 N 秒 | 用 LOG_REQ+codec |
| 运行日志/告警推送 | P1 | ✅ | ✅ | ◐ 告警 | log.subscribe |
| 烧录/整片擦除 | P2 | ✅ | ◐ | ✗ | 最高危，需二次确认+本地授权 |
| XCP 标定/A2L | P3 | ✅ | ◐ | ✗ | |
| ELF 上传/同步 | P1 | ✅ | ✅ | ✗ | |

## 3. 方案设计

### 3.1 角色
- **Agent（服务端）**：接探针的本地 OpenScope，新建 `remote.dll`（或在 aiapi 上扩展）对外提供远程会话；内部直接复用 ai_bridge 的方法实现。
- **客户端**：远端 OpenScope（network.dll 客户端角色改走新协议）、网页（浏览器 WebSocket）、小程序（`wx.connectSocket`）。
- **Relay（中转，可选）**：云端服务，负责设备注册、配对、WSS 终结与消息转发。

### 3.2 两种连接模式
1. **局域网直连**：Agent 监听 `0.0.0.0:<port>`（默认仍 127.0.0.1，需用户在界面显式开启 LAN），客户端 `ws://<局域网IP>:port`。网页可直连（http 页面）；小程序**正式版不能直连 ws://IP**，仅开发阶段可用。可选 mDNS/UDP 广播发现。
2. **云端中转**：Agent 主动出站连接 `wss://relay.<备案域名>/agent`（无需公网 IP/端口映射）；客户端连 `wss://relay.<域名>/client`。Relay 按 device_id 配对转发，不解析业务内容（可选端到端加密）。小程序须在后台配置 socket 合法域名（HTTPS/WSS + ICP 备案 + 有效证书）。
   - Relay 技术栈建议：Go 或 Node.js 单进程 + Nginx/Caddy 终结 TLS；Redis（可选）存在线表；部署在国内云（阿里云/腾讯云）与备案域名同主体。
   - 带宽：波形流在 Agent 侧先降采样，中转只转发，单设备 ≤50KB/s；小程序订阅固定为 `rate_hz≤20`、曲线≤4、`mode:"minmax"`。

### 3.3 统一协议（OpenScope Remote Protocol, ORP v1）
- 传输：WebSocket；子协议 `orp.v1`。
- **文本帧 = JSON-RPC 2.0**，方法名与 aiapi 一致（`connect`、`vars.list`、`vars.write`、`acq.start`…），新增：
  - `session.hello {client:"web|mp|openscope", ver, token, accept_kinds:[1,2,3], resume_session_id?, last_seq?}` → `{session_id, device, caps, role, accept_kinds, resumed}`：`accept_kinds` 为客户端能解析的二进制帧 kind，服务端取交集，**绝不向未声明 3 的客户端发送 kind=3**（小程序默认 `[1,2]`）。
  - `control.acquire {}` → `{ok, holder}` / `control.release {}`：申请/释放控制权（写值、暂停等需持有）；小程序在 `onHide` 时主动 `control.release`。
  - `auth.elevate {role:"operator"}`：已配对客户端申请提升权限（Agent 端确认后生效）。
  - `stream.subscribe {vars:[id...], rate_hz, mode:"latest|minmax|raw", window_ms}` / `stream.unsubscribe`
  - `history.fetch {vars, t0, t1, max_points, mode:"raw|minmax"}`：支持 minmax 降采样返回（同 kind=2 帧）。
  - 通知（无 id）：`stream.meta {stream_id, vars:[{var_id,name,value_type:"f32|f64"}], mode, rate_hz}`（**value 类型在此声明**）、`log.event`、`device.state`、`alert`
- **二进制帧 = 波形数据**（**全部字段小端序**，单帧上限 **64KB**，超出拆多帧）：
  - 帧头 16 字节：`ver u8 | kind u8 | stream_id u16 | seq u32 | t0_hi u32 | t0_lo u32`
  - `t0 = t0_hi * 2^32 + t0_lo`，单位 µs（Agent 采集时钟）。**选高低两个 uint32 而不用 uint64/float64 的理由**：小程序/ES5 环境 DataView 没有可靠的 `getBigUint64`；拆成两个 uint32 在 C 端和 JS 端都是整数运算、µs 精确无舍入；JS 侧用 `hi*4294967296+lo` 得到的值在 2^53 内精确（约 285 年）。
  - 帧内每个样本的时间 `dt` = 相对 t0 的 **uint32 微秒偏移**（单帧覆盖 ≤ 71 分钟，足够）。
  - value 类型（float32 或 float64）由 `stream.meta` 的 `value_type` 声明，帧内不重复携带。
  - kind=1 flat：`n u16`，随后 n 个 `{var_id u16, dt u32, value f32|f64}`。
  - kind=2 minmax：`n u16`，随后 n 个桶 `{var_id u16, dt u32(桶起点), min, max, last}`（同 value_type）。
  - kind=3 Gorilla 压缩块（复用 netcodec，仅远端 OpenScope 等声明支持的客户端）。
  - 网页/小程序用 DataView（little-endian=true）解析；或握手时声明 `json_stream:true` 退化为 JSON（低速、调试用）。
- 降采样：Agent 按客户端 `rate_hz` 和屏幕宽度做 min/max 桶（保证尖峰不丢）；远端 OpenScope 可选 raw。
- 心跳：客户端每 15s `ping` 方法或 WS ping；30s 无响应断开；客户端指数退避重连并恢复订阅。
- 背压：每会话发送队列上限，超限丢弃旧波形帧（保留控制消息）。

### 3.4 鉴权与配对
- Agent 首次启用远程时生成 `device_id` + 设备密钥（本地保存，DPAPI 加密）。
- **配对**：Agent 界面显示 6 位配对码（5 分钟有效）及二维码，二维码内容格式：`orp://pair?d=<device_id>&c=<配对码>&r=<中转或局域网地址，URL 编码>`（如 `r=wss%3A%2F%2Frelay.example.com%2Fclient` 或 `r=ws%3A%2F%2F192.168.1.8%3A18766`）；后续可升级为微信小程序码（scene 携带同样参数，扫码直接打开小程序配对页）。客户端提交配对码 → Agent/Relay 签发长期 `client_token`（可吊销，界面列出已配对客户端）。
- 每次连接 `session.hello` 携带 token；Relay 模式下 Relay 校验 token→device 归属，再转发；直连模式由 Agent 校验。
- **可选：微信登录**（是否启用待 Tom 决定）：Relay 提供 HTTPS 接口 `POST https://<同一备案域名>/api/wx/login {code}`，服务端调用微信 code2session 换 openid，签发用户 token 与已配对设备绑定；与 WSS 共用同一备案域名；**AppSecret 只保存在 Relay 服务器上**，不下发到小程序或 Agent。
- **小程序默认配对角色为 viewer**；需要写值时调用 `auth.elevate {role:"operator"}`（Agent 端确认）并 `control.acquire` 获取控制权，用完/`onHide` 时 `control.release`。

### 3.5 权限与安全
- 角色：`viewer`（只读：状态/变量/波形/日志）、`operator`（+写变量、暂停运行、录制）、`admin`（+读写任意内存、断点、烧录、擦除、XCP 写）。配对时 Agent 端选择授予的角色。
- 危险操作（mem.write、flash.*、vars.write 到非白名单、连接 under-reset）：
  - 需 `admin` 或显式授权；Agent 本地弹窗确认（可设"本次会话允许"）；烧录额外要求 Agent 端开关"允许远程烧录"默认关闭。
  - 写值范围限制（可选按 A2L/ELF 类型限幅）、审计日志（谁、何时、写了什么）。
- 同一时刻仅一个 operator/admin 持有"控制权"（control lock），其他客户端只读，防止并发写冲突。
- 传输：Relay 全程 WSS；LAN 直连默认关闭，开启时提示风险，可选自签证书 wss。
- 现有 network.dll 二进制协议在新协议稳定后标记为 legacy（仅 OpenScope↔OpenScope 兼容）。

### 3.6 错误码表
沿用 aiapi 现有约定：JSON-RPC `error.code` 为协议层错误；业务失败统一 `-32000`，`error.data.os_err` 携带模块错误码（与 `module_api.h` 的 `OS_ERR_*` 一致），message 为人类可读文本。

| error.code | os_err | 名称 | 含义 / 客户端处理 |
|---|---|---|---|
| -32700 | — | PARSE_ERROR | JSON 无法解析 |
| -32600 | — | INVALID_REQUEST | 缺 method 等；**请求过大**也用此码（message `request too large (max N bytes)`，aiapi 已实现，连接保持） |
| -32601 | — | METHOD_NOT_FOUND | 未知方法（aiapi 现为 -32000/os_err -2 "unknown method"，ORP 统一改为 -32601） |
| -32602 | -2 | INVALID_PARAMS | 参数缺失/越界（OS_ERR_INVALID_ARG） |
| -32000 | -1 | FAIL | 通用失败（OS_ERR_FAIL） |
| -32000 | -3 | NO_DEVICE | 未发现探针（OS_ERR_NO_DEVICE） |
| -32000 | -4 | NOT_CONNECTED | **未连接探针/目标**（OS_ERR_NOT_CONNECTED） |
| -32000 | -5 | TIMEOUT | 操作超时（OS_ERR_TIMEOUT） |
| -32000 | -6 | CANCELED | 被用户取消/本地拒绝确认（OS_ERR_CANCELED） |
| -32000 | -7 | BUSY | 设备忙（OS_ERR_BUSY） |
| -32001 | — | TOO_MANY_CLIENTS | **客户端过多**（aiapi 已实现，`too many clients (max 8)`） |
| -32002 | — | UNAUTHORIZED | **未配对或 token 失效/吊销** → 回到配对页 |
| -32003 | — | FORBIDDEN | **权限不足**（角色不够）→ 提示 `auth.elevate` |
| -32004 | — | CONTROL_LOCKED | **控制权被占用**，`error.data.holder` 给出持有者 → 只读或稍后 `control.acquire` |
| -32005 | — | DEVICE_OFFLINE | **设备离线**（Relay 模式下 Agent 未在线）→ 显示离线并轮询 |
| -32006 | — | SESSION_EXPIRED | 恢复会话失败（超过保留期）→ 重新订阅 |

### 3.7 单机 127.0.0.1 联调方法
1. 启动 OpenScope（Agent）：开启远程服务监听 `127.0.0.1:18766`（开发期可关闭鉴权或使用固定测试 token）。
2. **远端 OpenScope**：再开一个实例 `OpenScope.exe --remote-connect=ws://127.0.0.1:18766`（独立配置目录，避免抢 ST-Link）。
3. **网页**：本地 `npx serve` / Vite 开 `http://127.0.0.1:5173`，页面连接 `ws://127.0.0.1:18766`（http 页可连 ws）。
4. **小程序**：微信开发者工具 → 详情 → 本地设置 → 勾选"不校验合法域名、web-view、TLS 版本以及 HTTPS 证书"，`wx.connectSocket({url:'ws://127.0.0.1:18766'})`；真机预览需改用局域网 IP 并同样勾选（仅调试），正式版必须走 WSS 中转。
5. 中转联调：本机运行 relay（`ws://127.0.0.1:8080`），Agent 以出站方式连 relay，三端连 relay，验证同一套协议在中转模式下可用。
6. 提供 `scripts/orp_cli.py` 与协议自动化测试（复用 test_ai_full 思路）和一个 mock agent（无板也能让小程序/网页联调）。

### 3.8 orp-client SDK
- 纯 JavaScript 实现，构建输出兼容 **ES5**（不依赖 BigInt、class 字段等），网页与小程序共用。
- 传输层抽象接口：`transport.open(url) / send(data: string|ArrayBuffer) / close() / onOpen / onMessage / onClose / onError`。
  - 浏览器适配：原生 `WebSocket`（`binaryType="arraybuffer"`）。
  - 小程序适配：`wx.connectSocket` / `SocketTask`，**由小微编写**。
- SDK 负责：JSON-RPC 请求/响应匹配、配对与 token 存储回调、心跳、指数退避重连与 `resume_session_id` 会话恢复、`req_id` 幂等、二进制帧解析（小端、t0_hi/t0_lo、dt 偏移、按 stream.meta 的 value_type 解码）。

### 3.9 M0 落地约定（v0.3 补充，mock 与 remote.dll 共同遵守）
- **WebSocket 子协议**：客户端可带 `Sec-WebSocket-Protocol: orp.v1`，服务端回同名；不带也可连。
- **配对方法名**：`pair.claim {code, client:"web|mp|os", name}` → `{token, role:"viewer", device_id}`。配对码 6 位、5 分钟有效、成功一次即轮换（mock 的 `--fixed-code` 例外，仅测试用）。
- **握手前允许的方法**：仅 `ping`、`pair.claim`、`session.hello`；其余返回 `-32002`。
- `session.hello {token, client, accept_kinds, orp:1, resume_session_id?, last_seq?}` → `{session_id, device, role, accept_kinds(交集), resumed, caps, control, streams?}`；`caps` 含 `max_frame / max_request / modes / max_rate_hz / max_vars(仅 mp)`。恢复失败返回 `-32006`；会话断开后保留 120 s，恢复时角色、控制权、订阅全部还原，并重发 `stream.meta`。
- **心跳**：客户端每 10 s 发 `ping`（任何请求都算活动）；服务端 30 s 无消息即断开连接（close 4000），会话按上条保留。
- **请求上限**：单条文本请求 ≤ 256 KB−1，超过回 `-32600 "request too large"`，连接保留。连接数上限 8，超出回 `-32001` 后关闭。
- **二进制帧布局**：16 B 头之后紧跟 `n u16`，再接 n 条记录：kind=1 为 `var_id u16 | dt u32 | value(f32|f64)`；kind=2 为 `var_id u16 | dt u32 | min | max | last`。`t0` 为 Unix 微秒；`seq` 按会话单调递增（跨流共用）。kind=3（压缩）M0/M1 均不实现，`accept_kinds` 交集中会被剔除。
- **订阅**：`stream.subscribe {vars:[id|name], mode, rate_hz(0,200], value_type:"f32|f64"}` → `{stream_id, mode, rate_hz, vars, value_type, kind}`，随后先推 `stream.meta` 通知再推帧；mp 客户端自动钳制 `rate_hz≤20`、最多 4 变量、默认 `minmax`。所需 kind 不在 accept_kinds 内回 `-32602`。`stream.unsubscribe {stream_id?}`（省略则全部退订）。
- **history.fetch** 的帧使用 `stream_id=0`，在 result 之前发出；result 为 `{frames, records, kind, stream_id:0, t0, t1}`。
- **控制权冲突**：`-32004`，`error.data.holder` 给出当前持有者；远程烧录/写内存方法统一回 `-32003`。
- **vars.write** 必须 operator + 控制权；可带 `req_id` 幂等，重复请求返回首次结果并带 `duplicate:true`。
- **auth.elevate**：mock 自动批准；remote.dll 在本机弹窗/日志确认（M1 默认对 127.0.0.1 自动批准 operator，0.0.0.0 下需本地确认）。admin 不可远程授予。

## 4. 分阶段计划与分工

| 阶段 | 内容 | 负责 | 预估 |
|---|---|---|---|
| M0 协议定稿 | 本文档评审；输出 ORP v1 协议规范（方法、错误码、二进制帧、权限矩阵）+ mock agent | 小程（主）、小微/网页参与评审 | 3~4 天 |
| M1 Agent 本地版 | remote 服务（复用 ai_bridge）：hello/token、stream.subscribe+降采样二进制帧、心跳、控制权、权限；修 network 握手；127.0.0.1 联调 | 小程 | 1.5~2 周 |
| M1' 小程序骨架 | 连接/配对页、设备状态、变量搜索与收藏、数值面板、简单波形（canvas 2D）、写值确认 | 小微（连 mock agent / 127.0.0.1） | 并行 2 周 |
| M2 远端 OpenScope 客户端 | network.dll 客户端改走 ORP；变量树/波形/写值/断点/录制复用现有 UI | 小程 | 1~1.5 周 |
| M2' 网页端 | 设备页、变量树、多曲线波形（uPlot/ECharts）、写值、日志、CSV 导出 | 待定（小网或小程） | 2 周 |
| M3 云端中转 | relay 服务（WSS、设备注册、配对、转发、在线表、限流）、部署脚本；Agent 出站连接 | 小程 | 1.5 周 |
| M4 安全与发布 | 审计日志、危险操作确认、远程烧录开关、压测（多客户端、弱网）、小程序提审（合法域名配置） | 小程 + 小微 | 1 周 |

接口契约：M0 结束时冻结 ORP v1；之后变更走版本号（`caps` 协商）。小微与网页端开发全程可用 mock agent，不依赖开发板。

## 5. 待 Tom 拍板的开放问题
1. 云服务器与**已 ICP 备案域名**是否已有？（小程序正式版必需 WSS 合法域名，与小程序主体是否一致）可否使用现有 8.133.18.102 服务器？
2. 网页端由谁做（网站建设小网 / 小程序）？是否放在 www.opendebugger.com 下？
3. 小程序是否需要写值/暂停等控制能力，还是首版只读监控？
4. 首版是否支持远程烧录（建议首版不开放）？
5. 是否需要多用户账号体系（微信登录/手机号），还是仅设备配对码 + token？
6. 局域网直连是否首版就要（网页、远端 OpenScope 可用；小程序正式版不可用）？
7. 是否需要端到端加密（中转服务器不可见数据）？
8. 现有 network.dll 二进制协议是否保留兼容，还是直接切换到新协议？

## 6. 小程序端要求（小微提出，已纳入方案）
1. **联调/监听地址**：开发者工具可直连 `ws://127.0.0.1`；真机预览手机无法访问电脑回环，Agent 需可配置监听 `0.0.0.0`，用局域网 IP 访问。**默认仍只监听 127.0.0.1；切到 0.0.0.0 时强制开启鉴权**（必须先配对拿 token，否则拒绝 `session.hello`），界面显示风险提示与当前局域网 IP/二维码。上线走 `wss://<已备案域名>`，端口 443（Nginx/Caddy 反代到 relay）。
2. **消息格式**：控制消息 JSON（JSON-RPC 2.0）；波形/采样等高频数据走二进制帧（ArrayBuffer，见 3.3），支持批量（一帧多变量多样本）与降采样（latest/minmax，按 `rate_hz`）。**网页端与小程序共用同一套 ORP v1 协议**，共用一份 JS/TS SDK（`orp-client`：连接、配对、心跳、重连、二进制解析），小程序与网页仅 UI 不同。
3. **小程序 v1 功能范围**：连接与配对（配对码或扫码二维码）、从 ELF 取变量列表（搜索+收藏）、实时变量数值与波形、变量写值（二次确认）、连接状态与错误提示。**不做**烧录、擦除、任意内存读写、断点等重型/高危操作（服务端按 `client:"mp"` 只授予 viewer/operator 能力）。
4. **后台断开与会话恢复**：小程序切后台 socket 会被关闭。协议要求：
   - 心跳：客户端每 10~15s `ping`（或 WS ping），服务端 30s 无心跳回收会话资源但**保留会话状态 2~5 分钟**（订阅列表、控制权、写值序号）。
   - `session.hello {token, resume_session_id, last_seq}`：重连后恢复订阅；服务端返回 `resumed:true`，必要时补发最后一段降采样数据（或客户端用 `history.fetch` 补齐断档）。
   - 客户端指数退避重连（1s→2s→4s… 上限 30s），`onShow` 立即重连，`onHide` 主动 `stream.unsubscribe` 节省流量。
   - 写值等控制请求带 `req_id` 幂等，重连后重发不会重复执行。
   - 控制权（control lock）随会话保留期保留，超时自动释放。


## 7. 修订记录
| 版本 | 日期 | 作者 | 内容 |
|---|---|---|---|
| v0.1 | 2026-10-25 | 程序员小程 | 初稿：现状调研、功能清单、方案、分阶段计划；纳入小微第一轮要求（第 6 节） |
| v0.2 | 2026-10-25 | 程序员小程 | 按小微评审修订：二进制帧小端序、t0 拆 hi/lo uint32、帧内 uint32 µs 偏移、value_type 在 stream.meta 声明、单帧 64KB；小程序订阅 rate_hz≤20/≤4 曲线/minmax；session.hello 增加 accept_kinds；history.fetch 支持 minmax；二维码 orp://pair 格式；可选微信登录（code2session 在 Relay）；错误码表；小程序默认 viewer + auth.elevate；control.acquire/release；orp-client 纯 JS(ES5) + 传输层抽象 |
| v0.3 | 2026-10-25 | 程序员小程 | M0 落地：新增 3.9（pair.claim 方法名、握手前方法白名单、子协议 orp.v1、帧记录布局与 n u16、t0 为 Unix µs、seq 会话级递增、history 帧 stream_id=0、请求上限 256KB、心跳 30s/会话保留 120s、control 冲突 data.holder、kind=3 暂不实现） |
