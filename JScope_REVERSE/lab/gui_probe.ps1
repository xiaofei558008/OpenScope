param([string]$Prj = "", [string]$Tag = "t", [int]$WaitSec = 8, [string]$Keys = "", [int]$KeysWait = 3, [switch]$Kill)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class P {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x,int y,int cx,int cy,uint f);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x,int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f,uint dx,uint dy,uint d,IntPtr e);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public struct RECT { public int Left, Top, Right, Bottom; }
  public static uint Pid; public static System.Collections.Generic.List<IntPtr> W = new System.Collections.Generic.List<IntPtr>();
  public static bool Cb(IntPtr h, IntPtr p) { uint pid; GetWindowThreadProcessId(h, out pid); if (pid == Pid && IsWindowVisible(h)) W.Add(h); return true; }
  public static string Title(IntPtr h) { var t = new StringBuilder(512); GetWindowTextW(h, t, 512); return t.ToString(); }
}
"@
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
if ($Prj -eq "") { $proc = Start-Process -FilePath $exe -PassThru } else { $proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", $Prj) -PassThru }
Start-Sleep -Seconds $WaitSec
[P]::Pid = $proc.Id
function Snap([string]$suffix) {
  [P]::W.Clear(); [void][P]::EnumWindows([P+EnumProc]{ param($a,$b) [P]::Cb($a,$b) }, [IntPtr]::Zero)
  $main = [IntPtr]::Zero
  foreach ($h in [P]::W) { $t = [P]::Title($h); if ($t -eq "SEGGER J-Scope V9.66") { $main = $h } }
  if ($main -ne [IntPtr]::Zero) {
    [void][P]::SetWindowPos($main, [IntPtr]::Zero, 0, 0, 1500, 800, 0x0040)
    Start-Sleep -Milliseconds 400
    $r = New-Object P+RECT; [void][P]::GetWindowRect($main, [ref]$r)
    $w = $r.Right-$r.Left; $ht = $r.Bottom-$r.Top
    $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w,$ht)
    $g = [System.Drawing.Graphics]::FromImage($bmp); $g.CopyFromScreen($r.Left,$r.Top,0,0,(New-Object System.Drawing.Size -ArgumentList @($w,$ht)))
    $bmp.Save("D:\OpenScope\JScope_REVERSE\lab\$Tag$suffix.png"); $g.Dispose(); $bmp.Dispose()
  }
  $names = @(); foreach ($h in [P]::W) { $t = [P]::Title($h); if ($t -ne "" -and $t -ne "MSCTFIME UI" -and $t -ne "Default IME") { $names += $t } }
  Write-Host ("[{0}{1}] windows: {2}" -f $Tag, $suffix, ($names -join " | "))
  return $main
}
$main = Snap "_a"
if ($Keys -ne "") {
  [void][P]::SetForegroundWindow($main); Start-Sleep -Milliseconds 300
  $vk = $Keys.Split(',') | ForEach-Object { [int]$_ }
  foreach ($k in $vk) { [void][P]::keybd_event($k,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 40 }
  foreach ($k in $vk) { [void][P]::keybd_event($k,0,2,[IntPtr]::Zero) }
  Start-Sleep -Seconds $KeysWait
  [void](Snap "_b")
}
if ($Kill) { $proc.Refresh(); if (-not $proc.HasExited) { [void][P]::SetForegroundWindow($main); [void][P]::keybd_event(0x1B,0,0,[IntPtr]::Zero); [void][P]::keybd_event(0x1B,0,2,[IntPtr]::Zero); Start-Sleep -Seconds 1; $proc.Refresh(); if (-not $proc.HasExited) { $proc.Kill(); Write-Host "[$Tag] killed" } } }
