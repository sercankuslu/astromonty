// guidance.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "guidance.h"
#include <math.h>
#include <winsock2.h>
#include <stdio.h>
#include "..\dsPIC33\TCPIP Demo App\OCTimer.h"
//#include "..\dsPIC33\protocol.h"
// #include "..\PIC18F97J60\TCPIP Demo App\DisplayBuffer.h"
//#include "..\PIC18F97J60\TCPIP Demo App\Control.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 200
#define DEFAULT_PORT "9764"

#define MAX_LOADSTRING 100


extern RR rr1;
extern RR rr2;
extern RR rr3;
//extern ALL_PARAMS Params;

// Глобальные переменные:
#define FIRST_TIMER 1
#define SECOND_TIMER 2
#define TH_TIMER 3
#define FIRST_TIMER_INTERVAL 200 
#define SECOND_TIMER_INTERVAL 1000 
#define TH_TIMER_INTERVAL 1000 
int nTimerID;
HINSTANCE hInst;// текущий экземпляр
HWND hWindow;
HWND hwndDialog = NULL;

TCHAR szTitle[MAX_LOADSTRING];                          // Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];                    // имя класса главного окна

char Text[256] = "Test";
typedef DWORD
/*struct DRAW_BUF {
    DWORD Value;
    LONG Pos;
    GD_STATE State;
} 
*/
DRAW_BUF;

DWORD Buf1Size = 0;
DWORD Buf2Size = 0;
DWORD Buf3Size = 0;

BUF_TYPE DrawT1Buffer[576000];
BUF_TYPE DrawT2Buffer[576000];
BUF_TYPE DrawT3Buffer[576000];

//double DrawVBuffer[576000];
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                    MyRegisterClass(HINSTANCE hInstance);
BOOL                    InitInstance(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        KeyDialog(HWND, UINT, WPARAM, LPARAM);
void                    GetItemRect(HWND hDlg, RECT * rect, int nIDDlgItem);
 
void Calc();
//void DrawRRGraph(HDC hdc, DRAW_BUF * Buf, DWORD BufSize, DWORD SizeX, DWORD SizeY, POINT * TX, DWORD Px, DWORD Py);
void DrawRRGraph(HDC hdc, double dx, double TimerStep, BUF_TYPE * Buf, DWORD BufSize, DWORD SizeX, DWORD SizeY, DWORD Px, DWORD Py);
void DrawIface( LPPAINTSTRUCT ps, RECT * rect);
int BerkleyClient();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                         HINSTANCE hPrevInstance,
                         LPTSTR    lpCmdLine,
                         int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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

    //SetTimer(hWindow, TH_TIMER, TH_TIMER_INTERVAL, (TIMERPROC) NULL);
    if (!IsWindow(hwndDialog)) { 
        // окно в немодальном режиме                        
        //hwndDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWindow, (DLGPROC)KeyDialog); 
        //ShowWindow(hwndDialog, SW_SHOW);
        //SetTimer(hwndDialog, FIRST_TIMER, FIRST_TIMER_INTERVAL, (TIMERPROC) NULL);
        //SetTimer(hwndDialog, SECOND_TIMER, SECOND_TIMER_INTERVAL, (TIMERPROC) NULL);
    }

    typedef union {
        BYTE Val:5;
        struct __16
        {        
            BYTE Step:4;
            BYTE b8:4;
        } b16;
        struct __8
        {        
            BYTE b0:1;
            BYTE Step:3;
            BYTE b8:4;
        } b8;
        struct __4
        {        
            BYTE b0:2;
            BYTE Step:2;
            BYTE b8:4;
        } b4;
        struct __2
        {        
            BYTE b0:3;
            BYTE Step:1;
            BYTE b8:4;
        } b2;
        struct __1
        {        
            BYTE b0:4;
            BYTE Step:1;
            BYTE b8:3;
        } b1;
    } TEST_STRUCT; 
    volatile TEST_STRUCT B;
    B.Val = 0;
    BYTE B16 = 0;
    BYTE B4 = 0;
    BYTE B2 = 0;
    BYTE B1 = 0;
    for(int i = 0; i<64;i++){
        if((B16 == 5)&&(B4 == 5)&&(B2 == 5)&&(B1 == 5)){
            break;
        }        
        B.Val--;
        B16 = B.b16.Step;
        B4 = B.b4.Step;
        B2 = B.b2.Step;
        B1 = B.b1.Step;
    }

    OCSetup();
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
        Calc();
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
        //RECT rect;
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
                        //ShowWindow(hwndDialog, SW_SHOW);
                        //SetTimer(hwndDialog, FIRST_TIMER, FIRST_TIMER_INTERVAL, (TIMERPROC) NULL);
                        //SetTimer(hwndDialog, SECOND_TIMER, SECOND_TIMER_INTERVAL, (TIMERPROC) NULL);
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
                switch (wParam) { 
                case TH_TIMER: 
                    //Calc();	
                    InvalidateRect(hWnd, NULL, TRUE);
                    k = true;
                }
                break;
         case WM_PAINT:{
                hdc = BeginPaint(hWnd, &ps);
                static DWORD SizeX = 10;
                static DWORD SizeY = 10;
                DWORD Px;
                DWORD Py;
                HGDIOBJ original = NULL;
                RECT rect;               
                original = SelectObject(hdc,GetStockObject(DC_PEN)); 
                GetClientRect(hWnd, &rect);    
                MoveToEx(hdc, rect.left+9, rect.bottom - 9, NULL);
                LineTo(hdc, rect.right - 9, rect.bottom - 9);
                MoveToEx(hdc, rect.left+9, rect.bottom - 9, NULL);
                LineTo(hdc, rect.left+9, rect.top);  
                Px = rect.left + 10 + 100;
                Py = rect.bottom - 10 - 00 * SizeY ;//- (rect.bottom/4);
                DrawRRGraph(hdc, rr1.dx, rr1.TimerStep, DrawT1Buffer, Buf1Size, SizeX, SizeY, Px, Py);
                //DrawRRGraph(hdc, rr2.dx, rr2.TimerStep, DrawT2Buffer, Buf2Size, SizeX, SizeY, Px, Py); 
                //DrawRRGraph(hdc, rr3.dx, rr3.TimerStep, DrawT3Buffer, Buf3Size, SizeX, SizeY, Px, Py);                 
                SelectObject(hdc,original);
                EndPaint(hWnd, &ps);
                break;
        }
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
{/*
    HDC hdc;    
    RECT rect;
    PAINTSTRUCT ps;    
    static int X = 5;
    static int Y = 2;
    static bool bb = false;
    static KEYS_STR Key;
    ST_RESULT rv = STR_OK;  
    static bool OnLine = FALSE;
    static BYTE bfr[64] = "";
    static int length = 0;

    //HWND hDisplay = NULL;
    UNREFERENCED_PARAMETER(lParam);
    
    switch (message) {
    case WM_TIMER: 
        switch (wParam) 
        { 
        case FIRST_TIMER: 
            ProcessMenu(&Key);
            //ExecuteCommands();
            GetItemRect(hDlg,&rect,IDC_STATIC);
            if(OnLine){
                CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
                if(BerkleyClient() < 0) {
                    OnLine = FALSE;
                } 
            } else {
                CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
                //rv = RunClient((BYTE*)bfr, sizeof(bfr), &length);            
                //rv = RunServer(0, (BYTE*)bfr, sizeof(bfr), &length);            
            }
            InvalidateRect(hDlg, &rect, false);
            break;
        case SECOND_TIMER:
            
            break;
        }
        break;
    case WM_INITDIALOG:        
        ProcessMenu(&Key);
        return (INT_PTR)TRUE;
    case WM_PAINT:
        //GetItemRect(hDlg,&rect,IDC_STATIC);        
        GetItemRect(hDlg,&rect,IDC_STATIC);
        Params.Common.Flags.bits.NeedToRedrawMenus = 1;
        ProcessMenu(&Key);
        hdc = BeginPaint(hDlg, &ps); 
        DrawIface(&ps, &rect);
        EndPaint(hDlg, &ps);
        break;
    case WM_COMMAND:{

        //SetPixelDB(X, Y, false);
        //XorImage(X,Y,4,8,b);

        switch (LOWORD(wParam)){        
        case IDC_BUTTON_UP:
            {      
                if(Y>0) Y--;
                Key.keys.up = 1;
                
            }
            break;
        case IDC_BUTTON_DOWN:
            {      
                if(Y<SIZE_Y) Y++;
                Key.keys.down = 1;
            }
            break;
        case IDC_BUTTON_LEFT:
            {      
                if(X>0) X--;
                Key.keys.left = 1;
            }
            break;
        case IDC_BUTTON_RIGHT:
            {      
                if(X<SIZE_Y) X++;
                Key.keys.right = 1;
            }
            break;
        case IDC_BUTTON_ESC:
            Key.keys.esc = 1;
            break;
        case IDC_BUTTON_ENTER:
            Key.keys.enter = 1;
            break;
        case IDC_BUTTON_CON:
            Params.Local.Status ^= 1;
            //Params.Local.IsModified.bits.Flag = 1;
            break;
        case IDC_BUTTON_A:
            //Params.Alpha.StatusFlag.bits.Run ^= 1;
            //Params.Alpha.IsModified.bits.Flag = 1;
            break;
        case IDC_BUTTON_D:
            //Params.Delta.StatusFlag.bits.Run ^= 1;
            //Params.Delta.IsModified.bits.Flag = 1;
            break;
        case IDC_BUTTON_G:
            //Params.Gamma.StatusFlag.bits.Run ^= 1;
            //Params.Gamma.IsModified.bits.Flag = 1;
            break;
        case IDC_BUTTON_CON2:
            //Params.Local.ConnectFlag ^= 1;
            //BerkleyClient();            
            break;
        case IDC_BUTTON_A2:
            //Params.Alpha.StatusFlag.bits.Enable ^= 1;
//             Params.Alpha.IsModified.bits.Flag = 1;
//             Params.Alpha.IsModified.bits.Angle = 1;
            break;
        case IDC_BUTTON_D2:
            //Params.Delta.StatusFlag.bits.Enable ^= 1;
//             Params.Delta.IsModified.bits.Flag = 1;
//             Params.Delta.IsModified.bits.Angle = 1;
            break;
        case IDC_BUTTON_G2:
            //Params.Gamma.StatusFlag.bits.Enable ^= 1;
//             Params.Gamma.IsModified.bits.Flag = 1;
//             Params.Gamma.IsModified.bits.Angle = 1;
            break;
        case IDC_CHECK1:
            OnLine = IsDlgButtonChecked(hDlg, IDC_CHECK1);            
            break;
        case IDOK: 
        case IDCANCEL:
            {                
                DestroyWindow(hDlg); 
                hwndDialog = NULL; 
                return (INT_PTR)TRUE;
            }            
            break;
        default:            
            break;
        }

        {                         
            //ProcessMenu(&Key);
            GetItemRect(hDlg,&rect,IDC_STATIC);
            InvalidateRect(hDlg, &rect, false);
            //RedrawWindow(hDlg,&rect,NULL,RDW_INTERNALPAINT);
        }       
        }   
    default:;
        
    }    */
    return (INT_PTR)FALSE;
}

void Calc()
{
    
    //DisplayInit();

    //ST_COMMANDS Cmd = STC_REQEST_DATA;
    //ST_FLAGS Flag = STF_OK;
    //ST_RESULT rv;
    BYTE Len = 0;
    BYTE * Value = NULL;
    BYTE * pPointer = NULL;
    //ST_ATTRIBUTE_TYPE  Attribute = STA_ALPHA;
    

    char Str[] = "All good!";
    BYTE * Str2 = NULL;
    

    // //    double XX = 5.0 * Grad_to_Rad;
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
    WORD Buf[256];
    WORD BufSize = 256;

    OCSetup(); 
    rr1.MaxSpeed = 10.0 * Grad_to_Rad;
    rr1.MinAngle = 0;
    rr1.MaxAngle = 180.0 * Grad_to_Rad;
    rr1.CacheState = ST_ACCELERATE;
    rr1.CacheCmdCounter = 32000;

    Buf1Size = sizeof(DrawT1Buffer)/sizeof(DrawT1Buffer[0]);
    Buf1Size = rr1.CacheCmdCounter;
    CalculateMove(&rr1,DrawT1Buffer,Buf1Size);
    //ProcessCmd(&rr1);
    //PushCmdToQueue(&rr1, ST_ACCELERATE, 10.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(&rr1, ST_RUN, 10.0 * Grad_to_Rad, 15.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(&rr1, ST_STOP, 0.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, 1);
    //for(;;){
    //    if(Control(&rr1, Buf, BufSize))
    //        break;
    // }
//     rr1.XPosition = 10.0 * Grad_to_Rad / rr1.dx;
//     rr2.XPosition = 0;
    //rr3.XPosition = 0;
//     rr1.VMax = 10.0 * Grad_to_Rad;
//     rr1.LastCmdV = 0.0;
//     rr1.LastCmdX = 20.0 * Grad_to_Rad;
//     rr1.T.Val = 0;
//     rr1.TimeBeg = 0;
//     rr3.VMax = 10.0 * Grad_to_Rad;
//     rr3.LastCmdV = 0.0;
//     rr3.LastCmdX = 0.0;
//     rr3.T.Val = 0;
//     rr3.TimeBeg = 0;
    //GoToCmd(&rr1, 0.0 * Grad_to_Rad, 20.0 * Grad_to_Rad, 0); 
    //GoToCmd(&rr1, 0.0004166667 * Grad_to_Rad, 10.0 * Grad_to_Rad, 0);
    //GoToCmd(&rr2, 0.0 * Grad_to_Rad, 20.0 * Grad_to_Rad, 0);
//     rr2.LastCmdV = 1.0 * Grad_to_Rad;
//     rr2.LastCmdX = 10.0 * Grad_to_Rad;
//     rr2.XPosition = rr2.LastCmdX/rr2.dx;
    //PushCmdToQueue(&rr2, ST_RUN, 1.0 * Grad_to_Rad,  180 * Grad_to_Rad, 1);
    
/*
    PushCmdToQueue(&rr1, ST_ACCELERATE, 20.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr1, ST_RUN, 0.0 * Grad_to_Rad,  45.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr1, ST_ACCELERATE, 20.0 * Grad_to_Rad, 0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr1, ST_RUN, 0.0 * Grad_to_Rad,  0.0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, -180 * Grad_to_Rad, -1);

    PushCmdToQueue(&rr2, ST_ACCELERATE, 10 * Grad_to_Rad, 180 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr2, ST_RUN, 10.0 * Grad_to_Rad,  20 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr2, ST_DECELERATE, 0.0 * Grad_to_Rad, 180 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr2, ST_ACCELERATE, 10 * Grad_to_Rad, 0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr2, ST_RUN, 10.0 * Grad_to_Rad,  0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr2, ST_DECELERATE, 0.0 * Grad_to_Rad, -180 * Grad_to_Rad, -1);

    PushCmdToQueue(&rr3, ST_ACCELERATE, 5 * Grad_to_Rad, 180 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr3, ST_RUN, 5.0 * Grad_to_Rad,  10 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr3, ST_DECELERATE, 0.0 * Grad_to_Rad, 180 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr3, ST_ACCELERATE, 5 * Grad_to_Rad, 0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr3, ST_RUN, 5.0 * Grad_to_Rad,  0 * Grad_to_Rad, -1);
    PushCmdToQueue(&rr3, ST_DECELERATE, 0.0 * Grad_to_Rad, -180 * Grad_to_Rad, -1);
   */
    /*
    int k = 0;
    k = 0;
    int t = 0;
    if(rr1.CmdCount>0){
        Buf1Size = sizeof(DrawT1Buffer)/sizeof(DrawT1Buffer[0]);
        for(DWORD i = 0; i < Buf1Size;i++){         
            //Control(&rr1);
            //ProcessOC(&rr1);
            DrawT1Buffer[i].Value = rr1.T.Val;
            DrawT1Buffer[i].Pos = rr1.XPosition;
            DrawT1Buffer[i].State = rr1.RunState;

            if(rr1.RunState == ST_STOP) {
                Buf1Size = i;
                break;
            }
            if(rr1.RunState == ST_DECELERATE) {
                k = 1;
            }
            if(rr1.RunState == ST_RUN) {                
                if((rr1.XPosition>30000)&&(t==0)) {
                    //GoToCmd(&rr1, 0.0004166667 * Grad_to_Rad, 12.0 * Grad_to_Rad, 0);
                    t = 1;
                }
            }
        }
    }
    if(rr2.CmdCount>0){
        Buf2Size = sizeof(DrawT2Buffer)/sizeof(DrawT2Buffer[0]);
        for(DWORD i = 0; i < Buf2Size;i++){         
            //Control(&rr2);
            //ProcessOC(&rr2);
            DrawT2Buffer[i].Value = rr2.T.Val;
            DrawT2Buffer[i].State = rr2.RunState;
            DrawT2Buffer[i].Pos = rr2.XPosition;
            if(rr2.RunState == ST_STOP) {
                Buf2Size = i;
                break;
            }
        }
    }
    if(rr3.CmdCount>0){
        Buf3Size = sizeof(DrawT3Buffer)/sizeof(DrawT3Buffer[0]);
        for(DWORD i = 0; i < Buf3Size;i++){         
            //Control(&rr3);
            //ProcessOC(&rr3);
            DrawT3Buffer[i].Value = rr3.T.Val;
            DrawT3Buffer[i].State = rr3.RunState;
            DrawT3Buffer[i].Pos = rr3.XPosition;
            if(rr3.RunState == ST_STOP) {
                Buf3Size = i;
                break;
            }
        }
    }*/
}
void DrawRRGraph(HDC hdc, double dx, double TimerStep, BUF_TYPE * Buf, DWORD BufSize, DWORD SizeX, DWORD SizeY, DWORD Px, DWORD Py)
{
    POINT TX = {Px,Py};
    POINT TX1 = {Px,Py};
    POINT TV = {Px,Py};
    POINT TV1 = {Px,Py};
    POINT TA = {Px,Py};
    POINT TA1 = {Px,Py};

    POINT Tx = {Px,Py};
    POINT Tx1 = {Px,Py};
    POINT Tv = {Px,Py};
    POINT Tv1 = {Px,Py};
    POINT Ta = {Px,Py};
    POINT Ta1 = {Px,Py};

    DWORD KX = 0;
    DWORD LX = 0;    
    DWORD KV = 0;
    DWORD LV = 0;  
    DWORD KA = 0;
    DWORD LA = 0;  
    DWORD T = 0;
    DWORD T1 = 0;
    DWORD T2 = 0;
    double DX = 0;
    double DY = 0;  
    double V = 0;
    double V1 = 0;
    double dT = 0;
    double A = 0;
    double K = rr1.K;
    double B = rr1.B;
    double TT = 0;
    double dV = 0;
    SizeY = 20;
    SizeX = 20;
    MoveToEx(hdc, TX.x, TX.y, NULL);
    for(DWORD i = 0; i < BufSize; i++)
    {  
       
        T2 = T1;
        T1 = Buf[i].R;
        //while(T1 <= T){
        //    T1 += 65536;
        //}
        if(T2 == T1){
            break;
        }
        T = T1;
        DX = (T * TimerStep * 10000/ SizeX);
        DY = (i * dx * 1000 / SizeY);
        TX.x = Px + (int)DX;
        TX.y = Py - (int)DY;
        KX = TX.x;
        TT = T * TimerStep;
        Tx.x = Px + (int)DX;
        Tx.y = Py - (int)(B*1000*TT * TT/((1-K*TT)*SizeY));
        Tv.x = Px + (int)DX;
        Tv.y = Py - (int)(B*1000*TT*(2-K*TT)/((1-K*TT)*(1-K*TT)*SizeY));
        Ta.x = Px + (int)DX;
        Ta.y = Py - (int)(B*2*1000/((1-K*TT)*(1-K*TT)*(1-K*TT)*SizeY));
        //if(KX!=LX)
        {
            //LX = KX;
            //SetPixel(hdc, TX->x, TX->y,RGB(0,150,150));
            SetDCPenColor(hdc,RGB(0,0,250));
            MoveToEx(hdc, TX1.x, TX1.y, NULL);
            LineTo(hdc, TX.x, TX.y);
            TX1 = TX;
        }

        //if(i>1)
        {
            dT = (T1 - T2) * TimerStep;
            
            if(dT > 0.0){
                V1 = V;
                V = dx / dT;
                //dV = (V - V1);
                //A = dV / dT;  
            }
                        
            TV.x = Px + (int)DX;
            TV.y = Py - (int)(V * 1000) / SizeY;

           

            //TA.x = Px + (int) DX;
            //TA.y = Py - (int)(A * 1 / SizeY);

            KV = TV.x;
            if(KV!=LV)
            {
                LV = KV;
                SetDCPenColor(hdc,RGB(0,250,0));
                MoveToEx(hdc, TV1.x, TV1.y, NULL);
                LineTo(hdc, TV.x, TV.y);
                TV1 = TV;
                //SetDCPenColor(hdc,RGB(250,0,0));
                //MoveToEx(hdc, TA1.x, TA1.y, NULL);
                //LineTo(hdc, TA.x, TA.y);
                //TA1 = TA;
                SetDCPenColor(hdc,RGB(0,0,128));
                MoveToEx(hdc, Tx1.x, Tx1.y, NULL);
                LineTo(hdc, Tx.x, Tx.y);
                Tx1 = Tx;
                SetDCPenColor(hdc,RGB(0,128,0));
                MoveToEx(hdc, Tv1.x, Tv1.y, NULL);
                LineTo(hdc, Tv.x, Tv.y);
                Tv1 = Tv;
                SetDCPenColor(hdc,RGB(128,0,0));
                MoveToEx(hdc, Ta1.x, Ta1.y, NULL);
                LineTo(hdc, Ta.x, Ta.y);
                Ta1 = Ta;
            }
        }
       
        
        //T += Buf[i].R;
    }   
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



void DrawIface( LPPAINTSTRUCT ps, RECT * rect)
{    /*
    LONG iWidth = rect->right - rect->left;
    LONG iHeight = rect->bottom - rect->top;
    int R = 2;
    LONG BegX = (iWidth - 133 * R) / 2;
    LONG BegY = (iHeight - 64 * R) / 2;
    HDC hMemDC; 
    HBITMAP hbmScreen = NULL;
    BOOL b = FALSE;
    
    hMemDC = CreateCompatibleDC(ps->hdc);

    BitBlt(hMemDC, 0, 0, iWidth, iHeight, ps->hdc, rect->left, rect->top, WHITENESS);

    hbmScreen = CreateCompatibleBitmap(ps->hdc, iWidth, iHeight);
    SelectObject(hMemDC, hbmScreen);  
    FloodFill(hMemDC,1,1,RGB(255,255,255));
    for(LONG i = 0; i < 64; i++)
        for(LONG j = 0; j < 133; j++){
            b = GetPixelDB((WORD)j, (WORD)i);
            if(!b){
                for(LONG k = 0; k < R; k++)
                    for(LONG l = 0; l < R; l++)
                        SetPixel(hMemDC, BegX + j * R + k, BegY + i * R + l, RGB(255,255,255));
            } else {
                for(LONG k = 0; k < R; k++)
                    for(LONG l = 0; l < R; l++)
                        SetPixel(hMemDC, BegX + j * R + k, BegY + i * R + l, RGB(0,0,0));
            }
        }    
    // быстро копируем результат отрисовки
    BitBlt(ps->hdc, rect->left,rect->top, iWidth, iHeight, hMemDC, 0, 0, SRCCOPY);
    // освобождаем контекст
    DeleteDC(hMemDC);
    DeleteObject(hbmScreen);
*/
}

int BerkleyClient()
{/*
    //----------------------
    // Declare and initialize variables.
    WSADATA wsaData;
    int iResult;

    static SOCKET ConnectSocket;
    static struct sockaddr_in clientService; 
    volatile int err;

    char recvbuf[DEFAULT_BUFLEN] = "";
    int recvbuflen = DEFAULT_BUFLEN;
    static bool f = true;
    int i = 0; 
    static enum
    {
        BSD_INIT = 0,
        BSD_START,
        BSD_CONNECT,        
        BSD_RECEIVE,
        BSD_OPERATION,
        BSD_SEND,
        BSD_CLOSE
    } BSDClientState = BSD_INIT;
    switch(BSDClientState)
    {
    case BSD_INIT:
        {
            //----------------------
            // Initialize Winsock
            iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
            if (iResult != NO_ERROR) {
                BSDClientState = BSD_CLOSE; 
                break;
            }
            //SetClientDisconnect();
            BSDClientState = BSD_START;
            // no break
        }
    case BSD_START:
        {
            //----------------------
            // Create a SOCKET for connecting to server
            ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (ConnectSocket == INVALID_SOCKET) {
                BSDClientState = BSD_CLOSE; 
                break;
            }
            BSDClientState = BSD_CONNECT;
            // no break
        }
    case BSD_CONNECT:
        {
            //----------------------
            // The sockaddr_in structure specifies the address family,
            // IP address, and port of the server to be connected to.
            clientService.sin_family = AF_INET;
            clientService.sin_addr.s_addr = inet_addr( "192.168.1.21" );
            clientService.sin_port = htons( 9764 );

            //----------------------
            // Connect to server.
            iResult = connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) );
            if ( iResult == SOCKET_ERROR) {
                BSDClientState = BSD_CLOSE; 
                break;
            }            
            BSDClientState = BSD_OPERATION;
            break;
        }
    case BSD_RECEIVE:
        {   
            struct timeval timeout = {1, 0};
            fd_set Set;            
            Set.fd_count = 1;
            Set.fd_array[0] = ConnectSocket;
            if(select(0,&Set, NULL, NULL, &timeout)) {                
                i = recv(ConnectSocket, recvbuf, recvbuflen, 0);
                if(i<0){
                    err = WSAGetLastError();
                }
            }
            BSDClientState = BSD_OPERATION; 
        }
    case BSD_OPERATION:
        {
            ST_RESULT res =  STR_NEED_DISCONNECT;//RunClient((BYTE*)recvbuf, recvbuflen, &i);
            switch(res){
            case STR_NEED_ANSWER:  
                i = send(ConnectSocket, recvbuf, i, 0);       
                if(i<0){
                    err = WSAGetLastError();
                }

            case STR_OK:
                BSDClientState = BSD_RECEIVE;
                break;
            case STR_NEED_DISCONNECT:	            	
            default:
                BSDClientState = BSD_CLOSE;            
            }            
        }  
        break;
    case BSD_CLOSE:
        {
            iResult = shutdown(ConnectSocket, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                printf("shutdown failed: %d\n", WSAGetLastError());                
            }
            // cleanup
            closesocket(ConnectSocket);
            WSACleanup();
            //SetClientDisconnect();
            BSDClientState = BSD_INIT; 
            return -1;
        }
        break;
    }*/
    return 0;
}