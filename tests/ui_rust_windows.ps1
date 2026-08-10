# Rust 版 bug4/5/6 回归（request.md "rust开发 bug" #4 菜单窗口置灰 / #5 变量树右键无菜单 / #6 数值窗口）
#   A. 菜单 窗口→波形窗口/数值窗口 不再置灰（bug4）
#   B. 点 窗口→波形窗口 -> 新增波形 tab（tab 数 1->2，OpenScopeChart 子窗口 2 个，新窗口 ID=2301）（bug4）
#   C. 点 窗口→数值窗口 -> 新增数值 tab（OpenScopeNum ID=2351 + 其 ListView/编辑框/写入按钮）（bug4/6）
#   D. 加载 ELF 后右键变量树 -> 弹出右键菜单（含 添加变量到波形/数值窗口/全部勾选/取消勾选）（bug5）
#   E. 全部勾选 + 添加变量到数值窗口 -> 数值窗口自动创建并显示 3 行全局变量（bug5/6）
#
# 注：跨进程读取 ListView/TabControl 逐项文本（LVM_GETITEMTEXTW/TCM_GETITEMW 传外进程指针）会
# 让目标进程在写回时崩溃（SendMessage 不做跨进程内存封送）。因此本脚本只做"标量"级断言
# （计数、控件 ID、窗口类、菜单状态），这些足以确定性证明 bug4/5/6 的修复。
param(
    [string]$ElfFile = "D:/OpenScope/tests/elf_sample.out",
    [string]$ExePath = "D:/OpenScope/rust/target/release/openscope-app.exe"
)
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsWinUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern IntPtr GetMenu(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr GetSubMenu(IntPtr hMenu, int nPos);
    [DllImport("user32.dll")] public static extern int GetMenuItemCount(IntPtr hMenu);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetMenuString(IntPtr hMenu, uint uIDItem, StringBuilder lpString, int nMaxCount, uint uFlag);
    [DllImport("user32.dll")] public static extern uint GetMenuState(IntPtr hMenu, uint uId, uint uFlags);
    [DllImport("user32.dll")] public static extern int GetDlgCtrlID(IntPtr h);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
# 常量
$MF_BYPOSITION = 0x400
$MF_BYCOMMAND  = 0x0000
$MF_GRAYED     = 0x0001
$MF_DISABLED   = 0x0002
$TCM_GETITEMCOUNT = 0x1304
$LVM_GETITEMCOUNT = 0x1004
$TVM_GETCOUNT     = 0x1105
$WM_COMMAND       = 0x0111
$WM_CONTEXTMENU   = 0x007B
$WM_CANCELMODE    = 0x001F
# 菜单/命令 ID（与 app.rs 一致）
$IDM_WIN_CHART = 2012
$IDM_WIN_NUM   = 2013
$IDM_CTX_ADD_CHART = 9501
$IDM_CTX_ADD_NUM   = 9502
$IDM_CTX_CHECK_ALL = 9503
$IDM_CTX_UNCHECK_ALL = 9504

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsWinUi]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsWinUi]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsWinUi]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $sb=New-Object System.Text.StringBuilder 128; [OsWinUi]::GetClassName($h,$sb,128)|Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false }; return $true }
    [OsWinUi]::EnumChildWindows($Parent,$cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-ChildByCtrlId([IntPtr]$Parent, [int]$Id) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) if ([OsWinUi]::GetDlgCtrlID($h) -eq $Id) { $script:hit=$h; return $false }; return $true }
    [OsWinUi]::EnumChildWindows($Parent,$cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Count-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:cnt = 0
    $cb = { param($h,$l) $sb=New-Object System.Text.StringBuilder 128; [OsWinUi]::GetClassName($h,$sb,128)|Out-Null
        if ($sb.ToString() -eq $Class) { $script:cnt++ }; return $true }
    [OsWinUi]::EnumChildWindows($Parent,$cb,[IntPtr]::Zero)|Out-Null; return $script:cnt
}
function TreeCount([IntPtr]$tree) {
    if ($tree -eq [IntPtr]::Zero) { return 0 }
    return [OsWinUi]::SendMessage($tree, $TVM_GETCOUNT, [IntPtr]0, [IntPtr]0).ToInt64()
}
function TabCount([IntPtr]$tab) {
    return [OsWinUi]::SendMessage($tab, $TCM_GETITEMCOUNT, [IntPtr]0, [IntPtr]0).ToInt64()
}
function ListCount([IntPtr]$list) {
    return [OsWinUi]::SendMessage($list, $LVM_GETITEMCOUNT, [IntPtr]0, [IntPtr]0).ToInt64()
}
function Get-Main([int]$ProcId) { return Find-ByClass $ProcId "OpenScopeMain" }
function Wait-Main([int]$ProcId, [int]$tries) {
    for ($i = 0; $i -lt $tries; $i++) {
        $m = Get-Main $ProcId
        if ($m -ne [IntPtr]::Zero) { return $m }
        Start-Sleep -Milliseconds 200
    }
    return [IntPtr]::Zero
}

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

$existing = @(Get-Process openscope-app -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) { Write-Output "ERROR: app already running: $($existing.Id -join ',')"; exit 3 }

# ---------- A. 菜单 窗口→波形/数值 不再置灰（bug4） ----------
$proc = Start-Process -FilePath $ExePath -PassThru
try {
    $main = Wait-Main $proc.Id 50
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 800
    $menu = [OsWinUi]::GetMenu($main)
    Check ($menu -ne [IntPtr]::Zero) "A0 主窗口存在菜单栏"
    if ($menu -ne [IntPtr]::Zero) {
        $sub = [OsWinUi]::GetSubMenu($menu, 3)  # 窗口(&W)
        Check ($sub -ne [IntPtr]::Zero) "A1 窗口菜单存在"
        if ($sub -ne [IntPtr]::Zero) {
            $sChart = [OsWinUi]::GetMenuState($sub, $IDM_WIN_CHART, $MF_BYCOMMAND)
            $sNum   = [OsWinUi]::GetMenuState($sub, $IDM_WIN_NUM,   $MF_BYCOMMAND)
            Write-Output ("A 窗口菜单项状态: 波形=$sChart 数值=$sNum")
            Check ((($sChart -band ($MF_GRAYED -bor $MF_DISABLED)) -eq 0) -and (($sNum -band ($MF_GRAYED -bor $MF_DISABLED)) -eq 0)) "A2 波形窗口/数值窗口菜单项未置灰（bug4 修复）"
            $sb = New-Object System.Text.StringBuilder 128
            [OsWinUi]::GetMenuString($sub, $IDM_WIN_CHART, $sb, 128, $MF_BYCOMMAND) | Out-Null
            Check ($sb.ToString() -eq "波形窗口") "A3 菜单文本「波形窗口」"
            $sb2 = New-Object System.Text.StringBuilder 128
            [OsWinUi]::GetMenuString($sub, $IDM_WIN_NUM, $sb2, 128, $MF_BYCOMMAND) | Out-Null
            Check ($sb2.ToString() -eq "数值窗口") "A4 菜单文本「数值窗口」"
        }
    }
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- B. 窗口→波形窗口 新增 tab（bug4） ----------
$proc = Start-Process -FilePath $ExePath -PassThru
try {
    $main = Wait-Main $proc.Id 50
    Start-Sleep -Milliseconds 800
    $tab = Find-ChildByClass $main "SysTabControl32"
    Check ($tab -ne [IntPtr]::Zero) "B0 右侧 TabControl 存在"
    $c0 = TabCount $tab
    Write-Output ("B 初始 tab 数 = " + $c0)
    [OsWinUi]::PostMessage($main, $WM_COMMAND, [IntPtr]$IDM_WIN_CHART, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 900
    $c1 = TabCount $tab
    Write-Output ("B 点波形窗口后 tab 数 = " + $c1)
    Check ($c1 -eq ($c0 + 1)) "B1 点 窗口→波形窗口 后新增一个 tab（bug4 修复）"
    $charts = Count-ChildByClass $tab "OpenScopeChart"
    Check ($charts -eq 2) "B2 波形子窗口共 2 个（初始+新增）"
    Check ((Find-ChildByCtrlId $tab 2301) -ne [IntPtr]::Zero) "B3 新增波形窗口控制 ID=2301（tab 与窗口映射一致）"
    Check (-not $proc.HasExited) "B4 进程存活"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- C. 窗口→数值窗口 新增 tab + 数值窗口控件（bug4/6） ----------
$proc = Start-Process -FilePath $ExePath -PassThru
try {
    $main = Wait-Main $proc.Id 50
    Start-Sleep -Milliseconds 800
    [OsWinUi]::PostMessage($main, $WM_COMMAND, [IntPtr]$IDM_WIN_NUM, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 900
    $tab = Find-ChildByClass $main "SysTabControl32"
    $c1 = TabCount $tab
    Check ($c1 -ge 2) "C1 点 窗口→数值窗口 后新增 tab"
    $numwin = Find-ChildByClass $tab "OpenScopeNum"
    Check ($numwin -ne [IntPtr]::Zero) "C2 数值子窗口 OpenScopeNum 已创建（bug6）"
    if ($numwin -ne [IntPtr]::Zero) {
        Check ([OsWinUi]::GetDlgCtrlID($numwin) -eq 2351) "C3 数值窗口控制 ID=2351（tab 与窗口映射一致）"
        $list = Find-ChildByClass $numwin "SysListView32"
        Check ($list -ne [IntPtr]::Zero) "C4 数值窗口含 ListView"
        $edit = Find-ChildByClass $numwin "Edit"
        $btn  = Find-ChildByClass $numwin "Button"
        Check ($edit -ne [IntPtr]::Zero) "C5 数值窗口含编辑框"
        Check ($btn -ne [IntPtr]::Zero) "C6 数值窗口含「写入」按钮"
    }
    Check (-not $proc.HasExited) "C7 进程存活"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- D. 右键变量树弹出菜单（bug5） ----------
$proc = Start-Process -FilePath $ExePath -ArgumentList $ElfFile -PassThru
try {
    $main = Wait-Main $proc.Id 50
    Start-Sleep -Milliseconds 1200
    $tree = Find-ChildByClass $main "SysTreeView32"
    $tc = TreeCount $tree
    Write-Output ("D 树数量 = " + $tc)
    Check ($tc -eq 3) "D0 ELF 加载后树含 3 个全局变量"
    [OsWinUi]::PostMessage($main, $WM_CONTEXTMENU, $tree, [IntPtr]0) | Out-Null
    $pop = [IntPtr]::Zero
    for ($i = 0; $i -lt 30 -and $pop -eq [IntPtr]::Zero -and -not $proc.HasExited; $i++) {
        $pop = Find-ByClass $proc.Id "#32768"
        if ($pop -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($pop -ne [IntPtr]::Zero) "D1 右键变量树弹出菜单（bug5 修复）"
    if ($pop -ne [IntPtr]::Zero) {
        $pmenu = [OsWinUi]::SendMessage($pop, 0x1E1, [IntPtr]0, [IntPtr]0)  # MN_GETHMENU
        if ($pmenu -ne [IntPtr]::Zero) {
            $n = [OsWinUi]::GetMenuItemCount($pmenu)
            $names = @()
            for ($i = 0; $i -lt $n; $i++) {
                $sb = New-Object System.Text.StringBuilder 128
                [OsWinUi]::GetMenuString($pmenu, $i, $sb, 128, $MF_BYPOSITION) | Out-Null
                $names += $sb.ToString()
            }
            Write-Output ("D 右键菜单项: " + ($names -join " | "))
            Check (($names -contains "添加变量到波形窗口") -and ($names -contains "添加变量到数值窗口")) "D2 右键菜单含 添加变量到波形/数值窗口"
            Check (($names -contains "全部勾选") -and ($names -contains "全部取消勾选")) "D3 右键菜单含 全部勾选/全部取消勾选"
        }
        # 关闭弹出菜单
        [OsWinUi]::PostMessage($main, $WM_CANCELMODE, [IntPtr]0, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 400
    }
    Check (-not $proc.HasExited) "D4 进程存活"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- E. 全部勾选 + 添加变量到数值窗口 -> 自动创建并显示（bug5/6） ----------
$proc = Start-Process -FilePath $ExePath -ArgumentList $ElfFile -PassThru
try {
    $main = Wait-Main $proc.Id 50
    Start-Sleep -Milliseconds 1200
    $tree = Find-ChildByClass $main "SysTreeView32"
    Check ((TreeCount $tree) -eq 3) "E0 ELF 加载后树含 3 个全局变量"
    [OsWinUi]::PostMessage($main, $WM_COMMAND, [IntPtr]$IDM_CTX_CHECK_ALL, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 500
    [OsWinUi]::PostMessage($main, $WM_COMMAND, [IntPtr]$IDM_CTX_ADD_NUM, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 1200
    $tab = Find-ChildByClass $main "SysTabControl32"
    $numwin = Find-ChildByClass $tab "OpenScopeNum"
    Check ($numwin -ne [IntPtr]::Zero) "E1 无数值窗口时自动创建数值窗口（bug5 自动新建）"
    if ($numwin -ne [IntPtr]::Zero) {
        Check ([OsWinUi]::GetDlgCtrlID($numwin) -eq 2351) "E2 自动创建数值窗口控制 ID=2351"
        $list = Find-ChildByClass $numwin "SysListView32"
        $lc = ListCount $list
        Write-Output ("E 数值窗口 ListView 行数 = " + $lc)
        Check ($lc -eq 3) "E3 数值窗口显示 3 个全局变量（bug6 数值窗口）"
    }
    Check (-not $proc.HasExited) "E4 进程存活"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
