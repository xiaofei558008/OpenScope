param(
  [Parameter(Mandatory=$true)][string]$Seq,
  [int]$MainPid = 0,
  [string]$Tag = "k",
  [int]$ShotEach = 0,
  [string]$Title = ""
)
# Seq 语法：步骤用 ';' 分隔；组合键用 '+' 连接；支持 TAB DOWN UP ENTER ESC SPACE ALT CTRL SHIFT F5 DEL
# 例： "ALT+SPACE;M;UP;UP;ENTER"
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class K {
    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
  public static uint Pid;
  public static System.Collections.Generic.List<IntPtr> W = new System.Collections.Generic.List<IntPtr>();
  public static bool Cb(IntPtr h, IntPtr p) { uint pid; GetWindowThreadProcessId(h, out pid); if (pid == Pid && IsWindowVisible(h)) W.Add(h); return true; }
  public static string Title(IntPtr h) { var t = new StringBuilder(512); GetWindowTextW(h, t, 512); return t.ToString(); }
}
"@
[void][K]::SetProcessDPIAware()
if ($MainPid -eq 0) { $p = Get-Process JScope | Select-Object -First 1; $MainPid = $p.Id }
[K]::Pid = $MainPid

function Foreground() {
  [K]::W.Clear(); [void][K]::EnumWindows([K+EnumProc]{ param($a,$b) [K]::Cb($a,$b) }, [IntPtr]::Zero)
  $fg = [K]::GetForegroundWindow()
  # 优先把该进程的对话框拉到前台；否则用主窗口
  $dlg = [IntPtr]::Zero; $main = [IntPtr]::Zero
  foreach ($h in [K]::W) {
    $t = [K]::Title($h)
    if ($t -notlike "*IME*" -and $t -ne "") {
      if ($t -like "*SEGGER J-Scope*") { $main = $h } else { $dlg = $h }
    }
  }
  $target = $main
  if ($dlg -ne [IntPtr]::Zero) { $target = $dlg }
  if ($Title -ne "") {
    foreach ($h in [K]::W) { if ([K]::Title($h) -eq $Title) { $target = $h } }
  }
  if ($target -ne [IntPtr]::Zero) { [void][K]::BringWindowToTop($target); [void][K]::SetForegroundWindow($target) }
  return $target
}
function Snap([string]$suffix) {
  $main = [IntPtr]::Zero
  foreach ($h in [K]::W) { $t = [K]::Title($h); if ($t -like "*SEGGER J-Scope*") { $main = $h } }
  if ($main -eq [IntPtr]::Zero) { return }
  $r = New-Object K+RECT; [void][K]::GetWindowRect($main, [ref]$r)
  $w = $r.Right-$r.Left; $ht = $r.Bottom-$r.Top
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w,$ht)
  $g = [System.Drawing.Graphics]::FromImage($bmp); $g.CopyFromScreen($r.Left,$r.Top,0,0,(New-Object System.Drawing.Size -ArgumentList @($w,$ht)))
  $bmp.Save("D:\OpenScope\JScope_REVERSE\lab\$Tag$suffix.png"); $g.Dispose(); $bmp.Dispose()
  Write-Host "  [shot $Tag$suffix.png]"
}
$MAP = @{ "TAB"=9; "DOWN"=0x28; "UP"=0x26; "LEFT"=0x25; "RIGHT"=0x27; "ENTER"=13; "ESC"=0x1B; "SPACE"=0x20;
          "ALT"=0x12; "CTRL"=0x11; "SHIFT"=0x10; "DEL"=0x2E; "BACK"=8; "F5"=0x74; "F9"=0x78; "HOME"=0x24; "END"=0x23 }
function Press([string]$step) {
  $keys = @()
  foreach ($k in $step.Split('+')) {
    $k = $k.Trim().ToUpper()
    if ($MAP.ContainsKey($k)) { $keys += $MAP[$k] }
    elseif ($k.Length -eq 1) { $keys += [int][char]$k }
    else { Write-Host "  ?unknown key $k" }
  }
  [void](Foreground)
  Start-Sleep -Milliseconds 200
  foreach ($vk in $keys) { [void][K]::keybd_event([byte]$vk, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 40 }
  foreach ($vk in $keys) { [void][K]::keybd_event([byte]$vk, 0, 2, [IntPtr]::Zero); Start-Sleep -Milliseconds 40 }
  Start-Sleep -Milliseconds 400
}
$i = 0
foreach ($step in $Seq.Split(';')) {
  $s = $step.Trim()
  if ($s -eq "") { continue }
  if ($s -like "WAIT:*") { Start-Sleep -Seconds ([double]($s.Substring(5))); continue }
  if ($s -like "TYPE:*") {
    Add-Type -AssemblyName System.Windows.Forms
    $tg = Foreground; Start-Sleep -Milliseconds 300
    Write-Host ("    [TYPE] target=0x{0:x} '{1}'  fg=0x{2:x} '{3}'" -f [int64]$tg, [K]::Title($tg), [int64][K]::GetForegroundWindow(), [K]::Title([K]::GetForegroundWindow()))
    [System.Windows.Forms.SendKeys]::SendWait($s.Substring(5))
    Start-Sleep -Milliseconds 400
    continue
  }
  Write-Host ("  step: {0}" -f $s)
  Press $s
  $i++
  if ($ShotEach -gt 0) { [void](Foreground); Snap ("_" + $i) }
}
[void](Foreground)
Snap "_final"
