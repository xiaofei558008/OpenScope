---
title: 'F22 抽屉式收起/弹出：消息窗口（与左侧变量树一致）'
type: 'feature'
created: '2026-08-10'
status: 'draft'
review_loop_iteration: 0
followup_review_recommended: false
context: ['_bmad-output/project-context.md']
warnings: []
---

<intent-contract>

## Intent

**Problem:** request.md 特性 22 要求"elf 解析后的变量列表窗口需要实现抽屉缩起来和弹出；类似 VSCode 各个子窗口都可以调整，下面的消息窗口也是类似，需要能全部调整缩起来和拉出"。左侧变量树（elf 树）已有自动隐藏/钉住抽屉（`tree_auto`/`tree_hidden` + `OSTreeStrip` 细条 + `OSTreePin`），但底部消息窗口只有上下拉伸（F14），**无法像变量树一样收成细条再点出**。

**Approach:** 给底部消息窗口实现与左侧变量树一致的抽屉行为：
1. 双击横向分隔条 `hSplitH` 或（后续）拖到最底 → 消息栏收成底部细条 `OSLogStrip`
2. 点击底部细条 → 消息栏弹出恢复
3. 收起状态持久化到布局（layout.ini），重启恢复
4. 新增测试钩子 `WM_OS_LOG_HIDE`（wParam=1 收起 / 0 展开），供 UI 回归脚本驱动

## Boundaries & Constraints

**Always:**
- 纯 C11 + Win32，不引入新依赖；消息窗口与变量树视觉/交互一致（主题色）
- `hSplitH` 保留原有拖动拉伸功能；收起仅新增双击触发
- `g_app.log_h` 保留，收起时忽略、展开时恢复
- 布局格式向后兼容（新增 `log_hidden=` 键，旧布局无此键 → 默认展开）
- ABI 冻结：`OS_Module`/`OS_App` 结构体字段只能追加，不能改顺序/类型
- 测试钩子消息编号沿用 `WM_APP+32`（当前占用至 `WM_APP+31`）

**Block If:**
- 无。全程自动化，不弹人工确认。

**Never:**
- 不删除左侧变量树已有抽屉逻辑（已有回归测试依赖）
- 不引入新库/新文件（改动局限在 `code/src/mainwin.c`、`code/src/app.h`、`code/src/layout.c` + 新增回归脚本 `tests/ui_log_drawer_drive.ps1`）
- 不动 `module_api.h` / `OS_Module` ABI

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| HAPPY_PATH | 双击 `hSplitH` | 消息栏收成底部细条 `OSLogStrip`；`hLog` 隐藏；右侧窗口区占满 | 无错误 |
| EXPAND | 点击 `OSLogStrip` | 消息栏弹出恢复；`hLog` 显示、`OSLogStrip` 隐藏 | 无错误 |
| HOOK_COLLAPSE | `WM_OS_LOG_HIDE` wParam=1 | 同 HAPPY_PATH（测试钩子） | 无错误 |
| HOOK_EXPAND | `WM_OS_LOG_HIDE` wParam=0 | 同 EXPAND（测试钩子） | 无错误 |
| RESTORE | layout.ini 含 `log_hidden=1` | 启动时消息栏收起、细条可见 | 无错误 |
| LEGACY | 旧 layout.ini 无 `log_hidden` 键 | 默认展开（保持旧行为） | 无错误 |
| DRAG | 拖动 `hSplitH` 调整高度 | 仍按 F14 拉伸（不受收起影响） | 无错误 |

</intent-contract>

## Code Map

- `code/src/app.h` -- `OS_App` 结构体（追加 `hLogStrip`/`log_hidden` 字段）、`WM_OS_LOG_HIDE` 消息宏
- `code/src/mainwin.c` -- 主窗口布局 `layout()`、横向分隔条 `splith_proc`、窗口创建 `WM_CREATE`、消息处理 `os_mainwin_proc`、类注册 `os_mainwin_register`
- `code/src/layout.c` -- 布局保存/恢复（新增 `log_hidden` 键）
- `tests/ui_log_drawer_drive.ps1` -- 新增 UI 回归：收起/弹出/细条可见性/进程不闪退
- `tests/run_regression.sh` -- 纳入新回归脚本

## Tasks & Acceptance

**Execution:**
- [ ] `code/src/app.h` -- 追加 `HWND hLogStrip;` `int log_hidden;` 字段 + `#define WM_OS_LOG_HIDE (WM_APP + 32)` -- 状态与测试钩子
- [ ] `code/src/mainwin.c` -- 新增 `OSLogStrip` 类注册与 `log_strip_proc`（点击弹出 + 主题色绘制"▲ 消息"）-- 细条窗口
- [ ] `code/src/mainwin.c` -- `WM_CREATE` 创建 `hLogStrip`；`splith_proc` 增加 `WM_LBUTTONDBLCLK` 双击收起/弹出 + 类加 `CS_DBLCLKS` -- 触发入口
- [ ] `code/src/mainwin.c` -- `layout()` 处理 `log_hidden`：收起时隐藏 `hLog`/`hSplitH`、显示并定位底部细条，右侧窗口区占满；展开时恢复原逻辑 -- 布局
- [ ] `code/src/mainwin.c` -- `os_mainwin_proc` 处理 `WM_OS_LOG_HIDE`；`os_mainwin_apply_theme` 重绘细条 -- 测试钩子+主题
- [ ] `code/src/layout.c` -- 保存/恢复 `log_hidden` 键 -- 持久化
- [ ] `tests/ui_log_drawer_drive.ps1` -- 新增回归：双击收起→细条可见→点击弹出→钩子收起/弹出→布局持久化→进程不闪退
- [ ] `tests/run_regression.sh` -- NON_JLINK 数组追加 `ui_log_drawer_drive.ps1`

**Acceptance Criteria:**
- Given 消息栏展开态，when 双击 `hSplitH`，then 消息栏收成底部细条 `OSLogStrip`（`hLog` 不可见、细条可见），右侧窗口区占满
- Given 收起态，when 点击 `OSLogStrip`，then 消息栏弹出恢复（`hLog` 可见、细条隐藏）
- Given 测试钩子 `WM_OS_LOG_HIDE` wParam=1/0，when 发送，then 收起/展开状态与双击/点击一致
- Given 布局含 `log_hidden=1`，when 重启，then 消息栏默认收起
- Given 旧布局无 `log_hidden`，when 重启，then 消息栏默认展开（无回归）
- Given 全部收起/弹出操作，when 全程观察，then 进程不闪退
- 构建 0 error / 0 warning；回归全量 ALL PASS

## Spec Change Log

## Review Triage Log

## Design Notes

消息窗口抽屉与左侧变量树完全对称：
- 变量树：`tree_auto`/`tree_hidden` 控制 + `OSTreeStrip`（左侧 8px 细条）点击弹出 + `OSTreePin` 钉住
- 消息栏：新增 `log_hidden` 控制 + `OSLogStrip`（底部细条）点击弹出；双击 `hSplitH` 收起

`OSLogStrip` 是水平细条（高约 10px，位于状态栏之上），`WM_PAINT` 用 `os_theme_brush(TH_PANEL)` 填充 + `os_theme(TH_TEXT)` 绘制"▲ 消息"，点击 `WM_LBUTTONDOWN` 调 `log_set_hidden(0)`。收起时右侧窗口区 `right_h` 从 `rc.bottom - bh - log_h - 22 - 5` 变为 `rc.bottom - bh - 22 - STRIP_H`（消息栏与分隔条让位给右侧窗口区）。`hSplitH` 类注册加 `CS_DBLCLKS` 才能收到 `WM_LBUTTONDBLCLK`。

## Verification

**Commands:**
- `python build.py --quiet` -- expected: rc=0，0 error / 0 warning
- `powershell -ExecutionPolicy Bypass -File tests/ui_log_drawer_drive.ps1` -- expected: ALL PASS
- `bash tests/run_regression.sh` -- expected: NON_JLINK 全部 ALL PASS
- `python packaging/make_setup.py --publish` -- expected: 打包 + 发布成功，HTTP 200
