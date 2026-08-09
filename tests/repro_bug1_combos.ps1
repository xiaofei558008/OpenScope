# Repro Bug 1: 添加 2 个变量后 App 异常退出 —— 多场景综合复现
# 场景A: 树右键添加 fsin + 对话框添加 fDeg 到同一波形窗口
# 场景B: 数值窗口 + 对话框添加 2 个变量
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = "",
    [string]$ReplayCsv = "D:\OpenScope\tests\chart_replay2.csv"
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
public class Repro3 {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", EntryPoint="SendMessage", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessageW(IntPtr h, uint msg, IntPtr wp, string lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    public const uint LB_SETCURSEL = 0x0186, LB_GETCOUNT = 0x018B;
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h, $l)
        $wp = [uint32]0
        [Repro3]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [Repro3]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [Repro3]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}
function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [Repro3]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [Repro3]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}
function Send-Cmd([IntPtr]$H, [int]$Id) {
    [Repro3]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

# 通过对话框添加变量到 hwnd（MENU_ADD=id）
function Add-ViaDialog([IntPtr]$hwnd, [int]$menuAdd, [string]$name, [int]$ProcId) {
    [Repro3]::PostMessage($hwnd, 0x0111, [IntPtr]$menuAdd, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 600
    $dlg = Find-ByClass $ProcId "OSDlgPick"
    if ($dlg -eq [IntPtr]::Zero) { Write-Output "  FAIL: OSDlgPick not found"; return $false }
    $script:edith = [IntPtr]::Zero
    $cb2 = { param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [Repro3]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq "Edit") { $script:edith = $h; return $false }
        return $true
    }
    [Repro3]::EnumChildWindows($dlg, $cb2, [IntPtr]::Zero) | Out-Null
    if ($script:edith -eq [IntPtr]::Zero) { Write-Output "  FAIL: edit not found"; return $false }
    [Repro3]::SendMessageW($script:edith, 0x000C, [IntPtr]0, $name) | Out-Null
    Start-Sleep -Milliseconds 400
    $script:listh = [IntPtr]::Zero
    $cb3 = { param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [Repro3]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq "ListBox") { $script:listh = $h; return $false }
        return $true
    }
    [Repro3]::EnumChildWindows($dlg, $cb3, [IntPtr]::Zero) | Out-Null
    $cnt = [Repro3]::SendMessage($script:listh, 0x018B, [IntPtr]0, [IntPtr]0).ToInt64()
    if ($cnt -le 0) { Write-Output "  FAIL: no match for '$name' (cnt=$cnt)"; [Repro3]::SendMessage($dlg, 0x0010, [IntPtr]0, [IntPtr]0) | Out-Null; return $false }
    [Repro3]::SendMessage($script:listh, 0x0186, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 100
    [Repro3]::SendMessage($dlg, 0x0111, [IntPtr]2401, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 400
    return $true
}

$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) { Write-Output "ERROR: OpenScope already running"; exit 3 }
$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--select-leaf=fsin", "--replay=$ReplayCsv", "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
$fails = 0
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    if ($main -eq [IntPtr]::Zero) { Write-Output "FAIL main"; exit 1 }
    Start-Sleep -Milliseconds 800   # 等 ELF 加载完毕
    if ($proc.HasExited) { Write-Output "CRASH after startup"; $fails++; }

    # 场景A: 波形窗口，var1 树右键添加 (fsin 已选中)，var2 对话框添加
    Send-Cmd $main 2012
    Start-Sleep -Milliseconds 400
    $chart = Find-ChildByClass $main "OSChartWin"
    Write-Output "chart=$($chart)"
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART -> 添加 fsin
    Start-Sleep -Milliseconds 600
    Write-Output "scenarioA: tree-add fsin, exited=$($proc.HasExited)"
    if ($proc.HasExited) { Write-Output "CRASH A1"; $fails++ }
    if ($chart -ne [IntPtr]::Zero) {
        Add-ViaDialog $chart 3001 "fDeg" $proc.Id | Out-Null
        Write-Output "scenarioA: dialog-add fDeg, exited=$($proc.HasExited)"
        if ($proc.HasExited) { Write-Output "CRASH A2"; $fails++ }
    }
    Start-Sleep -Milliseconds 2000
    if ($proc.HasExited) { Write-Output "CRASH A3 (delayed)"; $fails++ }

    # 场景B: 数值窗口，对话框添加 2 个变量
    if (-not $proc.HasExited) {
        Send-Cmd $main 2013   # IDM_WIN_NUM
        Start-Sleep -Milliseconds 400
        $num = Find-ChildByClass $main "OSNumWin"
        Write-Output "num=$($num)"
        if ($num -ne [IntPtr]::Zero) {
            Add-ViaDialog $num 3101 "fsin" $proc.Id | Out-Null
            Add-ViaDialog $num 3101 "fDeg" $proc.Id | Out-Null
            Write-Output "scenarioB: num 2 vars, exited=$($proc.HasExited)"
            if ($proc.HasExited) { Write-Output "CRASH B"; $fails++ }
        }
        Start-Sleep -Milliseconds 2000
        if ($proc.HasExited) { Write-Output "CRASH B2 (delayed)"; $fails++ }
    }

    if ($fails -gt 0) {
        Write-Output "CRASH REPRODUCED ($fails scenarios)"
        if (Test-Path $log) { Get-Content $log -Encoding UTF8 | Select-Object -Last 30 }
        exit 4
    }
    Write-Output "NO CRASH (all scenarios)"
    exit 0
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
