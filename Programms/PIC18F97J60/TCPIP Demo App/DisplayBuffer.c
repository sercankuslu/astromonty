
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
    BYTE b = 0;
    if( (X < SIZE_X)&&(Y < SIZE_Y)){
        a = GetAddr((Y >> 3) * SIZE_X + X);   
        if(color){
            (*a) = (*a)|(0x80 >> (Y & 0x07));
        } else {
            (*a) =(*a)&(~(0x80 >> (Y & 0x07)));
        }

    }
}
BOOL GetPixelDB(WORD X, WORD Y)
{ 
    BYTE * a;        
    if( (X < SIZE_X )&&(Y < SIZE_Y )){
        a = GetAddr(((Y >> 3) * SIZE_X) + X);   
        if(((*a)&(0x80 >> (Y & 0x07)))>0) {
            return TRUE;
        }else{
            return FALSE;
        }
    }
    return FALSE;
}
void DisplayInit()
{
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
    WORD  count;    
    BYTE i;	
    BYTE  YBank = (Y >> 3);    
    BYTE  YPos  =  Y&0x07;  
    BYTE  XPos  = X;
    DWORD_VAL Mask;
    DWORD_VAL Data;
    BYTE  Data1;
    BYTE  Data2;
    BYTE  Data3;
    BYTE  Mask1;//^
    BYTE  Mask2;//=    
    BYTE  Mask3;//v
    WORD  FontMask;
    BYTE* ptr;
    WORD* Image;
	WORD  I;
	switch(CFont){
        case 0:
            FontMask = (WORD)ARIAL_MASK<<8;
        break;
        case 1:
            FontMask = (WORD)ARIAL_B_MASK<<6;
        break;
        default:
            FontMask = 0;
    }       
    
    Mask.Val  = (FontMask>>YPos);
    Mask2 = Mask.byte.LB;
    Mask3 = Mask.byte.HB;
    Mask.Val  = (FontMask<<(16-YPos));
    Mask1 = Mask.byte.HB;
        
    ptr = Text;
    while ( *ptr ){
        Image = GetSymbolImage(*ptr++,&count,CFont);		
        for(i=0;i<count;i++){
            	switch(CFont){
            case 0:
                I = *Image<<8;
            break;
            case 1:
                I = *Image<<6;
            break;
            default:      
                I=0;              
         }       
			
            Data.Val = I>>YPos;
            Data2 = Data.byte.LB;
            Data3 = Data.byte.HB;
            Data.Val  = (I<<(16-YPos));
            Data1 = Data.byte.HB;
            WriteByteAtBank(YBank,   XPos, Data1,Mask1);
            WriteByteAtBank(YBank+1, XPos, Data2,Mask2);
            WriteByteAtBank(YBank+2, XPos, Data3,Mask3);            
            XPos++;
            Image++;
        }
        //промежуток между символами
        WriteByteAtBank(YBank,   XPos, 0,Mask1);
        WriteByteAtBank(YBank+1, XPos, 0,Mask2);
        WriteByteAtBank(YBank+2, XPos, 0,Mask3);            
        XPos++;
    }    
}
//битовое изображение, хранится построчно 
void OutImage(BYTE X, BYTE Y, BYTE SX, BYTE SY,BYTE* Image)
{
	#ifdef _WINDOWS
    UNUSED(X);
    UNUSED(Y);
    UNUSED(SX);
    UNUSED(SY);
    UNUSED(Image);
    #endif
}
