# OpenScope 配置对话框 UI 驱动（复现用户操作序列）：
#   1. 启动 bin\Release\OpenScope.exe
#   2. 主窗口点“连接”（IDC_BTN_CONNECT=2002）
#   3. 对话框输入目标器件名（IDC_DEVICE=1003，默认 STM32L432KB）
#   4. 点“连接”（IDC_CONNECT=1006）
#   5. 观察进程是否退出 / 是否有崩溃 MessageBox，并把结果写到 stdout
#
# 用法: powershell -ExecutionPolicy Bypass -File module\jlink\tests\ui_connect_drive.ps1 [-Device STM32L432KB]
param(
    [string]$Device = "STM32L432KB",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class OsUi {
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr FindWindow(string cls, string title);
    [DllImport("user32.dll")] public static extern IntPtr GetDlgItem(IntPtr h, int id);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern bool SetWindowText(IntPtr h, string t);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, System.Text.StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, System.Text.StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern int GetDlgCtrlID(IntPtr h);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildById([IntPtr]$Parent, [int]$Id) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        if ([OsUi]::GetDlgCtrlID($h) -eq $Id) { $script:hit = $h; return $false }
        return $true
    }
    [OsUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Get-Text([IntPtr]$Hwnd) {
    if ($Hwnd -eq [IntPtr]::Zero) { return "" }
    $sb = New-Object System.Text.StringBuilder 512
    [OsUi]::GetWindowText($Hwnd, $sb, 512) | Out-Null
    return $sb.ToString()
}

$root = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
if ($ExePath) {
    $exe = $ExePath
} else {
    $exe = Join-Path $root "bin\Release\OpenScope.exe"
}
if (-not (Test-Path $exe)) { Write-Error "not found: $exe"; exit 2 }

$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) {
    Write-Output "ERROR: OpenScope already running: $($existing.Id -join ',')"
    exit 3
}

$proc = Start-Process -FilePath $exe -PassThru
Write-Output "started pid=$($proc.Id)"

try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    if ($main -eq [IntPtr]::Zero) { Write-Output "MAINWINDOW NOT FOUND"; exit 1 }
    Write-Output "main hwnd=$main"

    # 点“连接”打开配置对话框（PostMessage，避免被对话框模态循环阻塞）
    [OsUi]::PostMessage($main, 0x111, [IntPtr]2002, [IntPtr]0) | Out-Null
    $dlg = [IntPtr]::Zero
    for ($i = 0; $i -lt 50; $i++) {
        $dlg = Find-ByClass $proc.Id "OSJLinkCfg"
        if ($dlg -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    if ($dlg -eq [IntPtr]::Zero) { Write-Output "CONFIG DIALOG NOT FOUND"; exit 1 }
    Write-Output "dlg hwnd=$dlg"

    # 输入器件名并点“连接”（用 EnumChildWindows 找控件，跨进程更稳）
    $ed = Find-ChildByClass $dlg "Edit"
    for ($i = 0; $i -lt 25 -and $ed -eq [IntPtr]::Zero; $i++) {
        Start-Sleep -Milliseconds 100
        $ed = Find-ChildByClass $dlg "Edit"
    }
    if ($ed -eq [IntPtr]::Zero) { Write-Output "DEVICE EDIT NOT READY"; exit 1 }
    [OsUi]::SetWindowText($ed, $Device) | Out-Null
    Write-Output "device set: $Device readback='$(Get-Text $ed)'"
    Start-Sleep -Milliseconds 200

    # 复现扫描不一致：点 5 次“刷新”，每次读状态栏文本
    for ($i = 0; $i -lt 5; $i++) {
        [OsUi]::PostMessage($dlg, 0x111, [IntPtr]1005, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 700
        $st = Find-ChildById $dlg 1008
        $status = Get-Text $st
        $list = Find-ChildByClass $dlg "ListBox"
        $count = [OsUi]::SendMessage($list, 0x18B, [IntPtr]0, [IntPtr]0)  # LB_GETCOUNT
        Write-Output "refresh #$i -> status='$status' list_count=$count"
    }

    [OsUi]::PostMessage($dlg, 0x111, [IntPtr]1006, [IntPtr]0) | Out-Null
    Write-Output "connect clicked"

    $dismissed = @{}
    for ($i = 0; $i -lt 150; $i++) {
        foreach ($t in @("OpenScope 崩溃", "J-Link", "J-Link 连接失败", "J-Link 扫描")) {
            if ($dismissed[$t]) { continue }
            $mb = [OsUi]::FindWindow("#32770", $t)
            if ($mb -ne [IntPtr]::Zero) {
                Write-Output "MSGBOX: $t"
                [OsUi]::SendMessage($mb, 0x111, [IntPtr]1, [IntPtr]0) | Out-Null
                $dismissed[$t] = $true
            }
        }
        if ($proc.HasExited) {
            Write-Output "PROCESS EXITED rc=$($proc.ExitCode)"
            break
        }
        Start-Sleep -Milliseconds 200
    }
    if (-not $proc.HasExited) {
        Write-Output "PROCESS STILL ALIVE"
        # 关闭配置对话框
        [OsUi]::PostMessage($dlg, 0x111, [IntPtr]1, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 500
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output "done"
