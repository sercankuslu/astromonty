
#include "stdafx.h"
#ifndef _WINDOWS
#   include "GenericTypeDefs.h"
#endif
#include "pcf8535.h"
#include "DisplayBuffer.h"
#include "font.h"

#ifndef _WINDOWS
#pragma udata DISPLAY0 =0x800
static BYTE DisplayBuffer0[256];
#pragma udata DISPLAY1 =0x900
static BYTE DisplayBuffer1[256];
#pragma udata DISPLAY2 =0xA00
static BYTE DisplayBuffer2[256];
#pragma udata DISPLAY3 =0xB00
static BYTE DisplayBuffer3[256];
#pragma udata DISPLAY4 =0xC00
static BYTE DisplayBuffer4[40];
#pragma udata
#else
static BYTE DisplayBuffer0[256];
static BYTE DisplayBuffer1[256];
static BYTE DisplayBuffer2[256];
static BYTE DisplayBuffer3[256];
static BYTE DisplayBuffer4[40];
#endif



BYTE * GetAddr(WORD addr)
{
    WORD_VAL aa;
    aa.Val = addr;
    switch (aa.byte.HB){
        case 0:
            return &DisplayBuffer0[aa.byte.LB];
        case 1:
            return &DisplayBuffer1[aa.byte.LB];
        case 2:
            return &DisplayBuffer2[aa.byte.LB];
        case 3:
            return &DisplayBuffer3[aa.byte.LB];
        case 4:
            if(aa.byte.LB < 40)
                return &DisplayBuffer4[aa.byte.LB];
        default:
            return NULL;
    }    
}
void SetPixelDB(WORD X, WORD Y, BOOL color)
{ 
    BYTE * a;
    if( (X < SIZE_X)&&(Y < SIZE_Y)){
        a = GetAddr((Y >> 3) * SIZE_X + X);   
        if(color){
            (*a) = (*a)|(0x01 << (Y & 0x07));
        } else {
            (*a) =(*a)&(~(0x01 << (Y & 0x07)));
        }
    }
}
BOOL GetPixelDB(WORD X, WORD Y)
{ 
    BYTE * a;        
    if( (X < SIZE_X )&&(Y < SIZE_Y )){
        a = GetAddr(((Y >> 3) * SIZE_X) + X);   
        if(((*a)&(0x01 << (Y & 0x07)))>0) {
            return TRUE;
        }else{
            return FALSE;
        }
    }
    return FALSE;
}
void DisplayInit()
{
    //InitFonts();
    DisplayClear();
}

void DisplayClear(void)
{    
    //очистка буфера экрана
    memset(DisplayBuffer0, 0, sizeof(DisplayBuffer0));
    memset(DisplayBuffer1, 0, sizeof(DisplayBuffer1));
    memset(DisplayBuffer2, 0, sizeof(DisplayBuffer2));
    memset(DisplayBuffer3, 0, sizeof(DisplayBuffer3));
    memset(DisplayBuffer4, 0, sizeof(DisplayBuffer4));
}

void DisplayDraw(BYTE addr)
{    
    WORD i;
    #define BLOCK_SIZE 64

    LCDSetXY(addr,0,0);	

    for(i=0;i<256;i+=BLOCK_SIZE){			
        LCDSendData(addr, &DisplayBuffer0[i],BLOCK_SIZE);
    }   
    for(i=0;i<256;i+=64){			
        LCDSendData(addr, &DisplayBuffer1[i],BLOCK_SIZE);
    }   
    for(i=0;i<256;i+=64){			
        LCDSendData(addr, &DisplayBuffer2[i],BLOCK_SIZE);
    }   
    for(i=0;i<256;i+=64){			
        LCDSendData(addr, &DisplayBuffer3[i],BLOCK_SIZE);
    }   
    LCDSendData(addr, DisplayBuffer4, 40);

}
void WriteByteAtBank(BYTE Bank, BYTE Pos, BYTE Data,BYTE Mask)
{   
    WORD Address;
    WORD MemBank;
    WORD MemPos;
    BYTE Temp = 0;
    BYTE TempMask = 0xFF^Mask;
    
    if(Mask == 0) return;
    if(Bank>7) return;
    if(Pos>132) return;
    Address = Bank; 
    Address = Address*133+Pos;
    MemBank = (Address>>8);
    MemPos  = (Address&0x00FF);
    switch(MemBank){
        case 0: 
            Temp = DisplayBuffer0[MemPos]&TempMask;
            DisplayBuffer0[MemPos]=Temp|(Data&Mask);                
        break;
        case 1: 
            Temp = DisplayBuffer1[MemPos]&TempMask;
            DisplayBuffer1[MemPos]=Temp|(Data&Mask);                
        break;
        case 2: 
            Temp = DisplayBuffer2[MemPos]&TempMask;
            DisplayBuffer2[MemPos]=Temp|(Data&Mask);                
        break;
        case 3: 
            Temp = DisplayBuffer3[MemPos]&TempMask;
            DisplayBuffer3[MemPos]=Temp|(Data&Mask);                
        break;
        case 4: 
            Temp = DisplayBuffer4[MemPos]&TempMask;
            DisplayBuffer4[MemPos]=Temp|(Data&Mask);                
        break;
    }
    
}
void OutTextXY(BYTE X,BYTE Y,BYTE* Text,BYTE CFont)
{    
      
    WORD FontSize = 0;
    WORD XX = 0;
    BYTE* ptr;
    static WORD Image[12];
    WORD count = 12;  
    ptr = Text;
    XX = X;
    while ( *ptr ){
        count = 12;
        GetSymbolImage(*ptr++, Image, &count, CFont);		
        switch(CFont){
        case 0: FontSize = SIZE_ARIAL;
            break;
        case 1: FontSize = SIZE_ARIAL_B;
            break;
        default: ;
        }
         OutImageW(XX,Y,count,FontSize,Image);
         XX += count+1;
    }    
}
//битовое изображение
// формат изображения:
// байты : 0 1 2 3 4 ...
// биты ( строки)
// 0
// 1
// 2
// 3
// 4
// ...
// 0.0 в верхнем левом углу
void OutImage(WORD X, WORD Y, WORD SX, WORD SY, BYTE* Image)
{
	WORD i;
	WORD j;
    for(i = 0; i < SX; i++){
        for(j = 0; j < SY; j++){            
            SetPixelDB( X + i, Y + j, (Image[i] & ( 0x01 << (j & 0x07)))?TRUE:FALSE);
        }
    }
}
void OutImageW(WORD X, WORD Y, WORD SX, WORD SY, WORD* Image)
{
	WORD i;
	WORD j;
    for( i = 0; i < SX; i++){
        for( j = 0; j < SY; j++){            
            SetPixelDB( X + i, Y + j, (Image[i] & ( 0x01 << (j & 0x0F)))?TRUE:FALSE);
        }
    }
}
