# JScope 逆向分析 — 备份恢复说明

本备份为 **JScope 逆向分析任务** 的工作快照（2026-10-11）。

## 内容

| 路径 | 内容 | 是否在 git |
|---|---|---|
| `JSCOPE_ANALYSIS.md` | 完整分析报告 | ✅ |
| `report.html` | 可分享的单文件报告页（双主题） | ✅ |
| `request.md` | 聚焦任务书 | ✅ 于 `D:\OpenScope` 仓库 |
| `dumps/` | 各路径反编译文本、字符串、函数清单 | ✅ |
| `tools/ghidra_scripts/` | 5 个 Ghidra 脚本 | ✅ |
| `tools/run_*.bat`、`tools/xref.py` | 分析调用封装 | ✅ |
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

### 2. 恢复 Ghidra 工程（可选，用于继续分析）

把本备份的 `ghidra_proj/` 放回 `D:\OpenScope\JScope_REVERSE\ghidra_proj\`。
工程是为 J-Link V966 的 `JScope.exe` 建立的，换版本需重新导入。

### 3. 重建工具链（若需要重跑分析）

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

## 本次分析结论摘要

- JScope.exe **不静态导入** JLinkARM：运行时按名 `GetProcAddress` 解析 **29 个 API**
- **HSS 高速采样**（J-Link SDK 未公开文档）：`JLINK_HSS_Start(16B/变量描述符数组 + {0,0} 终止项, 数量, 周期µs, 1)`；
  采样包 = 4 字节 µs 时间戳 + 各变量按预算偏移紧密排列；10 ms 周期批读约 2 MB
- **RTT VarFormat**：上行通道名 `JScope_<类型字符+尺寸数字成对串>`，目标端零 ELF 依赖
- 数据模型：静态 32 MB 时间戳环 + **每变量 64 MB** 数据环（固定 8 B/样本槽位）；
  HSS 与 RTT 共用同一 `UnpackSamples`
- `TimeResolutionUs` 全局**只写不读**，HSS 时间戳原样存储（本身即 µs）

详见 `JSCOPE_ANALYSIS.md` 或浏览器打开 `report.html`。
