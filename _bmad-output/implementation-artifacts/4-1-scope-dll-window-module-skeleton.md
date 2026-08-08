---
story_key: 4-1-scope-dll-window-module-skeleton
epic: 4
story: 4.1
title: scope.dll 窗口模块骨架
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 4.1: scope.dll 窗口模块骨架

Status: done

## Story

As a user,
I want 主界面“窗口”菜单能看到 scope.dll 提供的“示波器窗口”并能创建,
so that 新窗口类型可由 DLL 扩展。

## Acceptance Criteria

1. module/scope 实现 os_module_get，capabilities 含 OS_CAP_WINDOW。
2. 构建复制到 dll\scope.dll 后启动框架，打开窗口菜单可见“示波器窗口”，点击后创建子窗口并登记到 g_app.wins。

## Tasks / Subtasks

- [x] Task 1: 编写 module/scope/scope.c（OS_Module、window_types=["scope.bar","示波器窗口"]、create_window/destroy_window）(AC: 1)
- [x] Task 2: 新建 scope.vcxproj（复用 jlink 工程模板，GUID 与解决方案一致）产出 dll\scope.dll (AC: 2)
- [x] Task 3: 全量构建 0 error/0 warning；scope_smoke 冒烟 PASS（加载/能力/窗口类型/建窗/合成样本 no-crash）(AC: 1)
- [x] Task 4: 端到端验证：启动 OpenScope.exe 后向主窗口发送“示波器窗口”菜单命令（WM_COMMAND 2200），成功创建 OSScopeWin 子窗口 (AC: 2)

## Dev Notes

- 进程模块列表确认应用加载 scope.dll；窗口菜单分发由框架 os_mainwin_rebuild_window_menu 自动完成（winmods → window_types）。

### File List

- module/scope/scope.c、module/scope/scope.vcxproj
- module/scope/tests/scope_smoke.c
- tests/build_tests.bat（接入 scope_smoke）
