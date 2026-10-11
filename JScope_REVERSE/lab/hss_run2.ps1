param(
  [string]$Prj  = "D:\OpenScope\JScope_REVERSE\lab\hss_test.jscope",
  [string]$Tag  = "hss",
  [int]$WarmupMs = 6000,
  [int]$SampleMs = 12000,
  [string]$ExtraArgs = "",
  [switch]$NoStart,
  [switch]$ClickPlay
)

$ErrorActionPreference = "Continue"
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W2 {
  [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr after, int x, int y, int cx, int cy, uint flags);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, IntPtr extra);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr p);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowTextW(IntPtr h, System.Text.StringBuilder s, int n);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassNameW(IntPtr h, System.Text.StringBuilder s, int n);
  public delegate bool EnumProc(IntPtr h, IntPtr p);
  public static System.Collections.Generic.List<string> DlgTitles = new System.Collections.Generic.List<string>();
  public static uint TargetPid = 0;
  public static bool Collect(IntPtr h, IntPtr p) {
    uint pid; GetWindowThreadProcessId(h, out pid);
    if (pid == TargetPid) {
      var t = new System.Text.StringBuilder(256); GetWindowTextW(h, t, 256);
      var c = new System.Text.StringBuilder(64);  GetClassNameW(h, c, 64);
      if (t.Length > 0) DlgTitles.Add(c.ToString() + " | " + t.ToString());
    }
    return true;
  }
  [DllImport("user32.dll")] public static extern void mouse_event(uint flags, uint dx, uint dy, uint data, IntPtr extra);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
}
"@

$outDir = "D:\OpenScope\JScope_REVERSE\lab"
function Shot([IntPtr]$h, [string]$path) {
  $r = New-Object W2+RECT
  [void][W2]::GetWindowRect($h, [ref]$r)
  $w = $r.Right - $r.Left; $ht = $r.Bottom - $r.Top
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w, $ht)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size -ArgumentList @($w, $ht)))
  $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
  $g.Dispose(); $bmp.Dispose()
  Write-Host "[shot] $path  (${w}x${ht} @ $($r.Left),$($r.Top))"
}

$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$argList = @("-openprj", $Prj)
if ($ExtraArgs -ne "") { $argList += $ExtraArgs.Split(' ') }
Write-Host "[run] $exe $($argList -join ' ')"
$proc = Start-Process -FilePath $exe -ArgumentList $argList -PassThru
$h = [IntPtr]::Zero
for ($i = 0; $i -lt 30; $i++) {
  Start-Sleep -Milliseconds 500
  $proc.Refresh()
  if ($proc.HasExited) { Write-Host "[run] exited early: $($proc.ExitCode)"; exit 1 }
  if ($proc.MainWindowHandle -ne 0) { $h = $proc.MainWindowHandle; break }
}
if ($h -eq [IntPtr]::Zero) { Write-Host "[run] no window"; exit 1 }

# 放到 (0,0) 并调整到 1500x800，保证底部状态栏可见
[void][W2]::SetWindowPos($h, [IntPtr]::Zero, 0, 0, 1500, 800, 0x0040)
[void][W2]::SetForegroundWindow($h)
Start-Sleep -Milliseconds $WarmupMs
$proc.Refresh()
if ($proc.HasExited) { Write-Host "[run] exited during warmup: $($proc.ExitCode)"; exit 1 }

Shot $h "$outDir\${Tag}_before.png"

if (-not $NoStart) {
  if ($ClickPlay) {
    # 点击工具栏 ▶ (窗口坐标 ~110,80)
    [void][W2]::SetCursorPos(112, 80)
    Start-Sleep -Milliseconds 200
    [void][W2]::mouse_event(0x0002, 0, 0, 0, [IntPtr]::Zero)   # LEFTDOWN
    Start-Sleep -Milliseconds 60
    [void][W2]::mouse_event(0x0004, 0, 0, 0, [IntPtr]::Zero)   # LEFTUP
    Write-Host "[run] clicked play button"
  } else {
    [void][W2]::SetForegroundWindow($h)
    Start-Sleep -Milliseconds 300
    [void][W2]::keybd_event(0x74, 0, 0, [IntPtr]::Zero)        # F5 down
    Start-Sleep -Milliseconds 80
    [void][W2]::keybd_event(0x74, 0, 2, [IntPtr]::Zero)        # F5 up
    Write-Host "[run] sent F5 (keybd_event)"
  }
}

Start-Sleep -Milliseconds $SampleMs
$proc.Refresh()
if ($proc.HasExited) {
  Write-Host "[run] 采集期间退出/崩溃: $($proc.ExitCode)  (0xC0000094=整数除零)"
} else {
  Shot $h "$outDir\${Tag}_after.png"
  [W2]::DlgTitles.Clear(); [W2]::TargetPid = $proc.Id
  [void][W2]::EnumWindows([W2+EnumProc]{ param($a,$b) [W2]::Collect($a,$b) }, [IntPtr]::Zero)
  Write-Host "[run] 该进程的顶层窗口/对话框:"
  foreach ($t in [W2]::DlgTitles) { Write-Host "        $t" }
}

Write-Host "[run] closing"
[void][W2]::PostMessage($h, 0x10, [IntPtr]0, [IntPtr]0)
Start-Sleep -Seconds 2
$proc.Refresh()
if (-not $proc.HasExited) {
  # 可能有"工程已修改"确认框，回车确认后再关
  [void][W2]::keybd_event(0x0D, 0, 0, [IntPtr]::Zero)
  [void][W2]::keybd_event(0x0D, 0, 2, [IntPtr]::Zero)
  Start-Sleep -Seconds 2
  $proc.Refresh()
  if (-not $proc.HasExited) { $proc.Kill(); Write-Host "[run] killed" } else { Write-Host "[run] exit=$($proc.ExitCode)" }
} else { Write-Host "[run] exit=$($proc.ExitCode)" }
