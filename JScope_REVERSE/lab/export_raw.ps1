param(
  [string]$Prj = "D:\OpenScope\JScope_REVERSE\f303_debug.jscope",
  [string]$RawPath = "D:\OpenScope\JScope_REVERSE\lab\export.jraw",
  [int]$Warmup = 10,
  [int]$RunSec = 15,
  [int]$AfterSave = 6,
  [string]$Tag = "exp"
)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class EX {
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool EnumWindows(P cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x, int y, int cx, int cy, uint fl);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out R r);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  [DllImport("user32.dll")] public static extern short VkKeyScanW(char c);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  public delegate bool P(IntPtr h, IntPtr p);
  public struct R { public int L; public int T; public int Rr; public int B; }
  public static uint Pid;
  public static System.Text.StringBuilder O = new System.Text.StringBuilder();
  public static bool Cb(IntPtr h, IntPtr p) {
    uint pid; GetWindowThreadProcessId(h, out pid);
    var t = new StringBuilder(160); GetWindowTextW(h, t, 160);
    if (pid == Pid && IsWindowVisible(h) && t.Length > 0) {
      R r; GetWindowRect(h, out r);
      O.Append(string.Format("0x{0:x}\t{1}\t({2},{3})\t{4}x{5}\n", (long)h, t.ToString(), r.L, r.T, r.Rr - r.L, r.B - r.T));
    }
    return true;
  }
  public static string TT(IntPtr h) { var t = new StringBuilder(160); GetWindowTextW(h, t, 160); return t.ToString(); }
}
"@
[void][EX]::SetProcessDPIAware()
$OUT = "D:\OpenScope\JScope_REVERSE\lab"
function Wins([int]$pid_) {
  [EX]::Pid = [uint32]$pid_; [EX]::O.Length = 0
  [void][EX]::EnumWindows([EX+P]{ param($a,$b) [EX]::Cb($a,$b) }, [IntPtr]::Zero)
  return [EX]::O.ToString()
}
function MainHwnd([int]$pid_) {
  $txt = Wins $pid_
  foreach ($line in ($txt -split "`n")) {
    if ($line -like "*SEGGER J-Scope V9.66*") {
      $hx = ($line -split "`t")[0]
      return [IntPtr][Convert]::ToInt64($hx.Substring(2), 16)
    }
  }
  return [IntPtr]::Zero
}
function Key([int]$vk, [int]$n = 1) { for ($i=0; $i -lt $n; $i++) { [EX]::keybd_event([byte]$vk,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 45; [EX]::keybd_event([byte]$vk,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 90 } }
function CtrlKey([int]$vk) {
  [EX]::keybd_event(0x11,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 60
  [EX]::keybd_event([byte]$vk,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 60
  [EX]::keybd_event([byte]$vk,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 60
  [EX]::keybd_event(0x11,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 150
}
function TypeText([string]$s) {
  foreach ($ch in $s.ToCharArray()) {
    $vk = [EX]::VkKeyScanW($ch); if ($vk -eq -1) { continue }
    $code = $vk -band 0xFF; $sh = ($vk -shr 8) -band 0xFF
    $ns = ($sh -band 1) -ne 0; $nc = ($sh -band 2) -ne 0
    if ($ns) { [EX]::keybd_event(0x10,0,0,[IntPtr]::Zero) }
    if ($nc) { [EX]::keybd_event(0x11,0,0,[IntPtr]::Zero) }
    [EX]::keybd_event([byte]$code,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 25
    [EX]::keybd_event([byte]$code,0,2,[IntPtr]::Zero); Start-Sleep -Milliseconds 25
    if ($nc) { [EX]::keybd_event(0x11,0,2,[IntPtr]::Zero) }
    if ($ns) { [EX]::keybd_event(0x10,0,2,[IntPtr]::Zero) }
  }
  Start-Sleep -Milliseconds 300
}
function Snap([string]$name) {
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @(1920, 1080)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen(0, 0, 0, 0, (New-Object System.Drawing.Size -ArgumentList @(1920, 1080)))
  $bmp.Save("$OUT\$name.png", [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose()
  Write-Host "  [snap $name]"
}

Get-Process JScope -ErrorAction SilentlyContinue | ForEach-Object { try { $_.Kill() } catch {} }
Start-Sleep -Seconds 2
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", $Prj) -PassThru
Start-Sleep -Seconds $Warmup
$proc.Refresh()
if ($proc.HasExited) { Write-Host ("EXITED " + $proc.ExitCode); exit 1 }
$main = MainHwnd $proc.Id
Write-Host ("main hwnd = 0x{0:x}" -f [int64]$main)
[void][EX]::SetWindowPos($main, [IntPtr]::Zero, 0, 0, 1920, 1040, 0x0040)
[void][EX]::BringWindowToTop($main); [void][EX]::SetForegroundWindow($main)
Start-Sleep -Milliseconds 800
Write-Host "F5 start sampling"
Key 0x74
Start-Sleep -Seconds $RunSec
Snap "${Tag}_sampling"
Write-Host "Ctrl+R export raw"
CtrlKey 0x52
Start-Sleep -Seconds 3
Write-Host "windows:"; Write-Host (Wins $proc.Id)
Snap "${Tag}_savedlg"
TypeText $RawPath
Start-Sleep -Milliseconds 500
Key 13
Start-Sleep -Seconds $AfterSave
Write-Host "windows after save:"; Write-Host (Wins $proc.Id)
Snap "${Tag}_saved"
Write-Host ("jraw exists: " + (Test-Path $RawPath))
if (Test-Path $RawPath) { Write-Host ("size = " + (Get-Item $RawPath).Length) }
