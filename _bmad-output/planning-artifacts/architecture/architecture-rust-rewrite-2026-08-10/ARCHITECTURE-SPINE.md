# OpenScope Rust 重写 — 架构主干（ARCHITECTURE SPINE）

- 日期：2026-08-10
- 状态：规划完成（READY）
- 目标：用 Rust 重写现有 C11 Win32 应用（类 CANape MCU 变量采集/标定工具），版本从 2.0.0 起。
- 依据：request.md 需求 12（Rust 重写）+ 既有全部功能需求/特性/Bug 修复清单（1~11 + 新增特性 1~21 + Bug 1~16）。

## AD-R1 技术选型

- 语言：Rust（stable 1.97，`x86_64-pc-windows-msvc`）。内存安全消除 C 版反复出现的越界/野指针/悬垂类崩溃（Bug1/4/16）。
- GUI：`windows` crate（windows-rs）直接调用 Win32 —— 原生控件/自定义绘制，无外部运行时依赖，最贴近原 C 版行为与"最小资源占用"。
- 模块化：Cargo workspace。每个功能一个 crate，核心框架 crate 通过 trait 接口调用模块（替代 C 版 DLL + `os_module_get` ABI；J-Link 仍 FFI 动态加载 `JLink_x64.dll`）。
- ELF/DWARF：`object` crate（ELF 解析/节/符号）+ `gimli` crate（DWARF v4/v5 类型树、DW_FORM_rnglistx）——比手写解析器更健壮。
- J-Link 动态库：`libloading` crate（LoadLibrary+GetProcAddress），沿用 C 版连接序列结论（AD-JLINK）。

## AD-R2 工程布局（cargo workspace）

```
rust/
  Cargo.toml                    # workspace
  crates/
    openscope-core/             # 主框架：Win32 消息循环、主窗口、窗口/标签管理、模块注册表
    openscope-elf/              # ELF/DWARF 解析 → 变量树模型（全局/结构体展开 + 绝对地址）
    openscope-jlink/            # J-Link 驱动（FFI JLink_x64.dll）：扫描/连接/块读/写
    openscope-acq/              # 采集：自由运行高速采样 + 连续地址块读 + 环形缓冲 + UI 节流
    openscope-ui/               # 波形/数值窗口（原生 Win32 子窗口 + 自定义绘制）、布局、主题
    openscope-app/              # 主程序二进制（装配 + 入口）
  build.py                      # 一键构建（规范化 PATH，调 cargo build --release）
  packaging/openscope.iss       # Inno Setup 打包 v2.x
```

依赖方向单向：app → core → ui/acq/jlink/elf；core 不依赖具体模块。

## AD-R3 核心数据模型

- `Leaf`（变量）：id、name、类型、地址、size、kind/is_signed/is_ptr/bitfield/enum 元数据（对应 C 版 `OS_Leaf`）。
- `Sample`（样本）：ts_us（Unix 微秒）、var_id、value（f64 或 raw）、text 格式化字符串（对应 `OS_Sample`）。
- `Ring`：有界环形缓冲 `OS_RING_CAP=8192`（`ArrayVec`/`VecDeque` + 互斥锁）。
- 窗口模型：`ChartWin`/`NumWin`，系列固定上限（16 路，每路 8192 点），与 C 版常量一致，避免动态增长导致的内存问题。

## AD-R4 采集路径（移植 C 版 F21/高速采样结论）

1. `AcqThread`（`std::thread`）自由运行：`connected` 检查 → 观测叶按地址排序 → 连续区间合并为一次块读（`JLINKARM_ReadMem`）→ 按偏移切片成样本 → 推入 ring。
2. 去掉固定 Sleep；UI 刷新限频（`WM_OS_SAMPLES` 类消息 30~60Hz）。
3. 掉线自动重连：整库重载 DLL（FreeLibrary+LoadLibrary）+ 3×1.5s 重试 + 连接节流（对照 C 版 Bug16 checkpoint-26）。

## AD-JLINK 连接序列（不可违背，源自 A/B 实测）

- 仿真器选择 `EMU_SelectByIndex/USBSN` 必须在 `JLINKARM_Open` **之前**（SDK 契约，checkpoint-27 Bug16 根因）。
- `Device = <核心名>` 必须在 open **之后**设置（open 前设会破坏 4000kHz 块读 ret=-5，checkpoint-26 回归）。
- open 成功后 `ExecCommand("SuppressInfoDialogs = 1")` 抑制 "Target device setting"/"Device Selection" 弹窗（需求 21）。

## AD-R5 生命周期与线程

- 主线程：Win32 消息泵（`GetMessageW`/`DispatchMessageW`）。
- 采集线程：`std::thread`，通过 `PostMessageW` 通知 UI 节流刷新；与 J-Link 调用的互斥用 `Mutex`/临界区等价物。
- 关闭流程：置停止标志 → 采集线程 join → 保存布局 → 销毁窗口。

## AD-R6 打包与发布

- 版本号唯一来源：`rust/crates/openscope-app/build.rs` 或 Cargo.toml `version = "2.0.0"`；打包读取一致。
- Inno Setup 产出 `dist/OpenScope-Setup-2.x.x.exe`，发布 `www.opendebugger.com`（D:\OpenDebugger），git tag + 双远端推送。

## 里程碑（Checkpoint 拆分）

- **checkpoint-28（v2.0.0 里程碑）**：workspace 脚手架 + 主窗口框架 + ELF 加载与变量树 + J-Link 连接/块读 + 采集 + 简单波形显示（垂直切片，可运行可观测）。
- **checkpoint-29（v2.0.0）**：补齐波形/数值窗口全部交互（缩放/光标/Δ/堆叠/多选添加）、布局保存恢复、主题、CSV 记录回放、打包发布。
