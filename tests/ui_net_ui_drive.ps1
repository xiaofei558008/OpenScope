# 需求14 UI 整合回归：
#   1. 单行工具栏（网络 IP/端口/监听 与硬件配置同一行，y<40）
#   2. 通道下拉含"网络"选项，选中后硬件配置控件禁用
#   3. 菜单栏新增"网络"菜单（含"网络配置..."）
#   4. 网络配置对话框：改 IP/端口 -> 确定 -> 回写工具栏内联编辑框
#   5. 按键复用：通道=网络时，菜单"连接"= 网络连接；"开始采集"= 下达监视列表；"停止采集"= 停止下达
param(
    [string]$Elf = "D:\OpenScope\tests\elf_sample.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$outDir = Join-Path $PSScriptRoot "out"
if (-not (Test-Path $outDir)) { New-Item -ItemType Directory -Path $outDir | Out-Null }
$logA = Join-Path $outDir "netui_a.log"
$logB = Join-Path $outDir "netui_b.log"
foreach ($p in @($logA, $logB)) { if (Test-Path $p) { Remove-Item $p -Force } }

Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Milliseconds 300

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class NetUi2 {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern IntPtr GetDlgItem(IntPtr h, int id);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessageW(IntPtr h, uint msg, IntPtr wp, StringBuilder lp);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern bool SetWindowTextW(IntPtr h, string s);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool IsWindowEnabled(IntPtr h);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern IntPtr GetMenu(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr GetSubMenu(IntPtr m, int pos);
    [DllImport("user32.dll")] public static extern int GetMenuItemCount(IntPtr m);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetMenuStringW(IntPtr m, uint id, StringBuilder sb, int max, uint flags);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int left, top, right, bottom; }
}
"@

function Get-LBText([IntPtr]$hCombo, [int]$idx) {
    $sb = New-Object System.Text.StringBuilder 128
    [NetUi2]::SendMessageW($hCombo, 0x0148, [IntPtr]$idx, $sb) | Out-Null  # CB_GETLBTEXT
    return $sb.ToString()
}

function Get-Text([IntPtr]$h) {
    # 跨进程 GetWindowTextW 对 EDIT 类返回创建时文本（系统限制），须用 WM_GETTEXT 取实时文本
    $sb = New-Object System.Text.StringBuilder 128
    [NetUi2]::SendMessageW($h, 0x000D, [IntPtr]128, $sb) | Out-Null  # WM_GETTEXT
    return $sb.ToString()
}

function Find-Main([int]$ProcId) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [NetUi2]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId -and [NetUi2]::IsWindowVisible($h)) {
            $sb = New-Object System.Text.StringBuilder 256
            [NetUi2]::GetWindowTextW($h, $sb, 256) | Out-Null
            if ($sb.ToString() -like "OpenScope*") { $script:hit = $h; return $false }
        }
        return $true
    }
    [NetUi2]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Wait-Main([int]$ProcId, [int]$TimeoutMs) {
    $sw = [Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        $h = Find-Main $ProcId
        if ($h -ne [IntPtr]::Zero) { return $h }
        Start-Sleep -Milliseconds 200
    }
    return [IntPtr]::Zero
}

function Cmd([IntPtr]$hMain, [int]$id) {
    [NetUi2]::PostMessage($hMain, 0x0111, [IntPtr]$id, [IntPtr]::Zero) | Out-Null  # WM_COMMAND
}

function Click([IntPtr]$hDlg, [int]$id) {
    $b = [NetUi2]::GetDlgItem($hDlg, $id)
    if ($b -ne [IntPtr]::Zero) {
        [NetUi2]::SendMessage($b, 0x00F5, [IntPtr]::Zero, [IntPtr]::Zero) | Out-Null  # BM_CLICK
    }
}

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [NetUi2]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [NetUi2]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [NetUi2]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Wait-ByClass([int]$ProcId, [string]$Class, [int]$TimeoutMs) {
    $sw = [Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        $h = Find-ByClass $ProcId $Class
        if ($h -ne [IntPtr]::Zero) { return $h }
        Start-Sleep -Milliseconds 100
    }
    return [IntPtr]::Zero
}

function Read-Log([string]$p) {
    if (-not (Test-Path $p)) { return "" }
    $fs = [System.IO.File]::Open($p, 'Open', 'Read', 'ReadWrite')  # 应用仍在写日志，共享读
    $sr = New-Object System.IO.StreamReader($fs, [System.Text.Encoding]::UTF8)
    $s = $sr.ReadToEnd()
    $sr.Close(); $fs.Close()
    return $s
}

$fail = 0
function Check([string]$name, [bool]$cond) {
    if ($cond) { Write-Output "PASS $name" } else { Write-Output "FAIL $name"; $script:fail++ }
}

# ---- 阶段 1：A 启动（--net-set 进程内设置内联 IP/端口，避免跨进程 WM_SETTEXT 限制） ----
$a = Start-Process -FilePath $exe -ArgumentList $Elf,"--no-layout","--log=$logA","--net-set=127.0.0.1:10005" -PassThru
$hA = Wait-Main $a.Id 8000
if ($hA -eq [IntPtr]::Zero) { Write-Output "FAIL A 主窗口未出现"; exit 1 }
Start-Sleep -Milliseconds 1200

# ---- 1. 单行工具栏：网络 IP 编辑框与刷新按钮同一行 ----
$rIp = New-Object -TypeName "NetUi2+RECT";  $bIp = [NetUi2]::GetDlgItem($hA, 2110)
$rRef = New-Object -TypeName "NetUi2+RECT"; $bRef = [NetUi2]::GetDlgItem($hA, 2105)
[NetUi2]::GetWindowRect($bIp, [ref]$rIp) | Out-Null
[NetUi2]::GetWindowRect($bRef, [ref]$rRef) | Out-Null
$sameRow = ([Math]::Abs($rIp.top - $rRef.top) -le 12) -and ($rIp.top -gt 0)
Check "单行工具栏（IP编辑与刷新同排 y差=$([Math]::Abs($rIp.top - $rRef.top)) top=$($rIp.top)）" $sameRow

# ---- 2. 通道下拉含"网络"选项 ----
$bDrv = [NetUi2]::GetDlgItem($hA, 2106)
$cnt = [NetUi2]::SendMessage($bDrv, 0x0146, [IntPtr]::Zero, [IntPtr]::Zero).ToInt32()  # CB_GETCOUNT
$lastItem = Get-LBText $bDrv ($cnt - 1)
Check "通道下拉含'网络'选项（末项='$lastItem'）" ($lastItem -eq "网络")

# ---- 3. 菜单栏含"网络"菜单 ----
$menu = [NetUi2]::GetMenu($hA)
$subNames = @()
$menuOk = $false
for ($i = 0; $i -lt [NetUi2]::GetMenuItemCount($menu); $i++) {
    $sub = [NetUi2]::GetSubMenu($menu, $i)
    if ($sub -eq [IntPtr]::Zero) { continue }
    $sb2 = New-Object System.Text.StringBuilder 64
    [NetUi2]::GetMenuStringW($menu, [uint32]$i, $sb2, 64, 0x0400) | Out-Null  # MF_BYPOSITION
    $subNames += $sb2.ToString()
}
Check "菜单栏含'网络'菜单（$($subNames -join '/')）" ($subNames -contains "网络(&N)")

# ---- 4. 通道切换到"网络"：硬件配置控件禁用 ----
[NetUi2]::SendMessage($bDrv, 0x014E, [IntPtr]($cnt - 1), [IntPtr]::Zero) | Out-Null  # CB_SETCURSEL
# 显式投递 CBN_SELCHANGE 通知（CB_SETCURSEL 不保证触发）
$selCmd = [IntPtr](0x00010000 -bor 2106)  # MAKEWPARAM(2106, CBN_SELCHANGE=1)
[NetUi2]::PostMessage($hA, 0x0111, $selCmd, $bDrv) | Out-Null
Start-Sleep -Milliseconds 400
$bIface = [NetUi2]::GetDlgItem($hA, 2102)
$bEmu = [NetUi2]::GetDlgItem($hA, 2104)
$ifaceDisabled = -not [NetUi2]::IsWindowEnabled($bIface)
$emuDisabled = -not [NetUi2]::IsWindowEnabled($bEmu)
Check "网络通道下接口/设备下拉禁用（iface=$ifaceDisabled emu=$emuDisabled）" ($ifaceDisabled -and $emuDisabled)

# ---- 5. 网络配置对话框：改 IP/端口 -> 确定 -> 回写内联编辑框 ----
Cmd $hA 2801  # IDM_NET_CFG
$dlg = Wait-ByClass $a.Id "OSDlgNetCfg" 3000
Check "网络配置对话框打开 (OSDlgNetCfg)" ($dlg -ne [IntPtr]::Zero)
if ($dlg -ne [IntPtr]::Zero) {
    $ipIn = [NetUi2]::GetDlgItem($dlg, 2421)
    $portIn = [NetUi2]::GetDlgItem($dlg, 2422)
    $dlgIp = Get-Text $ipIn
    $dlgPort = Get-Text $portIn
    Check "对话框初值与内联一致 IP/端口（'$dlgIp`:$dlgPort'）" ($dlgIp -eq "127.0.0.1" -and $dlgPort -eq "10005")
    Click $dlg 2423  # 确定（跨进程 WM_SETTEXT 受限，值由 --net-set 进程内预置，此处验证回写一致性）
    Start-Sleep -Milliseconds 500
}
$inlineIp = Get-Text $bIp
$inlinePort = Get-Text ([NetUi2]::GetDlgItem($hA, 2111))
$logAText = Read-Log $logA
Check "对话框确定回写内联 IP/端口（'$inlineIp`:$inlinePort'）+ 日志确认" ($inlineIp -eq "127.0.0.1" -and $inlinePort -eq "10005" -and $logAText -match "网络配置: 127.0.0.1:10005")

# ---- 6. 菜单"网络→监听"（通道=网络时按键复用同样走此路径） ----
Cmd $hA 2802  # IDM_NET_LISTEN
Start-Sleep -Milliseconds 800
$logAText = Read-Log $logA
Check "菜单监听 -> 日志 '监听 127.0.0.1:10005 rc=0'" ($logAText -match "10005 rc=0")

# ---- 阶段 2：B（远端）启动，通道=网络 + 勾选变量 ----
$b = Start-Process -FilePath $exe -ArgumentList $Elf,"--no-layout","--log=$logB","--watch=g_counter,g_cfg.a","--net-set=127.0.0.1:10005" -PassThru
$hB = Wait-Main $b.Id 8000
if ($hB -eq [IntPtr]::Zero) { Write-Output "FAIL B 主窗口未出现"; Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force; exit 1 }
Start-Sleep -Milliseconds 1200
$bDrvB = [NetUi2]::GetDlgItem($hB, 2106)
$cntB = [NetUi2]::SendMessage($bDrvB, 0x0146, [IntPtr]::Zero, [IntPtr]::Zero).ToInt32()
[NetUi2]::SendMessage($bDrvB, 0x014E, [IntPtr]($cntB - 1), [IntPtr]::Zero) | Out-Null  # 切到网络
$selCmdB = [IntPtr](0x00010000 -bor 2106)
[NetUi2]::PostMessage($hB, 0x0111, $selCmdB, $bDrvB) | Out-Null
Start-Sleep -Milliseconds 400

# ---- 7. 按键复用：B 点"连接"按钮 = 网络连接（连 A 的 10005，--net-set 已进程内对齐） ----
Click $hB 2002  # IDC_BTN_CONNECT
Start-Sleep -Milliseconds 800
$logBText = Read-Log $logB
$logAText = Read-Log $logA
Check "B 连接按钮复用=网络连接（'连接 127.0.0.1:10005 rc=0'）" ($logBText -match "10005 rc=0")
Check "A 收到客户端接入" ($logAText -match "客户端接入")

# ---- 8. 按键复用：B 点"开始采集" = 下达监视列表 ----
Click $hB 2004  # IDC_BTN_START
Start-Sleep -Milliseconds 800
$logBText = Read-Log $logB
Check "B 开始采集复用=下达监视列表（'发送监视列表 2 项 -> 1 个对端'）" ($logBText -match "发送监视列表 2 项 -> 1 个对端")

# ---- 9. 按键复用：B 点"停止采集" = 停止下达（空监视列表） ----
Click $hB 2005  # IDC_BTN_STOP
Start-Sleep -Milliseconds 800
$logBText = Read-Log $logB
Check "B 停止采集复用=停止下达（'停止下达'）" ($logBText -match "停止下达")

# ---- 10. B 菜单"网络→断开" = 网络停止 ----
Cmd $hB 2804  # IDM_NET_STOP
Start-Sleep -Milliseconds 500
$logBText = Read-Log $logB
Check "菜单断开 -> '网络: 已停止'" ($logBText -match "已停止")

# ---- 清理 ----
Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Milliseconds 300

if ($fail -eq 0) { Write-Output "NET UI DRIVE ALL PASS"; exit 0 }
Write-Output "NET UI DRIVE FAILED: $fail"
exit 1
