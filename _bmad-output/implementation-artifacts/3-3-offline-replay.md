---
story_key: 3-3-offline-replay
epic: 3
story: 3.3
title: 离线回放
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 3.3: 离线回放

Status: done

## Story

As a user,
I want 选择 CSV 文件后能离线回放采集曲线,
so that 没有硬件时也能查看历史数据。

## Acceptance Criteria

1. 存在 OpenScope 导出的 CSV 时，选择回放文件后按时间戳顺序向窗口分发样本，状态栏显示“回放中”。
2. 回放结束显示“回放完成”并可重新开始。

## Tasks / Subtasks

- [x] Task 1: 修复回放核心 bug：split_csv 会把 r->pending 就地改写（逗号→NUL），下一 tick 重解析时整行只剩首字段，导致只有第一行数据能产出样本；改为在副本上切分、保留 pending 原文 (AC: 1)
- [x] Task 2: 新增 tests/replay_smoke.c 回归测试（无硬件/无 UI）：合成 CSV + 可控时钟驱动 os_replay_tick，覆盖缺文件、列映射、时序节拍、引号字段、EOF 自动停止、EOF 后 no-op (AC: 1, 2)
- [x] Task 3: 将测试构建接入 build.bat（python build.py 每次全量构建后自动跑 replay_smoke + jlink_smoke）(AC: 1)

## Dev Notes

- replay_smoke 全部 PASS（14 项断言），jlink_smoke PASS，全量构建 0 error/0 warning。
- 回放 UI 流程（“离线回放…”菜单 → GetOpenFileName → os_replay_start + 10ms timer → os_replay_tick）已接线；列名按叶子名映射（重名取首个，已知限制）。

### File List

- code/src/datalog.c（splitbuf 修复）
- tests/replay_smoke.c（新增回归测试）
- tests/build_tests.bat（新增测试构建脚本）
- build.bat（集成测试步骤）
