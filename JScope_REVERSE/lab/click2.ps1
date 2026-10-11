param([Parameter(Mandatory=$true)][int]$X,[Parameter(Mandatory=$true)][int]$Y,[int]$N=1)
Add-Type @"
using System;using System.Runtime.InteropServices;
public class CK {
    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x,int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f,uint dx,uint dy,uint d,IntPtr e);
}
"@
[void][CK]::SetProcessDPIAware()
[void][CK]::SetCursorPos($X,$Y); Start-Sleep -Milliseconds 250
for ($i=0;$i -lt $N;$i++) {
  [CK]::mouse_event(0x0002,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 60
  [CK]::mouse_event(0x0004,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 200
}
Write-Host "clicked ($X,$Y) x$N"
