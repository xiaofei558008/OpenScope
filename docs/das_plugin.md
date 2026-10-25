# das.dll — Infineon DAS / miniWiggler 驱动插件

独立驱动模块（与 jlink.dll / stlink.dll 同级），源码 `module/das/`，输出 `dll/das.dll`。
主程序只通过 `code/src/module_api.h`（API v5，`OS_CAP_DRIVER`）与之交互：
`os_module_get()` → `OS_Module{init, deinit, command}`；无窗口、无 on_samples。

## 后端
- Infineon DAS（UDAS/TAS 服务器）+ MCD API（SPRINT MCD 1.1，DAS 实现 1.14.12）。
- `mcdxdas.dll` **运行时动态加载**（`mcdload.h`：`LoadLibraryExA(..., LOAD_WITH_ALTERED_SEARCH_PATH)`，
  不链接导入库）。查找顺序：环境变量 `OPENSCOPE_DAS_DIR` → `C:\Program Files\DAS64\clients` →
  `C:\Program Files (x86)\DAS64\clients`。首次 SCAN/CONNECT 时才绑定；未安装 DAS 时模块照常加载，SCAN 返回 NO_DEVICE。
- 绑定的导出：`mcd_*_f`（initialize/exit/qry_servers/open_server/close_server/qry_systems/qry_devices/
  qry_cores/open_core/close_core/qry_error_info/execute_txlist/run/stop/step/qry_state/qry_rst_classes/rst/…）。
- 头文件：`module/das/mcd/mcd_api.h`、`mcd_types.h`（Infineon DAS64 安装目录原样复制，SPRINT BSD 类许可，未修改）。
- 连接字符串：`open_server(system_key="", config="McdHostName=\"localhost\"\nMcdServerName=\"<srv>\"\n")`，
  `<srv>` = `OS_ConnectCfg.serial`（SCAN 返回的服务器名，通常 `UDAS`），打开失败回退 `UDAS`。
  注意：system_key 传 NULL 会让 mcdxdas.dll 访问冲突，必须传 `""`。
- 连接链：qry_systems(1) → qry_devices(取 [0]) → qry_cores → open_core(core N)。
  TC375 Lite Kit 实测：server `UDAS`，acc_hw `DAS JDS AURIX LITE KIT V2.0 (TC375) LK8PIM1G`，
  device `TC37x`(0x10207083)，3 核 `Core 0: CPU0 TriCore 1.6P` …，open_core **不停核**。

## 命令语义（mod_command）
| OS_CMD | 行为 |
|---|---|
| SCAN | qry_servers(running) → items[i]: serial=服务器名, name="DAS <srv>: <acc_hw>"；无则 NO_DEVICE |
| CONNECT | in=OS_ConnectCfg：serial=服务器名（空→UDAS），device 含 `coreN`/`cpuN`/`tcN`/纯数字 选核（默认 0）；总是 Attach（不复位不停核） |
| DISCONNECT / IS_CONNECTED | 关闭 core+server / out int |
| READ_MEM | OS_MemReq；返回读到字节数。拆成 ≤1024B 事务、每 txlist ≤64 个；≥0xF0000000（SFR）用 access_width=4，非对齐时扩为 4 字节对齐窗口再截取；失败节流 1s 自动重连一次 |
| WRITE_MEM | 拒绝与 0x80000000–0x8FFFFFFF、0xA0000000–0xAFFFFFFF 相交的区间（PFLASH/DFLASH/**UCB**）→ INVALID_ARG；SFR 写须 4 字节对齐 |
| HALT / GO | mcd_stop / mcd_run |
| RESET / RESET_RUN | mcd_rst(复位类 0 优先, halt=1/0)；RESET_RUN 后若未运行再 mcd_run |
| GET_CPU_STATE | qry_state → RUNNING/STOP/—；停核时读 CSFR PC（0xF881FE08 + core*0x20000） |
| GET_INFO / AI_DESCRIBE | 驱动信息 / JSON 描述 |
| FLASH_DOWNLOAD / FLASH_ERASE | **不支持**（返回 FAIL + 日志），烧录用 MemTool 或 TASKING dbgtc，避免误写 UCB |

返回码：`OS_ERR_*`（module_api.h）。所有 MCD 调用在一个 CRITICAL_SECTION 内串行化。

## 纯逻辑函数（建议单元测试目标，`das_util.c/h`，无 Windows/MCD 依赖）
- `das_range_is_protected(addr,size)`：边界 0x7FFFFFFC+4、0x8FFFFFFF、0x90000000、0x9FFFFFFF、0xA0000000、0xAF400000(UCB)、0xAFFFFFFF、0xB0000000；size=0 视为 1 字节。
- `das_addr_is_sfr(addr)`：≥0xF0000000。
- `das_split_chunks(addr,size,max_bytes,out_addr,out_len,max_chunks)`：整除/余数/size=0/超 max_chunks 返回 -1/max_bytes=0 返回 -1。
- `das_align4(addr,size,&a,&s)`：非对齐首尾扩展。
- `das_state_text(state,&halted)`：1→RUNNING/0，2,3→STOP/1，其它→"—"/-1。
- `das_parse_core_index(device)`："core1"/"CPU2"/"tc0"/"1"→N；"Cortex-M4"/"TC37x"/NULL/""→0；N≥8→0。

## 插件级测试目标（mock mcdxdas.dll 时）
- `mod_connect_locked`：open_server 失败回退 UDAS、qry_devices 为 0 时 NO_DEVICE 且资源释放。
- `das_xfer_locked`：>64KB 读拆多 txlist；num_tx_ok<num_tx 判失败；SFR access_width=4。
- `mod_write`：保护区拒绝且**不调用** execute_txlist。
- `mod_read` 失败 → 节流重连一次。
- `das_run_ctl`：rst 类向量选择（classes&1 优先，否则最低位）。

## ELF（code/src/elf.c）相关修复
TASKING TriCore 输出 DWARF v3，`DW_AT_location` / `DW_AT_data_member_location` 用 `DW_FORM_block`；
原实现把 `DW_FORM_block` 当成只有 ULEB 长度而不跳过数据，导致 main.c 所在 CU 解析失败、所有变量无类型。
现 block/block1/block2/block4 均按 exprloc 保存（kind 8）并正确跳过；`os_elf_arch_name` 增加 EM 44 = "TriCore"。
TASKING 每个变量单独一个 CU（DW_TAG_imported_unit + DW_FORM_ref_addr 引用主 CU 的类型），已有全局 DIE 索引可解析。
