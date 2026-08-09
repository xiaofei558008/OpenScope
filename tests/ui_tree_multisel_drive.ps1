# OpenScope 树 Ctrl 多选批量添加 UI 回归（request.md Bug 4 补充）：
#   左侧 elf 变量列表 Ctrl 多选 -> 右键批量添加到波形/数值窗口。
#   跨进程无法伪造 Ctrl/指针式 TVM_SETITEMSTATE，用进程内测试钩子 WM_OS_TREE_TEST_SELECT
#   程序化选中叶子项 [start, start+count)（等价 Ctrl 手选结果），再触发右键菜单命令验证：
#   1. 添加到波形窗口：日志 "树右键批量添加变量: 3 个" + 3 条 波形窗口添加变量
#   2. 添加到数值窗口：日志 "树右键批量添加变量: 2 个" + 2 条 数值窗口添加变量
#   3. 第二个波形窗口：日志 "树右键批量添加变量: 4 个"
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
public class OsMselUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);

    // 自定义消息（与 app.h 一致）
    public const uint WM_OS_TREE_TEST_SELECT = 0x800C;  // WM_APP(0x8000)+12
    // 菜单命令 ID
    public const int IDM_WIN_CHART = 2012;
    public const int IDM_WIN_NUM = 2013;
    public const int IDM_TREE_ADD_CHART = 2305;
    public const int IDM_TREE_ADD_NUM = 2306;
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsMselUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsMselUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsMselUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsMselUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

function Count-LogLines([string]$Pattern) {
    if (-not (Test-Path $log)) { return 0 }
    return (Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern | Measure-Object).Count
}

function Log-Has([string]$Pattern) {
    return ((Count-LogLines $Pattern) -gt 0)
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
    Start-Sleep -Milliseconds 600   # 等待 ELF 解析 + 树填充

    # 1) 波形窗口：多选 3 个叶子 -> 右键"添加到波形窗口"
    Send-Cmd $main ([OsMselUi]::IDM_WIN_CHART)
    Start-Sleep -Milliseconds 250
    $ret = [OsMselUi]::SendMessage($main, [OsMselUi]::WM_OS_TREE_TEST_SELECT, [IntPtr]0, [IntPtr]3)
    Check ($ret.ToInt32() -eq 3) "测试钩子选中 3 个叶子（返回 $($ret.ToInt32())）"
    Send-Cmd $main ([OsMselUi]::IDM_TREE_ADD_CHART)
    Start-Sleep -Milliseconds 300
    Check (Log-Has '树右键批量添加变量: 3 个') "波形: 批量添加日志 3 个"
    Check ((Count-LogLines '波形窗口添加变量: id=') -ge 3) "波形: 3 条逐变量添加日志"

    # 2) 数值窗口：多选 2 个叶子 -> 添加到数值窗口
    Send-Cmd $main ([OsMselUi]::IDM_WIN_NUM)
    Start-Sleep -Milliseconds 250
    $ret2 = [OsMselUi]::SendMessage($main, [OsMselUi]::WM_OS_TREE_TEST_SELECT, [IntPtr]3, [IntPtr]2)
    Check ($ret2.ToInt32() -eq 2) "测试钩子选中 2 个叶子（返回 $($ret2.ToInt32())）"
    Send-Cmd $main ([OsMselUi]::IDM_TREE_ADD_NUM)
    Start-Sleep -Milliseconds 300
    Check (Log-Has '树右键批量添加变量: 2 个') "数值: 批量添加日志 2 个"
    Check ((Count-LogLines '数值窗口添加变量: id=') -ge 2) "数值: 2 条逐变量添加日志"

    # 3) 第二个波形窗口：多选 4 个叶子 -> 批量添加到波形窗口
    Send-Cmd $main ([OsMselUi]::IDM_WIN_CHART)
    Start-Sleep -Milliseconds 250
    $ret3 = [OsMselUi]::SendMessage($main, [OsMselUi]::WM_OS_TREE_TEST_SELECT, [IntPtr]5, [IntPtr]4)
    Check ($ret3.ToInt32() -eq 4) "测试钩子选中 4 个叶子（返回 $($ret3.ToInt32())）"
    Send-Cmd $main ([OsMselUi]::IDM_TREE_ADD_CHART)
    Start-Sleep -Milliseconds 300
    Check (Log-Has '树右键批量添加变量: 4 个') "波形2: 批量添加日志 4 个"

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '树右键批量添加变量|窗口添加变量' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
