# OpenScope 同 tab 多窗口同时更新 UI 回归（request.md Bug 19）：
#   Bug19: 同一个标签页的多个窗口，总是只有第一个窗口更新/绘图——
#          os_ds_drain 只推送到 wins[i].hwnd（group[0]），group[1..] 收不到样本。
#   验证：
#     1. 同 tab 建 2 个波形窗口，分别添加不同变量（g_counter / g_cfg.a）
#     2. --replay 回放 tile_multi_update.csv（200 行 50ms 间隔）
#     3. WM_OS_CHART_QUERY 钩子查询两个窗口：series_count==1 且 series[0].count>0
#        （修复前第二个窗口 count==0）
#     4. 全程进程不闪退
param(
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$elf = Join-Path $root "tests\elf_sample.out"
$csv = Join-Path $root "tests\tile_multi_update.csv"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Collections.Generic;
public class OsMultiUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    public const uint WM_COMMAND = 0x111;
    public const uint WM_OS_WIN_ADD_VAR = 0x8023;    // WM_APP+35
    public const uint WM_OS_CHART_QUERY = 0x8029;    // WM_APP+41
    public const int IDM_WIN_CHART = 2012;
    public const int IDM_TAB_ADD_CHART = 2503;
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsMultiUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsMultiUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsMultiUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildrenByClass([IntPtr]$Parent, [string]$Class) {
    $script:lst = New-Object System.Collections.Generic.List[IntPtr]
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsMultiUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:lst.Add($h) }
        return $true
    }
    [OsMultiUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:lst
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

$proc = Start-Process -FilePath $exe -ArgumentList @($elf, "--no-layout", "--replay=$csv") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 600

    # 同 tab 两个波形窗口
    [OsMultiUi]::SendMessage($main, [OsMultiUi]::WM_COMMAND, [IntPtr]([OsMultiUi]::IDM_WIN_CHART), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    [OsMultiUi]::SendMessage($main, [OsMultiUi]::WM_COMMAND, [IntPtr]([OsMultiUi]::IDM_TAB_ADD_CHART), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    $charts = Find-ChildrenByClass $main "OSChartWin"
    Check ($charts.Count -eq 2) "同 tab 两个波形窗口（实际 $($charts.Count)）"
    if ($charts.Count -ne 2) { throw "无法继续" }

    # 分别添加不同变量（elf_sample.out 叶：0=g_counter, 1=g_cfg.a, 2=g_cfg.b, 3=g_raw）
    [OsMultiUi]::SendMessage($main, [OsMultiUi]::WM_OS_WIN_ADD_VAR, $charts[0], [IntPtr]0) | Out-Null
    [OsMultiUi]::SendMessage($main, [OsMultiUi]::WM_OS_WIN_ADD_VAR, $charts[1], [IntPtr]1) | Out-Null
    Start-Sleep -Milliseconds 400
    Check ((Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形窗口添加变量: id=0' | Measure-Object).Count -ge 1) "窗口A 添加 g_counter"
    Check ((Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形窗口添加变量: id=1' | Measure-Object).Count -ge 1) "窗口B 添加 g_cfg.a"

    # 等回放推送样本（200 行 x 50ms，窗口 1.5s 内建成，样本充足）
    Start-Sleep -Milliseconds 1500
    $r0 = [OsMultiUi]::SendMessage($charts[0], [OsMultiUi]::WM_OS_CHART_QUERY, [IntPtr]0, [IntPtr]0).ToInt32()
    $r1 = [OsMultiUi]::SendMessage($charts[1], [OsMultiUi]::WM_OS_CHART_QUERY, [IntPtr]0, [IntPtr]0).ToInt32()
    $s0 = ($r0 -shr 16) -band 0xFFFF; $c0 = $r0 -band 0xFFFF
    $s1 = ($r1 -shr 16) -band 0xFFFF; $c1 = $r1 -band 0xFFFF
    Write-Output "窗口A: 系列=$s0 点数=$c0；窗口B: 系列=$s1 点数=$c1"
    Check (($s0 -eq 1) -and ($c0 -gt 0)) "窗口A 收到样本并绘图（点数 $c0）"
    Check (($s1 -eq 1) -and ($c1 -gt 0)) "窗口B 同时收到样本并绘图（点数 $c1）——Bug19 修复验证"

    Check (-not $proc.HasExited) "进程未闪退"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
