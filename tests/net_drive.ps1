# 需求14 网络远程操作全功能 E2E（三实例，真实硬件）：
#   A=探针侧：监听 127.0.0.1:10000 + UI 点击连接 J-Link + 采集 + 广播（一对多 fan-out）
#   B=远端显示侧：连接 + ELF 双向同步 + 勾选变量 + 建波形窗口 + 发监视列表（驱动 A 采集）
#                 + 网络写变量（回 ACK）+ 下载采集历史（异步 CHUNK）+ 窗口截图
#   C=第二个远端：连接 + 发监视列表（验证一对多）
# 断言全部基于 openscope.log 中的 ASCII 标记 + 截图文件，避免编码问题。
param(
    [string]$Elf = "D:\OpenScope\tests\elf_sample.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$outDir = Join-Path $PSScriptRoot "out"
if (-not (Test-Path $outDir)) { New-Item -ItemType Directory -Path $outDir | Out-Null }
$logA = Join-Path $outDir "net_a.log"   # 探针侧实例日志
$logB = Join-Path $outDir "net_b.log"   # 远端显示侧实例日志
$logC = Join-Path $outDir "net_c.log"   # 第二个远端实例日志
$shotBmp = Join-Path $outDir "net_remote_chart.bmp"
$shotMain = Join-Path $outDir "net_remote_main.png"
$shotProbe = Join-Path $outDir "net_probe_main.png"
foreach ($p in @($logA, $logB, $logC, $shotBmp, $shotMain, $shotProbe)) {
    if (Test-Path $p) { Remove-Item $p -Force }
}

Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Milliseconds 300

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class NetUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern IntPtr GetDlgItem(IntPtr h, int id);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int left, top, right, bottom; }
}
"@

function Find-Main([int]$ProcId) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [NetUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId -and [NetUi]::IsWindowVisible($h)) {
            $sb = New-Object System.Text.StringBuilder 256
            [NetUi]::GetWindowTextW($h, $sb, 256) | Out-Null
            if ($sb.ToString() -like "OpenScope*") { $script:hit = $h; return $false }
        }
        return $true
    }
    [NetUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Wait-Main([int]$ProcId, [int]$TimeoutMs) {
    $sw = [Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        $h = Find-Main $ProcId
        if ($h -ne [IntPtr]::Zero) { return $h }
        Start-Sleep -Milliseconds 200
    }
    return [IntPtr]::Zero
}

function Click-Button([IntPtr]$hMain, [int]$id) {
    $b = [NetUi]::GetDlgItem($hMain, $id)
    if ($b -ne [IntPtr]::Zero) {
        [NetUi]::SendMessage($b, 0x00F5, [IntPtr]::Zero, [IntPtr]::Zero) | Out-Null  # BM_CLICK
        return $true
    }
    return $false
}

function Get-ButtonText([IntPtr]$hMain, [int]$id) {
    $b = [NetUi]::GetDlgItem($hMain, $id)
    if ($b -eq [IntPtr]::Zero) { return "" }
    $sb = New-Object System.Text.StringBuilder 256
    [NetUi]::GetWindowTextW($b, $sb, 256) | Out-Null
    return $sb.ToString()
}

function Save-WindowPng([IntPtr]$h, [string]$path) {
    try {
        Add-Type -AssemblyName System.Drawing
        $r = New-Object NetUi+RECT
        [NetUi]::GetWindowRect($h, [ref]$r) | Out-Null
        $w = $r.right - $r.left; $hh = $r.bottom - $r.top
        if ($w -le 0 -or $hh -le 0) { return }
        $bmp = New-Object System.Drawing.Bitmap $w, $hh
        $g = [System.Drawing.Graphics]::FromImage($bmp)
        $hdc = $g.GetHdc()
        [NetUi]::SendMessage($h, 0x0317, $hdc, [IntPtr]::Zero) | Out-Null  # WM_PRINT, CLR=PRF_CLIENT
        $g.ReleaseHdc($hdc)
        $g.Dispose()
        $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
        $bmp.Dispose()
    } catch { Write-Output "  (warn) PrintWindow 截图失败: $($_.Exception.Message)" }
}

# ---- 阶段 1：A（探针）启动 + UI 点击连接 J-Link ----
$a = Start-Process -FilePath $exe -ArgumentList $Elf,"--no-layout","--log=$logA","--net-listen=10000" -PassThru
$hA = Wait-Main $a.Id 8000
if ($hA -eq [IntPtr]::Zero) { Write-Output "FAIL A 主窗口未出现"; exit 1 }
Start-Sleep -Milliseconds 1500   # 等模块加载/设备扫描完成
Click-Button $hA 2002 | Out-Null # 连接按钮

# 轮询等 A 的 J-Link 连接成功（日志 "J-Link 已连接"）
$hw = $false
for ($i = 0; $i -lt 16; $i++) {
    Start-Sleep -Milliseconds 500
    $tail = Get-Content $logA -Tail 200 -Encoding UTF8 -ErrorAction SilentlyContinue
    if ($tail -match "J-Link 已连接") { $hw = $true; break }
}
if ($hw) { Write-Output "INFO A 已连接 J-Link（硬件就绪）" } else { Write-Output "WARN A 未检测到 J-Link 连接，硬件相关断言将降级" }

# ---- 阶段 2：B（远端显示侧）启动 ----
$bArgs = @(
    $Elf,"--no-layout","--log=$logB","--net-connect=127.0.0.1:10000","--net-sync",
    "--watch=g_counter,g_cfg.a","--net-win=chart,g_counter,g_cfg.a","--net-watch",
    "--net-write=g_cfg.b=123","--net-download",
    "--net-shot-at=$shotBmp,10500","--net-exit=13500"
)
$b = Start-Process -FilePath $exe -ArgumentList $bArgs -PassThru
$hB = Wait-Main $b.Id 8000
if ($hB -eq [IntPtr]::Zero) { Write-Output "FAIL B 主窗口未出现"; Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force; exit 1 }

# 校验 B 的网络工具栏新按钮（同步采集/下载记录）
$btWatch = Get-ButtonText $hB 2117
$btLog = Get-ButtonText $hB 2118
if ($btWatch -eq "同步采集") { Write-Output "PASS UI 按钮 同步采集 (2117)" } else { Write-Output "FAIL UI 按钮 2117 文本='$btWatch'" }
if ($btLog -eq "下载记录") { Write-Output "PASS UI 按钮 下载记录 (2118)" } else { Write-Output "FAIL UI 按钮 2118 文本='$btLog'" }

# ---- 阶段 3：C（第二个远端，一对多 fan-out）----
Start-Sleep -Seconds 2
$c = Start-Process -FilePath $exe -ArgumentList $Elf,"--no-layout","--log=$logC","--net-connect=127.0.0.1:10000","--watch=g_counter","--net-watch","--net-exit=10000" -PassThru

# ---- 阶段 3.5：采集运行中抓取两个主窗口（报告用，WM_PRINT 抓取）----
Start-Sleep -Seconds 5
if ($hA -ne [IntPtr]::Zero) { Save-WindowPng $hA $shotProbe }
if ($hB -ne [IntPtr]::Zero) { Save-WindowPng $hB $shotMain }

# ---- 阶段 4：等 B 自动退出（net-exit=13500），随后清理 A/C ----
Start-Sleep -Seconds 15
Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Milliseconds 300

# ---- 阶段 5：读各实例独立日志并断言（--log= 钩子，无共享文件竞争）----
function Read-Log([string]$p) {
    if (Test-Path $p) { return [System.IO.File]::ReadAllText($p, [System.Text.Encoding]::UTF8) }
    return ""
}
$logAText = Read-Log $logA
$logBText = Read-Log $logB
$logCText = Read-Log $logC

$fail = 0
function Check([string]$name, [bool]$cond) {
    if ($cond) { Write-Output "PASS $name" } else { Write-Output "FAIL $name"; $script:fail++ }
}

# 1. 监听 + 2 个客户端连接均 rc=0（各实例各自日志）
Check "A 监听 rc=0" (([regex]::Matches($logAText, "10000 rc=0")).Count -ge 1)
Check "B 连接 rc=0" (([regex]::Matches($logBText, "10000 rc=0")).Count -ge 1)
Check "C 连接 rc=0" (([regex]::Matches($logCText, "10000 rc=0")).Count -ge 1)

# 2. 一对多 fan-out：A 收到 2 个客户端接入
$joined = ([regex]::Matches($logAText, "客户端接入")).Count
Check "一对多 fan-out：A 客户端接入 ≥2 (count=$joined)" ($joined -ge 2)

# 3. ELF 双向同步：B 收 A 的 ELF（HELLO 回发 + ELF_REQ 回发），A 收 B 的 ELF（上传）
$elfA = ([regex]::Matches($logAText, "收到 ELF 变量表 4 项")).Count
Check "A 收到 B 上传的 ELF 变量表 4 项 (count=$elfA)" ($elfA -ge 1)
$elfB = ([regex]::Matches($logBText, "收到 ELF 变量表 4 项")).Count
Check "B 收到 A 回发的 ELF 变量表 4 项 ≥2 (count=$elfB)" ($elfB -ge 2)

# 4. 远端监视列表驱动采集：A 勾选 + 启动采集
$watchB = ([regex]::Matches($logAText, "WATCH_LIST 2 项")).Count + ([regex]::Matches($logAText, "远端监视 2 项")).Count
Check "B 监视列表 2 项下达并被 A 接收 (count=$watchB)" ($watchB -ge 1)
$watchC = ([regex]::Matches($logAText, "WATCH_LIST 1 项")).Count + ([regex]::Matches($logAText, "远端监视 1 项")).Count
Check "C 监视列表 1 项下达并被 A 接收 (count=$watchC)" ($watchC -ge 1)
$acq = ([regex]::Matches($logAText, "采集启动 rc=0")).Count
Check "远端监视驱动采集启动 rc=0 (count=$acq)" ($acq -ge 1)

if ($hw) {
    # 5. A 实际采集（J-Link 硬件路径）
    $rate = ([regex]::Matches($logAText, "采集速率:")).Count
    Check "A 硬件采集运行（采集速率日志 ≥2）(count=$rate)" ($rate -ge 2)

    # 6. 网络写变量：A 经 J-Link 写 MCU，B 收到 ACK 0
    Check "A 侧 WRITE_VAR g_cfg.b=123 -> 0" (([regex]::Matches($logAText, "WRITE_VAR g_cfg.b=123 -> 0")).Count -ge 1)
    Check "B 侧网络写入 ACK 0" (([regex]::Matches($logBText, "网络写入 g_cfg.b=123 -> ACK 0")).Count -ge 1)

    # 7. 异步传输：B 下载 A 的采集历史（CHUNK 流）
    $hist = [regex]::Match($logBText, "历史数据回传完成 (\d+) 样本")
    if ($hist.Success -and [int]$hist.Groups[1].Value -ge 50) {
        Check "异步历史回传完成 $($hist.Groups[1].Value) 样本（≥50）" $true
    } else {
        Check "异步历史回传完成（样本数=$($hist.Groups[1].Value)）" $false
    }
    Check "A 侧历史回传日志" (([regex]::Matches($logAText, "历史回传 \d+ 样本")).Count -ge 1)

    # 8. 远端样本注入显示（B 每秒节流日志的累计值取最大）
    $inj = [regex]::Matches($logBText, "样本注入 \d+ 个（累计 (\d+)）")
    $totalInj = 0; foreach ($m in $inj) { $v = [int]$m.Groups[1].Value; if ($v -gt $totalInj) { $totalInj = $v } }
    Check "B 远端样本注入（累计最多 $totalInj 个样本）" ($totalInj -ge 20)
} else {
    Write-Output "SKIP 硬件依赖断言（J-Link 未连接）：采集/写值/历史回传走降级路径"
    Check "写值 ACK 往返（降级路径也应有 ACK 日志）" (([regex]::Matches($logBText, "收到 ACK code=")).Count -ge 1)
}

# 9. 截图验证：B 的波形窗口在数据注入后截图（WM_PRINT 渲染，非空白）
if (Test-Path $shotBmp) {
    $sz = (Get-Item $shotBmp).Length
    Check "B 波形窗口截图存在且非空白（$sz 字节）" ($sz -gt 10000)
} else {
    Check "B 波形窗口截图存在" $false
}

Write-Output "---- A（探针）日志片段 ----"
($logAText -split "`r?`n" | Where-Object { $_ -match "network|采集|ELF|ACK|J-Link|WRITE" } | Select-Object -Last 10) | ForEach-Object { Write-Output $_ }
Write-Output "---- B（远端）日志片段 ----"
($logBText -split "`r?`n" | Where-Object { $_ -match "network|采集|ELF|ACK|样本|历史" } | Select-Object -Last 10) | ForEach-Object { Write-Output $_ }

if ($fail -eq 0) { Write-Output "NET DRIVE ALL PASS"; exit 0 }
Write-Output "NET DRIVE FAILED: $fail"
exit 1
