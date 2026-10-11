param([string]$Out = "D:\OpenScope\JScope_REVERSE\lab\screen.png")
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class F {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public static uint Pid; public static System.Collections.Generic.List<IntPtr> W = new System.Collections.Generic.List<IntPtr>();
  public static bool Cb(IntPtr h, IntPtr p) { uint pid; GetWindowThreadProcessId(h, out pid); if (pid == Pid && IsWindowVisible(h)) W.Add(h); return true; }
  public static string Title(IntPtr h) { var t = new StringBuilder(512); GetWindowTextW(h, t, 512); return t.ToString(); }
}
"@
$p = Get-Process JScope | Select-Object -First 1
[F]::Pid = $p.Id
[F]::W.Clear(); [void][F]::EnumWindows([F+EnumProc]{ param($a,$b) [F]::Cb($a,$b) }, [IntPtr]::Zero)
foreach ($h in [F]::W) { Write-Host ("win 0x{0:x} '{1}'" -f [int64]$h, [F]::Title($h)) }
# 取最后一个可见窗口（对话框通常最后创建），拉到前台
if ([F]::W.Count -gt 0) { $top = [F]::W[[F]::W.Count-1]; [void][F]::BringWindowToTop($top); [void][F]::SetForegroundWindow($top) }
Start-Sleep -Milliseconds 700
$b = New-Object System.Drawing.Bitmap -ArgumentList @(1500, 850)
$g = [System.Drawing.Graphics]::FromImage($b)
$g.CopyFromScreen(0, 0, 0, 0, (New-Object System.Drawing.Size -ArgumentList @(1500, 850)))
$b.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $b.Dispose()
Write-Host "saved $Out"
