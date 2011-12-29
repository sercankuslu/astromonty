// guidance.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "guidance.h"
#include <math.h>
#include <stdio.h>


#define MAX_LOADSTRING 100

#define PI 3.1415926535897932384626433832795
#define ACCELERATE_SIZE 111
#define FREQ_STEP 20
#define BUF_SIZE 1024
//static double Accelerate[ACCELERATE_SIZE];
const double Grad_to_Rad = PI / 180.0;
const double Rad_to_Grad = 180.0 / PI;

typedef double ARR_TYPE;
typedef struct FREQ_POWER {
    WORD Freq;
    double Power;
} FREQ_POWER;
static FREQ_POWER FreqPower[] = {
    {0,     0.850000000},
    {100,   0.764642857},
    {250,   0.666666667},
    {500,   0.600000000},
    {750,   0.533333333},
    {1000,  0.466666667},
    {1250,  0.400000000},
    {1500,  0.333333333},
    {1750,  0.222222222},
    {2000,  0.100000000},
    {2200,  0.000000000}
};
typedef enum Cmd {                  // команды
	CM_STOP,                // Остановиться (снижаем скорость до остановки)
	CM_RUN_WITH_SPEED,      // Двигаться с заданной скоростью до окончания 
	CM_RUN_TO_POINT,        // Двигаться до указанного угла        
} GD_CMD;
typedef enum State {                // состояния
        ST_FREE,                // ожидает получения команды (значение, устанавливаемое после каждого состояния)
	ST_STOP,                // остановлен
	ST_ACCELERATE,          // разгоняется
	ST_RUN,                 // движется с постоянной скоростью
	ST_DECELERATE           // тормозит
} GD_STATE;
typedef struct RR {
    
    // Время
    // IntervalArray
    // |      NextWriteTo
    // |      |      NextReadFrom
    // |      |      | 
    // v      v      v
    // 0======-------=========
    // 
    //       окончание предыдущей операции ( исходные параметры операции)
    //       |   промежуточное состояние
    //       |   |   Конец операции 
    //       |   |   |
    //       v   v   v 
    // -------========--------
    // 
    
    ARR_TYPE    IntervalArray[BUF_SIZE];    // массив отсчетов времени (кольцевой буффер)
    WORD        NextReadFrom;               // индекс массива времени. указывает на первый значащий элемент
    WORD        NextWriteTo;                // индекс массива времени. указывает на первый свободный элемент
    WORD        DataCount;                  // количество данных в массиве.

    // команды
    GD_CMD      Cmd;
    GD_STATE    State;    
    GD_STATE    NextState;
    
    // настоящее состояние
    DWORD       CurrentX;                   // текущий номер шага
    BYTE        Direction;                  // направление движения
    
    // соманндные переменные
    double      cmdTargetSpeed;             // разгоняемся до скорости
    double      cmdTargetAngle;             // двигаемся до угла
    //DWORD   TargetX;
    
    
    // исхoдные параметры:
    ARR_TYPE    TimeBeg;  
    DWORD       XaccBeg;                    //(желательно целое число шагов)
    double       Xbeg;
    

    // параметры указывающие на момент окончания
    double      Vend;                       //(надо знать скорость, на которой завершится ускорение)
    //ARR_TYPE    Tend;                       //(для ускорения работы нужно знать сразу время в шагах, когда закончить маневр)
    double       Xend;
    DWORD       XaccEnd;                    //координата ускорения (DWORD)

    // константы
    double      K;
    double      B;
    double      TimerStep;
    double      dx;

} RR;
RR rr1;
// действия при командах
// CM_STOP
// 1. установить state = ST_DECELERATE
//    задать Vend = 0.0
//    установить NextState = ST_STOP
//
// CM_RUN_WITH_SPEED
// 1. 


// Глобальные переменные:
#define FIRST_TIMER 1
int nTimerID;
HINSTANCE hInst;// текущий экземпляр
HWND hWindow;

TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
 

int Run(RR * rr);
int Acceleration(RR * rr);
int Deceleration(RR * rr);
ARR_TYPE CalculateT(double X, double K, double B, double TimerStep);

void Calc(HWND hWnd, HDC hdc);
DWORD MaxAcceleration(DWORD Xb, DWORD Xe,double dx, double K, double B, double * T, DWORD Len, DWORD * Xpos);



int APIENTRY _tWinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: разместите код здесь.

	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GUIDANCE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
		
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUIDANCE));

	SetTimer(hWindow, FIRST_TIMER, 1, (TIMERPROC) NULL);
	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
//  КОММЕНТАРИИ:
//
//    Эта функция и ее использование необходимы только в случае, если нужно, чтобы данный код
//    был совместим с системами Win32, не имеющими функции RegisterClassEx'
//    которая была добавлена в Windows 95. Вызов этой функции важен для того,
//    чтобы приложение получило "качественные" мелкие значки и установило связь
//    с ними.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUIDANCE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GUIDANCE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	hWindow = hWnd;
	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static int T = 0;
	static bool k = false;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER: 

		switch (wParam) 
		{ 
		case FIRST_TIMER: 	
			T++;
			if(T>=360) T-=360;
			//GetClientRect(hWnd, &rect);
			rect.top = 100;
			rect.bottom = 300;
			rect.left = 100;
			rect.right = 300;
			//InvalidateRect(hWnd, &rect, TRUE);
			k = true;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
				//LineTo(hdc, 100,100);
		// TODO: добавьте любой код отрисовки...

				
// 		if(k)
// 		{
// 			double X = 200+100*sin(T*PI/180);
// 			double Y = 200+100*cos(T*PI/180);
// 			MoveToEx(hdc, 200,200, NULL);
// 			LineTo(hdc,(int)X, (int)Y);
// 			k=false;
// 		} else 
			Calc(hWnd, hdc);	
		EndPaint(hWnd, &ps);
	

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
		
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

double LinInt(double x1,double y1,double x2,double y2, double x)
{
	if(x1!=x2) {
		return y1+(y2-y1)*(x-x1)/(x2-x1);
	}else return y1;
}
void Calc(HWND hWnd, HDC hdc)
{
    double D = 0.0;
    static double A = 0.0; //ускорение в радианах в сек за сек
    static double A1 = 0.0; //ускорение в радианах в сек за сек
    static double A2 = 0.0; //ускорение в радианах в сек за сек
    static double V1 = 0.0; //ускорение в радианах в сек за сек
    static double dt = 0.0; // изменение времени
    static double V = 0.0;  // мгновенная скорость в радианах
    //static double dX = 1.0/(200.0*16.0);// шаг перемещения в градусах (в 1 градусе 3200 шагов)
    static double dX = PI/(180.0*200.0*16.0); // шаг перемещения в радианах
    static double X = 0;    // полное перемещение в радианах
    static double X1 = 0;    // полное перемещение в радианах
    static double T = 0;    // полное время
    static double T1 = 0;    // полное время
    static double T2 = 0;    // полное время
    double dt1 = 0.0; // изменение времени
    int W = 0;       // флаги для оптимизации вывода
    int K1 = -1;
    int W1 = 0;
    int K3 = -1;
    double timer1 = 0;  // значение таймера
    //DWORD F = 0;
    DWORD i = 0;
    //DWORD k = 1;
    RECT rect;
    static double Mass = 500.0f;
    static double Radius = 0.30f;
    static double Length = 2.0f;
    static double Reduction = 360.0f;
    static double I = ((Mass*Radius*Radius/4) + (Mass*Length*Length/12))/Reduction; 
    static double K = (-0.000349812 * 200 * 180/PI)/I;
    //static double K = (-0.000154286 * 200 * 180/PI)/I;
    //static double B = 0.79962406 / I;
    static double B = 0.751428571 / I;

    static DWORD SizeX = 500;
    static DWORD SizeY = 50;
    static double Pi = PI;
    static double TT[64];
    static DWORD TTLen = 64;
    static DWORD Count = 0;
    DWORD XPos = 0;

    HGDIOBJ original = NULL;

    //Save original object.
    original = SelectObject(hdc,GetStockObject(DC_PEN));

    //double Vf = 180 * 200/PI;
    //char RRR[256];
    GetClientRect(hWnd, &rect);
    MoveToEx(hdc, rect.left+9, rect.bottom - 9, NULL);
    LineTo(hdc, rect.right - 9, rect.bottom - 9);
    MoveToEx(hdc, rect.left+9, rect.bottom - 9, NULL);
    LineTo(hdc, rect.left+9, rect.top);        
    

    for (DWORD i = 0; i < rect.bottom/SizeY ; i++) {
        if(i % 10 == 0){
            SetDCPenColor(hdc,RGB(0,0,0));
        } else {
            SetDCPenColor(hdc,RGB(200,200,200));
        }
        MoveToEx(hdc, rect.left + 10, rect.bottom - 10 - (int)(i*SizeY), NULL);
        LineTo(hdc,   rect.right -10, rect.bottom - 10 - (int)(i*SizeY) );  
    }
    for (DWORD i = 0; i < rect.right*10/(SizeX) ; i++) {
        if(i % 10 == 0){
            SetDCPenColor(hdc,RGB(0,0,0));
        } else {
            SetDCPenColor(hdc,RGB(200,200,200));
        }
        MoveToEx(hdc, rect.left + 10 + (int)(i*SizeX/10), rect.bottom - 10, NULL);
        LineTo(  hdc, rect.left + 10 + (int)(i*SizeX/10), rect.top); 
    }

    DWORD Px = rect.left + 10;
    DWORD Py = rect.bottom - 10 ;//- (rect.bottom/4);
    POINT TX = {Px,Py};
    POINT TV = {Px,Py};
    POINT TA = {Px,Py};
    POINT VA = {Px,Py};

       
    SetDCPenColor(hdc,RGB(0,200,0));
    MoveToEx(hdc, Px, Py - (int)(0.004166667*SizeY), NULL);
    LineTo(hdc, rect.right - 9, Py - (int)(0.004166667*SizeY));

    dt = 0.0;
    T = 0.0;
    T1 = 0.0;
    X = 0.0;
    V = 0.0;
    V1 = 0.0;
    rr1.B = B;
    rr1.K = K;
    rr1.dx = dX;
    rr1.TimerStep = 1;
    rr1.Vend = 10.0 * Grad_to_Rad;
    rr1.Xend = 15.0 * Grad_to_Rad;
    rr1.DataCount = 0;
    rr1.NextWriteTo = 0;
    rr1.NextReadFrom = 0;    
    rr1.XaccBeg = 0;
    rr1.Xbeg = 0;
    rr1.TimeBeg = (ARR_TYPE)(1.0 * Grad_to_Rad/((rr1.B + 10.0 * Grad_to_Rad * rr1.K) * rr1.TimerStep));
       
    rr1.State = ST_ACCELERATE;
    rr1.NextState = ST_RUN;
   

    do{		
        
         switch(rr1.State){
             case ST_ACCELERATE:
                 Acceleration(&rr1);                  
                 if(rr1.State == ST_RUN){
                     rr1.Vend = 10.0 * Grad_to_Rad;
                     rr1.NextState = ST_DECELERATE;
                 }
                 if(rr1.State == ST_DECELERATE){
                     rr1.Vend = 0.0 * Grad_to_Rad;
                     rr1.NextState = ST_STOP;
                 }
                 break;
             case ST_RUN:
                 Run(&rr1);
                 if(rr1.State == ST_DECELERATE){
                     rr1.Vend = 0.0 * Grad_to_Rad;
                     rr1.Xend = 0;
                     rr1.NextState = ST_STOP;
                 }
                 break;
             case ST_DECELERATE:                
                 Deceleration(&rr1);                
                 break;
             case ST_STOP:
                 break;
         }
        int j;     
        //V = 5.0*Grad_to_Rad;
        for( i = 0; i < rr1.DataCount; i++) 
        {            
            T = rr1.IntervalArray[rr1.NextReadFrom] * rr1.TimerStep;
            rr1.NextReadFrom++;
            if(rr1.NextReadFrom >= BUF_SIZE) rr1.NextReadFrom -= BUF_SIZE;
            V = 1.0*Grad_to_Rad;
            //X += dX;
            //T += dX/V;
            
              if(T-T1 != 0.0){
                  X += dX;
                  V = dX/(T-T1);
              }else {
                  V = 0.0;
              }
            
            //A = rr1.B*T/(1-rr1.K*T);
            
            //A = (V-V1)/(T-T1);
              
//             T = TT[i];
//             X += dX;
//             V = B*T/(1-K*T);
//             A = B/(1-K*T);
        
            K3 = (int)(T*SizeX);
            //if( K3 != K1)
            {
                //Change the DC pen color
                SetDCPenColor(hdc,RGB(0,0,255));
                MoveToEx(hdc, TX.x, TX.y, NULL);
                TX.x = Px + (int)(T*SizeX);
                TX.y = Py - (int)(X*Rad_to_Grad*SizeY);
                LineTo(hdc, TX.x, TX.y);  

                
                MoveToEx(hdc, TV.x, TV.y, NULL);
                TV.x = Px + (int)(T*SizeX);
                TV.y = Py - (int)(V*Rad_to_Grad*SizeY);
                if(rr1.State == ST_STOP){
                    SetDCPenColor(hdc,RGB(0,100,0));
                }
                if(rr1.State == ST_RUN){
                    SetDCPenColor(hdc,RGB(0,150,0));
                }
                if(rr1.State == ST_DECELERATE){
                    SetDCPenColor(hdc,RGB(0,200,0));
                }
                if(rr1.State == ST_ACCELERATE){
                    SetDCPenColor(hdc,RGB(0,255,0));                    
                }
                LineTo(hdc, TV.x, TV.y);  

                SetDCPenColor(hdc,RGB(255,0,0));
                MoveToEx(hdc, TA.x, TA.y, NULL);
                TA.x = Px + (int)(T*SizeX);
                TA.y = Py - (int)(A*Rad_to_Grad*SizeY);
                LineTo(hdc, TA.x, TA.y);  

                SetDCPenColor(hdc,RGB(200,200,0));
                MoveToEx(hdc, VA.x, VA.y, NULL);
                VA.x = Px + (int)(V*Rad_to_Grad*SizeY/10);
                VA.y = Py - (int)(A*Rad_to_Grad*SizeY/10);
                LineTo(hdc, VA.x, VA.y);                  

                K1 = K3;
            }
            T1 = T;
            V1 = V;
        }     
        rr1.DataCount = 0;
    }while ( /*(X < 0.590) && */(rr1.State!= ST_STOP)) /*rr1.State!=ST_STOP)*/;       
    //Restore original object.
    SelectObject(hdc,original);

}

// Время
// IntervalArray
// |      NextWriteTo
// |	  |      NextReadFrom
// |      |      | 
// v      v      v
// 0======-------=========
// 


int Run(RR * rr)
{
    // x = V*T
    // T = X/V;
    WORD i;
    WORD j;
    WORD FreeData = BUF_SIZE - rr->DataCount;  
    double X = 0.0;
    //X = rr->Vend * rr->TimeBeg;
    DWORD Xb = rr->Xbeg/rr->dx;
    DWORD Xe = rr->Xend/rr->dx;
    ARR_TYPE T = 0;
    ARR_TYPE T1;// = 0;
    for (i = 0; i < FreeData; i++){
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;  
        X += rr->dx;
        Xb++;
        T = rr->TimeBeg + (ARR_TYPE)(X / (rr->Vend * rr->TimerStep));
        if((Xe != 0) && (Xb >= Xe)){
            FreeData = i;
            rr->State = rr->NextState;                                
            break;
        }
        rr->IntervalArray[j] = T;
        T1 = T;
    }	
    rr->TimeBeg = T1;        
    rr->DataCount += FreeData;
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->Xbeg = Xb * rr->dx;
    return 0;
}
// разгон с текущей скорости до требуемой
//    ARR_TYPE    TimeBeg;  
//    DWORD       XaccBeg;                    //(желательно целое число шагов)

int Acceleration(RR * rr)
{
    WORD j;        
    WORD FreeData = BUF_SIZE - rr->DataCount;
    ARR_TYPE T = 0;
    ARR_TYPE T1 = 0;
    ARR_TYPE dT = 0;
    double X;       // временныя переменная 
    ARR_TYPE Tb = 0.0;    
    // V = B*T/(1-K*T)
    // T = V/(B+K*V)
    X = rr->XaccBeg * rr->dx; 
    DWORD Xb = rr->Xbeg/rr->dx;
    DWORD Xe = rr->Xend/rr->dx;
    if(rr->XaccBeg > 0){
        T1 = CalculateT( X, rr->K, rr->B, rr->TimerStep);         
    }
    Tb = rr->TimeBeg - T1;    
    
    if(rr->Vend != 0.0)
        dT = rr->dx/(rr->Vend * rr->TimerStep);

    for(WORD i = 0; i < FreeData; i++) {        
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;
        X += rr->dx;    
        Xb++;
        T = CalculateT( X, rr->K, rr->B, rr->TimerStep);           
        if((T - T1 <= dT)||((Xe != 0)&&(Xb >= Xe))){
            FreeData = i;
            rr->State = rr->NextState;                                
            break;
        }
        rr->IntervalArray[j] = Tb + T;  
        T1 = T;
    }
    rr->TimeBeg = Tb + T1;
    rr->XaccBeg += FreeData; 
    rr->DataCount += FreeData;
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->Xbeg = Xb * rr->dx;
    return 0;
}

ARR_TYPE CalculateT(double X, double K, double B, double TimerStep)
{
    double D;	
    double Kx;

    Kx = X * K;
    D = Kx * Kx + 4.0 * X * B;
    if(D >= 0.0){
        return (ARR_TYPE)((-Kx + sqrt(D))/(2.0 * TimerStep * B ));    
    }
    return (ARR_TYPE) 0;
}

// торможение с текущей скорости до требуемой
int Deceleration(RR * rr)
{    
    WORD j;        
    WORD FreeData = BUF_SIZE - rr->DataCount;
    ARR_TYPE T = 0;
    ARR_TYPE T1 = 0;
    ARR_TYPE dT = 0;
    double X;       // временныя переменная 
    ARR_TYPE Tb = 0.0;    
    // V = B*T/(1-K*T)
    // T = V/(B+K*V)
    X = rr->XaccBeg * rr->dx; 
    DWORD Xb = rr->Xbeg/rr->dx;
    DWORD Xe = rr->Xend/rr->dx;
    if(rr->XaccBeg > 0){
        T1 = CalculateT( X, rr->K, rr->B, rr->TimerStep);         
    }
    Tb = rr->TimeBeg + T1;    

    if(rr->Vend != 0.0)
        dT = rr->dx/(rr->Vend * rr->TimerStep);

    for(WORD i = 0; i < FreeData; i++) {        
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;
        X -= rr->dx;  
        if(X<0.0){
            FreeData = i;
            rr->State = rr->NextState;            
            break;
        }
        Xb++;
        T = CalculateT( X, rr->K, rr->B, rr->TimerStep);           
        if((T1 - T <= dT)||((Xe != 0)&&(Xb >= Xe))){
            FreeData = i;
            rr->State = rr->NextState;                                
            break;
        }
        rr->IntervalArray[j] = Tb - T;  
        T1 = T;

    }
    rr->TimeBeg = Tb - T1;
    rr->XaccBeg -= FreeData; 
    rr->DataCount += FreeData;
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->Xbeg = Xb * rr->dx;
    return 0;
}
