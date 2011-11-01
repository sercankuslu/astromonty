#ifndef _DISPLAY_BUFFER_H
#define _DISPLAY_BUFFER_H
#include "TCPIP Stack/TCPIP.h"

void DisplayInit(void);
void DisplayClear(void);
void DisplayDraw(BYTE addr);
void OutTextXY(BYTE X,BYTE Y,BYTE* Text,BYTE CFont);
#endif //_DISPLAY_BUFFER_H
