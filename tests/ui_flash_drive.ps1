# Flash dialog smoke (no hardware program): open dialog, check controls, close.
param(
    [string]$Elf = "D:\OpenScope\tests\linix_stm32l031_v1.2.out",
    [string]$ExePath = ""
)
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsFlashUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr h, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern int GetDlgCtrlID(IntPtr h);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = {
        param($h, $l)
        $wp = [uint32]0
        [OsFlashUi]::GetWindowThreadProcessId($h, [ref]$wp) | Out-Null
        if ($wp -eq $ProcId) {
            $sb = New-Object System.Text.StringBuilder 256
            [OsFlashUi]::GetClassName($h, $sb, 256) | Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit = $h; return $false }
        }
        return $true
    }
    [OsFlashUi]::EnumWindows($cb, [IntPtr]::Zero) | Out-Null
    return $script:hit
}
function Get-ChildIds([IntPtr]$Parent) {
    $script:ids = @()
    $cb = { param($h, $l) $script:ids += [OsFlashUi]::GetDlgCtrlID($h); return $true }
    [OsFlashUi]::EnumChildWindows($Parent, $cb, [IntPtr]::Zero) | Out-Null
    return $script:ids
}

$root = Split-Path -Parent $PSScriptRoot
if ($ExePath) { $exe = $ExePath } else { $exe = Join-Path $root "bin\Release\OpenScope.exe" }

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}

$proc = Start-Process -FilePath $exe -ArgumentList @($Elf, "--no-layout") -PassThru
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "main window"
    Start-Sleep -Milliseconds 800

    # IDM_FLASH = 2030
    [OsFlashUi]::PostMessage($main, 0x111, [IntPtr]2030, [IntPtr]0) | Out-Null
    $dlg = [IntPtr]::Zero
    for ($i = 0; $i -lt 40 -and $dlg -eq [IntPtr]::Zero -and -not $proc.HasExited; $i++) {
        $dlg = Find-ByClass $proc.Id "OSDlgFlash"
        if ($dlg -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 150
    }
    Check ($dlg -ne [IntPtr]::Zero) "OSDlgFlash open"
    if ($dlg -ne [IntPtr]::Zero) {
        $ids = Get-ChildIds $dlg
        Check ($ids -contains 2901) "path edit"
        Check ($ids -contains 2902) "browse button"
        Check ($ids -contains 2908) "download button"
        Check ($ids -contains 2909) "erase button"
        [OsFlashUi]::SendMessage($dlg, 0x111, [IntPtr]2910, [IntPtr]0) | Out-Null
        Start-Sleep -Milliseconds 300
        Check ((Find-ByClass $proc.Id "OSDlgFlash") -eq [IntPtr]::Zero) "dialog closed"
    }
    Check (-not $proc.HasExited) "process alive"
}
finally {
    if (-not $proc.HasExited) { $proc.Kill() }
}
Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
