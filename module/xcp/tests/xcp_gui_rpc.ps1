param([int]$AiPort=18799,[int]$SlavePort=5701,[string]$Transport="tcp")
$ErrorActionPreference="Stop"
$ws=New-Object System.Net.WebSockets.ClientWebSocket
$ws.ConnectAsync([Uri]"ws://127.0.0.1:$AiPort/",[Threading.CancellationToken]::None).Wait()
$script:id=0
function Rpc($m,$p){ $script:id++; $j="{""jsonrpc"":""2.0"",""id"":$($script:id),""method"":""$m"",""params"":$p}"
  $b=[Text.Encoding]::UTF8.GetBytes($j); $ws.SendAsync([ArraySegment[byte]]$b,'Text',$true,[Threading.CancellationToken]::None).Wait()
  while($true){ $buf=New-Object byte[] 65536; $ms=New-Object IO.MemoryStream
    do{ $r=$ws.ReceiveAsync([ArraySegment[byte]]$buf,[Threading.CancellationToken]::None).Result; $ms.Write($buf,0,$r.Count)}while(-not $r.EndOfMessage)
    $t=[Text.Encoding]::UTF8.GetString($ms.ToArray()); if($t -match """id"":$($script:id)[,}]"){ return $t } } }
$pass=0;$fail=0; function CK($c,$m,$x){ if($c){$script:pass++;"PASS $m"}else{$script:fail++;"FAIL $m :: $x"} }
$r=Rpc "xcp.status" "{}"; CK ($r -match '"loaded":1') "xcp.dll 已加载" $r
$r=Rpc "xcp.connect" "{""transport"":""$Transport"",""port"":""127.0.0.1:$SlavePort""}"; CK ($r -match '"connected":1') "connect $Transport" $r
$rnd=New-Object Random 7; $data=New-Object byte[] 2400; $rnd.NextBytes($data); $base=0x20001000
$sw=[Diagnostics.Stopwatch]::StartNew(); $ok=$true
for($o=0;$o -lt 2400;$o+=240){ $hex=-join ($data[$o..($o+239)]|%{$_.ToString("x2")}); $r=Rpc "xcp.write" ('{"address":"0x'+($base+$o).ToString("X")+'","data":"'+$hex+'"}'); if($r -notmatch '"ok":true'){$ok=$false;$r;break} }
CK $ok "写 2400 B（10×240）" ""
$back=""; for($o=0;$o -lt 2400;$o+=240){ $r=Rpc "xcp.read" ('{"address":"0x'+($base+$o).ToString("X")+'","size":240}'); if($r -match '"data":"([0-9a-f]*)"'){$back+=$matches[1]}else{$r;break} }
CK ($back -eq (-join ($data|%{$_.ToString("x2")}))) "读回 2400 B 一致 ($([int]$sw.ElapsedMilliseconds) ms)" $back.Length
$r=Rpc "xcp.disconnect" "{}"; CK ($r -match '"connected":0') "disconnect" $r
$r=Rpc "xcp.read" "{""address"":""0x20000000"",""size"":4}"; CK ($r -match 'error') "断开后读报错" $r
$r=Rpc "xcp.connect" "{""transport"":""$Transport"",""port"":""127.0.0.1:$SlavePort""}"; CK ($r -match '"connected":1') "重连" $r
$r=Rpc "xcp.read" ('{"address":"0x'+$base.ToString("X")+'","size":16}'); CK ($r -match ('"data":"'+(-join ($data[0..15]|%{$_.ToString("x2")})))) "重连后读回" $r
"RESULT $pass pass $fail fail"