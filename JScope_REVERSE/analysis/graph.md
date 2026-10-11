# JScope 9.66 图形渲染与交互专题（GraphWidget / GraphContainer / TableWidget）

**分析对象**：`C:\Program Files\SEGGER\JLink_V966\JScope.exe`（PE32+ x64，基址 `0x140000000`，Qt4 GUI）
**方法**：纯静态，基于 `dumps/alldecomp/chunk_*.c`（Ghidra 12.1.4 反编译）+ `dumps/globals.txt` + `dumps/defined_strings.txt`
**证据分级**：**[观测]** 直接来自反编译/字符串；**[推导]** 由观测确定性推出；**[推断]** 合理但未完全证实；**[未知]** 未解决
**说明**：反编译为 Ghidra 自动命名（`FUN_xxx`），本文给出语义命名均为推导结论。`[观测]` 的地址即真实 VA。

---

## 0. 关键结论速查

| 问题 | 结论 | 证据 |
|---|---|---|
| 绘制主流程 | `GraphWidget::paintEvent` = **`FUN_14004c1b0` @ `0x14004c1b0`**，顺序：定几何 → 网格 → 中心轴细刻度 → 游标区间 min/max → 各符号曲线 → 3 条游标线 → 左边缘零线标记 → FPS 统计 → 鼠标提示 | [观测] |
| 网格间距 | **一个大格 = 100 px**，常量 100 硬编码在 ≥5 处；线色 黑底 + 白 `alpha=0x3C(60)` | [观测] |
| 小格 | 中心轴每 **20 px** 一个 ±5 px 刻度（即 1 大格 = 5 小格） | [观测] |
| x 变换 | `x = xoff + (t - XStart) / (Zoom/100)`，**Zoom 单位 = µs/Div（Div=100px）**，默认 1 s/Div | [观测]+[推导] |
| y 变换 | `y = yCenter - (value/YResolution)*100 - YOffset*100`，**100 px = 1 个 YResolution**，YOffset 以"大格"为单位 | [观测] |
| DrawStyle | 0=Line(`drawLines`) / 1=Points(`drawPoints`) / 2=Line&Points / 3=Rectangle(两条竖线) | [观测] |
| 自动 Y 缩放 | 每帧在画每个符号前调用；`need = 2*max(YMaxSampled,\|YMinSampled\|) / 大格行数`，按 **1-2-5-10 阶梯**只升不降 | [观测] |
| 缩放 | 上/下方向键 = Zoom In/Out，实为 **切换"Div 下拉框"的相邻项**（档位表 27 项，`0x140096f40`，1-2-5 步进，默认索引 18 = 1 s/Div） | [观测]+[推导] |
| 平移 | ←/→ = 平移可视窗口的 1/10；Home=到 0；End=到最新；鼠标左键拖拽 = 同时平移 X 和 Y | [观测] |
| 游标 | 3 条：cursor0=鼠标(`+0x108`)、X1(`+0x130`)、X2(`+0x158`)，结构 0x28 B | [观测] |
| 无滚轮缩放 | 全部函数均未引用 `QWheelEvent`（只有 Qt 自身类方法），J-Scope 不支持滚轮缩放 | [观测] |
| FPS | `1e6 / 单次 paintEvent 耗时(µs)`（瞬时值，非滑动平均），显示 `min(实测, 目标FPS)`，目标默认 30 | [观测] |
| 采样率 | `count × 1e7 / Δt(µs)` —— 数值上是真实采样率的 **10 倍**（疑为原程序单位假设问题） | [观测]/[未知] |
| 缓冲 | `Buffer: <总字节(B/kB/MB)> (<8388608> samples), <占用%>%`，总字节 = **符号数 × 64 MiB** | [观测]+[推导] |
| 颜色 | 每个符号 `GraphColorRed/Green/Blue` 存于 `.jscope` 为 **0-255 整数**；游标色固定为橙/浅绿/浅蓝 | [观测] |

---

## 1. 绘制主流程

### 1.1 函数定位

`GraphWidget` 的 RTTI 名字在 `0x140072370`，类型描述符 `.?AVGraphWidget@@` 在 `0x140097928`，`qt_metacast` 桩在 **`FUN_14003ba30` @ `0x14003ba30`**（与字符串 `"GraphWidget"` 逐字节比较 → 确认类名）。

整个"画图"函数是 **`FUN_14004c1b0` @ `0x14004c1b0`（size=692，静态 callers=0）**：
[观测] 它 `QPainter::QPainter(local_res8, param_1+0x10)` —— `param_1+0x10` 是 QWidget 子对象；`callers=0` 且只被虚表调用，符合 `paintEvent` 的调用方式。**[推导] 即 `GraphWidget::paintEvent`。**

```c
void FUN_14004c1b0(longlong param_1) {          /* GraphWidget::paintEvent */
  uVar6 = FUN_140043990();                       /* QPC 计时开始（用于 FPS） */
  QPainter::QPainter(local_res8, param_1 + 0x10);
  lVar1 = *(longlong *)(param_1 + 0x20);         /* 绘图区矩形 x1,y1,x2,y2 @ +0x14,+0x18,+0x1c,+0x20 */
  iVar11 = (*(int *)(lVar1 + 0x1c) - *(int *)(lVar1 + 0x14)) + 1;   /* W */
  iVar10 = (*(int *)(lVar1 + 0x20) - *(int *)(lVar1 + 0x18)) + 1;   /* H */
  iVar4 = iVar10 / 100;  *(int *)(param_1 + 100) = iVar4;   /* 横向大格数 */
  iVar5 = iVar11 / 100;  *(int *)(param_1 + 0x68) = iVar5;  /* 纵向大格数 */
  iVar11 = (iVar11 % 100) / 2;  *(int *)(param_1 + 0x74) = iVar11;  /* x 原点(居中留边) */
  iVar10 = (iVar10 % 100) / 2;  *(int *)(param_1 + 0x78) = iVar10;  /* y 原点 */
  *(int *)(param_1 + 0x6c) = (iVar4 / 2) * 100 + iVar10;   /* 垂直中心像素 */
  *(int *)(param_1 + 0x70) = (iVar5 / 2) * 100 + iVar11;   /* 水平中心像素 */
  ...
  FUN_14004b290(param_1,local_res8);   /* 1. 背景 + 100px 网格 */
  FUN_14004a870(param_1,local_res8);   /* 2. 中心轴 + 20px 细刻度 */
  FUN_14004bc80(param_1);              /* 3. 按 X1/X2 游标区间算每符号 min/max/Δ */
  FUN_14004ad90(param_1,local_res8);   /* 4. 画所有符号曲线（选中者最后画） */
  FUN_14004b450(param_1,local_res8,param_1 + 0x108);  /* 5a. 鼠标游标竖线+箭头 */
  FUN_14004b450(param_1,local_res8,param_1 + 0x130);  /* 5b. X1 游标 */
  FUN_14004b450(param_1,local_res8,param_1 + 0x158);  /* 5c. X2 游标 */
  FUN_14004af10(param_1,local_res8);   /* 6. 左边缘"零线位置"彩色三角标记 */
  if (uVar8 = FUN_140043970(uVar6), uVar8 != 0)
      FUN_14003f3f0(1000000 / uVar8);  /* 7. 记录本帧 FPS */
  FUN_14004b8b0(param_1);              /* 8. 鼠标悬停 QToolTip：各符号在当前 x 的值 */
  QPainter::~QPainter(local_res8);
}
```

### 1.2 paintEvent 子函数职责表 [观测]

| 地址 | 职责 | 关键证据 |
|---|---|---|
| `FUN_14004b290` | **背景 + 大网格** | `QColor::setRgb(0,0,0,0xff)`→`fillRect`；`setRgb(0xff,0xff,0xff,0x3c)`→`drawLines` |
| `FUN_14004a870` | **中心轴 + 20px 细刻度** | `setRgb(0xc0,0xc0,0xc0,0xff)`，轴在 `+0x6c`/`+0x70`，刻度步长 `0x14`(=20) |
| `FUN_14004bc80` | 游标 X1↔X2 区间内的每符号 **min/max/Δ** | 调 `FUN_140049fe0` 二分求索引，再 `FUN_14004a2e0` |
| `FUN_14004ad90` | 遍历 `DAT_14286d3b8` 符号链表画曲线 | 跳过 `+0x35e==0`(隐藏) 或 `+0x388==0`(无数据)；`+0x380==1`(选中) 者移到循环末尾最后画（置顶） |
| `FUN_14004b640` | **单符号曲线绘制**（含 DrawStyle 分支） | 见 §3 |
| `FUN_14004b450` | 一条游标（竖线 + 顶部/底部三角箭头 + 填充） | `drawLines` + `QPainterPath` + `fillPath` |
| `FUN_14004af10` | 左侧边缘每符号一个 10px 宽的彩色三角，标出该符号 **y=0 的像素位置** | `FUN_14004a2b0` = `yCenter - YOffset*100`；越界则贴边 |
| `FUN_14004b8b0` | 鼠标悬停 `QToolTip`，逐符号 "名字: 值" | `QToolTip::showText`；`FUN_14003ece0(sym,idx)` 取值 |
| `FUN_14004bdc0` | mouseMoveEvent（拖拽平移 + YOffset 拖动） | 见 §5.3 |
| `FUN_14004c020` | mousePressEvent（`param_2+0x30==2` 右键 → 菜单） | `FUN_14003ee00(0)` 清选中；`FUN_14004a560` 建菜单 |
| `FUN_14004c470` | 其它鼠标事件：刷新鼠标游标像素/时间 | `QCursor::pos` → `mapFromGlobal` → `FUN_140049830` |

### 1.3 采样点读取（64 MB 环）

**数据模型与既有报告 §6 一致**，此处补充绘图侧读法：

- **时间戳环（全局，静态 32 MB）**：`DAT_14085d1f0`（`uint32`），容量 `0x800000` = 8,388,608 条，即 **32 MB** [观测]
  - `FUN_14003ed70(i)` @ `0x14003ed70`：逻辑索引 → 物理索引 → 返回 `*(uint32*)(&DAT_14085d1f0 + idx*4)`
  - 头/计数：`DAT_14285da78`（环形起点）、`DAT_14285da7c`（已写入总数）；`7c < 78` 即"已回绕"
  - `FUN_14003ed20()`：有效样本数 = `7c >= 78 ? 7c : 0x800000`
  - `DAT_14285da80` = 当前写指针，`DAT_14285da84` = 最新时间戳（`FUN_14003ec80`）
- **每符号数据环（堆，64 MiB/符号）**：`sym+0x3d0` 基址，**8 字节/样本**（`FUN_14003ece0` @ `0x14003ece0`：`*(longlong*)(sym+0x3d0) + idx*8`）
- **索引映射（`FUN_14003ece0`/`FUN_14003ed70` 同构）** [观测]：
  ```c
  if (idx > 0x800000) return 0;
  if (DAT_14285da7c < DAT_14285da78) {              /* 已回绕 */
      u = DAT_14285da78 + idx;  idx = (u < 0x800000) ? u : u - 0x800000;
  }
  ```
  即**索引是相对"最老样本"的相对偏移**，O(1) 定位，不做 memmove。

**关键：绘图时不抽样、不降采样、不 LOD。** [观测]
- `FUN_14004b640` 的循环是 `while (param_4 < param_5) { ... param_4++ }`，**逐样本**取 `t=FUN_14003ed70(i)`、`v=FUN_14003ece0(sym,i)`，每个样本都调用一次 `drawLines/drawPoints`（每个像素可能几十次 `drawLines`）。
- 起点/终点来自 `FUN_14004a060`（二分：首个 `t >= XStart` 的索引）与 `FUN_140049f40`（二分：首个 `t > 视图末端` 的索引），只做"可见区间裁剪"，**没有 min/max 包络**。
- [推断] 所以缩放到极小时性能靠"每帧样本数上限"自然限制：可见区间长度由 `W/100 * Zoom` 决定，当 Zoom 最小时可见样本很少，但 Zoom 最大时（1 µs/Div）反而样本少。真正的大样本量场景（Zoom 小但采样率高）会**每帧绘制上万次 drawLines** —— 这与 `FUN_14004bc30` 的节流（见 §7）配合。

---

## 2. 坐标变换与网格

### 2.1 结构体（GraphWidget）关键偏移 [观测]

| 偏移 | 类型 | 含义 |
|---|---|---|
| `+0x20` | ptr | 绘图区矩形 `(x1,y1,x2,y2)` 在 `+0x14/+0x18/+0x1c/+0x20`（所有者未定，见 §9） |
| `+0x28` | ptr | FPS 标签 QLabel |
| `+0x30` | ptr | Buffer 标签 QLabel |
| `+0x38` | ptr | Samples/s 标签 QLabel |
| `+0x40` | double | **XStart**（视图左边界时间，µs） |
| `+0x48` | longlong | 上次重绘的 QPC 时间戳（节流用） |
| `+0x50` | int | 绘图区宽度一半（初始 `W/2`） |
| `+0x54` | int | 鼠标拖拽时"最后命中的样本索引" |
| `+0x64` | int | 横向大格数 = `H/100` |
| `+0x68` | int | 纵向大格数 = `W/100` |
| `+0x6c` | int | 垂直中心像素 = `(H/100/2)*100 + yoff` |
| `+0x70` | int | 水平中心像素 = `(W/100/2)*100 + xoff` |
| `+0x74` | int | x 原点 = `(W%100)/2`（居中） |
| `+0x78` | int | y 原点 = `(H%100)/2` |
| `+0x80` | double | **Zoom**：每 100 px 代表的时间（µs/Div），默认 `0x412e848000000000`=**1000000.0**（1 s/Div） |
| `+0x88` | int | 100（占位/预留，未见别的用法） |
| `+0x2b` | bool | Autoscroll（自动跟随最新数据），默认 1 |
| `+0x11c` | int | 鼠标像素 x |
| `+0x120` | longlong | 鼠标处时间戳 |
| `+0x144 / +0x148` | int / longlong | **X1 游标** 像素 x / 时间戳 |
| `+0x16c / +0x170` | int / longlong | **X2 游标** 像素 x / 时间戳 |
| `+0x108 +0x00/0x04/0x14/0x18/0x20` | — | 游标结构 ×3（`+0x108`鼠标/`+0x130`X1/`+0x158`X2），每 0x28 B：`int id; QColor; (pad) int px_x; longlong time; bool visible` |
| `+0xe0` | ptr | 图形区右键 QMenu（"Set/Clear measurement X1/X2"） |

游标三结构初始值 [观测]：`+0x108=0/0x130=1/0x158=2`（id），`+0x128=1`（鼠标游标默认可见），颜色见 §8。

### 2.2 变换公式 [观测]

**正向（值 → 像素）`FUN_14004a0f0` @ `0x14004a0f0`**：

```c
dVar3 = FUN_14004a1b0(*(undefined4 *)(param_3 + 0x314), param_4);   /* 按 TypeID 解出 double 值 */
param_2[1] = (int)((double)(*(int *)(param_1 + 0x6c)                       /* yCenter */
                            - (int)((dVar3 / (double)*(uint *)(param_3 + 0x358)) * 100.0))  /* v/YRes*100 */
                   - *(double *)(param_3 + 0x350) * 100.0);                 /* YOffset*100 */
*param_2   = (int)(((double)param_5 - dVar1) / (dVar2 / 100.0) + (double)param_6);   /* x */
```
→ **`y = yCenter − (value / YResolution) × 100 − YOffset × 100`**
→ **`x = xoff + (t − XStart) / (Zoom / 100)`**

含义（三处独立证据互证）：
1. **100 px = 1 个大格 = 1 个 YResolution**；
2. `YOffset` 以"大格"为单位（`YOffset*100` 像素）；
3. `Zoom` 以 µs/大格 为单位。

**反向（像素 → 时间）`FUN_140049830` @ `0x140049830`**：
```c
dVar5 = (FUN_1400496f0(param_1) - XStart) * ((x - xoff) / (W - 2*xoff + 1)) + XStart;
```
其中 `FUN_1400496f0` = `XStart + FUN_140049800()`；`FUN_140049800` @ `0x140049800` = **可视时间跨度**：
```c
return (double)((W + 1) / 100) * *(double *)(param_1 + 0x80);   /* 大格数 × Zoom */
```
→ 线性插值，`x = xoff` 对应 `XStart`，`x = xoff + 可画宽度` 对应 `XStart + span`。与 §2.2 正向公式在数学上等价（两条独立路径）。[推导]

**注意**：`FUN_140049830` 与正向公式中的 `x/100` 是**整数除法**（`(W+1)/100` 而非 `W/100.0`），所以"最后一格"可能略有偏差 —— 属实现细节。[观测]

### 2.3 网格绘制 [观测]

`FUN_14004b290` @ `0x14004b290`：

```c
iVar7 = (y2-y1)+1;  iVar6 = (x2-x1)+1;
QColor::setRgb(&c,0,0,0,0xff);      QPainter::setPen(painter,&c);
QBrush::QBrush(&b,1);
QPainter::fillRect(painter, QRectF{0,0,iVar6,iVar7}, &b);        /* 背景全黑 */
QColor::setRgb(&c,0xff,0xff,0xff,0x3c);   /* 白，alpha=60 → 浅灰网格 */
QPainter::setPen(painter,&c);
/* 竖线：x = 0x74 + n*100，y 从 0x78 到 (H-0x78) */
for (n = 0; n < *(int*)(g+0x68) + 1; n++) { line.x = g[0x74] + n*100 ... drawLines(...,1); }
/* 横线：y = 0x78 + n*100，x 从 0x74 到 (W-0x74) */
for (n = 0; n < *(int*)(g+0x64) + 1; n++) { line.y = g[0x78] + n*100 ... drawLines(...,1); }
```

- **大格 = 100 px 硬编码**（`iVar3 = iVar3 + 100` / `iVar4 = iVar4 + 100`）。
- 网格线只画在绘图区内（起点 `+0x74/+0x78`，即居中后的原点）。
- 背景为**纯黑**，网格线为**白色 alpha=60**（半透明 → 视觉上是深灰）。
- 每条线一次 `drawLines(count=1)`，即每帧 N 次 `drawLines` 调用（不是一次性批量）。

`FUN_14004a870` @ `0x14004a870`（中心轴 + 细刻度）：
- 先画两条 `0xc0c0c0` 的中心轴：竖线 `x = 0x6c`（垂直中心），横线 `y = 0x70`（水平中心）；
- 然后沿两条中心轴每 **`0x14`=20 px** 画一对 ±5 px 的短刻度（`local_34+5` / `local_34-5`），循环上限 `((W - (W+1)%100) - ...)`。
- → 视觉上每个 100 px 大格被 20 px 的刻度分成 **5 小格**（即 1 小格 = Zoom/5 时间）。[推导]

---

## 3. DrawStyle（0/1/2/3）

**`FUN_14004b640` @ `0x14004b640`** 是分支点（`param_3` = 符号，`param_4..param_5` = 样本索引区间）[观测]：

```c
QPen::setWidth(local_78, (*(char *)(param_3 + 0x380) == '\x01') ? 4 : 2);  /* 选中符号线宽 4，其余 2 */
QPen::setColor(local_78, *(QColor **)(param_3 + 0x378));
QPainter::setPen(param_2, local_78);
iVar3 = *(int *)(param_3 + 0x348);                 /* DrawStyle */
if (iVar3 == 0)      pcVar6 = FUN_140049dd0;       /* 0 → 线 */
else if (iVar3 == 2) pcVar6 = FUN_140049e10;       /* 2 → 线 + 点 */
else if (iVar3 == 3) pcVar6 = FUN_140049ec0;       /* 3 → 矩形 */
else                 pcVar6 = FUN_140049e90;       /* 1（及其它）→ 点 */
...
do {
   uVar5 = FUN_14003ece0(param_3, param_4);        /* 值指针 */
   uVar4 = FUN_14003ed70(param_4);                 /* 时间戳 */
   local_98 = (int)(double)uVar4;                  /* x 像素(相对) */
   local_90 = iVar3;                               /* x 原点偏移 */
   FUN_14004a0f0(param_1, &local_80, param_3, uVar5, ...);   /* 得 y 像素 */
   (*pcVar6)(local_68, uVar1, local_80);           /* 画从上一点到当前点的图元 */
   param_4++;
} while (param_4 < param_5);
```

四个图元函数 [观测]：

| DrawStyle | 组合框文本 | 函数 | 实际 QPainter 调用 |
|---|---|---|---|
| **0** | `Line` | `FUN_140049dd0` | `drawLines(&QLine{prev, cur}, 1)` |
| **1** | `Points` | `FUN_140049e90` | `drawPoints(&QPoint{cur}, 1)` |
| **2** | `Line & Points` | `FUN_140049e10` | `drawPoints(cur,1)` **+** `drawLines(prev,中点..中点,cur)` —— 线被拆成两段（为避免与点重叠的取巧写法）[观测] |
| **3** | `Rectangle` | `FUN_140049ec0` | 两条 `drawLines`：`(x1,y1)-(x1,y2)` 与 `(x2,y1)-(x2,y2)`（**两条竖线**，横边由相邻样本对补齐 → 视觉上是矩形带） |

- 组合框按顺序添加 `Line`(0) / `Points`(1) / `Line & Points`(2) / `Rectangle`(3)：`FUN_1400521c0` 引用字符串 `"Points"`、`"Line & Points"`、`"Rectangle"`（`"Line"` 为第 0 项）；`.jscope` 的 `DrawStyle` 与之对应（`FUN_140044af0` 读写 `sym+0x348`）。
- **没有 `drawRects`** 用于曲线：全程序唯一 `QPainter::drawRects` 在 `FUN_1400532c0`（画表格列头 16×16 色块图标）。

---

## 4. 自动缩放（YResolution / YOffset）

### 4.1 相关字段 [观测]

| 偏移 | 类型 | .jscope 键 | 含义 |
|---|---|---|---|
| `+0x348` | int | `DrawStyle` | 见 §3 |
| `+0x350` | double | `YOffset` | 垂直偏移，单位 = 一个大格 |
| `+0x358` | uint | `YResolution` | **每大格(100px)代表的值** |
| `+0x35c` | bool | （未序列化） | **"Y 分辨率被手动改过"** 运行时标志 |
| `+0x35d` | bool | `IsAutoConfig` | 自动配置开关 |
| `+0x35e` | bool | `ShowInGraph` | 参与绘图 |
| `+0x380` | bool | （未序列化） | 当前选中符号（线宽 4、置顶、自动缩放只对它） |
| `+0x388` | bool | （未序列化） | 已有数据 |
| `+0x398` | double | `YMaxSampled` | 采样到的最大值 |
| `+0x3a0` | double | `YMinSampled` | 采样到的最小值 |
| `+0x3a8` | double | `YAvgSampled` | 采样均值 → 表格 "Moving Average" 列 |
| `+0x3b8 / +0x3c0 / +0x3c4` | double / int / bool | — | X1↔X2 区间 Δ值 / 类型码 / 有效标志 |
| `+0x378` | QColor* | `GraphColorRed/Green/Blue` | 曲线颜色（堆上 QColor） |
| `+0x370` | QWidget* | — | 表格里该符号的"颜色按钮" |

### 4.2 自动调整逻辑 [观测]

**触发点**：`FUN_14004b640` 开头（**每个符号、每一帧**）：
```c
if (*(char *)(param_3 + 0x35c) == '\0')      /* 未被手动改过 */
    FUN_14004b830(param_1, param_3);
```

**`FUN_14004b830` @ `0x14004b830`**：
```c
dVar2 = *(double *)(param_2 + 0x398);                       /* YMaxSampled */
if (dVar2 <= ABS(*(double *)(param_2 + 0x3a0)))             /* 与 |YMinSampled| 取大 */
    dVar2 = ABS(*(double *)(param_2 + 0x3a0));
uVar1 = (ulonglong)((dVar2 + dVar2) / (double)(uint)((H + 1) / 100));   /* 2*maxAbs / 大格行数 */
if (*(uint *)(param_2 + 0x358) < (uint)uVar1) {            /* 只升不降 */
    FUN_14003ee40(param_2, uVar1 & 0xffffffff);
    return 1;
}
```
→ **`YResolution ← ≥ 2·max(|YMinSampled|, YMaxSampled) / 大格行数`**，且**只增大不减小**（对称显示 ±max 的假设）。
[推断] 该式意味着"以垂直中心为零点，上下各铺满半个视图"，与 §2.2 中 `y = yCenter - v/YRes*100` 的居中语义一致。

**取整到 1-2-5 阶梯** `FUN_14003ee40` @ `0x14003ee40`：
```c
while (uVar2 < param_2 && uVar2 < 2000000000) {
    /* 取当前 YResolution 的最高位数字 n */
    if (n==1) new = cur*2;  else if (n==2) new = (cur>>1)*5;  else if (n==5) new = cur*2;
    else new = 10 * 10^(digits-1);
    YResolution = new;
}
```
→ 阶梯为 **1, 2, 5, 10, 20, 50, 100, 200, 500, …**，上限 2e9。[观测]
下行版本 `FUN_14003e8e0` @ `0x14003e8e0`（由 Y-Res down 调用）：`n==5 → (cur/5)*2`，`n==1||2 → cur>>1`，否则落到 `5*10^k`。

### 4.3 手动调整 [观测]

| 操作 | 实现 | 步长 |
|---|---|---|
| 表格右键 `Y-Resolution up` / `+` 键 | `FUN_140054200` → `FUN_14003ee40(sym, YRes+1)` | 上一档 1-2-5 |
| `Y-Resolution down` / `-` 键 | `FUN_1400541b0` → `FUN_14003e8e0(sym, YRes-1)` | 下一档 |
| `Y-Offset up` / `Ctrl`+`+` | `FUN_140054170`：`YOffset += 0.5` | **0.5 大格 = 50 px** |
| `Y-Offset down` / `Ctrl`+`-` | `FUN_140054130`：`YOffset -= 0.5` | 50 px |
| `Autoscale Y-Res`（可勾选） | `FUN_140053920` 建菜单；勾选态 = `(sym+0x35c == 0)`；`FUN_14003f0a0` 复位 | — |
| 鼠标纵向拖拽 | `FUN_14004bdc0`：`YOffset = YOffset + (y_prev − y_cur)/100` | **1 px = 0.01 大格** |
| 直接编辑表格单元格 | `FUN_140053c80` 列 0xe(分辨率) / 0xf(偏移)，正则 `[+-]?\d*`；偏移变化阈值 `|Δ| ≥ 0.01` | — |

- 任何手动改分辨率都会置 `sym+0x35c = 1`（关闭自动）；"Autoscale Y-Res" 取消勾选即恢复自动。
- 注意：自动缩放**只在被选中的那个符号**（`FUN_14004b640` 对每个符号都调用，但 `FUN_14004b830` 用符号自己的 YMin/YMax），实际上每个符号各自独立自动定标。[观测]

---

## 5. 缩放与平移

### 5.1 缩放 UI：不是一个"缩放因子"，而是**下拉框档位** [观测]

`ToolBar` 类（RTTI 字符串区 `0x140073820`，元对象槽 0..10）的槽 8/9/10 = `_OnZoomCurrentIndexChanged(int)` / `_OnFPSCurrentIndexChanged(int)` / `_OnCheckedAutoscroll(int)`：

```c
void FUN_1400560d0(param_1, param_2) {                 /* _OnZoomCurrentIndexChanged */
  QVariant *v = QComboBox::itemData(*(QComboBox**)(param_1 + 0x80), ..., param_2);
  uVar1 = QVariant::toUInt(v, 0);
  FUN_140049a10(DAT_14286d3a0, uVar1);                 /* DAT_14286d3a0 = GraphWidget */
  FUN_14004e120(DAT_14286d388);                        /* → QWidget::update */
}
void FUN_140055fc0(param_1) {   /* _OnActionZoomIn  → 下拉框索引 -1 */
  iVar1 = QComboBox::currentIndex(*(QComboBox**)(param_1 + 0x80));
  if (-1 < iVar1 + -1) QComboBox::setCurrentIndex(combo, iVar1 + -1);
}
void FUN_140056000(param_1) {   /* _OnActionZoomOut → 下拉框索引 +1 */
  iVar1 = currentIndex(...); iVar2 = count(...);
  if (iVar1 + 1 < iVar2) setCurrentIndex(combo, iVar1 + 1);
}
```

**档位表**（`FUN_140054790` @ `0x140054790` = ToolBar 构造函数，chunk_007.c:1103-1139）[观测]：
```c
puVar16 = &DAT_140096f40;   lVar15 = 0x1b;      /* 27 项 uint32 表 */
do {
   if      (*puVar16 < 1000)    { div = 1;       suffix = " us/Div"; }
   else if (*puVar16 < 1000000) { div = 1000;    suffix = " ms/Div"; }
   else                         { div = 1000000; suffix = " s/Div";  }
   QVariant::QVariant(&v, *puVar16);
   label = QString::number(*puVar16 / div) + suffix;
   QComboBox::insertItem(combo, count, icon, label, v);
   puVar16++;
} while (--lVar15);
QComboBox::setCurrentIndex(combo, 0x12);        /* 默认索引 18 */
```
- 表基址 `0x140096f40`，27 项 uint32，**单位 µs/Div（Div = 100 px）**。前两项已确认是 `0x1, 0x2` [观测]；
- 默认索引 `0x12`=18，且 GraphWidget 构造函数把 `+0x80` 初始化为 `1000000.0` → **索引 18 = 1,000,000 µs/Div = 1 s/Div**；[推导] 因此档位就是 **1,2,5 ×10^k**：`1,2,5,10,20,50,100,200,500,1000,2000,5000,10000,20000,50000,100000,200000,500000,1000000(索引18),2000000,5000000,10000000,20000000,50000000,100000000,200000000,500000000`（最大 500 s/Div）。两条独立观测（索引 18 与初值 1e6）互证。
- **Zoom In = 档位索引减小（时间/大格变小）**，Zoom Out = 索引增大。

快捷键（`QKeySequence`）[观测]：
- `Zoom In` = `0x1000013` = **Qt::Key_Up**；`Zoom Out` = `0x1000015` = **Qt::Key_Down**（`FUN_140054790`，`QAction::setShortcut` 后 `QWidget::addAction` → 全窗口生效）
- `Start Sampling` = `0x1000034` = **F5**；`Stop Sampling` = `0x1000038` = **F9**
- 与 SEGGER UM08028 的"UP/DOWN 缩放"一致。

### 5.2 `FUN_140049a10`：设置 Zoom 并保持视野锚点 [观测]

```c
void FUN_140049a10(longlong g, uint newZoom) {
  dVar6 = *(double *)(g + 0x80);                  /* 旧 Zoom */
  dVar1 = *(double *)(g + 0x40);                  /* 旧 XStart */
  uVar3 = FUN_140049830(g, *(undefined4 *)(g + 0x11c));   /* 鼠标处时间 t_m */
  *(double *)(g + 0x80) = (double)newZoom;
  if (newZoom >= 旧Zoom)                          /* 放大时间跨度（Zoom Out） */
      *(double *)(g + 0x40) = dVar1 - ((double)uVar3 - dVar1) * 0.5;
  else                                            /* Zoom In */
      *(double *)(g + 0x40) = ((double)uVar3 - dVar1) * 0.5 + dVar1;
  /* 夹取：XStart + span <= 最新时间戳 → XStart = 最新 − span；XStart < 0 → 0 */
  uVar2 = FUN_14003ec80();                        /* 最新时间戳 */
  if ((double)uVar2 - FUN_140049800(g) <= XStart)  XStart = uVar2 - span;
  if (XStart < 0.0) XStart = 0;
  FUN_140049c50(g, ±1);                           /* 吸附到最近样本 */
  FUN_14003b990(g);                               /* emit UpdateRequest() */
  FUN_140055470(DAT_14286d390, 0);                /* 取消"跟随最新"按钮 */
  FUN_140048320(DAT_14286d3a8, (int)XStart);      /* 同步滚动条 */
}
```
- `XStart ← XStart ± 0.5·(t_mouse − XStart)`：数学上"保持鼠标所指时间不动"当且仅当档位恰好 ×2（Zoom In）/ ×1.5（Zoom Out）[推导]。1-2-5 阶梯里 `5→2`(÷2.5)、`2→1`(÷2)、`1→2`(×2)、`2→5`(×2.5)、`5→10`(×2) —— 所以**大多数档位切换时鼠标锚点会轻微漂移**，属原程序近似的锚定策略。[推断]
- 边界夹取存在：`XStart >= 0` 且 `XStart + span <= 最新时间戳` [观测]。

### 5.3 平移 [观测]

| 操作 | 实现 | 公式 |
|---|---|---|
| **←**（Key_Left `0x1000012`） | `FUN_14004a750` | `XStart -= (int)(span/10)`；`<0 → 0` |
| **→**（Key_Right `0x1000014`） | `FUN_14004a7c0` | `XStart += (int)(span/10)`；`> 最新戳 → XStart = 最新 − span` |
| **Home**（`0x1000010`） | `FUN_14004a840` | `XStart = 0` |
| **End**（`0x1000011`） | `FUN_14004a6e0` | `XStart = 最新时间戳 − span`（`<0→0`） |
| **鼠标左键拖拽** | `FUN_14004bdc0`（`param_2+0x20`/`+0x24` = 新/旧像素 x） | `XStart = (Zoom/100)·(x_prev − x_cur) + XStart`；`<0→0` |
| 鼠标纵向拖拽 | 同上 | `YOffset = (y_prev − y_cur)/100 + YOffset` |

平移到 Home/End 前都会 `FUN_140055470(DAT_14286d390, 0)`（取消"跟随最新"的按钮按下态）。[观测]
平移结束后统一 `FUN_14004e120(DAT_14286d388)` = 触发所有图窗重绘。[观测]
`XStart` 同时作为**水平滚动条的值**：`FUN_140047f50` 设 `min=0, max=最新时间戳, singleStep=Zoom, value=XStart`；`FUN_140048320` 回写。[观测]

**边界限制**：视图**不能越过数据首尾**（`XStart ≥ 0`、`XStart + span ≤ 最新戳`），也没有"未采样区域"的空白填充。[观测]

**无滚轮缩放**：全程序没有任何函数引用 `QWheelEvent`（`dumps/defined_strings.txt` 中只有 Qt 自身类的 `?wheelEvent@QWidget/QMenu/QComboBox/QAbstractSlider/QAbstractScrollArea`，无 JScope 自己的重写）。[观测] → J-Scope 9.66 **不支持鼠标滚轮缩放**。

---

## 6. 游标与数值列

### 6.1 游标 0/1/2 与"值"列 [观测]

- 游标 3 个：0 = 鼠标（`+0x108`）、1 = X1（`+0x130`）、2 = X2（`+0x158`）。
- `FUN_1400498d0(g, n)`：`n=0 → +0x120`(鼠标时间)，`n=1 → +0x148`(X1)，`n=2 → +0x170`(X2)。
- `FUN_140049830(g, x)`：像素 x → 时间戳（§2.2）。
- `FUN_14004bae0(g, cursor, on)`：开启游标时 `cursor.time = FUN_140049830(g, g+0x11c)`（**用鼠标当前位置作为新游标的落点**），并算像素 `cursor.px = (time − XStart)/(Zoom/100) + (W%100)/2`。
- `_SlotOnShowX1/X2` → `FUN_14004bc10/14004bc20`（`FUN_14004bae0(+0x130/+0x158, 1)`）；`_SlotOnHideX1/X2` → `FUN_14004bbb0/14004bbc0`（置 0）。
- 快捷键 **Ctrl+1 / Ctrl+2** = Show X1 / X2（`0x4000031` / `0x4000032`，`FUN_14004？` 中 `QShortcut::setKey` 后 connect 到 `1_SlotOnShowX1/2`）[推断：`0x04000000`=Qt::CTRL]
- `_SlotOnMoveRightX1/X2` → `FUN_14004a4b0(cursor)`：`cursor.time += step`；`_SlotOnMoveLeftX1/X2` → `FUN_14004a410`：`-=`。`step` 由 `(double)param_1[0x10] / 10.0` 得到 —— `param_1[0x10]` 即 `GraphWidget+0x80`(Zoom)，反编译把它当有符号整数读取，故 [推断] 语义为 **Zoom/10 = 1/10 大格 = 10 px**（[观测] 只有"除以 10"这个常数与 `param_1+0x80` 这一操作数）。

### 6.2 TableWidget 列定义（`FUN_140054250` @ `0x140054250`）[观测]

| 列 | 内容 | 更新来源 |
|---|---|---|
| 0 | 勾选框 = `ShowInGraph`(`sym+0x35e`) + 颜色按钮（`sym+0x370` 的 QWidget palette） | `FUN_140054250` |
| 2 | DrawStyle 组合框（cellWidget） | `FUN_140053c10` 回读 |
| **7** | **Value**（鼠标游标处值） | `FUN_140053420(x, item, sym, 0)` |
| **8** | X1 处值（列头带游标颜色图标） | `FUN_140053420(..., 1)`；未显示 X1 时 `hideColumn(8)` |
| **9** | X2 处值 | `FUN_140053420(..., 2)`；同上 `hideColumn(9)` |
| **10** | **Δ（X1↔X2 区间 max−min）**，列头文字 `"Y"`+图标 | `sym+0x3b8`（`+0x3c0`=类型、`+0x3c4`=有效），无效显示 `"N/A"` |
| **11 / 12 / 13** | `YMinSampled`(`+0x3a0`) / `YMaxSampled`(`+0x398`) / **`YAvgSampled`(`+0x3a8`, 头"Moving Average")** | 直接取自符号结构 |
| **14** | `Y Resolution` → `FUN_140053680` 格式化为 `"%i/div"` / `"%ik/div"` / `"%iM/div"` | 每帧由 `YResolution` 重算 |
| **15** | `Y Offset`（double，`|v|<0.01` 归零） | 直接取 `+0x350` |

**Value 列的取值链**（`FUN_140053420` @ `0x140053420`）：
```c
iVar1 = FUN_140049710(DAT_14286d3a0, param_4);          /* param_4 = 0/1/2 选择游标 */
if (iVar1 == -1)  item = "N/A";
else { pdVar3 = FUN_14003ece0(param_3, iVar1);          /* 该符号数据环第 iVar1 个样本 */
       switch (sym->TypeID) { case 1: float; case 2: double; case 5/6: bool/uint8;
                              case 7: uint16; case 8: uint32; case 10: int8; case 11: int16;
                              case 12: int-float; case 14: 位域(FUN_14003eaa0); } }
```
- `FUN_140049710` @ `0x140049710` = **像素/时间 → 环索引**：先 `FUN_14004a060`（二分首个 `t >= XStart`）与 `FUN_140049f40`（二分首个 `t > XStart+span`）定区间，再 `FUN_140049fe0` 二分找到 `t >= cursor.time` 的样本；若 `cursor.time > 最新时间戳`（`FUN_14003ec80`）则返回 `-1`（→ "N/A"）。[观测]
- **Min/Max/Δ 的刷新时机**：`FUN_14004bc80` 在**每次 paintEvent** 里对每个可见符号、用当前 X1/X2 时间戳二分索引后调 `FUN_14004a2e0` 计算（只算 `+0x3b8` 的 Δ=max−min 与类型码）；`YMin/Max/AvgSampled` 则由采集线程在写入样本时维护（见既有报告 §6），**表格刷新由 `_SignalUpdateTable` 触发**（`FUN_140054250`，每次数据更新一次，`FUN_140052110` 里带 `FUN_14003ede0()` 脏标志）。[观测/推断]
- 表格单元格编辑：`FUN_140053c80`（列 0 勾选、列 14 分辨率、列 15 偏移），正则 `"[+-]?\d*"`、分辨率上限 `< 2000000000`、偏移变化阈值 `0.01`。[观测]

### 6.3 悬停提示 [观测]

`FUN_14004b8b0`：仅当主窗口是活动窗口且鼠标在 GraphWidget 矩形内时，遍历符号链表，对 `+0x388`(有数据) 的符号取 `idx = FUN_140049710(g, 0)`（鼠标游标索引），构造 `"<名字>: <值>\n..."` 调 `QToolTip::showText`；`idx == -1` 时 `QToolTip::hideText()`。bool 型显示 `"true"/"false"`。

---

## 7. 性能统计（FPS / Buffer / Samples/s）

三个标签由 **`FUN_14004fd10` @ `0x14004fd10`** 统一刷新（调用者 `FUN_14004d870`，属 MainWindow 的 `_SlotOnUpdateGUI` 路径）：`FUN_14004ff40`(FPS) → `FUN_14004fd40`(Buffer) → `FUN_140050030`(Samples/s)。[观测]

### 7.1 FPS —— `FUN_14004ff40` @ `0x14004ff40`

```c
uVar1 = FUN_14003ed60();      /* DAT_140096dd8 = 目标 FPS，初值 0x1e = 30 */
uVar2 = FUN_14003eb10();      /* DAT_14285daa8 = 实测 FPS */
if (uVar1 < uVar2) uVar2 = uVar1;                 /* 取 min */
pQVar3 = QString::number(uVar2);                  /* → 文本 */
QString::fromAscii(local_res18, "FPS: ");
QString::append(local_res18, pQVar3);
QString::fromAscii(local_res20, <0x1400718b4>);   /* 第二段字面量，dump 中未定义 */
QString::append(...);
QLabel::setText(*(QLabel **)(param_1 + 0x28), ...);
```
- **实测 FPS 的来源**：`FUN_14004c1b0` 末尾 `FUN_14003f3f0(1000000 / uVar8)`，`uVar8 = FUN_140043970(t0)` = **QXPerformanceCounter 差值换算成 µs**（`FUN_140043050`：`elapsed_us = ticks / (freq/1e6)` [观测]）。`FUN_14003f3f0` 直接写 `DAT_14285daa8`。
- → **FPS = 1,000,000 / 本次 paintEvent 耗时（µs）**，是**瞬时单帧值**，不是滑动平均；没有第二个计数器做平均。[观测]
- **目标 FPS** 初值 `0x1e`(=30)，由 `FUN_14003f420` 写入：`ToolBar` 的 FPS 下拉框 `_OnFPSCurrentIndexChanged` → `FUN_140056070`（`itemData` 取 uint）；菜单/设置也可设（`FUN_140056070`、`FUN_140054790`）。字符串 `"Target FPS: "`(`0x140078778`) 是工具栏标签。[观测]
- **节流（真正的"帧率上限"）** 在 `_UpdateScreen` 槽 `FUN_14004bc30` @ `0x14004bc30`：
  ```c
  uVar1 = FUN_14003ed60();                                 /* 目标 FPS */
  uVar2 = FUN_140043970(*(undefined8 *)(param_1 + 0x48));  /* 距上次重绘的 µs */
  if (1000000 / uVar1 <= uVar2) {
      *(undefined8 *)(param_1 + 0x48) = FUN_140043990();   /* 记录时刻 */
      QWidget::update(param_1);                            /* 触发 paintEvent */
  }
  ```
  → 重绘频率被限制在目标 FPS（默认 30 Hz），数据到达更快的部分直接丢帧（时间戳/数据仍完整入环）。[观测]
- **刷新定时器周期**：标签刷新不在 QTimer 里，而是**挂在 GUI 更新信号链**上（`FUN_14004d870` ← MainWindow 元对象槽）。MainWindow 构造函数 `FUN_14004c6a0` 里创建了 QTimer（`QTimer`/`setInterval`/`start` 出现在其 callees 中）—— [未知] 该定时器周期未在本次范围内读出。

### 7.2 Buffer —— `FUN_14004fd40` @ `0x14004fd40`

```c
iVar3 = FUN_14003ed50();          /* DAT_14085d1e8 = 符号链表节点数（增删时 +1/-1） */
iVar3 = iVar3 * 0x4000000;        /* × 67,108,864 = 64 MiB */
dVar1 = (double)FUN_14003ead0();  /* DAT_14285da7c * 100.0 / 8388608 = 环占用百分比 */
iVar2 = iVar3 / 1024;             if (iVar3 < 0x401) iVar2 = iVar3;         /* → kB */
puVar4 = (iVar3 < 0x401) ? "Byte" : "kB";                                   /* 单位（首级） */
puVar5 = (iVar2 < 0x401) ? puVar4 : "MB";                                   /* 单位（二级） */
strcat(buf, "Buffer: ");
iVar3 = iVar2 / 1024;             if (iVar2 < 0x401) iVar3 = iVar2;         /* → 最终数值 */
sprintf(tmp, "%d", iVar3);  strcat(buf, tmp);  strcat(buf, puVar5);
strcat(buf, <0x140060de4 短串>);                                            /* 由串布局推定为 " (" */
sprintf(tmp, "%d", 0x800000);    strcat(buf, tmp);                          /* 8,388,608 */
strcat(buf, " samples), ");
sprintf(tmp, "%d", (int)dVar1);  strcat(buf, tmp);                          /* 占用百分比 */
strcat(buf, <0x140077654>);                                                 /* 由串布局推定为 "%" */
QLabel::setText(*(QLabel **)(param_1 + 0x30), ...);
```
**重构显示格式** `[推导，由字符串布局 0x140077628..0x140077658 反推]`：
```
Buffer: <N> <Byte|kB|MB> (<8388608> samples), <占用%>%
```
- 单位串在 `0x140077631..0x140077647` 这 23 字节内：`0x140077634`→`"Byte"`（ASCII dump 中可见，长度 4 所以 "kB"/"MB" 被 4 字符最小长度过滤掉）、`0x14007763c`→`"kB"`、`0x140077640`→`"MB"`、`0x140077644`→`"%d"` 格式串。[推导]
- **总字节数 = 符号数 × 64 MiB**（每个符号的数据环是 8,388,608 样本 × 8 B = 64 MiB）→ 这个"Buffer"是**已分配缓冲总内存**，`8388608` 是单环容量（样本数），百分比 = `DAT_14285da7c / 0x800000 × 100` = **时间戳环已写入比例**。[观测+推导]
- 边界风格：`Byte`(≤1 kB) → `kB`(≤1 MB) → `MB` 整数（截断，不是四舍五入）。[观测]

### 7.3 采样率 —— `FUN_140050030` @ `0x140050030`

```c
uVar1 = FUN_14003eb00();            /* DAT_14285d1f8 */
if (uVar1 < 1000) pcVar2 = "Samples/s";
else { uVar1 = uVar1 / 1000; pcVar2 = "kSamples/s"; }
sprintf(..., "%d", uVar1);  strcat(..., pcVar2);
QLabel::setText(*(QLabel **)(param_1 + 0x38), ...);
```
- `DAT_14285d1f8` 的唯一写入点在采集泵 **`FUN_140041710` @ `0x140041710`**：
  ```c
  DAT_14085d1ec = param_1 / DAT_14085d184;                    /* 本次读入的样本数 */
  uVar2 = FUN_140043970(DAT_14285d1f0);                       /* 距上次的 µs */
  DAT_14285d1f8 = (uint)(DAT_14085d1ec * 10000000) / uVar2;
  DAT_14285d1f0 = FUN_140043990();
  ```
  → **显示值 = 样本数 × 10⁷ / Δt(µs)**。[观测]
- ⚠️ 按 µs 计算该式等于"每 0.1 秒的样本数" = **真实 Samples/s 的 10 倍**（正常应为 ×10⁶）。[观测] 到底是有意的显示放大、还是算错，**[未知]**；OpenScope 不应照抄此公式。
- **它不是窗口平均值**：只用"上一次泵调用至今"的样本数与时间，即**最近一次读到的瞬时速率**（窗口 ≈ 一次读取周期，通常 10 ms 级）。[观测+推断]

---

## 8. 颜色

### 8.1 曲线颜色 [观测]

- 每符号颜色是**堆上 QColor 指针** `sym+0x378`（`FUN_14004af10` 里 `pQVar2 = *(QColor **)(lVar1 + 0x378); if (pQVar2 == 0) return;`）。
- 颜色选择：表格每行的"颜色按钮" `FUN_140053760`（点按钮 → `QColorDialog::exec` → 新 QColor → 写 `sym+0x378`，同时同步按钮 palette）。
- `.jscope` 里用三个整数键 `GraphColorRed` / `GraphColorGreen` / `GraphColorBlue`（`FUN_140044bb0` → `QColor::setRgb(r,g,b)`，`FUN_1400468c0` 写出）。
- **新符号默认色如何分配**：`_SignalAssignColors` / `_SlotOnAssignColors`（字符串在 `0x1400728bc` / `0x140072a0d`）与 `FUN_14003？` 未在本次范围内定位到具体实现 → **[未知]**（本次未找到预设色表常量；不排除是运行时生成 HSV 配色）。

### 8.2 游标与其它固定色 [观测]

`GraphWidget` 构造函数（`FUN_14003b9c0` 区域，chunk_006.c:1100-1150）：

| 目标 | R,G,B | 说明 |
|---|---|---|
| `graph+0x10c`（游标 0 = 鼠标） | `0xff,0xa5,0x00` | **橙** |
| `graph+0x134`（游标 1 = X1） | `0xa5,0xff,0xa5` | **浅绿** |
| `graph+0x15c`（游标 2 = X2） | `0xa5,0xa5,0xff` | **浅蓝** |
| 网格线 | `0xff,0xff,0xff` alpha `0x3c`(60) | 黑底上的浅灰网格 |
| 背景 `fillRect` | `0,0,0`, alpha `0xff` | 纯黑 |
| 中心轴 + 20px 刻度 | `0xc0,0xc0,0xc0` | 中灰 |
| 表格列头 X1/X2 色块图标 | 取自对应游标色（`FUN_1400532c0` 用 `QPainter::drawRects` 画 16×16 填充方块，底色 `QColor(0x13)`） | |

游标色可被 `FUN_140049990(g, c0, c1, c2)` 一次性改写（由 `FUN_140048d10` 调用，疑为"设置/载入配色"路径）。[观测]

---

## 9. 未知点 / 待确认

1. **`graph+0x20` 到底指向什么**：它给出绘图区矩形（`+0x14/+0x18/+0x1c/+0x20` = x1,y1,x2,y2），但所有者/刷新点未定位（未找到给它赋值的函数）。可能是 GraphContainer 里的 viewport 或缓存的 `QRect`。**需要补充反编译**。
2. **`graph+0x88 = 100`** 初值语义未定（未见其它引用）。
3. **缩放档位表的完整 27 个值**：只确认 `0x140096f40` = 1、`0x140096f44` = 2 与默认索引 18 ↔ 1e6。1-2-5 阶梯为双证据推导，**未逐字节验证**。
4. **新符号默认配色分配算法**（`_SlotOnAssignColors` / `GraphWidget` 建符号时）未定位。
5. **状态栏刷新定时器的周期**（MainWindow 的 QTimer `setInterval` 实参）未读出。
6. **`"FPS: "` 之后第二段字面量**（`0x1400718b4`，≤12 字节，Ghidra 未定义）内容未知。
7. **采样率公式的 ×10**：是有意放大还是缺陷，缺少文档/运行时证据。
8. `YMinSampled/YMaxSampled/YAvgSampled` 的**具体更新算法**（是否含滑窗、均值是否指"移动平均"）在采集侧（`FUN_14003feb0` 等），本次未展开。

---

## 10. 对 OpenScope 的启示（建议）

1. **保留"100 px = 1 大格"这一硬约束**。J-Scope 把网格间距、Y 分辨率、"N/div" 文本、YOffset 步长（0.5 格）、游标移动步长（0.1 格）**全部绑在同一个 100 px 常数上**，代码里找不到第二套坐标系。OpenScope 若沿用"每格 = 一个可读数量级"的设计，UI 文案与交互步长就能自然统一（也便于直接显示 `us/Div`、`ms/Div`、`s/Div`）。
2. **x/y 变换写成"双公式互为逆运算并各自二分查找"**：J-Scope 同时有 `像素→时间`(`FUN_140049830`) 与 `时间→像素`(`FUN_14004a0f0`)，且用整数除法（`(W+1)/100`）导致末端 1 格轻微非线性。OpenScope 应**统一用 double**并只保留一套变换函数，避免"网格线与数据点对不齐"的像素级偏移。
3. **不要照抄"逐样本 drawLines"的绘制策略**。J-Scope 每帧对可见区间**每个样本**调一次 `QPainter::drawLines`（无 min/max 包络、无 LOD）。OpenScope 的 64 MB 环在高采样率+小 z 缩放时必然爆帧。建议：按像素列做 **min/max 包络** + `QPainterPath`/批量 `drawPolyline`，或直接走 OpenGL/QPainter 的 `drawLines(QLine*, count)` 批量重载。
4. **自动定标公式可借鉴，但应改成"只对变化量做，且允许降档"**：J-Scope 的 `YResolution = 2·max(|min|,|max|) / 格数` 且**只升不降**（`FUN_14004b830` 只在 `cur < need` 时上调），一旦遇到一次尖峰，Y 分辨率就永远回不去；且它假设数据关于 0 对称。OpenScope 建议用 `(max−min)/(格数−1)` 并做滞回（降档需连续 N 帧满足），避免曲线被压平。
5. **状态栏三项的语义要分开**：`FPS:` 是**单帧绘制耗时的倒数**且被目标 FPS 节流（`FUN_14004bc30`）；`Samples/s:` 是**最近一次读取周期的瞬时速率**（并且 J-Scope 那版还偏大 10 倍）；`Buffer:` 报的是**已分配缓冲内存**而非"已用数据量"。OpenScope 至少应把"目标样本率 / 实际样本率 / 缓冲占用 / 丢帧数"分开显示，并显示掉帧计数（J-Scope 没有丢帧可视化，只有 % 占用）。
6. **交互集**：Zoom 用"档位表"（1-2-5 ×10^k）而不是连续缩放，好处是缩放永远落在可读刻度上；但 J-Scope 的"鼠标锚点固定"公式 (`±0.5·(t_m−XStart)`) 只对 ×2 精确，非 2 倍档位会漂移。OpenScope 应直接实现 `XStart_new = t_m − (x_m − xoff)·Zoom_new/100` 的精确锚定，并补上滚轮缩放（J-Scope 缺失）。

---

## 11. 需要补充反编译

| 目标 | 为什么需要 |
|---|---|
| `graph+0x20` 的赋值点（resizeEvent / 布局函数） | 确定绘图区矩形来源；目前只能当"魔法字段" |
| `_SlotOnAssignColors` 实现（字符串 `0x140072a0d` / `0x140076f90` 的引用者） | 新符号默认配色算法 + 是否有预设色表 |
| MainWindow 构造 `FUN_14004c6a0` 的 QTimer `setInterval` 实参 | 状态栏/GUI 刷新周期 |
| `FUN_14003feb0` / `FUN_14003ff40`（采集侧写环） | `YMin/Max/AvgSampled` 的确切更新算法与"Moving Average"的定义窗口 |
| 静态数据 `0x140096f40` 的 27 个 dword 原始字节 | 逐值验证缩放档位表（需读 exe 的 .data/.rdata 原始字节，本次未做） |
| `FUN_140053000`(TableWidget 行刷新, size=651) / `FUN_140052ea0` | 表格行↔符号映射与"当前选中符号"的维护细节 |
