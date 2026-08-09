# Repro Bug 1: 添加 2 个变量后 App 异常退出（通过窗口右键"添加变量..."模糊搜索对话框）
# 启动 -> 建波形窗口 -> 用对话框添加 fsin -> 再用对话框添加 fDeg -> 观察崩溃
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = "",
    [string]$ReplayCsv = "D:\OpenScope\tests\chart_replay2.csv",
    [string[]]$Vars = @("fsin", "fDeg")
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
public class Repro2 {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", EntryPoint="SendMessage", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessageW(IntPtr h, uint msg, IntPtr wp, string lp);
    [DllImport("user32.dll")] public static extern IntPtr SendMessageT(IntPtr h, uint msg, IntPtr wp, StringBuilder lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool GetWindowText(IntPtr h, StringBuilder sb, int max);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    public const uint WM_SETTEXT = 0x000C, WM_COMMAND = 0x0111, WM_CLOSE = 0x0010;
    public const uint LB_SETCURSEL = 0x0186, LB_GETCURSEL = 0x0188, LB_GETCOUNT = 0x018B;
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h, $l)
        $wp = [uint32]0
        [Repro2]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [Repro2]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [Repro2]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}
function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [Repro2]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [Repro2]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}
function Send-Cmd([IntPtr]$H, [int]$Id) {
    [Repro2]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) { Write-Output "ERROR: OpenScope already running"; exit 3 }
$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--select-leaf=fsin", "--replay=$ReplayCsv", "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    if ($main -eq [IntPtr]::Zero) { Write-Output "FAIL main window"; exit 1 }
    Write-Output "main=$($main)"

    # 建波形窗口
    Send-Cmd $main 2012
    Start-Sleep -Milliseconds 500
    $chart = Find-ChildByClass $main "OSChartWin"
    Write-Output "chart=$($chart)"
    if ($chart -eq [IntPtr]::Zero) { Write-Output "FAIL chart window"; exit 1 }

    # 通过对话框添加变量
    function Add-VarViaDialog([string]$name) {
        # 对话框是模态的：用 PostMessage 打开，避免 SendMessage 在模态循环内阻塞
        [Repro2]::PostMessage($chart, 0x0111, [IntPtr]3001, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 600
        $dlg = Find-ByClass $proc.Id "OSDlgPick"
        if ($dlg -eq [IntPtr]::Zero) { Write-Output "  FAIL: OSDlgPick not found for '$name'"; return $false }
        $edit = [Repro2]::SendMessage($dlg, 0x0111, [IntPtr]0, [IntPtr]0)  # no-op
        $edith = $null
        # 找对话框内的 EDIT 子控件
        $cb2 = { param($h, $l)
            $sb = New-Object System.Text.StringBuilder 128
            [Repro2]::GetClassName($h, $sb, 128) | Out-Null
            if ($sb.ToString() -eq "Edit") { $script:edith = $h; return $false }
            return $true
        }
        $script:edith = [IntPtr]::Zero
        [Repro2]::EnumChildWindows($dlg, $cb2, [IntPtr]::Zero) | Out-Null
        if ($script:edith -eq [IntPtr]::Zero) { Write-Output "  FAIL: edit not found in dlg"; return $false }
        # 输入名称 -> EN_CHANGE 刷新列表
        [Repro2]::SendMessageW($script:edith, 0x000C, [IntPtr]0, $name) | Out-Null
        Start-Sleep -Milliseconds 400
        # 找到 LISTBOX 并选第 0 项
        $script:listh = [IntPtr]::Zero
        $cb3 = { param($h, $l)
            $sb = New-Object System.Text.StringBuilder 128
            [Repro2]::GetClassName($h, $sb, 128) | Out-Null
            if ($sb.ToString() -eq "ListBox") { $script:listh = $h; return $false }
            return $true
        }
        [Repro2]::EnumChildWindows($dlg, $cb3, [IntPtr]::Zero) | Out-Null
        $cnt = [Repro2]::SendMessage($script:listh, 0x018B, [IntPtr]0, [IntPtr]0).ToInt64()
        Write-Output "  dialog '$name': listCount=$cnt"
        if ($cnt -le 0) { [Repro2]::SendMessage($dlg, 0x0010, [IntPtr]0, [IntPtr]0) | Out-Null; return $false }
        [Repro2]::SendMessage($script:listh, 0x0186, [IntPtr]0, [IntPtr]0) | Out-Null  # LB_SETCURSEL 0
        Start-Sleep -Milliseconds 100
        # 点确定 (IDD_PICK_OK=2401)
        [Repro2]::SendMessage($dlg, 0x0111, [IntPtr]2401, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 300
        Write-Output "  dialog '$name' OK, exited=$($proc.HasExited)"
        return $true
    }

    $i = 1
    foreach ($v in $Vars) {
        Write-Output "adding var$i = $v"
        Add-VarViaDialog $v | Out-Null
        if ($proc.HasExited) {
            Write-Output "CRASH during add var$i ($v): exitcode=$($proc.ExitCode)"
            if (Test-Path $log) { Get-Content $log -Encoding UTF8 | Select-Object -Last 25 }
            exit 4
        }
        $i++
    }

    Start-Sleep -Milliseconds 3000
    if ($proc.HasExited) {
        Write-Output "CRASH REPRODUCED: exit code $($proc.ExitCode)"
        if (Test-Path $log) { Get-Content $log -Encoding UTF8 | Select-Object -Last 25 }
        exit 4
    }
    Write-Output "NO CRASH (2 vars added)"
    exit 0
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
