param([string]$ExePath = "D:/OpenScope/bin/Release/OpenScope.exe")
$ErrorActionPreference = "Stop"
Add-Type @"
using System; using System.Runtime.InteropServices; using System.Text;
public class COUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
}
"@
function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit=[IntPtr]::Zero
    $cb={param($h,$l) $wp=[uint32]0; [COUi]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if($wp -eq $ProcId){$sb=New-Object System.Text.StringBuilder 256;[COUi]::GetClassName($h,$sb,256)|Out-Null
            if($sb.ToString() -eq $Class){$script:hit=$h;return $false}};return $true}
    [COUi]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null;return $script:hit
}
$log = "D:/OpenScope/bin/Release/openscope.log"
if (Test-Path $log) { Remove-Item $log -Force }
$proc = Start-Process -FilePath $ExePath -ArgumentList @("D:/OpenScope/tests/linix_stm32l031_v1.2.out","--no-layout") -PassThru
$main=[IntPtr]::Zero
for($i=0;$i -lt 50 -and -not $proc.HasExited;$i++){ $main=Find-ByClass $proc.Id "OpenScopeMain"; if($main -ne [IntPtr]::Zero){break}; Start-Sleep -Milliseconds 200 }
if($main -eq [IntPtr]::Zero){Write-Output "FAIL main"; if(-not $proc.HasExited){$proc.Kill()}; exit 1}
Start-Sleep -Milliseconds 1200
[COUi]::SendMessage($main,0x111,[IntPtr]2002,[IntPtr]0)|Out-Null  # CONNECT
Start-Sleep -Milliseconds 4000
Write-Output "exited=$($proc.HasExited) code=$(if($proc.HasExited){$proc.ExitCode}else{'N/A'})"
if($proc.HasExited){ Write-Output "CRASH"; Get-Content $log -Encoding UTF8 | Select-Object -Last 12 } else { Write-Output "OK" }
if(-not $proc.HasExited){$proc.Kill()}
