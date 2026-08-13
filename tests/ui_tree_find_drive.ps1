# OpenScope Ctrl+F 快速搜索变量回归：
#   1. 真实键盘 Ctrl+F（keybd_event）-> 加速键表 -> 弹窗"搜索变量（模糊搜索·Ctrl+F）"
#   2. 编辑框输入模糊关键字 -> 列表刷新
#   3. WM_OS_PICK_TEST_SELECT 选中前 2 项
#   4. 测试钩子添加（等价右键菜单）：波形窗口 2 个 + 数值窗口 2 个（对话框保持打开）
#   5. 列表右键菜单出现（#32768）后关闭
#   6. 确定 -> 日志 "搜索定位变量: 选中 2 个，定位 2 个" + 变量树展开可见
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
public class OsTreeFindUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] public static extern bool AttachThreadInput(uint a, uint b, bool attach);
    [DllImport("kernel32.dll")] public static extern uint GetCurrentThreadId();
    [DllImport("user32.dll", EntryPoint="GetWindowThreadProcessId")] public static extern uint GetWindowThreadProcessId2(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, UIntPtr extra);
    public const uint WM_GETTEXT = 0x0D;
    public const uint WM_SETTEXT = 0x0C;
    public const uint WM_COMMAND = 0x111;
    public const byte VK_CONTROL = 0x11;
    public const byte VK_F = 0x46;
    public const uint KEYUP = 0x2;
    public const uint WM_CONTEXTMENU = 0x7B;
    public const uint WM_CANCELMODE = 0x1F;
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsTreeFindUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsTreeFindUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsTreeFindUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsTreeFindUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsTreeFindUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-Dlg([int]$ProcId) {
    # 弹窗是顶层窗口（WS_POPUP），类名 OSDlgPick
    return Find-ByClass $ProcId "OSDlgPick"
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
    # 等模块加载/J-Link 扫描完成（ShowWindow 在模块加载后才执行），避免过早注入按键
    Start-Sleep -Milliseconds 2000

    # ---- 1. 真实 Ctrl+F 键盘输入 -> 加速键表 -> 搜索弹窗 ----
    # （与 ui_help_drive 的 F1 注入同一模式：挂接当前前台窗口线程输入后 SetForegroundWindow）
    $fg = [OsTreeFindUi]::GetForegroundWindow()
    $fgTid = [uint32]0
    if ($fg -ne [IntPtr]::Zero) { [OsTreeFindUi]::GetWindowThreadProcessId2($fg, [ref]$fgTid) | Out-Null }
    $myTid = [OsTreeFindUi]::GetCurrentThreadId()
    if ($fgTid -ne 0 -and $fgTid -ne $myTid) {
        [OsTreeFindUi]::AttachThreadInput($myTid, $fgTid, $true) | Out-Null
        [OsTreeFindUi]::SetForegroundWindow($main) | Out-Null
        [OsTreeFindUi]::AttachThreadInput($myTid, $fgTid, $false) | Out-Null
    } else {
        [OsTreeFindUi]::SetForegroundWindow($main) | Out-Null
    }
    Start-Sleep -Milliseconds 400
    [OsTreeFindUi]::keybd_event([OsTreeFindUi]::VK_CONTROL, 0, 0, [UIntPtr]::Zero)
    [OsTreeFindUi]::keybd_event([OsTreeFindUi]::VK_F, 0, 0, [UIntPtr]::Zero)
    [OsTreeFindUi]::keybd_event([OsTreeFindUi]::VK_F, 0, [OsTreeFindUi]::KEYUP, [UIntPtr]::Zero)
    [OsTreeFindUi]::keybd_event([OsTreeFindUi]::VK_CONTROL, 0, [OsTreeFindUi]::KEYUP, [UIntPtr]::Zero)
    $dlg = [IntPtr]::Zero
    for ($i = 0; $i -lt 25 -and $dlg -eq [IntPtr]::Zero; $i++) {
        Start-Sleep -Milliseconds 200
        $dlg = Find-Dlg $proc.Id
    }
    if ($dlg -eq [IntPtr]::Zero) {
        # 前台注入失败诊断：列出被测进程全部顶层窗口
        $cbD = {
            param($h, $l)
            $wp = [uint32]0
            [OsTreeFindUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
            if ($wp -eq $proc.Id) {
                $sb = New-Object System.Text.StringBuilder 256
                [OsTreeFindUi]::GetClassName($h, $sb, 256) | Out-Null
                Write-Output "  [diag] win: $($sb.ToString())"
            }
            return $true
        }
        [OsTreeFindUi]::EnumWindows($cbD, [IntPtr]::Zero) | Out-Null
    }
    Check ($dlg -ne [IntPtr]::Zero) "Ctrl+F 弹出搜索对话框（OSDlgPick）"

    if ($dlg -ne [IntPtr]::Zero) {
        $sb = New-Object System.Text.StringBuilder 256
        [OsTreeFindUi]::GetWindowText($dlg, $sb, 256) | Out-Null
        Check ($sb.ToString() -match '搜索变量') "弹窗标题为搜索变量（$($sb.ToString())）"

        # ---- 2. 输入模糊关键字 -> 列表刷新 ----
        $edit = Find-ChildByClass $dlg "Edit"
        if ($edit -ne [IntPtr]::Zero) {
            [OsTreeFindUi]::SendMessage($edit, [OsTreeFindUi]::WM_SETTEXT, [IntPtr]0, [IntPtr]([System.Runtime.InteropServices.Marshal]::StringToHGlobalUni("fsin"))) | Out-Null
        }
        Start-Sleep -Milliseconds 300

        # ---- 3. 测试钩子选中前 2 项 ----
        # WM_OS_PICK_TEST_SELECT = WM_APP+30 = 0x801E（对话框内程序化选中范围 [start, start+cnt)）
        [OsTreeFindUi]::SendMessage($dlg, 0x801E, [IntPtr]0, [IntPtr]2) | Out-Null
        Start-Sleep -Milliseconds 100

        # ---- 4. 测试钩子添加（等价右键菜单）：波形 + 数值（对话框保持打开） ----
        # WM_OS_PICK_TEST_ADD = WM_APP+43 = 0x802B（wParam=1 波形, 0 数值）
        [OsTreeFindUi]::SendMessage($dlg, 0x802B, [IntPtr]1, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 200
        Check (Log-Has '搜索对话框添加变量: 2 个到波形窗口') "搜索列表添加 2 个到波形窗口"
        [OsTreeFindUi]::SendMessage($dlg, 0x802B, [IntPtr]0, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 200
        Check (Log-Has '搜索对话框添加变量: 2 个到数值窗口') "搜索列表添加 2 个到数值窗口"
        $stillOpen = Find-Dlg $proc.Id
        Check ($stillOpen -ne [IntPtr]::Zero) "添加后对话框保持打开"

        # ---- 5. 列表右键菜单出现（#32768 弹出菜单）后 WM_CANCELMODE 关闭 ----
        # 注意：必须 PostMessage 触发（TrackPopupMenu 内部模态循环，
        # SendMessage 会同步阻塞等菜单关闭，与后续 CANCELMODE 互相死锁）
        $list = Find-ChildByClass $dlg "SysListView32"
        if ($list -ne [IntPtr]::Zero) {
            [OsTreeFindUi]::PostMessage($list, [OsTreeFindUi]::WM_CONTEXTMENU, $list, [IntPtr]0) | Out-Null
            Start-Sleep -Milliseconds 400
            $menu = Find-ByClass $proc.Id "#32768"
            Check ($menu -ne [IntPtr]::Zero) "列表右键弹出上下文菜单（#32768）"
            if ($menu -ne [IntPtr]::Zero) {
                [OsTreeFindUi]::PostMessage($dlg, [OsTreeFindUi]::WM_CANCELMODE, [IntPtr]0, [IntPtr]0) | Out-Null
                Start-Sleep -Milliseconds 300
            }
        } else {
            Check $false "列表 SysListView32 存在"
        }

        # ---- 6. 确定 -> 关闭 + 定位日志 + 变量树展开可见 ----
        # IDD_PICK_OK = 2401
        [OsTreeFindUi]::SendMessage($dlg, [OsTreeFindUi]::WM_COMMAND, [IntPtr]2401, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 300
        $closed = $true
        $again = Find-Dlg $proc.Id
        if ($again -ne [IntPtr]::Zero) { $closed = $false }
        Check $closed "确定后弹窗关闭"
        Check (Log-Has '搜索定位变量: 选中 2 个，定位 2 个') "搜索定位变量日志（选中 2 定位 2）"
        $tree = Find-ChildByClass $main "SysTreeView32"
        Check ($tree -ne [IntPtr]::Zero -and [OsTreeFindUi]::IsWindowVisible($tree)) "定位后变量树可见（搜索自动展开）"
    }

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '搜索|定位|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
