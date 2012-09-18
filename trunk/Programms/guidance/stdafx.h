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

#define __EXTENSION 
typedef union BYTE_VAL
{
    BYTE Val;
    struct __PACKED
    {
        __EXTENSION BYTE b0:1;
        __EXTENSION BYTE b1:1;
        __EXTENSION BYTE b2:1;
        __EXTENSION BYTE b3:1;
        __EXTENSION BYTE b4:1;
        __EXTENSION BYTE b5:1;
        __EXTENSION BYTE b6:1;
        __EXTENSION BYTE b7:1;
    } bits;
} BYTE_VAL;
typedef union DWORD_VAL
{
    DWORD Val;
    WORD w[2];
    BYTE v[4];
    struct 
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct 
    {
        __EXTENSION BYTE b0:1;
        __EXTENSION BYTE b1:1;
        __EXTENSION BYTE b2:1;
        __EXTENSION BYTE b3:1;
        __EXTENSION BYTE b4:1;
        __EXTENSION BYTE b5:1;
        __EXTENSION BYTE b6:1;
        __EXTENSION BYTE b7:1;
        __EXTENSION BYTE b8:1;
        __EXTENSION BYTE b9:1;
        __EXTENSION BYTE b10:1;
        __EXTENSION BYTE b11:1;
        __EXTENSION BYTE b12:1;
        __EXTENSION BYTE b13:1;
        __EXTENSION BYTE b14:1;
        __EXTENSION BYTE b15:1;
        __EXTENSION BYTE b16:1;
        __EXTENSION BYTE b17:1;
        __EXTENSION BYTE b18:1;
        __EXTENSION BYTE b19:1;
        __EXTENSION BYTE b20:1;
        __EXTENSION BYTE b21:1;
        __EXTENSION BYTE b22:1;
        __EXTENSION BYTE b23:1;
        __EXTENSION BYTE b24:1;
        __EXTENSION BYTE b25:1;
        __EXTENSION BYTE b26:1;
        __EXTENSION BYTE b27:1;
        __EXTENSION BYTE b28:1;
        __EXTENSION BYTE b29:1;
        __EXTENSION BYTE b30:1;
        __EXTENSION BYTE b31:1;
    } bits;
} DWORD_VAL;

#endif
#ifndef _WORD_VAL_
#define _WORD_VAL_
typedef union WORD_VAL
{
    WORD Val;
    BYTE v[2];
    struct 
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct 
    {
        __EXTENSION BYTE b0:1;
        __EXTENSION BYTE b1:1;
        __EXTENSION BYTE b2:1;
        __EXTENSION BYTE b3:1;
        __EXTENSION BYTE b4:1;
        __EXTENSION BYTE b5:1;
        __EXTENSION BYTE b6:1;
        __EXTENSION BYTE b7:1;
        __EXTENSION BYTE b8:1;
        __EXTENSION BYTE b9:1;
        __EXTENSION BYTE b10:1;
        __EXTENSION BYTE b11:1;
        __EXTENSION BYTE b12:1;
        __EXTENSION BYTE b13:1;
        __EXTENSION BYTE b14:1;
        __EXTENSION BYTE b15:1;
    } bits;
} WORD_VAL;
#endif

#define MY_DEFAULT_RR_PARA_K			(-0.257809479)
#define MY_DEFAULT_RR_PARA_B			(0.205858823)
#define MY_DEFAULT_RR_PARA_Mass			(500.0)
#define MY_DEFAULT_RR_PARA_Radius		(0.5)
#define MY_DEFAULT_RR_PARA_Length		(3.0)
#define MY_DEFAULT_RR_PARA_Rdct			(360.0)
#define MY_DEFAULT_RR_PARA_TimerStep	        (0.000000025)
#define MY_DEFAULT_RR_PARA_SPT			(200ul)
#define MY_DEFAULT_RR_PARA_uSPS			(16ul)
#define MY_DEFAULT_RR_PARA_XMinPos		(0ul)
#define MY_DEFAULT_RR_PARA_XMaxPos		(576000ul)
#define MY_DEFAULT_RR_PARA_XParkPos		(0ul)
#define MY_DEFAULT_RR_PARA_VMax			(10.0)

#if !defined _APP_CONFIG_TYPE
#define _APP_CONFIG_TYPE
typedef struct { 
    DWORD_VAL MyIPAddr;
    DWORD_VAL MyMask;
    DWORD_VAL MyGateway;
    DWORD_VAL PrimaryDNSServer;
    DWORD_VAL SecondaryDNSServer;
    char NetBIOSName[16];
    DWORD Time;
} AppConfigType;
#endif //#if !defined _APP_CONFIG_TYPE
#endif //_WINDOWS

// TODO. Установите здесь ссылки на дополнительные заголовки, требующиеся для программы
