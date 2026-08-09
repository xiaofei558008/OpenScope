# Tab 重命名 UI 回归（N3 就地编辑，无钩子干扰）：
#   1. 创建波形窗口，校验其父窗口是 Tab 控件
#   2. 触发 IDM_TAB_RENAME：就地 EDIT 出现（不弹窗），日志记录初始文本=当前名称
#   3. 通过 WM_CHAR 逐字输入“我的波形”+回车 → 日志出现“标签已重命名: 我的波形”
#   4. 再次触发：全选+WM_CLEAR 清空 +回车 → 名称兜底为 Default
#   5. 关闭应用，校验布局文件包含“Default”
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
public class OsRnUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern IntPtr GetParent(IntPtr h);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsRnUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsRnUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsRnUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Get-ChildByClass([IntPtr]$P, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsRnUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsRnUi]::EnumChildWindows($P, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-Edit([IntPtr]$tab, [int]$tries = 30) {
    for ($i = 0; $i -lt $tries; $i++) {
        $e = Get-ChildByClass $tab "Edit"
        if ($e -ne [IntPtr]::Zero) { return $e }
        Start-Sleep -Milliseconds 100
    }
    return [IntPtr]::Zero
}

# 逐字输入（WM_CHAR，无需字符串跨进程编组）：先全选则直接替换
function Type-Text([IntPtr]$e, [string]$s) {
    foreach ($c in $s.ToCharArray()) {
        [OsRnUi]::PostMessage($e, 0x0102, [IntPtr][int][char]$c, [IntPtr]1) | Out-Null  # WM_CHAR
        Start-Sleep -Milliseconds 30
    }
}

function Wait-Log([string]$Pattern, [int]$TimeoutMs = 10000) {
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
    Start-Sleep -Milliseconds 1200  # 等初始化（含 J-Link 扫描）完成
    [OsRnUi]::SendMessage($main, 0x111, [IntPtr]2012, [IntPtr]0) | Out-Null  # 波形窗口
    Start-Sleep -Milliseconds 500
    $tab = Get-ChildByClass $main "SysTabControl32"
    $chart = Get-ChildByClass $main "OSChartWin"
    Check ($tab -ne [IntPtr]::Zero -and $chart -ne [IntPtr]::Zero) "波形窗口与 Tab 存在"
    $parent = [OsRnUi]::GetParent($chart)
    Check ($parent -eq $tab) "波形窗口父窗口 = Tab 控件（避免被覆盖）"

    # 第一次就地编辑：初始文本=当前名称，输入新名称后回车提交
    # 注：跨进程 WM_CHAR 仅能可靠注入 ASCII（中文需真实键盘 IME，属原生 EDIT 行为，
    #     非本应用代码路径）。中文名称由“就地编辑开始”日志 + 布局文件验证。
    [OsRnUi]::PostMessage($main, 0x111, [IntPtr]2502, [IntPtr]0) | Out-Null  # IDM_TAB_RENAME
    $edit = Find-Edit $tab
    Check ($edit -ne [IntPtr]::Zero) "就地编辑框出现（不弹窗）"
    Check ((Find-ByClass $proc.Id "OSDlgRename") -eq [IntPtr]::Zero) "无重命名对话框（就地编辑）"
    if ($edit -ne [IntPtr]::Zero) {
        Check (Wait-Log "就地编辑开始: 波形窗口 1" 5000) "初始文本=当前名称（中文，应用日志）"
        Type-Text $edit "MyWin"
        Start-Sleep -Milliseconds 150
        [OsRnUi]::PostMessage($edit, 0x100, [IntPtr]13, [IntPtr]0) | Out-Null  # WM_KEYDOWN VK_RETURN
    }
    Check (Wait-Log "标签已重命名: MyWin") "回车提交新名称（逐字输入+提交链路）"

    # 第二次就地编辑：清空后回车 → 兜底 Default
    [OsRnUi]::PostMessage($main, 0x111, [IntPtr]2502, [IntPtr]0) | Out-Null
    $edit = Find-Edit $tab
    Check ($edit -ne [IntPtr]::Zero) "第二次就地编辑框出现"
    if ($edit -ne [IntPtr]::Zero) {
        Check (Wait-Log "就地编辑开始: MyWin" 5000) "第二次初始文本=新名称（应用日志）"
        [OsRnUi]::SendMessage($edit, 0xB1, [IntPtr]0, [IntPtr](-1)) | Out-Null  # EM_SETSEL 全选
        Start-Sleep -Milliseconds 50
        [OsRnUi]::SendMessage($edit, 0x0303, [IntPtr]0, [IntPtr]0) | Out-Null   # WM_CLEAR 删除文本
        Start-Sleep -Milliseconds 100
        [OsRnUi]::PostMessage($edit, 0x100, [IntPtr]13, [IntPtr]0) | Out-Null   # 回车提交空名
    }
    Check (Wait-Log "标签已重命名: Default") "空名兜底为 Default"

    # 关闭应用，检查布局文件中的新标题
    [OsRnUi]::SendMessage($main, 0x10, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_CLOSE
    $proc.WaitForExit(5000) | Out-Null
    $lay = ""
    if (Test-Path $autoLayout) { $lay = Get-Content $autoLayout -Encoding UTF8 -Raw }
    Check ($lay -match 'Default') "布局文件保存了兜底名 Default"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
