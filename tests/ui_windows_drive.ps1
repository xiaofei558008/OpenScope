# OpenScope 窗口管理 UI 回归：
#   1. 以 ELF 路径启动应用（命令行加载）
#   2. 通过“窗口”菜单创建 波形窗口/数值窗口/示波器窗口
#   3. 校验 Tab 数量与“同一时刻只显示一个窗口”
#   4. 树中选择叶变量，触发 添加到波形/数值/示波器窗口 命令
#   5. 切换 Tab、关闭窗口，全程观察进程是否闪退
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = "",
    [string]$LeafName = "SystemCoreClock"
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
public class OsWinUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int left, top, right, bottom; }

    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Unicode)]
    public struct TVITEMW {
        public uint mask;
        public IntPtr hItem;
        public uint state;
        public uint stateMask;
        public IntPtr pszText;
        public int cchTextMax;
        public int iImage;
        public int iSelectedImage;
        public int cChildren;
        public IntPtr lParam;
    }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsWinUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsWinUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsWinUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsWinUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsWinUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsWinUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
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

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--select-leaf=$LeafName", "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"

    $tree = Find-ChildByClass $main "SysTreeView32"
    Check ($tree -ne [IntPtr]::Zero) "变量树存在"
    $tcount = 0
    for ($i = 0; $i -lt 50; $i++) {
        $tcount = [OsWinUi]::SendMessage($tree, 0x1105, [IntPtr]0, [IntPtr]0).ToInt64()  # TVM_GETCOUNT
        if ($tcount -gt 0) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($tcount -gt 0) "ELF 已加载（树节点 $tcount 个）"

    # 叶节点已通过命令行 --select-leaf 在应用进程内选中（避免跨进程指针消息）
    $selLog = ""
    if (Test-Path $log) {
        $selLog = (Get-Content $log -Encoding UTF8 | Select-String -Pattern '命令行选中叶变量' | Select-Object -Last 1)
    }
    Check ($selLog -match 'rc=0') "已选择树节点（叶）"

    # 创建三种窗口
    Send-Cmd $main 2012   # IDM_WIN_CHART
    Write-Output "chart cmd alive=$(-not $proc.HasExited)"
    Send-Cmd $main 2013   # IDM_WIN_NUM
    Write-Output "num cmd alive=$(-not $proc.HasExited)"
    Send-Cmd $main 2200   # IDM_WIN_MODULE_BASE + 0 (scope.bar)
    Write-Output "scope cmd alive=$(-not $proc.HasExited)"
    Start-Sleep -Milliseconds 500
    $tab = Find-ChildByClass $main "SysTabControl32"
    Write-Output "tab found alive=$(-not $proc.HasExited)"
    $ntab = [OsWinUi]::SendMessage($tab, 0x1304, [IntPtr]0, [IntPtr]0).ToInt64()  # TCM_GETITEMCOUNT
    Write-Output "tabcount=$ntab alive=$(-not $proc.HasExited)"
    Check ($ntab -eq 3) "Tab 数量 = 3（实际 $ntab）"

    $chart = Find-ChildByClass $main "OSChartWin"
    $num = Find-ChildByClass $main "OSNumWin"
    $scope = Find-ChildByClass $main "OSScopeWin"
    Check ($chart -ne [IntPtr]::Zero -and $num -ne [IntPtr]::Zero -and $scope -ne [IntPtr]::Zero) "三类窗口均已创建"
    Check ([OsWinUi]::IsWindowVisible($scope) -and -not [OsWinUi]::IsWindowVisible($chart) -and -not [OsWinUi]::IsWindowVisible($num)) "同一时刻只显示当前 Tab 窗口"

    # 树右键“添加到…”命令
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART
    Send-Cmd $main 2306   # IDM_TREE_ADD_NUM
    Send-Cmd $main 2307   # IDM_TREE_ADD_SCOPE
    Start-Sleep -Milliseconds 400
    $added = 0
    if (Test-Path $log) {
        $added = (Get-Content $log -Encoding UTF8 | Select-String -Pattern '添加变量' | Measure-Object).Count
    }
    Check ($added -ge 3) "三个窗口均收到添加变量（日志 $added 条）"

    # 切换 Tab：用方向键（WM_KEYDOWN），Tab 控件在进程内处理并触发 TCN_SELCHANGE
    function Select-Tab([int]$Idx, [IntPtr]$TabHwnd) {
        $cur = [OsWinUi]::SendMessage($TabHwnd, 0x130B, [IntPtr]0, [IntPtr]0).ToInt64()  # TCM_GETCURSEL
        if ($cur -lt 0) {
            [OsWinUi]::SendMessage($TabHwnd, 0x130C, [IntPtr]0, [IntPtr]0) | Out-Null
            $cur = 0
        }
        $delta = $Idx - $cur
        $vk = if ($delta -ge 0) { 0x27 } else { 0x25 }  # VK_RIGHT / VK_LEFT
        for ($i = 0; $i -lt [Math]::Abs($delta); $i++) {
            [OsWinUi]::SendMessage($TabHwnd, 0x100, [IntPtr]$vk, [IntPtr]0) | Out-Null  # WM_KEYDOWN
            Start-Sleep -Milliseconds 60
        }
    }
    Select-Tab 0 $tab
    Start-Sleep -Milliseconds 200
    Check ([OsWinUi]::IsWindowVisible($chart) -and -not [OsWinUi]::IsWindowVisible($scope)) "Tab0 -> 波形窗口可见"
    Select-Tab 1 $tab
    Start-Sleep -Milliseconds 200
    Check ([OsWinUi]::IsWindowVisible($num) -and -not [OsWinUi]::IsWindowVisible($chart)) "Tab1 -> 数值窗口可见"

    # 关闭当前 Tab（数值窗口）
    Send-Cmd $main 2501   # IDM_TAB_CLOSE
    Start-Sleep -Milliseconds 300
    $ntab2 = [OsWinUi]::SendMessage($tab, 0x1304, [IntPtr]0, [IntPtr]0).ToInt64()
    Check ($ntab2 -eq 2) "关闭 Tab 后数量 = 2（实际 $ntab2）"

    # 波形窗口标题栏 × 关闭（切到 Tab0 后点击右上角）
    Select-Tab 0 $tab
    Start-Sleep -Milliseconds 200
    $cr = New-Object OsWinUi+RECT
    [OsWinUi]::GetClientRect($chart, [ref]$cr) | Out-Null
    $lp = (($cr.right - 12) -band 0xFFFF) -bor ((10 -band 0xFFFF) -shl 16)
    [OsWinUi]::SendMessage($chart, 0x201, [IntPtr]1, [IntPtr]$lp) | Out-Null
    Start-Sleep -Milliseconds 300
    $ntab3 = [OsWinUi]::SendMessage($tab, 0x1304, [IntPtr]0, [IntPtr]0).ToInt64()
    Check ($ntab3 -eq 1) "波形窗口 × 关闭后 Tab 数量 = 1（实际 $ntab3）"

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '已加载 ELF|添加变量|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
