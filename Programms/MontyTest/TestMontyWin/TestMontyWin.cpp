// TestMontyWin.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "TestMontyWin.h"
#include <my_global.h>
//#include <mysql.h>
#include <mysql.h>



#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна
HWND MyHWND;

// Дескриптор соединения

MYSQL *conn;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	        WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	        About(HWND, UINT, WPARAM, LPARAM);
void                            MyDraw(HWND hWnd);
BOOL NeedClose;
RECT CirclePos;

struct REQ_DATA_TYPE {
	UINT HD;
	float Alpha;
	float Alpha_m;
	float Delta;
	float Delta_m;
	float Size;
};
struct DATA_TYPE {
	UINT HD;
	double Alpha;
	double Delta;
	double Size;
};
struct DOUBLE_RECT {
	double top;
	double bottom;
	double left;
	double right;
};

DATA_TYPE* phData;
unsigned long DataLen = 0;
DOUBLE_RECT DataMaxSize;
int StarMaxSize = -5;
RECT ViewRect = {
    0,0,1280,900
};

// ViewRect.top = 5;
// ViewRect.left = 5;
// ViewRect.bottom = 605;
// ViewRect.right = 805;

bool RequestData(MYSQL *conn, DATA_TYPE** phData, ULONG *DataLen, char* Condition);


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
	LoadString(hInstance, IDC_TESTMONTYWIN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTMONTYWIN));
	
	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	// Закрываем соединение с сервером базы данных
	mysql_close(conn);
	if(DataLen>0) delete [] phData;
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
	wcex.hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTMONTYWIN));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TESTMONTYWIN);
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
   MyHWND = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   // Получаем дескриптор соединения
   conn = mysql_init(NULL);
   static REQ_DATA_TYPE* pData;

   if(conn == NULL)
   {
	   // Если дескриптор не получен - выводим сообщение об ошибке
	   //fprintf(stderr, "Error: can't create MySQL-descriptor\n");
	   //MessageBox(hWnd,_T("Error: can't create MySQL-descriptor\n"),_T("Error"),0);
	   return FALSE;
   }
   if(!RequestData(conn, &phData, &DataLen, NULL)) return false;
   CirclePos.top = ViewRect.top - 10;
   CirclePos.left = ViewRect.left - 10;
   CirclePos.right = ViewRect.right + 10;
   CirclePos.bottom=ViewRect.bottom + 10;
   SetTimer(hWnd, 1,  100 , NULL);
   InvalidateRect(hWnd, &CirclePos, TRUE);
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
    //char condition[256];
    static float T = 0;
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
	case WM_PAINT:		                              
        MyDraw(hWnd);		
		break;
	case WM_DESTROY:		
		PostQuitMessage(0);
		break;
        case WM_TIMER:
            /*T+= 1.0;
            if(T>24) T=0;
            sprintf(condition,"W<6 AND Alpha_grad > %f AND Alpha_grad < %f", T, T+1.0);
            if(DataLen>0) delete [] phData;
            DataLen = 0;
            RequestData(conn, &phData, &DataLen, condition);
            InvalidateRect(hWnd, &CirclePos, TRUE);*/
			//DrawPos.top = CirclePos.top-2;
			//DrawPos.left = CirclePos.left-2;
			//DrawPos.right = CirclePos.right+2;
			//DrawPos.bottom=CirclePos.bottom+2;
			//CirclePos.left+=2;
			//CirclePos.right+=2;
				
            
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

void MyDraw(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc;   	
    RECT lpRect;
    UINT size;
    double aspectX = (ViewRect.right-ViewRect.left)/(DataMaxSize.right - DataMaxSize.left);
    double aspectY = (ViewRect.bottom-ViewRect.top)/(DataMaxSize.bottom - DataMaxSize.top);
    

    hdc = BeginPaint(hWnd, &ps);  	

    Rectangle(hdc, ViewRect.left-1,ViewRect.top-1,ViewRect.right+1,ViewRect.bottom+1);

    HPEN hPen = CreatePen (PS_SOLID, 1, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject (hdc, hPen);    
    HBRUSH hBrush = CreateSolidBrush(0x000000); 
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    
    FillRect( hdc, &ViewRect, hBrush);
       
    SelectObject(hdc, hOldBrush);
    
    UINT X;
    UINT Y;
    for(UINT i = 0; i< DataLen; i++){
        size = StarMaxSize - phData[i].Size + 1;
        Y = ViewRect.top  + (DataMaxSize.bottom - phData[i].Delta)*aspectY;
        X = ViewRect.left + (DataMaxSize.right  - phData[i].Alpha)*aspectX;
        
        /*lpRect.top    = Y - size/2;
        lpRect.bottom = Y + size/2;
        lpRect.left   = X - size/2;
        lpRect.right  = X + size/2;*/
		COLORREF StarColor = RGB(phData[i].Size*17, phData[i].Size*17, phData[i].Size*17);
        switch(size){
			/*case 5:
				SetPixel(hdc, X+2, Y-2, StarColor);
				SetPixel(hdc, X-2, Y+2, StarColor);
				SetPixel(hdc, X-2, Y-2, StarColor);
				SetPixel(hdc, X+2, Y+2, StarColor);* /
			case 4:
				SetPixel(hdc, X, Y-2, StarColor);
				SetPixel(hdc, X, Y+2, StarColor);
				SetPixel(hdc, X-2, Y, StarColor);
				SetPixel(hdc, X+2, Y, StarColor);
            case 3:
                SetPixel(hdc, X+1, Y-1, StarColor);
                SetPixel(hdc, X-1, Y+1, StarColor);
                SetPixel(hdc, X-1, Y-1, StarColor);
                SetPixel(hdc, X+1, Y+1, StarColor);
            case 2: 
                SetPixel(hdc, X, Y-1, StarColor);
                SetPixel(hdc, X, Y+1, StarColor);
                SetPixel(hdc, X-1, Y, StarColor);
                SetPixel(hdc, X+1, Y, StarColor);
            case 0:*/
            case 1: SetPixel(hdc, X, Y, StarColor);
                break;
            default:				
                //Ellipse(hdc, X - size/2, Y - size/2, X + size/2, Y + size/2);
				SetPixel(hdc, X, Y, StarColor);
            
        }
            //if((phData[i].HD == 39801)||    //бетельгейзе
            //    //(phData[i].HD == 8890)||  // Альфа малой медведицы
            //    (phData[i].HD == 37742)||   //Алнитак
            //    (phData[i].HD == 35468)||   //Белатрикс
            //    (phData[i].HD == 37128)||   //Алнилам
            //    (phData[i].HD == 36486)||   //Минтака	62509	
            //    (phData[i].HD == 29139)     //Альдебаран
            //    //(phData[i].HD == 148478)  //Антарес
            //    //(phData[i].HD == 48915)   // сириус
            //    ){ 
            //    Rectangle(hdc, lpRect.left, lpRect.top, lpRect.right, lpRect.bottom); 
            //}            
            //    Ellipse(hdc, lpRect.left, lpRect.top, lpRect.right, lpRect.bottom);	
            //}
    
    }   
    SelectObject(hdc, hOldPen);
   

    // Удаляем Pen
    //DeleteObject (hPen);
    EndPaint(hWnd, &ps);	
   
}
bool RequestData(MYSQL *conn, DATA_TYPE** phData, ULONG *DataLen, char* Condition)
{
	//recno	TYC1	TYC2	TYC3	pmRA	pmDE	BTmag	VTmag	RA	DE
    char sqlSelect[] = "SELECT RA, DE, BTmag FROM Tycho2";
    char sqlWhere[] = "WHERE";
    static char Req[32768];
    // Дескриптор результирующей таблицы
    MYSQL_RES *res;
    // Дескриптор строки
    MYSQL_ROW row;
    REQ_DATA_TYPE* pData;
    if((*DataLen)>0) delete [] (*phData);

    if ((Condition!=NULL)&&(strlen(Condition)>0)){
        sprintf(Req, "%s %s %s",sqlSelect, sqlWhere, Condition);
    } else {
        sprintf(Req, "%s",sqlSelect);
    }
    // Подключаемся к серверу
    if(!mysql_real_connect(conn,
        "localhost",
        "root",
        "1qweDCVB",
        "mybase",
        NULL,
        NULL,
        0
        ))
    {
        // Если нет возможности установить соединение с сервером 
        // базы данных выводим сообщение об ошибке        
        return FALSE;
    }
    else  {        
        // Выполняем SQL-запрос
        //if(mysql_query(conn, "SELECT HD, Alpha_grad, Alpha_Min, Delta_grad, Delta_Min, W FROM Stars ") != 0)
        //if(mysql_query(conn, "SELECT HD, Alpha_grad, Alpha_Min, Delta_grad, Delta_Min, W FROM Stars WHERE W<8") != 0)
        if(mysql_query(conn, Req) != 0)
            //if(mysql_query(conn, "SELECT HD, Alpha_grad, Alpha_Min, Delta_grad, Delta_Min, W FROM Stars where HD=39801") != 0)
            //MessageBox(hWnd,_T("Error: can't execute SQL-query\n"),_T("Error"),0);
            return FALSE;

        // Получаем дескриптор результирующей таблицы
        res = mysql_store_result(conn);
        if(res == NULL) return FALSE;      

        // Если имеется хотя бы одна запись - выводим	  
        (*DataLen) = mysql_num_rows(res);
        if((*DataLen) > 0) {
            //pData = new REQ_DATA_TYPE[(*DataLen)];
			(*phData) = new DATA_TYPE[(*DataLen)];
			memset((*phData), 0, (*DataLen)*sizeof(DATA_TYPE));
            // В цикле перебираем все записи
            // результирующей таблицы
            UINT i = 0;
            while((row = mysql_fetch_row(res)) != NULL) {
                // Выводим результат в стандартный поток
                /*sscanf_s(row[0],"%lu", &pData[i].HD);
                sscanf_s(row[1],"%f", &pData[i].Alpha);
                sscanf_s(row[2],"%f", &pData[i].Alpha_m);
                sscanf_s(row[3],"%f", &pData[i].Delta);
                sscanf_s(row[4],"%f", &pData[i].Delta_m);
                sscanf_s(row[5],"%f", &pData[i].Size);*/
				float Alpha;
				float Delta;
				float Size;
				sscanf_s(row[0],"%f", &Alpha);
				sscanf_s(row[1],"%f", &Delta);
				sscanf_s(row[2],"%f", &Size);
				(*phData)[i].Alpha = Alpha;
				(*phData)[i].Delta = Delta;
				(*phData)[i].Size  = Size;
                i++;
            }
            // Освобождаем память, занятую результирующей таблицей
            mysql_free_result(res);
        }
    }   
    if((*DataLen)==0) return FALSE;
    DataMaxSize.top = 90;
    DataMaxSize.bottom = -90;
    DataMaxSize.left = 24*15;
    DataMaxSize.right = 0;

    //(*phData) = new DATA_TYPE[(*DataLen)];
    //memset((*phData), 0, (*DataLen)*sizeof(DATA_TYPE));

    for(UINT i = 0; i< (*DataLen); i++){
        /*if(pData[i].Alpha>=0) (*phData)[i].Alpha = (pData[i].Alpha + pData[i].Alpha_m/60)*15;
        else (*phData)[i].Alpha = (pData[i].Alpha - pData[i].Alpha_m/60)*15;
        if(pData[i].Delta>=0) (*phData)[i].Delta = (pData[i].Delta + pData[i].Delta_m/60);
        else (*phData)[i].Delta = (pData[i].Delta - pData[i].Delta_m/60);*/
        //(*phData)[i].Size =  pData[i].Size;
        //(*phData)[i].HD = pData[i].HD;

        if((*phData)[i].Alpha< DataMaxSize.left)   DataMaxSize.left   = (*phData)[i].Alpha;
        if((*phData)[i].Alpha> DataMaxSize.right)  DataMaxSize.right  = (*phData)[i].Alpha;
        if((*phData)[i].Delta< DataMaxSize.top)    DataMaxSize.top    = (*phData)[i].Delta;
        if((*phData)[i].Delta> DataMaxSize.bottom) DataMaxSize.bottom = (*phData)[i].Delta;
        if((*phData)[i].Size > StarMaxSize) StarMaxSize = (*phData)[i].Size;
    }
    //delete [] pData;
    return TRUE;
}