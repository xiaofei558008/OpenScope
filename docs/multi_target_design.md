# OpenScope 多目标（Multi-Target）调试/采样设计

状态：草案 v0.1（2026-10-25）  作者：Grok Bot（为 Tom 起草）  评审：Tom、小序

## 0. 目标与非目标
- 目标：一个 OpenScope 实例同时连接 N 个目标（jlink/stlink/das 探针、XCP 从站、远程 ORP），各目标变量在**同一时间轴**上对比；CPU 开销低，弱 PC 上依旧流畅。
- 非目标（第一期）：跨目标硬件级同步触发（只预留接口）；跨目标同时单步。

## 1. 现状调研（基于 HEAD 4aaebe6）
| 方面 | 现状 | 单目标假设 |
|---|---|---|
| 插件 API | `module_api.h` v5，`os_module_get()` 返回单个 `OS_Module`，`init(fw,&ctx)` 一次 | 每 DLL 一个 ctx；jlink/stlink/das 内部用文件级全局（das.c `g_ctx`） |
| 驱动绑定 | `g_app.driver / driver_ctx`，探针驱动互斥选一个；`g_app.xcpmod`、`g_app.netmod` 各一个 | `g_app.driver` 在 *.c 中被引用 ~90 处（mainwin/srcwin/bpwin/ai_bridge/datasrv） |
| 符号 | 单个 ELF + 变量树；`g_app.elf/vars/leaf*` 在 vartree.c(54)、mainwin.c(36)、ai_bridge.c(20) 等引用 | 变量名全局唯一，leaf id 全局 |
| 采集 | `datasrv.c` 一个轮询线程，读完全部观测变量 → 写全局环 `g_app.ring[OS_RING_CAP=8192]` → 每 16ms PostMessage(WM_OS_SAMPLES) 主线程 drain | 一条线程一个驱动 |
| 时间戳 | 轮询周期起点 `qpc_now_us()`（主机 QPC，µs），同一轮全部变量共用 | 无每目标延迟补偿 |
| 日志 | `os_log` → 文件 + ListView；模块经 `fw->log` | **已实现**来源前缀（见 §10） |
| aiapi/ORP | 方法无 target 参数（`connect`、`vars.list`、`mem.read`…） | 隐含当前驱动 |

## 2. 设备模型
```
Target {
  id: "tc375"            // 用户可改的短别名，唯一，[a-z0-9_]，用于变量前缀/日志前缀
  transport: {kind: das|jlink|stlink|xcp|orp, serial/port/ip, speed}
  mcu: "TC375" | "STM32F407" ...
  core: 0..n             // 多核 MCU 每核一个 Target（共享 transport 会话，见 §7）
  symbols: ELF | A2L | MAP 路径
  active: bool           // 未激活 = 不连接、不采样、零开销
  sample: {rate_cap_hz, period_ms}
}
```
运行时：`TargetRt { Target cfg; const OS_Module* mod; void* inst; SymbolTable* syms; AcqThread thr; Ring* ring; stats }`，放进 `g_app.targets[OS_MAX_TARGETS=8]`。

## 3. UI
- **设备管理器（弹窗）**：采纳 Tom 方案。列表：☑激活 | 别名 | 类型 | MCU | 端口/序列号 | 符号文件 | 状态灯 | 实测速率。增/删/编辑/扫描探针；网络（ORP 服务端/客户端、XCP 以太网）同一对话框的“网络”页。
- 左侧变量树按目标分组（根节点 = 目标别名 + 状态），未激活目标折叠灰显。
- 主界面去掉各探针设置，仅保留工具栏“**当前目标**”下拉框（见 §4）。
- 曲线图例显示 `tc375:g_sin1`，颜色按目标分组可选。

## 4. 调试/源码加载 —— 推荐“当前目标（Active Target）”模型
- **采样**：所有激活目标同时进行，与“当前目标”无关。
- **运行控制/断点/源码窗/反汇编/烧录/CPU 状态**：只作用于“当前目标”。切换当前目标 = 源码窗切换到该目标 ELF 的源文件索引，断点列表按目标过滤。
- 理由：调试器 UI（单步、断点命中、源码高亮）本质是单焦点；同时对多目标单步复杂且易误操作。VS/Lauterbach 多核调试也采用类似“焦点核”模型。
- 可选增强（M5）：“全部暂停/全部运行”广播按钮（顺序执行，日志标注每目标耗时，不保证同步）。
- 仿真（无硬件）：Target.transport=`sim`，即现有 mock/ORP mock 作为一种 transport，统一模型。

## 5. 变量命名
- 规范名：`<target>:<path>`，如 `tc375:g_cfg.gain`、`f407:asin_data[3]`。
- 只有一个激活目标时允许省略前缀（向后兼容旧脚本/工程）；多目标时无前缀名若唯一可解析，否则报 `ambiguous` 并列出候选。
- leaf id 改为 `(target_idx<<24)|local_id`，对外仍是整数，旧 id 在单目标时不变（target_idx=0）。

## 6. 跨目标时间同步
1. **主机时间戳**（默认）：每个采集线程读批次前后各取 QPC，样本时间 = (t_before+t_after)/2；记录批次耗时 = 不确定度。DAS 实测单批约 0.5–3 ms → 时间误差同量级。
2. **每目标固定延迟补偿**：设备管理器里可设 `latency_us`（默认 0），通过“同一信号接入两块板”的标定得出。
3. **目标时间戳**（可选）：若固件提供自由运行计数器变量（如 TC375 STM0、STM32 DWT_CYCCNT），每批读一次，拟合 target_tick→host_us（线性回归，滑窗）消除 USB 抖动；精度可到 ~10 µs。
4. **硬件同步脚**（预留）：GPIO 同步脉冲/外部触发，第一期只定义接口。
- 图表 X 轴统一为主机 µs；曲线 tooltip 显示时间不确定度。

## 7. 插件 API 变更（module_api v6）
- 新增实例化接口，DLL 可多开：
```c
int  (*open)(const OS_Framework* fw, const OS_TargetCfg* cfg, void** inst);  // 每目标一次
void (*close)(void* inst);
int  (*command)(void* inst, int cmd, void* in, void* out);                   // 原 ctx → inst
```
- `fw->log` 已按模块区分；v6 再提供 `fw->log_inst(inst, level, fmt, ...)` 以输出目标别名（`tc375 das: ...`）。
- **同一 DLL 两实例**：
  - stlink：两根 ST-Link 按序列号区分，各自 USB 句柄 → 可行，需去掉全局状态。
  - jlink：JLinkARM.dll 每进程单会话（官方 `JLINK_` API 全局）；多 J-Link 需 `JLinkARM_x64.dll` 复制改名多次 LoadLibrary 或用 J-Link SDK 的多实例 DLL。M3 评估。
  - das：MCD API 支持一个 server 下多 device/core；两 miniWiggler = 同一 tas_server 两个 device → 一个 DLL 内多 `mcd_core_st*`，共享 server 句柄（引用计数）。TC375 多核 = 同 device 多 core。
- v5 模块经适配层包装（单实例，打开第二个实例时返回 `OS_ERR_BUSY`），保证旧 DLL 可用。

## 8. XCP 多从站
- 每个 XCP 从站 = 一个 Target（独立 A2L、独立 DAQ 列表、独立 CAN-ID/IP:port）。
- 同一 CAN 通道上的多个从站共享物理通道对象（引用计数），按 ID 分发；DAQ 时间戳用从站 timestamp（若 A2L 提供）走 §6 方法 3。

## 9. 工程文件格式
`project.osproj`（JSON），新增：
```json
{ "version": 2,
  "targets": [ {"id":"tc375","transport":{"kind":"das","serial":""},"mcu":"TC375","core":0,
                "symbols":"out/tc375_scope.elf","active":true,"rate_cap_hz":2000} ],
  "active_target": "tc375",
  "watch": ["tc375:g_sin1","f407:asin_data[0]"],
  "charts": [ ... 曲线名带前缀 ... ] }
```
读取 v1 工程：自动生成一个 target `t0`（来自原驱动+ELF 设置），变量名不加前缀。

## 10. 日志来源前缀（**已实现**，commit 4aaebe6）
- module_mgr 为每个模块提供独立 `OS_Framework` 副本，`log` 指向按槽位区分的跳板，输出 `<模块名>: <消息>`；消息已带模块名前缀（remote/network/aiapi/das 旧写法）时不重复。
- 多目标后改为 `<目标别名> <模块>: ...`，如 `tc3x das: 已连接`（v6 `log_inst`）。
- 回归：build.bat 17 组 ALL PASS；真板 tc_e2e 通过，日志可见 `das: ...`。

## 11. aiapi / ORP 影响
- 所有与目标相关的方法增加可选 `target` 参数（缺省 = 当前目标；只有一个激活目标时行为不变）：`connect/disconnect/halt/go/reset/mem.read/mem.write/elf.load/vars.list/cpu.state`。
- 新方法：`targets.list/add/remove/set_active/activate`、`targets.stats`（速率、CPU、延迟）。
- 变量类方法接受带前缀名；`vars.list` 返回 `target` 字段。
- ORP 升 v0.8：`hello` 返回 targets；`vars.read` 样本带 `target`；`control.changed` 带 target。remote.dll 作为客户端时 = 一个 Target（transport=orp），其远端多目标映射为 `remote别名/远端target:var`（M5）。

## 12. 性能（一等需求）
### 12.1 基线实测（Tom 的 PC：i5-6200U 2C/4T，背景负载约 51%；TC375 + miniWiggler，10 条曲线，20 s）
| 场景 | OpenScope 进程 | tas_server | 采样速率 |
|---|---|---|---|
| 已连接，未采集 | 0.1 % 单核 | 0 % | 0 |
| 采集 10 变量，无图表窗 | **14.6 % 单核**（3.7 % 总） | **77.6 % 单核** | 16.6 k 样本/s |
| 采集 10 变量 + 图表窗 10 曲线 | **43.9 % 单核**（11 % 总） | 76.9 % 单核 | 16.7 k 样本/s |
脚本：`D:\OpenScope\_agent_test\tc\cpu_base.py [--chart]`。

### 12.2 调研发现的热点
1. **默认自由运行轮询**：`poll_interval_ms=0`（main.c:349），数据线程无间隔连续读 → tas_server 吃满约 0.8 核（主要开销在 DAS 服务端，而非 OpenScope）。每目标都这样则 2 个 DAS 目标即占满弱 PC。
2. **图表重绘**：加图表后 OpenScope 从 14.6% → 43.9%；WM_OS_SAMPLES 每 16 ms 触发 drain + 重绘（≈60 fps），缩放动画 16 ms 定时器；整窗 InvalidateRect，未按像素宽度抽稀。
3. **大样本结构**：`OS_Sample` 含 `text[64]`，环 8192 项全局一把，所有线程/目标共用 → 拷贝量大、多目标需锁。
4. **日志**：每条非主线程日志 malloc + PostMessage + ListView 插入，高频日志（如重连失败）会刷屏卡 UI。
5. **单次读延迟**：DAS 单 4 字节读 ~2.7 ms，必须批量（das.dll 已按 txlist 批量）。

### 12.3 设计措施
- **每目标一个采集线程**，阻塞/批量 I/O：一轮一次 txlist（DAS）/一次 USB 批（stlink）/XCP DAQ 被动接收；**禁止忙等**，周期由 `rate_cap_hz` 决定，用可等待定时器（`CreateWaitableTimerEx` HIGH_RESOLUTION）补睡。
- **每目标采样率上限**：默认 1 kHz/变量组（可调至“最大”）；UI 显示实际速率与上限。修正热点 1：默认不再自由运行。
- **共享环形缓冲**：每目标一个 SPSC 无锁环（采集线程写、UI 读），紧凑样本 `{u64 t_us; u32 leaf; f64 v}`（24B），文本值另存；UI 按需批量读取。
- **UI 重绘解耦**：独立 30 fps 刷新定时器（可设 15/30/60），仅重绘 dirty 曲线区域；每像素列 min/max 抽稀（M4M 风格），绘制成本 ∝ 像素宽 × 曲线数，与采样率无关；窗口最小化/不可见时不重绘。
- **日志限速/批处理**：主线程每 100 ms 批量插入；同源同文重复消息合并（“×N”）；每源 ≥ 50 条/s 时丢弃并提示。
- **未激活目标零开销**：不建线程、不开连接、不占环；仅在设备管理器显示。
- **CPU 预算**：低端 4 线程 PC 上，每个激活目标（10 变量、1 kHz、图表 30 fps）**OpenScope 进程 < 10 % 单核**；探针服务端（tas_server/ST-Link）另列，目标 < 25 % 单核（靠速率上限保证）。
- **性能测试（CI 门槛）**：`perf_multi_target`：mock 驱动 ×4 目标 × 10 变量 × 1 kHz，图表窗打开，运行 30 s，测进程 CPU 时间/墙钟；超预算即 FAIL。真板版本 `cpu_base.py` 手动跑并记录到 docs/perf_log.md。

## 13. 迁移/兼容
- v5 插件经适配层可用（单实例）；aiapi/ORP 不带 target 时行为与今日一致；v1 工程自动转换（§9）；命令行 `--driver/--elf` 仍创建单个 target `t0`。

## 14. 里程碑
| M | 内容 | 验收 |
|---|---|---|
| M0 | 日志前缀（已完成）+ 性能基线 + perf 测试框架 | 本文 §12.1、perf_multi_target 能跑 |
| M1 | 修热点：采样率上限、30fps 重绘、min/max 抽稀、日志批处理（仍单目标） | 10 曲线 OpenScope < 10% 单核 |
| M2 | TargetRt 抽象，`g_app.driver` 90 处改为 `active_target()`；每目标采集线程 + SPSC 环 | 单目标回归全过 |
| M3 | module_api v6 open/close 多实例；das/stlink 多实例；jlink 评估 | 2×DAS 或 TC375+STM32 同时采样 |
| M4 | 设备管理器 UI、分组变量树、工程 v2、变量前缀 | Tom 手测 |
| M5 | aiapi/ORP target 参数、XCP 多从站、目标时间戳拟合 | e2e 多目标脚本通过 |

## 15. 分工（小序仅能用 box，无硬件/Windows GUI）
- **我（Windows+真板）**：UI（设备管理器、变量树、当前目标）、采集线程与驱动 v6 改造、真板测试与性能测量、集成提交。
- **小序（box）**：纯逻辑模块与单测——SPSC 环形缓冲、min/max 抽稀、变量名解析（前缀/歧义）、工程 v1→v2 转换、时间戳线性拟合、日志限速/合并器、mock 多实例 MCD/驱动、aiapi/ORP target 参数的 mock 服务器与一致性测试、perf_multi_target 的 mock 驱动。接口以纯 C 头文件 + CMake 目标交付，我负责接入。

## 16. 需要 Tom 决定
1. 是否采用“当前目标”调试模型（推荐），还是要多目标同时调试窗口？
2. 默认采样率上限（建议 1 kHz/目标；“最大”需手动开启）——会改变现在默认自由运行的行为。
3. 变量前缀分隔符 `:`（与 C++ `::` 可能冲突，可选 `@` 或 `/`）。
4. OpenScope CPU 预算 <10% 单核/目标 是否认可；探针服务端开销是否计入。
5. 多 J-Link 支持优先级（技术风险最高）。
6. 最大目标数（建议 8）。
