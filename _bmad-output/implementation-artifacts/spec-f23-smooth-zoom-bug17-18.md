# BMAD Spec — F23 波形丝滑缩放/拖拽/框选 + Bug17 + Bug18

## 1. Clarify（需求澄清）

request.md 本轮三项：

- **特性 23**：波形窗口鼠标缩放存在跳变。保持现有功能（滚轮缩放 X/Y、测量标记、F 全局、Ctrl+B 堆叠），需实现：
  1. 更丝滑、连续的拉伸（缩放平滑动画，替代 step factor 0.8/1.25）
  2. 拖拽波形平移视图
  3. 鼠标框选局部区域放大
- **Bug 17**：安装包开始安装界面显示 v1.13.0，但安装后关于显示 v1.14.0 正确。
  根因：packaging/openscope.iss 第 8 行 `AppVerName=OpenScope 1.13.0`、第 28 行 `VersionInfoProductVersion=1.13.0.0` 是旧版本。make_setup.py 只校验
  `VersionInfoVersion={full}` 和 `OpenScope-Setup-{display}`，漏掉 AppVerName/VersionInfoProductVersion。
- **Bug 18**：仍无"全部隐藏全局变量窗口"功能。
  现状：`tree_auto_tick` 钉住态（tree_auto=0）强制 `tree_auto_expand`，即使 tree_hidden=1 也被展开；无显式折叠动作（消息栏有双击分隔条折叠，树没有）；tree_hidden 未持久化到 layout。

## 2. Plan（实现规划）

### Bug 17（最小改动）
- openscope.iss：第 8 行 → `AppVerName=OpenScope 1.14.0`；第 28 行 → `VersionInfoProductVersion=1.14.0.0`。
- packaging/make_setup.py：校验增补 `AppVerName=OpenScope {display}` 与 `VersionInfoProductVersion={full}`，防止回归。

### Bug 18（变量栏完全隐藏）
1. `split_proc`（mainwin.c:667）：加 `WM_LBUTTONDBLCLK` → `tree_set_hidden(g_app.tree_hidden ? 0 : 1)`（消息栏折叠同款交互）。OSSplitter 类注册加 `wc.style = CS_DBLCLKS`（mainwin.c:2911 附近）。
2. 新增 `tree_set_hidden(int)` 辅助函数（仿 `log_set_hidden` mainwin.c:703）：设置 g_app.tree_hidden、layout()、os_log。
3. `tree_auto_tick`（mainwin.c:821）：修正钉住态强制展开——当 `tree_hidden` 已为 1（手动折叠）时保持隐藏，不再被 `tree_auto=0` 分支强制展开；悬停细条仍展开。调整分支顺序：先判断 `tree_hidden` 隐藏态，再处理钉住。
4. layout.c：保存/恢复 `tree_hidden`（save 在 line 121 附近，load 在 line 262 附近）。
5. 新测试钩子 `WM_OS_TREE_HIDE (WM_APP+33)` → `tree_set_hidden(wParam)`。
6. 新回归脚本 tests/ui_tree_fold_drive.ps1：双击 OSSplitter 折叠 → 树隐藏/细条可见；钩子展开；钉住后保持；layout 持久化。

### 特性 23（chartwin.c）
- 新增 OS_ChartWin 字段：平滑动画（anim 目标 + SetTimer 插值）、拖拽状态（d0/moving）、框选矩形（bx0/by0/bx1/by1）。
- 滚轮缩放改为连续：`factor = pow(0.8, delta/120.0)`（每 tick 1/120 增量连续），并启动 SetTimer 平滑动画插值 vx0/vx1/vylo/vyhi，避免跳变。
- 左键拖拽：WM_LBUTTONDOWN 记录起点与当前视图；WM_MOUSEMOVE 位移>阈值进入平移（fit_x/fit_y=0，按像素差平移 vx0/vx1 与 vylo/vyhi）；WM_LBUTTONUP 结束。单击（无位移）仍走测量标记路径。
- Ctrl+左键框选：WM_LBUTTONDOWN(Ctrl) 记录框选起点；WM_MOUSEMOVE 更新框选矩形；WM_LBUTTONUP 应用局部缩放（vx0/vx1 = 框选 X 区间，vylo/vyhi = 框选 Y 区间，fit_x/fit_y=0）。绘制框选虚线框。
- 新测试钩子 + tests/ui_chart_f23_drive.ps1。

## 3. Implement
见各源文件编辑（本 spec 驱动的实现）。

## 4. Review
- 回归：tests/run_regression.sh（新增 2 个脚本进 NON_JLINK 数组）。
- 打包：make_setup.py --publish 校验所有版本字段。
- 版本提升：1.14.0 → 1.15.0（checkpoint-29）。
