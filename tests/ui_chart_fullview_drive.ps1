# OpenScope 波形全局视图（停止采集后整体展示）交互回归：
#   用户反馈 bug：全局显示（view_all=1）下滚轮缩小/框选/拖拽平移行为错误——
#   界面直接跳转到小段波形区域，只能按 F 恢复；拖拽平移在全局视图无效果。
#   根因：滚轮缩放 no-op 时 view_all 被无条件清除 → fit_x=1 跌回"最后 600 点"窗口；
#        拖拽平移无数据范围夹紧（拖出界）。
# 测试场景（--replay 模拟采集数据，WM_OS_CHART_FITALL 模拟停止采集广播）：
#   A. 全局视图滚轮【缩小】(no-op)：不得清除全局视图；随后放大时"起点"应为全量范围
#   B. 全局视图框选局部区域：目标区间 = 框选像素映射的全量子区间
#   C. 全局视图 Ctrl+拖拽平移：夹紧到数据范围（vx0 >= 数据起点，不拖出界）
#   D. 全程不闪退
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
public class OsFullViewUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int left, top, right, bottom; }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsFullViewUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsFullViewUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsFullViewUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsFullViewUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsFullViewUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsFullViewUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

function Log-Has([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern
    return ($null -ne $m -and $m.Count -gt 0)
}

function Log-LastStart([string]$Pattern) {
    if (-not (Test-Path $log)) { return $null }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern | Select-Object -Last 1
    if ($null -eq $m) { return $null }
    if ($m.Line -match "起点=\[(\d+),(\d+)\]") { return @([int64]$Matches[1], [int64]$Matches[2]) }
    return $null
}

function Log-LastRange([string]$Pattern) {
    if (-not (Test-Path $log)) { return $null }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern | Select-Object -Last 1
    if ($null -eq $m) { return $null }
    if ($m.Line -match "\[(\d+),(\d+)\]us") { return @([int64]$Matches[1], [int64]$Matches[2]) }
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

# chart_replay.csv 数据时间窗：1000000..8900000 us（span 7.9s，回放按真实时钟 ~7.9s 播完）
$FULL0 = [int64]1000000
$FULL1 = [int64]8900000
$FULLSPAN = $FULL1 - $FULL0

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

    Send-Cmd $main 2012   # IDM_WIN_CHART
    Start-Sleep -Milliseconds 250
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART
    # 等回放完整播完（7.9s），保证全量数据已入缓冲、时间窗确定
    $replayDone = $false
    for ($i = 0; $i -lt 75 -and -not $replayDone; $i++) {
        if (Log-Has '回放结束') { $replayDone = $true; break }
        Start-Sleep -Milliseconds 200
    }
    Check $replayDone "回放完整结束（全量数据入缓冲）"
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"

    $cr = New-Object OsFullViewUi+RECT
    [OsFullViewUi]::GetClientRect($chart, [ref]$cr) | Out-Null
    $wr = New-Object OsFullViewUi+RECT
    [OsFullViewUi]::GetWindowRect($chart, [ref]$wr) | Out-Null
    $cw = $cr.right - $cr.left
    $ch = $cr.bottom - $cr.top
    # 绘图区约 x=56..w-6, y=26..h-18
    $pl = 56; $pr = $cr.right - 6
    $pt = 26; $pb = $cr.bottom - 18
    $px = [int](($pl + $pr) / 2)
    $py = [int](($pt + $pb) / 2)
    # WM_MOUSEWHEEL 用屏幕坐标
    $sx = [int]($wr.left + $px)
    $sy = [int]($wr.top + $py)

    # ---- 模拟停止采集：发送 WM_OS_CHART_FITALL（全局展示全部波形） ----
    [OsFullViewUi]::SendMessage($chart, 0x8000 + 6, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200

    # ---- A1. 全局视图滚轮【缩小】(no-op)：不得清除全局视图 ----
    $wpOut = (0 -bor ((-120 -band 0xFFFF) -shl 16))   # delta=-120 缩小
    $lp = ($sx -band 0xFFFF) -bor (($sy -band 0xFFFF) -shl 16)
    [OsFullViewUi]::SendMessage($chart, 0x20A, [IntPtr]$wpOut, [IntPtr]$lp) | Out-Null
    Start-Sleep -Milliseconds 200
    $zoomCount = 0
    if (Test-Path $log) { $zoomCount = @(Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形 X 轴缩放').Count }
    Check ($zoomCount -eq 0) "全局视图滚轮缩小为 no-op（无缩放日志）"

    # ---- A2. 随后滚轮【放大】："起点"必须是全量范围（验证全局视图未被破坏） ----
    $wpIn = (120 -shl 16)   # delta=+120 放大
    [OsFullViewUi]::SendMessage($chart, 0x20A, [IntPtr]$wpIn, [IntPtr]$lp) | Out-Null
    Start-Sleep -Milliseconds 400
    $start1 = Log-LastStart '波形 X 轴缩放.*起点='
    if ($start1 -ne $null) {
        $sSpan = $start1[1] - $start1[0]
        # 全量范围 = 回放完整数据（变量加入前播过的开头几行不在此系列，允许 15% 余量）
        Check ($sSpan -gt ($FULLSPAN * 0.85)) "缩小 no-op 后放大起点仍为全量时间窗（起点 $($start1[0])..$($start1[1])，span $sSpan）"
        $script:seriesFull0 = $start1[0]
        $script:seriesFull1 = $start1[1]
    } else {
        Check $false "放大起点可解析"
    }

    # ---- B. 全局视图普通左键框选：目标 = 框选像素映射的全量子区间 ----
    [OsFullViewUi]::SendMessage($chart, 0x8000 + 6, [IntPtr]0, [IntPtr]0) | Out-Null  # FITALL 恢复全局
    Start-Sleep -Milliseconds 200
    $b0x = [int]($pl + ($pr - $pl) * 0.2)
    $b1x = [int]($pl + ($pr - $pl) * 0.5)
    $b0y = $py; $b1y = $py + 60
    if ($b1y -gt $pb) { $b1y = $pb - 2 }
    $lpd = ($b0x -band 0xFFFF) -bor (($b0y -band 0xFFFF) -shl 16)
    [OsFullViewUi]::SendMessage($chart, 0x201, [IntPtr]1, [IntPtr]$lpd) | Out-Null  # 普通 DOWN
    Start-Sleep -Milliseconds 80
    $lpm = ($b1x -band 0xFFFF) -bor (($b1y -band 0xFFFF) -shl 16)
    [OsFullViewUi]::SendMessage($chart, 0x200, [IntPtr]0, [IntPtr]$lpm) | Out-Null  # MOVE
    Start-Sleep -Milliseconds 120
    [OsFullViewUi]::SendMessage($chart, 0x202, [IntPtr]0, [IntPtr]$lpm) | Out-Null  # UP
    Start-Sleep -Milliseconds 600   # 等待动画收敛
    Check (Log-Has '波形框选缩放') "全局视图框选缩放日志"
    $box = Log-LastRange '波形框选缩放: X=\['
    if ($box -ne $null -and $script:seriesFull0 -ne $null) {
        $sSpan = $script:seriesFull1 - $script:seriesFull0
        $exp0 = $script:seriesFull0 + [int64]($sSpan * 0.2)
        $exp1 = $script:seriesFull0 + [int64]($sSpan * 0.5)
        $d0 = [Math]::Abs($box[0] - $exp0)
        $d1 = [Math]::Abs($box[1] - $exp1)
        # 目标窗口 = 系列全量 20%~50% 子区间（容差 6%）
        Check (($d0 -lt $sSpan * 0.06) -and ($d1 -lt $sSpan * 0.06)) "框选目标 = 全量 20%~50% 子区间（X=[$($box[0]),$($box[1])]us，期望 [$exp0,$exp1]）"
    } else {
        Check $false "框选区间可解析"
    }

    # ---- C. 全局视图 Ctrl+拖拽平移：夹紧到数据范围（不拖出界） ----
    [OsFullViewUi]::SendMessage($chart, 0x8000 + 6, [IntPtr]0, [IntPtr]0) | Out-Null  # FITALL
    Start-Sleep -Milliseconds 200
    $down = ($px -band 0xFFFF) -bor (($py -band 0xFFFF) -shl 16)
    [OsFullViewUi]::SendMessage($chart, 0x201, [IntPtr](1 -bor 0x08), [IntPtr]$down) | Out-Null  # Ctrl+DOWN
    Start-Sleep -Milliseconds 80
    $mvx = $px + 80   # 向右拖 80px（意图向过去平移——全局视图已无界外数据，应被夹紧）
    $move = ($mvx -band 0xFFFF) -bor (($py -band 0xFFFF) -shl 16)
    [OsFullViewUi]::SendMessage($chart, 0x200, [IntPtr]0, [IntPtr]$move) | Out-Null
    Start-Sleep -Milliseconds 120
    [OsFullViewUi]::SendMessage($chart, 0x202, [IntPtr]0, [IntPtr]$move) | Out-Null
    Start-Sleep -Milliseconds 200
    Check (Log-Has '波形拖拽平移结束') "全局视图 Ctrl+拖拽平移日志"
    $pan = Log-LastRange '波形拖拽平移结束: X=\['
    if ($pan -ne $null -and $script:seriesFull0 -ne $null) {
        # 全局视图 span = 全量：拖拽夹紧后 vx0 必须 >= 系列数据起点（旧实现会拖出界）
        Check ($pan[0] -ge $script:seriesFull0 -and $pan[1] -le $script:seriesFull1) "拖拽平移夹紧在数据范围（X=[$($pan[0]),$($pan[1])]us，数据 [$($script:seriesFull0),$($script:seriesFull1)]）"
    } else {
        Check $false "拖拽平移区间可解析"
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
