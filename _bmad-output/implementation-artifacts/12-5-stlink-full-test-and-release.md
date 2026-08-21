---
story_key: 12-5-stlink-full-test-and-release
epic: 12
story: 12.5
title: STM32G431RB 全量测试 + 回归 + 打包发布
status: done
baseline_commit: 0d50915
created: 2026-08-22
---

# Story 12.5: STM32G431RB 全量测试 + 回归 + 打包发布

Status: done

## Story

As a user, I want ST-Link 全量测试通过并发布, so that 需求 13 完整交付。

## Acceptance Criteria

1. ST-Link 实机全量：扫描（NUCLEO-G431RB）、连接（STM32G43x/G44x, Cortex-M4）、2000 读无泄漏、写/回读/恢复一致、断开。
2. 非硬件回归（elf/dump/enc/replay/chartview/tilecalc）全 PASS；jlink 模块仍加载、默认驱动。
3. 版本 1.18.0 打包；checkpoint-34 提交 + tag v1.18.0 + 推送 gitee_origin/github_origin + 语音"任务执行完毕"。

## Tasks / Subtasks

- [x] Task 1: stlink_smoke + stlink_target_smoke 入 build_tests.bat
- [x] Task 2: 版本 1.17.3 → 1.18.0（version.rc/version.h）
- [x] Task 3: OpenScope.sln 增 stlink 项目 + build.bat 输出
- [x] Task 4: checkpoint-34 提交/tag/双远端推送 + 语音

## Dev Notes

- jlink_smoke（EMU_GetList）/bug9_smoke 需 J-Link 硬件；本机当前仅 ST-Link 在线时，J-Link 相关用例为硬件门控（非本故事引入）。

### File List

- module/stlink/tests/*、tests/build_tests.bat、code/src/version.rc、OpenScope.sln、build.bat
