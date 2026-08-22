---
stepsCompleted: [init, technical-overview, integration-patterns, implementation-research, research-synthesis]
workflowType: 'research'
research_type: 'technical'
research_topic: 'OpenScope 网络远程操作（WebSocket 传输 + 无损压缩 + 分块 + 异步传输）'
research_goals: '为 request.md 需求 14 提供可落地技术选型：WebSocket C 库、时序数据无损压缩、分块传输、多核并行、一对多、模块化 .dll'
user_name: 'OpenScope'
date: '2026-08-22'
web_research_enabled: true
source_verification: true
---

# 技术研究：网络远程操作（需求 14）

## 1. 结论摘要（TL;DR）

1. **传输**：WebSocket（RFC 6455）——全双工、单 TCP 长连接、帧式消息，天然适配"远程端下达采集指令 + 本地端持续回传样本"的双向流。**选型**：优先 `mongoose`（单文件 C，WebSocket 客户端/服务端都支持，MIT，零外部构建依赖，最契合本项目纯 C + 独立 .dll）；备选自研 RFC6455 最小实现（与项目自研 ELF 解析的"零依赖"风格一致）。
2. **无损压缩**（时序样本 `timestamp_us + double` 高度相关）：
   - **实时同步模式**：`delta + varint`（时间戳单调递增→存差值）+ `XOR-delta + varint`（浮点值，Gorilla 式）——5~10x、近零 CPU、逐样本流式，符合实时低延迟。
   - **异步批量模式**：`zstd`（比 zlib 更快、压缩率更高，原生支持多线程 `nbWorkers`），满足"多核并行压缩/解压"与"未来 trace 超大流量"。
3. **分块传输**：固定 64KB 分块 + 块头（chunk_id / chunk_idx / chunk_total / len / crc），支持乱序重组与断点续传；大数据量先分块再逐块压缩/传输。
4. **一对多**：WebSocket 不支持组播；但**服务端可持有 N 个客户端连接做 fan-out 广播**（mongoose 原生支持多连接），因此"一对多"可行，作为 Epic 的后续 story。
5. **模块化**：网络功能独立编译成 `network.dll`（OS_Module 插件），新增能力位 `OS_CAP_NET`；协议/编解码/分块/缓冲为**传输无关的纯 C 内核**，可脱离网络单测。

## 2. WebSocket 库选型（C / Windows / 客户端+服务端）

| 方案 | 形态 | 客户端 | 服务端 | 依赖/构建 | 结论 |
| --- | --- | --- | --- | --- | --- |
| **mongoose** | 单文件 mongoose.c/.h | ✅ | ✅ | 零外部依赖，drop-in | ✅ 首选 |
| libwebsockets | 库（CMake） | ✅ | ✅ | 构建重、体积大 | 备选 |
| 自研 RFC6455 | ~500 行 C | ✅ | ✅ | 零依赖，完全可控 | 备选（风格最契合） |

- 本工程已自研 ELF/DWARF 解析（零第三方依赖）；`mongoose` 是"生产级 + 单文件"的最省事选择，[github.com/cesanta/mongoose](https://github.com/cesanta/mongoose)。
- 为保证"可单测"，**协议/编解码/分块/缓冲内核与传输解耦**：内核只消费/产出字节流，传输层（mongoose 或自研）只负责搬运字节流。集成测试用**内存回环**（不依赖真实 socket）。

## 3. 无损压缩方案

时序样本的结构是 `[ts_us(int64)][var_id][raw/value]`，同一变量连续样本高度相关。

- **实时同步（低延迟）**：`delta + varint`
  - 时间戳：存 `ts[i]-ts[i-1]`（单调，通常为几十 µs，varint 1~2 字节）。
  - 数值：浮点用 Gorilla 的 **XOR-delta + 前导零计数 + varint**；整型用差值 varint。
  - 特点：逐样本、无缓冲、可流式，5~10x 压缩，CPU 可忽略。
- **异步批量（高吞吐）**：`zstd`
  - 采集停止后整块压缩，`ZSTD_c_nbWorkers` 多线程并行，解压也并行；
  - 优于 zlib（速度/压缩率双赢），面向未来 trace 超大流量，[github.com/facebook/zstd](https://github.com/facebook/zstd)。
- 本项目首版（测试交付物）实现**自包含的 delta+varint + XOR-float 编解码**（无外部库、可单测）；zstd 作为异步模式的插拔式后端在设计上预留。

参考：[时间序列压缩技术](https://c13n.club/blog/2026-06-15/)、[时序数据压缩算法对比](https://www.enuoidc.com:8443/help/37053.html)。

## 4. 分块传输 + 重传

- 大 payload 切成固定块（如 64KB），每块带 `chunk_id/chunk_idx/chunk_total/len/crc32`。
- 接收端按 chunk_id 聚合、按 chunk_idx 排序、crc 校验，缺块请求重传 → 支持断点续传与乱序。
- 同步模式的样本流不分块（流式帧）；异步批量模式（log 文件/大缓冲）分块 + 逐块压缩。

## 5. 异步传输模式（先落盘后传输）

- 本地采集时样本写入 **RAM 环形 + 磁盘 spool**（复用现有 `datalog` 落盘）。
- 远端"停止采集/记录"后，本地把 spool 文件**分块读取 → zstd 并行压缩 → 逐块发送**；远端解压 → 复原。
- 该模式与实时模式共享协议（样本消息），仅传输时机/压缩后端不同。

## 6. 协议设计（二进制，传输无关）

```
帧头 [magic 'OSN1' 4B][type 1B][flags 1B][seq 4B][payload_len 4B][payload]
type: HELLO=1 / ELF_SYNC=2 / WATCH_LIST=3 / SAMPLE_BATCH=4 / WRITE_VAR=5 / ACK=6 /
      CHUNK=7 / BYE=8
```
- SAMPLE_BATCH 的 payload 用 delta+varint 编解码（见 §3）。
- ELF_SYNC 携带变量名+地址表（本地/远程双向同步，解决"elf 可能两边编译"）。

## 7. 研究综合 → 对规划的直接结论

1. 新增 `module/network` → `network.dll`（`OS_CAP_NET`），内核（`netproto/netcodec/netchunk/netbuf`）纯 C、可单测。
2. 传输选 mongoose；首版集成测试用内存回环（loopback）避免 socket 依赖。
3. 实时模式 delta+varint 编解码 + 异步模式分块/zstd（预留），自包含实现首版。
4. UI 增加 IP/端口配置；一对多作为后续 story（服务端 fan-out）。
