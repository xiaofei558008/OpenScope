param(
  [Parameter(Mandatory=$true)][string]$Prj,
  [string]$Extra = "",
  [int]$Warmup = 10,
  [int]$AfterF5 = 12,
  [string]$Tag = "run",
  [switch]$NoF5,
  [switch]$KeepOpen
)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class JR {
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
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  public delegate bool P(IntPtr h, IntPtr p);
  public struct R { public int L; public int T; public int Rr; public int B; }
  public static uint Pid;
  public static System.Text.StringBuilder O = new System.Text.StringBuilder();
  public static bool Cb(IntPtr h, IntPtr p) {
    uint pid; GetWindowThreadProcessId(h, out pid);
    var t = new StringBuilder(160); GetWindowTextW(h, t, 160);
    if (pid == Pid && IsWindowVisible(h) && t.Length > 0) {
      R r; GetWindowRect(h, out r);
      O.Append(string.Format("  0x{0:x} '{1}' ({2},{3}) {4}x{5}\n", (long)h, t.ToString(), r.L, r.T, r.Rr-r.L, r.B-r.T));
    }
    return true;
  }
  public static string TT(IntPtr h) { var t = new StringBuilder(160); GetWindowTextW(h, t, 160); return t.ToString(); }
}
"@
[void][JR]::SetProcessDPIAware()
$OUT = "D:\OpenScope\JScope_REVERSE\lab"
function Wins([int]$pid_) {
  [JR]::Pid = [uint32]$pid_; [JR]::O.Length = 0
  [void][JR]::EnumWindows([JR+P]{ param($a,$b) [JR]::Cb($a,$b) }, [IntPtr]::Zero)
  return [JR]::O.ToString()
}
function Snap([string]$name, [int]$w = 1920, [int]$h = 1080) {
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w, $h)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen(0, 0, 0, 0, (New-Object System.Drawing.Size -ArgumentList @($w, $h)))
  $bmp.Save("$OUT\$name.png", [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose()
  Write-Host "  [snap $name.png]"
}

Get-Process JScope -ErrorAction SilentlyContinue | ForEach-Object { try { $_.Kill() } catch {} }
Start-Sleep -Seconds 2
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$args = @("-openprj", $Prj)
if ($Extra -ne "") { $args += $Extra.Split(' ') }
Write-Host ("[run] $exe " + ($args -join ' '))
$proc = Start-Process -FilePath $exe -ArgumentList $args -PassThru
Start-Sleep -Seconds $Warmup
$proc.Refresh()
if ($proc.HasExited) { Write-Host ("[run] EXITED {0}" -f $proc.ExitCode); exit 1 }
Write-Host "[run] windows:"; Write-Host (Wins $proc.Id)
Snap "${Tag}_a"
if (-not $NoF5) {
  # 主窗口拉到前台并调整到固定位置，方便读状态栏
  [JR]::Pid = [uint32]$proc.Id; [JR]::O.Length = 0
  [void][JR]::EnumWindows([JR+P]{ param($a,$b) [JR]::Cb($a,$b) }, [IntPtr]::Zero)
  $main = [IntPtr]::Zero
  foreach ($line in ([JR]::O.ToString() -split "`n")) {
    if ($line -like "*SEGGER J-Scope V9.66*") {
      $hex = (($line -split "'")[0] -replace '.*0x', '').Trim()
      $main = [IntPtr][Convert]::ToInt64($hex, 16)
    }
  }
  if ($main -ne [IntPtr]::Zero) {
    [void][JR]::SetWindowPos($main, [IntPtr]::Zero, 0, 0, 1400, 1000, 0x0040)
    [void][JR]::BringWindowToTop($main); [void][JR]::SetForegroundWindow($main)
    Start-Sleep -Milliseconds 600
    Write-Host "[run] F5 (start sampling)"
    [JR]::keybd_event(0x74,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 80
    [JR]::keybd_event(0x74,0,2,[IntPtr]::Zero)
  }
  Start-Sleep -Seconds $AfterF5
  $proc.Refresh()
  if ($proc.HasExited) { Write-Host ("[run] J-Scope 崩溃/退出: {0}" -f $proc.ExitCode) }
  else { Write-Host "[run] windows:"; Write-Host (Wins $proc.Id) }
  Snap "${Tag}_b"
}
if (-not $KeepOpen) {
  # 先停采（F9），避免强杀把探针留在采样态
  [JR]::keybd_event(0x78,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 80
  [JR]::keybd_event(0x78,0,2,[IntPtr]::Zero)
  Start-Sleep -Seconds 2
  $proc.Refresh()
  if (-not $proc.HasExited) {
    [JR]::Pid = [uint32]$proc.Id; [JR]::O.Length = 0
    [void][JR]::EnumWindows([JR+P]{ param($a,$b) [JR]::Cb($a,$b) }, [IntPtr]::Zero)
    foreach ($line in ([JR]::O.ToString() -split "`n")) {
      if ($line -like "*SEGGER J-Scope V9.66*") {
        $hex = (($line -split "'")[0] -replace '.*0x', '').Trim()
        $h = [IntPtr][Convert]::ToInt64($hex, 16)
        [void][JR]::PostMessage($h, 0x10, [IntPtr]::Zero, [IntPtr]::Zero)
      }
    }
    Start-Sleep -Seconds 2
    $proc.Refresh()
    if (-not $proc.HasExited) { try { $proc.Kill() } catch {} }
  }
}
Write-Host "[run] done"
