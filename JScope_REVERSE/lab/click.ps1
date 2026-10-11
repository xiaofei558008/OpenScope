param([Parameter(Mandatory=$true)][int]$X, [Parameter(Mandatory=$true)][int]$Y, [int]$Double = 0, [string]$Out = "")
Add-Type @"
using System;using System.Runtime.InteropServices;
public class C {
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x,int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f,uint dx,uint dy,uint d,IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk,byte sc,uint f,IntPtr e);
}
"@
if ($Out -ne "") {
  Add-Type -AssemblyName System.Drawing
  [void][C]::keybd_event(0x1B,0,0,[IntPtr]::Zero); [void][C]::keybd_event(0x1B,0,2,[IntPtr]::Zero)
  Start-Sleep -Milliseconds 500
}
[void][C]::SetCursorPos($X, $Y)
Start-Sleep -Milliseconds 250
for ($i=0; $i -lt 1 + $Double; $i++) {
  [C]::mouse_event(0x0002,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 60
  [C]::mouse_event(0x0004,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 250
}
Write-Host "clicked $X,$Y"
