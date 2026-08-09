# N9(a) 回归：变量添加进波形/数值窗口后应自动纳入采集（观测勾选）。
# 用 --layout-load 创建带变量的窗口，避免直接操作窗口句柄（P/Invoke 回调不稳定）。
# 验证：日志中"波形窗口添加变量: id=... (观测 N)" 的 N 随添加递增到 2/3。
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }
$log = Join-Path (Split-Path $exe) "openscope.log"
if (Test-Path $log) { Remove-Item -LiteralPath $log -Force }
$layout = Join-Path $env:TEMP "n9_layout.ini"

# 布局：1 个波形窗口（2 变量）+ 1 个数值窗口（1 变量）
# 注意：叶名需与 ELF 完全一致（find_by_name 精确匹配），fDeg 全名是 AbsEnc.fDeg
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
wins=2
[win]
type=chart
title=N9波形
vars=fsin
vars+AbsEnc.fDeg
[win]
type=num
title=N9数值
vars=AbsEnc.fRpm
"@
[System.IO.File]::WriteAllText($layout, $body, (New-Object System.Text.UTF8Encoding $true))

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Milliseconds 400
$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--layout-load=$layout") -PassThru
Start-Sleep -Milliseconds 2500
if ($proc.HasExited) { Write-Output "FAIL app exited early: $($proc.ExitCode)"; exit 4 }
if (-not $proc.HasExited) { Stop-Process -Id $proc.Id -Force }
Start-Sleep -Milliseconds 300

$lines = @()
if (Test-Path $log) { $lines = Get-Content $log -Encoding UTF8 }
$adds = @($lines | Select-String -Pattern '窗口添加变量: id=' | ForEach-Object { $_.Line })
$watch2 = @($lines | Select-String -Pattern '\(观测 2\)').Count
$watch3 = @($lines | Select-String -Pattern '\(观测 3\)').Count
$watch1 = @($lines | Select-String -Pattern '\(观测 1\)').Count

Write-Output "添加日志条数: $($adds.Count)"
foreach ($a in $adds) { Write-Output "  $a" }
# 波形窗口 fsin+fDeg -> 出现 观测 2；数值窗口 fSin -> 观测 3
Check ($watch1 -ge 1) "首次添加后观测=1"
Check ($watch2 -ge 1) "波形窗口添加第2变量后观测=2"
Check ($watch3 -ge 1) "数值窗口添加变量后观测=3"

Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
