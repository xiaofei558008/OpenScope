# OpenScope Epic 12 — ST-Link 支持（request.md 需求 13）

> BMAD 规划工件。覆盖 request.md 需求 13（增加 ST-Link 支持）。
> 关联架构决策：AD-12（ST-Link 动态绑定 CubeProgrammer_API.dll）、AD-13（多驱动可选）。
> 技术依据：`_bmad-output/planning-artifacts/research/technical-stlink-cubeprogrammer-research-2026-08-22.md`。
> 目标基线：checkpoint-23（v1.9.0）。STM32G431RB Nucleo 已接入本机（`USB\VID_0483&PID_374E`）。

## 需求来源（request.md 需求 13）

- ST-Link 驱动安装目录：`C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\api\lib\CubeProgrammer_API.dll`。
- 参考：[HSS_DataVisualizer](https://github.com/DigitalAllianceStudio/HSS_DataVisualizer)（支持 ST-Link 与 J-Link）。
- 目标板 STM32G431RB Nucleo 已接入，可随时实机验证。
- 开发完成后：ST-Link 全量测试 → 修复 bug → 回归测试 + 单元测试全部通过后结束任务。

## 关键实现决策（源自技术研究）

1. **新模块 `module/stlink`**（`OS_CAP_DRIVER`, name=`stlink`），镜像 `module/jlink` 结构，`LoadLibrary`+`GetProcAddress` 动态绑定 CubeProgrammer_API.dll（不链接 `.lib`）。
2. **依赖加载**：`SetDllDirectoryW` 指向依赖 DLL 目录；优先 `dll\stlink\`（随包自含），回退 ST 安装目录 `bin\`。
3. **readMemory 双重指针**：`readMemory(addr, unsigned char** data, size)` 由 DLL 分配缓冲，模块 `mod_read` 适配为调用方缓冲（`OS_MemReq::data`）并释放临时缓冲。
4. **连接模式**：`HOTPLUG_MODE`（不 halt/不复位目标，非侵入）；无需芯片型号（IDCODE 自识别）。
5. **速度档位**：ST-Link SWD/JTAG 频率为枚举档位（`freq.swdFreq[]/jtagFreq[]`），UI 需驱动感知联动。
6. **宿主多驱动**：`g_app.driver` 保持"当前选中驱动"，`datasrv.c`/写值零改动（AD-13）。

---

## Epic 12：ST-Link 支持（checkpoint-23 目标，v1.9.0）

### Story 12.1 — stlink 驱动模块骨架 + CubeProgrammer_API.dll 动态绑定

- **AC**：`module/stlink` 编译产出 `stlink.dll`（`OS_CAP_DRIVER`、name=`stlink`、导出 `os_module_get`、api_version=3）；`mod_init` 用 `SetDllDirectoryW` + `LoadLibraryA` 绑定 CubeProgrammer_API.dll（优先 `dll\stlink\`，回退 `C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\`），随后 `setDisplayCallbacks`（`logMessage` 宽字符 → UTF-8 → `OS_Framework::log`）+ `setVerbosityLevel(ONE)`；绑定失败仅告警不崩溃（缺 DLL 时模块仍可加载，连接时给明确错误）。不链接 `CubeProgrammer_API.lib`。
- **文件**：`module/stlink/stlink.h`、`module/stlink/stlink.c`、`module/stlink/stlink.vcxproj`（镜像 `module/jlink`）。
- **测试**：`module/stlink/tests/stlink_smoke.c`（bind/unbind；无 DLL 环境报错路径不崩）。

### Story 12.2 — ST-Link 扫描 / 连接 / 断开 / 信息

- **AC**：
  - `OS_CMD_SCAN` → `getStLinkList(&list, 0)`，回填 `OS_DeviceInfo`（serial=SN、name=board 或 firmwareVersion），日志"ST-Link 扫描: 发现 N 个设备"。
  - `OS_CMD_CONNECT` → 按 `OS_ConnectCfg` 构造 `debugConnectParameters`（iface→dbgPort=SWD/JTAG、serial→SN、connectionMode=HOTPLUG_MODE、frequency 从 `freq.swdFreq[]/jtagFreq[]` 就近选档）→ `connectStLink`；失败给明确错误并 `disconnect()` 清理。
  - `OS_CMD_DISCONNECT` → `disconnect()` + `deleteInterfaceList()`。
  - `OS_CMD_IS_CONNECTED` → `checkDeviceConnection()`。
  - `OS_CMD_GET_INFO` → `getDeviceGeneralInf()`（cpu/name/deviceId → `OS_DriverInfo`）。
  - `OS_CMD_RESET/GO` → `reset()`/`execute()`；`OS_CMD_HALT` 记"不支持"。
- **文件**：`module/stlink/stlink.c`（`mod_scan/mod_connect/mod_disconnect/mod_get_info`）。
- **测试**：`module/stlink/tests/probe_devinfo.py`（Nucleo-G431RB 实机 scan→connect→get_info→disconnect）。

### Story 12.3 — ST-Link 内存读写适配 + 掉线自愈 + 依赖分发

- **AC**：
  - `OS_CMD_READ_MEM` → `readMemory(addr, &tmp, size)` → `memcpy` 到 `req->data` → 释放 tmp（实测确认释放方式，防高速采集泄漏）；成功返回 `size`，失败返回负 `OS_ERR_*`。
  - `OS_CMD_WRITE_MEM` → `writeMemory(addr, data, size)`，成功返回 `OS_ERR_OK`。
  - 读写用 `CRITICAL_SECTION` 互斥（AD-11）；掉线（`checkDeviceConnection()==0`）自动重连节流 + 时间基停摆（复用 `datasrv.c` 的 `OS_POLL_STALL_MS`，对齐 jlink `mod_read`）。
  - `dll\stlink\` 收录 `CubeProgrammer_API.dll` + 依赖 DLL（Qt5Core/Qt5Xml/Qt5SerialPort/FileManager/STLinkUSBDriver/libeay32/libstdc++-6/libwinpthread-1/mfc120/msvcp120/msvcr120/psa_sdm/stlibp11_SAM/xerces-c_3_1/zlib1）；缺失时回退 ST 安装目录 `bin\`。
- **文件**：`module/stlink/stlink.c`（`mod_read/mod_write` + 重连节流）、`packaging/`（分发清单）。
- **测试**：`stlink_smoke.c` 读/写一致性（写入回读一致、非零）、块读、掉线重连回归。

### Story 12.4 — 宿主多驱动选择（UI 仿真器下拉 + 设备/速度联动）

- **AC**：
  - `app.h` 增 `drivers[OS_MAX_MODULES]` + `driver_count`；`module_mgr.c` 收集全部 `OS_CAP_DRIVER`，`g_app.driver` 默认 jlink（回退首个驱动）。
  - `mainwin.c` 工具栏新增"仿真器"下拉（J-Link / ST-Link，按 `drivers[]` 名枚举）；切换时：断开旧连接 → 重指 `g_app.driver` → `IDC_CFG_EMU` 设备列表重扫 → 速度下拉按新驱动刷新（ST-Link 用 `freq.swdFreq[]/jtagFreq[]` 档位，J-Link 保持预置档位+手输）。
  - 连接/采集/写值仍走 `g_app.driver` 命令通道，`datasrv.c` 与 `os_ds_write_leaf` **零改动**。
- **文件**：`app.h`、`code/src/module_mgr.c`、`code/src/mainwin.c`（新增 `IDC_CFG_DRIVER` + 联动刷新）。
- **测试**：`tests/ui_driver_switch_drive.ps1`（切换仿真器 → 设备列表刷新 → 连接日志前缀 ST-Link/J-Link 正确）。

### Story 12.5 — STM32G431RB 全量硬件测试 + 回归 + 打包发布

- **AC**：
  - Nucleo-G431RB 实机全量：SWD 连接、多速度档位采集（变量非零、时间戳正确）、批量块读（多变量合并一次读）、写值标定（Enter 写入 + 回读）、断开/重连、掉线自愈。
  - 全量回归 dev+installed：J-Link 路径不受影响（原 bug9_smoke/speed_smoke/ui_* 全 ALL PASS）。
  - 单元测试 `stlink_smoke.c` + `target_smoke.c` 全 PASS。
  - 版本 v1.9.0 打包安装 → checkpoint-23 提交 + tag `v1.9.0` + 推送 gitee_origin + github_origin → 末尾 Windows 语音"任务执行完毕"（`tools/notify_done.ps1`）。
- **文件**：`module/stlink/tests/*`、`tests/ui_stlink_drive.ps1`、`version.h/version.rc`、`packaging/`。
- **测试**：dev + installed 双跑，全量回归套件。

---

## Sprint 计划

- **Sprint-13**：Story 12.1 → 12.2 → 12.3 → 12.4 → 12.5（骨架绑定 → 连接链路 → 读写/自愈/分发 → 宿主多驱动 → 全量硬件测试+回归+打包）→ checkpoint-23 提交 + tag v1.9.0 + 推送双远端 → 末尾语音"任务执行完毕"。

## 验收风险

- **readMemory 缓冲所有权**：官方示例未显式 free，模块须实测确认释放方式（`free()` 或 DLL 内部复用），否则高速采集每周期泄漏 → 12.3 重点。
- **依赖 DLL 分发**：CubeProgrammer_API.dll 依赖 ~18 个 DLL，若未随包拷入 `dll\stlink\`，则依赖用户已装 STM32CubeProgrammer 且加载路径指向 `bin\`；安装包需验证自含性（干净机器）。
- **ST-Link 频率为枚举档位**：与 J-Link 任意 kHz 语义不同；速度下拉需驱动感知联动，避免用户手输非法频率导致 `connectStLink` 失败。
- **全局单会话**：CubeProgrammer_API 无句柄、全局单会话，模块须单例 + 互斥（同 jlink `g_ctx`）；高速掉线后是否需"整库重载"（对应 J-Link Bug16 `jlink_reload`）需实机验证后决定是否引入 `stlink_reload`。
- **HOTPLUG 前提**：默认 HOTPLUG 不 halt 目标；若目标处于复位/睡眠需切 NORMAL/UNDER_RESET，UI 可在高级项暴露连接模式（延后，默认 HOTPLUG）。
- **连接日志前缀**：J-Link 与 ST-Link 日志需可区分（`ST-Link 连接:` vs `J-Link 连接:`），供回归断言与用户排障。
