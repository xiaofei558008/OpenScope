Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class FC {
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, uint dx, uint dy, uint d, IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  [DllImport("user32.dll")] public static extern short VkKeyScanW(char c);
  [DllImport("user32.dll")] public static extern bool EnumWindows(P cb, IntPtr p);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x, int y, int cx, int cy, uint fl);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out R r);
  public delegate bool P(IntPtr h, IntPtr p);
  public struct R { public int L; public int T; public int Rr; public int B; }
  public static string Want = ""; public static IntPtr Found = IntPtr.Zero;
  public static bool Cb(IntPtr h, IntPtr p) { var t = new StringBuilder(128); GetWindowTextW(h, t, 128); if (IsWindowVisible(h) && t.ToString() == Want) { Found = h; return false; } return true; }
}
"@
[void][FC]::SetProcessDPIAware()

function FindWin([string]$t) {
  [FC]::Want = $t; [FC]::Found = [IntPtr]::Zero
  [void][FC]::EnumWindows([FC+P]{ param($a,$b) [FC]::Cb($a,$b) }, [IntPtr]::Zero)
  return [FC]::Found
}
function Click([int]$x, [int]$y) {
  [void][FC]::SetCursorPos($x, $y); Start-Sleep -Milliseconds 250
  [FC]::mouse_event(0x0002,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 80
  [FC]::mouse_event(0x0004,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 350
}
function TypeText([string]$s) {
  foreach ($ch in $s.ToCharArray()) {
    $vk = [FC]::VkKeyScanW($ch)
    if ($vk -eq -1) { continue }
    $code = $vk -band 0xFF; $sh = ($vk -shr 8) -band 0xFF
    $needShift = ($sh -band 1) -ne 0; $needCtrl = ($sh -band 2) -ne 0
    if ($needShift) { [FC]::keybd_event(0x10,0,0,[IntPtr]::Zero) }
    if ($needCtrl)  { [FC]::keybd_event(0x11,0,0,[IntPtr]::Zero) }
    [FC]::keybd_event([byte]$code,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 25
    [FC]::keybd_event([byte]$code,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 25
    if ($needCtrl)  { [FC]::keybd_event(0x11,0,2,[IntPtr]::Zero) }
    if ($needShift) { [FC]::keybd_event(0x10,0,2,[IntPtr]::Zero) }
  }
}
function Snap([string]$path, [int]$w, [int]$h) {
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w, $h)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen(0, 0, 0, 0, (New-Object System.Drawing.Size -ArgumentList @($w, $h)))
  $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose()
}
$dlg = FindWin "Create new project"
if ($dlg -eq [IntPtr]::Zero) { Write-Host "config dialog NOT FOUND"; exit 1 }
$r = New-Object FC+R; [void][FC]::GetWindowRect($dlg, [ref]$r)
Write-Host ("dialog at ({0},{1}) {2}x{3}" -f $r.L, $r.T, ($r.Rr-$r.L), ($r.B-$r.T))
[void][FC]::SetWindowPos($dlg, [IntPtr]::Zero, 0, 0, 0, 0, 0x0041)   # 移到 (0,0)，保持尺寸
[void][FC]::BringWindowToTop($dlg); [void][FC]::SetForegroundWindow($dlg)
Start-Sleep -Milliseconds 500
[void][FC]::GetWindowRect($dlg, [ref]$r)
Write-Host ("moved to ({0},{1}) {2}x{3}" -f $r.L, $r.T, ($r.Rr-$r.L), ($r.B-$r.T))
$x0 = $r.L; $y0 = $r.T

# 1) 器件名
Click ($x0 + 240) ($y0 + 178)
TypeText "STM32F303RE"
Start-Sleep -Milliseconds 300
# 2) 接口 → SWD（点开下拉，选 SWD）
Click ($x0 + 90) ($y0 + 318)
Start-Sleep -Milliseconds 500
Snap "D:\OpenScope\JScope_REVERSE\lab\combo.png" 700 600
Write-Host "combo opened, screenshot saved"
# 3) ELF 路径
Click ($x0 + 240) ($y0 + 570)
TypeText "D:\C_Code\aa1_-pmsm_ctrl\EWARM\antman\Exe\antman.out"
Start-Sleep -Milliseconds 300
Snap "D:\OpenScope\JScope_REVERSE\lab\filled_final.png" 760 760
Write-Host "done"
