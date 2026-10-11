param([string]$Prj = "D:\OpenScope\JScope_REVERSE\lab\v_ok.jscope", [int]$Wait = 9)
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class N {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public static uint Pid; public static System.Collections.Generic.List<string> T = new System.Collections.Generic.List<string>();
  public static bool Cb(IntPtr h, IntPtr p) { uint pid; GetWindowThreadProcessId(h, out pid); if (pid == Pid && IsWindowVisible(h)) { var t = new StringBuilder(256); GetWindowTextW(h, t, 256); if (t.Length > 0) T.Add(t.ToString()); } return true; }
}
"@
Get-Process JScope -ErrorAction SilentlyContinue | ForEach-Object { $_.Kill() }
Start-Sleep -Seconds 2
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", $Prj) -PassThru
Start-Sleep -Seconds $Wait
$proc.Refresh()
$state = "alive"
if ($proc.HasExited) { $state = "EXITED(" + $proc.ExitCode + ")" }
[N]::Pid = $proc.Id; [N]::T.Clear()
[void][N]::EnumWindows([N+EnumProc]{ param($a,$b) [N]::Cb($a,$b) }, [IntPtr]::Zero)
$titles = ($N::T | Where-Object { $_ -notlike "*IME*" }) -join " | "
Write-Host ("PROBE {0}  state={1}" -f (Split-Path $Prj -Leaf), $state)
Write-Host ("      windows: {0}" -f $titles)
if (-not $proc.HasExited) { $proc.Kill() }
