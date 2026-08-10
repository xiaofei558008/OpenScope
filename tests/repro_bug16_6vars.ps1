# Repro Bug16: 添加 6 个变量之后闪退（着重内存管理）
# 用 WM_OS_TREE_TEST_SELECT(0x800C) 选中前 N 个叶子 -> IDM_TREE_ADD_CHART(2305) 批量添加
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = "",
    [int]$AddCount = 6,
    [switch]$Connect    # 是否连接 J-Link 并开始采集（触发采集路径）
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
public class Bug16Ui {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [Bug16Ui]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256
            [Bug16Ui]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [Bug16Ui]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Send-Cmd([IntPtr]$H, [int]$Id) {
    [Bug16Ui]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}
$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) { Write-Output "ERROR: OpenScope already running: $($existing.Id -join ',')"; exit 3 }
$args = @($Elf, "--no-layout")
if ($Connect) { $args += "--layout-load=D:/OpenScope/tests/f21_verify.ini" }
$proc = Start-Process -FilePath $exe -ArgumentList $args -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    if ($main -eq [IntPtr]::Zero) { Write-Output "FAIL main window"; exit 1 }
    Start-Sleep -Milliseconds 1200

    Send-Cmd $main 2012   # 新建波形窗口
    Start-Sleep -Milliseconds 400
    # 选中前 AddCount 个叶子并批量添加
    $sel = [Bug16Ui]::SendMessage($main, 0x800C, [IntPtr]0, [IntPtr]$AddCount).ToInt64()
    Write-Output "selected leaves=$sel (requested $AddCount)"
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART
    Start-Sleep -Milliseconds 1500
    Write-Output "after chart add, exited=$($proc.HasExited)"

    # 数值窗口再加一批
    Send-Cmd $main 2013   # 新建数值窗口 (ID?)
    Start-Sleep -Milliseconds 400
    Send-Cmd $main 2306   # IDM_TREE_ADD_NUM
    Start-Sleep -Milliseconds 1500
    Write-Output "after num add, exited=$($proc.HasExited)"

    if ($Connect) {
        Send-Cmd $main 2002   # CONNECT
        Start-Sleep -Milliseconds 2500
        Send-Cmd $main 2004   # START
        Start-Sleep -Milliseconds 4000
        Send-Cmd $main 2005   # STOP
        Start-Sleep -Milliseconds 1200
        Send-Cmd $main 2003   # DISCONNECT
        Start-Sleep -Milliseconds 800
        Write-Output "after acq cycle, exited=$($proc.HasExited)"
    }

    Start-Sleep -Milliseconds 2000
    Write-Output "final exited=$($proc.HasExited) exitcode=$(if ($proc.HasExited) { $proc.ExitCode } else { 'N/A' })"
    if ($proc.HasExited) {
        Write-Output "CRASH REPRODUCED: exit code $($proc.ExitCode)"
        if (Test-Path $log) { Get-Content $log -Encoding UTF8 | Select-Object -Last 40 }
        exit 4
    }
    Write-Output "NO CRASH (add $AddCount)"
    exit 0
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
