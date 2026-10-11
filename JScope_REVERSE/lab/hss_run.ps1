param(
  [string]$Prj  = "D:\OpenScope\JScope_REVERSE\lab\hss_test.jscope",
  [string]$Out  = "D:\OpenScope\JScope_REVERSE\lab\hss_shot.png",
  [int]$WarmupMs = 6000,
  [int]$SampleMs = 10000,
  [int]$StartKey = 0x74,
  [switch]$NoStart
)

$ErrorActionPreference = "Continue"
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W {
  [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int c);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
}
"@

function Shot([IntPtr]$h, [string]$path) {
  $r = New-Object W+RECT
  $ok = [W]::GetWindowRect($h, [ref]$r)
  $w = $r.Right - $r.Left; $ht = $r.Bottom - $r.Top
  Write-Host "[shot] ok=$ok size=${w}x${ht} at $($r.Left),$($r.Top)"
  if ($w -le 0 -or $ht -le 0) { return }
  $bmp = New-Object System.Drawing.Bitmap -ArgumentList @($w, $ht)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size -ArgumentList @($w, $ht)))
  $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
  $g.Dispose(); $bmp.Dispose()
  Write-Host "[shot] saved $path"
}

$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
Write-Host "[run] launching with $Prj"
$proc = Start-Process -FilePath $exe -ArgumentList @("-openprj", $Prj) -PassThru
$h = [IntPtr]::Zero
for ($i = 0; $i -lt 30; $i++) {
  Start-Sleep -Milliseconds 500
  $proc.Refresh()
  if ($proc.HasExited) { Write-Host "[run] exited early: $($proc.ExitCode)"; exit 1 }
  if ($proc.MainWindowHandle -ne 0) { $h = $proc.MainWindowHandle; break }
}
Write-Host ("[run] hwnd=0x{0:x} title='{1}'" -f [int64]$h, $proc.MainWindowTitle)
if ($h -eq [IntPtr]::Zero) { exit 1 }

[void][W]::ShowWindow($h, 3)
[void][W]::SetForegroundWindow($h)
Start-Sleep -Milliseconds $WarmupMs
if ($proc.HasExited) { Write-Host "[run] exited during warmup: $($proc.ExitCode)"; exit 1 }

if (-not $NoStart) {
  Write-Host "[run] F5 (start sampling)"
  [void][W]::PostMessage($h, 0x100, [IntPtr]$StartKey, [IntPtr]0)
  Start-Sleep -Milliseconds 80
  [void][W]::PostMessage($h, 0x101, [IntPtr]$StartKey, [IntPtr]0)
}

Start-Sleep -Milliseconds $SampleMs
$proc.Refresh()
if ($proc.HasExited) {
  Write-Host "[run] J-Scope 在采集期间崩溃/退出: $($proc.ExitCode)  (0xC0000094 = 整数除零)"
} else {
  Shot $h $Out
}

Write-Host "[run] closing"
[void][W]::PostMessage($h, 0x10, [IntPtr]0, [IntPtr]0)
Start-Sleep -Seconds 3
$proc.Refresh()
if (-not $proc.HasExited) { $proc.Kill(); Write-Host "[run] killed" } else { Write-Host "[run] exit=$($proc.ExitCode)" }
