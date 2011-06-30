#ifndef _FONT_H
#define _FONT_H
#include "GenericTypeDefs.h"

#define ARIAL   0
#define ARIAL_B 1
#define ARIAL_B_MASK 0x03FF
#define ARIAL_MASK 0xFF
static BYTE CFont = 0;
WORD* GetSymbolImage(BYTE symbol,WORD* OutWordCount);
WORD* GetStringImage(BYTE* string, WORD* OutWordCount);
void SetFont(BYTE Font);
 

#define ARIAL_B  0   //Arial_B
#define ARIAL    0   //Arial_B

#endif //_FONT_H
