---
story_key: 4-3-realtime-curve-and-value-writeback
epic: 4
story: 4.3
title: 实时曲线绘制与数值写回
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 4.3: 实时曲线绘制与数值写回

Status: done

## Story

As a user,
I want 窗口能绘制系列变量的实时曲线并支持修改数值,
so that 我能观测波形并进行标定。

## Acceptance Criteria

1. 采集运行且窗口含已添加系列时，每批 OS_Sample 到达后曲线按时间更新（滚动历史缓冲）。
2. 用户在数值图表上触发编辑时调用 os_fw_write_leaf，写值成功后在曲线/数值表体现 written 标记。

## Tasks / Subtasks

- [x] Task 1: on_samples 按 var_id 匹配系列并写入滚动历史环（SCOPE_HIST=1024），InvalidateRect 触发重绘 (AC: 1)
- [x] Task 2: WM_PAINT 双缓冲绘制：网格、Y 自动缩放、彩色折线、图例（名称=最新值）、written 白点标记 (AC: 1)
- [x] Task 3: “写值”按钮/图例双击打开数值对话框，经 g_fw->write_leaf 写回并回显 written 样本 (AC: 2)
- [x] Task 4: scope_smoke 覆盖合成样本派发与 on_reload 无崩溃

## Dev Notes

- 实时曲线依赖采集数据（硬件），本机以模块级冒烟（合成样本）验证数据通路无崩溃；绘图正确性待用户环境实测。
- 数值对话框为模块内纯代码窗口，写值失败弹 MessageBox 显示框架错误。

### File List

- module/scope/scope.c（scope_paint / series_append / val_proc / mod_on_samples）
