# 录制按键回归：可先录再采；再点停止 → 询问是否保存 → 选路径。
#   采集进行中停录弹对话框时界面不得卡死（原 Bug7）。
#   1. 连接 J-Link -> 点「录制」-> 开始采集
#   2. 再点「录制」(停止) -> MessageBox「是否保存」出现，进程存活
#   3. 选「否」-> 对话框关闭、进程存活、主线程可断开
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsB7Ui {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsB7Ui]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsB7Ui]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsB7Ui]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-MsgBox([int]$ProcId) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsB7Ui]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) {
            $cls=New-Object System.Text.StringBuilder 64; [OsB7Ui]::GetClassName($h,$cls,64)|Out-Null
            if ($cls.ToString() -eq "#32770") {
                $tt=New-Object System.Text.StringBuilder 128; [OsB7Ui]::GetWindowText($h,$tt,128)|Out-Null
                if ($tt.ToString() -eq "录制") { $script:hit=$h; return $false }
            }
        }; return $true }
    [OsB7Ui]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}

$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$layout = Join-Path $env:TEMP "b7_layout.ini"
$body = @"
[layout]
version=1
main_x=100
main_y=100
main_w=1200
main_h=700
tree_w=340
log_h=170
active=0
wins=1
[win]
type=num
title=B7数值
vars=fsin
"@
[System.IO.File]::WriteAllText($layout, $body, (New-Object System.Text.UTF8Encoding $true))

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}
function Wait-Log([string]$Pattern, [int]$TimeoutMs = 8000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        if (Test-Path $log) {
            if (Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch $Pattern -Quiet) { return $true }
        }
        Start-Sleep -Milliseconds 200
    }
    return $false
}

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--layout-load=$layout") -PassThru
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 1500

    [OsB7Ui]::SendMessage($main, 0x111, [IntPtr]2002, [IntPtr]0) | Out-Null  # CONNECT
    Check (Wait-Log "已连接: " 10000) "连接 J-Link 成功（日志）"

    # 先录制再开始采集
    [OsB7Ui]::SendMessage($main, 0x111, [IntPtr]2006, [IntPtr]0) | Out-Null  # LOGSTART 开录
    Check (Wait-Log "录制已开启" 4000) "先点录制（可先于采集）"

    [OsB7Ui]::SendMessage($main, 0x111, [IntPtr]2004, [IntPtr]0) | Out-Null  # START
    Check (Wait-Log "采集已开始" 6000) "采集已开始（日志）"
    Check (Wait-Log "采集落盘: RAM 缓冲" 4000) "录制开启后采集建立临时缓存"
    Start-Sleep -Milliseconds 1500

    # 再点录制 = 停止 → 询问是否保存（PostMessage，避免模态阻塞脚本）
    [OsB7Ui]::PostMessage($main, 0x111, [IntPtr]2006, [IntPtr]0) | Out-Null
    $dlg = [IntPtr]::Zero
    for ($i = 0; $i -lt 40 -and $dlg -eq [IntPtr]::Zero -and -not $proc.HasExited; $i++) {
        $dlg = Find-MsgBox $proc.Id
        if ($dlg -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($dlg -ne [IntPtr]::Zero) "停止录制弹出「是否保存」对话框"
    Check (-not $proc.HasExited) "对话框弹出时进程未退出"

    if ($dlg -ne [IntPtr]::Zero) {
        # IDNO = 7
        [OsB7Ui]::SendMessage($dlg, 0x111, [IntPtr]7, [IntPtr]0) | Out-Null
        $dlgGone = $false
        for ($i = 0; $i -lt 30 -and -not $proc.HasExited; $i++) {
            if ((Find-MsgBox $proc.Id) -eq [IntPtr]::Zero) { $dlgGone = $true; break }
            Start-Sleep -Milliseconds 200
        }
        Check $dlgGone "选「否」后对话框关闭"
    }
    Check (-not $proc.HasExited) "取消保存后进程存活（界面未卡死）"

    [OsB7Ui]::PostMessage($main, 0x111, [IntPtr]2003, [IntPtr]0) | Out-Null  # DISCON
    Check (Wait-Log "已断开连接" 5000) "断开操作正常处理（主线程响应）"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
