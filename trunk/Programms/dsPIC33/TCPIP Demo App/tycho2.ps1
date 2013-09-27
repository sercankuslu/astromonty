$ty = Get-Content D:\Projects\Astromonty\Programms\guidance\tycho2.txt
#$cat = ((,,,),(,,,),(,,,))
$path5 = "D:\Projects\Astromonty\Programms\guidance\tycho2.js"
$path6 = "D:\Projects\Astromonty\Programms\guidance\tycho2_5_6.js"
$path7 = "D:\Projects\Astromonty\Programms\guidance\tycho2_6_7.js"
$path8 = "D:\Projects\Astromonty\Programms\guidance\tycho2_7_8.js"
$path9 = "D:\Projects\Astromonty\Programms\guidance\tycho2_8_9.js"
$ColumnName = "//_RAJ2000 |_DEJ2000  |BTmag | VTmag|TYC1|TYC2|TYC3| pmRA|   pmDE|HIP   |"
"var Tycho2 = [" > $path5
$ColumnName >> $path5
"var Tycho2_5_6 = [" > $path6
$ColumnName >> $path6
"var Tycho2_6_7 = [" > $path7
$ColumnName >> $path7
"var Tycho2_7_8 = [" > $path8
$ColumnName >> $path8
"var Tycho2_8_9 = [" > $path9
$ColumnName >> $path9
#       0        1    2    3    4    5    6     7     8   9       10       11
#_RAJ2000|_DEJ2000|TYC1|TYC2|TYC3|pmRA|pmDE|BTmag|VTmag|HIP|RA(ICRS)|DE(ICRS)

$v = 0
for($i = 0; $i -lt $ty.Count; $i++){
    $t = $ty[$i] -split "[|]"
    if(!(($t[7] -match "\d") -or ($t[8] -match "\d"))){
        continue
    }
    if($t[7] -notmatch "\d"){
        $mag = ([System.Double]$t[8])
    } elseif($t[8] -notmatch "\d"){
        $mag = ([System.Double]$t[7])
    } elseif(([System.Double]$t[7]) -le ([System.Double]$t[8])) {
        $mag = ([System.Double]$t[7])
    } elseif(([System.Double]$t[8]) -le ([System.Double]$t[7])) {
        $mag = ([System.Double]$t[8])        
    } else {
        continue
    }
    
    $str = "[" + $t[0] + "," + $t[1] + "," + $t[7] + "," + $t[8] + "," + $t[2] + "," + $t[3] + "," + $t[4] + "," + $t[5] + "," + $t[6] + "," + $t[9] + "],"    
    if($mag -le 5.0){
        $str >> $path5
        Write-Host "." -ForegroundColor White -NoNewline
    } elseif($mag -le 6.0){
        $str >> $path6
        Write-Host "." -ForegroundColor Blue -NoNewline
    } elseif($mag -le 7.0){
        $str >> $path7
        Write-Host "." -ForegroundColor Green -NoNewline
    } elseif($mag -le 8.0){
        $str >> $path8
        Write-Host "." -ForegroundColor Cyan -NoNewline
    } elseif($mag -le 9.0){
        $str >> $path9
        Write-Host "." -ForegroundColor Yellow -NoNewline
    }
    #if($mag -le 9.0){
    #    if($v % 100 -eq 0){
    #        Write-host $v
    #    }
    #    $v++
    #}
}
"]" >> $path5
"]" >> $path6
"]" >> $path7
"]" >> $path8
"]" >> $path9
#RA,DE,VTmag,BTmag,TYC1,TYC2,TYC3,pmRA,pmDE
#_RAJ2000  |_DEJ2000  |TYC1|TYC2|TYC3|pmRA |pmDE   |BTmag |VTmag 

