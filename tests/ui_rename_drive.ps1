# Tab 重命名 UI 回归：
#   1. 创建波形窗口，校验其父窗口是 Tab 控件
#   2. 用 --rename-tab 钩子（与对话框 OK 同一更新路径）重命名为“我的波形”
#   3. 再触发 IDM_TAB_RENAME 打开对话框并点确定（默认文本路径）
#   4. 校验日志与布局文件中的新名称
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

function Find-ChildByClass([IntPtr]$P, [string]$Class) {
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

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--no-layout", "--rename-tab=我的波形") -PassThru
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
    $tab = Find-ChildByClass $main "SysTabControl32"
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($tab -ne [IntPtr]::Zero -and $chart -ne [IntPtr]::Zero) "波形窗口与 Tab 存在"
    $parent = [OsRnUi]::GetParent($chart)
    Check ($parent -eq $tab) "波形窗口父窗口 = Tab 控件（避免被覆盖）"

    $hookRenamed = $false
    if (Test-Path $log) {
        $hookRenamed = [bool](Get-Content $log -Encoding UTF8 | Select-String -Pattern '标签已重命名: 我的波形')
    }
    Check $hookRenamed "钩子重命名为“我的波形”"

    # 对话框路径：打开后点确定（保留当前文本）
    [OsRnUi]::PostMessage($main, 0x111, [IntPtr]2502, [IntPtr]0) | Out-Null  # IDM_TAB_RENAME
    $dlg = [IntPtr]::Zero
    for ($i = 0; $i -lt 30; $i++) {
        $dlg = Find-ByClass $proc.Id "OSDlgRename"
        if ($dlg -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 100
    }
    Check ($dlg -ne [IntPtr]::Zero) "重命名对话框打开"
    if ($dlg -ne [IntPtr]::Zero) {
        Start-Sleep -Milliseconds 300
        [OsRnUi]::SendMessage($dlg, 0x111, [IntPtr]2602, [IntPtr]0) | Out-Null  # IDD_RN_OK
    }
    Start-Sleep -Milliseconds 400
    $dlgRenamed = $false
    if (Test-Path $log) {
        $dlgRenamed = [bool](Get-Content $log -Encoding UTF8 | Select-String -Pattern '标签已重命名: 我的波形')
    }
    Check $dlgRenamed "对话框 OK 路径保留名称"

    # 关闭应用，检查布局文件中的新标题
    [OsRnUi]::SendMessage($main, 0x10, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_CLOSE
    $proc.WaitForExit(5000) | Out-Null
    $lay = ""
    if (Test-Path $autoLayout) { $lay = Get-Content $autoLayout -Encoding UTF8 -Raw }
    Check ($lay -match '我的波形') "布局文件保存了新标签名"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
