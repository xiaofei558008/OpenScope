---
stepsCompleted: [init, technical-overview, integration-patterns, implementation-research, research-synthesis]
inputDocuments: [request.md, readme.md, ARCHITECTURE-SPINE.md, module/jlink/jlink.c]
workflowType: 'research'
lastStep: 6
research_type: 'technical'
research_topic: 'OpenScope 新增 ST-Link 支持（CubeProgrammer_API.dll 动态绑定）'
research_goals: '为 request.md 需求 13 的 ST-Link 支持提供可落地的技术依据：API 契约、与 J-Link 模块的差异、依赖加载策略、实机环境'
user_name: 'OpenScope'
date: '2026-08-22'
web_research_enabled: true
source_verification: true
---

# 技术研究报告：OpenScope 新增 ST-Link 支持（CubeProgrammer_API.dll）

**Date:** 2026-08-22
**Research Type:** technical
**Scope:** 为 `request.md` 需求 13（增加 ST-Link 支持）提供规划输入，确定 `module/stlink` 驱动模块的 API 绑定方式、与既有 `module/jlink` 的差异、以及部署/性能风险。

---

## 1. 研究结论摘要（TL;DR）

1. **绑定对象已确认**：`C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\api\lib\CubeProgrammer_API.dll`（2,289,152 字节）已安装；权威头文件 `api\include\CubeProgrammer_API.h`（1,089 行）与官方示例 `api\src\Example1.cpp`（ST-Link 全流程）均已就位。
2. **ST-Link 实机已接入**：`ST-Link Debug`（`USB\VID_0483&PID_374E`，Nucleo-G431RB 板载 ST-LINK/V2-1）+ VCP `COM5`。目标芯片 STM32G431RB 为 **Cortex-M4F**，与工程默认 `Cortex-M4` 一致。
3. **核心差异（关键实现点）**：`readMemory(address, unsigned char** data, size)` 为**双重指针**——缓冲由 DLL 内部分配回填，调用方需释放；而 J-Link 的 `ReadMem(addr,size,uint8_t*)` 使用调用方缓冲。ST-Link 的 SWD/JTAG 频率是**枚举档位**（`freq.swdFreq[]/jtagFreq[]`），不是 J-Link 的任意 kHz。
4. **依赖加载是最大部署风险**：`CubeProgrammer_API.dll` 依赖约 18 个 DLL（Qt5Core/Qt5Xml/Qt5SerialPort/FileManager/STLinkUSBDriver/OpenSSL/xerces/mfc 等），全部位于 `bin\`，**不在** `api\lib\`。动态绑定前必须用 `SetDllDirectoryW` 把依赖目录加入 DLL 搜索路径。
5. **架构契合**：新 `stlink.dll` 完全镜像 `jlink.dll` 的 `OS_CAP_DRIVER` 命令契约（`OS_CMD_SCAN/CONNECT/DISCONNECT/READ_MEM/WRITE_MEM/GET_INFO/...`），宿主只需把"单驱动硬编码"改为"多驱动可选"（本报告 §4）。

---

## 2. 环境事实（本机实测，2026-08-22）

| 项目 | 值 |
| --- | --- |
| CubeProgrammer_API.dll | `C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\api\lib\CubeProgrammer_API.dll`（2,289,152 B）；`bin\` 下有一份同尺寸副本 |
| 头文件 | `api\include\CubeProgrammer_API.h`、`DeviceDataStructure.h`、`DisplayManager.h` |
| 导入库 | `api\lib\x64\CubeProgrammer_API.lib`、`api\lib\x86\CubeProgrammer_API.lib` |
| 官方示例 | `api\src\Example1.cpp`（ST-Link）、`Example2/3.cpp`、`main.cpp`（setDisplayCallbacks） |
| ST-Link 硬件 | `ST-Link Debug` `USB\VID_0483&PID_374E`；`STMicroelectronics STLink Virtual COM Port (COM5)` |
| 目标芯片 | STM32G431RB（Nucleo），Cortex-M4F |
| 依赖 DLL（`bin\`） | FileManager.dll、STLinkUSBDriver.dll、Qt5Core/Qt5Xml/Qt5SerialPort.dll、libeay32.dll、libstdc++-6.dll、libwinpthread-1.dll、mfc120/msvcp120/msvcr120.dll、psa_sdm.dll、stlibp11_SAM.dll、xerces-c_3_1.dll、zlib1.dll 等 |

---

## 3. CubeProgrammer_API 契约（经头文件 + 示例核实）

### 3.1 初始化

- `void setDisplayCallbacks(displayCallBacks c)` —— **必须先于一切调用**。`displayCallBacks` 含 `logMessage(int msgType, const wchar_t* str)`（宽字符）等；配合 `void setVerbosityLevel(int level)` 抑制/开放内部打印。ST-Link 模块应把 `logMessage` 桥接到 `OS_Framework::log`（UTF-8 转码），并把 verbosity 设为 `CUBEPROGRAMMER_VER_LEVEL_ONE`（仅警告/错误/成功）。

### 3.2 枚举 / 连接 / 断开

| 函数 | 签名 | 说明 |
| --- | --- | --- |
| `getStLinkList` | `int getStLinkList(debugConnectParameters** stLinkList, int shared)` | 返回探针数，回填结构数组；用后 `deleteInterfaceList()` |
| `connectStLink` | `int connectStLink(debugConnectParameters)` | 按值传参；0=成功。无需芯片型号（靠 DBGMCU IDCODE 自识别） |
| `checkDeviceConnection` | `int checkDeviceConnection()` | 1=已连接（对应 J-Link `IsConnected`） |
| `disconnect` | `void disconnect()` | 断开并卸载 Flash Loader |
| `deleteInterfaceList` | `void deleteInterfaceList()` | 释放 `getStLinkList` 产生的列表 |

`debugConnectParameters` 关键字段（选值）：

- `dbgPort`：`SWD=1` / `JTAG=0`（映射 `OS_IF_SWD/JTAG`）。
- `index` / `serialNumber[33]` / `firmwareVersion[20]` / `targetVoltage[5]` / `board[100]`：来自 `getStLinkList` 回填，连接时按 index 或 SN 选定。
- `connectionMode`：`HOTPLUG_MODE`（**不 halt/不复位目标，非侵入**，适配变量采集）；`NORMAL_MODE` / `UNDER_RESET_MODE` 用于需要复位/重启的目标。
- `resetMode`：`SOFTWARE_RESET` / `HARDWARE_RESET` / `CORE_RESET`。
- `frequency`：从 `freq.swdFreq[]`（`swdFreqNumber`）或 `freq.jtagFreq[]` 中选一档；**非任意值**。
- `shared`：0（不使用 ST-LINK Server 共享）。

### 3.3 内存读写

| 函数 | 签名 | 语义 |
| --- | --- | --- |
| `readMemory` | `int readMemory(unsigned int address, unsigned char** data, unsigned int size)` | 0=成功；**`*data` 由 DLL 分配并回填**，调用方负责释放 |
| `writeMemory` | `int writeMemory(unsigned int address, char* data, unsigned int size)` | 0=成功；调用方提供缓冲 |

> ⚠️ **所有权关键**：`readMemory` 的 `unsigned char**` 与 J-Link `ReadMem(addr,size,uint8_t*)` 语义相反。官方 `Example1.cpp` 用法 `unsigned char* dataStruct = 0; readMemory(addr, &dataStruct, size);` 后**未显式 free**（示例存在泄漏/由 DLL 内部管理二义）。`module/stlink` 必须在 `mod_read` 内把 DLL 回填缓冲 `memcpy` 到 `OS_MemReq::data` 后**实测确认释放方式**（`free()` 或 DLL 内部复用），否则高速采集每周期泄漏。

### 3.4 控制 / 信息

- `reset(debugResetMode)` → `OS_CMD_RESET`。
- `execute(unsigned int address)` → `OS_CMD_GO`（运行到地址）。
- `getDeviceGeneralInf()` → `generalInf*`（`cpu[20]`、`name[100]`、`deviceId`、`flashSize`）→ `OS_CMD_GET_INFO`。
- **无 `getDllVersion()` 导出**：`OS_DriverInfo::dll_version` 以模块自身版本 + `firmwareVersion` 填充。

---

## 4. J-Link vs ST-Link 差异对照（模块适配依据）

| 关注点 | J-Link（JLink_x64.dll） | ST-Link（CubeProgrammer_API.dll） |
| --- | --- | --- |
| 扫描 | `EMU_GetNumDevices` / `EMU_GetList` | `getStLinkList(&list, 0)` |
| 连接 | `Open` + `TIF_Select` + `ExecCommand("Device=…")` + `SetSpeed` + `Connect`（多步） | `connectStLink(params)`（单次调用） |
| 读内存 | `ReadMem(addr,size,uint8_t*)`（调用方缓冲） | `readMemory(addr, unsigned char**, size)`（DLL 分配） |
| 写内存 | `WriteMem(addr,size,const uint8_t*)` | `writeMemory(addr, char*, size)` |
| 连接态 | `IsConnected` | `checkDeviceConnection` |
| 速度 | `SetSpeed(khz)` 任意值 | `frequency` ∈ `freq.swdFreq[]/jtagFreq[]` 枚举档位 |
| 芯片型号 | 核心名即可（已实现 F17） | **完全不需要**（IDCODE 自识别） |
| 会话模型 | 全局单会话（需 Bug16 整库重载防脏会话） | 全局单会话（无句柄，需同源防护，重载必要性待实测） |
| 依赖 | 单文件 JLink_x64.dll | 主 DLL + ~18 依赖 DLL（Qt/OpenSSL/xerces/mfc，`bin\`） |

---

## 5. 实现研究：绑定与部署策略

### 5.1 动态绑定（镜像 AD-7，新增 AD-12）

`stlink.dll` 不链接 `CubeProgrammer_API.lib`，运行时 `LoadLibraryA` + `GetProcAddress` 绑定下列符号（`extern "C"` 导出，无名字修饰）：`setDisplayCallbacks`、`setVerbosityLevel`、`getStLinkList`、`deleteInterfaceList`、`connectStLink`、`checkDeviceConnection`、`disconnect`、`getDeviceGeneralInf`、`readMemory`、`writeMemory`、`reset`、`execute`。

### 5.2 依赖加载（关键）

`api\lib\` 只含主 DLL 与导入库，依赖 DLL 全在 `bin\`。绑定顺序：

1. 定位候选：优先 `dll\stlink\CubeProgrammer_API.dll`（随安装包分发、自含），否则回退 `C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\CubeProgrammer_API.dll`。
2. `SetDllDirectoryW(<候选同目录的 bin 或 dll\stlink>)` 后再 `LoadLibraryA`，使 Qt/OpenSSL/xerces 依赖可解析（`AddDllDirectory` + `LOAD_LIBRARY_SEARCH_DEFAULT_DIRS` 亦可，`SetDllDirectory` 最简且向下兼容）。
3. 分发二选一（规划建议 **a 优先、b 兜底**）：
   - **(a) 随包自含**：安装包把 `CubeProgrammer_API.dll` + 依赖 DLL 拷入 `dll\stlink\`（与 `dll/JLink_x64.dll` 的"拷贝到 dll 文件夹"约定一致）。
   - **(b) 依赖已装工具**：仅当 (a) 缺失时回退 ST 安装目录 + `SetDllDirectoryW(bin)`。

### 5.3 模块命令映射

| `OS_CMD_*` | ST-Link 实现 |
| --- | --- |
| `SCAN` | `getStLinkList` → 填 `OS_DeviceInfo`（serial=firmwareVersion+SN, name=board） |
| `CONNECT` | 按 `OS_ConnectCfg`（iface→dbgPort、serial→SN、speed_khz→frequency 档位、device 忽略）→ `connectStLink`（HOTPLUG_MODE 默认） |
| `DISCONNECT` | `disconnect()` + `deleteInterfaceList()` |
| `IS_CONNECTED` | `checkDeviceConnection()` |
| `READ_MEM` | `readMemory` 双重指针 → memcpy 到 `req->data` → 释放临时缓冲 |
| `WRITE_MEM` | `writeMemory` |
| `GET_INFO` | `getDeviceGeneralInf()`（cpu/name/deviceId） |
| `HALT/GO/RESET` | `reset()` / `execute()`（HALT 无直接等价，记不支持） |

### 5.4 速度档位联动

ST-Link 的 SWD/JTAG 频率是枚举档位，主界面 `IDC_CFG_SPEED` 需在选中 ST-Link 时按 `getStLinkList` 返回的 `freq.swdFreq[]/jtagFreq[]` 动态填充（J-Link 保持"预置档位 + 手输 kHz"）。此差异落在 Story 12.4。

---

## 6. 宿主多驱动选择研究

现状（`module_mgr.c:84-107`）：`g_app.driver` 单槽，`name_has_jlink()` 优先 J-Link，否则取首个 `OS_CAP_DRIVER`。`datasrv.c` / `os_ds_write_leaf` 只引用 `g_app.driver`（8 处）。

改造最小侵入方案（新增 AD-13）：

- `app.h` 增 `OS_Module* drivers[OS_MAX_MODULES]` + `driver_count` + `active_driver` 索引；`g_app.driver` 语义改为"指向当前选中驱动"。
- `module_mgr.c` 把全部 `OS_CAP_DRIVER` 模块记入 `drivers[]`，默认选中 jlink（回退首个驱动）。
- `mainwin.c` 新增"仿真器"下拉（`IDC_CFG_DRIVER`），切换时：断开旧连接 → `g_app.driver` 指向新驱动 → 重扫 `IDC_CFG_EMU` 设备列表 → 按驱动刷新速度档位。
- **datasrv.c 零改动**（仍经 `g_app.driver` 命令通道，AD-2 不变）。

---

## 7. 参考与引用

- 官方头文件与示例（本机）：`CubeProgrammer_API.h`、`Example1.cpp`（ST-Link 全流程）。
- 用户指定参考（支持 ST-Link 与 J-Link 的可视化项目）：[HSS_DataVisualizer](https://github.com/DigitalAllianceStudio/HSS_DataVisualizer)。
- ST 社区（display callbacks/verbosity 与连接失败排查）：[API: STLink detected but cannot be connected](https://community.st.com/stm32cubeprogrammer-mcus-30/api-stlink-detected-but-cannot-be-connected-to-2645)。
- `getStLinkList` 结构布局/跨语言互操作注意（C# interop）：[Stack Overflow](https://stackoverflow.com/questions/74776893/c-interop-with-complex-c-library-stmcubeprogrammer)。
- Rust bindings 印证函数签名：[stm32cubeprogrammer-sys](https://docs.rs/stm32cubeprogrammer-sys/latest/stm32cubeprogrammer_sys/struct.CubeProgrammer_API.html)。

---

## 8. 研究综合：对规划的直接结论

1. 新增 `module/stlink`（`OS_CAP_DRIVER`, name=`stlink`），镜像 `module/jlink` 结构，动态绑定 CubeProgrammer_API.dll（AD-12）。
2. 宿主由单驱动改为多驱动可选，UI 增"仿真器"下拉（AD-13），`g_app.driver` 保持为"当前驱动"，datasrv/写值零改动。
3. 三个必须实测的风险点进入验收清单：① `readMemory` 缓冲释放方式（防泄漏）；② 依赖 DLL 分发/加载路径（随包 vs 回退安装目录）；③ ST-Link 高速掉线是否需"整库重载"（对应 J-Link Bug16 的 `jlink_reload`）。
4. 速度档位语义差异（枚举档位 vs 任意 kHz）需在 UI 层做驱动感知处理。
