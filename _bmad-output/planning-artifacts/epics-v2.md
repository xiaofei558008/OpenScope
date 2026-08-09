# OpenScope Epics & Stories（v6：request.md 更新版全覆盖）

> BMAD 规划工件。覆盖 request.md 全量需求（含更新后的特性 3/6、新增 8~13、Bug 1~9、需求 9/10）。
> 生成日期：2026-08-09。关联 checkpoint-20 基线（v1.8.1，需求 10 语音通知已完成）。**本轮 Epic 10：Bug 7/8/9 修复 + 选芯片简化（只需核心名）。checkpoint-21（v1.8.2）目标。**

## 本轮根因结论（BMAD 排查，2026-08-09）

### Bug 9（非4000速度采集值全0）— 根因已实测确认
- J-Link 模块 `mod_read`（jlink.c:314）：`JLINKARM_ReadMem` 失败返回 **1**（正值，SEGGER 约定"未能读取的字节数"），代码用 `r >= 0` 判定成功 → **失败被当成成功，零缓冲被推为有效样本** → UI 显示 0。
- 实测（tests/speedprobe/read_smoke/rawpoll/dropstate）：边缘目标连接在 connect 后约 25ms **掉线**（`IsConnected`→0，后续读返回 rc=1），非默认速度更易掉；`mod_write` 用 `r == size` 判定成功同样错误（成功返回 0）。
- 具体芯片名（STM32F103C8/STM32L031/RP2040_M0 等与目标不符）会让 J-Link 连接**挂起**；通用核心名（Cortex-M4/M0/M0+/M3）在所有速度均稳定连接+读取。
- 修复：`r == 0` 严格成功判定（read/write 两处）+ 掉线自动重连恢复 + 设备列表简化为核心名。

### Bug 7（采集时录制对话框卡死）— 根因
- `cmd_log_start` 在采集运行中直接 `GetSaveFileNameW`（模态对话框），**未像 `cmd_replay_open` 一样先停采集**；采集线程在读取失败（Bug 9 场景）时经 `os_log → os_mainwin_append_log` 跨线程 `SendMessage(ListView_InsertItem)` 到主线程控件，与模态对话框互相等待 → 卡死。
- 修复：录制前停采集、选完路径后恢复（与回放对话框一致）；日志回调改为主线程安全（跨线程时 PostMessage WM_OS_LOG）。

### Bug 8（自动隐藏归位到左侧树面板）— 根因
- 工具栏有固定按钮 `IDC_BTN_PIN`（"钉住变量栏"，菜单栏下方），与树面板顶部 `OSTreePin` 钉图标（N12）重复。用户要求自动隐藏/钉住只属于左侧 elf 变量树。
- 修复：删除工具栏 `IDC_BTN_PIN` 按钮及布局位，保留树钉图标 `OSTreePin` + 左侧细条 `OSTreeStrip` 为唯一入口。

### 用户新需求（选芯片简化：只需核心名）
- 回答：纯 SWD/JTAG 标准协议读写内存/读 Flash 地址内容，**不需要知道具体芯片型号**（仅需通用核心名，CoreSight AHB-AP 调试架构一致）。具体芯片名只在烧录 Flash 算法/特殊连接序列时需要。实测核心名全速稳定、错误芯片名会挂起 → 支持用户方案。

## Requirements Inventory（request.md 全量）

| ID | 需求 | 状态 | 落地点 |
|----|------|------|--------|
| F1 | 布局保存/恢复 | ✅ checkpoint-12 | layout.c |
| F2 | 布局文件另存/导入 | ✅ checkpoint-12 | layout.c |
| F3 | tab 多窗口+重命名；**重命名就地编辑，不弹窗；空名→Default** | 🔄 就地编辑改造 | mainwin.c N3 |
| F4 | 应用图标 | ✅ checkpoint-16 | version.rc |
| F5 | 连接配置嵌入主界面 | ✅ checkpoint-14/15/16 | mainwin.c |
| F6 | 关于/帮助注明组织+版本+网址；**删除关于标签，仅帮助文档** | 🔄 删关于按钮 | mainwin.c N6 |
| F7 | 波形缩放/整体展示/CTRL+B/F | ✅ checkpoint-16 | chartwin.c |
| F8 | SWD/JTAG 速度更多设置或手工写入 | ✅ 实现，待回归 | mainwin.c |
| F9 | 多变量采集恒0修复；变量移除；CTRL+B 坐标轴颜色跟随；左侧变量栏自动隐藏/钉住+分隔条 | ✅ 实现，待回归 | chartwin/numwin/vartree/mainwin |
| F10 | 数值窗口就地写入（Enter 一次）；实时更新勾选框 | ✅ 实现，待回归 | numwin.c |
| F11 | 窗口最大化填满tab+缩放；一个tab多窗口 | ✅ 实现，待回归 | mainwin group |
| F12 | 左侧 elf 窗口钉图标+自动隐藏 | 🔄 已有按钮/细条，需钉图标 | mainwin.c N12 |
| B1 | 添加2变量后异常退出 | ✅ 修复（datasrv 栈→堆），多路回归 | datasrv.c |
| B2 | 关闭重开任务栏有图标但页面不可见 | ✅ 修复（布局屏外回退） | layout.c |
| B3 | 标签页窗口缩放/最大化/最小化未实现；单tab多窗口不支持；**增加单窗口全屏/退出全屏** | 🔄 需补全屏 | mainwin.c N11/Bug3 |
| F13 | 波形分析增强：变量多选、Ctrl+B 逐行堆叠、多轴左置、采样圆点、光标/Δ/HUD | 🆕 checkpoint-18 | mainwin/chartwin |
| R9 | 每次开发后填写 checkpoint、提交 git、**添加 tag 并推送 gitee_origin 与 github_origin** | 🆕 checkpoint-18 | 流程 |
| F14 | 底部消息栏支持上下拉伸，方便调整各区域展示内容 | ✅ checkpoint-19 DONE | mainwin.c |
| F16 | tab 和右侧空白处右键支持新建 tab（波形/数值/示波器窗口） | ✅ checkpoint-19 DONE | mainwin.c |
| B4(补) | 左侧 elf 变量列表：Ctrl 连续选择多变量 + 右键批量添加到窗口（波形/数值/示波器） | ✅ checkpoint-19 DONE | mainwin.c tree |
| B5(补) | 波形窗口内部文字“波形窗口1”去掉；采样点圆点随录制时间增长全部消失需修复 | ✅ checkpoint-19 DONE | chartwin.c |
| R10 | 每次 BMAD 执行完全部任务，用 Windows 发出语音"任务执行完毕"提示用户检查 | ✅ checkpoint-20 DONE | tools/notify_done.ps1 |
| B7 | 采集时点击"记录"，界面卡死在选择录制文件路径对话框 | 🔄 修复 | mainwin.c cmd_log_start |
| B8 | 自动隐藏功能应归位到左侧 elf 变量列表，而非菜单栏下固定按键 | 🔄 修复 | mainwin.c（删 IDC_BTN_PIN） |
| B9 | 除 4000 外速度连接，采集到的变量值全为 0 | 🔄 修复 | jlink.c mod_read/mod_write + mainwin.c g_devices |
| F17 | 跳过选芯片环节：只需选芯片核心（如 Cortex-M0+）；纯内存读写不需知道型号/架构 | 🔄 实现 | mainwin.c g_devices 核心列表 |

## Epic 5：窗口管理完善（N3 就地重命名 + N6 删关于 + Bug3 全屏）

### Story 5.1 — Tab 就地重命名（F3 更新）
- AC：双击/右键 tab → 直接就地编辑（不再弹 OSDlgRename）；编辑中 Enter 提交/Esc 取消；删空全部字符提交 → 名称置 `Default`；`--rename-tab` 钩子保持可用。
- 文件：mainwin.c（tab_rename → 就地 EDIT 覆盖 tab 标签）、mainwin.h。
- 测试：ui_rename_drive.ps1 扩展（就地编辑 + Default 兜底）。

### Story 5.2 — 删除“关于”按钮，仅保留帮助文档入口（F6 更新）
- AC：工具栏“关于”按钮移除；帮助菜单改为“帮助/关于”→ 显示组织“晶圆上的生物技术开发和提供支持”+版本号+网址 www.opendebugger.com；不再有独立 About 标签/按钮。
- 文件：mainwin.c（移除 IDC_BTN_ABOUT 按钮位，保留菜单项改写文案）、version.rc（版本号沿用）。
- 测试：ui_connect_drive.ps1 校验按钮列表不再含“关于”；帮助菜单文案。

### Story 5.3 — 单窗口全屏/退出全屏（Bug3）
- AC：tab 内任意窗口（波形/数值/模块）可最大化填满当前 tab；再次触发还原平铺；双击标题栏或右键菜单“全屏/还原”切换；快捷键（如 F11）可选。
- 文件：mainwin.c（WM_OS_WIN_MAXIMIZE 已有，补双击/F11 入口）、chartwin.c/numwin.c 标题栏。
- 测试：ui_windows_drive.ps1 扩展（双击最大化/还原，group 多窗口平铺）。

## Epic 6：数据面板交互（N12 钉图标 + 回归全绿）

### Story 6.1 — 左侧 elf 变量栏钉图标+自动隐藏（F12）
- AC：变量栏标题区有钉图标按钮（钉住/自动隐藏切换）；未钉住时移出鼠标超时自动隐藏为左侧细条，悬停/点击细条展开；与 N9(d) 现有 tree_auto/hTreeStrip 合并。
- 文件：mainwin.c（布局区加钉按钮）、app.h。
- 测试：新增 ui_tree_pin_drive.ps1（钉住=常显；自动隐藏=移出隐藏/悬停展开）。

### Story 6.2 — 新需求回归收口（F8/F9/F10/F11 确认 + B1/B2 多路验证）
- AC：`ui_chartview`/`ui_n9_watch`/`repro_bug1_dialog|combos`/`ui_bug2_restore` 全部 ALL PASS；速度下拉手工输入生效（atoi 解析）。
- 测试：dev + installed 双跑。

## Sprint 计划

- Sprint-5：Story 5.1 → 5.2 → 5.3（N3/N6/Bug3）
- Sprint-6：Story 6.1 → 6.2（N12 + 全量回归）→ 版本 1.6.0 打包 → checkpoint-17

## Epic 7：波形窗口分析增强（Feature 13，checkpoint-18）

### Story 7.1 — 变量选择对话框多选（F13a） ✅ checkpoint-18 DONE
- AC：右键“添加变量”弹窗列表改为多选（ListView 报表模式）：Ctrl+A 全选、按住 Ctrl 单击多选、按住 Shift 起止范围选（含起止本身）；确定后返回全部选中变量 id，波形/数值窗口一次添加全部。
- 文件：mainwin.c（OSDlgPick 由 LISTBOX → ListView + LVN_KEYDOWN Ctrl+A；`os_dlg_pick_vars` 新 API；`os_dlg_pick_var` 保持单选用作模块回调）、chartwin.c/numwin.c（MENU_ADD 改调多选）。
- 测试：ui_pick_multi_drive.ps1（模糊搜索→ListView 多选→确定→日志“波形窗口添加变量”条数==选中数）。

### Story 7.2 — Ctrl+B 堆叠排列 + 多坐标轴左置（F13b/c） ✅ checkpoint-18 DONE
- AC：Ctrl+B 将全部信号一行一行单独排列（每路独立一行+独立 Y 轴），再按 Ctrl+B 恢复全部叠加排列；多坐标轴（独立 Y 轴）放置到界面左侧；保留菜单“多坐标轴”与日志“波形多坐标轴: %d”。
- 文件：chartwin.c（`multiaxis` → `stacked` 语义：逐行 lane 布局，左侧 Y 轴刻度，行分隔线）。
- 测试：ui_chart_n13_drive.ps1（Ctrl+B 日志切换、堆叠不崩溃）。

### Story 7.3 — 采样点圆点显示（F13d） ✅ checkpoint-18 DONE
- AC：放大到可见采样点较少（≤120）时，每个采样点画实心圆点，便于观察采样间隔与采样时间。
- 文件：chartwin.c（chart_draw 曲线循环中按可见点数画圆）。

### Story 7.4 — 光标 + Δ 测量 + HUD 数值（F13e/f/g） ✅ checkpoint-18 DONE
- AC：鼠标悬停绘图区显示十字光标（XY 向）；同一 X 轴将各 Y 轴曲线数值同时显示（HUD：变量名=值(类型)）；点击两次设置测量锚点，绘制连线并显示 ΔX(us)/ΔY；悬停采样点显示变量名/值/数据类型。
- 文件：chartwin.c（WM_MOUSEMOVE/WM_MOUSELEAVE/WM_LBUTTONDOWN 光标与锚点；chart_draw 画十字+HUD+Δ 读值）。
- 测试：ui_chart_n13_drive.ps1（两次点击→日志“波形测量Δ”）。

### Story 8.1 — checkpoint 管理：git tag + 双远端推送（需求 9） ✅ checkpoint-18 DONE
- AC：每次 checkpoint 完成后：版本号 bump → 打包安装 → 回归 → git 提交并打 tag（`v1.7.0`…）→ 推送 gitee_origin 与 github_origin 两个远端。
- 文件：无（流程）。`git remote -v` 已确认：gitee_origin=git@gitee.com:xiaofei558008/open-scope.git，github_origin=git@github.com:xiaofei558008/OpenScope.git。

## Epic 8：布局交互完善 + 树多选 + 波形显示修复（F14/F16/Bug4补/Bug5补，checkpoint-19）

### Story 8.2 — 消息栏上下拉伸（F14） ✅ checkpoint-19 DONE
- AC：右侧窗口区与底部日志栏之间出现横向分隔条；拖动分隔条上下调整日志栏高度（限制 40~400px）；关闭/启动后布局恢复含 log_h。
- 文件：mainwin.c（新增 hSplitH 横向分隔条 + WM_OS_SPLIT_V 处理，log_h 变更后 layout()）、app.h。
- 测试：ui_logsplit_drive.ps1（拖动分隔条 -> 日志栏尺寸变化，布局保存恢复）。

### Story 8.3 — tab/右侧空白右键新建窗口（F16） ✅ checkpoint-19 DONE
- AC：tab 标签条空白处右键弹出菜单：新建波形窗口/数值窗口/示波器窗口（新 tab）；右侧面板空白处右键同样弹出；入口与窗口菜单复用。
- 文件：mainwin.c（tab_context_menu 空区分支；right_panel_proc 右键转发）。
- 测试：ui_rightmenu_drive.ps1（右键空白 -> 菜单 -> 新建窗口日志）。

### Story 8.4 — 树 Ctrl 多选 + 右键批量添加（Bug4补） ✅ checkpoint-19 DONE
- AC：变量树启用 TVS_MULTISELECT（Ctrl 单击连续多选/Shift 范围）；右键选中多项时“添加到波形/数值/示波器窗口”批量添加全部选中叶变量；右键未选中项则单选该项，右键空白保持多选。
- 文件：mainwin.c（树创建加 TVS_MULTISELECT；tree_context_menu / tree_add_to_native / tree_add_to_scope 遍历 TVGN_NEXTSELECTED）。
- 测试：ui_tree_multisel_drive.ps1（程序化多选 -> 右键批量添加日志条数==选中数）。

### Story 8.5 — 去波形标题 + 圆点消失修复（Bug5补） ✅ checkpoint-19 DONE
- AC：波形窗口内部不再绘制“波形窗口 1”标题文字（保留右上 × 与标题栏点击区域）；采样点圆点按“可见时间窗内实际绘制点数（<=120）”判定而非缓冲区总点数，录制时间变长后放大仍显示圆点。
- 文件：chartwin.c（chart_draw 删除标题 DrawText；chart_draw_series 圆点判定改用可见点数）。
- 测试：ui_chart_n13_drive.ps1 扩展（长时间回放放大后圆点仍显示日志/不闪退）。

## Epic 9：自动完成语音通知（需求 10，checkpoint-20）

### Story 9.1 — 每次全自动开发循环结束播报"任务执行完毕"（R10） ✅ checkpoint-20 DONE
- AC：BMAD 完成全部任务（回归全 PASS + 打包安装 + checkpoint 提交/tag/双远端推送）后，用 Windows TTS 语音播报"任务执行完毕"，提示用户检查；播报脚本可复用、可参数化（文本/语速/音量），在无 .NET System.Speech 环境下回退 SAPI `SpVoice`。
- 文件：tools/notify_done.ps1（新脚本；非应用代码，不打包进 OpenScope.exe）。
- 验证：本机语音引擎可用（System.Speech + SAPI 均 OK，含中文 Huihui）；脚本退出码 0；循环末尾实际播报。
- 说明：需求 10 为 process/notification 型，无 C 应用代码改动；版本 1.8.0→1.8.1 保持 checkpoint↔版本↔tag 三一致，重新打包安装验证版本 1.8.1.0。

## Sprint 计划

- Sprint-7：Story 7.1 → 7.2/7.3 → 7.4（N13a→N13b/c/d→N13e/f/g）
- Sprint-8：全量回归（dev+installed）→ 版本 1.7.0 打包安装 → checkpoint-18 提交 + tag v1.7.0 + 推送双远端
- Sprint-9：Story 8.2 → 8.3 → 8.4 → 8.5（F14 → F16 → Bug4补 → Bug5补）→ 全量回归（dev+installed）→ 版本 1.8.0 打包安装 → checkpoint-19 提交 + tag v1.8.0 + 推送双远端 ✅ checkpoint-19 DONE（14 项回归 dev+installed 全部 ALL PASS；tag v1.8.0 已推送 gitee_origin + github_origin）
- Sprint-10：Story 9.1（需求 10 语音通知）→ 版本 1.8.0→1.8.1 打包安装 → checkpoint-20 提交 + tag v1.8.1 + 推送双远端 → 末尾 Windows 播报"任务执行完毕" ✅ checkpoint-20 DONE（tools/notify_done.ps1 验证播报成功；安装版 1.8.1.0；tag v1.8.1 已推送 gitee_origin + github_origin；末尾已播报"任务执行完毕"）

## Epic 10：Bug 7/8/9 修复 + 选芯片简化（Bug7/8/9 + F17，checkpoint-21）

### Story 10.1 — 采集时录制对话框卡死修复（B7）
- AC：采集运行中点击"记录"弹出文件选择对话框，界面不卡死；对话框关闭后采集自动恢复；记录正常开始。
- 文件：mainwin.c（cmd_log_start 先 `os_ds_stop()` 再 GetSaveFileNameW，对话框关闭后按原状态恢复采集）、app.h（WM_OS_LOG）、mainwin.c os_mainwin_append_log（非主线程时 PostMessage WM_OS_LOG，主线程插入 ListView）。
- 测试：ui_record_dialog_drive.ps1（采集运行 → 发"记录"按钮 → 出现文件对话框 → 关闭 → 采集恢复 + 不崩溃）。

### Story 10.2 — 自动隐藏归位到左侧树面板（B8）
- AC：工具栏不再有"钉住变量栏/自动隐藏"按钮；树面板顶部 `OSTreePin` 钉图标（金色=钉住/灰=自动隐藏）+ 左侧细条 `OSTreeStrip` 为唯一开关；行为与 N9(d) 一致。
- 文件：mainwin.c（g_tool_btns 删 IDC_BTN_PIN、layout items 删 IDC_BTN_PIN、#define 删除、update_pin_button → refresh_tree_pin 仅刷新图标）。
- 测试：ui_features_drive.ps1 既有 N12 钉图标用例保留；新增断言工具栏不再含"钉住变量栏"。

### Story 10.3 — 非4000速度读值为0修复 + 选芯片简化（B9 + F17）
- AC：`JLINKARM_ReadMem`/`WriteMem` 以 `r == 0` 判定成功（修复失败被当作成功推零样本）；读失败且 `IsConnected==0` 时自动重连一次恢复；设备下拉改为核心名列表（Cortex-M0+/M0/M3/M4/M7/M23/M33/A5 等，默认 Cortex-M4）；纯内存读写不再需要具体芯片型号。
- 文件：module/jlink/jlink.c（mod_read/mod_write 严格判定 + 掉线重连）、mainwin.c（g_devices 核心列表）。
- 测试：tests/speed_smoke.c 扩展（核心名全速连接+读取非零）、新增 jlink_retcodes 回归（模块层读返回 rc==0 才成功）。

## 验收风险

- mod_write 原 `r == size` 判定在旧 J-Link 版本可能返回字节数？实测 v96600 WriteMem 失败返回 -1、成功返回 0（SEGGER 手册：>0=未能写入字节数），统一 `r == 0` 安全。
- 自动重连须节流（500ms），避免边缘目标 25ms 掉线导致高频重连刷日志；重连失败仍走 poll_thread fail_count 停止路径。
- 删除工具栏 PIN 按钮需同步 ui_connect_drive.ps1 按钮文字断言（若存在"钉住变量栏"检查）。
- 录制暂停采集的间隙不会录数据（用户选路径期间本来就无新样本），可接受。

## Sprint 计划

- Sprint-11：Story 10.1 → 10.2 → 10.3（B7 → B8 → B9+F17）→ 新增回归（ui_record_dialog / ui_features 钉图标保留 / speed_smoke 全速）→ 全量回归 dev+installed → 版本 1.8.1→1.8.2 打包安装 → checkpoint-21 提交 + tag v1.8.2 + 推送双远端 → 末尾 Windows 播报"任务执行完毕"

## 验收风险
- 删除关于按钮需同步 ui_connect_drive.ps1（其按文字找按钮）与工具栏布局数组。
- Bug3 全屏复用现有 group_max 机制即可（最大化即填满 tab），注意 group_max 关闭窗口后的复位。
- Ctrl+B 语义从“单图多 Y 轴”改为“逐行堆叠”，须保留日志“波形多坐标轴: %d”兼容 ui_chartview_drive.ps1。
- 跨进程无法伪造键盘 Ctrl 状态：Ctrl+A 用 ListView 原生多选 + LVN_KEYDOWN 实现；回归用 LVM_SETITEMSTATE 程序化多选验证多添加链路，Ctrl/Shift 手选为系统标准行为。
- 光标/Δ/HUD 为纯视觉交互；回归通过“波形测量Δ”日志 + 不崩溃验证，视觉人工验收。
