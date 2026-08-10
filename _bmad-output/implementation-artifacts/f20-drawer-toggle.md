# F20 左侧 elf 侧边栏折叠/展开按钮（Drawer Toggle）

request.md 新增特性 20：左侧 elf 变量栏需要实现侧边栏折叠/展开按钮，类似左右箭头
（抽屉开关，Drawer Toggle）。

## 现状（checkpoint-23 / v1.9.0）

- 变量栏 `SysTreeView32` 位于主窗口左侧 `(0, bh, tree_w, right_h)`，顶部右侧浮有钉图标
  `OSTreePin`（钉住常显/灰=自动隐藏）。
- 自动隐藏：`tree_auto`（钉住开关）+ `tree_hidden`（当前是否隐藏）。未钉住且光标离开
  树区超过 800ms → 隐藏，只留左侧 8px 细条 `OSTreeStrip`（画 "❯"，点击/悬停展开）。
- 缺陷：变量栏**展开时没有显式折叠按钮**——要收起只能等光标移开（未钉住时），钉住时
  根本无法显式折叠；折叠态细条过窄、箭头不醒目。

## 设计

在变量栏顶部增加一行 24px 头部栏：

```
┌─┬──────────────────────────┬────┐
│❮│ 变量                     │ 📌 │  ← OSTreeHeader (btnface 背景 + "变量" 标签)
└─┴──────────────────────────┴────┘
│                             │
│      SysTreeView32          │    ← 树下移 24px（避免遮住折叠按钮）
│                             │
```

- `OSTreeToggle` 按钮（头部栏左侧，22×22）：展开态画 "❮"（左箭头，点击折叠）。
- `OSTreePin` 钉按钮移入头部栏右侧（`tree_w-34`）。
- 折叠后：头部栏 + 两个按钮隐藏，保留左侧 8px 细条 `OSTreeStrip` 画 "❯"（展开），
  点击/悬停展开 —— 即"展开态见左箭头、折叠态见右箭头"的抽屉开关语义。
- 新增 `tree_force_hidden` 状态：显式折叠（按钮点击）置 1；`tree_auto_tick` 钉住分支
  在 `tree_force_hidden=1` 时不再强制展开，保证"钉住 + 手动折叠"能保持收起；自动隐藏
  不改该标志；任何展开路径（按钮/细条点击/悬停）清除该标志。
- 测试钩子：`WM_OS_TREE_TOGGLE (WM_APP+14)`，wParam=0 展开 / 1 折叠 / 2 切换。

## 改动文件

- `code/src/app.h`：OS_App 增 `hTreeHeader/hTreeToggle`、`tree_force_hidden`；新消息
  `WM_OS_TREE_TOGGLE`。
- `code/src/mainwin.c`：注册 `OSTreeHeader/OSTreeToggle` 窗口类；创建控件；
  `layout()` 头部栏定位 + 树下移；`tree_auto_tick/tree_auto_expand` force 逻辑；
  WndProc 处理 `WM_OS_TREE_TOGGLE`。
- `tests/ui_tree_toggle_drive.ps1`：新 UI 回归。

## 测试策略

- 构建：`python build.py --quiet` 0 error/0 warning + `build_tests.bat` 单元回归。
- 新增 UI 回归 `ui_tree_toggle_drive.ps1`：折叠按钮点击 → 树隐藏+细条可见；细条点击
  展开；钉住后手动折叠保持收起（force 逻辑）；`WM_OS_TREE_TOGGLE` 钩子；全程无闪退。
- 全量回归 `tests/run_regression.sh` dev + 安装版。
- 打包 1.10.0 + 静默安装验证 + 发布。
