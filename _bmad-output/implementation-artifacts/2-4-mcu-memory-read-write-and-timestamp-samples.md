---
story_key: 2-4-mcu-memory-read-write-and-timestamp-samples
epic: 2
story: 2.4
title: MCU 内存读写与时间戳样本
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 2.4: MCU 内存读写与时间戳样本

Status: done

## Story

As a user,
I want 连接后能按变量绝对地址读取 MCU 内存并写入修改值,
so that 采集和标定真正可用。

## Acceptance Criteria

1. 框架轮询线程对每个 watched 叶子调用 OS_CMD_READ_MEM，返回数据按叶子类型解码为 OS_Sample（ts_us 为读取时刻 Unix 微秒）。
2. 写值路径把用户输入编码后调用 OS_CMD_WRITE_MEM 并生成 written=1 的样本。
3. 读取失败（未连接/超时）返回 OS_ERR_NOT_CONNECTED/OS_ERR_TIMEOUT 且不影响其它叶子。

## Tasks / Subtasks

- [x] Task 1: 实现 OS_CMD_READ_MEM / OS_CMD_WRITE_MEM（EnterCriticalSection 串行化，AD-11）(AC: 1, 2)
- [x] Task 2: 未连接/非法参数返回 OS_ERR_NOT_CONNECTED / OS_ERR_INVALID_ARG (AC: 3)
- [x] Task 3: 冒烟测试覆盖未连接时的 READ/WRITE 返回 NOT_CONNECTED（不崩溃）(AC: 3)

## Dev Notes

- 读取返回值 >=0 视为成功字节数；JLINKARM_ReadMem/WriteMem 失败返回负值映射为 OS_ERR_TIMEOUT。
- **实测（2026-08-08，STM32L432K8U6）**：`JLINKARM_ReadMem` 成功返回 0（非字节数）、`JLINKARM_WriteMem` 返回写入字节数，已按此修正 mod_read/mod_write 契约。RAM 0x20000000 起 8KB 写/读/回读校验通过（0x20002000 起总线错误，实际 RAM 为 8KB）；Flash 0x08000000 起 64KB 读取通过。

### File List

- module/jlink/jlink.c（mod_read / mod_write / mod_command）
