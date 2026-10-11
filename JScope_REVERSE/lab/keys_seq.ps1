param([Parameter(Mandatory=$true)][string]$Seq, [string]$Tag="k", [int]$MainPid=0)
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class S {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
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
if ($MainPid -eq 0) { $p = Get-Process JScope | Select-Object -First 1; $MainPid = $p.Id }
[S]::Pid = $MainPid
function Shot([string]$suffix) {
  [S]::W.Clear(); [void][S]::EnumWindows([S+EnumProc]{ param($a,$b) [S]::Cb($a,$b) }, [IntPtr]::Zero)
  $main = [IntPtr]::Zero
  $list = @()
  foreach ($h in [S]::W) { $t = [S]::Title($h); if ($t -like "*SEGGER J-Scope*") { $main = $h }; if ($t -ne "") { $list += $t } }
  Write-Host ("[{0}{1}] {2}" -f $Tag, $suffix, ($list -join " | "))
  if ($main -ne [IntPtr]::Zero) {
    $r = New-Object S+RECT; [void][S]::GetWindowRect($main, [ref]$r)
    $w = $r.Right-$r.Left; $ht = $r.Bottom-$r.Top
    $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w,$ht)
    $g = [System.Drawing.Graphics]::FromImage($bmp); $g.CopyFromScreen($r.Left,$r.Top,0,0,(New-Object System.Drawing.Size -ArgumentList @($w,$ht)))
    $bmp.Save("D:\OpenScope\JScope_REVERSE\lab\$Tag$suffix.png"); $g.Dispose(); $bmp.Dispose()
  }
  return $main
}
$main = Shot "_0"
# Seq 形如 "TAB,DOWN,ENTER,CLICK:725:663"
$i = 1
foreach ($step in $Seq.Split(',')) {
  [void][S]::SetForegroundWindow($main)
  Start-Sleep -Milliseconds 250
  switch -Regex ($step.Trim()) {
    '^TAB$'   { [void][S]::keybd_event(9,0,0,[IntPtr]::Zero); [void][S]::keybd_event(9,0,2,[IntPtr]::Zero) }
    '^DOWN$'  { [void][S]::keybd_event(0x28,0,0,[IntPtr]::Zero); [void][S]::keybd_event(0x28,0,2,[IntPtr]::Zero) }
    '^UP$'    { [void][S]::keybd_event(0x26,0,0,[IntPtr]::Zero); [void][S]::keybd_event(0x26,0,2,[IntPtr]::Zero) }
    '^ENTER$' { [void][S]::keybd_event(13,0,0,[IntPtr]::Zero); [void][S]::keybd_event(13,0,2,[IntPtr]::Zero) }
    '^ESC$'   { [void][S]::keybd_event(0x1B,0,0,[IntPtr]::Zero); [void][S]::keybd_event(0x1B,0,2,[IntPtr]::Zero) }
    '^SPACE$' { [void][S]::keybd_event(0x20,0,0,[IntPtr]::Zero); [void][S]::keybd_event(0x20,0,2,[IntPtr]::Zero) }
    '^CLICK:(\d+):(\d+)$' {
      [void][S]::SetCursorPos([int]$Matches[1], [int]$Matches[2]); Start-Sleep -Milliseconds 200
      [S]::mouse_event(0x0002,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 60
      [S]::mouse_event(0x0004,0,0,0,[IntPtr]::Zero)
    }
    '^WAIT:(\d+)$' { Start-Sleep -Seconds ([int]$Matches[1]) }
  }
  Start-Sleep -Milliseconds 500
  [void](Shot ("_" + $i)); $i++
}
