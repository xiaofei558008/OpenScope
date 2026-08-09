# OpenScope 消息栏上下拉伸 UI 回归（request.md 新增特性 14）：
#   F14: 拖动横向分隔条（OSSplitterH）改变消息栏高度 log_h，随后进程不闪退。
#   验证：默认高度 -> 拖到 ~100px -> 消息栏 ListView 高度跟随 -> 拖到 ~220px 同样跟随。
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsSplitUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int left, top, right, bottom; }
    [StructLayout(LayoutKind.Sequential)] public struct POINT { public int x, y; }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsSplitUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsSplitUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsSplitUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsSplitUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsSplitUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
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

# 拖动横向分隔条：把消息栏高度改为 TargetLogH（通过设置光标到目标 client y 再发 WM_MOUSEMOVE）
function Drag-LogSplitter([IntPtr]$Main, [IntPtr]$Splitter, [int]$TargetLogH) {
    $cr = New-Object OsSplitUi+RECT
    [OsSplitUi]::GetClientRect($Main, [ref]$cr) | Out-Null
    $y = $cr.bottom - 34 - 27 - $TargetLogH   # log_h = rc.bottom - 61 - 分隔条 client y
    $pt = New-Object OsSplitUi+POINT
    $pt.x = 60; $pt.y = $y
    [OsSplitUi]::ClientToScreen($Main, [ref]$pt) | Out-Null
    [OsSplitUi]::SetCursorPos($pt.x, $pt.y) | Out-Null
    [OsSplitUi]::SendMessage($Splitter, 0x201, [IntPtr]1, [IntPtr]0) | Out-Null   # WM_LBUTTONDOWN MK_LBUTTON
    Start-Sleep -Milliseconds 80
    [OsSplitUi]::SendMessage($Splitter, 0x200, [IntPtr]1, [IntPtr]0) | Out-Null   # WM_MOUSEMOVE（读 GetCursorPos）
    Start-Sleep -Milliseconds 200
    [OsSplitUi]::SendMessage($Splitter, 0x202, [IntPtr]0, [IntPtr]0) | Out-Null   # WM_LBUTTONUP
    Start-Sleep -Milliseconds 120
}

function Get-LogHeight([IntPtr]$Main) {
    $lv = Find-ChildByClass $Main "SysListView32"
    if ($lv -eq [IntPtr]::Zero) { return -1 }
    $wr = New-Object OsSplitUi+RECT
    [OsSplitUi]::GetWindowRect($lv, [ref]$wr) | Out-Null
    return ($wr.bottom - $wr.top)
}

$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) {
    Write-Output "ERROR: OpenScope already running: $($existing.Id -join ',')"
    exit 3
}

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"

    $splitter = Find-ChildByClass $main "OSSplitterH"
    Check ($splitter -ne [IntPtr]::Zero) "横向分隔条 OSSplitterH 存在"

    $h0 = Get-LogHeight $main
    Check ($h0 -ge 120 -and $h0 -le 260) "初始消息栏高度合理（实际 $h0）"

    if ($splitter -ne [IntPtr]::Zero) {
        Drag-LogSplitter $main $splitter 100
        $h1 = Get-LogHeight $main
        Check ([Math]::Abs($h1 - 100) -le 20) "拖到 ~100px 生效（实际 $h1）"

        Drag-LogSplitter $main $splitter 220
        $h2 = Get-LogHeight $main
        Check ([Math]::Abs($h2 - 220) -le 20) "拖到 ~220px 生效（实际 $h2）"

        Check (-not $proc.HasExited) "拖动后进程未闪退"
    }
    Check (-not $proc.HasExited) "进程未闪退"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
