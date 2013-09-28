#$cat = ((,,,),(,,,),(,,,))
$path5 = ".\tycho2.js"
$path6 = ".\tycho2_5_6.js"
$path7 = ".\tycho2_6_7.js"
$path8 = ".\tycho2_7_8.js"
$path9 = ".\tycho2_8_9.js"
#              //    0    |    1     | 2  |  3 |  4 |  5  |  6    |   7  |   8  |  9   |"
$ColumnName = "//_RAJ2000 |_DEJ2000  |TYC1|TYC2|TYC3| pmRA|   pmDE| BTmag| VTmag|   HIP|"
#               ----------|----------|----|-----|-|-------|-------|------|------|------
#$ColumnName = "//_RAJ2000 |_DEJ2000  |BTmag | VTmag|TYC1|TYC2|TYC3| pmRA|   pmDE|HIP   |"
function ProcessCatalog
{
	Param(
	[parameter(ValueFromPipeline=$true)]$ty
	)
	process {
		$v = 0
		if($ty -eq ''){
			return
		}
		$t = $ty -split "[|]"		
		if(!(($t[7] -match "\d") -or ($t[8] -match "\d"))){
			return
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
			return
		}
		$hip = $t[9]
		if($hip -eq ''){
			$hip = '      '
		}
		$str = "[" + $t[0] + "," + $t[1] + "," + $t[2] + "," + $t[3] + "," + $t[4] + "," + $t[5] + "," + $t[6] + "," + $t[7] + "," + $t[8] + "," + $hip + "],"    
		if($mag -le 5.0){
			$str >> $path5
			Write-Host "." -ForegroundColor White -NoNewline
		} elseif($mag -le 6.0){
			$str >> $path6
			Write-Host "." -ForegroundColor Yellow -NoNewline
		} elseif($mag -le 7.0){
			$str >> $path7
			Write-Host "." -ForegroundColor Cyan -NoNewline
		} elseif($mag -le 8.0){
			$str >> $path8
			Write-Host "." -ForegroundColor green -NoNewline
		} elseif($mag -le 9.0){
			$str >> $path9
			Write-Host "." -ForegroundColor blue -NoNewline
		}
		#if($mag -le 9.0){
		#    if($v % 100 -eq 0){
		#        Write-host $v
		#    }
		#    $v++
		#}
	}
}

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

Get-Content "F:\astromonty\Programms\dsPIC33\TCPIP Demo App\Tycho2.tsv"        -totalcount -1 | ProcessCatalog
Get-Content "F:\astromonty\Programms\dsPIC33\TCPIP Demo App\Tycho2_sup1.tsv"   -totalcount -1 | ProcessCatalog
Get-Content "F:\astromonty\Programms\dsPIC33\TCPIP Demo App\Tycho2_sup2.tsv"   -totalcount -1 | ProcessCatalog

"]" >> $path5
"]" >> $path6
"]" >> $path7
"]" >> $path8
"]" >> $path9
Write-Host "Complete" -ForegroundColor green
