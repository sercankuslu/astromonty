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
#include "roundbuffer.h"
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

#ifndef _WINDOWS
#pragma romdata overlay MSG_SECTION
#define C_ROM const rom
#else 
#define C_ROM const
#endif

typedef enum MSGS {
    MSG_NOMSG,
    MSG_MW_ALPHA, MSG_MW_DELTA, MSG_MW_GAMMA, MSG_MW_MENU, MSG_C_NET, 
    MSG_C_ALPHA, MSG_C_DELTA, MSG_C_GAMMA, 
    MSG_M_SETTINGS, MSG_M_S_OBSERV, 
    MSG_MO_GOTO, MSG_MO_MANUAL, MSG_MO_SPACECRAFT,
    MSG_C_START, MSG_C_CONTINUE,
    MSG_S_MONTY, MSG_S_DISPLAY, 
    MSG_SM_TYPE,
    MSG_SNL_NAME, MSG_SN_NAME, MSG_SN_IP, MSG_SN_MASK, MSG_SN_GATE, MSG_SN_DNS1, MSG_SN_DNS2, MSG_SN_NTP,
    MSG_C_ERROR,
    MSG_ERR_NOTREACHABLE0,MSG_ERR_NOTREACHABLE1, MSG_ERR_NOTREACHABLE2, MSG_ERR_NOTREACHABLE3,
}MSGS;
static C_ROM char * const MsgsCommon[]=
{    
    "",
    "А:", "Д:", "Г:", "Меню", "Сеть",
    "Альфа", "Дельта", "Гамма",       
    "Настройки",  "Наблюдение", 
    "Навести", "Ручной режим", "Режим спутников",
    "Старт", "Продолжить", 
    "Монтировка", "Экран", 
    "Тип монтировки",
    "AS-CONTROL", "Имя:", "IP:", "Mask:", "Gate:", "DNS1:", "DNS2:", "NTP:"
    "Ошибка"
    "Указанные координаты", "в данный момент вре-", "мени находятся вне", "зоны видимости",
};

#ifndef _WINDOWS
#pragma romdata
#endif
ALL_PARAMS Params;

void DrawMenuLine( BYTE ID, MSGS Msg_id, const char * Value, int PosY,int PosX , BYTE Mode );
void DrawScrollBar(int Pos, int Max);
void XtoTimeString( char * Text, double X, BOOL hour );
BYTE ProcessKeys(BYTE * KeyPressed, BYTE * PosX, BYTE MaxX, BYTE* PosY, BYTE MaxY, MENU_ID LastState, MENU_ID * State, BYTE * SelPosX, BYTE * SelPosY);
void IPtoText (DWORD IP, char * Text, BOOL ForEdit);
int SubStrToInt(const char* Text, int Beg, int * Val);
void TextToTimeD(char* TmpValue, BOOL TmpIsHours, double * TmpDoValue);
void GetMsgFromROM(MSGS Msg_id, char* Msg);

void ProcessMenu( BYTE * KeyPressed )
{
    static double Xa = 0.0;//91.3 * PI / 180.0;
    static double Xd = -33.1 * PI / 180.0;
    static double Xg = 47.2 * PI / 180.0;
    static char TimeT[] = "23:56";    
    

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
    static char TmpValue[25];
    static char MsgValue[25];
    static DWORD * TmpDWValue = NULL;
    static double * TmpDoValue = NULL;
    static BOOL TmpIsHours = false;
    BYTE Selected = 0;
    static MSGS EditTxt = MSG_NOMSG; 
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
        Params.Alpha.Angle = 0;
        Params.Delta.Angle = 3.14/2; 
        Params.Gamma.Angle = 3.14/4;
        Params.Alpha.StatusFlag = AXIS_ENABLE;
        Params.Delta.StatusFlag = AXIS_ENABLE;
        Params.Gamma.StatusFlag = AXIS_ENABLE;
        Params.Alpha.StatusFlag |= AXIS_RUN;
        Params.Delta.StatusFlag |= AXIS_RUN;
        Params.Gamma.StatusFlag |= AXIS_RUN;
        Params.NeedToUpdate = 0;
        Params.Alpha.NeedToUpdate = 0;
        GetMsgFromROM(MSG_SNL_NAME, (char*)&Params.Local.Name);
        memset(TmpValue,0,sizeof(TmpValue));
        Init = true;            
    }
    //Params.Alpha.Angle += (2.0 * PI /(360.0 * 200.0 * 16.0))*13.333333333334/5.0;
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
                XtoTimeString((char*)&TmpValue, Params.Alpha.Angle, 0 );                
                DrawMenuLine(0, MSG_MW_ALPHA, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                if(Params.Alpha.StatusFlag & AXIS_RUN){
                    color = 0;
                    Effect = NORMAL;
                } else {
                    color = 1;
                    Effect = INVERT;
                }
                FloodRectangle(A_FlagX+1,1,D_FlagX,9,color);
                GetMsgFromROM(MSG_C_ALPHA, (char*)&MsgValue);
                OutTextXY(A_FlagX+3,2,(const char*)MsgValue,ARIAL_L,Effect);
                Params.Alpha.NeedToUpdate |= C_ANGLE;
            }
            if(Params.Delta.StatusFlag & AXIS_ENABLE){
                XtoTimeString((char*)&TmpValue, Params.Delta.Angle, 0 );                
                DrawMenuLine(1, MSG_MW_DELTA, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                if(Params.Delta.StatusFlag & AXIS_RUN){
                    color = 0;
                    Effect = NORMAL;
                } else {
                    color = 1;
                    Effect = INVERT;
                }
                FloodRectangle(D_FlagX+1,1,G_FlagX,9,color);
                GetMsgFromROM(MSG_C_DELTA, (char*)&MsgValue);
                OutTextXY(D_FlagX+3,2,(const char*)MsgValue,ARIAL_L,Effect);
                Params.Delta.NeedToUpdate |= C_ANGLE;
            }
            if(Params.Gamma.StatusFlag & AXIS_ENABLE){
                XtoTimeString((char*)&TmpValue, Params.Gamma.Angle, 0 );                
                DrawMenuLine(2, MSG_MW_GAMMA, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                if(Params.Gamma.StatusFlag & AXIS_RUN){
                    color = 0;
                    Effect = NORMAL;
                } else {
                    color = 1;
                    Effect = INVERT;
                }
                FloodRectangle(G_FlagX+1,1,131,9,color);
                GetMsgFromROM(MSG_C_GAMMA, (char*)&MsgValue);
                OutTextXY(G_FlagX+3,2,(const char*)MsgValue,ARIAL_L,Effect);
                Params.Gamma.NeedToUpdate |= C_ANGLE;
            }
            GetMsgFromROM(MSG_MW_MENU, (char*)&MsgValue);
            OutTextXY(2,53,(const char*)MsgValue,ARIAL_B,NORMAL);
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
            GetMsgFromROM(MSG_C_NET, (char*)&MsgValue);
            OutTextXY(Con_FlagX+3,2,(const char*)MsgValue,ARIAL_L,Effect);
            Params.NeedToUpdate |= ALPHA|DELTA|GAMMA;
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
            GetMsgFromROM(MSG_MW_MENU, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 2);            
            DrawMenuLine(0, MSG_M_S_OBSERV, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);            
            DrawMenuLine(1, MSG_M_SETTINGS, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
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
            GetMsgFromROM(MSG_M_SETTINGS, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);     
            DrawMenuLine(0,MSG_C_NET, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(1,MSG_S_MONTY, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(2,MSG_S_DISPLAY, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(3,MSG_M_S_OBSERV, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            
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
            GetMsgFromROM(MSG_S_MONTY, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);        
            DrawMenuLine(0,MSG_SM_TYPE, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(1,MSG_C_ALPHA, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(2,MSG_C_DELTA, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(3,MSG_C_GAMMA, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);            

            EndProcess = true;
            break;
        case S_NETWORK: // ***************************************************************************************************************
            Selected = ProcessKeys(KeyPressed, &PosX, 7, &PosY, 0, SETTINGS, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
                switch(SelPosX){
                case 1:
                     EditTxt = MSG_SN_IP;
                     TmpDWValue = &Params.Local.IP;
                     break;
                case 2:
                    EditTxt = MSG_SN_MASK;
                    TmpDWValue = &Params.Local.Mask;
                    break;
                case 3:
                    EditTxt = MSG_SN_GATE;
                    TmpDWValue = &Params.Local.Gate;
                    break;
                case 4:
                    EditTxt = MSG_SN_DNS1;
                    TmpDWValue = &Params.Local.DNS1;
                    break;
                case 5:
                    EditTxt = MSG_SN_DNS2;
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
            GetMsgFromROM(MSG_C_NET, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 7);              
            DrawMenuLine(0,MSG_SNL_NAME, (const char*)Params.Local.Name, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            IPtoText(Params.Local.IP,   (char*)TmpValue, 0);
            DrawMenuLine(1,MSG_SN_IP, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            IPtoText(Params.Local.Mask, (char*)TmpValue, 0);
            DrawMenuLine(2,MSG_SN_MASK, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            IPtoText(Params.Local.Gate, (char*)TmpValue, 0);
            DrawMenuLine(3,MSG_SN_GATE, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            IPtoText(Params.Local.DNS1, (char*)TmpValue, 0);
            DrawMenuLine(4,MSG_SN_DNS1, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);            
            IPtoText(Params.Local.DNS2, (char*)TmpValue, 0);
            DrawMenuLine(5,MSG_SN_DNS2, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);   
            IPtoText(Params.Local.NTP, (char*)TmpValue, 0);
            DrawMenuLine(6,MSG_SN_NTP, TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);            

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
            GetMsgFromROM(MSG_M_S_OBSERV, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 3);        
            //DrawMenuLine(0,O_GoTo, NULL, PosX, 0, SELECT_LINE);
            DrawMenuLine(0,MSG_MO_GOTO, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(1,MSG_MO_MANUAL, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(2,MSG_MO_SPACECRAFT, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);                      
            
            EndProcess = true;
            break;
        case O_GOTO: // ***************************************************************************************************************
            Selected = ProcessKeys(KeyPressed, &PosX, 4, &PosY, 0, OBSERV, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
                switch(SelPosX){
                case 0:
                    EditTxt = MSG_C_ALPHA;                    
                    TmpDoValue = &Params.Alpha.TargetAngle;
                    TmpIsHours = true;
                    XtoTimeString((char*)TmpValue,*TmpDoValue, 1);
                    break;
                case 1:
                    EditTxt = MSG_C_DELTA;                    
                    TmpDoValue = &Params.Delta.TargetAngle;
                    TmpIsHours = false;
                    XtoTimeString((char*)TmpValue,*TmpDoValue, 0);
                    break;
                case 2:
                    EditTxt = MSG_C_GAMMA;                    
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
            GetMsgFromROM(MSG_MO_GOTO, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);        
            XtoTimeString((char*)&TmpValue, Params.Alpha.TargetAngle, 1 );
            DrawMenuLine(0, MSG_MW_ALPHA, (const char*)TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            XtoTimeString((char*)&TmpValue, Params.Delta.TargetAngle, 0 );
            DrawMenuLine(1, MSG_MW_DELTA, (const char*)TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            XtoTimeString((char*)&TmpValue, Params.Gamma.TargetAngle, 0 );
            DrawMenuLine(2, MSG_MW_GAMMA, (const char*)TmpValue, PosX, 0, SELECT_LINE|FONT_TYPE_B);    
            DrawMenuLine(3, MSG_C_START, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);  
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
                    int B=0;                    
                    int c = 0;                    
                    c = SubStrToInt((const char*)TmpValue, 0, &B);
                    TmpDWval.byte.LB = (BYTE)B;
                    c = SubStrToInt((const char*)TmpValue, c, &B);
                    TmpDWval.byte.HB = (BYTE)B;
                    c = SubStrToInt((const char*)TmpValue, c, &B);
                    TmpDWval.byte.UB = (BYTE)B;
                    c = SubStrToInt((const char*)TmpValue, c, &B);
                    TmpDWval.byte.MB = (BYTE)B; 
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
            GetMsgFromROM(EditTxt, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
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
            DrawMenuLine(0, MSG_NOMSG, TmpValue, 0, PosY, SELECT_COLUMN|FONT_TYPE_B);
            EndProcess = true;
            break;
        }    
        case EDIT_ANGLE: {// ***************************************************************************************************************
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
            GetMsgFromROM(EditTxt, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
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
            DrawMenuLine(0, MSG_NOMSG, TmpValue, 0, PosY, SELECT_COLUMN|FONT_TYPE_B);
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
            GetMsgFromROM(MSG_C_ERROR, (char*)&MsgValue);
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 5);
            DrawMenuLine(0, MSG_ERR_NOTREACHABLE0, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(1, MSG_ERR_NOTREACHABLE1, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(2, MSG_ERR_NOTREACHABLE2, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(3, MSG_ERR_NOTREACHABLE3, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(4, MSG_C_CONTINUE, NULL, PosX, PosY, SELECT_LINE);
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

void DrawMenuLine( BYTE ID, MSGS Msg_id, const char * Value, int PosY, int PosX, BYTE Mode )
{
    static int CPosY = 0;
    static int CPosX = 0;
    char Name[25] = "";
    WORD StringXPos = 0;
    int ValueLength = 0;
    BYTE color =0;
    BYTE Line;
    EFFECT Effect;
    EFFECT Effect1;
    BYTE fsize = 0;
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
    if(Msg_id != MSG_NOMSG){
        GetMsgFromROM(Msg_id, (char*)&Name);
        StringXPos = OutTextXY(5,Line + 2,Name,F,Effect); 
    }
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
    BYTE Grad;
    BYTE Min;
    BYTE Sec;
    BYTE mSec;
    BYTE Sign = 1;    
    Xg = X * 180/(PI); // в часах времени
    if (Xg < 0) Sign = 0;
    
    if(Xg < 0.0) Xg = -Xg;
    Grad = (BYTE)Xg;    
    Xg -= (double)Grad;
    Xg *= 60;    
    Min = (BYTE)(Xg);
    Xg -= (double)Min;
    Xg *= 60;
    Sec = (BYTE)(Xg);
    Xg -= (double)Sec;
    Xg *= 100;
    mSec = (BYTE)(Xg);
    if(Sign){
        if(hour){
            Grad /= 15;
            sprintf (Text, "+%0.2dh%0.2d'%0.2d.%0.2d\"", Grad,Min,Sec,mSec);
        } else sprintf (Text, "+%0.2d`%0.2d'%0.2d.%0.2d\"", Grad,Min,Sec,mSec);
    } else {
        if(hour){
            Grad /= 15;
            sprintf (Text, "-%0.2dh%0.2d'%0.2d.%0.2d\"", Grad,Min,Sec,mSec);
        } else sprintf (Text, "-%0.2d`%0.2d'%0.2d.%0.2d\"", Grad,Min,Sec,mSec);
    }

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

        if(TmpIsHours){
            MB *= 15;
        }
        if(MB >= 0){
            Xg += (double)MB;
        } else {
            Xg = -Xg + (double)MB;
        }
        if((MB == 0)&&(TmpValue[0]=='-')){
            Xg = -Xg;
        }        
        Xg = Xg * (PI)/180.0;
        (*TmpDoValue) = Xg;
    } 
}

void ExecuteCommands()
{
    static BYTE datareq = STC_REQEST_DATA;
    static ST_ATTRIBUTE RequestData[] = {
        {STA_COMMAND,  sizeof(BYTE), &datareq},
        {STA_NULL,  0, NULL},
    };
    static ST_ATTRIBUTE Data = {
        STA_NULL,  0, NULL
    };
    RB_RV rv;
    if(!IsClientConnected()){ 
        Params.Local.ConnectFlag = 0;
        return; 
    }else {
        Params.Local.ConnectFlag = 1;
    }

    while(IsDataInBuffer(IN_BUFFER)){
        BYTE m[10];
        Data.pValue = m;
        rv = PopAttr(&Data,IN_BUFFER);
        if(rv == RB_OK) {
            switch(Data.type){
                case STA_ALPHA:
                    memcpy(&Params.Alpha.Angle,m, sizeof(double));                    
                    break;
                case STA_DELTA:
                    memcpy(&Params.Delta.Angle,m, sizeof(double));
                    break;
                case STA_GAMMA:
                    memcpy(&Params.Gamma.Angle,m, sizeof(double));
                    break;
            }
        }
    }
    if(Params.NeedToUpdate>0){
        if(Params.NeedToUpdate & ALPHA){
            if(Params.Alpha.NeedToUpdate & C_ANGLE){
                rv = PushAttr(RequestData[0], OUT_BUFFER);
                RequestData[1].type = STA_ALPHA;
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                Params.Alpha.NeedToUpdate ^= C_ANGLE;
            }
        }
        if(Params.NeedToUpdate & DELTA){
            if(Params.Delta.NeedToUpdate & C_ANGLE){
                rv = PushAttr(RequestData[0], OUT_BUFFER);
                RequestData[1].type = STA_DELTA;
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                Params.Delta.NeedToUpdate ^= C_ANGLE;
            }
        }
        if(Params.NeedToUpdate & GAMMA){
            if(Params.Gamma.NeedToUpdate & C_ANGLE){
                rv = PushAttr(RequestData[0], OUT_BUFFER);
                RequestData[1].type = STA_GAMMA;
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                Params.Gamma.NeedToUpdate ^= C_ANGLE;
            }
        }
    }
    
    if(Params.NeedToCommit>0){

    }
}
void GetMsgFromROM(MSGS Msg_id, char* Msg)
{        
    int i = 0;        
    do{
        Msg[i] = MsgsCommon[Msg_id][i];        
    }while(Msg[i++] != '\0');        
    
}
