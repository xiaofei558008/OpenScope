# OpenScope 右键新建窗口 UI 回归（request.md 新增特性 16）：
#   右侧空白处右键 / tab 标签条空白处右键 -> 弹出"新建波形/数值/示波器窗口"菜单。
#   跨进程无法点击菜单项（TrackPopupMenu 为应用线程内模态），故验证：
#   A. 对 OSRightPanel 发 WM_RBUTTONUP -> 进程出现 #32768 弹出菜单窗口
#   B. 对 SysTabControl32 发 WM_RBUTTONUP -> 同样出现弹出菜单
#   C. WM_CANCELMODE 关闭菜单、菜单窗口消失、进程不闪退
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
using System.Collections.Generic;
public class OsRMenu {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsRMenu]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsRMenu]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsRMenu]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsRMenu]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsRMenu]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

# 统计该进程拥有的 #32768（弹出菜单/下拉）窗口；返回句柄列表
function Get-PopupMenus([int]$ProcId) {
    $script:list = New-Object System.Collections.Generic.List[IntPtr]
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsRMenu]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 64
            [OsRMenu]::GetClassName($h, $sb, 64) | Out-Null
            if ($sb.ToString() -eq '#32768') { $script:list.Add($h) }
        }
        return $true
    }
    [OsRMenu]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:list
}

function Wait-Popup([int]$ProcId, [int]$BaseCount, [int]$MsTimeout) {
    for ($i = 0; $i -lt ($MsTimeout / 100); $i++) {
        $m = Get-PopupMenus $ProcId
        if ($m.Count -gt $BaseCount) { return $m }
        Start-Sleep -Milliseconds 100
    }
    return $null
}

function Wait-NoPopup([int]$ProcId, [int]$BaseCount, [int]$MsTimeout) {
    for ($i = 0; $i -lt ($MsTimeout / 100); $i++) {
        $m = Get-PopupMenus $ProcId
        if ($m.Count -le $BaseCount) { return $true }
        Start-Sleep -Milliseconds 100
    }
    return $false
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
    $right = Find-ChildByClass $main "OSRightPanel"
    $tab = Find-ChildByClass $main "SysTabControl32"
    Check ($right -ne [IntPtr]::Zero) "右侧面板 OSRightPanel 存在"
    Check ($tab -ne [IntPtr]::Zero) "Tab 控件存在"

    $base = (Get-PopupMenus $proc.Id).Count
    Write-Output "--- 初始 #32768 窗口数: $base ---"

    # A. 右侧面板空白处右键
    [OsRMenu]::PostMessage($right, 0x205, [IntPtr]0x0002, [IntPtr]0) | Out-Null  # WM_RBUTTONUP MK_RBUTTON
    $m1 = Wait-Popup $proc.Id $base 4000
    Check ($null -ne $m1) "A: 右侧空白右键弹出新建窗口菜单"
    if ($null -ne $m1) {
        Start-Sleep -Milliseconds 300
        # 关闭菜单：WM_CANCELMODE 给菜单所有者（主窗口）
        [OsRMenu]::SendMessage($main, 0x001F, [IntPtr]0, [IntPtr]0) | Out-Null
        if (-not (Wait-NoPopup $proc.Id $base 3000)) {
            # 兜底：向弹出菜单窗口发 ESC
            $mm = Get-PopupMenus $proc.Id
            foreach ($h in $mm) { if ($h -ne [IntPtr]::Zero) { [OsRMenu]::PostMessage($h, 0x100, [IntPtr]0x1B, [IntPtr]0) | Out-Null } }
            Wait-NoPopup $proc.Id $base 3000 | Out-Null
        }
        $m1b = Get-PopupMenus $proc.Id
        Check ($m1b.Count -le $base) "A: 菜单已关闭"
        Check (-not $proc.HasExited) "A: 关闭菜单后进程未闪退"
    }

    # B. tab 标签条空白处右键（无窗口时整个标签条都是空白）
    [OsRMenu]::SetCursorPos(50, 60) | Out-Null
    Start-Sleep -Milliseconds 100
    [OsRMenu]::PostMessage($tab, 0x205, [IntPtr]0x0002, [IntPtr]0) | Out-Null
    $m2 = Wait-Popup $proc.Id $base 4000
    if ($null -eq $m2) {
        # 部分 tab 控件在 WM_RBUTTONDOWN 时发 NM_RCLICK
        [OsRMenu]::PostMessage($tab, 0x204, [IntPtr]0x0002, [IntPtr]0) | Out-Null  # WM_RBUTTONDOWN
        $m2 = Wait-Popup $proc.Id $base 3000
    }
    Check ($null -ne $m2) "B: tab 空白处右键弹出新建窗口菜单"
    if ($null -ne $m2) {
        Start-Sleep -Milliseconds 300
        [OsRMenu]::SendMessage($main, 0x001F, [IntPtr]0, [IntPtr]0) | Out-Null
        if (-not (Wait-NoPopup $proc.Id $base 3000)) {
            $mm = Get-PopupMenus $proc.Id
            foreach ($h in $mm) { if ($h -ne [IntPtr]::Zero) { [OsRMenu]::PostMessage($h, 0x100, [IntPtr]0x1B, [IntPtr]0) | Out-Null } }
            Wait-NoPopup $proc.Id $base 3000 | Out-Null
        }
        $m2b = Get-PopupMenus $proc.Id
        Check ($m2b.Count -le $base) "B: 菜单已关闭"
        Check (-not $proc.HasExited) "B: 关闭菜单后进程未闪退"
    }

    Check (-not $proc.HasExited) "进程未闪退"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
