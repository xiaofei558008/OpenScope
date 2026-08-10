# 需求21：连接时不得弹出 "Target device setting" / "Device Selection" 芯片型号设置弹窗。
# 检查窗口：首次连接（全新 DLL）+ 断开重连（重载 DLL）均无任何 J-Link/设备/型号对话框。
# 退出码：0 = 无弹窗（通过），非 0 = 检测到弹窗（失败）。
param([string]$ExePath = "D:/OpenScope/bin/Release/OpenScope.exe")
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsDlgW {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
function Get-WindowsByPid([int]$ProcId) {
    $script:list = New-Object System.Collections.ArrayList
    $cb = { param($h,$l) $wp=[uint32]0; [OsDlgW]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256
            [OsDlgW]::GetClassName($h,$sb,256)|Out-Null
            $tsb=New-Object System.Text.StringBuilder 256
            [OsDlgW]::GetWindowText($h,$tsb,256)|Out-Null
            [void]$script:list.Add([pscustomobject]@{ h=$h; cls=$sb.ToString(); title=$tsb.ToString() }) }
        return $true }
    [OsDlgW]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null
    return $script:list
}
function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsDlgW]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256
            [OsDlgW]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsDlgW]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
# 需求21：匹配 "Target device setting" / "Device Selection" / J-Link 目标设置等设备/型号对话框
function Find-DeviceDialog([int]$ProcId) {
    $ws = Get-WindowsByPid $ProcId
    foreach ($w in $ws) {
        if ($w.cls -eq "#32770" -and ($w.title -match "Target|Device|J-Link|JLink|Chip|Select")) {
            return $w
        }
    }
    return $null
}
$root = Split-Path -Parent $PSScriptRoot
$exe = $ExePath
$proc = Start-Process -FilePath $exe -ArgumentList @("D:/OpenScope/tests/linix_stm32l031_v1.2.out") -PassThru
$main = [IntPtr]::Zero
for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
    $main = Find-ByClass $proc.Id "OpenScopeMain"
    if ($main -ne [IntPtr]::Zero) { break }
    Start-Sleep -Milliseconds 200
}
if ($main -eq [IntPtr]::Zero) { Write-Output "FAIL main window"; if (-not $proc.HasExited) { $proc.Kill() }; exit 1 }
Start-Sleep -Milliseconds 1500
$fail = 0

Write-Output "--- 1) 首次连接（全新 DLL，无重载） ---"
[OsDlgW]::SendMessage($main, 0x111, [IntPtr]2002, [IntPtr]0) | Out-Null   # CONNECT
Start-Sleep -Milliseconds 3000
$d1 = Find-DeviceDialog $proc.Id
if ($d1) { Write-Output "DIALOG FIRST-CONNECT: '$($d1.title)' cls=$($d1.cls)"; $fail = 1 } else { Write-Output "PASS first connect: no device dialog" }

Write-Output "--- 2) 断开后重连（触发 DLL 重载路径） ---"
[OsDlgW]::SendMessage($main, 0x111, [IntPtr]2003, [IntPtr]0) | Out-Null   # DISCONNECT
Start-Sleep -Milliseconds 1500
[OsDlgW]::SendMessage($main, 0x111, [IntPtr]2002, [IntPtr]0) | Out-Null   # CONNECT
Start-Sleep -Milliseconds 3000
$d2 = Find-DeviceDialog $proc.Id
if ($d2) { Write-Output "DIALOG RECONNECT: '$($d2.title)' cls=$($d2.cls)"; $fail = 1 } else { Write-Output "PASS reconnect: no device dialog" }

Write-Output "--- windows at end ---"
Get-WindowsByPid $proc.Id | ForEach-Object { Write-Output ("  {0} | '{1}'" -f $_.cls, $_.title) }
if (-not $proc.HasExited) { $proc.Kill() }
if ($fail) { Write-Output "FAIL: device-selection dialog detected"; exit 1 }
Write-Output "PASS ALL: no Target/Device/Selection dialog on first-connect or reconnect"
exit 0
