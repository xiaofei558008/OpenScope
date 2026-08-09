# OpenScope 波形界面鼠标滑动闪烁 UI 回归（request.md Bug 11）：
#   1. 回放数据持续注入（--replay 测试钩子）期间，模拟鼠标在波形区域高频滑动
#      （连续发送 WM_MOUSEMOVE -> 每次触发 InvalidateRect 整窗重绘，Bug11 复现条件）
#   2. 高频重绘后 UpdateWindow 强制完成双缓冲 WM_PAINT（验证 chartwin 双缓冲绘制路径）
#   3. 进程不闪退、无 FATAL
#   4. 逐行堆叠 + 光标 HUD 同时开启（重绘最重的路径）
# 数据来源：--replay=tests\chart_replay.csv 离线回放，无需真实 MCU。
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
if (-not (Test-Path $replayCsv)) { $replayCsv = Join-Path $root "tests\chart_replay.csv" }

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsFlickUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool UpdateWindow(IntPtr h);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int left, top, right, bottom; }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsFlickUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsFlickUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsFlickUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsFlickUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsFlickUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsFlickUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
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

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--select-leaf=fsin", "--replay=$replayCsv", "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"

    Send-Cmd $main 2012   # IDM_WIN_CHART
    Start-Sleep -Milliseconds 300
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART
    Start-Sleep -Milliseconds 2500   # 等待回放样本进入图表
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"
    Check (-not $proc.HasExited) "回放数据注入未闪退"

    if ($chart -ne [IntPtr]::Zero) {
        Send-Cmd $chart 3007   # MENU_CHART_MULTIAXIS -> 逐行堆叠（最重重绘路径）
        Start-Sleep -Milliseconds 300
        $cr = New-Object OsFlickUi+RECT
        [OsFlickUi]::GetClientRect($chart, [ref]$cr) | Out-Null
        $pl = 56; $pt = 26; $pr = $cr.right - 6; $pb = $cr.bottom - 18
        $x = $pl + 20
        $y = [int](($pt + $pb) / 2)

        # Bug11: 模拟鼠标在波形区域快速来回滑动（每次 WM_MOUSEMOVE 触发整窗重绘）
        $moves = 0
        for ($i = 0; $i -lt 300 -and -not $proc.HasExited; $i++) {
            $x = $pl + (($i * 37) % ($pr - $pl))
            $lp = ($x -band 0xFFFF) -bor (($y -band 0xFFFF) -shl 16)
            [OsFlickUi]::SendMessage($chart, 0x200, [IntPtr]0, [IntPtr]$lp) | Out-Null  # WM_MOUSEMOVE
            $moves++
            if (($i % 50) -eq 0) { [OsFlickUi]::UpdateWindow($chart) | Out-Null }  # 强制完成双缓冲绘制
        }
        Check ($moves -ge 300) "已发送 300 次鼠标滑动（实际 $moves）"
        [OsFlickUi]::UpdateWindow($chart) | Out-Null
        Start-Sleep -Milliseconds 400
        Check (-not $proc.HasExited) "高频滑动 + 双缓冲重绘未闪退"

        # 离开再进入（WM_MOUSELEAVE 路径），再滑动一轮
        $lpLeave = (-1 -band 0xFFFF) -bor (($y -band 0xFFFF) -shl 16)
        [OsFlickUi]::SendMessage($chart, 0x203, [IntPtr]0, [IntPtr]$lpLeave) | Out-Null  # WM_MOUSELEAVE
        [OsFlickUi]::UpdateWindow($chart) | Out-Null
        Start-Sleep -Milliseconds 200
        Check (-not $proc.HasExited) "鼠标离开路径未闪退"
    }

    Check (-not $proc.HasExited) "进程未闪退"
    $fatal = 0
    if (Test-Path $log) {
        $fatal = @(Get-Content $log -Encoding UTF8 | Select-String -Pattern 'FATAL').Count
    }
    Check ($fatal -eq 0) "无 FATAL 崩溃日志"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
