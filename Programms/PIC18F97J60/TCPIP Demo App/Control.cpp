#include "stdafx.h"

#ifndef _WINDOWS
#include "GenericTypeDefs.h"
#endif

#include "..\..\dsPIC33\protocol.h"
#include "DisplayBuffer.h"
#include "Control.h"

// положение элементов
#define  Con_FlagX  0
#define  A_FlagX    26
#define  D_FlagX    57
#define  G_FlagX    98
#define  Line0      12
#define  Line1      25
#define  Line2      38
#define  Line3      51
void DrawMenuLine(BYTE ID, BYTE * Name, BYTE Select);

void ProcessMenu( BYTE * KeyPressed )
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
    
    BYTE SettingsName[] = "Настройки";
    BYTE S_ObservName[] = "Наблюдение";
    BYTE S_NetName[] = "Сеть";
    BYTE S_AlphaName[] = "Альфа";
    BYTE S_DeltaName[] = "Дельта";
    BYTE S_GammaName[] = "Гамма";
    BYTE S_TypeName[] = "Тип монтировки";
    BYTE S_Display[] = "Экран";
    BYTE S_AxisName[] = "Оси монтировки";

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
                if(Select < 6) Select++;
                (*KeyPressed) ^= 0x02;
                break;
            }
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,MenuB,ARIAL_L,NORMAL);
            DrawRectangle(122,10,132,63,1);
            FloodRectangle(123,11,131,31,1);

            if(Select == 0){
                color = 1;
                Effect = INVERT;
            } else {
                color = 0;
                Effect = NORMAL;
            }
            FloodRectangle(1,Line0,120,Line0+13,color);
            OutTextXY(15,Line0 + 2,S_ObservName,ARIAL_B,Effect);

            if(Select == 1){
                color = 1;
                Effect = INVERT;
            } else {
                color = 0;
                Effect = NORMAL;
            }
            FloodRectangle(1,Line1,120,Line1+13,color);
            OutTextXY(15,Line1 + 2,SettingsName,ARIAL_B,Effect);            

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
            if((*KeyPressed) & 0x01) { //UP
                if((Select == 0)&&(Page>0)){
                    Select = 3;
                    Page--;
                } else {
                    if(Select > 0) Select--;
                }
                (*KeyPressed) ^= 0x01;
                break;
            }
            if((*KeyPressed) & 0x02) { //DOWN
                Select++;
                if(Select > 3) {
                    Page++;
                    Select = 0;
                }
                (*KeyPressed) ^= 0x02;
                break;
            }
            DisplayClear();
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,SettingsName,ARIAL_L,NORMAL);
            DrawRectangle(122,10,132,63,1);
            FloodRectangle(123,11,131,31,1);
        
            DrawMenuLine(0,S_NetName, Select);
            DrawMenuLine(1,S_AxisName, Select);
            DrawMenuLine(2,S_TypeName, Select);
            DrawMenuLine(3,S_Display, Select);
            
            EndProcess = true;
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
}

void DrawMenuLine(BYTE ID, BYTE * Name, BYTE Select)
{
    BYTE color =0;
    BYTE Line;
    EFFECT Effect;
    if(Select == ID){
        color = 1;
        Effect = INVERT;
    } else {
        color = 0;
        Effect = NORMAL;
    }
    switch (ID){
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
    }
    FloodRectangle(1,Line,120,Line+13,color);
    OutTextXY(15,Line + 2,Name,ARIAL_B,Effect);            
}