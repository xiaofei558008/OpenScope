---
story_key: 2-3-jlink-x64-dll-dynamic-bind-and-connect-disconnect
epic: 2
story: 2.3
title: JLink_x64.dll 动态绑定与连接/断开
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 2.3: JLink_x64.dll 动态绑定与连接/断开

Status: done

## Story

As a user,
I want 模块能加载 `dll\JLink_x64.dll` 并完成真实连接/断开,
so that MCU 内存可被访问。

## Acceptance Criteria

1. 通过 GetProcAddress 绑定 JLINKARM_Open/Close/Connect/Disconnect/ExecCommand/EMU_* 等导出。
2. OS_CMD_CONNECT 成功后 `connected` 状态为 1，OS_CMD_IS_CONNECTED 返回 1；OS_CMD_DISCONNECT 后回到 0。
3. 无设备/连接失败时返回 OS_ERR_NO_DEVICE 并记录日志。

## Tasks / Subtasks

- [x] Task 1: 修正绑定符号大小写（JLINKARM_Open 等）并验证 GetProcAddress 全部命中 (AC: 1)
- [x] Task 2: 仅已连接时调用 GetFirmwareString（未连接会崩溃）(AC: 2)
- [x] Task 3: 连接时优先 `JLINKARM_EMU_SelectByUSBSN` / `EMU_SelectByIndex`，失败回退 ExecCommand SelectEmuBySN/Index (AC: 2)
- [x] Task 4: 冒烟验证绑定与状态机（init=0、GET_INFO=0、SCAN=0、is_connected=0、PASS）(AC: 1, 2)

## Dev Notes

- 本机 J-Link PRO 已接入 USB：`JLINKARM_EMU_GetList` 返回 1 台设备，DLL v96600、HW v40000。
- **实测（2026-08-08，STM32L432K8U6）**：SWD @4MHz 连接成功。修复 4 个真实问题：ExecCommand 需 3 参（缺 BufferSize）；接口选择必须用 `JLINKARM_TIF_Select`；器件设置用 `Device = <name>`（`SetDevice`/`SelectInterface` 在本 DLL 返回 Unknown command）；`JLINKARM_GetFirmwareString(char*,int)` 按无参绑定会挂死。

### File List

- module/jlink/jlink.c（os_jlink_bind / jlink_do_connect / mod_disconnect）
- module/jlink/jlink.h
