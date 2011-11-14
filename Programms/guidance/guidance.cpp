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
static double Accelerate[ACCELERATE_SIZE];

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
 

void Calc(HWND hWnd, HDC hdc);
double M(double F);
int SolvQuadratic(double A, double B, double C, double* X1, double* X2);
int Calculate_dT(double Xbeg, double Xend, double V, double A, double* T);
int Calculate_A(DWORD F, double *A);
int InitAccelerate(FREQ_POWER* FreqPower, WORD Len, double I);


int APIENTRY _tWinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: разместите код здесь.
		{
			// I = (m*r^2/4) + (m*l^2)/12
			static double Mass = 500.0f;
			static double Radius = 0.30f;
			static double Length = 2.0f;
			static double Reduction = 360.0f;
			static double Grad_to_Rad = 180.0/PI;
			double I = ((Mass*Radius*Radius/4) + (Mass*Length*Length/12))/Reduction; 
			//double L = (2 * Reduction * Grad_to_Rad)/(Mass*Radius*Radius);
			InitAccelerate(FreqPower, sizeof(FreqPower)/sizeof(FreqPower[0]), I);
		}
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
			InvalidateRect(hWnd, &rect, TRUE);
			k = true;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
				//LineTo(hdc, 100,100);
		// TODO: добавьте любой код отрисовки...

				
		if(k)
		{
			double X = 200+100*sin(T*PI/180);
			double Y = 200+100*cos(T*PI/180);
			MoveToEx(hdc, 200,200, NULL);
			LineTo(hdc,(int)X, (int)Y);
			k=false;
		} else Calc(hWnd, hdc);	
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
	{
		
		double A = 0.0; //ускорение в радианах в сек за сек
		double dt = 0.0; // изменение времени
		double V = 0.0;  // мгновенная скорость в радианах
		//static double dX = 1.0/(200.0*16.0);// шаг перемещения в градусах (в 1 градусе 3200 шагов)
		static double dX = PI/(180.0*200.0*16.0); // шаг перемещения в радианах
		double X = 0;    // полное перемещение в радианах
		double T = 0;    // полное время
		int K = 0;       // флаги для оптимизации вывода
		int K1 = 0;
		int K2 = 0;
		int K3 = 0;
		double timer1 = 0;  // значение таймера
		DWORD F = 0;
		DWORD i = 0;
		DWORD k = 16;
		RECT rect;
        double Vf = 180 * 200/PI;
		char RRR[256];
        GetClientRect(hWnd, &rect);
        MoveToEx(hdc, rect.left+9, rect.bottom - 9, NULL);
        LineTo(hdc, rect.right, rect.bottom - 9);
        MoveToEx(hdc, rect.left+9, rect.bottom - 9, NULL);
        LineTo(hdc, rect.left+9, rect.top);        
        do{
			if(k>=16)
			{
				F =(DWORD)(V * Vf);
				Calculate_A(F, &A);
				Calculate_dT(0, dX*16, V, A, &dt);
				V = dX*16/dt;				
				k=0;
			}
			k++;			
			T += dt/16;
			X += dX;
			timer1 = T/0.0000002; // результат вычислений
			K = (int)(T*500.0);
			K2 = (int)(V*20.0);
			if((K != K1)||(K2!=K3)) {
				K1 = K;
				K3 = K2;
				SetPixel(hdc, rect.left + 10 + (int)(T*500), rect.bottom - 10 - (int)(X*180*20/PI), RGB(0,0,255));
				SetPixel(hdc, rect.left + 10 + (int)(T*500), rect.bottom - 10 - (int)(V*180*20/PI), RGB(0,255,0));
				SetPixel(hdc, rect.left + 10 + (int)(T*500), rect.bottom - 10 - (int)((A)*180*5/PI), RGB(255,0,0));                
				SetPixel(hdc, rect.left + 10 + (int)(T*500), rect.bottom - 10 - (int)(100), RGB(255,0,255));
				MoveToEx(hdc, rect.left + 9 + (int)(T)*500, rect.bottom - 9, NULL);
				LineTo(  hdc, rect.left + 9 + (int)(T)*500, rect.top); 
			}
			//if(X>PI) break;
			i++;
		}while ( i<32000);
		sprintf(RRR,"%d",i);
		TextOutA(hdc, 0,0, (char*)RRR, strlen(RRR));
		sprintf(RRR,"%fs",T);
		TextOutA(hdc, 100,0, (char*)RRR, strlen(RRR));
//         i=0;        
//         do{
//             F =(DWORD)(V * Vf);
//             Calculate_A(F, &A);            
//             Calculate_dT(0, dX, V, -A, &dt);
//             V = dX/dt;
//             X += dX;
//             T += dt;
//             timer1 = T/0.0000002; // результат вычислений
//             K = (int)(T*500.0);
//             K2 = (int)(V*20.0);
//             if((K != K1)||(K2!=K3)) {
//                 K1 = K;
//                 K3 = K2;
//                 SetPixel(hdc, rect.left + 10 + (int)(T*500), rect.bottom - 10 - (int)(X*180*20/PI), RGB(0,0,255));
//                 SetPixel(hdc, rect.left + 10 + (int)(T*500), rect.bottom - 10 - (int)(V*180*20/PI), RGB(0,255,0));
//                 SetPixel(hdc, rect.left + 10 + (int)(T*500), rect.bottom - 10 - (int)((A)*180*5/PI), RGB(255,0,0));                
//                 SetPixel(hdc, rect.left + 10 + (int)(T)*500, rect.bottom - 10 - (int)(100), RGB(255,0,255));
//             }
//             //if(X>PI) break;
//             i++;
//        }while ( i< 11000);
	}
}
// должна возвращать значение ускорения в зависимости от скорости
// F - частота полных шагов
// L - момент инерции системы
int Calculate_A(DWORD F, double *A)
{
	DWORD f = (DWORD)(F/FREQ_STEP);
	if(f < ACCELERATE_SIZE){
		*A = Accelerate[f];
	} else {
		*A = Accelerate[ACCELERATE_SIZE-1];
	}
	return 0;
}

// функция возвращает время, прошедшее
// при движении с заданными параметрами на промежутке (Xbeg,Xend), 
// при начальной скорости V и ускорении A
int Calculate_dT(double Xbeg, double Xend, double V, double A, double* T)
{
	double T1 = 0.0;
	double T2 = 0.0;
	int res = 0;
	if((A==0.0)&&(V==0.0)){
		return -1; // ошибка
	}
	if((A == 0)&&(V != 0.0)){
		*T = (Xend-Xbeg)/V;
		return 0;  // нет ошибки
	}
	// Xend = Xbeg + V*T + (A * T^2)/2
	// (A/2)T^2 + V*T + (Xbeg - Xend) = 0
	res = SolvQuadratic(A/2,V,(Xbeg-Xend),&T1, &T2); 
	if(res > 0) {
		*T = T1;
		return 0;
	}
	else return 1;
}
// вычисляет корени квадратного уравнения
int SolvQuadratic(double A, double B, double C, double* X1, double* X2)
{
	// Ax^2+Bx+C=0
	double D = 0.0;
	if((X1 == NULL) || (X2 == NULL)) return -1; // ошибка: неверные указатели
	*X1 = 0.0;
	*X2 = 0.0;
	if(A == 0.0){
		// Bx+C = 0
		// x = -C/B
		if(C!=0.0){
			*X1 = -B/C;
		} else {
			*X1 = 0.0;
		}
		return 1; // не квадратное уравнение, один корень
	}
	D = B*B - 4.0 * A * C;
	if(D<0.0) return 0; // дискриминант 0 корней нет

	*X1 = (-B+sqrt(D))/(2*A);
	*X2 = (-B-sqrt(D))/(2*A);
	return 2; // два корня уравнения
}
/**************************************************************************
 * M = Ia =>   a = M/I
 * M - момент силы
 * I - момент инерции
 * a - угловое ускорение в радианах в секунду за секунду
 * для альфы: ( сплошной цилиндр длинны l, радиуса r и массы m, ось перпендикулярна целиндру и проходит через его середину)
 * I = (m*r^2/4) + (m*l^2)/12  
 * 
 * для дельты: ( полый тонкостенный цилиндр длинны l, радиуса r и массы m, ось перпендикулярна целиндру и проходит через его середину)
 * I = (m*r^2/2)+ (m*l^2/12)
 **************************************************************************
 * Параметры функции
 * Power - массив моментов силы двигателя
 * Freq  - массив частот шагов двигателя
 * Len   - размер массивов
 * I	 - Момент инерции  
 * 
 ************************************************************************/
int InitAccelerate(FREQ_POWER* FreqPower, WORD Len, double I)
{    
	double Lm = 0.0;
	int i;
	int j;
    int k = 0;
    WORD Freq1;
    WORD Freq2;
    BYTE b = 0;
	WORD F = 0;
	for(j = 0; j < ACCELERATE_SIZE; j++){
		for(int i = k; i< Len-1; i++){
            Freq1 = FreqPower[i].Freq;  
            Freq2 = FreqPower[i+1].Freq;            
			if((F >= Freq1) && (F <= Freq2)){
				Lm = LinInt(Freq1,FreqPower[i].Power,Freq2,FreqPower[i+1].Power, F);
                b = 1;
                k = i;
				break;
			}
		}
		if(b == 0) 
            Lm = FreqPower[Len - 1].Power;
		F += FREQ_STEP;
		Accelerate[j] = Lm / I;
	}
	return 0;
}
