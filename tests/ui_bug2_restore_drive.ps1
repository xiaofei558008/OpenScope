# Bug 2 回归：正常关闭后重开，任务栏有图标但窗口不可见
# 根因：layout.ini 保存了最小化(-32768)/屏外坐标，重开把主窗口放在虚拟屏外。
# 修复：os_layout_load_from 用 MonitorFromRect 校验，屏外则回退默认位置。
# 测试：
#   A. layout 主窗口坐标 = (-32768,-32768)（最小化关闭哨兵） -> 窗口必须在虚拟屏内
#   B. layout 主窗口坐标 = (5000,5000)（正数但屏外）       -> 窗口必须在虚拟屏内
#   C. layout 主窗口坐标 = (100,100)（屏内）               -> 保持原坐标
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$autoLayout = Join-Path $env:LOCALAPPDATA "OpenScope\layout.ini"
$backup = Join-Path $env:TEMP "layout_ini_backup.ini"
if (Test-Path $autoLayout) { Copy-Item -LiteralPath $autoLayout -Destination $backup -Force }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class Bug2Ui {
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern int GetSystemMetrics(int idx);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int left, top, right, bottom; }
}
"@

function Rect-OnScreen([int]$L, [int]$T, [int]$R, [int]$B) {
    $vx = [Bug2Ui]::GetSystemMetrics(76); $vy = [Bug2Ui]::GetSystemMetrics(77)
    $vw = [Bug2Ui]::GetSystemMetrics(78); $vh = [Bug2Ui]::GetSystemMetrics(79)
    $cx = [int](($L + $R) / 2); $cy = [int](($T + $B) / 2)
    return ($cx -ge $vx -and $cx -lt ($vx + $vw) -and $cy -ge $vy -and $cy -lt ($vy + $vh))
}

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

function Write-Layout([int]$x, [int]$y) {
    $dir = Join-Path $env:LOCALAPPDATA "OpenScope"
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
    $body = @"
[layout]
version=1
main_x=$x
main_y=$y
main_w=1000
main_h=700
tree_w=340
log_h=170
active=-1
wins=0
"@
    [System.IO.File]::WriteAllText($autoLayout, $body, (New-Object System.Text.UTF8Encoding $true))
}

function Start-Check([string]$Tag, [int]$x, [int]$y, [bool]$ExpectOnScreen) {
    Write-Layout $x $y
    Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force
    Start-Sleep -Milliseconds 400
    $proc = Start-Process -FilePath $exe -ArgumentList @($Elf) -PassThru
    try {
        $main = [IntPtr]::Zero
        for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
            $proc.Refresh()
            $main = $proc.MainWindowHandle
            if ($main -ne [IntPtr]::Zero) { break }
            Start-Sleep -Milliseconds 200
        }
        if ($main -eq [IntPtr]::Zero) { Check $false "${Tag}: 主窗口未创建"; return }
        Start-Sleep -Milliseconds 500   # 等布局恢复 + 首次显示稳定
        $vis = [Bug2Ui]::IsWindowVisible($main)
        $r = New-Object Bug2Ui+RECT
        [Bug2Ui]::GetWindowRect($main, [ref]$r) | Out-Null
        $on = Rect-OnScreen $r.left $r.top $r.right $r.bottom
        Check $vis "${Tag}: 窗口可见 (visible=$vis)"
        if ($ExpectOnScreen) {
            Check $on "${Tag}: 窗口在虚拟屏内 (rect=$($r.left),$($r.top)-$($r.right),$($r.bottom))"
            Check ($r.right -gt $r.left -and $r.bottom -gt $r.top) "${Tag}: 窗口有非零尺寸"
        } else {
            Check (($r.left -eq $x) -and ($r.top -eq $y)) "${Tag}: 窗口坐标保持 ($($r.left),$($r.top)) vs ($x,$y)"
        }
    }
    finally {
        if (-not $proc.HasExited) { Stop-Process -Id $proc.Id -Force }
    }
}

Start-Check "A" (-32768) (-32768) $true   # 最小化关闭哨兵
Start-Check "B" 5000 5000 $true            # 正数但屏外
Start-Check "C" 100 100 $false             # 屏内坐标应保持

# 恢复原布局文件
if (Test-Path $backup) { Copy-Item -LiteralPath $backup -Destination $autoLayout -Force }

Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
