Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class CB {
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern int GetSystemMetrics(int i);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x,int y);
  [DllImport("user32.dll")] public static extern IntPtr WindowFromPoint(POINT p);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out R r);
  [DllImport("user32.dll")] public static extern bool EnumWindows(P cb, IntPtr p);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  public delegate bool P(IntPtr h, IntPtr p);
  public struct POINT { public int X; public int Y; }
  public struct R { public int L; public int T; public int Rr; public int B; }
  public static string Want=""; public static IntPtr Found=IntPtr.Zero;
  public static bool Cb(IntPtr h, IntPtr p) { var t=new StringBuilder(128); GetWindowTextW(h,t,128); if (IsWindowVisible(h) && t.ToString()==Want) { Found=h; return false; } return true; }
  public static string TT(IntPtr h) { var t = new StringBuilder(128); GetWindowTextW(h, t, 128); return t.ToString(); }
}
"@
[void][CB]::SetProcessDPIAware()
Add-Type -AssemblyName System.Drawing
Write-Host ("screen metrics: {0}x{1}" -f [CB]::GetSystemMetrics(0), [CB]::GetSystemMetrics(1))
[CB]::Want = "Create new project"; [CB]::Found = [IntPtr]::Zero
[void][CB]::EnumWindows([CB+P]{ param($a,$b) [CB]::Cb($a,$b) }, [IntPtr]::Zero)
$d = [CB]::Found
$r = New-Object CB+R; [void][CB]::GetWindowRect($d, [ref]$r)
Write-Host ("dialog rect = ({0},{1}) {2}x{3}" -f $r.L, $r.T, ($r.Rr-$r.L), ($r.B-$r.T))
$bmp = New-Object System.Drawing.Bitmap -ArgumentList @(2000, 1200)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen(0, 0, 0, 0, (New-Object System.Drawing.Size -ArgumentList @(2000, 1200)))
$bmp.Save("D:\OpenScope\JScope_REVERSE\lab\dpiaware.png"); $g.Dispose(); $bmp.Dispose()
Write-Host "saved dpiaware.png (2000x1200)"
# 命中测试
foreach ($p in @(@(200,179), @(300,220), @(230,179))) {
  [void][CB]::SetCursorPos($p[0],$p[1]); Start-Sleep -Milliseconds 120
  $pt = New-Object CB+POINT; $pt.X=$p[0]; $pt.Y=$p[1]
  $h = [CB]::WindowFromPoint($pt)
  Write-Host ("pt({0},{1}) -> '{2}'" -f $p[0], $p[1], [CB]::TT($h))
}
