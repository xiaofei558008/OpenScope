param(
  [Parameter(Mandatory=$true)][string]$WinTitle,
  [int]$X = 0, [int]$Y = 0, [int]$W = 0, [int]$H = 0
)
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class PL {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x, int y, int cx, int cy, uint f);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
  public static string Want = "";
  public static IntPtr Found = IntPtr.Zero;
  public static bool Cb(IntPtr h, IntPtr p) {
    var t = new StringBuilder(256); GetWindowTextW(h, t, 256);
    if (t.ToString() == Want) { Found = h; return false; }
    return true;
  }
}
"@
[PL]::Want = $WinTitle
[PL]::Found = [IntPtr]::Zero
[void][PL]::EnumWindows([PL+EnumProc]{ param($a,$b) [PL]::Cb($a,$b) }, [IntPtr]::Zero)
if ([PL]::Found -eq [IntPtr]::Zero) { Write-Host "window not found: $WinTitle"; exit 1 }
$r = New-Object PL+RECT
[void][PL]::GetWindowRect([PL]::Found, [ref]$r)
Write-Host ("before: ({0},{1}) {2}x{3}" -f $r.Left, $r.Top, ($r.Right - $r.Left), ($r.Bottom - $r.Top))
if ($W -eq 0) { $W = $r.Right - $r.Left }
if ($H -eq 0) { $H = $r.Bottom - $r.Top }
[void][PL]::SetWindowPos([PL]::Found, [IntPtr]::Zero, $X, $Y, $W, $H, 0x0040)
[void][PL]::BringWindowToTop([PL]::Found)
[void][PL]::SetForegroundWindow([PL]::Found)
Start-Sleep -Milliseconds 600
[void][PL]::GetWindowRect([PL]::Found, [ref]$r)
Write-Host ("after:  ({0},{1}) {2}x{3}" -f $r.Left, $r.Top, ($r.Right - $r.Left), ($r.Bottom - $r.Top))
