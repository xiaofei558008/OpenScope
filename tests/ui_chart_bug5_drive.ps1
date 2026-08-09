# OpenScope 波形圆点 Bug5 回归：
#   "波形放大后采样点圆点没有实现（一开始有，随着录制时间变长全部消失）"
#   旧代码按缓冲区总点数判定圆点（npts<=120），录制越长越不满足；
#   修复后按可见时间窗 [x0,x1] 内实际点数判定。
#   测试：用 2000 采样点的长回放灌满缓冲 -> 滚轮放大 15 次 ->
#        日志出现 "波形采样点圆点: 可见 N 点"（即放大后圆点仍显示）+ 不闪退。
#   另外验证波形内部标题已去掉：进程不闪退、波形窗口正常渲染。
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
public class OsBug5 {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int left, top, right, bottom; }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsBug5]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsBug5]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsBug5]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsBug5]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsBug5]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsBug5]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
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
    Start-Sleep -Milliseconds 250
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART (选中叶 = fsin)
    Start-Sleep -Milliseconds 2000   # 等待 2000 采样点回放灌入缓冲
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"
    Check (Log-Has '回放结束') "长回放（2000 点）已灌满缓冲"
    Check (-not $proc.HasExited) "长回放注入未闪退"

    if ($chart -ne [IntPtr]::Zero) {
        # 滚轮放大 X 轴（WM_MOUSEWHEEL 用屏幕坐标，每步 0.8 倍，15 步 -> 可见点 ~70 个）
        $cr = New-Object OsBug5+RECT
        [OsBug5]::GetClientRect($chart, [ref]$cr) | Out-Null
        $wr = New-Object OsBug5+RECT
        [OsBug5]::GetWindowRect($chart, [ref]$wr) | Out-Null
        $sx = [int](($wr.left + $wr.right) / 2)
        $sy = [int]($wr.top + [int](($cr.bottom - 26 + 18) / 2))
        $wl = ($sx -band 0xFFFF) -bor (($sy -band 0xFFFF) -shl 16)
        for ($i = 0; $i -lt 15; $i++) {
            [OsBug5]::SendMessage($chart, 0x20A, [IntPtr](120 -shl 16), [IntPtr]$wl) | Out-Null
            Start-Sleep -Milliseconds 60
        }
        # 重绘 + 日志落盘可能受回归环境负载影响，最多等待 3s（200ms 步进）
        $t0 = Get-Date
        while (-not (Log-Has '波形 X 轴缩放') -and ((Get-Date) - $t0).TotalMilliseconds -lt 3000) {
            Start-Sleep -Milliseconds 200
        }
        Start-Sleep -Milliseconds 200
        Check (Log-Has '波形 X 轴缩放') "滚轮放大 X 轴执行"
        $t0 = Get-Date
        while (-not (Log-Has '波形采样点圆点') -and ((Get-Date) - $t0).TotalMilliseconds -lt 3000) {
            Start-Sleep -Milliseconds 200
        }
        Check (Log-Has '波形采样点圆点') "放大后采样点圆点仍显示（可见点计数 <= 120）"
        Check (-not $proc.HasExited) "缩放+圆点渲染未闪退"
    }

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '回放|圆点|缩放|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
