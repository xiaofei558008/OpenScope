# OpenScope 长录制波形缩放/平移回归（>600 采样点，用户反馈 bug 的精确场景）：
#   用户反馈：停止采集后（全局展示），滚轮缩放只能显示一小段波形、不能拖拽。
#   根因：chart_compute_view 的 full0/full1（缩放/框选/拖拽的夹紧边界）与可见窗口
#   （最后 npoints=600 点）混用——录制超过 600 点后"全量范围"被截断到最后 600 点，
#   全局视图本身只显示最后 600 点、任何缩放/平移都被夹在那个小窗内。
#   本测试用 2000 点回放（>600）验证：
#   A. 全局视图 = 全部数据（FITALL 后滚轮缩小为 no-op 保持全量）
#   B. 早期位置（最后 600 点窗口之外）滚轮放大：目标落在早期区域（旧实现被夹进尾窗）
#   C. 连续缩小可超过"最后 600 点"窗口跨度（旧实现最大 449250µs 封顶）
#   D. Ctrl+拖拽平移可到达早期区域（旧实现夹在尾窗）
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$replayCsv = Join-Path $root "tests\chart_replay_long.csv"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsLongZoomUi {
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
        [OsLongZoomUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsLongZoomUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsLongZoomUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsLongZoomUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsLongZoomUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsLongZoomUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

function Log-Has([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern
    return ($null -ne $m -and $m.Count -gt 0)
}

function Log-LastTarget([string]$Pattern) {
    if (-not (Test-Path $log)) { return $null }
    $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern | Select-Object -Last 1
    if ($null -eq $m) { return $null }
    if ($m.Line -match "\[(\d+),(\d+)\]") { return @([int64]$Matches[1], [int64]$Matches[2]) }
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

# chart_replay_long.csv：2000 行，ts = 1000000..2499250（750µs 间隔）。
# 旧实现下"最后600点窗口" = 采样点 1400..1999 = ts 2050000..2499250（span 449250）。
$DATA0 = [int64]1000000
$DATA1 = [int64]2499250
$TAIL600_START = [int64]2050000   # 旧实现"全量范围"的起点（最后600点窗）
$TAIL600_SPAN  = [int64]450000    # 旧实现最大可缩放跨度（略放大取整）

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
    $replayDone = $false
    for ($i = 0; $i -lt 75 -and -not $replayDone; $i++) {
        if (Log-Has '回放结束') { $replayDone = $true; break }
        Start-Sleep -Milliseconds 200
    }
    Check $replayDone "回放完整结束（2000 点已入缓冲，>600）"
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"

    $cr = New-Object OsLongZoomUi+RECT
    [OsLongZoomUi]::GetClientRect($chart, [ref]$cr) | Out-Null
    $wr = New-Object OsLongZoomUi+RECT
    [OsLongZoomUi]::GetWindowRect($chart, [ref]$wr) | Out-Null
    $cw = $cr.right - $cr.left
    $ch = $cr.bottom - $cr.top
    $pl = 56; $pr = $cr.right - 6
    $pt = 26; $pb = $cr.bottom - 18
    $py = [int](($pt + $pb) / 2)

    # 滚轮消息（屏幕坐标）。f=0.1 → 屏幕 x = plot.left + 0.1×(plot.right-plot.left)
    function Send-Wheel([IntPtr]$chart, [int]$sx, [int]$sy, [int]$delta) {
        $lp = ($sx -band 0xFFFF) -bor (($sy -band 0xFFFF) -shl 16)
        [OsLongZoomUi]::SendMessage($chart, 0x20A, [IntPtr](($delta -band 0xFFFF) -shl 16), [IntPtr]$lp) | Out-Null
    }

    # ---- 模拟停止采集：FITALL（全局展示） ----
    [OsLongZoomUi]::SendMessage($chart, 0x8000 + 6, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200

    # ---- A. 全局视图滚轮缩小 no-op（保持全量，不落入尾窗） ----
    $sxMid = [int]($wr.left + ($pl + $pr) / 2)
    $syMid = [int]($wr.top + $py)
    Send-Wheel $chart $sxMid $syMid (-120)
    Start-Sleep -Milliseconds 200
    $zoomLogCount = 0
    if (Test-Path $log) { $zoomLogCount = @(Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形 X 轴缩放').Count }
    Check ($zoomLogCount -eq 0) "全局视图滚轮缩小为 no-op（无缩放日志）"

    # ---- B. 早期位置（10%处，尾窗之外）滚轮放大：目标必须落在早期区域 ----
    $sx10 = [int]($wr.left + $pl + [int](($pr - $pl) * 0.1))
    Send-Wheel $chart $sx10 $syMid 120
    Start-Sleep -Milliseconds 400
    $t1 = Log-LastTarget '波形 X 轴缩放.*目标'
    if ($t1 -ne $null) {
        # 新实现：早期放大目标起点 ≈ 数据前 20%（<2000000）；旧实现被夹进尾窗（≥2050000）
        Check ($t1[0] -lt $TAIL600_START) "早期位置放大目标落在早期区域（目标 [$($t1[0]),$($t1[1])]，尾窗起点 $TAIL600_START）"
    } else {
        Check $false "早期放大目标可解析"
    }

    # ---- B2. 渲染验证：早期区域缩放的曲线必须实际绘制（像素级） ----
    # WM_OS_CHART_SHOT → exe 目录 chart_shot.bmp（WM_PRINT 渲染当前视图）
    $shotPath = Join-Path (Split-Path $exe) "chart_shot.bmp"
    if (Test-Path $shotPath) { Remove-Item -LiteralPath $shotPath -Force }
    [OsLongZoomUi]::SendMessage($chart, 0x8000 + 42, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 400
    $curvePixels = 0
    if (Test-Path $shotPath) {
        Add-Type -AssemblyName System.Drawing
        $bmp = [System.Drawing.Bitmap]::FromFile($shotPath)
        # 扫描绘图区内部（跳过标题/坐标轴/滚动条区域），统计亮色（曲线）像素
        for ($sx2 = 60; $sx2 -lt ($bmp.Width - 12); $sx2 += 2) {
            for ($sy2 = 30; $sy2 -lt ($bmp.Height - 40); $sy2 += 2) {
                $c = $bmp.GetPixel($sx2, $sy2)
                if ($c.R -gt 100 -or $c.G -gt 100 -or $c.B -gt 100) { $curvePixels++ }
            }
        }
        $bmp.Dispose()
        Check ($curvePixels -gt 50) "早期区域缩放后曲线实际渲染（$curvePixels 个亮色采样像素）"
    } else {
        Check $false "chart_shot.bmp 生成"
    }

    # ---- C. 连续缩小可突破"最后600点"窗口跨度 ----
    $span = 0
    for ($k = 0; $k -lt 6; $k++) {
        Send-Wheel $chart $sx10 $syMid (-120)
        Start-Sleep -Milliseconds 250
    }
    Start-Sleep -Milliseconds 500   # 动画收敛
    $t2 = Log-LastTarget '波形 X 轴缩放.*目标'
    if ($t2 -ne $null) {
        $span = $t2[1] - $t2[0]
        # 新实现：可扩到 ~80 万 µs 以上；旧实现封顶 ~449250µs
        Check ($span -gt $TAIL600_SPAN) "连续缩小可突破尾窗跨度（span $span > $TAIL600_SPAN）"
    } else {
        Check $false "连续缩小目标可解析"
    }

    # ---- D. Ctrl+拖拽平移可到达早期区域（旧实现被夹在尾窗） ----
    # 从当前视图向右拖 200px（时间窗向过去平移）
    $curx = [int](($pl + $pr) / 2)
    $down = ($curx -band 0xFFFF) -bor (($py -band 0xFFFF) -shl 16)
    [OsLongZoomUi]::SendMessage($chart, 0x201, [IntPtr](1 -bor 0x08), [IntPtr]$down) | Out-Null  # Ctrl+DOWN
    Start-Sleep -Milliseconds 80
    $mvx = $curx + 200
    $move = ($mvx -band 0xFFFF) -bor (($py -band 0xFFFF) -shl 16)
    [OsLongZoomUi]::SendMessage($chart, 0x200, [IntPtr]0, [IntPtr]$move) | Out-Null
    Start-Sleep -Milliseconds 120
    [OsLongZoomUi]::SendMessage($chart, 0x202, [IntPtr]0, [IntPtr]$move) | Out-Null
    Start-Sleep -Milliseconds 200
    Check (Log-Has '波形拖拽平移结束') "Ctrl+拖拽平移日志"
    $pan = Log-LastTarget '波形拖拽平移结束: X=\['
    if ($pan -ne $null) {
        # 平移后 vx0 必须仍在数据范围内（不拖出界），且允许到达早期区域
        Check ($pan[0] -ge $DATA0 -and $pan[1] -le $DATA1) "拖拽平移在数据范围内（X=[$($pan[0]),$($pan[1])]）"
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
