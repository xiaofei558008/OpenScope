# OpenScope 变量栏完全隐藏/展开 UI 回归（request.md Bug 18）：
#   Bug18: 左侧 elf 变量树（全局变量窗口）支持"全部隐藏"。
#   验证：
#     1. 双击左侧竖向分隔条（OSSplitter）-> 变量树完全隐藏（hTree 隐藏，左侧细条 OSTreeStrip 可见）
#     2. 悬停/点击左侧细条 OSTreeStrip -> 变量树展开恢复
#     3. 测试钩子 WM_OS_TREE_HIDE=1/0 等价于隐藏/展开
#     4. 钉住状态下手动隐藏后仍保持隐藏（Bug18: 不被 tree_auto_tick 钉住分支强制展开）
#     5. 布局持久化：隐藏后关闭 -> 默认 layout.ini 含 tree_hidden=1 -> 重开恢复隐藏
#     6. 全程观察进程是否闪退
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
$layout = if ($LayoutSave) { $LayoutSave } else { Join-Path $env:TEMP "openscope_bug18_layout.ini" }
if (Test-Path $layout) { Remove-Item -LiteralPath $layout -Force }

# 默认布局路径（WM_CLOSE -> os_layout_save_auto 写入）
$defaultDir = Join-Path $env:LOCALAPPDATA "OpenScope"
$defaultLayout = Join-Path $defaultDir "layout.ini"
$defaultBackup = Join-Path $env:TEMP "openscope_bug18_default_layout.bak"
$hadDefault = Test-Path $defaultLayout
if ($hadDefault) { Copy-Item $defaultLayout $defaultBackup -Force }

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsTreeFoldUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int cmd);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int left, top, right, bottom; }
    [StructLayout(LayoutKind.Sequential)] public struct POINT { public int x, y; }
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsTreeFoldUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsTreeFoldUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsTreeFoldUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsTreeFoldUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsTreeFoldUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

# 等主窗口真正可见（ShowWindow 在模块加载后才执行，见 main.c 注释）；超时后 ShowWindow 兜底
function Wait-MainVisible([IntPtr]$h) {
    $deadline = (Get-Date).AddSeconds(6)
    while ((Get-Date) -lt $deadline) {
        if ([OsTreeFoldUi]::IsWindowVisible($h)) { return $true }
        Start-Sleep -Milliseconds 200
    }
    [OsTreeFoldUi]::ShowWindow($h, 5) | Out-Null  # SW_SHOW 兜底
    Start-Sleep -Milliseconds 300
    return [OsTreeFoldUi]::IsWindowVisible($h)
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

# 会话 1：隐藏 -> 关闭（触发默认布局保存）-> 验证 tree_hidden=1 已持久化
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

    $tree = Find-ChildByClassRetry $main "SysTreeView32"
    Check ($tree -ne [IntPtr]::Zero) "变量树存在"

    $splitter = Find-ChildByClassRetry $main "OSSplitter"
    Check ($splitter -ne [IntPtr]::Zero) "竖向分隔条 OSSplitter 存在"

    # 初始：树展开，细条不可见
    Check ([OsTreeFoldUi]::IsWindowVisible($tree)) "初始变量树可见"
    $strip = Find-ChildByClass $main "OSTreeStrip"
    Check ($strip -eq [IntPtr]::Zero -or -not [OsTreeFoldUi]::IsWindowVisible($strip)) "初始左侧细条不可见"

    # 1) 双击竖向分隔条 -> 完全隐藏
    if ($splitter -ne [IntPtr]::Zero) {
        [OsTreeFoldUi]::SendMessage($splitter, 0x203, [IntPtr]1, [IntPtr]0) | Out-Null  # WM_LBUTTONDBLCLK
        Start-Sleep -Milliseconds 200
    }
    Check (-not [OsTreeFoldUi]::IsWindowVisible($tree)) "双击分隔条后变量树完全隐藏"
    $strip = Find-ChildByClassRetry $main "OSTreeStrip"
    Check ($strip -ne [IntPtr]::Zero -and [OsTreeFoldUi]::IsWindowVisible($strip)) "左侧细条 OSTreeStrip 可见"
    Check (Log-Has '变量栏已完全隐藏') "日志记录变量栏完全隐藏"

    # 2) 点击左侧细条 -> 展开恢复
    if ($strip -ne [IntPtr]::Zero) {
        [OsTreeFoldUi]::SendMessage($strip, 0x201, [IntPtr]1, [IntPtr]0) | Out-Null  # WM_LBUTTONDOWN
        Start-Sleep -Milliseconds 200
    }
    Check ([OsTreeFoldUi]::IsWindowVisible($tree)) "点击左侧细条后变量树展开恢复"
    $strip = Find-ChildByClass $main "OSTreeStrip"
    Check ($strip -eq [IntPtr]::Zero -or -not [OsTreeFoldUi]::IsWindowVisible($strip)) "展开后左侧细条隐藏"
    Check (Log-Has '变量栏展开') "日志记录变量栏展开"

    # 3) 测试钩子 WM_OS_TREE_HIDE=1（WM_APP+33 = 0x8000+33 = 0x8021）隐藏
    [OsTreeFoldUi]::SendMessage($main, 0x8021, [IntPtr]1, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200
    Check (-not [OsTreeFoldUi]::IsWindowVisible($tree)) "钩子隐藏：变量树完全隐藏"
    $strip = Find-ChildByClass $main "OSTreeStrip"
    Check ($strip -ne [IntPtr]::Zero -and [OsTreeFoldUi]::IsWindowVisible($strip)) "钩子隐藏：左侧细条可见"

    # 4) 钉住（tree_auto=0）状态下仍保持隐藏：Bug18 根因——tree_auto_tick 钉住分支会强制展开
    [OsTreeFoldUi]::SendMessage($main, 0x8009, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_OS_TREE_AUTOHIDE=0 钉住
    Start-Sleep -Milliseconds 700   # 等 3~4 个 200ms 轮询 tick，若 Bug18 未修复此处会被强制展开
    Check (-not [OsTreeFoldUi]::IsWindowVisible($tree)) "钉住状态下变量栏仍保持完全隐藏（不被 tick 强制展开）"
    Check (Log-Has '变量栏已完全隐藏') "隐藏状态未被钉住分支破坏"

    # 4b) 钉住模式悬停回归（用户反馈"不能全部缩进全隐藏"的根因）：
    #     旧实现钉住态光标停在细条上（x≤10）200ms 后悬停判定把树重新弹出——
    #     双击分隔条隐藏后光标恰好就在细条位置，树立刻弹回。
    #     修复后：钉住模式悬停细条不展开，必须显式点击。
    #     光标须用【客户区坐标】换算（GetWindowRect 含边框，直接用会落在 x<0 区域）。
    $mwr = New-Object OsTreeFoldUi+RECT
    [OsTreeFoldUi]::GetWindowRect($main, [ref]$mwr) | Out-Null
    $mainH = $mwr.bottom - $mwr.top
    $cpt = New-Object OsTreeFoldUi+POINT
    $cpt.x = 4; $cpt.y = [int]($mainH / 2)
    [OsTreeFoldUi]::ClientToScreen($main, [ref]$cpt) | Out-Null
    [OsTreeFoldUi]::SetCursorPos($cpt.x, $cpt.y) | Out-Null  # 真实光标移到细条上（客户区 x=4）
    Start-Sleep -Milliseconds 700   # 3~4 个 tick
    Check (-not [OsTreeFoldUi]::IsWindowVisible($tree)) "钉住模式下悬停细条不自动展开（保持完全隐藏）"

    # 4c) 自动隐藏模式（tree_auto=1）悬停细条仍自动展开（自动隐藏 UX 不被破坏）
    [OsTreeFoldUi]::SendMessage($main, 0x8009, [IntPtr]1, [IntPtr]0) | Out-Null  # WM_OS_TREE_AUTOHIDE=1
    Start-Sleep -Milliseconds 700   # 光标已在细条上，3~4 个 tick 内应悬停展开
    Check ([OsTreeFoldUi]::IsWindowVisible($tree)) "自动隐藏模式下悬停细条自动展开"
    [OsTreeFoldUi]::SendMessage($main, 0x8009, [IntPtr]0, [IntPtr]0) | Out-Null  # 恢复钉住

    # 4d) 拖拽分隔条到最左（x<60）→ 完全隐藏（用户操作路径：鼠标拽着分隔线往左拉；
    #     旧实现 120px 下限夹紧"拉不过去"）
    [OsTreeFoldUi]::SendMessage($main, 0x8003, [IntPtr]30, [IntPtr]0) | Out-Null  # WM_OS_SPLIT x=30
    Start-Sleep -Milliseconds 200
    Check (-not [OsTreeFoldUi]::IsWindowVisible($tree)) "拖拽分隔条到左侧后变量树完全隐藏"
    $strip = Find-ChildByClass $main "OSTreeStrip"
    Check ($strip -ne [IntPtr]::Zero -and [OsTreeFoldUi]::IsWindowVisible($strip)) "拖拽隐藏后左侧细条可见"
    Check (Log-Has '拖拽分隔条到左侧') "日志记录拖拽分隔条隐藏"

    # 4e) 从隐藏态细条位置向右拖出（x≥16）→ 展开并按拖拽位置设宽
    [OsTreeFoldUi]::SendMessage($main, 0x8003, [IntPtr]200, [IntPtr]0) | Out-Null  # WM_OS_SPLIT x=200
    Start-Sleep -Milliseconds 200
    Check ([OsTreeFoldUi]::IsWindowVisible($tree)) "向右拖拽分隔条后变量树展开"
    Check (Log-Has '变量栏展开: 拖拽分隔条') "日志记录拖拽分隔条展开"

    # 5) 钩子 WM_OS_TREE_HIDE=0 展开
    [OsTreeFoldUi]::SendMessage($main, 0x8021, [IntPtr]0, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200
    Check ([OsTreeFoldUi]::IsWindowVisible($tree)) "钩子展开：变量树可见"

    # 6) 持久化：钩子隐藏后关闭 -> WM_CLOSE 触发 os_layout_save_auto 写默认 layout.ini
    [OsTreeFoldUi]::SendMessage($main, 0x8021, [IntPtr]1, [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 200
    Check (-not [OsTreeFoldUi]::IsWindowVisible($tree)) "关闭前变量树已完全隐藏"
    [OsTreeFoldUi]::SendMessage($main, 0x0010, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_CLOSE
    Start-Sleep -Milliseconds 800
    $deadline = (Get-Date).AddSeconds(8)
    do {
        Start-Sleep -Milliseconds 200
        $proc.Refresh()
    } while (-not $proc.HasExited -and (Get-Date) -lt $deadline)
    Check $proc.HasExited "会话1 正常退出（无闪退）"
    if (Test-Path $defaultLayout) {
        $m = Get-Content $defaultLayout -Encoding UTF8 | Select-String -Pattern '^tree_hidden=1$'
        Check ($null -ne $m -and $m.Count -gt 0) "默认布局含 tree_hidden=1"
        Copy-Item $defaultLayout $layout -Force  # 复制给会话2 用
    } else {
        Check $false "默认布局文件已生成（$defaultLayout）"
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}

# 会话 2：带 --layout-load 启动 -> 恢复隐藏状态
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
        if ($main2 -ne [IntPtr]::Zero) { Check (Wait-MainVisible $main2) "会话2 主窗口可见" }

        $tree2 = Find-ChildByClassRetry $main2 "SysTreeView32"
        Check ($tree2 -ne [IntPtr]::Zero) "会话2 变量树存在"
        if ($tree2 -ne [IntPtr]::Zero) {
            Start-Sleep -Milliseconds 300
            Check (-not [OsTreeFoldUi]::IsWindowVisible($tree2)) "会话2 恢复：变量树隐藏（layout 持久化）"
            $strip2 = Find-ChildByClass $main2 "OSTreeStrip"
            Check ($strip2 -ne [IntPtr]::Zero -and [OsTreeFoldUi]::IsWindowVisible($strip2)) "会话2 恢复：左侧细条可见"

            # 点击细条 -> 展开
            if ($strip2 -ne [IntPtr]::Zero) {
                [OsTreeFoldUi]::SendMessage($strip2, 0x201, [IntPtr]1, [IntPtr]0) | Out-Null
                Start-Sleep -Milliseconds 200
            }
            Check ([OsTreeFoldUi]::IsWindowVisible($tree2)) "会话2 点击细条后展开恢复"
        }

        Check (-not $proc2.HasExited) "会话2 进程未闪退"
    }
    finally {
        if (-not $proc2.HasExited) { $proc2.Kill() }
    }
} else {
    Check $false "会话2 布局文件缺失（跳过恢复验证）"
}

# 恢复用户默认布局
if ($hadDefault) { Copy-Item $defaultBackup $defaultLayout -Force }
elseif (Test-Path $defaultLayout) { Remove-Item -LiteralPath $defaultLayout -Force }

Write-Output "--- 日志关键行 ---"
if (Test-Path $log) {
    Get-Content $log -Encoding UTF8 | Select-String -Pattern '变量栏|FATAL' | ForEach-Object { $_.Line }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
