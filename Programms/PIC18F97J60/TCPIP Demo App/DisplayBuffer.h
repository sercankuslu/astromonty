#ifndef _DISPLAY_BUFFER_H
#define _DISPLAY_BUFFER_H

#include "font.h"

#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "TCPIP Stack/TCPIP.h"
#endif

#define SIZE_X 133
#define SIZE_Y 64
typedef enum EFFECT {
    NORMAL, 
    INVERT
} EFFECT;

typedef enum MENU_ID {
    MAIN_WINDOW, SETTINGS, S_NETWORK, S_MONTY, SM_TYPESELECT, SM_ALPHA, SM_DELTA, SM_GAMMA
} MENU_ID;

void DisplayInit(void);
void DisplayClear(void);
void DisplayDraw(BYTE addr);
void OutTextXY(WORD X,WORD Y,BYTE* Text,FONT CFont, EFFECT Effect);
void SetPixelDB(WORD X, WORD Y, BOOL color);
BYTE * GetAddr(WORD addr);
BOOL GetPixelDB(WORD X, WORD Y);
void OutImage(WORD X, WORD Y, WORD SX, WORD SY, BYTE* Image);
void OutImageW(WORD X, WORD Y, WORD SX, WORD SY, WORD* Image);
void Line(WORD X, WORD Y, WORD X1, WORD Y1,BOOL color);
void DrawRectangle(WORD X, WORD Y, WORD X1, WORD Y1,BOOL color);
void FloodRectangle(WORD X, WORD Y, WORD X1, WORD Y1,BOOL color);

void DrawMenu(MENU_ID Menu, BYTE Select_id);

#endif //_DISPLAY_BUFFER_H
