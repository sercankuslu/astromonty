#ifndef _FONT_H
#define _FONT_H

#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "GenericTypeDefs.h"
#endif

typedef enum FONT {
    ARIAL_L, 
    ARIAL_B
} FONT;

#ifdef _WINDOWS
#define rom 
#endif

#define ARIAL_B_MASK 0x03FF
#define ARIAL_MASK 0x00FF
#define SIZE_ARIAL_B 10
#define SIZE_ARIAL 8

int GetSymbolImage(BYTE symbol, const rom WORD** Image, WORD* ImageSize, FONT CFont);
WORD* GetStringImage(BYTE* string, WORD* OutWordCount);

 

//#define ARIAL_B  0   //Arial_B
//#define ARIAL    0   //Arial_B

#endif //_FONT_H
