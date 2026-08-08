---
stepsCompleted:
  - step-01-validate-prerequisites
  - step-02-design-epics
  - step-03-create-stories
  - step-04-final-validation
inputDocuments:
  - request.md
  - _bmad-output/planning-artifacts/architecture/architecture-OpenScope-2026-08-08/ARCHITECTURE-SPINE.md
  - _bmad-output/project-context.md
---

# OpenScope - Epic Breakdown

## Overview

本项目是类 CANape 的 MCU 变量采集/标定工具：Win32 原生 UI + C11 框架，通过动态加载 DLL 模块扩展功能；首个业务模块为 J-Link 驱动（扫描 USB 外设、SWD/JTAG 配置、读写 MCU 变量并带时间戳）。本文件把 `request.md` 需求分解为 4 个独立可交付的 Epic 与 13 个 Story，每个 Story 可被单个开发会话完成。

## Requirements Inventory

### Functional Requirements

- FR1: 提供 Win32 原生 UI 主界面（Visual Studio 原生支持的 UI 工具实现）。
- FR2: UI 支持选择/监控/热加载 .elf：用户重编译后软件立即感知并弹窗提示；刷新全部全局变量地址；找不到的变量弹窗询问是否忽略。
- FR3: 变量解析：全局变量、结构体展开，结合数据类型获取变量绝对地址。
- FR4: UI 提供连接、断开连接、数据采集按键；支持 log 数据、离线回放数据；数据可另存为文件（CSV，MF4 延后）。
- FR5: UI 支持添加新窗口（波形/数值，可由 C 语言编译为独立 DLL 模块）；窗口内支持根据已加载 ELF 快速模糊搜索变量、绘制实时曲线、修改变量值。
- FR6: 模块化开发：一个功能一个 DLL；框架动态加载 .dll 扩展功能；新功能便于单元测试。
- FR7: J-Link 业务模块：扫描 USB 外设发现 J-Link；未发现时弹窗提示；支持选择 SWD/JTAG 与时钟速度；调用 dll/JLink_x64.dll；连接后读写 MCU 变量；数据带读取时刻时间戳。

### NonFunctional Requirements

- NFR1: 全 C 语言开发，保证执行速度与最小系统资源占用。
- NFR2: UI 使用 Microsoft Visual Studio 原生支持的 UI 工具（Win32）。
- NFR3: 编译器位于 C:\Program Files\Microsoft Visual Studio\18\Community。
- NFR4: code 文件夹为软件框架（含 main.c）；module 文件夹为新增功能模块，编译后 DLL 拷贝到 dll 文件夹供框架调用。
- NFR5: J-Link 驱动安装目录为 C:\Program Files\SEGGER\JLink_V966（运行时依赖 dll/JLink_x64.dll）。

### Additional Requirements

- 模块 ABI 冻结：`OS_Module`/`OS_Framework` 结构布局不变（AD-1）。
- 驱动访问只走模块命令通道 `OS_CMD_*`（AD-2）；样本必须带 Unix 微秒时间戳（AD-4）。
- ELF 热加载由宿主统一管理并广播 `on_reload`（AD-5）；依赖方向单向（AD-6）。
- J-Link 用运行时 `GetProcAddress` 动态绑定（AD-7）；记录格式 v1=CSV（AD-8）。
- 构建环境归一：`build.py` 规范化 PATH 后调 `build.bat`（AD-9）。

### UX Design Requirements

无独立 bmad-ux 文档；UI 需求已并入 FR1/FR4/FR5（主窗口按钮、树形变量、波形/数值窗口、连接配置对话框）。

### FR Coverage Map

- FR1: Epic 1（可运行核心框架）
- FR2: Epic 1（ELF 解析/热加载基线）+ Epic 3 依赖
- FR3: Epic 1（变量解析修复）
- FR4: Epic 2（连接/读写）+ Epic 3（采集/CSV/回放）
- FR5: Epic 4（scope 窗口模块）
- FR6: Epic 1（模块框架）+ Epic 2/Epic 4（模块实例）
- FR7: Epic 2（J-Link 驱动模块）

## Epic List

### Epic 1: 可运行的核心框架
恢复并验证 OpenScope 框架可编译、可启动、可加载模块：修复当前全部编译错误（elf.c/ui.c/numwin.c/vartree.c/util.c/module_api.h），统一构建环境，验证 ELF 加载与模块管理基线。
**FRs covered:** FR1, FR2, FR3, FR6

### Epic 2: J-Link 驱动模块（连接并读写 MCU 变量）
实现 request.md 指定的首个业务模块：扫描 J-Link 设备、SWD/JTAG + 时钟配置、动态调用 JLink_x64.dll、连接/断开、读写 MCU 变量并带时间戳。
**FRs covered:** FR4（连接/断开/读写）, FR6, FR7

### Epic 3: 数据采集、记录与离线回放
完成采集控制（开始/停止）、CSV 记录与另存、离线回放，与 J-Link 驱动集成形成"采集→记录→回放"闭环。
**FRs covered:** FR4

### Epic 4: 可扩展窗口模块（scope.dll：实时波形与写值）
以独立 DLL 提供波形窗口模块：变量快速模糊搜索、系列管理、实时曲线绘制、数值修改写回，验证"一个功能一个 DLL"扩展路径。
**FRs covered:** FR5, FR6

## Epic 1: 可运行的核心框架

### Story 1.1: 统一构建环境并修复框架编译错误

As a developer,
I want 一条命令即可在干净环境下完成构建且全部框架源码编译通过,
So that 后续模块开发有可运行的基线。

**Acceptance Criteria:**

**Given** 仓库当前存在 PATH/Path 大小写重复的环境问题（MSB6001）且 ui.c/numwin.c/vartree.c/util.c/module_api.h 存在编译错误
**When** 运行 `python build.py`（或等价的干净环境构建）
**Then** OpenScope 框架工程（code/OpenScope.vcxproj）编译 0 error 0 warning 增量
**And** 修复包括：numwin.c/vartree.c 补 `<commctrl.h>`；ui.c 移除对 `WM_APP_REPLAY_END`/`os_stop_replay`/`os_stop_monitor`/`fw_leaf_find` 等不存在符号的引用；module_api.h 的 `struct OS_Variable;` 前向声明不产生解析错误；util.c `is_ptr` 引用修正
**And** `build.py` 提交到仓库根并写入 project-context.md 约定

### Story 1.2: 修复 ELF/DWARF 解析编译错误

As a developer,
I want elf.c 在 DWARF5（rnglistx 等）与 DW_Unit 结构上编译通过,
So that 新版编译器产出的 .elf 也能解析变量。

**Acceptance Criteria:**

**Given** elf.c 引用 `DW_FORM_rnglistx`（未声明）与 `DW_Unit.unit_type`（结构无此成员）
**When** 修复常量与结构字段/逻辑
**Then** elf.c 编译通过，且 ELF32/ELF64 符号表与 DWARF 类型树解析接口（os_elf_open/var_at/find_vars）签名不变
**And** 用仓库内样例（或构造最小 ELF）验证 os_elf_open 成功返回

### Story 1.3: 应用启动与模块加载自检

As a user,
I want OpenScope.exe 能启动并显示主窗口、日志区输出模块加载结果,
So that 我能确认框架基线可用。

**Acceptance Criteria:**

**Given** 构建产物 bin\Release\OpenScope.exe 存在，dll\ 下放有模块
**When** 启动程序
**Then** 主窗口创建成功（标题 "OpenScope - MCU 变量采集与标定"），日志区显示模块加载结果（jlink/scope 或"未找到驱动模块"警告）
**And** 进程可正常退出无崩溃

## Epic 2: J-Link 驱动模块（连接并读写 MCU 变量）

### Story 2.1: J-Link 驱动模块骨架

As a user,
I want 主框架能从 dll\ 加载 jlink.dll 并识别为驱动模块,
So that 后续连接/读写功能以模块方式接入。

**Acceptance Criteria:**

**Given** module/jlink 下有实现 os_module_get 的源码
**When** 构建并复制到 dll\jlink.dll 后启动框架
**Then** 日志显示"已加载模块 jlink vX"且 `capabilities` 含 OS_CAP_DRIVER，框架 driver 指向该模块
**And** `OS_CMD_GET_INFO` 返回 name/version/dll_version/connected=0

### Story 2.2: J-Link 设备扫描与连接配置对话框

As a user,
I want 点击配置时扫描 USB 上的 J-Link，并可选择 SWD/JTAG 与时钟速度,
So that 我能为当前目标板配置连接参数。

**Acceptance Criteria:**

**Given** 已加载 jlink 驱动模块
**When** 用户触发连接配置（OS_CMD_CONFIGURE）
**Then** 弹出对话框：接口选择（SWD/JTAG）、时钟速度（含 0=自动）、目标设备型号（可编辑）、连接按钮
**And** 执行 OS_CMD_SCAN 后列表显示发现的 J-Link（序列号/名称）；未发现任何设备时弹窗提示"没有发现 JLink 设备"
**And** 确认后把 OS_ConnectCfg 传给 OS_CMD_CONNECT

### Story 2.3: JLink_x64.dll 动态绑定与连接/断开

As a user,
I want 模块能加载 dll\JLink_x64.dll 并完成真实连接/断开,
So that MCU 内存可被访问。

**Acceptance Criteria:**

**Given** dll\JLink_x64.dll 存在且模块已配置
**When** 调用 OS_CMD_CONNECT（SWD/JTAG + 速度 + 设备型号）
**Then** 通过 GetProcAddress 绑定 JLINK_Open/Close/Connect/Disconnect 等导出并连接成功，`connected` 状态为 1，`OS_CMD_IS_CONNECTED` 返回 1
**And** OS_CMD_DISCONNECT 后状态回到 0；无设备/连接失败时返回 OS_ERR_NO_DEVICE 并记录日志

### Story 2.4: MCU 内存读写与时间戳样本

As a user,
I want 连接后能按变量绝对地址读取 MCU 内存并写入修改值,
So that 采集和标定真正可用。

**Acceptance Criteria:**

**Given** 已连接且已加载 ELF（叶子表含绝对地址）
**When** 框架轮询线程对每个 watched 叶子调用 OS_CMD_READ_MEM
**Then** 返回数据按叶子类型解码为 OS_Sample，`ts_us` 为读取时刻 Unix 微秒，且 `address/size/raw` 正确
**And** 写值路径把用户输入编码后调用 OS_CMD_WRITE_MEM 并生成 written=1 的样本
**And** 读取失败（未连接/超时）返回 OS_ERR_NOT_CONNECTED/OS_ERR_TIMEOUT 且不影响其它叶子

## Epic 3: 数据采集、记录与离线回放

### Story 3.1: 采集开始/停止与轮询集成

As a user,
I want 点击采集开始/停止按钮后框架按固定周期轮询所有勾选变量,
So that 我可以控制采样过程。

**Acceptance Criteria:**

**Given** 已连接驱动并加载 ELF
**When** 点击"开始采集"
**Then** 轮询线程启动（间隔默认 20ms，可配置），所有 watched 叶子按周期采样并写入环形缓冲
**And** 点击"停止采集"后线程停止，UI 状态（按钮/状态栏）同步更新

### Story 3.2: CSV 记录与另存

As a user,
I want 采集数据能记录日志并另存为 CSV 文件,
So that 我可以离线分析。

**Acceptance Criteria:**

**Given** 采集运行中
**When** 用户选择保存/另存
**Then** 生成 CSV（表头：时间戳 ISO + 各叶子路径/值），文件可被文本编辑器打开
**And** 停止采集后文件正确关闭，无数据损坏

### Story 3.3: 离线回放

As a user,
I want 选择 CSV 文件后能离线回放采集曲线,
So that 没有硬件时也能查看历史数据。

**Acceptance Criteria:**

**Given** 存在 OpenScope 导出的 CSV
**When** 用户选择回放文件
**Then** 按时间戳顺序向窗口分发样本，状态栏显示"回放中"；回放结束显示"回放完成"并可重新开始

## Epic 4: 可扩展窗口模块（scope.dll）

### Story 4.1: scope.dll 窗口模块骨架

As a user,
I want 主界面"窗口"菜单能看到 scope.dll 提供的"示波器窗口"并能创建,
So that 新窗口类型可经 DLL 扩展。

**Acceptance Criteria:**

**Given** module/scope 实现 os_module_get 且 capabilities 含 OS_CAP_WINDOW
**When** 构建复制到 dll\scope.dll 后启动框架并打开窗口菜单
**Then** 菜单列出"示波器窗口"，点击后创建子窗口并登记到 g_app.wins

### Story 4.2: 变量快速模糊搜索与系列管理

As a user,
I want 在窗口内根据已加载 ELF 模糊搜索变量并添加为系列,
So that 我可以快速选择要观测的变量。

**Acceptance Criteria:**

**Given** 已加载 ELF（叶子表非空）
**When** 在窗口内打开"添加变量"对话框并输入关键字
**Then** 通过 os_fw_leaf_find 返回子串不区分大小写匹配列表，选择后加入系列并显示路径
**And** ELF 重载后按名称重新解析 id，找不到的系列置 -1 并标记

### Story 4.3: 实时曲线绘制与数值写回

As a user,
I want 窗口能绘制系列变量的实时曲线并支持修改数值,
So that 我能观测波形并进行标定。

**Acceptance Criteria:**

**Given** 采集运行中且窗口含已添加系列
**Then** 每批 OS_Sample 到达后曲线按时间更新（滚动历史缓冲 OS_CHART_HIST）
**And** 用户在数值/图表上触发编辑时调用 os_fw_write_leaf，写值成功后在曲线/数值表体现 written 标记

