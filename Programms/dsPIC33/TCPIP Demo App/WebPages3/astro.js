
var CoordX = 84.053;
var CoordY = -1.201;
var Scale = 25; // увеличить в 10 раз
var Magnitude = 5; // величина звезд
var WSizeX = 0;
var WSizeY = 0;
// текущие координаты телескопа(зелёный крестик)
var CurrentPosition = {
	X : 84.053,
	Y : -1.201,
};
var MousePosition = {
	X : 0,
	Y : 0,
};
// текущее положение просмотра
var ViewPosition = {
	X : 84.053,
	Y : -1.201,
};
// целевые координаты телескопа(желтый крестик)
var TargetPosition = {
	X : 87.053,
	Y : 4.201,
};
var StarView;

function onMouseDown(e){
	this.BeginDrag = true;
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
	//StarView.updateStars();
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
function loadCanvas() {
	StarView = document.getElementById('my_canvas');
	StarView.onmousedown = onMouseDown;
	StarView.updateStars = updateStars;
	StarView.onmousemove = mousemoveCanv;
	StarView.onclick = targetSelect;
	var elem = document.getElementById('CanvasBox');
	elem.ondragstart = function() {
		return false;
	};
	if (elem.addEventListener) {
	  // IE9+, Opera, Chrome/Safari
		elem.addEventListener ("mousewheel", onMouseWheel, false);
	  // Firefox
		elem.addEventListener ("DOMMouseScroll", onMouseWheel, false);
	} else { // IE<9
	  elem.attachEvent ("onmousewheel", onMouseWheel);
	}
	StarView.updateStars();
	updatePM();
};	
function AngleToObj(A,h){
	var tA = A;
	if(h){
		tA = A/15;
	}
	var grad = Math.floor(tA);
	tA = (tA - grad)*60;
	var min = Math.floor(tA);
	var sec = ((tA - min)*60);
	return {H:grad,M:min,S:sec};
};
function objToAngle(O,h){
	var tA = 0;
	tA = O.M + O.S/60;
	if(O.H>=0){
		tA = O.H + tA/60;
	} else {
		tA = O.H - tA/60;
	}
	if(h) return tA*15;
	return tA;
};
function AngleToString(A,h){
	var tA = A;
	if(h){
		tA = A/15;
	}		
	var grad = Math.floor(tA);
	tA = (tA - grad)*60;
	var min = Math.floor(tA);
	var sec = ((tA - min)*60);
	return "" + grad + "°" + min + "'" + sec.toFixed(2) + '"';
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
	StarView.updateStars();
}
function updateStars() {
	var canvas = this;
	if(canvas.getContext) {
		var ctx = canvas.getContext('2d');
		WSizeX = canvas.width;
		WSizeY = canvas.height;
		var ImgLeft  = ViewPosition.X + WSizeX/(2*Scale);
		var ImgRight = ViewPosition.X - WSizeX/(2*Scale);
		var ImgTop   = ViewPosition.Y + WSizeY/(2*Scale);
		var ImgBottom = ViewPosition.Y - WSizeY/(2*Scale);
		var i = 0;
		
		var maxMag = -5;
		var ArrsLength = Tycho2.length;
		ctx.clearRect(0, 0, WSizeX, WSizeY);
		
		
		for ( i = 0;i < ArrsLength; i++) {
			if(Tycho2[i][2] > Magnitude) break;
			if(Tycho2[i][2] < maxMag) continue;
			if(Tycho2[i][0] < ImgRight) continue;
			if(Tycho2[i][0] > ImgLeft) continue;
			if(Tycho2[i][1] < ImgBottom) continue;
			if(Tycho2[i][1] > ImgTop) continue;
			ctx.beginPath();
			switch (Tycho2[i][2]){
				case -1.088: ctx.fillStyle = "blue"; // сириус
				break;
				case 0.283 : ctx.fillStyle = "cyan"; //Ригель
				break;
				case 0.769 : ctx.fillStyle = "red"; // бетельгейзе
				break;
				default :    ctx.fillStyle = "white";
				break;
			}					
			ctx.arc((ImgLeft-Tycho2[i][0])*Scale,(ImgTop - Tycho2[i][1])*Scale,(9 - Tycho2[i][2])/4+0.5,0,Math.PI*2,true);
			ctx.fill();
		};
	} else {
		document.getElementById('my_canvas').style.display = 'none';
		document.getElementById('no-canvas').style.display = 'block';
	}
	updateCross();
};
function updateCross(){
	var canvas = document.getElementById('my_canvas');
	if(canvas.getContext) {
		DrawCross(RAToX(CurrentPosition.X),DEToY(CurrentPosition.Y),"green",true);
		DrawCross(RAToX(TargetPosition.X),DEToY(TargetPosition.Y),"yellow",false);
	}
}

function DrawCross(X,Y,color,wCircle){
	var canvas = document.getElementById('my_canvas');
	if(canvas.getContext) {
		var ctx = canvas.getContext('2d');
		ctx.beginPath();
		ctx.strokeStyle  = color;
		if(wCircle){
			ctx.arc(X, Y,30,0,Math.PI*2,true);
			ctx.stroke();
		}
		ctx.moveTo(X + 30, Y);
		ctx.lineTo(X + 5, Y);
		ctx.stroke();
		ctx.moveTo(X - 5, Y);
		ctx.lineTo(X - 30, Y);
		ctx.stroke();
		ctx.moveTo(X, Y - 30);
		ctx.lineTo(X, Y - 5);
		ctx.stroke();
		ctx.moveTo(X, Y + 5);
		ctx.lineTo(X, Y + 30);
		ctx.stroke();
	}
}
function mousemoveCanv(e){
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
	if(this.BeginDrag) this.Drag = true;
	if(!this.Drag){
		document.getElementById('outX').value = AngleToString(XToRA(e.pageX - pos.x -1),true);
		document.getElementById('outY').value = AngleToString(YToDE(e.pageY - pos.y -1),false);
	} else {
		var dX = (e.pageX - pos.x -1) - MousePosition.X;
		var dY = (e.pageY - pos.y -1) - MousePosition.Y; 
		ViewPosition.X += dX/Scale;
		ViewPosition.Y += dY/Scale;
		this.updateStars();
		MousePosition.X = e.pageX - pos.x -1;
		MousePosition.Y = e.pageY - pos.y -1;
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
	if(this.Drag) {
		this.Drag = false;
		this.BeginDrag = false;
		return;
	} else {
		this.Drag = false;
		this.BeginDrag = false;
	}
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
	var RAobj = AngleToObj(TargetPosition.X,true);
	var DEobj = AngleToObj(TargetPosition.Y,false);
	document.getElementById('RAh').value = RAobj.H;
	document.getElementById('RAm').value = RAobj.M;
	document.getElementById('RAs').value = RAobj.S.toFixed(2);
	
	document.getElementById('DEh').value = DEobj.H;
	document.getElementById('DEm').value = DEobj.M;
	document.getElementById('DEs').value = DEobj.S.toFixed(2);
	StarView.updateStars();
}
function GoTo(){
	CurrentPosition.X = TargetPosition.X;
	CurrentPosition.Y = TargetPosition.Y;
	ViewPosition.X = TargetPosition.X;
	ViewPosition.Y = TargetPosition.Y;
	StarView.updateStars();
}
//var ElementUpgraded = 0;
function updateTarget(id){
	var Aobj = {
		H : 0,
		M : 0,
		S : 0,
	};
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

	Aobj.H = +document.getElementById('DEh').value;
	Aobj.M = +document.getElementById('DEm').value;
	Aobj.S = +document.getElementById('DEs').value;
	if(Aobj.S >= 60) {
		Aobj.S = Aobj.S - 60;
		Aobj.M += 1;
		document.getElementById('DEm').value = Aobj.M;
		document.getElementById('DEs').value = Aobj.S.toFixed(2);
	}
	if(Aobj.M >= 60) {
		Aobj.M = Aobj.M - 60;
		Aobj.H += 1;
		document.getElementById('DEm').value = Aobj.M;
		document.getElementById('DEh').value = Aobj.H;
	}
	if(Aobj.S <= -1) {
		Aobj.S += 60;
		Aobj.M -= 1;
		document.getElementById('DEm').value = Aobj.M;
		document.getElementById('DEs').value = Aobj.S.toFixed(2);
	}
	if(Aobj.M <= -1) {
		Aobj.M += 60;
		Aobj.H -= 1;
		document.getElementById('DEm').value = Aobj.M;
		document.getElementById('DEh').value = Aobj.H;
	}
	TargetPosition.Y = +objToAngle(Aobj, false);
	
	StarView.updateStars();
}
function updatePM(){
	var ArrsLength = Tycho2.length;
	var now = new Date();
	var j2000 = new Date(Date.UTC(2000, 0, 1, 11, 58, 55, 816));// 11:58:55,816 1 января 2000 года по UTC	
	var ddd = now - j2000;
	var YearsFromJ2000 = ddd/(3600000*24*365);
	for (var i = 0;i < ArrsLength; i++) {
		Tycho2[i][0] += Tycho2[i][6]/3600;
		Tycho2[i][1] += Tycho2[i][7]/3600;
	}
}