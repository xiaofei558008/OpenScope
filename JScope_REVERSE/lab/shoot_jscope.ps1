param(
  [Parameter(Mandatory=$true)][string]$Prj,
  [string]$ExtraArgs = "",
  [int]$WarmupMs = 5000,
  [int]$SampleMs = 8000,
  [string]$Out = "D:\OpenScope\JScope_REVERSE\lab\shot.png",
  [switch]$NoStart,
  [switch]$KeepOpen
)

Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class Win32 {
  [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
}
"@

$exe = "C:\Program Files\SEGGER\JLink_V966\JScope.exe"
$argLine = "-openprj `"$Prj`" $ExtraArgs"
Write-Host "[lab] start: $exe $argLine"
$proc = Start-Process -FilePath $exe -ArgumentList $argLine -PassThru

$h = [IntPtr]::Zero
for ($i = 0; $i -lt 40; $i++) {
  Start-Sleep -Milliseconds 500
  $proc.Refresh()
  if ($proc.HasExited) { Write-Host "[lab] J-Scope 已退出 (exit=$($proc.ExitCode))"; exit 1 }
  if ($proc.MainWindowHandle -ne 0) { $h = $proc.MainWindowHandle; break }
}
if ($h -eq [IntPtr]::Zero) { Write-Host "[lab] 未拿到主窗口句柄"; exit 1 }
Write-Host ("[lab] window handle = 0x{0:x}" -f [int64]$h)

[void][Win32]::ShowWindow($h, 3)   # maximize
[void][Win32]::SetForegroundWindow($h)
Start-Sleep -Milliseconds $WarmupMs

function Send-Key([int]$vk) {
  [void][Win32]::PostMessage($h, 0x100, [IntPtr]$vk, [IntPtr]0)      # WM_KEYDOWN
  Start-Sleep -Milliseconds 60
  [void][Win32]::PostMessage($h, 0x101, [IntPtr]$vk, [IntPtr]0)      # WM_KEYUP
}

if (-not $NoStart) {
  Write-Host "[lab] 发送 F5 (开始采集)"
  Send-Key 0x74
}

Start-Sleep -Milliseconds $SampleMs

# 截图（窗口矩形）
$r = New-Object Win32+RECT
[void][Win32]::GetWindowRect($h, [ref]$r)
$w = $r.Right - $r.Left; $ht = $r.Bottom - $r.Top
Write-Host "[lab] capture ${w}x${ht} @ $($r.Left),$($r.Top) -> $Out"
$bmp = New-Object System.Drawing.Bitmap($w, $ht)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.CopyFromScreen($r.Left, $r.Top, 0, 0, (New-Object System.Drawing.Size($w, $ht)))
$bmp.Save($Out, [System.Drawing.Imaging.ImageFormat]::Png)
$g.Dispose(); $bmp.Dispose()

if (-not $KeepOpen) {
  Write-Host "[lab] 关闭 J-Scope"
  [void][Win32]::PostMessage($h, 0x10, [IntPtr]0, [IntPtr]0)          # WM_CLOSE
  Start-Sleep -Seconds 3
  $proc.Refresh()
  if (-not $proc.HasExited) { $proc.Kill(); Write-Host "[lab] 强制结束" }
  else { Write-Host "[lab] 已退出 exit=$($proc.ExitCode)" }
} else {
  Write-Host "[lab] 保持窗口打开 (pid=$($proc.Id))"
}
