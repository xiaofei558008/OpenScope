---
stepsCompleted:
  - step-01-validate-prerequisites
  - step-02-design-epics
  - step-03-create-stories
  - step-04-final-validation
inputDocuments:
  - request.md
  - _bmad-output/planning-artifacts/architecture/architecture-rust-rewrite-2026-08-10/ARCHITECTURE-SPINE.md
  - _bmad-output/project-context.md
---

# OpenScope Rust 重写 — Epic 与 Story 拆分

## Overview

需求 12：用 Rust 重写现有 C11 Win32 应用，版本从 2.0.0 起。Rust 版必须复刻 request.md 1~11 + 新增特性 1~21 + Bug 1~16 已修复的**全部功能**（含高速采样、无芯片型号弹窗、内存安全）。本文件把重写分解为可逐个会话交付的 Epic 与 Story。

## Requirements Inventory（合并 C 版已交付功能）

- FR1 主界面：连接/断开/采集/记录/回放按钮、连接参数直接上界面（仿真口/速度/J-Link 设备列表/刷新），无弹窗选芯片。
- FR2 ELF 选择/热加载：动态监控重编译 → 弹窗提示 → 刷新全局变量地址 → 找不到的变量询问忽略。
- FR3 变量解析：全局变量/结构体展开，结合类型获取绝对地址（DWARF）。
- FR4 波形窗口：添加/移除变量（模糊搜索 + ctrl+a/ctrl/shift 多选）、实时曲线、滚轮缩放 X/Y、Ctrl+B 多坐标轴与逐行堆叠、F 全局显示、采样点圆点放大、光标 Δ 测量、悬停数值 HUD。
- FR5 数值窗口：右键添加变量（同多选）、值直接写入（Enter 一次）、变量前勾选实时更新。
- FR6 多窗口/tab：多 tab、tab 内多个波形/数值窗口、最大化/缩放/拉伸、tab 重命名（就地）、tab/空白处右键新建、单窗口全屏。
- FR7 采集：自由运行高速采样（块读合并、无固定 Sleep、UI 节流）、掉线自动重连（整库重载 DLL + 重试）。
- FR8 记录/回放：采集 log 另存 CSV、离线回放数据。
- FR9 布局：关闭时保存布局、可另存/加载布局文件、启动恢复。
- FR10 主题：白色（默认）/黑色（IAR/Notepad++ 参考）。
- FR11 其它：底部消息栏上下拉伸、消息多选复制/清除、左侧变量栏自动隐藏/钉住、删除示波器模块、帮助文档（晶圆上的生物技术开发/版本号/网址）。
- FR12 无弹窗：不触发 "Target device setting"/"Device Selection"（EMU 选择在 open 前、Device 在 open 后、SuppressInfoDialogs=1）。
- FR13 打包发布：Inno Setup 安装包、发布 www.opendebugger.com、git tag + 双远端、语音播报"任务执行完毕"。

## Epic 列表

### Epic R1: Rust 工程基础 + 主框架 + 变量树（checkpoint-28）
cargo workspace 脚手架、windows-rs 主窗口（菜单/工具栏/状态栏/日志栏/左右分栏/tab）、ELF/DWARF 解析（object+gimli）→ 变量树、J-Link 模块 FFI（扫描/连接/块读）、采集线程 + 环形缓冲、基础波形窗口。目标：垂直切片可运行可观测（连上真实 J-Link 读变量、画曲线）。
**FRs:** FR1, FR2, FR3, FR7（连接/读）, FR12
- R1.1 cargo workspace + windows-rs 依赖 + build.py 一键构建（cargo build --release）。
- R1.2 主窗口框架：消息循环、菜单、工具栏按钮、状态栏、底部日志栏、左侧变量树、右侧 tab 区。
- R1.3 ELF/DWARF：加载 .elf、解析全局变量/结构体、绝对地址、热加载提示。
- R1.4 J-Link 模块：扫描设备列表、连接（AD-JLINK 序列）、断开、块读。
- R1.5 采集：AcqThread 自由运行块读 + ring + UI 节流 + 掉线重连。
- R1.6 波形窗口 v1：添加变量、实时曲线、基础缩放。

### Epic R2: 波形/数值窗口全功能（checkpoint-29）
波形窗口完整交互（多选添加/移除、Ctrl+B 多轴/堆叠、光标 Δ、HUD、圆点采样点）、数值窗口（值写入、实时更新勾选）、多窗口/tab 管理（重命名/最大化/全屏/拉伸）、布局保存恢复、主题。
**FRs:** FR4, FR5, FR6, FR9, FR10
- R2.1 波形窗口完整交互。
- R2.2 数值窗口读写。
- R2.3 多窗口/tab 管理 + 布局保存/加载。
- R2.4 主题（白/黑）。

### Epic R3: 记录/回放 + 打磨（checkpoint-30）
CSV 记录/另存、离线回放、消息栏拉伸/多选复制/清除、变量栏自动隐藏/钉住、帮助文档、Bug 类边界场景回归。
**FRs:** FR8, FR11

### Epic R4: 打包发布 v2.0.0（checkpoint-31）
版本 2.0.0、Inno Setup 打包、发布 www.opendebugger.com、git tag + 双远端推送、语音播报。
**FRs:** FR13
