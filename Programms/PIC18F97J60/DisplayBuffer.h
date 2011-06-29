#ifndef _DISPLAY_BUFFER_H
#define _DISPLAY_BUFFER_H
#include "GenericTypeDefs.h"

void DisplayInit();
void DisplayDraw(BYTE addr);
void OutTextXY(BYTE X,BYTE Y,BYTE* Text);
#endif //_DISPLAY_BUFFER_H
