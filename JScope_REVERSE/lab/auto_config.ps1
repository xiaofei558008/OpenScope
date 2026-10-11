param([string]$ElfPath = "D:\C_Code\aa1_-pmsm_ctrl\EWARM\antman\Exe\antman.out",
      [string]$SavePath = "D:\OpenScope\JScope_REVERSE\lab\reference.jscope",
      [int]$SampleEvery = 1000)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class AC {
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
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  public delegate bool P(IntPtr h, IntPtr p);
  public struct R { public int L; public int T; public int Rr; public int B; }
  public static string Want = ""; public static IntPtr Found = IntPtr.Zero;
  public static System.Collections.Generic.List<string> All = new System.Collections.Generic.List<string>();
  public static bool Cb(IntPtr h, IntPtr p) {
    var t = new StringBuilder(160); GetWindowTextW(h, t, 160);
    if (IsWindowVisible(h) && t.Length > 0) All.Add(t.ToString());
    if (Want != "" && IsWindowVisible(h) && t.ToString() == Want) { Found = h; return false; }
    return true;
  }
}
"@
[void][AC]::SetProcessDPIAware()
$EXE = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$OUT = "D:\OpenScope\JScope_REVERSE\lab"

function FindWin([string]$t) {
  [AC]::Want = $t; [AC]::Found = [IntPtr]::Zero
  [void][AC]::EnumWindows([AC+P]{ param($a,$b) [AC]::Cb($a,$b) }, [IntPtr]::Zero)
  return [AC]::Found
}
function ListWins() {
  [AC]::Want = ""; [AC]::All.Clear()
  [void][AC]::EnumWindows([AC+P]{ param($a,$b) [AC]::Cb($a,$b) }, [IntPtr]::Zero)
  return ([AC]::All | Where-Object { $_ -notlike "*IME*" }) -join " | "
}
function Click([int]$x, [int]$y) {
  [void][AC]::SetCursorPos($x, $y); Start-Sleep -Milliseconds 250
  [AC]::mouse_event(0x0002,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 80
  [AC]::mouse_event(0x0004,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 350
}
function Key([int]$vk, [int]$n = 1) { for ($i=0; $i -lt $n; $i++) { [AC]::keybd_event([byte]$vk,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 45; [AC]::keybd_event([byte]$vk,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 90 } }
function TypeText([string]$s) {
  foreach ($ch in $s.ToCharArray()) {
    $vk = [AC]::VkKeyScanW($ch); if ($vk -eq -1) { continue }
    $code = $vk -band 0xFF; $sh = ($vk -shr 8) -band 0xFF
    $ns = ($sh -band 1) -ne 0; $nc = ($sh -band 2) -ne 0
    if ($ns) { [AC]::keybd_event(0x10,0,0,[IntPtr]::Zero) }
    if ($nc) { [AC]::keybd_event(0x11,0,0,[IntPtr]::Zero) }
    [AC]::keybd_event([byte]$code,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 25
    [AC]::keybd_event([byte]$code,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 25
    if ($nc) { [AC]::keybd_event(0x11,0,2,[IntPtr]::Zero) }
    if ($ns) { [AC]::keybd_event(0x10,0,2,[IntPtr]::Zero) }
  }
  Start-Sleep -Milliseconds 300
}
function Snap([string]$name, [int]$w = 900, [int]$h = 780) {
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w, $h)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen(0, 0, 0, 0, (New-Object System.Drawing.Size -ArgumentList @($w, $h)))
  $bmp.Save("$OUT\$name.png", [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose()
  Write-Host "  [snap $name]"
}

Get-Process JScope -ErrorAction SilentlyContinue | ForEach-Object { try { $_.Kill() } catch {} }
Start-Sleep -Seconds 2
Start-Process -FilePath $EXE | Out-Null
Start-Sleep -Seconds 8
Write-Host ("[1] windows: " + (ListWins))

$w = FindWin "Welcome to J-Scope"
if ($w -ne [IntPtr]::Zero) {
  [void][AC]::BringWindowToTop($w); [void][AC]::SetForegroundWindow($w); Start-Sleep -Milliseconds 400
  Key 0x28      # DOWN → Create new project
  Key 13        # ENTER → Start
  Start-Sleep -Seconds 3
}
Write-Host ("[2] windows: " + (ListWins))

$d = FindWin "Create new project"
if ($d -eq [IntPtr]::Zero) { Write-Host "!! config dialog not found"; Snap "fail_nodialog"; exit 1 }
[void][AC]::SetWindowPos($d, [IntPtr]::Zero, 0, 0, 0, 0, 0x0041)
[void][AC]::BringWindowToTop($d); [void][AC]::SetForegroundWindow($d); Start-Sleep -Milliseconds 500
$r = New-Object AC+R; [void][AC]::GetWindowRect($d, [ref]$r)
$x0 = $r.L; $y0 = $r.T
Write-Host ("[3] dialog at ({0},{1}) {2}x{3}" -f $x0, $y0, ($r.Rr-$r.L), ($r.B-$r.T))

Click ($x0 + 240) ($y0 + 178); TypeText "STM32F303RE"
Click ($x0 + 90) ($y0 + 318); Key 0x53; Key 13            # 接口下拉 → 'S' → SWD → Enter
Click ($x0 + 200) ($y0 + 475)
[AC]::keybd_event(0x11,0,0,[IntPtr]::Zero); Key 0x41; [AC]::keybd_event(0x11,0,2,[IntPtr]::Zero)
TypeText "$SampleEvery"
Click ($x0 + 240) ($y0 + 570); TypeText $ElfPath
Snap "cfg_filled"
Write-Host ("[4] focus = '" + (FindWin "Create new project") + "'")
Click ($x0 + 135) ($y0 + 625)      # OK
Start-Sleep -Seconds 4
Write-Host ("[5] windows: " + (ListWins))
Snap "after_ok"
