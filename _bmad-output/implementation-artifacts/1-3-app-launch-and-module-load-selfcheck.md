---
story_key: 1-3-app-launch-and-module-load-selfcheck
epic: 1
story: 1.3
title: 应用启动与模块加载自检
status: in-progress
baseline_commit: 9635f86f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 1.3: 应用启动与模块加载自检

Status: in-progress

## Story

As a user,
I want OpenScope.exe 能启动并显示主窗口、日志区输出模块加载结果,
so that 我能确认框架基线可用。

## Acceptance Criteria

1. 进程启动后主窗口创建成功（标题 "OpenScope - MCU 变量采集与标定"），进程稳定运行（不立即崩溃退出）。
2. 日志区输出模块加载结果（当前无模块时输出"未找到驱动模块"警告属预期）。
3. 进程可正常终止无崩溃。

## Tasks / Subtasks

- [x] Task 1: 冒烟启动 OpenScope.exe（AC: 1, 2）
  - [x] 1.1 启动进程，等待数秒
  - [x] 1.2 确认进程存活
- [x] Task 2: 终止进程（AC: 3）

## Dev Notes

- GUI 应用冒烟测试：无交互会话下用 Start-Process + 存活检查 + Stop-Process 完成。
- 无模块时日志会提示"未找到驱动模块"，属当前预期（Epic 2 后消除）。

### File List

- 无源码改动（仅验证）
