param([int]$Pid_ = 0, [string]$Keys = "", [string]$Out = "", [switch]$Enter, [switch]$Esc)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;
public class G {
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x,int y,int cx,int cy,uint f);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left,Top,Right,Bottom; }
}
"@
if ($Pid_ -eq 0) { $p = Get-Process JScope -ErrorAction Stop | Select-Object -First 1 } else { $p = Get-Process -Id $Pid_ }
$h = $p.MainWindowHandle
[void][G]::SetForegroundWindow($h)
Start-Sleep -Milliseconds 400
function K([int]$vk,[int]$times=1) { for($i=0;$i -lt $times;$i++){ [void][G]::keybd_event($vk,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 60; [void][G]::keybd_event($vk,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 120 } }
if ($Keys -ne "") {
  # 形如 "13" 或 "17,83"（多键同时按，逗号分隔）
  $vk = $Keys.Split(',') | ForEach-Object { [int]$_ }
  foreach ($k in $vk) { [void][G]::keybd_event($k,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 40 }
  foreach ($k in $vk) { [void][G]::keybd_event($k,0,2,[IntPtr]::Zero) }
  Start-Sleep -Milliseconds 300
}
if ($Enter) { K 13; }
if ($Esc) { K 27; }
if ($Out -ne "") {
  $r = New-Object G+RECT
  [void][G]::GetWindowRect($h, [ref]$r)
  $w = $r.Right-$r.Left; $ht = $r.Bottom-$r.Top
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w,$ht)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen($r.Left,$r.Top,0,0,(New-Object System.Drawing.Size -ArgumentList @($w,$ht)))
  $bmp.Save($Out,[System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose()
  Write-Host "saved $Out (${w}x${ht})"
}
