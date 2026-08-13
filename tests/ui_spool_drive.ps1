# OpenScope 长时间采集自动落盘回归（J-Link 硬件组）：
#   采集自动建立 RAM(≤10MB) 缓冲，停止时写出时间戳命名 CSV
#   （%LOCALAPPDATA%\OpenScope\records\rec_YYYYMMDD_HHMMSS.csv）。
#   测试：连接(默认4000kHz) -> 开始采集 ~4s -> 停止 ->
#        日志"采集落盘"起止 + rec_*.csv 存在且行数>1。
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$recDir = Join-Path $env:LOCALAPPDATA "OpenScope\records"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsSpoolUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsSpoolUi]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsSpoolUi]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsSpoolUi]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsSpoolUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}
$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}
function Has-Log([string]$Pattern) {
    if (-not (Test-Path $log)) { return $false }
    return [bool](Get-Content $log -Encoding UTF8 | Select-String -SimpleMatch $Pattern -Quiet)
}
function Wait-Log([string]$Pattern, [int]$TimeoutMs = 10000) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $TimeoutMs) {
        if (Has-Log $Pattern) { return $true }
        Start-Sleep -Milliseconds 200
    }
    return $false
}

$layout = Join-Path $env:TEMP "spool_test.ini"
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
title=SpoolTest
vars=fsin
"@
[System.IO.File]::WriteAllText($layout, $body, (New-Object System.Text.UTF8Encoding $true))

$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) {
    Write-Output "ERROR: OpenScope already running: $($existing.Id -join ',')"
    exit 3
}

# 记录测试开始前的 rec 文件，避免误判旧文件
$before = @()
if (Test-Path $recDir) { $before = @(Get-ChildItem $recDir -Filter "rec_*.csv" | Select-Object -ExpandProperty Name) }

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--layout-load=$layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 1500

    # 连接（默认速度 4000kHz）
    Send-Cmd $main 2002   # IDC_BTN_CONNECT
    Check (Wait-Log "已连接: " 10000) "连接成功（日志）"

    # 开始采集 ~4s
    Send-Cmd $main 2004   # IDC_BTN_START
    Check (Wait-Log "采集已开始" 6000) "采集已开始（日志）"
    Check (Has-Log "采集落盘: RAM 缓冲") "采集开始时建立落盘 RAM 缓冲"
    Start-Sleep -Milliseconds 4000

    # 停止采集 -> 落盘收尾
    Send-Cmd $main 2005   # IDC_BTN_STOP
    Check (Wait-Log "采集落盘完成" 6000) "停止时落盘完成（日志）"
    Start-Sleep -Milliseconds 500

    # rec_*.csv 新文件存在且行数 > 1（表头 + 样本行）
    $newFiles = @()
    if (Test-Path $recDir) {
        $newFiles = @(Get-ChildItem $recDir -Filter "rec_*.csv" | Where-Object { $before -notcontains $_.Name })
    }
    Check ($newFiles.Count -ge 1) "落盘 CSV 已生成（rec_*.csv 新文件）"
    if ($newFiles.Count -ge 1) {
        $f = $newFiles[0]
        $lines = (Get-Content $f.FullName -Encoding UTF8 | Measure-Object -Line).Lines
        Check ($lines -gt 1) "落盘 CSV 含样本行（$($f.Name) 共 $lines 行）"
        Write-Output "INFO 落盘文件: $($f.FullName) ($lines 行, $($f.Length) 字节)"
        $header = Get-Content $f.FullName -Encoding UTF8 -TotalCount 1
        Check ($header -match 'timestamp_us' -and $header -match 'fsin') "落盘 CSV 表头含 timestamp_us 与 fsin"
    }

    Check (-not $proc.HasExited) "进程存活"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
