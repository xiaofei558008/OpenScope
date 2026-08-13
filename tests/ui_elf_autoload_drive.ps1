# OpenScope 启动自动加载上次 ELF/OUT 回归（用户需求）：
#   会话1：命令行加载 ELF → load_elf_path 立即持久化 elf= 到默认布局 → 关闭
#   会话2：不带 ELF 参数启动（布局恢复）→ 自动加载上次 ELF
#         → 日志"已自动加载上次 ELF" + "ELF 变量表重建: 511 个叶子"
#   会话3：layout.ini 的 elf 指向不存在文件 → 警告日志 + 不闪退
#   测试前后备份/恢复用户默认布局。
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }

$defaultDir = Join-Path $env:LOCALAPPDATA "OpenScope"
$defaultLayout = Join-Path $defaultDir "layout.ini"
$defaultBackup = Join-Path $env:TEMP "openscope_elfautoload_layout.bak"
$hadDefault = Test-Path $defaultLayout
if ($hadDefault) { Copy-Item $defaultLayout $defaultBackup -Force }

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsElfAutoUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsElfAutoUi]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsElfAutoUi]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsElfAutoUi]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
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

$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) {
    Write-Output "ERROR: OpenScope already running: $($existing.Id -join ',')"
    exit 3
}

# ---- 会话1：加载 ELF（load 即持久化 elf=）后正常关闭 ----
$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "会话1 主窗口创建"
    Check (Wait-Log "已加载 ELF" 6000) "会话1 ELF 加载成功"
    [OsElfAutoUi]::SendMessage($main, 0x0010, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_CLOSE
    $deadline = (Get-Date).AddSeconds(8)
    do { Start-Sleep -Milliseconds 200; $proc.Refresh() } while (-not $proc.HasExited -and (Get-Date) -lt $deadline)
    Check $proc.HasExited "会话1 正常退出"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Check (Test-Path $defaultLayout) "默认布局已生成"
if (Test-Path $defaultLayout) {
    $m = Get-Content $defaultLayout -Encoding UTF8 | Select-String -Pattern '^elf='
    Check ($null -ne $m -and $m.Count -gt 0) "布局含 elf= 键"
} else {
    Check $false "布局文件存在"
}

# ---- 会话2：不带 ELF 参数启动 → 布局恢复自动加载上次 ELF ----
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$proc2 = Start-Process -FilePath $exe -PassThru
Write-Output "started pid=$($proc2.Id)"
try {
    $main2 = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc2.HasExited; $i++) {
        $main2 = Find-ByClass $proc2.Id "OpenScopeMain"
        if ($main2 -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main2 -ne [IntPtr]::Zero) "会话2 主窗口创建"
    Check (Wait-Log "已自动加载上次 ELF" 8000) "会话2 自动加载上次 ELF（日志）"
    Check (Has-Log "ELF 变量表重建: 511 个叶子") "会话2 变量表重建（511 叶子，解析成功）"
    Check (-not $proc2.HasExited) "会话2 进程存活"
    [OsElfAutoUi]::SendMessage($main2, 0x0010, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 1500
}
finally {
    if (-not $proc2.HasExited) { $proc2.Kill() }
}

# ---- 会话3：elf= 指向不存在的文件 → 警告不闪退 ----
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
if (Test-Path $defaultLayout) {
    $content = [System.IO.File]::ReadAllText($defaultLayout, [System.Text.Encoding]::UTF8)
    $content = $content -replace '(?m)^elf=.*$', 'elf=D:\nonexistent\missing.elf'
    [System.IO.File]::WriteAllText($defaultLayout, $content, (New-Object System.Text.UTF8Encoding $true))
}
$proc3 = Start-Process -FilePath $exe -PassThru
try {
    $main3 = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc3.HasExited; $i++) {
        $main3 = Find-ByClass $proc3.Id "OpenScopeMain"
        if ($main3 -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main3 -ne [IntPtr]::Zero) "会话3 主窗口创建"
    Check (Wait-Log "上次的 ELF 文件不存在" 8000) "会话3 缺失 ELF 警告日志（不弹窗）"
    Check (-not $proc3.HasExited) "会话3 进程存活（无闪退）"
    [OsElfAutoUi]::SendMessage($main3, 0x0010, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 1500
}
finally {
    if (-not $proc3.HasExited) { $proc3.Kill() }
}

# 恢复用户默认布局
if ($hadDefault) { Copy-Item $defaultBackup $defaultLayout -Force }
elseif (Test-Path $defaultLayout) { Remove-Item -LiteralPath $defaultLayout -Force }

Write-Output "--- 日志关键行 ---"
if (Test-Path $log) {
    Get-Content $log -Encoding UTF8 | Select-String -Pattern 'ELF|布局|FATAL' | ForEach-Object { $_.Line }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
