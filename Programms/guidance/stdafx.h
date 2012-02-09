// stdafx.h: включаемый файл для стандартных системных включаемых файлов
// или включаемых файлов для конкретного проекта, которые часто используются, но
// не часто изменяются
//

//#pragma once
#ifdef _WINDOWS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows:
#include <windows.h>

// Файлы заголовков C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define USE_PROTOCOL_SERVER
#define USE_PROTOCOL_CLIENT

#define UNUSED(x) x

#ifndef _DWORD_VAL_
#define _DWORD_VAL_

typedef struct _WORD
{
    WORD LW;
    WORD HW;
} _WORD;
typedef struct _BYTE
{
    BYTE LB;
    BYTE HB;
    BYTE UB;
    BYTE MB;
} _BYTE;
typedef union 
{
    DWORD Val;
    _WORD word;
    _BYTE byte;

} DWORD_VAL;
#endif

#endif

// TODO. Установите здесь ссылки на дополнительные заголовки, требующиеся для программы
