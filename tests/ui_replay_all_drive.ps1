# OpenScope 回放全量加载回归（长时间采集落盘 CSV 的"全部显示"）：
#   1. --replay-all=<csv> 启动即全量加载（2000 行 > 65536 环？2000 < 65536，
#      但桶缓存路径全走一遍）：日志"回放加载完成: ... （N 行，M 个变量桶缓存）"
#   2. 新建波形窗口 + 添加 fsin → 系列自动挂接桶缓存：日志"波形桶缓存: fsin N 桶"
#   3. WM_OS_CHART_SHOT 渲染 BMP → 像素级验证曲线实际绘制（包络渲染）
#   4. 进程不闪退
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$replayCsv = Join-Path $root "tests\chart_replay_long.csv"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsReplayAllUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsReplayAllUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsReplayAllUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsReplayAllUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsReplayAllUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsReplayAllUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsReplayAllUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

function Log-Has([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern
    return ($null -ne $m -and $m.Count -gt 0)
}

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) {
    Write-Output "ERROR: OpenScope already running: $($existing.Id -join ',')"
    exit 3
}

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--select-leaf=fsin", "--replay-all=$replayCsv", "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"

    # ---- 1. 全量加载完成（2000 行，2 变量列映射） ----
    $loadDone = $false
    for ($i = 0; $i -lt 50 -and -not $loadDone; $i++) {
        if (Log-Has '回放加载完成') { $loadDone = $true; break }
        Start-Sleep -Milliseconds 200
    }
    Check $loadDone "回放加载完成（全量解析 + 桶缓存）"
    # 该 ELF 只有 fsin 有对应变量（fDeg 列无匹配叶，跳过映射）
    Check (Log-Has '回放加载完成.*2000 行，1 个变量桶缓存') "2000 行 1 变量桶缓存日志"

    # ---- 2. 新建波形窗口 + 添加 fsin → 自动挂接桶 ----
    Send-Cmd $main 2012   # IDM_WIN_CHART
    Start-Sleep -Milliseconds 400
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART
    Start-Sleep -Milliseconds 800
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"
    Check (Log-Has '波形桶缓存: fsin 2000 桶') "fsin 系列自动挂接 2000 桶缓存"

    # ---- 3. 渲染验证：桶包络曲线实际绘制（像素级） ----
    if ($chart -ne [IntPtr]::Zero) {
        $shotPath = Join-Path (Split-Path $exe) "chart_shot.bmp"
        if (Test-Path $shotPath) { Remove-Item -LiteralPath $shotPath -Force }
        [OsReplayAllUi]::SendMessage($chart, 0x8000 + 42, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_OS_CHART_SHOT
        Start-Sleep -Milliseconds 400
        $curvePixels = 0
        if (Test-Path $shotPath) {
            Add-Type -AssemblyName System.Drawing
            $bmp = [System.Drawing.Bitmap]::FromFile($shotPath)
            for ($sx = 60; $sx -lt ($bmp.Width - 12); $sx += 2) {
                for ($sy = 30; $sy -lt ($bmp.Height - 40); $sy += 2) {
                    $c = $bmp.GetPixel($sx, $sy)
                    if ($c.R -gt 100 -or $c.G -gt 100 -or $c.B -gt 100) { $curvePixels++ }
                }
            }
            $bmp.Dispose()
            Check ($curvePixels -gt 100) "全量桶包络曲线实际渲染（$curvePixels 个亮色采样像素）"
        } else {
            Check $false "chart_shot.bmp 生成"
        }
    }

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '回放|桶|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
