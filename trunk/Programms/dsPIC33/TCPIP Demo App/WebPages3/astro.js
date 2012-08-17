
var CoordX = 84.053;
var CoordY = -1.201;
var Scale = 2; // увеличить в 10 раз
var Magnitude = 1; // величина звезд
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
var Net = [
//X,Y,Z,X1,Y1,Z1,X2,Y2,Z2,X3,Y3,Z3
[{X:0,Y:0,Z:100},{X: 135,Y:0,Z:100}, {X: 135,Y:0,Z:-100}, {X:0,Y:0,Z:-100}],
[{X:0,Y:0,Z:100},{X:-135,Y:0,Z:100}, {X:-135,Y:0,Z:-100}, {X:0,Y:0,Z:-100}],
[{X:0,Y:0,Z:100},{X:0,Y:135,Z:100}, {X: 0,Y:135,Z:-100}, {X:0,Y:0,Z:-100}],
[{X:0,Y:0,Z:100},{X:0,Y:-135,Z:100}, {X:0,Y:-135,Z:-100}, {X:0,Y:0,Z:-100}],
[{X:100,Y:0,Z:0},{X:100,Y:135,Z:0}, {X:-100,Y:135,Z:0}, {X:-100,Y:0,Z:0}],
[{X:100,Y:0,Z:0},{X:100,Y:-135,Z:0}, {X:-100,Y:-135,Z:0}, {X:-100,Y:0,Z:0}],
];

var GeoPosition = {
	Lon : 37.6028,
	Lat : 55.791437,
}
// текущие координаты телескопа(зелёный крестик)
var CurrentPosition = {
	X : 84.053,
	Y : -1.201,
};
var MousePosition = {
	X : 0,
	Y : 0,
};

var MousePositionStar = {
	X : 0,
	Y : 0,
};
function filterStars(element,index,array){
	if((element[2] < Magnitude) && ViewPosition.inRadius(element[0]*gradToRad,element[1]*gradToRad)) return true;
	return false;
};

// текущее положение просмотра
var ViewPosition = {
	X : 84.053,
	Y : -1.201,
	X1 : 0,
	Y1 : 0,
	Scale1 : 0,
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
		if((Math.abs(Scale - this.Scale1)>2)||(Math.abs(this.X1-this.X)>10/Scale) || (Math.abs(this.Y1-this.Y)>10/Scale)){
			this.Catalog = Tycho2.filter(filterStars);
			this.X1 = this.X;
			this.Y1 = this.Y;
			this.Scale1 = Scale;
		}
	},	
	inRadius:function(RAr,DEr){
		var L = Math.acos(this.SinViewYr*Math.sin(DEr)+this.CosViewYr*Math.cos(DEr)*Math.cos(this.ViewXr-RAr));
		if(L < 2*Math.PI/Scale) {
			return true; 
		}else{ 
			return false;
		}
	},
	TranslateCoord:function(X,Y){
		var DEr = Y*gradToRad;
		var RAr = X*gradToRad;		
		//if(this.inRadius(RAr,DEr)){
			var D = this.ToDecart( RAr, DEr,100);
			return this.Rotate(D);
		//}
		//return {X:0,Y:0,V:false};
	},
	Rotate:function(D){
		var D1 = this.rotateDecartZ(D);			
		D1 = this.rotateDecartX(D1);
		return {X:(WSizeXd2+D1.X)*Scale,Y:(WSizeYd2-D1.Z)*Scale,V:true};
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
	}
};
// целевые координаты телескопа(желтый крестик)
var TargetPosition = {
	X : 87.053,
	Y : 4.201,
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
	if(Scale<40){
		Scale = +Scale + e.wheelDelta/120;
	} else if(Scale<300){
		Scale = +Scale + e.wheelDelta/30;
	} else {
		Scale = +Scale + e.wheelDelta/10;		
	}
	if(Scale < 4) {Scale = 4;};
	if(Scale > 1000) {Scale = 1000;};
	e.preventDefault ? e.preventDefault() : (e.returnValue = false);
	updateScale(3);
	if(!DrawingStar) StarView.updateStars();
}


// функция возвращает координату X на экране по координате на небе(градусы) 
function RAToX(X){			
	return (ViewPosition.X - X)*Scale + WSizeX/2;
};
function DEToY(Y){
	return (ViewPosition.Y - Y)*Scale + WSizeY/2;
};
function XToRA(RA){
	return (WSizeX/2 - RA)/Scale + ViewPosition.X
};
function YToDE(DE){
	return (WSizeY/2 - DE)/Scale + ViewPosition.Y
};
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
function UpdateTime(){	
	var now = new Date();
	var E2012 = new Date(Date.UTC(2012, 0, 1));
	var dt = now - E2012;
	var days = Math.floor(dt/(3600*24*1000));
	
	//var Utctime = new Date(1344782676000);	
	var B = PI2*(days-81)/365;	
	var T = 9.87*Math.sin(2*B) - 7.53*Math.cos(B) - 1.5*Math.sin(B);
	document.getElementById('UTCTime').value = ISODateString(now,true);
	//document.getElementById('EqEq').value = T.toFixed(2);
		
	var utDay = now.getUTCDate();
	var utMonth = now.getUTCMonth()+1;
	var utYear = now.getUTCFullYear();
	var utHours = now.getUTCHours();
	var utMinutes = now.getUTCMinutes();
	var utSeconds = now.getUTCSeconds();
	if (utYear<1900) utYear=utYear+1900;

	UT = utHours + utMinutes/60 + utSeconds/3600;
	
	var JD = JulDay (utDay, utMonth, utYear, UT);
	var LST = LM_Sidereal_Time(JD, GeoPosition.Lon);
	document.getElementById('LST').value =  LST;
	if(!DrawingStar) StarView.updateStars();
}

function loadCanvas() {
	StarView = document.getElementById('my_canvas');
	StarView.onmousedown = onMouseDown;
	StarView.updateStars = updateStars;
	StarView.drawStars = drawStars;
	StarView.DrawCross = DrawCross;
	StarView.onmousemove = mousemoveCanv;
	StarView.onclick = targetSelect;
	StarView.onmouseup = onMouseUp;
	StarView.updateCross = updateCross;
	var elem = document.getElementById('CanvasBox');
	document.getElementById('starScale').value = Scale;
	document.getElementById('starScaleN').value = Scale;
	document.getElementById('starMagn').value = Magnitude;
	document.getElementById('starMagnN').value = Magnitude;
	elem.ondragstart = function() {
		return false;
	};
	ViewPosition.SetPosition(CoordX,CoordY);
	if (elem.addEventListener) {
	  // IE9+, Opera, Chrome/Safari
		elem.addEventListener ("mousewheel", onMouseWheel, false);
	  // Firefox
		elem.addEventListener ("DOMMouseScroll", onMouseWheel, false);
	} else { // IE<9
	  elem.attachEvent ("onmousewheel", onMouseWheel);
	}
	var timer = setInterval(UpdateTime, 1000);
	StarView.updateStars();	
	updateTargetForm();
	//загрузка и коррекция координат каталога
	correctCoordinate(Tycho2);
	setTimeout(LoadData,200);
	var D = ToDecart(CurrentPosition.X,CurrentPosition.Y);
	var D1 = rotateDecart(D, 40*Math.PI/180, 5*Math.PI/180,0);
};
var NextCat=0;
function LoadData(){
	try{
		switch(NextCat){
		case 0: loadScript("stars/Tycho2_5.js", function(){scriptCallback(Tycho2_5);}, 0 );
			break;
		case 1:	loadScript("stars/Tycho2_6.js", function(){scriptCallback(Tycho2_6);}, 0 );
			break;
		case 2:	loadScript("stars/Tycho2_71.js", function(){scriptCallback(Tycho2_71);}, 0 );
			break;
		case 3:	loadScript("stars/Tycho2_72.js", function(){scriptCallback(Tycho2_72);}, 0 );
			break;
		case 4:	loadScript("stars/Tycho2_73.js", function(){scriptCallback(Tycho2_73);}, 0 );
			break;
		case 5:	loadScript("stars/Tycho2_81.js", function(){scriptCallback(Tycho2_81);}, 0 );
			break;
		case 6:	loadScript("stars/Tycho2_82.js", function(){scriptCallback(Tycho2_82);}, 0 );
			break;
		case 7:	loadScript("stars/Tycho2_83.js", function(){scriptCallback(Tycho2_83);}, 0 );
			break;
		case 8:	loadScript("stars/Tycho2_84.js", function(){scriptCallback(Tycho2_84);}, 0 );
			break;
		case 9:	loadScript("stars/Tycho2_85.js", function(){scriptCallback(Tycho2_85);}, 0 );
			break;
		case 10: loadScript("stars/Tycho2_86.js", function(){scriptCallback(Tycho2_86);}, 0 );
			break;
		case 11: loadScript("stars/Tycho2_87.js", function(){scriptCallback(Tycho2_87);}, 0 );
			break;
		case 12: loadScript("stars/Tycho2_88.js", function(){scriptCallback(Tycho2_88);}, 0 );
			break;
		}
	} catch (e) {
	}
	NextCat++;
}
function scriptCallback(Catalog){
	correctCoordinate(Catalog);
	Tycho2 = Tycho2.concat(Catalog);
	//StarView.updateStars();
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
	ViewPosition.SetPosition(ViewPosition.X,ViewPosition.Y);
	StarView.updateStars();
}
function updateMagn(id){
	var Magn = document.getElementById('starMagn');
	var MagnN = document.getElementById('starMagnN');
	if(Magn.value == MagnN.value) return;
	if(id==1){
		Magnitude = Magn.value;
		MagnN.value = Magn.value;
	} else {
		Magnitude = MagnN.value;
		Magn.value = MagnN.value;
	}
	if(!DrawingStar) StarView.updateStars();
}
//***************************************************************************************** drawStars
function drawStars(Catalog){
	DrawingStar = 1;
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
		
		for ( i = 0;i < ArrsLength; i++) {
			elem.RA = Catalog[i][0];
			elem.DE = Catalog[i][1];
			elem.mag = Catalog[i][2];
			if(elem.mag > Magnitude) break;
			if(elem.mag < maxMag) continue;
			var D = ViewPosition.TranslateCoord(elem.RA,elem.DE);
			//if(D.V){
				ctx.beginPath();
				switch (elem.mag){
					case -1.088: ctx.fillStyle = "blue"; // сириус
								 ctx.strokeStyle = "blue";
					break;
					case 0.283 : ctx.fillStyle = "cyan"; //Ригель
								 ctx.strokeStyle = "cyan"; 
					break;
					case 0.769 : ctx.fillStyle = "red"; // бетельгейзе
						ctx.strokeStyle = "red";
					break;
					default :    ctx.fillStyle = "white";
						ctx.strokeStyle = "white";
					break;
				}
				// спец звезды
				switch (Catalog[i][3]){
					case 0:ctx.fillStyle = "red";
						break;
					case 1:ctx.fillStyle = "red";
						break;
					case 2:ctx.fillStyle = "green";
						break;
					case 3:ctx.fillStyle = "blue";
						break;
					case 4:ctx.fillStyle = "cyan";
						break;
					case 5:ctx.fillStyle = "yellow";
						break;
					case 6:ctx.fillStyle = "white";
						break;
				}
				Starsize = (9 - elem.mag)*Scale/50+0.5;
				ctx.arc(D.X,D.Y, Starsize,0,PI2,true);
				ctx.fill();
			//}
		};
		//context.bezierCurveTo(controlX1, controlY1, controlX2, controlY2, endX, endY);
		for ( i = 0;i < 6; i++){
			drawNetwork(ctx,Net[i],"darkgreen");
		}
	} else {
		document.getElementById('my_canvas').style.display = 'none';
		document.getElementById('no-canvas').style.display = 'block';
	}
	DrawingStar = 0;
}

function drawNetwork(ctx,NetK,color){
		ctx.beginPath();
		var D1 = ViewPosition.Rotate(NetK[0]);
		ctx.moveTo(D1.X,D1.Y);
		D2 = ViewPosition.Rotate(NetK[1]);
		D3 = ViewPosition.Rotate(NetK[2]);
		D4 = ViewPosition.Rotate(NetK[3]);
		ctx.bezierCurveTo(D2.X, D2.Y, D3.X, D3.Y, D4.X, D4.Y);
		ctx.strokeStyle = color;
		ctx.stroke();
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
	var canvas = document.getElementById('my_canvas');
	if(canvas.getContext) {
		var ctx = canvas.getContext('2d');
		ctx.clearRect(0, 0, WSizeX, WSizeY);
		if(Tycho2) {
			StarView.drawStars(ViewPosition.Catalog);
			//StarView.drawStars(Test);
			document.getElementById('StarCounter').innerHTML = Tycho2.length + " stars are loaded.";
			document.getElementById('StarCounter').style.width = WSizeX*(Tycho2.length)/(120552) + 'px';
		}
		StarView.updateCross();
		
	}
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
			var Coord = ViewPosition.TranslateCoord(X,Y);				
			var ctx = canvas.getContext('2d');
			ctx.beginPath();
			ctx.strokeStyle  = color;
			if(wCircle){
				ctx.arc(Coord.X, Coord.Y,30,0,Math.PI*2,true);
				ctx.stroke();
			}
			ctx.moveTo(Coord.X + 30, Coord.Y);
			ctx.lineTo(Coord.X + 5, Coord.Y);
			ctx.stroke();
			ctx.moveTo(Coord.X - 5, Coord.Y);
			ctx.lineTo(Coord.X - 30, Coord.Y);
			ctx.stroke();
			ctx.moveTo(Coord.X, Coord.Y - 30);
			ctx.lineTo(Coord.X, Coord.Y - 5);
			ctx.stroke();
			ctx.moveTo(Coord.X, Coord.Y + 5);
			ctx.lineTo(Coord.X, Coord.Y + 30);
			ctx.stroke();
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
		MousePositionStar.X = XToRA(e.pageX - pos.x -1);
		MousePositionStar.Y = YToDE(e.pageY - pos.y -1);
		document.getElementById('outXt').value = "α : " + AngleToString(MousePositionStar.X,true);
		document.getElementById('outYt').value = "δ : " + AngleToString(MousePositionStar.Y,false);
	} else {
		var dX = (e.pageX - pos.x -1) - MousePosition.X;
		var dY = (e.pageY - pos.y -1) - MousePosition.Y; 
		ViewPosition.SetPosition(ViewPosition.X+dX/Scale,ViewPosition.Y+dY/Scale);
		if(!DrawingStar) this.updateStars();
		MousePosition.X = e.pageX - pos.x -1;
		MousePosition.Y = e.pageY - pos.y -1;
	}
	// спрятать информацию о звезде, если двинули мышкой
	if(document.getElementById('StarInfo'))
		document.getElementById('StarInfo').style.display = 'none'; 		
	timerId = setTimeout(ShowStarNumber,100);
}
//поиск в каталоге имени звезды
function SelectStars(element,index,array){
	var length = 25/(Scale);
	if(	  (element[0] >= MousePositionStar.X - length)
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
		TmpCat = Tycho2.filter(SelectStars);
		if(TmpCat[0]){
			if(document.getElementById('StarInfo'))
				document.getElementById('StarInfo').style.display = 'block';
			if(document.getElementById('outTYC'))
				document.getElementById('outTYC').value = "TYC:" + TmpCat[0][3] + "-" + TmpCat[0][4] + "-" + TmpCat[0][5]; 
			if(document.getElementById('outSa'))
				document.getElementById('outSa').value = "α : " + AngleToString(TmpCat[0][0],true);
			if(document.getElementById('outSg'))
				document.getElementById('outSg').value = "δ : " + AngleToString(TmpCat[0][1],false);
		} else {
			if(document.getElementById('StarInfo')) document.getElementById('StarInfo').style.display = 'none';
		}
	}	
}

function getPosition(e){
	var left = 0;
	var top  = 0;

	while (e.offsetParent){
		left += e.offsetLeft;
		top  += e.offsetTop;
		e	 = e.offsetParent;
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
	TargetPosition.X = XToRA(e.pageX - pos.x-1);
	TargetPosition.Y = YToDE(e.pageY - pos.y-1);
	updateTargetForm();
	if(!DrawingStar) StarView.updateStars();
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
	//CurrentPosition.X = TargetPosition.X;
	//CurrentPosition.Y = TargetPosition.Y;
	//StarView.updateStars();
	//UpdateSelPos();
	newAJAXCommand('index.htm',function(){}, false,"ang0=" + TargetPosition.X + "&ang1="+TargetPosition.Y);	
}
function GoToView(){
	ViewPosition.X = CurrentPosition.X;
	ViewPosition.Y = CurrentPosition.Y;
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
	
	TargetPosition.X = +objToAngle(Aobj, true);
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
	TargetPosition.Y = +objToAngle(Bobj, false);
	
	if(!DrawingStar) StarView.updateStars();
}

function correctCoordinate(Catalog){
	var ArrsLength = Catalog.length;
	var now = new Date();
	var j2000 = new Date(Date.UTC(2000, 0, 1, 11, 58, 55, 816));// 11:58:55,816 1 января 2000 года по UTC	
	var ddd = now - j2000;
	var YearsFromJ2000 = ddd/(3600000*24*365);
	for (var i = 0;i < ArrsLength; i++) {
		Catalog[i][0] += Catalog[i][6]/3600;
		Catalog[i][1] += Catalog[i][7]/3600;
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
 A = Math.floor(y/100);
 JD =  Math.floor(365.25*(y+4716)) + Math.floor(30.6001*(m+1)) + d - 13 -1524.5 + u/24.0;
 return JD
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


// x = r*cos(a)cos(d)
// y = r*cos(a)sin(d)
// z = r*cos(a)

function ToDecart(a,d,S){
	var x = S*Math.cos(d)*Math.cos(a);
	var y = S*Math.cos(d)*Math.sin(a);
	var z = S*Math.sin(d);
	return {X:x,Y:y,Z:z};
}

function rotateDecart(D,xa,ya,za){
	var X;
	var Y;
	var Z;
	if(xa!=0){
		X =  D.X;
		Y =  D.Y*Math.cos(xa) - D.Z*Math.cos(xa);
		Z =  D.Y*Math.sin(xa) + D.Z*Math.cos(xa);		
	}
	if(ya!=0){		
		X =  D.X*Math.cos(ya) + D.Z*Math.sin(ya);
		Y =  D.Y;
		Z = -D.X*Math.sin(ya) + D.Z*Math.cos(ya);
	}
	if(za!=0){		
		X =  D.X*Math.cos(za) - D.Y*Math.sin(za);
		Y =  D.X*Math.sin(za) + D.Y*Math.cos(za);
		Z =  D.Z;
	}
	return {X:X,Y:Y,Z:Z};
}

var Test = //RA,DE,VTmag,TYC1,TYC2,TYC3,pmRA,pmDE
	[
	[0,90,-3,1,1,1,0,0],
	[0,-90,-3,2,1,1,0,0],
	[0,0,-3,3,1,1,0,0],
	[90,0,-3,4,1,1,0,0],
	[180,0,-3,5,1,1,0,0],
	[270,0,-3,6,1,1,0,0]];
