# 设备列表回归：jlink/stlink/miniwiggler(DAS)/xcp/网络 统一作为左树设备节点。
#   1. 启动（独立 workspace），默认设备节点已生成
#   2. 通过 WM_OS_DEV_ADD 添加 5 类设备（同类可重复添加，按设备 ID/序列号区分）
#   3. 校验 workspace.json 的 dev_kind/serial/id 字段与设备数量
#   4. 打开设备配置对话框（WM_OS_DEV_CFG），改名并确定 → 持久化
#   5. 校验旧命令栏配置控件已隐藏，只保留“添加设备/连接/采集”等全局按钮
#   6. 重启后设备列表从 workspace.json 恢复
param([string]$ExePath = "")
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsDevUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern int GetDlgCtrlID(IntPtr h);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern bool SetWindowText(IntPtr h, string t);
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
    $cb = { param($h,$l) $wp=[uint32]0; [OsDevUi]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsDevUi]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsDevUi]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-ChildById([IntPtr]$P, [int]$Id) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) if ([OsDevUi]::GetDlgCtrlID($h) -eq $Id) { $script:hit=$h; return $false }; return $true }
    [OsDevUi]::EnumChildWindows($P,$cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-FirstTree([IntPtr]$P) {
    # 变量树/源码树两个 SysTreeView32：选节点数最多的（变量树）
    $script:treehit = [IntPtr]::Zero
    $script:best = -1
    $cb = { param($h,$l) $sb=New-Object System.Text.StringBuilder 64
        [OsDevUi]::GetClassName($h,$sb,64)|Out-Null
        if ($sb.ToString() -eq "SysTreeView32") {
            $n = [int][OsDevUi]::SendMessage($h, 0x1105, [IntPtr]::Zero, [IntPtr]::Zero)
            if ($n -gt $script:best) { $script:best = $n; $script:treehit = $h }
        }
        return $true }
    [OsDevUi]::EnumChildWindows($P,$cb,[IntPtr]::Zero)|Out-Null
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

$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
$ws = Join-Path $env:TEMP "openscope_devices_ws.json"
$layout = Join-Path $env:TEMP "openscope_devices_layout.ini"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
if (Test-Path $ws) { Remove-Item -LiteralPath $ws -Force }
$layoutBody = @"
[layout]
version=1
main_x=100
main_y=100
main_w=1200
main_h=700
tree_w=360
log_h=150
active=0
wins=0
"@
[System.IO.File]::WriteAllText($layout, $layoutBody, (New-Object System.Text.UTF8Encoding $true))
$env:OPENSCOPE_WORKSPACE = $ws
$env:OPENSCOPE_LAYOUT = (Join-Path $env:TEMP "openscope_devices_layout_runtime.ini")

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}
function Start-App {
    $p = Start-Process -FilePath $exe -ArgumentList @("--layout-load=$layout") -PassThru
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 60 -and -not $p.HasExited; $i++) {
        $main = Find-TopByClass $p.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 150
    }
    Start-Sleep -Milliseconds 1000
    return @($p, $main)
}
function Stop-App($p) {
    if ($p -and -not $p.HasExited) { Stop-Process -Id $p.Id -Force; Start-Sleep -Milliseconds 600 }
}

# ---- 第一次启动 ----
$pair = Start-App
$proc = $pair[0]; $main = $pair[1]
Check ($main -ne [IntPtr]::Zero) "主窗口创建"
Check (Wait-Log "设备列表" 6000) "启动时设备列表初始化"
$tree = Find-FirstTree $main
Check ($tree -ne [IntPtr]::Zero) "变量/设备树存在"
$countBefore = 0
for ($i = 0; $i -lt 30; $i++) {
    $tree = Find-FirstTree $main
    $countBefore = [int][OsDevUi]::SendMessage($tree, 0x1105, [IntPtr]::Zero, [IntPtr]::Zero) # TVM_GETCOUNT
    if ($countBefore -gt 0) { break }
    Start-Sleep -Milliseconds 100
}
Check ($countBefore -gt 0) "默认设备节点已显示（$countBefore）"
Write-Output "treecount_before=$countBefore"

# ---- 添加 5 类设备（默认设备可能已覆盖同类，因此再各加一个）----
$kinds = @(1,2,3,4,5)  # JLink STLink DAS XCP NET
foreach ($k in $kinds) {
    [OsDevUi]::PostMessage($main, 0x8046, [IntPtr]$k, [IntPtr]::Zero) | Out-Null
    Start-Sleep -Milliseconds 300
}
Start-Sleep -Milliseconds 800
$added = 0
if (Test-Path $log) { $added = (Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch "已添加设备").Count }
Check ($added -ge 5) "日志记录至少 5 次设备添加（实际 $added）"
$tree = Find-FirstTree $main
$countAfter = [int][OsDevUi]::SendMessage($tree, 0x1105, [IntPtr]::Zero, [IntPtr]::Zero)
Check ($countAfter -gt $countBefore) "树节点增加（$countBefore -> $countAfter）"

# ---- workspace.json 字段校验 ----
Start-Sleep -Milliseconds 500
Check (Test-Path $ws) "workspace.json 已自动保存"
$json = Get-Content $ws -Raw -Encoding UTF8
foreach ($k in $kinds) {
    $m = [regex]::Matches($json, '"dev_kind":' + $k)
    Check ($m.Count -ge 1) "workspace 含 dev_kind=$k 设备（$($m.Count) 个）"
}
Check ($json -match '"serial"') "workspace 含设备 serial 字段"
Check ($json -match '"id":\d+') "workspace 含稳定设备 id"

# ---- 旧工具栏配置控件应隐藏 ----
$legacyIds = @(2101,2102,2103,2104,2105,2106,2110,2111,2120,2121,2122,2123,2124,2125)
$visible = 0
foreach ($id in $legacyIds) {
    $h = Find-ChildById $main $id
    if ($h -ne [IntPtr]::Zero -and [OsDevUi]::IsWindowVisible($h)) { $visible++ ; Write-Output "visible legacy ctrl id=$id" }
}
Check ($visible -eq 0) "旧命令栏配置控件全部隐藏（可见 $visible 个）"
$addBtn = Find-ChildById $main 2014
Check ($addBtn -ne [IntPtr]::Zero -and [OsDevUi]::IsWindowVisible($addBtn)) "“添加设备”按钮可见"

# ---- 设备配置对话框：改名 JLink → MyJLink ----
[OsDevUi]::PostMessage($main, 0x8047, [IntPtr](-1), [IntPtr]::Zero) | Out-Null
$dlg = [IntPtr]::Zero
for ($i = 0; $i -lt 40; $i++) {
    $dlg = Find-TopByClass $proc.Id "OSDlgDevCfg"
    if ($dlg -ne [IntPtr]::Zero) { break }
    Start-Sleep -Milliseconds 100
}
Check ($dlg -ne [IntPtr]::Zero) "设备配置对话框打开"
if ($dlg -ne [IntPtr]::Zero) {
    $nameEdit = Wait-ChildById $dlg 2960
    Check ($nameEdit -ne [IntPtr]::Zero) "设备名称编辑框存在"
    if ($nameEdit -ne [IntPtr]::Zero) { [OsDevUi]::SendMessage($nameEdit, 0x000C, [IntPtr]::Zero, "MyJLink") | Out-Null }
    $ok = Wait-ChildById $dlg 2983
    Check ($ok -ne [IntPtr]::Zero) "确定按钮存在"
    if ($ok -ne [IntPtr]::Zero) {
        # 跨进程 BM_CLICK 受 UIPI/模拟输入限制，直接给对话框发 WM_COMMAND(BN_CLICKED)
        $wp = [IntPtr](2983 -band 0xFFFF)
        [OsDevUi]::SendMessage($dlg, 0x0111, $wp, $ok) | Out-Null
    }
    Start-Sleep -Milliseconds 600
    Check (Wait-Log "设备配置已保存: MyJLink" 4000) "改名已保存并写入日志"
}
$json = Get-Content $ws -Raw -Encoding UTF8
Check ($json -match '"name":"MyJLink"') "改名持久化到 workspace.json"

# ---- 设备配置对话框：为 J-Link 1（下标 0）选择 ELF，确定后立即解析并挂到该设备 ----
[OsDevUi]::PostMessage($main, 0x8047, [IntPtr]0, [IntPtr]::Zero) | Out-Null
$dlg = [IntPtr]::Zero
for ($i = 0; $i -lt 40; $i++) {
    $dlg = Find-TopByClass $proc.Id "OSDlgDevCfg"
    if ($dlg -ne [IntPtr]::Zero) { break }
    Start-Sleep -Milliseconds 100
}
if ($dlg -ne [IntPtr]::Zero) {
    $elfEdit = Wait-ChildById $dlg 2961
    Check ($elfEdit -ne [IntPtr]::Zero) "设备配置含 ELF 编辑框"
    if ($elfEdit -ne [IntPtr]::Zero) {
        $elfPath = Join-Path $PSScriptRoot "elf_sample.out"
        [OsDevUi]::SendMessage($elfEdit, 0x000C, [IntPtr]::Zero, $elfPath) | Out-Null
    }
    $ok = Wait-ChildById $dlg 2983
    if ($ok -ne [IntPtr]::Zero) { [OsDevUi]::SendMessage($dlg, 0x0111, [IntPtr]2983, $ok) | Out-Null }
    Start-Sleep -Milliseconds 700
    Check (Wait-Log "已加载 ELF" 5000) "设备配置对话框加载 ELF 生效"
    $json = Get-Content $ws -Raw -Encoding UTF8
    $elfJson = $elfPath.Replace('\','\\')
    Check ($json -match [regex]::Escape($elfJson)) "设备 ELF 路径持久化"
    $tree = Find-FirstTree $main
    $countElf = [int][OsDevUi]::SendMessage($tree, 0x1105, [IntPtr]::Zero, [IntPtr]::Zero)
    Check ($countElf -gt $countAfter) "设备变量树出现 ELF 变量（$countAfter -> $countElf）"
}

Stop-App $proc

# ---- 重启：设备列表从 workspace 恢复 ----
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$pair = Start-App
$proc = $pair[0]; $main = $pair[1]
Check ($main -ne [IntPtr]::Zero) "重启主窗口创建"
Check (Wait-Log "设备列表已恢复" 6000) "重启后设备列表从 workspace 恢复"
Stop-App $proc

Write-Output ("RESULT: {0}" -f ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES($fails)" })))
if ($fails -ne 0) { exit 1 }
exit 0
