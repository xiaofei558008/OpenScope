---
story_key: 1-2-fix-elf-dwarf-compile-errors
epic: 1
story: 1.2
title: 修复 ELF/DWARF 解析编译错误
status: review
baseline_commit: 9635f86f9c536cbdb4c263ebdcf67d9987e7d610
created: 2026-08-08
---

# Story 1.2: 修复 ELF/DWARF 解析编译错误

Status: review

## Story

As a developer,
I want elf.c 在 DWARF5（rnglistx 等）与 DW_Unit 结构上编译通过,
so that 新版编译器产出的 .elf 也能解析变量。

## Acceptance Criteria

1. `DW_FORM_rnglistx` 有定义（DWARF5 值 0x23）且 `case` 使用正常。
2. `DW_Unit` 结构增加 `unit_type` 成员，DWARF5 单元头解析不再报 C2039。
3. `python build.py` 全量构建 0 error（Story 1.1 已清除非 elf 错误）。

## Tasks / Subtasks

- [x] Task 1: elf.c 增加 `#define DW_FORM_rnglistx 0x23`（AC: 1）
- [x] Task 2: DW_Unit 增加 `uint8_t unit_type` 成员（AC: 2）
- [x] Task 3: 全量构建验证 0 error（AC: 3）

## Dev Notes

- DWARF5 form 常量：loclistx=0x22、rnglistx=0x23、strx1=0x25（0x24 保留）。
- elf.c 解析单元头时对 version>=5 读取 unit_type，属 DWARF5 必需字段。
- 验收以 `python build.py` 输出为准（0 error 0 warning）。

### File List

- 修改 `code/src/elf.c`

## Dev Agent Record

### Completion Notes List

- 补齐 DW_FORM_rnglistx（0x23），修正 case 表达式非常量错误。
- DW_Unit 增加 unit_type 成员，消除 C2039。
- 全量构建通过：0 error / 0 warning，OpenScope.exe 产出。
