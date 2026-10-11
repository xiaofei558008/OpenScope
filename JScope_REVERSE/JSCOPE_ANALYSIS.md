# SEGGER J-Scope 逆向分析报告

**分析对象**：`C:\Program Files\SEGGER\JLink_V966\JScope.exe`（9.66）
**分析日期**：2026-10-11（第二轮全量分析已并入）
**分析工具**：Ghidra 12.1.4 headless（自建 JDK 21 工具链）+ pefile/capstone 辅助
**参考方法论**：REA（morluto/rea）——区分「观测/推导/推断/未知」，保留证据出处

---

## 阅读指引（第二轮更新）

本文件是第一轮的精编版；**完整报告以 `report.html` 为准**（19 节、含 8 个专题的完整细节）。
第二轮把全部 **2,001 个函数**反编译（`dumps/alldecomp/`，2.2 MB C 代码 + 调用图索引），
并按 8 个专题深挖，各成独立报告：

| 专题报告 | 内容 |
|---|---|
| `analysis/trigger.md` | 触发子系统：24 字节触发结构、8 个类型比较器、命中后重开记录、TriggerWidget UI、XML 真身 |
| `analysis/export.md` | CSV / JRAW 导出与 Import Raw：逐字节格式、上界语义、两个真实缺陷 |
| `analysis/graph.md` | 波形渲染：100 px/大格、坐标变换、DrawStyle、自动定标 1-2-5、27 档缩放、FPS |
| `analysis/elf.md` | ELF/DWARF：libdwarf 用法、TypeID 三个产出点、0x3F0 符号记录、符号可见性规则 |
| `analysis/rttplus.md` | RTT / RTT-Plus 逐字节实证（含与 UM08028 文档的差异） |
| `analysis/infra.md` | 作业框架（单 worker + 32 槽环形队列 + 16 项处理器表）、堆守卫、分段日志、时间基准 |
| `analysis/session.md` | 启动序列、DLL 三级查找、配置对话框、INI/注册表、菜单快捷键全表、生命周期 |

**第三轮（未解项攻关）新增工具**：`tools/rip_scan.py`（全镜像 RIP 相对引用扫描，重同步式线性反汇编，
用于回答"谁写了这个全局"）、`tools/dll_hss.py`（JLink_x64.dll 定向反汇编）、
`tools/ghidra_scripts/DecompAt.java` + `tools/run_orphans.bat`（补建"只被数据表引用"的函数 →
`dumps/orphans.txt`）。

**第三轮（未解项攻关）已闭环 8/10** —— 详见 `report.html` §17 与下面两处：

| 未解项 | 结论 |
|---|---|
| `JLINK_HSS_GetCaps` 结构 | **32 字节**：`+0x00` 最大变量数（超出→-3）、`+0x04` 最大采样率 Hz（DLL 用 `1e6/x` 求最小周期，与 J-Scope 同式）、`+0x08` 能力位（bit1 门控 flags.bit0）；能力缺失时回退 `{5, 50}` → 最小周期 20000 µs（= J-Scope 的 20 ms 回退） |
| HSS `Flags=1` | DLL **确实解码**：仅当 `caps+8.bit1` 置位时保留 flags.bit0，否则静默清掉；bit0 物理含义在探针固件 |
| HSS 负返回码 | `-4`=无 HSS 能力、`-3`=变量数超 `caps+0`、`-2`=块数组 malloc 失败 —— 全部在 DLL 的 HSS_Start 里产生 |
| DLL 版本门槛 `0xC79B` | 版本整数是十进制打包（`major=v/10000`）→ **V5.10.99** |
| RTT 读缓冲 | `DAT_14085d1b0` **全镜像无人写** → 缓冲恒为 `0x2000`（8 KB） |
| 调试堆守卫开关 | `DAT_14285eb9c` **全镜像无人写** → 发布版签名/哨兵校验全部关闭 |
| ≈30 MB 静态区 | 108,121 条指令的 RIP 引用扫描**零命中** → 纯虚拟预留，无功能影响 |
| 作业处理器表 | 16 项全部还原；6 个处理器 Ghidra 从未建函数（含"暂停"= 单字节赋值 `DAT_140096DD0=1`） |
| 日志回调 | `0x14000a760` 是 `ret 0` **空桩** —— 日志只累积在 4 KB 全局缓冲，段末弹框 |
| 采样率 10× | 三处独立证据（µs 换算、超时×1000 比较点、显示路径无补偿）确认公式恒为真值 10 倍；只需硬件对照定案 |

---

## 真机实测结果（2026-10-12，J-Link PRO + STM32F303RC）

三项待实测项全部闭环（完整数据见 `report.html` §17）：

| 项 | 结果 |
|---|---|
| **HSS caps / flags** | `JLINK_HSS_GetCaps` 32 B = `{maxBlocks=100, maxRateHz=100000, capBits=0x2, …0}`；**`flags.bit0` = 时间戳时基（1=µs，0=ms）**，实测比率 0.994 vs 0.001；J-Scope 恒传 1 |
| **采样率 10 倍因子** | 请求 100 µs → 真值 **9868 Sa/s**（时间戳中位增量 99 µs），J-Scope 状态栏显示 **81 kSamples/s** → **放大 ≈8.2×**（公式为 10×）✓ 缺陷坐实 |
| **RTT-Plus 配置串** | 实测 48 B：每条 **16 B = {addr, 0, size, 0}** ×2 + **16 B 全 0 终止** —— 与 SEGGER 文档（8 B/条）**不一致**；目标端不回应则报 *"Timeout. No response from target!"*（需回 256 B 0） |

附带产出：`.jscope` 读取端的硬性要求（XML 声明、`<Windows>`、`Scope/Options TimeResolutionUs`、
`Trigger/Options` 六属性、符号 18 属性齐全）与可用参考工程 `f303_debug.jscope`；
台架 `tools/jlink_lab.py`（ctypes 直调 JLink_x64.dll）、GUI 自动化 `lab/*.ps1`（DPI 感知）。

**第二轮对第一轮的修正（重要）**

1. **RTT 读缓冲是 `max(0x2000, 通道缓冲)`，不是 min**（`FUN_14003ef60`）。
2. **TypeID 3/4 没有任何产出点**（第一轮猜的 float128/half 无代码支撑）；**新增 TypeID 14 = 位域**（可观测）。
3. **配置结构偏移修正**：JTAG_IRPre 在 `+0x5B4`、DRPre 在 `+0x5B0`、`+0x5B8` 是"会话模式"标志（不是 JTAG 值）。
   根因：XML 读写用的"项目结构"基址比运行期 Configuration **高 8 字节**，混用会得到矛盾字段表。
4. **符号记录是 0x3F0 字节**（`FUN_140040ab0`），`+0x3D8` 是首子指针而非记录尾。
5. **`File too large - maximum file size (%d bytes)` 属 `JScopeSettings.ini`**（上限 30000 B），与导出无关。
6. **`FUN_14003f110` 是 ERROR 日志行**（段末汇总成弹窗），不是直接弹窗。
7. **RTT-Plus 配置串与 SEGGER 文档不一致**：实测每条 16 字节（`+0 Addr / +4 0 / +8 Size / +12 未初始化`），
   终止 12 字节 0；文档写的是 8 字节/条 + 8 字节 0。**以二进制为准，需实测确认**（见 report.html §17#4）。
8. **采样率显示疑有 10 倍因子**（`numSamples×10⁷/elapsed_µs`），已列入未知点。
9. **采集过程不落盘**；`<DataFile>/<RawFile>` 只是"上次导出路径"。
10. **`CDeviceDataModel` 等 `CDevice*` 类是 MCU 选型对话框**，不是符号树（符号树在 SymbolSelection / QTreeWidget）。

---

## 0. 证据等级说明

| 标记 | 含义 |
|---|---|
| **[观测]** | 直接来自反汇编/字符串/PE 结构，可复核 |
| **[推导]** | 由观测经确定性推理得到（如结构体字段偏移） |
| **[推断]** | 合理但未完全证实的解释 |
| **[未知]** | 本次未解决 |

所有地址为 **镜像基址 `0x140000000`** 下的虚拟地址（VA）。

---

## 1. 二进制基本面 [观测]

| 项目 | 值 |
|---|---|
| 格式 | PE32+ x86-64 |
| 链接器 | MSVC 14.29（VS2019 工具链） |
| 编译时间戳 | 2026-08-05 10:18:02 UTC |
| 体积 | 679,920 字节（.text 371,712） |
| GUI 框架 | **Qt4**（`QtCore4.dll` 141 导入 / `QtGui4.dll` 1034 导入） |
| 调试器后端 | **动态加载** `JLink_x64.dll`（**无静态导入**） |
| ELF 解析 | **静态链接 libdwarf** |
| 版本资源 | `ProductVersion = 9.66`；`FileVersion` 字段损坏（内容为 `\tB`）[观测] |
| PDB 路径残留 | `D:\Work\JLink\Output\Release\x64\JScope.pdb` [观测] |

**关键结构发现**：`.data` 段虚拟大小 **41,776,096 字节（≈39.8 MB）** —— 远超常规，因为其中静态驻留了一个 **32 MB 时间戳环**（见 §6）。

---

## 2. 总体架构 [观测/推导]

```
┌─────────────────────────────────────────────────┐
│ JScope.exe  (Qt4 GUI, 单进程)                    │
│                                                  │
│  MainWindow ──┬── GraphContainer / GraphWidget   │
│               ├── SymbolSelection (变量树)        │
│               ├── TableWidget (数值窗口)          │
│               ├── TriggerWidget                  │
│               └── DIALOG_CustMsgBox              │
│                                                  │
│  异步 Job 队列 (命名 Job, 工作线程执行)            │
│   ├ APP: <CreateProject> / <Init> / <Exit>       │
│   ├ APP: <ParseELFFile>                          │
│   ├ APP: <ReadStart> / <ReadPause> / <ReadStop>  │
│   ├ APP: <ReadPeriodic>   ← 10 ms 周期采样泵      │
│   ├ APP: <CheckELFModifications>                 │
│   └ APP: <UpdateProjSettings> / <SetAppStartAction>│
│                                                  │
│  自研带守卫的堆分配器                              │
│   ("Wrong alloc signature in memory block !"     │
│    "Buffer overflow in memory block. %d ...")    │
└───────────────┬─────────────────────────────────┘
                │ GetProcAddress(按名字，29 个 API)
                ▼
        JLink_x64.dll  ──USB/TCP──▶  J-Link 探针 ──SWD/JTAG──▶  MCU
```

**DLL 查找顺序** [观测]（字符串 `JLink_x64.dll`、`bin_x64\JLink_x64.dll`、注册表键）：
1. `JLink_x64.dll`（当前目录/搜索路径）
2. `bin_x64\JLink_x64.dll`
3. 注册表 `HKLM\Software\SEGGER\J-Link\InstallPath`、`HKLM\Software\SEGGER\Flasher\InstallPath`

> **对 OpenScope 的意义**：J-Scope 不静态链接 JLinkARM，而是运行时按名解析 —— 这样同一个 exe 能适配任意 J-Link 版本。OpenScope 若想同时支持 J-Link / ST-Link，应沿用「后端抽象 + 运行时加载」这一模式。

---

## 3. J-Link API 使用清单（29 个）[观测]

全部通过 `GetProcAddress` 按**字符串名**解析，并由懒加载 thunk 缓存（每个 thunk 形如 `if (!pFn) pFn = GetProcAddress(h, "NAME"); if (!pFn) return; pFn(...)`）。

### 3.1 连接与配置（22）
```
JLINKARM_OpenEx            JLINKARM_Close
JLINKARM_EMU_SelectByUSBSN JLINK_EMU_SelectByUSBNickname
JLINKARM_SelectIP
JLINKARM_DEVICE_SelectDialog  JLINKARM_DEVICE_GetInfo
JLINKARM_TIF_Select        JLINKARM_SetSpeed      JLINKARM_ConfigJTAG
JLINKARM_ExecCommand       JLINK_ExecCommand
JLINKARM_Connect           JLINKARM_Reset
JLINKARM_Halt              JLINKARM_Go            JLINK_Go
JLINKARM_IsHalted          JLINK_IsHalted
JLINKARM_EMU_HasCapEx
JLINKARM_GetDLLVersion     JLINKARM_GetCompileDateTime
```

### 3.2 采集（7）—— **核心**
```
JLINK_HSS_GetCaps          JLINK_HSS_Start
JLINK_HSS_Read             JLINK_HSS_Stop          ← 高速采样(异步)
JLINK_RTTERMINAL_Read      JLINK_RTTERMINAL_Write
JLINK_RTTERMINAL_Control                           ← RTT(同步)
```

> ⚠️ **重要**：`JLINK_HSS_*` 属于 **J-Link SDK 功能，SEGGER 未公开文档化**（KB 仅说明它是 SDK 一部分，J-Scope / Ozone / Embedded Studio 使用它）。但函数确实从 `JLinkARM.dll` 导出，可被第三方调用。本次逆向的 §5 即为其真实调用契约。

---

## 4. 连接序列 [观测] `FUN_1400419f0`

```
1) 若 (TIF == 0 /*JTAG*/ || TIF == 7) && JTAG_IRPre >= 0
       JLINKARM_ConfigJTAG(JTAG_IRPre)
2) JLINKARM_ExecCommand("SetSkipInitECCRAMOnConnect = 1", NULL, 0)
3) 若 设备名为空 或 设备查找失败:
       idx = JLINKARM_DEVICE_SelectDialog(parent, 0, 0)     // 弹窗选型
       JLINKARM_DEVICE_GetInfo(idx, &info)                  // 0x978 字节结构
   否则:
       JLINKARM_ExecCommand("device = <name>", errbuf, 0x400)
4) 若有脚本: JLINKARM_ExecCommand("ScriptFile = <path>", errbuf, 0x400)
5) JLINKARM_TIF_Select(TIF)
6) JLINKARM_SetSpeed(speed_khz)
7) 若命令行给了 -RTTSearchRanges:
       JLINK_ExecCommand("SetRTTSearchRanges <addr>,<size>[,...]", 0, 0)
8) rc = JLINKARM_Connect();  rc < 0 → 失败
```

**HSS 能力探测与时间分辨率** [观测]（同函数尾部）：

```c
if (mode == HSS) {
    if (!JLINKARM_EMU_HasCapEx(0x35)) {          // 0x35 = HSS 能力位
        period_us = 20000;                       // 回退 20 ms 轮询
    } else {
        if (JLINKARM_GetDLLVersion() > 0xc79b) { // DLL 版本门限 [推断]
            JLINK_HSS_GetCaps(&caps);
            TimeResUs = 1000000 / caps.field_at_4;   // caps 结构 +4 处字段
        } else {
            period_us = 10;
        }
    }
}
```

> **[推断]** `caps.field_at_4` 应为 HSS 时间戳频率（Hz），故 `1e6 / freq = µs/tick`。若它是「ns/tick」，此式量纲不符。**此点为重实现时最需要实测确认的一处。**

**⚠️ 重要修正 [观测]**：该全局（`_DAT_14285da90`）经 Ghidra 全量交叉引用确认**只有 2 处写入、0 处读取**（写入点：`FUN_1400419f0` 的 caps 换算、`FUN_14003f400` 的纯 setter）。即：

- 本版本中该「时间分辨率」是**写后即弃的残留状态**；
- 更关键的是 —— **样本里的时间戳是原样存储的**（`FUN_14003ff40` 直接 `tsRing[i] = sample[0]`，无任何缩放）。

**结论：HSS 探针给出的时间戳本身就是 µs**，主机侧无需换算。这大大简化了 OpenScope 的重实现。

---

## 5. HSS 高速采样协议（异步模式）[观测]

### 5.1 变量描述符数组

`FUN_140041f30` 构造并下发：

```c
// 每个变量 16 字节；末尾追加一个 {addr=0,size=0} 终止项
struct HssVar {          // 16 bytes
    uint32_t addr;       // +0x00  变量绝对地址
    uint32_t size;       // +0x04  字节数
    uint32_t reserved0;  // +0x08  = 0
    uint32_t reserved1;  // +0x0C  = 0
};

uint32_t n = 0;
HssVar *list = malloc((n + 1) * 16);       // +1 给终止项
memset(list, 0, (n+1) * 16);

uint32_t off = 4;                          // ★ 从 4 开始：前 4 字节是时间戳
for (each selected variable v) {
    v->sampleOffset = off;                 // 记在 v+0x38C
    off += v->size;                        // v+0x31C
    list[i].addr = v->addr;                // v+0x318
    list[i].size = v->size;
    /* 同时复位统计量：
       v->yMaxSampled(+0x398) = -DBL_MAX   (0xFFEFFFFFFFFFFFFF)
       v->yMinSampled(+0x3A0) = +DBL_MAX   (0x7FEFFFFFFFFFFFFF)
       v->writePtr(+0x3C8) = v->dataBuf(+0x3D0) = 0  */
}
list[n].size = 0;                          // 终止项

rc = JLINK_HSS_Start(list, n, samplingPeriod_us /*config+0x5A8*/, 1 /*flags*/);
```

**返回码** [观测]（错误串逐字来自二进制）：
| rc | 含义（原文字符串） |
|---|---|
| `-2` | `Failed to allocate memory on debug probe` |
| `-3` | `Too many variables specified` |
| `-4` | `Target hardware does not support HSS` |
| 其它 <0 | `Failed to inititalize HSS.`（原文拼写错误 `inititalize`） |

启动成功后立即：
```c
if (JLINK_IsHalted()) JLINK_Go();     // 让目标跑起来
```

### 5.2 采样包格式 [观测]

`JLINK_HSS_Read(buf, size)` 返回本次读到的**字节数**，缓冲区内容为紧密排列的定长样本：

```
每个样本 = sampleSize 字节（= 4 + Σ(变量size)）
┌────────────┬──────────────┬──────────────┬─────
│ ts (u32,µs)│ var0 数据     │ var1 数据     │ ...
│  offset 0  │ offset 4     │ offset 4+s0  │
└────────────┴──────────────┴──────────────┴─────
```

变量在其样本内的偏移在 `JLINK_HSS_Start` 之前就固定算好（`v+0x38C`），读取后直接按偏移切片。

### 5.3 读取泵 [观测] `FUN_140040fa0`

```c
int bufSize = 0x200000 - (0x200000 % (sampleSize * numVars));  // ≈2 MB 对齐
void *buf = malloc(bufSize);
int n = JLINK_HSS_Read(buf, bufSize);      // 返回字节数
if (n >= 0) UnpackSamples(n, buf);
free(buf);
```

由 **10 ms 周期 Job**（`APP: <ReadPeriodic>`，`job->period = 10`）驱动 —— 即探针在后台以设定周期自主采样并缓存，主机每 10 ms 批量取走积压数据。**采样率与主机轮询频率解耦**，这正是 HSS 能到 µs 级的原因。

---

## 6. 数据模型：双环结构 [观测]

**这是 J-Scope 高性能绘图的核心设计。**

### 6.1 时间戳环（静态，32 MB）

```c
static uint32_t tsRing[0x800000];   // 8,388,608 × 4 B = 32 MB
uint32_t writeIdx;                  // DAT_14285da80，0..0x7FFFFF 循环
uint32_t wrapped;                   // DAT_14285da78  是否已绕回
uint32_t lastIdx;                   // DAT_14285da7c
uint32_t curTs;                     // DAT_14285da84
```

**时间戳语义** [观测] `FUN_14003ff40` —— 三种模式分支：

```c
if (mode == HSS)            tsRing[writeIdx] = sample[0];            // 探针给的 µs
else if (rttWithTimestamp)  tsRing[writeIdx] = sample[0];            // 目标给的 µs
else {                      // RTT 无时间戳 → 合成 100 µs 均匀栅格
    if (writeIdx != 0)            tsRing[writeIdx] = tsRing[lastIdx] + 100;
    else if (wrapped)             tsRing[0]        = tsRing[lastIdx] + 100;
    else                          tsRing[0]        = 0;              // 首个样本 t=0
}
```

与二进制中告警串逐字吻合：
> `RTT channel is set up to provide no timestamp!`
> `Without timestamp J-Scope will assume that data points are uniformly distributed with a gap of 100 [us] each.`

### 6.2 每变量数据环（堆，**每变量 64 MB**）[观测]

```c
memcpy(v->dataBuf + writeIdx * 8,        // ★ 固定 8 字节步长
       sample + v->sampleOffset,
       v->size);                          // 1/2/4/8 字节实际值
```
分配处：`memset(v->dataBuf, 0, 0x4000000)` = **67,108,864 B = 64 MB / 变量**。

即 8,388,608 样本 × 8 B/样本。**无论变量是 1/2/4/8 字节，都占 8 字节槽位** —— 用内存换 O(1) 均匀索引。

> **代价与启示**：4 变量 = 256 MB，8 变量 = 512 MB。OpenScope 若做长时间记录，应改为**按类型宽度紧凑存储**（1/2/4/8 B），或分段/落盘。这是 J-Scope 一个明显的可优化点。

### 6.3 样本解包 [观测] `FUN_14003fa70`

```c
void UnpackSamples(uint32_t nBytes, uint8_t *p) {
    numSamples = nBytes / sampleSize;              // DAT_14085d1ec（用于算采样率）
    for (each sample) {
        StoreTimestamp(p);                          // 写时间戳环
        for (each variable v) {
            StoreVarSample(v, p);                   // memcpy 到 v 的环
            /* 触发判定 FUN_1400415a0(ring, v, prevPtr, curPtr) → 0/1/2/3 */
            /* 越界/回绕处理；命中触发则调 FUN_140042ac0(v) */
        }
        p += sampleSize;
        writeIdx = (writeIdx + 1) & 0x7FFFFF;
    }
}
```

**采样率计算**（`FUN_140041710` 尾部）[观测]：
```c
elapsed = timer_ticks();                                  // 100 ns 单位 [推断]
samplesPerSec = (uint)(numSamples * 10000000 / elapsed);   // 状态栏 "kSamples/s"
```
对应 UI 字符串 `kSamples/s`、`Samples/s`、`FPS: `、`Buffer: `。

---

## 7. RTT 同步模式 [观测]

### 7.1 两种子模式 `FUN_140042100`

| 子模式 | 条件 | 上行通道 | 下行通道 |
|---|---|---|---|
| **RTT-Plus / Data** | `config+0x5A6 != 0` | `JScope_Data` (+ 可选 `JScope_Data_t`) | `JScope_Config` |
| **RTT / VarFormat** | `config+0x5A6 == 0` | 前缀 `JScope_` 的任意上行通道 | — |

启动时先 `JLINK_RTTERMINAL_Control(0, 0)`。

### 7.2 VarFormat 编码 —— **本次逆向的关键成果** [观测]

上行通道名形如 **`JScope_<VarFormat>`**，`VarFormat` 是**「类型字符 + 尺寸数字」的成对串**，例如：

```
JScope_t4i4f4      →  时间戳 + int32 + float32
JScope_u2i2u4      →  uint16 + int16 + uint32
JScope_t4f4f4f4    →  时间戳 + 3 个 float32
```

解析逻辑（`FUN_14003fc90`）：跳过首个 `_` 之前的内容，之后**严格每次取 2 个字符** `(type, sizeDigit)`（含 `t`，它同样消耗一位）：

| 类型字符 | 尺寸数字 | TypeID | 含义 |
|---|---|---|---|
| `t` | （仍占 1 位，值被忽略） | — | **时间戳标志**（置 `IsRTTWithTimeStamp`） |
| `b` | 1 | 5 | bool |
| `f` | 4 | 1 | float32 |
| `f` | 8 | 2 | float64 |
| `u` | 1 / 2 / 4 / 8 | 6 / 7 / 8 / 9 | uint8/16/32/64 |
| `i` | 1 / 2 / 4 / 8 | 10 / 11 / 12 / 13 | int8/16/32/64 |
| 其它 | — | 忽略 | — |

尺寸数字即实际字节数（`'1'=1, '2'=2, '4'=4, '8'=8`）。解析出的类型用于 `AddSymbolFromFormat(size, typeID, 0, desc)`，描述串即二进制中的：
```
"%d Byte bool RTT Data %d"      "%d Byte float RTT Data %d"
"%d Byte unsigned RTT Data %d"  "%d Byte signed RTT Data %d"
```

失败时的两条例外路径（原文）：
> `No RTT Up-Channel named "JScope_%Format%" found`
> `The first RTT Channel named JScope_... found did not contain a valid VarFormat`

> ⚠️ **注意**：RTT-VarFormat 模式下**目标端无需 ELF** —— 通道名自带布局。这是与 HSS 模式的本质差异。

### 7.3 RTT 数据读取 [观测] `FUN_140042260`

RTT 无「整样本」保证，读取会在样本中间截断，因此 J-Scope 用 **≤64 字节的残留缓冲**拼接跨读取的半个样本：

```c
if (targetErrorPending()) { partialLen = 0; /* 清状态 */ }
else partialLen = carryLen;                          // 上次残留

memcpy(buf, carry, partialLen);                       // 先补上残留
n = JLINK_RTTERMINAL_Read(ch, buf + partialLen, min(readBufSize, 0x200000) - partialLen);

total = n + partialLen;
remainder = total % sampleSize;
UnpackSamples(total - remainder, buf);                // 只解完整样本
if (remainder == 0) carryLen = 0;
else { carryLen = min(remainder, 0x40);               // ★ 上限 64 字节
       memcpy(carry, buf + (total - remainder), carryLen); }
```

RTT 读缓冲：`readBufSize = max(0x2000, channelBufSize)`（**第二轮修正：原为 min，实测为 max**），
分配标签 `"RTT read buffer allocation"`；跨读取残留缓冲上限 64 字节（`DAT_14285da30`）。

**关键点**：HSS 与 RTT **共用同一个 `UnpackSamples`**（`FUN_14003fa70`）—— 上层数据模型完全统一，只有「字节从哪来」不同。这是非常干净的架构。

### 7.4 RTT 控制通道 [观测] `FUN_140040ed0`

```
循环最多 101 次：
   n = JLINK_RTTERMINAL_Read(ch, tmp, 256)
   累计满 256 字节 →
       若 tmp[0] != 0 → 报 "Target Error: %s" 并返回失败
       否则返回成功
   Sleep(1)
超时 → 返回 0xFFFFFFFE
```

---

## 8. 类型系统（TypeID）[观测]

`TypeID` 存于变量记录 `+0x314`，写入 `.jscope` 的 `TypeID` 属性。

| TypeID | 类型 | 树中可选 | 备注 |
|---|---|---|---|
| 1 | float32 | ✔ | `size==4 && type=="float"`，MaxValue=FLT_MAX |
| 2 | float64 | ✔ | `size==8 && type=="double"`，MaxValue=DBL_MAX |
| 3 / 4 | **保留槽（无产出点）** | ✘ | **第二轮确证：全程序无任何代码写入 3/4**，只在 `FUN_1400504d0` 的禁用名单里 |
| 5 | bool | ✔ | `size==1 && (type=="bool"‖"_Bool")`，MaxValue=1.0 |
| 6–9 | uint8/16/32/**64** | 6,7,8 ✔ / **9 ✘** | 无符号 + size，MaxValue=255/65535/4294967295/2⁶⁴ |
| 10–13 | int8/16/32/**64** | 10,11,12 ✔ / **13 ✘** | 有符号 + size，MaxValue=127/32767/2147483647/2⁶³ |
| **14 (0xE)** | **位域** | ✔ | **第二轮新增**：DWARF `DW_AT_bit_size` 存在时；位宽 `+0x338`、位偏移 `+0x33C`，取样 `(raw>>shift)&((1<<width)-1)` |

不支持时 UI 行为 [观测]：`QTreeWidgetItem::setDisabled(true)` + 第 4 列文本 `"Symbol type is not supported"`。

> **[观测·第二轮]** TypeID 3/4 无产出点；第一轮的"float128/half"推测不成立。
> 判定函数 `FUN_140040050` 的完整分支见 `analysis/elf.md` §3。
> **J-Scope 9.66 无法观测任何 64 位整型**（9/13 分类得出但树中禁用）。

---

## 9. 变量记录布局（0x5BC 配置结构 / 变量结构）[推导]

由 `.jscope` 写出函数 `FUN_1400468c0` 的字段访问顺序还原：

| 偏移 | 类型 | XML 属性 | 说明 |
|---|---|---|---|
| `+0x004` | char[] | `Name` | 叶名 |
| `+0x108` | char[] | `NameWithParents` | 全路径名 |
| `+0x110` | char[] | `Type` | 类型名 |
| `+0x210` | char[] | `File` | 来源文件 |
| `+0x314` | int | `TypeID` | 见 §8 |
| `+0x318` | uint32 | `Address` | 绝对地址 |
| `+0x31C` | int | `Size` | 字节数 |
| `+0x348` | int | `DrawStyle` | Line/Points/Line&Points/Rectangle |
| `+0x350` | double | `YOffset` | |
| `+0x358` | int | `YResolution` | |
| `+0x35D` | bool | `IsAutoConfig` | |
| `+0x35E` | bool | `ShowInGraph` | |
| `+0x35F` | bool | — | 树中是否显示勾选框 |
| `+0x378` | `QColor*` | `GraphColorRed/Green/Blue` | |
| `+0x388` | bool | — | 本周期是否有数据 |
| `+0x38C` | uint32 | `Offset` | **样本内字节偏移** |
| `+0x398` | double | `YMaxSampled` | 初值 `-DBL_MAX` |
| `+0x3A0` | double | `YMinSampled` | 初值 `+DBL_MAX` |
| `+0x3A8` | double | `YAvgSampled` | |
| `+0x3B0` | double | `MaxValue` | |
| `+0x3C8` | `long long` | — | 数据环写指针 |
| `+0x3D0` | `void*` | — | 64 MB 数据环基址 |

**配置结构（0x5BC 字节）** 关键字段：

| 偏移 | 含义 |
|---|---|
| `+0x38C` | `ScriptFile` |
| `+0x59C` | `TargetInterface`（0=JTAG, 1=SWD, 7=JTAG 变体） |
| `+0x5A0` | `InterfaceSpeed` (kHz) |
| `+0x5A4` | 模式：0=HSS, 1=RTT |
| `+0x5A5` | `IsRTTWithTimeStamp` |
| `+0x5A6` | RTT 子模式（Data/Plus） |
| `+0x5A8` | `SamplingPeriod` (µs) |
| `+0x5AC` | 连接类型：0=USB, 2=TCP/IP, 4=会话 |
| `+0x5B0` | **`JTAG_DRPre`**（第二轮修正） |
| `+0x5B4` | **`JTAG_IRPre`**（真正传给 `JLINKARM_ConfigJTAG` 的值） |
| `+0x5B8` | **会话模式标志**：置位时停止采集**跳过 `JLINK_HSS_Stop`** |

> ⚠ **读偏移前先确认基址**：XML 读写走"项目结构" `P`（0x5E8），运行期配置 `C`（0x5BC）是它的 `+0x08` 成员，
> 因此同一字段在两处的偏移天然相差 8（如 `IsRTTSession`：写方读 `C+0x5A4`、读取方写 `P+0x5AC`）。
> 第一轮把两者混看，才出现"+0x5B0 是 IRPre"这类偏差。

---

## 10. 文件格式

### 10.1 工程文件 `.jscope`（XML）[观测]

由 `FUN_140047020`（读）/ `FUN_1400471d0`（写）确认的层次与属性名：

```xml
<JScopeProject>
  <FileVersion major=".." minor=".."/>
  <Project>
    <Configuration>
      <Connection>
        <TargetInterface/><InterfaceSpeed/>
        <SerialNo/><Nickname/>
        <JTAG_IRPre/><JTAG_DRPre/><ScriptFile/>
      </Connection>
      <TimeResolutionUs/>  <SamplingPeriod/>
      <IsRTTSession/>  <IsRTTWithTimeStamp/>  <IsRTTPlus/>
      <ElfFile/>  <JLink/>  <Scope/>  <Trigger>
        <Level/><Auto/><Rise/><Fall/>
      </Trigger>
    </Configuration>
    <SelectedSymbols>
      <Symbol>
        <Options Name=".." Type=".." Size=".." Address=".." TypeID=".."
                 GraphColorRed=".." GraphColorGreen=".." GraphColorBlue=".."
                 YMinSampled=".." YMaxSampled=".." YAvgSampled=".."
                 Offset=".." ShowInGraph=".." YOffset=".." IsAutoConfig=".."
                 YResolution=".." MaxValue=".." File=".." DrawStyle=".."
                 NameWithParents=".."/>
      </Symbol>
    </SelectedSymbols>
    <DataFile/>  <RawFile/>  <Timestamp/>
  </Project>
</JScopeProject>
```

读入时**强依赖层次**：`JScopeProject → FileVersion → Project → Configuration/SelectedSymbols`，缺失任一层即失败。

### 10.2 设置文件 `JScopeSettings.ini` [观测]

段落：`DataFiles` / `ProjectFiles` / `ELFFiles` / `LogLevel` / `Current` / `PROJECT`，键 `PATH`；MRU 列表格式 `%sRecent File List %d\`、条目 `[%d]: %s`。
注册表：`Software\SEGGER\%s\`（`%s` = "J-Scope"）。
解析错误串：`Error while parsing subkey "%s" in settings file. Syntax error.`

### 10.3 数据导出（第二轮逐字节补齐，详见 `analysis/export.md`）

| 功能 | 实现 | 格式要点 |
|---|---|---|
| Export Data…（Ctrl+D） | `FUN_140047390`，**同步** | CSV 文本：`Timestamp;名1;名2…`，**分隔符是分号**；时间戳 `%u`、浮点 `;%f`(6 位)、整型 `;%i`（无符号也按有符号打印）；TypeID 3/4/9/13 **整列不输出**；范围 = 物理下标 `0..writeIdx-2`（最新样本不导出；**`writeIdx==0` 时下溢成 2³² 次循环 —— 真实缺陷**） |
| Export Raw…（Ctrl+R） | `FUN_1400476d0`，**同步** | **JRAW：无头裸样本流** `repeat N { u32 ts_us; var0.size; var1.size; … }`，小端、无对齐；N = `lastIdx`（绕回后 `0x800000`，逻辑下标从最老样本开始）；变量顺序 = 变量树顺序 = HSS 描述符顺序 |
| Import Raw（Ctrl+T） | 作业 `APP: <ImportRaw>` → `FUN_140046e80` | 只校验 `fileSize % sampleSize == 0`（不满足仅提示）与容量上限 `nSym×64 MB + 32 MB`；**清双环 → 强制"有时间戳" → 复用实时 `UnpackSamples`** |

**采集过程不落盘**：全二进制只有三处数据文件 I/O（CSV 写、JRAW 写、JRAW 读）。
`<DataFile>/<RawFile>` 仅是"上次导出路径"。

> **[第二轮修正]** `File too large - maximum file size (%d bytes) exceeded!` 属 **`JScopeSettings.ini` 读取器**
> （上限 30000 字节，`FUN_14004f590`），**与数据导出无关**。

---

## 11. 命令行接口 [观测]

| 选项 | 说明 |
|---|---|
| `-openprj <FILENAME>` | 打开已有工程 |
| `-USB <SN/nickname>` | 按序列号/昵称选择 USB 探针（**仅可与 `-openprj` 同用**） |
| `-IP <hostname/SN/nickname>` | 选择 TCP/IP 探针（同样仅配 `-openprj`） |
| `-RTTSearchRanges <A>,<S>[,<A1>,<S1>,...]` | 定义 RTT 搜索范围；转为 `SetRTTSearchRanges` 下发给 DLL |

`SuppressGUI = 1/0` 会作为 ExecCommand 下发。

---

## 12. 关键结论与对 OpenScope 的启示

### 12.1 三条最有价值的结论

1. **HSS 是 OpenScope 高速采样的现成答案。**
   `JLINK_HSS_Start(变量表, 数量, 周期µs, 1)` + 周期 `JLINK_HSS_Read` 到 2 MB 缓冲。
   探针**自主**按周期采样并缓存、带 µs 时间戳、主机批量取走 —— 采样率与 USB 往返解耦。
   这正对应 request.md 第 15 条「第二步：真 µs 采样（片上缓冲，XCP 式）」，
   且**不需要在 MCU 上跑采样代码**（HSS 走 AHB-AP 后台访问，不占用 DWT/SWO）。
   > 注意：HSS 属 J-Link SDK 功能，无公开文档；本报告的 §5 是可直接使用的调用契约。
   > 另有现成开源参考：ARMmbed/pyOCD 生态外的 STMViewer（见文末来源）。

2. **HSS 与 RTT 可以共用同一数据模型。**
   J-Scope 把两者的字节流都喂给同一个 `UnpackSamples`，统一成「定长样本 + 首 4 字节 µs 时间戳」。
   OpenScope 的网络模块（request.md 第 14 条）应当照搬这个抽象：
   **采集后端只负责产出「带时间戳的定长样本流」，压缩/传输/绘图全部复用**。
   这也直接解决「本地 ST-Link/J-Link 采集 → 网络传给远端」的接口设计。

3. **RTT VarFormat 让目标端零 ELF 依赖。**
   通道名 `JScope_t4i4f4` 自带完整布局。OpenScope 若要在网络对端（无 ELF）显示波形，
   这是最省事的方案 —— 发送方把变量布局编码进通道名/握手包即可。

### 12.2 建议 OpenScope 抄 / 不抄的地方

| 项 | 建议 | 理由 |
|---|---|---|
| 运行时按名解析后端 DLL | **抄** | 已在 OpenScope 网络模块验证过的模式；同时兼容 J-Link/ST-Link |
| 双环数据模型（时间戳环 + 每变量环） | **抄，但改进** | 索引 O(1)、绘图简单；但 8 B/样本固定槽位太浪费 |
| 8,388,608 样本固定深度 | **不抄** | 应改为可配置深度或分段落盘（OpenScope 已有历史回传需求） |
| 每变量 64 MB 预分配 | **不抄** | 8 变量即 512 MB；应按类型宽度紧凑存储（1/2/4/8 B） |
| 时间戳统一为 µs、uint32 | **抄** | 与 J-Scope 对齐便于互操作；注意 2^32 µs ≈ 71.6 分钟后回绕 |
| 无时间戳时合成 100 µs 栅格 | **抄** | 低成本兜底，避免绘图断链 |
| RTT 跨读取 ≤64 B 残留拼接 | **抄** | 流式协议必备；比「丢弃半样本」健壮得多 |
| 10 ms 采样泵 + 2 MB 批读 | **抄** | 让探针/目标做缓冲，主机不与采样率耦合 |
| 带守卫的自研堆分配器 | **可选** | 调试期价值高（能抓越界/野指针），发布期有开销 |
| `JLINKARM_DEVICE_SelectDialog` 选型弹窗 | **不抄** | 与 request.md 第 17/21 条「不要弹窗选芯片」直接冲突 |

### 12.3 已知坑（逆向中发现，第三轮更新）

- `JLINK_HSS_Start` 第 4 参数恒为 `1`（flags）[观测]；第三轮在 DLL 内证实：**flags.bit0 受 `caps+8.bit1` 门控**
  （不支持时被静默清零），位义本体在探针固件。
- **HSS 错误码全部来自 DLL**：`-4` 无 HSS 能力 / `-3` 变量数超上限（`caps+0`）/ `-2` 块数组分配失败。
- **采样周期会被 DLL 静默钳位**到探针上限（`max(period, 1e6/caps+4)`），J-Scope 不提示 ——
  OpenScope 应主动读 `caps+4` 并提示用户。
- **`JLINK_HSS_Read` 返回字节数**（DLL 内部即为 `memcpy` 长度），按 `nBytes/sampleSize` 解包正确。
- `caps.field_at_4` 单位需实测（`1e6/x` 得 µs → 应为 Hz）；该全局**只写不读**，不影响采样。
- **采样率显示疑有 10 倍因子**（§6），需实测确认。
- **RTT-Plus 配置串与 SEGGER UM08028 文档不一致**（16 B/条 vs 文档 8 B/条），以二进制为准并需实测。
- TypeID 3/4 是保留槽（无产出点）；**64 位整型（9/13）不可观测**。
- `-USB` / `-IP` **必须**与 `-openprj` 同用（两阶段解析：phase2 只跑 `-openprj`，phase1 才跑其余），
  否则分别报 `"-USB/-IP should only be used in combination with -openprj."`。
- RTT 模式**强制要求**指定目标器件：`In RTT mode specifying the target device is mandatory.`
- CSV 导出在 `writeIdx==0` 时下溢（2³² 次循环）；JRAW/CSV **都丢掉最新那个样本**。
- 全部未解项与实测方案见 `report.html` §17。

---

## 13. 本次分析产出物

| 路径 | 内容 |
|---|---|
| **`report.html`** | **完整报告页（19 节，含 8 个专题的全部细节，双主题单文件）** |
| `JSCOPE_ANALYSIS.md` | 本报告（精编版 + 第二轮修正说明） |
| `analysis/*.md` | 8 个专题报告：trigger / export / graph / elf / rttplus / infra / session |
| `dumps/alldecomp/` | **全部 2,001 个函数反编译**（9 分块 + `index.tsv` 调用图索引） |
| `dumps/globals.txt` | 4,837 个全局变量（地址/类型/初值/引用者） |
| `tools/ghidra_scripts/*.java` | 7 个 Ghidra 脚本：`DumpAllDecomp` / `DumpGlobals` / `DumpOverview` / `XrefDecomp` / `CallersDecomp` / `RenameByStringRef` / `RefsTo` |
| `tools/run_*.bat` | Ghidra headless 调用封装（每次调用一个专用 .bat，避免 cmd 引号陷阱） |
| `dumps/decomp_hss.txt` 等 | 第一轮专题反编译（HSS / RTT / XML / ELF / 数据通路 / 动态缓冲） |
| `dumps/defined_strings.txt` | 3,035 条字符串（含 RTTI 类名） |
| `ghidra_proj/` | Ghidra 工程（可直接打开继续分析；未进 git） |

**复现方法**：
```bash
tools\run_ghidra.bat                 # 导入 + 全量分析（约 65 s）
tools\run_all.bat                    # 全量反编译 + 全局变量导出
tools\run_script.bat DumpOverview.java
python analysis\report\build.py       # 由 analysis/report/*.html 拼装 report.html
```

---

## 14. 来源与外部佐证

- [SEGGER KB — J-Link High-Speed Sampling](https://kb.segger.com/J-Link_High-Speed_Sampling)（确认 HSS 属 J-Link SDK，J-Scope/Ozone/Embedded Studio 使用；无 API 细节）
- [SEGGER J-Link SDK](https://www.segger.cn/products/debug-probes/j-link/tools/j-link-sdk/)（HSS 函数签名与结构体的权威出处为 SDK 内 `UM08002_JLinkDLL.pdf`）
- [cortex-debug issue #1004](https://github.com/Marus/cortex-debug/issues/1004)（确认 `JLINK_HSS_*` 由 libjlinkarm 导出，GDB Server 不暴露该能力）
- [STMViewer HSS 实现提交](https://github.com/klonyyy/STMViewer/commit/c190a25a69a6c1849387093d75ff91d9b2c60775)（开源 HSS 集成实践）
- 本地 `Doc/Manuals/UM08001_JLink.pdf`（J-Link Getting Started，第 27 页为 J-Scope 简介）

---

*报告基于静态逆向。除 §11 的既有字符串外，未运行 J-Scope、未连接任何调试探针。*
