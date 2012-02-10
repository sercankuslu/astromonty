#ifndef _DISPLAY_BUFFER_H
#define _DISPLAY_BUFFER_H
#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "TCPIP Stack/TCPIP.h"
#endif
#define SIZE_X 133
#define SIZE_Y 64

void DisplayInit(void);
void DisplayClear(void);
void DisplayDraw(BYTE addr);
void OutTextXY(BYTE X,BYTE Y,BYTE* Text,BYTE CFont);
void SetPixelDB(WORD X, WORD Y, BOOL color);
BYTE * GetAddr(WORD addr);
BOOL GetPixelDB(WORD X, WORD Y);
void OutImage(WORD X, WORD Y, WORD SX, WORD SY, BYTE* Image);
void OutImage(WORD X, WORD Y, WORD SX, WORD SY, WORD* Image);

#endif //_DISPLAY_BUFFER_H
