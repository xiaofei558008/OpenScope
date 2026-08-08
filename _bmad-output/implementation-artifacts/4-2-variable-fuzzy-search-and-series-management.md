---
story_key: 4-2-variable-fuzzy-search-and-series-management
epic: 4
story: 4.2
title: 变量快速模糊搜索与系列管理
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 4.2: 变量快速模糊搜索与系列管理

Status: done

## Story

As a user,
I want 在窗口内根据已加载 ELF 模糊搜索变量并添加为系列,
so that 我可以快速选择要观测的变量。

## Acceptance Criteria

1. 在窗口内打开“添加变量”对话框并输入关键字，通过 fw->leaf_find 返回子串不区分大小写匹配列表，选择后加入系列并显示路径。
2. ELF 重载后按名称重新解析 id，找不到的系列置 -1 并标记。

## Tasks / Subtasks

- [x] Task 1: 模块内实现“添加变量”模态对话框（EDIT 实时搜索 + LISTBOX 列表 + 确定/取消），搜索走 g_fw->leaf_find（子串、不区分大小写）(AC: 1)
- [x] Task 2: 系列管理：添加（颜色分配）、图例点击选择、删除系列按钮 (AC: 1)
- [x] Task 3: on_reload 按名称重解析系列 leaf_id，找不到置 -1，图例显示 [missing] (AC: 2)

## Dev Notes

- 对话框为纯代码 Win32 窗口（无 .rc），随模块加载注册类。
- 系列上限 16 条（SCOPE_MAX_SERIES），颜色取 8 色调色板循环。
- 交互流程需用户环境实测；模块级冒烟覆盖创建/销毁与 on_reload 无崩溃。

### File List

- module/scope/scope.c（pick_proc / series 管理 / resolve_ids）
