---
story_key: 3-2-csv-log-and-save-as
epic: 3
story: 3.2
title: CSV 记录与另存
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 3.2: CSV 记录与另存

Status: done

## Story

As a user,
I want 采集数据能记录日志并另存为 CSV 文件,
so that 我可以离线分析。

## Acceptance Criteria

1. 生成 CSV（表头：时间戳 ISO + 各叶子路径 值），文件可被文本编辑器打开。
2. 停止采集后文件正确关闭，无数据损坏。

## Tasks / Subtasks

- [x] Task 1: 修复 os_datalog_append 输出格式：原实现每行只写一个变量的值，与多列表头不匹配；改为每轮询周期一行、全部 watched 叶子各一列 (AC: 1)
- [x] Task 2: 表头含 UTF-8 BOM、`timestamp_us,time_iso,<叶子名>...`，值列按叶子顺序对齐 (AC: 1)
- [x] Task 3: 停止记录（os_datalog_stop）flush+close，退出清理 (AC: 2)

## Dev Notes

- 记录动作由 os_ds_push_batch → os_datalog_append 驱动；“记录/停止记录”按钮与“开始 CSV 记录…”菜单已接线（mainwin.c）。
- 真实数据写入依赖采集（硬件），格式正确性以代码审查 + 构建验证；另存对话框为系统 GetSaveFileNameW。

### File List

- code/src/datalog.c（os_datalog_append 宽表重写）
