Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class ST {
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool EnumWindows(P cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out R r);
  public delegate bool P(IntPtr h, IntPtr p);
  public struct R { public int L; public int T; public int Rr; public int B; }
  public static string O = "";
  public static bool Cb(IntPtr h, IntPtr p) {
    uint pid; GetWindowThreadProcessId(h, out pid);
    var t = new StringBuilder(128); GetWindowTextW(h, t, 128);
    if (t.Length > 0 && IsWindowVisible(h) && t.ToString() != "MSCTFIME UI" && t.ToString() != "Default IME") {
      R r; GetWindowRect(h, out r);
      O += string.Format("  hwnd=0x{0:x} pid={1} '{2}' ({3},{4}) {5}x{6}\n", (long)h, pid, t.ToString(), r.L, r.T, r.Rr-r.L, r.B-r.T);
    }
    return true;
  }
  public static string TT(IntPtr h) { var t = new StringBuilder(128); GetWindowTextW(h, t, 128); return t.ToString(); }
}
"@
[void][ST]::SetProcessDPIAware()
Write-Host "=== visible windows (physical coords) ==="
Write-Host ([ST]::O)
[ST]::O = ""
[void][ST]::EnumWindows([ST+P]{ param($a,$b) [ST]::Cb($a,$b) }, [IntPtr]::Zero)
Write-Host ([ST]::O)
$fg = [ST]::GetForegroundWindow()
Write-Host ("foreground = 0x{0:x} '{1}'" -f [int64]$fg, [ST]::TT($fg))
