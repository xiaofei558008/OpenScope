# OpenScope 左侧 elf 变量栏自动隐藏 UI 回归（request.md Bug 12）：
#   1. 开启自动隐藏（WM_OS_TREE_AUTOHIDE=1）-> 光标移出变量栏区域 -> 变量树向左边界隐藏，
#      只留左侧细条 OSTreeStrip
#   2. 光标移回左侧细条（x<=10）-> 变量树自动展开
#   3. 钉住（WM_OS_TREE_AUTOHIDE=0）-> 变量树始终展开
#   4. 全程观察进程是否闪退
# 数据来源：加载 ELF 后变量树即时可用，无需真实 MCU。
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
public class OsTreeHideUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int left, top, right, bottom; }
    [StructLayout(LayoutKind.Sequential)]
    public struct POINT { public int x, y; }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsTreeHideUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsTreeHideUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsTreeHideUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsTreeHideUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsTreeHideUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsTreeHideUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

# 客户端坐标 (cx,cy) -> 屏幕坐标（SetCursorPos 用；避免窗口边框偏移导致 client x 为负）
function Get-ClientScreen([IntPtr]$H, [int]$Cx, [int]$Cy) {
    $pt = New-Object OsTreeHideUi+POINT
    $pt.x = $Cx; $pt.y = $Cy
    [OsTreeHideUi]::ClientToScreen($H, [ref]$pt) | Out-Null
    return $pt
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

    $tree = Find-ChildByClass $main "SysTreeView32"
    Check ($tree -ne [IntPtr]::Zero) "变量树存在"

    $wr = New-Object OsTreeHideUi+RECT
    [OsTreeHideUi]::GetWindowRect($main, [ref]$wr) | Out-Null
    $away = Get-ClientScreen $main 1200 200     # 光标远离：客户区右侧（x >> 变量栏宽）
    $hover = Get-ClientScreen $main 4 200       # 悬停左侧细条：客户区 x=4（细条宽 8）

    # 开启自动隐藏
    [OsTreeHideUi]::SendMessage($main, 0x8009, [IntPtr]1, [IntPtr]0) | Out-Null  # WM_OS_TREE_AUTOHIDE
    Start-Sleep -Milliseconds 300

    # 光标移到右侧空白区（x >> 变量栏宽）等待自动隐藏（>800ms + 200ms tick）
    [OsTreeHideUi]::SetCursorPos($away.x, $away.y) | Out-Null
    Start-Sleep -Milliseconds 2000
    Check (-not [OsTreeHideUi]::IsWindowVisible($tree)) "光标移出后变量树自动隐藏（向左边界收起）"
    $strip = Find-ChildByClass $main "OSTreeStrip"
    Check ($strip -ne [IntPtr]::Zero -and [OsTreeHideUi]::IsWindowVisible($strip)) "左侧细条 OSTreeStrip 可见"
    Check (Log-Has '变量栏自动隐藏') "日志记录自动隐藏"

    # 光标移回左侧细条（客户区 x<=10）-> 自动展开
    [OsTreeHideUi]::SetCursorPos($hover.x, $hover.y) | Out-Null
    Start-Sleep -Milliseconds 800
    Check ([OsTreeHideUi]::IsWindowVisible($tree)) "悬停左侧细条后变量树自动展开"
    Check (Log-Has '变量栏展开') "日志记录变量栏展开"

    # 钉住：开启自动隐藏后再钉住 -> 光标移出也不再隐藏
    [OsTreeHideUi]::SendMessage($main, 0x8009, [IntPtr]1, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200
    [OsTreeHideUi]::SetCursorPos($away.x, $away.y) | Out-Null
    Start-Sleep -Milliseconds 1200
    Check (-not [OsTreeHideUi]::IsWindowVisible($tree)) "自动隐藏模式下再次隐藏"
    [OsTreeHideUi]::SetCursorPos($hover.x, $hover.y) | Out-Null
    Start-Sleep -Milliseconds 800
    Check ([OsTreeHideUi]::IsWindowVisible($tree)) "再次展开"
    [OsTreeHideUi]::SendMessage($main, 0x8009, [IntPtr]0, [IntPtr]0) | Out-Null  # 钉住
    Start-Sleep -Milliseconds 200
    [OsTreeHideUi]::SetCursorPos($away.x, $away.y) | Out-Null
    Start-Sleep -Milliseconds 1500
    Check ([OsTreeHideUi]::IsWindowVisible($tree)) "钉住后光标移出保持展开"

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '变量栏|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
