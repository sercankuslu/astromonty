#include "stdafx.h"

#ifndef _WINDOWS
#include "GenericTypeDefs.h"
#endif

#include "..\..\dsPIC33\protocol.h"
#include "DisplayBuffer.h"
#include "Control.h"
#include <stdio.h>

#define PI 3.1415926535897932384626433832795
// положение элементов
#define  Con_FlagX  0
#define  A_FlagX    26
#define  D_FlagX    57
#define  G_FlagX    98
#define  Line0      12
#define  Line1      25
#define  Line2      38
#define  Line3      51
#define true 1
#define false 0
#define ENTER 1
#define ESC   2
#ifndef _WINDOWS
#define TEXT static rom
#else
#define TEXT static
#endif
static ALL_PARAMS Params;


void DrawMenuLine( BYTE ID, BYTE * Name, BYTE * Value, int PosY,int PosX );
void DrawScrollBar(int Pos, int Max);
void XtoTimeString( char * Text, double X, BOOL hour );
BYTE ProcessKeys(BYTE * KeyPressed, BYTE * PosX, BYTE MaxX, BYTE* PosY, BYTE MaxY, MENU_ID LastState, MENU_ID * State, BYTE * SelPosX, BYTE * SelPosY);
void IPtoText (DWORD IP, char * Text, BOOL ForEdit);

void ProcessMenu( BYTE * KeyPressed )
{

    static double Xa = 0.0;//91.3 * PI / 180.0;
    static double Xd = -33.1 * PI / 180.0;
    static double Xg = 47.2 * PI / 180.0;
    
    BYTE TextA[20] = "A:";
    BYTE TextD[20] = "D:";    
    BYTE TextG[20] = "G:";    
    TEXT BYTE TimeT[] = "23:56";
    TEXT BYTE MenuB[] = {0xCC,0xE5,0xED,0xFE,0x00};//"Menu";
    TEXT BYTE ConnectFlag[] = {0xD1,0xE5,0xF2,0xFC,0x00};//{"Con"}; // Сеть   
    TEXT BYTE AlphaFlag[] = {0xC0,0xEB,0xFC,0xF4,0xE0,0x00};//{"Alph"};  // Алфа
    TEXT BYTE DeltaFlag[] = {0xC4,0xE5,0xEB,0xFC,0xF2,0xE0,0x00};//{"Delt"};  // Делт
    TEXT BYTE GammaFlag[] = {0xC3,0xE0,0xEC,0xEC,0xE0,0x00};//{"Gamm"};  // Гамма
    
    TEXT BYTE SettingsName[] = "Настройки";    
    TEXT BYTE ObservName[] = "Наблюдение";
    TEXT BYTE O_GoTo[] = "Навести";
    TEXT BYTE O_Manual[] = "Ручной режим";
    TEXT BYTE O_Space[] = "Режим спутников";
    TEXT BYTE S_Observ[] = "Наблюдение";
    TEXT BYTE S_NetName[] = "Сеть";
    TEXT BYTE S_AlphaName[] = "Альфа";
    TEXT BYTE S_DeltaName[] = "Дельта";
    TEXT BYTE S_GammaName[] = "Гамма";
    TEXT BYTE S_MontyName[] = "Монтировка";
    TEXT BYTE S_Display[] = "Экран";
    TEXT BYTE S_MontyTypeName[] = "Тип монтировки";
    TEXT BYTE SN_Name[] = "Имя:";
    TEXT BYTE SN_IP[] =   "IP:";    
    TEXT BYTE SN_Mask[] = "Mask:";
    TEXT BYTE SN_Gate[] = "Gate:";
    TEXT BYTE SN_DNS1[] = "DNS1:";
    TEXT BYTE SN_DNS2[] = "DNS2:";
    TEXT BYTE SN_NTP[] =  "NTP:";
    

    static MENU_ID State = MAIN_WINDOW;
    // признаки: true- белый фон, false-черный фон
    static BOOL Con = true;
    static BOOL A_Run = true;
    static BOOL D_Run = true;
    static BOOL G_Run = false;
    static BOOL Time_Run = true;
    BOOL EndProcess = false;
    BYTE color = 0;
    EFFECT Effect = NORMAL;
    static BYTE PosX = 0;
    static BYTE PosY = 0;
    static BYTE TmpPosY = 0;
    static BYTE SelPosX = 0;
    static BYTE SelPosY = 0;
    static BYTE TmpValue[20];
    static DWORD * TmpDWValue = NULL;
    BYTE Selected = 0;
    static BYTE * EditTxt = NULL; 
    DWORD_VAL TmpDWval;

    //     Con ^= 1;
    //     A_Run ^= 1;
    //     D_Run ^= 1;
    Time_Run ^= 1;

    if(Time_Run){
        TimeT[2] = ':';
    } else {
        TimeT[2] = ' ';
    }
    
    Params.Local.IP = 0xA8C00105;
    Params.Local.Mask = 0xFFFFFF00;
    Params.Local.Gate = 0xC0A80101;
    Params.Local.DNS1 = 0xC0A80101;
    Params.Local.DNS2 = 0xC0A80102;


    while(!EndProcess){
        switch (State) {
        case MAIN_WINDOW:  
            if((*KeyPressed) & 0x80) {
                State = MENU;
                (*KeyPressed) ^= 0x80;
                PosX = 0;
                PosY = 0;
                break;
            }
            if((*KeyPressed) & 0x40) { //Enter                
                State = OBSERV;                    
                PosX = 0;
                PosY = 0;
                (*KeyPressed) ^= 0x40;
                break;
            }
            Xa += (2.0 * PI /(360.0 * 200.0 * 16.0))*13.333333333334/2.0;
            XtoTimeString((char*)&TextA[2], Xa, 1 );
            XtoTimeString((char*)&TextD[2], Xd, 0 );
            XtoTimeString((char*)&TextG[2], Xg, 0 );
            
            DisplayClear();
            DrawRectangle(0,51,132,63,1);
            OutTextXY(15,13,TextA,ARIAL_B,NORMAL);
            OutTextXY(15,13 + 13,TextD,ARIAL_B,NORMAL);
            OutTextXY(15,13 + 26,TextG,ARIAL_B,NORMAL);
            OutTextXY(2,53,MenuB,ARIAL_B,NORMAL);
            Line(36,52,36,63,1);
            OutTextXY(101,53,TimeT,ARIAL_B,NORMAL);
            Line(99,52,99,63,1);
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
            EndProcess = true;
            break;
        case MENU:
            Selected = ProcessKeys(KeyPressed, &PosX, 2, &PosY, 0, MAIN_WINDOW, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { 
                switch(SelPosX){
                case 0:
                     State = OBSERV;
                    break;
                case 1:
                    State = SETTINGS;
                    break;
                case 2:
                    break;
                }                                     
                break;
            } else {
                if(Selected == ESC) break;
            }

            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,MenuB,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 2);
            DrawMenuLine(0, ObservName, NULL, PosX, 0);
            DrawMenuLine(1, SettingsName, NULL, PosX, 0);
            EndProcess = true;
            break;
        case SETTINGS:
            Selected = ProcessKeys(KeyPressed, &PosX, 4, &PosY, 0, MENU, &State, &SelPosX, &SelPosY);
            
            if(Selected == ENTER) { //Enter
                switch(SelPosX){
                case 0:
                    State = S_NETWORK;
                    break;
                case 1:
                    State = S_MONTY;
                    break;
                case 2:
                    State = S_DISPLAY;
                    break;
                case 3:
                    State = S_OBSERV;
                    break;                
                }                                      
                break;
            } else {
                if(Selected == ESC) break;
            }

            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,SettingsName,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);        
            DrawMenuLine(0,S_NetName, NULL, PosX, 0);
            DrawMenuLine(1,S_MontyName, NULL, PosX, 0);
            DrawMenuLine(2,S_Display, NULL, PosX, 0);
            DrawMenuLine(3,S_Observ, NULL, PosX, 0);
            
            EndProcess = true;
            break;
        case S_MONTY:
            Selected = ProcessKeys(KeyPressed, &PosX, 4, &PosY, 0, SETTINGS, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter
                switch(SelPosX){
                case 0:
                    State = SM_ALPHA;
                    break;
                case 1:
                    State = SM_DELTA;
                    break;
                case 2:
                    State = SM_GAMMA;
                    break;
                case 3:
                    State = SM_TYPESELECT;
                    break;
                case 4:
                    break;
                }                         
                break;
            }else {
                if(Selected == ESC) break;
            }
            
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,S_MontyName,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);        
            DrawMenuLine(0,S_MontyTypeName, NULL, PosX, 0);
            DrawMenuLine(1,S_AlphaName, NULL, PosX, 0);
            DrawMenuLine(2,S_DeltaName, NULL, PosX, 0);
            DrawMenuLine(3,S_GammaName, NULL, PosX, 0);            

            EndProcess = true;
            break;
        case S_NETWORK:
            Selected = ProcessKeys(KeyPressed, &PosX, 7, &PosY, 0, SETTINGS, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter            
                State = EDIT_IP;
                //EditTxt = SN_IP;                                
                switch(SelPosX){
                case 1:
                     EditTxt = SN_IP;
                     TmpDWValue = &Params.Local.IP;                     
                     break;                
                }
                IPtoText(*TmpDWValue,(char*)TmpValue, 1);
                break;
            } else {
                if(Selected == ESC) break;
            }
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,S_NetName,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 7);              
            DrawMenuLine(0,SN_Name, (BYTE*)Params.Local.Name, PosX, 0);
            IPtoText(Params.Local.IP,   (char*)TmpValue, 0);
            DrawMenuLine(1,SN_IP, TmpValue, PosX, 0);
            IPtoText(Params.Local.Mask, (char*)TmpValue, 0);
            DrawMenuLine(2,SN_Mask, TmpValue, PosX, 0);
            IPtoText(Params.Local.Gate, (char*)TmpValue, 0);
            DrawMenuLine(3,SN_Gate, TmpValue, PosX, 0);
            IPtoText(Params.Local.DNS1, (char*)TmpValue, 0);
            DrawMenuLine(4,SN_DNS1, TmpValue, PosX, 0);            
            IPtoText(Params.Local.DNS2, (char*)TmpValue, 0);
            DrawMenuLine(5,SN_DNS2, TmpValue, PosX, 0);            
            DrawMenuLine(6,SN_NTP, NULL, PosX, 0);            

            EndProcess = true;
            break;
        case SM_TYPESELECT:
            Selected = ProcessKeys(KeyPressed, &PosX, 0, &PosY, 0, S_MONTY, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
            } else {
                if(Selected == ESC) break;
            }            
            DisplayClear();
            EndProcess = true;
            break;
        case OBSERV:
            Selected = ProcessKeys(KeyPressed, &PosX, 3, &PosY, 0, MENU, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
            } else {
                if(Selected == ESC) break;
            }             
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,ObservName,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 3);        
            DrawMenuLine(0,O_GoTo, NULL, PosX, 0);
            DrawMenuLine(1,O_Manual, NULL, PosX, 0);
            DrawMenuLine(2,O_Space, NULL, PosX, 0);                      
            
            EndProcess = true;
            break;
        case EDIT_IP:
            TmpPosY = PosY;
            Selected = ProcessKeys(KeyPressed, &PosX, 10, &PosY, strlen((const char*)TmpValue) + 1, S_NETWORK, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
                //(*TmpDWValue)
                for(int i = 0;i<strlen((const char*)TmpValue);i++){
                    if(TmpValue[i]=='.') TmpValue[i] = ' ';
                }
                BYTE UB=0;
                BYTE MB=0;
                BYTE HB=0;
                BYTE LB=0;
                sscanf((const char*)TmpValue,"%d %d %d %d",&UB,&MB,&HB,&LB);
                (*TmpDWValue) = TmpDWval.Val;
            } else {
                if(Selected == ESC) break;
            }             
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,EditTxt,ARIAL_L,NORMAL);
            //DrawScrollBar(PosX, 1);    
            
            if(PosY>0){                
                if(TmpValue[PosY-1] == '.'){
                    if(TmpPosY < PosY) {
                        PosY++;         
                    }else PosY--;
                }
                if(TmpPosY != PosY){
                    PosX = '9' - TmpValue[PosY-1];
                }
                TmpValue[PosY-1] = '9' - PosX;
            }
            DrawMenuLine(0, NULL, TmpValue, 0, PosY);
            EndProcess = true;
            break;
        default:
            Selected = ProcessKeys(KeyPressed, &PosX, 3, &PosY, 0, MENU, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
            } else {
                if(Selected == ESC) break;
            } 
            DisplayClear();
            EndProcess = true;
            break;
        }
    }
}

void DrawMenuLine( BYTE ID, BYTE * Name, BYTE * Value, int PosY, int PosX )
{
    BYTE color =0;
    BYTE Line;
    BYTE * ptr = Value;
    EFFECT Effect;
    EFFECT Effect1;
    static int CPosY = 0;
    static int CPosX = 0;
    int i = 0;
    int ValueLength = 0;
    WORD StringXPos = 0;
    if(PosY < 0) return;
    if(PosY == 0) CPosY = 0;
    while(PosY > CPosY + 3) CPosY++;
    while(PosY < CPosY) CPosY--;
    if(PosX < 0) return;
    if(PosX == 0) CPosX = 0;
    while(PosX > CPosX) CPosX++;
    while(PosX < CPosX) CPosX--;

    if(PosY == ID){
        color = 1;
        Effect = INVERT;
        Effect1 = NORMAL;
    } else {
        color = 0;
        Effect = NORMAL;
        Effect1 = INVERT;
    }
    switch ((int)ID - CPosY){
        case 0:
            Line = Line0;
            break;
        case 1:
            Line = Line1;
            break;
        case 2:
            Line = Line2;
            break;
        case 3:
            Line = Line3;
            break;
        default : return;
    }
    FloodRectangle(1,Line,120,Line+13,color);
    if(Name != NULL)
        StringXPos = OutTextXY(5,Line + 2,Name,ARIAL_B,Effect); 
    if(Value!= NULL){
        ValueLength = strlen((const char*)Value);
        if(PosX > 0 ){            
            StringXPos = OutTextXYx(StringXPos + 5,Line + 2,Value, PosX-1, ARIAL_B,Effect);
            StringXPos = OutTextXYx(StringXPos,Line + 2,&Value[PosX-1], 1, ARIAL_B, Effect1 );
            if(PosX - 1 < ValueLength)
                OutTextXYx(StringXPos,Line + 2,&Value[PosX], ValueLength - PosX, ARIAL_B, Effect );
        } else {
            OutTextXY(StringXPos + 5,Line + 2,Value, ARIAL_B,Effect);
        }
    }
}
void DrawScrollBar(int Pos, int Max)
{
    int Size = 53 / Max;
    int Y = Pos * Size;
    DrawRectangle(122,10,132,63,1);
    FloodRectangle(123,11 + Y,131,11 + Y + Size,1);

}

BYTE ProcessKeys(BYTE * KeyPressed, BYTE * YPos, BYTE YMax, BYTE* XPos, BYTE XMax, MENU_ID LastState, MENU_ID * State, BYTE * YSelPos, BYTE * XSelPos)
{
    if((*KeyPressed) & 0x80) { //ESC
        (*State) = LastState;
        (*YSelPos) = 0;
        (*XSelPos) = 0;
        (*YPos) = 0;
        (*XPos) = 0;
        (*KeyPressed) ^= 0x80;
        return ESC;
    }
    if((*KeyPressed) & 0x40) { //Enter         
        (*YSelPos) = (*YPos);
        (*XSelPos) = (*XPos);
        (*YPos) = 0;
        (*XPos) = 0;
        (*KeyPressed) ^= 0x40;
        return ENTER;
    }
    if((*KeyPressed) & 0x01) { //UP
        if((*YPos) > 0) (*YPos)--;
        (*KeyPressed) ^= 0x01;       
    }
    if((*KeyPressed) & 0x02) { //DOWN
        if(YMax > 0)
            if((*YPos) < YMax - 1) (*YPos)++;
        (*KeyPressed) ^= 0x02;       
    }
    if((*KeyPressed) & 0x04) { //LEFT
        if((*XPos) > 0) (*XPos)--;
        (*KeyPressed) ^= 0x04;        
    }
    if((*KeyPressed) & 0x08) { //RIGHT
        if(XMax > 0)
            if((*XPos) < XMax-1) (*XPos)++;
        (*KeyPressed) ^= 0x08;        
    }
    return 0;
}

void XtoTimeString( char * Text, double X, BOOL hour )
{    
    double Xg;
    signed short Grad;
    BYTE Min;
    BYTE Sec;
    BYTE mSec;
    Xg = X * 180/(PI); // в часах времени
    if (hour) {
        Xg /= 15.0;
    }
    Grad = (signed short)Xg;    
    Xg -= (double)Grad;
    Xg *= 60;
    if(Xg < 0.0) Xg = -Xg;
    Min = (BYTE)(Xg);
    Xg -= (double)Min;
    Xg *= 60;
    Sec = (BYTE)(Xg);
    Xg -= (double)Sec;
    Xg *= 100;
    mSec = (BYTE)(Xg);
    sprintf (Text, "%+0.2d`%0.2d'%0.2d.%0.2d\"", Grad,Min,Sec,mSec);
}

void IPtoText (DWORD IP, char * Text, BOOL ForEdit)
{
    DWORD_VAL T;
    T.Val = IP;
    if(ForEdit){
        sprintf (Text, "%0.3d.%0.3d.%0.3d.%0.3d", T.byte.UB,T.byte.MB,T.byte.HB,T.byte.LB);
    } else {
        sprintf (Text, "%d.%d.%d.%d", T.byte.UB,T.byte.MB,T.byte.HB,T.byte.LB);
    }
}

void EditIp(char * Text, DWORD * IP)
{
    
}