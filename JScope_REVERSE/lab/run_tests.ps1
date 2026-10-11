$files = @("t1_scope_ok.jscope","t2_scope_notr.jscope","t3_scope_sym.jscope","t4_notime.jscope")
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class R {
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
foreach ($f in $files) {
  Get-Process JScope -ErrorAction SilentlyContinue | ForEach-Object { $_.Kill() }
  Start-Sleep -Seconds 2
  $proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", "D:\OpenScope\JScope_REVERSE\lab\$f") -PassThru
  Start-Sleep -Seconds 10
  $proc.Refresh()
  if ($proc.HasExited) { Write-Host ("{0,-24} EXITED({1})" -f $f, $proc.ExitCode); continue }
  [R]::Pid = $proc.Id; [R]::T.Clear()
  [void][R]::EnumWindows([R+EnumProc]{ param($a,$b) [R]::Cb($a,$b) }, [IntPtr]::Zero)
  $titles = @($R::T | Where-Object { $_ -notlike "*IME*" })
  $box = @($titles | Where-Object { $_ -like "*Warning*" -or $_ -like "*Error*" }).Count
  $verdict = "LOAD-OK"
  if ($box -gt 0) { $verdict = "FAIL" }
  Write-Host ("{0,-24} {1}   [{2}]" -f $f, $verdict, ($titles -join " | "))
  $proc.Kill()
}
