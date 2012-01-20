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
#define BUF_SIZE 256
//static double Accelerate[ACCELERATE_SIZE];
const double Grad_to_Rad = PI / 180.0;
const double Rad_to_Grad = 180.0 / PI;

typedef DWORD ARR_TYPE;
//typedef double ARR_TYPE;


// Действия
// 1 часовое ведение
// 1.1 разгон до скорости
// 1.2 движение до максимальной координаты
// 2 наведение из нулевой позиции 
// 2.1 разгон до максимальной скорости
// 2.2 торможение до 0
// 3 наведение на произвольную позицию
// 3.1 если направление движения совпадает с текущим то:
// 3.1.1 разгон до максимальной скорости
// 3.1.2 торможение
// 3.2 если направление движения не совпадает с текущим то:
// 3.2.1 торможение до 0
// 3.2.2 смена направления
// 3.2.3 разгон до максимума
// 3.2.4 торможение
// 4 навеление на произвольную позицию с произвольной скорости в произвольную скорость
// 4.1 если направление движения совпадает с текущим то:
// 4.1.1 разгон до максимальной скорости
// 4.1.2 торможение до требуемой скорости, если требуемая скорость совпадает с текущей
// 4.1.3 торможение до нуля, если требуемая скорость не совпадает с текущей
// 4.1.4 разгон до требуемой скорости
// 4.2 если направление движения не совпадает с текущим то:
// 4.2.1 торможение до 0
// 4.2.2 смена направления
// 4.2.3 разгон до максимума
// 4.2.4 торможение


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
    ARR_TYPE    Interval;                   // текущий интервал при расчетах. нужен для переключения на более "грубый" режим
    ARR_TYPE    IntervalArray[BUF_SIZE];    // массив отсчетов времени (кольцевой буффер)
    WORD        NextReadFrom;               // индекс массива времени. указывает на первый значащий элемент
    WORD        NextWriteTo;                // индекс массива времени. указывает на первый свободный элемент
    WORD        DataCount;                  // количество данных в массиве.
    //DWORD_VAL   T;
    ARR_TYPE    T1;                         // предыдущий интервал (оптимизация)

    // команды
    GD_CMD      Cmd;
    GD_STATE    State;   
    GD_STATE    RunState; 
    GD_STATE    NextState;

    // исхoдные параметры:
    ARR_TYPE    TimeBeg;  
    DWORD       XaccBeg;                    //параметры функции ускорения (желательно целое число шагов)
    double      Xbeg;

    // параметры указывающие на момент окончания
    double      Vend;                       //(надо знать скорость, на которой завершится ускорение)
    double      Xend;
    DWORD       XaccEnd;                    //координата ускорения (DWORD)

    // константы
    double      K;
    double      B;
    double      TimerStep;
    double      dx;
    double      Mass;
    double      Radius;
    double      Length;
    double      Reduction;

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
    static double A = 0.0; //ускорение в радианах в сек за сек
    static double A1 = 0.0; //ускорение в радианах в сек за сек
    static double A2 = 0.0; //ускорение в радианах в сек за сек
    static double dt = 0.0; // изменение времени
    static double V0 = 1.0* Grad_to_Rad;  // мгновенная скорость в радианах
    static double V = 0.0;  // мгновенная скорость в радианах
    static double V1 = 0.0; //мгновенная скорость в радианах
    static double V2 = 0.0;  // мгновенная скорость в радианах
    //static double dX = 1.0/(200.0*16.0);// шаг перемещения в градусах (в 1 градусе 3200 шагов)
    static double dX = PI/(180.0*200.0*16.0); // шаг перемещения в радианах
    static double X0 = 0;    // полное перемещение в радианах
    static double X = 0;    // полное перемещение в радианах
    static double X1 = 0;    // полное перемещение в радианах
    static double T = 0;    // полное время
    static double T1 = 0;    // полное время
    static double T2 = 0;    // полное время
    double dt1 = 0.0; // изменение времени
    int K1 = -1;
    int K3 = -1;
    DWORD i = 0;
    BYTE L = 255;
    RECT rect;
    static double Mass = 500.0f;
    static double Radius = 0.30f;
    static double Length = 3.0f;
    static double Reduction = 360.0f;
    static double I = ((Mass*Radius*Radius/4) + (Mass*Length*Length/12))/Reduction; 
    static double K = (-0.000349812 * 200 * 180/PI)/I;
    //static double K = (-0.000154286 * 200 * 180/PI)/I;
    //static double B = 0.79962406 / I;
    static double B = 0.751428571 / I;

    static DWORD SizeX = 800;
    static DWORD SizeY = 30;
    static double Pi = PI;
    static double TT[64];
    static DWORD TTLen = 64;
    static DWORD Count = 0;

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
    POINT TV2 = {Px,Py};
    POINT TA = {Px,Py};
//    POINT VA = {Px,Py};
    POINT X0T = {Px,Py};
       
    //SetDCPenColor(hdc,RGB(0,200,0));
    //MoveToEx(hdc, Px, Py - (int)(0.004166667*SizeY), NULL);
    //LineTo(hdc, rect.right - 9, Py - (int)(0.004166667*SizeY));

    dt = 0.0;
    T = 0.0;
    T1 = 0.0;
    X = 0.0;
    V = 0.0;
    V1 = 0.0;
    rr1.B = B;
    rr1.K = K;
    rr1.dx = dX;
    rr1.TimerStep = 0.0000002;
    rr1.Vend = 10 * Grad_to_Rad;
    rr1.Interval = 355;
    V0 = 5.0 * Grad_to_Rad;
    double XX = 5.0 * Grad_to_Rad;

    // вычислим время в которое пересекутся две функции:
    // x = X0 + V0*T
    // x = B*T*T/(1-K*T)
    // надо еще учесть текущую координату
    double TC = (((XX*K/2.0-V0)-sqrt((V0-K*XX/2.0)*(V0-K*XX/2.0)+ 2.0*XX*(B+V0*K)))/(-2.0*(V0*K+B)));
    
    //double XC = V0 * TC + XX;
    double XT = B * TC * TC / (1 - K * TC);
    //double XL = (XC - XT)- XX / 2;
    rr1.Xend = XT; // здесь удвоенная координата. т.к. после ускорения сразу идет торможение
    rr1.DataCount = 0;
    rr1.NextWriteTo = 0;
    rr1.NextReadFrom = 0;    
    rr1.XaccBeg = 0;
    rr1.Xbeg = 0;
    rr1.T1 = 0;
    rr1.TimeBeg = (ARR_TYPE)(0.0001*BUF_SIZE/rr1.TimerStep);
       
    rr1.State = ST_ACCELERATE;
    rr1.NextState = ST_RUN;
   
    do {		        
         switch(rr1.State){
         case ST_ACCELERATE:
             Acceleration(&rr1);                  
             if(rr1.State == ST_RUN){
                 rr1.Vend = 10.0 * Grad_to_Rad;
                 rr1.Xend = 10.0 * Grad_to_Rad;
                 rr1.NextState = ST_DECELERATE;
             }
             if(rr1.State == ST_DECELERATE){
                 rr1.Vend = 0;
                 rr1.Xend = 00.0 * Grad_to_Rad;
                 rr1.NextState = ST_STOP;
             }
             break;
         case ST_RUN:
             Run(&rr1);
             if(rr1.State == ST_DECELERATE){
                 rr1.Vend = 0.004166667 * Grad_to_Rad;
                 rr1.Xend = 20.0 * Grad_to_Rad;
                 rr1.NextState = ST_STOP;
             }
             break;
         case ST_DECELERATE:                
             Deceleration(&rr1); 
             if(rr1.State == ST_RUN){
                 rr1.Vend = V0;
                 rr1.Xend = 30.0 * Grad_to_Rad;
                 rr1.NextState = ST_STOP;
             }
             break;
         case ST_STOP:
             break;
        }
        //V = 5.0*Grad_to_Rad;
        for( i = 0; i < rr1.DataCount; i++) 
        {            
            T = rr1.IntervalArray[rr1.NextReadFrom] * rr1.TimerStep;
            rr1.NextReadFrom++;
            if(rr1.NextReadFrom >= BUF_SIZE) rr1.NextReadFrom -= BUF_SIZE;
             //V = 1.0*Grad_to_Rad;
             
                if(T-T1 != 0.0){
                    X += dX;
                    V = dX/(T-T1);
                }else {
                    V = 0.0;
                }
            X += dX;
//             A = (V-V1)/(T-T1);
            //  Формула для вычисления мгновенной скорости         
            //V2 = B * T *(2 - K * T) / ((1-K * T)*(1-K * T));
            //V2 = B*T/(1-K*T);
            X0 = XX + V0*T;

            K3 = (int)(T*SizeX);
            if( K3 != K1)
            {
                //Change the DC pen color                
                SetDCPenColor(hdc,RGB(0,L,255));
                MoveToEx(hdc, TX.x, TX.y, NULL);
                TX.x = Px + (int)(T*SizeX);
                TX.y = Py - (int)(X*Rad_to_Grad*SizeY/2);
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

                MoveToEx(hdc, TV2.x, TV2.y, NULL);
                TV2.x = Px + (int)(T*SizeX);
                TV2.y = Py - (int)(V2*Rad_to_Grad*SizeY);
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
                LineTo(hdc, TV2.x, TV2.y);  

                SetDCPenColor(hdc,RGB(255,0,0));
                MoveToEx(hdc, TA.x, TA.y, NULL);
                TA.x = Px + (int)(T*SizeX);
                TA.y = Py - (int)(A*Rad_to_Grad*SizeY/30)- (rect.bottom/2);
                LineTo(hdc, TA.x, TA.y);  

                SetDCPenColor(hdc,RGB(200,200,0));
                MoveToEx(hdc, X0T.x, X0T.y, NULL);
                X0T.x = Px + (int)(T*SizeX);
                X0T.y = Py - (int)(X0*Rad_to_Grad*SizeY/2);
                LineTo(hdc, X0T.x, X0T.y);                  
                
                K1 = K3;
            }
            T1 = T;
            V1 = V;
        }     
        rr1.DataCount = 0;
        L+=128;
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
    WORD k = 0;
    WORD m = 32;
    WORD i;
    WORD j;
    WORD FreeData = BUF_SIZE - rr->DataCount;  
    double X = 0.0;
    //X = rr->Vend * rr->TimeBeg;
    DWORD Xb = (DWORD)(rr->Xbeg/rr->dx);
    DWORD Xe = (DWORD)(rr->Xend/rr->dx);
    ARR_TYPE T = 0;
    ARR_TYPE T1 = 0;// = 0;
    ARR_TYPE T2 = 0;
    ARR_TYPE e;
    e = (ARR_TYPE)(0.000070 / rr->TimerStep); //70us

    for( i = 0; i < FreeData; i++) {       
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;                                              
        if(k == 0) {
            if(rr->Interval < e){
                //if(m < 32) m++;
                m = 32;
            } else {
                //if( m > 1) m--;
                m = 1;
            } 
            X += rr->dx*m;
            T2 = (ARR_TYPE)(X / (rr->Vend * rr->TimerStep));
            rr->Interval = (T2 - T1) / m;            
        }             
        Xb++;
        T += rr->Interval;
        if((Xe != 0)&&(Xb >= Xe)){
            FreeData = i;
            rr->State = rr->NextState;                                
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = rr->TimeBeg + T;                    
        T1 = T;            
    }

    rr->TimeBeg += T1;        
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
    ARR_TYPE T2 = 0;    
    ARR_TYPE dT = 0;
    double X;       // временная переменная 
    ARR_TYPE Tb = 0;  
    double D;      
    DWORD Xb = (DWORD)(rr->Xbeg/rr->dx);
    DWORD Xe = (DWORD)(rr->Xend/rr->dx);
    double K = rr->K;
    double B = rr->B;
    double VKpB = 0.0; 
    
    double dx;
    double a;
    double c;
    double d;    
    WORD i = 0;
    ARR_TYPE e;
    WORD k = 0;
    WORD m = 1;
    
    e = (ARR_TYPE)(0.000070 / rr->TimerStep); //70us
    dx = rr->dx;
    X = rr->XaccBeg * rr->dx; 
    d = K/(2.0 * B * rr->TimerStep);
    c = K*d;
    a = 4.0 * B/(K*K);   
//     Tb = rr->TimeBeg - rr->T1;       	
//     T1 = rr->T1;  
    if(rr->XaccBeg > 0){
        X = rr->XaccBeg * rr->dx; 
        D = X *(X + a);
        if(D >= 0.0){
            T1 = (ARR_TYPE)((-X - sqrt(D))*d);
        }  
    } else T1 = 0;
    Tb = rr->TimeBeg - T1;
    T = T1;

    // вычисление времени окончания
    if(rr->Vend != 0.0){       
        VKpB = rr->Vend * K + B;
        D = B * VKpB;
        dT = (ARR_TYPE)((-VKpB + sqrt(D))/(-K * VKpB * rr->TimerStep));
    }
    // оптимизировано 35 uSec (1431.5 тактов за шаг)
    for( i = 0; i < FreeData; i++) {       
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;                                              
        if(k == 0) {
            if(rr->Interval < e){
                //if(m < 32) m++;
                m = 32;
            } else {
                //if( m > 1) m--;
                m = 1;
            } 
            X += dx*m;
            D = X *(X + a);
            if(D >= 0.0){
                T2 = (ARR_TYPE)((-X - sqrt(D))*d);
            }                
            rr->Interval = (T2 - T1) / m;            
        }             
        Xb++;
        T += rr->Interval;
        if(((dT != 0)&&(T >= dT))||((Xe != 0)&&(Xb >= Xe))){
            FreeData = i;
            rr->State = rr->NextState;                                
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = Tb + T;                    
        T1 = T;            
    }
    rr->T1 = T1;
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
    ARR_TYPE T2 = 0;    
    ARR_TYPE dT = 0;
    double X;       // временная переменная 
    ARR_TYPE Tb = 0;  
    double D;      
    DWORD Xb = (ARR_TYPE)(rr->Xbeg/rr->dx);
    DWORD Xe = (ARR_TYPE)(rr->Xend/rr->dx);
    double K = rr->K;
    double B = rr->B;
    double VKpB = 0.0; 

    double dx;
    double a;
    double c;
    double d;    
    WORD i = 0;
    ARR_TYPE e;
    WORD k = 0;
    WORD m = 1;
    e = (ARR_TYPE)(0.000070 / rr->TimerStep); //70us
    dx = rr->dx;
    X = rr->XaccBeg * rr->dx; 
    d = K/(2.0 * B * rr->TimerStep);
    c = K*d;
    a = 4.0 * B/(K*K);    

//     Tb = rr->TimeBeg + rr->T1;       	
//     T1 = rr->T1;
    X = rr->XaccBeg * rr->dx; 
    if(rr->XaccBeg > 0){        
        D = X *(X + a);
        if(D >= 0.0){
            T1 = (ARR_TYPE)((-X - sqrt(D))*d);
        } 
    }
    Tb = rr->TimeBeg + T1;
    T = T1; 


    if(rr->Vend != 0.0){        
        VKpB = rr->Vend * K + B;
        dT = (ARR_TYPE)((-VKpB + sqrt(B * VKpB))/(-K * VKpB * rr->TimerStep));
    }

    for( i = 0; i < FreeData; i++) {    
        // "грубые" вычисления
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;                                              
        if(k == 0) {
            if(rr->Interval < e){                
                m = 32;
                //if( m < 32 ) m += 8;
            } else {
                //if( m > 1 ) m -= 8;
                m = 1;
            } 
            X -= dx*m;
            if(X<=0.0){
                FreeData = i;
                rr->State = rr->NextState;       
                break;
            }
            D = X *(X + a);
            if(D >= 0.0){
                T2 = (ARR_TYPE)((-X - sqrt(D))*d);
            }                
            rr->Interval = (T1 - T2) / m;
        }             
        Xb++;
        T -= rr->Interval;
        if(((dT != 0)&&(T <= dT))||((Xe != 0)&&(Xb >= Xe))){
            FreeData = i;
            rr->State = rr->NextState;                                
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = Tb - T;                    
        T1 = T;      
    }
    rr->T1 = T1;
    rr->TimeBeg = Tb - T1;
    rr->XaccBeg -= FreeData; 
    rr->DataCount += FreeData;
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->Xbeg = Xb * rr->dx;
    return 0;
}
