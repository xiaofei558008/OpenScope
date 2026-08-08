---
story_key: 3-1-acquisition-start-stop-and-poll-integration
epic: 3
story: 3.1
title: 采集开始/停止与轮询集成
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 3.1: 采集开始/停止与轮询集成

Status: done

## Story

As a user,
I want 点击采集开始/停止按钮后框架按固定周期轮询所有勾选变量,
so that 我可以控制采样过程。

## Acceptance Criteria

1. 点击“开始采集”后轮询线程启动（间隔默认 20ms），所有 watched 叶子按周期采样并写入环形缓冲。
2. 点击“停止采集”后线程停止，UI 状态（按钮/状态栏）同步更新。

## Tasks / Subtasks

- [x] Task 1: 确认主窗口“开始采集/停止采集”命令走 os_ds_start/os_ds_stop（datasrv.c 轮询线程）(AC: 1, 2)
- [x] Task 2: 轮询线程经 OS_CMD_READ_MEM 读取 watched 叶子，os_ds_push_batch 入环并分发 (AC: 1)
- [x] Task 3: 连接断开时线程退出并回写 OS_ACQ_STOPPED、发 WM_OS_ACQ_STATE (AC: 2)
- [x] Task 4: 清理 6 个编译警告（缺原型/const/未用变量/类型不匹配），恢复 0 error/0 warning

## Dev Notes

- 采集闭环依赖驱动连接（J-Link + 目标板），本机无目标板，以代码路径审查 + 构建 + 启动冒烟验证；真实采集需用户环境实测。
- 按钮使能条件：连接后且 watch_count>0 才能开始采集（mainwin.c）。

### File List

- code/src/datasrv.c（poll_thread / os_ds_start / os_ds_stop / os_ds_push_batch）
- code/src/mainwin.c（命令接线）
- code/src/mainwin.h（补 os_mainwin_register 等原型）
