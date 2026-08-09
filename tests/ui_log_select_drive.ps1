# OpenScope 消息区域多选 + 右键复制/清除 UI 回归（request.md Bug 13）：
#   1. 日志 ListView 未设置 LVS_SINGLESEL -> 支持 Ctrl 连续选择 / Shift 起止范围选
#   2. 程序化选中若干消息（LVM_SETITEMSTATE，等价 Shift/Ctrl 手选结果）
#   3. 触发"复制选中"（IDM_LOG_COPY）-> 剪贴板内容包含选中消息文本
#   4. 触发"全部清除"（IDM_LOG_CLEAR）-> 日志清空（仅剩"已清除全部消息"一条）
#   5. 全程观察进程是否闪退
# 数据来源：应用启动即产生多条日志，无需真实 MCU / 回放。
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
public class OsLogUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", EntryPoint="GetWindowLongPtrW")] public static extern IntPtr GetWindowLongPtr(IntPtr h, int idx);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsLogUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsLogUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsLogUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsLogUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsLogUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsLogUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

function Get-LogItemCount([IntPtr]$HLog) {
    return [OsLogUi]::SendMessage($HLog, 0x1004, [IntPtr]0, [IntPtr]0).ToInt64()  # LVM_GETITEMCOUNT
}

function Select-LogItems([IntPtr]$HMain, [int]$Index, [int]$Count) {
    # Bug13 测试钩子 WM_OS_LOG_TEST_SELECT(0x801F): 主线程进程内选中 [Index, Index+Count)
    # （等价 Shift/Ctrl 手选结果）。返回实际选中数。
    # 说明：跨进程指针式 LVM_SETITEMSTATE 会读取发送方进程地址导致 comctl32 崩溃，
    #       故必须走进程内钩子（与 WM_OS_PICK_TEST_SELECT / WM_OS_TREE_TEST_SELECT 一致）。
    return [OsLogUi]::SendMessage($HMain, 0x801F, [IntPtr]$Index, [IntPtr]$Count).ToInt64()
}

function Get-SelectedCount([IntPtr]$HLog, [int]$N) {
    $sel = 0
    for ($i = 0; $i -lt $N; $i++) {
        $st = [OsLogUi]::SendMessage($HLog, 0x102C, [IntPtr]$i, [IntPtr]0x0002).ToInt64()  # LVM_GETITEMSTATE
        if (($st -band 0x0002) -ne 0) { $sel++ }
    }
    return $sel
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

    # 无窗口时唯一的 SysListView32 子控件就是消息区域日志
    $hLog = [IntPtr]::Zero
    for ($i = 0; $i -lt 40 -and -not $proc.HasExited; $i++) {
        $hLog = Find-ChildByClass $main "SysListView32"
        if ($hLog -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($hLog -ne [IntPtr]::Zero) "消息区域 ListView 存在"

    $n = 0
    for ($i = 0; $i -lt 40 -and -not $proc.HasExited; $i++) {
        $n = Get-LogItemCount $hLog
        if ($n -ge 5) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($n -ge 5) "日志已产生 >= 5 条（实际 $n）"

    # 1. 多选支持：未设置 LVS_SINGLESEL(0x0004)
    $style = [OsLogUi]::GetWindowLongPtr($hLog, -16).ToInt64()  # GWL_STYLE
    Check (($style -band 0x0004) -eq 0) "日志列表未设 LVS_SINGLESEL（支持多选）"

    # 2. 选中 3 条消息（进程内钩子，等价 Shift 起止范围选）
    $selHook = Select-LogItems $main 0 3
    Check ($selHook -eq 3) "程序化选中 3 条消息（钩子返回 $selHook）"
    $sel = Get-SelectedCount $hLog $n
    Check ($sel -eq 3) "ListView 实际选中 3 条消息（实际 $sel）"

    # 3. 复制选中 -> 剪贴板
    Send-Cmd $main 2601   # IDM_LOG_COPY
    Start-Sleep -Milliseconds 500
    $clip = ""
    try { $clip = Get-Clipboard -Raw -ErrorAction Stop } catch { $clip = "" }
    Check ($clip -match '信息' -or $clip -match '已加载' -or $clip -match 'OpenScope') "剪贴板含选中消息文本（长度 $($clip.Length)）"
    $clipLines = @($clip -split "`r?`n" | Where-Object { $_.Trim().Length -gt 0 })
    Check ($clipLines.Count -ge 3) "剪贴板含 3 行消息（实际 $($clipLines.Count)）"

    # 4. 全部清除 -> 仅剩一条"已清除全部消息"
    Send-Cmd $main 2602   # IDM_LOG_CLEAR
    Start-Sleep -Milliseconds 400
    $n2 = Get-LogItemCount $hLog
    Check ($n2 -eq 1) "全部清除后日志仅剩 1 条（实际 $n2）"

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '复制|清除|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
