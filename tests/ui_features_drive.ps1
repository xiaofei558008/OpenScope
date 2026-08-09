# 特性回归：N6 无关于按钮 / N11 单tab多窗口+最大化 / Bug3 全屏 / N12 钉图标 / N10 数值窗口结构 / Ctrl+B 不崩溃
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
$autoLayout = Join-Path $env:LOCALAPPDATA "OpenScope\layout.ini"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
if (Test-Path $autoLayout) { Remove-Item -LiteralPath $autoLayout -Force }

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsFeUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern IntPtr GetParent(IntPtr h);
    [DllImport("user32.dll")] public static extern int GetDlgCtrlID(IntPtr h);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsFeUi]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsFeUi]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsFeUi]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Get-ChildByClass([IntPtr]$P, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $sb=New-Object System.Text.StringBuilder 128; [OsFeUi]::GetClassName($h,$sb,128)|Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false }; return $true }
    [OsFeUi]::EnumChildWindows($P,$cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Get-ChildClasses([IntPtr]$P) {
    $script:classes = New-Object System.Collections.ArrayList
    [OsFeUi]::EnumChildWindows($P, { param($h,$l) $sb=New-Object System.Text.StringBuilder 128
        [OsFeUi]::GetClassName($h,$sb,128)|Out-Null; [void]$script:classes.Add($sb.ToString()); return $true }, [IntPtr]::Zero)|Out-Null
    return $script:classes
}
function Get-ButtonIds([IntPtr]$P) {
    $script:ids = New-Object System.Collections.ArrayList
    [OsFeUi]::EnumChildWindows($P, { param($h,$l) $sb=New-Object System.Text.StringBuilder 128
        [OsFeUi]::GetClassName($h,$sb,128)|Out-Null
        if ($sb.ToString() -eq "Button") { [void]$script:ids.Add([OsFeUi]::GetDlgCtrlID($h)) }
        return $true }, [IntPtr]::Zero)|Out-Null
    return $script:ids
}
function Wait-Log([string]$Pattern, [int]$TimeoutMs = 8000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        if (Test-Path $log) {
            if (Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch $Pattern -Quiet) { return $true }
        }
        Start-Sleep -Milliseconds 200
    }
    return $false
}
$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--no-layout") -PassThru
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 1200
    $tab = Get-ChildByClass $main "SysTabControl32"
    Check ($tab -ne [IntPtr]::Zero) "Tab 控件存在"

    # N6：工具栏无“关于”按钮（IDC_BTN_ABOUT=2010 已删除）
    $btnIds = Get-ButtonIds $main
    $hasAbout = ($btnIds -contains 2010)
    Check (-not $hasAbout) "无“关于”按钮（工具栏已删除 About）"

    # Bug8：工具栏已无“钉住变量栏”按钮（IDC_BTN_PIN=2015 已删除，自动隐藏唯一入口=左侧树钉图标）
    $hasPinBtn = ($btnIds -contains 2015)
    Check (-not $hasPinBtn) "Bug8 工具栏已无“钉住变量栏”按钮（自动隐藏归位到左侧树）"

    # 建波形窗口
    [OsFeUi]::SendMessage($main, 0x111, [IntPtr]2012, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 500
    $chart = Get-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero -and [OsFeUi]::GetParent($chart) -eq $tab) "波形窗口在 tab 中"

    # N11：同一 tab 添加第二个窗口（数值窗口）
    [OsFeUi]::SendMessage($main, 0x111, [IntPtr]2504, [IntPtr]0) | Out-Null  # IDM_TAB_ADD_NUM
    Start-Sleep -Milliseconds 500
    $num = Get-ChildByClass $main "OSNumWin"
    Check ($num -ne [IntPtr]::Zero -and [OsFeUi]::GetParent($num) -eq $tab) "N11 同tab添加数值窗口成功"
    $winClasses = Get-ChildClasses $tab
    $winCount = @($winClasses | Where-Object { $_ -eq "OSChartWin" -or $_ -eq "OSNumWin" }).Count
    Check ($winCount -ge 2) "N11 一个tab多窗口（count=$winCount）"

    # N11：最大化/还原
    [OsFeUi]::SendMessage($main, 0x111, [IntPtr]2505, [IntPtr]0) | Out-Null  # IDM_TAB_MAXIMIZE
    Check (Wait-Log "窗口最大化" 5000) "N11 窗口最大化（填满tab）"
    [OsFeUi]::SendMessage($main, 0x111, [IntPtr]2505, [IntPtr]0) | Out-Null
    Check (Wait-Log "窗口还原" 5000) "N11 窗口还原"

    # Bug3：全屏/退出全屏（父窗口切换：tab -> 主窗口 -> tab）
    [OsFeUi]::SendMessage($main, 0x111, [IntPtr]2506, [IntPtr]0) | Out-Null  # IDM_TAB_FULLSCREEN
    Check (Wait-Log "窗口全屏" 5000) "Bug3 进入全屏（日志）"
    Start-Sleep -Milliseconds 300
    $fsChart = ([OsFeUi]::GetParent($chart) -eq $main)
    $fsNum = ($num -ne [IntPtr]::Zero -and [OsFeUi]::GetParent($num) -eq $main)
    Check ($fsChart -or $fsNum) "Bug3 全屏后目标窗口父窗口=主窗口"
    [OsFeUi]::SendMessage($main, 0x111, [IntPtr]2506, [IntPtr]0) | Out-Null
    Check (Wait-Log "窗口退出全屏" 5000) "Bug3 退出全屏（日志）"
    Start-Sleep -Milliseconds 300
    Check ([OsFeUi]::GetParent($chart) -eq $tab) "Bug3 退出全屏后父窗口=Tab"

    # N12：钉图标存在，点击切换日志
    $pin = Get-ChildByClass $main "OSTreePin"
    Check ($pin -ne [IntPtr]::Zero) "N12 钉图标控件存在"
    if ($pin -ne [IntPtr]::Zero) {
        [OsFeUi]::PostMessage($pin, 0x0201, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_LBUTTONDOWN
        Start-Sleep -Milliseconds 200
        $pinLog = ((Test-Path $log) -and ((Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch "变量栏已钉住常显" -Quiet) -or
                                          (Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch "变量栏改为自动隐藏" -Quiet)))
        Check $pinLog "N12 点击钉图标切换状态（日志）"
    }

    # N10 结构：数值窗口含 ListView（实时勾选框列所在控件）
    $lv = Get-ChildByClass $num "SysListView32"
    Check ($lv -ne [IntPtr]::Zero) "N10 数值窗口含 ListView（实时列）"

    # Ctrl+B（chartwin 多坐标轴）不崩溃
    [OsFeUi]::PostMessage($chart, 0x0100, [IntPtr][int][char]'B', [IntPtr]1) | Out-Null  # WM_KEYDOWN 'B'
    Start-Sleep -Milliseconds 200
    Check (-not $proc.HasExited) "Ctrl+B 按键后进程未崩溃"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
