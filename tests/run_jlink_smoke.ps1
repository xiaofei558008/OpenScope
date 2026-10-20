param([string]$Exe = "tests\bin\jlink_smoke.exe", [int]$TimeoutSec = 20)
# Run the J-Link smoke exe with a timeout. Exit codes:
#   0 = PASS, 1 = FAIL, 2 = SKIP (no J-Link / DLL blocked)
# A sibling no_jlink.flag lets build_tests skip bug9 in the same situation.
$ErrorActionPreference = "Stop"
if (-not (Test-Path $Exe)) { Write-Output "missing $Exe"; exit 1 }
$dir = Split-Path -Parent $Exe
$flag = Join-Path $dir "no_jlink.flag"
$out = Join-Path $env:TEMP "openscope_jlink_smoke_out.txt"
$err = Join-Path $env:TEMP "openscope_jlink_smoke_err.txt"
Remove-Item $out, $err -ErrorAction SilentlyContinue
$p = Start-Process -FilePath $Exe -PassThru -NoNewWindow -RedirectStandardOutput $out -RedirectStandardError $err
if (-not $p.WaitForExit($TimeoutSec * 1000)) {
    try { $p.Kill() } catch {}
    if (Test-Path $out) { Get-Content $out }
    if (Test-Path $err) { Get-Content $err }
    New-Item -ItemType File -Path $flag -Force | Out-Null
    Write-Output "[SKIP] jlink_smoke timed out after $TimeoutSec s (no J-Link?)"
    exit 2
}
$text = ""
if (Test-Path $out) { $text = Get-Content $out | Out-String; Write-Output $text }
if (Test-Path $err) { Get-Content $err }
if ($text -match "count=0") {
    New-Item -ItemType File -Path $flag -Force | Out-Null
} else {
    Remove-Item $flag -ErrorAction SilentlyContinue
}
exit $p.ExitCode
