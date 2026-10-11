param([string]$Path = "D:\OpenScope\JScope_REVERSE\lab\v_minimal.jscope", [string]$Out = "D:\OpenScope\JScope_REVERSE\lab\guiopen.png")
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms
Add-Type @"
using System;using System.Runtime.InteropServices;using System.Text;
public class O {
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte sc, uint f, IntPtr e);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public static uint Pid; public static System.Collections.Generic.List<IntPtr> W = new System.Collections.Generic.List<IntPtr>();
  public static bool Cb(IntPtr h, IntPtr p) { uint pid; GetWindowThreadProcessId(h, out pid); if (pid == Pid && IsWindowVisible(h)) W.Add(h); return true; }
  public static string Title(IntPtr h) { var t = new StringBuilder(512); GetWindowTextW(h, t, 512); return t.ToString(); }
}
"@
$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$proc = Start-Process -FilePath $exe -PassThru
Start-Sleep -Seconds 8
[O]::Pid = $proc.Id
function Top() {
  [O]::W.Clear(); [void][O]::EnumWindows([O+EnumProc]{ param($a,$b) [O]::Cb($a,$b) }, [IntPtr]::Zero)
  $main = [IntPtr]::Zero; $names = @()
  foreach ($h in [O]::W) { $t = [O]::Title($h); if ($t -like "*SEGGER J-Scope*") { $main = $h }; if ($t -ne "" -and $t -notlike "*IME*") { $names += $t } }
  Write-Host ("windows: " + ($names -join " | "))
  return $main
}
$main = Top
[void][O]::SetForegroundWindow($main); Start-Sleep -Milliseconds 400

# 1) 关掉欢迎框（ESC）
[void][O]::keybd_event(0x1B,0,0,[IntPtr]::Zero); [void][O]::keybd_event(0x1B,0,2,[IntPtr]::Zero)
Start-Sleep -Seconds 2
[void](Top)
[void][O]::SetForegroundWindow($main); Start-Sleep -Milliseconds 300

# 2) Ctrl+O 打开工程
[void][O]::keybd_event(0x11,0,0,[IntPtr]::Zero); [void][O]::keybd_event(0x4F,0,0,[IntPtr]::Zero)
Start-Sleep -Milliseconds 100
[void][O]::keybd_event(0x4F,0,2,[IntPtr]::Zero); [void][O]::keybd_event(0x11,0,2,[IntPtr]::Zero)
Start-Sleep -Seconds 3
[void](Top)

# 3) 输入路径并回车
[System.Windows.Forms.SendKeys]::SendWait($Path)
Start-Sleep -Milliseconds 500
[System.Windows.Forms.SendKeys]::SendWait("{ENTER}")
Start-Sleep -Seconds 4
[void](Top)

# 4) 截屏
$b = New-Object System.Drawing.Bitmap -ArgumentList @(1500, 850)
$g = [System.Drawing.Graphics]::FromImage($b)
$g.CopyFromScreen(0, 0, 0, 0, (New-Object System.Drawing.Size -ArgumentList @(1500, 850)))
$b.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $b.Dispose()
Write-Host "saved $Out"
$proc.Refresh(); if (-not $proc.HasExited) { $proc.Kill() }
