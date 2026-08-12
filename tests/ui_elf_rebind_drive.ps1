# OpenScope ELF 重载窗口变量按名重绑 UI 回归（request.md 需求2 加固）：
#   场景：用户重新编译后 ELF 变量顺序漂移/地址变化。旧实现窗口只存叶下标，
#   重建后下标指向不同变量 => 静默绑错变量。新实现按变量全名重绑 leaf_id。
#   流程：
#     1. 临时目录放 elf_sample.out 副本（g_counter id=0 @0x20000000），命令行加载
#     2. 数值窗口 + 波形窗口各添加 g_counter（树测试钩子选中叶 0 -> 批量添加）
#     3. 用 elf_sample_v2.out 覆盖（新增 g_extra 在首位，g_counter id=1 @0x20000004）
#     4. 2s mtime 轮询弹出"ELF 更新" -> 点"是" -> 验证：
#        - 日志 "数值变量重绑: g_counter id=0->1 @0x20000004"
#        - 日志 "波形变量重绑: g_counter id=0->1 @0x20000004"
#        - "变量表重建: 5 个叶子"（v2 多 g_extra）
#     5. 再用 v1 覆盖回去 -> 点"是" -> id=1->0 @0x20000000（双向重绑稳定）
#     6. 全程进程不闪退
param(
    [string]$ExePath = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }

$tmp = Join-Path $env:TEMP "openscope_rebind_test"
if (Test-Path $tmp) { Remove-Item -Recurse -Force $tmp }
New-Item -ItemType Directory -Path $tmp | Out-Null
$fw = Join-Path $tmp "fw.out"
Copy-Item (Join-Path $root "tests\elf_sample.out") $fw -Force

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsRebindUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    public const uint WM_COMMAND = 0x111;
    public const uint WM_OS_TREE_TEST_SELECT = 0x800C;  // WM_APP+12
    public const int IDM_WIN_CHART = 2012;
    public const int IDM_WIN_NUM = 2013;
    public const int IDM_TREE_ADD_CHART = 2305;
    public const int IDM_TREE_ADD_NUM = 2306;
    public const int IDYES = 6;
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsRebindUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsRebindUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsRebindUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ElfUpdateDlg([int]$ProcId) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsRebindUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsRebindUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq "#32770") {
                $t = New-Object System.Text.StringBuilder 256
                [OsRebindUi]::GetWindowText($h, $t, 256) | Out-Null
                if ($t.ToString() -eq "ELF 更新") { $script:hit = $h; return $false }
            }
        }
        return $true
    }
    [OsRebindUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Wait-ElfUpdateDlg([int]$ProcId, [int]$Ms) {
    $deadline = (Get-Date).AddMilliseconds($Ms)
    while ((Get-Date) -lt $deadline) {
        $d = Find-ElfUpdateDlg $ProcId
        if ($d -ne [IntPtr]::Zero) { return $d }
        Start-Sleep -Milliseconds 200
    }
    return [IntPtr]::Zero
}

function Count-LogLines([string]$Pattern) {
    if (-not (Test-Path $log)) { return 0 }
    return (Get-Content $log -Encoding UTF8 | Select-String -Pattern $Pattern | Measure-Object).Count
}

function Log-Has([string]$Pattern) { return ((Count-LogLines $Pattern) -gt 0) }

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

$proc = Start-Process -FilePath $exe -ArgumentList @($fw, "--no-layout") -PassThru
Write-Output "started pid=$($proc.Id) fw=$fw"
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 800   # ELF 解析 + 树填充
    Check (Log-Has 'ELF 变量表重建: 4 个叶子') "v1 加载：4 个叶子 (g_counter/g_cfg.a/g_cfg.b/g_raw)"

    # 数值窗口添加 g_counter（叶 0）
    [OsRebindUi]::SendMessage($main, [OsRebindUi]::WM_COMMAND, [IntPtr]([OsRebindUi]::IDM_WIN_NUM), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    $r = [OsRebindUi]::SendMessage($main, [OsRebindUi]::WM_OS_TREE_TEST_SELECT, [IntPtr]0, [IntPtr]1)
    Check ($r.ToInt32() -eq 1) "测试钩子选中 g_counter（叶0）"
    [OsRebindUi]::SendMessage($main, [OsRebindUi]::WM_COMMAND, [IntPtr]([OsRebindUi]::IDM_TREE_ADD_NUM), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    Check (Log-Has '数值窗口添加变量: id=0') "数值窗口添加 g_counter id=0"

    # 波形窗口添加 g_counter（叶 0）
    [OsRebindUi]::SendMessage($main, [OsRebindUi]::WM_COMMAND, [IntPtr]([OsRebindUi]::IDM_WIN_CHART), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    [OsRebindUi]::SendMessage($main, [OsRebindUi]::WM_OS_TREE_TEST_SELECT, [IntPtr]0, [IntPtr]1) | Out-Null
    [OsRebindUi]::SendMessage($main, [OsRebindUi]::WM_COMMAND, [IntPtr]([OsRebindUi]::IDM_TREE_ADD_CHART), [IntPtr]0) | Out-Null
    Start-Sleep -Milliseconds 300
    Check (Log-Has '波形窗口添加变量: id=0') "波形窗口添加 g_counter id=0"

    # 用 v2 覆盖（模拟重新编译：顺序漂移 + 地址变化）
    Copy-Item (Join-Path $root "tests\elf_sample_v2.out") $fw -Force
    $dlg = Wait-ElfUpdateDlg $proc.Id 5000
    Check ($dlg -ne [IntPtr]::Zero) "检测到 ELF 更新弹窗（2s mtime 轮询）"
    if ($dlg -ne [IntPtr]::Zero) {
        [OsRebindUi]::SendMessage($dlg, [OsRebindUi]::WM_COMMAND, [IntPtr]([OsRebindUi]::IDYES), [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 800
    }
    Check (Log-Has 'ELF 变量表重建: 5 个叶子') "v2 重建：5 个叶子（新增 g_extra）"
    Check (Log-Has '数值变量重绑: g_counter id=0->1 @0x20000004') "数值窗口 g_counter 重绑 id 0->1 新地址 0x20000004"
    Check (Log-Has '波形变量重绑: g_counter id=0->1 @0x20000004') "波形窗口 g_counter 重绑 id 0->1 新地址 0x20000004"
    Check (Log-Has '数值窗口变量重绑: 成功 1 缺失 0') "数值窗口重绑汇总 成功1 缺失0"
    Check (Log-Has '波形窗口变量重绑: 成功 1 缺失 0') "波形窗口重绑汇总 成功1 缺失0"
    # 需求2 回归：树重建期间 TVN_ITEMCHANGED 用旧 lParam 回写新叶表，曾把 g_extra 误置观测
    Check (Log-Has 'ELF 变量表重建: 5 个叶子（原观测 1 个，缺失 0 个）') "v2 重建无伪观测（原观测1 缺失0）"
    Start-Sleep -Milliseconds 400
    $script:hitMiss = [IntPtr]::Zero
    $cbMiss = {
        param($h, $l)
        $wp = [uint32]0
        [OsRebindUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $proc.Id) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsRebindUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq "#32770") {
                $t = New-Object System.Text.StringBuilder 256
                [OsRebindUi]::GetWindowText($h, $t, 256) | Out-Null
                if ($t.ToString() -eq "变量缺失") { $script:hitMiss = $h; return $false }
            }
        }
        return $true
    }
    [OsRebindUi]::EnumWindows($cbMiss, [IntPtr]::Zero) | Out-Null
    $missDlg = $script:hitMiss
    Check ($missDlg -eq [IntPtr]::Zero) "无误报的变量缺失弹窗"

    # 再覆盖回 v1（双向重绑稳定）
    Copy-Item (Join-Path $root "tests\elf_sample.out") $fw -Force
    $dlg2 = Wait-ElfUpdateDlg $proc.Id 5000
    Check ($dlg2 -ne [IntPtr]::Zero) "第二次 ELF 更新弹窗"
    if ($dlg2 -ne [IntPtr]::Zero) {
        [OsRebindUi]::SendMessage($dlg2, [OsRebindUi]::WM_COMMAND, [IntPtr]([OsRebindUi]::IDYES), [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 800
    }
    Check (Log-Has '数值变量重绑: g_counter id=1->0 @0x20000000') "回滚 v1：数值窗口 g_counter 重绑 id 1->0 @0x20000000"
    Check (Log-Has '波形变量重绑: g_counter id=1->0 @0x20000000') "回滚 v1：波形窗口 g_counter 重绑 id 1->0 @0x20000000"
    Check ((Count-LogLines '变量表重建: 4 个叶子（原观测 1 个，缺失 0 个）') -ge 1) "回滚 v1 无伪观测（原观测1 缺失0）"

    Check (-not $proc.HasExited) "进程未闪退"
    Write-Output "--- 日志关键行 ---"
    if (Test-Path $log) {
        Get-Content $log -Encoding UTF8 | Select-String -Pattern '重绑|变量表重建|FATAL' | ForEach-Object { $_.Line }
    }
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
    if (Test-Path $tmp) { Remove-Item -Recurse -Force $tmp }
}
Write-Output ($(if ($fails -eq 0) { "ALL PASS" } else { "FAILURES: $fails" }))
exit $fails
