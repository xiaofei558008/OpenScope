---
story_key: 12-1-stlink-driver-module-skeleton
epic: 12
story: 12.1
title: ST-Link 驱动模块骨架 + CubeProgrammer_API.dll 动态绑定
status: done
baseline_commit: 0d50915
created: 2026-08-22
---

# Story 12.1: ST-Link 驱动模块骨架 + CubeProgrammer_API.dll 动态绑定

Status: done

## Story

As a user, I want `dll\stlink.dll` 作为驱动模块被框架加载并动态绑定 CubeProgrammer_API.dll, so that ST-Link 仿真器可像 J-Link 一样接入。

## Acceptance Criteria

1. `module/stlink` 编译产出 `dll\stlink.dll`（`OS_CAP_DRIVER`, name=`stlink`, api_version=3, 导出 `os_module_get`）。
2. 动态绑定（`LoadLibrary` + `GetProcAddress`，不链接 `.lib`）：优先 `dll\stlink\`，回退 ST 安装目录 `bin\`/`api\lib`，用 `SetDllDirectoryW` 解决 Qt/OpenSSL 依赖。
3. 绑定后 `setDisplayCallbacks`（logMessage/initProgressBar/loadBar 均非空）+ `setVerbosityLevel(1)` + `setLoadersPath`（否则 connectStLink 报 "Unable to list supported devices"）。
4. 缺 DLL 时 init 仅告警不崩溃。

## Tasks / Subtasks

- [x] Task 1: module/stlink/stlink.h（OS_StLinkApi + 与官方头文件布局一致的 OS_StLinkDcp/Freq/Callbacks，静态断言尺寸 308/104）
- [x] Task 2: module/stlink/stlink.c（os_stlink_bind/unbind + stlink_load_lib + display callbacks + setLoadersPath）
- [x] Task 3: module/stlink/stlink.vcxproj（镜像 jlink，TargetName=stlink，OutDir=dll\）
- [x] Task 4: 冒烟 stlink_smoke（bind + scan + get_info + get_freq）实机 PASS（NUCLEO-G431RB）

## Dev Notes

- `connectStLink` 崩溃根因：displayCallbacks 的 initProgressBar/loadBar 空指针被 DLL 调用 → 补空实现。
- `readMemory(addr, unsigned char** data, size)` 为双重指针（DLL 分配）；跨 CRT free 崩溃 → 绑定 msvcrt.dll 的 `free` 释放（dumpbin /imports 实测 CubeProgrammer_API.dll 依赖 msvcrt.dll）。

### File List

- module/stlink/stlink.h、stlink.c、stlink.vcxproj、tests/stlink_smoke.c
- code/src/module_api.h（+OS_CMD_GET_FREQ、OS_FreqList）
