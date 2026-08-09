# OpenScope 开发进度

本文件记录项目开发进度与检查点（checkpoint）。每次阶段性完成后更新并提交 git，防止终端崩溃/卡死导致进度丢失。

## 检查点

- **checkpoint-0（2026-08-08）**：基线。已有 OpenScope 框架源码（ELF/DWARF 解析、主窗口、波形/数值窗口、数据采集/记录/回放、模块管理器）+ build 脚本 + JLink_x64.dll；安装 BMAD Method v6.10.0（`_bmad` + `.agents/skills`）。
- **checkpoint-1（2026-08-08）**：BMAD 规划完成。产出 `_bmad-output/project-context.md`（22 条规则）、架构 spine（AD-1~AD-11，lint 0 发现，双评审）、`epics.md`（4 Epic/13 Story）、`readiness-report.md`（READY）、`sprint-status.yaml`。
- **checkpoint-2（2026-08-08）**：Epic 1 完成。`build.py` 干净构建入口；修复 numwin/vartree（commctrl.h）、util（is_ptr）、module_api.h（interface 宏冲突→iface）、elf.c（DW_FORM_rnglistx + DW_Unit.unit_type）；ui.c 移入 `_agent_extra`；全量构建 0 error/0 warning；启动冒烟测试通过（主窗口标题正确）。
- **checkpoint-3（2026-08-08）**：Epic 2 完成（J-Link 驱动模块）。
  - 新建 `module/jlink`（jlink.c/h、jlink_dlg.c、tests/jlink_smoke.c、tests/probe_emu.py）；构建产出 `dll\jlink.dll`，0 error/0 warning。
  - 修复扫描崩溃：`JLINKARM_EMU_GetNumDevices` 为无参签名；`EMU_GetDeviceInfo` 旧 API 会访问违例，改用官方 `JLINKARM_EMU_GetList(host, infos, count)` 两段式枚举 + 264 字节 CONNECT_INFO 布局（pylink 源码佐证 + 本机 ctypes 实证）。
  - 冒烟测试 PASS：init=0、GET_INFO=0（dll=96600 hw=40000）、scan count=1（sn=174504925 name=J-Link PRO）、is_connected=0。
  - 修复框架模块加载：module_mgr.c `dll_dir` 误指向 `bin\Release\dll`，改为 `..\..\dll`；进程模块列表验证 OpenScope.exe 已加载 jlink.dll + JLink_x64.dll。
  - Story 2.1~2.4 全部完成；连接/读写 MCU 路径已实现，待用户环境硬件实测。
- **checkpoint-4（2026-08-08）**：Epic 3 完成（采集/记录/回放）。
  - 修复 CSV 记录格式：os_datalog_append 每行只写一个变量 → 宽表（每周期一行、watched 叶子各一列），与表头对齐。
  - 修复回放核心 bug：split_csv 就地改写 pending 行导致后续行无法产出样本；改在副本上切分。
  - 修复 WRITE_MEM 返回契约（jlink 模块成功返回 OS_ERR_OK）与位域掩码 64 位边界。
  - 清理 6 个编译警告，恢复 0 error/0 warning。
  - 新增 tests/replay_smoke.c（14 项断言 ALL PASS）+ tests/build_tests.bat，接入 build.bat 每次构建自动回归（replay_smoke + jlink_smoke 均 PASS）。
  - Story 3.1~3.3 完成；真实采集/记录依赖硬件，待用户环境实测。
- **checkpoint-5（2026-08-08）**：Epic 4 完成（scope.dll 窗口模块）。
  - 新建 module/scope（scope.c + scope.vcxproj）：窗口类型 `scope.bar`（示波器窗口），capabilities=OS_CAP_WINDOW。
  - 功能：添加变量模糊搜索对话框（fw->leaf_find）、系列管理（添加/图例选择/删除）、实时曲线绘制（双缓冲、Y 自动缩放、written 标记）、数值写回（fw->write_leaf）。
  - 全量构建 0 error/0 warning；scope_smoke 冒烟 ALL PASS；端到端验证：启动应用后发送“示波器窗口”菜单命令成功创建 OSScopeWin 子窗口。
  - Story 4.1~4.3 完成；实时曲线/写值交互待用户环境实测。
- **checkpoint-6（2026-08-08）**：J-Link 真实目标读写实测（STM32L432K8U6，SWD @4MHz）。
  - 修复模块连接序列（对照 pylink 实证）：ExecCommand 签名 2 参→3 参（缺 BufferSize）；接口选择改用 `JLINKARM_TIF_Select`；器件设置改为 `Device = <name>`（`SetDevice`/`SelectInterface` 在该 DLL 均为 Unknown command）。
  - 修复 `JLINKARM_GetFirmwareString(char*, int)` 签名（原按无参绑定导致挂死）；修复 `ReadMem` 成功返回 0（非字节数）/`WriteMem` 返回写入字节数的语义。
  - 新增 `module/jlink/tests/target_smoke.c` 硬件测试 + `tests/build_target_test.bat`，并保留 probe_connect/probe_devinfo/probe_map2/probe_emu 诊断脚本。
  - 实测结果（ALL PASS）：
    - 连接：成功，固件 "J-Link Pro V4 compiled Sep 22 2022 15:00:37"，CPUID=0x410CC601（Cortex-M4）。
    - RAM 0x20000000：0x20000000–0x20001FFF（8KB）写/读/回读校验通过；0x20002000–0x20003FFF 写入不驻留/总线错误 —— **该单元实际 RAM 为 8KB，非声称的 16KB**。
    - Flash 0x08000000：64KB 全部可读，向量表有效（SP=0x20000798、Reset=0x08002F05），已烧录固件。
- **checkpoint-7（2026-08-08）**：支持 .out 文件解析（ELF 同格式）+ 修复 ELF 符号表解析 bug。
  - 打开文件对话框本已支持 `*.elf;*.axf;*.out`，`os_elf_open` 按魔数判定；新增 `tests/elf_sample.out`（ELF32 ARM + DWARF4，含全局 int、全局结构体 cfg_t{a,b}、无调试符号变量）端到端验证。
  - 修复 elf.c 真 bug：ELF 节头 `sh_link`（指向 .strtab）误读偏移 32（实为 sh_size），ELF32 应为 24、ELF64 应为 40 —— 此前会导致符号表字符串表解析错误，DWARF 缺失时变量兜底失效。
  - 新增 `tests/elf_smoke.c`（.out 全局变量/结构体展开/符号兜底/魔数拒绝，全部 PASS）+ `tests/gen_elf_out.py`，接入 build_tests.bat 自动回归。
- **checkpoint-8（2026-08-08）**：真实 IAR 多编译单元 .out（tests/enc.out）全量解析 + 结构体原子展开。
  - 修复 4 个 DWARF 解析 bug：DWARF5 form 常量错位（0x20 实为 ref_sig8、implicit_const=0x21/loclistx=0x22/rnglistx=0x23）；ref1/2/4/8/ref_udata 未加 CU 相对基址；DW_FORM_ref_addr 属性值被丢弃；类型引用只在本单元查找。
  - 解析器重构：全部 DWARF 单元先入池，建立全局 DIE 偏移索引，跨单元类型引用（ref_addr/ref4）可解析；坏单元跳过继续。
  - 实测 enc.out：296 个 CU 全部解析，16 个全局变量全部带 DWARF 类型；结构体/联合体/数组展开为 285 个原子叶子（0 残留 struct/union、0 零尺寸），如 MT6835.AbsEnc.AngleBit、ta_enc.index_tx、modbus_rtu_slave.addr、ADC_Temper.MCU_Temper；char 数组保留为整块叶子。
  - 新增 `tests/enc_smoke.c`（真实 .out 回归，全部 PASS）+ `tests/dump_elf.c`（ELF 转储工具）；`python build.py --quiet` 0 error/0 warning，全部冒烟通过。
- **checkpoint-9（2026-08-08）**：request.md 第 8 条完成 —— .exe 安装包 + 版本号。
  - 版本化：新增 `code/src/version.rc`（1.0.0.0）并接入 vcxproj；OpenScope.exe 版本信息确认生效；模块版本升 1.0.0；About 对话框 v1.0.0；`module_mgr.c` 安装布局优先 `exe\dll\`（回退开发布局 `..\..\dll`）。
  - 打包：Inno Setup 6.7.3 静默安装到 `tools\innosetup`（gitignore，不入库）；新增 `packaging/openscope.iss` + `packaging/make_setup.py`（版本号以 version.rc 为唯一来源并校验一致）+ `packaging/make_icon.py`（assets/openscope.ico）。
  - 产物：`dist/OpenScope-Setup-1.0.0.exe`（10.3 MB，x64，安装到 Program Files\OpenScope，含 exe + dll\ 三模块 + 开始菜单/桌面快捷方式）。
  - 验证全通过：安装包版本信息 1.0.0.0；静默安装布局正确；安装后 exe 版本 1.0.0.0 且启动冒烟 OK；卸载器静默卸载成功、目录清理干净。
- **checkpoint-10（2026-08-08）**：修复 J-Link 对话框“没有发现设备/选择 STM32L432KB 闪退”，新增文件日志 + 崩溃处理器（1.0.1 修复版）。
  - 日志：此前只有内存 UI 日志，闪退即丢。新增 `openscope.log` 文件日志（exe 目录，写不进去回退 `%LOCALAPPDATA%\OpenScope`），时间戳+级别+同步刷新；`SetUnhandledExceptionFilter` 崩溃处理器记录异常码/地址/栈（模块+偏移）后弹窗终止。
  - 根因（UI 驱动 + 日志复现定位）：`os_jlink_scan_devices` 误返回 `mod_scan` 错误码 `OS_ERR_OK(0)` 而非设备数 → 对话框永远走“没有发现 JLink 设备”分支（但模块日志显示“发现 1 个设备”），列表为空、状态错乱，与该用户报告完全吻合；修复为成功时返回 `req.count`。
  - 顺带修复：速度下拉框传的是索引而非 kHz（默认 4000 实际下发 5）；模块加载后“连接”按钮未重新启用（一直禁用）；`EMU_SelectByIndex` 在旧固件返回 -1 导致偶发 `Connect rc=-257`，增加去掉显式选择自动重试一次。
  - 验证：新增 `module/jlink/tests/ui_connect_drive.ps1`（真实 UI 驱动：打开对话框→输入器件名→刷新 5 次→连接→观察进程/弹窗）。开发版与安装版均验证：连续扫描一致（发现 1 个、列表 1 项、状态正确），STM32L432KB 连接 rc=0 成功，无 FATAL、无闪退。
  - 版本升 1.0.1：version.rc/About/模块/安装包同步，重新打包 `dist/OpenScope-Setup-1.0.1.exe`，静默安装验证版本 1.0.1.0 + 连接流程通过。
- **checkpoint-11（2026-08-08）**：窗口管理改 Tab 标签页 + 坐标轴 + 树右键添加变量 + 启动缺失 ELF 只警告（1.1.0）。
  - Tab 标签页：右侧面板改为 Tab 容器（hTab，SysTabControl32），新增窗口=新增 Tab，不再分割面积；Tab 点击/方向键切换、右键菜单/×/模块窗口“关闭”按钮均可关闭；修复原关闭消息发到静态面板导致失效的 bug（`GetParent`→主窗口，模块经 `fw->post_msg`）。`OSRightPanel` 自定义类转发 Tab 的 WM_NOTIFY 到主窗口。
  - 坐标轴：波形窗口（chartwin）与示波器窗口（scope.dll）都补上 Y 轴刻度槽（左 56px）+ 底部时间轴（按 ts 自动刻度）+ 无数据提示“等待采集数据…”；scope 增加时间戳存储与图例宽字符绘制（原 DrawTextA 中文乱码）。
  - 树右键：新增“添加到波形窗口/数值窗口/示波器窗口”（优先当前激活窗口，否则已有，否则新建）；修复位域叶节点在树中 lParam=-1 导致右键置灰的 bug（现挂叶 id）；模块 API v2 新增 `win_add_var`，scope 实现。
  - 启动 ELF：支持命令行 `OpenScope.exe <elf> [--select-leaf=名]`（自动化/快速打开）；加载失败不再弹窗，只在日志窗口 Warning。
  - 测试：`tests/ui_windows_drive.ps1` 全量 UI 回归（ELF 加载→选叶→建 3 窗口→Tab 数量/切换/关闭→三类窗口添加变量→无闪退），开发版+安装版 ALL PASS；J-Link 对话框回归 PASS；构建 0 error/0 warning。
  - 测试夹具：用户删除 tests/enc.out 并新增自己的 `linix_stm32l031_v1.2.out`（18 全局变量/511 原子叶，真实 IAR），`enc_smoke.c` 改用它做真实文件回归（通用断言+新文件地址断言），enc.out 从版本库移除。
  - 版本 1.1.0：重新打包 `dist/OpenScope-Setup-1.1.0.exe`，安装版验证版本与全部 UI 功能。
- **checkpoint-12（2026-08-08）**：布局保存/恢复/导入导出（request.md 新增需求 1/2，1.2.0）。
  - 新增 `code/src/layout.c/h`：自定义 UTF-8 文本布局格式（主窗口位置/大小、tree_w/log_h、活动 Tab、每个窗口的 type/title/变量名列表，变量用 `|` 分隔）；关闭时自动保存到 `%LOCALAPPDATA%\OpenScope\layout.ini`（exe 目录写不进时回退），启动时自动恢复。
  - 菜单“文件→保存布局为…/加载布局…”导出/导入布局文件便于分享；命令行新增 `--layout-load=<文件>` / `--layout-save=<文件>` / `--no-layout`。
  - 恢复流程：先建窗口（波形/数值/scope.bar，`os_win_create_by_type`），变量按名解析；无 ELF 时挂起（pending），加载 ELF 后 `os_layout_apply_pending()` 自动补挂。
  - 模块 API v3：新增 `win_enum_var`（枚举窗口变量名，scope 实现，jlink 置空）；chartwin/numwin 增加 `os_chart_var_name/os_num_var_name`。
  - 修复 2 个自引入 bug：`LayoutData` 约 1.3MB 放主线程栈导致 0xC00000FD 栈溢出（改堆分配）；`[win]` 无 `=` 导致 parse_key 置空 key、窗口解析为 0（先按原始行比较）；命令行参数先截尾部空格再匹配（`--no-layout ` 被误当 ELF 路径）。
  - 回归：`tests/ui_layout_drive.ps1`（A 建窗口+变量→关闭自动保存；B 重启自动恢复+ELF 后补挂；C `--layout-load` 导入）开发版+安装版 ALL PASS；`ui_windows_drive.ps1` 加 `--no-layout` 隔离后 ALL PASS；构建 0 error/0 warning。
  - 版本 1.2.0：重新打包 `dist/OpenScope-Setup-1.2.0.exe`（10.3MB），安装版验证版本 1.2.0.0 + 布局全套功能。
- **checkpoint-13（2026-08-09）**：修复波形窗口空白（页面窗口移入 Tab 控件）+ Tab 重命名（1.3.0）。
  - 波形空白根因：页面窗口此前是 Tab 控件的兄弟窗口，真实桌面上被 Tab 控件覆盖/遮挡导致“全空白”。改用标准 Tab 页模式：波形/数值/示波器窗口直接作为 SysTabControl32 的子窗口，位置由 TCM_ADJUSTRECT 计算，永远绘制在 Tab 之上；用应用内 `--shot=<路径>` 钩子（WM_PRINT 抓取）验证 chart_draw/scope_render 输出正常（深色绘图区+坐标轴）。
  - 顺带支持 WM_PRINT：chartwin/scope 增加 WM_PRINT 渲染（截图/PrintWindow 可正确抓取），util.c 新增 `os_save_window_bmp`（GDI 保存 BMP，需 gdi32.lib）。
  - Tab 重命名：双击标签或右键菜单“重命名标签”→ OSDlgRename 对话框 → 更新窗口标题与标签文本（`tab_set_title`）；新增 `--rename-tab=<名>` 测试钩子。
  - 修复日志 bug：`os_log` 窄字符 vsnprintf 的 `%ls` 遇中文按 C locale 转换失败输出空行（布局已正确保存但日志为空）——重命名日志先转 UTF-8 再 `%s` 输出。
  - 回归：`tests/ui_rename_drive.ps1`（父窗口=Tab、钩子重命名、对话框 OK、布局保存）开发版+安装版 ALL PASS；`ui_windows_drive`/`ui_layout_drive`/`ui_connect_drive` 全部 ALL PASS；构建 0 error/0 warning。
  - 版本 1.3.0：重新打包 `dist/OpenScope-Setup-1.3.0.exe`（10.3MB），安装版验证版本 1.3.0.0 + 重命名功能。
- **checkpoint-14（2026-08-09）**：连接配置直接嵌入主界面，连接不再弹配置对话框（1.4.0）。
  - 主界面新增连接配置行（按钮栏下方）：MCU 型号 EDIT、仿真接口 COMBO（SWD/JTAG）、时钟速度 COMBO（0自动~5000kHz）、J-Link 设备 COMBO、刷新按钮；启动/模块加载后自动扫描填充设备列表，刷新按钮可重扫。
  - `cmd_connect` 重写：直接读取界面控件构造 `OS_ConnectCfg`（接口/速度/设备名/仿真器索引）调 `OS_CMD_CONNECT`，不再 `OS_CMD_CONFIGURE` 弹窗；成功/失败只走状态栏+日志，无任何弹窗；布局中树/右侧/日志下移一行。
  - `module/jlink/tests/ui_connect_drive.ps1` 重写为主界面直连回归：校验控件齐全、设备列表自动扫描+刷新、输入 STM32L432KB 直连成功、无配置对话框、断开成功；支持 `-ExePath` 跑安装版。
  - 回归：连接测试开发版+安装版 ALL PASS；ui_windows_drive/ui_layout_drive/ui_rename_drive 全部 ALL PASS；构建 0 error/0 warning。
  - 版本 1.4.0：重新打包 `dist/OpenScope-Setup-1.4.0.exe`（10.3MB），安装版验证版本 1.4.0.0 + 主界面直连功能。
- **checkpoint-15（2026-08-09）**：删除 MCU 型号输入框，全部按键合并为菜单栏正下方一行工具栏（1.4.1）。
  - 移除主界面 MCU 型号 EDIT 与标签；连接时使用通用默认设备 `Cortex-M4`（J-Link 按内核自动识别；空设备会令旧版 J-Link DLL 在 Connect 时崩溃，已实测 0xC0000005）。
  - 工具栏单行化：打开ELF/连接/断开/开始采集/停止采集/记录/停止记录/离线回放/停止回放/关于 + 接口(SWD/JTAG)/速度/ J-Link 设备/刷新 全部排列在菜单栏正下方一行，布局统一按文字宽度自适应。
  - 回归：连接测试（开发版+安装版）ALL PASS；ui_windows_drive/ui_layout_drive/ui_rename_drive 全部 ALL PASS（layout C 阶段与 rename 增加轮询/等待，消除连续运行竞态）；构建 0 error/0 warning。
  - 版本 1.4.1：重新打包 `dist/OpenScope-Setup-1.4.1.exe`（10.3MB），安装版验证版本 1.4.1.0 + 主界面直连。
- **checkpoint-17（2026-08-09）**：完成 request.md 新增特性 8~12 + Bug 1/2/3 全部 + N3/N6 更新（1.6.0）。
  - N3(更新)：tab 就地重命名不再弹窗，直接在标签上修改；清空/删除全部字符后回车兜底为 Default；修复焦点检查导致编辑框被过早销毁的 bug（仅当焦点转移到本应用其他窗口时才提交，外部程序/桌面失焦保留编辑框）。
  - N6(更新)：删除关于按钮/工具栏“关于”，仅保留帮助菜单中的“关于 OpenScope”文档（晶圆上的生物技术开发和提供支持 + 版本号 + www.opendebugger.com）。
  - N8 速度设置：SWD/JTAG 时钟速度更多预设 + 支持手工输入速度（连接直接读取控件值）。
  - N9 多变量采集/移除 + CTRL+B 多坐标轴修复 + 左侧变量栏自动隐藏/钉住 + 分隔条调宽。
  - N10 数值窗口就地写入（值栏直接输入回车写入一次，不再弹窗）+ 变量前“实时”勾选框控制是否实时更新。
  - N11 窗口最大化填满当前 tab + 一个 tab 可同时放置多个波形/数值窗口（OS_WinItem.group[] 组机制）。
  - N12 左侧 elf 栏钉图标 OSTreePin + 未钉住自动隐藏（tree_auto_tick 定时检查）。
  - Bug1 修复：添加 2 个变量后 App 异常退出（repro_bug1_dialog/combos 回归 ALL PASS）。
  - Bug2 修复：关闭后重开任务栏有图标但窗口不可见（os_mainwin_show + ui_bug2_restore_drive 回归 ALL PASS）。
  - Bug3 修复：单窗口全屏/退出全屏（SetParent 临时改父为主窗口铺满客户区，退出还原回 tab）。
  - 测试：新增 `tests/ui_features_drive.ps1`（16 项：N6 无关于按钮 / N11 多窗口+最大化 / Bug3 全屏 / N12 钉图标 / N10 结构 / Ctrl+B 不崩溃），全部回归 dev+installed 两版 ALL PASS。
  - 版本 1.6.0：重新打包 `dist/OpenScope-Setup-1.6.0.exe`（10.4MB），安装版验证版本 1.6.0.0。
- **checkpoint-18（2026-08-09）**：完成 request.md 新增特性 13（波形窗口分析增强）全部 + 需求 9（git tag + 双远端推送）（1.7.0）。
  - N13a 添加变量弹窗多选：列表改 ListView（SysListView32，报表模式 + LVS_EX_FULLROWSELECT），原生支持 Ctrl+A 全选、Ctrl+单击多选、Shift 起止范围选（含起止本身）；新增 `os_dlg_pick_vars`（单变量 `os_dlg_pick_var` 复用其封装），波形/数值窗口“添加变量”一次批量添加全部选中项；新增 `WM_OS_PICK_TEST_SELECT`（WM_APP+30）测试钩子（跨进程无法伪造键盘状态/指针式 LVM_SETITEMSTATE），在对话框进程内程序化选中范围等价 Ctrl/Shift 手选结果。
  - N13b/c 多坐标轴左置 + Ctrl+B 堆叠：`multiaxis` 重构为 `stacked`，逐行堆叠时每路信号独立一行、独立 Y 轴刻度左置（信号颜色），叠加排列时共享 Y 轴 + 图例；Ctrl+B / 菜单“逐行堆叠 (Ctrl+B)”切换，日志保持 `波形多坐标轴: 1/0`（兼容旧回归）。
  - N13d 放大后采样点圆点显示：可见采样点 <=120 时以 2px 圆点绘制，便于观察采样间隔/时间。
  - N13e 光标测量：绘图区两次左键分别设置测量标记 1/2，日志输出 `波形测量标记1: t=..值=..` 与 `波形测量Δ: ΔX=.. ΔY=..`，图上以虚点线标出并显示 Δ 读数框（第三次点击清除）。
  - N13f/g 光标数值 HUD：鼠标悬停绘图区显示十字光标（PS_DOT），右侧 320px HUD 框列出各系列“变量名 = 数值 (类型)”在鼠标时刻的取值（同一 X 轴各 Y 曲线数值同时显示）；类型名含 int32_t/uint8_t/float/bool/enum/char/指针 + :bf 位域。
  - 测试：新增 `tests/ui_pick_multi_drive.ps1`（N13a：ListView 结构/扩展样式/模糊搜索填充 178 项/范围选中含起止/全选首末项/批量添加 2 个 + 条数核对）与 `tests/ui_chart_n13_drive.ps1`（N13b-g：堆叠切换/滚轮放大/两次点击 Δ 测量/光标 HUD 渲染），dev+installed 两版全部 ALL PASS；连同既有 ui_rename/ui_windows/ui_chartview/ui_layout/ui_n9_watch/ui_bug2_restore/ui_features/ui_connect 全套回归 dev+installed 全部 ALL PASS；构建 0 error/0 warning。
  - 版本 1.7.0：重新打包 `dist/OpenScope-Setup-1.7.0.exe`（10.4MB），安装版验证版本 1.7.0.0。
  - 需求 9：checkpoint-18 提交 git + `git tag v1.7.0` + 推送 `gitee_origin` 与 `github_origin` 双远端（首个 tag，便于 checkpoint 管理）。
- **checkpoint-19（2026-08-09）**：完成 request.md 新增特性 14/16 + Bug 4/5 补充（1.8.0）。
  - F14 消息栏上下拉伸：新增横向分隔条 `OSSplitterH`（菜单栏/工具栏与消息栏之间，5px，IDC_SIZENS 光标），拖动发送 `WM_OS_SPLIT_V`（WM_APP+11）按"主窗高 - 34 - 27 - 分隔条 y"实时调整 `log_h`（clamp [40, 窗高-120]），layout() 同时移动消息栏；`log_h` 继续随布局保存/恢复。
  - F16 tab/右侧空白右键新建窗口：`OSRightPanel` 右键（WM_RBUTTONUP）与 tab 标签条空白处右键（NM_RCLICK → hit<0）均弹"新建波形/数值/示波器窗口"上下文菜单（new_win_context_menu，含全部模块窗口项）。
  - Bug4 补充 树 Ctrl 多选批量添加：树改 `TVS_EX_MULTISELECT` 扩展样式（`TreeView_SetExtendedStyle`，TVS_MULTISELECT 为不存在的基本样式常量；`TVGN_FIRSTSELECTED` 未定义，用 `TVGN_NEXTSELECTED(NULL)` 取首个选中）；新增 `tree_selected_ids` 收集全部选中叶 id、`tree_context_select_hit`（右键未选中项则单选命中项）；"添加到波形/数值/示波器"全部走批量路径（新增 `tree_add_to_native`/`tree_add_to_scope`，优先当前激活窗口），日志 `树右键批量添加变量: N 个`。
  - Bug5 补充 波形内部标题 + 圆点消失：chart_draw 移除内部标题文字"波形窗口1"绘制；圆点判定由"缓冲总点数 npts<=120"改为"可见时间窗 [x0,x1] 内实际绘制点数 vis_npts<=120"，录制再长放大后圆点仍显示（状态变化记 `波形采样点圆点: 可见 N 点` 调试日志）。
  - 测试：新增 `tests/ui_logsplit_drive.ps1`（F14：拖 170→100→220 消息栏高度跟随 + 不闪退）、`tests/ui_rightmenu_drive.ps1`（F16：右侧/tab 空白右键弹出 #32768 菜单 + WM_CANCELMODE 关闭 + 不闪退）、`tests/ui_tree_multisel_drive.ps1`（Bug4：`WM_OS_TREE_TEST_SELECT` 进程内选择测试钩子选 3/2/4 个叶批量添加到波形/数值/示波器 + 逐变量日志核对）、`tests/ui_chart_bug5_drive.ps1`（Bug5：`chart_replay_long.csv` 2000 采样点长回放 + 滚轮放大 15 次 → `波形采样点圆点: 可见 N 点` + 不闪退）；连同既有 ui_rename/ui_windows/ui_chartview/ui_layout/ui_n9_watch/ui_bug2_restore/ui_features/ui_pick_multi/ui_chart_n13/ui_connect 全套回归 dev+installed 全部 ALL PASS；构建 0 error/0 warning。
  - 版本 1.8.0：重新打包 `dist/OpenScope-Setup-1.8.0.exe`（10.4MB），安装版验证版本 1.8.0.0。
  - 需求 9：checkpoint-19 提交 git + `git tag v1.8.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。

  - N4 应用图标：`version.rc` 加载 `icon\OpenScope.ico`（IDI_APP=1），主窗口类改 `WNDCLASSEXW` + hIcon/hIconSm，任务栏/窗口标题图标生效。
  - N5 MCU 型号选择：主界面新增 MCU 型号下拉（ID 2101，Cortex-M4/M3/M0/A5 + STM32L432KB/F103C8/F407VG/F429ZI/G431KB/nRF52832/NRF5340/RP2040 共 12 项），默认 Cortex-M4；连接直接读取下拉文本传入 `OS_ConnectCfg.device`，不弹窗。
  - N6 关于框：新增“晶圆上的生物技术开发和提供支持” + 版本号 v1.5.0 + 网址 www.opendebugger.com。
  - N7 波形视图：滚轮缩放 X 轴（围绕鼠标、最小 1ms、夹紧到全量范围）+ Ctrl+滚轮缩放 Y 轴；F 键全局显示（view_all）；Ctrl+B 多坐标轴（每路独立 Y 值域）；菜单新增“整体展示/多坐标轴/缩放复位”；停止采集广播 `WM_OS_CHART_FITALL`（整体展示+暂停），开始采集广播 `WM_OS_CHART_LIVE`（跟随最新）。
  - 修复新建波形窗口 X 缩放失效 bug：`calloc` 后 `view_all=0/fit_x=0/vx0=vx1=0` 使 X 时间窗为空、滚轮 X 缩放静默返回；初始化为 `fit_x=1/fit_y=1`（跟随最新）后 X 缩放正常。
  - M1 完善：无 J-Link 设备时连接弹窗提示“没有发现 JLink 设备...”（与需求一致）；jlink/scope 模块版本同步 1.5.0。
  - 测试：新增 `tests/ui_chartview_drive.ps1` + `tests/chart_replay.csv`（`--replay` 测试钩子回放 fsin 正弦数据驱动波形，无需真实 MCU）覆盖 N5/N7 全部交互；ui_chartview/ui_windows/ui_layout/ui_rename 四个回归开发版+安装版全部 ALL PASS；构建 0 error/0 warning。
  - 版本 1.5.0：重新打包 `dist/OpenScope-Setup-1.5.0.exe`（10.3MB），安装版验证版本 1.5.0.0 + N4~N7/M1 全套功能。

## 待办（BMAD 规划产出后更新）

- [x] 生成项目上下文 `project-context.md`
- [x] 架构 spine（ARCHITECTURE-SPINE.md）
- [x] Epics / Stories 分解
- [x] 实施就绪检查
- [x] Sprint 计划
- [x] Epic 1：修复框架构建（Story 1.1/1.2/1.3）
- [x] Epic 2：J-Link 驱动模块（Story 2.1~2.4，硬件实测待用户环境）
- [x] Epic 3：采集/记录/回放（Story 3.1~3.3，硬件实测待用户环境）
- [x] Epic 4：scope 窗口模块（Story 4.1~4.3，交互实测待用户环境）
- [x] J-Link 硬件实测：RAM 8KB 读写 + Flash 64KB 读取（checkpoint-6）
- [x] .out 文件解析验证 + ELF sh_link 修复（checkpoint-7）
- [x] 真实 enc.out 全量解析 + 结构体原子展开（checkpoint-8）

## 新增需求（request.md 第 8 条，已完成）

- [x] 工程开发完成后打包成 .exe 安装包并添加版本号，便于发布（用户 2026-08-08 加入 request.md；checkpoint-9 完成）

## 新增特性（request.md 新增特性 1~7，全部完成）

- [x] 1/2 布局保存/恢复 + 布局文件导出导入（checkpoint-12）
- [x] 3 tab 多窗口 + 重命名（checkpoint-11/13）
- [x] 4 应用图标（checkpoint-16）
- [x] 5 连接配置嵌入主界面 + MCU 型号选择（checkpoint-14/15/16）
- [x] 6 关于框写明“晶圆上的生物技术开发和提供支持”+版本号+www.opendebugger.com（checkpoint-16）
- [x] 7 波形滚轮缩放 X/Y、停止后整体展示、Ctrl+B 多坐标轴、F 全局显示（checkpoint-16）
- [x] 8 SWD/JTAG 更多速度设置 + 手工输入时钟速度（checkpoint-17）
- [x] 9 波形多变量采集 + 变量移除 + CTRL+B 多坐标轴 + 左侧变量栏自动隐藏/钉住/分隔条（checkpoint-17）
- [x] 10 数值窗口就地写入 + 实时更新勾选框（checkpoint-17）
- [x] 11 窗口最大化填满 tab + 一个 tab 多窗口（checkpoint-17）
- [x] 12 左侧 elf 栏钉图标 + 自动隐藏（checkpoint-17）
- [x] 13 波形窗口分析增强：多选添加变量 / 多坐标轴左置 + Ctrl+B 堆叠 / 放大采样点圆点 / 光标 Δ 测量 / 悬停数值 HUD（checkpoint-18）
- [x] 14 底部消息栏支持上下拉伸（checkpoint-19）
- [x] 16 tab 和右侧空白处右键新建 tab（波形/数值/示波器窗口）（checkpoint-19）

## 需求（request.md 软件功能清单）

- [x] 9 每次开发后填好 checkout point、提交 git、添加 tag 并推送到远端 gitee_origin / git_hub（checkpoint-18 起执行：`git tag v1.7.0` + 双远端推送；checkpoint-19：`git tag v1.8.0` + 双远端推送）

## Bug（request.md，已修复）

- [x] Bug1 添加 2 个变量后 App 异常退出（checkpoint-17）
- [x] Bug2 关闭重开任务栏有图标但窗口不可见（checkpoint-17）
- [x] Bug3 单窗口全屏/退出全屏（checkpoint-17）
- [x] Bug4 左侧变量栏自动隐藏/钉住 + Ctrl 多选批量添加到窗口（checkpoint-17 / checkpoint-19）
- [x] Bug5 去掉波形内部标题"波形窗口1" + 放大采样点圆点录制变长后消失（checkpoint-19）

## 总结

- 4 个 Epic / 13 个 Story 全部完成：框架可构建可启动、J-Link 驱动模块（扫描/连接/读写）、采集/CSV/回放、scope 示波器窗口模块。
- 构建与回归：`python build.py --quiet` 0 error/0 warning，自动运行 replay_smoke + jlink_smoke + scope_smoke 全部 PASS。
- 打包发布：`dist/OpenScope-Setup-1.0.0.exe` 安装包（版本号 1.0.0.0），安装/卸载/启动均已验证。
- 依赖真实硬件的验收项（连接 MCU、实时采集曲线、写值回读）留待用户环境实测。
- 硬件实测已覆盖：连接、RAM 读写、Flash 读取；实时采集曲线/写值交互仍可在应用 UI 中实测。
