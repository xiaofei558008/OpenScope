# F21/Step1 高速采集回归：自由运行 + 连续地址块读。
#   1. 布局预建数值窗口观测 3 个连续变量（fsin@0x20000390 + cnt@0x20000394 +
#      ang_rd_error_cnt@0x20000396，恰好相邻可合并为一次 7 字节块读）
#   2. 连接 J-Link（默认 4000kHz；12000 高速连接有环境性失败风险，由 ui_speed12000 单独覆盖）
#      -> 开始采集
#   3. 断言日志出现"自由运行高速模式"（确认已走高速路径，非 20ms 定时循环）
#   4. 解析"采集速率: N 样本/s"，断言 N > 2000（旧 20ms 循环 ~50/s，实测连续 3 变量 ~8k/s）
#   5. 断言采集线程存活 + 断开
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsF21Ui {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsF21Ui]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsF21Ui]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsF21Ui]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$layout = Join-Path $env:TEMP "f21_verify.ini"
$body = @"
[layout]
version=1
main_x=100
main_y=100
main_w=1200
main_h=700
tree_w=340
log_h=170
active=0
wins=1
[win]
type=num
title=F21验证
vars=fsin
vars+=cnt
vars+=ang_rd_error_cnt
"@
[System.IO.File]::WriteAllText($layout, $body, (New-Object System.Text.UTF8Encoding $true))

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}
function Has-Log([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    return [bool](Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch $Pattern -Quiet)
}
function Wait-Log([string]$Pattern, [int]$TimeoutMs = 8000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        if (Test-Path $log) {
            if (Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch $Pattern -Quiet) { return $true }
        }
        Start-Sleep -Milliseconds 200
    }
    return $false
}
# 解析第一条"采集速率: N 样本/s"的 N
function Get-Rate([int]$TimeoutMs = 6000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        if (Test-Path $log) {
            $m = Get-Content $log -Encoding UTF8 | Select-String -Pattern '采集速率: (\d+) 样本/s'
            if ($m -and $m.Count -gt 0) { return [int]$m[0].Matches[0].Groups[1].Value }
        }
        Start-Sleep -Milliseconds 200
    }
    return -1
}

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--layout-load=$layout") -PassThru
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 1500

    # 连接（默认速度）+ 开始采集
    [OsF21Ui]::SendMessage($main, 0x111, [IntPtr]2002, [IntPtr]0) | Out-Null
    $connected = Wait-Log "已连接: " 10000
    Check $connected "连接成功（日志）"
    [OsF21Ui]::SendMessage($main, 0x111, [IntPtr]2004, [IntPtr]0) | Out-Null
    Check (Wait-Log "采集已开始" 6000) "采集已开始（日志）"
    Check (Has-Log "自由运行高速模式") "采集已走自由运行高速模式"
    Start-Sleep -Milliseconds 2500

    # 核心断言：采样速率远超旧 20ms 循环（~50/s）
    $rate = Get-Rate
    Write-Output ("INFO 采集速率 = $rate 样本/s（3 连续变量，块读合并）")
    Check ($rate -gt 2000) "高速采样速率 > 2000 样本/s（实测 $rate）"
    $threadExit = (Has-Log "采集线程已退出") -or (Has-Log "采集停止：长时间") -or (Has-Log "采集停止：MCU 连接已断开")
    Check (-not $threadExit) "采集期间线程存活（Bug10 未回归）"
    Check (-not $proc.HasExited) "采集期间进程存活"

    # 停止 -> 断开
    [OsF21Ui]::PostMessage($main, 0x111, [IntPtr]2005, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 400
    [OsF21Ui]::PostMessage($main, 0x111, [IntPtr]2003, [IntPtr]0) | Out-Null
    Check (Wait-Log "已断开连接" 5000) "断开成功（日志）"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
