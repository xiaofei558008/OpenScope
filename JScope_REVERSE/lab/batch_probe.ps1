﻿param([string[]]$Files = @("v_minimal.jscope","v_nodecl.jscope","v_oneline.jscope"))
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class B {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public static uint Pid; public static System.Collections.Generic.List<string> T = new System.Collections.Generic.List<string>();
  public static bool Cb(IntPtr h, IntPtr p) { uint pid; GetWindowThreadProcessId(h, out pid); if (pid == Pid && IsWindowVisible(h)) { var t = new StringBuilder(256); GetWindowTextW(h, t, 256); if (t.Length>0) T.Add(t.ToString()); } return true; }
}
"@
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
foreach ($f in $Files) {
  $path = "D:\OpenScope\JScope_REVERSE\lab\$f"
  $proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", $path) -PassThru
  Start-Sleep -Seconds 7
  [B]::Pid = $proc.Id; [B]::T.Clear()
  [void][B]::EnumWindows([B+EnumProc]{ param($a,$b) [B]::Cb($a,$b) }, [IntPtr]::Zero)
  $warn = ($B::T | Where-Object { $_ -like "*Warning*" -or $_ -like "*Error*" }).Count
  $proc.Refresh()
  $state = "alive"; if ($proc.HasExited) { $state = "EXITED(" + $proc.ExitCode + ")" }
  if ($warn -gt 0) { Write-Host ("{0,-22} FAIL  {1}  [{2}]" -f $f, $state, ($B::T -join " | ")) }
  else { Write-Host ("{0,-22} PASS  {1}  [{2}]" -f $f, $state, ($B::T -join " | ")) }
  $proc.Refresh(); if (-not $proc.HasExited) { $proc.Kill() }
  Start-Sleep -Seconds 2
}
