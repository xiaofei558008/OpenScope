# OpenScope 波形窗口 F23 UI 回归（request.md 新增特性 23）：
#   1. 滚轮缩放连续化：验证缩放目标日志（不再步进 0.8/1.25，改用连续 factor + 平滑动画）
#   2. 左键拖拽平移视图：LBUTTONDOWN -> 位移超阈值 -> MOUSEMOVE -> 平移结束日志
#   3. Ctrl+左键框选局部放大：LBUTTONDOWN(Ctrl) -> MOUSEMOVE -> LBUTTONUP -> 框选缩放日志
#   4. 全程观察进程是否闪退
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
public class OsChartF23Ui {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int cmd);
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
        [OsChartF23Ui]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsChartF23Ui]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsChartF23Ui]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsChartF23Ui]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsChartF23Ui]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClassRetry([IntPtr]$Parent, [string]$Class) {
    $deadline = (Get-Date).AddSeconds(5)
    while ((Get-Date) -lt $deadline) {
        $h = Find-ChildByClass $Parent $Class
        if ($h -ne [IntPtr]::Zero) { return $h }
        Start-Sleep -Milliseconds 200
    }
    return $h
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsChartF23Ui]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

function Log-Has([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern
    return ($null -ne $m -and $m.Count -gt 0)
}

# 提取日志中最后一次匹配行的数字对（X=[a,b] 形式）
function Log-LastRange([string]$Pattern) {
    if (-not (Test-Path $log)) { return $null }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern
    if ($null -eq $m -or $m.Count -eq 0) { return $null }
    $line = $m[$m.Count - 1].Line
    if ($line -match '\[(-?\d+),(-?\d+)\]') { return @([long]$matches[1], [long]$matches[2]) }
    return $null
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

    Send-Cmd $main 2012   # IDM_WIN_CHART 新建波形窗口
    Start-Sleep -Milliseconds 300
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART 添加选中叶(fsin)
    Start-Sleep -Milliseconds 2500   # 等待回放样本流入
    $chart = Find-ChildByClassRetry $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"
    Check (-not $proc.HasExited) "回放数据注入未闪退"

    # 窗口客户区中心坐标（滚轮/鼠标消息用客户区坐标，lParam 低16位=x 高16位=y）
    $cr = New-Object OsChartF23Ui+RECT
    [OsChartF23Ui]::GetWindowRect($chart, [ref]$cr) | Out-Null
    # 绘图区约在 x=56..w-6, y=26..h-18，取中心
    $cw = $cr.right - $cr.left
    $ch = $cr.bottom - $cr.top
    $px = [int]($cw / 2)
    $py = [int](($ch - 18) / 2 + 8)
    if ($px -lt 60) { $px = 60 }
    if ($py -lt 30) { $py = 30 }

    # ---- 1. 滚轮缩放 X 轴（连续 factor + 平滑动画目标日志） ----
    # 注意：WM_MOUSEWHEEL 的 lParam 是【屏幕坐标】（代码内会 ScreenToClient），
    # 而 WM_LBUTTONDOWN/MOVE/UP 的 lParam 是【客户区坐标】——两者必须分别构造。
    # 屏幕坐标 = 窗口左上角 + 客户区中心（GetWindowRect 返回窗口屏幕矩形）。
    $sx = [int]($cr.left + $cw / 2)
    $sy = [int]($cr.top + ($ch - 18) / 2 + 8)
    $lp = ($sx -band 0xFFFF) -bor (($sy -band 0xFFFF) -shl 16)
    $wp = (120 -shl 16)   # delta=+120 向上滚 -> 放大
    # 时序坑：--replay 回放按真实时钟推进，图表收到首批样本前 have_t=0，滚轮缩放被跳过。
    # 这里在等待数据到达期间重试滚轮，直到出现缩放目标日志（最多 ~6s）。
    $wheelOk = $false
    for ($try = 0; $try -lt 12 -and -not $wheelOk; $try++) {
        [OsChartF23Ui]::SendMessage($chart, 0x20A, [IntPtr]$wp, [IntPtr]$lp) | Out-Null  # WM_MOUSEWHEEL
        Start-Sleep -Milliseconds 400
        if (Log-Has '波形 X 轴缩放.*目标') { $wheelOk = $true }
    }
    Check $wheelOk "滚轮缩放 X 轴（连续目标日志）"

    # 连续缩放：再来一档，验证 X 区间连续收窄（放大后 span 变小）
    $r1 = Log-LastRange '波形 X 轴缩放.*目标'
    [OsChartF23Ui]::SendMessage($chart, 0x20A, [IntPtr]$wp, [IntPtr]$lp) | Out-Null
    Start-Sleep -Milliseconds 400
    $r2 = Log-LastRange '波形 X 轴缩放.*目标'
    if ($r1 -ne $null -and $r2 -ne $null) {
        $s1 = $r1[1] - $r1[0]
        $s2 = $r2[1] - $r2[0]
        Check ($s2 -lt $s1 -and $s2 -gt 0) "连续滚轮缩放区间收窄 ($s1 -> $s2 us)"
    } else {
        Check $false "滚轮缩放目标区间可解析（r1=$($r1 -join ',') r2=$($r2 -join ',')）"
    }

    # ---- 2. 左键拖拽平移视图 ----
    # 先记录平移前 X 区间
    $before = Log-LastRange '波形 X 轴缩放.*目标'
    # 按下起点（绘图区中部），再移动 +80px 向右（时间窗向过去方向平移）
    $down = ($px -band 0xFFFF) -bor (($py -band 0xFFFF) -shl 16)
    [OsChartF23Ui]::SendMessage($chart, 0x201, [IntPtr]1, [IntPtr]$down) | Out-Null  # WM_LBUTTONDOWN
    Start-Sleep -Milliseconds 80
    $mvx = $px + 80
    $move = ($mvx -band 0xFFFF) -bor (($py -band 0xFFFF) -shl 16)
    [OsChartF23Ui]::SendMessage($chart, 0x200, [IntPtr]0, [IntPtr]$move) | Out-Null   # WM_MOUSEMOVE
    Start-Sleep -Milliseconds 120
    $up = $move
    [OsChartF23Ui]::SendMessage($chart, 0x202, [IntPtr]0, [IntPtr]$up) | Out-Null     # WM_LBUTTONUP
    Start-Sleep -Milliseconds 200
    Check (Log-Has '波形拖拽平移结束') "左键拖拽平移结束日志"
    $afterDrag = Log-LastRange '波形拖拽平移结束: X=\['
    if ($before -ne $null -and $afterDrag -ne $null) {
        # 向右拖 80px -> X 窗左移 -> vx0 变小
        Check ($afterDrag[0] -lt $before[0]) "拖拽平移 X 窗左移 (vx0 $($before[0]) -> $($afterDrag[0]))"
    } else {
        Check $false "拖拽平移区间可解析（before=$($before -join ',') after=$($afterDrag -join ',')）"
    }

    # ---- 3. Ctrl+左键框选局部放大 ----
    # 框选区域：左上按下（Ctrl），向右下拖（垂直+水平跨度均 > 6px 才应用）
    $b0x = [int]($cw * 0.35)
    $b1x = [int]($cw * 0.9)
    if ($b0x -lt 60) { $b0x = 60 }
    if ($b1x -gt ($cw - 10)) { $b1x = $cw - 10 }
    $b0y = $py
    $b1y = $py + 60
    if ($b1y -gt ($ch - 20)) { $b1y = $ch - 20 }
    if ($b1y -le $b0y) { $b1y = $b0y + 40 }
    $lpd = ($b0x -band 0xFFFF) -bor (($b0y -band 0xFFFF) -shl 16)
    [OsChartF23Ui]::SendMessage($chart, 0x201, [IntPtr](1 -bor 0x08), [IntPtr]$lpd) | Out-Null  # Ctrl+DOWN
    Start-Sleep -Milliseconds 80
    $lpm = ($b1x -band 0xFFFF) -bor (($b1y -band 0xFFFF) -shl 16)
    [OsChartF23Ui]::SendMessage($chart, 0x200, [IntPtr]0, [IntPtr]$lpm) | Out-Null             # MOVE
    Start-Sleep -Milliseconds 120
    [OsChartF23Ui]::SendMessage($chart, 0x202, [IntPtr]0, [IntPtr]$lpm) | Out-Null             # UP
    Start-Sleep -Milliseconds 200
    Check (Log-Has '波形框选缩放') "Ctrl+框选缩放日志"
    $boxRange = Log-LastRange '波形框选缩放: X=\['
    if ($boxRange -ne $null -and $afterDrag -ne $null) {
        $bs = $boxRange[1] - $boxRange[0]
        $as = $afterDrag[1] - $afterDrag[0]
        # 框选区域约为全窗的 55%，缩放后区间应明显小于框选前区间
        Check ($bs -gt 0 -and $bs -lt $as) "框选缩放区间收窄（$bs < $as us）"
    } else {
        Check $false "框选缩放区间可解析"
    }

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
