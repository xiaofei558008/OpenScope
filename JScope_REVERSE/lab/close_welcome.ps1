Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class CW {
  [DllImport("user32.dll")] public static extern bool EnumWindows(P cb, IntPtr p);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  public delegate bool P(IntPtr h, IntPtr p);
  public static string Want = "";
  public static IntPtr Found = IntPtr.Zero;
  public static bool Cb(IntPtr h, IntPtr p) { var t = new StringBuilder(128); GetWindowTextW(h, t, 128); if (IsWindowVisible(h) && t.ToString() == Want) { Found = h; return false; } return true; }
}
"@
function Find([string]$title) {
  [CW]::Want = $title; [CW]::Found = [IntPtr]::Zero
  [void][CW]::EnumWindows([CW+P]{ param($a,$b) [CW]::Cb($a,$b) }, [IntPtr]::Zero)
  return [CW]::Found
}
$w = Find "Welcome to J-Scope"
if ($w -ne [IntPtr]::Zero) {
  Write-Host "closing Welcome dialog"
  [void][CW]::BringWindowToTop($w); [void][CW]::SetForegroundWindow($w)
  Start-Sleep -Milliseconds 300
  # Qt 对话框：ESC 会触发 reject（等同 Cancel/关闭）
  [CW]::keybd_event(0x1B,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 60
  [CW]::keybd_event(0x1B,0,2,[IntPtr]::Zero)
  Start-Sleep -Milliseconds 800
  $w2 = Find "Welcome to J-Scope"
  if ($w2 -ne [IntPtr]::Zero) { Write-Host "ESC 无效，改点右上角关闭"; [void][CW]::PostMessage($w2, 0x10, [IntPtr]::Zero, [IntPtr]::Zero) }
  Start-Sleep -Milliseconds 800
} else { Write-Host "no welcome dialog" }
$c = Find "Create new project"
Write-Host ("config dialog = 0x{0:x}" -f [int64]$c)
