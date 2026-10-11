﻿param([string]$Title = "Create new project", [int]$X = 0, [int]$Y = 0, [int]$W = 0, [int]$H = 0)
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class M {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x,int y,int cx,int cy,uint f);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
  public static string Want; public static IntPtr Found = IntPtr.Zero;
  public static bool Cb(IntPtr h, IntPtr p) { var t = new StringBuilder(256); GetWindowTextW(h, t, 256); if (t.ToString() -eq Want) { Found = h; return false; } return true; }
}
"@
[M]::Want = $Title
[M]::Found = [IntPtr]::Zero
[void][M]::EnumWindows([M+EnumProc]{ param($a,$b) [M]::Cb($a,$b) }, [IntPtr]::Zero)
if ([M]::Found -eq [IntPtr]::Zero) { Write-Host "window '$Title' not found"; exit 1 }
$r = New-Object M+RECT; [void][M]::GetWindowRect([M]::Found, [ref]$r)
Write-Host ("before: {0},{1} {2}x{3}" -f $r.Left,$r.Top,($r.Right-$r.Left),($r.Bottom-$r.Top))
if ($W -eq 0) { $W = $r.Right-$r.Left }
if ($H -eq 0) { $H = $r.Bottom-$r.Top }
[void][M]::SetWindowPos([M]::Found, [IntPtr]::Zero, $X, $Y, $W, $H, 0x0040)
[void][M]::BringWindowToTop([M]::Found)
Start-Sleep -Milliseconds 400
$r2 = New-Object M+RECT; [void][M]::GetWindowRect([M]::Found, [ref]$r2)
Write-Host ("after:  {0},{1} {2}x{3}" -f $r2.Left,$r2.Top,($r2.Right-$r2.Left),($r2.Bottom-$r2.Top))
