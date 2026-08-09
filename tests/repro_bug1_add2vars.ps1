# Repro Bug 1: 添加 2 个变量后 App 异常退出
# 启动 -> 建波形窗口 -> 加 fsin -> 再在树中选择第二个叶变量并加入同一波形窗口 -> 观察崩溃
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = "",
    [string]$Leaf2 = "AbsEnc.fDeg"
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$replayCsv = Join-Path $root "tests\chart_replay.csv"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class ReproUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", EntryPoint="SendMessage", CharSet=CharSet.Unicode)] public static extern IntPtr SendMessageT(IntPtr h, uint msg, IntPtr wp, StringBuilder lp);
    [DllImport("user32.dll")] public static extern IntPtr SendMessageS(IntPtr h, uint msg, IntPtr wp, string lp);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int left, top, right, bottom; }
    public const uint TVM_SELECTITEM = 0x110B;
    public const uint TVM_GETNEXTITEM = 0x110A;
    public const uint TVM_GETITEMW = 0x110C;
    public const uint TVM_EXPAND = 0x1102;
    public const uint TVGN_CHILD = 0x4, TVGN_NEXT = 0x1, TVGN_ROOT = 0x0, TVGN_CARET = 0x9;
    public const uint TVE_EXPAND = 0x2;
    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Unicode)]
    public struct TVITEMW { public uint mask; public IntPtr hItem; public uint state; public uint stateMask;
        public IntPtr pszText; public int cchTextMax; public int iImage; public int iSelectedImage;
        public int cChildren; public IntPtr lParam; }
    public const uint TVIF_TEXT = 0x1, TVIF_PARAM = 0x4, TVIF_HANDLE = 0x10;
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h, $l)
        $wp = [uint32]0
        [ReproUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [ReproUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [ReproUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}
function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [ReproUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [ReproUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}
function Send-Cmd([IntPtr]$H, [int]$Id) {
    [ReproUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}
function Get-ItemText([IntPtr]$tree, [IntPtr]$hItem) {
    $ti = New-Object ReproUi+TVITEMW
    $buf = [Runtime.InteropServices.Marshal]::AllocHGlobal(512)
    $ti.mask = 0x1  # TVIF_TEXT
    $ti.hItem = $hItem
    $ti.pszText = $buf
    $ti.cchTextMax = 256
    $p = [Runtime.InteropServices.Marshal]::AllocHGlobal([Runtime.InteropServices.Marshal]::SizeOf($ti))
    [Runtime.InteropServices.Marshal]::StructureToPtr($ti, $p, $true)
    [ReproUi]::SendMessage($tree, 0x110C, [IntPtr]0, $p) | Out-Null
    [Runtime.InteropServices.Marshal]::FreeHGlobal($p)
    $txt = [Runtime.InteropServices.Marshal]::PtrToStringUni($buf)
    [Runtime.InteropServices.Marshal]::FreeHGlobal($buf)
    return $txt
}
function Find-ItemByName([IntPtr]$tree, [string]$Name) {
    # 支持带点的叶名 "Parent.leaf"：先定位父根节点（文本=Parent），再在子节点中找文本=leaf
    $dot = $Name.IndexOf('.')
    if ($dot -gt 0) {
        $pName = $Name.Substring(0, $dot)
        $cName = $Name.Substring($dot + 1)
        $root = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0, [IntPtr]0)
        if ($root -eq [IntPtr]::Zero) { return [IntPtr]::Zero }
        $node = $root
        while ($node -ne [IntPtr]::Zero) {
            if ((Get-ItemText $tree $node) -eq $pName) {
                $child = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0x4, $node)
                while ($child -ne [IntPtr]::Zero) {
                    if ((Get-ItemText $tree $child) -eq $cName) { return $child }
                    $child = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0x1, $child)
                }
                return [IntPtr]::Zero
            }
            $node = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0x1, $node)
        }
        return [IntPtr]::Zero
    }
    # 无点：广度优先精确匹配
    $root = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0, [IntPtr]0)
    if ($root -eq [IntPtr]::Zero) { return [IntPtr]::Zero }
    $stack = New-Object System.Collections.Stack
    $stack.Push($root)
    while ($stack.Count -gt 0) {
        $node = $stack.Pop()
        if ($node -eq [IntPtr]::Zero) { continue }
        $txt = Get-ItemText $tree $node
        if ($txt -eq $Name) { return $node }
        $child = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0x4, $node)
        while ($child -ne [IntPtr]::Zero) {
            $stack.Push($child)
            $child = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0x1, $child)
        }
    }
    return [IntPtr]::Zero
}

$existing = @(Get-Process OpenScope -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) {
    Write-Output "ERROR: OpenScope already running: $($existing.Id -join ',')"
    exit 3
}
$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--select-leaf=fsin", "--replay=$replayCsv", "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Write-Output "main=$($main)"
    if ($main -eq [IntPtr]::Zero) { Write-Output "FAIL main window"; exit 1 }

    $tree = Find-ChildByClass $main "SysTreeView32"
    Write-Output "tree=$($tree)"
    # 校验句柄：TVM_GETCOUNT = 0x1105
    $cnt = [ReproUi]::SendMessage($tree, 0x1105, [IntPtr]0, [IntPtr]0).ToInt64()
    Write-Output "treeCount=$cnt"
    # 列出所有顶层根节点文本
    $dbgRoot = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0, [IntPtr]0)
    Write-Output "rootItem=$($dbgRoot)"
    $dbgNode = $dbgRoot
    $n = 0
    while ($dbgNode -ne [IntPtr]::Zero -and $n -lt 8) {
        $t = Get-ItemText $tree $dbgNode
        Write-Output "  ROOT[$n] [$t]"
        $dbgNode = [ReproUi]::SendMessage($tree, 0x110A, [IntPtr]0x1, $dbgNode)
        $n++
    }

    # 建波形窗口并加 fsin
    Send-Cmd $main 2012
    Start-Sleep -Milliseconds 400
    Send-Cmd $main 2305   # IDM_TREE_ADD_CHART
    Start-Sleep -Milliseconds 1500
    Write-Output "added var1 (fsin), exited=$($proc.HasExited)"

    # 选第二个叶变量并加入同一波形窗口
    $n2 = Find-ItemByName $tree $Leaf2
    Write-Output "leaf2 item=$($n2)"
    if ($n2 -ne [IntPtr]::Zero) {
        [ReproUi]::SendMessage($tree, 0x110B, [IntPtr]0x9, $n2) | Out-Null  # TVM_SELECTITEM TVGN_CARET
        Start-Sleep -Milliseconds 200
        [ReproUi]::SendMessage($tree, 0x1102, [IntPtr]0x2, $n2) | Out-Null  # TVM_EXPAND
        Start-Sleep -Milliseconds 200
        Send-Cmd $main 2305
        Start-Sleep -Milliseconds 2000
        Write-Output "added var2, exited=$($proc.HasExited)"
    } else {
        Write-Output "leaf2 not found; trying direct add via tree select"
    }

    Start-Sleep -Milliseconds 3000
    Write-Output "final exited=$($proc.HasExited) exitcode=$(if ($proc.HasExited) { $proc.ExitCode } else { 'N/A' })"
    if ($proc.HasExited) {
        Write-Output "CRASH REPRODUCED: exit code $($proc.ExitCode)"
        if (Test-Path $log) { Get-Content $log -Encoding UTF8 | Select-Object -Last 30 }
        exit 4
    }
    Write-Output "NO CRASH"
    exit 0
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
