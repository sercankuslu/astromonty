#ifndef _DISPLAY_BUFFER_H
#define _DISPLAY_BUFFER_H
#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "TCPIP Stack/TCPIP.h"
#endif

void DisplayInit(void);
void DisplayClear(void);
void DisplayDraw(BYTE addr);
void OutTextXY(BYTE X,BYTE Y,BYTE* Text,BYTE CFont);
#endif //_DISPLAY_BUFFER_H
