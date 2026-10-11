param([Parameter(Mandatory=$true)][string]$Text, [int]$X = -1, [int]$Y = -1, [switch]$Clear)
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class TY {
    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, uint dx, uint dy, uint d, IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  [DllImport("user32.dll")] public static extern short VkKeyScanW(char c);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  public static string FT() { var t = new StringBuilder(256); GetWindowTextW(GetForegroundWindow(), t, 256); return t.ToString(); }
}
"@
[void][TY]::SetProcessDPIAware()
if ($X -ge 0 -and $Y -ge 0) {
  [void][TY]::SetCursorPos($X, $Y); Start-Sleep -Milliseconds 250
  [TY]::mouse_event(0x0002,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 80
  [TY]::mouse_event(0x0004,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 300
}
Write-Host ("  [typer] fg = '" + [TY]::FT() + "'")
if ($Clear) {
  [TY]::keybd_event(0x11,0,0,[IntPtr]::Zero)   # Ctrl down
  [TY]::keybd_event(0x41,0,0,[IntPtr]::Zero)   # A
  [TY]::keybd_event(0x41,0,2,[IntPtr]::Zero)
  [TY]::keybd_event(0x11,0,2,[IntPtr]::Zero)
  Start-Sleep -Milliseconds 150
  [TY]::keybd_event(0x2E,0,0,[IntPtr]::Zero)   # DEL
  [TY]::keybd_event(0x2E,0,2,[IntPtr]::Zero)
  Start-Sleep -Milliseconds 150
}
foreach ($ch in $Text.ToCharArray()) {
  $vk = [TY]::VkKeyScanW($ch)
  if ($vk -eq -1) { continue }
  $code = $vk -band 0xFF
  $shift = ($vk -shr 8) -band 0xFF
  $needShift = ($shift -band 1) -ne 0
  $needCtrl  = ($shift -band 2) -ne 0
  if ($needShift) { [TY]::keybd_event(0x10,0,0,[IntPtr]::Zero) }
  if ($needCtrl)  { [TY]::keybd_event(0x11,0,0,[IntPtr]::Zero) }
  [TY]::keybd_event([byte]$code,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 25
  [TY]::keybd_event([byte]$code,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 25
  if ($needCtrl)  { [TY]::keybd_event(0x11,0,2,[IntPtr]::Zero) }
  if ($needShift) { [TY]::keybd_event(0x10,0,2,[IntPtr]::Zero) }
}
Write-Host ("  [typer] typed " + $Text.Length + " chars")
