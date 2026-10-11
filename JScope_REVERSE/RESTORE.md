# JScope 逆向分析 — 备份恢复说明

本备份为 **JScope 逆向分析任务** 的工作快照（2026-10-11）。

## 内容

| 路径 | 内容 | 是否在 git |
|---|---|---|
| **`report.html`** | **完整报告页（19 节 · 双主题单文件 · 421 KB）** | ✅ |
| `JSCOPE_ANALYSIS.md` | 精编报告（含第二轮修正说明与专题索引） | ✅ |
| `analysis/*.md` | **8 个专题报告**：trigger / export / graph / elf / rttplus / infra / session | ✅ |
| `analysis/report/` | 报告页的构建源（分节 HTML + `md2html.py` + `build.py`） | ✅ |
| `dumps/alldecomp/` | **全部 2,001 个函数反编译**（9 分块 + `index.tsv` 调用图索引，2.2 MB） | ✅ |
| `dumps/globals.txt` | 4,837 个全局变量（地址/类型/初值/引用者） | ✅ |
| `request.md` | 聚焦任务书 | ✅ 于 `D:\OpenScope` 仓库 |
| `dumps/*.txt` | 第一轮专题反编译、字符串、函数清单 | ✅ |
| `tools/ghidra_scripts/` | 7 个 Ghidra 脚本（含全量反编译 `DumpAllDecomp`、`DumpGlobals`） | ✅ |
| `tools/run_*.bat`、`tools/xref.py` | 分析调用封装（每个调用一个专用 .bat） | ✅ |
| `ghidra_proj/` | **Ghidra 工程数据库（29 MB）** | ❌ 已 gitignore |

**`ghidra_proj/` 未进 git** —— 但它能让你直接打开工程继续分析，无需重跑。
`tools/`（1.9 GB 的 Ghidra 与 JDK）同样未进 git，因为可重新下载。

## 恢复步骤

### 1. 恢复仓库

```bash
git clone git@gitee.com:xiaofei558008/open-scope.git     # 或 github_origin
cd open-scope
git checkout jscope-re-v1
```

### 2. 重新生成报告页（无需工具链）

```bash
python analysis/report/md2html.py analysis/graph.md analysis/report/13_graph.html 13 s13 "标题"
python analysis/report/build.py          # 拼装 analysis/report/*.html → report.html
```

### 3. 恢复 Ghidra 工程（可选，用于继续分析）

把本备份的 `ghidra_proj/` 放回 `D:\OpenScope\JScope_REVERSE\ghidra_proj\`。
工程是为 J-Link V966 的 `JScope.exe` 建立的，换版本需重新导入。

### 4. 重建工具链（若需要重跑分析）

本机原先没有任何反汇编工具，以下是本次搭建的来源：

```bash
# JDK 21（Ghidra 12.x 必需）
curl -L -o jdk21.zip "https://api.adoptium.net/v3/binary/latest/21/ga/windows/x64/jdk/hotspot/normal/eclipse"

# Ghidra 12.1.4
curl -L -o ghidra.zip "https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_12.1.4_build/ghidra_12.1.4_PUBLIC_20260921.zip"
```

解压到 `D:\OpenScope\JScope_REVERSE\tools\`，然后：

```bat
tools\run_ghidra.bat                  :: 导入 + 全量分析（约 65 s）
tools\run_script.bat DumpOverview.java
```

### 注意事项

- `.bat` 内必须 `set JAVA_HOME=...\tools\jdk-21.0.12.1+1`，否则 analyzeHeadless 找不到 Java。
- **不要**在 Git Bash 里用 `cmd //c "...参数..."` 传带引号或正则的参数 —— 引号会被吞掉。
  每次调用写一个专用 `.bat`（这正是 `tools/` 下有一堆 `run_*.bat` 的原因）。
- Ghidra 脚本中 `Pattern.matcher(s).matches()` 是整串匹配，子串匹配要用 `.find()`。

## 分析结论摘要（第二轮）

- JScope.exe **不静态导入** JLinkARM：运行时按名 `GetProcAddress` 解析 **29 个 API**
- **HSS 高速采样**（J-Link SDK 未公开文档）：`JLINK_HSS_Start(16B/变量描述符数组 + {0,0} 终止项, 数量, 周期µs, 1)`；
  采样包 = 4 字节 µs 时间戳 + 各变量按预算偏移紧密排列；10 ms 周期批读约 2 MB
- **RTT VarFormat**：上行通道名 `JScope_<类型字符+尺寸数字成对串>`，目标端零 ELF 依赖
- 数据模型：静态 32 MB 时间戳环 + **每变量 64 MB** 数据环（固定 8 B/样本槽位）；
  HSS 与 RTT 共用同一 `UnpackSamples`
- `TimeResolutionUs` 全局**只写不读**，HSS 时间戳原样存储（本身即 µs）

**第三轮（未解项攻关，10 项闭环 8 项）**

- **HSS 契约在主机侧完全闭环**（反汇编 JLink_x64.dll）：caps 为 **32 B**（`+0` 最大变量数、`+4` 最大采样率 Hz、
  `+8` 能力位）；`-2/-3/-4` 三个错误码都在 DLL 内产生；周期会被**静默钳位**到 `1e6/caps+4`；
  `JLINK_HSS_Read` 返回**字节数**；`flags.bit0` 受 `caps+8.bit1` 门控（位义在探针固件）
- **版本门槛 `0xC79B` = V5.10.99**（版本整数十进制打包：`major=v/10000`）
- **两个"从未被写"的全局**：`DAT_14085d1b0`（RTT 读缓冲 ⇒ 恒 0x2000/8 KB）、
  `DAT_14285eb9c`（堆守卫开关 ⇒ 发布版校验全关）
- **≈30 MB 静态区零引用**（108,121 条指令 RIP 扫描）→ 纯预留，无功能影响
- **作业处理器表 16 项全部还原**；其中 6 个函数 Ghidra 从未创建（"暂停"= 单字节 `DAT_140096DD0=1`）
- **日志回调是空桩**（`0x14000a760` = `ret 0`）：日志只进 4 KB 全局缓冲，段末弹框
- 新工具：`tools/rip_scan.py`、`tools/dll_hss.py`、`tools/ghidra_scripts/DecompAt.java`（+ `run_orphans.bat`）

**第二轮补充（全量反编译 + 8 专题）**

- **RTT-Plus 逐字节**：配置串实测 **16 B/条**（`+0 Addr / +4 0 / +8 Size / +12 未初始化`），终止 12 B 0 ——
  与 SEGGER UM08028 文档（8 B/条）**不一致**；下行 `JScope_Config`、上行 `JScope_Data[_t]`
- **触发**：24 B 配置结构 + 8 个类型比较器；命中即 `writeIdx=0` 重开记录；XML 真身
  `<Trigger><Options Level Symbol On Auto Rise Fall/></Trigger>`（`Auto` 在本版本无读取方）
- **导出**：CSV（**分号**分隔 / `%u;%f;%i` / 物理下标）与 **JRAW 无头裸流**
  （`repeat N {u32 ts_us; var0.size; …}`，与 HSS 样本流同构，可被 J-Scope 直接导入）
- **基础设施**：单 worker 线程（`"WORKER"`）+ 32 槽环形命令队列 + 16 项处理器表；
  调试堆块头 `0xDEADBEEF` + 32×`0xCE` 尾部哨兵（**发布版守卫开关无人写 = 关闭**）
- **TypeID 14 = 位域**（新增可观测类型）；TypeID 3/4 无产出点；**64 位整型不可观测**
- **配置结构**：`ProjectSettings(0x5E8)` 的 `+0x08` 即 `Configuration(0x5BC)` ——
  读写基址相差 8，是"字段表互相矛盾"的根因

详见 `JSCOPE_ANALYSIS.md` 或浏览器打开 `report.html`。
