# OpenScope 主界面直连回归（连接配置直接放主界面，不弹配置对话框）：
#   1. 启动 bin\Release\OpenScope.exe
#   2. 校验主界面连接配置控件存在（MCU型号 EDIT / 接口 / 速度 / J-Link 设备列表 / 刷新）
#   3. 设备列表应已扫描出 J-Link
#   4. 输入 MCU 型号 -> 点“连接” -> 校验日志已连接且无弹窗
#   5. 点“断开”
param(
    [string]$Device = "STM32L432KB",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsCfgUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern int GetDlgCtrlID(IntPtr h);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern bool SetWindowText(IntPtr h, string t);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsCfgUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 128
            [OsCfgUi]::GetClassName($h, $sb, 128) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsCfgUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildById([IntPtr]$P, [int]$Id) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        if ([OsCfgUi]::GetDlgCtrlID($h) -eq $Id) { $script:hit = $h; return $false }
        return $true
    }
    [OsCfgUi]::EnumChildWindows($P, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

$root = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }

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
    Start-Sleep -Milliseconds 800

    # 校验主界面连接配置控件
    $dev = Find-ChildById $main 2101
    $iface = Find-ChildById $main 2102
    $speed = Find-ChildById $main 2103
    $emu = Find-ChildById $main 2104
    $refresh = Find-ChildById $main 2105
    Check ($dev -ne [IntPtr]::Zero -and $iface -ne [IntPtr]::Zero -and
           $speed -ne [IntPtr]::Zero -and $emu -ne [IntPtr]::Zero -and
           $refresh -ne [IntPtr]::Zero) "主界面连接配置控件齐全"

    # 设备列表已扫描（至少 1 项）
    $emuCount = [OsCfgUi]::SendMessage($emu, 0x0146, [IntPtr]0, [IntPtr]0).ToInt64()  # CB_GETCOUNT
    Check ($emuCount -ge 1) "J-Link 设备列表已扫描（$emuCount 项）"
    if ($emuCount -ge 1) {
        # 刷新按钮再扫一次
        [OsCfgUi]::SendMessage($main, 0x111, [IntPtr]2105, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 500
        $emuCount2 = [OsCfgUi]::SendMessage($emu, 0x0146, [IntPtr]0, [IntPtr]0).ToInt64()
        Check ($emuCount2 -ge 1) "刷新后设备列表仍正常（$emuCount2 项）"
    }

    # 输入 MCU 型号并点连接（不弹配置对话框）
    [OsCfgUi]::SetWindowText($dev, $Device) | Out-Null
    Write-Output "device set: $Device"
    [OsCfgUi]::SendMessage($main, 0x111, [IntPtr]2002, [IntPtr]0) | Out-Null  # IDC_BTN_CONNECT

    $connected = $false
    for ($i = 0; $i -lt 40; $i++) {
        if (Test-Path $log) {
            $connected = [bool](Get-Content $log -Encoding UTF8 | Select-String -Pattern '已连接: J-Link|J-Link 已连接')
            if ($connected) { break }
        }
        Start-Sleep -Milliseconds 200
    }
    Check $connected "主界面直连 STM32L432KB 成功（日志确认）"
    Check (-not $proc.HasExited) "进程未闪退"
    # 不应弹出配置对话框（连接流程已内嵌到主界面）
    $dlg = Find-ByClass $proc.Id "OSJLinkCfg"
    Check ($dlg -eq [IntPtr]::Zero) "未弹出配置对话框"

    # 断开
    [OsCfgUi]::SendMessage($main, 0x111, [IntPtr]2003, [IntPtr]0) | Out-Null  # IDC_BTN_DISCON
    Start-Sleep -Milliseconds 600
    $disconnected = $false
    if (Test-Path $log) {
        $disconnected = [bool](Get-Content $log -Encoding UTF8 | Select-String -Pattern 'J-Link 已断开')
    }
    Check $disconnected "断开成功（日志确认）"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
