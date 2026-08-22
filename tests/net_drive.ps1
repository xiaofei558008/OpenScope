# Two-instance network E2E test: A listens 127.0.0.1:10000, B connects + ELF sync.
$ErrorActionPreference = "Stop"
$exe = "D:\OpenScope\bin\Release\OpenScope.exe"
$log = "D:\OpenScope\bin\Release\openscope.log"

Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Milliseconds 300

if (-not (Test-Path $log)) { New-Item -ItemType File -Path $log | Out-Null }
$startSize = (Get-Item $log).Length

$a = Start-Process -FilePath $exe -ArgumentList "--no-layout","--net-listen=10000","--net-exit=7000" -PassThru
Start-Sleep -Seconds 2

$b = Start-Process -FilePath $exe -ArgumentList "--no-layout","--net-connect=127.0.0.1:10000","--net-sync","--net-exit=7000" -PassThru

Start-Sleep -Seconds 9
Get-Process OpenScope -ErrorAction SilentlyContinue | Stop-Process -Force

$bytes = (Get-Item $log).Length - $startSize
$newlog = ""
if ($bytes -gt 0) {
    $fs = [System.IO.File]::Open($log, 'Open', 'Read', 'ReadWrite')
    $fs.Seek($startSize, 'Begin') | Out-Null
    $sr = New-Object System.IO.StreamReader($fs, [System.Text.Encoding]::UTF8)
    $newlog = $sr.ReadToEnd()
    $sr.Close(); $fs.Close()
}

# ASCII-only assertions (log messages are UTF-8 Chinese + ASCII markers)
$rc0 = ([regex]::Matches($newlog, "10000 rc=0")).Count   # listen + connect both rc=0
$elf = ([regex]::Matches($newlog, "ELF")).Count          # ELF sync messages

$fail = 0
if ($rc0 -ge 2) { Write-Output "PASS  listen+connect rc=0 (count=$rc0)" } else { Write-Output "FAIL  listen+connect rc=0 (count=$rc0)"; $fail++ }
if ($elf -ge 3) { Write-Output "PASS  ELF sync messages (count=$elf)" } else { Write-Output "FAIL  ELF sync messages (count=$elf)"; $fail++ }

Write-Output "---- new log ----"
Write-Output $newlog

if ($fail -eq 0) { Write-Output "NET DRIVE ALL PASS"; exit 0 } else { Write-Output "NET DRIVE FAILED: $fail"; exit 1 }
