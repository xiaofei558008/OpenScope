# J-Scope 9.66 专题逆向：启动 / 会话 / 连接 / 配置 / 命令行 / 设置持久化

**分析对象**：`C:\Program Files\SEGGER\JLink_V966\JScope.exe`（PE32+ x64，镜像基址 `0x140000000`，Qt4 GUI）
**方法**：纯静态（Ghidra 已有全量反编译产物 + 直读 PE 字节解析 .rdata/.data 常量表），**未运行程序、未连接探针**
**证据分级**：[观测] 直接来自反编译/PE 字节；[推导] 由观测确定性推理；[推断] 合理未证实；[未知] 未解决
**证据位置**：`dumps/alldecomp/chunk_XXX.c:<行号>`（函数头给出 VA/size/调用关系）；常量表地址已用 PE 字节直接读出
**分工边界**：触发（TriggerWidget 内部逻辑）、绘图、导出/导入 raw、ELF 解析、RTT 协议细节由其他专题负责，本文只写"外围"

> 与 `JSCOPE_ANALYSIS.md` 的差异：该报告 §4/§5/§6/§7/§10.1/§11 与本专题有少量重叠，本文对**重叠处只做校正与补全，不重复展开**；新增内容为：完整启动序列、选项表精确掩码语义、DLL 三级查找精确实现、JScopeSettings.ini 完整段落/键与读改写算法、菜单/工具栏/快捷键全表与槽函数地址、状态栏字段与刷新链、生命周期清理、单实例结论。

---

## 1. 启动序列

### 1.1 入口链（全部 [观测]）

| 阶段 | 函数 | 位置 | 说明 |
|---|---|---|---|
| PE EntryPoint | `entry` @ `0x1400595c8` | `chunk_007.c:3789` | `__security_init_cookie()` → `FUN_140059454()` |
| CRT 初始化 | `FUN_140059454` @ `0x140059454` | `chunk_007.c:3743` | `__scrt_initialize_crt(1)` → `_initterm_e`/`_initterm`（跑 .CRT 静态初始化器）→ `FUN_14004c670` |
| 包装 | `FUN_14004c670` @ `0x14004c670` | `chunk_006.c:3491` | `__p___argv()` / `__p___argc()` → `FUN_14004c520(argc, argv)` |
| **main** | `FUN_14004c520` @ `0x14004c520` | `chunk_006.c:3442` | 见 1.2 |
| 主窗口 | `FUN_14004c6a0` @ `0x14004c6a0` | `chunk_006.c:3510` | `MainWindow` 构造（1864 B，41 callee） |
| 异步 Init Job | `FUN_14003e2e0` @ `0x14003e2e0` | `chunk_005.c:376` | 投递 `"APP: <Init> Job"`，job body = `FUN_14003cc50`，完成回调 = `LAB_14004db30` |
| Init Job 体 | `FUN_14003cc50` @ `0x14003cc50` | `chunk_004.c:6900` | 工作线程执行：设置 → 命令行 → DLL 版本 → 启动对话框决策 |

### 1.2 `FUN_14004c520` = main [观测]（`chunk_006.c:3442`）

```c
setvbuf(stdout, NULL, 4 /*_IONBF*/, 0);            // 日志走 stdout，无缓冲
this = operator new(0x10);
QApplication::QApplication(this, &argc, argv, 0x1040807);  // Qt4；this 与 vftable 手工赋值
FUN_140036d80();                                    // 早期全局初始化
DAT_14286d3b8 = 0; DAT_14286d3b0 = 0; ... DAT_14286d388 = 0;   // 全局符号表/主窗口指针清零
lVar1 = operator new(0x348);
if (lVar1) pQVar2 = FUN_14004c6a0(lVar1, argc, argv);   // MainWindow
QApplication::exec();
// 退出后 delete MainWindow、delete QApplication（vtable+0x18）
```

要点：
- `QApplication` 构造第 4 参 `0x1040807` = Qt4 `ApplicationFlags`（含 `Qt::WindowSystemMenuHint` 等），[观测]字面量。
- **Qt 主事件循环 `QApplication::exec()` 在 main 里，所有对话框（Welcome/NewProject/About/设备选择）都用 `QDialog::exec()` 模态阻塞，而不是信号驱动的异步窗口** [观测]。

### 1.3 `MainWindow` 构造顺序 [观测]（`chunk_006.c:3510`）

```
QMainWindow::QMainWindow(this, 0, 0)
vftable = MainWindow::vftable (+0x10 二次基类)
DAT_14286d388 = this                       // 全局唯一 MainWindow 指针（62 处引用）
setWindowTitle(FUN_14003ea80())            // → &DAT_14285dab0 = "SEGGER J-Scope V9.66"
setMinimumSize(800, 800); setGeometry(...)
FUN_14002a340()                            // 载入 0x48 = 72 个 PNG → QIcon（见 §8.4）
MenuBar   = FUN_140029ad0()          → this+0x2e0   // §8.1
ToolBar   = FUN_140054790()          → this+0x2e8   // §8.2
TriggerWidget = FUN_1400561b0()      → this+0x2f0   // 触发专题
QSplitter                    → this+0x2d8
GraphContainer = FUN_1400479a0()     → this+0x2f8   // 绘图专题
TableWidget    = FUN_140051790()     → this+0x300   // 数值窗口专题
StatusBar = FUN_14004fbe0()          → this+0x320   // §7
QTimer(1000 ms) → this+0x328；connect "2timeout()" → "1_SlotOnTimeout()" ；start()
show()
// 17 个 connect（信号→槽），全部 AutoConnection
*(this+0x338) = argc;  *(this+0x330) = argv;        // 命令行上下文直接挂在窗口对象上
FUN_14003e2e0(this, this+0x330, &LAB_14004db30);    // 投递 APP: <Init> Job
```

`<Init>` Job 的参数就是 `MainWindow+0x330`，即 `{+0x00 argv, +0x08 argc, +0x10 startAction(2B), +0x12 flags}` [推导]。`FUN_14003cc50` 直接读写这几个字节（`*(undefined1*)(plVar2+0x12)`、`*(undefined2*)(plVar2+2)`）。

### 1.4 `FUN_14003cc50`（Init Job 体）[观测]（`chunk_004.c:6900`）

```c
DAT_14285da18 = argc;  *(plVar2+0x12) = 0;                 // 清 "项目已由命令行打开" 标志
iVar4 = FUN_14004f940();                                   // ① 载入 JScopeSettings.ini（§5）
FUN_14003f930();                                           // 刷新窗口标题
build "Software\\SEGGER\\%s\\"  (app="J-Scope")             // ②
FUN_14004e4b0(1, 该路径, 10);                               // 读 10 条 MRU（§5.2）
FUN_14004e150(DAT_14286d388);                              // 应用设置到主窗口
iVar5 = FUN_140041cd0(argc, argv, 2);                      // ③ 命令行 阶段2（只认 -openprj）
if (0 < iVar5) { *(plVar2+0x12)=1; goto 结束; }
FUN_14003f830(); FUN_140041780(); FUN_14003e990();         // ④ 日志/DLL版本串/作用域
if (argc > 1)                                             // ⑤ 逐条回显参数
    for (i=1;i<argc;i++) FUN_14003f1e0("[%d]: %s", i, argv[i]);
if (argc < 2) { ... FUN_140040e20() ... }                  // ⑥ 启动对话框决策（§1.5）
if (iVar4 < 0) FUN_14003f120("Failed to initialize %s", app);
else {
    iVar4 = FUN_140041cd0(argc, argv, 1);                  // ⑦ 命令行 阶段1（全部选项）
    if (0 < iVar4) *(plVar2+0x12)=1;
    if (DAT_14285da1d != 0) FUN_14004dca0(DAT_14286d388);  // 立即退出
}
(*pcVar3)(uVar1);                                          // ⑧ 完成回调 LAB_14004db30
```

字符串 `"Application log started"`、`"Optional command line parameters found"` 的引用者也是本函数 [观测，`dumps/globals.txt:2558-2559`]，但被内联进 `FUN_14003e990`/`FUN_14003f830` 一侧，反编译文本中不可见。

### 1.5 WelcomeDialog / NewProjectDialog 出现条件 [观测 + 推断]

决策函数 `FUN_140040e20` @ `0x140040e20`（`chunk_005.c:2909`）：

```c
local_res8[0] = 0; local_res10[0] = 4;                 // REG_DWORD，默认 0
hKey = RegCreateKeyA(HKEY_CURRENT_USER /*0x80000001*/, "Software\\SEGGER\\J-Scope\\");
RegQueryValueExA(hKey, "DontShowAgainStartJScope", ..., &value, &size);
return value;                                          // 0 / 1 / 2
```

调用侧（`FUN_14003cc50`）与消费者（`FUN_14004d380` @ `0x14004d380`，= 槽 `_SlotOnFinAsyncAppInit`）：

| 条件 | 行为 |
|---|---|
| `argc ≥ 2` 且扫描到 `-openprj`/`.jscope` 路径 [推断] | `MainWindow+0x342=1` → `FUN_14004d380` 立即 `QWidget::close()`（启动即退出，属"命令行开工程"语义） |
| `argc < 2` 且注册表值 = **0 / 不存在** | `MainWindow+0x341=1` → **WelcomeDialog**（`FUN_140057220`，`chunk_007.c:2599`），并把 `MainWindow+0x230`（10 条 MRU）传入 |
| `argc < 2` 且注册表值 = **2** | `MainWindow+0x340=1` → 直接弹 **NewProjectDialog**（`FUN_14002a410`，标题 `"Create new project"`） |
| `argc < 2` 且注册表值 = **1** | 不弹任何对话框，直接空主窗口（但仍会尝试用 `FUN_14004f7f0` 读出的上次工程路径） |

`FUN_14004d380` 主体 [观测]：

```c
if (param_1[0x342]) { QWidget::close(param_1); return; }        // 命令行已开工程 → 退
if (param_1[0x341])      this = FUN_140057220(.., param_1+0x230, 10);   // WelcomeDialog（10 条 MRU）
else if (param_1[0x340]) this = FUN_14002a410(.., param_1, 0);          // New Project
QDialog::exec(this);
*(int*)(param_1 + 0x33c) = 返回值;   // WelcomeDialog 结果
FUN_14004d160(param_1);              // 刷新菜单使能（FUN_140029da0）
```

写入方 `FUN_140042750` @ `0x140042750`（`chunk_005.c:4124`）—— WelcomeDialog 的"Do not show this message again."勾选框：

```c
RegCreateKeyA(HKCU, "Software\\SEGGER\\J-Scope\\");
RegSetValueExA(hKey, "DontShowAgainStartJScope", 0, 4 /*REG_DWORD*/, &value, 4);
```

WelcomeDialog（`FUN_140057220`）控件与文案 [观测]：
- 标题 `"Welcome to %s"`（%s = `"J-Scope"`）
- `"Please select one of the following start options:"`
- `"Open recent project:"` 下拉（`CustComboBox`），逐条填 `MRU 路径`；`"Other..."`、`"Create new project"`、`"Do not show this message again."`、默认按钮 `"Start J-Scope"`
- `setModal(true)`、`setFixedSize(sizeHint())`、去掉 `WindowContextHelpButtonHint`

---

## 2. 命令行接口

### 2.1 选项表（PE 常量，直读 .data）[观测]

选项表位于 `0x140074040` 起，**每条 4 个 qword（32 B）**，共 4 条；由 `FUN_140041cd0` 以 `ppuVar3 += 4` 遍历：

| # | 结构体 VA | 选项串 | 掩码 | 处理函数 | 帮助串 |
|---|---|---|---|---|---|
| 0 | `0x140074040` | `-openprj` (`0x140074260`) | **`0x3`** | `FUN_14003dc80` (`0x140074050`) | `0x140074270` " Opens an existing project\n Syntax:  -openprj <FILENAME>" |
| 1 | `0x140074060` | `-USB` (`0x1400742b0`) | **`0x1`** | `FUN_14003dcb0` (`0x140074070`) | `0x1400742b8` |
| 2 | `0x140074080` | `-IP` (`0x1400742f8`) | **`0x1`** | `FUN_14003dd90` (`0x140074090`) | `0x140074300` |
| 3 | `0x1400740a0` | `-RTTSearchRanges` (`0x140074348`) | **`0x1`** | `FUN_14003de20` (`0x1400740b0`) | `0x140074360` |

> 4 条掩码与处理函数指针均**已用 PE 字节逐个读出**（`0x140074048=3`、`0x140074068=1`、`0x140074088=1`、`0x1400740a8=1`）[观测]。
> `-openprj` 掩码 `0x3` 使它在两个阶段都被接受；其余 3 条只在阶段 1 生效。

### 2.2 两阶段解析 `FUN_140041cd0(argc, argv, phase)` [观测]（`chunk_005.c:3595`）

```c
DAT_14285da1f = 1;
JLINKARM_ExecCommand("SuppressGUI = 1", 0, 0);          // 抑制 DLL 自己弹 GUI
for (uVar4 = 1; uVar4 < argc; ) {
    pcVar6 = argv[uVar4];
    for (each of 4 table entries) {
        if (pcVar6[0] != '-' && pcVar6[0] != '/') {     // 不是选项
            if (strstr(path, ".jscope") == NULL) break;
            iVar9 = FUN_14003dc80(pcVar6, 0, phase);    // 裸 .jscope 路径也当 -openprj
            ...
        }
        if ((entry.mask & phase) == 0) { uVar5++; iVar9 = 0; }   // 阶段不匹配 → 静默跳过
        else {
            iVar1 = strlen(entry.name);
            if (strncmp(pcVar6, entry.name, iVar1) == 0) {
                pcVar7 = argv[uVar5];                   // 下一个 argv 若不是选项则作为值
                if (!pcVar7 || pcVar7[0]=='-'||pcVar7[0]=='/') pcVar7 = pcVar6 + iVar1;
                else uVar5++;
                iVar1 = entry.handler(pcVar7, iVar1, phase);   // (值串, 选项名长度, 阶段)
                if (iVar1 == 0) goto next_arg;
                if (iVar1 < 0 || DAT_14285da1c) { DAT_14285da1f=0; goto fail; }
                if (iVar1 == 1) return 1;
                break;
            }
        }
        ...
    }
    if (iVar9 == 1) {   // 表内无匹配
        sprintf(buf, "CMDLINE: Unknown command line option \"%s\" detected!", pcVar6);
        FUN_14003f1a0(buf); FUN_14003f640(buf); DAT_14285da1c = 1; ret = -1;
    }
fail:
    sprintf(buf, "CMDLINE: Failed to execute command line option \"%s\"!", pcVar6);
    FUN_14003f1a0(buf); FUN_14003f640(buf); DAT_14285da1c = 1; return -1;
next_arg:
    uVar4 = uVar5;
}
JLINKARM_ExecCommand("SuppressGUI = 0", 0, 0);
DAT_14285da1f = 0;
return ret;
```

要点：
- **阶段 2** 只匹配 `-openprj`（掩码 3）：`FUN_14003dc80` 在阶段 2 只做 `DAT_14285da1e = 1`（"曾出现 -openprj"）并返回 0，用于在真正打开之前先探明；
- **阶段 1** 才真正执行 `FUN_14004ea70()`（打开工程）。
- **未知选项**：报 `"CMDLINE: Unknown command line option \"%s\" detected!"`（`0x140074498`），Error 对话框 + Warning 对话框各一次，并置 `DAT_14285da1c`。
- 选项名匹配是 **前缀比较**（`strncmp(arg, name, strlen(name))`），因此 `-openprjX` 也会命中 `-openprj` [推导]。

### 2.3 各选项处理函数 [观测]

**`-openprj <FILENAME>` → `FUN_14003dc80`**（`chunk_005.c:2`）

```c
if (phase != 1) { if (phase == 2) DAT_14285da1e = 1; return 0; }
return FUN_14004ea70();      // 真正打开
```
`FUN_14004ea70`（`chunk_006.c:5344`）：若给了路径 → `FUN_14004eda0(path, 0)`；否则弹出 `"Open project"` 文件对话框（filter `"All files (*.*);;J-Scope project files (*.jscope)"`，默认 `"J-Scope project files (*.jscope)"`，初始目录取 ini `[PATH] ProjectFiles`），成功后把路径写回 ini。

**`-USB <SN/nickname>` → `FUN_14003dcb0`**（`chunk_005.c:21`）

```c
if (DAT_14285da1e == 0) {                 // 即阶段2 没见到 -openprj
    FUN_14003f430("-USB should only be used in combination with -openprj.");   // 0x1400743e0
    return -1;
}
FUN_14004e9c0(&cfg);                      // 取当前（已从工程加载的）连接配置
if (is_decimal(sn)) cfg.SerialNo = dec(sn);
else { strcpy(cfg.Nickname, sn, 0x80); cfg.SerialNo = 0; }
cfg.ConnectionKind = 1;                   // USB
FUN_14004ec40(&cfg);                      // 回写全局配置
```
非法 SN → `"Error while parsing USB S/N"`（`0x140074418`）。

**`-IP <hostname/SN/nickname>` → `FUN_14003dd90`**（`chunk_005.c:67`）

```c
if (DAT_14285da1e == 0) { FUN_14003f430("-IP should only be used in combination with -openprj."); return -1; }
FUN_14004e9c0(&cfg); strcpy(cfg.IP, value, 0x80); cfg.ConnectionKind = 2; FUN_14004ec40(&cfg);
```
两个"仅可与 -openprj 同用"的强制校验**分别在两个各自的处理函数里**，判据是全局 `DAT_14285da1e`（由 `-openprj` 的阶段 2 探针置位）；不存在一个统一的校验点。

**`-RTTSearchRanges <A>,<S>[,<A1>,<S1>,...]` → `FUN_14003de20`**（`chunk_005.c:98`）

```c
FUN_1400276b0(value, 0x2c /*','*/, 0x20 /*' '*/);        // 逗号换成空格（就地改写 argv）
sprintf(&DAT_14285d210, "SetRTTSearchRanges %s", value); // 存到全局 512 B 缓冲
return 0;
```
该全局在连接序列 `FUN_1400419f0` 里被 `JLINK_ExecCommand` 下发；失败报 `"Wrong syntax for \"-RTTSearchRange\".\n"`（注意原文少了个 s）[观测，`chunk_005.c:3564`]。

### 2.4 无独立 `-h/--help` 选项

表中的帮助串（`0x140074270/2b8/300/360`）是**给未知选项报错路径准备的 printable 列表**（[推断]：格式化错误消息时可拼接），未发现任何 `printf`/`usage()` 调用点引用它们 [观测，`globals.txt` 中这 4 条的 refs 指向选项表自身的第 4 个 qword]。

---

## 3. JLink DLL 加载

### 3.1 惰性加载入口

所有 29 个 `JLINK*` API 都是**同名 thunk**，形态统一（`chunk_003.c:70` 起，共 29 个）：

```c
void JLINKARM_Close(void) {
    if (DAT_14085b3f8 == NULL)
        DAT_14085b3f8 = (code *)FUN_1400293b0("JLINKARM_Close");   // 按名解析并缓存
    if (DAT_14085b3f8 == NULL) return;
    DAT_14085b3f8(...);
}
```

`FUN_1400293b0` @ `0x1400293b0`（`chunk_003.c:69`）：

```c
if (FUN_140029490(1) < 0) return NULL;      // 确保 DLL 已加载
pFVar4 = GetProcAddress(DAT_14085b3a8, param_1);
if (pFVar4 == NULL) {
    sprintf(buf, "DLL function %s(...) not found !", param_1);
    MessageBoxA(NULL, buf, "J-Link Error", 0x42030);          // MB_ICONERROR|MB_SETFOREGROUND...
}
return pFVar4;
```

### 3.2 精确查找顺序 `FUN_140029490` @ `0x140029490`（`chunk_003.c:105`）[观测]

```
若 DAT_14085b3a8 != NULL → 返回 0（已加载）

① VirtualQuery(&LAB_140028ee0) → .AllocationBase → GetModuleFileNameA  // 本 exe 自身路径
   把文件名替换为 "JLink_x64.dll"      （FUN_140029930 = _splitpath_s, FUN_1400297e0 = 拼路径）
   LoadLibraryA(...)                    → 成功返回 1

② GetModuleFileNameA(DAT_14085b3a0)    // 另一个模块基址（[推断]：宿主/进程主模块）
   同上换 "JLink_x64.dll" 再 LoadLibraryA  → 成功返回 2

③ FUN_1400296f0(0xffffffff80000001 /*HKEY_CURRENT_USER*/)
        FUN_1400292f0 读 HKxx\Software\SEGGER\J-Link\InstallPath
                     （失败则 HKxx\Software\SEGGER\Flasher\InstallPath）
        失败则再试 "<InstallPath>\bin_x64\JLink_x64.dll"
   → 成功返回 3
④ FUN_1400296f0(0xffffffff80000002 /*HKEY_LOCAL_MACHINE*/) → 成功返回 4
⑤ 若 param_1 != 0:
        sprintf(buf, "Target DLL %s not found !", <最后一次尝试的完整路径>);
        MessageBoxA(NULL, buf, "J-Link Error", 0x42030);
        exit(1);                        // 直接退出进程
   否则返回 0xff
```

关键事实：
- **没有**"裸 `JLink_x64.dll` 交给 LoadLibrary 搜索路径"这一步 —— 第一次尝试就是 **exe 同目录**的绝对路径 [观测，与 `JSCOPE_ANALYSIS.md` §2 的表述不同，此处以代码为准]。
- `bin_x64\` 子目录只在**注册表 InstallPath 分支**里出现（`0x140064358` = `bin_x64\JLink_x64.dll`）。
- 注册表键：`Software\SEGGER\J-Link` 优先，失败回退 `Software\SEGGER\Flasher`；值名 `InstallPath`（`0x140064338`）；HKCU 先于 HKLM。
- 失败提示串：`"Target DLL "` + 路径 + `" not found !"`，标题 `"J-Link Error"`；缺函数串 `"DLL function "` + 名 + `"(...) not found !"` [观测，`0x140064370/140064380/140064390/1400643a0/1400643b0`]。

### 3.3 DLL 版本串与门槛

`FUN_140041780` @ `0x140041780`（`chunk_005.c:3341`，只被 Init Job 调用一次）[观测]：

```c
build "<app> - [" ...                        // 日志前缀
uVar1 = JLINKARM_GetDLLVersion();            // 返回按位打包的 version
FUN_1400271f0(local_a8, uVar1);              // 解包
local_b8 = JLINKARM_GetCompileDateTime();    // → %s
FUN_140028380(local_98, 0x80, "JLinkARM.dll V%d.%.2d%s (DLL compiled %s)",
              local_a4 /*major*/, ... /*minor*/, ... /*suffix 如 "a"*/, local_b8);
输出到日志（经 MainWindow）;
```

即 DLL 版本**不是显示在某个 UI 控件里，而是作为一条日志文本输出**（走 `FUN_14003e9a0`/主窗口日志通道）[观测]。

版本门槛（`FUN_1400419f0`，已由 `JSCOPE_ANALYSIS.md` §4 记录，此处补精确常量）[观测]：
```c
if (JLINKARM_EMU_HasCapEx(0x35)) {          // 0x35 = HSS 能力位
    if (0xc79b < JLINKARM_GetDLLVersion()) { // 32283 —— 版本门限
        JLINK_HSS_GetCaps(&caps);
        _DAT_14285da90 = 1000000 / caps.field_at_4;
    } else { period = 10; FUN_14003f400(10); }
} else { period = 20000; FUN_14003f400(20000); }
```
`0xc79b = 32283` = `V4.96a` 一类的打包版本号 [推断]；实际含义需实测确认（沿用 `JSCOPE_ANALYSIS.md` §4 的未知点）。

---

## 4. 连接 / 配置对话框

### 4.1 类清单与构造地址 [观测]

| 类 | 构造函数 | 位置 | 职责 |
|---|---|---|---|
| `NewProjectDialog` | `FUN_14002a410` (4584 B) | `chunk_003.c:898` | **连接 + 采样 + ELF 的唯一配置对话框**；既当"新建工程"（`param_3=0`，标题 `"Create new project"`）也当"工程设置"（`param_3=1`，标题 `"Project settings"`） |
| `WelcomeDialog` | `FUN_140057220` | `chunk_007.c:2599` | 启动向导：最近工程下拉 + 其他 + 新建 + 不再提示 |
| `AboutDialog` | `FUN_14002ce80` | `chunk_003.c:2598` | 见 §9.1 |
| `CDeviceSelectionDialog` | `FUN_14002d5c0` | `chunk_003.c:2846` | **J-Scope 自带的器件选择表**（不是 DLL 的） |
| `CTableView` / `CDeviceDataProxyModel` | `FUN_14002d6c0` 等 | `chunk_003.c:2900` 起 | 上者的表格视图 + 过滤代理 |
| `TIFSpeedComboBox` | `FUN_1400373a0` | `chunk_004.c:1638` | 目标接口速率下拉（可编辑 + 校验） |
| `CustComboBox` | `FUN_140032f90` | `chunk_003.c:7414` | 带自定义绘制的下拉 |
| `CustLineEdit` | `FUN_140037210` | `chunk_004.c:1540` | 带高亮/主题适配的输入框 |

### 4.2 `TIFSpeedComboBox` [观测]（`chunk_004.c:1638` / 填充逻辑 `chunk_004.c:1892`）

```c
QComboBox::QComboBox(this, parent);
vftable = TIFSpeedComboBox::vftable;
setEditable(true);
this+0x28 = &DAT_140070ed0;   // 速率值数组（按接口切换）
this+0x38 = 0x21;             // 条目数 33（默认 JTAG 表）
this+0x40 = new QRegExpValidator(this);
connect(lineEdit, "2editingFinished(void)",        this, "1_SlotOnCustSpeedEntered(void)");
connect(this,     "2currentIndexChanged(int)",     this, "2SignalSpeedChanged(void)");
if (parent && slotName) connect(this, "2SignalSpeedChanged(void)", parent, slotName);
FUN_1400377e0(this);          // 填充
```

`FUN_1400377e0`（填充）：
- 清空后在 `+0x3c` 非 0 时插入一条 `"%d"`（=自定义值，取数组最大值）；
- 逐个插入 `FUN_140037700(表[i])` 格式化的条目；
- 算出最大值 `uVar9` → 建正则 **`"[0-9]{0,%d}"`**（`0x140071138`）→ `setValidator`。
  即：**速率输入框只允许 0~最大值的十进制数字**（与新建工程对话框的采样率 `"^[0-9]*$"` 不同，后者不限制位数）。

### 4.3 `NewProjectDialog`（`FUN_14002a410`）完整控件与语义 [观测]

**连接组 `"Connection to J-Link"`**

| 字段偏移 | 控件 | 串 |
|---|---|---|
| +0x68 | RadioButton | `"USB"` (`0x14006c788`)，默认选中 |
| +0x90 | RadioButton | `"TCP/IP"` |
| +0xa0 | RadioButton | `"Existing Session"`（构造后 `hide()`） |
| +0x70 | CheckBox | `"SN / Nickname"` → `_SlotOnToggleCheckUSB(int)` |
| +0x78 | LineEdit | SN（USB 时） |
| +0x98 | LineEdit | TCP/IP 目标（hostname/SN/nickname） |
| +0xa8/+0xb0 | LineEdit + 按钮 | `"Specify Target Device"` → `_SlotOnReleaseBtnTarget()` |
| +0xb8/+0xc0 | LineEdit + 按钮 | `"Script file (optional)"` → `_SlotOnReleaseBtnScript()` |

**接口与速率 `"Target Interface & Speed"`**

- +0xc8 = 接口下拉（4 项，表 `PTR_DAT_140096af0`）→ `_SlotOnChangedCmbBxInterface(int)` → 写 `cfg+0x5a4`
- +0xd0 = `TIFSpeedComboBox` → `_SlotOnChangedCmbBxSpeed()` → 写 `cfg+0x5a8`
  （注意：**配置结构里 `+0x5a8` 既当采样周期、又被此处置为速率** [观测有冲突]；`FUN_1400419f0` 里 `JLINKARM_SetSpeed(cfg+0x5a0)`、`JLINK_HSS_Start(.., cfg+0x5a8, ..)`，故 `+0x5a8` 同时承担"接口速率 kHz"与"采样周期 µs"两种语义 —— 这是反编译的字段重叠，**真值需实测**，见 §10）
- +0xd8 = 只读标签，构造时 `&DAT_14006c650`

**采样源 `"Sampling Source"`**（QButtonGroup，exclusive，id 0/1/2）[观测]

| 偏移 | 文本 | id | 点击效果（`FUN_14002b7d0`） | 连线 |
|---|---|---|---|---|
| +0xe8 | **`"HSS (asynchronous)"`** | 0（默认 `setChecked(1)`） | `cfg+0x5ac=0, cfg+0x5ae=0`；**显示** ELF 组(+0x58) 与采样率组(+0x50) | — |
| +0xf0 | **`"RTT (synchronus)"`** | 1 | `cfg+0x5ac=1, cfg+0x5ae=0`；**隐藏** ELF 组与采样率组 | — |
| +0xf8 | **`"RTT-Plus (synchronus)"`** | 2 | `cfg+0x5ac=1, cfg+0x5ae=1`；**显示** ELF 组，**隐藏**采样率组 | — |

每组后跟 `?` 超链接标签指向 `https://kb.segger.com/UM08028_J-Scope#...`。注意拼写是 SEGGER 原文的 **`synchronus`**（少一个 o）。

**采样率 `"Sampling Rate"`**

- `"Sample every "` + LineEdit(+0x100) + 标签(+0x108) 初始 `"us, sampling rate: 10 kHz"`
- 提示 `"Set 0 to sample as fast as possible. <a href=\"...\">?</a>"`
- LineEdit 的 validator = `QRegExp("^[0-9]*$")`（`0x14006c6f8`）；`textChanged` → `_SlotOnTextChangedLineSampRate(QString)`（`FUN_14002be90`），把 µs 换算成 kHz 显示（`use/1000`）[推导]

**ELF `"ELF File"`**

- LineEdit(+0x110) + 按钮(+0x118) → `_SlotOnReleaseBtnElfFile()`（`FUN_14002b940`）
- 文件对话框：标题 `"Open output file"`，filter **`"Output files (*.elf *.out *.obj *.axf);;All files (*.*)"`**（`0x14006c658`）
- 选定后把路径同时写入 UI 与 `cfg+0x88`

**按钮**：`"OK"`（`0x14006cc0c`）→ `_SlotOnReleaseBtnOK()`，`"Cancel"`。

**初始化（编辑已有工程时）**：对话框构造末尾按 `cfg` 回填所有控件（器件名 `cfg+0x8`、ELF `cfg+0x88`、TCP 目标 `cfg+0x498`、脚本 `cfg+0x394`、速率 `cfg+0x59c`、连接型 `cfg+0x518`、采样模式 `cfg+0x5ac/0x5ae`、周期 `cfg+0x5b0`）[观测]。

### 4.4 OK 校验 `FUN_14002ba50` @ `0x14002ba50`（`chunk_003.c:1592`）[观测]

```c
if (isChecked(btn RTT(+0xf0))) {                 // RTT 模式
    if (targetDeviceLineEdit(+0xa8) 文本为空)
        { FUN_14003f640("In RTT mode specifying the target device is mandatory."); return; }
} else {                                          // HSS 与 RTT-Plus
    if (elfLineEdit(+0x110) 文本为空)
        { FUN_14003f640("No ELF file selected."); return; }
}
FUN_14002c250(...);                               // 汇总 UI → cfg（含速率数字解析）
if (新建)      { FUN_14003e110(0); FUN_14003e380(cfg); }   // 投递 APP: <InitProjSettings>
else           { FUN_14003e800(cfg); }                      // 投递 APP: <UpdateProjSettings>
QDialog::accept();   // vtable+0x198
```

**注意**："RTT 必须有器件名"这条校验只看是否勾了 RTT 单选，**不区分 RTT / RTT-Plus**；RTT-Plus 因为会显示 ELF 组，所以走 ELF 必填分支 —— 与 RTT-Plus 的语义（需要 ELF）一致 [推导]。

采样率数字本身**没有范围校验**，只靠 `^[0-9]*$` 挡非数字；`"Set 0 to sample as fast as possible"` 说明 0 是合法值 [观测]。

### 4.5 `JLINKARM_DEVICE_SelectDialog` 何时弹

两个**互不相同**的器件选择机制 [观测]：

| 机制 | 触发点 | 说明 |
|---|---|---|
| J-Scope **自带** `CDeviceSelectionDialog` | 用户点对话框里的 `"Specify Target Device"` 按钮 → `FUN_14002bc30` (`chunk_003.c:1672`) | 先 `JLINKARM_DEVICE_GetInfo(0xffffffff)` 取器件总数 N，按 `每个器件 0x218 字节` 分配缓冲，逐条 `JLINKARM_DEVICE_GetInfo(i, &info_0x978)`，构表 → `new CDeviceSelectionDialog` → `exec()`；确认后把器件名（≤0x80 B）写入 `cfg+0x8` 与 LineEdit |
| DLL 的 `JLINKARM_DEVICE_SelectDialog` | 连接序列 `FUN_1400419f0` 内，**当工程/命令行未指定器件名或该名字在 DLL 中查不到**时 | 由 `JLINKARM_DEVICE_SelectDialog(parent, 0, 0)` 弹出 DLL 自带选择框，再用 `GetInfo(idx, &info)` 取回 |

即：**用户在对话框里选过器件 → 走自带表格；没选过就直接连 → DLL 兜底弹窗**。`JSCOPE_ANALYSIS.md` §12.2 "不要抄弹窗选芯片"的结论对**第二条**成立；第一条是 J-Scope 自己的 UI，可自行决定是否复刻。

---

## 5. 设置持久化

### 5.1 配置文件 `JScopeSettings.ini`

**路径** = **exe 所在目录** + `JScopeSettings.ini` [观测]：
`FUN_14004f940` (`chunk_006.c:6165`) → `FUN_14004ef00("JScopeSettings.ini")` (`chunk_006.c:5637`)：
```c
DAT_14286c7c0 = FUN_140043110(0);              // 建临界区当锁
FUN_14004f6a0();                               // 加锁
uVar3 = malloc(0x104); memset(uVar3, 0, 0x104);
FUN_1400440e0(uVar3, "JScopeSettings.ini", &DAT_142865180, 0x104);  // 见下
free(uVar3);
if (file_exists(&DAT_142865180)) FUN_14004f590();  // 读进内存 30000 B
DAT_142865288 = 1;  FUN_14004f7e0();               // 解锁
```
`FUN_1400440e0` (`chunk_005.c:5452`) = 把"含 `:` 或 UNC 开头的串当绝对路径，否则与传入的目录拼接"；配合 `FUN_140043430` = `GetModuleFileNameA` 包装 —— 即 **ini 与 exe 同目录**（不是 `%APPDATA%`）[推导]。

**内存模型**：整个文件读入**固定 30000 字节缓冲** `DAT_142865290`，长度存 `DAT_142865284`；任何写操作都是**全文件重写**（`FUN_14004f510` 做 memmove 插入 + `FUN_14004f6c0` 用 `CreateFileA(GENERIC_WRITE)` 整块写回）。
文件 >30000 B 时报：
```
Could not open file [%s]
File too large - maximum file size (%d bytes) exceeded!     // %d 恒为 30000
```
读失败报：`Could not open file [%s]\nFailed to read data!`；写失败报 `Could not save file [%s]\nFailed to write data!` [观测]。

### 5.2 段落 / 键

| 段落 | 键 | 读 | 写 | 语义 |
|---|---|---|---|---|
| `PROJECT` | `Current` | `FUN_14004f7f0` (`chunk_006.c:6075`) | `FUN_14004f950` (`chunk_006.c:6177`) | 上次打开的 `.jscope` 全路径；写成功后还会 `FUN_14004e310(1,path)` 更新 MRU |
| `PATH` | `DataFiles` | `FUN_14004f820` (`chunk_006.c:6090`) | `FUN_14004f9b0` (`chunk_006.c:6199`) | 导出 Data(csv) 上次目录 |
| `PATH` | `ELFFiles` | `FUN_14004f890` (`chunk_006.c:6111`) | `FUN_14004fa30` (`chunk_006.c:6223`) | ELF 选择上次目录 |
| `PATH` | `ProjectFiles` | `FUN_14004f910` (`chunk_006.c:6150`) | `FUN_14004fab0` (`chunk_006.c:6247`) | 工程文件上次目录 |
| `LOG` | `LogLevel` | `FUN_14004f8c0` (`chunk_006.c:6126`) | —（无写入点） | 日志级别 1..9，默认 3 |

段落常量：`PATH`=`0x1400773fc`，`PROJECT`=`0x140077440`，`LOG`=`0x140077434` [观测，PE 字节]。
行格式：**`key="value"\r\n`**（`FUN_14004f1e0`：`sprintf(buf, "%s=\"%s\"\r\n", key, value)`），段落头 **`[section]\r\n`**（`FUN_14004f2a0`：`"[%s]\r\n"`）。
查找是**纯文本扫描**：先找 `[section]` 行，再在该段内找 `key` 后跟空格或 `=` 的行 [观测]。

错误串（`FUN_14004f0c0`，`chunk_006.c:5715`）：
```c
"Error while parsing settings file. Syntax error."                       // 0x140077390（key 为 NULL 时）
"Error while parsing subkey \"%s\" in settings file. Syntax error."      // 0x140077350（带 key 名）
```
另两条通用文件错误串在 `FUN_14004f590`/`FUN_14004f6c0`（见 §5.1）。

### 5.3 MRU（最近文件列表）

`FUN_14004e4b0(i, regPath, count)` @ `chunk_006.c:4973` —— **MRU 存在注册表，不在 ini**：

```c
sprintf(&DAT_14285ef40 + i*500, "%sRecent File List %d\\", regPath /*"Software\SEGGER\J-Scope\"*/, i);
hKey = RegCreateKeyA(HKEY_CURRENT_USER /*0x80000001*/, 该子键);
DAT_14285f908[i*4] = count;                      // 期望条数
for (j = 1; j <= count; j++) {
    sprintf(name, "File%i", j);                  // 1-based！
    size = 0x104;
    RegQueryValueExA(hKey, name, &type, buf_272, &size);
    存入 DAT_14285f920 + (i==1 ? idx*0x104 : ...) + i*0x1044;
}
```
写入方 `FUN_14004e610(i)` (`chunk_006.c:5036`)：`RegSetValueExA(hKey, "File%i", 0, 2 /*REG_EXPAND_SZ*/, path)`。

调用：`FUN_14004e4b0(1, "Software\\SEGGER\\J-Scope\\", 10)`（启动时读 10 条）；WelcomeDialog 只显示这 10 条（`FUN_140057220(.., MainWindow+0x230, 10)`）。
`"[%d]: %s"`（`0x140074618`）是参数回显格式，与 MRU 无关 [观测，反例纠正]。

### 5.4 注册表 `Software\SEGGER\%s\`

| 子键 | 值名 | 类型 | 用途 | 读 / 写 |
|---|---|---|---|---|
| `Software\SEGGER\J-Scope\` | `DontShowAgainStartJScope` | REG_DWORD | 0=显示欢迎向导（默认）、1=不显示、2=直接进新建工程 | `FUN_140040e20` / `FUN_140042750` |
| `Software\SEGGER\J-Scope\Recent File List 1\` | `File1..File10` | REG_EXPAND_SZ | MRU | `FUN_14004e4b0` / `FUN_14004e610` |
| `Software\SEGGER\J-Link\` 或 `...\Flasher\` | `InstallPath` | REG_SZ | JLink DLL 根目录 | `FUN_1400292f0` |

`%s` = `"J-Scope"`（`FUN_14003ea60` @ `chunk_005.c:819` 返回字面量 `"J-Scope"`）；完整应用名 `"SEGGER %s %s%s"` → `"SEGGER J-Scope V9.66"`（`DAT_14285dab0`，窗口标题），`"J-Scope V9.66"`（`DAT_14285daf0`）。版本串由 `FUN_140041470` 生成：`sprintf(buf, "V%d.%.2d", 9, 0x42)` = **`V9.66`** [观测]（`chunk_005.c:3209`）。

### 5.5 `LogLevel` 取值与生效点

- **取值**：`FUN_14004f8c0` (`chunk_006.c:6126`)：读 `[LOG] LogLevel`；**读失败/键不存在 → 返回 3**；值 <1 → 1；值 >9 → 9。即**有效域 1..9，默认 3**。
- **生效**：唯一消费者 `FUN_140041570(level)` @ `chunk_005.c:3244`：
  ```c
  iVar1 = FUN_14004f8c0();
  return (level <= iVar1) || (iVar1 == 9);      // 9 == 全开
  ```
- **调用点**（5 处，均为"是否输出这条消息"的门闸）：
  - `FUN_14003f830`（作用域进入 / 进度开始）：门槛 `depth*2`
  - `FUN_14003e9a0`（作用域退出 / 状态）：门槛 `depth*2 - 1`
  - `FUN_14003f1a0`（命令行错误）：门槛 `depth*2`
  - `FUN_14003f1e0`（`"[%d]: %s"` 参数回显）：门槛 `depth*2`
  - `FUN_1400423e0`（`"ERROR: %s"` 且归集到 `DAT_14285db80` 累积串）：门槛 `depth*2 - 1`，**但这是唯一"即使不满足门槛也强制累积到缓冲"的路径**（末尾按 `[%s]` + 弹框）
  - 从 `depth`（`DAT_14285eb8c`）参与门槛可见：**LogLevel 语义 = 缩进深度阈值**，即"最多打印到第几层嵌套"，而不是传统的"严重性等级" [推导]。
- 输出目的地：`FUN_14003f1e0`/`FUN_14003f830`/`FUN_14003e9a0` 都经 `_guard_check_icall(DAT_14286d388, text)`（`DAT_14286d388` = 唯一 MainWindow 实例）送达主窗口日志通道；`FUN_14003f430`(Error)/`FUN_14003f640`(Warning) 则经 `FUN_14003f580` → `FUN_14004df50` 弹 `DIALOG_CustMsgBox`（标题 `"Error"` / `"Warning"`，格式 `"%s %s"`）[观测/推断]。
- main 里 `setvbuf(stdout, NULL, _IONBF, 0)` 说明还有一条 stdout 输出路径 [观测]。

---

## 6. 工程文件会话流

> 元素层次与已分析过的 `FUN_140047020`（读）/ `FUN_1400471d0`（写）见 `JSCOPE_ANALYSIS.md` §10.1。本节只补**保存时机 / 版本号 / 错误串 / 两个未写全的元素**。

### 6.1 文件版本号 [观测]（`chunk_005.c:7317`）

```c
FUN_140046320(root):  <FileVersion major="1" minor="1"/>
```
写入常量直接是 **`major = 1`, `minor = 1`**（`FUN_1400398d0(doc, FileVersion, "major", 1)` / `"minor", 1`）。

读取与兼容判据 `FUN_1400455e0` @ `chunk_005.c:6706`：
```c
读不到 major → "Project file major version can not be determined!\nIt might not be compatible with this version of J-Scope."
读不到 minor → "Project file minor version can not be determined!\nIt might not be compatible with this version of J-Scope."
if (major > 1 || (major == 1 && minor > 1))
    Warning("The project has a newer file version and might not be compatible with this version of J-Scope.");
```
即：**当前版本 1.1，只警告不拒绝**（继续用当前结构解析）。

### 6.2 `<JLink/>` / `<Scope/>` 元素内容 [观测]

`<Project><Configuration>` 的子元素由 `FUN_140046110` 依次调用：

| 元素 | 函数 | 位置 | 属性 |
|---|---|---|---|
| `<JLink><Options .../></JLink>` | `FUN_1400463b0` | `chunk_005.c:7343` | `Connection`（默认 4，否则 `cfg+0x5ac` 非 0 时用 `cfg+0x5ac`… 见下）、`IsRTTSession`(+0x5a4)、`IsRTTWithTimeStamp`(+0x5a5)、`IsRTTPlus`(+0x5a6)、`SamplingPeriod`(+0x5a8)、`SerialNo`(+0x594)、`JTAG_IRPre`(+0x5b4)、`JTAG_DRPre`(+0x5b0)、`IP`(+0x490)、`TargetDevice`(+0x0)、`Nickname`(+0x510)、`InterfaceSpeed`(+0x5a0)、`TargetInterface`（枚举→串：`0→"JTAG"`、`1→"SWD"`、`3→"FINE"`、`7→"cJTAG"`，其它→`"SWD"`） |
| `<Scope><Options TimeResolutionUs=".."/></Scope>` | `FUN_140046750` | `chunk_005.c:7498` | 只有一个属性 `TimeResolutionUs`，写在 `<Scope><Options>` 上 |
| `<ScriptFile Name=".."/>` | `FUN_1400467d0` | — | 仅当 `cfg+0x394` 非空才写 |
| `<DataFile Name=".."/>` | `FUN_140046240` | — | 仅当 `cfg+0x18c` 非空 |
| `<RawFile Name=".."/>` | `FUN_1400466e0` | — | 仅当 `cfg+0x290` 非空 |

`Connection` 属性的取值来源在反编译中呈现为 `uVar3 = 4; if (cfg+0x5b8 == 0) uVar3 = cfg+0x5ac;` —— **`cfg+0x5ac` 同时被 NewProjectDialog 当"是否 RTT"写入、被这里当 `Connection` 写出**，属字段重叠，记入 §10 未决。

### 6.3 保存时机 [观测]

**没有任何自动保存 / 退出时自动写回**。全部是显式动作：

| 事件 | 路径 |
|---|---|
| 文件→保存 / Ctrl+S | MenuBar 槽 4 → `thunk_FUN_14003e6e0` → `FUN_14004eb40` (`chunk_006.c:5382`)：`FUN_1400471d0(路径)` 写盘 → 成功 `FUN_14003e9a0("Project saved successfully")`、`FUN_14004f950(path)` 更新 ini `[PROJECT]Current`、置 `DAT_14286506d=1`、`FUN_14004eef0(0)` **清脏标志**；失败 `FUN_14003e9a0("Failed to save project file")` |
| 文件→另存为 / Ctrl+Shift+S | 槽 5 → `thunk_FUN_14003e740` → `FUN_14004ebc0` (`chunk_006.c:5410`)：先弹 `"Save project as"` 文件框（filter `"J-Scope project files (*.jscope)"`）再走 `FUN_14004eb40` |
| 关闭工程 / 打开另一工程 / 退出 | `FUN_14004e8f0` (`chunk_006.c:5181`)：**若脏标志 `DAT_142865068 > 0`** → `FUN_14004e720` (`chunk_006.c:5084`) 弹出提示：<br>文本 `"The J-Scope project has been modified.\nDo you want to save your changes?"`，按钮 `"Yes"`(`0x140074740`) / `"No"`(`0x14007473c`)<br>返回 0(Yes) → 有路径走 `FUN_14004eb40`，无路径走 `FUN_14004ebc0`(Save As)；返回 2 表示取消并**中止退出** |

脏标志：`DAT_142865068`，由 `FUN_14004eef0(n)` (`chunk_006.c:5624`) 写并顺带刷新标题；标题由 `FUN_14003f930` 拼：`"<应用名> - [<工程路径>]"`，脏时追加 `" *"`（`0x140075054` = `" - ["`，`0x14007505c` = `" *"`，`0x14005f580` = `"]"`）[观测]。

### 6.4 读取/写入失败的用户可见错误串 [观测]

| 串 | 出处 |
|---|---|
| `"Failed to open project file %s."` | `FUN_140047020`（打开 XML 失败） |
| `"Failed to close project file."` | `FUN_140047020` / `FUN_1400471d0` |
| `"Failed to save project file"` | `FUN_14004eb40` |
| `"Project saved successfully"` / `"Project opened successfully"` / `"New project created successfully"` / `"Failed"` | `FUN_14004eb40` / `FUN_14004eda0` |
| `"Opening project file [%s] ..."` / `"Creating new project ..."` | `FUN_14004eda0`（`"%s project file [%s] ..."`） |
| `"Close project"` | `FUN_14004e8f0` |

---

## 7. 状态栏

### 7.1 `StatusBar`（`FUN_14004fbe0` @ `chunk_006.c:6306`）[观测]

```c
QStatusBar::QStatusBar(this, parent);
vftable = StatusBar::vftable;
+0x28 = +0x30 = +0x38 = NULL;
+0x30 = FUN_140035ff0(this, "");     // QLabel
+0x28 = FUN_140035ff0(this, "");     // QLabel
+0x38 = FUN_140035ff0(this, "");     // QLabel
addPermanentWidget(+0x38); addPermanentWidget(+0x30); addPermanentWidget(+0x28);
setSizeGripEnabled(true);
```
即**右侧三个永久标签，从右到左依次是 +0x38 / +0x30 / +0x28**。

### 7.2 三个字段与刷新函数 [观测]

| 偏移 | 刷新函数 | 内容模板 | 数据来源 |
|---|---|---|---|
| **+0x38**（最右） | `FUN_140050030` (`chunk_006.c:6486`) | `"%d "` + (`"Samples/s"` 或 `"kSamples/s"`) | `FUN_14003eb00` = `DAT_14285d1f8`（每秒样本数）；≥1000 时除以 1000 并切单位 |
| **+0x30** | `FUN_14004fd40` (`chunk_006.c:6392`) | `"Buffer: "` + `"%d "` + (`"Byte"`/`"KB"`/`"MB"`) + `" ("` + `"%d"` + `" samples), "` + `"%d"` + `" %"` | ① `FUN_14003ed50()` = 已用样本数 `DAT_14085d1e8`，乘 `0x4000000`（=64MB/变量）得字节数；阈值 0x401 切 Byte/KB/MB；② 固定容量 `0x800000` = 8 388 608 样本；③ `FUN_14003ead0()` = 环形缓冲填充百分比（`writeIdx * 100 * 2^-23`，未绕回时恒 100.0）<br>最终形如 **`Buffer: 512 KB (8388608 samples), 42 %`** |
| **+0x28**（最左） | `FUN_14004ff40` (`chunk_006.c:6451`) | `"FPS: "` + 数字 + `"  "` | `min(FUN_14003ed60()=DAT_140096dd8, FUN_14003eb10()=DAT_14285daa8)`，即**取"目标 FPS"与"实际 FPS"的较小者** |

对应 PE 字符串：`"FPS: "`(0x140077620)、`"Buffer: "`(0x140077628)、`"Byte"`(0x140077634)、`"KB"`(0x14007763c)、`"MB"`(0x140077640)、`"%d "`(0x140077644)、`" samples), "`(0x140077648)、`" %"`(0x140077654)、`"kSamples/s"`(0x140077658)、`"Samples/s"`(0x140077668)，`"  "`(0x1400718b4) [观测，PE 直读]。

**没有**"连接状态"和"通道"字段 —— 状态栏只有这 3 个数字标签 [观测，反例纠正]。

### 7.3 更新触发与周期

调用链 [观测]：

```
FUN_14004d870  = _SlotOnUpdateGUI（MainWindow metacall 索引 0x15）
  └─ FUN_14004fd10(StatusBar)                    // chunk_006.c:6378
       ├─ FUN_14004ff40(...)   → +0x28 FPS
       ├─ FUN_14004fd40(...)   → +0x30 Buffer
       └─ FUN_140050030(...)   → +0x38 Samples/s
```

- `_SlotOnUpdateGUI` 由信号 `2_SignalUpdateGUI()` 触发（`AutoConnection`），发送方是各工作函数用的 `FUN_14004d150`（解锁后发信号）——即**状态栏跟随工作线程的事件刷新，不是独立定时器** [推导]。
- **唯一的定时器**：`MainWindow+0x328`，`QTimer::setInterval(1000)`（**1 秒**），`2timeout()` → `1_SlotOnTimeout()` = `FUN_14004d7e0` (`chunk_006.c`)。其作用是**每秒投递一次 `"APP: <CheckELFModifications> Job"`（`FUN_14003e0b0`）**，与状态栏刷新无直接关系 [观测]。
- 工具栏另有 FPS 下拉（`_OnFPSCurrentIndexChanged`）→ 写目标 FPS（`FUN_14003f420(0x1e)` 为默认 30）[观测]。

---

## 8. 菜单 / 工具栏 / 快捷键全表

### 8.1 数据结构与注册机制 [观测]

`.data` 中的**静态动作描述符表**，每个描述符 **40 字节（0x28）**：

```c
struct ActionDesc {          // 40 B
    const char*  text;       // +0x00  QAction 文本（含 & 助记符）
    QKeySequence shortcut;   // +0x08  由 .CRT 静态初始化器填入（QKeySequence ctor）
    const char*  slotName;   // +0x10  moc 槽名（"1_SlotOnAction...()"）
    const char*  statusTip;  // +0x18  状态提示
    QAction*     action;     // +0x20  运行时创建后回填
};
```
菜单表（`0x1400963c8`，4 条，每条 32 B）：

```c
struct MenuDesc {            // 32 B
    const char* title;       // -0x18
    ActionDesc* actions;     // -0x10
    int         count;       // -0x08（int，落在上一字段高 4 字节）
    QMenu*      menu;        // +0x00 运行时
};
```
`FUN_140029ad0`（MenuBar ctor，`chunk_003.c:411`）：
```c
for (i = 0; i < 4; i++) {
    menu = FUN_140029f00(menubar, table[i].title);     // new QMenu + setTitle
    for (j = 0; j < table[i].count; j++)
        FUN_140029fa0(menubar, menu, &table[i].actions[j]);
}
setCheckable(DAT_140096300, true); setChecked(DAT_140096300, true);   // Symbol Watch
setCheckable(DAT_140096328, true); setChecked(DAT_140096328, true);   // Graph window
```
`FUN_140029fa0`（单个动作）：
```c
act = new QAction(menubar);
desc->action = act;
act->setText(desc->text);
if (QKeySequence::operator int(desc->shortcut) != 0) act->setShortcut(desc->shortcut);
if (desc->slotName) connect(act, "2triggered(void)", menubar, desc->slotName, 0);
menu->addAction(act);
act->setStatusTip(desc->statusTip);
```
即 **快捷键与槽连接全部由这张静态表驱动**；`setShortcut` 只在键序列非空时调用。

### 8.2 File 菜单（8 项，`0x1400961a0`）[观测]

| # | 文本 | 快捷键（Qt4 原始值） | 槽名 | 状态提示 |
|---|---|---|---|---|
| 0 | `&New Project` | `0x0400004e` = **Ctrl+N** | `1_SlotOnActionNewProjectDialog (void)` | Create a new Project |
| 1 | `&Open Project` | `0x0400004f` = **Ctrl+O** | `1_SlotOnActionOpenProjectDialog(void)` | Open an existing Project |
| 2 | `&Save Project` | `0x04000053` = **Ctrl+S** | `1_SlotOnActionSaveProject (void)` | Save the current Project |
| 3 | `Save Project &as ...` | `0x06000053` = **Ctrl+Shift+S** | `1_SlotOnActionSaveProjectAs (void)` | Save the current Project as ... |
| 4 | `Export &Data ...` | `0x04000044` = **Ctrl+D** | `1_SlotOnActionExportData (void)` | Save data to csv |
| 5 | `Export &Raw ...` | `0x04000052` = **Ctrl+R** | `1_SlotOnActionExportRaw (void)` | Save raw data to file |
| 6 | `Impor&t Raw` | `0x04000054` = **Ctrl+T** | `1_SlotOnActionImportRaw (void)` | Load data from file |
| 7 | `E&xit` | `0x08000058` = **Alt+X** | `1_SlotOnActionExit (void)` | Quit this app |

QAction 对象地址（由 `FUN_140029da0` 的使能刷新交叉验证）：`0x1400961c0/1e8/210/238/260/288/2b0/2d8`。

### 8.3 View / Tools / Help 菜单 [观测]

| 菜单 | `0x1400963b0`=`&File` (8) · `0x1400963d0`=`&View` (2) · `0x1400963f0`=`&Tools` (1) · `0x140096410`=`&Help` (2) |
|---|---|

| 菜单 | # | 文本 | 快捷键 | 槽名 | 提示 |
|---|---|---|---|---|---|
| &View | 0 | `Symbol Watch` | 无 | `1_SlotOnActionWatchWindow (void)` | Show watch window |
| &View | 1 | `Graph window` | 无 | `1_SlotOnActionGraphWindow (void)` | Show Graph window |
| &Tools | 0 | `Project Settings` | `0x0400002c` = **Ctrl+,** | `1_SlotOnActionProjectSettings(void)` | Show Project Settings |
| &Help | 0 | `User Guide` | `0x01000030` = **F1** | `1_SlotOnActionUserGuide (void)` | Open the manual |
| &Help | 1 | `&About` | `0x0100003b` = **F12** | `1_SlotOnActionAbout (void)` | Show about dialog |

> View 的两项在 ctor 里被置为 **checkable + checked**（`DAT_140096300` = View[0] = `Symbol Watch` 的 QAction，`DAT_140096328` = View[1] = `Graph window` 的 QAction；由 40 字节描述符布局 `+0x20 = QAction*` 交叉验证：`0x1400962e0+0x20 = 0x140096300`、`0x140096308+0x20 = 0x140096328`）。
> 点击行为：`FUN_14002a260`（槽 0xc = `_SlotOnActionWatchWindow`）读 `DAT_140096300` 的勾选态 → `FUN_14004d140`（转发到 `MainWindow+0x300` 的 vtable+0x58）；`FUN_14002a160`（槽 0xb = `_SlotOnActionGraphWindow`）读 `DAT_140096328` → `FUN_14004cf50`。两者**各自读各自的 action，无交叉** [观测]。

菜单表四项已用 PE 字节逐个确认：`&File`(8)、`&View`(2)、`&Tools`(1)、`&Help`(2)；标题串 `0x140064a44/4c/54/5c`，动作表基址 `0x1400961a0/2e0/330/360`（`count` 字段位于 `0x1400963c0/3e0/400/420`）。

### 8.4 MenuBar 槽派发表（`FUN_14003beb0`，`chunk_004.c:5812`）[观测]

| index | 槽名 | 实现函数 |
|---|---|---|
| 0 | `_SlotOnActionUserGuide()` | `FUN_14002a1f0` — `FUN_140036e60("https://kb.segger.com/UM08028_J-Scope")`（ShellExecute 类），失败报 `"Failed to open User Guide at:\n\"%s\""` |
| 1 | `_SlotOnActionAbout()` | `FUN_14002a0c0` → `FUN_14002ce80`（AboutDialog），`QDialog::exec` |
| 2 | `_SlotOnActionExit()` | `FUN_14002a130` → `FUN_14004dca0(MainWindow)` |
| 3 | `_SlotOnActionNewProjectDialog()` | `FUN_14002a1a0` → `FUN_14004de70(MainWindow)`（发 `_SignalNewProjectDialog`） |
| 4 | `_SlotOnActionSaveProject()` | `thunk_FUN_14003e6e0` |
| 5 | `_SlotOnActionSaveProjectAs()` | `thunk_FUN_14003e740` |
| 6 | `_SlotOnActionOpenProjectDialog()` | `FUN_14002a1b0` → `FUN_14003e420`（投递 `"APP: <OpenProjFile> Job"`） |
| 7 | `_SlotOnActionProjectSettings()` | `FUN_14002a1c0` → `FUN_14004dea0`（发 `_SignalOptionsDialog`） |
| 8 | `_SlotOnActionExportData()` | `thunk_FUN_140047390` |
| 9 | `_SlotOnActionExportRaw()` | `thunk_FUN_1400476d0` |
| 10 | `_SlotOnActionImportRaw()` | `FUN_14002a190` → `FUN_14004cf60` |
| 11 | `_SlotOnActionGraphWindow()` | `FUN_14002a160` |
| 12 | `_SlotOnActionWatchWindow()` | `FUN_14002a260` |

### 8.5 ToolBar（`FUN_140054790`，`chunk_007.c:830`）[观测]

**布局（从左到右）**：`Sampling:` 标签 → 分裂按钮（默认动作=Start Sampling，菜单里含 Attach to Debugger / Start Sampling）→ `Stop Sampling (F9)` → 分隔 → `Target:` 标签 → `Start Target` → `Reset Target` → F5 动作 → 弹性空格 → 缩放下拉 → Zoom In → Zoom Out → 分隔 → `Target FPS: ` 标签 → FPS 下拉 → 分隔 → `Autoscroll` 复选框。

| 偏移 | 控件/动作 | 文本 / 提示 | 快捷键 | 槽名 |
|---|---|---|---|---|
| +0x30 | QAction（仅标签） | `Sampling:` | — | — |
| +0x38 | QAction | （无文本） | **`0x01000034` = F5** | `1_OnActionStartSamplingShortcut(void)` |
| +0x40 | QAction | `Attach to Debugger` | — | `1_OnActionStartSamplingNoReset(void)` |
| +0x48 | QAction | `Start Sampling` | — | `1_OnActionStartSamplingNormal(void)` |
| +0x50 | QAction | 提示 `Stop Sampling (F9)` | **`0x01000038` = F9** | `1_OnActionStopSampling(void)` |
| +0x58 | QAction | 提示 `Zoom In` | **`0x01000013` = ↑ (Key_Up)** | `1_OnActionZoomIn(void)` |
| +0x60 | QAction | 提示 `Zoom Out` | **`0x01000015` = ↓ (Key_Down)** | `1_OnActionZoomOut(void)` |
| +0x68 | QAction（仅标签） | `Target:` | — | — |
| +0x70 | QAction | 提示 `Start Target` | — | `1_OnActionStartStopTarget(void)` |
| +0x78 | QAction | 提示 `Reset Target` | — | `1_OnActionResetTarget(void)` |
| +0x80 | QComboBox（缩放） | 27 项（见下），默认 `setCurrentIndex(0x12)` = 第 19 项 | — | `1_OnZoomCurrentIndexChanged(int)` |
| +0x88 | QLabel | `Target FPS: ` | — | — |
| +0x90 | QComboBox（FPS） | 11 项（见下），默认 index 7 = 30 | — | `1_OnFPSCurrentIndexChanged(int)` |
| +0xa0 | QToolButton | `setPopupMode(1)`（MenuButtonPopup）+ `setDefaultAction(+0x48)` | — | — |
| +0xa8 | QMenu | 含 +0x40 / +0x48 | — | — |
| +0x98 | QCheckBox | `Autoscroll` | — | `1_OnCheckedAutoscroll(int)` |
| +0xb0 | int | 状态机当前态（`FUN_140055480/55d0/5730/5800/...` 按态换图标/使能） | — | — |

**缩放下拉 27 项**（`0x140096f40`，单位自动后缀 `" us/Div"` / `" ms/Div"` / `" s/Div"`，数值 = µs/格）[观测，PE 直读]：
```
1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000,
100000, 200000, 500000, 1000000, 2000000, 5000000, 10000000, 20000000,
50000000, 100000000, 200000000, 500000000
```
默认选中第 19 项（index 0x12）= **200000 µs = 200 ms/Div**。

**FPS 下拉 11 项**（`0x140096fb0`）[观测，PE 直读]：`1, 2, 5, 10, 15, 20, 25, 30, 40, 50, 60`，默认 index 7 = **30**（并由 `FUN_14003f420(0x1e)` 落进全局）。

**ToolBar 槽派发表（`FUN_14003c630`，`chunk_004.c:6424`）**

| index | 槽名 | 实现 |
|---|---|---|
| 0 | `_OnActionStartSamplingNoReset` | `FUN_140055e50` |
| 1 | `_OnActionStartSamplingNormal` | `FUN_140055e90` |
| 2 | `_OnActionStartSamplingShortcut` | `thunk_FUN_140056130` |
| 3 | `_OnActionStopSampling` | `FUN_140055f90` |
| 4 | `_OnActionZoomIn` | `FUN_140055fc0` |
| 5 | `_OnActionZoomOut` | `FUN_140056000` |
| 6 | `_OnActionStartStopTarget` | `FUN_140055ed0` |
| 7 | `_OnActionResetTarget` | `FUN_140055dc0` |
| 8 | `_OnZoomCurrentIndexChanged(int)` | `FUN_1400560d0` |
| 9 | `_OnFPSCurrentIndexChanged(int)` | `FUN_140056070` |
| 10 | `_OnCheckedAutoscroll(int)` | `FUN_140056050` |

### 8.6 其它 QShortcut（局部，非菜单）[观测，仅登记]

| 宿主 | 键 | 槽 |
|---|---|---|
| `GraphWidget`（`chunk_006.c:1180` 区） | → (Right `0x01000014`) | `_OnTriggeredMoveGraphToRight(void)` |
| | ← (Left `0x01000012`) | `_OnTriggeredMoveGraphToLeft(void)` |
| | Home (`0x01000010`) | `_OnTriggeredMoveGraphToStart(void)` |
| | End (`0x01000011`) | `_OnTriggeredMoveGraphToEnd(void)` |
| | Ctrl+1 (`0x04000031`) / Ctrl+2 (`0x04000032`) | `_SlotOnShowX1(void)` / `_SlotOnShowX2(void)` |
| `CustTextView`（日志/文本视图） | Ctrl+C / Ctrl+A / Ctrl+F / Shift+F3 / Ctrl+G / Shift+Ctrl+G / Escape | `_OnCopy` / `_OnSelectAll` / `_SlotOnActFind` / `_SlotOnActFindPrev` / `_SlotOnActFindNext` / `_SlotOnEscape` |

（`GraphWidget`/`TableWidget` 内部细节属绘图/数值窗口专题。）

### 8.7 NewProjectDialog 槽派发表（`FUN_14003c020`，`chunk_004.c:5932`）[观测]

| index | 槽名 | 实现 |
|---|---|---|
| 0 | `_SlotOnToggleRadioUSB(bool)` | `FUN_14002c1b0` |
| 1 | `_SlotOnToggleCheckUSB(int)` | `FUN_14002c030` |
| 2 | `_SlotOnToggleRadioTCPIP(bool)` | `FUN_14002c150` |
| 3 | `_SlotOnToggleRadioExSes(bool)` | `FUN_14002c090` |
| 4 | `_SlotOnReleaseBtnTarget()` | `FUN_14002bc30`（器件表对话框） |
| 5 | `_SlotOnReleaseBtnScript()` | `FUN_14002bb30` |
| 6 | `_SlotOnChangedCmbBxInterface(int)` | `FUN_14002b8d0` |
| 7 | `_SlotOnChangedCmbBxSpeed()` | `FUN_14002b900` |
| 8 | `_SlotOnBtnGroupSamplingModeClicked(int)` | `FUN_14002b7d0` |
| 9 | `_SlotOnTextChangedLineSampRate(QString)` | `FUN_14002be90` |
| 10 | `_SlotOnReleaseBtnElfFile()` | `FUN_14002b940` |
| 11 | `_SlotOnReleaseBtnOK()` | `FUN_14002ba50` |
| 12 | `_SlotOnReleaseBtnCancel()` | `FUN_14002b930` |

### 8.8 MainWindow 槽派发表（`FUN_14003ba80`，`chunk_004.c:5424`）[观测]

信号 0..15：`_SignalFinAsyncAppInit, _SignalUpdateGUI, _SignalUpdateData, _SignalUpdateGraph, _SignalUpdateTable, _SignalNewProjectDialog, _SignalOptionsDialog, _SignalShowSymbolSelection, _SignalShowCustMsgBox, _SignalUpdateProgBar, _SignalGetSaveFileName, _SignalAssignColors, _SignalReset, _SignalClose, _SignalShowFileDialogOpen, _SignalShowFileDialogSave`

槽 16..32：

| index | 槽名 | 实现 |
|---|---|---|
| 0x10 | `_SlotOnNewProjectDialog` | `FUN_14004d4e0`（NewProjectDialog, `param_3=0`） |
| 0x11 | `_SlotOptionsDialog` | `FUN_14004d9c0`（NewProjectDialog, `param_3=1` = Project settings；确认后比对 ELF 路径，变了就发"重新解析 ELF"流程） |
| 0x12 | `_SlotOnShowSymbolSelection` | `FUN_14004d6c0` |
| 0x13 | `_SlotOnShowCustMsgBox` | `FUN_14004d5a0` |
| 0x14 | `_SlotOnUpdateProgBar` | `FUN_14004d930` |
| 0x15 | `_SlotOnUpdateGUI` | `FUN_14004d870` → 状态栏刷新（§7.3） |
| 0x16 | `_SlotOnUpdateData` | `FUN_14004d830` |
| 0x17 | `_SlotOnUpdateGraph` | `FUN_14004d900` |
| 0x18 | `_SlotOnUpdateTable` | `FUN_14004d990` |
| 0x19 | `_SlotOnGetSaveFileName` | `FUN_14004d460` |
| 0x1a | `_SlotOnAssignColors` | `FUN_14004d290` |
| 0x1b | `_SlotOnTimeout` | `FUN_14004d7e0`（1 s 定时器 → 投递 ELF 变更检查 Job） |
| 0x1c | `_SlotOnReset` | `FUN_14004d540` |
| 0x1d | `_SlotOnClose` | `QWidget::close` |
| 0x1e | `_SlotOnFinAsyncAppInit` | `FUN_14004d380`（§1.5 对话框决策） |
| 0x1f | `_SlotOnShowFileDialogOpen` | `FUN_14004d5e0` |
| 0x20 | `_SlotOnShowFileDialogSave` | `FUN_14004d650` |

### 8.9 图标资源 [观测]

`FUN_14002a340`（`chunk_003.c:860`）：循环 **`0x48 = 72`** 次，每项 `{const uchar* data; int size; QIcon* icon;}`（24 B），`QPixmap::loadFromData` + `new QIcon`，表基址 `0x140096430`。取用接口 `FUN_14002a320(i)` 返回 `(&DAT_140096438)[i*3]`。工具栏按状态机（+0xb0）用不同索引换图标。

---

## 9. About / 退出 / 单实例

### 9.1 AboutDialog（`FUN_14002ce80`，`chunk_003.c:2598`）[观测]

```c
QDialog ctor; vftable = AboutDialog::vftable; setModal(true);
setWindowTitle("About");                              // 0x14006d268
第一段文本 = sprintf("%s\nCompiled: %s %s\nhttps://www.segger.com", 版本串);
    版本串 = FUN_14003ea60() = "J-Scope"（[观测]：此处实际传入的是应用名字面量）
第二段文本 = "Copyright (C) %s 1998 - %s\n\nThe GUI of this software uses Qt4 which is under LGPLv21\n"
             "Add. 3rd party licensing and source code of Qt4:\nhttps://www.segger.com/qt-usage/"   // 0x14006d2b0
            （%s/%s 由 FUN_1400271c0/FUN_1400271b0 提供 = 编译日期/时间 [推断]）
第三段 = QLabel + QPixmap::loadFromData(&PNG_14006cca0, 0x3a2 /*930 B*/, "PNG")   // 0x14006d358
按钮 = "OK"；connect("2_Sig_CloseDlg(void)" → "1_Slot_OnBtnOK(void)")
setWindowFlags(... 清 0xfffe3fff ...); 再 |0x800; 再 |0x40000
setFixedSize(minimumSize()); show();
```
`"12:17:58"`(`0x14006d270`) 疑为编译时间常量 [推断]。

### 9.2 退出路径与清理 [观测]

```
菜单 File→Exit / Alt+X
  MenuBar 槽 2 = FUN_14002a130  →  FUN_14004dca0(MainWindow)   // chunk_006.c
      → FUN_14003bc00()                                         // MainWindow 的一个槽/方法
（Init Job 结束时若 DAT_14285da1d != 0 也直接调 FUN_14004dca0）
```

工程关闭 / 采样停止 / 链路关闭：

```
FUN_14004e8f0  (关闭工程; chunk_006.c:5181)
  ├─ FUN_14003ef10()  → FUN_14003f090()          // 若"正在采样"
  ├─ FUN_14003eef0()  → FUN_14003ef20()          // 若已连接 → JLINKARM_Close()
  ├─ if (脏) FUN_14004e720()                     // 保存询问（§6.3）
  ├─ FUN_14003ea40(); FUN_14003f830("Close project"); FUN_14004eef0(0); FUN_14003f330(); FUN_14003f320()
  └─ 之后由上层 FUN_14003ce50 决定是否重新投递 job

FUN_14003d7d0  (停止采样 + 断开; chunk_004.c:7535)
  ├─ 释放 HSS 变量表缓冲 DAT_14285da70
  ├─ if (mode == RTT)  JLINK_RTTERMINAL_Control(1)          // 关闭 RTT 通道
  ├─ else if (!skipStop && DAT_14085d180)  JLINK_HSS_Stop()  // 停 HSS（cfg+0x5b8 可跳过）
  ├─ memset(&DAT_14285d420, 0, 0x5bc)                        // 清连接配置
  └─ FUN_14003ef20()  →  if (DAT_14285da8c) { if (!DAT_14285d9d8) JLINKARM_Close(); ... }
```
`JLINKARM_Close`、`JLINK_HSS_Stop`、`JLINK_RTTERMINAL_Control` 全部经由按名解析的 thunk（§3.1）[观测]。**工程文件的写回只在显式保存时发生，退出时不会自动写 `.jscope`**（只会弹保存询问）。

### 9.3 单实例：**没有** [观测]

- 全程序 `CreateMutexA` **只有一个调用点**：`FUN_140043110` @ `0x140043110`（`chunk_005.c:4594`）：
  ```c
  if (param_1 != NULL) { HANDLE h = CreateMutexA(NULL, 0, param_1); ...; }
  else { InitializeCriticalSectionAndSpinCount(...); }
  ```
- 三个调用者全部传 **`0`**：`FUN_140039400`（`chunk_004.c:3223`，job 队列锁）、`FUN_14004c6a0`（MainWindow，存 `MainWindow+0x318`）、`FUN_14004ef00`（ini 锁）。→ **`CreateMutexA` 分支是死代码**。
- 未发现 `OpenMutexA` / `QLocalServer` / `QLocalSocket` / `FindWindow` / `CreateSemaphore` 的任何使用 [观测]。
- **结论：J-Scope 9.66 无单实例限制**，可以同时开多个进程（两个进程会共享同一个 `JScopeSettings.ini`，且都以 30000 B 整块重写 —— 存在写覆盖风险）[推导]。

---

## 10. 未知点 / 需实测确认

| # | 问题 | 现状 |
|---|---|---|
| U1 | 配置结构 `+0x5a8` 的语义 | 新建工程对话框把 `TIFSpeedComboBox` 的值写进 `cfg+0x5a8`（当"接口速率"），而 `JLINKARM_SetSpeed` 用的是 `cfg+0x5a0`；HSS 启动用 `cfg+0x5a8` 当采样周期。反编译里存在**字段重叠/别名**，需实测某一工程文件里 `InterfaceSpeed` 与 `SamplingPeriod` 两个 XML 属性是否同源 [观测有矛盾] |
| U2 | `Connection` 属性（`<JLink><Options Connection="..">`） | 反编译显示 `uVar3 = 4; if (cfg+0x5b8 == 0) uVar3 = cfg+0x5ac;`，与"连接类型"语义不吻合，疑为 Ghidra 字段错配 |
| U3 | `MainWindow+0x33C` 的语义 | `FUN_14003cc50` 写 0 或 2；`FUN_14004d380` 写 WelcomeDialog 的 `exec()` 返回值。两项用途冲突，未决 |
| U4 | WelcomeDialog 的 `exec()` 返回值语义 | 只知 `FUN_14004d380` 把结果存 `+0x33c`，未见消费点 |
| U5 | `MainWindow+0x340/0x341` 的精确枚举 | 仅由 `FUN_140040e20` 的 0/1/2 分支推出三个状态；`-openprj` 与 `.jscope` 裸路径参数下 `FUN_140041cd0` 的返回值为 0，因此 `+0x342=1` 的触发条件未能确证 [推断] |
| U6 | `_guard_check_icall(DAT_14286d388, str)` 的真实形态 | `DAT_14286d388` 是 MainWindow 指针却被当函数指针调用；疑为 Ghidra 对 `QMetaObject::invokeMethod`/虚调用的建模失真。日志实际落到哪个控件（`CustTextView`? 状态栏?）未确证 |
| ~~U7 选项表第 2/3 条的掩码~~ | — | ✅ 已用 PE 直读确认：`-IP`/`-RTTSearchRanges` 掩码均为 `0x1`（见 §2.1） |
| U8 | `0x1040807` / `0x42030` / `0x200` / `0x300` 等 Qt/Win32 标志常量 | 未逐位解码（不影响重实现） |
| U9 | 采样率输入框是否有隐含上限 | 只有 `^[0-9]*$`；`FUN_14002c250` 汇总时是否 clamp 未查 |
| U10 | `StatusBar` 的 "FPS" 取值 | `min(DAT_140096dd8, DAT_14285daa8)` 中两者分别是"目标/实际 FPS"还是反过来，未确证 |

---

## 11. 需要补充反编译（当前 dump 不足）

| 函数/对象 | 位置 | 需要什么 |
|---|---|---|
| `FUN_14003f1e0` 的**调用者上下文** | `chunk_005.c:1767` | `"[%d]: %s"` 回显被内联，需汇编级确认 `"Application log started"` / `"Optional command line parameters found"` 两条串的插入点 |
| `FUN_14004ef00` 的**实参链** | `chunk_006.c:5637` | `FUN_1400440e0` 的寄存器传参被 Ghidra 丢了，ini 绝对路径（`<exedir>\JScopeSettings.ini`）需汇编确认 |
| `FUN_14004bfc0` 附近（`FUN_14004bc30`..`FUN_14004c020`） | `chunk_006.c` | `DAT_142865068`（脏标志）的全部写点，确认是否存在除保存/新建之外的清点 |
| `FUN_14002c250`（NewProjectDialog 汇总） | `chunk_003.c` | UI → cfg 的完整字段映射（含采样率数字解析与 clamp） |
| `FUN_14004dca0` / `FUN_14003bc00` | `chunk_006.c` / `chunk_004.c` | 退出时真正调用的方法（vtable 偏移未定） |
| `FUN_14003bd60` / `FUN_14003bda0`（`thunk_FUN_14003bd60`） | `chunk_004.c` | `FUN_14004e150`/`FUN_14004e1f0` 尾部调用的虚函数 |
| ~~`0x1400963c0` / `0x140096400` / `0x140096420` 三个 count 字~~ | — | ✅ 已用 PE 直读确认：`&File=8`、`&View=2`、`&Tools=1`、`&Help=2`（见 §8.3） |
| `FUN_140034630`（`DIALOG_CustMsgBox` ctor） | `chunk_003.c` | 消息框按钮/图标标志（`0x200`/`0x300`）的解码 |

---

## 12. 对 OpenScope 的启示

1. **命令行只做"薄壳"，两阶段解析值得抄。**
   J-Scope 把 `-openprj` 单独放在阶段 2 先"探明"，再在阶段 1 才真正执行；`-USB`/`-IP` 的"必须与 `-openprj` 同用"就靠阶段 2 留下的一个布尔标志实现，**没有额外的校验框架**。
   OpenScope 若要加网络/多探针参数，照这个模式只需一张 `{串, 掩码, 处理函数}` 表 + 一个 `for` 循环（`FUN_140041cd0` 全文不到 100 行 C）。注意 J-Scope 用的是**前缀匹配**（`strncmp(arg, name, strlen(name))`），这会让 `-openprjX` 也命中，重实现时建议改成整串匹配。

2. **后端 DLL 的三级查找 + 按名解析是可直接复用的模板，但要修正一处认知。**
   实际顺序是 **exe 同目录 → 另一模块目录 → 注册表 `SEGGER\J-Link\InstallPath`（再 `bin_x64\`）→ 注册表 `SEGGER\Flasher`**，而不是"先交给系统搜索路径"。HKCU 先于 HKLM。
   29 个 API 全部惰性解析 + 失败给 `MessageBox` + `exit(1)`，语义干净。OpenScope 若同时支持 J-Link/ST-Link，建议照此抽象，但**把 `exit(1)` 换成可恢复错误**（J-Scope 一旦找不到 DLL 就直接杀进程）。

3. **设置持久化：J-Scope 的实现有明确缺陷，别照抄。**
   ① `JScopeSettings.ini` 与 exe 同目录（`Program Files` 下通常无写权限，且多进程会互相覆盖）；② 整个文件用一个 **30000 字节定长缓冲**读改写，**超过就拒绝启动读取**；③ 每次改一个键都**整文件重写**。
   OpenScope 应改为 `%APPDATA%` 下的 `QSettings`/JSON，并做"读-改-写+文件锁"，MRU 也放一起而不是拆到注册表（J-Scope 的 MRU 在 `HKCU\Software\SEGGER\J-Scope\Recent File List 1\`，键名 `File1..File10`，**1 基**）。

4. **`LogLevel` 其实不是"严重性等级"，而是"缩进深度阈值"。**
   `FUN_140041570(level)` 就是 `level <= LogLevel || LogLevel == 9`，而调用方传的 `level` 全部形如 `depth*2±1`。值域 1..9、默认 3。
   OpenScope 若要分级日志，建议**分开两个概念**：严重性（Error/Warn/Info/Debug）+ 深度阈值，别学 J-Scope 把两者压进一个整数。

5. **UI 侧最有复用价值的三个设计**：
   - **静态动作描述符表驱动菜单/快捷键**（§8.1）：`{文本, QKeySequence, 槽名, 状态提示}` 40 字节一条，`setShortcut` 与 `connect` 全自动。OpenScope 加菜单只需加一行数据。
   - **"Welcome 向导三态"**：注册表一个 DWORD 控制"显示向导 / 不显示 / 直接进新建工程"，比自己发明状态机省事（`FUN_140040e20`）。
   - **状态栏三字段**（FPS / Buffer / Samples/s）与 buffer 百分比的自解释格式 `Buffer: 512 KB (8388608 samples), 42 %` —— 这种"绝对量+占位率"的写法对调试采样很友好，OpenScope 可直接借鉴（OpenScope 还应加**连接状态**，J-Scope 这里是没有的）。

6. **不要复刻的两点**：
   - **无单实例**且 `JScopeSettings.ini` 无锁整写 → 多开会互相踩。OpenScope 有网络多实例需求，务必加命名互斥或至少给设置文件加锁。
   - **`-USB`/`-IP` 与 `-openprj` 强绑定**：J-Scope 这么做是因为连接参数只存在工程文件里。OpenScope 的连接参数已经独立于工程，应该允许 `--connect` 单独使用，把"两阶段解析"当作**机制**而不是**限制**来继承。
