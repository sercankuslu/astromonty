
var CoordX = 84.053;
var CoordY = -1.201;
var Scale = 10; // увеличить в 10 раз
var Magnitude = 6; // величина звезд
var WSizeX = 0;
var WSizeY = 0;
var WSizeXd2 = 0;
var WSizeYd2 = 0;
var timerId;
var PI2 = Math.PI*2;
var gradToRad = Math.PI/180;
var radToGrad = 180/Math.PI;
var LocalStarTime = 0;
var ViewXr;
var ViewYr;
var DrawingStar = 0;
var Follow = true;
var Loading = false;
var ServerDate = 0;
var GeoPosition = {
    Lon : 37.6028,
    //Lon : 217.6028,
    Lat : 55.791437,
}
// текущие координаты телескопа(зелёный крестик)
var CurrentPosition = {
    X : 101.287155,
    Y : -16.716116,
};
var MousePosition = {
    X : 0,
    Y : 0,
};

var MousePositionStar = {
    X : 0,
    Y : 0,
};
var AbsolutePosition = {
    a : 0,
    d : 0,
    visible : false
};
function filterStars(element,index,array){
    var BTmag = element[7];
    var VTmag = element[8];
    if(BTmag === undefined) BTmag = VTmag;
    if(VTmag === undefined) VTmag = BTmag;
    if(((BTmag < Magnitude)||(VTmag < Magnitude)) && ViewPosition.inRadius(element[0]*gradToRad,element[1]*gradToRad)) return true;
    return false;
};

function enableHM(){
    return true;
};

// текущее положение просмотра
var ViewPosition = {
    X : 84.053,
    Y : -1.201,
    X1 : 0,
    Y1 : 0,
    Scale1 : 0,
    Magn1 : 0,
    ViewXr : 0,
    ViewYr : 0,
    SinViewXr: 0,
    CosViewXr: 0,
    SinViewYr: 0,
    CosViewYr: 0,
    Catalog:[],
    SetPosition:function(X,Y){
        this.X = X;
        this.Y = Y;
        if(this.X >=  360) this.X -= 360;
        if(this.X <= -360) this.X += 360;
        this.ViewXr = X*gradToRad;
        this.ViewYr = Y*gradToRad;
        this.SinViewXr = Math.sin(this.ViewXr);
        this.CosViewXr = Math.cos(this.ViewXr);
        this.SinViewYr = Math.sin(this.ViewYr);
        this.CosViewYr = Math.cos(this.ViewYr);
        if((Magnitude != this.Magn1)||(Math.abs(Scale - this.Scale1) > Scale)||(Math.abs(this.X1-this.X)>150/Scale) || (Math.abs(this.Y1-this.Y)>150/Scale)){
            this.Catalog = Tycho2.filter(filterStars);
            this.X1 = this.X;
            this.Y1 = this.Y;
            this.Scale1 = Scale;
            this.Magn1 = Magnitude;
        }
    },
    inRadius:function(RAr,DEr){
        var L = Math.acos(this.SinViewYr*Math.sin(DEr)+this.CosViewYr*Math.cos(DEr)*Math.cos(this.ViewXr-RAr));
        if(isNaN(L)){
            return true;
        }
        if(L < 2*Math.PI/Scale) {
            return true;
        }else{
            return false;
        }
    },
    polarToDecart:function(a,d){
        var DEr = d*gradToRad;
        var RAr = a*gradToRad;
        var D = this.ToDecart( RAr, DEr,100);
        return this.Rotate(D);
    },
    decartToPolar:function(D){
        var A;
        var d = Math.asin(D.Z/100);
        if(D.X>0){
            if(D.Y>=0){
                A = Math.atan(D.Y/D.X);
            }else{
                A = Math.atan(D.Y/D.X) + PI2;
            }
        } else if(D.X<0){
            A = Math.atan(D.Y/D.X)+Math.PI;
        } else {
            if(D.Y>0) {
                A = Math.PI/2;
            } else
            if(D.Y<0) {
                A = Math.PI*3/2;
            } else
                A = 0;
        }
        A = A-Math.PI;
        if(A<0) A+=PI2;
        return {a:A,d:d};
    },
    Rotate:function(D){
        var D1 = this.rotateDecartZ(D);
        D1 = this.rotateDecartX(D1);
        return {X:(WSizeXd2+D1.X)*Scale,Y:D1.Y,Z:(WSizeYd2-D1.Z)*Scale,V:D1.Y>0};
    },
    unRotate:function(D){
        var X = D.X/Scale-WSizeXd2;
        var Z = -D.Z/Scale+WSizeYd2;
        var R = 100*Math.cos(Math.asin(-Z/100));
        var A = Math.acos(X/R);
        var Y = R*Math.sin(A);
        //if(Y<0) Y = -Y;
        D2 = {X:X,Y:Y,Z:Z};
        var D1 = this.rotateDecartMX(D2);
        D2 = this.rotateDecartMZ(D1);
        return {X:D2.X,Y:D2.Y,Z:D2.Z};
    },
    ToDecart:function(a,d,S){
        var CosD = Math.cos(d);
        var x = S*CosD*Math.cos(a);
        var y = S*CosD*Math.sin(a);
        var z = S*Math.sin(d);
        return {X:x,Y:y,Z:z};
    },
    rotateDecartZ:function(D){
        return {
            X:D.X*this.SinViewXr - D.Y*this.CosViewXr,
            Y:D.X*this.CosViewXr + D.Y*this.SinViewXr,
            Z:D.Z
        };
    },
    rotateDecartX:function(D){
        return {
            X :  D.X,
            Y :  D.Y*this.CosViewYr + D.Z*this.SinViewYr,
            Z :  -D.Y*this.SinViewYr + D.Z*this.CosViewYr,
        };
    },
    rotateDecartMZ:function(D){
        return {
            X: -D.X*this.SinViewXr - D.Y*this.CosViewXr,
            Y:  D.X*this.CosViewXr - D.Y*this.SinViewXr,
            Z:  D.Z
        };
    },
    rotateDecartMX:function(D){
        return {
            X :  D.X,
            Y :  D.Y*this.CosViewYr - D.Z*this.SinViewYr,
            Z :  D.Y*this.SinViewYr + D.Z*this.CosViewYr,
        };
    }
};
// целевые координаты телескопа(желтый крестик)
var TargetPosition = {
    X : 87.053,
    Y : 4.201,
    SetPosition:function(X,Y){
        this.X = X;
        this.Y = Y;
        if(this.X >=  360) this.X -= 360;
        if(this.X <= -360) this.X += 360;
    }
};
var StarView;

function onMouseDown(e){
    this.Drag = true;
    this.Select = true;
    e = e || window.event;
    var canvas = document.getElementById('my_canvas');
    var pos = getPosition(canvas);
    if (e.pageX == null && e.clientX != null ) {
        var html = document.documentElement;
        var body = document.body;
        var canvas = document.getElementById('my_canvas');

        e.pageX = e.clientX + (html && html.scrollLeft || body && body.scrollLeft || 0) - (html.clientLeft || 0);
        e.pageY = e.clientY + (html && html.scrollTop || body && body.scrollTop || 0) - (html.clientTop || 0);
    }

    MousePosition.X = e.pageX - pos.x-1;
    MousePosition.Y = e.pageY - pos.y-1;
}
function onMouseUp(e){
    this.Drag = false;
}

function onMouseWheel(e) {
    e = e || event;

    if (!e.wheelDelta) {
        e.wheelDelta = -40*e.detail; // Firefox
    }
    var info = document.getElementById('delta');
    Scale += Math.floor(e.wheelDelta/120 + e.wheelDelta*Scale/1000);
    
    if(Scale < 3) {Scale = 3;};
    if(Scale > 1000) {Scale = 1000;};
    e.preventDefault ? e.preventDefault() : (e.returnValue = false);
    updateScale(3);
    if(!DrawingStar) StarView.updateStars();
}


/* use a function for the exact format desired... */
function ISODateString(d,UTC){
    function pad(n){return n<10 ? '0'+n : n}
    if(UTC){
        return d.getUTCFullYear()+'-'
              + pad(d.getUTCMonth()+1)+'-'
              + pad(d.getUTCDate())+' '
              + pad(d.getUTCHours())+':'
              + pad(d.getUTCMinutes())+':'
              + pad(d.getUTCSeconds());
    } else {
        return d.getFullYear()+'-'
              + pad(d.getMonth()+1)+'-'
              + pad(d.getDate())+' '
              + pad(d.getHours())+':'
              + pad(d.getMinutes())+':'
              + pad(d.getSeconds());
    }
}
var speed = 0.0;
var angle1 = 0.0;
//LocalStarTime*15,GeoPosition.Lat
function UpdateTime(){
    var now = new Date(ServerDate);
    document.getElementById('UTCTime').value = ISODateString(now,true);
    //document.getElementById('EqEq').value = T.toFixed(2);

    var utDay = now.getUTCDate();
    var utMonth = now.getUTCMonth()+1;
    var utYear = now.getUTCFullYear();
    var utHours = now.getUTCHours();
    var utMinutes = now.getUTCMinutes();
    var utSeconds = now.getUTCSeconds();
    var utmSeconds = now.getUTCMilliseconds();
    if (utYear<1900) utYear=utYear+1900;

    UT = utHours + (utMinutes + ( utSeconds + utmSeconds/1000)/60)/60;
    
    var JD = JulDay (utDay, utMonth, utYear, UT);
    var LST = LM_Sidereal_Time(JD, GeoPosition.Lon);
    
    //speed = (speed + LocalStarTime*15 - angle1)/2;       
    //if(speed > 0.005)
    //    speed = LocalStarTime*15 - angle1;
    //angle1 = LocalStarTime*15;
        
    //document.getElementById('Speed').value = speed;
    
    document.getElementById('LST').value =  LST;
    if(Follow){
        ViewPosition.SetPosition(CurrentPosition.X,CurrentPosition.Y);
    } else {
        ViewPosition.SetPosition(ViewPosition.X,ViewPosition.Y);
    }
    updateTargetCrosPos();
    if(!DrawingStar) StarView.updateStars();
}
function enableFollow(){
    Follow = document.getElementById('followbox').checked;
}
function loadCanvas() {
    onResize();
    document.getElementById('gotobutton').disabled = true;

    //document.getElementById('my_canvas').width = document.getElementById('page').clientWidth - 500;
    StarView = document.getElementById('my_canvas');
    StarView.onmousedown = onMouseDown;
    StarView.updateStars = updateStars;
    StarView.drawStars = drawStars;
    StarView.DrawCross = DrawCross;
    StarView.onmousemove = mousemoveCanv;
    StarView.onclick = targetSelect;
    StarView.onmouseup = onMouseUp;
    StarView.updateCross = updateCross;
    var elem = document.getElementById('my_canvas');//('CanvasBox');
    document.getElementById('starScale').value = Scale;
    document.getElementById('starScaleN').value = Scale;
    document.getElementById('starMagn').value = Magnitude;
    document.getElementById('starMagnN').value = Magnitude;
    elem.ondragstart = function() {
        return false;
    };
    ViewPosition.SetPosition(CoordX,CoordY);
    //if (elem.addEventListener) {
      // IE9+, Opera, Chrome/Safari
        //elem.addEventListener ("mousewheel", onMouseWheel, false);
      // Firefox
        //elem.addEventListener ("DOMMouseScroll", onMouseWheel, false);
    //} else { // IE<9
    //  elem.attachEvent ("onmousewheel", onMouseWheel);
   // }
    elem.onmousewheel = onMouseWheel;
    document.body.onresize = onResize;
    var timer = setInterval(UpdateTime, 1000);
    StarView.updateStars();
    updateTargetForm();
    //загрузка и коррекция координат каталогаs
    correctCoordinate(Tycho2);
    setTimeout(LoadData,200);
};
function onResize(){
    document.getElementById('shadow-one').style.width = document.body.clientWidth * 0.95 + "px";
    document.getElementById('shadow-one').style.height = document.body.clientHeight * 0.95 + "px";
    WSizeX = document.getElementById('page').clientWidth;
    WSizeY = document.getElementById('page').clientHeight;
    document.getElementById('my_canvas').width =  (document.getElementById('page').clientWidth - 430);
    document.getElementById('my_canvas').height = document.getElementById('my_canvas').width ;//   document.getElementById('page').clientHeight * 0.7; //
    WSizeY = document.getElementById('my_canvas').clientHeight;
    WSizeX = document.getElementById('my_canvas').clientWidth;
    
    WSizeXd2 = WSizeX/(2*Scale);
    WSizeYd2 = WSizeY/(2*Scale);
};
var NextCat=0;
function LoadData(){
    try{
        switch(NextCat){
        case 0: loadScript("stars/Tycho2_5_6.js", function(){scriptCallback(Tycho2_5_6);}, 0 );
            break;
        case 1: loadScript("stars/Tycho2_6_7.js", function(){scriptCallback(Tycho2_6_7);}, 0 );
            break;
        case 2: loadScript("stars/Tycho2_7_8.js", function(){scriptCallback(Tycho2_7_8);}, 0 );
            break;
        case 3: loadScript("stars/Tycho2_8_9.js", function(){scriptCallback(Tycho2_8_9);}, 0 );
            break;
        //case 4:    loadScript("stars/Tycho2_73.js", function(){scriptCallback(Tycho2_73);}, 0 );
            // break;
        // case 5:    loadScript("stars/Tycho2_81.js", function(){scriptCallback(Tycho2_81);}, 0 );
            // break;
        // case 6:    loadScript("stars/Tycho2_82.js", function(){scriptCallback(Tycho2_82);}, 0 );
            // break;
        // case 7:    loadScript("stars/Tycho2_83.js", function(){scriptCallback(Tycho2_83);}, 0 );
            // break;
        // case 8:    loadScript("stars/Tycho2_84.js", function(){scriptCallback(Tycho2_84);}, 0 );
            // break;
        // case 9:    loadScript("stars/Tycho2_85.js", function(){scriptCallback(Tycho2_85);}, 0 );
            // break;
        // case 10: loadScript("stars/Tycho2_86.js", function(){scriptCallback(Tycho2_86);}, 0 );
            // break;
        // case 11: loadScript("stars/Tycho2_87.js", function(){scriptCallback(Tycho2_87);}, 0 );
            // break;
        // case 12: loadScript("stars/Tycho2_88.js", function(){scriptCallback(Tycho2_88);}, 0 );
            // break;
        default : document.getElementById('gotobutton').disabled = false;
                  Loading = false;
                  document.getElementById('progressbar').style.display = 'none';
        }
    } catch (e) {
        document.getElementById('gotobutton').disabled = false;
        Loading = false;
    }
    NextCat++;
}
function scriptCallback(Catalog){
    correctCoordinate(Catalog);
    Tycho2 = Tycho2.concat(Catalog);
    if(!DrawingStar) StarView.updateStars();
    setTimeout(LoadData,10);
}
function AngleToObj(A,h){
    var tA = Math.abs(A);
    if(h){
        tA = tA/15;
    }
    var N = (A>=0)?1:-1;
    var grad = Math.floor(tA);
    tA = (tA - grad)*60;
    var min = Math.floor(tA);
    var sec = ((tA - min)*60);
    //grad = grad * N;
    return {Sign:N,H:grad,M:min,S:sec};
};
function objToAngle(O,h){
    var tA = 0;
    tA = O.M + O.S/60;
    var H = Math.abs(O.H);
    tA = H + tA/60;

    if(h) return tA*15*O.Sign;
    return tA*O.Sign;
};
function AngleToString(A,h){
    var s = "° ";
    if(h){
        s = "h "
    }
    var O = AngleToObj(A,h);
    var sign = (O.Sign>0)?"+":"-";
    return ""+ sign + O.H + s + O.M + "' " + O.S.toFixed(2) + '"';
};
function updateScale(id){
    var range = document.getElementById('starScale');
    var rangeN = document.getElementById('starScaleN');
    if((range.value == rangeN.value) && (range.value == Scale)) return;
    switch (id){
    case 1:
        Scale = +range.value;
        rangeN.value = +range.value;
        break;
    case 2:
        Scale = +rangeN.value;
        range.value = +rangeN.value;
        break;
    case 3:
        rangeN.value = +Scale;
        range.value = +Scale;
        break;
    }
    if(Scale < 20 && Magnitude>=8) {
        Magnitude--;
        updateMagn(3);
    } else
    if(Scale < 10 && Magnitude>=7) {
        Magnitude--;
        updateMagn(3);
    } else
    if(Scale > 10 && Magnitude <7) {
        Magnitude++;
        updateMagn(3);
    } else
    if(Scale > 20 && Magnitude <9) {
        Magnitude++;
        updateMagn(3);
    }
    ViewPosition.SetPosition(ViewPosition.X,ViewPosition.Y);
    if(!DrawingStar) StarView.updateStars();
}
function updateMagn(id){
    var Magn = document.getElementById('starMagn');
    var MagnN = document.getElementById('starMagnN');
    if(id!=3 && Magn.value == MagnN.value) return;
    if(id==1){
        Magnitude = Magn.value;
        MagnN.value = Magn.value;
    } else if(id == 2){
        Magnitude = MagnN.value;
        Magn.value = MagnN.value;
    } else {
        Magn.value= Magnitude;
        MagnN.value= Magnitude;
    }
    if(id!=3){
        ViewPosition.SetPosition(ViewPosition.X,ViewPosition.Y);
        if(!DrawingStar) StarView.updateStars();
    }
}
//***************************************************************************************** drawStars
function drawStars(Catalog){
    var canvas = this;
    if(canvas.getContext) {
        var ctx = canvas.getContext('2d');
        WSizeX = canvas.width;
        WSizeY = canvas.height;
        WSizeXd2 = canvas.width/(2*Scale);
        WSizeYd2 = canvas.height/(2*Scale);
        var i = 0;

        var maxMag = -5;
        var ArrsLength = Catalog.length;
        var elem = {RA:0,DE:0,mag:0};
        var Starsize;
        DrawNetwork(ctx,"#005500");
        // Звёзды
        //if(0)
        for ( i = 0;i < ArrsLength; i++) {
            elem.RA = Catalog[i][0];
            elem.DE = Catalog[i][1];
            elem.BTmag = Catalog[i][7];
            elem.VTmag = Catalog[i][8];
            if(elem.BTmag === undefined) elem.BTmag = elem.VTmag;
            if(elem.VTmag === undefined) elem.VTmag = elem.BTmag;
            
            elem.mag = (elem.BTmag >= elem.VTmag)?elem.VTmag:elem.BTmag;
            if(elem.mag > Magnitude) break;
            if(elem.mag < maxMag) continue;
            var D = ViewPosition.polarToDecart(elem.RA,elem.DE);
            if(D.V){
                ctx.beginPath();
                var R = (9 - elem.mag);//*((Scale/3));
                var BTR = (9 - elem.BTmag);
                var VTR = (9 - elem.VTmag);
                Starsize = (R * 3/10) + (Scale/50);
                var r = Math.floor((R) * 15 + 100 + (Scale/10)) ;
                var g = Math.floor(VTR * 15 + 100 + (Scale/10)) ;
                var b = Math.floor(BTR * 15 + 100 + (Scale/10)) ;
                var color = 'rgb(' + r + ',' + g + ',' + b + ');';
                ctx.fillStyle = color;
                if(ctx.fillStyle == "#000000") ctx.fillStyle = "#FFFFFF";
                ctx.strokeStyle = color;
                ctx.arc(D.X,D.Z, Starsize,0,PI2,true);
                ctx.fill();
            }
        };

    } else {
        document.getElementById('my_canvas').style.display = 'none';
        document.getElementById('no-canvas').style.display = 'block';
    }
    DrawingStar = 0;
}
// рисует сетку кривыми "#003300"
function DrawNetwork(ctx,color)
{
    var N = CalculateNet();
    var i =0;
    var j = 0;
    var D1;
    var D2;
    var D3;
    var D4;
    ctx.strokeStyle = color;
    for(j=0;j<19;j++){
        for(i=0;i<24;i++){
            N[j][i].D = ViewPosition.Rotate(N[j][i]);
        }
    }
    for(j=0;j<18;j++){
        for(i=0;i<24;i++){
            if(N[j][i].D.V){
            ctx.beginPath();
            if(i<23){ // горизонтальные
                ctx.moveTo(N[j][i].D.X,N[j][i].D.Z);
                ctx.lineTo(N[j][i+1].D.X,N[j][i+1].D.Z);
            } else {
                ctx.moveTo(N[j][i].D.X,N[j][i].D.Z);
                ctx.lineTo(N[j][0].D.X,N[j][0].D.Z);
            }
                ctx.stroke();
                ctx.beginPath();
                // вертикальные
                //if(j<18){
                    ctx.moveTo(N[j][i].D.X,N[j][i].D.Z);
                    ctx.lineTo(N[j+1][i].D.X,N[j+1][i].D.Z);
                //}
                ctx.stroke();
            }
        }
    }
};
function drawNetCurve(ctx,NetK,color){
    if(1){
        ctx.beginPath();
        var D1 = ViewPosition.Rotate(NetK[0]);
        ctx.moveTo(D1.X,D1.Y);
        D2 = ViewPosition.Rotate(NetK[1]);
        D3 = ViewPosition.Rotate(NetK[2]);
        D4 = ViewPosition.Rotate(NetK[3]);
        ctx.bezierCurveTo(D2.X, D2.Y, D3.X, D3.Y, D4.X, D4.Y);
        ctx.strokeStyle = color;
        ctx.stroke();
    }else{
        ctx.beginPath();
        var D1 = ViewPosition.Rotate(NetK[0]);
        ctx.moveTo(D1.X,D1.Y);
        D2 = ViewPosition.Rotate(NetK[1]);
        D3 = ViewPosition.Rotate(NetK[2]);
        ctx.quadraticCurveTo(D2.X, D2.Y, D3.X, D3.Y);
        ctx.strokeStyle = color;
        ctx.stroke();
    }
}

function drawVisibleCircle(ctx,X,Y){
    ctx.beginPath();
    ctx.strokeStyle = "cyan";
    ctx.arc(X,Y, 90*Scale,0,PI2,true);
    ctx.stroke();
    ctx.beginPath();
    ctx.moveTo(X - 10, Y);
    ctx.lineTo(X + 10, Y);
    ctx.stroke();
    ctx.moveTo(X, Y - 10);
    ctx.lineTo(X, Y + 10);
    ctx.stroke();
}
function updateStars() {
    DrawingStar = 1;
    var canvas = document.getElementById('my_canvas');
    if(canvas.getContext) {
        var ctx = canvas.getContext('2d');
        ctx.clearRect(0, 0, WSizeX, WSizeY);
        if(Tycho2) {
            StarView.drawStars(ViewPosition.Catalog);
            //StarView.drawStars(Test);
            //if(document.getElementById('progressbar').style.display == 'block'){
                if(document.getElementById('out_starCount'))
                    document.getElementById('out_starCount').innerHTML = Tycho2.length + " звёзд.";
                if(document.getElementById('StarCounter')){
                    document.getElementById('StarCounter').innerHTML = Math.floor(100*(Tycho2.length)/(120552)) + '%';
                    document.getElementById('StarCounter').style.width = 100*(Tycho2.length)/(120552) + '%';
                }
            //}
        }
        StarView.updateCross();
    }
    DrawingStar = 0;
};

function updateCross(){
    var canvas = this;
    if(canvas.getContext) {
        this.DrawCross(CurrentPosition.X,CurrentPosition.Y,"green",true);
        this.DrawCross(TargetPosition.X,TargetPosition.Y,"yellow",false);
        this.DrawCross(LocalStarTime*15,GeoPosition.Lat,"cyan",false);
    }
}

function DrawCross(X,Y,color,wCircle){
    var canvas = this;
    if(canvas.getContext) {
        if(ViewPosition.inRadius(X*gradToRad,Y*gradToRad)){
            var Coord = ViewPosition.polarToDecart(X,Y);
            if(Coord.V){
                var ctx = canvas.getContext('2d');
                ctx.beginPath();
                ctx.strokeStyle  = color;
                if(wCircle){
                    ctx.arc(Coord.X, Coord.Z,30,0,Math.PI*2,true);
                    ctx.stroke();
                }
                ctx.moveTo(Coord.X + 30, Coord.Z);
                ctx.lineTo(Coord.X + 5, Coord.Z);
                ctx.stroke();
                ctx.moveTo(Coord.X - 5, Coord.Z);
                ctx.lineTo(Coord.X - 30, Coord.Z);
                ctx.stroke();
                ctx.moveTo(Coord.X, Coord.Z - 30);
                ctx.lineTo(Coord.X, Coord.Z - 5);
                ctx.stroke();
                ctx.moveTo(Coord.X, Coord.Z + 5);
                ctx.lineTo(Coord.X, Coord.Z + 30);
                ctx.stroke();
            }
        }
    }
}
var OldMouse = {
    X:0,
    Y:0,
}
function mousemoveCanv(e){
    clearTimeout(timerId);
    e = e || window.event;
    if(e.type != 'mousemove') return;
    var canvas = document.getElementById('my_canvas');
    var pos = getPosition(canvas);
    if (e.pageX == null && e.clientX != null ) {
        var html = document.documentElement;
        var body = document.body;
        var canvas = document.getElementById('my_canvas');

        e.pageX = e.clientX + (html && html.scrollLeft || body && body.scrollLeft || 0) - (html.clientLeft || 0);
        e.pageY = e.clientY + (html && html.scrollTop || body && body.scrollTop || 0) - (html.clientTop || 0);
    }
    if((e.pageX == OldMouse.X)&&(e.pageY == OldMouse.Y)) return;
    OldMouse.X = e.pageX;
    OldMouse.Y = e.pageY;
    if(this.Drag) this.Select = false;
    if(!this.Drag){
        var D = {X:e.pageX - pos.x-1,Y:0,Z:e.pageY - pos.y-1};
        var D1 = ViewPosition.unRotate(D);
        var P = ViewPosition.decartToPolar(D1);
        MousePositionStar.X = P.a * radToGrad;
        MousePositionStar.Y = P.d * radToGrad;
        document.getElementById('outXt').value = AngleToString(MousePositionStar.X,true);
        document.getElementById('outYt').value = AngleToString(MousePositionStar.Y,false);      
        
    } else {
        var dX = (e.pageX - pos.x -1) - MousePosition.X;
        var dY = (e.pageY - pos.y -1) - MousePosition.Y;
        if(Math.abs(ViewPosition.Y) >80) dX *= Scale/10;
        //dX *= 1 + (1 - Math.cos(Math.abs(ViewPosition.Y)*gradToRad)) * Scale/100;
        ViewPosition.SetPosition(ViewPosition.X+(dX/Scale)*(1+Math.sin(Math.abs(ViewPosition.Y*gradToRad))),ViewPosition.Y+dY/Scale);
        if(!DrawingStar) this.updateStars();
        MousePosition.X = e.pageX - pos.x -1;
        MousePosition.Y = e.pageY - pos.y -1;
        document.getElementById('followbox').checked = false;
        Follow = false;
    }
    // спрятать информацию о звезде, если двинули мышкой
    if(document.getElementById('StarInfo'))
        document.getElementById('StarInfo').style.display = 'none';
    timerId = setTimeout(ShowStarNumber,100);
}
//поиск в каталоге имени звезды
function SelectStars(element,index,array){
    var length = 2.0/(Scale);
    // на Scale == 1, 0.83 градуса
    if(   (element[0] >= MousePositionStar.X - length)
        &&(element[0] <= MousePositionStar.X + length)
        &&(element[1] >= MousePositionStar.Y - length)
        &&(element[1] <= MousePositionStar.Y + length)){
        return true;
    }
}
// Вывод в подсказку TYC номера звезды
function ShowStarNumber(){
    var TmpCat;
    if(Tycho2) {
        TmpCat = ViewPosition.Catalog.filter(SelectStars);
        if(TmpCat[0]){
            if(document.getElementById('StarInfo'))
                document.getElementById('StarInfo').style.display = 'block';
            if(document.getElementById('outTYC'))
                document.getElementById('outTYC').value = TmpCat[0][2] + "-" + TmpCat[0][3] + "-" + TmpCat[0][4];
            if(document.getElementById('outHIP')){
                if((TmpCat[0][9] === undefined)||(TmpCat[0][9] == '')) {
                    document.getElementById('loutHIP').style.display = 'none';
                } else {
                    document.getElementById('loutHIP').style.display = 'block';
                    document.getElementById('outHIP').value = TmpCat[0][9];
                }          
            }
            if(document.getElementById('outSa'))
                document.getElementById('outSa').value = AngleToString(TmpCat[0][0],true);
            if(document.getElementById('outSg'))
                document.getElementById('outSg').value = AngleToString(TmpCat[0][1],false);
            if(document.getElementById('outBTmag')){
                if(TmpCat[0][7] === undefined){
                    document.getElementById('loutBTmag').style.display = 'none';
                } else {
                    document.getElementById('loutBTmag').style.display = 'block';
                    document.getElementById('outBTmag').value = TmpCat[0][7];
                }               
            }
            if(document.getElementById('outVTmag')){
                if(TmpCat[0][8] === undefined){
                    document.getElementById('loutVTmag').style.display = 'none';
                } else {
                    document.getElementById('loutVTmag').style.display = 'block';
                    document.getElementById('outVTmag').value = TmpCat[0][8];
                }               
            }
            if(document.getElementById('outName')){
                document.getElementById('outName').value = '';                
                document.getElementById('loutName').style.display = 'none';
            }
            for(var i = 0; i< StarName.length; i++){
                if(document.getElementById('outTYC')){
                    if(StarName[i].TYC == document.getElementById('outTYC').value){
                        if(document.getElementById('outName')){                        
                            document.getElementById('loutName').style.display = 'block';                        
                            document.getElementById('outName').value = StarName[i].Name;
                        }
                        break;
                    }
                }
            }
        } else {
            if(document.getElementById('StarInfo')) document.getElementById('StarInfo').style.display = 'none';
        }
    }
}
function ShowTargetStarInfo(){
    var TmpCat;
    if(Tycho2) {
        TmpCat = ViewPosition.Catalog.filter(SelectStars);
        if(TmpCat[0]){
            if(document.getElementById('info_Star'))
                document.getElementById('info_Star').style.display = 'block';
            if(document.getElementById('info_outTYC'))
                document.getElementById('info_outTYC').value = TmpCat[0][2] + "-" + TmpCat[0][3] + "-" + TmpCat[0][4];
            if(document.getElementById('info_outHIP')){
                if((TmpCat[0][9] === undefined)||(TmpCat[0][9] == '')) {
                    document.getElementById('info_loutHIP').style.display = 'none';
                } else {
                    document.getElementById('info_loutHIP').style.display = 'block';
                    document.getElementById('info_outHIP').value = TmpCat[0][9];
                }        
            }
            if(document.getElementById('info_outSa'))
                document.getElementById('info_outSa').value = AngleToString(TmpCat[0][0],true);
            if(document.getElementById('info_outSg'))
                document.getElementById('info_outSg').value = AngleToString(TmpCat[0][1],false);
            if(document.getElementById('info_outBTmag')){
                if(TmpCat[0][7] === undefined){
                    document.getElementById('info_loutBTmag').style.display = 'none';
                } else {
                    document.getElementById('info_loutBTmag').style.display = 'block';
                    document.getElementById('info_outBTmag').value = TmpCat[0][7];
                }               
            }
            if(document.getElementById('info_outVTmag')){
                if(TmpCat[0][8] === undefined){
                    document.getElementById('info_loutVTmag').style.display = 'none';
                } else {
                    document.getElementById('info_loutVTmag').style.display = 'block';
                    document.getElementById('info_outVTmag').value = TmpCat[0][8];
                }               
            }
            if(document.getElementById('info_outName')){
                document.getElementById('info_outName').value = '';                
                document.getElementById('info_loutName').style.display = 'none';
            }
            for(var i = 0; i< StarName.length; i++){
                if(document.getElementById('info_outTYC')){
                    if(StarName[i].TYC == document.getElementById('info_outTYC').value){
                        if(document.getElementById('info_outName')){                        
                            document.getElementById('info_loutName').style.display = 'block';                        
                            document.getElementById('info_outName').value = StarName[i].Name;
                        }
                        break;
                    }
                }
            }
        } else {
            if(document.getElementById('info_Star')) document.getElementById('info_Star').style.display = 'none';
        }
    }
}
function getPosition(e){
    var left = 0;
    var top  = 0;

    while (e.offsetParent){
        left += e.offsetLeft;
        top  += e.offsetTop;
        e     = e.offsetParent;
    };

    left += e.offsetLeft;
    top  += e.offsetTop;

    return {x:left, y:top};
}
function targetSelect(e){
    if(!this.Select) return;
    e = e || window.event;
    var canvas = document.getElementById('my_canvas');
    var pos = getPosition(canvas);
    if (e.pageX == null && e.clientX != null ) {
        var html = document.documentElement;
        var body = document.body;
        var canvas = document.getElementById('my_canvas');

        e.pageX = e.clientX + (html && html.scrollLeft || body && body.scrollLeft || 0) - (html.clientLeft || 0);
        e.pageY = e.clientY + (html && html.scrollTop || body && body.scrollTop || 0) - (html.clientTop || 0);
    }
    var D = {X:e.pageX - pos.x-1,Y:0,Z:e.pageY - pos.y-1};
    var D1 = ViewPosition.unRotate(D);
    var P = ViewPosition.decartToPolar(D1);
    var TmpCat;
    if(Tycho2) {
        TmpCat = ViewPosition.Catalog.filter(SelectStars);
        if(TmpCat[0]){
            P.a = TmpCat[0][0] * gradToRad;
            P.d = TmpCat[0][1] * gradToRad;
        }
    }
    TargetPosition.SetPosition(P.a * radToGrad,P.d * radToGrad);
    updateTargetForm();
    updateTargetCrosPos();
    if(!DrawingStar) StarView.updateStars();
    ShowTargetStarInfo();
}
function updateTargetCrosPos(){
    var aR = TargetPosition.X * gradToRad;
    var dR = TargetPosition.Y * gradToRad;
    var azR = LocalStarTime * 15 * gradToRad;
    var dzR = GeoPosition.Lat * gradToRad;
    var L = Math.acos(Math.sin(dzR)*Math.sin(dR)+Math.cos(dzR)*Math.cos(dR)*Math.cos(azR-aR));        
    var angleA = 90 - LocalStarTime * 15 + TargetPosition.X;
    var angleD = 90 - GeoPosition.Lat + TargetPosition.Y;
    if(angleA < 0) angleA += 360;
    if(angleA >= 360) angleA -= 360;
    var Visible = (L <= Math.PI/2) && (angleA > 0 && angleA < 180) && (angleD > 0 && angleD < 180);
    //document.getElementById('RealA').value = Visible?angleA.toFixed(2):"not visible";
    //document.getElementById('RealD').value = Visible?angleD.toFixed(2):"not visible";   
    var color = Visible?"#00DD00":"#FF0000";    
    document.getElementById('VisRA').style.color = color;
    document.getElementById('VisDE').style.color = color;    
    AbsolutePosition.a = angleA;
    AbsolutePosition.d = angleD;
    AbsolutePosition.visible = Visible;
    if(!Loading) document.getElementById('gotobutton').disabled = !Visible;
}
function updateTargetForm(){
    var RAobj = AngleToObj(TargetPosition.X,true);
    var DEobj = AngleToObj(TargetPosition.Y,false);
    document.getElementById('RAh').value = RAobj.H;
    document.getElementById('RAm').value = RAobj.M;
    document.getElementById('RAs').value = RAobj.S.toFixed(2);
    var sign = (DEobj.Sign>0)?"+":"-";
    document.getElementById('DEh').value = "" + sign + DEobj.H;
    document.getElementById('DEm').value = DEobj.M;
    document.getElementById('DEs').value = DEobj.S.toFixed(2);
}
function GoTo(){
    if(AbsolutePosition.visible && !Loading){
        document.getElementById('gotobutton').disabled = true;
        newAJAXCommand('index.htm',function(){document.getElementById('gotobutton').disabled = false;}, false,"ang0="+AbsolutePosition.a + "&ang1="+AbsolutePosition.d);
    }
}

function Stop(){    
    newAJAXCommand('index.htm',function(){}, false,"stop=1");
}
function GoToView(){
    ViewPosition.SetPosition(CurrentPosition.X,CurrentPosition.Y);
    if(!DrawingStar) StarView.updateStars();
}
//var ElementUpgraded = 0;
function updateTarget(id){
    var Aobj = {};
    var Bobj = {};
    Aobj.Sign = 1;
    Aobj.S = +document.getElementById('RAs').value;
    Aobj.M = +document.getElementById('RAm').value;
    Aobj.H = +document.getElementById('RAh').value;
    if(Aobj.S >= 60) {
        Aobj.S -= 60;
        Aobj.M += 1;
        document.getElementById('RAm').value = Aobj.M;
        document.getElementById('RAs').value = Aobj.S.toFixed(2);
    }
    if(Aobj.M >= 60) {
        Aobj.M = Aobj.M - 60;
        Aobj.H += 1;
        document.getElementById('RAm').value = Aobj.M;
        document.getElementById('RAh').value = Aobj.H;
    }
    if(Aobj.S <= -1) {
        Aobj.S += 60;
        Aobj.M -= 1;
        document.getElementById('RAm').value = Aobj.M;
        document.getElementById('RAs').value = Aobj.S.toFixed(2);
    }
    if(Aobj.M <= -1) {
        Aobj.M += 60;
        Aobj.H -= 1;
        document.getElementById('RAm').value = Aobj.M;
        document.getElementById('RAh').value = Aobj.H;
    }

    if(document.getElementById('DEh').value.indexOf("-")>=0) Bobj.Sign = -1; else Bobj.Sign = 1;
    Bobj.H = +document.getElementById('DEh').value;
    Bobj.M = +document.getElementById('DEm').value;
    Bobj.S = +document.getElementById('DEs').value;
    if(Bobj.S >= 60) {
        Bobj.S = Bobj.S - 60;
        Bobj.M += 1;
        document.getElementById('DEm').value = Bobj.M;
        document.getElementById('DEs').value = Bobj.S.toFixed(2);
    }
    if(Bobj.M >= 60) {
        Bobj.M = Bobj.M - 60;
        Bobj.H += 1;
        document.getElementById('DEm').value = Bobj.M;
        document.getElementById('DEh').value = Bobj.H;
    }
    if(Bobj.S <= -1) {
        Bobj.S += 60;
        Bobj.M -= 1;
        document.getElementById('DEm').value = Bobj.M;
        document.getElementById('DEs').value = Bobj.S.toFixed(2);
    }
    if(Bobj.M <= -1) {
        Bobj.M += 60;
        Bobj.H -= 1;
        document.getElementById('DEm').value = Bobj.M;
        document.getElementById('DEh').value = Bobj.H;
    }
    TargetPosition.SetPosition(+objToAngle(Aobj, true),+objToAngle(Bobj, false));
    if(!DrawingStar) StarView.updateStars();
}

function correctCoordinate(Catalog){
    var ArrsLength = Catalog.length;
    var now = new Date();
    var j2000 = new Date(Date.UTC(2000, 0, 1, 11, 58, 55, 816));// 11:58:55,816 1 января 2000 года по UTC
    var ddd = now - j2000;
    var YearsFromJ2000 = ddd/(3600000*24*365);
    // pmRA, pmDE в миллисекундах дуги в год
    if(1)
    for (var i = 0;i < ArrsLength; i++) {
        var ra = Catalog[i][5];
        var de = Catalog[i][6];
        if((ra === undefined) || (de === undefined)) continue;
        ra = ra/3600000;
        de = de/3600000;
        Catalog[i][0] += ra*YearsFromJ2000;
        Catalog[i][1] += de*YearsFromJ2000;
       //Catalog[i][2] = (9 - Catalog[i][2])/4+0.5;
    }
}



function Tooltip(options) {

  var offsetFromCursor = (options.offset === undefined) ? 10 : options.offset;
  var tooltipElem;
  var isEnabled = true;

  var elem = options.elem;
  var html = options.html;

  elem.on({
    mouseenter: onMouseEnter,
    mouseleave: onMouseLeave,
    mousemove: onMouseMove
  });

  function onMouseEnter(e) {
    show(e.pageX, e.pageY);
  }

  function onMouseLeave() {
    hide();
  }

  function onMouseMove(e) {
    show(e.pageX, e.pageY);
  }


  function hide() {
    getTooltipElem().remove();
  };

  function getTooltipElem() {
    if (!tooltipElem) {
      tooltipElem = $('<div/>', {
        "class" : 'tooltip',
        html: html
      });
    }
    return tooltipElem;
  }

  function show(pageX, pageY) {
    var tooltipElem = getTooltipElem();

    if (!tooltipElem.is(':visible')) {
      // первым делом - отобразить подсказку, чтобы можно было получить её размеры
      tooltipElem.appendTo('body');
    }

    var scrollY = $(window).scrollTop();
    var winBottom = scrollY + $(window).height();

    var scrollX = $(window).scrollLeft();
    var winRight  = scrollX + $(window).width();

    var newLeft = pageX + offsetFromCursor;
    var newTop = pageY + offsetFromCursor;

    if (newLeft + tooltipElem.outerWidth() > winRight) { // если за правой границей окна
      newLeft -= tooltipElem.outerWidth();
      newLeft -= offsetFromCursor + 2; // немного левее, чтобы курсор был не над подсказкой
    }

    if (newTop + tooltipElem.outerHeight() > winBottom) { // если за нижней границей окна
      newTop -= tooltipElem.outerHeight();
      newTop -= offsetFromCursor + 2;  // немного выше
    }

    tooltipElem.css({
      left: newLeft,
      top: newTop
    });
  };
}


var loadScript = function(src, callback, appendTo) {
    var script = document.createElement('script');

    if (!appendTo) {
        appendTo = document.getElementsByTagName('head')[0];
    }

    if (script.readyState && !script.onload) {
        // IE, Opera
        script.onreadystatechange = function() {
            if (script.readyState == "loaded" || script.readyState == "complete") {
                script.onreadystatechange = null;
                callback();
            }
        }
    }
    else {
        // Rest
        script.onload = callback;
    }

    script.src = src;
    appendTo.appendChild(script);
}
// Вычисление звездного времени
function GM_Sidereal_Time (jd) {
    var t_eph, ut, MJD0, MJD;

    MJD = jd - 2400000.5;
    MJD0 = Math.floor(MJD);
    ut = (MJD - MJD0)*24.0;
    t_eph  = (MJD0-51544.5)/36525.0;
    return  6.697374558 + 1.0027379093*ut + (8640184.812866 + (0.093104 - 0.0000062*t_eph)*t_eph)*t_eph/3600.0;
}

function LM_Sidereal_Time (jd, longitude) {
    var GMST = GM_Sidereal_Time(jd);
    var LMST =  24.0*frac((GMST + longitude/15.0)/24.0);
    LocalStarTime = LMST;
    return HoursMinutesSeconds(LMST);
}
function frac(X) {
 X = X - Math.floor(X);
 if (X<0) X = X + 1.0;
 return X;
}

function JulDay (d, m, y, u){
    if (y<1900) y=y+1900
    if (m<=2) {m=m+12; y=y-1}
    if (m<=2) {m=m+12; y=y-1}
    JD =  Math.floor(365.25*(y+4716)) + Math.floor(30.6001*(m+1)) + d - 13 -1524.5 + u/24.0;
    return JD;
}
function HoursMinutesSeconds(time) {

 var h = Math.floor(time);
 var min = Math.floor(60.0*frac(time));
 var secs = Math.round(60.0*(60.0*frac(time)-min));

 var str;
 if (min>=10) str=h+":"+min;
 else  str=h+":0"+min;
 //if (min==60) str=(h+1)+":00";
 if (secs<10) str = str + ":0"+secs;
 else str = str + ":"+secs;
 return " " + str;

}

function CalculateNet(){
    var i = 0;
    var j = 0;
    var Net = [
        [{X:0,Y:0,Z:100}],
        //[{X:0,Y:0,Z:0},{X:0,Y:0,Z:0}]
    ];
    for(j=0;j<19;j++){
        Net.push([]);
        var R = 100*Math.cos((j-9)*10*gradToRad); // радиус окружности
        var Z = 100*Math.sin((j-9)*10*gradToRad);
        for(i=0;i<24;i++){
            Net[j].push({X:0,Y:0,Z:0,D:{X:0,Y:0}});
            Net[j][i].X = R*Math.cos(i*15*gradToRad);
            Net[j][i].Y = R*Math.sin(i*15*gradToRad);
            Net[j][i].Z = Z;
        }
    }
    return Net;
}

// realangle = 90 - (LST - a) = 90 - LST + a;
