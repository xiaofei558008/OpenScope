# J-Scope 9.66 — 运行时基础设施专题（作业队列 / 线程 / 定时器 / 堆分配器 / 日志 / 时间基准）

分析对象：`JScope.exe`（PE32+ x64，imagebase 0x140000000，Qt4 GUI，SEGGER J-Scope 9.66）。
证据全部来自 `dumps/alldecomp/chunk_000.c .. chunk_008.c`（2001 个函数）+ `dumps/globals.txt`（符号/初值/引用者）+ `analysis/funcmap.txt`。
证据分级：**[观测]** = 反编译代码直接可见；**[推导]** = 由多处观测交叉推理；**[推断]** = 合理解释但缺直接证据；**[未知]** = 需补充。

> 通用读数约定（本文反复使用）：
> - `FUN_140043d10(size)` = 无标签分配；`FUN_140043d20(size, "tag")` = 带标签/带守卫分配；`FUN_140043e60(ptr)` = 释放；`FUN_140043fd0(ptr,size)` = 再分配。三个入口都归一到同一个堆（见 §4）。
> - `FUN_140043990()` = QPC 当前 tick（已减去首次基准）；`FUN_140043970(base)` = 距 base 的**微秒**数；`FUN_140043cc0()` = 距进程首次调用的**毫秒**数（winmm `timeGetTime`）。
> - `DAT_14286d388` = **MainWindow 单例指针**（`chunk_006.c:3550  DAT_14286d388 = (QObject *)param_1;`，在 `MainWindow::MainWindow` 里赋值）。全程序 62 处引用，是"UI 上下文"的唯一入口。

---

## 0. 关键地址速查表

| 作用 | 地址 / 全局 | 备注 |
|---|---|---|
| 作业提交（包装请求描述符） | `FUN_140039100` → `FUN_140039140` | 16 个作业创建函数都调它 |
| 环形命令队列槽标志数组 | `DAT_14085cf50`（32×u32，步长 0x10） | `memset(...,0x200)` = 32 槽 |
| 环形命令队列槽作业指针数组 | `DAT_14085cf58` | 与上者同一结构体交替排列 |
| 生产者写索引 | `DAT_14085d158`（u32） | `if (0x1f < idx+1) idx = 0;` |
| 周期作业链表头 | `DAT_14085d150` | 节点 +0x00=next, +0x08=prev |
| 处理器表（命令分派） | `DAT_14085d170`（malloc 0x100，拷贝自只读表 `0x140073ef0`） | 16 项 × 0x10 字节，第 0 项 = `FUN_14003cc50` |
| 处理器条目数 | `DAT_14085d15c` = 16 | 由 `FUN_140039300(&PTR_FUN_140073ef0, 0x10)` 写入 |
| 工作线程句柄 / 唤醒事件 | `DAT_14085d160` / `DAT_14085d168` | 线程名 "WORKER" |
| 调度器停机标志 | `DAT_14085cf40` | 置 1 后新提交返回 0xffffffff |
| 作业调度临界区对象 | `DAT_14085cf48` | `FUN_140043110(0)` 建的 spinlock-CS |
| 带守卫堆开关 | `DAT_14285eb9c` | **全程序无写入点**，见 §4.5 |
| 堆块链表头 / 块数 / 字节数 | `DAT_14285ec30` / `_DAT_14285eb90` / `_DAT_14285eb94` | |
| UI 侧临界区 | `MainWindow+0x318` | `FUN_14004d020`/`FUN_14004d150` 进出 |
| 日志累积缓冲 | `DAT_14285db80`（0x1000 字节） | 段结束弹窗用 |
| 日志嵌套深度 / 错误计数 | `DAT_14285eb8c` / `DAT_14285eb84` | |

---

## 1. 作业（Job）系统

### 1.1 结论速览

- 不是 Qt 的 `QThreadPool/QtConcurrent`，也不是 `QTimer` 驱动；而是 **自研"命名 Job + 单工作线程 + 环形命令队列 + 处理器表"** 的框架，线程名 `"WORKER"`，用 `CreateThread` 创建。
- 两类作业并存：
  1. **一次性命令作业**（ring queue，32 槽，id 0..15 用处理器表分派）—— Init/Exit/ReadStart/ReadStop/ParseELFFile/SaveProject…
  2. **周期作业**（有序链表，`due`/`period` 毫秒字段，回调返回非 0 即自我注销）—— 只有 `APP: <ReadPeriodic>`（10 ms）。
- 作业名字符串只是**内存分配标签**（`FUN_140043d20(size,"APP: <ReadStart>")`），不是运行时查找键；运行时靠 **id（`job+0x18`）** 分派。**[观测]**

### 1.2 作业对象的创建（16 个创建函数）

统一模式（以最小作业为例，`chunk_005.c:590`）：

```c
puVar1 = (undefined8 *)FUN_140043d20(0x28,"APP: <ReadStop>");   // 带标签分配
*(undefined4 *)(puVar1 + 3) = 7;      // +0x18 = 命令 id（7=ReadStop）
*puVar1 = &LAB_140042ee0;             // +0x00 = 作业完成回调（done）
puVar1[1] = puVar1;                   // +0x08 = 自指针
puVar1[2] = 0;                        // +0x10 = 上下文参数（多数=0，CreateProject 传 MainWindow*）
FUN_140039100(puVar1);                // 提交
```

**作业=0x20 字节头 + 载荷**，完整清单（id 由 `*(u32*)(job+0x18)` 观测，名称来自分配标签字符串）：

| id | 名称(分配标签) | 创建函数 | 分配大小 | 载荷/特有字段 |
|---|---|---|---|---|
| 0 | `APP: <Init> Job` | `FUN_14003e2e0` (chunk_005:380) | 0x30 | +0x20/+0x28 两个启动参数；**同时**注册处理器表 `FUN_140039300(&PTR_FUN_140073ef0,0x10)` |
| 1 | `APP: <Exit>` | `FUN_14003e1b0` | 0x28 | done 回调是 `&LAB_140042eb0`（与其它不同） |
| 2 | `APP: <CreateProject> Job` | `FUN_14003e110` | 0x128 | +0x20 起拷入 0x104 字节路径；+0x10=MainWindow* |
| 3 | `APP: <InitProjSettings>` | `FUN_14003e380` | 0x608 | +0x20 起 `memcpy(...,0x5e8)` 工程设置快照 |
| 4 | `APP: <ParseELFFile>` | `FUN_14003e4e0` | 0x128 | +0x20 起 0x104 路径；`+0x124` 一个 bool |
| 5 | `APP: <ReadStart>` | `FUN_14003e620` | 0x28 | done 回调 = `FUN_140042f60` |
| 6 | `APP: <ReadPause>` | `FUN_14003e5c0` | 0x28 | |
| 7 | `APP: <ReadStop>` | `FUN_14003e680` | 0x28 | |
| 8 | `APP: <SaveProject>` | `FUN_14003e6e0` | 0x28 | |
| 9 | `APP: <SaveProjectAs>` | `FUN_14003e740` | 0x28 | |
| 10(0xa) | `APP: <OpenProjFile> Job` | `FUN_14003e420` | 0x138 | `+0x128`=完成回调函数指针, `+0x130`=回调上下文（见 §1.6） |
| 11(0xb) | `APP: <ImportRaw>` | `FUN_14003e210` | 0x128 | +0x20 起 0x104 blob（手工 8 字节展开拷贝） |
| 12(0xc) | `APP: <CheckELFModifications> Job` | `FUN_14003e0b0` | 0x28 | 由 UI 1 s 定时器投递 |
| 13(0xd) | `APP: <UpdateProjSettings> Job` | `FUN_14003e800` | 0x608 | +0x20 起 0x5e8 |
| 14(0xe) | `APP: <SetAppStartAction> Job` | `FUN_14003e7a0` | 0x28 | +0x20 = u32 动作码 |
| 15(0xf) | （无名字，栈上描述符） | `FUN_14004db40` 内联构造 | 0x20 | **保留=终止工作线程**，见 §1.7 |

> 说明：0x128 = 0x20 头 + 0x104 路径 + 4 字节填充；0x608 = 0x20 头 + 0x5e8（工程设置结构体，`chunk_006.c:4240` 也有 `FUN_140043d20(0x5bc,"Project settings")`，0x5bc 是基址大小）；0x138 = 0x20 + 0x104 + 回调对。**[观测]**

### 1.3 作业对象字段布局（伪结构体）

```c
struct JOB {                       // 一次性命令作业
  void (*done)(JOB*);   // +0x00  处理器执行完由工作线程调用（销毁/善后）
  JOB   *self;          // +0x08  指向自身（处理器用 *((JOB**)job+1) 取回 job）
  void  *arg;           // +0x10  上下文（CreateProject/OpenProjFile 传 MainWindow*）
  uint32 id;            // +0x18  命令 id 0..15 → 处理器表索引
  int32  result;        // +0x1c  处理器写结果码，done 回调读（如 <0 表示失败）
  uint8  payload[];     // +0x20  各作业私有载荷
};

struct PERIODIC_JOB {              // 周期作业（另一套布局，同样 0x28 起）
  PERIODIC_JOB *next;   // +0x00  链表 next
  PERIODIC_JOB *prev;   // +0x08  链表 prev
  int (*tick)(JOB*);    // +0x10  回调；返回 0 继续，非 0 注销
  void  *payload;       // +0x18  回调非 0 时被释放的附加缓冲
  uint32 period_ms;     // +0x20  周期
  uint32 due_ms;        // +0x24  下次到期时刻（工作线程写）
};
```

证据：
- `id` 偏移：工作线程 `uVar1 = *(uint *)(puVar3 + 3);`（`chunk_004.c:3259`，`undefined8*`+3 = +0x18）**[观测]**
- `result` 偏移：`<ReadStart>` 处理器 `*(undefined4 *)(param_1 + 0x1c) = iVar2;`，完成回调 `iVar1 = *(int *)(param_1 + 0x1c); ... if (iVar1 < 0) FUN_14003f110("Failed to start data sampling.");`（`chunk_005.c:4481`）**[观测]**
- `self` 语义：`FUN_140040e10`（作业析构）`if (*(longlong*)(param_1+8) != 0) param_1 = *(longlong*)(param_1+8); FUN_140043e60(param_1);` **[观测]**
- `period_ms/due_ms`：见 §1.5。

**作业生命周期（[观测]）**：创建者分配 → `FUN_140039100` 入队 → 工作线程调处理器 → 工作线程调 `done(job)` → 标准 `done` 走 `FUN_140040e10` 释放作业内存。即**作业由工作线程自销毁，创建者不再碰它**（只有周期作业由 `FUN_14003d7d0` 在停机路径显式删除）。

### 1.4 提交路径：`FUN_140039100` / `FUN_140039140`（生产者）

`FUN_140039100(job)` 是瘦包装，构造**请求描述符**后调用真正的提交函数：

```c
// FUN_140039100  (chunk_004.c:3037)
local_c = 1; local_20 = 0; uStack_18 = 0; local_10 = -1;   // tick=1, cb=0, cbarg=0, timeout=-1
local_28 = param_1;                                        // 描述符+0x00 = job
FUN_140039140(&local_28);
```

`FUN_140039140(desc)` 语义（`chunk_004.c:3060`，**这是队列核心**）：

```c
if (DAT_14085cf40 == '\0') {                 // 未停机
  FUN_140039400();                           // 进作业临界区（FUN_140043110 懒建 CS）
  FUN_140039370();                           // 懒建工作线程 + 事件（"WORKER"）
  timeout_us = (desc.timeout_ms == -1) ? 0 : desc.timeout_ms * 1000;
  t0 = FUN_140043990();
  while (1) {
    if (slot[WRITE_IDX].flag == 0) {          // 找到空槽 → 入队
      slot[WRITE_IDX].job  = desc.job;        // *(u64*)(&DAT_14085cf58 + idx*0x10) = *param_1
      slot[WRITE_IDX].flag = 1;
      DAT_14085d158 = (idx+1 > 0x1f) ? 0 : idx+1;   // 环形回绕（32 槽）
      unlock(); SetEvent(DAT_14085d168);      // 唤醒工作线程
      return 0;
    }
    if (desc.cb && desc.cb(desc.cbarg) < 0) break;        // 进度回调请求取消
    if (timeout_us && FUN_140043970(t0) > timeout_us) { unlock(); return 0xfffffffe; }
    unlock(); Sleep(1 - (elapsed_us/1000)); Sleep 到 1ms 边界; lock();   // 队列满→1ms 节拍轮询
  }
  unlock(); return 0xfffffffd;
}
return 0xffffffff;                                        // 已停机
```

**[观测]** 要点：
- **队列已满 = 生产者原地阻塞轮询**（1 ms 节拍 `Sleep`，期间可被进度回调打断），**不扩容、不丢弃**。32 槽上限。
- 返回值语义：`0` 成功 / `0xfffffffe(-2)` 超时 / `0xfffffffd(-3)` 被进度回调取消 / `0xffffffff(-1)` 调度器已停机。
- **超时/取消只是生产者不再等待，作业仍在队列里、仍会被执行**（没有出队/撤回逻辑）——所谓"取消"是**假取消**。**[观测]**
- 生产者之间用 `DAT_14085cf48` 临界区互斥，消费者不持该锁。

### 1.5 执行路径：工作线程主循环 `FUN_140039430`

线程入口 `FUN_140039430`（`chunk_004.c:3241`，`callers=0`，仅由 `CreateThread` 引用）：

```c
for (;;) {
  /* (A) 排空环形命令队列：从 index 0 起扫描非空槽 */
  while (slot[i].flag != 0) {
    job = slot[i].job;
    id  = *(u32*)(job + 0x18);
    if (id < DAT_14085d15c) {                       // 已知命令
      if (id == DAT_14085d15c - 1) { /* 15 → 停机流程，见 §1.7 */ return 0; }
      (*(code*)(DAT_14085d170 + id*0x10))(job);     // 处理器表分派，参数=job
    }
    if (*(code**)job != 0) (*(code**)job)(job);      // done 回调（自销毁）
    slot[i].flag = 0;
    i = (i+1 < 0x20) ? i+1 : 0;
  }
  /* (B) 走周期作业链表，执行所有到期的 */
  job = DAT_14085d150;  min_wait = 0;
  do {
    due = *(u32*)(job+0x24);
    now = FUN_140043cc0();                           // 毫秒
    if ((int)(due - now) < 1) {                      // 到期（有符号差 → 天然处理 32 位回绕）
      rc = (*(code*)job[2])(job);                    // +0x10 回调
      if (rc == 0) *(u32*)(job+0x24) = now + *(int*)(job+4 /*+0x20 period*/);
      else { unlink(job); if (job[3 /*+0x18 payload*/]) free(job[3]); free(job); }   // 自我注销
    } else if (min_wait == 0 || due < min_wait) min_wait = due;
    job = *job;                                      // +0x00 next
  } while (job);
  /* (C) 睡眠：无周期作业 → WaitForSingleObject(event, INFINITE)；否则等到最近的 due */
  if (min_wait == 0) WaitForSingleObject(event, INFINITE);
  else { now = FUN_140043cc0(); WaitForSingleObject(event, min_wait - now); }
}
```

**[观测]** 状态机其实就是三段式：**(A) 命令环 → (B) 周期链 → (C) 等事件/等到期**。没有 pending/running/done 的显式状态字段；`slot[i].flag`（0/1）是唯一的队列状态，作业执行期间该槽保持 1（所以同 id 作业可以排队但不会并发）。因为**只有一个工作线程**，所有作业天然串行，互不加锁；这与 §1.4 生产者端的锁形成"多生产者-单消费者"模型。

### 1.6 处理器实现（已识别的）

处理器只通过只读表 `0x140073ef0`（16×0x10，第 0 项 = `0x14003cc50`，`globals.txt:2512`）间接调用，因此 Ghidra 里 `callers=0`。已定位：

| id | 处理函数 | 判定依据 | 等级 |
|---|---|---|---|
| 0 Init | `FUN_14003cc50` | 表首项值 = 14003cc50（globals.txt）；体内读 `*(job+0x10)`/`*(job+0x20)`/`*(job+0x28)`、写注册表 `Software\SEGGER\J-Scope\` | [观测] |
| 1 Exit | `FUN_14004db40` | 体内 `FUN_14003e1b0()`（投递 `<Exit>` 作业）+ 等待 + `QCoreApplication::exit(0)`；`callers=0` | [推导] |
| 3 InitProjSettings | `FUN_14003cec0` | 字符串 `"Init project settings"` 唯一引用者（globals.txt:2562）；`callers=0` | [推导] |
| 4 ParseELFFile | `FUN_14003d210` | 体内 `FUN_14004e1f0(mw,uVar,"Parsing ELF file ...",…)` 进度条 + `FUN_14003e880` 释放旧 ELF | [推导] |
| 5 ReadStart | `FUN_14003d5c0` | 体内 `*(int*)(job+0x1c) = iVar2`（结果码）、`FUN_14003f830("Read start")`、末尾创建 `APP: <ReadPeriodic>` | [推导] |
| 10 OpenProjFile | `FUN_14003d950` | 读 `*(job+0x128)`/`*(job+0x130)`，与创建者 `FUN_14003e420` 写入的偏移一一对应；并以 `*(job+0x10)` 为参数回调 | [推导] |
| 12 CheckELFModifications | `FUN_14003d9b0` | 体内比较 ELF 的 (size,mtime) 并弹 `"The elf file at\n%s\nwas modified.…"`、调 `FUN_14003e4e0` 重新解析 | [推导] |
| 13 UpdateProjSettings | `FUN_14003db70` | 体内 `FUN_14003f830("Update project")` | [推断] |
| 未定 | `FUN_14003ce50`, `FUN_14003dcb0`, `FUN_14003dd90` | 候选池（均 `callers=0`），对应 id 2/6/7/8/9/11/14 中未定者 | [未知] |

### 1.7 停机 / 终止工作线程

`FUN_14004db40`（Exit 处理器）在最后一个阶段做了 **"用栈上描述符当作业"** 的技巧：

```c
uVar2 = FUN_1400430f0(0,0);      // CreateEventA(NULL, FALSE, FALSE, NULL) —— 自动复位事件
local_10 = 0xf;                  // 描述符 +0x18 = id = 15 = DAT_14085d15c-1
local_20 = uVar2;                // 描述符 +0x08 = 事件句柄
local_18 = param_1;              // 描述符 +0x10 = MainWindow*
FUN_140039100(&local_28);        // 把栈地址塞进队列槽
while (FUN_1400443d0(uVar2,10) != 0) { QCoreApplication::processEvents(0); }  // 边抽事件边等
FUN_140043000(param_1 + 0x318);  // 删除 UI 临界区
QCoreApplication::exit(0);
```

工作线程侧对应分支（`chunk_004.c:3260`）：

```c
if (uVar1 == DAT_14085d15c - 1) {            // id == 15
  DAT_14085cf40 = 1;                         // 停机标志 → 之后新提交返回 -1
  FUN_140042ff0(DAT_14085d160); FUN_140042ff0(DAT_14085d168);   // CloseHandle ×2
  DAT_14085d160 = 0; DAT_14085d168 = 0; DAT_14085d158 = 0;
  if (DAT_14085cf48) FUN_140043000(&DAT_14085cf48);   // 删作业 CS
  memset(&DAT_14085cf50,0,0x200);                     // 清 32 槽
  if (puVar3[1] != 0) FUN_1400443b0();                // SetEvent(描述符+0x08) 唤醒提交者
  return 0;                                            // 线程退出
}
```

**[观测]** 由于 id 15 的"作业"是**栈上**结构，工作线程必须只读它的 +0x08/+0x18（它确实如此），属于"约定式"而非类型安全的做法。`<Exit>` 作业的 done 回调是 `&LAB_140042eb0`（与其它 `&LAB_140042ee0` 不同），大概率就是"不释放内存（因为是栈对象）"的分支。**[推断]**（0x140042eb0/0x140042ee0 在 `dumps/functions.txt` 里没有对应函数，见 §8）

### 1.8 周期作业（10 ms 读数）

创建于 `FUN_14003d5c0`（ReadStart 处理器，`chunk_004.c:7512`）：

```c
puVar3 = (undefined8 *)FUN_140043d20(0x28,"APP: <ReadPeriodic>");
DAT_14285da10 = puVar3;              // 全局单例（便于停机时删除）
*puVar3 = 0; puVar3[1] = 0; puVar3[2] = 0; puVar3[3] = 0; puVar3[4] = 0;
puVar3[2] = FUN_140041710;           // +0x10 = 周期回调
*(undefined4 *)(puVar3 + 4) = 10;    // +0x20 = 周期 = 10 (ms)
FUN_1400392d0(puVar3);               // 插入周期链表头
```

**"10" 的来源就是这一行字面量**（10 ms）。周期字段单位是毫秒：工作线程用 `FUN_140043cc0()`（winmm `timeGetTime`，ms）比较并 `now + period` 重排；`due` 在 +0x24。**[观测]**

回调 `FUN_140041710`（`chunk_005.c:3315`，`callers=0`）：

```c
FUN_14004e9c0(&DAT_14285d420);                    // 进 RTT 临界区
iVar1 = (DAT_14285d9c4 == 0) ? FUN_140040fa0()    // 非 RTT 分支
                             : FUN_140042260();   // RTT 分支
if (iVar1 < 0) FUN_14003e680();                   // 出错 → 投递 <ReadStop> 作业
uVar2 = FUN_140043970(DAT_14285d1f0);             // 距上次的微秒数
DAT_14285d1f8 = (u32)((DAT_14085d1ec * 10000000) / uVar2);   // 采样率统计
DAT_14285d1f0 = FUN_140043990();
return 0;                                          // 0 = 继续周期执行
```

**[观测]**：周期作业回调返回 0 = 继续；`ReadStop` 是靠 `<ReadStop>` 作业 + `FUN_14003d7d0` 删除周期节点实现的（`FUN_1400392f0()` 摘链 → `FUN_140043e60(DAT_14285da10)` 释放）。
**[推导]** 采样率字段 `DAT_14285d1f8` 被状态栏读取：`FUN_140050030` → `uVar1 = FUN_14003eb00(); if (uVar1<1000) "Samples/s" else uVar1/=1000 → "kSamples/s"`，`QWidget::setText(*(QLabel**)(mw+0x38), …)`。
**[未知]** 但 `DAT_14085d1ec = 该次读入字节数 / DAT_14085d184(每帧字节数)` 已是"每次 tick 的样本数"，乘以 `1e7` 再除微秒数得到的是**真值的 10 倍**（应为 `1e6`）——要么 `DAT_14285d1f8` 约定为"1/10 单位"，要么状态栏这一栏存在 10× 显示偏差。仅影响显示统计，不影响采样。

### 1.9 队列 vs 周期表 —— 一张对比

| | 环形命令队列 | 周期作业链表 |
|---|---|---|
| 结构 | 32×0x10 定长槽（`DAT_14085cf50/58`），生产者写索引 `DAT_14085d158` | 无序双向链表 `DAT_14085d150`，插入在头 |
| 消费者 | 从 index 0 扫非空槽，FIFO | 每次循环全表扫描，选最早到期 |
| 满/慢 | 生产者 `Sleep(1)` 原地等（带进度回调/超时） | 不存在（链表可无限长） |
| 注销 | 无（执行完自释放） | 回调返回非 0 自注销，或停机时显式摘链 |
| 用途 | 用户命令（开关采集/读写工程/解析 ELF…） | 10 ms 采样泵、1 s ELF 变更检查（后者其实是 UI 定时器投递的 ring 作业） |

---

## 2. 线程模型

### 2.1 全程序只有一个 `CreateThread`

`grep CreateThread|_beginthreadex|_beginthread|QThread` 在 2001 个函数的反编译里**只命中一处**：

```c
// FUN_140043190  (chunk_005.c:4621)   唯一的线程创建包装
pvVar1 = CreateThread(NULL, 0, param_1, param_2, (param_5 & 1) << 2, &param_5);
if (pvVar1 != NULL) {
  if (param_4 != 0) FUN_140044750(pvVar1, param_4);   // SetThreadDescription(name)
  if (param_3 != NULL) *param_3 = threadId;
}
```

调用者唯一：`FUN_140039370`（`chunk_004.c:3184`）：

```c
if (DAT_14085d160 == 0) DAT_14085d160 = FUN_140043190(FUN_140039430, 0, local_res8, "WORKER", 0);
if (DAT_14085d168 == 0) DAT_14085d168 = FUN_1400430f0(0,0);     // CreateEventA
```

- **[观测]** 线程名 `"WORKER"`，`FUN_140044750` 用 `SetThreadDescription`（`LoadLibraryA("Kernel32.dll")` + `GetProcAddress("SetThreadDescription")`，`FUN_140044640`）设置，属"诊断友好"做法。
- **[观测]** 懒启动：第一次提交作业时才建线程与事件。
- **[观测]** 非 `_beginthreadex`（不使用 CRT 每线程数据）、非 `QThread`。所以：**UI 主线程（Qt）+ 1 个工作线程**，没有线程池。JLinkARM.dll 内部的线程不在本分析范围。**[推断]** 某些第三方入口（Qt 插件/系统回调）可能另有线程，但应用自身只有一条。

### 2.2 同步原语与锁清单

| 原语 | 位置 | 用途 |
|---|---|---|
| CriticalSection(+SpinCount) 或 Mutex | `FUN_140043110` `FUN_140043d20(0x30,"Mutex info")`：`param_1==NULL → InitializeCriticalSectionAndSpinCount(...,0)`，否则 `CreateMutexA` | 通用锁对象（`+0x00` 类型标志，`+0x08` 句柄/CS） |
| 上锁/解锁 | `FUN_1400443e0(lock,timeout)` / `FUN_140044370(lock)` | `timeout==-1 → EnterCriticalSection`；否则 `TryEnterCriticalSection` + 自旋 1 ms 睡眠，超时返回 `0x102` |
| 作业调度 CS | `DAT_14085cf48` | 生产者互斥（消费者不加） |
| UI 状态 CS | `MainWindow+0x318`，包装 `FUN_14004d020(mw)` / `FUN_14004d150(mw)`（各 28 个调用者） | 保护 UI 状态字段与跨线程请求结构 |
| 事件 | `FUN_1400430f0` = `CreateEventA`；`FUN_1400443b0` = `SetEvent`；`FUN_1400443d0` = `WaitForSingleObject` | 唤醒工作线程 / 工作线程等待 UI 完成 |
| 自旋锁 | `DAT_14285ec2c`（`LOCK`/`UNLOCK` 内联） | 仅堆统计（块数/字节数） |

### 2.3 工作线程 → UI 线程的更新通道

**没有** `PostMessage/SendMessage/PostThreadMessage`，**没有** `SetTimer`，**没有** `QMetaObject::invokeMethod`（三者在全量反编译里 0 命中）。**[观测]**

实际通道是 **Qt 信号 + 队列连接（AutoConnection/QueuedConnection）+ 共享状态由 CS 保护**，固定四步：

```c
// 例：FUN_14004e2c0(mw, text)  —— 写状态栏/日志文本区
FUN_14004d020(mw);                            // EnterCriticalSection(mw+0x318)
FUN_140027140(mw + 0x2c, param_2, 0x184);     // 拷入 MainWindow+0x2c 的 388 字节文本区
FUN_14004d150(mw);                            // LeaveCriticalSection
thunk_FUN_14003bd60(mw);                      // → QMetaObject::activate(mw, &DAT_140096c70, idx, 0)
```

- `thunk_FUN_14003bdxx` 是 moc 生成的信号发射体（`MainWindow` 序数 0..0x20 的方法由 `FUN_14003ba80` 的 switch 分派，`MainWindow::qt_metacall` = `FUN_14003be00`）。**[观测]**
- 连接类型在构造时写死（`chunk_006.c:3693-3714`）：`QObject::connect(mw,"2_SignalUpdateGUI()",mw,"1_SlotOnUpdateGUI()",0)`（0 = AutoConnection，跨线程自动变队列）与 `…,2`（显式 QueuedConnection）。信号清单：`_SignalClose / _SignalNewProjectDialog / _SignalOptionsDialog / _SignalShowSymbolSelection / _SignalShowCustMsgBox / _SignalUpdateProgBar / _SignalUpdateGUI / _SignalUpdateData / _SignalUpdateSymbolSelection / _SignalGetSaveFileName / _SignalAssignColors`。**[观测]**
- 因此：**工作线程写 `MainWindow` 的字段（锁保护）→ emit 信号 → Qt 把 `QMetaCallEvent` 投到 GUI 线程事件队列 → 槽在 GUI 线程读同一批字段刷新控件**。由于信号是从非 Qt 线程发出的，"可重入性"由 AutoConnection 判定保证。**[推导]**

**反向（UI 线程 → 工作线程）** 只有两条：
1. `FUN_140039100` 提交作业（§1.4）；
2. 全局标志（如 `DAT_14285d9c4`、`DAT_14085d181`、`DAT_14285da80` 等）——工作线程读，UI 写。**[观测]**

**工作线程阻塞等 UI（模态对话框）** 的模式（`FUN_14004e1f0` / `FUN_14004df50`）：

```c
lVar1 = FUN_1400430f0(0,0);              // 建事件
FUN_14004d020(mw);                       // 锁
*(mw+0x1e0)=&local_res10; *(mw+0x1e8)=text; *(mw+0x1f0)=total; *(mw+0x1f8)=lVar1; *(mw+500)=cur;
FUN_14004d150(mw);                       // 解锁
thunk_FUN_14003bda0(mw);                 // emit 信号（让 GUI 建/刷新进度框）
FUN_1400443d0(lVar1, 0xffffffff);        // WaitForSingleObject(INFINITE) —— 工作线程阻塞
FUN_140042ff0(lVar1);                    // CloseHandle
```

**[观测]** 这就是 `FUN_14004e1f0(mw, handle, "Parsing ELF file ...", total, cur)` 在**工作线程里**调用的实现（ELF 解析进度条），也是 `FUN_14003f580`→`FUN_14004df50`（错误对话框）的实现：**工作线程挂起等 GUI 用户点击**。

---

## 3. 定时器

全程序只有 **2 个 `QTimer` 实例**（`QTimer::setInterval` 只出现 2 次）+ 1 个 winmm 毫秒基准 + 若干 `Sleep` 轮询。**[观测]**

| 定时器 | 位置 | 周期 | 连接/用途 | 等级 |
|---|---|---|---|---|
| `QTimer`（`mw+0x328`） | `MainWindow` 构造 `chunk_006.c:3680-3691` | **1000 ms** | `connect(timer,"2timeout()",mw,"1_SlotOnTimeout()",0)`；槽体 = `FUN_14004d7e0`（`FUN_14003ba80` 的 case 0x1b，与 `_SlotOnTimeout` 的对应为推断）→ `if (读采集未开 && !check_pending) { DAT_14286d378=1; FUN_14003e0b0(); }` 即**每秒投递一次 `APP: <CheckELFModifications>` 作业** | 连接/槽体 [观测]，槽序号归属 [推导] |
| `QTimer`（`Dialog+0x30`） | `DIALOG_CustMsgBox` 构造 `chunk_003.c:8849-8861` | **10 ms** | `connect(timer,"2timeout()",dialog,"1_SlotOnTimer()",0)`；仅当描述符 `*(int*)(param_3+0x34) > 0`（有进度步数）时创建并启动 → 进度框步进/重绘 | [推导] |
| winmm `timeGetTime` | `FUN_140043cc0`（§6） | 系统 15.6 ms | 周期作业调度基准；`timeBeginPeriod/timeEndPeriod` 指针**被加载但无调用点** | [观测] |
| `Sleep(5)×2000` | `FUN_1400424f0`（`chunk_005.c:4017`） | 5 ms×2000 ≈ 10 s | 搜索 RTT 控制块，边搜边用 `FUN_14004e1f0` 刷进度条 | [观测] |
| `Sleep(10)` + `processEvents` 轮询 | `FUN_14004d9c0`（UI 线程等解析完成）、`FUN_14004db40`（退出握手） | 10 ms | 主线程等待工作线程状态变化（`while (DAT_14286d379) { processEvents; Sleep(10); }`） | [观测] |
| `Sleep(1)` 节拍 | `FUN_140039140` 队列满时 | 1 ms | 生产者退让 | [观测] |

**没有** `SetTimer/KillTimer/timerEvent/QTimer::singleShot/timeSetEvent`。**[观测]** Qt 定时器只在 GUI 线程跑，两个定时器都属于"UI 轮询/进度"类；真正的采集节拍在 `WORKER` 线程的周期作业上。

---

## 4. 自研带守卫堆分配器

### 4.1 四个入口

| 函数 | 签名 | 说明 |
|---|---|---|
| `FUN_140043d10(size)` | 7 字节转发 | `FUN_140043d20(size, 0)` —— 无标签（`chunk_005.c:5256`） |
| `FUN_140043d20(size, tag)` | size=310, callers=33 | **分配**：可选块头/哨兵 + 统计 + 链表登记（`chunk_005.c:5271`） |
| `FUN_140043e60(ptr)` | size=359, callers=40 | **释放**：先查共享内存表 → 校验签名 → 0xCE 清填 → 摘链 → `free`（`chunk_005.c:5338`） |
| `FUN_140043fd0(ptr, size)` | size=268, callers=1 | **再分配**（`realloc` 语义，保留 tag；`chunk_005.c:5402`） |

### 4.2 块头伪结构体（守卫模式开启时）

```c
// 实际 malloc 大小 = 请求大小 + 0x40
uint8 *base   = malloc(size + 0x40);
struct BLOCK_HEADER {          // 0x20 字节，位于 base
  /* +0x00 */ uint32  size;     //   用户请求的原始大小（未对齐）
  /* +0x04 */ uint32  magic;    //   0xDEADBEEF  —— 签名
  /* +0x08 */ const char *tag;  //   分配标签字符串指针（可为 NULL）
  /* +0x10 */ BLOCK_HEADER *next;  // 全局已分配块双向链表
  /* +0x18 */ BLOCK_HEADER *prev;
};
/* +0x20 */ 用户指针 payload = base + 0x20;
/* +0x20+size .. +0x20+size+0x1F */  32 字节 0xCE 尾部哨兵（4×u64 = 0xcececececececece）
```

关键代码（`chunk_005.c:5284-5324`）：

```c
if (DAT_14285eb9c != 0) uVar3 = param_1 + 0x40;       // 守卫模式：多要 0x40
puVar4 = malloc((ulonglong)uVar3);
if (DAT_14285eb9c == 0) return puVar4;                // 非守卫模式：裸 malloc
puVar1 = (undefined8 *)(((ulonglong)uVar3 - 0x20) + (longlong)puVar4);
*puVar1 = 0xcececececececece; puVar1[1] = 0xcececececececece;   // 尾部哨兵（写两遍=32B）
*puVar4  = param_1;                                   // +0x00 size
puVar4[1] = 0xdeadbeef;                               // +0x04 magic
*(undefined8 *)(puVar4 + 2) = param_2;                // +0x08 tag
/* 统计 + 双向链表头插 */ _DAT_14285eb90 += 1; _DAT_14285eb94 += param_1;
*(uint **)(puVar4 + 4) = DAT_14285ec30;               // +0x10 next
if (DAT_14285ec30 != 0) *(uint **)(DAT_14285ec30 + 6) = puVar4;   // old->+0x18 = new
DAT_14285ec30 = puVar4;
return puVar4 + 8;                                    // = base + 0x20
```

- **只有尾部哨兵，没有头部哨兵**；头部靠 `magic` 签名 + `size` 校验。**[观测]**
- 分配**不清零**（`RequestedSize` 处的旧内容保留）；只有 `DYNBUFFER`/`FUN_14003e050` 等调用点自己 `memset`。**[观测]**
- payload 相对 `malloc` 结果偏移 0x20，**保持 16 字节对齐**。**[观测]**
- 释放时用 `memset(payload, 0xCE, size)` 覆盖（MSVC 调试填 0xDD，SEGGER 用 0xCE）。**[观测]**

### 4.3 校验函数 `FUN_140044520`（"Debug Error" 弹窗）

```c
// FUN_140044520(block_header*)   callers = {FUN_140043e60, FUN_140043fd0}
if (param_1[1] != 0xdeadbeef) { MessageBoxA(0,"Wrong alloc signature in memory block !"," Debug Error",0x42030); return -1; }
iVar1 = 0;
do { iVar1++;
     if (*(char *)((ulonglong)*param_1 + 0x20 + (longlong)param_1) != -0x32) {   // 0xCE
        FUN_14002c8f0(local_218,0x200,"Buffer overflow in memory block. %d bytes overflowed.");
        MessageBoxA(0,local_218," Debug Error",0x42030); return -1; }
} while (iVar1 < 0x20);      // 逐字节检查 32 字节哨兵
return 0;
```

- **只在 `free`/`realloc` 前校验**，不是每次访问都查；用**逐字节**比较 32 字节（未向量化）——所以堆检查是"事后发现"，不是实时防护。**[观测]**
- 哨兵被踩 → 通过 `MessageBoxA` 弹 " Debug Error" 并**返回 -1 使 `free` 直接跳过释放**（内存泄漏但避免二次破坏）。**[观测]** 注意 `free` 侧的判定是 `if (-1 < iVar2) { …释放…; return; }`，即校验失败就不 free。
- 消息里的 `%d` 参数在反编译里丢失（`FUN_14002c8f0` 是 `vsnprintf` 的封装，应传 `iVar1`），属 Ghidra 参数恢复问题。**[未知]**

### 4.4 共享内存旁路

`FUN_140044610(ptr)` 在 `DAT_14285eba8` 表（`DAT_14285eb98` 项，每项 0x10 字节 {HANDLE, 视图地址}）里查指针；命中说明是 `CreateFileMapping/MapViewOfFile` 得来的区域，`free` 走 `UnmapViewOfFile + CloseHandle + memmove 紧缩表` 而不是 `free`，`realloc` 则直接弹 "SYS_MEM_Realloc() can not be used for shared memory !"。**[观测]**
**但全程序没有 `CreateFileMapping/MapViewOfFile` 调用**（0 命中），该表也无写入点 → **共享内存分支是死代码**（跨产品复用该内存模块的残留）。**[推断]**

### 4.5 守卫开关与统计

- `DAT_14285eb9c`（守卫模式开关）在 2001 个函数里**只有 4 处读取、0 处写入**（`globals.txt:4780` refs 全是那 4 处）→ 发布版本里守卫模式**默认关闭**，等价于 `malloc/free` 直通。开启方式不在本 dump 内（可能是调试版/注册表/调试器手工置位）。**[观测 + 推断]**
- 统计：`_DAT_14285eb90`（块数）、`_DAT_14285eb94`（总字节），在自旋锁 `DAT_14285ec2c` 下更新；**全程序无人读**（无内存用量 UI/报告）。**[观测]**
- tag 字符串：写进块头 +0x08，`realloc` 时透传（`FUN_140043d20(_NewSize, *(undefined8 *)((longlong)param_1 + -0x18))`）。**唯一用途是给调试器/事后分析看**（没有遍历链表打印的函数）。**[观测 + 推断]**
- 标签用例（抽样 125 个调用点）：`"APP: <ReadStop>"` 等 16 个作业、`"RTT read buffer allocation"`、`"DYNBUFFER: dynamic buffer"`、`"Mutex info"`、`"Project settings"`、`"Node dimension size"`、`"Temp. MsgBox string"(0x8000)`、`"CustTextView.cpp: Tmp. copy buffer"`。

### 4.6 与普通 `malloc` 的分工

| 走带守卫/带标签堆 | 走裸 `malloc` / 无标签 |
|---|---|
| 全部作业对象（0x28..0x608） | 64 MB 采样缓冲：`FUN_14003d10(0x4000000)`（`FUN_14003e050`，每个 scope 一个） |
| RTT 读缓冲 `min(0x2000, DAT_14085d1b0)` | `DYNBUFFER` **扩容时**的新块（`FUN_1400351b0` 用 `FUN_140043d10`，标签丢失） |
| 工程设置 0x5bc / 0x5e8 快照、锁对象、节点尺寸、各类临时缓冲 | 第三方/Qt 内部、CRT 内部 |

**注意分工不一致处**：`DYNBUFFER` 首次分配带标签，扩容后换成无标签块（因为 `FUN_1400351b0` 调 `FUN_140043d10`）——内存统计里会看到"标签漂移"。**[观测]**

### 4.7 DYNBUFFER（动态缓冲）自身结构

`FUN_1400352e0`(init) / `FUN_1400351b0`(ensure) / `FUN_1400353f0`(set@offset) / `FUN_1400353b0`(free) / `FUN_140035470`(reset)，另有一族 `FUN_140035480/540/5e0/640` 的**块池分配器**（8 字节对齐 `(n+7)&~7`，池块最小 4 MB，块头 0x18={data,next,used,cap}）。

```c
struct DYNBUFFER {            // 源码里自报家门："DYNBUFFER: dynamic buffer"
  void  *data;        // +0x00  缓冲区（FUN_140043d20 带标签分配）
  uint32 max;         // +0x08  容量上限（硬边界，超出报 "…out of array bounds."）
  uint32 allocated;   // +0x0c  当前已分配
  uint32 used;        // +0x10  已用（高水位，SetData 更新）
  uint32 dirty;       // +0x14  SetData 置 1
  /* +0x18 unknown */ //         reset 时清零
  int32  fillByte;    // +0x1c  >=0 时按此字节填充新空间；<0 表示不填充
  void (*onError)(const char*);  // +0x20 报错回调（可为 NULL）
};
```

- 增长策略：`new = allocated*2; if (new <= need+1) new = need+1; if (new > max) new = max;`（**倍增 + 上限裁剪**），`memcpy` 旧数据 + 按 `fillByte` 填新区。**[观测]**
- 错误码：`0` 成功，`0xfffffffe(-2)` 分配失败，`0xfffffffd(-3)` 越界（`"Debugger accesses memory in dynamic buffer at offset 0x%08X which is out of array bounds."`）——**与作业提交的返回码同一套代号**。**[观测]**

---

## 5. 错误上报与日志

### 5.1 分层的日志 API（全部以 `MainWindow*` 为汇聚点）

| 函数 | 语义 | 关键证据 |
|---|---|---|
| `FUN_14003f830(name)` | **段开始**：`depth++`；depth 从 0→1 时清空日志缓冲 `DAT_14285db80=0`、错误计数 `DAT_14285eb84=0`、记时间戳 `_DAT_14285eb88 = FUN_140043cc0()`；并输出段名 | `chunk_005.c:1807` |
| `FUN_14003e990()` → `FUN_14003e9a0(0)` | **段结束**：`depth--`；**回到 depth 0 时**：若 `DAT_14285eb84 != 0` → `FUN_14003f430()`（错误框）；否则若日志非空 → `FUN_14003f4f0(&DAT_14285db80,…)`（**把累积日志整段弹给用户看**） | `chunk_005.c:781-801` |
| `FUN_14003f110(msg)` → `FUN_1400423e0(msg,0)` | **ERROR 行**：`FUN_140028380(buf,0x800,"ERROR: %s",msg)` → 去尾换行 → `_guard_check_icall(DAT_14286d388, buf)`（回调 UI）+ 追加到 `DAT_14285db80` + `DAT_14285eb84++` | `chunk_005.c:3974` |
| `FUN_14003f1a0/FUN_14003f1e0` | **INFO 行**（level = `depth*2`） | `chunk_005.c:1405/1425` |
| `FUN_14003f280(msg)` | **WARNING 行**：`"WARNING: %s"` → 回调 + `FUN_14003f640` | `chunk_005.c:1457` |
| `FUN_140041570(level)` | **级别闸门**：`return (level <= LogLevel || LogLevel == 9)` | `chunk_005.c:3248` |
| `FUN_14004f8c0()` | 读注册表 `Software\SEGGER\J-Scope\LogLevel`（`FUN_14004efc0(&DAT_140077434,"LogLevel",…)`）；缺省 **3**，`<1→1`，`>9→9` | `chunk_006.c:6128` |
| `FUN_14003f930()` | 输出“环境信息”一块（JLink DLL 版本/编译时间 + 目标信息 `FUN_14004e9a0/…`）→ `FUN_14004e2c0(mw, text)` | `chunk_005.c:1882` |

**级别语义 = 段嵌套深度**：ERROR 行用 `depth*2-1`、INFO 行用 `depth*2` 作为 level，而 `LogLevel` 默认 3 → **depth 1 的 INFO 会打，depth 2 的 INFO 不打**。所以 `LogLevel` 实际是"日志嵌套深度上限"（9 = 全打）。**[推导]**

### 5.2 日志落地位置：**没有日志文件**

- `DAT_14285db80`（0x1000 字节）是唯一的日志容器，只有 3 个引用者（`FUN_14003e9a0`、`FUN_14003f830`、`FUN_1400423e0`）→ 只用于"段结束弹窗"。**[观测]**
- 全量字符串里没有 `*.log` / `jscope.log` / 日志文件路径；只有 `"Application log started"`（`0x1400745d8`，由 Init 处理器 `FUN_14003cc50` 用作段名）。**[观测]**
- 结论：**J-Scope 不写磁盘日志**；日志经过 UI 回调显示在 MainWindow 的文本区（`MainWindow+0x2c`，0x184 字节）并在操作结束以模态框汇总。**[推导]**

### 5.3 错误弹窗族（与日志分离）

| 函数 | 内容 | 按钮/标志 |
|---|---|---|
| `FUN_14003f580(title, subtitle, msg, …)` | 底层：`FUN_14004df50(DAT_14286d388, title+subtitle, msg, …)` → 建事件 + emit 信号 + `WaitForSingleObject` | 可直接由工作线程调用 |
| `FUN_14003f430(msg)` | `"Error"` + msg | `0x300` |
| `FUN_14003f640(msg)` | `"Warning"` + msg | `0x300` |
| `FUN_14003f490(msg)` | `"Information"` + msg | `0x400` |
| `FUN_14003f4f0(msg, btn1, btn2, …)` | 自定义双按钮（返回 1 = 第一个按钮），如 `"Reparse & Match symbols" / "Reparse & Clear symbols"` | `0x400` |
| `FUN_14004d0f0` | UI 线程真正建窗并 `QDialog::exec()` | 类 `DIALOG_CustMsgBox`（`chunk_003.c:8743` 的 vftable 赋值） |

**用法示范**（RTT 出错重试，`FUN_1400427b0`，由 `<ReadPeriodic>` 路径触发，跑在 WORKER 线程）：

```c
if ((DAT_14285d415 == 1) && (FUN_140040ed0(local_318,0x100) == -1))
    FUN_14003f580("Got pending Error from Target.",…,local_318,…);
...
iVar1 = FUN_14003f580("Error.", &DAT_14005f306, local_218 /*"…\nRetry?"*/, &DAT_140074740);
if (iVar1 != 0) return 0xffffffff;      // 非第一个按钮 → 放弃
```

**`FUN_14003f110` 不是弹窗**——它只写日志行 + UI 回调；弹窗入口是 `FUN_14003f580`。**[观测]**

### 5.4 崩溃/异常处理：无自定义处理器

- `SetUnhandledExceptionFilter` 仅出现在 CRT 里：`__raise_securityfailure`（`0x1400590d4`，"Libraries: Visual Studio 2015/2017/2019 Release"）→ `TerminateProcess(…, 0xC0000409)`；`FUN_140059798`（CRT `__report_gsfailure` 家族：`RtlCaptureContext/RtlVirtualUnwind/IsProcessorFeaturePresent`）；`FUN_140059980` 装 `FUN_140059990`（C++ `terminate` 前的 `0xE06D7363` 判定）。`chunk_008.c` 的 `_seh_filter_exe` 亦然。**[观测]**
- **没有** 应用自定义的 minidump / crash report / `AddVectoredExceptionHandler` / `SetErrorMode`。**[观测]**
- `__security_check_cookie` 出现在 **141 个函数**（约 7%），分布：chunk_003=50、chunk_005=64、chunk_006=68、chunk_004=30、chunk_007=21、chunk_001=34…**[观测]** 含义：`/GS` 开着，但只有**含局部缓冲/数组的函数**才有 cookie；chunk_005/006（应用逻辑与 UI）占比高，说明应用代码大量使用栈上固定缓冲（例如 `local_818[2048]`、`local_418[1024]`、`local_318[256]`）——这是"用固定缓冲 + `_s` 版本 API"的 C 风格编码，也解释了为什么 `FUN_140028380(dst, n, fmt, …)` 这种 `_snprintf_s` 封装被调用了 38 次。

### 5.5 内存不足路径

`FUN_140043d20` 分配失败 → `"Failed to allocate 0x%08X bytes of memory!"` + `MessageBoxA(NULL, msg, "Error", 0x42010)`（**直接 Win32 弹窗，绕过 Qt**，可能在非 UI 线程弹出）；作业创建失败统一走 `FUN_14003f430("Insufficient memory!")`（30 处）。**[观测]**

---

## 6. 时间基准

### 6.1 高精度：QPC → 微秒

```c
// FUN_140043990  (chunk_005.c:5082)  —— 8 个调用者
QueryPerformanceCounter(local_res8);
if (DAT_14285edb0.QuadPart == 0) DAT_14285edb0 = local_res8[0];      // 首调用捕获基准
return local_res8[0].QuadPart - (longlong)DAT_14285edb0;             // 相对 tick

// FUN_140043970(base)  (chunk_005.c:5067)  —— 5 个调用者
lVar1 = FUN_140043990();
FUN_140043050(lVar1 - param_1);                                      // 转微秒

// FUN_140043050(ticks)  (chunk_005.c:4560) —— 唯一的频率换算点
if (DAT_14285ec3c == 0) {                     // 懒初始化
   DAT_14285ec3c = 1;
   QueryPerformanceFrequency(&DAT_14285eda0);         // 频率(qword)
   _DAT_14285edac = (float)DAT_14285eda0;             // 频率(float)
   DAT_14285eda8 = _DAT_14285edac / 1e+06;            // ticks per µs (float)
}
fVar2 = (float)param_1 / DAT_14285eda8;               // ticks / (ticks/µs) = µs
return (longlong)fVar2 + 溢出补偿;                     // 带 2^63 边界补偿
```

**[观测]** 要点：
- **全局唯一基准** `DAT_14285edb0`（首个 `FUN_140043990` 调用时捕获）→ 返回值天然是"进程运行 tick 数"，**不存在跨基准比较问题**，也把"QPC 频率全局"从"每次调用查频率"简化成一次性 `DAT_14285eda0/eda8/edac`。
- 换算只用 **float 单精度**（10 MHz 频率下 `ticks/µs = 10.0` 精确；`ticks/(freq/1e6)` 在 float 精度内对 ms 级间隔足够）。**没有 100 ns 单位**，最小实用单位是 **µs**，超过 2^24 tick（约 1.7 s @10MHz）后 float 的有效位开始丢精度。**[推导]**
- **tick 溢出**：QPC 是 64 位计数器不会在进程寿命内回绕，所以 `FUN_140043990` 不做回绕处理；`FUN_140043050` 只做了 `float` 转 `int64` 的 2^63 溢出补偿（`fVar2 = 9.223372e18 …`），属于编译器生成的 `(long long)(float)` 边界代码。

### 6.2 毫秒节拍：winmm `timeGetTime`

```c
// FUN_140043cc0  (chunk_005.c:5231)  —— 周期作业调度的唯一时钟
if (DAT_14285ed30 == 0) FUN_1400446e0();            // LoadLibraryA("Winmm.dll")
if (DAT_14285ed48 == 0) iVar1 = 0; else iVar1 = (*DAT_14285ed48)();   // timeGetTime()
if (DAT_14285edb8 == 0) DAT_14285edb8 = iVar1;      // 首次基准
return iVar1 - DAT_14285edb8;                        // 相对毫秒
```

`FUN_1400446e0` 一次性 `GetProcAddress` 三个符号：`timeBeginPeriod`(+0x08)、`timeEndPeriod`(+0x10)、`timeGetTime`(+0x18)。**[观测]**
- `timeBeginPeriod/timeEndPeriod` **被解析但没有任何调用点**（`globals.txt` 中 `14285ed38/14285ed40` 无符号、无引用）→ 系统定时器分辨率仍是默认 15.6 ms；`period = 10` 的周期作业实际会以 ≥15.6 ms 的粒度触发（除事件唤醒带来的偏移抖动）。**[观测 + 推导]**
- **tick 回绕**：`timeGetTime` 是 32 位毫秒（49.7 天回绕）。代码用**有符号差**处理：周期作业比较 `(int)(due - now) < 1`，等待时长 `min_wait - now`（`chunk_004.c:3298/3326`）；`FUN_140043cc0` 本身返回 `int`（`iVar1 - DAT_14285edb8` 允许为负）→ 回绕在算术层面自然正确。**[观测]**
- 与 QPC 的分工：**ms 用于调度节拍（便宜、粗）**，**µs 用于测量（超时、进度、采样率统计）**。

### 6.3 其它时间点

- `Sleep(1)` 节拍对齐（队列满时）、`Sleep(5/10)` 轮询（RTT 搜索、等待解析完成/退出）。
- 日志时间戳：`_DAT_14285eb88 = FUN_140043cc0()`（段开始的 ms 时刻）。**[观测]**
- UI 侧的时间：数据帧计数 `DAT_14285da80`（0..0x7FFFFF 环形回绕 + `DAT_14285da78` 记录被覆盖的样本数）——**用"样本索引"而不是时间戳做横轴**（这也是 `FUN_14003ead0` 里 `100.0 * 1.19e-7` 那种定点数魔法出现的地方）。**[观测]**

---

## 7. 未知点

1. **`DAT_14285eb9c`（守卫堆开关）无写入点**：[观测] 事实成立；谁/何时打开（注册表？调试版？被 strip 的初始化函数？）——[未知]。
2. **`0x140042eb0` / `0x140042ee0`（作业 done 回调）不是独立函数**（`dumps/functions.txt` 里是空洞），只能从用法推断：`0x140042ee0` 组 ≈ `FUN_140040e10`（释放作业）；`0x140042eb0`（`<Exit>` 用）应不释放。[推断/未知]
3. **处理器表 16 项中的 2/6/7/8/9/11/14 尚未逐个确认**（候选 `FUN_14003ce50`、`FUN_14003dcb0`、`FUN_14003dd90`，均 `callers=0`）。表内容需 dump `0x140073ef0..0x140073fef` 的 16 个指针。
4. **`DAT_14285d1f8` 采样率的 ×1e7 因子**：与"每次 tick 样本数 / 微秒数"的理论值差 10 倍；无法确定是 `DAT_14085d1ec` 的单位特殊，还是状态栏存在 10× 显示偏差。[未知]
5. **`FUN_14001a510`（QPC 用到的 `FUN_140043050` float 路径）在多长时间间隔后精度不足**：未做量化测试。[未知]
6. **工作线程和 JLinkARM.dll 回调线程的关系**：JLink 的 `JLINK_*` 回调（如 `FUN_14003de20` "SetRTTSearchRanges"、`FUN_14003d950` 等以指针注册的函数）在哪个线程被调用未确认；若是 DLL 内部线程，则 UI 更新路径可能不止 WORKER 一条。[未知]
7. **`FUN_14003e9a0` 在 `FUN_14003f430()` 处参数丢失**（应为日志文本），确认"有错时弹的是累积日志还是单条错误"。[未知]
8. **`FUN_1400392f0` 的实参丢失**（Ghidra 显示无参调用 `FUN_1400393d0()`）：是"摘 head"还是"摘指定节点"。[未知]

---

## 8. 对 OpenScope 的启示

1. **抄：单工作线程 + 32 槽命令环 + id→处理器表 + 完成回调自销毁。**
   OpenScope 的设备动作（开始/暂停/停止采集、解析 ELF、上传/下载变量表、保存工程、检查 ELF 变更）与这套 id 表几乎是 1:1。收益：**串行化免费获得**（一个 worker，任意两个设备操作不会并发）、**生命周期简单**（作业由 worker 自释放，创建者不关心谁管内存）、**UI 只投递不等待**。注意补上 SEGGER 的缺陷：**队列满时不要无限等**（加"队列满即失败"或可增长队列），并给作业加**真实撤销**（SEGGER 的"取消/超时"是假的——作业照样执行）。

2. **抄：所有跨线程 UI 更新走"锁内写状态 + emit 队列信号"。**
   OpenScope 是 C11 + 自绘/Qt，无论哪种，'worker 只写状态结构 + 用一次 PostMessage/信号唤起 UI，UI 线程读同一结构渲染' 这条纪律都值得照搬；**绝不要**让 worker 直接碰控件。SEGGER 还额外做了两件值得学的事：
   - 反向"worker 阻塞等 UI"（进度条 `FUN_14004e1f0`：建事件 → 发信号 → `WaitForSingleObject` → 关闭句柄），让长任务能显示进度又不用回调套回调；
   - 退出时的确定性握手（`DAT_14286d37c` 状态机 + 退出作业 + 事件等待 + 逐级 `CloseHandle`），避免"关窗后 worker 还在跑"。

3. **抄：可开关的带守卫堆（标签 + 0xDEADBEEF + 32 字节尾哨兵）。**
   增量成本仅 0x40 字节/块、payload 仍 16 字节对齐、`free` 时逐字节验哨兵并弹诊断信息；标签字符串在"多缓冲区 + 多线程"的项目里是**定位泄漏/越界最快的线索**。OpenScope 已有 JLink 块读/写与多缓冲（HSS/RTT/网络镜像），建议**默认开启守卫模式**（SEGGER 默认关闭，等于白写），并把 `free` 时校验改成"每次释放都校验 + 记录首个越界块的 tag"。

4. **抄：日志的"段嵌套 + 深度即级别 + 段末汇总弹窗"。**
   `log_section_begin("Parse ELF")` → 期间 ERROR/WARNING/INFO 累积到内存缓冲并计数 → `log_section_end()` 时"有错弹错误框 / 无错但有输出弹信息框"（级别由注册表 `LogLevel` 控制在多深还打）。这比"每个错误一个 MessageBox"体验好得多，也天然产生**可复制的操作日志**（OpenScope 的 request.md 驱动开发流程 + 网络 E2E 脚本正好需要这个：一次自动化操作导出一段日志文本）。

5. **抄：双时钟分工（QPC→µs 用于测量、winmm ms 用于调度）+ 首调用捕获基准。**
   `FUN_140043990/970/050` 这三层（相对 tick / 相对 µs / 频率换算一次性缓存）干净且便宜，OpenScope 的采样率统计、超时、进度都该用它；但**别学** SEGGER 的 `timeGetTime` 做周期调度（15.6 ms 分辨率 + 加载了 `timeBeginPeriod` 却不用）——OpenScope 要么 `timeBeginPeriod(1)`，要么直接用 QPC 做等待计算并 `Sleep` 到目标时刻。

6. **别抄：**
   - 用全局单例 + 一把大锁串起"应用状态"（`MainWindow+0x318` 保护几十个字段、`DAT_14285d420` RTT 状态 0x5bc 字节全局）。OpenScope 的 `net_drive`（多实例、跨机）会直接撞车——应按"实例/会话"对象化，锁粒度按对象而不是全局。
   - 把"栈上描述符"塞进跨线程队列（id 15 的停机作业）；OpenScope 里一旦有超时/重试就会踩悬垂指针。
   - `_guard_check_icall(DAT_14286d388, msg)` 式的"全局对象指针当函数指针调"写法，以及 `MessageBoxA` 与 Qt 弹窗混用（OOM 时弹出 Win32 框，可能在非 UI 线程）。
   - 用样本索引当时间轴（`DAT_14285da80` + `DAT_14285da78`）而不记录时间戳——多速率/多实例合并数据时会很难对齐（OpenScope 的 ELF 变量 + 网络远端数据尤其明显）。

---

## 9. 需要补充反编译

1. **处理器表内容**：`0x140073ef0 .. 0x140073fef`（16 个 0x10 字节条目）——需要把该只读区当作数据 dump 出来，才能把 16 个 id 全部对齐到函数地址（当前只有第 0 项 `= 0x14003cc50`）。
2. **`0x140042eb0` / `0x140042ee0`**：被当作 `job->done` 调用但未被 Ghidra 识别为函数（`functions.txt` 里 140042e76..140042ef0 是空洞）→ 需要强制反编译，确认"释放 vs 不释放"。
3. **`FUN_14003ce50` / `FUN_14003dcb0` / `FUN_14003dd90`**（均 `callers=0`）：判断它们分别是哪个 id 的处理器（2/6/7/8/9/11/14）。
4. **`FUN_1400392f0` 的实际参数**（Ghidra 丢失）：确认周期作业摘链是"摘 head"还是"摘指定节点"。
5. **`DAT_14285eb9c` 的写入者**：建议在 `JScope.exe` 全映像里搜 `14285eb9c` 的 32 位相对寻址（`mov [rip+disp]`），若确实没有写入者则确认守卫堆为"编译期开关 + 发布版关闭"。
6. **`FUN_1400423e0` 里 `_guard_check_icall(DAT_14286d388, buf)` 的真实调用形态**：需要看反汇编（是 `call qword ptr [DAT_14286d388]` 还是 `mov rax,[DAT_14286d388]; call [rax+off]`），以确定"日志回调"到底是全局函数指针还是 `MainWindow` 的虚函数/信号。
7. **`<Init>` 处理器 `FUN_14003cc50` 中 `FUN_140041cd0((int)plVar2[1], *plVar2, 2)` 的语义**（命令行参数解析？）——与作业系统的边界有关，未深挖。
8. **`FUN_14004e1f0` / `FUN_14004df50` 的 UI 侧配对槽**（MainWindow 里消费 `mw+0x1e0/0x1d0` 的槽函数）：确认 GUI 端何时 `SetEvent` 解除工作线程阻塞（对应的 `FUN_14004dc70/4dd70/4df50` 的对称函数）。
