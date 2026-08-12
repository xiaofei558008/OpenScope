# OpenScope 帮助文档（F1）+ 关于框版本一致性 UI 回归（request.md 需求12 + Bug17 同类）：
#   需求12: readme.md 内容写入帮助文档，F1 键弹出，帮助菜单也有"帮助文档"选项。
#   版本一致性: 关于框版本号必须与 exe 文件版本（version.rc 唯一来源）一致——
#               checkpoint-30 曾出现关于框/启动日志硬编码 1.15.0 与 version.rc 1.16.0 失配。
#   验证：
#     1. 帮助菜单命令 IDM_HELP_DOC(2702) -> OSHelpWin 窗口出现，EDIT 含 readme 关键内容
#     2. 真实 F1 键盘输入（keybd_event）-> OSHelpWin 再次出现（加速键表生效）
#     3. F1 再次按下（或 WM_CLOSE）-> 帮助窗口关闭
#     4. 关于框（IDC_BTN_ABOUT=2010）文本含 exe 文件版本号 + "晶圆上的生物技术开发" + 网址
#     5. 全程进程不闪退
param(
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }

# exe 文件版本 -> 期望显示版本 "1.16.0"
$fv = (Get-Item $exe).VersionInfo.FileVersion   # 例如 1.16.0.0
$parts = $fv.Split('.')
$expectVer = "$($parts[0]).$($parts[1]).$($parts[2])"
Write-Output "exe FileVersion=$fv 期望关于框显示 v$expectVer"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsHelpUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, StringBuilder sb);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern bool AttachThreadInput(uint a, uint b, bool attach);
    [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, UIntPtr extra);
    [DllImport("kernel32.dll")] public static extern uint GetCurrentThreadId();
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    public const uint WM_COMMAND = 0x111;
    public const uint WM_CLOSE = 0x10;
    public const uint WM_GETTEXT = 0x0D;
    public const uint WM_GETTEXTLENGTH = 0x0E;
    public const byte VK_F1 = 0x70;
    public const uint KEYUP = 0x2;
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsHelpUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsHelpUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsHelpUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-DlgByTitle([int]$ProcId, [string]$Title) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsHelpUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsHelpUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq "#32770") {
                $t = New-Object System.Text.StringBuilder 256
                [OsHelpUi]::GetWindowText($h, $t, 256) | Out-Null
                if ($t.ToString() -eq $Title) { $script:hit = $h; return $false }
            }
        }
        return $true
    }
    [OsHelpUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Wait-Window([int]$ProcId, [string]$Class, [int]$Ms) {
    $deadline = (Get-Date).AddMilliseconds($Ms)
    while ((Get-Date) -lt $deadline) {
        $h = Find-ByClass $ProcId $Class
        if ($h -ne [IntPtr]::Zero) { return $h }
        Start-Sleep -Milliseconds 150
    }
    return [IntPtr]::Zero
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

$proc = Start-Process -FilePath $exe -ArgumentList @("--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 500

    # 1) 帮助菜单命令 -> OSHelpWin
    [OsHelpUi]::SendMessage($main, [OsHelpUi]::WM_COMMAND, [IntPtr]2702, [IntPtr]0) | Out-Null
    $help = Wait-Window $proc.Id "OSHelpWin" 2500
    Check ($help -ne [IntPtr]::Zero) "菜单命令打开帮助文档窗口 (OSHelpWin)"
    if ($help -ne [IntPtr]::Zero) {
        $edit = [IntPtr]::Zero
        $cb = {
            param($h, $l)
            $sb = New-Object System.Text.StringBuilder 64
            [OsHelpUi]::GetClassName($h, $sb, 64) | Out-Null
            if ($sb.ToString() -eq "Edit") { $script:editHit = $h; return $false }
            return $true
        }
        [OsHelpUi]::EnumChildWindows($help, $cb, [IntPtr]::Zero) | Out-Null
        $edit = $script:editHit
        Check ($edit -ne [IntPtr]::Zero) "帮助窗口含只读 EDIT"
        if ($edit -ne [IntPtr]::Zero) {
            $len = [OsHelpUi]::SendMessage($edit, [OsHelpUi]::WM_GETTEXTLENGTH, [IntPtr]0, [IntPtr]0).ToInt32()
            Check ($len -gt 1500) "帮助文本长度 $len > 1500（readme 全文内嵌）"
            $sb = New-Object System.Text.StringBuilder ($len + 8)
            [OsHelpUi]::SendMessage($edit, [OsHelpUi]::WM_GETTEXT, [IntPtr]($len + 4), $sb) | Out-Null
            $txt = $sb.ToString()
            Check ($txt.Contains("OpenScope")) "帮助含软件名 OpenScope"
            Check ($txt.Contains("晶圆上的生物技术开发")) "帮助含开发支持信息（需求6）"
            Check ($txt.Contains("www.opendebugger.com")) "帮助含网址（需求6）"
            Check ($txt.Contains("快速开始")) "帮助含 readme 章节（快速开始）"
        }
        # 关闭帮助（WM_CLOSE）
        [OsHelpUi]::SendMessage($help, [OsHelpUi]::WM_CLOSE, [IntPtr]0, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 400
        $gone = Find-ByClass $proc.Id "OSHelpWin"
        Check ($gone -eq [IntPtr]::Zero) "WM_CLOSE 关闭帮助窗口"
    }

    # 2) 真实 F1 键盘输入 -> 加速键表弹出帮助
    $fg = [OsHelpUi]::GetForegroundWindow()
    $fgTid = [OsHelpUi]::GetWindowThreadProcessId($fg, [ref]([uint32]$null))
    $myTid = [OsHelpUi]::GetCurrentThreadId()
    [OsHelpUi]::AttachThreadInput($myTid, $fgTid, $true) | Out-Null
    [OsHelpUi]::SetForegroundWindow($main) | Out-Null
    [OsHelpUi]::AttachThreadInput($myTid, $fgTid, $false) | Out-Null
    Start-Sleep -Milliseconds 300
    [OsHelpUi]::keybd_event([OsHelpUi]::VK_F1, 0, 0, [UIntPtr]::Zero)
    [OsHelpUi]::keybd_event([OsHelpUi]::VK_F1, 0, [OsHelpUi]::KEYUP, [UIntPtr]::Zero)
    $help2 = Wait-Window $proc.Id "OSHelpWin" 2500
    Check ($help2 -ne [IntPtr]::Zero) "F1 键弹出帮助文档（加速键表）"
    if ($help2 -ne [IntPtr]::Zero) {
        [OsHelpUi]::SendMessage($help2, [OsHelpUi]::WM_CLOSE, [IntPtr]0, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 300
    }

    # 3) 关于框版本一致性（version.rc 唯一来源）
    # 注意：MessageBox 是模态对话框，SendMessage(WM_COMMAND) 会一直阻塞到关闭——必须 PostMessage
    [OsHelpUi]::PostMessage($main, [OsHelpUi]::WM_COMMAND, [IntPtr]2010, [IntPtr]0) | Out-Null
    $about = [IntPtr]::Zero
    $deadline = (Get-Date).AddSeconds(2.5)
    while ((Get-Date) -lt $deadline) {
        $about = Find-DlgByTitle $proc.Id "关于"
        if ($about -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 150
    }
    Check ($about -ne [IntPtr]::Zero) "关于对话框弹出"
    if ($about -ne [IntPtr]::Zero) {
        $all = New-Object System.Text.StringBuilder 1024
        $cb2 = {
            param($h, $l)
            $sb = New-Object System.Text.StringBuilder 512
            [OsHelpUi]::GetWindowText($h, $sb, 512) | Out-Null
            if ($sb.Length -gt 0) { $all.Append($sb.ToString()) | Out-Null }
            return $true
        }
        [OsHelpUi]::EnumChildWindows($about, $cb2, [IntPtr]::Zero) | Out-Null
        $atxt = $all.ToString()
        Check ($atxt.Contains("v$expectVer")) "关于框版本 v$expectVer 与 version.rc 一致"
        Check ($atxt.Contains("晶圆上的生物技术开发")) "关于框含开发支持信息（需求6）"
        Check ($atxt.Contains("www.opendebugger.com")) "关于框含网址（需求6）"
        [OsHelpUi]::SendMessage($about, [OsHelpUi]::WM_COMMAND, [IntPtr]1, [IntPtr]0) | Out-Null  # IDOK
        Start-Sleep -Milliseconds 300
    }

    Check (-not $proc.HasExited) "进程未闪退"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
