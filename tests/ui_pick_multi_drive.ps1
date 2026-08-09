# OpenScope 变量选择对话框多选功能 UI 回归（request.md 新增特性 13a）：
#   1. 添加变量弹窗的列表为 ListView（SysListView32，报表模式，原生 Ctrl+单击/Shift 范围多选）
#   2. 扩展样式含 LVS_EX_FULLROWSELECT
#   3. 模糊搜索键入后列表填充（数量 >= 2）
#   4. 测试钩子 WM_OS_PICK_TEST_SELECT 范围选中 = Ctrl/Shift 手选结果（含起止本身）
#   5. 确定 -> 波形窗口一次性批量添加全部选中变量（日志 + 数量核对）
#   6. 全程观察进程是否闪退
# 数据来源：加载 ELF 后变量树即时可用，无需真实 MCU / 回放。
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
public class OsPickUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr GetDlgItem(IntPtr parent, int id);
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
        [OsPickUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsPickUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsPickUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsPickUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsPickUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsPickUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
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

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--select-leaf=fsin", "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id)"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 500   # 等待变量树构建完成

    # 创建波形窗口并打开“添加变量”弹窗（异步 PostMessage，避免 SendMessage 阻塞至弹窗关闭）
    Send-Cmd $main 2012   # IDM_WIN_CHART
    Start-Sleep -Milliseconds 300
    $chart = Find-ChildByClass $main "OSChartWin"
    Check ($chart -ne [IntPtr]::Zero) "波形窗口已创建"
    [OsPickUi]::PostMessage($chart, 0x111, [IntPtr]3001, [IntPtr]0) | Out-Null  # MENU_CHART_ADD (async, 不阻塞)
    $dlg = [IntPtr]::Zero
    for ($i = 0; $i -lt 40 -and -not $proc.HasExited; $i++) {
        $dlg = Find-ByClass $proc.Id "OSDlgPick"
        if ($dlg -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($dlg -ne [IntPtr]::Zero) "添加变量弹窗已打开"

    if ($dlg -ne [IntPtr]::Zero) {
        $hEdit = [OsPickUi]::GetDlgItem($dlg, 2403)   # IDD_PICK_EDIT
        $hList = [OsPickUi]::GetDlgItem($dlg, 2404)   # IDD_PICK_LIST
        Check ($hList -ne [IntPtr]::Zero) "弹窗内列表控件存在"

        # 1. 列表必须是 ListView（SysListView32）且带 FULLROWSELECT（原生 Ctrl/Shift 多选前提）
        $clsSb = New-Object System.Text.StringBuilder 128
        [OsPickUi]::GetClassName($hList, $clsSb, 128) | Out-Null
        Check ($clsSb.ToString() -eq "SysListView32") "列表控件为 ListView（实际 $($clsSb.ToString())）"
        $exStyle = [OsPickUi]::SendMessage($hList, 0x1037, [IntPtr]0, [IntPtr]0).ToInt64()  # LVM_GETEXTENDEDLISTVIEWSTYLE
        Check (($exStyle -band 0x20) -ne 0) "扩展样式含 LVS_EX_FULLROWSELECT"

        # 2. 键入 'f' 触发模糊搜索填充列表
        [OsPickUi]::SendMessage($hEdit, 0x102, [IntPtr][int][char]'f', [IntPtr]1) | Out-Null  # WM_CHAR 'f'
        $n = 0
        for ($i = 0; $i -lt 30; $i++) {
            $n = [OsPickUi]::SendMessage($hList, 0x1004, [IntPtr]0, [IntPtr]0).ToInt64()  # LVM_GETITEMCOUNT
            if ($n -ge 2) { break }
            Start-Sleep -Milliseconds 200
        }
        Check ($n -ge 2) "模糊搜索列表填充 >= 2 项（实际 $n）"

        # 3. 测试钩子：程序化选中 [0,2) 范围（等价 Shift 起止范围选，含起止本身）
        [OsPickUi]::SendMessage($dlg, 0x801E, [IntPtr]0, [IntPtr]2) | Out-Null
        $st0 = [OsPickUi]::SendMessage($hList, 0x102C, [IntPtr]0, [IntPtr]2).ToInt64()   # LVM_GETITEMSTATE(0,LVIS_SELECTED)
        $st1 = [OsPickUi]::SendMessage($hList, 0x102C, [IntPtr]1, [IntPtr]2).ToInt64()
        $st2 = [OsPickUi]::SendMessage($hList, 0x102C, [IntPtr]2, [IntPtr]2).ToInt64()
        Check (($st0 -band 2) -ne 0 -and ($st1 -band 2) -ne 0) "范围选中含起止本身 (0,1 已选)"
        if ($n -gt 2) {
            Check (($st2 -band 2) -eq 0) "范围外第 3 项未选中"
        }

        # 4. 测试钩子：全选（等价 Ctrl+A 结果）
        [OsPickUi]::SendMessage($dlg, 0x801E, [IntPtr]0, [IntPtr]$n) | Out-Null
        $stFirst = [OsPickUi]::SendMessage($hList, 0x102C, [IntPtr]0, [IntPtr]2).ToInt64()
        $stLast  = [OsPickUi]::SendMessage($hList, 0x102C, [IntPtr]($n - 1), [IntPtr]2).ToInt64()
        Check (($stFirst -band 2) -ne 0 -and ($stLast -band 2) -ne 0) "全选覆盖首项与末项"

        # 5. 回退为仅选中前 2 项，点击确定批量添加
        [OsPickUi]::SendMessage($dlg, 0x801E, [IntPtr]0, [IntPtr]2) | Out-Null
        Send-Cmd $dlg 2401   # IDD_PICK_OK
        Start-Sleep -Milliseconds 400
        Check (Log-Has '波形窗口批量添加变量: 2 个') "波形窗口批量添加日志 (2 个)"
        $addCount = @(Get-Content $log -Encoding UTF8 | Select-String -Pattern '波形窗口添加变量: id=').Count
        Check ($addCount -eq 2) "实际逐条添加日志条数 = 2（实际 $addCount）"
    }

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '添加变量|批量|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
