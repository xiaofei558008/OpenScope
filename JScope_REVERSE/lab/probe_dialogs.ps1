param([string]$Prj = "D:\OpenScope\JScope_REVERSE\lab\hss_test.jscope", [string]$Tag = "probe")
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class D {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassNameW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public struct RECT { public int Left, Top, Right, Bottom; }
  public static uint Pid; public static System.Collections.Generic.List<IntPtr> Wins = new System.Collections.Generic.List<IntPtr>();
  public static bool Cb(IntPtr h, IntPtr p) {
    uint pid; GetWindowThreadProcessId(h, out pid);
    if (pid == Pid && IsWindowVisible(h)) Wins.Add(h);
    return true;
  }
}
"@
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", $Prj) -PassThru
Start-Sleep -Seconds 8
[D]::Pid = $proc.Id
function Dump([string]$phase) {
  [D]::Wins.Clear()
  [void][D]::EnumWindows([D+EnumProc]{ param($a,$b) [D]::Cb($a,$b) }, [IntPtr]::Zero)
  Write-Host "--- $phase ---"
  foreach ($h in [D]::Wins) {
    $t = New-Object System.Text.StringBuilder 512; [void][D]::GetWindowTextW($h, $t, 512)
    $c = New-Object System.Text.StringBuilder 128; [void][D]::GetClassNameW($h, $c, 128)
    $r = New-Object D+RECT; [void][D]::GetWindowRect($h, [ref]$r)
    Write-Host ("  0x{0:x} [{1}] '{2}' {3}x{4}" -f [int64]$h, $c.ToString(), $t.ToString(), ($r.Right-$r.Left), ($r.Bottom-$r.Top))
  }
  return [D]::Wins
}
$w1 = Dump "after-start"
foreach ($h in $w1) {
  $t = New-Object System.Text.StringBuilder 512; [void][D]::GetWindowTextW($h, $t, 512)
  if ($t.ToString() -like "*Warning*" -or $t.ToString() -like "*Error*") {
    # 截图该对话框
    $r = New-Object D+RECT; [void][D]::GetWindowRect($h, [ref]$r)
    $w = $r.Right-$r.Left; $ht = $r.Bottom-$r.Top
    $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w,$ht)
    $g = [System.Drawing.Graphics]::FromImage($bmp); $g.CopyFromScreen($r.Left,$r.Top,0,0,(New-Object System.Drawing.Size -ArgumentList @($w,$ht)))
    $bmp.Save("D:\OpenScope\JScope_REVERSE\lab\${Tag}_box.png"); $g.Dispose(); $bmp.Dispose()
    Write-Host "  [saved ${Tag}_box.png]"
    [void][D]::SetForegroundWindow($h)
    [void][D]::keybd_event(0x0D,0,0,[IntPtr]::Zero); [void][D]::keybd_event(0x0D,0,2,[IntPtr]::Zero)
    Start-Sleep -Seconds 2
  }
}
[void](Dump "after-enter")
$proc.Refresh()
if (-not $proc.HasExited) { $proc.Kill() }
