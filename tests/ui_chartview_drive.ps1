# OpenScope 波形窗口视图功能 UI 回归（request.md N7 + N5）：
#   1. 主界面 MCU 型号下拉存在且默认 Cortex-M4（N5）
#   2. 波形窗口滚轮缩放 X 轴 / Ctrl+滚轮缩放 Y 轴（N7）
#   3. F 键全局显示 / 菜单多坐标轴（Ctrl+B）（N7）
#   4. 停止采集 -> 波形整体展示（N7）
#   5. 全程观察进程是否闪退
# 数据来源：--replay=tests\chart_replay.csv 离线回放（测试钩子），无需真实 MCU。
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$replayCsv = Join-Path $root "tests\chart_replay.csv"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsChartUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr GetDlgItem(IntPtr parent, int id);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", EntryPoint="SendMessage", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessageText(IntPtr h, uint msg, IntPtr wp, StringBuilder lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int left, top, right, bottom; }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsChartUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsChartUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsChartUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsChartUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsChartUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsChartUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
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

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--select-leaf=fsin", "--replay=$replayCsv", "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"

    # N5：MCU 型号下拉
    $devCombo = [OsChartUi]::GetDlgItem($main, 2101)
    Check ($devCombo -ne [IntPtr]::Zero) "MCU 型号下拉存在 (ID 2101)"
    if ($devCombo -ne [IntPtr]::Zero) {
        $dcnt = [OsChartUi]::SendMessage($devCombo, 0x146, [IntPtr]0, [IntPtr]0).ToInt64()  # CB_GETCOUNT
        Check ($dcnt -ge 5) "MCU 型号预置列表 >= 5 项（实际 $dcnt）"
        $dcur = [OsChartUi]::SendMessage($devCombo, 0x147, [IntPtr]0, [IntPtr]0).ToInt64()   # CB_GETCURSEL
        if ($dcur -ge 0) {
            $sb = New-Object System.Text.StringBuilder 128
            [OsChartUi]::SendMessageText($devCombo, 0x148, [IntPtr]$dcur, $sb) | Out-Null  # CB_GETLBTEXT
            Check ($sb.ToString() -eq "Cortex-M4") "MCU 默认型号 = Cortex-M4（实际 $($sb.ToString())）"
        }
    }

    # 创建波形窗口并添加 fsin 变量；等待回放数据流入
    Send-Cmd $main 2012   # IDM_WIN_CHART
    Start-Sleep -Milliseconds 300
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART (选中叶 = fsin)
    Start-Sleep -Milliseconds 2500   # 等待回放样本进入图表
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"
    Check (-not $proc.HasExited) "回放数据注入未闪退"

    # N7：滚轮缩放 X 轴（消息坐标用窗口中心屏幕坐标）
    $cr = New-Object OsChartUi+RECT
    [OsChartUi]::GetWindowRect($chart, [ref]$cr) | Out-Null
    $cx = [int](($cr.left + $cr.right) / 2)
    $cy = [int](($cr.top + $cr.bottom) / 2)
    $lp = ($cx -band 0xFFFF) -bor (($cy -band 0xFFFF) -shl 16)
    $wp = (120 -shl 16)   # delta=+120，向上滚 -> 放大
    [OsChartUi]::SendMessage($chart, 0x20A, [IntPtr]$wp, [IntPtr]$lp) | Out-Null  # WM_MOUSEWHEEL
    Start-Sleep -Milliseconds 300
    Check (Log-Has '波形 X 轴缩放') "滚轮缩放 X 轴"

    # N7：Ctrl+滚轮缩放 Y 轴（低字 MK_CONTROL=0x08）
    $wpY = (120 -shl 16) -bor 0x08
    [OsChartUi]::SendMessage($chart, 0x20A, [IntPtr]$wpY, [IntPtr]$lp) | Out-Null
    Start-Sleep -Milliseconds 300
    Check (Log-Has '波形 Y 轴缩放') "Ctrl+滚轮缩放 Y 轴"

    # N7：F 键全局显示
    [OsChartUi]::SendMessage($chart, 0x100, [IntPtr]0x46, [IntPtr]0) | Out-Null  # WM_KEYDOWN 'F'
    Start-Sleep -Milliseconds 300
    Check (Log-Has '波形全局显示') "F 键全局显示"

    # N7：菜单多坐标轴（等价 Ctrl+B）
    Send-Cmd $chart 3007   # MENU_CHART_MULTIAXIS
    Start-Sleep -Milliseconds 300
    Check (Log-Has '波形多坐标轴: 1') "多坐标轴已启用"

    # N7：停止采集 -> 波形整体展示
    Send-Cmd $main 2005   # IDC_BTN_STOP
    Start-Sleep -Milliseconds 300
    Check (Log-Has '波形整体展示 \(停止采集\)') "停止采集后波形整体展示"

    # 缩放后再次 F 恢复整体
    [OsChartUi]::SendMessage($chart, 0x100, [IntPtr]0x46, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形|MCU|回放|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
