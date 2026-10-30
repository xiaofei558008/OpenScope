param([int]$AiPort=18799,[string]$B64,[int]$TimeoutMs=15000)
$Body=[Text.Encoding]::UTF8.GetString([Convert]::FromBase64String($B64))
$ws=New-Object System.Net.WebSockets.ClientWebSocket
$ws.ConnectAsync([Uri]"ws://127.0.0.1:$AiPort/",[Threading.CancellationToken]::None).Wait()
foreach($j in ($Body -split '\|\|')){ $b=[Text.Encoding]::UTF8.GetBytes($j); $sw=[Diagnostics.Stopwatch]::StartNew(); $ws.SendAsync([ArraySegment[byte]]$b,'Text',$true,[Threading.CancellationToken]::None).Wait()
 $id=([regex]'"id":(\d+)').Match($j).Groups[1].Value; $done=$false
 while(-not $done){ $buf=New-Object byte[] 65536; $t=$ws.ReceiveAsync([ArraySegment[byte]]$buf,[Threading.CancellationToken]::None)
   $left=$TimeoutMs-[int]$sw.ElapsedMilliseconds; if($left -le 0 -or -not $t.Wait($left)){ "TIMEOUT id=$id after $([int]$sw.ElapsedMilliseconds)ms"; exit 2 }
   $txt=[Text.Encoding]::UTF8.GetString($buf,0,$t.Result.Count); if($txt -match """id"":$id[,}]"){ "$([int]$sw.ElapsedMilliseconds)ms $txt"; $done=$true } } }