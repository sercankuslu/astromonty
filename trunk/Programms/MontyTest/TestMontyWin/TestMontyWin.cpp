// TestMontyWin.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "TestMontyWin.h"
#include <my_global.h>
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

static REQ_DATA_TYPE* pData;/*[] = {
	{23.0, 	44.7, 	+42.0, 	03.0,  8.834},
	{00.0, 	18.4, 	+44.0, 	01.0,  8.105},
	{19.0, 	31.2, 	+43.0, 	38.0,  9.914},
	{04.0, 	36.5, 	+44.0, 	59.0,  8.552},
	{11.0, 	05.5, 	+43.0, 	32.0,  8.765},
	{11.0, 	47.2, 	+43.0, 	28.0,  8.705},
	{12.0, 	32.0, 	+43.0, 	29.0,  9.536},
	{02.0, 	33.5, 	+45.0, 	39.0,  8.838},
	{02.0, 	38.2, 	+46.0, 	33.0,  9.102},
	{17.0, 	48.8, 	+45.0, 	42.0,  8.264},
	{19.0, 	28.8, 	+46.0, 	03.0,  8.569},
	{17.0, 	39.2, 	+46.0, 	09.0,  8.207},
	{02.0, 	49.8, 	+48.0, 	08.0,  9.487},
	{02.0, 	51.1, 	+48.0, 	06.0,  9.321},
	{07.0, 	24.5, 	+47.0, 	11.0, 10.179},
	{20.0, 	41.3, 	+48.0, 	09.0, 10.451},
	{17.0, 	35.6, 	+48.0, 	50.0,  8.682},
	{23.0, 	51.2, 	+48.0, 	59.0,  9.222},
	{10.0, 	20.7, 	+49.0, 	06.0, 10.046},
	{13.0, 	43.9, 	+49.0, 	08.0,  9.415},
	{09.0, 	05.2, 	+60.0, 	17.0,  8.869},
	{08.0, 	07.6, 	+60.0, 	41.0, 10.106},
	{02.0, 	46.7, 	+63.0, 	00.0,  9.534},
	{01.0, 	04.7, 	+74.0, 	50.0,  8.909},
	{22.0, 	02.9, 	+00.0, 	04.0,  9.079},
	{09.0, 	35.5, 	-01.0, 	47.0,  9.283},
	{14.0, 	42.0, 	-03.0, 	31.0,  8.974},
	{21.0, 	40.3, 	-01.0, 	47.0,  9.947},
	{23.0, 	23.8, 	-01.0, 	19.0,  8.991},
	{00.0, 	21.1, 	-02.0, 	21.0,  9.537},
};*/
DATA_TYPE* phData;
unsigned long DataLen;
RECT DataMaxSize;
   
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
	delete [] phData;
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
   // Дескриптор результирующей таблицы
   MYSQL_RES *res;
   // Дескриптор строки
   MYSQL_ROW row;

   if(conn == NULL)
   {
	   // Если дескриптор не получен - выводим сообщение об ошибке
	   //fprintf(stderr, "Error: can't create MySQL-descriptor\n");
	   MessageBox(hWnd,_T("Error: can't create MySQL-descriptor\n"),_T("Error"),0);
	   return FALSE;
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
	   MessageBox(hWnd,_T("Error: can't connect to database %s\n"),_T("Error"),0);
	    return FALSE;
   }
   else
   {
	   // Если соединение успешно установлено выводим фразу - "Success!"
	   //fprintf(stdout, "Success!\n"); // Выполняем SQL-запрос
	   if(mysql_query(conn, "SELECT HD, Alpha_grad, Alpha_Min, Delta_grad, Delta_Min, W FROM Stars WHERE W<4") != 0)
	   //if(mysql_query(conn, "SELECT HD, Alpha_grad, Alpha_Min, Delta_grad, Delta_Min, W FROM Stars where HD=39801") != 0)
		   MessageBox(hWnd,_T("Error: can't execute SQL-query\n"),_T("Error"),0);

	   // Получаем дескриптор результирующей таблицы
	   res = mysql_store_result(conn);
	   if(res == NULL) MessageBox(hWnd,_T("Error: can't get the result description\n"),_T("Error"),0);

	   // Если имеется хотя бы одна запись - выводим	  
	   DataLen = mysql_num_rows(res);
	   if(DataLen > 0)
	   {
		   pData = new REQ_DATA_TYPE[DataLen];
		   // В цикле перебираем все записи
		   // результирующей таблицы
		   UINT i = 0;
		   while((row = mysql_fetch_row(res)) != NULL)
		   {
			   // Выводим результат в стандартный поток
			   sscanf_s(row[0],"%lu", &pData[i].HD);
			   sscanf_s(row[1],"%f", &pData[i].Alpha);
			   sscanf_s(row[2],"%f", &pData[i].Alpha_m);
			   sscanf_s(row[3],"%f", &pData[i].Delta);
			   sscanf_s(row[4],"%f", &pData[i].Delta_m);
			   sscanf_s(row[5],"%f", &pData[i].Size);
			   i++;
		   }
	   }

	   // Освобождаем память, занятую результирующей таблицей
	   mysql_free_result(res);

   }      
   DataMaxSize.top = 0;
   DataMaxSize.bottom = 0;
   DataMaxSize.left = 0;
   DataMaxSize.right = 0;

   phData = new DATA_TYPE[DataLen];
   
   for(UINT i = 0; i< DataLen; i++){
	   if(pData[i].Alpha>=0) phData[i].Alpha = pData[i].Alpha + pData[i].Alpha_m/60;
	   else phData[i].Alpha = pData[i].Alpha - pData[i].Alpha_m/60;
	   if(pData[i].Delta>=0) phData[i].Delta = (pData[i].Delta + pData[i].Delta_m/60);
	   else phData[i].Delta = (pData[i].Delta - pData[i].Delta_m/60);
	   phData[i].Size =  8 - pData[i].Size;
	   phData[i].HD = pData[i].HD;
	   if(phData[i].Alpha< DataMaxSize.left) DataMaxSize.left = phData[i].Alpha;
	   if(phData[i].Alpha> DataMaxSize.right) DataMaxSize.right = phData[i].Alpha;
	   if(phData[i].Delta< DataMaxSize.top) DataMaxSize.top = phData[i].Delta;
	   if(phData[i].Delta> DataMaxSize.bottom) DataMaxSize.bottom = phData[i].Delta;
   }
   delete [] pData;
   CirclePos.top = 0;
   CirclePos.left = 0;
   CirclePos.right = 810;
   CirclePos.bottom=610;
   //SetTimer(hWnd, 1,  1000 , NULL);
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
	RECT DrawPos;

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
	double aspectX = 800/(DataMaxSize.right - DataMaxSize.left);
	double aspectY = 600/(DataMaxSize.bottom - DataMaxSize.top);


    hdc = BeginPaint(hWnd, &ps);  	
	Rectangle(hdc, 0,0,810,610);	


	for(UINT i = 0; i< DataLen; i++){
		size = phData[i].Size;
		lpRect.top =    10+(DataMaxSize.bottom - phData[i].Delta)*aspectY - size/2;
		lpRect.bottom = 10+(DataMaxSize.bottom - phData[i].Delta)*aspectY + size/2;
		lpRect.left =   10+(DataMaxSize.right - phData[i].Alpha)*aspectX - size/2;
		lpRect.right =  10+(DataMaxSize.right - phData[i].Alpha)*aspectX + size/2;

		if((phData[i].HD == 39801)|| //бетельгейзе
		   //(phData[i].HD == 8890)||  // Альфа малой медведицы
		   (phData[i].HD == 37742)|| //Алнитак
		   (phData[i].HD == 35468)|| //Белатрикс
		   (phData[i].HD == 37128)|| //Алнилам
		   (phData[i].HD == 36486)|| //Минтака	62509	
		   (phData[i].HD == 29139)   //Альдебаран
		   //(phData[i].HD == 148478) //Антарес
		   //(phData[i].HD == 48915)   // сириус
		   ) Rectangle(hdc, lpRect.left, lpRect.top, lpRect.right, lpRect.bottom); 
		Ellipse(hdc, lpRect.left, lpRect.top, lpRect.right, lpRect.bottom);	
	}    
    EndPaint(hWnd, &ps);	
   
}