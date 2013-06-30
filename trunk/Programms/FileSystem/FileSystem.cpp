// FileSystem.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "FileSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Единственный объект приложения

CWinApp theApp;

using namespace std;

BYTE FS[256*65536];

typedef struct _DIR_RECORD
{
    WORD wInodeID;					// Индекс инода
    BYTE bNameLength;               // Длинна имени файла
    char * pcFileName;              // Имя файла
}

typedef struct _INODE_RECORD
{
    WORD wInodeID;					// Индекс инода        
    WORD wFlags;					// флаги
    DWORD dwSize;                   // размер файла
    WORD pwTable[12];					// таблица размещения секторов 
									// [0..8] прямая адресация
									// [9] косвенная
									// [10] двойная косвенная
									// [11] тройная косвенная
} INODE_RECORD;

int uFSReadFile(
			  INODE_RECORD Inode,	// Inode файла
			  DWORD Address,		// Адрес внутри файла
			  void * Dest,			// Адрес буфера
			  DWORD Count			// Количество в байтах
			 )						// Возвращает количество реально считанных байт или значение < 0 при ошибке
{
    
}




int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// инициализировать MFC, а также печать и сообщения об ошибках про сбое
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO. Измените код ошибки соответственно своим потребностям
		_tprintf(_T("Критическая ошибка: сбой при инициализации MFC\n"));
		nRetCode = 1;
	}
	else
	{		
		memset(FS,0xFF,sizeof(FS));
	}

	return nRetCode;
}
