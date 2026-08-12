# OpenScope tab 内窗口列宽拖拽 + 最小化/还原 UI 回归（request.md Bug 6 补齐）：
#   Bug6: tab 内部窗口要支持最大化、最小化、鼠标拉伸边沿，一个 tab 装多个窗口。
#   （最大化/多窗口由 N11 实现并有 ui_features_drive 覆盖；本测试覆盖补齐部分）
#   验证：
#     1. 同 tab 两窗口平铺，列间 6px 分隔带：拖拽 +80px -> 左列增宽/右列减宽 + 日志"列宽调整"
#     2. 菜单命令"最小化/还原当前窗口"(IDM_TAB_MINIMIZE=2507) -> 一个窗口隐藏 + 日志"窗口最小化"
#     3. 点击 tab 底部最小化条按钮 -> 窗口还原可见 + 日志"窗口还原"
#     4. 测试钩子 WM_OS_WIN_MINIMIZE(WM_APP+34) 切换最小化/还原
#     5. 全程进程不闪退
param(
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
public struct RECT { public int Left, Top, Right, Bottom; }
public class OsTileUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT rc);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT rc);
    [DllImport("user32.dll")] public static extern bool ScreenToClient(IntPtr h, ref System.Drawing.Point p);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    public const uint WM_COMMAND = 0x111;
    public const uint WM_LBUTTONDOWN = 0x201;
    public const uint WM_LBUTTONUP = 0x202;
    public const uint WM_MOUSEMOVE = 0x200;
    public const uint WM_OS_WIN_MINIMIZE = 0x8022;  // WM_APP+34
    public const int IDM_WIN_CHART = 2012;
    public const int IDM_TAB_ADD_CHART = 2503;
    public const int IDM_TAB_MINIMIZE = 2507;
}
"@ -ReferencedAssemblies System.Drawing

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsTileUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsTileUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsTileUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildrenByClass([IntPtr]$Parent, [string]$Class) {
    $script:lst = New-Object System.Collections.Generic.List[IntPtr]
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsTileUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:lst.Add($h) }
        return $true
    }
    [OsTileUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:lst
}

function Get-WinRect([IntPtr]$h) {
    $rc = New-Object RECT
    [OsTileUi]::GetWindowRect($h, [ref]$rc) | Out-Null
    return $rc
}

function Count-LogLines([string]$Pattern) {
    if (-not (Test-Path $log)) { return 0 }
    return (Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern | Measure-Object).Count
}
function Log-Has([string]$Pattern) { return ((Count-LogLines $Pattern) -gt 0) }

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

$proc = Start-Process -FilePath $exe -ArgumentList @("--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 500

    # 同 tab 两个波形窗口：先建 tab，再"在当前标签添加波形窗口"
    [OsTileUi]::SendMessage($main, [OsTileUi]::WM_COMMAND, [IntPtr]([OsTileUi]::IDM_WIN_CHART), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 350
    [OsTileUi]::SendMessage($main, [OsTileUi]::WM_COMMAND, [IntPtr]([OsTileUi]::IDM_TAB_ADD_CHART), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 350

    $tab = [IntPtr]::Zero
    $tabs = Find-ChildrenByClass $main "SysTabControl32"  # EnumChildWindows 递归全部后代
    if ($tabs.Count -gt 0) { $tab = $tabs[0] }
    Check ($tab -ne [IntPtr]::Zero) "找到 tab 控件"

    $charts = Find-ChildrenByClass $tab "OSChartWin"
    Check ($charts.Count -eq 2) "同 tab 两个波形窗口（实际 $($charts.Count)）"
    if ($charts.Count -ne 2) { throw "无法继续" }

    $r0 = Get-WinRect $charts[0]; $r1 = Get-WinRect $charts[1]
    if ($r1.Left -lt $r0.Left) { $tmpR = $r0; $r0 = $r1; $r1 = $tmpR; $tmpH = $charts[0]; $charts[0] = $charts[1]; $charts[1] = $tmpH }
    $w0 = $r0.Right - $r0.Left; $w1 = $r1.Right - $r1.Left
    Write-Output "平铺初始: w0=$w0 w1=$w1 gap=$($r1.Left - $r0.Right)"
    Check ([math]::Abs($w0 - $w1) -le 3) "初始列宽均分（w0=$w0 w1=$w1）"
    Check (($r1.Left - $r0.Right) -ge 4 -and ($r1.Left - $r0.Right) -le 8) "列间分隔带约 6px"

    # 1) 拖拽分隔带 +80px
    $gapScreenX = ($r0.Right + $r1.Left) / 2
    $gapScreenY = ($r0.Top + $r0.Bottom) / 2
    $pt = New-Object System.Drawing.Point ([int]$gapScreenX), ([int]$gapScreenY)
    [OsTileUi]::ScreenToClient($tab, [ref]$pt) | Out-Null
    $gx = $pt.X; $gy = $pt.Y
    [OsTileUi]::SendMessage($tab, [OsTileUi]::WM_LBUTTONDOWN, [IntPtr]1, [IntPtr]($gx -bor ($gy -shl 16))) | Out-Null
    Start-Sleep -Milliseconds 150
    $gx2 = $gx + 80
    [OsTileUi]::SendMessage($tab, [OsTileUi]::WM_MOUSEMOVE, [IntPtr]1, [IntPtr]($gx2 -bor ($gy -shl 16))) | Out-Null
    Start-Sleep -Milliseconds 250
    [OsTileUi]::SendMessage($tab, [OsTileUi]::WM_LBUTTONUP, [IntPtr]0, [IntPtr]($gx2 -bor ($gy -shl 16))) | Out-Null
    Start-Sleep -Milliseconds 350
    $r0b = Get-WinRect $charts[0]; $r1b = Get-WinRect $charts[1]
    $w0b = $r0b.Right - $r0b.Left; $w1b = $r1b.Right - $r1b.Left
    Write-Output "拖拽后: w0=$w0b w1=$w1b"
    Check ([math]::Abs(($w0 + 80) - $w0b) -le 4) "拖拽 +80px 左列增宽（$w0 -> $w0b）"
    Check ([math]::Abs(($w1 - 80) - $w1b) -le 4) "拖拽 +80px 右列减宽（$w1 -> $w1b）"
    Check (Log-Has '列宽调整: tab0') "日志记录列宽调整"

    # 2) 菜单命令最小化当前窗口（g_cur_win 空则首个）
    [OsTileUi]::SendMessage($main, [OsTileUi]::WM_COMMAND, [IntPtr]([OsTileUi]::IDM_TAB_MINIMIZE), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 400
    $v0 = [OsTileUi]::IsWindowVisible($charts[0]); $v1 = [OsTileUi]::IsWindowVisible($charts[1])
    Check ((-not $v0) -and $v1) "菜单最小化：一个窗口隐藏另一个保留（v0=$v0 v1=$v1）"
    Check (Log-Has '窗口最小化: tab0') "日志记录窗口最小化"

    # 3) 点击 tab 底部最小化条按钮 -> 还原
    # 注意：不能跨进程 SendMessage TCM_ADJUSTRECT（>=WM_USER 的消息不封送指针，
    # comctl32 会在被测进程里解引用测试进程的指针 -> 0xC0000005）。
    # 按钮几何：x=pr.left+4+85（pr.left≈2 -> 89），y=pr.bottom-13（pr.bottom≈H-2 -> H-15）。
    $crc = New-Object RECT
    [OsTileUi]::GetClientRect($tab, [ref]$crc) | Out-Null
    $btnX = 89
    $btnY = $crc.Bottom - 15
    Write-Output "最小化条按钮中心: ($btnX,$btnY)（tab 客户区，H=$($crc.Bottom)）"
    [OsTileUi]::SendMessage($tab, [OsTileUi]::WM_LBUTTONDOWN, [IntPtr]1, [IntPtr]($btnX -bor ($btnY -shl 16))) | Out-Null
    Start-Sleep -Milliseconds 400
    $v0c = [OsTileUi]::IsWindowVisible($charts[0]); $v1c = [OsTileUi]::IsWindowVisible($charts[1])
    Check ($v0c -and $v1c) "点击最小化条按钮：两窗口均还原可见"
    Check (Log-Has '窗口还原: tab0') "日志记录窗口还原"

    # 4) 测试钩子 WM_OS_WIN_MINIMIZE 切换（作用于第二个窗口句柄）
    [OsTileUi]::SendMessage($main, [OsTileUi]::WM_OS_WIN_MINIMIZE, $charts[1], [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    Check (-not [OsTileUi]::IsWindowVisible($charts[1])) "钩子最小化第二个窗口"
    [OsTileUi]::SendMessage($main, [OsTileUi]::WM_OS_WIN_MINIMIZE, $charts[1], [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    Check ([OsTileUi]::IsWindowVisible($charts[1])) "钩子还原第二个窗口"

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '列宽调整|窗口最小化|窗口还原|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
