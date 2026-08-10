---
check_date: '2026-08-10'
project: 'OpenScope Rust 重写 v2.0.0'
status: 'READY'
---

# Rust 重写就绪报告

## 前置检查

| 项 | 状态 | 说明 |
|----|------|------|
| Rust 工具链 | ✅ | stable 1.97.1，`x86_64-pc-windows-msvc`（MSVC link.exe 已具备） |
| windows-rs 可用性 | ✅ | `windows` crate 0.61，crates.io 可拉取 |
| ELF/DWARF crate | ✅ | `object` 0.36 + `gimli` 0.31（DWARF v5/rnglistx 原生支持） |
| libloading | ✅ | 动态加载 JLink_x64.dll（LoadLibrary+GetProcAddress） |
| J-Link DLL | ✅ | `dll/JLink_x64.dll` v96600 在位 |
| 真实硬件 | ✅ | J-Link PRO + STM32L031 目标板（JLink.exe 验证健康） |
| 测试 ELF | ✅ | `tests/linix_stm32l031_v1.2.out` |

## 风险与对策

- **R1 windows-rs API 陌生**：以 C 版代码为逐函数移植蓝本，先用最小可编译窗口打通链路。
- **R2 DWARF 复杂度**：用 `object`+`gimli` 标准库，先解析全局变量符号，结构体展开后续迭代。
- **R3 高速采集时序**：沿用 C 版 A/B 实测结论（块读合并、open 前后 Device/EMU 顺序、SuppressInfoDialogs），不做未经测试的改动。
- **R4 重写规模**：Epic R1 交付"垂直切片"（连真机读变量画曲线），验证可行后再扩展 R2~R4。

## 里程碑

- checkpoint-28：Epic R1（垂直切片可运行）
- checkpoint-29：Epic R2（窗口全功能）
- checkpoint-30：Epic R3（记录/回放/打磨）
- checkpoint-31：Epic R4（v2.0.0 打包发布）
