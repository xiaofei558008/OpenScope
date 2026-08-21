---
story_key: 12-2-stlink-scan-connect-disconnect-info
epic: 12
story: 12.2
title: ST-Link 扫描 / 连接 / 断开 / 信息
status: done
baseline_commit: 0d50915
created: 2026-08-22
---

# Story 12.2: ST-Link 扫描 / 连接 / 断开 / 信息

Status: done

## Story

As a user, I want ST-Link 能扫描设备、连接目标、断开并读取设备信息, so that 变量采集链路可用。

## Acceptance Criteria

1. `OS_CMD_SCAN` → `getStLinkList` 回填 `OS_DeviceInfo`（serial/board/fw）。
2. `OS_CMD_CONNECT` → `connectStLink`（dbgPort=SWD/JTAG、connectionMode=HOTPLUG、frequency 从 freq.swdFreq[]/jtagFreq[] 就近选档，档位可能降序）。
3. `OS_CMD_DISCONNECT` → `disconnect()`；`OS_CMD_IS_CONNECTED` → `checkDeviceConnection()`。
4. `OS_CMD_GET_INFO` → `getDeviceGeneralInf()`（cpu/name/deviceId）。
5. `OS_CMD_GET_FREQ` → 返回 freq 档位（主界面联动）。

## Tasks / Subtasks

- [x] Task 1: mod_scan / stlink_scan_probes（getStLinkList + 副本到 ctx.probes + deleteInterfaceList）
- [x] Task 2: mod_connect_ex（连接参数覆盖 + pick_freq 就近选档，不假设档位顺序）
- [x] Task 3: mod_disconnect / mod_get_info / mod_get_freq / HALT/GO/RESET
- [x] Task 4: 实机验证 connect rc=0 + get_info 返回 STM32G43x/G44x (CPU:Cortex-M4)

## Dev Notes

- connectStLink 用 by-value `OS_StLinkDcp`（308 字节），MSVC x64 ABI 由编译器处理；实测连接成功。
- 必须 setLoadersPath 指向 `api\lib`（含 FlashLoader/ExternalLoader）才能识别设备。

### File List

- module/stlink/stlink.c
