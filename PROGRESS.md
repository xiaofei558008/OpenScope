# OpenScope 开发进度

本文件记录项目开发进度与检查点（checkpoint）。每次阶段性完成后更新并提交 git，防止终端崩溃/卡死导致进度丢失。

## 检查点

- **checkpoint-0（2026-08-08）**：基线。已有 OpenScope 框架源码（ELF/DWARF 解析、主窗口、波形/数值窗口、数据采集/记录/回放、模块管理器）+ build 脚本 + JLink_x64.dll；安装 BMAD Method v6.10.0（`_bmad` + `.agents/skills`）。
- **checkpoint-1（2026-08-08）**：BMAD 规划完成。产出 `_bmad-output/project-context.md`（22 条规则）、架构 spine（AD-1~AD-11，lint 0 发现，双镜头评审）、`epics.md`（4 Epic/13 Story）、`readiness-report.md`（READY）、`sprint-status.yaml`。
- **checkpoint-2（2026-08-08）**：Epic 1 完成。`build.py` 干净构建入口；修复 numwin/vartree（commctrl.h）、util（is_ptr）、module_api.h（interface 宏冲突→iface）、elf.c（DW_FORM_rnglistx + DW_Unit.unit_type）；ui.c 移入 _agent_extra；全量构建 0 error/0 warning；启动冒烟测试通过（主窗口标题正确）。

## 待办（BMAD 规划产出后更新）

- [x] 生成项目上下文 `project-context.md`
- [x] 架构 spine（ARCHITECTURE-SPINE.md）
- [x] Epics / Stories 分解
- [x] 实施就绪检查
- [x] Sprint 计划
- [x] Epic 1：修复框架构建（Story 1.1/1.2/1.3）
- [ ] Epic 2：J-Link 驱动模块（Story 2.1~2.4）
- [ ] Epic 3：采集/记录/回放
- [ ] Epic 4：scope 窗口模块
