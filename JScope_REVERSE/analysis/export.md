# JScope 9.66 专题逆向：数据导出 / 导入（CSV、JRAW、Import Raw、DataFile）

分析对象：`C:\Program Files\SEGGER\JLink_V966\JScope.exe`（PE32+ x64，基址 `0x140000000`，679,920 B）
证据来源：`dumps/alldecomp/chunk_000..008.c`（Ghidra 12.1.4 反编译）、`dumps/defined_strings.txt`、`dumps/globals.txt`；
少量**格式串/表项**因 Ghidra 未识别为字符串，改用只读方式直接读取 exe 对应 RVA 的原始字节 + capstone 反汇编补充（未运行 Ghidra、未改动 dumps）。

证据分级：**[观测]**＝反汇编/原始字节直接可见；**[推导]**＝由观测确定性推得；**[推断]**＝合理但未证实；**[未知]**＝本轮未解决。

---

## 1. 概述：整条链路与关键地址

```
菜单动作描述表 FUN_140001000                     (chunk_000.c:2)
  "Export &Data ..."  Ctrl+D  → 槽 1_SlotOnActionExportData  (void)
  "Export &Raw ..."   Ctrl+R  → 槽 1_SlotOnActionExportRaw   (void)
  "Impor&t Raw"       Ctrl+T  → 槽 1_SlotOnActionImportRaw   (void)

Qt 元对象槽名（0x140072c00 "MenuBar" 之后）
  _SlotOnActionExportData()  0x140072ce3
  _SlotOnActionExportRaw()   0x140072cfd
  _SlotOnActionImportRaw()   0x140072d16

MenuBar::qt_metacall 分派 FUN_14003beb0 (chunk_004.c:5814)
  case 8  → thunk_FUN_140047390   = CSV 导出
  case 9  → thunk_FUN_1400476d0   = JRAW 导出
  case 10 → FUN_14002a190 → FUN_14004cf60 = Import Raw 文件对话框

作业表 0x140073ef0（{函数, 作业名} 对）
  ...
  {0x14003d9a0, "Import Raw"}
  ...
0x14003d9a0 是一段 13 字节尾调用垫片：
  48 8b 49 08        mov  rcx,[rcx+8]
  48 83 c1 20        add  rcx,0x20
  e9 d3 94 00 00     jmp  0x140046e80      ← 真正的解析器
```

**核心结论（一句话）**：J-Scope 的导出只有两条路径——**CSV（文本）**与**JRAW（无头裸样本流）**；
两者都从内存环直接写出，命令名、格式串、顺序、上界均已逐字节确认。JRAW **没有任何头部/魔数**，
导入时靠「当前工程的变量表」还原布局，因此导出的 `.jraw` 离开 `.jscope` 工程即不可解释。

`.jraw` 的导入对话框标题是 **"Open JRAW file"**、过滤器是 **`JRAW (*.jraw)`**（`FUN_14004cf60`）；
题面提到的 `Import Raw` / `Binary file (*.raw)` 确实存在于二进制，但**只作为 .rdata 表中的数据**出现
（见 §6），任何已反编译函数都没有引用它们。

---

## 2. CSV 导出 `FUN_140047390 @ 0x140047390`（781 B）

### 2.1 流程 [观测]

```c
cfg = malloc(0x5bc);  InitCfgFromGlobal(cfg);            // FUN_140043d10 / FUN_14004e9c0
FUN_14004f820(path, 0x100);                              // 默认名 = INI [DataFiles] 上次用过的路径
FUN_1400368e0(&dlg, path, 0x100, "Export Data", dir, "CSV (*.csv)", 0);  // QFileDialog::getSaveFileName
if (path[0] == 0) return 0;                              // 用户取消
FUN_14004f9b0(path);                                     // 回写 INI [DataFiles]
strncpy_s(cfg+0x184, path, 0x104);                       // cfg.DataFile = 路径
FUN_14004ec40(cfg);                                      // 提交回全局配置(0x5bc memcpy)
fopen_s(&fp, path, "w");                                 // ★ 文本模式
if (!fp) { Error("File could not be opened!"); }
else if (varList != NULL) { ...写表头+数据...; fclose(fp); return 0; }
return -1;                                               // ★ 变量表为空:未 fclose,直接 -1
```

- 对话框标题 `"Export Data"`（0x1400759d8），过滤器 `"CSV (*.csv)"`（0x1400759c8），默认名取自
  `JScopeSettings.ini` 的 `[DataFiles]` 段（见 §5）。[观测]
- `fopen_s(..., "w")` **文本模式**：Windows 下 `"\n"` 会被 CRT 翻成 `\r\n`。**不写 BOM**（无 `fwrite`/`fwprintf` 宽字符调用）。[观测]
- 程序内**无任何 `setlocale`**（全 dumps 与字符串表中 0 命中），C 区域设置为 "C" → **小数点是 `.`**。[观测/推导]

### 2.2 表头 [观测]

```c
fputs("Timestamp", fp);                                  // 0x140075a08
for (n = varList; n; n = n->next) {
    fputs(";", fp);                                      // 0x140075a0c, 字面量 ";"
    fputs(*(char**)(*n + 0x108), fp);                    // ★ NameWithParents(指针, 见下)
}
fputs("\n", fp);
```

- **分隔符是分号 `;`，不是逗号**（尽管扩展名是 .csv）。[观测]
- 列名取 `var + 0x108` 的**指针**指向的字符串，即 `NameWithParents`（全路径名）。
  该字段在 ELF 载入时构造（`FUN_140044d50`，chunk_005.c:6266）：
  ```c
  if (找不到 "NameWithParents" 子节点) *(longlong*)(var+0x108) = var + 4;   // 退化指向 +4 的短名
  else { len = strlen+1; p = malloc(len); *(void**)(var+0x108) = p; ... }   // 堆上的全路径名
  ```
  即 `+0x004` 是定长短名（内联），`+0x108` 是**指向全路径名字符串的指针**。[观测]
- 表头只在变量链表非空时写；行循环里若某节点 `*n == 0` 则跳过该变量（不写该列）。[观测]

### 2.3 行格式与数值格式化 [观测]

反汇编（capstone，`0x1400474f2` 起）逐条确认：

```
call FUN_14003ecb0      ; eax = writeIdx
sub  eax,1              ; eax = writeIdx-1
je   skip               ; writeIdx==1 时整个数据区跳过
mov  r14d,eax           ; 计数器 = writeIdx-1
loop:
  call FUN_14003edd0            ; rax = &tsRing[0]            (0x14085d1f0)
  mov  r8d,[rsi+rax]            ; ts = *(uint32*)(tsRing + i*4)   ← 物理下标
  fprintf(fp, "%u", ts)         ; 0x14005f57c = "%u"
  for (n = varList; n; n = n->next) {
      switch (*(int*)(v+0x314)) {          // TypeID
      case 1:  xmm = cvtps2pd (float)  ; fprintf(fp, ";%f", (double)f)   // 0x140075a18 = ";%f"
      case 2:  xmm = movsd    (double) ; fprintf(fp, ";%f", d)
      case 5: case 6:  r8d = movzx byte  ; fprintf(fp, ";%i", ...)       // 0x140075a1c = ";%i"
      case 7:          r8d = movzx word  ; ...
      case 8: case 12: r8d = mov dword   ; ...
      case 10:         r8d = movsx byte  ; ...
      case 11:         r8d = movsx word  ; ...
      default:         /* 什么都不写 */     // TypeID 3,4,9,13 整列被静默丢弃
      }
  }
  fputs("\n", fp);
  i++;  tsPtr += 4;  dataPtr += 8;  while (--counter);
```

| 项 | 结论 | 级别 |
|---|---|---|
| 时间戳格式 | `"%u"`（uint32 微秒，**无前导分隔符**） | [观测] 0x14005f57c＝`%u` |
| 浮点格式 | `";%f"` → 默认 6 位小数（如 `1.234567`） | [观测] 0x140075a18＝`;%f` |
| 整型格式 | `";%i"` → 有符号十进制；**16/32 位无符号类型也按 `%i` 打印**，>2^31 显示为负 | [观测] 0x140075a1c＝`;%i` |
| 类型映射 | float32→double 后再 `%f`；float64 直接 `%f`；1/2/4 字节按 TypeID 选择零/符号扩展 | [观测] |
| 不支持类型 | TypeID 3/4/9/13（含 uint64/int64）**整列不输出**，行内该位置无任何字符 | [观测] |
| 行尾 | `"\n"`（fputs，文本模式下实际落盘为 CRLF） | [观测/推导] |
| 一行的样子 | `123456;0.500000;-3;42` | [推导] |

`";%f"` 的**分号在格式串里**，所以每个值列都自带分隔符，时间戳列不带——表头因此也必须手写 `";"`。[观测]

### 2.4 导出范围与顺序（**容易搞错，重点**）[观测]

- 数据区上界 = `FUN_14003ecb0()` 返回的 **writeIdx**（`DAT_14285da80`，"下一个待写槽位"），
  实际写 **`writeIdx - 1` 行**，即逻辑上"最后写入的那个样本被排除"，索引为 `0 .. writeIdx-2`。
- 取值用 **物理下标**：`tsRing[i*4]`、`var->dataBuf[i*8]`，`i` 从 0 递增，**没有做环形重映射**。
  因此在环**已经绕回**之后（writeIdx < 0x800000），导出的是「绕回点之后写的那 `writeIdx-1` 个样本」，
  而不是整环，也不是从最老样本开始。
- **可见范围/缩放与导出无关**：与 `FUN_140047f50`（X 轴拖动条）无任何耦合。

> ⚠️ 顺带发现一个真实缺陷 [观测]：`sub eax,1; je skip` 是 32 位比较。若恰好在 `writeIdx == 0`
> 时导出（环刚绕满一圈、或清零后立刻导出），`0-1` 得到 `0xFFFFFFFF`，循环将跑 **4,294,967,295 次**，
> 越界读取环形缓冲。OpenScope 若照抄这段逻辑必须修正。

---

## 3. JRAW 导出 `FUN_1400476d0 @ 0x1400476d0`（633 B）

### 3.1 流程与写入顺序 [观测]

capstone 逐条确认（`0x1400477d1` 起为数据区）：

```
call FUN_14003ed20            ; n = 有效样本数 = lastIdx(DA7C)，若已绕回则为 0x800000
if (n == 0) 跳过数据区
i = 0
loop:
  call FUN_14003ed70(i)       ; eax = 时间戳(环形逻辑下标, 已做绕回重映射)
  mov  [rsp+0x48], eax
  fwrite(&ts, 4, 1, fp)       ; ★ 主机字节序(小端)的 uint32
  for (n2 = varList; n2; n2 = n2->next) {
      v   = *n2
      p   = FUN_14003ece0(v, i)         ; = v->dataBuf(+0x3d0) + 逻辑下标*8
      sz  = *(uint32*)(v + 0x31c)       ; 变量字节数
      fwrite(p, sz, 1, fp)              ; ★ 从 8 字节槽里取前 sz 字节
  }
  i++;  while (i < n)
fclose(fp);
// 然后只用消息框"说明"格式：
snprintf(buf,0x400, "Raw data exported in the order:\nTimestamp");  // 0x140075a38
for (n = varList; n; n = n->next) {
    strncat(buf, "\n", 0x400);            // 0x14006e2bc = "\n"
    strncat(buf, (char*)(*n + 4), 0x400); // ★ 短名在 +0x004 内联数组
}
MsgBox(buf, "OK", 0, 0);                  // FUN_14003f4f0, 按钮串 0x14006cc0c = "OK"
```

**文件里没有任何头部**：没有魔数、没有版本、没有变量个数、没有变量描述、没有 padding。[观测]
唯一的"格式说明"是导出完成后弹出的 OK 消息框文字，这就是 0x140075a38 那条字符串的用途。[观测]

- 文件打开模式 `fopen_s(..., "wb")`（二进制，不做 CRLF 转换）。[观测]（"wb" 位于 0x140075a30）
- 写失败检查只有 `if (!fp) Error("File could not be opened!")`；无磁盘满/写错误分支。[观测]
- 变量顺序 = **变量链表的挂接顺序**（`DAT_14286d3b8`），即变量树/选择顺序，
  **不是按地址排序**。同一个顺序也是 HSS 下发时算样本内偏移（`v+0x38C`）的顺序，
  因此 JRAW 的样本布局 **等价于探针 HSS 的样本流**。[观测/推导]

### 3.2 精确布局与伪结构体

```c
/* ---- JRAW 文件（无头） ------------------------------------ */
/* 全文件 = N 个紧密排列的样本，N = fileSize / sampleSize    */
/* sampleSize = 4 + Σ(v->size)   （顺序同变量链表）           */

typedef struct JrawSample {          /* 小端 */
    uint32_t  timestamp_us;          /* +0x00  来自 tsRing（HSS/RTT 目标给的 µs） */
    /* 紧跟下列各变量的原始字节，按变量链表顺序紧密排列： */
    uint8_t   var0[var0.size];       /* +0x04 */
    uint8_t   var1[var1.size];
    ...                              /* 无对齐、无填充 */
} JrawSample;

/* 单变量编码 = 该变量在目标内存中的原始字节：
   float32→4B IEEE754 LE, float64→8B, uint8/int8→1B,
   uint16/int16→2B, uint32/int32→4B。(uint64/int64 由 §8 TypeID 9/13 表示,
   虽然树上被禁用, 但导出这里不做过滤, 只要在变量表里就会按 size 原样写出) */
```

| 字段 | 偏移 | 类型 | 说明 |
|---|---|---|---|
| 时间戳 | +0x00 | u32 LE | 微秒，来自 `tsRing`；HSS 与"RTT 带时间戳"模式为原值，RTT 无时间戳模式是 +100µs 合成值 |
| 变量数据 | +0x04 … | 原始字节 | 依次为 var0..varN（size 取自 `var+0x31C`） |
| 头部 | — | — | **不存在** |
| 对齐 | — | — | 无，紧密排列 |
| 字节序 | — | — | 主机字节序（x64 = 小端） |
| 变量名字段 | — | — | **文件里没有变量名**；仅存在于导出后的消息框 |

### 3.3 与 CSV 的取值差异 [观测]

| | CSV `FUN_140047390` | JRAW `FUN_1400476d0` |
|---|---|---|
| 上界 | `writeIdx - 1` | `lastIdx`；已绕回时 `0x800000`（整环） |
| 下标映射 | **物理下标**（不走重映射） | **逻辑下标**（`FUN_14003ed70`/`FUN_14003ece0` 内部做 `(oldest + i) mod 0x800000`） |
| 绕回后的含义 | 绕回后写入的那批（最近的 N 个）样本 | **整环、从最老样本到最新样本**（时间递增） |
| 末尾样本 | 最新一个样本不导出 | 最新一个样本不导出 |

两者在**未绕回**时等价（`writeIdx-1 == lastIdx`），绕回后语义不同。**最新那个样本两种导出都会丢掉。** [观测]

---

## 4. JRAW 导入（"Impor&t Raw"）

### 4.1 UI 入口 [观测]

```c
/* MenuBar slot case 10 → FUN_14002a190 →  */
void FUN_14004cf60(void) {                       // 0x14004cf60, 181 B
    memset(filterbuf,0,0x104); FUN_14004f820(filterbuf,0x104);   // INI [DataFiles] 默认名
    iVar = FUN_140036720(mainWin, out, 0x104, "Open JRAW file"  /*0x140076cf8*/,
                         filterbuf, "JRAW (*.jraw)" /*0x140075a20*/, 0);
    FUN_14004f9b0(out);                          // 回写 INI
    if (iVar >= 0) FUN_14003e210(out);           // 投递作业
}
```

- 参数顺序由 `FUN_140036720` 函数体（`QFileDialog::getOpenFileName` 的 7 参调用）反推：
  `(parent, outBuf, outLen, caption, dir, filter, selectedFilter)`。[推导]

### 4.2 作业投递 [观测]

```c
/* FUN_14003e210 */
job = FUN_140043d20(0x128, "APP: <ImportRaw>");
memcpy(job+0x20, path, 0x104);      // 路径存在作业体 +0x20
*(u32*)(job+0x18) = 0xb;            // 作业类型 11
job->worker = &LAB_140042ee0;       // 统一分派器
PostJob(job);                       // FUN_140039100
```

分派器 `LAB_140042ee0`（0x140042ee0）**未在 dumps 中**，但作业表 0x140073ef0 中 `"Import Raw" → 0x14003d9a0`，
而 0x14003d9a0 的机器码是 `mov rcx,[rcx+8]; add rcx,0x20; jmp 0x140046e80` —— 
即**作业体 +0x20 的路径字符串直接传给解析器 `FUN_140046e80`**。[观测（原始字节+capstone）]

### 4.3 解析器 `FUN_140046e80 @ 0x140046e80`（405 B）[观测]

```c
uint32_t sampleSize = 4;
for (n = varList; n; n = n->next) sampleSize += *(int*)(*n + 0x31c);

fopen_s(&fp, path, "rb");
if (!fp) { ErrorMsg("File could not be opened!"); return -1; }

fseek(fp,0,SEEK_END); uint32_t fileSize = ftell(fp); fseek(fp,0,SEEK_SET);

if (fileSize % sampleSize != 0) {                      // ① 对齐校验（唯一"格式校验"）
    snprintf(buf,0x400,
      "File size does not align with selected symbols!\n"
      "File size: %d Bytes\n"
      "Size of selected symbols + timestamp: %d Bytes",
      fileSize, sampleSize);                           // 0x140075a70
    MsgBox(buf);                                       // ★ 只提示, 不中止
}
int32_t nSym = FUN_14003ed50();                        // = 变量个数 DAT_14085d1e8
if (nSym * 0x4000000 + 0x2000000U < fileSize) {        // ② 缓冲区容量校验 (n×64MB + 32MB)
    MsgBox("File size exceeds symbol buffers!\n Excess data will be discarded.");  // 0x140075af0
}
void *p = malloc(fileSize); memset(p,0,fileSize);
fread(p, 1, fileSize, fp);
FUN_14004ded0(mainWin, fileSize - fileSize % sampleSize, p);   // 去掉不整除的尾部再解包
fclose(fp);
```

- **没有魔数校验**（文件里也没有魔数可校验）。"不匹配"的报错就是上面 ①，**提示后继续执行**，
  尾部多余字节被丢弃（`size - size % sampleSize`）。[观测]
- ② 的上界 `nSym*64MB + 32MB` 顺带确认了内存模型：**每变量环 64 MB、时间戳环 32 MB**（与既有报告 §6 一致）。[观测]
- 整个文件一次性读入内存（`malloc(fileSize)`），没有流式/分块处理。[观测]

### 4.4 进入数据模型：回填双环 + 写指针复位 [观测]

```c
void FUN_14004ded0(void *parent, uint32_t nBytes, void *data) {   // 0x14004ded0, 67 B
    FUN_14004d020();          // LockCriticalSection(*(parent+0x318))  FUN_1400443e0
    FUN_14003de60(nBytes, data);
    FUN_14004d150();          // Unlock                           FUN_140044370
}

void FUN_14003de60(uint32_t nBytes, void *data) {                 // 0x14003de60, 161 B
    memset(&tsRing, 0, 0x2000000);          // 清 32 MB 时间戳环
    FUN_14003e050();                        // 逐变量: 若 dataBuf==0 则 malloc(0x4000000); memset(0,64MB)
    FUN_140040980();                        // ★ 复位索引 & 通知 UI (见下)
    if (RTT模式 && 无时间戳) {                // DAT_14285d9c4!=0 && DAT_14285d9c5==0
        DAT_14285d9c5 = 1;                  // 临时强制"有时间戳"
        FUN_140042a60();                    // 重算 v->sampleOffset(+0x38C) = 4,4+s0,...
        DAT_140096dd0 = 0;
        FUN_14003fa70(nBytes - nBytes%sampleSize, data);   // ★ 与实时采样同一个解包器
        DAT_140096dd0 = 1;
        DAT_14285d9c5 = 0;
        FUN_140042a60();                    // 还原
        return;
    }
    FUN_140042a60();  DAT_140096dd0 = 0;
    FUN_14003fa70(nBytes - nBytes%sampleSize, data);
    DAT_140096dd0 = 1;
}

void FUN_140040980(void) {                  // 0x140040980, 134 B
    for (n = varList; n; n = n->next) {
        if (v->dataBuf) memset(v->dataBuf, 0, 0x4000000);
        v->writePtr(+0x3c8) = 0;            // 每变量写指针
        v->hasData(+0x388)  = 0;
    }
    DAT_14285da80 = 0;   // writeIdx   ← ★ 写指针复位
    DAT_14285da84 = 0;   // curTs
    DAT_14285da78 = 0;   // oldest 索引
    DAT_14285da7c = 0;   // lastIdx
    FUN_14004e120(mainWin);   // = Lock + thunk_FUN_14003bd60(UI 刷新) + Unlock
}
```

**回答题面问题**：
1. **是回填时间戳环 + 各变量环**（正是 §6 的双环），并非新建数据模型。[观测]
2. **写指针**：先由 `FUN_140040980` 全部清零，再交给 `FUN_14003fa70`（实时采样同一函数）
   从索引 0 开始**线性递增**，导入结束时 `writeIdx = N mod 0x800000`、`lastIdx = N-1`。[观测]
3. **时间戳永远按文件里的 4 字节处理**：RTT 无时间戳模式下会临时把 `DAT_14285d9c5` 置 1，
   使 `FUN_14003ff40` 走 `tsRing[i] = sample[0]` 分支（而不是合成 100 µs 栅格）。[观测]
4. 导入等价于"把离线数据当作一段 HSS 采样灌进去"，因此触发判定、统计量（YMax/YMin/YAvg）
   也与实时采样一致地更新。[观测/推导]

### 4.5 "`*.jraw` 导出 / `*.raw` 导入"是否为同一格式？答：是

| 证据 | 内容 |
|---|---|
| 导出 | `fwrite(&ts,4,1)` + 逐变量 `fwrite(p, v->size, 1)`，**无头** [观测] |
| 导入 | `sampleSize = 4 + Σ v->size`，只校验 `fileSize % sampleSize == 0`，直接整段解包 [观测] |
| 两侧 | 导出用 `FUN_14003ed70/3ece0`（逻辑下标），导入用 `FUN_14003fa70`（与 HSS/RTT 同一解包器），布局定义完全一致 |
| UI | 导入对话框过滤器是 **`JRAW (*.jraw)`**，标题 **`Open JRAW file`**（`FUN_14004cf60` 0x140076cf8 / 0x140075a20） |
| 反证 | `Binary file (*.raw)` (0x140074240) 与 `Import Raw` (0x1400741b0) 在 .rdata 表里，**无任何已反编译函数引用**，属另一条（未恢复的）代码路径，见 §6 |

即：**`.jraw` 与菜单 "Impor&t Raw" 是同一种格式**；`*.raw` 只是同一族格式在另一处（可能更老的对话框/
按扩展名分发/命令行）使用的扩展名与文案。名字不同、格式相同。[观测+推断]

---

## 5. DataFile / RawFile：只是"上次导出路径"，**不是自动记录**

### 5.1 工程 XML 中的读写 [观测]

工程读取 `FUN_140045370`（chunk_005.c:6612）依次取 `<JLink> <ScriptFile> <ElfFile> <DataFile> <RawFile> <Scope> <Trigger>`：

```c
FUN_140045500(node "DataFile", cfg) → FUN_140039c60(..., cfg + 0x18C, 0x104);   // 读文本到路径缓冲
FUN_140045b80(node "RawFile",  cfg) → FUN_140039c60(..., cfg + 0x290, 0x104);
```

工程写出 `FUN_140046110`（chunk_005.c:7234）：

```c
if (strlen(proj+0x18C) > 0) FUN_140046240(...);   // 建 <DataFile> 元素, 文本 = proj+0x184
if (strlen(proj+0x290) > 0) FUN_1400466e0(...);   // 建 <RawFile>  元素, 文本 = proj+0x288
```

（`proj` 对象在 +0x8 处内嵌 0x5BC 的配置结构，故 `proj+0x18C ↔ cfg+0x184`、`proj+0x290 ↔ cfg+0x288`；
与 §2.1 里 CSV 写入的 `cfg+0x184`、§3.1 里 JRAW 写入的 `cfg+0x288` **完全对上**。）[推导]

### 5.2 结论

| 元素 | 实际含义 | 依据 |
|---|---|---|
| `<DataFile>` | "Export &Data ..."（CSV）上次选择的目标文件路径 | CSV 导出 `strncpy(cfg+0x184, path)` [观测] |
| `<RawFile>` | "Export &Raw ..."（JRAW）上次选择的目标文件路径 | JRAW 导出 `strncpy(cfg+0x288, path)` [观测] |
| INI `[DataFiles]` | 文件对话框的默认名（CSV/JRAW/Import 三处都用 `FUN_14004f820`） | `FUN_14004f820(&DAT_1400773fc /*0x1400773f0 "DataFiles"*/, ...)` [观测] |
| `[ELFFiles]` / `[ProjectFiles]` | 同类，供 ELF / 工程对话框 | `FUN_14004f890` / 其它 [观测] |

**采集时是否写盘？否定。** 全二进制中调用 `fopen_s/fwrite` 的函数只有：
`FUN_140047390`（CSV 导出）、`FUN_1400476d0`（JRAW 导出）、`FUN_140046e80`（JRAW 导入，只读）；
其余文件 I/O 是 `JScopeSettings.ini` 读写（`FUN_14004f590`/`FUN_14004f6c0`，CreateFile/ReadFile）、
ELF 解析、许可证/设置。作业表 0x140073ef0 的 15 个作业里**没有"保存数据/记录"类作业**。[观测]
「采集时落盘」在本版本中不存在——`DataFile/RawFile` 只是"记住上次导出到哪"。

> 顺带修正既有报告 `JSCOPE_ANALYSIS.md` §10.3 的一个误判：
> `Could not open file [%s]\nFile too large - maximum file size (%d bytes) exceeded!`（0x140077280）
> 属于 **`JScopeSettings.ini` 的读取器**，上限 **30000 字节**：
> `FUN_14004f940 → FUN_14004ef00("JScopeSettings.ini") → FUN_14004f590`，
> 判据 `if (DAT_142865284 < 0x7531) ... else 报错(%d=30000)`。**与数据导出无关**。[观测]

---

## 6. UI / 作业对应关系汇总

| 菜单项（描述表 `FUN_140001000`） | 快捷键（QKeySequence） | 槽名（元对象 0x140072c00） | 分派 | 实现 |
|---|---|---|---|---|
| `Export &Data ...` / "Save data to csv" | Ctrl+D (`0x4000044`) | `_SlotOnActionExportData()` | `FUN_14003beb0` case 8 | `FUN_140047390` |
| `Export &Raw ...` / "Save raw data to file" | Ctrl+R (`0x4000052`) | `_SlotOnActionExportRaw()` | case 9 | `FUN_1400476d0` |
| `Impor&t Raw` / "Load data from file" | Ctrl+T (`0x4000054`) | `_SlotOnActionImportRaw()` | case 10 | `FUN_14004cf60` → 作业 `APP: <ImportRaw>`(类型 0xb) → `0x14003d9a0` → `FUN_140046e80` |

- 三个动作都没有"作业"参与（CSV/JRAW 导出是**同步**执行，直接在 UI 线程写文件）。[观测]
- 只有 Import Raw 走异步作业队列（名字 `"APP: <ImportRaw>"`，作业体 0x128 字节，路径在 +0x20）。[观测]
- 额外发现：0x140073ff0 处有一张 `{扩展名, 过滤器, 标志}` 表：
  `{".jscope", "J-Scope project files (*.jscope)", 1}`、`{".raw", "Binary file (*.raw)", 1}`、
  `{".elf", 0, 1}`；紧随其后 0x140074040 是命令行选项表（`{-openprj, 3, FUN_14003dc80, ...}`），
  被 `FUN_140041cd0`（命令行解析）以 32 字节步长遍历。**没有已反编译函数引用扩展名表**。→ [未知]

---

## 7. 未知点

1. [未知] `"Binary file (*.raw)"`(0x140074240) + `"Import Raw"`(0x1400741b0) 的实际使用者。
   二者只出现在 .rdata 表（0x140073ff0 扩展名表、0x140073fa8 附近），无 xref。
   推测是"按扩展名打开文件"（拖放 / 命令行 / 旧版对话框）的路径，格式应与 `.jraw` 相同。
2. [未知] 作业分派器 `LAB_140042ee0`（0x140042ee0）未反编译；利用作业名表 + 垫片机器码已能确定
   "Import Raw → FUN_140046e80"，但**导入期间是否弹进度条/是否有取消分支**无法确认。
3. [未知] TypeID 3/4 对应的导出行为：CSV 分支里被 `default` 丢弃，JRAW 会按 `size` 原样写出；
   但这两个类型在树上被禁用（见既有报告 §8），实际是否可能出现在变量表未确认。
4. [推断] JRAW 的时间戳单位是 µs（沿用 HSS 契约），文件本身不带单位信息，无法从文件自证。
5. [推断] 导入大文件（> nSym*64MB+32MB）时"Excess data will be discarded"的**实际丢弃方式**：
   代码只做了提示，随后的 `FUN_14004ded0(..., size - size%sampleSize, ...)` 并未按缓冲区容量截断，
   真正生效的截断发生在 `FUN_14003fa70` 写环时（索引按 0x800000 回绕，旧数据被覆盖）。

---

## 8. 对 OpenScope 的启示（可执行）

1. **先做 JRAW 兼容的导入/导出，成本极低、互操作价值高。**
   格式就是：`repeat N { u32 ts_us(LE) ; var0.size 字节 ; var1.size 字节 ; ... }`，无头、无对齐、无字节序转换。
   实现要点：①样本长度 = `4 + Σsize`；②文件长度必须是样本长度的整数倍，否则只处理完整样本并提示
   （照抄 J-Scope 的 `File size does not align with selected symbols!` 语义）；③变量顺序 = UI 变量树顺序，
   必须与用户看到的顺序一致；④只用 `lastIdx` 个样本（**最新那个不导出**）以对齐 J-Scope 行为。
   这样 OpenScope 导出的 `.jraw` 可被 J-Scope 直接 `Import Raw`，反之亦然（同一 `.jscope` 工程下）。
2. **给 OpenScope 自己的格式加"自描述头"，但保留裸流模式。**
   J-Scope 的 JRAW 离开工程文件就不可解释（连时间戳单位都没写）。建议 OpenScope 默认输出
   带魔数+版本+变量描述（名/类型/大小/顺序/单位）的头，同时提供一个 `--jscope-compat`（裸流）开关，
   兼顾互操作与可长期归档。
3. **CSV 要按"Excel 友好"还是"J-Scope 兼容"做成二选一。**
   J-Scope 用 **分号** 分隔、CRLF、无 BOM、小数点 `.`、浮点 `%f`（6 位）、整数 `%i`（16/32 位无符号按有符号打印）。
   若要被欧洲 Excel 直接双击打开，分号+无 BOM 是对的；若要给通用工具用，建议提供逗号+UTF-8 BOM 变体。
   特别注意：**不要把 J-Scope 的 `%i` 行为照抄**（uint32 > 2^31 会显示为负数），OpenScope 应按变量的
   真实类型选择 `%u/%d/%lld`。
4. **导出上界语义要显式定义，别抄 J-Scope 的坑。**
   J-Scope：CSV 只导 `writeIdx-1` 行且绕回后从物理 0 开始（丢掉更老数据）；JRAW 导整环但绕回前后语义不同；
   两者都丢最新样本；`writeIdx==0` 时 CSV 会跑 2^32 次（越界读）。OpenScope 建议：
   导出区间由用户显式指定（全部 / 可见时间窗 / 最近 N 秒），并保证时间递增、无重复、无越界。
5. **导入路径照抄"清环 + 强制有时间戳 + 复用实时解包器"这一设计。**
   `memset(tsRing,0,32MB)` → 每变量 `memset(ring,0,64MB)` → 索引归零 → 临时强制
   `hasTimestamp=1` → 用与实时采集**同一个** `UnpackSamples` 灌数据 → UI 刷新。
   好处：导入数据与实时数据在后续所有环节（触发判定、统计、绘图、导出）中完全同构，零特判代码。
6. **`.jscope` 兼容读取时，`<DataFile>`/`<RawFile>` 只当"上次导出路径"处理**，
   不要把它们当作自动记录文件（本版本 J-Scope 没有采集落盘功能）；反过来 OpenScope 若写 `.jscope`，
   填上这两个元素能让 J-Scope 的导出对话框记住路径，属于低成本的兼容加分项。

---

## 9. 需要补充反编译（不要自行运行 Ghidra）

| 地址 | 说明 | 目的 |
|---|---|---|
| `0x140042ee0` (`LAB_140042ee0`) | 作业分派器本体，**Ghidra 未建函数** | 确认 Import Raw（类型 0xb）分支有无进度条/取消/失败提示 |
| `0x14003d9a0` | Import Raw 作业垫片（13 字节，Ghidra 未建函数） | 已用 capstone 人工确认为 `jmp FUN_140046e80`；补反编译可固化结论 |
| `0x140073ff0` 扩展名表的读者 | 未找到 xref | 定位 `Binary file (*.raw)`/`Import Raw` 的真正使用者（拖放/命令行/旧对话框） |
| `0x1400427b0` | `jobs.txt` 中出现的另一作业体（size=528） | 确认是否与导出/记录有关（本轮未涉及） |

---

*本报告仅基于静态逆向与只读的原始字节读取；未运行 J-Scope、未连接探针、未运行 Ghidra、未修改 `dumps/`。*
