param([Parameter(Mandatory=$true)][int]$Pid_, [string]$Out = "D:\OpenScope\JScope_REVERSE\lab\shot.png")
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class Cap {
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
}
"@
$p = Get-Process -Id $Pid_ -ErrorAction Stop
$h = $p.MainWindowHandle
Write-Host "pid=$Pid_ hwnd=0x$('{0:x}' -f [int64]$h) title='$($p.MainWindowTitle)'"
if ($h -eq 0) { Write-Host "no main window"; exit 1 }
$r = New-Object Cap+RECT
$ok = [Cap]::GetWindowRect($h, [ref]$r)
$w = $r.Right - $r.Left
$ht = $r.Bottom - $r.Top
Write-Host "GetWindowRect=$ok rect=$($r.Left),$($r.Top),$($r.Right),$($r.Bottom) size=${w}x${ht}"
if ($w -le 0 -or $ht -le 0) { Write-Host "bad rect"; exit 1 }
$bmp = New-Object System.Drawing.Bitmap -ArgumentList $w, $ht
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size -ArgumentList $w, $ht))
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose(); $bmp.Dispose()
Write-Host "saved $Out"
