
#include "stdafx.h"
#ifndef _WINDOWS
#   include "GenericTypeDefs.h"
#endif
#include "pcf8535.h"
#include "DisplayBuffer.h"
#include "font.h"

#ifndef _WINDOWS
#pragma udata DISPLAY0 =0xA00
static BYTE DisplayBuffer0[256];
#pragma udata DISPLAY1 =0xB00
static BYTE DisplayBuffer1[256];
#pragma udata DISPLAY2 =0xC00
static BYTE DisplayBuffer2[256];
#pragma udata DISPLAY3 =0xD00
static BYTE DisplayBuffer3[256];
#pragma udata DISPLAY4 =0xE00
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



WORD OutTextXY( WORD X,WORD Y,BYTE* Text,FONT CFont, EFFECT Effect )
{          
    WORD FontSize = 0;
    WORD FontMask = 0xFFFF;
    WORD XX = 0;
    BYTE* ptr;
    static WORD Image[13];
    WORD count = 13;  
    WORD i = 0;
    ptr = Text;
    XX = X;
    while ( *ptr ){
        count = 13;
        GetSymbolImage(*ptr++, Image, &count, CFont);		
        switch(CFont){
        case ARIAL_L: 
            FontSize = SIZE_ARIAL;
            FontMask = ARIAL_MASK;
            break;
        case ARIAL_B: 
            FontSize = SIZE_ARIAL_B;
            FontMask = ARIAL_B_MASK;
            break;
        default: ;
        }
        switch(Effect){
            case NORMAL:
                Image[count] = 0;
                count++;
                break;
            case INVERT:
                for(i = 0;i<count;i++)
                {
                    Image[i] = Image[i]^FontMask;
                }
                Image[count] = FontMask;
                count++;                
                break;
        }
        OutImageW(XX,Y,count,FontSize,Image);
        XX += count;
    }   
    return XX;
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
void Line(WORD X, WORD Y, WORD X1, WORD Y1,BOOL color)
{
    WORD i = 0;
    WORD j = 0;
    WORD Xb = 0;
    WORD Yb = 0;
    WORD Sx = 0;
    WORD Sy = 0;
    if(X1 >= X) {
        Xb = X;
        Sx = X1 - X;
    } else {
        Xb = X1;
        Sx = X - X1;
    }    
    if(Y1 >= Y) {
        Yb = Y;
        Sy = Y1 - Y;
    } else {
        Yb = Y1;
        Sy = Y - Y1;
    }
    if(Sx < Sy){
        if(Sy == 0) Sy = 1;
        for (j = 0; j <= Sy; j++){
            SetPixelDB( Xb + j * Sx / Sy, Yb + j, color);
        }
    } else {
        if(Sx == 0) Sx = 1;
        for (i = 0; i <= Sx; i++){
            SetPixelDB( Xb + i, Yb + i * Sy / Sx, color);
        }
    }
}

void DrawRectangle(WORD X, WORD Y, WORD X1, WORD Y1,BOOL color)
{
    Line(X,Y,X,Y1,color);
    Line(X,Y,X1,Y,color);
    Line(X,Y1,X1,Y1,color);
    Line(X1,Y,X1,Y1,color);
}
void FloodRectangle(WORD X, WORD Y, WORD X1, WORD Y1,BOOL color)
{
    WORD i = 0;
    WORD j = 0;
    WORD Xb = 0;
    WORD Yb = 0;
    WORD Sx = 0;
    WORD Sy = 0;
    if(X1 >= X) {
        Xb = X;
        Sx = X1 - X;
    } else {
        Xb = X1;
        Sx = X - X1;
    }
    if(Y1 >= Y) {
        Yb = Y;
        Sy = Y1 - Y;
    } else {
        Yb = Y1;
        Sy = Y - Y1;
    }
    for (j = 0; j <= Sy; j++){
        for (i = 0; i <= Sx; i++){
            SetPixelDB( Xb + i, Yb + j, color);
        }
    }
}


