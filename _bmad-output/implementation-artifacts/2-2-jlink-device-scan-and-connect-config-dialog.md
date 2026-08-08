---
story_key: 2-2-jlink-device-scan-and-connect-config-dialog
epic: 2
story: 2.2
title: J-Link 设备扫描与连接配置对话框
status: done
baseline_commit: 042ba11f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 2.2: J-Link 设备扫描与连接配置对话框

Status: done

## Story

As a user,
I want 点击配置时扫描 USB 上的 J-Link，并可选择 SWD/JTAG 与时钟速度,
so that 我能为当前目标板配置连接参数。

## Acceptance Criteria

1. 配置对话框（OS_CMD_CONFIGURE）提供接口选择（SWD/JTAG）、时钟速度（含 0=自动）、目标设备型号（可编辑）、连接按钮。
2. OS_CMD_SCAN 后列表显示发现的 J-Link（序列号/名称）；未发现任何设备时弹窗提示“没有发现 JLink 设备”。
3. 确认后把 OS_ConnectCfg 传给 OS_CMD_CONNECT。

## Tasks / Subtasks

- [x] Task 1: 修复扫描崩溃：`JLINKARM_EMU_GetDeviceInfo` 旧签名（2 参）与 `JLINKARM_EMU_GetNumDevices`（无参）不符，改为官方支持的 `JLINKARM_EMU_GetList(host, infos, count)` 两段式枚举 (AC: 2)
- [x] Task 2: 定义 264 字节 `JLINKARM_EMU_CONNECT_INFO` 布局并验证（ctypes 探针实测返回 SN=174504925、product=J-Link PRO）(AC: 2)
- [x] Task 3: 冒烟测试验证扫描：`scan rc=0 count=1`、`dev[0] sn=174504925 name=J-Link PRO` (AC: 2)
- [x] Task 4: 配置对话框按 OS_ConnectCfg 保存接口/速度/设备/序列号/探针序号 (AC: 1, 3)

## Dev Notes

- 关键实证：`JLINKARM_EMU_GetNumDevices` 真实签名是 `uint32_t (void)`；`JLINKARM_EMU_GetDeviceInfo` 为旧式 2 参 `(uint32_t iEmu, void* pInfo)`，按 3 参调用会访问违例。pylink 源码（square/pylink）佐证正确做法为 `JLINKARM_EMU_GetList(host, NULL, 0)` 取数量，再 `GetList(host, arr, n)` 填充。
- Host 枚举：USB=1、IP=2、USB_OR_IP=3。
- 对话框与真实连接流程（CONFIGURE→CONNECT）需用户环境实测；本机无目标板，连接路径以 API 返回码/日志为准。

### File List

- module/jlink/jlink.c（mod_scan 重写）、module/jlink/jlink.h（OS_JLinkEmuInfo 重定义）
- module/jlink/jlink_dlg.c
- module/jlink/tests/probe_emu.py（GetList 签名实证）
