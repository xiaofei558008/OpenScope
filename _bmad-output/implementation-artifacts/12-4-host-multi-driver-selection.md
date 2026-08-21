---
story_key: 12-4-host-multi-driver-selection
epic: 12
story: 12.4
title: 宿主多驱动选择（仿真器下拉 + 设备/速度联动）
status: done
baseline_commit: 0d50915
created: 2026-08-22
---

# Story 12.4: 宿主多驱动选择

Status: done

## Story

As a user, I want 主界面能在 J-Link 与 ST-Link 之间切换仿真器, so that 两种仿真器都可采集/标定。

## Acceptance Criteria

1. `app.h` 增 `drivers[]`/`driver_ctxs[]`/`driver_count`；`module_mgr.c` 收集全部 `OS_CAP_DRIVER`，默认 jlink（回退首个）。
2. 主界面新增"仿真器"下拉（`IDC_CFG_DRIVER`），切换时断开旧连接 → 重指 `g_app.driver` → 重扫设备列表 → 按驱动刷新速度档位（ST-Link 用 `OS_CMD_GET_FREQ` 档位）。
3. 设备扫描/连接/采集/写值消息驱动感知；`datasrv.c` 与写值路径零改动（仍经 `g_app.driver`）。
4. 启动日志确认两驱动均加载且默认 jlink。

## Tasks / Subtasks

- [x] Task 1: app.h + module_mgr.c/h（drivers 收集 + select_driver/count/name/index）
- [x] Task 2: mainwin.c（IDC_CFG_DRIVER 下拉 + cfg_fill_drivers/speeds/emus + cmd_select_driver + CBN_SELCHANGE + 驱动感知消息）
- [x] Task 3: 布局/控件创建 + 启动自检（两模块加载，驱动= jlink）

## Dev Notes

- `g_app.driver` 语义不变（指向当前选中驱动），datasrv/write 8 处引用零改动。

### File List

- code/src/app.h、module_mgr.c、module_mgr.h、mainwin.c
