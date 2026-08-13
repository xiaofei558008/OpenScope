# OpenScope 数值窗口 min/max 列回归（用户反馈功能）：
#   数值窗口增加"最小值/最大值"两列，记录运行过程中变量的极值，
#   并支持手工修改（调试用）。
#   测试：回放正弦数据 -> 数值窗口添加 fsin -> 等待样本流入 ->
#         WM_OS_NUM_TEST_DUMP 钩子逐行日志 -> 断言 min<0 且 max>0（正弦振荡）；
#         再经 WM_OS_NUM_TEST_EDIT 钩子就地编辑 min=2.5 / max=1.5 -> 回车提交 ->
#         断言日志"手工设置"与 min/max 已更新。
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
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public const uint WM_SETTEXT = 0x0C;
    public const uint WM_KEYDOWN = 0x100;
    public const uint WM_CHAR = 0x102;
    public const int VK_RETURN = 0x0D;
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

# 向 EDIT 逐字符输入（WM_CHAR 走真实键盘路径，跨进程 wParam 传值无封送问题；
# WM_SETTEXT 跨进程字符串封送在部分环境下会截断，实测 "2.5" 只送达 "2"）
function Send-Text([IntPtr]$e, [string]$s) {
    foreach ($ch in $s.ToCharArray()) {
        [OsNumMmUi]::SendMessage($e, [OsNumMmUi]::WM_CHAR, [IntPtr]([int]$ch), [IntPtr]0) | Out-Null
    }
}

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

function Log-Has([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern
    return ($null -ne $m -and $m.Count -gt 0)
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
    # 等回放完整播完（7.9s），保证 min/max 已收敛、后续手工修改不被新样本覆盖
    $replayDone = $false
    for ($i = 0; $i -lt 75 -and -not $replayDone; $i++) {
        if (Log-Has '回放结束') { $replayDone = $true; break }
        Start-Sleep -Milliseconds 200
    }
    Check $replayDone "回放完整结束（min/max 已收敛）"
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

        # ---- 手工修改 min（列4）：就地编辑框输入 2.5 -> 回车提交 ----
        $list = Find-ChildByClass $num "SysListView32"
        Check ($list -ne [IntPtr]::Zero) "数值窗口内部 ListView 存在"
        if ($list -ne [IntPtr]::Zero) {
            # WM_OS_NUM_TEST_EDIT = WM_APP+45 = 0x802D（wParam=行, lParam=列4=min）
            [OsNumMmUi]::SendMessage($num, 0x802D, [IntPtr]0, [IntPtr]4) | Out-Null
            Start-Sleep -Milliseconds 300
            $edit = Find-ChildByClass $list "Edit"
            Check ($edit -ne [IntPtr]::Zero) "min 列就地编辑框打开"
            if ($edit -ne [IntPtr]::Zero) {
                Send-Text $edit "2.5"
                Start-Sleep -Milliseconds 100
                [OsNumMmUi]::PostMessage($edit, [OsNumMmUi]::WM_KEYDOWN, [IntPtr]13, [IntPtr]0) | Out-Null  # VK_RETURN
                Start-Sleep -Milliseconds 400
            }
            Check (Log-Has '数值窗口手工设置最小值 行0: 2.5') "手工设置最小值日志（2.5）"
            [OsNumMmUi]::SendMessage($num, 0x802C, [IntPtr]0, [IntPtr]0) | Out-Null
            Start-Sleep -Milliseconds 400
            $mm2 = Log-LastMm '数值minmax: 行0 '
            if ($mm2 -ne $null) {
                Check (([Math]::Abs($mm2[0] - 2.5)) -lt 0.001) "手工修改后 min=2.5（实际 $($mm2[0])）"
            } else {
                Check $false "手工修改后 minmax 日志可解析"
            }

            # ---- 手工修改 max（列5）：输入 1.5 -> 回车提交 ----
            [OsNumMmUi]::SendMessage($num, 0x802D, [IntPtr]0, [IntPtr]5) | Out-Null
            Start-Sleep -Milliseconds 300
            $edit2 = Find-ChildByClass $list "Edit"
            Check ($edit2 -ne [IntPtr]::Zero) "max 列就地编辑框打开"
            if ($edit2 -ne [IntPtr]::Zero) {
                Send-Text $edit2 "1.5"
                Start-Sleep -Milliseconds 100
                [OsNumMmUi]::PostMessage($edit2, [OsNumMmUi]::WM_KEYDOWN, [IntPtr]13, [IntPtr]0) | Out-Null  # VK_RETURN
                Start-Sleep -Milliseconds 400
            }
            Check (Log-Has '数值窗口手工设置最大值 行0: 1.5') "手工设置最大值日志（1.5）"
            [OsNumMmUi]::SendMessage($num, 0x802C, [IntPtr]0, [IntPtr]0) | Out-Null
            Start-Sleep -Milliseconds 400
            $mm3 = Log-LastMm '数值minmax: 行0 '
            if ($mm3 -ne $null) {
                Check (([Math]::Abs($mm3[0] - 2.5)) -lt 0.001) "max 修改不影响 min（仍 2.5，实际 $($mm3[0])）"
                Check (([Math]::Abs($mm3[1] - 1.5)) -lt 0.001) "手工修改后 max=1.5（实际 $($mm3[1])）"
            } else {
                Check $false "max 修改后 minmax 日志可解析"
            }
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
