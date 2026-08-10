# OpenScope 左侧 elf 侧边栏折叠/展开按钮（request.md 新增特性 20，Drawer Toggle）UI 回归：
#   1. 变量栏展开态：头部栏 OSTreeHeader + 折叠按钮 OSTreeToggle（❮）可见
#   2. 点击折叠按钮 -> 变量树向左边界收起，只留左侧细条 OSTreeStrip（❯），头部栏/按钮隐藏
#   3. 钉住模式下显式折叠保持收起（tree_force_hidden），光标移开不再被 tick 强制展开
#   4. 点击细条 -> 变量树展开
#   5. WM_OS_TREE_TOGGLE 测试钩子（0=展开 / 1=折叠 / 2=切换）
#   6. 全程观察进程是否闪退
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
public class OsTreeToggleUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)]
    public struct POINT { public int x, y; }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsTreeToggleUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsTreeToggleUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsTreeToggleUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsTreeToggleUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsTreeToggleUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
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
    # 等主窗口真正可见（IsWindowVisible 要求祖先链全部 WS_VISIBLE，初始检查前必须就绪）
    for ($i = 0; $i -lt 50 -and -not [OsTreeToggleUi]::IsWindowVisible($main); $i++) {
        Start-Sleep -Milliseconds 200
    }
    Start-Sleep -Milliseconds 300

    $tree = Find-ChildByClass $main "SysTreeView32"
    Check ($tree -ne [IntPtr]::Zero) "变量树存在"
    $hdr = Find-ChildByClass $main "OSTreeHeader"
    Check ($hdr -ne [IntPtr]::Zero) "头部栏 OSTreeHeader 存在"
    $tgl = Find-ChildByClass $main "OSTreeToggle"
    Check ($tgl -ne [IntPtr]::Zero) "折叠按钮 OSTreeToggle 存在"
    $strip = Find-ChildByClass $main "OSTreeStrip"
    Check ($strip -ne [IntPtr]::Zero) "细条 OSTreeStrip 存在"

    # 初始（默认钉住常显）：树展开，头部栏/折叠按钮可见，细条隐藏
    Check ([OsTreeToggleUi]::IsWindowVisible($tree)) "初始变量树展开"
    Check ([OsTreeToggleUi]::IsWindowVisible($tgl)) "初始折叠按钮可见"
    Check ([OsTreeToggleUi]::IsWindowVisible($hdr)) "初始头部栏可见"
    Check (-not [OsTreeToggleUi]::IsWindowVisible($strip)) "初始细条隐藏"

    # 点击折叠按钮（WM_LBUTTONDOWN 0x201）-> 显式折叠
    [OsTreeToggleUi]::SendMessage($tgl, 0x201, [IntPtr]1, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 400
    Check (-not [OsTreeToggleUi]::IsWindowVisible($tree)) "点击折叠按钮后变量树收起"
    Check ([OsTreeToggleUi]::IsWindowVisible($strip)) "折叠后细条可见"
    Check (-not [OsTreeToggleUi]::IsWindowVisible($tgl)) "折叠后折叠按钮隐藏"
    Check (-not [OsTreeToggleUi]::IsWindowVisible($hdr)) "折叠后头部栏隐藏"
    Check (Log-Has '变量栏折叠') "日志记录折叠"

    # 钉住模式 + 显式折叠：光标移开，等待 > tick(200ms) 多轮，仍保持收起（tree_force_hidden）
    [OsTreeToggleUi]::SetCursorPos(1200, 200) | Out-Null
    Start-Sleep -Milliseconds 1500
    Check (-not [OsTreeToggleUi]::IsWindowVisible($tree)) "钉住 + 显式折叠：光标移开仍保持收起"

    # 点击细条 -> 展开
    [OsTreeToggleUi]::SendMessage($strip, 0x201, [IntPtr]1, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 400
    Check ([OsTreeToggleUi]::IsWindowVisible($tree)) "点击细条后变量树展开"
    Check ([OsTreeToggleUi]::IsWindowVisible($tgl)) "展开后折叠按钮可见"
    Check (-not [OsTreeToggleUi]::IsWindowVisible($strip)) "展开后细条隐藏"
    Check (Log-Has '变量栏展开') "日志记录展开"

    # WM_OS_TREE_TOGGLE 测试钩子（0x800E）：1=折叠，0=展开，2=切换
    [OsTreeToggleUi]::SendMessage($main, 0x800E, [IntPtr]1, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    Check (-not [OsTreeToggleUi]::IsWindowVisible($tree)) "钩子: 折叠"
    [OsTreeToggleUi]::SendMessage($main, 0x800E, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    Check ([OsTreeToggleUi]::IsWindowVisible($tree)) "钩子: 展开"
    [OsTreeToggleUi]::SendMessage($main, 0x800E, [IntPtr]2, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    Check (-not [OsTreeToggleUi]::IsWindowVisible($tree)) "钩子: 切换(展开->折叠)"
    [OsTreeToggleUi]::SendMessage($main, 0x800E, [IntPtr]2, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    Check ([OsTreeToggleUi]::IsWindowVisible($tree)) "钩子: 切换(折叠->展开)"

    # 自动隐藏模式（非钉住）+ 显式折叠：先展开再折叠，光标移开后仍保持（force）直到悬停细条
    [OsTreeToggleUi]::SendMessage($main, 0x8009, [IntPtr]1, [IntPtr]0) | Out-Null  # 开启自动隐藏
    Start-Sleep -Milliseconds 300
    [OsTreeToggleUi]::SendMessage($main, 0x800E, [IntPtr]1, [IntPtr]0) | Out-Null  # 显式折叠
    Start-Sleep -Milliseconds 300
    [OsTreeToggleUi]::SetCursorPos(1200, 200) | Out-Null
    Start-Sleep -Milliseconds 1500
    Check (-not [OsTreeToggleUi]::IsWindowVisible($tree)) "自动隐藏模式 + 显式折叠：光标移开保持收起"
    # 悬停细条（客户区 x=4）-> 展开
    $pt = New-Object OsTreeToggleUi+POINT
    $pt.x = 4; $pt.y = 200
    [OsTreeToggleUi]::ClientToScreen($main, [ref]$pt) | Out-Null
    [OsTreeToggleUi]::SetCursorPos($pt.x, $pt.y) | Out-Null
    Start-Sleep -Milliseconds 800
    Check ([OsTreeToggleUi]::IsWindowVisible($tree)) "悬停细条后自动展开"

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
