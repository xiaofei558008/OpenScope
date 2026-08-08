---
project_name: 'OpenScope'
user_name: 'sophia\codexsandboxoffline'
date: '2026-08-08'
sections_completed:
  ['technology_stack', 'language_rules', 'framework_rules', 'testing_rules', 'quality_rules', 'workflow_rules', 'anti_patterns']
status: 'complete'
rule_count: 22
optimized_for_llm: true
---

# Project Context for AI Agents

_This file contains critical rules and patterns that AI agents must follow when implementing code in this project. Focus on unobvious details that agents might otherwise miss._

---

## Technology Stack & Versions

- 语言：C11（MSVC `/std:c11`），纯 C，不用 C++/MFC/WPF；x64（Debug/Release）。
- 编译器/工具链：`C:\Program Files\Microsoft Visual Studio\18\Community`（PlatformToolset v145），MSBuild 18.8，通过 `build.bat`（vcvars64 + msbuild OpenScope.sln）构建。
- UI：Win32 原生（CreateWindowW + 通用控件 comctl32 v6，manifest 已内嵌；TreeView/ListView/工具栏）。
- ELF/DWARF 解析：自研 `code/src/elf.c`（ELF32/64、ARM Cortex-M、DWARF 类型树）。
- J-Link：`dll/JLink_x64.dll`（26 MB，SEGGER JLink V966），运行时动态加载（GetProcAddress，无导入库）；SDK 目录 `C:\Program Files\SEGGER\JLink_V966`。
- 模块系统：每个功能一个 DLL，导出 `os_module_get()` 返回 `const OS_Module*`；ABI 契约在 `code/src/module_api.h`（OS_API_VERSION=1）。
- 构建产物：`bin\Release\OpenScope.exe`；模块 DLL 输出到 `dll\`；中间文件 `build\obj\`。
- 进度/版本管理：git（本仓库 `.git` 对沙箱只读，提交需提权）。

## Critical Implementation Rules

### Language-Specific Rules

- 所有源码必须是 C11，禁止 C++ 语法；头文件用 `#ifndef` 防护。
- 源码为 UTF-8（含中文注释），编译器已加 `/utf-8`；新文件必须保持 UTF-8 无 BOM。
- 只读数据用 `const`；跨线程状态用 `volatile LONG` + `Interlocked*` 或临界区（`g_app.ring_cs`）。
- 错误码统一用 `module_api.h` 的 `OS_ERR_*`；日志级别用 `OS_LOG_*`。

### Framework-Specific Rules（模块 ABI）

- **不得改动 `OS_Module` 结构体字段顺序/类型**（ABI 冻结）；只能追加版本化字段。
- 模块 DLL 编译时必须定义 `OPENSCOPE_MODULE_BUILD`，导出名必须是 `os_module_get`。
- 模块初始化在 `init` 里保存框架回调 `OS_Framework`；ELF 重载后必须实现 `on_reload` 重解析变量名→id（找不到置 -1）。
- 驱动模块必须声明 `OS_CAP_DRIVER`，窗口模块声明 `OS_CAP_WINDOW`；主框架按能力注册。
- `dll/JLink_x64.dll` 是依赖库不是模块：模块加载器必须跳过它（按文件名过滤）。
- 驱动读写走 `OS_CMD_READ_MEM/WRITE_MEM`（`OS_MemReq`），样本必须带 Unix 微秒时间戳 `ts_us`。

### Testing Rules

- 新增解析/编解码逻辑优先做成可独立调用的纯函数（如 `os_decode_value`/`os_parse_text`），便于单元验证。
- 验收以"构建通过 + 关键路径可运行"为准；GUI 流程至少人工/日志验证。

### Code Quality & Style Rules

- 文件组织：`code/src/` 框架源码（main.c + src/*.c）；`module/<name>/` 模块源码；`dll/` 模块产物。
- 命名：类型 `OS_*` 前缀；函数 `os_*`/`os_<模块>_*`；常量全大写下划线。
- 注释用中文，简述意图；头文件注释描述契约。

### Development Workflow Rules

- 构建必须用干净环境（本机存在 `PATH`/`Path` 大小写重复会导致 MSB6001）：用 `python build.py`（内部规范化 `Path` 后调用 build.bat），或手动 `cmd /c "set Path=%Path% & call build.bat"`。
- 每次阶段性成果必须 git 提交（checkpoint），提交信息格式 `checkpoint-N: <说明>`。
- 沙箱内 `.git` 只读：`git add/commit` 需提权执行。
- ELF 文件改动后需触发 `on_elf_reloaded` 广播，模块窗口同步刷新。

### Critical Don't-Miss Rules

- 不要在 `module_api.h` 前向声明处引入 `OS_Variable` 类型冲突（elf.h 已 typedef；注释用 `struct OS_Variable;` 时必须保证标记名可用）。
- Win32 源码要包含 `<commctrl.h>` 才能用 `LVITEMW`/`HTREEITEM`/`WC_LISTVIEWW` 等（numwin.c/vartree.c 曾因此编译失败）。
- 构建环境 `PATH` 大小写重复问题优先用 `build.py` 包装，不要直接改用户注册表 PATH。
- 沙箱无网络：不要依赖在线下载；J-Link 头/库缺失时用 `GetProcAddress` 动态绑定。
- 保持 `request.md` 的需求编号（FR1~FR7）在故事/AC 中可追溯。

---

## Usage Guidelines

**For AI Agents:**

- Read this file before implementing any code
- Follow ALL rules exactly as documented
- When in doubt, prefer the more restrictive option
- Update this file if new patterns emerge

**For Humans:**

- Keep this file lean and focused on agent needs
- Update when technology stack changes
- Review quarterly for outdated rules
- Remove rules that become obvious over time

Last Updated: 2026-08-08
