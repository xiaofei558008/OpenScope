# OpenScope Epics & Stories（v2：request.md 更新版全覆盖）

> BMAD 规划工件。覆盖 request.md 全量需求（含更新后的特性 3/6、新增 8~12、Bug 1~3）。
> 生成日期：2026-08-09。关联 checkpoint-16 基线（v1.5.0，特性 4/5/7 已完成，1~3/6 部分完成）。

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

## 验收风险

- 就地编辑 EDIT 控件必须覆盖 SysTabControl32 标签文本，避免 Z 序遮挡；Enter 提交后需刷新 TCM_INSERTITEM。
- 删除关于按钮需同步 ui_connect_drive.ps1（其按文字找按钮）与工具栏布局数组。
- Bug3 全屏复用现有 group_max 机制即可（最大化即填满 tab），注意 group_max 关闭窗口后的复位。
