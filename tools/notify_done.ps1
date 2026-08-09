# OpenScope 全自动开发循环完成语音通知（request.md 需求 10）
#
# 用法：powershell -ExecutionPolicy Bypass -File tools\notify_done.ps1 [-Text "任务执行完毕"] [-Rate 1] [-Volume 100]
# 说明：
#   - 优先 System.Speech（.NET），并选择中文语音（Microsoft Huihui Desktop / zh-CN）播报；
#   - 若 System.Speech 不可用，回退 SAPI COM（SAPI.SpVoice）；
#   - 播报失败返回退出码 1（供自动循环判断），成功返回 0。
param(
    [string]$Text = "任务执行完毕",
    [int]$Rate = 1,        # -10..10，1=稍快
    [int]$Volume = 100     # 0..100
)

$ok = $false

# 1) System.Speech（.NET Framework，Windows 自带）
try {
    Add-Type -AssemblyName System.Speech -ErrorAction Stop
    $synth = New-Object System.Speech.Synthesis.SpeechSynthesizer
    try {
        # 优先选中文语音（Huihui Desktop 为简体中文），无则用默认
        $zh = $synth.GetInstalledVoices() | Where-Object {
            $_.Enabled -and $_.VoiceInfo.Culture.Name -like 'zh*'
        } | Select-Object -First 1
        if ($zh) { $synth.SelectVoice($zh.VoiceInfo.Name) }
        $synth.Rate = $Rate
        $synth.Volume = $Volume
        $synth.Speak($Text)
        $ok = $true
        Write-Output "[notify_done] System.Speech 播报完成: $Text"
    } finally {
        $synth.Dispose()
    }
} catch {
    Write-Output "[notify_done] System.Speech 不可用: $($_.Exception.Message) -> 尝试 SAPI"
}

# 2) SAPI COM 兜底
if (-not $ok) {
    try {
        $voice = New-Object -ComObject SAPI.SpVoice
        $voice.Rate = $Rate
        $voice.Volume = $Volume
        $voice.Speak($Text) | Out-Null
        $ok = $true
        Write-Output "[notify_done] SAPI 播报完成: $Text"
    } catch {
        Write-Output "[notify_done] 语音播报失败: $($_.Exception.Message)"
    }
}

if (-not $ok) { exit 1 }
exit 0
