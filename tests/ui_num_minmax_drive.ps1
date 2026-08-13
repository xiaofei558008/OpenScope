# OpenScope 数值窗口 min/max 列回归（用户反馈功能）：
#   数值窗口增加"最小值/最大值"两列，记录运行过程中变量的极值。
#   测试：回放正弦数据 -> 数值窗口添加 fsin -> 等待样本流入 ->
#         WM_OS_NUM_TEST_DUMP 钩子逐行日志 -> 断言 min<0 且 max>0（正弦振荡）。
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$replayCsv = Join-Path $root "tests\chart_replay.csv"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsNumMmUi {
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
        [OsNumMmUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsNumMmUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsNumMmUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsNumMmUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsNumMmUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsNumMmUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

function Log-LastMm([string]$Pattern) {
    if (-not (Test-Path $log)) { return $null }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern | Select-Object -Last 1
    if ($null -eq $m) { return $null }
    if ($m.Line -match "min=(-?[\d.eE+-]+) max=(-?[\d.eE+-]+)") {
        return @([double]$Matches[1], [double]$Matches[2])
    }
    return $null
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

    Send-Cmd $main 2013   # IDM_WIN_NUM 新建数值窗口
    Start-Sleep -Milliseconds 300
    Send-Cmd $main 2306   # IDM_TREE_ADD_NUM 添加选中叶(fsin)到数值窗口
    Start-Sleep -Milliseconds 2500   # 等待回放样本流入
    $num = Find-ChildByClass $main "OSNumWin"
    Check ($num -ne [IntPtr]::Zero) "数值窗口已创建"

    if ($num -ne [IntPtr]::Zero) {
        # WM_OS_NUM_TEST_DUMP = WM_APP+44 = 0x802C：逐行日志输出 min/max
        [OsNumMmUi]::SendMessage($num, 0x802C, [IntPtr]0, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 400
        $mm = Log-LastMm '数值minmax: 行0 '
        if ($mm -ne $null) {
            # fsin 正弦在 -1..1 振荡：min<0 且 max>0 且 min<=max
            Check ($mm[0] -lt 0) "最小值列记录到负值（min=$($mm[0])）"
            Check ($mm[1] -gt 0) "最大值列记录到正值（max=$($mm[1])）"
            Check ($mm[0] -le $mm[1]) "min <= max"
        } else {
            Check $false "数值minmax 日志可解析"
        }
    }

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '数值|minmax|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
