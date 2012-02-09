// guidance.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "guidance.h"
#include <math.h>
#include <stdio.h>
#include "..\dsPIC33\TCPIP Demo App\OCTimer.h"
#include "..\dsPIC33\protocol.h"
#include "..\PIC18F97J60\TCPIP Demo App\DisplayBuffer.h"


#define MAX_LOADSTRING 100

extern RR rr1;
extern RR rr2;
extern RR rr3;
extern BYTE DisplayBuffer0[256];
extern BYTE DisplayBuffer1[256];
extern BYTE DisplayBuffer2[256];
extern BYTE DisplayBuffer3[256];
extern BYTE DisplayBuffer4[40];

// Глобальные переменные:
#define FIRST_TIMER 1
int nTimerID;
HINSTANCE hInst;// текущий экземпляр
HWND hWindow;
HWND hwndDialog = NULL;

TCHAR szTitle[MAX_LOADSTRING];                          // Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];                    // имя класса главного окна

char Text[256] = "Test";

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                    MyRegisterClass(HINSTANCE hInstance);
BOOL                    InitInstance(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        KeyDialog(HWND, UINT, WPARAM, LPARAM);
void                    GetItemRect(HWND hDlg, RECT * rect, int nIDDlgItem);
 
void Calc(HWND hWnd, HDC hdc);
void DrawRRGraph(HDC hdc, RR * rr,POINT * TX, POINT * TV, DWORD SizeX, DWORD SizeY, DWORD Px, DWORD Py, double * TL, int * K);
void DrawMenu( LPPAINTSTRUCT ps, RECT * rect);

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
    if (!IsWindow(hwndDialog)) { 
        // окно в немодальном режиме                        
        hwndDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWindow, (DLGPROC)KeyDialog); 
        ShowWindow(hwndDialog, SW_SHOW);
    }
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

        wcex.style		= CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc	= WndProc;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= hInstance;
        wcex.hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUIDANCE));
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
                case IDM_KEYS:
                    if (!IsWindow(hwndDialog)) { 
                        // окно в немодальном режиме                        
                        hwndDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)KeyDialog); 
                        ShowWindow(hwndDialog, SW_SHOW);
                    }
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
// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK KeyDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;    
    RECT rect;
    PAINTSTRUCT ps;    
    static int X = 100;
    static int Y = 100;
    //HWND hDisplay = NULL;
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_PAINT:
        hdc = BeginPaint(hDlg, &ps); 

        GetItemRect(hDlg,&rect,IDC_STATIC);
        DrawMenu(&ps, &rect);

        EndPaint(hDlg, &ps);
    case WM_COMMAND:
        switch (LOWORD(wParam)){        
        case IDC_BUTTON_UP:
            {                
                X = 200;
                //GetItemRect(hDlg,&rect,IDC_STATIC);
                //hDisplay = GetDlgItem (hDlg, IDC_STATIC);
                char temp[] = ("No text");
                memset(Text,0,sizeof(Text));
                memcpy((void*)Text,(void*)temp,sizeof(temp));
                InvalidateRect(hDlg, NULL, FALSE);
            }
            break;
        case IDC_BUTTON_DOWN:
            {
                char temp[] ="Text";
                memset(Text,0,sizeof(Text));
                memcpy((void*)Text,(void*)temp,sizeof(temp));
                InvalidateRect(hDlg, NULL, FALSE);
            }
            break;
        case IDC_BUTTON_LEFT:
            break;
        case IDC_BUTTON_RIGHT:
            break;
        case IDC_BUTTON_ESC:
            break;
        case IDC_BUTTON_ENTER:
            break;
        case IDOK: 
        case IDCANCEL:
            {
                //EndDialog(hDlg, LOWORD(wParam));
                DestroyWindow(hDlg); 
                hwndDialog = NULL; 
                //return TRUE; 
                return (INT_PTR)TRUE;
            }            
            break;
        default:
            break;
        }
    }
    return (INT_PTR)FALSE;
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
    static double TL = 0;    // полное время
    static double T2 = 0;    // полное время
    static double T2L = 0;    // полное время
    

//    int K1 = -1;
//    int K3 = -1;
    //DWORD i = 0;
//    BYTE L = 255;
    RECT rect;
    

    static DWORD SizeX = 100;
    static DWORD SizeY = 10;
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
    
    DWORD Px = rect.left + 10;
    DWORD Py = rect.bottom - 10 - 30 * SizeY ;//- (rect.bottom/4);

    
    for (DWORD i = 0; i < rect.bottom/SizeY ; i++) {
        if(i % 10 == 0){
            SetDCPenColor(hdc,RGB(0,0,0));
            MoveToEx(hdc, rect.left + 10, rect.bottom - 10 - (int)(i*SizeY), NULL);
            LineTo(hdc,   rect.right -10, rect.bottom - 10 - (int)(i*SizeY) );  
        } else {
            SetDCPenColor(hdc,RGB(200,200,200));
            if(SizeY >= 10) {
                MoveToEx(hdc, rect.left + 10, rect.bottom - 10 - (int)(i*SizeY), NULL);
                LineTo(hdc,   rect.right -10, rect.bottom - 10 - (int)(i*SizeY) );  
            }
        }       
    }   
    for (DWORD i = 0; i < rect.right*10/(SizeX) ; i++) {
        if(i % 10 == 0){
            SetDCPenColor(hdc,RGB(0,0,0));
            MoveToEx(hdc, rect.left + 10 + (int)(i*SizeX/10), rect.bottom - 10, NULL);
            LineTo(  hdc, rect.left + 10 + (int)(i*SizeX/10), rect.top); 
        } else {
            SetDCPenColor(hdc,RGB(200,200,200));
            if(SizeX >= 20){
                MoveToEx(hdc, rect.left + 10 + (int)(i*SizeX/10), rect.bottom - 10, NULL);
                LineTo(  hdc, rect.left + 10 + (int)(i*SizeX/10), rect.top); 
            }
        }        
    }
//     WORD Year;
//     BYTE Month;
//     BYTE Day;
//     BYTE DayOfWeak;
//     BYTE Hour;
//     BYTE Min;
//     BYTE Sec;
//     double uSec;
    DateTime GDate = {2012,2,2,4,13,02,00,0};
    DateTime GDate1 = {0,0,0,0,0,0,0,0};
    int JDN;
    double JD;
    
     GDateToJD(GDate, &JDN, &JD);
     JDToGDate(JD, &GDate1);
    
    POINT TX = {Px,Py};
    POINT TV = {Px,Py};
    POINT TX2 = {Px,Py};
    POINT TV2 = {Px,Py};
    //POINT TV2 = {Px,Py};
    //POINT TA = {Px,Py};
//    POINT VA = {Px,Py};
    //POINT X0T = {Px,Py};

    dt = 0.0;
    T = 0.0;
    TL = 0.0;
    X = 0.0;
    V = 0.0;
    V1 = 0.0;
    V0 = 5.0 * Grad_to_Rad;
   
    unsigned char Text[20] = "Testing";
    unsigned char Text1[] = "_______________________";
    unsigned char Text2[] = " a: 06h 45m 08.9173s";
    unsigned char Text3[] = " d:-16  42' 58.017\"";
    unsigned char Text4[] = " Соединение установлено";
    
    DisplayInit();
    
//     OutTextXY(0,54,Text1,1); // ___    
//     OutTextXY(0,10,Text1,1); // ___
    //OutTextXY(0,38,Text2,1); // a
    OutTextXY(0,24,Text3,1); // d  	
    //OutTextXY(0,12,Text4,0); // msg
//    double XX = 5.0 * Grad_to_Rad;
/*
    // вычислим время в которое пересекутся две функции:
    // x = X0 + V0*T
    // x = B*T*T/(1-K*T)
    // надо еще учесть текущую координату
    double TC = (((XX*K/2.0-V0)-sqrt((V0-K*XX/2.0)*(V0-K*XX/2.0)+ 2.0*XX*(B+V0*K)))/(-2.0*(V0*K+B)));
    
    //double XC = V0 * TC + XX;
    double XT = B * TC * TC / (1 - K * TC);
    //double XL = (XC - XT)- XX / 2;
    rr1.Xend = XT; // здесь удвоенная координата. т.к. после ускорения сразу идет торможение
*/
    OCInit();
    PushCmdToQueue(&rr1, ST_ACCELERATE, 18.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr1, ST_RUN, 0.0, 45.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, 90.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr1, ST_ACCELERATE, 18.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr1, ST_RUN, 0.0, 16.69 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, -1);

    PushCmdToQueue(&rr2, ST_ACCELERATE, 18.0 * Grad_to_Rad, -180.0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr2, ST_RUN, 0.0, -45.0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr2, ST_DECELERATE, 0.0 * Grad_to_Rad, -90.0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr2, ST_ACCELERATE, 18.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr2, ST_RUN, 0.0, -16.69 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr2, ST_DECELERATE, 0.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, 1);

    //Control(&rr1);    
    //Control(&rr2);
    do {        
        static int K1 = 0;
        static int K2 = 0;
//         for( i = 0; i < rr1.DataCount; i++) 
//         {
            Control(&rr1);
            ProcessOC(&rr1);
            Control(&rr2);
            ProcessOC(&rr2);
            //ProcessOC(&rr3);
            DrawRRGraph(hdc, &rr1, &TX, &TV, SizeX, SizeY, Px, Py, &TL, &K1);
            DrawRRGraph(hdc, &rr2, &TX2, &TV2, SizeX, SizeY, Px, Py, &T2L, &K2);
    } while ((rr1.RunState != ST_STOP)||(rr1.CacheState != ST_STOP));
    //Restore original object.
    SelectObject(hdc,original);

}
void DrawRRGraph(HDC hdc, RR * rr,POINT * TX, POINT * TV, DWORD SizeX, DWORD SizeY, DWORD Px, DWORD Py, double * TL, int * K)
{
    double T = 0.0;
    //double TL = 0.0;
    double X = 0.0;
    double V = 0.0;    
    static int K3 = 0;



    T = (double)(rr->T.Val * rr->TimerStep);
    //V = 1.0*Grad_to_Rad;
    if(T - (*TL) != 0.0){
        X = rr->XPosition * rr->dx;
        if(rr->RunDir >=0 ){
            V = rr->dx/(T - (*TL));
        } else {
            V = -rr->dx/(T - (*TL));
        }
    } else {
        V = 0.0;
    }
//     X0 = XX + V0*T;
    K3 = (int)(T * SizeX);
    if( K3 != (*K))
    {
        //Change the DC pen color
        //SetDCPenColor(hdc,RGB(0,L,255));
        MoveToEx(hdc, TX->x, TX->y, NULL);
        TX->x = Px + (int)(T * SizeX);
        TX->y = Py - (int)(X * Rad_to_Grad * SizeY);
        if(rr1.RunState == ST_STOP){
            SetDCPenColor(hdc,RGB(0,100,100));
        }
        if(rr1.RunState == ST_RUN){
            SetDCPenColor(hdc,RGB(0,200,200));
        }
        if(rr1.RunState == ST_DECELERATE){
            SetDCPenColor(hdc,RGB(0,150,150));
        }
        if(rr1.RunState == ST_ACCELERATE){
            SetDCPenColor(hdc,RGB(0,255,255));
        }
        LineTo(hdc, TX->x, TX->y);  


        MoveToEx(hdc, TV->x, TV->y, NULL);
        TV->x = Px + (int)(T*SizeX);
        TV->y = Py - (int)(V*Rad_to_Grad * SizeY);
        if(rr->RunState == ST_STOP){
            SetDCPenColor(hdc,RGB(0,100,0));
        }
        if(rr->RunState == ST_RUN){
            SetDCPenColor(hdc,RGB(0,200,0));
        }
        if(rr->RunState == ST_DECELERATE){
            SetDCPenColor(hdc,RGB(0,150,0));
        }
        if(rr->RunState == ST_ACCELERATE){
            SetDCPenColor(hdc,RGB(0,255,0));
        }
        LineTo(hdc, TV->x, TV->y);  

//         SetDCPenColor(hdc,RGB(200,200,0));
//         MoveToEx(hdc, X0T.x, X0T.y, NULL);
//         X0T.x = Px + (int)(T*SizeX);
//         X0T.y = Py - (int)(X0*Rad_to_Grad*SizeY);
//         LineTo(hdc, X0T.x, X0T.y);

        (*K) = K3;
    }
    (*TL) = T;
}
void GetItemRect(HWND hDlg, RECT * rect, int nIDDlgItem)
{
    POINT topleft;
    POINT bottright;
    GetWindowRect (GetDlgItem (hDlg, nIDDlgItem), rect);
    topleft.x = rect->left;
    topleft.y = rect->top;
    bottright.x = rect->right;
    bottright.y = rect->bottom;
    ScreenToClient(hDlg, &topleft);
    ScreenToClient(hDlg, &bottright);
    rect->bottom = bottright.y;
    rect->left = topleft.x;
    rect->right = bottright.x;
    rect->top = topleft.y;
}



void DrawMenu( LPPAINTSTRUCT ps, RECT * rect)
{    
    int iWidth = rect->right - rect->left;
    int iHeight = rect->bottom - rect->top;
    HDC hMemDC; 
    HBITMAP hbmScreen = NULL;
    BOOL b = FALSE;
    hMemDC = CreateCompatibleDC(ps->hdc);

    BitBlt(hMemDC, 0, 0, iWidth, iHeight, ps->hdc, rect->left, rect->top, SRCCOPY);

    hbmScreen = CreateCompatibleBitmap(ps->hdc, iWidth, iHeight);
    SelectObject(hMemDC, hbmScreen);

    SetPixelDB(7, 7, true);    
    
    for(int i = 0; i < iHeight; i++)
        for(int j = 0; j < iWidth; j++){
            b = GetPixelDB(j/3, i/3);
            if(!b){
                SetPixel(hMemDC, j, i, RGB(255,255,255));
            } else {
                SetPixel(hMemDC, j, i, RGB(0,0,0));
            }
        }
    // быстро копируем результат отрисовки
    BitBlt(ps->hdc, rect->left,rect->top, iWidth, iHeight, hMemDC, 0, 0, SRCCOPY);
    // освобождаем контекст
    DeleteDC(hMemDC);
    DeleteObject(hbmScreen);

}