---
title: 'Rust 重写补齐：ELF 只解析全局变量 + 窗口系统/右键菜单/数值窗口'
type: 'feature'
created: '2026-08-10'
baseline_revision: '81e1749'
status: 'complete'
review_loop_iteration: 1
followup_review_recommended: false
context: []
warnings: [multiple-goals, oversized]
---

<intent-contract>

## Intent

**Problem:** request.md "rust开发 bug" 3-6：ELF 解析把函数名也当变量（应只解析全局变量和结构体展开的全局变量）；菜单窗口的波形/数值窗口置灰无法添加；ELF 树右键无菜单无法把变量加到窗口；Rust 版远未复刻 C 版观测/窗口核心功能。

**Approach:** ① openscope-elf 按符号类型过滤掉函数（STT_FUNC/Text），只保留数据符号；② 实现窗口系统（每个窗口=一个 tab，波形/数值两类子窗口，可添加/移除/重命名/新建），点亮菜单"窗口"并接入右键菜单"添加变量到窗口"；③ 实现数值窗口（值直接写入+实时更新勾选）与波形增强（采样点圆点、滚轮缩放、多坐标轴）；④ 添加变量走模糊搜索弹窗（ctrl+a/ctrl/shift 多选）。

## Boundaries & Constraints

**Always:** 保持窗口类 "OpenScopeMain"/"OpenScopeChart" 注册兼容；采集线程/环形缓冲契约不变；所有新增代码 `cargo build --release` 零 error；回归脚本 tests/ui_rust_elf_drive.ps1 + tests/ui_rust_menu.ps1 继续 ALL PASS（ELF 过滤后 B 部分仅要求树数量>0，兼容）。

**Block If:** 无（全自动执行，不向用户提问）。

**Never:** 不改 JLink 连接序列/FFI 契约；不做 CSV 记录/回放、布局保存/加载、主题切换、消息栏多选（属后续 checkpoint，本 spec 明确排除）；不删除已有功能。

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| HAPPY_PATH | 加载含函数与全局变量的 ELF | 树中只出现数据符号，函数名不出现 | 无错误 |
| ERROR_CASE | 添加变量到窗口时无窗口 | 自动新建一个波形/数值窗口再添加 | 记录日志 |
| HAPPY_PATH | 右键树已勾选多变量 → 添加 | 选中变量进入目标窗口系列 | 无错误 |
| ERROR_CASE | 数值窗口值写入非法数值 | 不写内存，日志提示格式错误 | 保持原值 |

</intent-contract>

## Code Map

- `rust/crates/openscope-elf/src/lib.rs` -- open_elf 收集全部具名定义符号（含函数），需按 `sym.kind()` 过滤为数据符号
- `rust/crates/openscope-app/src/app.rs` -- 主窗口/菜单/树/wndproc；新增窗口注册表与右键菜单处理
- `rust/crates/openscope-app/src/chart.rs` -- 波形子窗口（单窗口），需支持每窗口系列、圆点、多坐标轴、右键移除
- `rust/crates/openscope-app/src/numwin.rs` -- 新增：数值窗口（ListView 勾选实时更新 + 变量名 + 值 + 写入）
- `rust/crates/openscope-app/src/windows.rs` -- 新增：窗口/tab 管理器（每窗口一个 tab，新建/重命名/切换）
- `rust/crates/openscope-app/src/main.rs` -- mod 声明、消息循环（快捷键已接）
- `tests/ui_rust_menu.ps1` -- 回归：A/B/D/E 保持 PASS
- `tests/ui_rust_elf_drive.ps1` -- 回归：保持 ALL PASS
- `tests/ui_rust_windows.ps1` -- 新增：右键菜单/窗口添加/数值窗口回归

## Tasks & Acceptance

**Execution:**
- [ ] `rust/crates/openscope-elf/src/lib.rs` -- open_elf 增加 `SymbolKind::Data/Tls/Common` 过滤，剔除函数与未知段 -- bug3 只显示全局变量
- [ ] `rust/crates/openscope-app/src/windows.rs` -- 新建窗口管理器：每窗口一个 tab 项，子窗口(Chart/Number)随 TCN_SELCHANGE 显示/隐藏；提供 add_window/new_tab/rename_tab -- bug4/6 tab 架构
- [ ] `rust/crates/openscope-app/src/numwin.rs` -- 数值窗口：ListView 列(实时更新✓/变量名/值)，Enter 写值(调 jlink.write_mem)，勾选切换实时更新 -- bug6 数值窗口
- [ ] `rust/crates/openscope-app/src/chart.rs` -- 每窗口独立系列列表；点数少/放大时画圆点；Ctrl+滚轮 Y 缩放、F 全局、Ctrl+B 多坐标轴、右键移除系列 -- bug6 波形增强
- [ ] `rust/crates/openscope-app/src/app.rs` -- 菜单"窗口"项去掉 MF_GRAYED 并接 add_window；树 WM_CONTEXTMENU 弹右键菜单(添加变量到波形/数值窗口、全选)；模糊搜索弹窗(ctrl+a/ctrl/shift 多选) -- bug4/5
- [ ] `rust/crates/openscope-app/src/main.rs` -- 声明新 mod -- 编译入口
- [ ] `tests/ui_rust_windows.ps1` -- 新增确定性回归：右键菜单存在并可添加变量、数值窗口创建 -- bug5 验证

**Acceptance Criteria:**
- Given 加载含函数/变量的 ELF，when 查看变量树，then 树中不出现函数名（bug3）
- Given 菜单 窗口→波形窗口，when 点击，then 新增波形 tab 且不再置灰（bug4）
- Given 树中勾选变量后右键，when 选"添加变量到数值窗口"，then 数值窗口出现该变量且值显示（bug5）
- Given 波形窗口缩放后，when 点数少于阈值，then 采样点以圆点呈现
- Given 数值窗口某行值列输入数字后回车，when 连接中，then 写入 MCU 内存
- Given tests/ui_rust_elf_drive.ps1 与 ui_rust_menu.ps1，when 运行，then 保持 ALL PASS

## Spec Change Log

<!-- Append-only. -->

## Review Triage Log

<!-- Append-only. -->

## Design Notes

窗口模型：每窗口 = 一个 tab（复用 Win32 TabControl）。App 维护 `Vec<OsWindow>`，每项含 kind(Chart/Number)、hwnd、标题、系列名列表。TCN_SELCHANGE 时 ShowWindow 选中窗口、隐藏其余。新增窗口插入新 tab 并选中。添加变量：确定目标窗口（右键时用最近/选中窗口，否则新建）→ 解析变量地址/类型生成系列 → 窗口重绘。数值窗口写入走 openscope_jlink 的 write_mem（若存在该 API；否则在 jlink crate 补 write_mem FFI，照 C 版 jlink.c 的 JLINKARM_WriteMem）。

## Verification

**Commands:**
- `export PATH="/c/Users/Administrator/.cargo/bin:$PATH" && cd D:/OpenScope/rust && cargo build --release` -- expected: 0 error
- `powershell -ExecutionPolicy Bypass -File tests/ui_rust_elf_drive.ps1` -- expected: ALL PASS
- `powershell -ExecutionPolicy Bypass -File tests/ui_rust_menu.ps1` -- expected: ALL PASS
- `powershell -ExecutionPolicy Bypass -File tests/ui_rust_windows.ps1` -- expected: ALL PASS

**Manual checks (if no CLI):**
- 截图验证：菜单窗口项可点、右键菜单弹出、数值窗口显示变量值。
