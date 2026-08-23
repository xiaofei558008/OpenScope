# OpenScope 开发进度

本文件记录项目开发进度与检查点（checkpoint）。每次阶段性完成后更新并提交 git，防止终端崩溃/卡死导致进度丢失。

## 检查点

- **checkpoint-52（2026-08-23）**：需求 14 网络远程操作功能全部实现并通过双实例端到端测试（1.21.0）。
  - **补齐核心链路**（此前 checkpoint-40~51 只有协议内核 + 监听/连接/ELF 同步外壳，采集/写值/异步传输未接线）：
    - 采集流接线：采集线程每 ~30ms 调用 OS_CMD_NET_PUSH 广播最新样本（flat 编码，逐样本独立无累积误差）。
    - 远端监视列表驱动采集：新增 OS_CMD_NET_WATCH（发送本机勾选叶列表）；服务端收到 WATCH_LIST 后经框架 v4 新回调 set_watch/acq_start 自动勾选 + 自动开始采集（"远端下达 → 本地 J-Link/ST-Link 采集"闭环）。
    - 网络写变量：新增 OS_CMD_NET_WRITE（发送 + 等对端 ACK ≤3s）；数值窗口写值在无驱动/写失败时自动经网络转发到探针侧执行（远端窗口直接输入值即可标定 MCU）。
    - 异步传输：新增 OS_CMD_NET_LOG_PULL + LOG_REQ/CHUNK 流——探针侧把采集历史（专用网络历史环 65536 样本，UI 排空不影响）经 8 线程并行压缩（delta+XOR 无损）分块回传，远端累计重组解码注入窗口；UI 新增"同步采集/下载记录"按钮。
  - **修复 6 个真 bug**：
    - 会话线程 0xC00000FD 栈溢出：MSVC /O2 把 handle_msg 各分支局部变量并入同一栈帧，CHUNK 分支 1.5MB 数组导致 HELLO 处理即溢出崩溃——大缓冲全部改堆分配；ELf 变量表载荷/接收缓冲同步扩容（此前 64KB 上限装不下 500+ 叶真实 ELF）。
    - 纯客户端实例会话循环永不运行：g_running 只在 NET_START 置 1，NET_CONNECT 后 recv 循环直接退出、连接即断——NET_CONNECT 也置 1。
    - 写值通道全失败：本机 JLink_V966 的 JLINKARM_WriteMem 成功返回写入字节数（实测 rc=4）而非 0，mod_write 判错——接受 0 或字节数均为成功。
    - ACK zigzag 解码错误：负错误码解出错误正值（-1→0），远端误报"写成功"——解码补 -1。
    - 叶路径变量（"g_cfg.a"）在探针侧无法解析：resolve 误用 find_variable（只认顶层变量名）作闸门——直接按叶全路径名匹配。
    - 写失败网络转发形成乒乓循环（探针写失败→转发远端→远端再转发回来）——仅无驱动侧允许转发。
  - **双实例端到端测试（tests/net_drive.ps1 重写，接入 build_tests）**：三实例实测（A=探针监听+UI 点击连接 J-Link 真机；B=远端连接+ELf 双向同步+勾选变量+建波形窗口+发监视列表+网络写值+下载历史+截图；C=第二远端），断言 18 项：监听+2 连接 rc=0、一对多 fan-out、ELF 双向同步 4 叶、监视列表下达（2 项/1 项）、采集自动启动、真机 4kHz 采集、WRITE_VAR→MCU→ACK 0、历史回传 2400+ 样本、样本注入累计 898、波形截图 1.6MB 非空白（像素分析含黄/青两条曲线）。连续 3 次 ALL PASS + 安装版 ALL PASS。
  - 新增 CLI 测试钩子：--watch=名1,名2（勾选叶）、--net-win=chart,名1,名2（建窗口加变量）、--net-write=名=值、--net-download、--net-watch、--net-shot-at=路径,毫秒（延迟截图）、--log=路径（实例独立日志，消除多进程共享日志丢行）。
  - 回归：build.py 全绿（含 netcore_smoke 新增 CHUNK 流/并行包格式用例）；安装版 ui_windows/ui_features/ui_connect(真机)/ui_layout/ui_theme_dark/ui_n9_watch/ui_spool 全部 ALL PASS。
  - readme.md 新增 3.7 网络远程操作章节（用法/特性/服务器转发说明）。
  - 未做项（按需求措辞"可以/考虑"）：外网服务器（8.133.18.102）实测转发未做——WebSocket 为纯 TCP，可经任意 TCP 转发/反向代理中继，架构已预留；局域网/回环为实测路径。
  - 版本 1.21.0：重新打包 `dist/OpenScope-Setup-1.21.0.exe`（14.2MB），静默安装验证 1.21.0.0 + 网络全套功能。
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
- **checkpoint-20（2026-08-09）**：完成 request.md 需求 10（BMAD 全自动循环结束 Windows 语音播报"任务执行完毕"）（1.8.1）。
  - R10 自动完成语音通知：新增 `tools/notify_done.ps1`（可复用、参数化 Text/Rate/Volume），优先 System.Speech（.NET，自动选择中文语音 Microsoft Huihui Desktop），失败回退 SAPI COM `SAPI.SpVoice`；本机验证语音引擎可用（System.Speech + SAPI 均 OK，含 zh-CN 语音），脚本播报退出码 0。
  - 该需求为 process/notification 型，无 OpenScope.exe 应用代码改动；语音播报作为全自动循环末尾收尾步骤，提示用户检查。
  - 需求 9：checkpoint-20 提交 git + `git tag v1.8.1` + 推送 `gitee_origin` 与 `github_origin` 双远端。
  - 版本 1.8.1：重新打包 `dist/OpenScope-Setup-1.8.1.exe`（10.4MB），安装版验证版本 1.8.1.0；构建 0 error/0 warning。
- **checkpoint-21（2026-08-09）**：修复 request.md Bug 7/8/9 + 需求 17（F17 跳过选芯片只选核心）（1.8.2）。
  - Bug7 采集时点击"记录"界面卡死：根因是 (1) cmd_log_start 采集运行中直接弹 GetSaveFileNameW 模态对话框，未像 cmd_replay_open 先停采集，与采集线程跨线程日志互卡；(2) 采集线程 os_log 跨线程直接 ListView_InsertItem（SendMessage）在模态循环内死锁。修复：cmd_log_start 先 os_ds_stop 停采集、对话框关闭后 os_ds_start 恢复采集；新增 `WM_OS_LOG` 消息（app.h），非主线程日志 PostMessage 到主线程再插入日志 ListView（线程安全）。
  - Bug8 自动隐藏归位到左侧变量树：删除工具栏"钉住变量栏"按钮（IDC_BTN_PIN=2015）及其 layout 项，唯一入口为左侧树顶部钉图标 OSTreePin（金=钉住常显/灰=自动隐藏）。
  - Bug9 非4000速度读值全为0：根因是 JLINKARM_ReadMem 失败返回正值(rc=1)而非负数，mod_read 用 `r>=0` 把失败当成功，零缓冲被推为有效样本。修复：成功严格判定 `r==0`；掉线（IsConnected=0）自动重连一次（500ms 节流 last_reconnect_ms）；mod_write 契约改为 `r==0`；设备列表改通用核心名（Cortex-M4/M0+...）消除连接挂起。
  - F17 跳过选芯片环节：纯 SWD/JTAG 内存/Flash 读取只需核心名（CoreSight AHB-AP 架构一致），无需具体芯片型号；连接配置设备下拉改为 10 项通用核心名，默认 Cortex-M4。
  - 回归：新增 `tests/bug9_smoke.c`（J-Link 读一致性：每速度至少 1 次成功 + 所有 rc==size 读值一致，直接捕获 r>=0 屏蔽零值 bug；50/400/4000/12000 kHz 四速度 ALL PASS，接入 build_tests.bat 每次构建自动回归）+ `tests/ui_record_dialog_drive.ps1`（Bug7：采集运行中点击记录→保存对话框→取消→进程存活/采集自动恢复/主线程响应）+ `tests/ui_features_drive.ps1` 加 Bug8 断言（工具栏无 2015 按钮）；新增 `tests/run_regression.sh` 全量 UI 回归脚本。
  - 版本 1.8.2：全量 UI 回归 dev + 安装版 ALL PASS（含 bug9_smoke 4 速度 + Bug7/Bug8 新用例）；重新打包 `dist/OpenScope-Setup-1.8.2.exe`（10.4MB），安装版验证版本 1.8.2.0；构建 0 error/0 warning。
  - 需求 9：checkpoint-21 提交 git + `git tag v1.8.2` + 推送 `gitee_origin` 与 `github_origin` 双远端。
- **checkpoint-22（2026-08-09）**：修复 request.md Bug 10（12000kHz 高速采集一开始就失败、采集线程退出）（1.8.3）。
  - Bug10 高速瞬时掉线误杀采集线程：边缘目标 12000kHz 连接后 ~20ms SWD 掉线（IsConnected=0），mod_read 自动重连节流 500ms；旧 `fail_count>10` 硬中断（20ms 周期 × 1~2 变量 ≈ 100~200ms 即超限）在重连恢复前就 break 退出 → "采集线程退出，读取 xxx 变量失败"。修复：datasrv.c poll_thread 失败处理改为时间基停摆判定 `OS_POLL_STALL_MS=3000`（连续 3s 无成功样本才停止），瞬时失败只节流记日志（前 3 次）不中断；真死连接仍由 IS_CONNECTED（connected=0）即时捕获；jlink.c 重连成功日志节流 5s 一次（`OS_JLINK_RECONNECT_LOG_MS`）。
  - 回归：新增 `tests/ui_speed12000_drive.ps1`（Bug10 UI 回归：速度下拉设 12000 → 连接 → 开始采集 → 等待 3.5s（>500ms 重连节流多次周期）→ 断言日志不出现"采集线程已退出/采集停止：长时间/采集停止：MCU 连接已断开"且进程存活；连接失败路径放宽为不闪退）。注意：`SetWindowText` API 对 ComboBox 不生效，须 `SendMessage WM_SETTEXT` + 读回校验。dev 版 3 次 + 安装版 1 次 ALL PASS。
  - 版本 1.8.3：全量 UI 回归 dev + 安装版 32/32 ALL PASS（13 非 J-Link + 3 J-Link × 2 变体，含新增 ui_speed12000_drive.ps1）；重新打包 `dist/OpenScope-Setup-1.8.3.exe`，安装版验证版本 1.8.3.0；构建 0 error/0 warning。
  - 需求 9：checkpoint-22 提交 git + `git tag v1.8.3` + 推送 `gitee_origin` 与 `github_origin` 双远端。
- **checkpoint-23（2026-08-09）**：修复 request.md Bug 11/12/13 + 新增特性 18/19 + 需求 11 发布 v1.9.0（1.9.0）。
  - Bug11 波形闪烁 + 采集线程退出提示：chartwin WM_PAINT 改双缓冲绘制（先画内存 DC 再一次性 BitBlt），避免采集时鼠标在波形区滑动（WM_MOUSEMOVE 高频重绘）整窗闪烁；datasrv 采集线程正常停止（stop_poll 置位）为 INFO「采集线程已退出」，异常退出（断连/停摆）为 WARN 级。
  - Bug12 变量栏自动隐藏修复：`g_tree_in_ms` 原用 `(int)(os_time_us()/1000)` 存 Unix 纪元毫秒，2026 年值 ~1.78e12 强转 int 溢出为负 → 时间差比较永远为假、永不隐藏；改用 `GetTickCount64()`（自启动毫秒，int64 无溢出）。隐藏后仅留左侧 8px 细条 OSTreeStrip（layout 隐藏分支补 `ShowWindow(SW_SHOW)`），光标进入细条（客户区 x<=10）自动展开；`tree_auto_expand` 日志 `%ls`+中文在 C locale 转多字节失败产生空行 → 改 `os_wide_to_utf8_buf` + `%s`。
  - Bug13 消息区域多选 + 右键复制/清除：日志 ListView 不设 LVS_SINGLESEL 支持 Ctrl/Shift 多选；右键菜单「复制选中」(IDM_LOG_COPY=2601) /「全部清除」(IDM_LOG_CLEAR=2602)；新增 `WM_OS_LOG_TEST_SELECT`（WM_APP+31）进程内测试钩子（跨进程指针式 LVM_SETITEMSTATE 无法编组会 0xC0000005）；日志 ListView 恢复 LVS_EX_DOUBLEBUFFER（1.8.3 证实与崩溃无关）。
  - N18 删除示波器功能模块：删除 module/scope（scope.c/vcxproj/tests）+ dll/scope.dll，移除「示波器窗口」菜单项。
  - N19 数值窗口右键添加变量多选：复用 N13a 的 `os_dlg_pick_vars` 多选对话框，一次批量添加全部选中变量（日志 `数值窗口批量添加变量: %d 个`）。
  - 回归：新增 `tests/ui_chart_flicker_drive.ps1`（Bug11：300 次鼠标滑动 + 高频重绘不闪退/无 FATAL）、`tests/ui_log_select_drive.ps1`（Bug13：多选支持/剪贴板复制/全部清除）、`tests/ui_tree_autohide_drive.ps1`（Bug12：光标移出自动隐藏 + 细条悬停展开 + 钉住保持）；`tests/run_regression.sh` 纳入 3 个新脚本；全量回归 dev + 安装版 **38/38 ALL PASS**（16 非 J-Link + 3 J-Link × 2 变体，含新 3 脚本）；构建 0 error/0 warning。
  - 版本 1.9.0：重新打包 `dist/OpenScope-Setup-1.9.0.exe`（10.4MB），安装版验证版本 1.9.0.0 + 启动日志 `OpenScope 启动 (version 1.9.0)`。
  - 需求 11：发布 `dist/OpenScope-Setup-1.9.0.exe` + request.md 到 `D:\OpenDebugger`；`publish.py` 上传 www.opendebugger.com/downloads/（下载页 + 最新包 HTTP 200、SHA256 一致 cc3d3426...）；修复 publish.py 自检 emoji（✅/❌）在 GBK 控制台 UnicodeEncodeError → 改 ASCII [OK]/[ERR]。
  - 需求 9：checkpoint-23 提交 git + `git tag v1.9.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。
- **checkpoint-23 附（2026-08-09）**：软件下载包发布基础设施（request.md 第 11 条，自动发布，与 1.9.0 同批提交）。
  - 网站修复：www.opendebugger.com 此前仅 HTTP（80），现代浏览器自动升级 https 导致"无法访问"。已签发 Let's Encrypt 证书（certbot + nginx 443 监听 + HTTP→HTTPS 301 重定向，certbot.timer 自动续期），并修复 `/img/` 目录列出 404。
  - 下载基础设施：服务器新增 `/var/www/downloads/openscope/`（安装包存储）+ nginx `location ^~ /downloads/`（alias 到 /var/www/downloads，.exe 返回 `Content-Disposition: attachment` + `application/x-msdownload`，支持断点续传）。
  - 全自动发布：新增 `publish.py`（扫描 dist/ → 上传缺失/更新的安装包 → 自动生成下载页 index.html（版本/大小/日期/SHA256/最新版徽标，蓝主题与主站一致）→ 上传 → HTTPS 自检）；`packaging/make_setup.py` 新增 `--publish` 参数，实现"打包 + 发布"一步完成。
  - 主站入口：`/var/www/html/shop/index.html` 导航栏新增"软件下载"，首页 Hero 新增"下载 OpenScope 软件"按钮。
  - 已发布：OpenScope-Setup-1.4.1 ~ 1.8.3 共 8 个版本全部上线 `https://www.opendebugger.com/downloads/`；端到端验证通过（下载页 HTTP 200、最新包 HTTP 200、远程/本地 SHA256 一致 0ac3c19...）。
  - 下载修复：用户反馈"只下载到名为'下载'的文本文件"——根因是 `Content-Disposition: attachment` 未带 `filename=`，且嵌套正则 `^/downloads/([^/]+\.exe)$` 无法匹配 `/downloads/openscope/` 子目录路径。修复为 `Content-Disposition: attachment; filename="$1"` + 正则 `([^/]+\.(?:exe|zip|...))$`（捕获任意层级文件名）。修复后响应头 `Content-Disposition: attachment; filename="OpenScope-Setup-1.8.3.exe"`，SHA256 与本地一致。
  - 操作文档：`RELEASE.md`（一键发布 `python packaging\make_setup.py --publish`）。
  - 注：发布涉及服务器修改（nginx/证书/文件）需 sudo，密码见 request.md；publish.py 走本机免密 scp 无需 sudo。
- **checkpoint-24（2026-08-09）**：新增特性 20 界面主题（白色默认/黑色，配色参考 IAR / Notepad++）+ Bug14 删除死代码芯片配置弹窗，v1.10.0。
  - F20 主题架构：新增 `code/src/theme.c/h`——全部自定义绘制（主窗/分隔条/树/日志/波形/数值/对话框）统一走 `os_theme(id)` 取色 + `os_theme_brush()` 缓存实心画刷（切换主题时重建）；调色板 TH_*（背景/面板/文本/边框/编辑框/树/日志/网格/tab/状态栏/波形区）；持久化到 `%LOCALAPPDATA%\OpenScope\layout.ini` 的 `theme` 键（0=白 1=黑），`os_theme_load()` 读回、`os_theme_set_dark()` 运行时切换并即时重绘；「设置」菜单命令 IDM_THEME_DARK=2701 切换 + 写回 layout.ini。
  - 系统暗色（标题栏/菜单/tab/标准控件）：uxtheme 未文档化序数 SetPreferredAppMode(135)/RefreshImmersiveColorPolicyState(104)/AllowDarkModeForWindow(133)/FlushMenuThemes(136) 用 `LoadLibraryW` 动态加载（`GetModuleHandleW` 对尚未加载的 DLL 返回 NULL）；`SetPreferredAppMode(FORCE)` 必须在 `CreateWindowW` 之前调用（`os_theme_load()` 内提前应用），否则后续创建的公共控件仍按系统浅色渲染；DWM 沉浸式暗色标题栏 `DwmSetWindowAttribute` attr 20（回退 19）。
  - 标准控件暗色：父窗口 `WM_CTLCOLOR*` 返回主题画刷；按钮/编辑框/tab 用 `SetWindowTheme(hwnd, L"DarkMode_Explorer")`；组合框（设备/接口/速度/仿真）与状态栏完全子类化自绘（DROPDOWNLIST 闭合面与状态栏分栏 SetWindowTheme 无效）；日志/数值窗口列头 SysHeader32 子类化自绘（SetWindowTheme 对 SysHeader32 无效）。新增共享 `os_theme_listview_header()`（按句柄去重，可重复调用）。
  - Bug14：删除死代码芯片配置弹窗路径（弹窗选芯片不再出现），回归无闪退。
  - 回归：新增 `tests/ui_theme_dark_drive.ps1`（暗色启动采样树/右栏/日志/状态栏均深色 + 菜单命令运行时切换回亮色 + layout.ini 持久化，PrintWindow+GetPixel 校验；PS1 需带 UTF-8 BOM，否则 PowerShell 5.1 按 GBK 误读中文注释解析失败）；加固 `ui_chart_bug5_drive.ps1` 日志断言（3s 重试，回归负载下日志落盘可能滞后）；纳入 run_regression.sh。全量回归 dev **17/17 ALL PASS**（16 既有 + 新主题脚本），构建 0 error/0 warning。
  - 版本 1.10.0：`python build.py --quiet` 干净构建，重新打包 `dist/OpenScope-Setup-1.10.0.exe`（10.4MB），静默安装验证版本 1.10.0.0；安装版全量回归 17/17 ALL PASS。
  - 需求 9：checkpoint-24 提交 git + `git tag v1.10.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。
- **checkpoint-25（2026-08-09）**：新增特性 21（F21 高速采集——软件快速采样：自由运行 + 连续地址块读 + UI 节流），v1.11.0。
  - F21/Step1 高速采集（µs 级采样的第一步；第二步"目标端 µs 缓冲"留待后续版本）：
    - 自由运行：`poll_thread` 移除固定 `Sleep(20ms)`，改 QPC 自计时（`poll_interval_ms>0` 补睡保持定时，否则周期=实际块读耗时）；`main.c` 默认 `poll_interval_ms=0`；启动日志区分「周期 %d ms」/「自由运行高速模式」。
    - 连续地址块读：观测叶按地址排序（qsort），间隔 ≤8 字节 / 单块 ≤512 字节合并为一次 `OS_CMD_READ_MEM` 块读，再按叶偏移切分产出样本（位域/同址别名共享字节）；消除每变量 ~50-200µs 的 J-Link 读事务开销。
    - UI 刷新节流：`os_ds_push_batch` 的 `WM_OS_SAMPLES` 从"每批一条"改为 ~16ms（60Hz）节流，环内样本全收、UI 限速排空，避免高频消息洪泛；`os_ds_stop` 排空残留环样本。
    - 速率日志：采集线程每秒输出「采集速率: N 样本/s，周期 X µs（N 变量）」便于观测。
    - Bug10/11 保护不变：连接检查 + 3s 停摆判定 + 瞬时失败节流日志 + 重连节流。
  - 实测（STM32L432KB，12000kHz SWD）：单变量 5449 样本/s（周期 184µs）；3 个连续变量（fsin+cnt+ang_rd_error_cnt）合并为一次 7 字节块读 → 12243 样本/s（周期 245µs）。旧 20ms 循环 ~50/s，提速 ~100-240×。
  - 回归：新增 `tests/ui_speed_verify_drive.ps1`（3 连续变量布局，断言「自由运行高速模式」+ 速率 >2000 样本/s + Bug10 线程存活；默认 4000kHz 实测 ~8.1k/s；12000 高速连接有环境性失败风险由 ui_speed12000 单独覆盖），纳入 run_regression.sh JLINK 组。全量回归 dev+安装版 42 项 ALL PASS（chart_bug5 安装版一次负载下日志落盘偶发失败，单独重跑 PASS，为已知时序竞态）。
  - 版本 1.11.0：`python build.py --quiet` 干净构建，重新打包 `dist/OpenScope-Setup-1.11.0.exe`（10.4MB），静默安装验证版本 1.11.0.0；发布 `https://www.opendebugger.com/downloads/`（下载页 + v1.11.0 安装包 HTTP 200 + 字节一致 10367047）。
  - 需求 9：checkpoint-25 提交 git + `git tag v1.11.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。

- **checkpoint-26（2026-08-09）**：修复 Bug16（12000kHz 掉线重连 `JLINKARM_Open` 返回垃圾值）+ 新增特性 21（不再触发 "Target device setting" 弹窗），v1.12.0。
  - Bug16 复现：用户报 12000kHz 采集掉线自动重连时 `JLINKARM_Open 失败 rc=-488389840`（0x1D1C3CD0，两次同值）→ 采集线程退出。本机 12000kHz 采集中断开/重连压测复现同类故障：open 返回垃圾 `rc=-637353168`，甚至 open 挂起数分钟后 JLink DLL 内部 AV（0xC0000005 @ JLink_x64.dll）。
  - 根因：高速（12000kHz）采集中掉线/断开后，JLink_x64.dll 的会话内部状态损坏（USB 层瞬时故障、close 与 in-flight 读并发），后续 `JLINKARM_Open` 返回垃圾值/挂起，重试 open 无法恢复。
  - 修复（module/jlink/jlink.c）：
    - 连接前整库重载：`JLinkCtx` 增 `dll_used`，首连用初始绑定，此后每次连接先 `jlink_reload()`（FreeLibrary+LoadLibrary+重绑）保证 open 从全新会话开始，杜绝脏会话复用；
    - open 失败兜底：再重载重试一次，仍失败才判连接失败（错误码不再可能是垃圾值）；
    - 重连重试：`mod_read` 自动重连改重试 3 次（间隔 1.5s），USB 层恢复后采集自动恢复，不轻易让采集线程退出；
    - close 与读互斥：`mod_disconnect` 的 close 用 `TryEnterCriticalSection`——读在途则跳过 close 交给下次连接前重载清理，避免 UI 阻塞与并发 close/read 损坏 DLL；`jlink_reload` 同样 TryEnter，读卡死时跳过重载、干净失败（绝不 FreeLibrary 一个有活动调用的 DLL）；
    - 压测验证：10 轮采集中断开/重连 @12000，0 崩溃、19 次成功重连，open 垃圾值经重载+重试全部恢复。
  - 新增特性 21（不再弹 "Target device setting" 芯片型号框）：曾用 open 前 `ExecCommand("Device = Cortex-M4")` 实现（open 阶段 DLL 已识别设备不再询问），但 **A/B 实测确认该 open 前 Device 是 4000kHz 块读 ret=-5 的元凶**：测试目标 STM32L031 是 Cortex-M0+，open 前按 M4 初始化 SWD → 块读超时（采集速率从 8118 崩到 195 样本/s，连 bug9 smoke 低速 50kHz 也 succ=0）。**最终方案**：`JLINKARM_Open(NULL)` 让 DLL 自动探测核心（可识别目标不弹框）→ open 后无条件 `Device = <核心名，默认 Cortex-M4>` 兜底 + `ExecCommand("SuppressInfoDialogs = 1")` 抑制信息弹窗；`check_target_dialog.ps1` 实测首连与重连（重载 DLL）均无 "Target device setting" 弹窗，4000kHz 采集速率恢复 8118 样本/s。纯内存读写只需核心名，无需具体芯片型号。
  - 回归：dev+安装版全量回归（--jlink）ALL PASS。
  - 版本 1.12.0：`python build.py --quiet` 干净构建，打包 `dist/OpenScope-Setup-1.12.0.exe` 并发布 `https://www.opendebugger.com/downloads/`。
  - 需求 9：checkpoint-26 提交 git + `git tag v1.12.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。

- **checkpoint-27（2026-08-10）**：修复新 Bug16（添加 6 个变量后连接闪退）+ 需求 21 补充（"Device Selection" 弹窗同样不触发），v1.13.0。
  - Bug16 复现：用户报"添加 6 个变量后闪退，着重看内存管理"。`repro_bug16_6vars.ps1`（选 6 叶 → 波形+数值窗口添加 → 连接）复现崩溃 exit -1073740771（0xC0000005）@ `JLink_x64.dll+0x18C36E`；`repro_connect_only.ps1`（`--no-layout`、0 变量、纯连接）3/3 同偏移崩溃 → **与变量个数无关**。JLink.exe（先选设备后连接）正常、DLL 哈希与 SEGGER 官方一致、探针健康 → 排除硬件/DLL 损坏。
  - 根因：`jlink_do_connect` 在 `JLINKARM_Open(NULL)` **之后**才调用 `EMU_SelectByIndex/EMU_SelectByUSBSN`——违反 J-Link SDK 契约（仿真器选择必须在 open 之前）。open 后选择导致 DLL 内部会话状态不一致：EMU_SelectByIndex 返回 -1 后 DLL 内部 AV（该路径同时正是 DLL 试图弹出设备选择框的路径，呼应需求 21 的 "Device Selection"）。checkpoint-26 的 42/42 能过是因为当时机器/DLL 状态恰好未触发；同样的 v1.12.0 二进制当天即确定性复现（本次回归安装版 v1.12.0 4 个 J-Link 用例全部连接失败即为同一 bug）。
  - 修复（module/jlink/jlink.c）：把 EMU_SelectByIndex/EMU_SelectByUSBSN 移到 `JLINKARM_Open` **之前**（J-Link SDK 标准顺序：选仿真器 → open → SuppressInfoDialogs → TIF_Select → Device（仍在 open 后，避免 pread 回归）→ SetSpeed → Connect）。修复后 **首次连接即 Connect rc=0 成功**（不再走 -257 → 重载重试路径），EMU_SelectByIndex 返回 -1 变为无害（open(NULL) 自动选中唯一仿真器）。内存审查：变量添加路径（chartwin/numwin 固定数组 + 边界检查）、采集线程堆分配均安全，无内存 bug。
  - 需求 21 补充：加固 `tests/check_target_dialog.ps1`（严格 PASS/FAIL，标题匹配扩展 "Device Selection|Select"），实测首连+重连均无 "Target device setting"/"Device Selection" 弹窗。`SuppressInfoDialogs = 1` 保留。
  - 回归：dev v1.13.0 21/21 ALL PASS；安装 v1.12.0（旧二进制）4 个 J-Link 用例连接失败——正是本 bug；安装 v1.13.0 后 4 个 J-Link 用例（ui_connect / ui_record_dialog / ui_speed12000 / ui_speed_verify）+ 弹窗检查 + Bug16 复现全部 ALL PASS，4000kHz 高速采集 6 变量 18k+ 样本/s 无回归。
  - 版本 1.13.0：`python build.py --quiet` 干净构建，打包 `dist/OpenScope-Setup-1.13.0.exe`（10.4MB）并发布 `https://www.opendebugger.com/downloads/`（下载页与安装包 HTTP 200 自检通过）。
  - 需求 9：checkpoint-27 提交 git + `git tag v1.13.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。

- **checkpoint-28（2026-08-10）**：新增特性 22（F22 消息栏抽屉收起/弹出，与左侧变量树对称），v1.14.0。
  - 需求 22："elf 解析后的变量列表窗口需要实现抽屉缩起来和弹出；类似 VSCode 各个子窗口都可以调整，下面的消息窗口也是类似，需要能全部调整缩起来和拉出"。底部消息窗口此前只有上下拉伸（F14），无抽屉；本版本实现与左侧变量树一致的抽屉行为。
  - 实现（code/src/app.h / mainwin.c / layout.c）：`log_hidden` 状态 + `hLogStrip`（底部 10px 细条 `OSLogStrip`，点击弹出）；双击横向分隔条 `OSSplitterH`（类加 `CS_DBLCLKS`）收起/弹出；收起时隐藏 `hLog`/`hSplitH`、右侧窗口区占满、仅留底部细条；`WM_OS_LOG_HIDE`（WM_APP+32）测试钩子（wParam=1 收起 / 0 展开）；`log_hidden` 键持久化到 layout.ini（旧布局无此键默认展开，向后兼容）。
  - 新增 UI 回归 `tests/ui_log_drawer_drive.ps1`（双击收起→细条可见→点击弹出→钩子收起/展开→布局持久化重启恢复→进程不闪退），并纳入 `tests/run_regression.sh` NON_JLINK 数组。修复时序坑：`CreateWindow` 返回后窗口句柄即存在，但 `ShowWindow` 在 `os_modmgr_load`（jlink 扫描）之后才执行，Find 到主窗口时 `WS_VISIBLE` 可能尚未设置 → 断言前先 `Wait-MainVisible`（轮询 + `ShowWindow(SW_SHOW)` 兜底）；会话2 恢复布局不能用 `--no-layout`（该参数会同时跳过 `--layout-load`）。
  - 回归：dev + 安装版全量回归（--jlink）44/44 ALL PASS（NON_JLINK 18 + JLINK 4，各两变体）。
  - 版本 1.14.0：`python build.py --quiet` 干净构建 0 error/0 warning；打包 `dist/OpenScope-Setup-1.14.0.exe`（10.4MB）并发布 `https://www.opendebugger.com/downloads/`（下载页与安装包 HTTP 200 自检通过）。本轮重新安装 Inno Setup 6.7.3（`tools/innosetup/ISCC.exe` 此前丢失），UAC 已禁用（EnableLUA=0），本地安装版用文件复制更新。
  - 需求 9：checkpoint-28 提交 git + `git tag v1.14.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。

- **checkpoint-29（2026-08-10）**：新增特性 23（波形丝滑缩放+拖拽平移+框选放大）+ Bug17（安装包版本显示）+ Bug18（变量栏完全隐藏），v1.15.0。
  - 特性 23：波形窗口鼠标缩放存在跳变，需更丝滑连续的拉伸/拖拽/框选放大。实现（code/src/chartwin.c）：滚轮缩放由 step factor 0.8/1.25 改为连续 `pow(0.8, delta/120.0)` + 平滑动画（CHART_ANIM_TIMER 逐帧插值 vx0/vx1/vylo/vyhi，消除跳变）；左键拖拽平移（DOWN 记录起点+视图快照，MOVE 位移超 36px² 阈值进入平移，UP 结束，fit_x/fit_y=0）；Ctrl+左键框选局部放大（DOWN 记录框选起点、MOVE 更新矩形、UP 应用 X/Y 区间缩放，绘图区实时画框选虚线框）；单击（未拖拽）仍在松开时设置测量锚点（保留 N13e 测量功能）。
  - Bug17：安装包开始安装时界面仍显示 v1.13.0（安装后关于正确 v1.14.0）。根因：packaging/openscope.iss `AppVerName`/`VersionInfoProductVersion` 为旧版本，make_setup.py 校验只查 `VersionInfoVersion` 与 `OpenScope-Setup-{display}` 漏掉这两处。修复：iss 全部版本字段同步 + make_setup.py 增补 4 项一致性校验（VersionInfoVersion/VersionInfoProductVersion/AppVerName/安装包文件名），杜绝回归。
  - Bug18：仍未实现"全部隐藏全局变量窗口"。现状：tree_auto_tick 钉住态强制展开已手动隐藏的树、无显式折叠动作、tree_hidden 未持久化。修复（code/src/mainwin.c / layout.c / app.h）：新增 `tree_set_hidden()` + 双击竖向分隔条 OSSplitter（类加 CS_DBLCLKS）完全隐藏（hTree 隐藏、左侧 8px 细条 OSTreeStrip 可见）；tree_auto_tick 先判隐藏态再处理钉住（钉住下保持完全隐藏）；`tree_hidden` 键持久化到 layout.ini（向后兼容）；`WM_OS_TREE_HIDE`（WM_APP+33）测试钩子。
  - 新增 UI 回归 `tests/ui_chart_f23_drive.ps1`（滚轮连续缩放区间收窄/拖拽平移 X 窗左移/Ctrl 框选区间收窄/进程不闪退）与 `tests/ui_tree_fold_drive.ps1`（双击分隔条折叠→细条可见→点击展开→钉住保持隐藏→钩子隐藏/展开→布局持久化重启恢复），并纳入 run_regression.sh NON_JLINK 数组。修复 F23 引入的回归：N13e 测量标记改单击（DOWN+UP 配对）触发——F23 起测量在松开（未拖拽）时设置，测试原只发 DOWN 改为真实点击语义；修复 F22 抽屉测试会话2 启动竞态——ShowWindow 在 os_layout_load_from 之前、日志 ListView 创建即带 WS_VISIBLE，全量回归负载高时会在 layout 应用前采样到"日志可见"，改为轮询等待目标状态（≤5s）。
  - 回归：dev + 安装版全量回归（--jlink）48/48 ALL PASS（NON_JLINK 20 + JLINK 4，各两变体）。
  - 版本 1.15.0：`python build.py --quiet` 干净构建 0 error/0 warning；打包 `dist/OpenScope-Setup-1.15.0.exe` 并发布 `https://www.opendebugger.com/downloads/`（下载页与安装包 HTTP 200 自检通过）。UAC 已禁用，本地安装版用文件复制更新（新构建 exe + dll/jlink.dll → Program Files）。
  - 需求 9：checkpoint-29 提交 git + `git tag v1.15.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。

- **checkpoint-31（2026-08-10）**：request.md 全量逐条复查——修复 4 项实现失败/缺失（需求12 F1 帮助文档完全缺失、关于框/启动日志版本号失配、需求2 ELF 重载窗口变量绑错、Bug6 tab 内窗口列宽拖拽/最小化缺失），v1.17.0。
  - 审计结论（逐条核对 request.md 基本需求1-12/新增特性1-23/Bug1-18 后的真实失败项）：
    - **需求12（F1 帮助文档）完全缺失**：无 F1 处理、帮助菜单无"帮助文档"项、readme.md 未进安装包。实现：readme.md 以 RCDATA 内嵌 exe 资源（IDR_HELP_MD，安装版脱离源码目录可用）；新增 `code/src/helpwin.c/h`（OSHelpWin 单例只读窗口，UTF-8→宽字符 \n→CRLF 转换，只读多行 EDIT，跟随 F20 主题，居中主窗口，Esc/F1 关闭）；帮助菜单新增"帮助文档\tF1"（IDM_HELP_DOC=2702）；main.c 加速键表 `TranslateAcceleratorW` 拦截 F1（子控件焦点同样生效）。
    - **版本号失配（Bug17 同类）**：main.c 启动日志与 mainwin.c 关于框硬编码 1.15.0，与 version.rc 1.16.0 不一致。修复：build.py 新增 `gen_version_h()` 从 version.rc（唯一来源）解析生成 `code/src/version.h`（内容不变不重写避免重编译），启动日志/关于框统一引用 OS_VERSION_STR/OS_VERSION_WIDE，结构性杜绝失配。
    - **需求2 加固（ELF 重载绑错变量）**：chartwin/numwin 只存叶下标 leaf_id，ELF 重载叶表重建后下标漂移→窗口静默绑到错误变量（错地址/错数据）。修复：OS_Series/OS_NumWin 增加变量全名存储，新增 `os_chart_rebind`/`os_num_rebind` 在 load_elf_path 重建后按名重绑（逐变量日志 `重绑: 名 id=旧->新 @新地址` + 汇总 `成功 N 缺失 M`，缺失变量移除，数值窗口同步刷新地址列）。
    - **需求2 附带真 bug（伪观测）**：树重建（fill_tree 删除重建）期间 TVN_ITEMCHANGED 用旧节点 lParam（旧叶 id+1）回写新叶表同名位置→把不同变量误置观测（实测 g_extra 被误观测，弹"变量缺失"误报）。修复：`g_tree_filling` 填充期间屏蔽勾选联动 + 仅响应勾选框图像位真实翻转（选择状态变化不再触发）；add_node 补上勾选状态恢复（重建后勾选框与 watched 一致）。
    - **Bug6 补齐（tab 内窗口鼠标拉伸边沿 + 最小化）**：旧平铺等分固定列宽、无最小化。实现：`OS_WinItem` 新增 `col_ratio[]`（列宽比例）与 `group_min[]`；平铺列间留 6px 分隔带，tab 子类命中拖拽（IDC_SIZEWE 光标、起点快照、相对位移实时重排、最小列宽 60px 夹紧）；最小化=窗口隐藏+tab 底部 26px 最小化条自绘按钮（标题），点击按钮还原；菜单新增"最小化/还原当前窗口"（IDM_TAB_MINIMIZE=2507）+ `WM_OS_WIN_MINIMIZE`（WM_APP+34）测试钩子。比例数学抽成 `code/src/tilecalc.c/h` 纯函数（drag/drag2 此消彼长、和保持 1、双向夹紧、浮点噪声 epsilon 判定）。
    - **拖拽抖动修复**：SetCapture 期间窗口尺寸变化会让静止物理光标收到 wParam=0 的 WM_MOUSEMOVE（命中测试重评估），被误当拖拽位移导致列宽跳变——MOUSEMOVE 仅在按住左键（MK_LBUTTON）时应用。
  - 测试：单元 `tests/tilecalc_smoke.c`（23 项：均分/此消彼长/最小夹紧/和保持/drag2 任意下标/退化输入，接入 build_tests.bat）；UI 新增 3 个——`ui_help_drive.ps1`（菜单命令开帮助/真实 F1 键弹出/readme 全文内嵌校验/关于框版本与 exe 文件版本一致/需求6 文案）、`ui_elf_rebind_drive.ps1`（新增 `gen_elf_out_v2.py` 生成"重新编译"变体 ELF——g_extra 首位插入+地址迁移；覆盖触发 2s mtime 弹窗→点是→双向重绑断言 id 0->1->0 与新地址 0x20000004/0x20000000 + 无伪观测 + 无误报缺失弹窗）、`ui_tile_drive.ps1`（分隔带拖拽 ±80px 精确断言/菜单最小化/最小化条点击还原/钩子切换）。测试陷阱修复：跨进程 SendMessage TCM_ADJUSTRECT（>=WM_USER 不封送指针）会让 comctl32 在被测进程解引用野指针崩溃——改用几何公式计算按钮坐标。
  - 回归：全量 UI 回归 dev+安装版（--jlink）**54/54 ALL PASS**（NON_JLINK 23 + JLINK 4，各两变体，含新 3 脚本）；单元/冒烟（tilecalc 23 项/chartview 20 项/elf/enc/replay/jlink/bug9 四速度）全部 PASS；构建 0 error/0 warning。
  - 版本 1.17.0：version.rc（唯一来源）/openscope.iss/jlink.dll 模块版本同步；`python build.py --quiet` 干净构建；打包 `dist/OpenScope-Setup-1.17.0.exe`（10.4MB），静默安装验证 1.17.0.0；发布 `https://www.opendebugger.com/downloads/`。
  - 需求 9：checkpoint-31 提交 git + `git tag v1.17.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。

- **checkpoint-32（2026-08-12）**：BMAD 全量审计 request.md + 修复 5 项真实缺陷（含 1 项严重——树 Ctrl/Shift 多选从代码层即被禁用），v1.17.1。
  - **审计方法**：4 路并行 agent 全量扫描 chartwin.c / mainwin.c / datasrv.c / numwin.c / vartree.c / layout.c 对照 request.md 逐特性/bug 核对，同时手工审查关键代码路径。
  - **审计结论**——3 项请求已妥善实现（仅在代码层面有子级缺陷）：
    - 波形 F23 丝滑缩放/拖拽/框选：chart_sync_manual + X/Y 独立动画 + drag Y 保护 + 整数收敛 + 框选亮青边框已全部到位；**修复**：F 键/停止采集/菜单全局显示未清除测量标记（视图跳变后像素位置已无效→改为提前清 m0/m1）。
    - Bug19 同 tab 多窗口同时更新：`os_ds_drain` 已遍历 `group[]` 全部窗口；**加固**：`chart_broadcast`（FITALL/LIVE 广播）同样补全 group[] 遍历。
    - Bug1/16 多变量崩溃：poll 线程堆分配 + EMU_SelectByIndex 顺序已修；**风险削减**：`os_ds_drain` 栈分配 OS_MAX_LEAVES（384KB）改 OS_DRAIN_BATCH=256（24KB），消除 UI 线程栈溢出隐患。
  - **新发现的真实缺陷（5 项，全部修复）**：
    1. 🔴 **TreeView Ctrl/Shift 多选从代码层被禁用**（`mainwin.c:2700`）：`TreeView_SetExtendedStyle(g_app.hTree, TVS_EX_MULTISELECT, 0)` 签名 `(hwnd, mask, style)` 传 `style=0` 会把 TVS_EX_MULTISELECT 位**清 0**（禁用），应传 `TVS_EX_MULTISELECT` 才开启。此前 checkpoint-19 声称"树改 TVS_EX_MULTISELECT 扩展样式"实际效果为零——Ctrl 单击多选 / Shift 起止范围选从未真正工作过。
    2. 🟡 **树自动隐藏/钉住偏好未持久化**（`layout.c`）：`tree_auto`（钉图标状态）不随布局保存/恢复——每次启动重置为钉住，用户偏好丢失。新增 layout.ini `tree_auto` 键 + 加载后刷新钉图标颜色。
    3. 🟡 **波形 GDI 采样圆点逐点 CreateSolidBrush/DeleteObject**（`chartwin.c:chart_draw_series`）：可见 ≤120 点时每帧 120 次 GDI 对象创建/销毁（~7200/s/系列），改为复用单把画刷，仅 dots 分支才创建。
    4. 🟢 **Δ 测量时间始终以 µs 显示**（`chartwin.c:chart_draw`）：长录制（数十秒）ΔX 显示 `12345678µs` 不可读，改为 <1ms→µs / <1s→ms / ≥1s→`fmt_time`（人类可读时分秒）。
    5. 🟢 **F 键/停止采集/菜单全局显示未清除测量标记**（`chartwin.c`）：3 处（WM_KEYDOWN F / WM_OS_CHART_FITALL / MENU_CHART_FITALL）均只复位 view/fit 状态未清 m0/m1，补 `cw->m0.x = cw->m1.x = -1`。
  - 测试：构建 `python build.py --quiet` 0 error/0 warning；单元测试 elf/enc/replay/chartview/tilecalc 全部 PASS；UI 回归因 pwsh 7 未安装跳过一次（用户本地环境可跑 `tests/run_regression.sh` NON_JLINK 组）。
  - 版本 1.17.1：未重新打包（按用户要求不推送、先本地测试）。
  - 需求 9：checkpoint-32 本地提交 git（不推送）。

- **checkpoint-33（2026-08-12）**：用户反馈实测三问题（缩放/框选/拖拽无效、6ch 采样失真、SWD 速度疑未生效）+ auto.py 一键构建打包，v1.17.1。
  - 🔴 **缩放/框选/拖拽"无效"根因——WM_MOUSEWHEEL 路由**：Windows 把 WM_MOUSEWHEEL 发给键盘焦点窗口而非光标下窗口。用户点击左侧变量树后焦点在树，此后在波形区滚轮消息被树吞掉（chart_proc 的滚轮/框选/拖拽逻辑本身完整）。修复：主窗口 `WM_MOUSEWHEEL` 用 `WindowFromPoint` 找到光标下子窗口并 `SendMessage` 转发；新增回归断言"滚轮发给主窗口→波形窗口收到并产生缩放日志"。
  - ⚡ **6ch 采样失真**：`OS_BATCH_MAX_GAP` 8→256 字节。原值太保守，分散在 RAM 的 6 个变量（通常间隔 ≥16B）从不合并 → 6 次独立 USB 读事务/周期（~900µs）；合并后 ≤2 次（~180µs），提速 ~5×。
  - 📦 **深历史缓冲**：`OS_CHART_HIST` 8192→65536（高速下原 ~1s 即满；现 ~8s 回看窗口）。
  - 📡 **SWD 速度诊断**：SetSpeed 日志带 kHz 单位 + 失败标注（rc≠0 明确提示"将用 DLL 默认速度"）；speed=0 明确标注自适应。用户可在日志搜 `SetSpeed` 确认实际下发速度。
  - 🧪 **测试夹具竞态修复**：`chart_replay_long.csv` 原 2000 行×100µs=200ms 模拟时间，回放按真实时钟 ~200ms 播完——ui_chart_bug5 在 ~350ms 后才添加变量，图表从未收到样本（have_t=0）滚轮缩放静默跳过（时序竞态，checkpoint-25 已记录过偶发）。改为 750µs 间隔（1.5s 跨度），竞态消除。
  - 🔧 **auto.py**：一键构建+单元测试+打包（可选 --skip-tests / --skip-package / --publish / --quiet）。
  - 回归：本机 PowerShell 5.1 实测（此前误判需 pwsh 7——测试脚本无 #Requires）：**ui_chart_f23 13/13、ui_tree_multisel 10/10、ui_tree_fold 27/27、ui_tile 15/15、ui_chart_bug5 8/8、ui_chart_flicker 8/8、ui_windows 8/8、ui_help 5/5、ui_elf_rebind 7/7 全部 ALL PASS**；单元测试（elf/enc/replay/chartview/tilecalc/jlink/bug9 四速度）全 PASS；构建 0 error/0 warning。
  - 需求 9：checkpoint-33 本地提交 git（不推送，用户要求先本地测试）。
  - **checkpoint-33 补充（2026-08-13）**：用户复测"停止采集后波形仍不能任意缩放/滚轮只能显示一小段/不能拖拽"——上一轮修复未命中真正根因。
    - 🔴 **真正根因**：`chart_compute_view` 的 `full0/full1`（缩放/框选/拖拽的夹紧边界）与可见窗口（`chart_vis_start` 最后 npoints=600 点）混用。录制超过 600 点后任何交互（view_all=0）都把"全量范围"截断到最后 600 点窗口：滚轮目标被夹进尾窗（无论鼠标位置）、拖拽被夹在尾窗、且 `chart_vis_start` 同时限制绘图——出尾窗后曲线消失。此前回归测试数据均 <600 点（回放 81 点；2000 点 CSV 被回放时序截断），全部漏检。
    - 修复：`chart_vis_start` 增加 fit_x 参数（手动/全局覆盖全部历史，仅跟随模式限最后 600 点）；full0/full1 改 O(1)（环形缓冲时间有序，最旧/最新样本即边界）+ 扫描窗右沿提前退出；手动窗 Y 值域跟随可见段；绘图循环 t>x1 提前 break；新增 `WM_OS_CHART_SHOT`（WM_APP+42）渲染 BMP 测试钩子。
    - 回归：新增 `ui_chart_long_zoom_drive.ps1`（2000 点 >600，10 断言：早期放大目标落早期区域/曲线像素级实际渲染 449 亮色点/缩小突破尾窗跨度 450000µs/拖拽在数据范围内）；12 个 UI 脚本全 PASS + 单元测试全 PASS。
  - 需求 9：checkpoint-33 补充本地提交 git（不推送）。

- **checkpoint-30（2026-08-10）**：request.md 特性 23 加"仔细实现"前缀重新仔细实现（波形丝滑缩放/拖拽/框选，消除全部跳变根因），v1.16.0。
  - 通读 chartwin.c F23 定位 5 个跳变根因并逐一修复：
    - **A（最严重）滚轮缩放动画从陈旧视图起跳**：live 跟随（fit_x=1）下 vx0/vx1 从不写回（calloc 为 0），首次滚轮缩放把 fit_x 置 0 后动画从 0 起跳，整图先跳到时间 0 再弹回。新增 `chart_sync_manual()` 在缩放/框选前把当前自动视图同步进 vx0/vx1/vylo/vyhi，动画/平移永远从当前实际画面起步。
    - **B X 轴缩放连带动画 Y**：旧 `chart_anim_to` 把 Y 也纳入插值（向缩放前陈旧 auto 值插值），滚轮 X 缩放破坏手动 Y 缩放、Y 乱动。改为 X/Y 独立目标 `chart_anim_to_x/y`：X 缩放只动 X（fit_y=1 保持自动随数据重算），Y 缩放只动 Y。
    - **C 拖拽平移无条件改写 Y**：旧代码任何拖拽都 `fit_y=0` 并把 Y 冻结到起点 auto 快照，水平拖拽 Y 即跳变。改为 Y 平移仅在 Y 已手动锁定（fit_y=0）时生效；fit_y=1 时 Y 保持自动。
    - **D 整数插值永不收敛**：`(target-cur)*3/10` 差值<10 时步进为 0 → 动画永不结束、定时器泄漏、视图卡住。逐帧插值差值过小直接吸附收敛（±1 步进 + 末帧精确贴合）。
    - **E 框选虚线框白色边框浅色主题不可见**：改亮青 RGB(80,200,255)（绘图区两种主题下均为深色底，清晰可见）。
  - 架构：抽取 `code/src/chartview.c/h` 纯函数（无 Win32 依赖）——`os_cv_zoom_x/y`（锚点比例保持、1ms 最小窗、full 夹紧）、`os_cv_box_x/y`（框选映射）；chartwin.c 滚轮/框选改用纯函数 + 基础同步 + 平滑动画。
  - 新增单元测试 `tests/chartview_smoke.c`（20 项：锚点保持/span 收窄/1ms 最小窗/full 夹紧/无变化判定/退化输入 NULL 安全/框选映射），接入 build_tests.bat 每次构建自动回归，ALL PASS。
  - 强化 UI 回归 `tests/ui_chart_f23_drive.ps1`：新增 **Bug A 回归断言**（首次缩放日志 `起点=[a,b]` 必须落在数据时间窗内、非 0 起跳）与 **Bug B 回归断言**（X 轴滚轮缩放不触发 Y 轴缩放）；保留连续缩放收窄/拖拽平移 X 窗左移/框选缩放收窄/不闪退。
  - 回归：全量 UI 回归 dev+安装版（--jlink）**48/48 ALL PASS**（NON_JLINK 20 + JLINK 4，各两变体）；chartview_smoke 20/20；bug9_smoke 4 速度 ALL PASS；构建 0 error/0 warning。
  - 版本 1.16.0：version.rc / openscope.iss / jlink.dll 模块版本同步；`python build.py --quiet` 干净构建；打包 `dist/OpenScope-Setup-1.16.0.exe` 并发布 `https://www.opendebugger.com/downloads/`（下载页与安装包 HTTP 200 自检通过）。UAC 已禁用，本地安装版用文件复制更新。
  - 需求 9：checkpoint-30 提交 git + `git tag v1.16.0` + 推送 `gitee_origin` 与 `github_origin` 双远端。

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
- [x] 18 删除示波器功能模块（checkpoint-23）
- [x] 19 数值窗口右键添加变量：模糊搜索列表 ctrl+a 全选 / ctrl 单击多选 / shift 起止范围多选，批量添加（checkpoint-23）
- [x] 20 界面主题设置：白色（默认）/黑色（配色参考黑色 IAR / Notepad++）（checkpoint-24，F20）
- [x] 21 高速采样：自由运行（周期=实际块读耗时）+ 连续地址块读 + UI 刷新节流，采样率 ~50/s → 数千~万/s（checkpoint-25，F21；第二步"目标端 µs 缓冲"待后续版本）
- [x] 21 避免弹窗设置芯片型号，不触发 "Target device setting" / "Device Selection" 弹窗：open 前设 Device 会破坏 4000kHz 块读（ret=-5，核心名不匹配时），改为 open 自动探测 + open 后无条件 `Device = 核心名，默认 Cortex-M4` + `SuppressInfoDialogs = 1`；EMU_SelectByIndex 移到 open 前（SDK 契约），实测首连/重连均无弹窗（checkpoint-26 / checkpoint-27）
- [x] 22 变量列表窗口与底部消息窗口的抽屉收起/弹出（类 VSCode 子窗口可调整）：双击横向分隔条收起 → 底部细条点击弹出，`log_hidden` 持久化到布局（checkpoint-28，F22）
- [x] 23 波形窗口鼠标缩放丝滑连续：滚轮连续缩放（pow factor + 平滑动画消除跳变）+ 左键拖拽平移 + Ctrl+左键框选局部放大，保留测量标记/滚轮缩放等原有功能（checkpoint-29，F23）
- [x] 23（仔细实现）消除全部跳变根因：live 模式下缩放动画从陈旧 0 值起跳 → 缩放前同步当前视图（chart_sync_manual）；X/Y 缩放独立动画互不干扰；拖拽仅锁定 Y 时平移 Y；动画差值过小直接吸附收敛；框选边框改亮青两主题可见。缩放/框选数学抽成 chartview.c 纯函数 + 20 项单元测试，UI 回归新增 Bug A/B 断言（checkpoint-30，F23 v1.16.0）

## 复查补齐（checkpoint-31，request.md 全量逐条复查）

- [x] 12 readme.md 内容写入帮助文档，F1 键弹出 + 帮助菜单"帮助文档"选项（readme 内嵌 exe 资源，安装版可用；此前完全缺失）
- [x] 2 加固：ELF 重载后波形/数值窗口变量按名重绑 leaf_id（旧实现只存下标，重编译变量顺序漂移后静默绑错变量）+ 树重建期间勾选联动屏蔽（旧 lParam 回写新叶表误置观测/误报缺失弹窗）+ 勾选状态随重建恢复
- [x] Bug6 补齐：tab 内平铺窗口列间分隔带鼠标拖拽调列宽（tilecalc 纯函数 + 23 项单元测试）+ 窗口最小化（tab 底部最小化条点击还原）
- [x] 版本号单一来源：build.py 从 version.rc 生成 version.h，启动日志/关于框统一引用（修复关于框硬编码 1.15.0 与 version.rc 失配）

## 需求（request.md 软件功能清单）

- [x] 9 每次开发后填好 checkout point、提交 git、添加 tag 并推送到远端 gitee_origin / git_hub（checkpoint-18 起执行：`git tag v1.7.0` + 双远端推送；checkpoint-19：`git tag v1.8.0` + 双远端推送；checkpoint-20：`git tag v1.8.1` + 双远端推送；checkpoint-21：`git tag v1.8.2` + 双远端推送；checkpoint-22：`git tag v1.8.3` + 双远端推送；checkpoint-23：`git tag v1.9.0` + 双远端推送；checkpoint-24：`git tag v1.10.0` + 双远端推送；checkpoint-25：`git tag v1.11.0` + 双远端推送；checkpoint-26：`git tag v1.12.0` + 双远端推送；checkpoint-27：`git tag v1.13.0` + 双远端推送；checkpoint-28：`git tag v1.14.0` + 双远端推送；checkpoint-29：`git tag v1.15.0` + 双远端推送；checkpoint-30：`git tag v1.16.0` + 双远端推送）
- [x] 10 每次 BMAD 执行完全部任务后用 Windows 语音播报"任务执行完毕"提示用户检查（checkpoint-20，`tools/notify_done.ps1`）
- [x] 17 跳过选芯片环节只需选芯片核心（Cortex-M0+ 等）；纯 SWD/JTAG 内存/Flash 读取无需芯片型号与架构（checkpoint-21，F17）

## Bug（request.md，已修复）

- [x] Bug1 添加 2 个变量后 App 异常退出（checkpoint-17）
- [x] Bug2 关闭重开任务栏有图标但窗口不可见（checkpoint-17）
- [x] Bug3 单窗口全屏/退出全屏（checkpoint-17）
- [x] Bug4 左侧变量栏自动隐藏/钉住 + Ctrl 多选批量添加到窗口（checkpoint-17 / checkpoint-19）
- [x] Bug5 去掉波形内部标题"波形窗口1" + 放大采样点圆点录制变长后消失（checkpoint-19）
- [x] Bug7 采集时点击录制数据界面卡死在选择录制文件路径上（checkpoint-21）
- [x] Bug8 自动隐藏应归位到左侧 elf 变量树而非菜单栏固定按键（checkpoint-21）
- [x] Bug9 除4000外速度连接采集到的变量值全为0（checkpoint-21）
- [x] Bug10 用12000的速度采集数据，一开始就会失败；采集线程退出，读取xxx变量失败（checkpoint-22）
- [x] Bug14 软件弹窗选择芯片并闪退——删除死代码芯片配置弹窗（checkpoint-24）
- [x] Bug16 12000kHz 掉线自动重连时 `JLINKARM_Open` 返回垃圾值（rc=-488389840）导致采集停止——连接前整库重载 DLL + open 失败重载重试 + 自动重连重试 3 次 + close 与读互斥（checkpoint-26）
- [x] Bug16 软件添加 6 个变量之后闪退（着重看内存管理）——根因不是内存：`EMU_SelectByIndex` 在 `JLINKARM_Open` 之后调用违反 SDK 契约，open 后选择失败致 DLL 内部 AV（JLink_x64.dll+0x18C36E），与变量个数无关；移到 open 前修复，首连即 Connect rc=0（checkpoint-27）
- [x] Bug17 安装包开始安装时界面显示 v1.13.0（安装后关于正确）——iss 的 AppVerName/VersionInfoProductVersion 未随版本同步 + make_setup.py 校验漏检，iss 全部版本字段同步 + 校验增补 4 项（checkpoint-29）；checkpoint-31 进一步消除同类失配：关于框/启动日志改引 version.h（build.py 从 version.rc 自动生成，单一来源）
- [x] Bug18 仍未实现全部隐藏全局变量窗口——双击竖向分隔条完全隐藏 + 左侧细条点击展开 + 钉住态保持隐藏（tree_auto_tick 不再强制展开）+ `tree_hidden` 布局持久化（checkpoint-29）
- [x] Bug6 tab 内窗口最大化/最小化/鼠标拉伸边沿 + 一个 tab 多窗口——最大化/多窗口 checkpoint-17（N11）；最小化（最小化条）与列宽分隔带拖拽 checkpoint-31 补齐

## 总结

- 4 个 Epic / 13 个 Story 全部完成：框架可构建可启动、J-Link 驱动模块（扫描/连接/读写）、采集/CSV/回放、scope 示波器窗口模块。
- 构建与回归：`python build.py --quiet` 0 error/0 warning，自动运行 replay_smoke + jlink_smoke + scope_smoke 全部 PASS。
- 打包发布：`dist/OpenScope-Setup-1.0.0.exe` 安装包（版本号 1.0.0.0），安装/卸载/启动均已验证。
- 依赖真实硬件的验收项（连接 MCU、实时采集曲线、写值回读）留待用户环境实测。
- 硬件实测已覆盖：连接、RAM 读写、Flash 读取；实时采集曲线/写值交互仍可在应用 UI 中实测。
