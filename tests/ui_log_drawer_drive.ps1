# OpenScope 消息栏抽屉收起/弹出 UI 回归（request.md 新增特性 22）：
#   F22: 底部消息窗口像左侧变量树一样支持抽屉收起/弹出。
#   验证：
#     1. 双击横向分隔条（OSSplitterH）-> 消息栏收成底部细条 OSLogStrip（hLog 隐藏）
#     2. 点击底部细条 OSLogStrip -> 消息栏弹出恢复（hLog 显示）
#     3. 测试钩子 WM_OS_LOG_HIDE=1/0 等价于收起/展开
#     4. 布局持久化：收起后关闭 -> 默认 layout.ini 含 log_hidden=1 -> 重开恢复收起
#     5. 全程观察进程是否闪退
# 已知坑（时序）：
#   - CreateWindow 返回后窗口句柄立即存在，但 ShowWindow 在 os_modmgr_load（jlink 扫描）之后才执行，
#     因此 Find 到主窗口时 WS_VISIBLE 可能尚未设置。必须先等主窗口可见再断言。
#   - WM_CLOSE 触发 os_layout_save_auto 写默认 %LOCALAPPDATA%\OpenScope\layout.ini；测试会备份/恢复用户布局。
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = "",
    [string]$LayoutSave = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$layout = if ($LayoutSave) { $LayoutSave } else { Join-Path $env:TEMP "openscope_f22_layout.ini" }
if (Test-Path $layout) { Remove-Item -LiteralPath $layout -Force }

# 默认布局路径（WM_CLOSE -> os_layout_save_auto 写入）
$defaultDir = Join-Path $env:LOCALAPPDATA "OpenScope"
$defaultLayout = Join-Path $defaultDir "layout.ini"
$defaultBackup = Join-Path $env:TEMP "openscope_f22_default_layout.bak"
$hadDefault = Test-Path $defaultLayout
if ($hadDefault) { Copy-Item $defaultLayout $defaultBackup -Force }

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsLogDrawerUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int cmd);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsLogDrawerUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsLogDrawerUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsLogDrawerUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsLogDrawerUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsLogDrawerUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

# 等主窗口真正可见（ShowWindow 在模块加载后才执行，见 main.c 注释）；超时后 ShowWindow 兜底
function Wait-MainVisible([IntPtr]$h) {
    $deadline = (Get-Date).AddSeconds(6)
    while ((Get-Date) -lt $deadline) {
        if ([OsLogDrawerUi]::IsWindowVisible($h)) { return $true }
        Start-Sleep -Milliseconds 200
    }
    [OsLogDrawerUi]::ShowWindow($h, 5) | Out-Null  # SW_SHOW 兜底
    Start-Sleep -Milliseconds 300
    return [OsLogDrawerUi]::IsWindowVisible($h)
}

# 轮询查找子窗口（窗口早期子窗口可能尚未全部创建）
function Find-ChildByClassRetry([IntPtr]$Parent, [string]$Class) {
    $deadline = (Get-Date).AddSeconds(5)
    while ((Get-Date) -lt $deadline) {
        $h = Find-ChildByClass $Parent $Class
        if ($h -ne [IntPtr]::Zero) { return $h }
        Start-Sleep -Milliseconds 200
    }
    return $h
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

# 会话 1：收起 -> 关闭（触发默认布局保存）-> 验证 log_hidden=1 已持久化
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
    Check (Wait-MainVisible $main) "主窗口可见（等待 ShowWindow）"

    $logListView = Find-ChildByClassRetry $main "SysListView32"
    Check ($logListView -ne [IntPtr]::Zero) "消息栏 ListView 存在"

    $splitter = Find-ChildByClassRetry $main "OSSplitterH"
    Check ($splitter -ne [IntPtr]::Zero) "横向分隔条 OSSplitterH 存在"

    # 初始：消息栏展开，底部细条不可见
    Check ([OsLogDrawerUi]::IsWindowVisible($logListView)) "初始消息栏可见"
    $strip = Find-ChildByClass $main "OSLogStrip"
    Check ($strip -eq [IntPtr]::Zero -or -not [OsLogDrawerUi]::IsWindowVisible($strip)) "初始底部细条不可见"

    # 1) 双击横向分隔条 -> 收起
    if ($splitter -ne [IntPtr]::Zero) {
        [OsLogDrawerUi]::SendMessage($splitter, 0x203, [IntPtr]1, [IntPtr]0) | Out-Null  # WM_LBUTTONDBLCLK
        Start-Sleep -Milliseconds 200
    }
    Check (-not [OsLogDrawerUi]::IsWindowVisible($logListView)) "双击分隔条后消息栏收起（hLog 隐藏）"
    $strip = Find-ChildByClassRetry $main "OSLogStrip"
    Check ($strip -ne [IntPtr]::Zero -and [OsLogDrawerUi]::IsWindowVisible($strip)) "底部细条 OSLogStrip 可见"
    Check (Log-Has '消息栏已收起') "日志记录消息栏收起"

    # 2) 点击底部细条 -> 弹出
    if ($strip -ne [IntPtr]::Zero) {
        [OsLogDrawerUi]::SendMessage($strip, 0x201, [IntPtr]1, [IntPtr]0) | Out-Null  # WM_LBUTTONDOWN
        Start-Sleep -Milliseconds 200
    }
    Check ([OsLogDrawerUi]::IsWindowVisible($logListView)) "点击底部细条后消息栏弹出（hLog 可见）"
    $strip = Find-ChildByClass $main "OSLogStrip"
    Check ($strip -eq [IntPtr]::Zero -or -not [OsLogDrawerUi]::IsWindowVisible($strip)) "弹出后底部细条隐藏"
    Check (Log-Has '消息栏已展开') "日志记录消息栏展开"

    # 3) 测试钩子 WM_OS_LOG_HIDE=1（WM_APP+32 = 0x8000+32 = 0x8020）收起
    [OsLogDrawerUi]::SendMessage($main, 0x8020, [IntPtr]1, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200
    Check (-not [OsLogDrawerUi]::IsWindowVisible($logListView)) "钩子收起：消息栏隐藏"
    $strip = Find-ChildByClass $main "OSLogStrip"
    Check ($strip -ne [IntPtr]::Zero -and [OsLogDrawerUi]::IsWindowVisible($strip)) "钩子收起：底部细条可见"

    # 4) 测试钩子 WM_OS_LOG_HIDE=0 展开
    [OsLogDrawerUi]::SendMessage($main, 0x8020, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200
    Check ([OsLogDrawerUi]::IsWindowVisible($logListView)) "钩子展开：消息栏可见"
    $strip = Find-ChildByClass $main "OSLogStrip"
    Check ($strip -eq [IntPtr]::Zero -or -not [OsLogDrawerUi]::IsWindowVisible($strip)) "钩子展开：底部细条隐藏"

    # 5) 持久化：钩子收起后关闭 -> WM_CLOSE 触发 os_layout_save_auto 写默认 layout.ini
    [OsLogDrawerUi]::SendMessage($main, 0x8020, [IntPtr]1, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200
    Check (-not [OsLogDrawerUi]::IsWindowVisible($logListView)) "关闭前消息栏已收起"
    [OsLogDrawerUi]::SendMessage($main, 0x0010, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_CLOSE
    Start-Sleep -Milliseconds 800
    $deadline = (Get-Date).AddSeconds(8)
    do {
        Start-Sleep -Milliseconds 200
        $proc.Refresh()
    } while (-not $proc.HasExited -and (Get-Date) -lt $deadline)
    Check $proc.HasExited "会话1 正常退出（无闪退）"
    if (Test-Path $defaultLayout) {
        $m = Get-Content $defaultLayout -Encoding UTF8 | Select-String -Pattern '^log_hidden=1$'
        Check ($null -ne $m -and $m.Count -gt 0) "默认布局含 log_hidden=1"
        Copy-Item $defaultLayout $layout -Force  # 复制给会话2 用
    } else {
        Check $false "默认布局文件已生成（$defaultLayout）"
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}

# 会话 2：带 --layout-load 启动 -> 恢复收起状态
# 注意：不能用 --no-layout（main.c 的 no_layout=1 会同时跳过 --layout-load）
if (Test-Path $layout) {
    $proc2 = Start-Process -FilePath $exe -ArgumentList @($Elf, "--layout-load=$layout") -PassThru
    Write-Output "started pid=$($proc2.Id)"
    try {
        $main2 = [IntPtr]::Zero
        for ($i = 0; $i -lt 50 -and -not $proc2.HasExited; $i++) {
            $main2 = Find-ByClass $proc2.Id "OpenScopeMain"
            if ($main2 -ne [IntPtr]::Zero) { break }
            Start-Sleep -Milliseconds 200
        }
        Check ($main2 -ne [IntPtr]::Zero) "会话2 主窗口创建"
        Check (Wait-MainVisible $main2) "会话2 主窗口可见"
        if ($main2 -ne [IntPtr]::Zero) {
            $lv2 = Find-ChildByClassRetry $main2 "SysListView32"
            $strip2 = Find-ChildByClassRetry $main2 "OSLogStrip"
            Check (-not [OsLogDrawerUi]::IsWindowVisible($lv2)) "重启后消息栏恢复收起（hLog 隐藏）"
            Check ($strip2 -ne [IntPtr]::Zero -and [OsLogDrawerUi]::IsWindowVisible($strip2)) "重启后底部细条可见"
            Check (-not $proc2.HasExited) "会话2 进程未闪退"
        }
    }
    finally {
        if (-not $proc2.HasExited) { $proc2.Kill() }
    }
} else {
    Check $false "会话2 前置布局文件已生成"
}

# 清理：恢复默认布局
if ($hadDefault -and (Test-Path $defaultBackup)) {
    Copy-Item $defaultBackup $defaultLayout -Force
} elseif (-not $hadDefault -and (Test-Path $defaultLayout)) {
    Remove-Item -LiteralPath $defaultLayout -Force
}
if (Test-Path $layout) { Remove-Item -LiteralPath $layout -Force }
if (Test-Path $defaultBackup) { Remove-Item -LiteralPath $defaultBackup -Force }

Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
