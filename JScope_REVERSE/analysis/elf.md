# JScope.exe 专题逆向：ELF/DWARF 解析、符号模型与变量树 UI

**对象**：`C:\Program Files\SEGGER\JLink_V966\JScope.exe`（9.66，PE32+ x64，基址 `0x140000000`，Qt4，静态链接 DWARF 解析库）
**资料**：`dumps/alldecomp/chunk_000..008.c`（2001 函数反编译）、`dumps/globals.txt`、`dumps/defined_strings.txt`、`dumps/jobs.txt`、`JSCOPE_ANALYSIS.md`
**方法**：纯静态。除读取 exe 原始字节（定位 `.rdata` 常量表，只读）外未运行任何工具链、未修改 `dumps/`。

## 0. 证据分级

| 标记 | 含义 |
|---|---|
| **[观测]** | 反编译代码/原始字节/字符串直接可见 |
| **[推导]** | 由观测经确定性推理得到（偏移、语义） |
| **[推断]** | 合理但未完全证实 |
| **[未知]** | 未解决 |

地址均为 VA。行号 `chunk_NNN.c:NNNN` 可直接定位。

---

## 1. ELF 载入流程

### 1.1 作业表：类型号 ↔ 执行体 ↔ 显示名 [观测]

`FUN_140043d20`（带守卫分配器）的作业对象都带一个"类型号"。该类型号是**命令分派表的 1 基下标**，表位于 `.rdata 0x140073ef0`（每项 16 字节 `{worker_fn, name}`，实测原始字节）：

| idx(=type-1) | type | worker | 名称串 |
|---|---|---|---|
| 0 | 1 | `FUN_14003cc50` | `Initialize application` |
| 1 | 2 | `FUN_14003ce50` | `Exit application` |
| 2 | 3 | `FUN_14003cea0` | `Create project` |
| 3 | 4 | `FUN_14003cec0` | `Initialize Project Settings` |
| 4 | **5** | **`FUN_14003d210`** | **`Parse ELF File`** |
| 5 | 6 | `FUN_14003d5c0` | `Start reading data` |
| 6 | 7 | `FUN_14003d7c0` | `Pause reading data` |
| 7 | 8 | `FUN_14003d7d0` | `Stop reading data` |
| 8 | 9 | `FUN_14003d930` | `Save project` |
| 9 | 10 | `FUN_14003d940` | `Save project as` |
| 10 | 11 | `FUN_14003d950` | `Open project file` |
| 11 | 12 | `FUN_14003d9a0` | `Import Raw` |
| 12 | **13** | **`FUN_14003d9b0`** | **`Check modified ELF`** |
| 13 | 14 | `FUN_14003db70` | `Update project settings` |
| 14 | 15 | `FUN_14003dc70` | `Set AppStart action` |

**推论**：作业类型 = 表下标+1 ⇒ *Parse ELF File* = **type 5**，*Check modified ELF* = **type 13**。（`FUN_14003e4e0` 写 `*(undefined4 *)(puVar7 + 3) = 4`、`FUN_14003e0b0` 写 `= 0xc` 是"0 基下标"，两者一致。）

作业对象另两个名字（`APP: <ParseELFFile>` @0x1400750b8、`APP: <CheckELFModifications> Job` @0x140075170）是**堆分配标签**，传给 `FUN_140043d20(size, tag)`，不是显示名。 [观测]

### 1.2 ParseELFFile

* **入队**：`FUN_14003e4e0(jobParam, flag)` — `chunk_005.c:461`
  ```
  puVar7 = FUN_140043d20(0x128, "APP: <ParseELFFile>");
  ... 把调用方的 0x104 字节参数结构拷贝到 job+0x20 ...
  *(undefined1 *)((longlong)puVar7 + 0x124) = param_2;   // "reparse & match" 标志
  *puVar7 = FUN_140042f30;  *(undefined4 *)(puVar7 + 3) = 4;   // 类型 4 → (0 基)
  FUN_140039100(puVar7);                                    // 入队
  ```
  参数结构 = 工程设置 `config+0x80` 起的 **ElfFile 路径(0x104 字节)**（`FUN_14004d6c0` 传 `lVar1 + 0x80`）。 [观测]
* **执行**：`FUN_14003d210(job)` — `chunk_004.c:7274`，943 字节。流程：
  ```
  FUN_14003f830("Parsing elf file");
  lVar3 = FUN_140002f60(lVar5 + 0x20, 1);          // 打开并解析 ELF（见 1.3）
  if (lVar3 == 0) FUN_14003f110("Could not open ELF file!");
  uVar1 = FUN_140002950(lVar3);                    // ELF 符号总数
  uVar4 = FUN_14004e1f0(...,"Parsing ELF file ...",uVar1);   // 进度框
  for (i = 0; i < uVar1; i++) {                    // 逐个符号
      lVar5 = FUN_140002920(lVar3, i);             // 取第 i 个符号
      puVar6 = FUN_140002da0(local_318, lVar5);    // 取该符号的信息块（0x108 字节）
      if (local_3b8 != 0) {                        // 该符号有源码归属
          name = FUN_140002cf0(lVar5);             // 所属源文件名
          if (name 与上一个不同) {                  // 新建"文件节点"
              uVar7 = FUN_140040ab0(0);            // ★ 分配 0x3F0 符号记录
              FUN_140027140(uVar7 + 0x210, name, 0x104);   // +0x210 = File
              *(undefined1 *)(uVar7 + 0x340) = 1;          // +0x340 = 文件根标记
              FUN_14003fc40(uVar7);                        // 挂到全局树根 DAT_14286d3b0
              uVar10 = *(ulonglong *)(uVar7 + 0x3d8);      // 该文件节点的首子
          } else { FUN_14003fee0(uVar10); uVar10 = ...→+0x3e0; }  // 同文件：追加兄弟
          FUN_1400404c0(lVar3, lVar5, uVar10, uVar7);  // 递归遍历 DWARF DIE 建树
      }
  }
  FUN_14004e1f0(...); FUN_1400028c0(lVar3);        // 关文件
  DAT_14286d3b0 = uVar11;                          // ★ 发布新符号树
  hObject = FUN_140043750(lVar5 + 0x20, 0xd);      // 重新打开 ELF
  FUN_140043660(hObject, 0, &local_270);           // 读"最后写时间"
  DAT_14285d9f8/da00/da04 = 时间戳;                 // ★ 存档供 Check 用
  if (*(char *)(lVar5 + 0x124) != '\0') {          // 若是"重解析"
      FUN_14004de60(...); FUN_140040a10();         // 旧新符号按身份配对，拷显示选项
      FUN_14003e880();                             // 清空已选列表
      DAT_14286d3b8 = DAT_14285da08;               // 换成"重匹配"出来的列表
      DAT_14085d1e8 = DAT_14285d9dc;
      FUN_140040bd0(local_3e8);                    // 释放旧树
  }
  ```
* **建树递归体** `FUN_1400404c0` — `chunk_005.c:2415`，见 §4/§5。

### 1.3 文件读取方式：一次性全量读入（非 mmap） [观测]

`FUN_140002f60(path, dumpFlag)` — `chunk_000.c:1794`：

```c
iVar1 = FUN_1400078f0(0, param_2, param_3, param_1);   // elf_begin（先探路）
if (iVar1 == -1) { FUN_14000a770(0x45); ... }          // 太多个 ELF 已打开
_File = fopen(param_1, "rb");
if (_File == NULL) { FUN_14000a770(0x46); ... }        // 打不开
fseek(_File,0,2); lVar2 = ftell(_File); fseek(_File,0,0);   // 取文件长度
if (_Count < 1) { FUN_14000a770(0x47); ... }           // 长度异常
puVar4 = FUN_140019ae0(0x498);                         // 解析器上下文 0x498 字节
*(size_t *)(puVar4 + 0x28) = _Count;                   // 记录文件长度
_DstBuf = FUN_140019ae0(lVar2);                        // ★ 分配整个文件大小
sVar5 = fread(_DstBuf, 1, _Count, _File);              // ★ 一次性读入内存
if (sVar5 == _Count) { fclose(_File); ...  }           // 失败 → FUN_14000a770(0x49) "error reading file data"
```

随后 **4 步解析**（任一返回负即失败）：
```
FUN_14000c2c0(puVar4)   // ELF 头 → 内部段表
FUN_14000cea0(puVar4)   // .debug_types
FUN_14000ca30(puVar4)   // .debug_info（CU 主解析）
FUN_14000bf80(puVar4)   // 源码行信息
```
时间统计串与之对应（`%s` 归属函数 `FUN_14000aaf0`）：`parsed debug types section in ms: %ld`、`parsed debug info section in ms: %ld`、`parsed source line info in ms: %ld`、`parsed macro info in ms: %ld`、`parsed call frame info in ms: %ld`。 [观测]

**同时打开上限 2 个 ELF**：注册表 `DAT_140097c00` 只有 2 槽（`FUN_140002f80` 循环到 `0x140097c10`），对应错误 `0x45 a maximum number of ELF files are already open`。 [观测]

**ELF32/ELF64 都支持**：`FUN_14000c2c0` 按 `*(char *)(ehdr+4) == 2` 分派 64 位（`FUN_14000c520`）/ 32 位（`FUN_14000c340`）段表展开。 [观测]

### 1.4 头部校验与错误串 [观测]

`FUN_14000dfc0` — `chunk_001.c` 内，逐项校验 `e_ident`：

```c
if (*piVar3 != 0x464c457f) { FUN_14000a770(0x3f); return -1; }  // "\x7fELF"
iVar2 = FUN_140007950(); if (iVar2 != 2) { FUN_14000a770(0x40); ... }  // e_type != ET_EXEC
iVar2 = FUN_140007860(); if (iVar2 == 0) { FUN_14000a770(0x41); ... }  // EI_CLASS == 0
iVar2 = FUN_140005310(); if (iVar2 == 2) { FUN_14000a770(0x42); ... }  // EI_DATA == 0
cVar1 = FUN_1400078d0(); if (cVar1 != '\x01') { FUN_14000a770(0x43); ... }  // EI_VERSION != EV_CURRENT(1)
iVar2 = FUN_140007980(); if (iVar2 == 0) { FUN_14000a770(0x44); ... }  // e_version == 0
```

错误码 → 文本表 `FUN_140007c50`（`chunk_000.c:6113`，`switch` 0x2B..0x4A），**逐字**：

| 码 | 文本 |
|---|---|
| 0x3F | file not tagged with ELF magic number |
| 0x40 | not an executable ELF file |
| 0x41 | file class is ELF_CLASS_NONE |
| 0x42 | data encoding is ELF_DATA_NONE |
| 0x43 | header version number is not EV_CURRENT |
| 0x44 | unsupported file version number |
| 0x45 | a maximum number of ELF files are already open |
| 0x46 | file could not be opened for reading |
| 0x47 | unexpected file size |
| 0x48 | parser is out of memory |
| 0x49 | error reading file data |
| 0x4A | failed to initialize libdwarf library |
| 0x35 | compilation unit with DWARF5 debug information detected - DWARF5 is not yet fully supported |
| 0x3E | a debug information entry (DIE) at a specific data section offset could not be parsed/generated |

> **重要**：**`e_type` 必须是 `ET_EXEC(2)`** —— 可重定位目标文件（`ET_REL=1`，即 `.o`）与共享库（`ET_DYN=3`）都会被拒（"not an executable ELF file"）。**必须是可执行 ELF**，这是 OpenScope 必须照抄的一条硬性约束。 [观测]

J-Scope 层自己的两条：`Could not open ELF file!`（`FUN_14003f110` @`FUN_14003d210`）、文件对话框 `Output files (*.elf *.out *.obj *.axf);;All files (*.*)` / `No ELF file selected.`。 [观测]

### 1.5 CheckELFModifications：**只比时间戳，不重新解析** [观测]

`FUN_14003d9b0` — `chunk_004.c:7622`，433 字节（type 13 的 worker）：

```c
lVar3 = FUN_140043d10(0x5bc); FUN_14004e9c0(lVar3);   // 取当前工程设置
iVar1 = FUN_14004ea40();
if (iVar1 != 0 &&
    ((*(char *)(lVar3+0x5a4) != 1 || *(char *)(lVar3+0x5a6) != 0) &&   // 跳过 RTT-VarFormat 模式
     (DAT_14286d3b0 != 0))) {                                           // 且已有符号树
  hObject = FUN_140043750(lVar3 + 0x80, 0xd);        // 打开 ElfFile 路径
  if (hObject == INVALID) {
      if (GetLastError() == 2)                       // ERROR_FILE_NOT_FOUND
          FUN_140028380(..., "The elf file at\n%s\nwas deleted or moved to a different location.\n", path);
      DAT_14286d378 = 0; return;                     // ★ 不重解析
  }
  iVar1 = FUN_140043660(hObject, 0, &local_238);     // ★ 只读"最后写时间"
  if (iVar1 == 0 && (时间 != DAT_14285d9f8/da00/da04)) {
      FUN_140028380(..., "The elf file at\n%s\nwas modified.\n"
                         "Should J-Scope reparse the elf file and try to match the selected symbols?", path);
      iVar1 = FUN_14003f4f0(msg, "Reparse & Match symbols", "Reparse & Clear symbols", 0);
      if (iVar1 == 1) FUN_14003e880();               // 选"Match"→清空旧选择
      DAT_14286d379 = 1;
      FUN_14003e4e0(lVar3 + 0x80, 1);                // ★ 才入队 ParseELFFile（flag=1）
  }
}
```

**结论**：`CheckELFModifications` 是**纯时间戳比对**（`GetFileTime` → `FileTimeToSystemTime`，`FUN_140043660`），
① 文件不存在 → 只弹提示，**不**重解析；
② 时间戳变了 → 弹 "Reparse & Match symbols / Reparse & Clear symbols" 二选一，然后入队一个 **完整** 的 `ParseELFFile`（flag=1 触发 §1.2 尾部的配对/清空逻辑）。
配对 = `FUN_140040a10`（按 `FUN_1400429c0` 的身份四元组把旧符号的显示选项拷到新符号）；清空 = `FUN_14003e880`。
**触发时机**：`FUN_14004d7e0`（`chunk_006.c:4231`，随 GUI 定时/刷新路径调用，且 `DAT_14286d379/378` 为 0 时）与菜单项 `Check modified ELF`（`@0x140073fb8`）。 [观测]

---

## 2. libdwarf 使用

### 2.1 库身份

静态链接的 C 语言 ELF+DWARF 解析库，对外只暴露一张**门面**（`FUN_1400028c0` … `FUN_140002fb0`，地址 0x140002xxx），内部是自带的 libelf 层（错误串 `file not tagged with ELF magic number` 等属 libelf `elf_errmsg` 家族）+ libdwarf API。 [观测/推断]

libdwarf API 名（静态字符串，`.rdata` 0x140060e00 起）：
`dwarf_attrval_flag`、`dwarf_attrval_string`、`dwarf_attrval_signed`、`dwarf_attrval_unsigned`、`dwarf_next_cu_header_c`、`dwarf_child_b`、`dwarf_siblingof_b`、`_dwarf_search_die_within_cu`、`dwarf_offdie_b`、`dwarf_tag`、`dwarf_get_address_size`、`dwarf_formaddr`、`dwarf_formflag`、`dwarf_formudata`、`dwarf_formsdata`、`dwarf_formexprloc`、`dwarf_srclines`、`dwarf_srcfiles`、`dwarf_loclist_n`、`dwarf_get_macro_details`、`dwarf_get_ranges`、`dwarf_get_str`、`_dwarf_abbrev_parse` … [观测]

### 2.2 会被读取的 DWARF 段 [观测]

段名常量表在 `.rdata`（`defined_strings.txt` 34–57 行）：

```
.debug_info .debug_types .debug_loc .debug_str .debug_abbrev .debug_frame .debug_line
.debug_macinfo .debug_macro .debug_ranges .debug_aranges
.eh_frame .symtab .strtab .symtab_shndx .note .note.iar
.debug_loclists .debug_rnglists .debug_line_str .debug_names .debug_addr .debug_str_offsets
```

**J-Scope 实际走的**（由 §1.3 的 4 步 + 统计串反推）：
`.debug_info`（主）、`.debug_types`（次）、`.debug_abbrev`、`.debug_str`、`.debug_line`、`.symtab/.strtab`，可选 `.debug_macinfo/.debug_macro`、`.debug_frame/.eh_frame`、`.debug_loc/.debug_ranges`（位置表达式）。DWARF5 的新段名（`.debug_loclists`/`.debug_rnglists`/`.debug_line_str`/`.debug_names`/`.debug_addr`/`.debug_str_offsets`）在表内但功能不完整。 [观测/推断]

### 2.3 DWARF 版本

* DWARF2/3/4：完整（`_ParseCFI_V4_Members`、`dwarf_next_cu_header_c`）。
* **DWARF5：能识别但明确不支持** —— 错误 0x35 `compilation unit with DWARF5 debug information detected - DWARF5 is not yet fully supported`。 [观测]
* 自报统计（`FUN_14000aaf0` 的 dump，仅在 `FUN_140002f60(...,1)` 时执行）：`%8u ELF symbols` / `%8u DWARF symbols` / `%8lld Funcs` / `%8lld InlineFuncs` / `%8lld Global variables` / `%8lld Types` / `%8u Members` / `%8u Local variables` / `%8lld Compilation units` / `%8lld SrcLines` / `%8lld Namespaces` / `%8lld Namespace extensions` / `%8lld Macros` / `%8u FDEs` / `%8u CIEs` / `Total init time: %ld ms`。 [观测]
* 地址信息完整性校验：`Symbol at address %llX: did not pass allocation size sanity check`（`FUN_14000e080`）。 [观测]

### 2.4 符号→DIE 门面（`FUN_140002xxx`）[观测]

| 门面 | 内层 | 语义 |
|---|---|---|
| `FUN_140002f60(path,1)` | `FUN_1400078f0` | 打开 ELF 并完成 4 步解析，返回 0x498 上下文 |
| `FUN_140002950` | `FUN_140007a10`→`FUN_140008f40(...,0x34)` | ELF 符号表项数 |
| `FUN_140002920(i)` | `FUN_140007420(...,&LAB_140007a00,i)` | 第 i 个 ELF 符号句柄 |
| `FUN_140002da0` | `FUN_1400088e0` | 符号信息块（0x88 字节：含名字、地址、大小、源文件…） |
| `FUN_140002c00` | `FUN_140005560`/`FUN_14000b830` | **类型信息块**（含 type_id、维度数、bit_size、bit_offset…） |
| `FUN_140002d20(die,1)` | `FUN_1400072b0(FUN_140005380,…)` | **DIE tag**（标准 `DW_TAG_*`：实测 0x2E=subprogram、0x13=structure_type、0x17=union_type、0x01=array_type、0x02=class_type） |
| `FUN_140002d40` | `FUN_1400070e0`→`FUN_140002fb0` | **类型名文本**（2578 字节的拼接器，产出 `float`/`double`/`_Bool` 等） |
| `FUN_140002cf0(die)` | `FUN_1400065c0(...,0x58)` | 所属源文件名 |
| `FUN_140002eb0` | `FUN_140006d80` | 有符号标志（→ `+0x330 IsSigned`） |
| `FUN_140002cd0` / `FUN_140002cb0` | `FUN_140005dc0` / `FUN_140005ab0` | **bit_size** / **bit_offset**（位域） |
| `FUN_140002e80` | `FUN_140006b10` | 成员在父结构中的字节偏移（`DW_AT_data_member_location`） |
| `FUN_140002e50`/`FUN_140002e30(hi)` | `FUN_140006a40` / `FUN_1400072b0(FUN_140006a00,…)` | 子 DIE 数 / 第 i 个子 DIE |
| `FUN_140002d80`/`FUN_140002d60(hi)` | `FUN_140007aa0` / `FUN_1400072b0(FUN_140007a60,…)` | 数组维度数 / 第 i 个维度 |
| `FUN_1400028c0` | `FUN_140003a30`/`FUN_140003a70` | 关闭 |

> 注意：**树里的"符号"来自 ELF 符号表（`FUN_140002950/2920`），不是直接遍历 CU 的 DIE**；DWARF 只用于给这些符号补类型/成员/数组维度。见 §4。 [观测]

---

## 3. TypeID 类型系统（1 … 14）

`TypeID` 存于符号记录 `+0x314`。全程序只有 **3 个产出点**、**1 个外来入口**。

### 3.1 产出点 A：ELF/DWARF 分类器 `FUN_140040050` [观测]

`chunk_005.c:2269`，在 `FUN_1400404c0` 建完节点后调用。**逐字逻辑**：

```c
if (*(longlong *)(param_1 + 0x3d8) != 0) {            // 有子节点 = 聚合体
    *(undefined4 *)(param_1 + 0x314) = 0; return; }   // TypeID 0，且不给勾选框
*(undefined1 *)(param_1 + 0x35f) = 1;                 // 叶 → 显示勾选框
if (*(int *)(param_1 + 0x314) == 0xe) return;         // 位域已定，保持
uVar2 = FUN_1400423a0(param_1 + 0x110);               // 类型名去掉尾部空格
/* --- bool --- */
if (size==1 && (strcmp(type,"")==0 || strcmp(type,"_Bool")==0))
      { MaxValue(+0x3B0)=1.0; TypeID=5; IsSigned(+0x330)=0; return; }
/* --- 浮点按名字 ---- */
if (size==4 && strcmp(type,"float")==0)  { Max=0x47efffffe0000000 /*FLT_MAX*/; TypeID=1; IsSigned=1; return; }
if (size==8 && strcmp(type,"double")==0) { Max=0x7fefffffffffffff /*DBL_MAX*/; TypeID=2; IsSigned=1; return; }
/* --- 其余按 [大小 × 有符号] 二维表 --- */
iVar1 = *(int *)(param_1 + 0x31c);                    // Size
if (*(char *)(param_1 + 0x330) == '\0') {             // IsSigned == 0 ⇒ 无符号
    size 1 → TypeID=6, Max=255.0
    size 2 → TypeID=7, Max=65535.0
    size 4 → TypeID=8, Max=4294967295.0
    size 8 → TypeID=9, Max=2^64
} else {                                              // IsSigned != 0 ⇒ 有符号
    size 1 → TypeID=10, Max=127.0
    size 2 → TypeID=11, Max=32767.0
    size 4 → TypeID=12, Max=2147483647.0
    size 8 → TypeID=13, Max=2^63
}
*(undefined4 *)(param_1 + 0x314) = 0;                 // 其它（含 size=3/16 等）→ 0
```

**判定依据表**：

| TypeID | 语义 | 判定依据（实测） | 消费（绘图/取值/文本/表格） |
|---|---|---|---|
| 0 | 非叶/未知 | 有子节点，或大小∉{1,2,4,8} 且非 float/double/_Bool | 树中无勾选框、不绘图 |
| 1 | float32 | `Size==4 && Type=="float"` | ✔ `FUN_140042ac0:case 1`、`FUN_14004a1b0:1`、`FUN_140056f60:1` |
| 2 | float64 | `Size==8 && Type=="double"` | ✔ |
| **3** | **无任何产出点**（见 §3.4） | — | ✘ `FUN_140042ac0` 无 case；树中禁用 |
| **4** | **无任何产出点** | — | ✘ 同上 |
| 5 | bool | `Size==1 && (Type=="" \|\| Type=="_Bool")` | ✔ 文本 `True/False` |
| 6/7/8 | uint8/16/32 | `IsSigned==0`，size=1/2/4 | ✔ |
| 9 | uint64 | `IsSigned==0`，size=8 | ✘ **树中禁用**，无绘图/取值分支 |
| 10/11/12 | int8/16/32 | `IsSigned!=0`，size=1/2/4 | ✔ |
| 13 | int64 | `IsSigned!=0`，size=8 | ✘ **树中禁用** |
| **14 (0xE)** | **位域 bitfield** | 见 §3.3 | ✔ 有绘图/取值/文本分支（走 `FUN_14003eaa0`） |

`+0x330` 的语义经 `.jscope` 读入器**直接证实**：`FUN_140044f40` 里 `FUN_140045250(param_1,"IsSigned",param_2 + 0x330)` ⇒ **`+0x330 = IsSigned`**。 [观测]

### 3.2 产出点 B：RTT-VarFormat 解析 `FUN_14003fc90` [观测]

`chunk_005.c:2041`。通道名 `JScope_<TypeChar><SizeDigit>…`，逐对解析：

```c
cVar2 = local_148[lVar1];  iVar4 = local_148[lVar1+1]; iVar7 = iVar4 - '0';
' t' → *(param_1 + 0x5a5) = 1;                       // IsRTTWithTimeStamp
' b' → iVar5 = (iVar7==1) ? 5 : 0;  fmt = "%d Byte bool RTT Data %d"
' f' → (iVar7==4) ? 1 : ((iVar7==8) ? 2 : 0);  fmt = "%d Byte float RTT Data %d"
' u' → '1'→6, '2'→7, 4→8, 8→9;   fmt = "%d Byte unsigned RTT Data %d"
' i' → '1'→10,'2'→11, 4→12, 8→13; fmt = "%d Byte signed RTT Data %d"
if (iVar5 != 0) FUN_14003dfa0(iVar7 /*size*/, iVar5 /*TypeID*/, 0, local_128);
```

`FUN_14003dfa0` 里：`*(undefined4 *)(lVar2 + 0x31c) = size; *(undefined4 *)(lVar2 + 0x314) = TypeID; *(undefined1 *)(lVar2 + 0x330) = 0;`
⇒ **RTT 通道名里的类型也进同一套 TypeID**，且 `IsSigned` 恒为 0（因为 TypeID 已区分有无符号）。注意：`f` 只有 4/8，**没有 3/4**；`u/i` 也不产 3/4。 [观测]

### 3.3 `case 0xe` = 位域（**新发现**）[观测]

树/绘图 switch 里出现的 `0xE`：

```c
// FUN_140042ac0 (chunk_005.c:4423)
case 0xe:
    uVar4 = FUN_14003eaa0(*(float *)param_2, param_1);   // 从样本槽取 raw u32 后抽位
    dVar7 = (double)uVar4;  ... 参与 min/max/avg 统计 ...
```
```c
// FUN_14003eaa0 (chunk_005.c:841) —— 唯一的抽取器
uint FUN_14003eaa0(uint param_1, longlong param_2) {
  if (param_2 != 0)
    return (1 << ((byte)*(undefined4 *)(param_2 + 0x338) & 0x1f)) - 1U
           & param_1 >> ((byte)*(undefined4 *)(param_2 + 0x33c) & 0x1f);
  return 0;
}
```

即 `value = (raw >> bitOffset) & ((1<<bitWidth)-1)`，`+0x338 = 位宽`，`+0x33C = 位偏移`。

**产生点**在 `FUN_1400404c0`（`chunk_005.c:2540-2555`）：

```c
if ((*param_4 < 0x18) && ((0x880004U >> (*param_4 & 0x1f) & 1) != 0)) {   // 父 tag ∈ {2(class), 0x13(struct), 0x17(union)}
    iVar5 = FUN_140002e80(param_2);                                       // DW_AT_data_member_location
    param_3[0xc6] = iVar5 + *(int *)(*(longlong *)(param_3 + 0xfa) + 0x318);   // +0x318 = 成员偏移+父地址
    if (FUN_140002cd0(param_2) != 0) {                                    // DW_AT_bit_size 非 0
        param_3[199] = param_4[199];                                      // Size 继承父（存储单元字节数）
        param_3[0xc5] = 0xe;                                              // ★ TypeID = 14
        param_3[0xce] = FUN_140002cd0(param_2);                           // ★ +0x338 = bit_size
        param_3[0xcf] = FUN_140002cb0(param_2);                           // ★ +0x33C = bit_offset
    }
}
```

**结论：TypeID 14 = 位域成员**（`DW_TAG_member` + `DW_AT_bit_size`/`DW_AT_bit_offset`）。16 进制 `0x880004` 的置位 = bit2 / bit19 / bit23 = tag 2 / 0x13 / 0x17。 [观测/推导]

消费点：`FUN_140042ac0:case 0xe`、`FUN_14004a1b0:case 0xe`、`FUN_140056f60:case 0xe`、`FUN_14003eaa0` 的另外两个调用者 `FUN_140053420`、`FUN_140056f60`（表格）。**位域可勾选、可绘图**（与 3/4/9/13 不同）。

### 3.4 TypeID 3 / 4 的结论

穷举了三个产出点（`FUN_140040050`、`FUN_14003fc90`、`FUN_14003dfa0`）与唯一外来入口（`.jscope` 读入器 `FUN_140044f40` 首分支 `*(undefined4*)(param_2+0x314) = 文件里的 TypeID` 原样透传）：

| 产出点 | 可能产生 |
|---|---|
| ELF/DWARF `FUN_140040050` | 0, 1, 2, 5, 6, 7, 8, 9, 10, 11, 12, 13（+ 已有的 14） |
| RTT VarFormat `FUN_14003fc90` | 0, 1, 2, 5, 6…13 |
| `.jscope` 读入 `FUN_140044f40` | **文件里写多少就是多少**（可手改成 3/4） |

⇒ **本版本没有任何代码路径会产生 TypeID 3 或 4**。它们只能来自**手工编辑/旧版本写的 `.jscope`**（`TypeID="3"`）。树的禁用分支
```c
iVar1 = *(int *)(param_2 + 0x314);
if ((((iVar1 == 3) || (iVar1 == 4)) || (iVar1 == 9)) || (iVar1 == 0xd)) {
    QTreeWidgetItem::setDisabled(pQVar2,true);
    setText(pQVar2,4,"Symbol type is not supported"); }
```
是**防御式遗留**。§8 曾猜 3=float128、4=half —— **代码里找不到支撑**：`long double`/`__float128`/`half` 这些串位于 **C++ demangler 的类型名表**（`0x14005fec0` 起的指针数组，被 `FUN_14000f540` 使用），而 `"float"`/`"double"` 的引用者只有 `FUN_140040050`。
⇒ **[推导]** 3/4 为保留/历史值，语义 **[未知]**。（保留 §8 的猜测作为 [推断] 即可，但不应当作事实。）

---

## 4. 符号可见性与树结构规则

### 4.1 顶层：源文件节点（不是符号本身）[观测]

`FUN_14003d210` 按 ELF 符号表顺序遍历；符号的所属源文件名（`FUN_140002cf0`）变化时**新建一个"文件节点"**：

```c
FUN_140027140(uVar7 + 0x210, file, 0x104);   // +0x210 = File
*(undefined1 *)(uVar7 + 0x340) = 1;          // +0x340 = 文件根
FUN_14003fc40(uVar7);                        // 挂到全局根 DAT_14286d3b0
```

渲染时（`FUN_1400507f0` 的 "Finishing Symbols" 段）：
```c
pQVar6 = *(QTreeWidgetItem **)(lVar8 + 0x360);
if (((*(char *)(lVar8 + 4) == '\0') && (*(char *)(lVar8 + 0x210) != '\0')) && ...) {
    QTreeWidget::setFirstItemColumnSpanned(tree, item, true);   // 整行跨列
    QTreeWidget::setItemExpanded(tree, item, true); }           // 默认展开
```
⇒ **只有"无名字(+0x004 为空) 且 有 File(+0x210)"的节点**才被当作文件标题行（跨列、展开）。 [观测]

### 4.2 可见性规则汇总

| 规则 | 证据 |
|---|---|
| 顶层 = 源文件分组节点；其子 = 该文件里的符号 | `FUN_14003d210`（同上）[观测] |
| 函数（`DW_TAG_subprogram` 0x2E）不进树 | `FUN_1400404c0`: `if (iVar4 == 0x2e) return;` [观测] |
| 叶子（无子节点）才有勾选框 `+0x35F=1` | `FUN_140040050` 首两行 [观测] |
| 有子节点 ⇒ TypeID=0、无勾选框（容器节点，仍可展开） | 同上 [观测] |
| 无名 struct/union：名字填 `<Anonymous>` | `if ((iVar4-0x13U & 0xfffffffb)==0 && name[0]=='\0') strncpy(name,"<Anonymous>")` [观测] |
| 匿名继承类：名字填 `<Inherited>` | `if (tag==2 && parentTag==2 && name[0]=='\0')` [观测] |
| 数组成员：**逐个展开成独立叶**，命名 `[n]` | `FUN_140040320`：`FUN_140028380(buf,0x100,&DAT_140074518,uVar4)`，格式串实测 = `"[%u]"` [观测] |
| 数组展开**上限 30000**（每维） | `uVar3 = 30000; if (*param_1 < 30000) uVar3 = *param_1;` [观测] |
| 递归深度上限 **33**（`param_2 < 0x21`）| `FUN_140040320` 入口条件 [观测] |
| 数组元素节点 `+0x334 = 1`，**不参与名字拼接** | `FUN_140040cd0`：`if (*(char*)(n+0x334)=='\0' && *(char*)(n+0x340)=='\0')` 才拼名 [观测] |
| **`NameWithParents` = 从根到该叶的 `.` 连接** | `FUN_140040cd0` 逆序 `memcpy` + `_Dst_00[-1] = 0x2e /*'.'*/` [观测] |
| 不支持类型（3/4/9/13）：条目**禁用**，第 4 列文字 `Symbol type is not supported` | `FUN_1400504d0` 尾段 [观测] |
| 已选中的叶：第 4 列勾选状态 = 命中 `DAT_14286d3b8` 已选列表 | `FUN_1400504d0`：`FUN_1400512f0(param_1,param_2)`（按 `+0x318` 地址 **且** `+0x108` NameWithParents 相等判定）[观测] |

> `+0x004`（叶名）、`+0x108`（NameWithParents）、`+0x110`（类型名）、`+0x210`（文件）、`+0x318`（地址）、`+0x31C`（大小）在 `FUN_1400504d0` 中被直接送进 5 列表格。
> **"不支持类型"只挡 ELF 来的 9/13（64 位整型）**——即 **J-Scope 无法观测 64 位整型变量**，这是重实现时必须注意的功能缺口。 [观测]

---

## 5. 变量/符号记录布局：**0x3F0 字节**（不是 0x3D8）

**分配点**：`FUN_140040ab0`（`chunk_005.c:2747`，4 个调用者）：
```c
puVar1 = (undefined4 *)FUN_140043d20(0x3f0);        // ★ 1008 字节
```
**独立佐证**：`chunk_007.c:2397` `memcmp((void *)*puVar1, (void *)*puVar3, 0x3f0);`（符号排序/比较时整块比较 0x3F0 字节）。
⇒ 任务书里的 "0x3D8 字节" 是误解：**0x3D8 是字段（首子指针）**，记录总长 **0x3F0**。 [观测]

`FUN_140040ab0` 的初始化（字索引 ×4 = 字节偏移）给出了全字段骨架：

```c
*(undefined8 *)(puVar1 + 0xee) = 0;                    // +0x3B8
*(undefined8 *)(puVar1 + 0xf0) = 0;                    // +0x3C0
*(undefined8 *)(puVar1 + 0xe6) = 0xffefffffffffffff;   // +0x398 = -DBL_MAX (YMax)
*(undefined8 *)(puVar1 + 0xe8) = 0x7fefffffffffffff;   // +0x3A0 = +DBL_MAX (YMin)
*puVar1 = 0;            // +0x314 TypeID
*(undefined1 *)(puVar1 + 1) = 0;                       // +0x004 Name
*(undefined1 *)(puVar1 + 0x44) = 0;                    // +0x110 Type
*(undefined1 *)(puVar1 + 0x84) = 0;                    // +0x210 File
*(undefined8 *)(puVar1 + 0xc6) = 0;                    // +0x318 Address
puVar1[200] = 0;                                       // +0x320 维度1 元素数
*(undefined8 *)(puVar1 + 0xca) = 0;                    // +0x328 维度链表头
puVar1[0xcc] = 0;                                      // +0x330 IsSigned
*(undefined1 *)(puVar1 + 0xcd) = 0;                    // +0x334 数组元素标记
*(undefined8 *)(puVar1 + 0xce) = 0;                    // +0x338 bitWidth + +0x33C bitOffset
*(undefined8 *)(puVar1 + 0x42) = 0;                    // +0x108 NameWithParents
*(undefined1 *)(puVar1 + 0xd0) = 0;                    // +0x340 文件根标记
puVar1[0xd7] = 0x10000;                                // +0x35C=0, +0x35D IsAutoConfig=0, +0x35E ShowInGraph=1, +0x35F=0
*(undefined8 *)(puVar1 + 0xd8) = 0;                    // +0x360 QTreeWidgetItem*
*(undefined8 *)(puVar1 + 0xda) = 0;                    // +0x368
*(undefined8 *)(puVar1 + 0xde) = 0;                    // +0x378 QColor*
*(undefined8 *)(puVar1 + 0xdc) = 0;                    // +0x370
puVar1[0xd2] = 0;                                      // +0x348 DrawStyle
*(undefined8 *)(puVar1 + 0xd4) = 0;                    // +0x350 YOffset (double)
puVar1[0xd6] = 1;                                      // +0x358 YResolution = 1
*(undefined2 *)(puVar1 + 0xe0) = 0;                    // +0x380 (byte 标志)
*(undefined8 *)(puVar1 + 0xec) = 0;                    // +0x3B0 MaxValue
*(undefined1 *)(puVar1 + 0xe2) = 0;                    // +0x388 本周期有数据
*(undefined8 *)(puVar1 + 0xe4) = 0;                    // +0x390
*(undefined8 *)(puVar1 + 0xea) = 0;                    // +0x3A8 YAvg
puVar1[0xe3] = 0;                                      // +0x38C 样本内偏移
*(undefined8 *)(puVar1 + 0xf2) = 0;                    // +0x3C8
*(undefined8 *)(puVar1 + 0xf4) = 0;                    // +0x3D0 dataBuf
*(undefined8 *)(puVar1 + 0xf6) = 0;                    // +0x3D8 firstChild
*(undefined8 *)(puVar1 + 0xf8) = 0;                    // +0x3E0 nextSibling
*(undefined8 *)(puVar1 + 0xfa) = 0;                    // +0x3E8 parent
```

### 5.1 成员偏移表（补全）

| 偏移 | 类型 | 含义 | 等级 | 证据 |
|---|---|---|---|---|
| `+0x004` | char[0x104] | `Name`（叶名） | 观测 | XML 属性 `Name`（`FUN_140044ce0`）；`FUN_1400504d0` 列 0 |
| `+0x108` | char* | `NameWithParents`（`.` 连接全路径） | 观测 | `FUN_140040cd0` 构造；XML `NameWithParents` |
| `+0x110` | char[0x100] | `Type`（类型名） | 观测 | XML `Type`；`FUN_140040050` 的名字比较 |
| `+0x210` | char[0x104] | `File`（源文件） | 观测 | `FUN_14003d210` 写入；XML `File` |
| `+0x314` | int | `TypeID`（0…14，见 §3） | 观测 | XML `TypeID` |
| `+0x318` | uint32 | `Address`（绝对地址） | 观测 | XML `Address`（`FUN_140044ac0`）；HSS 描述符 |
| `+0x31C` | int | `Size`（字节数） | 观测 | XML `Size`；`FUN_140040050` 判定 |
| `+0x320` | uint32 | 数组第 1 维元素数 | 推导 | `FUN_1400404c0` `param_3[200] = (int)uVar2` |
| `+0x328` | ptr | 维度链表头（0x10/节点 `{count, ?, next}`） | 观测 | `FUN_140043d20(0x10,"Node dimension size")`；`FUN_140040bd0` 释放链 |
| `+0x330` | char | **IsSigned** | 观测 | `.jscope` 属性名就叫 `IsSigned`（`FUN_140044f40`） |
| `+0x334` | char | 数组元素标记（不参与名字拼接、不进勾选判定） | 观测 | `FUN_140040320`、`FUN_140040cd0` |
| `+0x338` | int | **位域位宽** bitWidth | 观测 | `FUN_14003eaa0` |
| `+0x33C` | int | **位域位偏移** bitOffset | 观测 | `FUN_14003eaa0` |
| `+0x340` | char | 文件根标记（跨列显示文件行） | 观测 | `FUN_14003d210`、`FUN_1400507f0` |
| `+0x348` | int | `DrawStyle` | 观测 | XML `DrawStyle`（`FUN_140044af0`） |
| `+0x350` | double | `YOffset` | 观测 | XML `YOffset`；绘图 `FUN_14004a2b0` |
| `+0x358` | int | `YResolution`（初值 1） | 观测 | XML `YResolution`（`FUN_1400451f0`） |
| `+0x35C` | char | 未知布尔（初值 0，与 `ShowInGraph` 一起被 4 字节 init） | 未知 | 仅见读写 `chunk_006.c:2759`、`chunk_007.c:417` |
| `+0x35D` | char | `IsAutoConfig` | 观测 | XML `IsAutoConfig`（`FUN_140044c80`） |
| `+0x35E` | char | `ShowInGraph`（**初值 1**） | 观测 | `puVar1[0xd7]=0x10000` |
| `+0x35F` | char | 是否有勾选框（叶子=1） | 观测 | `FUN_140040050`、`FUN_1400504d0` |
| `+0x360` | ptr | `QTreeWidgetItem*`（回指 UI 条目） | 观测 | `FUN_1400504d0`；`FUN_140051130` 反查 |
| `+0x368`/`+0x36C`/`+0x370`/`+0x374` | int×4 | 颜色/线宽等显示参数块（被重解析配对整块拷贝） | 推导 | `FUN_140040a10` 拷贝 0x368..0x377 |
| `+0x378` | `QColor*` | 曲线颜色对象 | 观测 | XML `GraphColorRed/Green/Blue` → `QColor::setRgb` → `*(QColor**)(param_2+0x378)`（`FUN_140044bb0`） |
| `+0x380` | char(+pad) | **排他"当前符号"标志**（置 1 时先清掉其它所有已选符号的该位） | 观测 | `FUN_14003ee00`（`chunk_005.c:1137`） |
| `+0x388` | char | **本周期有数据**（未触发/无新样本=0） | 观测 | `FUN_14003fa70` 依 `FUN_1400415a0` 结果写 0/1 |
| `+0x38C` | uint32 | **样本内字节偏移**（HSS 布局） | 观测 | `FUN_140041f30`/`FUN_140042a60`；XML `Offset` |
| `+0x390` | 8B | 保留（init 清零，未见其它读） | 未知 | `FUN_14003ffd0` 区 |
| `+0x398` | double | `YMaxSampled`（init `-DBL_MAX`） | 观测 | XML `YMaxSampled`；`FUN_140042ac0` |
| `+0x3A0` | double | `YMinSampled`（init `+DBL_MAX`） | 观测 | XML `YMinSampled` |
| `+0x3A8` | double | `YAvgSampled` | 观测 | XML `YAvgSampled`；`FUN_140042ac0` 递推平均 |
| `+0x3B0` | double | `MaxValue`（**由类型决定的量程上限**，见 §3.1） | 观测 | `FUN_140040050` 写 FLT_MAX/DBL_MAX/255/65535/… |
| `+0x3B8` | 8B | 最新样本值（union，供表格直接取） | 推导 | `chunk_007.c:749-755` QVariant 双精度/整型分支 |
| `+0x3C0` | int | 最新样本值的种类标签（==2 时按 double 取） | 推导 | 同上 |
| `+0x3C8` | ptr | **指向数据环中"最新样本槽"的指针**（`dataBuf + writeIdx*8`） | 观测 | `FUN_14003fa70`；`FUN_140056f60` 直接解引用读值 |
| `+0x3D0` | ptr | 数据环基址（每变量 `0x4000000` = **64 MB**） | 观测 | `FUN_14003e050` |
| `+0x3D8` | ptr | **firstChild** | 观测 | `FUN_14003fc40`、树遍历 |
| `+0x3E0` | ptr | **nextSibling** | 观测 | `FUN_14003fee0`、树遍历 |
| `+0x3E8` | ptr | **parent** | 观测 | `FUN_140050e30` 沿 +1000(=0x3E8) 上溯到根 |

> 修正既有报告 §9：`+0x3C8` 不是"写指针"而是"最新样本指针"；`+0x360` 是 UI 回指指针；`+0x3B8/+0x3C0` 是"最新值 + 种类"；`+0x3E8` 是父指针；记录长度 **0x3F0**。

---

## 6. 树模型与 UI

### 6.1 关键事实：**符号树在模态对话框里**（"Select Symbols"）[观测]

| 类（Qt metaclass 名） | 实现 | 说明 |
|---|---|---|
| **`SymbolSelection`** | ctor `FUN_140050140`（标题 `"Select Symbols"`）；metacast `FUN_14003c320`；metacall `FUN_14003c2c0`→`FUN_14003c240` | **本专题的"变量树"**。含：搜索框 `FUN_140050340`、**5 列 QTreeWidget** `FUN_1400507f0`、OK/Cancel `FUN_1400503f0` |
| `CDeviceSelectionDialog` | ctor `FUN_14002d5c0`；setupUi `FUN_140032220`；metacast `FUN_14003b500` | **目标 MCU 选型对话框**（objectName `DeviceSelectionDialog`，标题 `Target Device Settings`，列 `Manufacturer / Device / NumCores / Flash Size`），**与变量树无关** |
| `CDeviceDataModel` | ctor `FUN_14002d480`（`QAbstractItemModel` 派生，objectName `DeviceDataModel`） | 上者的表格模型 |
| `CDeviceDataProxyModel` | ctor `FUN_14002d560`（`QSortFilterProxyModel` 派生） | 上者的过滤代理 |
| `UtilFilterHeader` | `FUN_14003caa0` 起 | 通用"每列带过滤框的表头"：`OnColFilterChanged/OnColSelectionChanged/OnColTextChanged`、`OnToggleFilter(bool)`、`OnToggleColumn(bool)`、`SignalFilterChanged/SelectionChanged/TextChanged`；给上者的 QTableView 用 |

> **纠正任务书前提**：`CDeviceDataModel / CDeviceDataProxyModel / CDeviceSelectionDialog / UtilFilterHeader` 这一组是 **J-Link 目标器件选择**（列 = Manufacturer/Device/NumCores/Flash Size），不是变量树。变量树 = **`SymbolSelection` 对话框**里的 **QTreeWidget（不是 QAbstractItemModel 视图）**。 [观测]

### 6.2 SymbolSelection 的 5 列与勾选列 [观测]

表头字符串数组 `0x140077690`（每项 `{QString data, maxWidth}`）：

| 列 | 表头 | 最大宽度 | 内容来源 |
|---|---|---|---|
| 0 | `Name` | 500 | `+0x210 File`（文件行，跨列+置灰底色 `QColor(6)`）或 `+0x004 Name` |
| 1 | `Type` | 500 | `+0x110 Type` |
| 2 | `Size` | 50 | `+0x31C`（QVariant int 写入 `setData(...,2,...)`） |
| 3 | `Address` | 120 | `"0x%X"` 格式化 `+0x318` |
| 4 | `Select` | 300 | 勾选框，或 `"Symbol type is not supported"` |

`FUN_1400504d0` 建立条目；`QTreeWidget::setSortingEnabled(true)`（列头可排序）；`setEditTriggers(0)`（禁编辑）；
双击列 4 → `editItem(tree,item,4)`（`FUN_140051720`）即"手动切换勾选"。 [观测]

### 6.3 搜索框：匹配什么、大小写 [观测]

* 控件：`FUN_140050340` 创建 QLineEdit（`param_1+0x38`），占位文字实测 = **`"Filter symbols"`**，`connect(textChanged(const QString&) → 1_SlotOnTextChangedLineEditSearch(const QString&))`。
* 槽实现：`FUN_1400515f0(param_1, QString)`（`chunk_006.c:7482`）：
  ```c
  FUN_140036ec0(local_118, qstring, 0x100);          // QString → 本地 8 位串（toLocal8Bit）
  if (local_118[0] == '\0') { FUN_140051490();       // 空串 → 全部 setItemHidden(false)
                              FUN_140050f60(...); }
  else { FUN_1400511f0(...);                         // 先全部 setItemHidden(true)
         FUN_1400513a0(param_1, DAT_14286d3b0, local_118); }  // 再对命中的枝"显形"
  ```
* **匹配谓词** `FUN_140050d00`（`chunk_006.c:7026`）——**按顺序试 5 个字段，任一命中即保留**：
  ```c
  if (FUN_140051020(param_1, param_2 + 0x210))           return 1;   // ① File
  if (FUN_140051020(param_1, param_2 + 0x004, param_3))  return 1;   // ② Name
  text = QTreeWidgetItem::text(item, col);  ...          // ③ 条目文本（列号由寄存器传入，Ghidra 丢失；[推断] 为 Type 列）
  if (FUN_140051020(param_1, local_e8, param_3))         return 1;
  FUN_140028380(local_e8,200,&DAT_140077db0, *(int*)(param_2+0x31c));  // ④ Size，格式 "%i"
  if (FUN_140051020(...)) return 1;
  FUN_140028380(local_e8,200,&DAT_140071900, *(int*)(param_2+0x318));  // ⑤ Address，格式 "0x%X"
  if (FUN_140051020(...)) return 1;
  return 0;
  ```
* **子串匹配** `FUN_140051020` → `FUN_1400277f0(pattern, text, len)`，逐字符经 `FUN_1400279f0()` 折叠后比较 ⇒ **大小写不敏感**（`FUN_1400277f0` 用 `FUN_1400279f0` 做大小写归一；`FUN_140027860` 是同类的小写化助手）。 [观测/推导]
* **命中"显形"** `FUN_140050e30`：从命中节点沿 `+0x3E8` 上溯到根，把整条祖先链 `setItemHidden(false)` 并 `setItemExpanded(true)`，同时展开自身子树 ⇒ 搜索是"连同父链一起显示"。 [观测]

### 6.4 勾选状态如何回传 [观测]

```
itemChanged(QTreeWidgetItem*, int)
   → SymbolSelection::_SlotOnTreeItemChanged  (FUN_14003c240 idx 0 → FUN_140051690)
        lVar2 = FUN_140051130(param_1, item, DAT_14286d3b0);   // 用 +0x360 反查记录
        if (checkState(item,4) == Checked)   FUN_140050760(param_1, lVar2);  // 追加到对话框私有链 param_1+0x50
        if (checkState(item,4) == Unchecked) FUN_140051360(param_1, lVar2);  // 从私有链摘除

BtnOK: "1_SlotOnReleaseBtnOK(void)"  (FUN_14003c240 idx 2 → FUN_140051590)
        FUN_14003e880();                       // ★ 清空全局已选列表 DAT_14286d3b8
        for (e = param_1[0x50]; e; e = e->next) FUN_14003df10(*e);   // ★ 按对话框顺序"尾插"回全局
        FUN_14004dc70(...); FUN_14004e120(...);  // 刷新
        vtable[0x198]();                         // → 关闭对话框（accept）
BtnCancel → FUN_14002b930()                      // reject
```
* 记录链节点：`{ record*, next* }` 共 **0x10 字节**（`FUN_140043d10(0x10)`）。全局链头 `DAT_14286d3b8`、计数 `DAT_14085d1e8`；对话框私有链头 `dialog+0x50`。 [观测]
* 重新打开对话框时勾选状态的恢复：`FUN_1400504d0` 用 `FUN_1400512f0` 在**全局已选链**里按 `Address(+0x318)` 且 `NameWithParents(+0x108)` 相等判定 → `setCheckState(4, 2)`。 [观测]

---

## 7. 多符号添加路径与样本布局

### 7.1 完整路径 [观测]

```
[菜单/工具条 "Add symbol"]
 F_14004d6c0  (chunk_006.c:4231)
   ├ 若 (mode==HSS 或 RTT-Plus) 且 DAT_14286d3b0==0:
   │    DAT_14286d379 = 1; FUN_14003e4e0(config+0x80, 0);     // 入队 ParseELFFile(flag=0)
   │    while (DAT_14286d379) { processEvents(); Sleep(10); } // ★ 轮询等待异步作业
   ├ 若符号树非空:
   │    dlg = new(0x60) SymbolSelection; FUN_140050140(dlg, mainWnd);
   │    QDialog::exec(dlg);                                    // 模态
   └ ...
    
[对话框内勾选] ─ 见 §6.4 ─
[OK] FUN_140051590 → FUN_14003e880()(清空) → 逐条 FUN_14003df10(记录) 尾插全局链
[开始采样] FUN_140041f30 按全局链顺序构造 HSS 描述符数组
```

### 7.2 顺序确实影响样本布局 [观测]

`FUN_140041f30`（`chunk_005.c:3710`）：

```c
_Dst = FUN_140043d10((DAT_14085d1e8 + 1) * 0x10);   // n+1 项 × 16B
DAT_14285da70 = _Dst;  DAT_14085d184 = 4;           // ★ 偏移从 4 开始（前 4 字节 = µs 时间戳）
for (plVar2 = DAT_14286d3b8; plVar2 != NULL; plVar2 = plVar2[1]) {   // ★ 按"已选链"顺序
    lVar1 = *plVar2;
    *(int *)(lVar1 + 0x38c) = DAT_14085d184;        // 记录样本内偏移
    DAT_14085d184 += *(int *)(lVar1 + 0x31c);       // 累加 Size
    *puVar6 = *(undefined4 *)(lVar1 + 0x318);       // HssVar.addr
    puVar6[1] = *(undefined4 *)(lVar1 + 0x31c);     // HssVar.size
    puVar6 += 4;
    /* 复位统计/指针：+0x3D0=0 +0x3C8=0 +0x390=0 +0x3A8=0 +0x398=-DBL_MAX +0x3A0=+DBL_MAX +0x388=0 */
}
puVar6[1] = 0;                                       // 终止项
iVar4 = JLINK_HSS_Start(_Dst, n, config+0x5A8 /*µs*/, 1);
```

* 顺序 = **勾选先后 / 会话链顺序**（尾插），**样本内偏移 = 4 + 前序各变量 Size 之和**。
  ⇒ **顺序影响样本字节布局**（以及 `+0x38C`、`JLINK_HSS_Start` 的变量表顺序）。 [观测]
* 同一逻辑在 RTT 模式复用：`FUN_140042a60` 用同样方式重算 `+0x38C`（HSS: 基址 4；RTT-Plus: 基址 4；RTT 无时间戳: 基址 0，即 `-(DAT_14285d9c5 != 0) & 4`）。 [观测]
* 解包侧 `FUN_14003fa70` 按 `DAT_14286d3b8` 链顺序逐变量 `memcpy(ring + idx*8, sample + +0x38C, Size)`，每变量固定 **8 字节/样本**槽位，环深 **0x800000 = 8,388,608**（`0x4000000`/8）。 [观测]
* "重解析 & Match"时的自动重选：`FUN_140041870`（建树时逐个符号）用 `FUN_1400429c0` 判身份（`File && Name && NameWithParents && Type` 全等）命中当前已选链 → `FUN_14003ffd0` 挂入临时链 `DAT_14285da08`；解析尾部整体替换 `DAT_14286d3b8`。 [观测]

---

## 8. 未知点

1. `+0x35C` 布尔的确切语义（初值 0；仅见 `chunk_006.c:2759` 读取与 `chunk_007.c:417` 置 1 两处）。 [未知]
2. `+0x390`（8 字节）从未被读，疑似 malloc 残留/保留字段。
3. `FUN_1400404c0` 的**真实参数个数**：Ghidra 显示 4 参调用而函数体用到 6 参（`param_5` 深度、`param_6` 是否根）；调用点处为栈传参，反编译丢失。结论"深度上限 33 / 根节点处理"基于函数体，行为可信但**调用约定细节未定**。
4. ELF 符号表筛选策略：是否包含 `STB_LOCAL`（static）符号、是否跳过 `STT_FUNC`（除 tag==0x2e 之外的过滤），未找到显式代码；`FUN_140007a10 → FUN_140008f40(...,0x34)` 的 `0x34` 是内部索引种类而非标准 `SHT_*`。 [未知]
5. 位域样本读取：`case 0xe` 只从样本里取 **32 位 raw**（`*(uint*)`），>32 位存储单元的位域如何处理 [未知]。
6. TypeID 3/4 的原始语义（现代码不产生）。
7. `FUN_14000aaf0` 打印的统计（`File Size: %llu`、`DWARF Version: %d`…）输出到哪个渠道（日志窗口/文件）未追踪。
8. 静态库的**准确版本**与上游项目名未确定（libdwarf + libelf 家族，DWARF5 部分支持）。

---

## 9. 对 OpenScope 的启示

1. **ELF 载入只需"整文件读入 + 4 步解析"，不需要 mmap。**
   J-Scope 用 `fopen/ftell/fread` 把整个 ELF 读进堆（`FUN_140002f60`），随后一次性解析 `.debug_info/.debug_types/.debug_line`。
   对 10 MB 级 MCU ELF 完全够用，实现简单、跨平台。**同时打开的 ELF 上限 2** 也值得照抄为资源约束。 [观测]

2. **必须校验 `e_type == ET_EXEC(2)`。**
   `FUN_14000dfc0` 拒绝 `.o`(ET_REL) 与共享库(ET_DYN)，报 "not an executable ELF file"。
   OpenScope 若想让用户直接拖 `.o`/`.axf`，需自己放宽这一条（并自行处理未重定位地址）；若照抄，用户的 `-c` 中间产物会被拒——**这是最常见的"用户以为坏掉了"场景**。
   建议：**接受 ET_REL/ET_DYN 但在 UI 明确提示地址未重定位**。

3. **类型分类器极简且可 1:1 照搬（约 60 行）。**
   判据 = `名字 ∈ {float, double, _Bool}` + `Size ∈ {1,2,4,8}` + `IsSigned` 标志，映射到 1/2/5/6..13；`DW_AT_bit_size` 存在 ⇒ 位域(14)。
   J-Scope **不支持 64 位整型与 16 字节浮点**（TypeID 9/13 在树里被禁用；无 long double 分支）。
   OpenScope 应把 **int64/uint64 补上**（绘图加 double 通道即可），并显式支持 **bitfield**（J-Scope 的做法值得抄：位宽/位偏移存元数据，取值时 `(raw>>off)&mask`，不改变样本布局）。

4. **符号可见性策略值得整体照抄：**
   顶层按**源文件分组**（同名符号在多个 .c 里不混淆）；函数不显示；`struct/union` 作为**容器节点**（不可勾选）展开到叶；**数组逐元素成叶**（`[n]` 命名，上限 30000，递归深度 33）；叶的 `NameWithParents` 用 `.` 连接。
   这套规则让"选一个 `g_app.motor.speed`"既精确又不会让树爆掉。**上限（30000/深度 33）是必须抄的护栏**——否则一个 `uint8_t buf[1<<20]` 会直接卡死 UI。

5. **采样布局与选择顺序强耦合，尽早固定契约。**
   `+0x38C = 4 + Σ前序 Size`，`JLINK_HSS_Start` 的描述符数组顺序 = 用户勾选顺序。
   OpenScope 与"网络远端"协商变量布局时，**必须把这份有序变量表作为唯一真源**（发送方定序、接收方照用），否则同一份数据两端解出的波形会错位——这正是 J-Scope 用 `Offset` 属性把布局写进 `.jscope` 的原因。 [观测]

6. **"ELF 改动检测"是纯时间戳 + 显式重解析，值得照抄。**
   `GetFileTime` 比对 + 二选一对话框（Reparse & **Match** / Reparse & **Clear**），Match 用 `(File, Name, NameWithParents, Type)` 四元组配对旧符号的显示选项。
   OpenScope 在"重新编译后自动重载 ELF"时，用同样的四元组匹配即可保留颜色/量程/绘图样式，避免用户每次重编译都要重配。

7. **UI 侧的搜索语义值得抄：5 字段（File/Name/Type/Size(' %i')/Address('0x%X')）子串、大小写不敏感、命中后连同父链一起显形、未命中全部隐藏。**
   比"只搜名字"好用得多（用户常按地址找变量）。实现上就是遍历一次符号树，代价极低。

---

## 10. 需要补充反编译（本次 dump 中缺失或不足）

| 目标 | 原因 |
|---|---|
| `FUN_1400404c0` 的**调用点**（`FUN_14003d210` / `FUN_140040320` / 自身递归） | 需要**完整参数列表**（栈参数 `depth`、`isRoot`）才能确证深度/根逻辑；建议看反汇编而非 C |
| ELF 符号表枚举 `FUN_140008f40(p, 0x34)`、`FUN_1400073c0`、`FUN_1400088e0` | 确认符号种类过滤（LOCAL/FUNC/SECTION）与 `0x34` 索引语义 |
| `FUN_140002fb0`（2578 字节类型名拼接器）全文 | 确认 `Type` 字符串的**规范形式**（`struct X *`、限定符、typedef 展开），这是类型分类器能否命中的前提 |
| `FUN_14000aaf0`（2874 字节 dump/统计） | 确认 `File Size/debug_info size/DWARF Version` 等输出渠道（日志窗口？`-log` CLI？） |
| `FUN_140053420` / `FUN_14005660` 区（TableWidget 取值） | `FUN_140053420` 是 `FUN_14003eaa0` 的第 3 个调用者，用于位域在表格中的显示 |
| `FUN_14002d5c0` / `FUN_14002bc30`（Target Device 对话框全链） | 本专题只需定性；若要抄"过滤表头"（UtilFilterHeader）需细读 |
| `FUN_14000ca30` / `FUN_14000bf80`（`.debug_info` / 行信息解析） | 若需确认"是否解析 DWARF 变量作用域/局部变量"及 `Source Language` 处理 |

---

*本报告全部结论可回溯到 `dumps/alldecomp/chunk_*.c` 的行号与 `.rdata` 常量表；未改动 `dumps/`。*
