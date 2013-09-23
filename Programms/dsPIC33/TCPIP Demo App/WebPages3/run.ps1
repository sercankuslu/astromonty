$filename =  "C:\Apache24\htdocs\status.xml"
[xml]$status = Get-Content $filename
$angle0 = 90.0;
$angle1 = 90.0;
#0,00416666666666666666666666666667
$da0 = -0.0004465
$da1 = 0.0
while(1){
    
    for($i = 0; $i -lt 360; $i++){
        $angle0 += $da0
        if($angle0 -ge 360.0){
            $angle0 = 0.0
        }
        $angle1 += $da1
        if($angle1 -ge 360.0){
            $angle1 = 0.0
        }
        $status.response.ang0 = "$angle0"
        $status.response.ang1 = "$angle1"
        $status.Save($filename)
        "a: " + $status.response.ang0 + " d: " +  $status.response.ang1 | write-host
        start-sleep -Milliseconds 100
    }
}