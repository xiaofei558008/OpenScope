$ErrorActionPreference = "Continue"
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class DP {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, uint dx, uint dy, uint d, IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
  public static uint Pid;
  public static System.Collections.Generic.List<IntPtr> W = new System.Collections.Generic.List<IntPtr>();
  public static bool Cb(IntPtr h, IntPtr p) { uint pid; GetWindowThreadProcessId(h, out pid); if (pid == Pid && IsWindowVisible(h)) W.Add(h); return true; }
  public static string T(IntPtr h) { var t = new StringBuilder(256); GetWindowTextW(h, t, 256); return t.ToString(); }
}
"@
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
Get-Process JScope -ErrorAction SilentlyContinue | ForEach-Object { try { $_.Kill() } catch {} }
Start-Sleep -Seconds 2
$proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", "D:\OpenScope\JScope_REVERSE\lab\t1_scope_ok.jscope") -PassThru
Start-Sleep -Seconds 10
[DP]::Pid = [uint32]$proc.Id
function Show([string]$phase) {
  [DP]::W.Clear()
  [void][DP]::EnumWindows([DP+EnumProc]{ param($a,$b) [DP]::Cb($a,$b) }, [IntPtr]::Zero)
  Write-Host "--- $phase ---"
  foreach ($h in [DP]::W) {
    $t = [DP]::T($h)
    if ($t -eq "" -or $t -like "*IME*") { continue }
    $r = New-Object DP+RECT; [void][DP]::GetWindowRect($h, [ref]$r)
    Write-Host ("   0x{0:x} '{1}' @({2},{3}) {4}x{5}" -f [int64]$h, $t, $r.Left, $r.Top, ($r.Right-$r.Left), ($r.Bottom-$r.Top))
  }
}
Show "启动后"
for ($round = 1; $round -le 4; $round++) {
  $btn = [IntPtr]::Zero
  foreach ($h in [DP]::W) { $t = [DP]::T($h); if ($t -like "*Warning*" -or $t -like "*Error*" -or $t -like "*Information*") { $btn = $h } }
  if ($btn -eq [IntPtr]::Zero) { Write-Host "round $round : 无对话框"; break }
  $r = New-Object DP+RECT; [void][DP]::GetWindowRect($btn, [ref]$r)
  # Qt 消息框的按钮在右下角：取右侧 12% ~ 6% 处的中心
  $x = [int]($r.Right - ($r.Right - $r.Left) * 0.09)
  $y = [int]($r.Bottom - 28)
  Write-Host ("round {0}: 点击 '{1}' 的确定按钮 @({2},{3})" -f $round, [DP]::T($btn), $x, $y)
  [void][DP]::SetForegroundWindow($btn)
  [void][DP]::SetCursorPos($x, $y); Start-Sleep -Milliseconds 250
  [DP]::mouse_event(0x0002,0,0,0,[IntPtr]::Zero); Start-Sleep -Milliseconds 80
  [DP]::mouse_event(0x0004,0,0,0,[IntPtr]::Zero)
  Start-Sleep -Seconds 2
  Show "round $round 之后"
}
try { $proc.Kill() } catch {}
