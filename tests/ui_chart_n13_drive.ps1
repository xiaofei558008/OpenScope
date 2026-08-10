# OpenScope 波形窗口分析增强 UI 回归（request.md 新增特性 13b~g）：
#   1. N13b/c: Ctrl+B / 菜单切换逐行堆叠（多坐标轴左置），日志 波形多坐标轴: 1/0
#   2. N13d: 放大后采样点圆点显示（滚轮缩放后进程不闪退）
#   3. N13e: 绘图区两次左键点击 -> 波形测量标记1 + 波形测量Δ（XY 向 Δ 值）
#   4. N13f/g: 鼠标移动 -> 十字光标 + HUD 数值（含变量名/值/类型），进程不闪退
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
public class OsChartN13 {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
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
        [OsChartN13]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsChartN13]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsChartN13]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsChartN13]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsChartN13]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsChartN13]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
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

    # 创建波形窗口并添加 fsin 变量；等待回放数据流入
    Send-Cmd $main 2012   # IDM_WIN_CHART
    Start-Sleep -Milliseconds 300
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART (选中叶 = fsin)
    Start-Sleep -Milliseconds 2500   # 等待回放样本进入图表
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"
    Check (-not $proc.HasExited) "回放数据注入未闪退"

    if ($chart -ne [IntPtr]::Zero) {
        # N13b/c: 菜单切换逐行堆叠（多坐标轴左置）
        Send-Cmd $chart 3007   # MENU_CHART_MULTIAXIS
        Start-Sleep -Milliseconds 300
        Check (Log-Has '波形多坐标轴: 1 \(菜单\)') "堆叠排列已启用 (波形多坐标轴: 1)"
        Send-Cmd $chart 3007
        Start-Sleep -Milliseconds 300
        Check (Log-Has '波形多坐标轴: 0 \(菜单\)') "恢复叠加排列 (波形多坐标轴: 0)"
        Send-Cmd $chart 3007   # 再开启堆叠，用于后续测量
        Start-Sleep -Milliseconds 300

        # 计算绘图区（client 坐标：top=26, left=56, bottom=H-18, right=W-6）
        $cr = New-Object OsChartN13+RECT
        [OsChartN13]::GetClientRect($chart, [ref]$cr) | Out-Null
        $pl = 56; $pt = 26; $pr = $cr.right - 6; $pb = $cr.bottom - 18
        $midY = [int](($pt + $pb) / 2)
        Write-Output "--- plot rect client: L=$pl T=$pt R=$pr B=$pb ---"

        # N13d: 滚轮放大 X 轴（WM_MOUSEWHEEL 用屏幕坐标），放大后采样点以圆点显示（视觉）
        $wr = New-Object OsChartN13+RECT
        [OsChartN13]::GetWindowRect($chart, [ref]$wr) | Out-Null
        $sx = [int](($wr.left + $wr.right) / 2)
        $sy = [int](($wr.top + $wr.bottom) / 2)
        $wl = ($sx -band 0xFFFF) -bor (($sy -band 0xFFFF) -shl 16)
        [OsChartN13]::SendMessage($chart, 0x20A, [IntPtr](120 -shl 16), [IntPtr]$wl) | Out-Null
        Start-Sleep -Milliseconds 300
        Check (Log-Has '波形 X 轴缩放') "滚轮放大 X 轴（采样点圆点模式触发）"

        # N13e: 绘图区两次左键(按下+松开=单击) -> 测量标记 + Δ
        # F23 起测量改为单击：未拖拽时在松开（WM_LBUTTONUP）设置锚点，须发 DOWN+UP 配对（真实点击语义）。
        # 只发 WM_LBUTTONDOWN 不再设置标记（DOWN 现用于记录拖拽起点/框选起点）。
        $x1 = [int]($pl + ($pr - $pl) * 0.25)
        $x2 = [int]($pl + ($pr - $pl) * 0.75)
        $lp1 = ($x1 -band 0xFFFF) -bor (($midY -band 0xFFFF) -shl 16)
        $lp2 = ($x2 -band 0xFFFF) -bor (($midY -band 0xFFFF) -shl 16)
        [OsChartN13]::SendMessage($chart, 0x201, [IntPtr]1, [IntPtr]$lp1) | Out-Null  # DOWN 位置1
        Start-Sleep -Milliseconds 60
        [OsChartN13]::SendMessage($chart, 0x202, [IntPtr]0, [IntPtr]$lp1) | Out-Null  # UP 位置1
        Start-Sleep -Milliseconds 200
        Check (Log-Has '波形测量标记1: t=') "第一击设置测量锚点"
        [OsChartN13]::SendMessage($chart, 0x201, [IntPtr]1, [IntPtr]$lp2) | Out-Null  # DOWN 位置2
        Start-Sleep -Milliseconds 60
        [OsChartN13]::SendMessage($chart, 0x202, [IntPtr]0, [IntPtr]$lp2) | Out-Null  # UP 位置2
        Start-Sleep -Milliseconds 200
        Check (Log-Has '波形测量Δ: ΔX=') "第二击计算 XY 向 Δ 值"

        # N13f/g: 鼠标移动 -> 十字光标 + HUD 数值（变量名/值/类型），不闪退
        $lp3 = (($midY) -band 0xFFFF) -bor (($midY -band 0xFFFF) -shl 16)
        [OsChartN13]::SendMessage($chart, 0x200, [IntPtr]0, [IntPtr]$lp3) | Out-Null  # WM_MOUSEMOVE
        Start-Sleep -Milliseconds 200
        Check (-not $proc.HasExited) "光标/HUD 渲染未闪退"
    }

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形|测量|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
