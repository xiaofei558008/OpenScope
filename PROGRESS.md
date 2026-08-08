# OpenScope 开发进度

本文件记录项目开发进度与检查点（checkpoint）。每次阶段性完成后更新并提交 git，防止终端崩溃/卡死导致进度丢失。

## 检查点

- **checkpoint-0（2026-08-08）**：基线。已有 OpenScope 框架源码（ELF/DWARF 解析、主窗口、波形/数值窗口、数据采集/记录/回放、模块管理器）+ build 脚本 + JLink_x64.dll；安装 BMAD Method v6.10.0（`_bmad` + `.agents/skills`）。
- **checkpoint-1（2026-08-08）**：BMAD 规划完成。产出 `_bmad-output/project-context.md`（22 条规则）、架构 spine（AD-1~AD-11，lint 0 发现，双评审）、`epics.md`（4 Epic/13 Story）、`readiness-report.md`（READY）、`sprint-status.yaml`。
- **checkpoint-2（2026-08-08）**：Epic 1 完成。`build.py` 干净构建入口；修复 numwin/vartree（commctrl.h）、util（is_ptr）、module_api.h（interface 宏冲突→iface）、elf.c（DW_FORM_rnglistx + DW_Unit.unit_type）；ui.c 移入 `_agent_extra`；全量构建 0 error/0 warning；启动冒烟测试通过（主窗口标题正确）。
- **checkpoint-3（2026-08-08）**：Epic 2 完成（J-Link 驱动模块）。
  - 新建 `module/jlink`（jlink.c/h、jlink_dlg.c、tests/jlink_smoke.c、tests/probe_emu.py）；构建产出 `dll\jlink.dll`，0 error/0 warning。
  - 修复扫描崩溃：`JLINKARM_EMU_GetNumDevices` 为无参签名；`EMU_GetDeviceInfo` 旧 API 会访问违例，改用官方 `JLINKARM_EMU_GetList(host, infos, count)` 两段式枚举 + 264 字节 CONNECT_INFO 布局（pylink 源码佐证 + 本机 ctypes 实证）。
  - 冒烟测试 PASS：init=0、GET_INFO=0（dll=96600 hw=40000）、scan count=1（sn=174504925 name=J-Link PRO）、is_connected=0。
  - 修复框架模块加载：module_mgr.c `dll_dir` 误指向 `bin\Release\dll`，改为 `..\..\dll`；进程模块列表验证 OpenScope.exe 已加载 jlink.dll + JLink_x64.dll。
  - Story 2.1~2.4 全部完成；连接/读写 MCU 路径已实现，待用户环境硬件实测。
- **checkpoint-4（2026-08-08）**：Epic 3 完成（采集/记录/回放）。
  - 修复 CSV 记录格式：os_datalog_append 每行只写一个变量 → 宽表（每周期一行、watched 叶子各一列），与表头对齐。
  - 修复回放核心 bug：split_csv 就地改写 pending 行导致后续行无法产出样本；改在副本上切分。
  - 修复 WRITE_MEM 返回契约（jlink 模块成功返回 OS_ERR_OK）与位域掩码 64 位边界。
  - 清理 6 个编译警告，恢复 0 error/0 warning。
  - 新增 tests/replay_smoke.c（14 项断言 ALL PASS）+ tests/build_tests.bat，接入 build.bat 每次构建自动回归（replay_smoke + jlink_smoke 均 PASS）。
  - Story 3.1~3.3 完成；真实采集/记录依赖硬件，待用户环境实测。
- **checkpoint-5（2026-08-08）**：Epic 4 完成（scope.dll 窗口模块）。
  - 新建 module/scope（scope.c + scope.vcxproj）：窗口类型 `scope.bar`（示波器窗口），capabilities=OS_CAP_WINDOW。
  - 功能：添加变量模糊搜索对话框（fw->leaf_find）、系列管理（添加/图例选择/删除）、实时曲线绘制（双缓冲、Y 自动缩放、written 标记）、数值写回（fw->write_leaf）。
  - 全量构建 0 error/0 warning；scope_smoke 冒烟 ALL PASS；端到端验证：启动应用后发送“示波器窗口”菜单命令成功创建 OSScopeWin 子窗口。
  - Story 4.1~4.3 完成；实时曲线/写值交互待用户环境实测。

## 待办（BMAD 规划产出后更新）

- [x] 生成项目上下文 `project-context.md`
- [x] 架构 spine（ARCHITECTURE-SPINE.md）
- [x] Epics / Stories 分解
- [x] 实施就绪检查
- [x] Sprint 计划
- [x] Epic 1：修复框架构建（Story 1.1/1.2/1.3）
- [x] Epic 2：J-Link 驱动模块（Story 2.1~2.4，硬件实测待用户环境）
- [x] Epic 3：采集/记录/回放（Story 3.1~3.3，硬件实测待用户环境）
- [x] Epic 4：scope 窗口模块（Story 4.1~4.3，交互实测待用户环境）

## 总结

- 4 个 Epic / 13 个 Story 全部完成：框架可构建可启动、J-Link 驱动模块（扫描/连接/读写）、采集/CSV/回放、scope 示波器窗口模块。
- 构建与回归：`python build.py --quiet` 0 error/0 warning，自动运行 replay_smoke + jlink_smoke + scope_smoke 全部 PASS。
- 依赖真实硬件的验收项（连接 MCU、实时采集曲线、写值回读）留待用户环境实测。
