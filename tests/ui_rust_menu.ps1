# Rust 版 bug1 回归（request.md "rust开发 bug" #1：没有实现之前C代码实现的菜单栏）
#   A. 主窗口存在菜单栏（GetMenu != 0），顶层菜单=文件/采集/记录回放/窗口/帮助（复刻 C 版）
#   B. 采集菜单各项文本/快捷键与 C 版一致（连接..F5/断开F6/开始采集F7/停止采集F8）
#   C. 菜单"关于"-> 弹关于对话框（标题"关于"）-> OK 关闭
#   D. 菜单"打开 ELF"(IDM_OPEN_ELF=9003) -> 弹文件对话框 -> 取消关闭
#   E. 菜单"退出"(IDM_EXIT=9002) -> 窗口销毁进程退出
#   F. 快捷键 Ctrl+O -> 弹文件对话框（TranslateAccelerator 生效；前台自动化尽力而为）
param(
    [string]$ExePath = "D:/OpenScope/rust/target/release/openscope-app.exe"
)
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsMenu {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr GetMenu(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr GetSubMenu(IntPtr hMenu, int nPos);
    [DllImport("user32.dll")] public static extern int GetMenuItemCount(IntPtr hMenu);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetMenuString(IntPtr hMenu, uint uIDItem, StringBuilder lpString, int nMaxCount, uint uFlag);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
$MF_BYPOSITION = 0x400

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsMenu]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsMenu]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsMenu]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function MenuText([IntPtr]$menu, [uint32]$pos) {
    $sb = New-Object System.Text.StringBuilder 256
    [OsMenu]::GetMenuString($menu, $pos, $sb, 256, $MF_BYPOSITION) | Out-Null
    return $sb.ToString()
}
function MenuItems([IntPtr]$menu) {
    $list = @()
    $n = [OsMenu]::GetMenuItemCount($menu)
    for ($i = 0; $i -lt $n; $i++) { $list += (MenuText $menu $i) }
    return $list
}
function Find-Dlg([int]$ProcId) { return Find-ByClass $ProcId "#32770" }
function Wait-Dlg([int]$ProcId, [int]$tries) {
    for ($i = 0; $i -lt $tries; $i++) {
        $d = Find-Dlg $ProcId
        if ($d -ne [IntPtr]::Zero) { return $d }
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

# ---------- A. 菜单栏存在 + 顶层菜单 ----------
$proc = Start-Process -FilePath $ExePath -PassThru
$main = [IntPtr]::Zero
try {
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 600
    $menu = [OsMenu]::GetMenu($main)
    Check ($menu -ne [IntPtr]::Zero) "A1 主窗口存在菜单栏（bug1 修复）"
    if ($menu -ne [IntPtr]::Zero) {
        $items = MenuItems $menu
        Write-Output ("A 顶层菜单: " + ($items -join " | "))
        Check ($items.Count -eq 5) "A2 顶层菜单 5 项"
        Check (($items -contains "文件(&F)") -and ($items -contains "采集(&A)") -and ($items -contains "记录/回放(&L)") -and ($items -contains "窗口(&W)") -and ($items -contains "帮助(&H)")) "A3 五个菜单名与 C 版一致"
    }
    # B. 采集菜单内容（子菜单 = GetSubMenu）
    if ($menu -ne [IntPtr]::Zero) {
        $sub = [OsMenu]::GetSubMenu($menu, 1)  # 采集(&A) 索引 1
        if ($sub -ne [IntPtr]::Zero) {
            $acqItems = MenuItems $sub
            Write-Output ("B 采集菜单: " + ($acqItems -join " | "))
            Check (($acqItems -contains "连接...`tF5") -and ($acqItems -contains "断开`tF6") -and ($acqItems -contains "开始采集`tF7") -and ($acqItems -contains "停止采集`tF8")) "B1 采集菜单项/快捷键与 C 版一致"
        }
    }
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- C. 菜单"关于" ----------
$proc = Start-Process -FilePath $ExePath -PassThru
try {
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Start-Sleep -Milliseconds 600
    [OsMenu]::PostMessage($main, 0x111, [IntPtr]9001, [IntPtr]0) | Out-Null  # IDM_ABOUT
    $dlg = Wait-Dlg $proc.Id 40
    Check ($dlg -ne [IntPtr]::Zero) "C1 菜单关于 -> 弹出关于对话框"
    if ($dlg -ne [IntPtr]::Zero) {
        $sb = New-Object System.Text.StringBuilder 128
        [OsMenu]::GetWindowText($dlg, $sb, 128) | Out-Null
        Check ($sb.ToString() -eq "关于") "C2 关于对话框标题为「关于」"
        # 同步 SendMessage WM_COMMAND IDOK 关闭 MessageBox（PostMessage 在模态对话框上不可靠）
        [OsMenu]::SendMessage($dlg, 0x111, [IntPtr]1, [IntPtr]0) | Out-Null  # IDOK
        for ($i = 0; $i -lt 20; $i++) {
            if ((Find-Dlg $proc.Id) -eq [IntPtr]::Zero) { break }
            Start-Sleep -Milliseconds 200
        }
        if ((Find-Dlg $proc.Id) -ne [IntPtr]::Zero) {
            [OsMenu]::SendMessage($dlg, 0x10, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_CLOSE 兜底
            Start-Sleep -Milliseconds 400
        }
        Check ((Find-Dlg $proc.Id) -eq [IntPtr]::Zero) "C3 OK 后关于对话框关闭"
    }
    Check (-not $proc.HasExited) "C4 进程存活"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- D. 菜单"打开 ELF" ----------
$proc = Start-Process -FilePath $ExePath -PassThru
try {
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Start-Sleep -Milliseconds 600
    [OsMenu]::PostMessage($main, 0x111, [IntPtr]9003, [IntPtr]0) | Out-Null  # IDM_OPEN_ELF
    $dlg = Wait-Dlg $proc.Id 40
    Check ($dlg -ne [IntPtr]::Zero) "D1 菜单打开ELF -> 弹出文件对话框"
    if ($dlg -ne [IntPtr]::Zero) {
        [OsMenu]::PostMessage($dlg, 0x111, [IntPtr]2, [IntPtr]0) | Out-Null  # IDCANCEL
        for ($i = 0; $i -lt 20; $i++) {
            if ((Find-Dlg $proc.Id) -eq [IntPtr]::Zero) { break }
            Start-Sleep -Milliseconds 200
        }
        Check ((Find-Dlg $proc.Id) -eq [IntPtr]::Zero) "D2 取消后文件对话框关闭"
    }
    Check (-not $proc.HasExited) "D3 进程存活"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- E. 菜单"退出" ----------
$proc = Start-Process -FilePath $ExePath -PassThru
try {
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Start-Sleep -Milliseconds 600
    [OsMenu]::PostMessage($main, 0x111, [IntPtr]9002, [IntPtr]0) | Out-Null  # IDM_EXIT
    for ($i = 0; $i -lt 40 -and -not $proc.HasExited; $i++) { Start-Sleep -Milliseconds 200 }
    Check ($proc.HasExited) "E1 菜单退出 -> 进程退出"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- F. 快捷键 Ctrl+O（尽力而为，失败仅 WARN） ----------
$proc = Start-Process -FilePath $ExePath -PassThru
try {
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Start-Sleep -Milliseconds 600
    [OsMenu]::SetForegroundWindow($main) | Out-Null
    Add-Type -AssemblyName System.Windows.Forms
    Add-Type -AssemblyName System.Drawing
    Start-Sleep -Milliseconds 300
    try { [System.Windows.Forms.SendKeys]::SendWait("^o") } catch { }
    $dlg = Wait-Dlg $proc.Id 25
    if ($dlg -ne [IntPtr]::Zero) {
        Write-Output "PASS F1 快捷键 Ctrl+O -> 弹出文件对话框"
        [OsMenu]::PostMessage($dlg, 0x111, [IntPtr]2, [IntPtr]0) | Out-Null
    } else {
        Write-Output "WARN F1 快捷键 Ctrl+O 未触发对话框（前台自动化环境限制，功能路径已在 D 验证）"
    }
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
