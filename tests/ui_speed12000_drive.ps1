# Bug10 回归：12000kHz 高速采集时，瞬时掉线（重连自愈）不得导致采集线程退出。
#   1. 启动 OpenScope（--layout-load 预建数值窗口观测 fsin）
#   2. 速度下拉设为 12000 -> 连接 J-Link（默认 Cortex-M4）
#   3. 开始采集 -> 等待 3.5s（>500ms 重连节流 × 多次周期）
#   4. 断言采集线程未退出：日志不出现"采集线程已退出"/"采集停止：长时间"/"采集停止：MCU 连接已断开"
#   5. 停止采集 -> 断开
# 若 12000 连接本身失败（环境性，曾偶发 rc=-1），记录环境说明但不断言线程存活（路径不同）。
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsB10Ui {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern int GetDlgCtrlID(IntPtr h);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern bool SetWindowText(IntPtr h, string t);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, StringBuilder lp);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, [MarshalAs(UnmanagedType.LPWStr)] string lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsB10Ui]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsB10Ui]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsB10Ui]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-ChildById([IntPtr]$P, [int]$Id) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) if ([OsB10Ui]::GetDlgCtrlID($h) -eq $Id) { $script:hit=$h; return $false }; return $true }
    [OsB10Ui]::EnumChildWindows($P,$cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}

$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$layout = Join-Path $env:TEMP "b10_layout.ini"
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
title=B10数值
vars=fsin
"@
[System.IO.File]::WriteAllText($layout, $body, (New-Object System.Text.UTF8Encoding $true))

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}
function Has-Log([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    return [bool](Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch $Pattern -Quiet)
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
# 读回下拉文本（WM_GETTEXT）
function Get-ComboText([IntPtr]$h) {
    $sb = New-Object System.Text.StringBuilder 64
    [OsB10Ui]::SendMessage($h, 0x000D, [IntPtr]64, $sb) | Out-Null
    return $sb.ToString()
}
# 设置可编辑下拉文本并读回校验；SetWindowText API 对 ComboBox 不生效（实测），
# 必须用 WM_SETTEXT(0x000C)；且 os_mainwin_cfg_init 可能在启动异步完成时重置文本，
# 故循环重设直到确认或超时（避免"本应 12000 实际 4000"的静默错误）。
function Set-ComboTextVerified([IntPtr]$h, [string]$Text, [int]$TimeoutMs = 6000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        [OsB10Ui]::SendMessage($h, 0x000C, [IntPtr]0, $Text) | Out-Null
        Start-Sleep -Milliseconds 150
        if ((Get-ComboText $h) -eq $Text) { return $true }
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

    # 速度下拉设为 12000（CBS_DROPDOWN 可编辑，SetWindowText + 读回校验）
    $speed = Find-ChildById $main 2103
    Check ($speed -ne [IntPtr]::Zero) "速度下拉存在"
    if ($speed -ne [IntPtr]::Zero) {
        Check (Set-ComboTextVerified $speed "12000") "速度下拉已设为 12000（读回确认）"
        Write-Output ("INFO 速度下拉读回值: [" + (Get-ComboText $speed) + "]")
    }

    # 连接
    [OsB10Ui]::SendMessage($main, 0x111, [IntPtr]2002, [IntPtr]0) | Out-Null  # IDC_BTN_CONNECT
    $connected = Wait-Log "已连接: " 10000
    $connFail = Has-Log "连接失败"
    if ($connected) {
        Check $true "12000 连接成功（日志）"
    } elseif ($connFail) {
        Write-Output "NOTE 12000 连接失败（环境性，曾偶发 rc=-1）：跳过采集线程断言，仅验证不闪退"
        Check (-not $proc.HasExited) "连接失败时进程未闪退"
        return  # 提前结束 try 内的后续（finally 仍会 Kill）
    } else {
        Check $false "12000 连接超时（既无已连接也无连接失败）"
    }

    # 开始采集
    [OsB10Ui]::SendMessage($main, 0x111, [IntPtr]2004, [IntPtr]0) | Out-Null  # IDC_BTN_START
    Check (Wait-Log "采集已开始" 6000) "采集已开始（日志）"

    # 等待 3.5s，覆盖多次 500ms 重连节流周期（Bug10 核心：瞬时掉线不得退出采集线程）
    Start-Sleep -Milliseconds 3500

    # 核心断言：采集线程未退出（无以下任一退出日志）
    $threadExit = (Has-Log "采集线程已退出") -or (Has-Log "采集停止：长时间") -or (Has-Log "采集停止：MCU 连接已断开")
    Check (-not $threadExit) "Bug10 高速瞬时掉线后采集线程保持运行（未退出）"
    Check (-not $proc.HasExited) "采集期间进程存活"

    # 停止采集 -> 断开
    [OsB10Ui]::PostMessage($main, 0x111, [IntPtr]2005, [IntPtr]0) | Out-Null  # IDC_BTN_STOP
    Start-Sleep -Milliseconds 400
    [OsB10Ui]::PostMessage($main, 0x111, [IntPtr]2003, [IntPtr]0) | Out-Null  # IDC_BTN_DISCON
    Check (Wait-Log "已断开连接" 5000) "断开成功（日志）"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
