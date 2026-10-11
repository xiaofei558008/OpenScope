Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class WH {
  [DllImport("user32.dll")] public static extern bool EnumWindows(P cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassNameW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool IsWindowEnabled(IntPtr h);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out R r);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr WindowFromPoint(POINT p);
  [DllImport("user32.dll")] public static extern bool GetCursorPos(out POINT p);
  public delegate bool P(IntPtr h, IntPtr p);
  public struct R { public int L; public int T; public int Rr; public int B; }
  public struct POINT { public int X; public int Y; }
  public static string O = "";
  public static bool Cb(IntPtr h, IntPtr p) {
    uint pid; GetWindowThreadProcessId(h, out pid);
    var t = new StringBuilder(128); GetWindowTextW(h, t, 128);
    var c = new StringBuilder(64); GetClassNameW(h, c, 64);
    if (t.Length > 0) {
      R r; GetWindowRect(h, out r);
      O += string.Format("hwnd=0x{0:x} pid={1} vis={2} en={3} cls={4} '{5}' ({6},{7}) {8}x{9}\n",
        (long)h, pid, IsWindowVisible(h), IsWindowEnabled(h), c.ToString(), t.ToString(), r.L, r.T, r.Rr-r.L, r.B-r.T);
    }
    return true;
  }
}
"@
Write-Host "=== processes ==="
Get-Process JScope -ErrorAction SilentlyContinue | ForEach-Object { Write-Host ("pid={0} title='{1}'" -f $_.Id, $_.MainWindowTitle) }
Write-Host "=== windows ==="
Write-Host ([WH]::O)
[WH]::O = ""
[void][WH]::EnumWindows([WH+P]{ param($a,$b) [WH]::Cb($a,$b) }, [IntPtr]::Zero)
Write-Host ([WH]::O)
$fg = [WH]::GetForegroundWindow()
Write-Host ("foreground = 0x{0:x}" -f [int64]$fg)
$pt = New-Object WH+POINT; [void][WH]::GetCursorPos([ref]$pt)
Write-Host ("cursor = ({0},{1})" -f $pt.X, $pt.Y)
$h2 = [WH]::WindowFromPoint($pt)
Write-Host ("window under cursor = 0x{0:x}" -f [int64]$h2)
