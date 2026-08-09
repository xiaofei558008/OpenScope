# Bug7 回归：采集进行中点击"记录"弹出保存文件对话框，界面不得卡死。
#   1. 启动 OpenScope（--layout-load 预建数值窗口观测 fsin）
#   2. 连接 J-Link -> 开始采集（采集线程运行中）
#   3. 点击"记录"(2006) -> 保存对话框(#32770) 出现，进程存活
#   4. 取消对话框 -> 校验对话框关闭、进程存活、采集自动恢复（日志"采集已开始"再次出现）
#   5. PostMessage 断开 -> 日志"已断开连接"出现（主线程未被卡死）
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
function LogCount([string]$Pattern) {
    if (-not (Test-Path $log)) { return 0 }
    return @(Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch $Pattern).Count
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

    # 连接（默认 Cortex-M4 / 4000 kHz）
    [OsB7Ui]::SendMessage($main, 0x111, [IntPtr]2002, [IntPtr]0) | Out-Null  # IDC_BTN_CONNECT
    Check (Wait-Log "已连接: " 10000) "连接 J-Link 成功（日志）"

    # 开始采集
    [OsB7Ui]::SendMessage($main, 0x111, [IntPtr]2004, [IntPtr]0) | Out-Null  # IDC_BTN_START
    Check (Wait-Log "采集已开始" 6000) "采集已开始（日志）"
    $startLogCount = LogCount "采集已开始"

    # 点击"记录" -> 应弹出保存文件对话框（Bug7：此前此处卡死）。
    # 注意必须 PostMessage：cmd_log_start 内 GetSaveFileNameW 是模态循环，
    # SendMessage 会阻塞到对话框关闭，脚本就无法去探测对话框了。
    [OsB7Ui]::PostMessage($main, 0x111, [IntPtr]2006, [IntPtr]0) | Out-Null  # IDC_BTN_LOGSTART
    $dlg = [IntPtr]::Zero
    for ($i = 0; $i -lt 30 -and $dlg -eq [IntPtr]::Zero -and -not $proc.HasExited; $i++) {
        $dlg = Find-ByClass $proc.Id "#32770"
        if ($dlg -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($dlg -ne [IntPtr]::Zero) "Bug7 点击"记录"弹出保存文件对话框"
    Check (-not $proc.HasExited) "Bug7 对话框弹出时进程未退出"

    # 取消对话框
    if ($dlg -ne [IntPtr]::Zero) {
        [OsB7Ui]::SendMessage($dlg, 0x111, [IntPtr]2, [IntPtr]0) | Out-Null  # IDCANCEL
        $dlgGone = $false
        for ($i = 0; $i -lt 30 -and -not $proc.HasExited; $i++) {
            if ((Find-ByClass $proc.Id "#32770") -eq [IntPtr]::Zero) { $dlgGone = $true; break }
            Start-Sleep -Milliseconds 200
        }
        Check $dlgGone "Bug7 取消后对话框关闭"
    }
    Check (-not $proc.HasExited) "Bug7 取消后进程存活（界面未卡死）"

    # 采集应自动恢复（cmd_log_start 对话框关闭后 os_ds_start 恢复采集）
    Start-Sleep -Milliseconds 800
    $resumed = (LogCount "采集已开始") -gt $startLogCount
    Check $resumed "Bug7 取消后采集自动恢复（再次"采集已开始"）"

    # 响应性：断开操作能被主线程处理（未被卡死）
    [OsB7Ui]::PostMessage($main, 0x111, [IntPtr]2003, [IntPtr]0) | Out-Null  # IDC_BTN_DISCON
    Check (Wait-Log "已断开连接" 5000) "Bug7 断开操作正常处理（主线程响应）"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
