# 每设备独立符号源 + 变量列表 + 模糊搜索回归：
#   1. 预置 workspace：两个 J-Link 设备，分别加载 elf_sample.out / elf_sample_v2.out
#   2. 启动后两个设备各自合并 ELF，左侧树按设备分组展示变量
#   3. Ctrl+F 搜索框：普通变量名、设备前缀 "J2::g_extra" 都能命中；
#      "J1::g_extra" 不命中（证明搜索按设备隔离）
param([string]$ExePath = "")
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsDevSym {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern int GetDlgCtrlID(IntPtr h);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, string lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr GetDlgItem(IntPtr h, int id);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
function Find-TopByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsDevSym]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsDevSym]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsDevSym]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-ChildByClass([IntPtr]$P, [string]$Class) {
    $script:hitc = [IntPtr]::Zero
    $cb = { param($h,$l) $sb=New-Object System.Text.StringBuilder 128
        [OsDevSym]::GetClassName($h,$sb,128)|Out-Null
        if ($sb.ToString() -eq $Class) { $script:hitc=$h; return $false }
        return $true }
    [OsDevSym]::EnumChildWindows($P,$cb,[IntPtr]::Zero)|Out-Null
    return $script:hitc
}
function Wait-ChildByClass([IntPtr]$P, [string]$Class, [int]$TimeoutMs = 4000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        $h = Find-ChildByClass $P $Class
        if ($h -ne [IntPtr]::Zero) { return $h }
        Start-Sleep -Milliseconds 80
    }
    return [IntPtr]::Zero
}
function Find-ChildById([IntPtr]$P, [int]$Id) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) if ([OsDevSym]::GetDlgCtrlID($h) -eq $Id) { $script:hit=$h; return $false }; return $true }
    [OsDevSym]::EnumChildWindows($P,$cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-VisibleTree([IntPtr]$P) {
    # 进程内有变量树/源码树两个 SysTreeView32；选节点数最多的那个（变量树）
    $script:treehit = [IntPtr]::Zero
    $script:best = -1
    $cb = { param($h,$l) $sb=New-Object System.Text.StringBuilder 64
        [OsDevSym]::GetClassName($h,$sb,64)|Out-Null
        if ($sb.ToString() -eq "SysTreeView32") {
            $n = [int][OsDevSym]::SendMessage($h, 0x1105, [IntPtr]::Zero, [IntPtr]::Zero)
            if ($n -gt $script:best) { $script:best = $n; $script:treehit = $h }
        }
        return $true }
    [OsDevSym]::EnumChildWindows($P,$cb,[IntPtr]::Zero)|Out-Null
    return $script:treehit
}
function Wait-ChildById([IntPtr]$P, [int]$Id, [int]$TimeoutMs = 4000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        $h = Find-ChildById $P $Id
        if ($h -ne [IntPtr]::Zero) { return $h }
        Start-Sleep -Milliseconds 80
    }
    return [IntPtr]::Zero
}
function Has-Log([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    return [bool](Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch $Pattern -Quiet)
}
function Wait-Log([string]$Pattern, [int]$TimeoutMs = 8000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        if (Has-Log $Pattern) { return $true }
        Start-Sleep -Milliseconds 150
    }
    return $false
}
function Search-Count([IntPtr]$dlg, [string]$needle) {
    $edit = [OsDevSym]::GetDlgItem($dlg, 2403)
    [OsDevSym]::SendMessage($edit, 0x000C, [IntPtr]::Zero, $needle) | Out-Null
    Start-Sleep -Milliseconds 400
    $list = [OsDevSym]::GetDlgItem($dlg, 2404)
    return [int][OsDevSym]::SendMessage($list, 0x1004, [IntPtr]::Zero, [IntPtr]::Zero) # LVM_GETITEMCOUNT
}

$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
$ws = Join-Path $env:TEMP "openscope_symbols_ws.json"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$elf1 = (Join-Path $PSScriptRoot "elf_sample.out").Replace('\','\\')
$elf2 = (Join-Path $PSScriptRoot "elf_sample_v2.out").Replace('\','\\')
$map1Path = Join-Path $env:TEMP "openscope_symbols_j1.map"
$mapBody = @"
Memory Configuration
Name             Origin             Length             Attributes
FLASH            0x0000000008000000 0x0000000000080000 xr

Linker script and memory map
 .bss           0x0000000020000000      0x400
                0x0000000020000000                g_counter
                0x0000000020000030                g_map_only
"@
[System.IO.File]::WriteAllText($map1Path, $mapBody, (New-Object System.Text.UTF8Encoding $false))
$map1 = $map1Path.Replace('\','\\')
$wsBody = @"
{
  "version": 2,
  "name": "Symbols",
  "active": 0,
  "projects": [
    {"id":1,"name":"J1","kind":0,"dev_kind":1,"driver_index":-1,"elf":"$elf1","a2l":"","map":"$map1","iface":0,"speed_khz":4000,"probe_index":-1,"serial":"","device":"Cortex-M4","net_ip":"127.0.0.1","net_port":10000,"net_mode":0},
    {"id":2,"name":"J2","kind":0,"dev_kind":1,"driver_index":-1,"elf":"$elf2","a2l":"","map":"","iface":0,"speed_khz":4000,"probe_index":-1,"serial":"","device":"Cortex-M4","net_ip":"127.0.0.1","net_port":10000,"net_mode":0}
  ]
}
"@
[System.IO.File]::WriteAllText($ws, $wsBody, (New-Object System.Text.UTF8Encoding $false))
$env:OPENSCOPE_WORKSPACE = $ws

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

$proc = Start-Process -FilePath $exe -ArgumentList @("--no-layout") -PassThru
$main = [IntPtr]::Zero
for ($i = 0; $i -lt 60 -and -not $proc.HasExited; $i++) {
    $main = Find-TopByClass $proc.Id "OpenScopeMain"
    if ($main -ne [IntPtr]::Zero) { break }
    Start-Sleep -Milliseconds 150
}
Start-Sleep -Milliseconds 1200
Check ($main -ne [IntPtr]::Zero) "主窗口创建"
Check (Wait-Log "设备列表已恢复: 2" 6000) "两个设备从 workspace 恢复"
Check (Wait-Log "Project J1 合并 ELF" 6000) "J1 加载自己的 ELF"
Check (Wait-Log "Project J2 合并 ELF" 6000) "J2 加载自己的 ELF"

$tree = Find-VisibleTree $main
Check ($tree -ne [IntPtr]::Zero) "变量树存在"
$treeCount = [int][OsDevSym]::SendMessage($tree, 0x1105, [IntPtr]::Zero, [IntPtr]::Zero)
Check ($treeCount -gt 9) "树按设备分组展示两套变量（节点数 $treeCount）"

# Ctrl+F 搜索对话框（IDM_TREE_FIND=2703）
[OsDevSym]::PostMessage($main, 0x0111, [IntPtr]2703, [IntPtr]::Zero) | Out-Null
$dlg = [IntPtr]::Zero
for ($i = 0; $i -lt 40; $i++) {
    $dlg = Find-TopByClass $proc.Id "OSDlgPick"
    if ($dlg -ne [IntPtr]::Zero) { break }
    Start-Sleep -Milliseconds 100
}
Check ($dlg -ne [IntPtr]::Zero) "变量模糊搜索对话框打开"
if ($dlg -ne [IntPtr]::Zero) {
    Check ((Wait-ChildById $dlg 2403) -ne [IntPtr]::Zero) "搜索输入框存在"
    $nExtra = Search-Count $dlg "g_extra"
    Check ($nExtra -ge 1) "搜索结果：普通变量名 g_extra 命中 $nExtra 项"
    $nCfg = Search-Count $dlg "cfg"
    Check ($nCfg -ge 4) "模糊搜索 cfg 同时命中两个设备（$nCfg 项）"
    $nJ2 = Search-Count $dlg "J2::g_extra"
    Check ($nJ2 -ge 1) "设备前缀搜索 J2::g_extra 命中 $nJ2 项"
    $nJ1 = Search-Count $dlg "J1::g_extra"
    Check ($nJ1 -eq 0) "设备前缀搜索 J1::g_extra 不命中（结果按设备隔离，$nJ1 项）"
    $nMap = Search-Count $dlg "g_map_only"
    Check ($nMap -ge 1) "J1 独立加载的 MAP 符号 g_map_only 可搜索到（$nMap 项）"
    $nMapJ1 = Search-Count $dlg "J1::g_map_only"
    Check ($nMapJ1 -ge 1) "MAP 符号归属 J1（J1::g_map_only 命中 $nMapJ1 项）"
    $cancel = [OsDevSym]::GetDlgItem($dlg, 2402)
    [OsDevSym]::SendMessage($dlg, 0x0111, [IntPtr]2402, $cancel) | Out-Null
    Start-Sleep -Milliseconds 300
}

# ---- 混源窗口：同一波形/数值窗口同时接收 J1(g_counter, id=0) 与 J2(g_extra, id=5) ----
[OsDevSym]::SendMessage($main, 0x0111, [IntPtr]2012, [IntPtr]::Zero) | Out-Null
Start-Sleep -Milliseconds 400
$chart = Wait-ChildByClass $main "OSChartWin"
Check ($chart -ne [IntPtr]::Zero) "波形窗口创建"
if ($chart -ne [IntPtr]::Zero) {
    [OsDevSym]::SendMessage($main, 0x8023, $chart, [IntPtr]0) | Out-Null   # WM_OS_WIN_ADD_VAR: J1 g_counter
    [OsDevSym]::SendMessage($main, 0x8023, $chart, [IntPtr]5) | Out-Null   # WM_OS_WIN_ADD_VAR: J2 g_extra
    Start-Sleep -Milliseconds 300
    $q = [OsDevSym]::SendMessage($chart, 0x8029, [IntPtr]::Zero, [IntPtr]::Zero).ToInt64() # WM_OS_CHART_QUERY
    $series = ($q -shr 16) -band 0xFFFF
    Check ($series -ge 2) "同一波形窗口含两个设备源（series=$series）"
}
[OsDevSym]::SendMessage($main, 0x0111, [IntPtr]2013, [IntPtr]::Zero) | Out-Null
Start-Sleep -Milliseconds 400
$num = Wait-ChildByClass $main "OSNumWin"
Check ($num -ne [IntPtr]::Zero) "数值窗口创建"
if ($num -ne [IntPtr]::Zero) {
    [OsDevSym]::SendMessage($main, 0x8023, $num, [IntPtr]0) | Out-Null
    [OsDevSym]::SendMessage($main, 0x8023, $num, [IntPtr]5) | Out-Null
    Start-Sleep -Milliseconds 300
    $lv = Find-ChildByClass $num "SysListView32"
    $rows = 0
    if ($lv -ne [IntPtr]::Zero) { $rows = [int][OsDevSym]::SendMessage($lv, 0x1004, [IntPtr]::Zero, [IntPtr]::Zero) }
    Check ($rows -ge 2) "同一数值窗口含两个设备源（rows=$rows）"
}
if (-not $proc.HasExited) { Stop-Process -Id $proc.Id -Force }
Write-Output ("RESULT: {0}" -f ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES($fails)" })))
if ($fails -ne 0) { exit 1 }
exit 0
