# F20 界面主题回归：暗色启动 / 运行时切换 / 亮色还原 / 持久化。
# 校验方式：PrintWindow 截图 + GetPixel 采样（主界面/树/右侧面板/日志/状态栏均须为深色）。
param(
    [string]$ExePath = ""
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$autoLayout = Join-Path $env:LOCALAPPDATA "OpenScope\layout.ini"
$hadLayout = Test-Path $autoLayout
if ($hadLayout) { $oldLayout = Get-Content -LiteralPath $autoLayout -Raw }
New-Item -ItemType Directory -Force -Path (Split-Path $autoLayout) | Out-Null

Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsThUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h, IntPtr hdc, uint flags);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsThUi]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsThUi]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsThUi]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Grab-Bmp([IntPtr]$h) {
    $r = New-Object OsThUi+RECT
    [OsThUi]::GetWindowRect($h, [ref]$r) | Out-Null
    $bmp = New-Object System.Drawing.Bitmap(($r.R - $r.L), ($r.B - $r.T))
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $dc = $g.GetHdc()
    [OsThUi]::PrintWindow($h, $dc, 2) | Out-Null
    $g.ReleaseHdc($dc); $g.Dispose()
    return $bmp
}
$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}
# 采样点（1280x800 逻辑坐标）：树/右侧面板/日志体/状态栏
$pts = @(
    @("tree",     200, 300),
    @("right",    900, 300),
    @("log",      640, 700),
    @("status",   300, 785)
)
function Sample-All([System.Drawing.Bitmap]$bmp, [bool]$ExpectDark) {
    $ok = $true
    foreach ($p in $pts) {
        $c = $bmp.GetPixel($p[1], $p[2])
        $lum = ($c.R + $c.G + $c.B) / 3
        $good = if ($ExpectDark) { $lum -lt 120 } else { $lum -gt 170 }
        if (-not $good) { $ok = $false }
        Write-Host ("{0,-7} ({1,4},{2,4}) = R{3,3} G{4,3} B{5,3} lum={6:N0} expect={7} {8}" -f $p[0],$p[1],$p[2],$c.R,$c.G,$c.B,$lum,$(if($ExpectDark){"dark"}else{"light"}),$(if($good){"ok"}else{"<<BAD"}))
    }
    return $ok
}
try {
    # 1) 暗色启动
    Set-Content -LiteralPath $autoLayout -Value "theme=1`n" -Encoding Ascii
    $proc = Start-Process -FilePath $exe -ArgumentList @("--no-layout") -PassThru
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "暗色启动：主窗口创建"
    # 深色重绘为非确定性竞态（PrintWindow 可能在重绘完成前抓帧），最多重试 ~9s
    $darkOk = $false
    for ($i = 0; $i -lt 6 -and -not $darkOk; $i++) {
        $bmp = Grab-Bmp $main
        $darkOk = Sample-All $bmp $true
        $bmp.Dispose()
        if (-not $darkOk) { Start-Sleep -Milliseconds 1500 }
    }
    Check $darkOk "暗色启动：树/右侧/日志/状态栏均为深色"

    # 2) 运行时切换 -> 亮色（菜单命令 IDM_THEME_DARK=2701）
    [OsThUi]::SendMessage($main, 0x111, [IntPtr]2701, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 4000
    $bmp = Grab-Bmp $main
    $lightOk = Sample-All $bmp $false
    $bmp.Dispose()
    Check $lightOk "运行时切换：树/右侧/日志/状态栏恢复浅色"
    $ini = Get-Content -LiteralPath $autoLayout
    Check (($ini | Select-String -SimpleMatch "theme=0" -Quiet)) "持久化：切换后 layout.ini theme=0"
}
finally {
    if ($proc -and -not $proc.HasExited) { $proc.Kill() }
    # 还原原 layout.ini
    if ($hadLayout) { Set-Content -LiteralPath $autoLayout -Value $oldLayout -Encoding UTF8 }
    else { Remove-Item -LiteralPath $autoLayout -Force -ErrorAction SilentlyContinue }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
