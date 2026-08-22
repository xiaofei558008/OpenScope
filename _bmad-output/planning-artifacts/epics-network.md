# OpenScope Epic 13 — 网络远程操作（request.md 需求 14）

> BMAD 规划工件。覆盖 request.md 需求 14（新增网络远程操作功能）。
> 关联架构决策：AD-14（传输无关内核 + 二进制协议）、AD-15（实时同步 + 异步批量）。
> 技术依据：`_bmad-output/planning-artifacts/research/technical-network-remote-research-2026-08-22.md`。

## 需求来源（request.md 需求 14）

- 网络端 OpenScope 加载 ELF → 变量采集波形显示 + 数值窗口修改；
- 本地 PC OpenScope 接收网络端下达的"待采集变量" → 用 ST-Link/J-Link 采集（值+时间戳）→ 回传网络端；
- 网络端下达变量写入 → 本地经 ST-Link/J-Link 写入目标 MCU；
- 本地/网络 ELF 变量地址+名称可按键双向同步（ELF 可能两边编译）；
- 推荐 WebSocket；大数据量需缓存+压缩（调研无损/分块传输，考虑未来 trace 超大流量）；
- 异步模式：本地先 log 到 RAM/硬盘，远端停止后再传输（压缩、多核并行）；
- IP/端口界面可配置；可服务器转发；可局域网；一对多（WebSocket 不支持则先不做）；
- 网络功能独立编译成 .dll。

## 关键实现决策（源自技术研究）

1. **传输**：WebSocket（RFC6455）全双工；选型优先 `mongoose`（单文件 C、客户端+服务端、MIT），备选自研 RFC6455。
2. **无损压缩**：实时同步用 **delta+varint（时间戳）+ XOR-delta（浮点 Gorilla 式）**；异步批量用 **zstd（多线程）**。
3. **分块**：固定 64KB 块 + 块头（id/idx/total/len/crc），支持乱序重组/续传。
4. **一对多**：WebSocket 不支持组播，但服务端 fan-out 多连接可行（后续 story）。
5. **模块化**：`network.dll`（`OS_CAP_NET`），内核（netproto/netcodec/netchunk/netbuf）传输无关、可单测。

---

## Epic 13：网络远程操作

### Story 13.1 — 传输无关内核：协议帧 + varint + 分块 + 缓冲 ✅ checkpoint-40 DONE
- AC：`netproto`（varint/zigzag、帧编解码、分块切分/重组、可增长缓冲）与 `netcodec`（时序样本 delta+varint + XOR-float 无损编解码）纯 C、无 socket 依赖；单元测试覆盖 varint/帧/编解码（无损往返+压缩率）/分块边界。
- 文件：module/network/netproto.{h,c}、netcodec.{h,c}、tests/netcore_smoke.c。
- 测试：`netcore_smoke`（单元）ALL PASS；100 常量样本压缩到 19.2%。

### Story 13.2 — network.dll 模块骨架（OS_CAP_NET）+ 冒烟测试 ✅ checkpoint-40 DONE
- AC：`network.dll` 导出 `os_module_get`、`OS_CAP_NET`、init/deinit/command（GET_INFO/ELF_RELOADED）；宿主可加载（module_mgr 通用加载）。
- 文件：module/network/network.c、network.vcxproj、module_api.h（+OS_CAP_NET）、OpenScope.sln、tests/network_smoke.c。
- 测试：`network_smoke`（冒烟）PASS。

### Story 13.3 — 端到端内存回环集成测试 ✅ checkpoint-40 DONE
- AC：样本 → 编码 → 帧 → 分块 → 重组 → 帧解码 → 样本解码，全链路无损（1000 样本往返一致）。
- 文件：tests/network_loopback.c。
- 测试：`network_loopback`（集成）PASS。

### Story 13.4 — WebSocket 传输层接入（客户端+服务端）【后续】
- AC：mongoose（或自研 RFC6455）接入 `network.dll`；支持连接、心跳、断线重连；IP/端口可配（宿主设置项）。
- 调研项：mongoose 单文件接入 Windows 构建；TLS 转发（服务器中转）。

### Story 13.5 — 远程操作协议落地：变量同步/采集/写入【后续】
- AC：HELLO/ELF_SYNC/WATCH_LIST/SAMPLE_BATCH/WRITE_VAR/ACK 消息流；本地↔远端双向 ELF 变量名+地址同步（按键触发）；数值窗口写入下发→本地写 MCU→回读 ACK。

### Story 13.6 — 异步批量传输 + zstd 多线程压缩【后续】
- AC：本地落盘 → 远端停止后分块读取 → zstd 多线程压缩 → 逐块传输 → 远端解压复原；压缩解压并行（多核）。

### Story 13.7 — 一对多 fan-out + 服务器转发【后续】
- AC：服务端维持 N 个客户端连接做 fan-out 广播；经服务器（8.133.18.102）转发验证。

---

## Sprint 计划

- Sprint-14：Story 13.1 → 13.2 → 13.3（内核+骨架+回环，checkpoint-40 ✅）
- Sprint-15：Story 13.4（WebSocket 传输）→ 13.5（远程协议）→ 13.6（异步+zstd）→ 13.7（一对多/转发）

## 验收风险

- 自研 RFC6455 的掩码/分片/心跳细节多，优先用 mongoose 降低风险；内核与传输解耦后二者可互换。
- 浮点 XOR-delta 位操作须经无损往返测试兜底（已覆盖）。
- zstd 需引入第三方库（静态链入 network.dll，避免运行时依赖）；多线程压缩的线程数按核数自适应。
- 服务器转发需鉴权/TLS，测试服务器 sudo 密码仅测试用，生产需换密钥。
