---
story_key: 1-1-unified-build-and-fix-framework-compile-errors
epic: 1
story: 1.1
title: 统一构建环境并修复框架编译错误
status: in-progress
baseline_commit: 9635f86f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 1.1: 统一构建环境并修复框架编译错误

Status: ready-for-dev

## Story

As a developer,
I want 一条命令即可在干净环境下完成构建且全部框架源码编译通过,
so that 后续模块开发有可运行的基线。

## Acceptance Criteria

1. 运行 `python build.py` 即可完成干净环境构建（内部规范化 PATH 大小写，规避 MSB6001）。
2. numwin.c/vartree.c 补齐 `<commctrl.h>` 后不再出现 LVITEMW/HTREEITEM 等未声明错误。
3. ui.c（被 mainwin.c 取代的旧版主窗口+对话框）移出编译范围（移到 `code/src/_agent_extra/`），不再产生任何编译错误。
4. util.c 的 `os_format_raw` 增加 `is_ptr` 参数（util.h + 3 个调用点 datasrv.c×2、datalog.c×1 同步），消除 C2065。
5. module_api.h 的 `struct OS_Variable;` 前向声明不再产生 C2236（随 vartree.c 修复后消失）。
6. 排除 elf.c（Story 1.2 处理）后，其余框架源码编译 0 error。

## Tasks / Subtasks

- [x] Task 1: 新增 `build.py` 干净环境构建包装（AC: 1）
  - [x] 1.1 读取当前环境 PATH/Path，构造只含单一 `Path` 键的环境块
  - [x] 1.2 调用 `cmd /c build.bat`，返回码透传
- [x] Task 2: 修复 numwin.c 头文件（AC: 2）
  - [x] 2.1 增加 `#include <commctrl.h>`
- [x] Task 3: 修复 vartree.c 头文件（AC: 2, 5）
  - [x] 3.1 增加 `#include <commctrl.h>`
  - [x] 3.2 确认 module_api.h C2236 消失
- [x] Task 4: 移除 stale ui.c（AC: 3）
  - [x] 4.1 移动 `code/src/ui.c` 到 `code/src/_agent_extra/ui.c`（保留参考）
  - [x] 4.2 确认无其它文件引用 ui.c 独有符号
- [x] Task 5: os_format_raw 增加 is_ptr（AC: 4）
  - [x] 5.1 更新 `util.h`/`util.c` 签名
  - [x] 5.2 更新 datasrv.c×2、datalog.c×1 调用点传入 `L->is_ptr`
- [x] Task 6: 构建验证（AC: 6）
  - [x] 6.1 `python build.py` 运行，收集剩余错误（应仅剩 elf.c）
  - [x] 6.2 记录构建日志到 `build_last.log`

## Dev Notes

- **架构约束**：模块 ABI（module_api.h）与 app.h 是当前权威接口；ui.c 是旧版实现（MainWndProc/make_menu/layout/os_refresh_tree/ui_init/ui_create_main_window/os_log/os_status），其功能已由 mainwin.c（os_mainwin_*、os_dlg_pick_var、os_fw_pick_variable）与 util.c（os_log）取代。当前框架无任何文件引用 ui.c 独有符号。
- **文件清单（UPDATE）**：`code/src/numwin.c`、`code/src/vartree.c`、`code/src/util.c`、`code/src/util.h`、`code/src/datasrv.c`、`code/src/datalog.c`、`code/src/main.c`（不需要动）。
- **文件清单（MOVE）**：`code/src/ui.c` → `code/src/_agent_extra/ui.c`。
- **测试标准**：本项目无单元测试框架；验收=干净构建 0 error（elf.c 除外，归 Story 1.2）。
- **已知环境事实**：沙箱内 `.git` 只读（提交需提权）；`python build.py` 是唯一推荐的构建入口；构建产物在 `bin/`、`build/obj/`（已被 .gitignore 忽略）。

### Project Structure Notes

- 目录：`code/src/` 框架；`module/<name>/` 模块；`dll/` 产物；`_agent_extra/` 为未编译的 WIP 保留区。
- vcxproj 通过 `src\*.c` 通配编译，因此把 ui.c 移入 `_agent_extra/` 即脱离编译。

### References

- [Source: _bmad-output/project-context.md#Critical-Implementation-Rules]
- [Source: _bmad-output/planning-artifacts/architecture/architecture-OpenScope-2026-08-08/ARCHITECTURE-SPINE.md#AD-9]
- [Source: build_last.log（错误基线）]

## Dev Agent Record

### Agent Model Used

deepseek-v4-pro（主代理执行）

### Debug Log References

- `build_last.log`

### Completion Notes List

- 新增 `build.py`：干净环境构建包装，规避 PATH/Path 大小写重复导致的 MSB6001。
- `numwin.c`/`vartree.c` 补 `<commctrl.h>`；`numwin.c` 修复形参 `h` 与局部变量重定义。
- `ui.c`（旧版主窗口实现，已被 mainwin.c 取代）移入 `_agent_extra/`，脱离编译。
- `os_format_raw` 增加 `is_ptr` 参数（util.h/util.c + datasrv.c×2 + datalog.c×1）。
- **根因修复**：`OS_ConnectCfg.interface` 与 Windows SDK 宏 `interface`（C 模式下 = `struct`）冲突，导致 module_api.h(69) C2236；改名为 `iface`。
- 构建验证：仅剩 elf.c 3 处错误（归 Story 1.2）。

### File List

- 新增 `build.py`
- 移动 `code/src/ui.c` → `code/src/_agent_extra/ui.c`
- 修改 `code/src/numwin.c`、`code/src/vartree.c`、`code/src/util.c`、`code/src/util.h`、`code/src/datasrv.c`、`code/src/datalog.c`、`code/src/module_api.h`
