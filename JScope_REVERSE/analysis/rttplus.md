# J-Scope 9.66 — RTT / RTT-Plus 协议逐字节实证

**专题**：J-Scope 到底往目标端发了什么字节（RTT-Plus 配置串）、以及 RTT 三种模式（RTT-Plus / RTT-Normal VarFormat / HSS 对照）的完整数据通路。

**证据来源**
- 反编译：`dumps/alldecomp/chunk_000..008.c`、`dumps/decomp_rtt.txt`、`dumps/funcmap.txt`、`dumps/globals.txt`（Ghidra 12.1.4 产物，只读）。
- **原文件机器码核对**：`C:\Program Files\SEGGER\JLink_V966\JScope.exe`（PE32+ x64，基址 0x140000000），用 Python + capstone 直接反汇编关键函数（未运行 Ghidra、未改 `dumps/`）。凡标 **[观测]** 的指令/常量均为该 PE 的真实机器码。
- 公开文档：UM08028 J-Scope（kb.segger.com，本次抓取原文用于第 7 节对比）。

**证据分级**：[观测] 机器码/反编译直接可见；[推导] 由多条观测必然推出；[推断] 合理解释但有替代可能；[未知] 无法确定。

---

## 0. 结论速查（先看这张表）

| 问题 | 结论 | 证据 |
|---|---|---|
| 谁“发送配置串”？ | **`FUN_140042630`**（不是 `FUN_140042a60`） | [观测] `JLINK_RTTERMINAL_Write` 全程序仅 1 个调用者 = `FUN_140042630` |
| `FUN_140042a60` 是什么？ | **样本布局/偏移计算器**（写 `var+0x38C` 偏移、累加出 `DAT_14085d184`=样本字节数） | [观测] `mov [rcx+38Ch], edx / add edx, [rcx+31Ch]` |
| 配置串条目大小 | **16 字节/变量**（不是 8 字节），缓冲 = `(nVars+1)*16` | [观测] `inc esi; shl esi,4` / `lea r8,[r8+0x10]` |
| 条目字段顺序 | **Addr 在前**：`+0..3 = Addr(u32 LE)`、`+4..7 = 0`、`+8..11 = Size(u32 LE)`、`+12..15 = 未初始化` | [观测] `mov [r8-10h], rcx`(8B) / `mov [r8-8], ecx`(4B) |
| 结束标记 | `16*nVars` 起 **12 字节 0**（第 13..16 字节未写，内容不定） | [观测] `mov [r14+rax*8], rbp` + `mov [r14+rax*8+8], ebp` |
| 写通道号 | `DAT_14085d1B8`（= `JScope_Config` 下行通道号，`FUN_1400424f0` 填充） | [观测] `mov ecx,[rip+…]`→0x14085D1B8 |
| 目标应答读哪条通道 | **上行数据通道** `DAT_14085D188`（不是 Config 通道），**整 256 字节** | [观测] `FUN_140040ed0` 内 `mov ecx,[rip+…]`→0x14085D188 |
| 应答语义 | 累计 256B 且 `byte[0]==0` → 成功；`byte[0]!=0` → 错误串；101 次读不到 256 → `0xFFFFFFFE` 超时 | [观测] |
| 时间戳判定（RTT-Plus） | 通道名**大小写不敏感**等于 `JScope_Data_t` → `+0x5A5(IsRTTWithTimeStamp)=1` | [观测] `FUN_140027760`（查 `DAT_140063CA0` 折叠表） |
| RTT-Normal 变量上限 | **12**（名字缓冲 32B：`"JScope_"`7 + 2×N + NUL ≤ 32） | [观测]+[推导] |
| “发配置串”前会干什么 | **`JLINKARM_Reset()` → `JLINKARM_Go()`**（每次尝试都复位目标） | [观测] `FUN_1400427b0` |
| RTT 读缓冲 | `readBufSize = max(0x2000, 通道缓冲大小)`，单次读 `min(readBufSize, 2MB) - carry` | [观测]（**修正旧报告 §7.3 的 min 说法**） |
| carry 缓冲 | 64 字节，`DAT_14285DA30`（计数 `DAT_14285DA24`），超出 64B 的残样本被丢弃 | [观测] |

---

## 1. RTT-Plus 配置串：逐字段实证（`FUN_140042630`）

### 1.1 调用点与时机 [观测]

`FUN_140042630` 唯一调用者 `FUN_1400427b0`（“RTT-Plus 握手”），`FUN_1400427b0` 唯一调用者 `FUN_14003d5c0`（采样线程体）。

`FUN_14003d5c0` 中 [观测]（chunk_004.c:7477-7496）：

```c
iVar2 = FUN_14003ef60();        // 连接 + （仅 Normal RTT 时）FUN_140042100 通道安装
FUN_140042a60();                // 样本布局
if (DAT_14285d9c4 != '\0') JLINK_RTTERMINAL_Control(0,0);   // RTT Start
if ((DAT_14285d9c6 != '\0') && (DAT_14285d414 != '\0')) {   // IsRTTPlus && 变量表有变化
    iVar2 = FUN_1400427b0();    // ← 复位目标 + 发配置 + 等 256B 应答
}
```

`DAT_14285d414` = “变量集合变化”脏标志，由 `FUN_14003df10`（加变量）与 `FUN_14003f0a0`（删变量）置 1，写配置成功后由 `FUN_140042630` 清零 [观测]。

### 1.2 `FUN_1400427b0` 的完整时序 [观测]

机器码顺序（0x1400427E7 → 0x1400428CD）：

```
if (DAT_14285D415 == 1 && FUN_140040ed0(buf, 256) == -1)   // 上次超时后，补读目标迟到错误
        MessageBox("Got pending Error from Target.")
JLINK_RTTERMINAL_Control(1, 0)                     // RTT Stop
if (DAT_14285D208 != 1) FUN_14003f390()            // → JLINKARM_Reset()   ← 复位目标
FUN_14003f890()                                    // → JLINKARM_Go()      ← 目标运行
JLINK_RTTERMINAL_Control(0, 0)                     // RTT Start
if (FUN_140042100(&cfg) < 0) return;               // 找通道 JScope_Data[/_t] + JScope_Config，装布局
FUN_140042630()                                    // ★ 写配置串到 JScope_Config
n = FUN_140040ed0(buf, 256)                        // ★ 读 256B 应答
  n == -2 → "Timeout. No response from target!" ，DAT_14285D415 = 1
  n == -1 → 用户可见 "Target Error: <目标串>\nRetry?"
  n ==  0 → 成功返回
失败时弹 "Error." / "No-Yes" 的 "%s\nRetry?" 对话框，最多重试 1 次（共 2 次尝试）
```

> `FUN_14003f390` = `JLINKARM_Reset()`（+ `DAT_14285D414=1`）、`FUN_14003f890` = `JLINKARM_Go()` [观测，chunk_005.c:1522/1831]。这与公开文档“J-Scope 先复位目标再发配置”一致。
> **触发条件**：RTT-Plus 且变量表有变化；因此改一次变量表 → 复位一次目标 + 重发配置，这是目标端必须能多次接受配置的原因。

### 1.3 缓冲区大小计算 [观测]

```asm
14004263B  mov esi, [rip+0x81ABA7]   ; esi = DAT_14085D1E8 = 变量个数 nVars
140042641  inc esi
140042643  shl esi, 4                ; esi = (nVars + 1) * 16   ← 总字节数
140042646  mov ecx, esi
140042648  call 0x140043D10          ; 自研分配器 FUN_140043D20(size, 0)（不零初始化）
140042650  test rax,rax / jne ...
          失败 → "Insufficient memory!" ; 返回 -1
```

- 分配器 `FUN_140043D10` → `FUN_140043D20`：自定义堆（可选 +0x40 校验尾巴），**不保证清零** [观测，0x140043D20]。因此条目里未写到的字节是**堆垃圾**，不是 0。

### 1.4 每条目的写入顺序与宽度 [观测]

```asm
1400426A0  mov rax,[rdx]            ; var = 链表节点->变量
1400426A3  lea r8,[r8+0x10]         ; 输出指针 += 16（先加）
1400426A7  inc r9d                  ; 计数
1400426AA  mov ecx,[rax+31Ch]       ; ecx = var->Size      ← +0x31C 是 Size
1400426B0  mov [r8-8],ecx           ; 4 字节写 entry+8
1400426B4  mov rax,[rdx]
1400426B7  mov ecx,[rax+318h]       ; ecx = var->Addr      ← +0x318 是 Addr
1400426BD  mov [r8-10h],rcx         ; 8 字节写 entry+0（零扩展 → entry+4..7 = 0）
1400426C1  mov rdx,[rdx+8]          ; next
```

字段语义 `+0x318=Addr / +0x31C=Size` 的三重独立佐证 [观测]：
1. `FUN_14003dfa0`（VarFormat 解析器建变量）把格式串里的**字节数**写进 `+0x31C`：`*(undefined4 *)(lVar2 + 0x31c) = param_1;`（chunk_005.c:195）。
2. `FUN_14003feb0` 用 `memcpy(dst, sample + [var+0x38C], [var+0x31C])` —— 第三参是字节长度。
3. `FUN_140042a60` 用 `+0x31C` 累加出样本总长。
   （工程文件 XML 侧同样：`"Address"` ↔ `+0x318`，见 chunk_005.c:7594。）

**终止符** [观测]：

```asm
1400426CA  movsxd rax,r9d           ; rax = nVars
1400426CF  add rax,rax              ; rax = 2*nVars
1400426D4  mov [r14+rax*8+8],ebp    ; 4 字节 0  → 16*nVars + 8
1400426D9  mov [r14+rax*8],rbp      ; 8 字节 0  → 16*nVars + 0
                                    ; 16*nVars + 12..15 未写
```

### 1.5 写调用参数与短写处理 [观测]

```asm
1400426E0  inc edi ; cmp edi,0x64 ; ja 失败        ; 最多 100 次写尝试，超限 → 返回 -1
1400426E7  mov ecx,[rip+0x81AACB]  ; 通道号 = DAT_14085D1B8
1400426ED  mov r8d,esi             ; 剩余长度 = total - written
1400426F0  mov edx,ebx             ; written
1400426F2  sub r8d,ebx
1400426F5  add rdx,r14             ; 源 = buf + written
1400426F8  call 0x140029220        ; JLINK_RTTERMINAL_Write(chan, buf+written, size-written)
1400426FD  test eax,eax ; jg 前进  ; ret > 0 → written += ret，立即续写
140042701  call 0x1400443C0        ; ret <= 0 → Sleep(1) 后从**同一偏移**重试
140042713  mov [DAT_14285D414], 0  ; 全部写完 → 清脏标志（失败路径不清）
```

- 返回：`0` 成功；`-1`（0xFFFFFFFF）分配失败或 100 次写不完 [观测]。
- **注意**：`FUN_1400427b0` **不检查** `FUN_140042630` 的返回值，直接进入 256B 应答等待 —— 配置写失败最终表现为“超时” [观测]。

### 1.6 伪结构体与字节示例

```c
/* J-Scope 9.66 实测线上格式（主机 → 目标 JScope_Config 下行缓冲） */
typedef struct {          // 16 字节，LE
    uint32_t Addr;        // +0   变量地址（写入时 8 字节存储，高 4 字节为 0）
    uint32_t Zero;        // +4   恒为 0
    uint32_t Size;        // +8   变量字节宽度（来自 ELF / VarFormat 尺寸位）
    uint32_t Unused;      // +12  **未写入**，malloc 残留，不可依赖
} JS_CFG_ENTRY;
/* 之后紧跟一个 16 字节槽：+0..11 为 0（结束标记），+12..15 未写入 */
```

示例：2 个变量 `0x20000000/4B`、`0x20001004/2B`，`nVars=2` → 写 48 字节：

```
偏移  字节                                     含义
00    00 00 00 20 00 00 00 00  04 00 00 00 ?? ?? ?? ??   Addr=0x20000000 Size=4
10    04 10 00 20 00 00 00 00  02 00 00 00 ?? ?? ?? ??   Addr=0x20001004 Size=2
20    00 00 00 00 00 00 00 00  00 00 00 00 ?? ?? ?? ??   结束（前 12B 为 0）
```

变量顺序 = 内部变量链表顺序（`DAT_14286D3B8`）：RTT-Plus 下即**界面/工程里变量列表的顺序**，与 `FUN_140042a60` 算出的样本偏移顺序完全一致 [观测]。

---

## 2. 应答读取与错误处理（`FUN_140040ed0`）

### 2.1 逐条确认 [观测]（机器码 0x140040ED0 起）

```asm
140040EF1  test rcx,rcx ; jne ...      ; param_1 == NULL → 返回 0xFFFFFFFD
140040F08  xor edi/ebx,edi/ebx         ; edi = 累计字节，ebx = 尝试次数
LOOP:
140040F12  mov ecx,[rip+0x81C270]      ; 读通道 = 0x14085D188 = **上行数据通道**
140040F18  lea rdx,[rsp+0x20]          ; 256B 栈缓冲
140040F1D  mov r8d,0x100
140040F23  inc ebx
140040F25  call 0x1400291B0            ; JLINK_RTTERMINAL_Read(chan, buf, 256)
140040F2A  add edi,eax                 ; 累计（负返回值也会被累加！）
140040F2C  cmp edi,0x100 ; je OK
140040F34  call 0x1400443C0            ; Sleep(1)
140040F3E  cmp ebx,0x64 ; jbe LOOP     ; 最多 101 次
140040F43  mov eax,0xFFFFFFFE          ; ← 超时
OK:
140040F4A  cmp byte [rsp+0x20],0 ; je 成功0
140040F51  ... FUN_14002C8F0(param_1, param_2, "Target Error: %s", buf)
140040F67  mov eax,0xFFFFFFFF          ; ← 目标错误串
```

- **读的是上行数据通道**（`DAT_14085D188`，即 `JScope_Data` / `JScope_...`），**不是** Config 通道。Config 通道在 J-Scope 里**从不被读**（`JLINK_RTTERMINAL_Read` 全程序只有 2 个调用者：本函数与数据读取 `FUN_140042260`）[观测]。
- 必须**累计正好 256 字节**；读到的第一字节即判据，其余 255 字节不校验 [观测]。
- 超时 = 101 次 × `Sleep(1)` ≈ 100 ms（Windows 上 Sleep(1) 实际 1–15 ms，故真实上限可能到 ~1.5 s）[推断]。
- 返回值语义：`0` 成功；`0xFFFFFFFF` 目标错误（错误串经 `FUN_14002C8F0` 格式化进调用方缓冲）；`0xFFFFFFFE` 超时；`0xFFFFFFFD` 空指针（本调用路径不会出现）[观测]。
- `FUN_14002C8F0` 是 vsnprintf 风格**写入缓冲**，不是弹窗；因此用户最终看到的是 `FUN_1400427b0` 里的 `"Target Error: <文本>\nRetry?"` 对话框 [观测]。
- 调用者：`FUN_140040ed0` ← 仅 `FUN_1400427b0`（RTT-Plus 握手）；其中“补读迟到错误”的分支用 `DAT_14285D415==1` 做标记 [观测]。
- 与“256 字节 0”的对应关系：**目标必须向上行缓冲写 ≥256 字节，且第 1 字节为 0**；建议一次写满 256 字节（上行缓冲须 ≥256B），否则 J-Scope 只能靠多次 Read 累积，窗口内凑不齐 256 即判超时并复位重试 [推导]。

---

## 3. `JScope_Data` / `JScope_Data_t` 与样本布局

### 3.1 名字判定 [观测]

`FUN_140042100`（RTT 通道安装）：

```c
iVar1 = FUN_1400424f0("JScope_Data", 0xb, &DAT_14085d188);   // 上行数据通道（前缀匹配）
iVar1 = FUN_140027760(&DAT_14085d190, "JScope_Data_t");      // 名字比较
if (iVar1 == 0) *(undefined1 *)(param_1 + 0x5a5) = 1;        // IsRTTWithTimeStamp
_DAT_14085d1bc = 1;                                          // Config 通道描述结构 +4 = 1
iVar1 = FUN_1400424f0("JScope_Config", 0xe, &DAT_14085d1b8); // 下行配置通道
```

- `FUN_140027760` 是**大小写不敏感比较**：`cVar1 = (&DAT_140063ca0)[*param_2];`，实测该 256 字节表把 `0x41..0x5A` 映射为小写、其余恒等 [观测]。所以目标端通道名 `jscope_data_t` 也会被认作带时间戳。
- 通道**查找**用的是 `strncmp(name, prefix, len)` → **区分大小写、前缀匹配**（`JScope_DataXXX` 也算匹配）[观测]。
- 时间戳标志 `+0x5A5` 只在 RTT-Plus 下由“通道名是否有 `_t`”决定；Normal 下由格式串里的 `t` 决定；HSS 下恒为“有 4 字节时间戳”（见 3.3）。

### 3.2 样本布局计算器 = `FUN_140042A60`（本任务原假设的“发配置串”其实是它）[观测]

```asm
140042A60  cmp byte [0x14285D9C4],0      ; IsRTTSession ?
           je  -> edx = 4                ; 非 RTT（HSS）：偏移基址 4
           movzx eax,[0x14285D9C5]       ; IsRTTWithTimeStamp
           neg al ; sbb edx,edx ; and edx,4   ; RTT：有 ts → edx=4，无 ts → 0
140042A7E  mov rax,[0x14286D3B8]         ; 变量链表头
LOOP:      mov rcx,[rax]
           mov [rcx+38Ch], edx           ; ★ 该变量在样本中的字节偏移
           mov rcx,[rax] ; mov rax,[rax+8]
           add edx,[rcx+31Ch]            ; ★ 累加 Size
           jne LOOP
           mov [0x14085D184], edx        ; ★ 样本总字节数 sampleSize
```

- 偏移基址：**RTT 有 `_t` → 4（4 字节时间戳在最前）**，RTT 无 `_t` → 0，HSS → 4（HSS 采样包自带 4 字节 µs 时间戳）[观测]。
- 变量按链表顺序紧密排布，宽度 = 各自 `Size(+0x31C)`，**无对齐填充** [观测]。
- 消费者：`FUN_14003FA70`（`param_2 += DAT_14085D184` 逐样本步进）、`FUN_14003FEB0`（按 `+0x38C/+0x31C` 取变量值）、`FUN_140042260`/`FUN_140040FA0`（按 `DAT_14085D184` 求整样本数）[观测]。
- 全局 `DAT_14285D9C4/9C5/9C6` 就是配置结构 `&DAT_14285D420` 的 `+0x5A4/+0x5A5/+0x5A6`（IsRTTSession / IsRTTWithTimeStamp / IsRTTPlus）[观测，0x14285D420+0x5A4=0x14285D9C4]。

`FUN_14003DE60` 里有一段“**临时把 `+0x5A5` 置 1 → 重算布局 → 重新解包 → 还原**”的巧用 [观测]：导入/回放路径需要按“带时间戳”的口径重排历史数据后再回到原布局，说明布局函数是无副作用的纯计算（只依赖链表与两个标志）。

### 3.3 与 HSS 的异同 [观测]

| | RTT-Plus | RTT-Normal (VarFormat) | HSS |
|---|---|---|---|
| 变量来源 | ELF / 工程文件（`+0x318` 地址、`+0x31C` 宽度） | 通道名格式串（地址恒 0，仅偏移有意义） | ELF |
| 描述符去哪 | RTT 下行 `JScope_Config`（16B/条 + 12B 0） | 不发送（编码在通道名里） | `JLINK_HSS_Start(desc, n, period, 1)` 的数组 |
| 描述符排布 | `{Addr@0, 0@4, Size@8, pad@12}`，**stride 16**，pad 未初始化 | — | `{Addr@0, Size@4, 0, 0}`（先 `memset` 全 0），**stride 16**，终止项 16B 全 0 |
| 时间戳 | 由 `JScope_Data_t` 决定 | 由格式串 `t` 决定 | **恒有**（`DAT_14085D184` 初值 = 4） |

HSS 侧机器码 [观测，`FUN_140041F30` 0x140041FD0-0x140041FFB]：`mov [rcx+38Ch], r9d / add r9d,[rcx+31Ch]`（与 `FUN_140042A60` 同语义）、`mov [rdi],eax`(Addr) / `mov [rdi+4],eax`(Size) / `add rdi,0x10`。

**共用点**：同一变量链表、同一布局语义、同一解包函数 `FUN_14003FA70`、同一“样本内 4 字节 µs 时间戳在偏移 0”的约定、同一“每变量每样本 8 字节槽”的内部存储（`FUN_14003FEB0`：`memcpy(var->buf + idx*8, sample + var->offset, var->size)`，目标缓冲 `/ 0x4000000` = 64 MB/变量）[观测]。差别只在“字节从哪来 + 谁做打包”。

---

## 4. RTT-Normal（VarFormat）补全

### 4.1 变量上限 12 的证据 [观测]+[推导]

- 描述结构里名字字段 = **32 字节**：`DAT_14085D190 = desc+8`，下一个字段在 `desc+0x28`（=8+32），且解析器 `FUN_140027140(local_148, &DAT_14085d190, 0x20)` **精确拷贝 32 字节** [观测，0x14003FCD1]。
- 解析器按 **2 字符/变量** 消费（类型字符 + 尺寸字符），并且要求这一对**两个字符都非 NUL**：`cmp byte [rsp+rsi+0x31], 0 ; je 结束` [观测]。
- 前缀 `"JScope_"` 占 7 字符（解析器先扫到**第一个 `_`** 再把 `+1` 当格式串起点）[观测，0x14003FCD6-0x14003FCF5]。
- ⇒ `7 + 2N + 1(NUL) ≤ 32` → **N ≤ 12**（12 时 7+24=31，第 32 字节为 NUL；第 13 对需要第 33 字节，必然被 NUL 截断）[推导]。与 UM08028 原文 “There is a limit of twelve variables in total (including timestamp)” 完全吻合。

### 4.2 `t4` 时间戳位 [观测]

```asm
14003FD3D  cmp al,0x74 ; jne ...          ; al = tolower(类型字符)（FUN_140027860）
14003FD44  mov byte [r14+5A5h],1          ; IsRTTWithTimeStamp = 1
14003FD4C  call 0x14004EC40               ; 把配置结构拷给 UI/其他线程
14003FD51  jmp 0x14003FE70                ; ← 直接跳到“消费 2 字符”的循环尾
```

- `t` **不产生变量**、**不校验第 2 字符**（`t8`、`tX` 一样置标志并吃掉 2 字符）；尺寸位被完全忽略 [观测]。
- 效果链：`+0x5A5=1` → `FUN_140042A60` 样本布局基址 = 4 → 样本首 4 字节被 `FUN_14003FF40` 当时间戳原样取出。**解析器先跑、布局后算**（`FUN_140042100` 中：`FUN_14003FC90` → 检查 `DAT_14085D1E8!=0` → `FUN_140042A60`），所以顺序正确 [观测]。
- 副作用：`t` 不要求在首位；但布局永远把它当“偏移 0 的 4 字节前缀”，若目标端把时间戳放在别处，两端解析必然错位 [推导]。

### 4.3 非法/不支持格式字符 [观测]

- 类型字符不是 `t/b/f/u/i`（`tolower` 后）→ 整个二元组被**静默忽略**，指针仍前进 2（`add rsi,2`），后续继续解析，不报错、不终止 [观测，0x14003FE70-0x14003FE79]。
- 支持表（`FUN_14003DFA0(size, type, 0, name)` 的 type 码）[观测]：

| 字符 | 接受尺寸 | 类型码 | 备注 |
|---|---|---|---|
| `t` | 任意（忽略） | — | 只置时间戳标志 |
| `b` | 1 | 5 | 其它尺寸 → 跳过（名字仍会被 sprintf，但丢弃） |
| `f` | 4 / 8 | 1 / 2 | 8 = double（**文档只写 4，见 §7**） |
| `u` | 1/2/4/8 | 6/7/8/9 | |
| `i` | 1/2/4/8 | 10/11/12/13 | |

- 解析循环在读每个二元组**之前**判断 `type==NUL || size==NUL` → 末尾落单的字符被忽略 [观测]。
- 生成的变量名 = `"%d Byte bool/float/unsigned/signed RTT Data %d"`，参数为 `(字节数, 已成功加入的变量序号)`（机器码把序号 `ebp` 放栈上当第 5 参，Ghidra 反编译漏掉了这个参数）[观测，0x14003FE43-0x14003FE54]。
- 变量地址 `+0x318` 在 VarFormat 路径下**从不赋值**（结构分配时清 0），取值只用偏移 → **与地址无关** [观测]。

### 4.4 `FUN_1400424F0`（通道查找）匹配规则 [观测]

```c
/* param_1 = 前缀串, param_2 = 前缀长度, param_3 = 描述结构指针 */
FUN_140042F90("Waiting until RTT Control Block is found...");
for (i = 0; ; i++) {
    Sleep(5);
    uVar1 = JLINK_RTTERMINAL_Control(3, param_3 + 1);   // 取缓冲个数（返回；同时写 desc+4）
    if (i == 2000) → -1                                 // 2000 × 5ms ≈ 10 s 放弃
    if (uVar1 != 0xFFFFFFFE) break;                     // 0xFFFFFFFE = 控制块尚未找到，继续等
}
if ((int)uVar1 < 0) return -1;
if (uVar1 == 0)     return 0;                            // 0 个通道：直接“成功”（见下）
do {
    *param_3 = uVar5;                                    // desc->Chan = 索引
    JLINK_RTTERMINAL_Control(2, param_3);                // 取该索引描述（名字/SIZE）
    if (strncmp((char *)(param_3 + 2), param_1, param_2) == 0) return 0;  // 命中
    uVar5++;
} while (uVar5 < uVar1);
return 0xFFFFFFFF;                                       // 未找到
```

- 匹配规则：**区分大小写的 strncmp 前缀匹配**（`param_3+2` 即 `desc+8` = 名字字段）[观测，0x1400425D0-0x1400425E6]；`Control` 的 `edx` 参数就是描述结构指针，`strncmp(desc->acName, prefix, len)`。
- 通道号返回值：写回 `*param_3`（即 `DAT_14085D188` = Data 通道号、`DAT_14085D1B8` = Config 通道号）[观测]。
- 只按**名字**判定，不看数据方向；`+4` 字段在搜索前被设成 0（Data/上行）或 1（Config/下行），随后 GetNumBufs 也会写这里 —— 该字段语义见 §8 未知点 [观测+推断]。
- 通道 0 个时返回 0 → 调用方按“成功”继续；Normal 模式随后因 `DAT_14085D1E8==0` 报 “did not contain a valid VarFormat” [观测]。

---

## 5. RTT 读取路径与 carry（`FUN_140042260`）

### 5.1 读缓冲分配 [观测]（`FUN_14003EF60`）

```asm
14003EFB6  mov ecx,0x2000
14003EFBB  lea rdx,"RTT read buffer allocation"
14003EFC2  cmp [DAT_14085D1B0], ecx          ; 上行通道缓冲大小
14003EFC8  cmova ecx, [DAT_14085D1B0]        ; ecx = max(0x2000, 通道缓冲大小)
14003EFCF  mov [DAT_14285D1FC], ecx          ; 分配尺寸
14003EFD5  call 0x140043D20                  ; 分配
```

> **修正**：旧报告（`JSCOPE_ANALYSIS.md` §7.3）写的 “`readBufSize = min(0x2000, channelBufSize)`” **方向相反**，实测是 `max`。通道缓冲 ≥8 KB 时按通道大小分配，否则至少 8 KB。
> `DAT_14085D1B0` = 描述结构 `+0x28`（32 字节名字字段之后的下一个 32 位字段，由 `Control(2,…)` 填充）[观测，`globals.txt`: `14085d1b0 refs=1 [FUN_14003ef60]`]。

### 5.2 单次读取与 carry 拼接 [观测，0x140042260]

```asm
140042268  cmp byte [DAT_14085D183],0 ; je 返回0        ; RTT 会话未运行 → 立即返回
140042289  call FUN_140041540                           ; 返回 1 = 尚无任何样本（idx/prev/oldest 全 0）
   → 是：DAT_14285DA24 = 0，并把 64B carry 区清零（4×movups xmm0）
   → 否：r14d = DAT_14285DA24（carry 字节数）
1400422C3  edi = DAT_14285D1FC ; eax = 0x200000
1400422E7  cmova edi,eax / sub edi,r14d                 ; 读长 = min(alloc, 2MB) - carry
1400422ED  memcpy(buf, &DAT_14285DA30, carry)           ; 旧残字节搬到最前
1400422FF  JLINK_RTTERMINAL_Read(DAT_14085D188, buf+carry, 读长)
140042304  test eax,eax ; js → 返回 -1
140042320  div [DAT_14085D184]                          ; edx = 总字节 % sampleSize
140042336  FUN_14003FA70(总字节 - 余数, buf)             ; ★ 与 HSS 共用的 UnpackSamples
140042347  eax = 0x40 ; cmova edi,eax                   ; carry = min(余数, 64)
14004235E  memcpy(&DAT_14285DA30, buf + 完整部分, carry)
140042363  DAT_14285DA24 = carry
```

- **carry 区**：`DAT_14285DA30` 起 **64 字节**，字节数 `DAT_14285DA24`；仅保留“最后一个不完整样本”的**前 64 字节** [观测]。
- ⚠️ **隐含限制**：若某次读到的不完整样本长度 > 64 字节，多出的字节被丢弃 → 后续样本会**错位**（`DAT_14085D184` 是整除基准）。变量多、样本 >64B 时有真实风险 [推导]。
- `UnpackSamples = FUN_14003FA70` 的调用者正好是 3 个：`FUN_140042260`（RTT 在线）、`FUN_140040FA0`（HSS 在线）、`FUN_14003DE60`（离线重解包）→ 确认两条在线路径共用 [观测]。
- `FUN_140040FA0`（HSS）批量大小：`0x200000 - (0x200000 % (DAT_14085D184 * DAT_14085D1E8))` ≈ 2 MB 对齐到“整周期” [观测，chunk_005.c:3008-3009]。

### 5.3 无时间戳时合成 100 µs [观测，`FUN_14003FF40`]

```c
if (DAT_14285D9C4 == 0)  { ts[idx] = *param_1; return; }      // HSS：样本自带 4B ts，原样存
if (DAT_14285D9C5 != 0)  { ts[idx] = *param_1; return; }      // RTT 带 ts：同上
if (DAT_14285DA80 != 0)  { ts[idx] = ts[DAT_14285DA7C] + 100; return; }   // 逐样本 +100 µs
if (DAT_14285DA78 < DAT_14285DA7C) { ts[0] = ts[DAT_14285DA7C] + 100; return; } // 环回
ts[0] = 0;                                                     // 首样本
```

相关全局 [观测]：
- `DAT_14085D1F0`：时间戳环（`uint32`/样本，长度 0x800000 = 8M 样本，清零 0x2000000 字节）；
- `DAT_14285DA80`：当前样本序号；`DAT_14285DA7C`：上一序号；`DAT_14285DA78`：最旧有效序号；`DAT_14285DA84`：当前样本时间戳；
- `DAT_14085D1EC`：`本次解包字节数 / 样本字节数`（`FUN_14003FA70` 开头计算）[观测]。

均值/最值更新：`FUN_140042AC0`（由 `FUN_14003FA70` 在 iVar4==0 时调用）按 `+0x314` 类型分支读 1/2/4/8 字节 [观测]。

---

## 6. `-RTTSearchRanges` → ExecCommand 的完整链路

1. **命令行/表驱动解析** [观测]：`FUN_140041CD0(argc, argv, modeMask)` 是选项分发器，按 4 指针/条的表 `PTR_…140074040`（stride 0x20：`{name, mask, handler, help}`）扫描；实测表内容：

```
0x140074040  -openprj          mask=3  handler=FUN_14003DC80  help="Opens an existing project…"
0x140074060  -USB              mask=1  handler=FUN_14003DCB0
0x140074080  -IP               mask=1  handler=FUN_14003DD90
0x1400740A0  -RTTSearchRanges  mask=1  handler=FUN_14003DE20  help="Defines RTT search ranges …"
```

   `FUN_14003CC50` 调用它两次：先 `modeMask=2`（初始化前，只认 `-openprj`），再 `modeMask=1`（初始化后，`-RTTSearchRanges` 在此生效）[观测]。
2. **选项名匹配**：`FUN_1400277F0(argv[i], "‑RTTSearchRanges", strlen)` = **大小写不敏感前缀比较**（逐字符过 `tolower` 表），命中后参数可以是“下一个 argv”或“紧贴的同一 token 尾部”，再调 handler [观测]。
3. **handler `FUN_14003DE20`** [观测]（decomp_rtt.txt:610）：

```c
FUN_1400276B0(param_1, 0x2C, 0x20);                       // ',' → ' '（原地改）
FUN_140028380(&DAT_14285D210, 0x200, "SetRTTSearchRanges %s", param_1);
return 0;
```
   即 `-RTTSearchRanges 20000000,1000,20001000,200` → 存成
   `"SetRTTSearchRanges 20000000 1000 20001000 200"`（0x200 字节全局缓冲 `DAT_14285D210`）。
4. **下发时机** [观测，`FUN_1400419F0`]：

```
JLINKARM_ConfigJTAG(...)  →  JLINKARM_ExecCommand("SetSkipInitECCRAMOnConnect = 1")
                          →  "device = %s" / "ScriptFile = %s"
                          →  JLINKARM_TIF_Select → JLINKARM_SetSpeed
                          →  if (DAT_14285D210[0] != 0) JLINK_ExecCommand(&DAT_14285D210, 0, 0)
                                失败 → "Wrong syntax for \"-RTTSearchRange\".\n"
                          →  JLINKARM_Connect()          ← 搜索范围必须在 Connect 之前设好
```
   （机器码核对：0x140041BEA=`JLINKARM_TIF_Select`(0x140028E80)、0x140041BF5=`JLINKARM_SetSpeed`(0x140028E40)、0x140041C0F=`JLINK_ExecCommand`(0x140028F30)、0x140041C2B=`JLINKARM_Connect`(0x140028A20) [观测]。注意用的是 **`JLINK_ExecCommand`** 而非 `JLINKARM_ExecCommand`。）

---

## 7. 与公开文档（UM08028）的差异

| 项 | UM08028 原文 | 9.66 实测 | 判定 |
|---|---|---|---|
| 配置串条目 | `<4 Byte Var Size><4 Byte Addr>` 每变量两个 4B 字段（= 8B/条） | **16B/条**：`Addr@0(4B)+0@4`、`Size@8(4B)`、`@12` 未初始化 | **不一致**，以实测字节为准 [观测] |
| 结束标记 | “8 Bytes of 0” | `16*nVars` 起 **12 字节 0**（后 4B 未写） | ≥8B 为 0，兼容；但步长是 16B [观测] |
| 应答通道 | “目标端需回 256 字节 0” | 回**上行数据通道**（`JScope_Data*`），J-Scope 精确累计 256B，`byte[0]` 判成败 | 补充细节 [观测] |
| 变量上限 12 | “limit of twelve variables in total (including timestamp)” | 通道名 32B：`"JScope_"`+2N+1 ≤ 32 → N ≤ 12 | **一致** [观测+推导] |
| 类型尺寸 | `i`/`u`: 1,2,4；`f`: 4 | 还接受 `u8`/`i8`（类型码 9/13）与 `f8`（类型码 2，double） | 9.66 为超集 [观测] |
| `<4B Var Size>` 语义 | 变量字节宽度 | 与实测一致（`+0x31C` 来自 ELF/格式串尺寸位） | 一致 [观测] |
| 复位目标后发配置 | “resets the target and sends a configuration string” | `JLINKARM_Reset()`+`JLINKARM_Go()` 后才发 | **一致** [观测] |

> 条目布局的差异有两种可能：(a) 文档描述的是目标端参考库的结构体（可能被编译器补齐/字段顺序不同）；(b) 文档对某个历史/未来版本。9.66 实际写到线上的字节是唯一“兼容性真值”，建议实现以 §1.6 的字节表为准，并对文档口径做兼容回退（见 §9）。

---

## 8. 未知点 / 无法从本 exe 判定

1. **`JLINK_RTTERMINAL_Desc` 的字段语义**：`+0` = 通道号、`+8..+0x27` = 32 字节名字、`+0x28` = 缓冲大小（仅 `FUN_14003EF60` 读一次）为 [推导]；`+4` 在 Data 搜索前置 0、Config 搜索前置 1（像是“上行/下行”方向选择），但 `Control(3, desc+4)` 又把返回的“缓冲个数”写进同一地址 —— 该字段究竟是输入方向还是输出数量，**需看 JLinkARM.dll 内部实现**（本 exe 之外）[未知]。
2. **`Control(3)`/`Control(2)` 的确切语义**：本 exe 只体现“`3` 返回通道个数、`2` 按 `desc->Chan` 填充描述并返回 0/负” [观测]，具体枚举是否包含下行缓冲、是否按方向过滤，[未知]。
3. **文档 vs 实测的配置条目布局**（§7 第一行）必须用真实目标端库（SEGGER 的 `JS_RTT_WaitForConfig` 参考实现）或抓一次真实连接才能定案；本机没有该目标端源码/样本 [未知]。
4. **配置条目 `+12..+15` 的实际内容**：取决于堆历史，非确定值；J-Scope 侧不读回，无副作用 [推导]。
5. **`FUN_14003F580` 的按钮极性**（“Retry?” 对话框：返回非 0 → 放弃）[推断]，不影响协议。
6. 变量上限 12 是**名字缓冲**推出的硬上限；RTT-Plus 侧没有 12 的限制（受下行缓冲大小约束，见 §9-4）[推导]。

### 需要补充反编译（若要把 §8 落实）
- `JLinkARM.dll` 的 `JLINK_RTTERMINAL_Control` / `JLINK_RTTERMINAL_Write` / `JLINK_RTTERMINAL_Read` 实现（导出函数，非本 exe 代码）。
- SEGGER 目标端参考库（`JS_RTT_WaitForConfig` / `JS_RTT_SendData`，通常在 SEGGER 的 RTT-Plus 例程里），用于交叉确认条目布局。

---

## 9. 对 OpenScope 的启示（写一个“兼容 J-Scope”的目标端 RTT-Plus 库）

1. **必须建两个 RTT 缓冲，名字精确**：上行 `"JScope_Data"`（无时间戳）或 `"JScope_Data_t"`（每样本前导 4B µs 时间戳），下行 `"JScope_Config"`。查找是**区分大小写的前缀匹配**，所以大小写必须完全一致；名字字段 32B 上限。上行缓冲建议 **≥256B**（应答要占满 256B，之后才开始流样本）。
2. **下行配置串按 16B/条解析**：`Addr = *(u32*)(p+0)`、`Size = *(u32*)(p+8)`、`p += 16`；`Addr==0 && Size==0` 视为结束（J-Scope 写 12 个 0）。为兼顾文档口径，可加兼容探测：若首条 `*(u32*)(p+4) != 0`（即“第 2 个 4B 非 0”）则按 8B/条 `{u32 Size, u32 Addr}` 解析；否则按 16B/条。**不要**依赖条目 `+12..+15`（堆垃圾）。
3. **收到配置后 100 ms 内回 256 字节**：`byte[0]=0` 表示接受（其余 255B 建议也写 0），失败时写非 0 开头 + ASCII 错误串（J-Scope 会显示 `Target Error: <串>`）。超时（>~100 ms）会触发 **`JLINKARM_Reset()` + 重发配置** 的整轮重试，所以 ACK 要快、且库要能容忍“被复位后重新收配置”。
4. **配置会重复下发**：只要变量表变化，J-Scope 就会复位目标 → `Go` → 重发配置。库必须在每次配置到达时**重建采样表**，并且不能假设“配置只来一次”。同时，下行缓冲至少要能一次容纳 `(nVars+1)*16` 字节（否则 J-Link 侧写会重试，最多 100 次 ×1 ms 后判失败）；对 N 个变量的实际约束 ≈ `downBufSize ≥ 16N+16`。
5. **采样必须整样本、按配置顺序、宽度=配置 Size**：样本 = `[可选 4B ts] + var1 + var2 + …`，无对齐填充；写入 RTT 上行缓冲要**按样本为单位整写**（J-Scope 用 ≤64B 的 carry 拼半样本，超过 64B 的半样本会被丢弃并导致错位）。用 `NO_BLOCK_SKIP` 或 `BLOCK_IF_FIFO_FULL` 模式，避免丢样本造成时间戳错位。
6. **时间戳建议必发**（用 `JScope_Data_t`）：无时间戳时 J-Scope 按“每样本 +100 µs 均匀栅格”**合成**时间戳（`DAT_14285DA80!=0 → ts[i]=ts[i-1]+100`），只适合等间隔采样；有真实时间戳才能反映抖动——这正是 OpenScope 采集场景需要的。另外 RTT-Normal（VarFormat）模式可作为**零 ELF 依赖**的备选：通道名 `JScope_<类型字符+尺寸数字>…`（如 `JScope_t4u1i4`），上限 12 个变量（含 `t`），类型字符大小写不敏感、非法字符被静默跳过，J-Scope 生成的变量地址为 0、只按偏移取值 —— 目标端无需任何配置通道与应答。
7. **实现顺序建议**：先做 RTT-Normal（协议最简：只要通道名正确 + 整样本写入），再做 RTT-Plus（配置解析 + 256B 应答 + 复位容忍），最后与主机侧 OpenScope 的 HSS（`JLINK_HSS_Start` 描述符 `{Addr@0,Size@4}`、stride 16、全 0 终止项、样本自带 4B 时间戳）对齐 —— 三者最终都汇聚到同一“样本 = 4B ts + 紧密排列变量”的解包语义上。
