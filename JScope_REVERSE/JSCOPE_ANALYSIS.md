# SEGGER J-Scope 逆向分析报告

**分析对象**：`C:\Program Files\SEGGER\JLink_V966\JScope.exe`
**分析日期**：2026-10-11
**分析工具**：Ghidra 12.1.4 headless（自建 JDK 21 工具链）+ pefile/capstone 辅助
**参考方法论**：REA（morluto/rea）——区分「观测/推导/推断/未知」，保留证据出处

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

RTT 读缓冲：`readBufSize = min(0x2000, channelBufSize)`，分配标签 `"RTT read buffer allocation"`。

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
| 1 | float32 | ✔ | |
| 2 | float64 | ✔ | |
| 3 | ？（16 字节，疑似 long double/float128） | ✘ | `FUN_1400504d0` 判为不支持 |
| 4 | ？（疑似 half/float16） | ✘ | 同上 |
| 5 | bool | ✔ | |
| 6–9 | uint8/16/32/**64** | 6,7,8 ✔ / **9 ✘** | uint64 树中禁用 |
| 10–13 | int8/16/32/**64** | 10,11,12 ✔ / **13 ✘** | int64 树中禁用 |

不支持时 UI 行为 [观测]：`QTreeWidgetItem::setDisabled(true)` + 第 4 列文本 `"Symbol type is not supported"`。

> **[推断]** TypeID 3/4 的确切语义未在本轮确证；结合 demangler 类型表中出现的 `half`、`__float128`，推测为 128 位浮点与 16 位浮点。

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
| `+0x59C` | `TargetInterface`（0=JTAG, 7=?, 1=SWD）[推断] |
| `+0x5A0` | `InterfaceSpeed` (kHz) |
| `+0x5A4` | 模式：0=HSS, 1=RTT |
| `+0x5A5` | `IsRTTWithTimeStamp` |
| `+0x5A6` | RTT 子模式（Data/Plus） |
| `+0x5A8` | `SamplingPeriod` (µs) |
| `+0x5B0`/`+0x5B4` | `JTAG_IRPre` / JTAG 配置值 |
| `+0x5B8` | 停止时是否跳过 HSS_Stop |

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

### 10.3 数据导出

支持 `.raw`（`Binary file (*.raw)`，另有 `Import Raw` 反向导入）、`.elf`、`DataFile`。
文件大小上限检查：`File too large - maximum file size (%d bytes) exceeded!`

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

### 12.3 已知坑（逆向中发现）

- `JLINK_HSS_Start` 第 4 参数为 `1`（flags）[观测]，语义未知 [未知]。
- `caps.field_at_4` 的单位需实测确认（§4 末）。
- TypeID 3/4 语义未确证（§8）。
- `-USB` / `-IP` 选项**必须**与 `-openprj` 同用，否则报错 —— 因连接参数来自工程文件。
- RTT 模式**强制要求**指定目标器件：`In RTT mode specifying the target device is mandatory.`

---

## 13. 本次分析产出物

| 路径 | 内容 |
|---|---|
| `JSCOPE_ANALYSIS.md` | 本报告 |
| `tools/ghidra_scripts/*.java` | 4 个 Ghidra 脚本：概览导出 / 字符串交叉引用反编译 / 调用者反编译 / 按字符串自动重命名 |
| `tools/run_*.bat` | Ghidra headless 调用封装 |
| `dumps/decomp_hss.txt` | HSS 全路径反编译 |
| `dumps/decomp_rtt.txt` | RTT 全路径反编译（含 VarFormat 解析器） |
| `dumps/datapath.txt` | 样本存储/时间戳逻辑 |
| `dumps/xml_fmt.txt` | 工程文件读写 |
| `dumps/elf_syms.txt` | ELF 符号树与类型判定 |
| `dumps/functions.txt` | 2712 个函数清单 |
| `dumps/defined_strings.txt` | 3035 条字符串（含地址） |
| `ghidra_proj/` | Ghidra 工程（可直接打开继续分析） |

**复现方法**：
```bash
tools\run_ghidra.bat                 # 导入 + 全量分析（约 65 s）
tools\run_script.bat DumpOverview.java
tools\run_hss.bat                    # 或 run_rtt.bat / run_b.bat ...
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
