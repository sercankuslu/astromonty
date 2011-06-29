#ifndef _FONT_H
#define _FONT_H
#include "GenericTypeDefs.h"

#define ARIAL_B_MASK 0x03FF

WORD* GetSymbolImage(BYTE symbol,WORD* OutWordCount);
WORD* GetStringImage(BYTE* string, WORD* OutWordCount);

#endif //_FONT_H
