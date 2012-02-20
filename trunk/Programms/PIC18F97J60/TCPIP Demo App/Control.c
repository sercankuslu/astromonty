#include "stdafx.h"

#ifndef _WINDOWS
#include "GenericTypeDefs.h"
#else
#define MY_DEFAULT_IP_ADDR_BYTE1        (192ul)
#define MY_DEFAULT_IP_ADDR_BYTE2        (168ul)
#define MY_DEFAULT_IP_ADDR_BYTE3        (1ul)
#define MY_DEFAULT_IP_ADDR_BYTE4        (111ul)
#define MY_DEFAULT_MASK_BYTE1           (255ul)
#define MY_DEFAULT_MASK_BYTE2           (255ul)
#define MY_DEFAULT_MASK_BYTE3           (255ul)
#define MY_DEFAULT_MASK_BYTE4           (0ul)
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
#define  Line_L0      12
#define  Line_L1      22
#define  Line_L2      32
#define  Line_L3      42
#define  Line_L4      52
#define true 1
#define false 0
#define ENTER 1
#define ESC   2

const char  TextA[] = "А:";
const char  TextD[] =  "Д:";
const char  TextG [] = "Г:";
const char  TextStart[] =       "Старт";
const char  MenuB[] =           "Меню"; //{0xCC,0xE5,0xED,0xFE,0x00};//"Menu";
const char  ConnectFlag[] =     "Сеть";  //{0xD1,0xE5,0xF2,0xFC,0x00};//{"Con"}; // Сеть   
const char  AlphaFlag[] =       "Альфа"; //{0xC0,0xEB,0xFC,0xF4,0xE0,0x00};//{"Alph"};  // Алфа
const char  DeltaFlag[] =       "Дельта"; //{0xC4,0xE5,0xEB,0xFC,0xF2,0xE0,0x00};//{"Delt"};  // Делт
const char  GammaFlag[] =       "Гамма";  //{0xC3,0xE0,0xEC,0xEC,0xE0,0x00};//{"Gamm"};  // Гамма

const char  SettingsName[] =    "Настройки";
const char  ObservName[] =      "Наблюдение";
const char  O_GoTo[] =          "Навести";
const char  O_Manual[] =        "Ручной режим";
const char  O_Space[] =         "Режим спутников";
const char  S_Observ[] =        "Наблюдение";
const char  S_NetName[] =       "Сеть";
const char  S_AlphaName[] =     "Альфа";
const char  S_DeltaName[] =     "Дельта";
const char  S_GammaName[] =     "Гамма";
const char  S_MontyName[] =     "Монтировка";
const char  S_Display[] = 	"Экран";
const char  S_MontyTypeName[] = "Тип монтировки";
const char  SN_Name[] =         "Имя:";
const char  SN_IP[] =           "IP:";
const char  SN_Mask[] =         "Mask:";
const char  SN_Gate[] =         "Gate:";
const char  SN_DNS1[] =         "DNS1:";
const char  SN_DNS2[] =         "DNS2:";
const char  SN_NTP[] =          "NTP:";

const char MsgContinue[] =      "Продолжить";
const char MsgError[] =         "Ошибка";

const char CoordNotReachable0[] = "Указанные координаты";
const char CoordNotReachable1[] = "в данный момент вре-";
const char CoordNotReachable2[] = "мени находятся вне";
const char CoordNotReachable3[] = "зоны видимости";

ALL_PARAMS Params;


void DrawMenuLine( BYTE ID, const char * Name, const char * Value, int PosY,int PosX , BYTE Mode );
void DrawScrollBar(int Pos, int Max);
void XtoTimeString( char * Text, double X, BOOL hour );
BYTE ProcessKeys(BYTE * KeyPressed, BYTE * PosX, BYTE MaxX, BYTE* PosY, BYTE MaxY, MENU_ID LastState, MENU_ID * State, BYTE * SelPosX, BYTE * SelPosY);
void IPtoText (DWORD IP, char * Text, BOOL ForEdit);
int SubStrToInt(const char* Text, int Beg, int * Val);
void TextToTimeD(char* TmpValue, BOOL TmpIsHours, double * TmpDoValue);

void ProcessMenu( BYTE * KeyPressed )
{

    static double Xa = 0.0;//91.3 * PI / 180.0;
    static double Xd = -33.1 * PI / 180.0;
    static double Xg = 47.2 * PI / 180.0;
    static char TimeT[] = "23:56";    
/*
#define TextA "A:"
#define TextD "D:"
#define TextG "G:"
#define TextStart "Старт"
#define MenuB "Меню" //{0xCC,0xE5,0xED,0xFE,0x00};//"Menu";
#define ConnectFlag "Сеть"  //{0xD1,0xE5,0xF2,0xFC,0x00};//{"Con"}; // Сеть   
#define AlphaFlag   "Альфа" //{0xC0,0xEB,0xFC,0xF4,0xE0,0x00};//{"Alph"};  // Алфа
#define DeltaFlag   "Дельта" //{0xC4,0xE5,0xEB,0xFC,0xF2,0xE0,0x00};//{"Delt"};  // Делт
#define GammaFlag   "Гамма"  //{0xC3,0xE0,0xEC,0xEC,0xE0,0x00};//{"Gamm"};  // Гамма
    
#define SettingsName    "Настройки"
#define ObservName      "Наблюдение"
#define O_GoTo          "Навести"
#define O_Manual        "Ручной режим"
#define O_Space         "Режим спутников"
#define S_Observ        "Наблюдение"
#define S_NetName       "Сеть"
#define S_AlphaName     "Альфа"
#define S_DeltaName     "Дельта"
#define S_GammaName     "Гамма"
#define S_MontyName     "Монтировка"
#define S_Display	"Экран"
#define S_MontyTypeName	"Тип монтировки"
#define SN_Name		"Имя:"
#define SN_IP           "IP:"    
#define SN_Mask         "Mask:"
#define SN_Gate         "Gate:"
#define SN_DNS1         "DNS1:"
#define SN_DNS2         "DNS2:"
#define SN_NTP          "NTP:"
*/


    static MENU_ID State = MAIN_WINDOW;
    static MENU_ID LastState = MAIN_WINDOW;

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
    static BYTE LastPosX = 0;
    static BYTE LastPosY = 0;
    static BYTE TmpPosY = 0;
    static BYTE SelPosX = 0;
    static BYTE SelPosY = 0;
    static char TmpValue[20];
    static DWORD * TmpDWValue = NULL;
    static double * TmpDoValue = NULL;
    static BOOL TmpIsHours = false;
    BYTE Selected = 0;
    static char * EditTxt = NULL; 
    DWORD_VAL TmpDWval;
    static BOOL Init = false;
    static int TimerCount = 0;
    //     Con ^= 1;
    //     A_Run ^= 1;
    //     D_Run ^= 1;
    TimerCount+=2;
    if(TimerCount % 5 == 0){
        Time_Run ^= 1;
    }

    if(Time_Run){
        TimeT[2] = ':';
    } else {
        TimeT[2] = ' ';
    }
    if(!Init){
        Params.Local.IP = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
        Params.Local.Mask = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
        Params.Local.Gate = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | 0x01 <<24ul;
        Params.Local.DNS1 = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | 0x02 <<24ul;
        Params.Local.DNS2 = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | 0x03 <<24ul;
        Params.Alpha.Angle = 0.0;
        Params.Delta.Angle = 0.0; 
        Params.Gamma.Angle = 0.0;
        Params.Alpha.StatusFlag |= AXIS_ENABLE;
        Params.Delta.StatusFlag |= AXIS_ENABLE;
        Params.Gamma.StatusFlag |= AXIS_ENABLE;
        Init = true;
    }
    Params.Alpha.Angle += (2.0 * PI /(360.0 * 200.0 * 16.0))*13.333333333334/5.0;
    while(!EndProcess){
        switch (State) {
        case MAIN_WINDOW:   // ***************************************************************************************************************
            if((*KeyPressed) & 0x80) {
                State = MENU;
                (*KeyPressed) ^= 0x80;
                PosX = 0;
                PosY = 0;
                DisplayClear();
                break;
            }
            if((*KeyPressed) & 0x40) { //Enter                
                State = OBSERV;                    
                PosX = 0;
                PosY = 0;
                (*KeyPressed) ^= 0x40;
                DisplayClear();
                break;
            }                        
            if(Params.Alpha.StatusFlag & AXIS_ENABLE){
                XtoTimeString((char*)&TmpValue, Params.Alpha.Angle, 1 );
                DrawMenuLine(0, (const char*)TextA, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                if(Params.Alpha.StatusFlag & AXIS_RUN){
                    color = 0;
                    Effect = NORMAL;
                } else {
                    color = 1;
                    Effect = INVERT;
                }
                FloodRectangle(A_FlagX+1,1,D_FlagX,9,color);
                OutTextXY(A_FlagX+3,2,(const char*)AlphaFlag,ARIAL_L,Effect);
                Params.Alpha.NeedToUpdate |= C_ANGLE;
            }
            if(Params.Delta.StatusFlag & AXIS_ENABLE){
                XtoTimeString((char*)&TmpValue, Params.Delta.Angle, 0 );
                DrawMenuLine(1, (const char*)TextD, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                if(Params.Delta.StatusFlag & AXIS_RUN){
                    color = 0;
                    Effect = NORMAL;
                } else {
                    color = 1;
                    Effect = INVERT;
                }
                FloodRectangle(D_FlagX+1,1,G_FlagX,9,color);
                OutTextXY(D_FlagX+3,2,(const char*)DeltaFlag,ARIAL_L,Effect);
                Params.Delta.NeedToUpdate |= C_ANGLE;
            }
            if(Params.Gamma.StatusFlag & AXIS_ENABLE){
                XtoTimeString((char*)&TmpValue, Params.Gamma.Angle, 0 );
                DrawMenuLine(2, (const char*)TextG, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                if(Params.Gamma.StatusFlag & AXIS_RUN){
                    color = 0;
                    Effect = NORMAL;
                } else {
                    color = 1;
                    Effect = INVERT;
                }
                FloodRectangle(G_FlagX+1,1,131,9,color);
                OutTextXY(G_FlagX+3,2,(const char*)GammaFlag,ARIAL_L,Effect);
                 Params.Gamma.NeedToUpdate |= C_ANGLE;
            }
            OutTextXY(2,53,(const char*)MenuB,ARIAL_B,NORMAL);
            OutTextXY(101,53,(const char*)TimeT,ARIAL_B,NORMAL);
            DrawRectangle(0,51,132,63,1);
            DrawRectangle(0,0,132,10,1);   
            Line(36,52,36,63,1);
            Line(99,52,99,63,1);

            if(Params.Local.ConnectFlag){
                color = 0;
                Effect = NORMAL;
            } else {
                color = 1;
                Effect = INVERT;
            }
            FloodRectangle(Con_FlagX+1,1,A_FlagX ,9,color);
            OutTextXY(Con_FlagX+3,2,(const char*)ConnectFlag,ARIAL_L,Effect);

            EndProcess = true;
            break;
        case MENU: // ***************************************************************************************************************
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
                DisplayClear();
                break;
            } else {
                if(Selected == ESC) {
                    DisplayClear();
                    break;
                }
            }            
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)MenuB,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 2);
            DrawMenuLine(0, (const char*)ObservName, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(1, (const char*)SettingsName, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            EndProcess = true;
            break;
        case SETTINGS: // ***************************************************************************************************************
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
                DisplayClear();
                break;
            } else {
                if(Selected == ESC) {
                    DisplayClear();
                    break;
                }
            }            
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)SettingsName,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);        
            DrawMenuLine(0,(const char*)S_NetName, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(1,(const char*)S_MontyName, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(2,(const char*)S_Display, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(3,(const char*)S_Observ, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            
            EndProcess = true;
            break;
        case S_MONTY: // ***************************************************************************************************************
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
                DisplayClear();
                break;
            }else {
                if(Selected == ESC) 
                {
                    DisplayClear();
                    break;
                }
            }            
            
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)S_MontyName,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);        
            DrawMenuLine(0,(const char*)S_MontyTypeName, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(1,(const char*)S_AlphaName, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(2,(const char*)S_DeltaName, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(3,(const char*)S_GammaName, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);            

            EndProcess = true;
            break;
        case S_NETWORK: // ***************************************************************************************************************
            Selected = ProcessKeys(KeyPressed, &PosX, 7, &PosY, 0, SETTINGS, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
                switch(SelPosX){
                case 1:
                     EditTxt = (char*)SN_IP;
                     TmpDWValue = &Params.Local.IP;
                     break;
                case 2:
                    EditTxt = (char*)SN_Mask;
                    TmpDWValue = &Params.Local.Mask;
                    break;
                case 3:
                    EditTxt = (char*)SN_Gate;
                    TmpDWValue = &Params.Local.Gate;
                    break;
                case 4:
                    EditTxt = (char*)SN_DNS1;
                    TmpDWValue = &Params.Local.DNS1;
                    break;
                case 5:
                    EditTxt = (char*)SN_DNS2;
                    TmpDWValue = &Params.Local.DNS2;
                    break; 
                default:
                    TmpDWValue = NULL;
                    break;
                }   
                if(TmpDWValue != NULL) {
                    IPtoText(*TmpDWValue,(char*)TmpValue, 1);
                    State = EDIT_IP;
                    LastState = S_NETWORK;
                    LastPosX = SelPosX;
                    LastPosY = SelPosY;
                }
                DisplayClear();
                break;
            } else {
                if(Selected == ESC) {
                    DisplayClear();
                    break;
                }
            }            
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)S_NetName,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 7);              
            DrawMenuLine(0,(const char*)SN_Name, (const char*)Params.Local.Name, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            IPtoText(Params.Local.IP,   (char*)TmpValue, 0);
            DrawMenuLine(1,(const char*)SN_IP, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            IPtoText(Params.Local.Mask, (char*)TmpValue, 0);
            DrawMenuLine(2,(const char*)SN_Mask, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            IPtoText(Params.Local.Gate, (char*)TmpValue, 0);
            DrawMenuLine(3,(const char*)SN_Gate, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            IPtoText(Params.Local.DNS1, (char*)TmpValue, 0);
            DrawMenuLine(4,(const char*)SN_DNS1, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);            
            IPtoText(Params.Local.DNS2, (char*)TmpValue, 0);
            DrawMenuLine(5,(const char*)SN_DNS2, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);   
            IPtoText(Params.Local.NTP, (char*)TmpValue, 0);
            DrawMenuLine(6,(const char*)SN_NTP, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);            

            EndProcess = true;
            break;
        case SM_TYPESELECT: // ***************************************************************************************************************
            Selected = ProcessKeys(KeyPressed, &PosX, 0, &PosY, 0, S_MONTY, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
                DisplayClear();
            } else {
                if(Selected == ESC){ 
                    DisplayClear();
                    break;
                }
            }            
            EndProcess = true;
            break;
        case OBSERV: // ***************************************************************************************************************
            Selected = ProcessKeys(KeyPressed, &PosX, 3, &PosY, 0, MENU, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter  
                switch(SelPosX){
                case 0:
                    State = O_GOTO;
                    break;                
                }    
                DisplayClear();
                break;
            } else {
                if(Selected == ESC) {
                    DisplayClear();
                    break;
                }
            }             
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)ObservName,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 3);        
            //DrawMenuLine(0,O_GoTo, NULL, PosX, 0, SELECT_LINE);
            DrawMenuLine(0,(const char*)O_GoTo, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(1,(const char*)O_Manual, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(2,(const char*)O_Space, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);                      
            
            EndProcess = true;
            break;
        case O_GOTO: // ***************************************************************************************************************
            Selected = ProcessKeys(KeyPressed, &PosX, 4, &PosY, 0, OBSERV, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
                switch(SelPosX){
                case 0:
                    EditTxt = (char*)TextA;                    
                    TmpDoValue = &Params.Alpha.TargetAngle;
                    TmpIsHours = true;
                    XtoTimeString((char*)TmpValue,*TmpDoValue, 1);
                    break;
                case 1:
                    EditTxt = (char*)TextD;                    
                    TmpDoValue = &Params.Delta.TargetAngle;
                    TmpIsHours = false;
                    XtoTimeString((char*)TmpValue,*TmpDoValue, 0);
                    break;
                case 2:
                    EditTxt = (char*)TextG;                    
                    TmpDoValue = &Params.Gamma.TargetAngle;
                    TmpIsHours = false;
                    XtoTimeString((char*)TmpValue,*TmpDoValue, 0);
                    break;
                case 3:
                    State = MAIN_WINDOW;  
                    // TODO: отправка команды на перевод телескопа на координаты
                    if(Params.Alpha.StatusFlag & AXIS_ENABLE) {
                        Params.Alpha.NeedToCommit |= T_ANGLE;
                        Params.NeedToCommit |= ALPHA;
                    }
                    if(Params.Delta.StatusFlag & AXIS_ENABLE) {
                        Params.Delta.NeedToCommit |= T_ANGLE;
                        Params.NeedToCommit |= DELTA;
                    }
                    if(Params.Gamma.StatusFlag & AXIS_ENABLE) {
                        Params.Gamma.NeedToCommit |= T_ANGLE;
                        Params.NeedToCommit |= GAMMA;                    
                    }
                    LastState = O_GOTO;
                    TmpDoValue = NULL;
                    // Как-то надо возвращать ошибку в виде окна на экране
                    break;
                default:
                    TmpDoValue = NULL;
                    break;
                }   
                if(TmpDoValue != NULL) {                    
                    State = EDIT_ANGLE;
                    LastState = O_GOTO;
                    LastPosX = SelPosX;
                    LastPosY = SelPosY;
                }
                DisplayClear();
                break;
            } else {
                if(Selected == ESC) {
                    DisplayClear();
                    break;
                }
            }                
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)O_GoTo,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);        
            XtoTimeString((char*)&TmpValue, Params.Alpha.TargetAngle, 1 );
            DrawMenuLine(0, (const char*)TextA, (const char*)TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            XtoTimeString((char*)&TmpValue, Params.Delta.TargetAngle, 0 );
            DrawMenuLine(1, (const char*)TextD, (const char*)TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            XtoTimeString((char*)&TmpValue, Params.Gamma.TargetAngle, 0 );
            DrawMenuLine(2, (const char*)TextG, (const char*)TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);    
            DrawMenuLine(3, (const char*)TextStart, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);  
            EndProcess = true;
            break;
        case EDIT_IP: {// ***************************************************************************************************************
            int i;
            TmpPosY = PosY;   
            PosX = '9' - TmpValue[PosY];
            Selected = ProcessKeys(KeyPressed, &PosX, 10, &PosY, strlen((const char*)TmpValue) + 1, LastState, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
                //(*TmpDWValue)
                for(i = 0;i < strlen((const char*)TmpValue);i++){
                    if(TmpValue[i]=='.') TmpValue[i] = ' ';
                }
                {                
                    int UB=0;
                    int MB=0;
                    int HB=0;
                    int LB=0;
                    int c = 0;                    
                    c = SubStrToInt((const char*)TmpValue, 0, &LB);
                    c = SubStrToInt((const char*)TmpValue, c, &HB);
                    c = SubStrToInt((const char*)TmpValue, c, &UB);
                    c = SubStrToInt((const char*)TmpValue, c, &MB);

                    TmpDWval.byte.UB = (BYTE)UB;
                    TmpDWval.byte.MB = (BYTE)MB;
                    TmpDWval.byte.HB = (BYTE)HB;
                    TmpDWval.byte.LB = (BYTE)LB;
                }
                (*TmpDWValue) = TmpDWval.Val;
                TmpDWValue = NULL;
                State = LastState;
                PosX = LastPosX;
                PosY = LastPosY;
                DisplayClear();
                break;
            } else {
                if(Selected == ESC) {
                    TmpDWValue = NULL;                    
                    PosX = LastPosX;
                    PosY = LastPosY;
                    DisplayClear();
                    break;
                }
            }
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)EditTxt,ARIAL_L,NORMAL);
            //DrawScrollBar(PosX, 1);    
            
            if(PosY>=0){                
                if(TmpValue[PosY] == '.'){
                    if(TmpPosY < PosY) {
                        PosY++;         
                    }else PosY--;
                }
                if(TmpPosY != PosY){
                    PosX = '9' - TmpValue[PosY];
                }
                TmpValue[PosY] = '9' - PosX;
            }
            DrawMenuLine(0, NULL, TmpValue, 0, PosY, SELECT_COLUMN|FONT_TYPE_B);
            EndProcess = true;
            break;
        }    
        case EDIT_ANGLE: {// ***************************************************************************************************************
            int i;
            BYTE MaxX = 10;
            BYTE TmpPosX = PosX;
            TmpPosY = PosY;            
            if(PosY == 0) MaxX = 2;
            Selected = ProcessKeys(KeyPressed, &PosX, MaxX, &PosY, strlen((const char*)TmpValue) + 1, LastState, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter                   
                TextToTimeD(TmpValue, TmpIsHours, TmpDoValue);
                TmpDoValue = NULL;
                State = LastState;
                PosX = LastPosX;
                PosY = LastPosY;
                DisplayClear();
                break;
            } else {
                if(Selected == ESC) {
                    TmpDWValue = NULL;                   
                    PosX = LastPosX;
                    PosY = LastPosY;
                    DisplayClear();
                    break;
                }
            }             
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)EditTxt,ARIAL_L,NORMAL);
            //DrawScrollBar(PosX, 1);   
            if(PosY>=0){                
                if((TmpValue[PosY] == '.')||(TmpValue[PosY] == '`')||(TmpValue[PosY] == '\'')||(TmpValue[PosY] == '"')){
                    if(TmpPosY < PosY) {
                        PosY++;         
                    }else PosY--;
                }
                if(PosY ==0){
                    if(TmpPosX != PosX){
                        if(TmpValue[PosY]=='+'){
                            TmpValue[PosY]='-';
                        } else {
                            TmpValue[PosY]='+';
                        }
                        PosX = TmpValue[PosY];
                    }   
                } else {
                    if(TmpPosY != PosY){
                        PosX = '9' - TmpValue[PosY];
                    }
                    TmpValue[PosY] = '9' - PosX;
                }
            }
            DrawMenuLine(0, NULL, TmpValue, 0, PosY, SELECT_COLUMN|FONT_TYPE_B);
            EndProcess = true;
            break;
            }        
        case ERROR_COORDINATE: // ***************************************************************************************************************
            PosX = 4;
            Selected = ProcessKeys(KeyPressed, &PosX, 5, &PosY, 0, LastState, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter 
                State = LastState;
                DisplayClear();
                break;
            } else {
                if(Selected == ESC){ 
                    DisplayClear();
                    break;
                }
            }
            DrawRectangle(0,0,132,10,1);
            OutTextXY(15,2,(const char*)MsgError,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 5);
            DrawMenuLine(0, CoordNotReachable0, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(1, CoordNotReachable1, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(2, CoordNotReachable2, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(3, CoordNotReachable3, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(4, MsgContinue, NULL, PosX, PosY, SELECT_LINE);
            EndProcess = true;
            break;
        default: // ***************************************************************************************************************
            Selected = ProcessKeys(KeyPressed, &PosX, 3, &PosY, 0, MENU, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter  
                DisplayClear();
            } else {
                if(Selected == ESC) {
                    DisplayClear();
                    break;
                }
            } 
            EndProcess = true;
            break;
        }
    }
}

void DrawMenuLine( BYTE ID, const char * Name, const char * Value, int PosY, int PosX, BYTE Mode )
{
    BYTE color =0;
    BYTE Line;
    char * ptr = (char*)Value;
    EFFECT Effect;
    EFFECT Effect1;
    static int CPosY = 0;
    static int CPosX = 0;
    BYTE fsize = 0;
    int i = 0;
    int ValueLength = 0;
    WORD StringXPos = 0;
    FONT F = ARIAL_B;
    if(PosY < 0) return;
    if(PosY == 0) CPosY = 0;
    if(Mode & FONT_TYPE_B) {
        while(PosY > CPosY + 3) CPosY++;
    } else {
        while(PosY > CPosY + 4) CPosY++;
    }
    while(PosY < CPosY) CPosY--;
    if(PosX < 0) return;
    if(PosX == 0) CPosX = 0;
    while(PosX > CPosX) CPosX++;
    while(PosX < CPosX) CPosX--;
    if((Mode & SELECT_COLUMN)||(Mode & SELECT_LINE)){
        if(PosY == ID){
            color = 1;
            Effect = INVERT;
            Effect1 = NORMAL;
        } else {
            color = 0;
            Effect = NORMAL;
            Effect1 = INVERT;
        }
    } else {
        color = 0;
        Effect = NORMAL;
        Effect1 = NORMAL;
    }
    
    if(Mode & FONT_TYPE_B) {
        F = ARIAL_B;
        fsize = 13;
        Line = Line0 + fsize*((int)ID - CPosY);   
        if(((int)ID - CPosY<0)||((int)ID - CPosY>3)) return;
    } else {
        F = ARIAL_L;
        fsize = 10;
        Line = Line0 + fsize*((int)ID - CPosY);
        if(((int)ID - CPosY < 0) || ((int)ID - CPosY > 4)) return;
    }

    FloodRectangle(1,Line,120,Line+fsize,color);
    if(Name != NULL)
        StringXPos = OutTextXY(5,Line + 2,Name,F,Effect); 
    if(Value!= NULL){
        ValueLength = strlen((const char*)Value);
        if(Mode & SELECT_COLUMN ){            
            StringXPos = OutTextXYx(StringXPos + 5,Line + 2,(const char*)Value, PosX, F,Effect);
            StringXPos = OutTextXYx(StringXPos,Line + 2,(const char*)&Value[PosX], 1, F, Effect1 );
            if(PosX < ValueLength)
                OutTextXYx(StringXPos,Line + 2,(const char*)&Value[PosX + 1], ValueLength - PosX, F, Effect );
        } else {
            OutTextXY(StringXPos + 5,Line + 2,(const char*)Value, F,Effect);
        }
    }
}
void DrawScrollBar(int Pos, int Max)
{
    int Size = 53 / Max;
    int Y = Pos * Size;
    FloodRectangle(122,10,132,63,0);
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
    BYTE Sign = 1;    
    Xg = X * 180/(PI); // в часах времени
    if (Xg < 0) Sign = 0;
    if (hour) {
        Xg /= 15.0;
    }
    if(Xg < 0.0) Xg = -Xg;
    Grad = (signed short)Xg;    
    Xg -= (double)Grad;
    Xg *= 60;    
    Min = (BYTE)(Xg);
    Xg -= (double)Min;
    Xg *= 60;
    Sec = (BYTE)(Xg);
    Xg -= (double)Sec;
    Xg *= 100;
    mSec = (BYTE)(Xg);
    sprintf (Text, "%0.1s%0.2d%0.1s%0.2d'%0.2d.%0.2d\"",Sign?"+":"-",Grad,hour?"h":"`",Min,Sec,mSec);

}

void IPtoText (DWORD IP, char * Text, BOOL ForEdit)
{
    DWORD_VAL T;
    T.Val = IP;
    if(ForEdit){
        sprintf (Text, "%0.3d.%0.3d.%0.3d.%0.3d", T.byte.LB,T.byte.HB,T.byte.UB,T.byte.MB);
    } else {
        sprintf (Text, "%d.%d.%d.%d", T.byte.LB,T.byte.HB,T.byte.UB,T.byte.MB);
    }
}
// распознает число с заданной позиции до любого символа, не являющегося числом int
int SubStrToInt(const char* Text, int Beg, int * Val)
{
    int i = 0;    
    int Tmp = 0;
    int Sig = 1;
    int Tsize = strlen(Text);
    for(i = Beg; i < Tsize;i++){
        if((Text[i] == '+') || (Text[i] == '-')||((Text[i] >= '0')&&(Text[i] <= '9'))){
            if(Text[i] == '+') {Sig = 1;} else {
                if(Text[i] == '-') {Sig = -1;} else {                    
                    Tmp *= 10;
                    Tmp += Text[i] - '0';
                }            
            }
        } else break;
    }
    (*Val) = Tmp*Sig;
    return i+1;
}
void TextToTimeD(char* TmpValue, BOOL TmpIsHours, double * TmpDoValue)
{
    int i = 0;
    int UB=0;
    int MB=0;
    int HB=0;
    int LB=0;
    double Xg = 0.0;                    
    int c = 0;  

    for(i = 0;i<strlen((const char*)TmpValue);i++){
        switch(TmpValue[i]){
            case '`':
            case 'h':
            case '.':
            case '\'':
            case '"':
                TmpValue[i] = ' ';
        }
    }
    {                          
        c = SubStrToInt((const char*)TmpValue, 0, &MB);
        c = SubStrToInt((const char*)TmpValue, c, &UB);
        c = SubStrToInt((const char*)TmpValue, c, &HB);
        c = SubStrToInt((const char*)TmpValue, c, &LB);

        Xg = (double)LB/100.0;
        Xg += (double)HB;
        Xg /= 60.0;
        Xg += (double)UB;
        Xg /= 60.0;

        if(MB >= 0){
            Xg += (double)MB;
        } else {
            Xg = -Xg + (double)MB;
        }
        if((MB == 0)&&(TmpValue[0]=='-')){
            Xg = -Xg;
        }
        if(TmpIsHours){
            Xg *= 15.0;
        }
        Xg = Xg * (PI)/180.0;
        (*TmpDoValue) = Xg;
    } 
}

void ExecuteCommands()
{

}