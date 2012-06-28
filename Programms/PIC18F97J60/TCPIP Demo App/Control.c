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


#include "DisplayBuffer.h"
#include "Control.h"

#include <stdio.h>

#define PI 3.1415926535897932384626433832795
// положение элементов
#define  Con_FlagX  0
#define  A_FlagX    26
#define  D_FlagX    57
#define  G_FlagX    98
#define  Line0      11
#define  Line1      24
#define  Line2      37
#define  Line3      50
#define  Line_L0      11
#define  Line_L1      21
#define  Line_L2      31
#define  Line_L3      41
#define  Line_L4      51
#define true 1
#define false 0
#define ENTER 1
#define ESC   2

#ifndef _WINDOWS
#pragma romdata overlay MSG_SECTION
#define C_ROM far const rom
// char *strncpypgm2ram (auto char *s1, auto const MEM_MODEL rom char *s2, auto sizeram_t n);
#else 
#define C_ROM const
#define rom
extern AppConfigType AppConfig;
#endif
typedef enum MENU_ID {
    MAIN_WINDOW, 
    MENU, SETTINGS, 
    OBSERV, O_GOTO, 
    S_OBSERV, S_NETWORK, S_MONTY,S_DISPLAY, 
    SM_TYPESELECT, SM_ALPHA, SM_DELTA, SM_GAMMA,
    EDIT_IP,EDIT_TIME,EDIT_ANGLE, 
    ERROR_COORDINATE,ERROR_NO_CONNECTION
} MENU_ID;
/*
typedef enum MSGS {
    MSG_NOMSG,
    MSG_MW_ALPHA, MSG_MW_DELTA, MSG_MW_GAMMA, MSG_MW_MENU, MSG_C_NET, MSG_MW_MODE_AUTO, MSG_MW_MODE_MANUAL, 
    MSG_C_ALPHA, MSG_C_DELTA, MSG_C_GAMMA, 
    MSG_M_SETTINGS, MSG_M_S_OBSERV, 
    MSG_MO_GOTO, MSG_MO_MANUAL, MSG_MO_SPACECRAFT,
    MSG_C_START, MSG_C_CONTINUE,
    MSG_S_MONTY, MSG_S_DISPLAY, 
    MSG_SM_TYPE,
    MSG_SNL_NAME, MSG_SN_NAME, MSG_SN_IP, MSG_SN_MASK, MSG_SN_GATE, MSG_SN_DNS1, MSG_SN_DNS2, MSG_SN_NTP,
    MSG_C_ERROR,
    MSG_ERR_NOTREACHABLE0,MSG_ERR_NOTREACHABLE1, MSG_ERR_NOTREACHABLE2, MSG_ERR_NOTREACHABLE3,
    MSG_ERR_NOCONNECTION0,MSG_ERR_NOCONNECTION1,MSG_ERR_NOCONNECTION2,MSG_ERR_NOCONNECTION3
}MSGS;
static C_ROM char * const MsgsCommon[]=
{    
    "",
    "А:", "Д:", "Г:", "Меню", "Сеть", "Авто наведение", "Ручное наведение", 
    "Альфа", "Дельта", "Гамма",       
    "Настройки",  "Наблюдение", 
    "Навести", "Ручной режим", "Режим спутников",
    "Старт", "Продолжить", 
    "Монтировка", "Экран", 
    "Тип монтировки",
    "AS-CONTROL", "Имя:", "IP:", "Mask:", "Gate:", "DNS1:", "DNS2:", "NTP:",
    "Ошибка",
    // TODO: сделать возможность автоматического разбиения строки 
    // TODO: сделать поддержку бегущей строки
    "Указанные координаты", "в данный момент вре-", "мени находятся вне", "зоны видимости",
    "Нет подключения к",    "серверу. Действие не", "доступно" , ""
};*/

typedef enum VAL_TYPE {
   VAL_BYTE, VAL_WORD, VAL_DWORD, VAL_FLOAT, VAL_STRING, VAL_IP_ADDRES, VAL_ANGLE, VAL_ANGLE_HOUR, VAL_NONE, VAL_FLAG
}VAL_TYPE;
typedef enum ITEM_TYPE {
    ITEM_FOLDER, ITEM_IP_ADDRES, ITEM_ANGLE, ITEM_MSG, ITEM_BUTTON, ITEM_STRING, ITEM_COMBO, ITEM_HEADER, ITEM_FOOTER, ITEM_VALUE
}ITEM_TYPE;
// тип постоянного элемента меню
typedef struct MENU_ITEMS_ROM {
   ITEM_TYPE Type;
   rom char * Name;
} MENU_ITEMS_ROM;
// тип изменяемого компонента элемента меню
typedef struct MENU_ITEMS_RAM {
   BYTE Id;
   VAL_TYPE ValType;
   void * Value; 
   PARAMS_FLAGS * Flags;
} MENU_ITEMS_RAM;
// 
typedef struct MENUS {
   BYTE Id;
   BYTE BrunchId;
} MENUS;
typedef struct MENU_STACK {
    BYTE LastId;
    BYTE LastPosY;
} MENU_STACK;
MENU_STACK MenuStack[10];
BYTE MStackHeap = 0;

#ifndef _WINDOWS
#pragma udata PARAMS_SECTION1
#endif

far ALL_PARAMS Params;

#ifndef _WINDOWS
#pragma udata
#endif

// список всех элементов меню 
const rom MENU_ITEMS_ROM MenuItems[] = {
    {ITEM_FOLDER,    "Главное окно"},              
    {ITEM_FOLDER,    "Меню"},                      
    {ITEM_ANGLE,     "¤:"},                        
    {ITEM_ANGLE,     "§:"},                        
    {ITEM_ANGLE,     "Г:"},                        
    {ITEM_FOLDER,    "Сеть"},                      
    {ITEM_MSG,       "Авто наведение"},            
    {ITEM_MSG,       "Ручное наведение"},          
    {ITEM_FOLDER,    "Альфа"},                     
    {ITEM_FOLDER,    "Дельта"},                    
    {ITEM_FOLDER,    "Гамма"},                     
    {ITEM_FOLDER,    "Настройки"},                 
    {ITEM_FOLDER,    "Наблюдение"},                
    {ITEM_FOLDER,    "Наблюдение"},                
    {ITEM_FOLDER,    "Навести"},                   
    {ITEM_FOLDER,    "Ручной режим"},              
    {ITEM_FOLDER,    "Режим спутников"},           
    {ITEM_BUTTON,    "Старт"},                     
    {ITEM_BUTTON,    "Продолжить"},                
    {ITEM_FOLDER,    "Монтировка"},                
    {ITEM_FOLDER,    "Экран"},                     
    {ITEM_FOLDER,    "Тип монтировки"},            
    {ITEM_STRING,    "Имя:"},                      
    {ITEM_IP_ADDRES, "IP:"},                       
    {ITEM_IP_ADDRES, "Mask:"},                     
    {ITEM_IP_ADDRES, "Gate:"},                     
    {ITEM_IP_ADDRES, "DNS1:"},                     
    {ITEM_IP_ADDRES, "DNS2:"},                     
    {ITEM_IP_ADDRES, "NTP:"},                      
    {ITEM_MSG,       "Ошибка"},                          
    {ITEM_MSG,       "Указанные координаты в данный момент времени находятся внезоны видимости"},     
    {ITEM_MSG,       "Нет подключения к серверу. Действие не доступно"},                              
    {ITEM_COMBO,     "Скорость:"},      
    {ITEM_HEADER,    "Сеть"},          
    {ITEM_HEADER,    "Альфа"},         
    {ITEM_HEADER,    "Дельта"},        
    {ITEM_HEADER,    "Гамма"},         
    {ITEM_ANGLE,     "¤:"},            
    {ITEM_ANGLE,     "§:"},            
    {ITEM_ANGLE,     "Г:"},            
    {ITEM_FOLDER,    "Наблюдение"},   
    {ITEM_COMBO,     "Тип:"},
    {ITEM_MSG,       "Азимутальная"}, 
    {ITEM_MSG,       "Экваториальная"}, 
    {ITEM_VALUE,     "10°"}, 
    {ITEM_VALUE,     "1°"}, 
    {ITEM_VALUE,     "10'"}, 
    {ITEM_VALUE,     "1'"}, 
    {ITEM_VALUE,     "10\""}, 
    {ITEM_VALUE,     "1\""}, 
};       
enum {
    IT_F_DESKTOP, IT_F_MENU, IT_A_ALPHA, IT_A_DELTA, IT_A_GAMMA, IT_F_NET, IT_M_AUTOAIM, IT_M_MANUALAIM, IT_F_ALPHA, IT_F_DELTA, IT_F_GAMMA,
    IT_F_OPTIONS, IT_F_OBSERV, IT_F_GOTO_MIROR, IT_F_GOTO, IT_F_MANUALMODE, IT_F_SPACEMODE, IT_B_START, IT_B_CONTINUE, IT_F_MONTYOPTION,
    IT_F_DISPLAY, IT_F_MONTYTYPE, IT_S_NETNAME, IT_IP_ADDRES, IT_IP_MASK, IT_IP_GATE, IT_IP_DNS1, IT_IP_DNS2, IT_IP_NTP,
    IT_M_ERROR, IT_M_ANGLEERROR, IT_M_CONNECTERROR, IT_C_SPEED, IT_H_NET, IT_H_ALPHA, IT_H_DELTA, IT_H_GAMMA,
    IT_A_TARGETALPHA, IT_A_TARGETDELTA, IT_A_TARGETGAMMA, IT_F_OBSERVOPT, IT_C_MONTYTYPE, IT_M_AZIMUT, IT_M_EQUATOR,
    IT_V_1, IT_V_2, IT_V_3, IT_V_4, IT_V_5
} ITEMS;

static MENU_ITEMS_RAM MenuItemsM[] = {
    {IT_A_ALPHA, VAL_ANGLE_HOUR, (void*)&Params.Alpha.Angle, &Params.Alpha.AngleFlag},
    {IT_A_DELTA, VAL_ANGLE, (void*)&Params.Delta.Angle, &Params.Delta.AngleFlag},
    {IT_A_GAMMA, VAL_ANGLE, (void*)&Params.Gamma.Angle, &Params.Gamma.AngleFlag},
    {IT_S_NETNAME, VAL_STRING, (void*)&Params.Local.Name, &Params.Local.NetFlags},
    {IT_IP_ADDRES, VAL_IP_ADDRES, (void*)&Params.Local.IP, &Params.Local.NetFlags},
    {IT_IP_MASK, VAL_IP_ADDRES, (void*)&Params.Local.Mask, &Params.Local.NetFlags},
    {IT_IP_GATE, VAL_IP_ADDRES, (void*)&Params.Local.Gate, &Params.Local.NetFlags},
    {IT_IP_DNS1, VAL_IP_ADDRES, (void*)&Params.Local.DNS1, &Params.Local.NetFlags},
    {IT_IP_DNS2, VAL_IP_ADDRES, (void*)&Params.Local.DNS2, &Params.Local.NetFlags},
    {IT_IP_NTP, VAL_IP_ADDRES, (void*)&Params.Local.NTP, &Params.Local.NetFlags},
    {IT_C_SPEED, VAL_FLOAT, (void*)&Params.Alpha.Speed, &Params.Alpha.SpeedFlag},
    {IT_F_DESKTOP, VAL_NONE, (void*)NULL, &Params.MainMenuFlag},
    {IT_H_NET, VAL_FLAG, (void*)&Params.Local.StatusFlag, &Params.Local.NetFlags},
    {IT_H_ALPHA, VAL_FLAG, (void*)&Params.Alpha.StatusFlag, &Params.Alpha.AngleFlag},
    {IT_H_DELTA, VAL_FLAG, (void*)&Params.Delta.StatusFlag, &Params.Delta.AngleFlag},
    {IT_H_GAMMA, VAL_FLAG, (void*)&Params.Gamma.StatusFlag, &Params.Gamma.AngleFlag},
    {IT_A_TARGETALPHA, VAL_ANGLE_HOUR, (void*)&Params.Alpha.TargetAngle, &Params.Alpha.TargetAngleFlag},
    {IT_A_TARGETDELTA, VAL_ANGLE, (void*)&Params.Delta.TargetAngle, &Params.Delta.TargetAngleFlag},
    {IT_A_TARGETGAMMA, VAL_ANGLE, (void*)&Params.Gamma.TargetAngle, &Params.Gamma.TargetAngleFlag},
    {IT_F_GOTO_MIROR, VAL_NONE, (void*)NULL, &Params.MainMenu2Flag},
};
// список папок
const rom MENUS Menus[] = {
    {IT_F_DESKTOP,   IT_A_ALPHA},     //Главное окно
    {IT_F_DESKTOP,   IT_A_DELTA},
    {IT_F_DESKTOP,   IT_A_GAMMA},
    {IT_F_DESKTOP,   IT_F_GOTO_MIROR},
    {IT_F_DESKTOP,   IT_H_NET},
    {IT_F_DESKTOP,   IT_H_ALPHA},
    {IT_F_DESKTOP,   IT_H_DELTA},
    {IT_F_DESKTOP,   IT_H_GAMMA},
    {IT_F_MENU,      IT_F_OBSERV},     //Меню
    {IT_F_MENU,      IT_F_OPTIONS},
    {IT_F_OPTIONS,   IT_F_NET},     //Настройки
    {IT_F_OPTIONS,   IT_F_MONTYOPTION},
    {IT_F_OPTIONS,   IT_F_DISPLAY},    
    {IT_F_OPTIONS,   IT_F_OBSERVOPT},
    {IT_F_NET,       IT_S_NETNAME},     //Сеть
    {IT_F_NET,       IT_IP_ADDRES},
    {IT_F_NET,       IT_IP_MASK},
    {IT_F_NET,       IT_IP_GATE},
    {IT_F_NET,       IT_IP_DNS1},
    {IT_F_NET,       IT_IP_DNS2},
    {IT_F_NET,       IT_IP_NTP},
    {IT_F_OBSERV,    IT_F_GOTO},    //Наблюдение
    {IT_F_OBSERV,    IT_F_MANUALMODE},
    //{IT_F_OBSERV,    IT_F_SPACEMODE},
    {IT_F_GOTO_MIROR,      IT_A_TARGETALPHA},     
    {IT_F_GOTO_MIROR,      IT_A_TARGETDELTA},
    {IT_F_GOTO_MIROR,      IT_A_TARGETGAMMA},
    {IT_F_GOTO_MIROR,      IT_C_SPEED},
    {IT_F_GOTO_MIROR,      IT_B_START},
    {IT_F_GOTO,      IT_A_TARGETALPHA},     //Навести
    {IT_F_GOTO,      IT_A_TARGETDELTA},
    {IT_F_GOTO,      IT_A_TARGETGAMMA},
    {IT_F_GOTO,      IT_C_SPEED},
    {IT_F_GOTO,      IT_B_START},
    {IT_F_MONTYOPTION,   IT_C_MONTYTYPE},
    {IT_C_MONTYTYPE,      IT_M_AZIMUT},
    {IT_C_MONTYTYPE,      IT_M_EQUATOR},
    {IT_C_SPEED,      IT_V_1},
    {IT_C_SPEED,      IT_V_2},
    {IT_C_SPEED,      IT_V_3},
    {IT_C_SPEED,      IT_V_4},
    {IT_C_SPEED,      IT_V_5},
   

};


// TODO: 
#ifndef _WINDOWS
//#pragma romdata

#else
#define strncpypgm2ram(Msg, Msg_id, x) strcpy(Msg, Msg_id)
static DWORD TIME_IN_SECONDS = 0;
DWORD SNTPGetUTCSeconds(){ 
    return TIME_IN_SECONDS;
}
#endif



void DrawMenuLine( BYTE ID,  C_ROM char * Msg_id, const char * Value, int PosY,int PosX , BYTE Mode );
void DrawScrollBar(int Pos, int Max);
void XtoTimeString( char * Text, float X, BOOL hour );
BYTE ProcessKeys(KEYS_STR * KeyPressed, BYTE * PosX, BYTE MaxX, BYTE* PosY, BYTE MaxY, BYTE * SelPosX, BYTE * SelPosY);
BYTE ModifyString(KEYS_STR * KeyPressed, BYTE* XPos, char * Value, VAL_TYPE ValType);
void IPtoText (DWORD IP, char * Text, BOOL ForEdit);
void TextToIP(char* TmpValue, DWORD * TmpDoValue);
int SubStrToInt(const char* Text, int Beg, int * Val);
void TextToTimeD(char* TmpValue, BOOL TmpIsHours, float * TmpDoValue);
void NewProcessMenu(BYTE * ItemId, KEYS_STR * KeyPressed);
//void GetMsgFromROM(MSGS Msg_id, char* Msg);

void ProcessMenu( KEYS_STR * KeyPressed )
{
    static BOOL Init = false;
    static BYTE Id = 0;
#ifdef _DISABLE_
    static float Xa = 0.0;//91.3 * PI / 180.0;
    static float Xd = (float)(-33.1 * PI / 180.0);
    static float Xg = (float)(47.2 * PI / 180.0);
    static char TimeT[] = "23:56";    
    static char SecondT[] = ".00";
    

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
    static char MsgValue[20];
    static DWORD * TmpDWValue = NULL;
    static float * TmpDoValue = NULL;
    static BOOL TmpIsHours = false;
    BYTE Selected = 0;
    static char EditTxt[20]; 
    DWORD_VAL TmpDWval;
    
    static int TimerCount = 0;
    static DWORD dwTime;
    static DateTime Date;
Params.Common.Flags.bits.NeedToRedrawTime = false;
    if(dwTime != SNTPGetUTCSeconds())
    {
        dwTime = SNTPGetUTCSeconds();      
        SecondsToTime(dwTime, &Date);
        Time_Run ^= 1;
        Params.Common.Flags.bits.NeedToRedrawTime = true;
        TimeT[0]   = '0' + Date.Hour / 10;
        TimeT[1]   = '0' + Date.Hour % 10;
        TimeT[3]   = '0' + Date.Min  / 10;
        TimeT[4]   = '0' + Date.Min  % 10;
        SecondT[1] = '0' + Date.Sec  / 10;
        SecondT[2] = '0' + Date.Sec  % 10;
        if(Time_Run){
            TimeT[2] = ':';
        } else {
            TimeT[2] = ' ';
        }
    }
#endif

    if(!Init){
        const char* TTT = "¤°";
        printf ("%s", TTT);
        AppConfig.MyIPAddr.Val = 0x3701A8C0;
        AppConfig.MyMask.Val = 0x00FFFFFF;
        AppConfig.MyGateway.Val = 0x0101A8C0;
        AppConfig.PrimaryDNSServer.Val = 0x0101A8C0;
        Params.Local.IP = AppConfig.MyIPAddr.Val;
        Params.Local.Mask = AppConfig.MyMask.Val;
        Params.Local.Gate = AppConfig.MyGateway.Val;
        Params.Local.DNS1 = AppConfig.PrimaryDNSServer.Val;
        Params.Local.DNS2 = AppConfig.SecondaryDNSServer.Val;
        Params.Local.Status = 0;
        Params.Local.NetFlags = PF_ENABLE | PF_CAN_SELECTED;   
        
        Params.Alpha.Angle = 0.0f;
        Params.Alpha.TargetAngle = 0.0f;
        Params.Alpha.Speed = 0.0f;
        Params.Alpha.SpeedFlag = PF_ENABLE | PF_CAN_SELECTED;
        Params.AlphaFlag = PF_ENABLE;
        Params.Alpha.AngleFlag = PF_ENABLE;
        Params.Alpha.TargetAngleFlag = PF_ENABLE | PF_CAN_SELECTED;
        Params.Alpha.StatusFlag.Run = 1;
    
        Params.Delta.Angle = 0.0f;
        Params.Delta.TargetAngle = 0.0f;
        Params.Delta.Speed = 0.0f;
        Params.DeltaFlag = PF_ENABLE;
        Params.Delta.AngleFlag = PF_ENABLE;
        Params.Delta.TargetAngleFlag = PF_ENABLE | PF_CAN_SELECTED;
        Params.Delta.StatusFlag.Run = 1;

        Params.Gamma.Angle = 0.0f;
        Params.Gamma.TargetAngle = 0.0f;                
        Params.Gamma.Speed = 0.0f;
        Params.GammaFlag = 0;
        Params.Gamma.AngleFlag = 0;
        Params.Gamma.TargetAngleFlag = 0;
        Params.Gamma.StatusFlag.Run = 0;

        Params.MainMenu2Flag = PF_ENABLE | PF_IS_HIDE | PF_CAN_SELECTED;
        Params.MainMenuFlag = PF_ENABLE | PF_ALT_MENU | PF_HIDE_SCROLL;

        strncpypgm2ram((char*)Params.Local.Name, "AC_CONTROL", sizeof(Params.Local.Name));        
        
        Params.Common.Flags.bits.NeedToRedrawMenus = true;

        Init = true;            
//         Params.Alpha.TargetAngle = 0.24f;
//         Params.NeedToCommit.bits.Alpha = 1;
//         Params.Alpha.NeedToCommit.bits.TargetAngle = 1;
    }
    //Params.Alpha.Angle += (2.0 * PI /(360.0 * 200.0 * 16.0))*13.333333333334/5.0;
    
    NewProcessMenu(&Id, KeyPressed);
#ifdef _DISABLE_
    EndProcess = true;
    while(!EndProcess){
        switch (State) {
        case MAIN_WINDOW:   // ***************************************************************************************************************
            if(KeyPressed->keys.esc) {
                State = MENU;
                KeyPressed->keys.esc ^= 1;
                PosX = 0;
                PosY = 0;
                DisplayClear();
                Params.Common.Flags.bits.NeedToRedrawMenus = true;
                break;
            }
            if(KeyPressed->keys.enter) { //Enter    
                if(Params.Local.Status){
                    State = OBSERV;
                } else {    
                    State = ERROR_NO_CONNECTION;
                    LastState = MAIN_WINDOW;
                }
                PosX = 0;
                PosY = 0;
                KeyPressed->keys.enter ^= 1;
                DisplayClear();
                Params.Common.Flags.bits.NeedToRedrawMenus = true;
                break;
            }                        
            if(Params.Alpha.StatusFlag.bits.Enable){
                if(Params.Alpha.IsModified.bits.Angle||Params.Common.Flags.bits.NeedToRedrawMenus){
                    XtoTimeString((char*)&TmpValue, Params.Alpha.Angle, 1 );                
                    DrawMenuLine(0, MSG_MW_ALPHA, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                    Params.Alpha.IsModified.bits.Angle = 0;
                }
                if(Params.Alpha.IsModified.bits.Flag||Params.Common.Flags.bits.NeedToRedrawMenus){
                    if(Params.Alpha.StatusFlag.bits.Run){
                        color = 0;
                        Effect = NORMAL;
                    } else {
                        color = 1;
                        Effect = INVERT;
                    }
                    FloodRectangle(A_FlagX,0,D_FlagX,8,color);
                    strncpypgm2ram((char*)&MsgValue, MSG_C_ALPHA, sizeof(MsgValue));
                    OutTextXY(A_FlagX+2,1,(const char*)MsgValue,ARIAL_L,Effect);
                    Params.Alpha.IsModified.bits.Flag = 0;
                }
                Params.Alpha.NeedToUpdate.bits.Angle = 1;
                Params.NeedToUpdate.bits.Alpha = 1;
            }
            if(Params.Delta.StatusFlag.bits.Enable){
                if(Params.Delta.IsModified.bits.Angle||Params.Common.Flags.bits.NeedToRedrawMenus){
                    XtoTimeString((char*)&TmpValue, Params.Delta.Angle, 0 );                
                    DrawMenuLine(1, MSG_MW_DELTA, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                    Params.Delta.IsModified.bits.Angle = 0;
                }
                if(Params.Delta.IsModified.bits.Flag||Params.Common.Flags.bits.NeedToRedrawMenus){
                    if(Params.Delta.StatusFlag.bits.Run){
                        color = 0;
                        Effect = NORMAL;
                    } else {
                        color = 1;
                        Effect = INVERT;
                    }
                    FloodRectangle(D_FlagX,0,G_FlagX,8,color);
                    strncpypgm2ram((char*)&MsgValue, MSG_C_DELTA, sizeof(MsgValue));
                    OutTextXY(D_FlagX+2,1,(const char*)MsgValue,ARIAL_L,Effect);
                    Params.Delta.IsModified.bits.Flag = 0;
                }
                Params.Delta.NeedToUpdate.bits.Angle = 1;
                Params.NeedToUpdate.bits.Delta = 1;
            }
            if(Params.Gamma.StatusFlag.bits.Enable){
                if(Params.Gamma.IsModified.bits.Angle||Params.Common.Flags.bits.NeedToRedrawMenus){
                    XtoTimeString((char*)&TmpValue, Params.Gamma.Angle, 0 );                
                    DrawMenuLine(2, MSG_MW_GAMMA, (const char*)TmpValue, 0, 0, NO_SELECT|FONT_TYPE_B);
                    Params.Delta.IsModified.bits.Angle = 0;
                }
                if(Params.Gamma.IsModified.bits.Flag||Params.Common.Flags.bits.NeedToRedrawMenus){
                    if(Params.Gamma.StatusFlag.bits.Run){
                        color = 0;
                        Effect = NORMAL;
                    } else {
                        color = 1;
                        Effect = INVERT;
                    }
                    FloodRectangle(G_FlagX,0,131,8,color);
                    strncpypgm2ram((char*)&MsgValue, MSG_C_GAMMA, sizeof(MsgValue));
                    OutTextXY(G_FlagX+2,1,(const char*)MsgValue,ARIAL_L,Effect);
                    Params.Gamma.IsModified.bits.Flag = 0;
                }
                Params.Gamma.NeedToUpdate.bits.Angle = 1;
                Params.NeedToUpdate.bits.Gamma = 1;
            }
            if(Params.Common.Flags.bits.NeedToRedrawTime||Params.Common.Flags.bits.NeedToRedrawMenus){
                WORD XX = 0;
                XX = OutTextXY(90,54,(const char*)TimeT,ARIAL_B,NORMAL);
                OutTextXY(XX,56,(const char*)SecondT,ARIAL_L,NORMAL);
                Params.Common.Flags.bits.NeedToRedrawTime = 0;
            }
           
            if(Params.Local.IsModified.bits.Flag||Params.Common.Flags.bits.NeedToRedrawMenus){
                if(Params.Local.Status){
                    color = 0;
                    Effect = NORMAL;
                } else {
                    color = 1;
                    Effect = INVERT;
                }
                FloodRectangle(Con_FlagX,0,A_FlagX ,8,color);
                strncpypgm2ram((char*)&MsgValue, MSG_C_NET, sizeof(MsgValue));
                OutTextXY(Con_FlagX+2,1,(const char*)MsgValue,ARIAL_L,Effect);
            }
            if(Params.Common.Flags.bits.NeedToRedrawMenus){
                Line(0,52,132,52,1);
                Line(0,9,132,9,1);
                if(1){
                    color = 0;
                    Effect = NORMAL;
                } else {
                    color = 1;
                    Effect = INVERT;
                }
                if(Params.Common.Flags.bits.Man_Auto)
                    strncpypgm2ram((char*)&MsgValue, MSG_MW_MODE_MANUAL, sizeof(MsgValue));
                else
                    strncpypgm2ram((char*)&MsgValue, MSG_MW_MODE_AUTO, sizeof(MsgValue));
                OutTextXY(2,54,(const char*)MsgValue,ARIAL_L, Effect);                
                //Line(36,52,36,63,1);
                Line(88,52,88,63,1);
                Params.Common.Flags.bits.NeedToRedrawMenus = 0;
            }
            EndProcess = true;
            break;
        case MENU: // ***************************************************************************************************************
            LastPosX = PosX;
            Selected = ProcessKeys(KeyPressed, &PosX, 2, &PosY, 0, MAIN_WINDOW, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { 
                switch(SelPosX){
                case 0:
                    if(Params.Local.Status){
                        State = OBSERV;
                    } else {    
                        State = ERROR_NO_CONNECTION;
                        LastState = MENU;
                    }
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
            if(LastPosX!=PosX) Params.Common.Flags.bits.NeedToRedrawMenus = 1;
            if(Params.Common.Flags.bits.NeedToRedrawMenus){
                DrawRectangle(0,0,132,10,1);
                strncpypgm2ram((char*)&MsgValue, MSG_MW_MENU, sizeof(MsgValue));
                OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
                DrawScrollBar(PosX, 2);            
                DrawMenuLine(0, MSG_M_S_OBSERV, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);            
                DrawMenuLine(1, MSG_M_SETTINGS, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
                Params.Common.Flags.bits.NeedToRedrawMenus = 0;
            }
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
            strncpypgm2ram((char*)&MsgValue, MSG_M_SETTINGS, sizeof(MsgValue));
            OutTextXY(15,2,(const char*)MsgValue, ARIAL_L,NORMAL);
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
            strncpypgm2ram((char*)&MsgValue, MSG_S_MONTY, sizeof(MsgValue));
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 4);        
            DrawMenuLine(0,MSG_SM_TYPE, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(1,MSG_C_ALPHA, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(2,MSG_C_DELTA, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);
            DrawMenuLine(3,MSG_C_GAMMA, NULL, PosX, 0, SELECT_LINE|FONT_TYPE_B);            

            EndProcess = true;
            break;
        case S_NETWORK: // ***************************************************************************************************************
            Params.Local.IP = AppConfig.MyIPAddr.Val;
            Params.Local.Mask = AppConfig.MyMask.Val;
            Params.Local.Gate = AppConfig.MyGateway.Val;
            Params.Local.DNS1 = AppConfig.PrimaryDNSServer.Val;
            Params.Local.DNS2 = AppConfig.SecondaryDNSServer.Val;
            Selected = ProcessKeys(KeyPressed, &PosX, 7, &PosY, 0, SETTINGS, &State, &SelPosX, &SelPosY);            
            if(Selected == ENTER) { //Enter   
                switch(SelPosX){
                case 1:
                     strncpypgm2ram((char*)&MsgValue, MSG_SN_IP, sizeof(MsgValue));
                     TmpDWValue = &Params.Local.IP;
                     break;
                case 2:
                    strncpypgm2ram((char*)&MsgValue, MSG_SN_MASK, sizeof(MsgValue));
                    TmpDWValue = &Params.Local.Mask;
                    break;
                case 3:
                    strncpypgm2ram((char*)&MsgValue, MSG_SN_GATE, sizeof(MsgValue));
                    TmpDWValue = &Params.Local.Gate;
                    break;
                case 4:
                    strncpypgm2ram((char*)&MsgValue, MSG_SN_DNS1, sizeof(MsgValue));
                    TmpDWValue = &Params.Local.DNS1;
                    break;
                case 5:
                    strncpypgm2ram((char*)&MsgValue, MSG_SN_DNS2, sizeof(MsgValue));
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
            strncpypgm2ram((char*)&MsgValue, MSG_C_NET, sizeof(MsgValue));
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 7);              
            DrawMenuLine(0,MSG_SN_NAME, (const char*)Params.Local.Name, PosX, 0, SELECT_LINE|FONT_TYPE_B);
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
            strncpypgm2ram((char*)&MsgValue, MSG_M_S_OBSERV, sizeof(MsgValue));
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
                    strncpypgm2ram((char*)&MsgValue, MSG_C_ALPHA, sizeof(MsgValue));
                    TmpDoValue = &Params.Alpha.TargetAngle;
                    TmpIsHours = true;
                    XtoTimeString((char*)TmpValue,*TmpDoValue, 1);
                    Params.Alpha.IsModified.bits.TargetAngle = 1;
                    break;
                case 1:
                    strncpypgm2ram((char*)&MsgValue, MSG_C_DELTA, sizeof(MsgValue));
                    TmpDoValue = &Params.Delta.TargetAngle;
                    TmpIsHours = false;
                    XtoTimeString((char*)TmpValue,*TmpDoValue, 0);
                    Params.Delta.IsModified.bits.TargetAngle = 1;
                    break;
                case 2:
                    strncpypgm2ram((char*)&MsgValue, MSG_C_GAMMA, sizeof(MsgValue));
                    TmpDoValue = &Params.Gamma.TargetAngle;
                    TmpIsHours = false;
                    XtoTimeString((char*)TmpValue,*TmpDoValue, 0);
                    Params.Gamma.IsModified.bits.TargetAngle = 1;
                    break;
                case 3:
                    State = MAIN_WINDOW;  
                    // отправка команды на перевод телескопа на координаты
                    if(Params.Alpha.StatusFlag.bits.Enable && Params.Alpha.IsModified.bits.TargetAngle) {
                        Params.Alpha.IsModified.bits.TargetAngle = 0;
                        Params.Alpha.NeedToCommit.bits.TargetAngle = 1;
                        Params.NeedToCommit.bits.Alpha = 1;
                    }
                    if(Params.Delta.StatusFlag.bits.Enable && Params.Delta.IsModified.bits.TargetAngle) {
                        Params.Alpha.IsModified.bits.TargetAngle = 0;
                        Params.Delta.NeedToCommit.bits.TargetAngle = 1;
                        Params.NeedToCommit.bits.Delta = 1;
                    }
                    if(Params.Gamma.StatusFlag.bits.Enable && Params.Gamma.IsModified.bits.TargetAngle) {
                        Params.Alpha.IsModified.bits.TargetAngle = 0;
                        Params.Gamma.NeedToCommit.bits.TargetAngle = 1;
                        Params.NeedToCommit.bits.Gamma = 1;                    
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
            strncpypgm2ram((char*)&MsgValue, MSG_MO_GOTO, sizeof(MsgValue));
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
            BYTE i;
            TmpPosY = PosY;   
            PosX = '9' - TmpValue[PosY];
            Selected = ProcessKeys(KeyPressed, &PosX, 10, &PosY, (BYTE)strlen((const char*)TmpValue) + 1, LastState, &State, &SelPosX, &SelPosY);            
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
            BYTE MaxX = 12;
            BYTE MaxY = (BYTE)strlen((const char*)TmpValue)+1;
            BYTE TmpPosX = PosX;
            TmpPosY = PosY; 
            switch (PosY) {  // +00h00'00"
            case 0: 
                PosY = 1; 
            case 1:
                MaxX = 4;
                break;            
            }
            Selected = ProcessKeys(KeyPressed, &PosX, MaxX, &PosY, MaxY, LastState, &State, &SelPosX, &SelPosY);            
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
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            //DrawScrollBar(PosX, 1);   
            if(PosY>0){                
                if((TmpValue[PosY-1] == '.')||
                   (TmpValue[PosY-1] == '`')||
                   (TmpValue[PosY-1] == '\'')||
                   (TmpValue[PosY-1] == '"')||
                   (TmpValue[PosY-1] == 'h')||
                   (TmpValue[PosY-1] == ':')){
                    if(TmpPosY < PosY) {
                        PosY++;
                        if(PosY >= MaxY) PosY = 1;
                    }else {
                        if(PosY == 0) PosY = MaxY - 1;
                        else PosY--;
                    }
                }
            }
            if(PosY == 0) PosY = MaxY - 2;
            if(PosY >= MaxY) PosY = 1; 
            if(PosX == 0) PosX = MaxX - 2;
            if(PosX >= MaxX - 1) PosX = 1;
            if(PosY == 1){                     
                if(TmpPosY != PosY){
                    PosX =  (TmpValue[PosY-1] == '-')?1:2;
                }
                if(TmpPosX != PosX){
                    if(PosX == 1)
                        TmpValue[PosY-1]='-';
                    else
                        TmpValue[PosY-1]='+';                        
                }   
            } else {
                if(TmpPosY != PosY){
                    PosX = '9' - TmpValue[PosY-1] + 1;
                }                    
                TmpValue[PosY-1] = '9' - PosX + 1;
            }
            
            DrawMenuLine(0, MSG_NOMSG, TmpValue, 0, PosY-1, SELECT_COLUMN|FONT_TYPE_B);
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
            strncpypgm2ram((char*)&MsgValue, MSG_C_ERROR, sizeof(MsgValue));            
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 5);
            DrawMenuLine(0, MSG_ERR_NOTREACHABLE0, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(1, MSG_ERR_NOTREACHABLE1, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(2, MSG_ERR_NOTREACHABLE2, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(3, MSG_ERR_NOTREACHABLE3, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(4, MSG_C_CONTINUE, NULL, PosX, PosY, SELECT_LINE);
            EndProcess = true;
            break;
         case ERROR_NO_CONNECTION: // ***************************************************************************************************************
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
            strncpypgm2ram((char*)&MsgValue, MSG_C_ERROR, sizeof(MsgValue));
            OutTextXY(15,2,(const char*)MsgValue,ARIAL_L,NORMAL);
            DrawScrollBar(PosX, 5);
            DrawMenuLine(0, MSG_ERR_NOCONNECTION0, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(1, MSG_ERR_NOCONNECTION1, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(2, MSG_ERR_NOCONNECTION2, NULL, PosX, PosY, NO_SELECT);
            DrawMenuLine(3, MSG_ERR_NOCONNECTION3, NULL, PosX, PosY, NO_SELECT);
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
    KeyPressed->Val = 0;  
#endif //#ifdef _DISABLE_
}

void DrawMenuLine( BYTE ID, C_ROM char * Msg_id, const char * Value, int PosX, int PosY, BYTE Mode )
{
    static int CPosY = 0;
    static int CPosX = 0;
    char Name[20] = "";
    WORD StringXPos = 0;
    WORD Xtmp = 0;
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
        Line = (BYTE)(Line0 + fsize*((int)ID - CPosY));
        if(((int)ID - CPosY<0)||((int)ID - CPosY>3)) return;
    } else {
        F = ARIAL_L;
        fsize = 10;
        Line = (BYTE)(Line0 + fsize*((int)ID - CPosY));
        if(((int)ID - CPosY < 0) || ((int)ID - CPosY > 4)) return;
    }

    FloodRectangle(1,Line,124,Line+fsize,color);
    if(Msg_id != NULL){
        strncpypgm2ram((char*)&Name, Msg_id, sizeof(Name));
        StringXPos = OutTextXY(5,Line + 2,Name,F,Effect); 
        Xtmp = StringXPos % 10;
        if( Xtmp > 0){
            StringXPos += 10 - Xtmp;
        }
    }
    if(Value!= NULL){
        ValueLength = strlen((const char*)Value);
        if(Mode & SELECT_COLUMN ){            
            StringXPos = OutTextXYx(StringXPos + 5,Line + 2,(const char*)Value, (BYTE)PosX, F,Effect);
            StringXPos = OutTextXYx(StringXPos,Line + 2,(const char*)&Value[PosX], 1, F, Effect1 );
            if(PosX < ValueLength)
                OutTextXYx(StringXPos,Line + 2,(const char*)&Value[PosX + 1], (BYTE)(ValueLength - PosX), F, Effect );
        } else {
            OutTextXY(StringXPos + 5,Line + 2,(const char*)Value, F,Effect);
        }
    }
}
void DrawScrollBar(int Pos, int Max)
{
    WORD Size = 0;
    WORD Y = 0;
    FloodRectangle(126,10,132,63,0);
    DrawRectangle(126,10,132,63,1);
    if(Max == 0) Max = 1;
    Size = (WORD)(53 / Max);
    Y = (WORD)(Pos * Size);
    FloodRectangle(127,11 + Y,131,11 + Y + Size,1);
}

void DrawMessageLine(C_ROM char * Msg_id, int Step, BOOL Blink)
{
    if((Msg_id == 0) || Step == 0 || Blink) return;
}

BYTE ProcessKeys(KEYS_STR * KeyPressed, BYTE* XPos, BYTE XMax, BYTE * YPos, BYTE YMax, BYTE * XSelPos, BYTE * YSelPos)
{
    if(KeyPressed->keys.esc) { //ESC
        (*YSelPos) = 0;
        (*XSelPos) = 0;
        (*YPos) = 0;
        (*XPos) = 0;
        KeyPressed->keys.esc = 0;
        Params.Common.Flags.bits.NeedToRedrawMenus = true;
        return ESC;
    }
    if(KeyPressed->keys.enter) { //Enter         
        (*YSelPos) = (*YPos);
        (*XSelPos) = (*XPos);
        (*YPos) = 0;
        (*XPos) = 0;
        KeyPressed->keys.enter = 0;
        Params.Common.Flags.bits.NeedToRedrawMenus = true;
        return ENTER;
    }
    if(KeyPressed->keys.up) { //UP
        if((*YPos) > 0) (*YPos)--;
        KeyPressed->keys.up = 0;      
    }
    if(KeyPressed->keys.down) { //DOWN
        if(YMax > 0)
            if((*YPos) < YMax - 1) (*YPos)++;
        KeyPressed->keys.down = 0;       
    }
    if(KeyPressed->keys.left) { //LEFT
        if((*XPos) > 0) (*XPos)--;
        KeyPressed->keys.left = 0;        
    }
    if(KeyPressed->keys.right) { //RIGHT
        if(XMax > 0)
            if((*XPos) < XMax-1) (*XPos)++;
        KeyPressed->keys.right = 0;        
    }
    return 0;
}


void XtoTimeString( char * Text, float X, BOOL hour )
{    
    double Xg;
    BYTE Grad;
    BYTE Min;
    BYTE Sec;
    BYTE mSec;
    BYTE Sign = 1;    
    Xg = X * 180/(PI); // в часах времени
    if (Xg < 0) Sign = 0;
    if(hour) Xg /= 15.0;
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
            /*Grad /= 15;*/
            sprintf (Text, "+%0.2dh%0.2d'%0.2d.%0.2d\"\0", Grad,Min,Sec,mSec);
        } else sprintf (Text, "+%0.2d°%0.2d'%0.2d.%0.2d\"\0", Grad,Min,Sec,mSec);
    } else {
        if(hour){
            /*Grad /= 15;*/
            sprintf (Text, "-%0.2dh%0.2d'%0.2d.%0.2d\"\0", Grad,Min,Sec,mSec);
        } else sprintf (Text, "-%0.2d°%0.2d'%0.2d.%0.2d\"\0", Grad,Min,Sec,mSec);
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
void TextToTimeD(char* TmpValue, BOOL TmpIsHours, float * TmpDoValue)
{
    int i = 0;
    int UB=0;
    int MB=0;
    int HB=0;
    int LB=0;
    double Xg = 0.0;                    
    int c = 0;  

    for(i = 0;i < (int)strlen((const char*)TmpValue);i++){
        switch(TmpValue[i]){
            case '°':
            case 'h':
            case '.':
            case '\'':
            case '"':
            case ':':
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
            //MB *= 15;
        }
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
            //MB *= 15;
        }
        Xg = Xg * (PI)/180.0;
        (*TmpDoValue) = (float)Xg;
    } 
}
void TextToIP(char* TmpValue, DWORD * TmpDoValue)
{
    BYTE i = 0;
    DWORD_VAL TmpDWval;
    int B=0;                    
    int c = 0;

    for(i = 0;i < strlen((const char*)TmpValue);i++){
        if(TmpValue[i]=='.') TmpValue[i] = ' ';
    }                       
    c = SubStrToInt((const char*)TmpValue, 0, &B);
    TmpDWval.byte.LB = (BYTE)B;
    c = SubStrToInt((const char*)TmpValue, c, &B);
    TmpDWval.byte.HB = (BYTE)B;
    c = SubStrToInt((const char*)TmpValue, c, &B);
    TmpDWval.byte.UB = (BYTE)B;
    c = SubStrToInt((const char*)TmpValue, c, &B);
    TmpDWval.byte.MB = (BYTE)B; 
    
    *TmpDoValue = TmpDWval.Val;
}
/*
void GetMsgFromROM(MSGS Msg_id, char* Msg)
{        
    int i = 0;        
    do{
        Msg[i] = MsgsCommon[Msg_id][i];        
    }while(Msg[i++] != '\0');        
    
}*/
void SecondsToTime(DWORD Seconds, DateTime * Date)
{
    DWORD Y = 70;
    DWORD DY;
    BYTE M = 1;
    BYTE D = 1;
    BYTE d = 1;//день недели
    BYTE h = 0;
    BYTE m = 0;
    BYTE s = 0;
    DWORD DM = 31;     
    DWORD s1; 
    DWORD dd;
    DWORD Days;      
    Days = Seconds/86400;
    //Seconds -= (DWORD)Days*86400;
    Seconds = Seconds % 86400;
    s1 = Days/7;
    dd = (DWORD)s1;
    d = (BYTE)((Days - dd*7) + 5);
    while(d>=7){d-=7;}

    dd  = Days/1461;
    Days -= (dd * 1461); //
    Y = (LONG)(dd*4 + 70);
    DY = 365; //1970
    while(Days >= DY){
        if((Y & 0x03) == 0){
            DY = 366; //366 дней           
        } else {
            DY = 365; //365 дней
        }        
        Y++;
        Days -= DY;           
    }   
    M = 1;
    while(Days>=DM){        
        switch (M){        
            case 4:case 6: case 9: case 11: 
                DM = 30;
                break;
            case 2:
                DM = 28;
                if((Y & 0x03) == 0){
                    DM = 29;                    
                }
                break;
            default:
                DM = 31;        
        }
        Days -= DM;       
        M++;
    }   
    D = (BYTE)(Days + 1);

    s1 = Seconds / 3600;
    h = (BYTE)s1;
    Seconds -= (DWORD)h * 3600;

    s1 = Seconds / 60;
    m = (BYTE)s1;
    Seconds -= (DWORD)m * 60;

    s = (BYTE)Seconds;

    Date->Sec = s;
    Date->Min = m;
    Date->Hour = h;
    Date->DayOfWeak = d;
    Date->Day = D;
    Date->Month = M;
    Date->Year = (WORD)Y;
 
}
WORD FindRAMItem(BYTE ItemId)
{
    WORD k = 0;
    static int RamMenusLen = 0;
    if(!RamMenusLen){
        RamMenusLen = sizeof(MenuItemsM)/sizeof(MenuItemsM[0]);
    }
    for(k = 0; k < RamMenusLen; k++){
        if(MenuItemsM[k].Id == ItemId) {
            return k;
        }
    }
    return 0;
}
void NewProcessMenu(BYTE * ItemId, KEYS_STR * KeyPressed)
{
    int i = 0;
    int j = 0;
    int k = 0;
    static BYTE PosX = 0;
    static BYTE PosY = 0;
    BYTE MaxX = 0;
    BYTE MaxY = 0;
    static BYTE SelPosX = 0;
    static BYTE SelPosY = 0;
    static MENU_ID State = SETTINGS;  // не нужен?
    BYTE Selected = 0;
    static int MenusLen = 0;
    static int RamMenusLen = 0;
    static char * TmpString = NULL;
    static BOOL BeginEdit = FALSE;
    DWORD * TmpDW = NULL;
    float * TmpFloat = NULL;
    static char TmpMsg[20] = "";
    static char TmpEditMsg[20] = "";
    static VAL_TYPE TmpValType = VAL_ANGLE;
    static WORD TmpIndex = 0;
    WORD wTmp = 0;
    PARAMS_FLAGS DefaultFlags = 0;
    PARAMS_FLAGS * TmpFlags = &DefaultFlags;
    EFFECT Effect;
    BOOL color = FALSE;
    BOOL TmpFlag = FALSE;
    // время
    WORD XX = 0;
    DWORD dwTime = 0;
    DateTime Date;
    char TimeT[] = "00:00";
    char SecondT[] = ".00";
    // координаты меню
    BYTE PX = 0;
    BYTE PX2 = 0;

    DefaultFlags = PF_ENABLE | PF_CAN_SELECTED;
    
    if(!Params.Common.Flags.bits.NeedToRedrawMenus && !KeyPressed->Val) 
        return;
    if(!MenusLen){
        MenusLen = sizeof(Menus)/sizeof(Menus[0]);
    }
    if(!RamMenusLen){
        RamMenusLen = sizeof(MenuItemsM)/sizeof(MenuItemsM[0]);
    }
    
    TmpFlags = &DefaultFlags;
    k = FindRAMItem(*ItemId);
    if(k != 0) TmpFlags = MenuItemsM[k].Flags;

    // заголовок
    if(!(*TmpFlags & PF_ALT_MENU)){
        // стандартный заголовок
        DrawRectangle(0,0,132,10,1);
        strncpypgm2ram((char*)&TmpMsg, MenuItems[*ItemId].Name, strlen(MenuItems[*ItemId].Name));
        OutTextXY(15,2,(const char*)TmpMsg,ARIAL_L,NORMAL);
    } else {
        // альтернативный заголовок
        Line(0,52,132,52,1);
        Line(0,9,132,9,1);
    }
    if(*TmpFlags & PF_ALT_MENU){
        Effect = NORMAL;
        
        if(Params.Common.Flags.bits.Man_Auto)
            strncpypgm2ram((char*)&TmpMsg, "Ручное наведение", sizeof(TmpMsg));
        else
            strncpypgm2ram((char*)&TmpMsg, "Авто наведение", sizeof(TmpMsg));
        OutTextXY(2,54,(const char*)TmpMsg,ARIAL_L, Effect);                        
        Line(88,52,88,63,1);

#ifdef _WINDOWS_
        SYSTEMTIME systemTime;
        FILETIME fileTime;
        UINT64 qwtime = 0;
        GetSystemTime( &systemTime );    
        SystemTimeToFileTime( &systemTime, &fileTime );
        qwtime = ((UINT64)fileTime.dwHighDateTime) << 32;
        qwtime += (UINT64)fileTime.dwLowDateTime;
        TIME_IN_SECONDS = (DWORD)qwtime/10000000;
#endif
        if(dwTime != SNTPGetUTCSeconds()) {
            dwTime = SNTPGetUTCSeconds();      
            SecondsToTime(dwTime, &Date);
            Params.Common.Flags.bits.NeedToRedrawTime = true;
            TimeT[0]   = '0' + Date.Hour / 10;
            TimeT[1]   = '0' + Date.Hour % 10;
            TimeT[3]   = '0' + Date.Min  / 10;
            TimeT[4]   = '0' + Date.Min  % 10;
            SecondT[1] = '0' + Date.Sec  / 10;
            SecondT[2] = '0' + Date.Sec  % 10;
            if(TimeT[2] == ':'){
                TimeT[2] = ' ';
            } else {
                TimeT[2] = ':';
            }
        }
        XX = OutTextXY(90,54,(const char*)TimeT,ARIAL_B,NORMAL);
        OutTextXY(XX,56,(const char*)SecondT,ARIAL_L,NORMAL);
    }
    
    switch (MenuItems[*ItemId].Type) {
    //*************************************************************************************************************************
    case ITEM_FOLDER:
    //*************************************************************************************************************************
        // подсчет количества строк 
        for(i = 0; i < MenusLen; i++){
            if(Menus[i].Id == *ItemId) {
                wTmp = FindRAMItem(Menus[i].BrunchId);
                switch (MenuItems[Menus[i].BrunchId].Type ) {
                    case ITEM_FOLDER:
                    case ITEM_STRING:
                    case ITEM_IP_ADDRES:
                    case ITEM_ANGLE:
                    case ITEM_BUTTON:
                    case ITEM_COMBO:
                        if(*MenuItemsM[wTmp].Flags & PF_ENABLE){
                            if(!(*TmpFlags & PF_CAN_SELECTED)){
                                if(PosY < MaxY) PosY++ ;
                            }
                            MaxY++;
                        }
                    break;
                    default:
                        break;
                }
            }
        }
        if(!(*TmpFlags & PF_HIDE_SCROLL))
            DrawScrollBar(PosY, MaxY);
        Selected = ProcessKeys(KeyPressed, &PosX, MaxX, &PosY, MaxY, &SelPosX, &SelPosY);
        if(Selected == ESC){
            if(MStackHeap != 0) {
                MStackHeap--;
                *ItemId = MenuStack[MStackHeap].LastId;
                PosY = MenuStack[MStackHeap].LastPosY;
            } else {
                MenuStack[MStackHeap].LastId = 0;
                MenuStack[MStackHeap].LastPosY = 0;
                MStackHeap++;
                *ItemId = 1;
                DisplayClear();
                return;
            }
            DisplayClear();
        }
         for(i = 0; i < MenusLen; i++){
            if(Menus[i].Id == *ItemId) {
                switch(MenuItems[Menus[i].BrunchId].Type){
                case ITEM_FOLDER:
                    TmpString = NULL;
                    TmpFlags = &DefaultFlags;
                    for(k = 0; k < RamMenusLen; k++){
                        if(MenuItemsM[k].Id == Menus[i].BrunchId) {
                            TmpFlags = MenuItemsM[k].Flags;
                        }
                    }
                    if(*TmpFlags & PF_ENABLE){
                        if(*TmpFlags & PF_CAN_SELECTED){
                            if(!(*TmpFlags & PF_IS_HIDE))
                                DrawMenuLine(j, MenuItems[Menus[i].BrunchId].Name, NULL, PosX, PosY, SELECT_LINE|FONT_TYPE_B);
                            if((Selected == ENTER)&&(SelPosY == j)){                        
                                MenuStack[MStackHeap].LastId = *ItemId;
                                MenuStack[MStackHeap].LastPosY = SelPosY;
                                MStackHeap++;
                                *ItemId = Menus[i].BrunchId;
                                DisplayClear();
                                return;
                            }
                        } else {
                            if(!(*TmpFlags & PF_IS_HIDE))
                                DrawMenuLine(j, MenuItems[Menus[i].BrunchId].Name, NULL, PosX, PosY, NO_SELECT|FONT_TYPE_B);
                        }
                    }
                    
                    break;
                case ITEM_HEADER:
                    TmpString = NULL;                    
                    TmpFlags = &DefaultFlags;
                    for(k = 0; k < RamMenusLen; k++){
                        if(MenuItemsM[k].Id == Menus[i].BrunchId) {
                            TmpFlags = MenuItemsM[k].Flags;
                            TmpFlag = *(BOOL*)MenuItemsM[k].Value;
                            if(TmpFlag){
                                color = 0;
                                Effect = NORMAL;
                            } else {
                                color = 1;
                                Effect = INVERT;
                            }
                            if(*TmpFlags & PF_ENABLE){
                                PX2 = OutTextXY(PX + 2,1,(const char*)MenuItems[Menus[i].BrunchId].Name,ARIAL_L, Effect);
                                FloodRectangle(PX, 0, PX2 + 2 ,8, color);
                                OutTextXY(PX + 2,1,(const char*)MenuItems[Menus[i].BrunchId].Name,ARIAL_L, Effect);
                                PX = PX2 + 2;
                            }
                        }
                    }
                    break;
                case ITEM_STRING:
                case ITEM_IP_ADDRES:
                case ITEM_ANGLE:
                case ITEM_BUTTON:
                case ITEM_COMBO:
                    TmpString = NULL;
                    TmpFlags = &DefaultFlags;
                    for(k = 0; k < RamMenusLen; k++){
                        if(MenuItemsM[k].Id == Menus[i].BrunchId) {
                            TmpFlags = MenuItemsM[k].Flags;
                            switch(MenuItemsM[k].ValType){
                            case VAL_STRING:
                                strcpy((char*)&TmpMsg, (char*)MenuItemsM[k].Value);
                                TmpString = (char*)&TmpMsg;
                                break;
                            case VAL_IP_ADDRES: 
                                TmpDW = (DWORD*)MenuItemsM[k].Value;
                                IPtoText(*TmpDW, (char*)TmpMsg, 0);
                                TmpString = (char*)&TmpMsg;
                                break;
                            case VAL_ANGLE: 
                                TmpFloat = (float*)MenuItemsM[k].Value;                                
                                XtoTimeString((char*)&TmpMsg, *TmpFloat, 0 ); 
                                TmpString = (char*)&TmpMsg;
                                break;
                            case VAL_ANGLE_HOUR:
                                TmpFloat = (float*)MenuItemsM[k].Value;                                
                                XtoTimeString((char*)&TmpMsg, *TmpFloat, 1 ); 
                                TmpString = (char*)&TmpMsg;
                                break;
                            default:
                                break;
                            }
                            break;
                        }
                    }
                    if(*TmpFlags & PF_ENABLE){
                        if(*TmpFlags & PF_CAN_SELECTED){
                            if(!(*TmpFlags & PF_IS_HIDE))
                                DrawMenuLine(j, MenuItems[Menus[i].BrunchId].Name, (const char*)TmpString, PosX, PosY, SELECT_LINE|FONT_TYPE_B);
                            if((Selected == ENTER)&&(SelPosY == j)){                        
                                MenuStack[MStackHeap].LastId = *ItemId;
                                MenuStack[MStackHeap].LastPosY = SelPosY;
                                MStackHeap++;
                                *ItemId = Menus[i].BrunchId;
                                DisplayClear();
                                return;
                            }
                        } else {
                            if(!(*TmpFlags & PF_IS_HIDE))
                                DrawMenuLine(j, MenuItems[Menus[i].BrunchId].Name, (const char*)TmpString, PosX, PosY, NO_SELECT|FONT_TYPE_B);                    
                        }
                        
                    }
                    break;
                }
                if(*TmpFlags & PF_ENABLE)
                    j++;
            }
        }
        break;
    //*************************************************************************************************************************
    case ITEM_ANGLE:
    case ITEM_IP_ADDRES:
    case ITEM_STRING:
    //*************************************************************************************************************************
        if(!BeginEdit){
            for(k = 0; k < RamMenusLen; k++){
                if(MenuItemsM[k].Id == *ItemId) {
                    BeginEdit = TRUE;
                    TmpIndex = k;
                    TmpValType = MenuItemsM[k].ValType;
                    switch(MenuItemsM[k].ValType){
                    case VAL_STRING:
                        strcpy((char*)&TmpEditMsg, (char*)MenuItemsM[k].Value);
                        break;
                    case VAL_IP_ADDRES: 
                        TmpDW = (DWORD*)MenuItemsM[k].Value;
                        IPtoText(*TmpDW, (char*)TmpEditMsg, 1);
                        break;
                    case VAL_ANGLE:  
                        // +90`59'59.99"
                        TmpFloat = (float*)MenuItemsM[k].Value;                                
                        XtoTimeString((char*)&TmpEditMsg, *TmpFloat, 0 ); 
                        break;
                    case VAL_ANGLE_HOUR:
                        // +12`59'59.99"
                        TmpFloat = (float*)MenuItemsM[k].Value;                                
                        XtoTimeString((char*)&TmpEditMsg, *TmpFloat, 1 ); 
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }
        }
        Selected = ModifyString(KeyPressed, &PosX, TmpEditMsg, TmpValType);
        DrawMenuLine(j,"", (const char*)TmpEditMsg, PosX, PosY, SELECT_COLUMN|FONT_TYPE_B);
        if(Selected == ESC){
            if(MStackHeap != 0) {
                MStackHeap--;
                *ItemId = MenuStack[MStackHeap].LastId;
                PosY = MenuStack[MStackHeap].LastPosY;
            }
            BeginEdit = FALSE;
            DisplayClear();
        }        
        if(Selected == ENTER){
            switch(TmpValType){
            case VAL_STRING:
                strcpy((char*)MenuItemsM[TmpIndex].Value, (char*)&TmpEditMsg);
                break;
            case VAL_ANGLE_HOUR:
                TextToTimeD(TmpEditMsg, TRUE, (float*)MenuItemsM[TmpIndex].Value);
                break;
            case VAL_ANGLE:
                TextToTimeD(TmpEditMsg, FALSE, (float*)MenuItemsM[TmpIndex].Value);
                break;
            case VAL_IP_ADDRES:
                TextToIP(TmpEditMsg, (DWORD*)MenuItemsM[TmpIndex].Value);
                break;
            }
            if(MStackHeap != 0) {
                MStackHeap--;
                *ItemId = MenuStack[MStackHeap].LastId;
                PosY = MenuStack[MStackHeap].LastPosY;
            }
            BeginEdit = FALSE;
            DisplayClear();
        }
        
        break;
    //*************************************************************************************************************************
    case ITEM_COMBO:
    //*************************************************************************************************************************
        // подсчет количества строк 
        for(i = 0; i < MenusLen; i++){
            if(Menus[i].Id == *ItemId) {
                wTmp = FindRAMItem(Menus[i].BrunchId);
                switch (MenuItems[Menus[i].BrunchId].Type ) {                
                case ITEM_VALUE:
                    if(*MenuItemsM[wTmp].Flags & PF_ENABLE){
                        if(!(*TmpFlags & PF_CAN_SELECTED)){
                            if(PosY < MaxY) PosY++ ;
                        }
                        MaxY++;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        if(!(*TmpFlags & PF_HIDE_SCROLL))
            DrawScrollBar(PosY, MaxY);
        Selected = ProcessKeys(KeyPressed, &PosX, MaxX, &PosY, MaxY, &SelPosX, &SelPosY);
        if(Selected == ESC){
            if(MStackHeap != 0) {
                MStackHeap--;
                *ItemId = MenuStack[MStackHeap].LastId;
                PosY = MenuStack[MStackHeap].LastPosY;
            } else {
                MenuStack[MStackHeap].LastId = 0;
                MenuStack[MStackHeap].LastPosY = 0;
                MStackHeap++;
                *ItemId = 1;
                DisplayClear();
                return;
            }
            DisplayClear();
        }
        for(i = 0; i < MenusLen; i++){
            if(Menus[i].Id == *ItemId) {
                switch(MenuItems[Menus[i].BrunchId].Type){                
                case ITEM_STRING:                
                case ITEM_VALUE:
                    TmpString = NULL;
                    TmpFlags = &DefaultFlags;
                    for(k = 0; k < RamMenusLen; k++){
                        if(MenuItemsM[k].Id == Menus[i].BrunchId) {
                            TmpFlags = MenuItemsM[k].Flags;
                            switch(MenuItemsM[k].ValType){
                            case VAL_STRING:
                                strcpy((char*)&TmpMsg, (char*)MenuItemsM[k].Value);
                                TmpString = (char*)&TmpMsg;
                                break;
                            default:
                                break;
                            }
                            break;
                        }
                    }
                    if(*TmpFlags & PF_ENABLE){
                        if(*TmpFlags & PF_CAN_SELECTED){
                            if(!(*TmpFlags & PF_IS_HIDE))
                                DrawMenuLine(j, MenuItems[Menus[i].BrunchId].Name, (const char*)TmpString, PosX, PosY, SELECT_LINE|FONT_TYPE_B);
                            if((Selected == ENTER)&&(SelPosY == j)){
                                wTmp = FindRAMItem(*ItemId);
                                // TODO: нужна функция распознавания строки в угол
								
                                //TODO: из этого MenuItems[Menus[i].BrunchId].Name сделать скорость
								// и записать сюда:
                                //TmpFloat = (float*)MenuItemsM[k].Value;                                


                                if(MStackHeap != 0) {
                                    MStackHeap--;
                                    *ItemId = MenuStack[MStackHeap].LastId;
                                    PosY = MenuStack[MStackHeap].LastPosY;
                                }
                                
                                DisplayClear();
                                return;
                            }
                        } else {
                            if(!(*TmpFlags & PF_IS_HIDE))
                                DrawMenuLine(j, MenuItems[Menus[i].BrunchId].Name, (const char*)TmpString, PosX, PosY, NO_SELECT|FONT_TYPE_B);                    
                        }

                    }
                    break;
                }
                if(*TmpFlags & PF_ENABLE)
                    j++;
            }
        }
        break; 
    //*************************************************************************************************************************        
    default:
    //*************************************************************************************************************************
        Selected = ProcessKeys(KeyPressed, &PosX, MaxX, &PosY, MaxY, &SelPosX, &SelPosY);
        if(Selected == ESC){
            if(MStackHeap != 0) {
                MStackHeap--;
                *ItemId = MenuStack[MStackHeap].LastId;
                PosY = MenuStack[MStackHeap].LastPosY;
            }
            BeginEdit = FALSE;
            DisplayClear();
        }
        break;
    }
    Params.Common.Flags.bits.NeedToRedrawMenus = 0; 
}
// 012345678901234
// +##`##'##.##"
// ###.###.###.###
// VAL_BYTE, VAL_WORD, VAL_DWORD, VAL_FLOAT, VAL_STRING, VAL_IP_ADDRES, VAL_ANGLE, VAL_ANGLE_HOUR
BYTE ModifyString(KEYS_STR * KeyPressed, BYTE* XPos, char * Value, VAL_TYPE ValType)
{   
    BYTE XMax = 0;
    BYTE YMax = 0;
    int XPosMod = 0;
    char BaseSymbol = '0';
    // подсчет количества столбцов
    XMax = strlen(Value);
    if(XMax == 0) 
        return 0;
    if(*XPos > XMax) 
        *XPos = XMax;

    if(KeyPressed->keys.left) { //LEFT
        if((*XPos) > 0) (*XPos)--;
        if((*XPos) > 0) XPosMod = -1;
        KeyPressed->keys.left = 0;        
    }
    if(KeyPressed->keys.right) { //RIGHT
        if(XMax > 0){
            if((*XPos) < XMax-1) (*XPos)++;
            if((*XPos) < XMax-1)  XPosMod = 1;
        }
        KeyPressed->keys.right = 0;        
    }    
    switch(ValType){
    case VAL_STRING:
        YMax = 160;
        BaseSymbol = ' ';
        break;
    case VAL_IP_ADDRES:
        BaseSymbol = '0';
        switch(*XPos){
        case 0: case 4: case 8: case 12:  // 0,1,2
            YMax = 3;
            break;    
        case 1: case 2:             // 0,1,2,3,4,5,6,7,8,9 (if Value[0] <2 0) 0,1,2,3,4,5 (if Value[0] == 2)
            if(Value[0] < '2')     
                    YMax = 10; 
            if(Value[0] == '2')
                YMax = 6; 
            break;    
        case 5: case 6:
            if(Value[4] < '2')     
                YMax = 10; 
            if(Value[4] == '2')
                YMax = 6; 
            break;    
        case 9: case 10:
            if(Value[8] < '2')     
                YMax = 10; 
            if(Value[8] == '2')
                YMax = 6; 
            break; 
        case 13: case 14:
            if(Value[12] < '2')     
                YMax = 10; 
            if(Value[12] == '2')
                YMax = 6; 
            break; 
        case 3:case 7:case 11:       // . 
            *XPos += XPosMod;
            YMax = 0;
            break;        
        default:
            YMax = 0;
        }      
        break;
    case VAL_ANGLE_HOUR:
        BaseSymbol = '0';
        switch(*XPos){
        case 0:                     // +, -
            YMax = 2; 
            BaseSymbol = '+';
            break;    
        case 1: YMax = 2; break;    // 0,1
        case 2:                     // 0,1,2,3,4,5,6,7,8,9 (if Value[1] == 0) 0,1,2 (if Value[1] == 1)
            if(Value[1] == '0')
                YMax = 10; 
            if(Value[1] == '1')
                YMax = 3; 
            break; 
        case 3:case 6:case 9:       // `, ', . ,"
            *XPos += XPosMod;
            YMax = 0;
            break;        
        case 4:case 7:              // 0,1,2,3,4,5,6
            YMax = 6; break;
        case 5: case 8: case 10: case 11:
            YMax = 10; break;
        case 12:
            *XPos -= 1;
        default:
            YMax = 0;
        }      
        break;
    case VAL_ANGLE:
        BaseSymbol = '0';
        switch(*XPos){
        case 0:                     // +, -
            YMax = 2; 
            break;    
        case 1:case 2:              // 0,1,2,3,4,5,6,7,8,9
        case 5: case 8: case 10: case 11:
            YMax = 10; break;
        case 3:case 6:case 9:
            *XPos += XPosMod;
            YMax = 0; 
            break;                  // `, ', . ,"
        case 4:case 7:              // 0,1,2,3,4,5,6
            YMax = 6; break;
        case 12:
            *XPos -= 1;
        default:
            YMax = 0;
        }      
        break;
    default:
        break;
    }
    if(KeyPressed->keys.up) { //UP
        switch(ValType){
        case VAL_ANGLE_HOUR:
        case VAL_ANGLE:
            if(*XPos == 0)
                if(Value[*XPos] == '+')
                    Value[*XPos] = '-';
                else
                    Value[*XPos] = '+';
            else {
                if(Value[*XPos] < BaseSymbol + YMax -1) 
                    Value[*XPos]++;
                else
                    Value[*XPos] = BaseSymbol;
            }
            break;
        default:
            if(Value[*XPos] < BaseSymbol + YMax -1) 
                Value[*XPos]++;
            else
                Value[*XPos] = BaseSymbol;
        }
        KeyPressed->keys.up = 0;      
    }
    if(KeyPressed->keys.down) { //DOWN
        switch(ValType){
        case VAL_ANGLE_HOUR:
        case VAL_ANGLE:
            if(*XPos == 0)
                if(Value[*XPos] == '+')
                    Value[*XPos] = '-';
                else
                    Value[*XPos] = '+';
            else {
                if(Value[*XPos] > BaseSymbol) 
                    Value[*XPos]--;
                else
                    Value[*XPos] = BaseSymbol + YMax -1;
            }
            break;
        default:
            if(Value[*XPos] > BaseSymbol) 
                Value[*XPos]--;
            else
                Value[*XPos] = BaseSymbol + YMax;
        }
        KeyPressed->keys.down = 0;       
    }
    
    if(KeyPressed->keys.esc) { //ESC
        (*XPos) = 0;
        KeyPressed->keys.esc = 0;
        Params.Common.Flags.bits.NeedToRedrawMenus = true;
        return ESC;
    }
    if(KeyPressed->keys.enter) { //Enter 
        (*XPos) = 0;
        KeyPressed->keys.enter = 0;
        Params.Common.Flags.bits.NeedToRedrawMenus = true;
        return ENTER;
    }
    return 0;
}