// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifdef _WINDOWS
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#ifndef _DWORD_VAL_
#define _DWORD_VAL_

#define __EXTENSION 
typedef union _BYTE_VAL
{
    BYTE Val;
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
    } bits;
} BYTE_VAL;
typedef union _DWORD_VAL
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
typedef union _WORD_VAL
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
#define MY_DEFAULT_RR_PARA_Rdct			(360.0/* * 40.0*/)
#define MY_DEFAULT_RR_PARA_TimerStep	        (0.0000002)
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

#include "uCmdProcess.h"

// TODO: reference additional headers your program requires here
