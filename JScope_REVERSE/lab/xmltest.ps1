$ErrorActionPreference = "Continue"
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class X {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassNameW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public static uint Pid;
  public static System.Collections.Generic.List<string> T = new System.Collections.Generic.List<string>();
  public static bool Cb(IntPtr h, IntPtr p) {
    uint pid; GetWindowThreadProcessId(h, out pid);
    if (pid != Pid) return true;
    var t = new StringBuilder(256); GetWindowTextW(h, t, 256);
    var c = new StringBuilder(128); GetClassNameW(h, c, 128);
    T.Add("[vis=" + IsWindowVisible(h) + " cls=" + c.ToString() + "] " + t.ToString());
    return true;
  }
}
"@
$files = @("t1_scope_ok.jscope","t2_scope_notr.jscope","t3_scope_sym.jscope","t4_notime.jscope","hss_test.jscope")
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
foreach ($f in $files) {
  Get-Process JScope -ErrorAction SilentlyContinue | ForEach-Object { try { $_.Kill() } catch {} }
  Start-Sleep -Seconds 2
  $proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", "D:\OpenScope\JScope_REVERSE\lab\$f") -PassThru
  Start-Sleep -Seconds 10
  $proc.Refresh()
  Write-Host ("===== {0}  pid={1} exited={2}" -f $f, $proc.Id, $proc.HasExited)
  if ($proc.HasExited) { Write-Host ("      exitcode={0}" -f $proc.ExitCode); continue }
  [X]::Pid = [uint32]$proc.Id
  [X]::T.Clear()
  [void][X]::EnumWindows([X+EnumProc]{ param($a,$b) [X]::Cb($a,$b) }, [IntPtr]::Zero)
  foreach ($w in [X]::T) { Write-Host ("      " + $w) }
  try { $proc.Kill() } catch {}
}
