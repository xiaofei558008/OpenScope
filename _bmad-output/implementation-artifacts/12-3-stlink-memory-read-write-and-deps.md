---
story_key: 12-3-stlink-memory-read-write-and-deps
epic: 12
story: 12.3
title: ST-Link 内存读写适配 + 依赖加载 + 掉线自愈
status: done
baseline_commit: 0d50915
created: 2026-08-22
---

# Story 12.3: ST-Link 内存读写适配 + 依赖加载 + 掉线自愈

Status: done

## Story

As a user, I want ST-Link 能正确读写 MCU 内存（含批量块读）且不泄漏/不崩溃, so that 高速采集可行。

## Acceptance Criteria

1. `OS_CMD_READ_MEM` → `readMemory` 双重指针适配为调用方缓冲（`OS_MemReq::data`），用 msvcrt `free` 释放 DLL 分配缓冲（实测 2000 次读无泄漏、无崩溃）。
2. `OS_CMD_WRITE_MEM` → `writeMemory`，实机写/回读/恢复一致。
3. 读写 `CRITICAL_SECTION` 互斥（AD-11）；掉线自动重连节流（500ms）+ 时间基停摆（复用 datasrv `OS_POLL_STALL_MS`）。
4. 依赖加载：`SetDllDirectoryW` 指向依赖 DLL 目录，优先 `dll\stlink\` 回退 ST `bin\`。

## Tasks / Subtasks

- [x] Task 1: mod_read（readMemory 双重指针 + msvcrt free）
- [x] Task 2: mod_write（writeMemory）
- [x] Task 3: 掉线重连节流（checkDeviceConnection + 重连）
- [x] Task 4: stlink_target_smoke 实机（200 读 + 写/回读/恢复，CCM SRAM 0x10000000）PASS

## Dev Notes

- 写测试初始用 SRAM 0x20000000/0x20002000 被运行固件竞争（字节 4-7 漂移），改用 CCM SRAM 0x10000000（默认固件不占用）稳定。

### File List

- module/stlink/stlink.c、tests/stlink_target_smoke.c
