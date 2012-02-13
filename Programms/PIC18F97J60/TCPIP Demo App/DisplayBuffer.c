
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



void OutTextXY(WORD X,WORD Y,BYTE* Text,FONT CFont,EFFECT Effect)
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
// положение элементов
#define  Con_FlagX  0
#define  A_FlagX    26
#define  D_FlagX    57
#define  G_FlagX    98

void DrawMenu(MENU_ID Menu, BYTE Select_id)
{
    BYTE TextA[] = "A: 06`45'08.9173\"";
    BYTE TextD[] = "D:-16`42'58.0170\"";    
    BYTE TextG[] = "G:-06`45'08.0170\"";    
    BYTE TimeT[] = "23:56";
    BYTE MenuB[] = {0xCC,0xE5,0xED,0xFE,0x00};//"Menu";
    BYTE ConnectFlag[] = {0xD1,0xE5,0xF2,0xFC,0x00};//{"Con"}; // Сеть   
    BYTE AlphaFlag[] = {0xC0,0xEB,0xFC,0xF4,0xE0,0x00};//{"Alph"};  // Алфа
    BYTE DeltaFlag[] = {0xC4,0xE5,0xEB,0xFC,0xF2,0xE0,0x00};//{"Delt"};  // Делт
    BYTE GammaFlag[] = {0xC3,0xE0,0xEC,0xEC,0xE0,0x00};//{"Gamm"};  // Гамма
    // признаки: true- белый фон, false-черный фон
    static BOOL Con = true;
    static BOOL A_Run = true;
    static BOOL D_Run = true;
    static BOOL G_Run = false;
    static BOOL Time_Run = true;
    BYTE color = 0;
    EFFECT Effect = NORMAL;

//     Con ^= 1;
//     A_Run ^= 1;
//     D_Run ^= 1;
    Time_Run ^= 1;

    if(Time_Run){
        TimeT[2] = ':';
    } else {
        TimeT[2] = ' ';
    }

    DisplayClear();
    switch (Menu) {
    case MAIN_WINDOW:  
        DrawRectangle(0,51,132,63,1);
        OutTextXY(15,13,TextA,ARIAL_B,NORMAL);
        OutTextXY(15,13 + 13,TextD,ARIAL_B,NORMAL);
        OutTextXY(15,13 + 26,TextG,ARIAL_B,NORMAL);
        OutTextXY(2,53,MenuB,ARIAL_B,NORMAL);
        Line(36,52,36,63,1);
        OutTextXY(105,53,TimeT,ARIAL_B,NORMAL);
        Line(103,52,103,63,1);
        DrawRectangle(0,0,132,10,1);   

        if(Con){
            color = 0;
            Effect = NORMAL;
        } else {
            color = 1;
            Effect = INVERT;
        }
        FloodRectangle(Con_FlagX+1,1,A_FlagX ,9,color);
        OutTextXY(Con_FlagX+3,2,ConnectFlag,ARIAL_L,Effect);
        if(A_Run){
            color = 0;
            Effect = NORMAL;
        } else {
            color = 1;
            Effect = INVERT;
        }
        FloodRectangle(A_FlagX+1,1,D_FlagX,9,color);
        OutTextXY(A_FlagX+3,2,AlphaFlag,ARIAL_L,Effect);
        if(D_Run){
            color = 0;
            Effect = NORMAL;
        } else {
            color = 1;
            Effect = INVERT;
        }
        FloodRectangle(D_FlagX+1,1,G_FlagX,9,color);
        OutTextXY(D_FlagX+3,2,DeltaFlag,ARIAL_L,Effect);
        if(G_Run){
            color = 0;
            Effect = NORMAL;
        } else {
            color = 1;
            Effect = INVERT;
        }
        FloodRectangle(G_FlagX+1,1,131,9,color);
        OutTextXY(G_FlagX+3,2,GammaFlag,ARIAL_L,Effect);

    	break;
    case SETTINGS:
        break;
    case S_MONTY:
        break;
    case S_NETWORK:
        break;
    case SM_TYPESELECT:
        break;
    default:
        break;
    }
}