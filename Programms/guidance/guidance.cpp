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
#define FIRST_TIMER_INTERVAL 100 
#define SECOND_TIMER_INTERVAL 1000 
#define TH_TIMER_INTERVAL 100
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

DWORD Buf1Size = 150;
DWORD Buf2Size = 0;
DWORD Buf3Size = 0;

double CurrTime = 0.0;

OC_RECORD DrawT1Buffer[500000];
//OC_RECORD DrawT2Buffer[576000];
//OC_RECORD DrawT3Buffer[576000];

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
void DrawRRGraph(HDC hdc, double dx, double TimerStep, OC_RECORD * Buf, DWORD BufSize, DWORD SizeX, DWORD SizeY, DWORD Px, DWORD Py);
void DrawIface( LPPAINTSTRUCT ps, RECT * rect);
int BerkleyClient();


TIMERPROC ThTimerProc(HWND hWnd, UINT a, UINT_PTR b, DWORD c)
{
    CurrTime += TH_TIMER_INTERVAL * 0.001;
    if(CurrTime > 15.0){
        CurrTime = 0.0;
        InvalidateRect(hWnd, NULL, TRUE);
    }
    InvalidateRect(hWnd, NULL, FALSE);
    return 0;
}




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

    SetTimer(hWindow, TH_TIMER, TH_TIMER_INTERVAL, (TIMERPROC) ThTimerProc);
    if (!IsWindow(hwndDialog)) { 
        // окно в немодальном режиме                        
        //hwndDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWindow, (DLGPROC)KeyDialog); 
        //ShowWindow(hwndDialog, SW_SHOW);
       // SetTimer(hwndDialog, FIRST_TIMER, FIRST_TIMER_INTERVAL, (TIMERPROC) NULL);
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
//          case WM_TIMER:  
//                 switch (wParam) { 
//                 case TH_TIMER: 
//                     CurrTime += TH_TIMER_INTERVAL * 0.001;
//                     if(CurrTime > 15.0){
//                         CurrTime = 0.0;
//                         InvalidateRect(hWnd, NULL, TRUE);
//                     }
//                     //Calc();	
//                     InvalidateRect(hWnd, NULL, FALSE);
//                     //k = true;
//                 }
//                 break;
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
                Px = rect.left + 10;
                Py = rect.bottom - 10 - rect.bottom /2  ;//- (rect.bottom/4);

                MoveToEx(hdc, Px - 1, Py - 1, NULL);
                LineTo(hdc, rect.right - 9, Py - 1);
                MoveToEx(hdc, Px - 1, rect.bottom - 9, NULL);
                LineTo(hdc, Px - 1, rect.top);  

                DrawRRGraph(hdc, PI / (360 * 200 * 16), 0.0000002, DrawT1Buffer, Buf1Size, rect.right, rect.bottom, Px, Py);
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


/*
typedef struct _iobuf {
    char *_ptr;
    int   _cnt;
    char *_base;
    int   _flag;
    int   _file;
    int   _charbuf;
    int   _bufsiz;
    char *_tmpfname;
} FILE;

// первая очередь
FILE *  fopen ( const char * filename, const char * mode );  //Open file
FILE *  freopen ( const char * filename, const char * mode, FILE * stream ); //Reopen stream with different file or mode
int     fclose ( FILE * stream );   //Close file
int     fsetpos ( FILE * stream, const fpos_t * pos );  //Set position indicator of stream
int     fseek ( FILE * stream, long int offset, int origin );   //Reposition stream position indicator
int     feof ( FILE * stream );     //Check end-of-file indicator
long int ftell ( FILE * stream );   //Get current position in stream
size_t  fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );   //Write block of data to stream
size_t  fread ( void * ptr, size_t size, size_t count, FILE * stream );  //Read block of data from stream
int     fflush ( FILE * stream );   //Flush stream
// вторая очередь или не реализуем
int     ferror ( FILE * stream );   //Check error indicator
int     fprintf ( FILE * stream, const char * format, ... ); //Write formatted data to stream
int     fscanf ( FILE * stream, const char * format, ... ); //Read formatted data from stream
int     fgetc ( FILE * stream );    //Get character from stream
int     fputc ( int character, FILE * stream ); //Write character to stream
char *  fgets ( char * str, int num, FILE * stream );   //Get string from stream
int     fputs ( const char * str, FILE * stream );  //Write string to stream
*/

#define FS_SECTOR_SIZE  256
#define FS_SECTOR_MASK  FS_SECTOR_SIZE-1
#define SECTOR_SIZE 256*1024
#define FS_INDERECT1_SIZE (FS_SECTOR_SIZE*FS_SECTOR_SIZE/sizeof(WORD))  // 32768 (32Kib)
#define FS_INDERECT2_SIZE (FS_SECTOR_SIZE*FS_SECTOR_SIZE*FS_SECTOR_SIZE/sizeof(DWORD))  // 4194304 (4 Mib)
#define FS_FILE_MAX_SIZE (FS_SECTOR_SIZE + FS_INDERECT1_SIZE + FS_INDERECT2_SIZE)    //4227328 (4.031 Mib)

// формат записи файла в таблице файлов
typedef struct _FILE_RECORD
{
    WORD ID;                        // индекс (у корневой 1, 0 у удаленных файлов/папок)
    WORD ParentId;                  // индекс папки
    WORD Data;                      // прямая адресация данных (у папок 0xFFFF)
    WORD TableLv1;                  // обычная косвенная адресация (у папок 0xFFFF)
    WORD TableLv2;                  // двойная косвенная адресация (у папок 0xFFFF)
    DWORD dataSize;                 // размер данных (у папок 0xFFFF)
    DWORD DataCRC;                  // crc16 данных файла (у папок 0xFFFF)
    BYTE Name[16];                  // имя файла
} FILE_RECORD;

/*
 * Вариант 2
 * с первого сектора файловой системы начинается файл таблицы файлов
 * и занимает первые 4 сектора + 1 сектор таблицы секторов
 * первая запись в этом файле-это сам файл таблицы
 * вторая запись в файле-файл свободного места
 * файл свободного места распределен по физическим секторам флеш памяти
 * 1 сектор файла на 1 сектор памяти, причем в первом секторе находится 1 сектор файла СМ + таблица секторов файла СМ
 * файл оссвободившегося места распределен по физическим секторам флеш памяти
 * 1 сектор файла на 1 сектор памяти, причем в первом секторе находится 1 сектор файла ОМ + таблица секторов файла ОМ
 * 24+256+256+256+256(в каждом секторе начинаем с 24 байта и занимаем 128 байт)
 * 
 *
 *
 *
 *
 */

typedef enum _FS_MODE {
    FS_NONE, FS_READ, FS_WRITE, FS_APPEND, FS_READ_UPDATE, FS_WRITE_UPDATE, FS_APPEND_UPDATE
} FS_MODE;
typedef enum _FS_RW_MODE {
    RW_NONE, RW_TEXT, RW_BINARY
}FS_RW_MODE;

#define FLAG_FOLDER 1
#define FLAG_FILE 2
DWORD CreateCheckSum(BYTE * val, WORD len);

extern BYTE FileSystem[64*1024*256];
char FSName[]   = "uFS";
char FFName[]   = ".bitmap";
DWORD GetFreeSector();  // возвращает первый свободный сектор и помечает его занятым в таблице
DWORD GetFreeSectors(WORD Count);  // возвращает первый сектор из цепочки свободных секторов длинной Count и помечает их занятыми в таблице

//-------------------------------------------------------------------------
void CreateFileSystem()
{
// формируем таблицу файлов
    BYTE TmpBuf[FS_SECTOR_SIZE];
    FILE_RECORD * FSRecord = (FILE_RECORD*)TmpBuf;
    WORD * Sector = NULL;
    WORD i;
    BYTE * SecMask = NULL;
    BYTE b = 0;
    //DWORD Time = 0;
#ifdef _WINDOWS_
    //Time = GetCurrentTime();
#endif
    memset(TmpBuf,0xff, sizeof(TmpBuf));
    FS_ClearAll();

    // имя ФС
    // первая запись-сам файл таблицы
    FSRecord->ID = 1;
    FSRecord->ParentId = 1;
    FSRecord->Data = 0;
    FSRecord->TableLv1 = 8;
    memcpy(FSRecord->Name,FSName,sizeof(FSName));
    FSRecord->dataSize = 0xFFFFFFFF;
    FSRecord->DataCRC = 0xFFFFFFFF;


    // вторая запись - файл свободного места
    FSRecord++;
    FSRecord->ID = 2;
    FSRecord->ParentId = 1;
    FSRecord->Data = 9;
    FSRecord->TableLv1 = 10;
    memcpy(FSRecord->Name,FFName,sizeof(FFName));
    //FSRecord->Date = Time;
    FSRecord->dataSize = 16386;
    FSRecord->DataCRC = 0xFFFFFFFF;

    FS_WriteArray(0, TmpBuf, FS_SECTOR_SIZE);
    memset(TmpBuf,0xff, sizeof(TmpBuf));
    
    // таблица секторов ТФ
    Sector = (WORD*)TmpBuf;
    for(i = 1; i<8; i++){
        *Sector = i;
        Sector++;
    }

    FS_WriteArray(FS_SECTOR_SIZE*8, TmpBuf, FS_SECTOR_SIZE);    
    memset(TmpBuf,0xff, sizeof(TmpBuf));


    // ФСМ
    SecMask = (BYTE*)TmpBuf;
    // заняты 11 секторов
    *SecMask++ = 0x00;
    *SecMask++ = 0xF8;

    FS_WriteArray(FS_SECTOR_SIZE*9, TmpBuf, FS_SECTOR_SIZE);
    memset(TmpBuf,0xff, sizeof(TmpBuf));

    // таблица секторов ФСМ
    Sector = (WORD*)TmpBuf;
    b = 0xFE;
    for(i = 1; i<64; i++){
        *Sector = i*1024;
        FS_WriteArray((*Sector)*FS_SECTOR_SIZE, &b, 1);
        Sector++;
    }

    FS_WriteArray(FS_SECTOR_SIZE*10, TmpBuf, FS_SECTOR_SIZE);
    memset(TmpBuf,0xff, sizeof(TmpBuf));
}
/*
 *        создать, если нет
 *        | удалить файл, если есть/не удалять файл
 *        | | текстовый(0)/бинарный(1)
 *        | | | запись в конец/запись по указателю
 *        | | | | чтение разрешено
 *        | | | | | запись разрешена
 *        | | | | | |
 *   |7|6|5|4|3|2|1|0|
 */
typedef union {
    BYTE Val;
    //BYTE_VAL VAL;
    struct {
        unsigned RE:1;
        unsigned WE:1;
        unsigned WrToEnd:1;
        unsigned Binary:1;
        unsigned Renew:1;
        unsigned New:1;
        unsigned b6:1;
        unsigned b7:1;
    } FILESTATEbits;
}FILESTATE;

typedef struct _FS_FILE{
    WORD ID;                // индекс файла
    FILESTATE State;
    DWORD FileRecordAddr;   // адрес файловой записи в таблице файлов
    FILE_RECORD FileRecord; // файловая запись из таблицы файлов

    DWORD FilePos;          // указатель файла
    DWORD FilePosAddr;      // реальный адрес указателя файла

    BYTE *Buf;              // буфер записи
} FS_FILE;
#define MAX_FS_HANDLE 20
FS_FILE FileHandles[MAX_FS_HANDLE];

// нужны:
// 1. указатель в таблице секторов
// 2. указатель в текущем секторе
//
//


/* fsetpos
 * 1. прочитать из записи файла номер сектора табицы таблиц 
 * 2. прочитать из таблицы таблиц номер сектора таблицы секторов (127 записей на сектор)
 * 3. прочитать из таблицы секторов номер сектора данных (128 записей на сектор)
 * 4. установить абсолютный адрес
 * 
 * нужно хранить для каждого указателя файла (для четения и записи)
 * 1. адрес записи файла (WORD) в файле таблицы файлов
 * 2. номер сектора таблицы секторов файла (WORD)
 * 3. номер сектора файла (WORD)
 * 4.
 */
int FS_fsetpos ( FS_FILE * stream, const DWORD * pos )  //Set position indicator of stream
{
    //FS_SECTOR_SIZE*FS_SECTOR_SIZE/2*(FS_SECTOR_SIZE/2-1)
    DWORD DataPos = *pos & (DWORD)FS_SECTOR_MASK;
    DWORD SectorTablePos = *pos/FS_SECTOR_SIZE;
    DWORD TableTablePos = SectorTablePos * 2/FS_SECTOR_SIZE;
    return 0;
}

// вычисляет адрес данных в файле по смещению
DWORD FS_GetAddr(FS_FILE * stream)
{
    DWORD DataPos;                                          // число, которое нужно прибавить к адресу сектора данных
    DWORD SectorTablePos;                                   // число, которое нужно прибавить к адресу сектора таблицы секторов
    DWORD TableTablePos;                                    // число, которое нужно прибавить к адресу сектора таблицы таблиц
    WORD SectorAddr;
    WORD TableTableAddr;
    DWORD Pos = stream->FilePos;
    
    if(Pos < FS_SECTOR_SIZE){
        return stream->FilePosAddr = (stream->FileRecord.Data * FS_SECTOR_SIZE) + Pos; // Адрес первого сектора лежит в записи файла
    } else {
        Pos -= FS_SECTOR_SIZE;
    }
    
    // 128*256 простая косвенная адресация
    if(Pos < FS_INDERECT1_SIZE){
        DataPos = Pos & (DWORD)FS_SECTOR_MASK;              // число, которое нужно прибавить к адресу сектора данных
        SectorTablePos = Pos/FS_SECTOR_SIZE;                // число, которое нужно прибавить к адресу сектора таблицы секторов
        // читаем адрес сектора данных. Адрес таблицы лежит в записи файла
        FS_ReadArray( (stream->FileRecord.TableLv1 * FS_SECTOR_SIZE + SectorTablePos * sizeof(WORD)), (BYTE*)&SectorAddr, sizeof(SectorAddr));
        return stream->FilePosAddr = (SectorAddr * FS_SECTOR_SIZE + DataPos);
    } else {
        Pos -= FS_INDERECT1_SIZE;
    }

    // 128*128*256 ( двойная косвенная адресация)
    if(Pos < FS_INDERECT2_SIZE){ 
        DataPos = Pos & (DWORD)FS_SECTOR_MASK;              // число, которое нужно прибавить к адресу сектора данных
        SectorTablePos = Pos * 2 / FS_SECTOR_SIZE;
        TableTablePos  = SectorTablePos * 2 / FS_SECTOR_SIZE;
        // читаем адрес сектора таблицы. Адрес таблицы лежит в записи файла
        FS_ReadArray( (stream->FileRecord.TableLv2 * FS_SECTOR_SIZE + TableTablePos * sizeof(WORD)), (BYTE*)&TableTableAddr, sizeof(TableTableAddr));
        // читаем адрес сектора данных
        FS_ReadArray( (TableTableAddr * FS_SECTOR_SIZE + SectorTablePos * sizeof(WORD)), (BYTE*)&SectorAddr, sizeof(SectorAddr));
        return stream->FilePosAddr = (SectorAddr * FS_SECTOR_SIZE + DataPos);
    }

    return (DWORD)EOF;
}

// внутреннее открытие существующего файла 
// инициализация структуры FS_FILE
// прочитать запись файла
// прочитать первую запись из корневой таблицы
// прочитать первую запись из таблицы секторов
int FS_OpenFile( FS_FILE * stream, DWORD RecordAddr)
{
    WORD TAddr;
    WORD SectorAddr;

    // адрес файловой записи в таблице файлов
    stream->FileRecordAddr = RecordAddr;   
    
    // файловая запись из таблицы файлов
    FS_ReadArray( RecordAddr, (BYTE*)&stream->FileRecord, sizeof(FILE_RECORD));
    stream->FilePos = 0;
    return 0;
}

// создаёт файл и возвращает адрес файловой записи
int FS_CreateFile( FS_FILE * stream, DWORD * RecordAddr)
{
    return 0;
}

// удаляет файл
int FS_RemoveFile( FS_FILE * stream)
{
    return 0;
}


// Buf  буфер
// Count количество байт
int FS_ReadFile( FS_FILE * stream, BYTE * Buf, WORD Count)
{
    DWORD DataPos = stream->FilePosAddr & (DWORD)FS_SECTOR_MASK;
    DWORD DataLeft = Count;
    while(DataLeft>0){
        if(DataPos + Count < FS_SECTOR_SIZE){
            FS_ReadArray( stream->FilePosAddr, Buf, Count);
            stream->FilePos += Count;
            FS_GetAddr(stream);
            DataLeft -= Count;
        } else {

        }
    }
    return 0;
}

int FS_GetFreeRecord(DWORD * RecordAddr)
{
    //FS_FILE * MFT = FileHandles[0];
    //if(MFT->ID != 0x0001){
    //    FS_OpenFile( MFT, 0);
   // }
    return 0;
}   

// возвращает первый свободный сектор и помечает его занятым в таблице
DWORD GetFreeSector()
{
    return 0;
}

// возвращает первый сектор из цепочки свободных секторов длинной Count и помечает их занятыми в таблице
DWORD GetFreeSectors(WORD Count)
{
    return 0;
}


//-------------------------------------------------------------------------
// копирует в Name имя папки или файла из строки пути 
// типа "/folder1/folder2/file1.txt". 
// в Name должно быть минимум 13 байт(8+.+3+\0). 
// (максимально допустимое количество символов в имени папки или файла)
// возвращает адрес следующего '/' после возвращенного имени, 
// или адрес '\0' если строка закончилась
// в случае ошибки возвращает NULL
//-------------------------------------------------------------------------
char * getNameFromPath(char * Path, char * Name)
//-------------------------------------------------------------------------
{
    if((Path == NULL) || (Name == NULL) || (Path[0]!= '/'))
        return 0;
    Path++;
    while ((*Path != '/')&&((*Path != '\0')))
    {
        *Name++ = *Path++;
    }
    *Name = '\0';
    return Path;
}
//-------------------------------------------------------------------------
// ищет файл. возвращает позицию записи файла на диске
DWORD fsearch(char * Name)
{
    return 0;
}
/*
typedef struct _FS_iobuf {
    char *_ptr;
    int   _cnt;
    char *_base;
    int   _flag;
    int   _file;
    int   _charbuf;
    int   _bufsiz;
    char *_tmpfname;
} FS_FILE;
*/
/*
"r"     read: Open file for input operations. The file must exist.
"w"     write: Create an empty file for output operations. If a file with the same name already exists, 
        its contents are discarded and the file is treated as a new empty file.
"a"     append: Open file for output at the end of a file. Output operations always 
        write data at the end of the file, expanding it. Repositioning operations (fseek, fsetpos, rewind) 
        are ignored. The file is created if it does not exist.
"r+"    read/update: Open a file for update (both for input and output). The file must exist.
"w+"    write/update: Create an empty file and open it for update (both for input and output). 
        If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.
"a+"    append/update: Open a file for update (both for input and output) with all output 
        operations writing data at the end of the file. Repositioning operations 
        (fseek, fsetpos, rewind) affects the next input operations, but output operations 
        (move the position back to the end of file. The file is created if it does not exist.
text:
r, r+, rt, rt+, r+t
w, w+, wt, wt+, w+t
a, a+, at, at+, a+t

binary:
rb, rb+, r+b
wb, wb+, w+b
ab, ab+, a+b

/folder/filename.bin
./filename.bin

Если файл на запись, выделяем буфер
*/

FS_FILE *  FS_fopen ( const char * filename, const char * mode )   //Open file
{
    int i;
    FILESTATE State;
    State.Val = 0;
    if ((filename == NULL)||(mode == NULL)) {
        return NULL;
    }
    
    for (i = 0; i < 3; i++)
    {
        switch (mode[i])
        {
        // взаимоисключающие
        case 'r':
            State.FILESTATEbits.RE = 1;
            State.FILESTATEbits.WE = 0;
            State.FILESTATEbits.Renew = 0;
            State.FILESTATEbits.WrToEnd = 0;
            State.FILESTATEbits.New = 0;
            break;
        case 'w':
            State.FILESTATEbits.WE = 1;
            State.FILESTATEbits.RE = 0;
            State.FILESTATEbits.Renew = 1;
            State.FILESTATEbits.WrToEnd = 0;
            State.FILESTATEbits.New = 1;
            break;
        case 'a':
            State.FILESTATEbits.WE = 1;
            State.FILESTATEbits.RE = 0;
            State.FILESTATEbits.Renew = 0;
            State.FILESTATEbits.WrToEnd = 1;
            State.FILESTATEbits.New = 1;
            break;
        // модификаторы
        case '+':
            State.FILESTATEbits.WE = 1;
            State.FILESTATEbits.RE = 1;
            break;
        case 'b':
            State.FILESTATEbits.Binary = 1;
            break;
        case 't':
            State.FILESTATEbits.Binary = 0;
            break;
        // конец строки
        case '\0':
            i = 3;
            break;
        default:
            return NULL;
        }
    }
    return NULL;
}


/* Запись файла:
 * fopen
 * 1. получить свободный сектор
 * 2. Записать заголовок
 * fwrite
 * 3. Начать писать данные в загловок
 * 4. Если данных больше, чем влазит в первый сектор,запросить свободный сектор, записать в заголовок указатель на первый сектор данных
 * 5  записать в свободный сектор данные
 * 6. если данных больше, чем помещается в сектор, запросить свободный сектор. Записать в заголовок указатель на первый сектор таблицы,
 * записать в таблицу адрес первого сектора файла и первого сектора данных.
 * 7. при заполнении сектора таблицы, за исключением последних 2 байт, запросить свободный сектор и в последние 2 байта таблицы записать значение адреса полученного сектора.
 * fclose
 * 8. если данных больше нет, записать размер файла в первый сектор
 */


// от скорости 0 до 0.2 градуса в сек делитель на 256

void Calc()
{
    /*
    //DisplayInit();

    //ST_COMMANDS Cmd = STC_REQEST_DATA;
    //ST_FLAGS Flag = STF_OK;
    //ST_RESULT rv;
    BYTE Len = 0;
    BYTE * Value = NULL;
    BYTE * pPointer = NULL;
    //ST_ATTRIBUTE_TYPE  Attribute = STA_ALPHA;
    DWORD a = RoundShiftRight(35, 20);

    char Str[] = "All good!";
    BYTE * Str2 = NULL;
    BYTE FSName1[] = "qqqqqqqqqqqqqqqq";
    BYTE FSName2[] = "wwwwwwwwwwwwwwww";
    BYTE FSName3[] = "eeeeeeeeeeeeeeee";

    WORD a1 = Crc16((BYTE*)FSName1, sizeof(FSName1));
    WORD a2 = Crc16((BYTE*)FSName2, sizeof(FSName2));
    WORD a3 = Crc16((BYTE*)FSName3, sizeof(FSName3));
    //CreateFileSystem(256, 256*1024);
    //WORD Config = DMACreateConfig(SIZE_BYTE, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, ONE_SHOT);
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
* /
    //WORD Buf[256];
    //WORD BufSize = 256;

    // работа от таймера TMR3(6.4us) от интервала 16777216(0.00074град/сек) до интервала 65536(0.2град/сек)
    // работа от таймера TMR2(25ns)  от интервала 65535(0.2град/сек) до интервала 1250(10град/сек)
    const char Name1[] = "RR1";
    BYTE Buf[256];
    FS_FILE stream;
    FS_FILE stream2;
    DWORD pos = sizeof(FILE_RECORD);
    */
    
    
    
    //OCSetup(); 
//     CreateFileSystem();
//     FS_OpenFile( &stream, 0);
//     DWORD Addr = FS_GetAddr(&stream);    
//     FS_ReadFile( &stream, Buf, pos);
//     FS_ReadFile( &stream, Buf, pos);
//     FS_OpenFile( &stream2, 1*pos);
//     stream2.FilePos = 1024;
//     Addr = FS_GetAddr(&stream2);


    //FS_fsetpos ( &stream, &pos );  //Set position indicator of stream
    //ReadFromFile(0, 8, Buf, 8);
    /*
    char * r;
    char * r1;
    char * r2;
    char * r3;
    char Fold[13];
    char path[] = "/style/style.css";
    FS_FILE * file;
    FILESTATE State;

// 
     /*file = FS_fopen ( path, "r" );
     file = FS_fopen ( path, "w" );
     file = FS_fopen ( path, "a" );
     file = FS_fopen ( path, "r+" );
     file = FS_fopen ( path, "w+" );
     file = FS_fopen ( path, "a+" );
     file = FS_fopen ( path, "rb" );
     file = FS_fopen ( path, "wb" );
     file = FS_fopen ( path, "ab" );
     file = FS_fopen ( path, "rb+" );
     file = FS_fopen ( path, "wb+" );
     file = FS_fopen ( path, "ab+" );
     file = FS_fopen ( path, "r+b" );
     file = FS_fopen ( path, "w+b" );
     file = FS_fopen ( path, "a+b" );
     file = FS_fopen ( path, "rt" );
     file = FS_fopen ( path, "wt" );
     file = FS_fopen ( path, "at" );
     file = FS_fopen ( path, "rt+" );
     file = FS_fopen ( path, "wt+" );
     file = FS_fopen ( path, "at+" );
     file = FS_fopen ( path, "r+t" );
     file = FS_fopen ( path, "w+t" );
     file = FS_fopen ( path, "a+t" );*/



    /*
    ProcessCmd(&rr1);
    return;
    rr1.MaxSpeed = 10.0 * Grad_to_Rad;
    rr1.MinAngle = 0;
    rr1.MaxAngle = 180.0 * Grad_to_Rad;
    rr1.CacheState = ST_ACCELERATE;
    rr1.CacheCmdCounter = 32000;

    Buf1Size = sizeof(DrawT1Buffer)/sizeof(DrawT1Buffer[0]);
    Buf1Size = rr1.CacheCmdCounter;
    */

    //CalculateMove(&rr1,DrawT1Buffer,Buf1Size);
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
    / *
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
void DrawRRGraph(HDC hdc, double dx, double TimerStep, OC_RECORD * Buf, DWORD BufSize, DWORD SizeX, DWORD SizeY, DWORD Px, DWORD Py)
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
    double T = 0;
    double T1 = 0;
    double T2 = 0;
    DWORD T3 = 0;
    LONG X1 = 0;
    //SizeY = 20;
    //SizeX = 20;
    double K = -3.384858359;
    double B = 40.27981447;
    double V = 0;
    double X = 0;
    double U = 1.0;
    double TmrStep = 0.0000002;
    DWORD StepsPerRound = 3200;
    double Dx = 1.0/(double)StepsPerRound;//180.0 / (360.0 * 200.0 * 16.0); //0.1125;
    double A = 0.0;// = U * (K*V + B);
    double tx = 0.0;// = (-V+sqrt(V * V + 2.0 * A * Dx))/(A);
    double R = 1.0;
    double a = 0.0;
    double a1 = 0.0;
    bool Pixel = true;

    MoveToEx(hdc, TX.x, TX.y, NULL);

    double Mx = 100;
    double My = 9;
    OC_INTERVAL_CALC Intrerval;
    Intrerval.A = A;
    Intrerval.V = V;
    Intrerval.K = K;
    Intrerval.B = B;
    Intrerval.AccSign = U;
    Intrerval.Rev = R;
    Intrerval.Dx = Dx;
    Intrerval.Tstep = TmrStep;
    Intrerval.MaxSpeed = 100.0;


    for(DWORD i = 0; i < StepsPerRound*120; i++)
    {  
        if( (i == StepsPerRound*20)
            //||(i == StepsPerRound*30)
            //||(i == StepsPerRound*40)
            //||(i == StepsPerRound*50)
            ||(i == StepsPerRound*60)
            //||(i == StepsPerRound*70)
            //||(i == StepsPerRound*80)
            //||(i == StepsPerRound*90)
            ||(i == StepsPerRound*100)
            ) 
           Intrerval.AccSign = -Intrerval.AccSign;

        T3 = GetMinInterval( &Intrerval);
        if(T3 > 65535){
            SetPixel(hdc, 50, 50, RGB(250,0,0));
        }

        if(Intrerval.A <= 0.1){
            //Intrerval.AccSign = -Intrerval.AccSign;
            //break;
        }

        if(Intrerval.V >= 10.0){
            //break;
        }
        //DrawT1Buffer[i].r = T3;
        tx = Intrerval.T1;//T3 * Intrerval.Tstep;
        X += Intrerval.Dx * Intrerval.Rev;
        T += tx;// / 0.0000002;
        a = (Intrerval.A - a1)/tx;
        a1 = Intrerval.A;

        TX.x = Px + T * Mx;
        TX.y = Py - X * My;

        TV.x = Px + T * Mx;
        TV.y = Py - Intrerval.Rev * Intrerval.V * My;

        TA.x = Px + T * Mx;
        TA.y = Py - Intrerval.A * My;

        Ta.x = Px + T * Mx;
        Ta.y = Py - a * My;

        if(i == 0) TA1 = TA;

        if((TX.x != TX1.x)||(TX.y != TX1.y)){
            if(Pixel)
                SetPixel(hdc, TX.x, TX.y, RGB(0,0,250));
            else{
                SetDCPenColor(hdc,RGB(0,0,250));
                MoveToEx(hdc, TX1.x, TX1.y, NULL);
                LineTo(hdc, TX.x, TX.y);
            }
            TX1 = TX;
        }

        if((TV.x != TV1.x)||(TV.y != TV1.y)){
            if(Pixel)
                SetPixel(hdc, TV.x, TV.y, RGB(0,250,0));
            else {
                SetDCPenColor(hdc,RGB(0,250,0));
                MoveToEx(hdc, TV1.x, TV1.y, NULL);
                LineTo(hdc, TV.x, TV.y);
            }
            TV1 = TV;
        }
        if((TA.x != TA1.x)||(TA.y != TA1.y)){
            if(Pixel)
                SetPixel(hdc, TA.x, TA.y, RGB(250,0,0));
            else {
                SetDCPenColor(hdc,RGB(250,0,0));
                MoveToEx(hdc, TA1.x, TA1.y, NULL);
                LineTo(hdc, TA.x, TA.y);
            }
            TA1 = TA;
        }

        if((Ta.x != Ta1.x)||(Ta.y != Ta1.y)){
            if(Pixel)
                SetPixel(hdc, Ta.x, Ta.y, RGB(128,0,0));
            else {
                SetDCPenColor(hdc,RGB(250,0,0));
                MoveToEx(hdc, Ta1.x, Ta1.y, NULL);
                LineTo(hdc, Ta.x, Ta.y);
            }
            Ta1 = Ta;
        }
        if(T >= CurrTime) {
            break;
        }


//          SetDCPenColor(hdc,RGB(0,0,128));
//          MoveToEx(hdc, Tx1.x, Tx1.y, NULL);
//          LineTo(hdc, Tx.x, Tx.y);
//         Tx1 = Tx;
//         SetDCPenColor(hdc,RGB(0,128,0));
//         MoveToEx(hdc, Tv1.x, Tv1.y, NULL);
//         LineTo(hdc, Tv.x, Tv.y);
//         Tv1 = Tv;
//         SetDCPenColor(hdc,RGB(128,0,0));
//         MoveToEx(hdc, Ta1.x, Ta1.y, NULL);
//         LineTo(hdc, Ta.x, Ta.y);
//         Ta1 = Ta;
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