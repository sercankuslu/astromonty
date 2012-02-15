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


static ALL_PARAMS Params;


void DrawMenuLine(BYTE ID, BYTE * Name, int Select);
void DrawScrollBar(int Pos, int Max);
void DoubleXtoTime( char* Text, double X, bool hour );

void ProcessMenu( BYTE * KeyPressed )
{

    static double Xa = 91.3 * PI / 180.0;
    static double Xd = -33.1 * PI / 180.0;
    static double Xg = 47.2 * PI / 180.0;
    
    BYTE TextA[20] = "A:";
    BYTE TextD[20] = "D:";    
    BYTE TextG[20] = "G:";    
    BYTE TimeT[] = "23:56";
    BYTE MenuB[] = {0xCC,0xE5,0xED,0xFE,0x00};//"Menu";
    BYTE ConnectFlag[] = {0xD1,0xE5,0xF2,0xFC,0x00};//{"Con"}; // Сеть   
    BYTE AlphaFlag[] = {0xC0,0xEB,0xFC,0xF4,0xE0,0x00};//{"Alph"};  // Алфа
    BYTE DeltaFlag[] = {0xC4,0xE5,0xEB,0xFC,0xF2,0xE0,0x00};//{"Delt"};  // Делт
    BYTE GammaFlag[] = {0xC3,0xE0,0xEC,0xEC,0xE0,0x00};//{"Gamm"};  // Гамма
    
    BYTE SettingsName[] = "Настройки";    
    BYTE ObservName[] = "Наблюдение";
    BYTE O_GoTo[] = "Навести";
    BYTE O_Manual[] = "Ручной режим";
    BYTE O_Space[] = "Режим спутников";
    BYTE S_Observ[] = "Наблюдение";
    BYTE S_NetName[] = "Сеть";
    BYTE S_AlphaName[] = "Альфа";
    BYTE S_DeltaName[] = "Дельта";
    BYTE S_GammaName[] = "Гамма";
    BYTE S_MontyName[] = "Монтировка";
    BYTE S_Display[] = "Экран";
    BYTE S_MontyTypeName[] = "Тип монтировки";
    BYTE SN_Name[] = "Имя:      Interface";
    BYTE SN_IP[] =   "IP:   192.168.1.2";
    BYTE SN_Mask[] = "Mask: 255.255.255.0";
    BYTE SN_Gate[] = "Gate: 192.168.1.1";
    BYTE SN_DNS1[] = "DNS1: 192.168.1.1";
    BYTE SN_DNS2[] = "DNS2: 192.168.1.3";
    BYTE SN_NTP[] =  "NTP : nrepus.cp.ru";
    

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
    static BYTE Select = 0;
    static BYTE Page = 0;

    //     Con ^= 1;
    //     A_Run ^= 1;
    //     D_Run ^= 1;
    Time_Run ^= 1;

    if(Time_Run){
        TimeT[2] = ':';
    } else {
        TimeT[2] = ' ';
    }
    while(!EndProcess){
        switch (State) {
        case MAIN_WINDOW:  
            if((*KeyPressed) & 0x80) {
                State = MENU;
                (*KeyPressed) ^= 0x80;
                Select = 0;
                Page = 0;
                break;
            }
            if((*KeyPressed) & 0x40) { //Enter                
                State = OBSERV;                    
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x40;
                break;
            }
            Xa += 2.0 * PI /(360.0 * 200.0 * 16.0);
            DoubleXtoTime((char*)&TextA[2], Xa, 1 );
            DoubleXtoTime((char*)&TextD[2], Xd, 0 );
            DoubleXtoTime((char*)&TextG[2], Xg, 0 );
            
            DisplayClear();
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
            EndProcess = true;
            break;
        case MENU:
            if((*KeyPressed) & 0x80) { //ESC
                State = MAIN_WINDOW;
                (*KeyPressed) ^= 0x80;
                Select = 0;
                Page = 0;
                break;
            }
            if((*KeyPressed) & 0x40) { //Enter
                switch(Select){
                case 0:
                     State = OBSERV;
                    break;
                case 1:
                    State = SETTINGS;
                    break;
                case 2:
                    break;
                }         
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x40;
                break;
            }
            if((*KeyPressed) & 0x01) { //UP
                if(Select > 0) Select--;
                (*KeyPressed) ^= 0x01;
                break;
            }
            if((*KeyPressed) & 0x02) { //DOWN
                if(Select < 2) Select++;
                (*KeyPressed) ^= 0x02;
                break;
            }
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,MenuB,ARIAL_L,NORMAL);
            DrawScrollBar(Select, 2);
            DrawMenuLine(0,ObservName, Select);
            DrawMenuLine(1,SettingsName, Select);
            EndProcess = true;
            break;
        case SETTINGS:
            if((*KeyPressed) & 0x80) { //ESC
                State = MENU;
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x80;
                break;
            }
            if((*KeyPressed) & 0x40) { //Enter
                switch(Select){
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
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x40;
                break;
            }
            if((*KeyPressed) & 0x01) { //UP
                if(Select > 0) Select--;
                (*KeyPressed) ^= 0x01;
                break;
            }
            if((*KeyPressed) & 0x02) { //DOWN
                if(Select < 3) Select++;
                (*KeyPressed) ^= 0x02;
                break;
            }
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,SettingsName,ARIAL_L,NORMAL);
            DrawScrollBar(Select, 4);        
            DrawMenuLine(0,S_NetName, Select);
            DrawMenuLine(1,S_MontyName, Select);
            DrawMenuLine(2,S_Display, Select);
            DrawMenuLine(3,S_Observ, Select);
            
            EndProcess = true;
            break;
        case S_MONTY:
            if((*KeyPressed) & 0x80) { //ESC
                State = SETTINGS;
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x80;
                break;
            }
            if((*KeyPressed) & 0x40) { //Enter
                switch(Select){
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
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x40;
                break;
            }
            if((*KeyPressed) & 0x01) { //UP
                if(Select > 0) Select--;
                (*KeyPressed) ^= 0x01;
                break;
            }
            if((*KeyPressed) & 0x02) { //DOWN
                if(Select < 3) Select++;
                (*KeyPressed) ^= 0x02;
                break;
            }
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,S_MontyName,ARIAL_L,NORMAL);
            DrawScrollBar(Select, 4);        
            DrawMenuLine(0,S_MontyTypeName, Select);
            DrawMenuLine(1,S_AlphaName, Select);
            DrawMenuLine(2,S_DeltaName, Select);
            DrawMenuLine(3,S_GammaName, Select);            

            EndProcess = true;
            break;
        case S_NETWORK:
            if((*KeyPressed) & 0x80) { //ESC
                State = SETTINGS;
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x80;
                break;
            }
            if((*KeyPressed) & 0x40) { //Enter
//                 switch(Select){
//                 case 0:
//                     State = SM_ALPHA;
//                     break;

                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x40;
                break;
            }
            if((*KeyPressed) & 0x01) { //UP
                if(Select > 0) Select--;
                (*KeyPressed) ^= 0x01;
                break;
            }
            if((*KeyPressed) & 0x02) { //DOWN
                if(Select < 6) Select++;
                (*KeyPressed) ^= 0x02;
                break;
            }
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,S_NetName,ARIAL_L,NORMAL);
            DrawScrollBar(Select, 7);        
            DrawMenuLine(0,SN_Name, Select);
            DrawMenuLine(1,SN_IP, Select);
            DrawMenuLine(2,SN_Mask, Select);
            DrawMenuLine(3,SN_Gate, Select);
            DrawMenuLine(4,SN_DNS1, Select);            
            DrawMenuLine(5,SN_DNS2, Select);            
            DrawMenuLine(6,SN_NTP, Select);            

            EndProcess = true;
            break;
        case SM_TYPESELECT:
            if((*KeyPressed) & 0x80) { //ESC
                State = S_MONTY;
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x80;
                break;
            }
            DisplayClear();
            EndProcess = true;
            break;
        case OBSERV:
            if((*KeyPressed) & 0x80) { //ESC
                State = MENU;
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x80;
                break;
            }
            if((*KeyPressed) & 0x01) { //UP
                if(Select > 0) Select--;
                (*KeyPressed) ^= 0x01;
                break;
            }
            if((*KeyPressed) & 0x02) { //DOWN
                if(Select < 2) Select++;
                (*KeyPressed) ^= 0x02;
                break;
            }
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,ObservName,ARIAL_L,NORMAL);
            DrawScrollBar(Select, 3);        
            DrawMenuLine(0,O_GoTo, Select);
            DrawMenuLine(1,O_Manual, Select);
            DrawMenuLine(2,O_Space, Select);                       
            
            EndProcess = true;
            break;
        default:
            if((*KeyPressed) & 0x80) { //ESC
                State = MENU;
                Select = 0;
                Page = 0;
                (*KeyPressed) ^= 0x80;
                break;
            }
            DisplayClear();
            EndProcess = true;
            break;
        }
    }
}

void DrawMenuLine(BYTE ID, BYTE * Name, int Select)
{
    BYTE color =0;
    BYTE Line;
    EFFECT Effect;
    static int Pos = 0;
    if(Select < 0) return;
    if(Select == 0) Pos = 0;
    while(Select > Pos + 3) Pos++;
    while(Select < Pos) Pos--;

    if(Select == ID){
        color = 1;
        Effect = INVERT;
    } else {
        color = 0;
        Effect = NORMAL;
    }
    switch ((int)ID - Pos){
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
    OutTextXY(5,Line + 2,Name,ARIAL_B,Effect);            
}
void DrawScrollBar(int Pos, int Max)
{
    int Size = 53 / Max;
    int Y = Pos * Size;
    DrawRectangle(122,10,132,63,1);
    FloodRectangle(123,11 + Y,131,11 + Y + Size,1);

}

void DoubleXtoTime( char* Text, double X, bool hour )
{    
    double Xg = X * 180/(PI); // в часах времени
    if (hour) {
        Xg /= 15.0;
    }
    signed char Grad = (signed char)(Xg);    
    Xg -= (double)Grad;
    Xg *= 60;
    if(Xg < 0.0) Xg = -Xg;
    BYTE Min = (BYTE)(Xg);
    Xg -= (double)Min;
    Xg *= 60;
    BYTE Sec = (BYTE)(Xg);
    Xg -= (double)Sec;
    Xg *= 100;
    BYTE mSec = (BYTE)(Xg);
    sprintf (Text, "%+0.2d`%0.2d'%0.2d.%0.2d\"", Grad,Min,Sec,mSec);
}