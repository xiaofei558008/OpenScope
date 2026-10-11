Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class P2 {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x,int y,int cx,int cy,uint f);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x,int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f,uint dx,uint dy,uint d,IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
  public static uint Pid; public static System.Collections.Generic.List<IntPtr> W = new System.Collections.Generic.List<IntPtr>();
  public static bool Cb(IntPtr h, IntPtr p) { uint pid; GetWindowThreadProcessId(h, out pid); if (pid == Pid && IsWindowVisible(h)) W.Add(h); return true; }
  public static string T(IntPtr h) { var t = new StringBuilder(256); GetWindowTextW(h, t, 256); return t.ToString(); }
}
"@
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$prj = "D:\OpenScope\JScope_REVERSE\lab\v_minimal.jscope"
$proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", $prj) -PassThru
Start-Sleep -Seconds 8
[P2]::Pid = $proc.Id
function Report([string]$phase) {
  [P2]::W.Clear(); [void][P2]::EnumWindows([P2+EnumProc]{ param($a,$b) [P2]::Cb($a,$b) }, [IntPtr]::Zero)
  Write-Host "--- $phase ---"
  foreach ($h in [P2]::W) {
    $t = [P2]::T($h)
    if ($t -eq "" -or $t -like "*IME*") { continue }
    $r = New-Object P2+RECT; [void][P2]::GetWindowRect($h, [ref]$r)
    Write-Host ("   0x{0:x} '{1}' @({2},{3}) {4}x{5}" -f [int64]$h, $t, $r.Left, $r.Top, ($r.Right-$r.Left), ($r.Bottom-$r.Top))
  }
}
Report "启动后"
# 逐个点击每个对话框的确定按钮（用其矩形右下角附近的典型按钮位置）
for ($round = 1; $round -le 3; $round++) {
  $dlgs = @()
  foreach ($h in [P2]::W) { $t = [P2]::T($h); if ($t -like "*Warning*" -or $t -like "*Error*" -or $t -like "*Information*") { $dlgs += $h } }
  if ($dlgs.Count -eq 0) { Write-Host "round $round : 无对话框"; break }
  foreach ($h in $dlgs) {
    $r = New-Object P2+RECT; [void][P2]::GetWindowRect($h, [ref]$r)
    $bx = [int](($r.Left + $r.Right) / 2 + ($r.Right - $r.Left) * 0.30)
    $by = $r.Bottom - 30
    Write-Host ("round {0}: 点击 '{1}' 的按钮 @({2},{3})" -f $round, [P2]::T($h), $bx, $by)
    [void][P2]::SetForegroundWindow($h)
    [void][P2]::SetCursorPos($bx, $by); Start-Sleep -Milliseconds 200
    [P2]::mouse_event(0x0002,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 80
    [P2]::mouse_event(0x0004,0,0,0,[IntPtr]::Zero)
    Start-Sleep -Seconds 2
  }
  Report "round $round 之后"
}
$prod = Get-Process -Id $proc.Id -ErrorAction SilentlyContinue
if ($prod) { $prod.Kill() }
