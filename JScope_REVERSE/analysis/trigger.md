# J-Scope 9.66 触发（Trigger）子系统逆向分析

**对象**：`C:\Program Files\SEGGER\JLink_V966\JScope.exe`（PE32+ x64，基址 `0x140000000`）
**方法**：静态分析（`dumps/alldecomp/*.c` 全量反编译 + `dumps/globals.txt` 交叉引用 + 对关键指令用 capstone 直接反汇编核对，未运行 Ghidra）
**证据分级**：**[观测]** 反汇编/反编译直接可见 · **[推导]** 由观测确定性推出 · **[推断]** 合理未证实 · **[未知]**

> 本文只覆盖触发子系统，其余部分见 `JSCOPE_ANALYSIS.md`。凡与既有报告冲突处，以本文（有指令级证据）为准，并已标注修正。

---

## 1. 概述

### 1.1 一句话结论 [观测]

J-Scope 的触发是**「命中即从触发点重开记录」**模型：

1. 每收到一个样本，**所有**变量的原始数据都无条件写入 8 字节/样本的环形缓冲（`FUN_14003feb0`）；
2. 随后对**每个变量**调用一次触发判定 `FUN_1400415a0`（返回 0/1/2/3）；
3. 若返回 **1（首次命中）**：把全局环形写指针 `writeIdx` 和 `wrapped` **清零**，该变量的 `hasData(+0x388)=1`，触发锁存位置 1；
4. 若返回 **0（未命中）**：该变量 `hasData=0` —— **曲线不绘制**（数据仍在环里，只是"不可见"）；
5. 若返回 **2（触发未使能）/ 3（已命中过）**：等同"放行"，`hasData=1`，且**不**重开记录。

绘图端只画 `hasData(+0x388) != 0` 的变量（`chunk_006.c:2450`：`ShowInGraph(+0x35E) && hasData(+0x388) && !disabled(+0x380)`）。因此触发使能前图上是空的，命中瞬间 x 轴从 0 重新开始。

`Ghidra` 已识别的类：`TriggerWidget`（RTTI `.?AVTriggerWidget@@` @ `0x140097B68`，类名字符串 @ `0x140073A20`）。

### 1.2 关键地址速查 [观测]

| 角色 | 地址 | 说明 |
|---|---|---|
| 触发判定 | `FUN_1400415a0` | 返回 0/1/2/3，见 §3 |
| 比较器 ×8 | `0x140041040/10B0/1110/1190/1200/1280/1300/1380` | 见 §4 |
| 样本解包 | `FUN_14003fa70` | `UnpackSamples`，触发门控在这里 |
| **LIVE 触发状态** | `0x142865050`（24 B） | 唯一权威状态，见 §2 |
| 快照变量（引擎） | `0x14285D9E0`（24 B） | `UnpackSamples`/起停时用 |
| 快照变量（UI） | `TriggerWidget+0x88`（堆 0x18） | 控件编辑用 |
| 工程文件内的偏移 | `project + 0x5D0` | 与 LIVE 同一地址（`0x142864A80+0x5D0 = 0x142865050`）**[推导]** |
| 状态存取器 | `FUN_14004ea20`（save）/ `FUN_14004ed00`（apply）/ `FUN_14004e830`（diff） | |
| 写回守卫 | `FUN_14004ed70` / `FUN_14004ea40`（`DAT_14286506C`="已打开工程"） | |
| UI | `FUN_1400561b0`（ctor）等，见 §6 | |
| XML 读/写 | `FUN_140045ee0` / `FUN_140046c50` | 见 §7 |

---

## 2. 数据结构 [观测]

### 2.1 触发结构体（0x18 = 24 字节）

由 `FUN_1400561b0`（分配 0x18 并逐字节初始化）与 `FUN_14004ea20/ed00`（整块搬运 24 B）共同确定：

```c
struct Trigger {          // sizeof = 0x18
  VarRecord *symbol;      // +0x00  触发符号（指向变量记录）；0 = 未选择
  double     level;       // +0x08  阈值（double）
  uint8_t    on;          // +0x10  触发使能   ← XML "On"
  uint8_t    auto_;       // +0x11  引擎从不读取（见 §7.3）  ← XML "Auto"
  uint8_t    rise;        // +0x12  上升沿     ← XML "Rise"
  uint8_t    fall;        // +0x13  下降沿     ← XML "Fall"
  uint8_t    triggered;   // +0x14  命中锁存（latch）
  uint8_t    pad[3];      // +0x15..0x17
};
```

证据（均为 [观测]）：
- 构造：`FUN_1400561b0` 中 `FUN_140043d10(0x18)` 后写 `+0x11,+0x14,+0x13,+0x10,+0x12`（`chunk_007.c:2093-2101`）。
- 字段语义：比较器读 `[rcx+0x12]/[rcx+0x13]`（沿选择）、`[rcx+8]`（阈值），写 `[rcx+0x14]=1`（`0x140041090: c6411401 mov byte ptr [rcx+0x14], 1`）。
- `FUN_1400415a0` 读 `[rcx+0x10]`（使能）与 `[rcx+0x14]`（锁存）。
- 阈值确为 double：`movsd xmm1, qword ptr [rcx + 8]`（`0x14004104E`）。

### 2.2 存放位置（4 份拷贝，同一布局）

| 实例 | 地址 | 谁用 |
|---|---|---|
| **LIVE（权威）** | `0x142865050`（+0x00..+0x17） | 全局唯一真值 |
| 引擎快照 | `0x14285D9E0` | `UnpackSamples` 开头 `FUN_14004ea20(&DAT_14285d9e0)` 拷入，判定全用这份 |
| UI 快照 | `TriggerWidget+0x88` | 控件读/改这份，再 `FUN_14004ed00` 写回 |
| 工程结构内 | `project + 0x5D0` | XML 读写目标 |

**[推导]** LIVE 就在"当前工程设置"大结构里：
- `FUN_14004ea20` 保存 `DAT_142865050/58/60` 三个 qword，`FUN_14004ed00` 恰好写回 `0x142865050(+0)`, `+0x54(+4)`, `0x142865058(+8)`, `+0x5C(+0xC)`, `0x142865060(+0x10, qword)` —— 即一整块 **24 字节**（§2.1 布局）。
- XML 读函数被调用为 `FUN_140045ee0(triggerNode, param_2 + 0x5d0)`（`chunk_005.c:6647`），而工程结构 `DAT_142864A80` 长 `0x5E8`（`FUN_14004ecc0` memcpy 0x5E8）。`0x142864A80 + 0x5D0 = 0x142865050`。
- 因此**工程加载 = 直接 memcpy 覆盖 LIVE 触发状态**，无需额外"应用"代码（也解释了 §5.2 为何 stop/start 不清它）。

### 2.3 状态存取器（写回受守卫）

```c
void FUN_14004ea20(Trigger *p);   // p = LIVE 的整块 24B 快照
bool FUN_14004e830(Trigger *p);   // p 与 LIVE 是否不同
void FUN_14004ed00(Trigger *p) {  // 有差异且"已打开工程"才写回
  if (FUN_14004e830(p)) {          // 内部: FUN_14004ed70() -> FUN_14004ea40() -> DAT_14286506C
     LIVE = *p;                   // 0x142865050..0x142865067
     thunk_FUN_14003f930();       // 0x14004ED90: inc [0x142865068]; jmp FUN_14003f930  (置"工程已修改"+通知UI)
  }
}
```
`DAT_14286506C` = "是否已打开工程"（`FUN_14004eda0` 成功时置 1）**[观测]**。

> **对 OpenScope 的启示**：这是一个非常干净的模式 —— 采集线程只做 `save → 改 → apply`，不需要锁；代价是 UI 读到的是上一次的快照（1 Hz 刷新，见 §6.6）。

### 2.4 触发所依赖的变量记录字段（既有报告 §9 补充）[观测]

| 偏移 | 用途（触发语境） |
|---|---|
| `+0x004` | `Name`（XML `Symbol` 存写它与匹配它，**不是**全路径名） |
| `+0x108` | `NameWithParents`（下拉框显示） |
| `+0x314` | `TypeID` —— 决定用哪个比较器 |
| `+0x31C` | `Size` |
| `+0x38C` | 样本内字节偏移 |
| `+0x388` | **`hasData`**：本周期是否可绘制（触发门控的落点） |
| `+0x398/+0x3A0/+0x3A8` | `yMax/yMin/yAvg`（仅"放行"的样本参与统计） |
| `+0x3C8` | `curPtr`：指向环内**当前/最后放行**样本（`Value:` 显示与绘图起点都用它） |
| `+0x3D0` | 每变量 64 MB 数据环基址 |

---

## 3. `FUN_1400415a0` 返回值语义 [观测]

`uint FUN_1400415a0(Trigger *t /*rcx*/, VarRecord *v /*rdx*/, void *prev /*r8*/, void *cur /*r9*/)`

反汇编（`0x1400415A0`，关键分支逐条核对）等价于：

```c
if (prev == NULL || cur == NULL)      return 0;   // [rcx] 之外的兜底
if (t->on == 0)                        return 2;   // 0x1400415C1: cmp byte [rcx+0x10],0
if (t->triggered == 1)                 return 3;   // 0x1400415D6: cmp byte [rcx+0x14],1
if (t->symbol != v)                    return 0;   // 0x1400415EB: cmp qword [rcx],rdx
switch (v->TypeID /*+0x314*/) {                    // 0x1400416D4 处的 14 项跳转表
  case 1:  return FUN_140041040(t,prev,cur);   // float32
  case 2:  return FUN_1400410b0(t,prev,cur);   // float64
  case 6:  return FUN_140041380(t,prev,cur);   // uint8
  case 7:  return FUN_140041280(t,prev,cur);   // uint16
  case 8:  return FUN_140041300(t,prev,cur);   // uint32
  case 10: return FUN_140041200(t,prev,cur);   // int8
  case 11: return FUN_140041110(t,prev,cur);   // int16
  case 12: return FUN_140041190(t,prev,cur);   // int32
  default: return 0;                           // 0,3,4,5,9,13 → 永不触发
}
```

**语义表**

| 返回值 | 含义 | 调用方动作（§5） |
|---|---|---|
| **0** | 未命中（触发使能了但条件未满足；**或本变量不是触发符号**；或类型不支持；或参数空） | `hasData=0`（隐藏曲线）；若本变量就是触发符号则仍推进 `curPtr` |
| **1** | **命中（边沿成立）** —— 真正意义上的"触发" | `writeIdx=0; wrapped=0`（重开记录）+ `hasData=1` + 统计累加 |
| **2** | 触发**未使能**（`on==0`）—— 对**每个**变量都返回 2 | 放行：`hasData=1` + 统计累加，不重开记录 |
| **3** | 之前**已命中**过（`triggered==1`）—— 对**每个**变量都返回 3 | 放行：`hasData=1` + 统计累加，不重开记录 |

要点 [观测]：
- **`on` 检查在符号匹配之前**，`triggered` 检查也在之前。所以命中后（`triggered=1`）**所有**变量（含非触发符号）都返回 3 → 全图恢复绘制；
- 触发符号以外的变量在等待触发期间恒返回 0，因此"触发前不显示"对全体变量统一生效；
- 类型不支持（bool `5`、int64 `13`、uint64 `9`、TypeID 3/4）→ 恒 0 → **选中这类符号做触发会永远不触发**（UI 不做任何提示）**[观测/推断]**。

> **修正既有报告 §6.3**：那里写的是"触发判定 → 0/1/2/3 → 命中则调 FUN_140042ac0"，但 `FUN_140042ac0` 实际是 **min/max/avg 统计累加器**（不是触发处理），且它接收两个参数 `(v, &ring[writeIdx])`（Ghidra 的 C 输出漏了第二个实参，反汇编 `0x14003FB7D/0x14003FB8B` 可见 `lea rdx,[rax+rcx*8]; call`）。

---

## 4. 各类型比较器逐个 [观测]

8 个比较器全部同构：`rcx = trigger`、`rdx = &prev`、`r8 = &cur`，命中时
```asm
mov byte ptr [rcx+0x14], 1     ; 置锁存
call FUN_14004ed00             ; 立即写回 LIVE（RCX 仍是 trigger 指针）
mov eax, 1
```

### 4.1 统一判定式

```
命中 = (rise && prev <  level && cur >= level)      // 上升沿：自下方穿越阈值
    || (fall && prev >  level && cur <= level)      // 下降沿：自上方穿越阈值
```

（`level` 为 `double`；整数先精确转 double 再比；两侧都取闭区间，即"等于阈值"算命中。）

### 4.2 逐函数表

| 函数 | 地址 | TypeID | 类型 | 取样/转换指令 | 备注 |
|---|---|---|---|---|---|
| `FUN_140041040` | `0x140041040` | 1 | float32 | `movss` + `cvtps2pd` | 阈值/样本都升为 double 比较 |
| `FUN_1400410b0` | `0x1400410b0` | 2 | float64 | `movsd` | |
| `FUN_140041380` | `0x140041380` | 6 | uint8 | `movzx eax,byte` + `cvtdq2pd` | 0..255 |
| `FUN_140041280` | `0x140041280` | 7 | uint16 | `movzx eax,word` + `cvtdq2pd` | 0..65535 |
| `FUN_140041300` | `0x140041300` | 8 | uint32 | 零扩展后 `cvtsi2sd xmm0, rax` | 无符号语义正确 |
| `FUN_140041200` | `0x140041200` | 10 | int8 | `movsx eax,byte` + `cvtdq2pd` | 有符号 |
| `FUN_140041110` | `0x140041110` | 11 | int16 | `movsx eax,word` + `cvtdq2pd` | 有符号 |
| `FUN_140041190` | `0x140041190` | 12 | int32 | `cvtdq2pd` | 有符号 |

跳转表证据（`0x1400416D4`，14 × int32，基址 `0x140000000`）[观测]：
```
TypeID 0 → 无比较器(ret 0)  TypeID 1 → 0x140041617 (→FUN_140041040)
TypeID 2 → 0x14004162E (float64)  TypeID 3,4,5 → 无比较器
TypeID 6 → 0x140041645 (uint8)  TypeID 7 → 0x14004165C (uint16)
TypeID 8 → 0x140041673 (uint32)  TypeID 9 → 无比较器
TypeID 10 → 0x14004168A (int8)  TypeID 11 → 0x1400416A1 (int16)
TypeID 12 → 0x1400416B8 (int32)  TypeID 13 → 无比较器
```

### 4.3 NaN 处理 [观测/推导]

无任何显式 NaN 分支；全部依赖 `comisd` 的**无序语义**：

```asm
comisd xmm1, xmm0        ; 若任一操作数为 NaN → ZF=PF=CF=1
jbe    <skip>            ; CF|ZF → 取跳转 → 本方向不成立
...
comisd xmm0, xmm1
jae    <hit>   (rise)    ; CF=0 才命中 → NaN 时 CF=1 → 不命中
jb     <no-hit>(fall)    ; CF=1 才命中 → NaN 时 CF=1 → 命中路径被跳过
```

**[推导] 结论**：`prev` 或 `cur` 为 NaN（含 ±inf 与 NaN 混用）时**两个沿都不会命中**，也就是说 NaN 既不会误触发也不会"卡住"某个沿。若 OpenScope 用 C 写 `prev < L && cur >= L`，IEEE 语义与之一致，**无需额外 NaN 分支**（前提：编译不加 `-ffast-math`）。

---

## 5. 门控与状态机

### 5.1 `UnpackSamples`（`FUN_14003fa70`）逐样本流程 [观测]

反汇编逐条核对后的等价伪代码（`0x14003FAD0..0x14003FC0E`）：

```c
FUN_14004ea20(&g_trigSnap);            // ① 取 LIVE 触发状态快照到 0x14285D9E0
numSamples = nBytes / sampleSize;
for (每个样本 p) {
  StoreTimestamp(p);                   // tsRing[writeIdx] = ts(µs)
  curTs = tsRing[writeIdx];
  if (FUN_14003ef00() != 0) goto ringAdvance;   // DAT_140096dd0 = 停止/中止标志：置位则跳过整段变量处理
  for (v = 变量链表头; v; v = v->next) {
     FUN_14003feb0(v, p);              // ★ 无条件写环: memcpy(v->ring + writeIdx*8, p + v->offs, v->size)
     base = v->ring;
     if (FUN_140041540()) {            // writeIdx==0 && wrapped==0 && lastIdx==0（采集首样本）
        v->curPtr = base;              //   指向 index 0；hasData 不动
        continue;
     }
     r = FUN_1400415a0(&g_trigSnap, v, base + (writeIdx-1)*8, base + writeIdx*8);
     writeIdx = g_writeIdx;            //   判定期间可能被比较器改过 → 重读
     if (r == 0) {                                     // —— 未命中 ——
        if (v == g_trigSnap.symbol) v->curPtr = base + writeIdx*8;   // 仅触发符号推进读数
        v->hasData = 0;                                // 不绘制
     } else {
        if (r == 1) { writeIdx = 0; g_writeIdx = 0; g_wrapped = 0; }  // ★ 从触发点重开记录
        else if (r != 2 && r != 3) { v->hasData = 0; continue; }
        v->curPtr  = base + writeIdx*8;
        FUN_140042ac0(v, base + writeIdx*8);           // min/max/avg 统计（只统计"放行"样本）
        v->hasData = 1;
     }
  }
ringAdvance:
  if (lastIdx < g_wrapped) g_wrapped++;
  g_writeIdx = lastIdx + 1;
  if (g_writeIdx > 0x7FFFFF) { g_writeIdx = 0; g_wrapped = 1; }
  g_lastIdx = lastIdx;
}
```

### 5.2 状态存储与复位 [观测]

| 事件 | 函数 | 对触发状态做了什么 |
|---|---|---|
| **开始采集** | `FUN_14003d5c0`（"Read start"） | `movups[0x14285D9E0]=0` `movq[0x14285D9F0]=0`（清快照 24B）→ `FUN_14004ea20` 快照 LIVE → `mov byte ptr [0x14285D9F4], 0` → **清 `triggered` 锁存** → `FUN_14004ed00` 写回。（`0x14285D9F4 = 0x14285D9E0+0x14`） |
| **停止采集** | `FUN_14003d7d0`（"Read stop"） | 只清**快照** `0x14285D9E0..0x14285D9F7`；**LIVE（含锁存）不动** → 停止后保存下来的触发配置与锁存位都不受影响 |
| **新建/初始化工程** | `FUN_14003cec0`（"Init project settings"） | 栈上置 `{symbol=0, level=0, flags=0}` 后 `FUN_14004ed00(&local)` → **LIVE 触发被清零** |
| **打开工程** | `FUN_140047020` → `FUN_14004ecc0` | XML 读入临时 0x5E8 结构 → `memcpy` 覆盖工程全局（含 `+0x5D0` 触发） |
| **清数据环** | `FUN_140040980` | 每变量 `ring` memset 0、`curPtr=0`、`hasData=0`，`writeIdx/lastIdx/wrapped/curTs=0`（**不碰触发状态**） |
| **原始文件导入** | `FUN_14003de60`（`Import Raw` → `FUN_14004ded0`） | 同样经 `UnpackSamples`，因此**导入数据也会触发/锁存**（会置 `triggered`） |

**[推断] 时序**：`triggered` 由比较器在 `UnpackSamples` 的样本循环中被置 1（并立即写回 LIVE），在 `FUN_14003d5c0` 开始下一次采集时被清 0。

### 5.3 命中瞬间的两处索引不一致 [推导]（潜在缺陷，OpenScope 应避免）

1. `writeIdx` 的归零发生在**同一变量的 for 循环内部**：链表顺序在触发符号**之前**的变量，其本样本已写在旧下标；在它**之后**的变量，因 `FUN_14003feb0` 读的是 LIVE 的 `writeIdx`（已为 0），会写进 index 0。→ **同一样本内不同变量落在不同下标**。
2. 归零后立刻 `curPtr = ring + 0` 并 `FUN_140042ac0(v, ring+0)`，但触发符号自己的本样本被写到了**旧下标** → `Value:` 显示与 min/max/avg 会吃到 index 0 上的**陈旧值**（首次触发时通常为 0）。
   时间轴同理：`tsRing[0]` 是旧值，直到下一帧才正确。

这两点不影响"能用"，但会让触发首帧的触发符号读数/统计偏一帧。**[推断]**（反汇编确定，是否被用户观察到未验证）

---

## 6. UI：`TriggerWidget`

### 6.1 类与对象布局 [观测]

`FUN_1400561b0`（ctor，size 1761，唯一调用者 `FUN_14004c6a0` = MainWindow ctor）：
`QWidget`（this+0/this+0x10 双 vtable）派生；`TriggerWidget::vftable` 写入 `[this]=vftable`、`[this+0x10]=vftable`。

| 偏移 | 类型 | 文本/用途 |
|---|---|---|
| `+0x28` | `QHBoxLayout*` | 主横向布局（margins 5,5,5,0） |
| `+0x30` | `QLabel*` | **"Trigger control: "** |
| `+0x38` | `QToolBar*` | 承载下面 3 个 QAction |
| `+0x40` | `QAction*` | checkable，tooltip **"Trigger Rising Edge"** |
| `+0x48` | `QAction*` | checkable，tooltip **"Trigger Falling Edge"** |
| `+0x50` | `QAction*` | checkable，tooltip **"Trigger Both Edges"** |
| `+0x58` | `QLabel*` | "Symbol: " |
| `+0x60` | `QComboBox*` | 触发符号（`setMinimumContentsLength(30)`） |
| `+0x68` | `QLabel*` | "Threshold: " |
| `+0x70` | `QLineEdit*` | 阈值输入 + `QDoubleValidator` |
| `+0x78` | `QLabel*` | "Value: " |
| `+0x80` | `QLineEdit*`（初始 `setDisabled(true)`） | 只读显示：实时值 / **"Triggered"** |
| `+0x88` | `Trigger*`（堆 0x18） | 本控件持有的触发快照 |
| `+0x90` | int | 上次变量列表代数（`FUN_14003ed50`） |
| `+0x94` | bool | "控件已使能"标志（0/1） |

控件加入布局的顺序：`Trigger control: → QToolBar → Symbol: → QComboBox → Threshold: → QLineEdit → Value: → 值显示`（`chunk_007.c:2059-2082`）。

### 6.2 信号连接 [观测]（`chunk_007.c:2083-2092`）

```c
connect(actionRising(+0x40), SIGNAL("2toggled(bool)"), this, SLOT("1_SlotOnActionRising(bool)"));
connect(actionFalling(+0x48),SIGNAL("2toggled(bool)"), this, SLOT("1_SlotOnActionFalling(bool)"));
connect(actionBoth(+0x50),   SIGNAL("2toggled(bool)"), this, SLOT("1_SlotOnActionBoth(bool)"));
connect(combo(+0x60),        SIGNAL("2currentIndexChanged(int)"), this, SLOT("1_SlotOnCurrentIndexChanged(int)"));
connect(thresholdEdit(+0x70),SIGNAL("2textChanged(const QString&)"), this, SLOT("1_SlotOnTextChanged(const QString&)"));
```

槽索引 → 实现（`FUN_14003c820` = `TriggerWidget::qt_metacall` → `FUN_14003c7a0`，按 `connect` 顺序 0..4）：

| slot | 名字 | 实现 | 行为（**直接操作触发快照的字节**） |
|---|---|---|---|
| 0 | `_SlotOnActionRising` | `FUN_140057190` | 勾选时：取消 Falling/Both 的勾选；`on=1, rise=1, fall=0`；取消勾选：`on=0`；随后 `FUN_140056da0` 写回 LIVE |
| 1 | `_SlotOnActionFalling` | `FUN_140057120` | 同上但 `rise=0, fall=1` |
| 2 | `_SlotOnActionBoth` | `FUN_1400570b0` | 同上但 `rise=1, fall=1`（"Both" = 两个沿都开，不是一个独立枚举值） |
| 3 | `_SlotOnCurrentIndexChanged` | `FUN_140057200` | `0x140057200: cmp edx,-1 / jne 0x140056da0` —— 索引为 -1 时忽略，否则等同 slot 4 |
| 4 | `_SlotOnTextChanged` | `FUN_140056da0` | 见 §6.3 |

> **注意**："启用/禁用触发"没有独立复选框 —— **勾选任一沿 = 开，全部取消 = 关**（`on` 由沿选择派生）。

### 6.3 写回路径 `FUN_140056da0` [观测]

```c
i = combo->currentIndex();
v = 遍历全局变量链表 DAT_14286d3b8 取第 i 项;
trig->symbol = (i >= 0) ? v : NULL;                   // **(trig+0x00)**
trig->level  = thresholdEdit->text().toDouble(NULL);  // **(trig+0x08)**，非法文本 → 0.0
FUN_14004ed00(trig);                                  // 写回 LIVE（有差异且在工程内）
```
- 阈值**输入校验**：ctor 里 `new QDoubleValidator(...)` + `QLineEdit::setValidator(edit+0x70, v)`（`chunk_007.c:2029-2037`）；无效文本由 `toDouble(NULL)` 静默变 0.0 [观测]。
- 符号选择**复用下拉框**（`QComboBox`），不是 SymbolSelection 变量树 [观测]。

### 6.4 刷新路径 `FUN_140056ad0` [观测]

`MainWindow(1 s QTimer, timeout → _SlotOnTimeout)` → `FUN_14003be00`(qt_metacall) → `FUN_14003ba80(idx=0x15)` → `FUN_14004d870` → `FUN_140056ad0`。
（`QTimer::setInterval(timer,1000)` @ `chunk_006.c:3688`；`FUN_140056ad0` 是唯一的 TriggerWidget 刷新入口）

分两支：
- **未采集**（`FUN_14003ef10()`=`DAT_14085d181`==0）：清空 `+0x80` → `FUN_14004ea20(widget+0x88)` 刷新快照 → `FUN_140056da0`（把快照推回 LIVE；因快照刚取自 LIVE，通常无差异、写回为 no-op）→ 判断是否需要重建下拉框（`FUN_140056ef0`：变量表代数 `DAT_14085D1E8` 变化 或 `FUN_140056e40` 发现 LIVE 与控件快照不同）→ 重建时用 `NameWithParents(+0x108)` 逐项 `insertItem`，并按名称 `memcmp` 选中当前触发符号对应的项 → 依据 `on/rise/fall` 复原 3 个 QAction 的 checked 状态。
- **采集中**：`on != 0` 且 `triggered == 1` → `setText(+0x80, "Triggered")`；否则若触发符号有效 → `FUN_140056f60` 格式化其当前值（`snprintf(buf,16,"%f", *curPtr)`，按 TypeID 取值，bool 显示 "True"/"False"）写入 `+0x80`。

### 6.5 "Triggered" 出现时机 [观测/推导]

- 出现位置：**TriggerWidget 的 "Value:" 只读框**（`+0x80`），**不是**状态栏/日志。全二进制中 "Triggered" 字符串只有 1 处引用（`FUN_140056ad0`，`globals.txt` 佐证）。
- 条件 [推导]：`采集进行中（DAT_14085d181!=0）` 且 `LIVE.on != 0` 且 `LIVE.triggered == 1`。
- 时延：由 1 Hz 定时器驱动，最坏 1 s 后才显示；触发命中本身会通过 `FUN_14004ed00 → thunk_FUN_14003f930` 投递一次 UI 通知事件（`FUN_14003f930` 末尾 `FUN_14004e2c0(DAT_14286d388, msg)`），因此实际可能立即刷新 **[推断]**。
- 停止采集后控件**恢复可用**（刷新走 §6.4 的"未采集"分支），`+0x80` 在该分支开头被 `setText("")` 清空 → "Triggered" 随之消失；但 LIVE 的锁存位**仍为 1**（见 §5.2），直到下一次开始采集才被清。

### 6.6 使能/禁用规则 [观测]

- ctor 末尾 `+0x94 = 1; FUN_140056950(this)` → 启动即**禁用**3 个 QAction、QComboBox、阈值框（图标换禁用版）。
- `FUN_140056a00`：`+0x94==0` 时**启用**（图标 0x19/0x17/0x15），`+0x94=1`。
- 禁用时图标索引 0x1A/0x18/0x16（`FUN_14002a320(idx)` 取图标）。
- `FUN_14004d870` 的判定（`chunk_006.c:4320-4328`）：`工程已打开(FUN_14004ea40!=0) 且 未采集(FUN_14003ef10()==0)` → **启用**（`FUN_140056a00`）；否则（未打开工程 **或** 采集中）→ **禁用**（`FUN_140056950`）。
  → **结论：触发只能在"停止状态"下配置**；一旦开始采集，控件立即变灰（采集期间 LIVE 快照被引擎每批数据取一次，本来就无法从 UI 改）。
  这解释了 ctor 里"先禁用"的写法（构造函数执行时还没有工程）。

---

## 7. 持久化（`.jscope`）

### 7.1 精确结构 [观测]（修正既有报告 §10.1 的 `<Level/><Auto/>...` 写法）

写：`FUN_140046c50`（被 `FUN_140046110` 调用，`param_2` = 工程结构）；读：`FUN_140045ee0`（被 `FUN_140045370` 调用，`param_2 + 0x5d0`）。属性名逐字节核对（`0x140075XXX`）：

```xml
<Project>
  <Configuration>
    ...
    <Trigger>
      <Options Level="%.17g" Symbol="<变量 Name 字段(+0x004)>"
               On="0|1" Auto="0|1" Rise="0|1" Fall="0|1"/>
    </Trigger>
  </Configuration>
  ...
</Project>
```

| XML 属性 | 字符串地址 | 触发字段 | 写入方式 |
|---|---|---|---|
| `Trigger` / `Options` | `0x140075830` / `0x1400756D8` | 元素名 | |
| `Level` | `0x1400757CC` | `+0x08` (double) | `snprintf("%.17g")`（`FUN_1400397c0`）→ 全精度 |
| `Symbol` | `0x1400756E0` | `+0x00` 所指变量记录的 `Name(+0x004)` | 字符串 |
| `On` | `0x1400757D4` | `+0x10` | 布尔，写成字符串 `"0"`/`"1"`（`0x140039760` 内部 `cmovne` 选择 `0x140070C68="0"` / `0x1400718FC="1"`） |
| `Auto` | `0x1400757D8` | `+0x11` | 同上 |
| `Rise` | `0x1400757E0` | `+0x12` | 同上 |
| `Fall` | `0x1400757E8` | `+0x13` | 同上 |

**写入条件** [观测]：`FUN_140046110` 中 `if (*(longlong *)(project + 0x5d0) != 0)`（触发符号非空）才写 `<Trigger>` 节点 → **未选符号时工程文件里没有触发段**。

**读入** [观测]：`FUN_140045ee0` 顺序读 `Level(double) → Symbol(字符串，落到全局缓冲 0x14285EDC0) → On(+0x10) → Auto(+0x11) → Rise(+0x12) → Fall(+0x13)`；随后 `FUN_140045a60` 用该名称在变量链表中逐个 `strcmp`（比较对象是 `var+4` 的 `Name`），命中则 `project+0x5D0 = var`，未命中则保持 0（触发等于没配）。
注意：`FUN_140045ee0` 对每个属性都用 `if (ret < 0) return -1` 的**强依赖**写法 —— 少一个属性就整段读失败 [观测]。

### 7.2 落盘后如何生效 [观测/推导]

`FUN_140047020`（打开工程）把 XML 读进**临时** 0x5E8 缓冲 → `FUN_14004ecc0` 整块 `memcpy` 到工程全局 `DAT_142864A80` —— 由于 LIVE 触发状态就位于 `0x142864A80+0x5D0`，**加载即生效，无任何"应用触发"的代码** [推导]。
副作用：`Level` 只进了 LIVE/工程结构，**控件上的 Threshold 输入框不会同步显示文件里的值**（全代码中没有对 `+0x70` 的 `setText` 调用；只有 `+0x80` 会被刷新）[观测]。下一次 `FUN_140056ad0` 会用 LIVE 值刷新控件快照，因此不点输入框时行为仍与文件一致；一旦用户改阈值文本，`textChanged` 会用输入框内容覆盖 [观测]。

### 7.3 `Auto` 在二进制里到底是什么？

**结论：在本版（9.66）中没有任何代码读取 `trigger+0x11`。** [观测]

证据（穷举）：
- LIVE 状态 `0x142865050..0x142865067` 只被 `FUN_14004ea20`（save）、`FUN_14004ed00`（apply）、`FUN_14004e830`（diff）触碰（`globals.txt` 仅列这三个函数）；
- `FUN_14004ea20` 的全部 5 个调用点：`FUN_14003d5c0`、`FUN_14003fa30`、`FUN_14003fa70`(UnpackSamples)、`FUN_140056ad0`、`FUN_140056e40`；
- 其中真正按字段读的只有：`FUN_1400415a0`（读 `+0x10`、`+0x14`）、8 个比较器（读 `+0x08/+0x12/+0x13`，写 `+0x14`）、`FUN_140056ad0`（读 `+0x10/+0x12/+0x13/+0x14`）、`FUN_140056e40`（**比较** `+0x10/+0x11/+0x12/+0x13` 用于变更检测）；
- 没有任何代码 `if (trigger->auto_) ...` 式的分支。

**[推断]** `Auto` 是历史遗留/保留字段（旧版本可能是"自动触发/自动停止"开关；也可能是"自动重新武装"），9.66 只做**读写往返 + 变更检测**，不产生行为差异。GUI 中也没有任何入口能改它（三个 QAction 只碰 `on/rise/fall`），所以它**只能来自外部编辑的 .jscope 文件**。
**[未知]** 其原始语义（老版本 J-Scope 或 UM08028 手册中的定义）本次未能证实。

### 7.4 与既有报告的差异汇总

| 既有报告 | 本文（有指令级证据） |
|---|---|
| `<Trigger><Level/><Auto/><Rise/><Fall/></Trigger>` | `<Trigger><Options Level= Symbol= On= Auto= Rise= Fall=/></Trigger>`，值均写作属性，布尔为 `"0"/"1"` [观测] |
| `+0x3C8` = "数据环写指针" | 实为 **`curPtr`**（指向环内当前可用样本，用于取值显示/绘图起点）；真正写指针是全局 `DAT_14285DA80` [观测] |
| 触发命中调 `FUN_140042ac0` 处理触发 | `FUN_140042ac0` 是 min/max/avg 累加器，仅在"放行"样本上调用 [观测] |

---

## 8. 未知点 / 存疑

1. **[未知] `Auto` 的原始语义**（§7.3）。需要老版本 .jscope 样本或 SEGGER 手册/发行说明比对。
2. **[未知]** 触发命中是否有**日志**输出：全字符串表中除 UI 文本外无任何 trigger 相关日志串，且 `FUN_14003f830`（AddLog）的调用点里没有触发路径 → **[推断] 无日志**，但未穷举所有日志调用点。
3. **[推断]** 触发首次命中时 index 0 的样本/时间戳陈旧（§5.3）；未用真机验证是否会看到一帧跳变。
4. **[未知]** `FUN_140056ad0` 的 1 Hz 刷新与"命中立即刷新"两者中，实际用户看到 "Triggered" 的延迟（取决于 `FUN_14004e2c0` 投递的事件是否立即被 Qt 处理）。
5. **[未知]** `0x142865044` 与触发同族的 qword 哨兵（`FUN_14003cec0` 写 `0x00F4240000000101`，只有 `FUN_14004e8a0` 比较、无人读值）语义；与触发**无关**但与"工程已修改"判定相邻，值得后续单独看。
6. **[推断]** HSS 侧无硬件触发：`JLINK_HSS_Start(变量表, n, period, 1)` 的参数里没有任何触发表；全部触发判定都在主机侧逐样本做 → 触发**不减少 USB/探针流量**。

---

## 9. 对 OpenScope 的启示（具体、可执行）

1. **触发状态直接放进"当前工程设置"结构体里，就能白拿持久化与加载即生效。**
   J-Scope 把 `Trigger` 放在工程块 `+0x5D0`（= LIVE 状态本体），于是"存盘 = 写 XML"、"读盘 = memcpy"，全程没有一句"apply trigger"的代码。OpenScope 若已有一个 `Settings/Project` 单例，建议把 trigger 作为其成员（而不是另立全局），并让 XML 读写与内存布局一一对应。

2. **采集线程用"快照 → 改 → 守卫写回"代替锁。**
   `save(FUN_14004ea20)` / `apply(FUN_14004ed00：仅当有差异且工程已打开)` 这一对是并发下最省事的做法：采集线程每批数据开头取一次快照（J-Scope 是 10 ms 一取），UI 线程随时改自己的副本再写回。代价是一个读批次（约 10 ms）的陈旧窗口 —— 对触发这种低频配置完全够用。注意 J-Scope 的 apply 内部还有 `inc [0x142865068]`（置"工程已修改"），OpenScope 可同样用它来驱动"未保存修改"提示。

3. **"命中即重开记录"可以用极低成本实现，但要在样本循环**外**归零索引。**
   骨架：`if (r==1) { writeIdx=0; wrapped=0; }` + 每变量一个 `hasData` 标志位控制绘制。J-Scope 把归零放在变量循环内部，导致同一样本内不同变量下标不一致、触发符号首帧取到陈旧值（§5.3）。OpenScope 应在**处理完整个样本（所有变量）之后**再归零，并把该样本显式写到 index 0，可顺手修掉这两个瑕疵。

4. **比较器用"类型 → 函数表"分派，并明确列出不支持触发的类型。**
   TypeID→比较器跳转表（§4.2）比每样本 switch 更好维护。同时注意：J-Scope 对 **bool、int64、uint64**（以及它的 float16/float128）**没有**比较器，用户选中会"永远不触发"且无提示 —— OpenScope 要么补齐（int64/uint64 用整数比较，注意 `(double)` 转换在 >2^53 时丢精度），要么在符号下拉框里禁用这些类型。

5. **整数阈值比较务必先升为 double 再比（与 J-Scope 对齐），浮点 NaN 不需要特判。**
   全部 8 个比较器都是"载入 → 转 double → `comisd`"，NaN 因无序标志自然两个沿都不命中（§4.3）。OpenScope 若写成 `prev < L && cur >= L` / `prev > L && cur <= L` 且不开 `-ffast-math`，行为与 J-Scope 完全一致 —— 便于将来做波形/触发结果互校。

6. **UI 抄这个最小集合即可，并把"启用"合并进"沿选择"。**
   实测 J-Scope 的触发栏只有：`Trigger control:` 标签 + 3 个互斥 checkable 动作（Rising/Falling/Both；全不选 = 关）+ 1 个符号下拉（复用已选变量列表）+ 1 个带 `QDoubleValidator` 的阈值框 + 1 个只读值框（显示 `%f` 实时值或 "Triggered"，只在采集中才有内容），且整套控件**只在"已打开工程 且 未采集"时可编辑**（采集中变灰，§6.6）。另外，如果 OpenScope 不需要 "Auto"，就在数据结构里删掉该字段，别像 J-Scope 一样留下一个"能存能读但没人用"的死字段（§7.3）。

---

## 10. 需要补充反编译

本次分析所用函数**全部**存在于 `dumps/alldecomp/`，未发现缺失。两处 Ghidra 未单独展开的跳转 thunk 已用 capstone 直接核对，结论如下（无需补充 Ghidra 作业）：

| 地址 | 名字 | 内容 |
|---|---|---|
| `0x140057210` | `thunk_FUN_140056da0` | `jmp 0x140056DA0`（TriggerWidget slot 4） |
| `0x14004ED90` | `thunk_FUN_14003f930` | `inc dword ptr [0x142865068]`（工程已修改计数）; `jmp 0x14003F930`（投递 UI 通知） |

可选后续（若要把本文的 [未知] 项清零）：
- `0x14003f930` 的完整反编译（确认通知事件的具体 ID 与是否立即重绘）—— dump 中已有该函数（`chunk_005.c:1876`），但 `FUN_14004e2c0` 的事件语义未展开；
- 老版本 J-Scope（9.5x 之前）的 `.jscope` 样本或手册，用来确认 `Auto` 的原始语义。

---

*本文所有地址均为镜像基址 `0x140000000` 下的 VA。除只读访问 exe 字节与既有 dump 外，未运行 J-Scope、未运行 Ghidra、未连接探针。*
