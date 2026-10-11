Add-Type @"
using System;using System.Runtime.InteropServices;
public class PP {
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x,int y);
  [DllImport("user32.dll")] public static extern bool GetCursorPos(out POINT p);
  [DllImport("user32.dll")] public static extern IntPtr WindowFromPoint(POINT p);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, System.Text.StringBuilder s, int n);
  public struct POINT { public int X; public int Y; }
  public static string TT(IntPtr h) { var t = new System.Text.StringBuilder(128); GetWindowTextW(h, t, 128); return t.ToString(); }
}
"@
foreach ($p in @(@(100,100), @(200,179), @(230,179), @(300,300), @(452,179))) {
  [void][PP]::SetCursorPos($p[0], $p[1]); Start-Sleep -Milliseconds 150
  $pt = New-Object PP+POINT; [void][PP]::GetCursorPos([ref]$pt)
  $h = [PP]::WindowFromPoint($pt)
  Write-Host ("set({0},{1}) -> get({2},{3}) hwnd=0x{4:x} '{5}'" -f $p[0], $p[1], $pt.X, $pt.Y, [int64]$h, [PP]::TT($h))
}
