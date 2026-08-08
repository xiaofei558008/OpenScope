---
story_key: 2-1-jlink-driver-module-skeleton
epic: 2
story: 2.1
title: J-Link 驱动模块骨架
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 2.1: J-Link 驱动模块骨架

Status: done

## Story

As a user,
I want 主框架能从 `dll\` 加载 jlink.dll 并识别为驱动模块,
so that 后续连接/读写功能以模块方式接入。

## Acceptance Criteria

1. 构建后 `dll\jlink.dll` 存在，包含 `os_module_get` 导出。
2. 框架启动时加载 jlink.dll，日志显示“已加载模块: jlink v0.1.0”，capabilities 含 `OS_CAP_DRIVER`，框架 `driver` 指向该模块。
3. `OS_CMD_GET_INFO` 返回 name/version/dll_version/connected=0。

## Tasks / Subtasks

- [x] Task 1: 编写 module/jlink/jlink.c + jlink.h（OS_Module 骨架、DllMain、动态绑定 JLink_x64.dll）(AC: 1)
- [x] Task 2: 编写 module/jlink/jlink_dlg.c 配置对话框骨架（接口/速度/设备/扫描列表）(AC: 2)
- [x] Task 3: 修复框架模块目录路径：module_mgr.c `dll_dir` 由 `exe\dll` 改为 `exe\..\..\dll`，否则扫描不到仓库根 `dll\` (AC: 2)
- [x] Task 4: 冒烟测试 jlink_smoke（LoadLibrary + os_module_get + init + GET_INFO）(AC: 3)

## Dev Notes

- 应用此前从未真正加载模块：`os_modmgr_load()` 扫描 `bin\Release\dll`（不存在），现改为仓库根 `dll\`；以进程模块列表验证 `OpenScope.exe` 进程中同时出现 jlink.dll 与 JLink_x64.dll。
- jlink_smoke 输出：`module: jlink v0.1.0 cap=0x1`、`init rc=0`、`get_info rc=0 dll=96600 hw=40000 emu=未连接 connected=0`、`PASS`。

### File List

- module/jlink/jlink.c、module/jlink/jlink.h、module/jlink/jlink_dlg.c
- code/src/module_mgr.c（dll_dir 路径修复）
- module/jlink/tests/jlink_smoke.c、module/jlink/tests/probe_emu.py
