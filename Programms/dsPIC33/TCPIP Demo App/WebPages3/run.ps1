$filename =  "C:\Apache24\htdocs\status.xml"
[xml]$status = Get-Content $filename
$angle0 = 90.0;
$angle1 = 90.0;

#0,00416666666666666666666666666667
#$da0 = -0.000417807934636275010445197530291
#        0.00041780746222949736255938206439
# 0.0002,7701935555555555555555555555556
#86164,090530833 за звездные сутки
#$da0 = -0.00044447
$da0 = -0.00417807934636275010445197530291 / 2
$da1 = 0.0

$speed = 0;
$angle1 = 0;
$LocalStarTime = 0;

# ¬ычисление звездного времени
function GM_Sidereal_Time ($jd) {
    $t_eph = 0;
    $ut = 0;
    $MJD0 = 0;
    $MJD = 0;

    $MJD = $jd - 2400000.5;
    $MJD0 = [Math]::floor($MJD);
    $ut = ($MJD - $MJD0)*24.0;
    $t_eph  = ($MJD0 - 51544.5)/36525.0;
    return  (6.697374558 + 1.0027379093*$ut + (8640184.812866 + (0.093104 - 0.0000062*$t_eph)*$t_eph)*$t_eph/3600.0);
}

function LM_Sidereal_Time ($jd, $longitude) {
    $GMST = GM_Sidereal_Time $jd 
    $LMST =  24.0 * (frac (($GMST + $longitude/15.0)/24.0));
    $LocalStarTime = $LMST;
    return $LocalStarTime #HoursMinutesSeconds($LMST);
}
function frac($X) {
    $X = $X - [Math]::floor($X);
    if ($X -lt 0) {
        $X = $X + 1.0;
    }
    return $X
}

function HoursMinutesSeconds($time) {

 $h = [Math]::floor($time);
 $min = [Math]::floor(60.0 * (frac  $time));
 $secs = [Math]::round(60.0*(60.0*(frac $time)-$min));
 
 if ($min -ge 10) {
    $str = "$h:$min";
 } else {
    $str = "$h:$min";
 }
 #if (min==60) str=(h+1)+":00";
 if ($secs -lt 10) {
    $str = $str + ":0"+$secs;
 } else {
    $str = $str + ":" + $secs;
 }
 return " " + $str;

}

function JulDay ($d, $m, $y, $u){
    if ($y -lt 1900) {
        $y = $y + 1900
    }
    if ( $m -le 2) {
        $m = $m + 12; 
        $y=$y-1
    }
    #if ( $m -le 2) {$m=$m+12; $y=$y-1}
    $JD =  [Math]::floor(365.25*($y+4716)) 
    $JD += [Math]::floor(30.6001*($m+1))
    $JD += $d - 13 - 1524.5 + $u/24.0;
    return $JD;
}


while(1){
    
    for($i = 0; $i -lt 360; $i++){
        $day = (date).ToUniversalTime()
        $UT = $day.Hour + ($day.Minute + ( $day.Second + $day.Millisecond/1000)/60)/60;
        
        $JD = JulDay $day.Day.ToString() $day.Month.ToString() $day.Year.ToString() $UT
        $LocalStarTime = LM_Sidereal_Time $JD 37.6028
        
        $speed = ($speed + $LocalStarTime*15 - $aa)/2;       
        if($speed > 0.005) {
            $speed = $LocalStarTime*15 - $aa;
        }
        $aa = $LocalStarTime*15 - $angle1;
        
        $da0 = -$speed

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
        "a: " + $status.response.ang0 + " d: " +  $status.response.ang1 + " speed: " + $da0 | write-host
        start-sleep -Milliseconds 100
    }
}
