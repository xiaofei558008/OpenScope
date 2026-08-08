# OpenScope 布局持久化 UI 回归：
#   A. 创建 3 窗口+变量 -> 关闭应用（自动保存布局到 %LOCALAPPDATA%\OpenScope\layout.ini）
#   B. 重新启动（带 ELF）-> 自动恢复窗口与变量（待解析变量在 ELF 后补挂）
#   C. 用 --layout-load 显式导入布局文件
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = "",
    [string]$LeafName = "AbsEnc.Param.anon.AngleBit"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
$autoLayout = Join-Path $env:LOCALAPPDATA "OpenScope\layout.ini"
$shareLayout = Join-Path $root "dist\_test_layout.ini"

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsLayUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsLayUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsLayUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsLayUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Find-ChildByClass([IntPtr]$P, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $sb = New-Object System.Text.StringBuilder 128
        [OsLayUi]::GetClassName($h, $sb, 128) | Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        return $true
    }
    [OsLayUi]::EnumChildWindows($P, $cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}

function Send-Cmd([IntPtr]$H, [int]$Id) {
    [OsLayUi]::SendMessage($H, 0x111, [IntPtr]$Id, [IntPtr]0) | Out-Null
}

function Start-App([string[]]$ArgList) {
    Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force
    Start-Sleep -Milliseconds 300
    return Start-Process -FilePath $exe -ArgumentList $ArgList -PassThru
}

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

function Wait-Main([System.Diagnostics.Process]$Proc) {
    for ($i = 0; $i -lt 50 -and -not $Proc.HasExited; $i++) {
        $m = Find-ByClass $Proc.Id "OpenScopeMain"
        if ($m -ne [IntPtr]::Zero) { return $m }
        Start-Sleep -Milliseconds 200
    }
    return [IntPtr]::Zero
}

# ---------- A：建窗口+变量 -> 关闭自动保存 ----------
if (Test-Path $autoLayout) { Remove-Item -LiteralPath $autoLayout -Force }
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$p = Start-App @($Elf, "--select-leaf=$LeafName")
$main = Wait-Main $p
Check ($main -ne [IntPtr]::Zero) "A: 主窗口创建"
Start-Sleep -Milliseconds 600
Send-Cmd $main 2012; Send-Cmd $main 2013; Send-Cmd $main 2200
Start-Sleep -Milliseconds 400
Send-Cmd $main 2305; Send-Cmd $main 2306; Send-Cmd $main 2307
Start-Sleep -Milliseconds 400
[OsLayUi]::SendMessage($main, 0x10, [IntPtr]0, [IntPtr]0) | Out-Null  # WM_CLOSE
$p.WaitForExit(5000) | Out-Null
Check (Test-Path $autoLayout) "A: 关闭后自动保存布局文件"
$content = ""
if (Test-Path $autoLayout) {
    $content = Get-Content $autoLayout -Encoding UTF8 -Raw
    Check ($content -match 'wins=3' -and $content -match 'type=chart' -and
           $content -match 'type=num' -and $content -match 'type=scope.bar') "A: 布局含 3 窗口类型"
    Check ($content -match 'vars=.*AngleBit') "A: 布局含变量名"
}

# ---------- B：重启（带 ELF）自动恢复 ----------
$p2 = Start-App @($Elf)
$main2 = Wait-Main $p2
Check ($main2 -ne [IntPtr]::Zero) "B: 主窗口创建"
$tab2 = Find-ChildByClass $main2 "SysTabControl32"
$n2 = [OsLayUi]::SendMessage($tab2, 0x1304, [IntPtr]0, [IntPtr]0).ToInt64()
Check ($n2 -eq 3) "B: 恢复 3 个窗口 Tab（实际 $n2）"
Start-Sleep -Milliseconds 800
$added2 = 0
if (Test-Path $log) {
    $added2 = (Get-Content $log -Encoding UTF8 | Select-String -Pattern '添加变量' | Measure-Object).Count
}
Check ($added2 -ge 3) "B: ELF 后补挂变量（日志 $added2 条）"
if (-not $p2.HasExited) { Stop-Process -Id $p2.Id -Force }

# ---------- C：显式导入布局（--layout-load） ----------
if (Test-Path $shareLayout) { Remove-Item -LiteralPath $shareLayout -Force }
Copy-Item -LiteralPath $autoLayout -Destination $shareLayout -Force
$p3 = Start-App @($Elf, "--layout-load=$shareLayout")
$main3 = Wait-Main $p3
$tab3 = Find-ChildByClass $main3 "SysTabControl32"
$n3 = [OsLayUi]::SendMessage($tab3, 0x1304, [IntPtr]0, [IntPtr]0).ToInt64()
Check ($n3 -eq 3) "C: --layout-load 导入 3 窗口（实际 $n3）"
if (-not $p3.HasExited) { Stop-Process -Id $p3.Id -Force }

Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
