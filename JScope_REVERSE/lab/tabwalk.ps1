Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class TW {
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  [DllImport("user32.dll")] public static extern bool EnumWindows(P cb, IntPtr p);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out R r);
  public delegate bool P(IntPtr h, IntPtr p);
  public struct R { public int L; public int T; public int Rr; public int B; }
  public static string Want = "Create new project";
  public static IntPtr Found = IntPtr.Zero;
  public static bool Cb(IntPtr h, IntPtr p) { var t = new StringBuilder(128); GetWindowTextW(h, t, 128); if (IsWindowVisible(h) && t.ToString() == Want) { Found = h; return false; } return true; }
}
"@
function Key([int]$vk) {
  [TW]::keybd_event([byte]$vk, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 50
  [TW]::keybd_event([byte]$vk, 0, 2, [IntPtr]::Zero); Start-Sleep -Milliseconds 120
}
[TW]::Found = [IntPtr]::Zero
[void][TW]::EnumWindows([TW+P]{ param($a,$b) [TW]::Cb($a,$b) }, [IntPtr]::Zero)
$h = [TW]::Found
if ($h -eq [IntPtr]::Zero) { Write-Host "dialog not found"; exit 1 }
Write-Host ("dialog hwnd=0x{0:x}" -f [int64]$h)
[void][TW]::SetForegroundWindow($h)
Start-Sleep -Milliseconds 400
$r = New-Object TW+R
for ($i = 1; $i -le 8; $i++) {
  Key 9          # TAB
  Key 0x37       # '7'
  Start-Sleep -Milliseconds 200
  [void][TW]::GetWindowRect($h, [ref]$r)
  $w = $r.Rr - $r.L; $ht = $r.B - $r.T
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w, $ht)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen($r.L, $r.T, 0, 0, (New-Object System.Drawing.Size -ArgumentList @($w, $ht)))
  $bmp.Save("D:\OpenScope\JScope_REVERSE\lab\tab$i.png"); $g.Dispose(); $bmp.Dispose()
  Write-Host "  tab$i captured"
}
