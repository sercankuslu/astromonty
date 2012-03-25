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
// ��������� ���������
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
    "�:", "�:", "�:", "����", "����", "���� ���������", "������ ���������", 
    "�����", "������", "�����",       
    "���������",  "����������", 
    "�������", "������ �����", "����� ���������",
    "�����", "����������", 
    "����������", "�����", 
    "��� ����������",
    "AS-CONTROL", "���:", "IP:", "Mask:", "Gate:", "DNS1:", "DNS2:", "NTP:",
    "������",
    // TODO: ������� ����������� ��������������� ��������� ������ 
    // TODO: ������� ��������� ������� ������
    "��������� ����������", "� ������ ������ ���-", "���� ��������� ���", "���� ���������",
    "��� ����������� �",    "�������. �������� ��", "��������" , ""
};*/

typedef enum VAL_TYPE {
   VAL_BYTE, VAL_WORD, VAL_DWORD, VAL_FLOAT, VAL_STRING, VAL_IP_ADDRES, VAL_ANGLE, VAL_ANGLE_HOUR
}VAL_TYPE;
typedef enum ITEM_TYPE {
    ITEM_FOLDER, ITEM_IP_ADDRES, ITEM_ANGLE, ITEM_MSG, ITEM_BUTTON, ITEM_STRING
}ITEM_TYPE;
// ��� ����������� �������� ����
typedef struct MENU_ITEMS_ROM {
   ITEM_TYPE Type;
   rom char * Name;
} MENU_ITEMS_ROM;
// ��� ����������� ���������� �������� ����
typedef struct MENU_ITEMS_RAM {
   BYTE Id;
   VAL_TYPE ValType;
   void * Value;        
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
// ������ ���� ��������� ���� 
const rom MENU_ITEMS_ROM MenuItems[] = {
   {ITEM_FOLDER,    "����"},                       // 0
   {ITEM_ANGLE,     "�:"},                         // 1
   {ITEM_ANGLE,     "�:"},                         // 2
   {ITEM_ANGLE,     "�:"},                         // 3
   {ITEM_FOLDER,    "����"},                       // 4
   {ITEM_MSG,       "���� ���������"},             // 5
   {ITEM_MSG,       "������ ���������"},           // 6
   {ITEM_FOLDER,    "�����"},                      // 7
   {ITEM_FOLDER,    "������"},                     // 8
   {ITEM_FOLDER,    "�����"},                      // 9
   {ITEM_FOLDER,    "���������"},                  // 10
   {ITEM_FOLDER,    "����������"},                 // 11
   {ITEM_FOLDER,    "�������"},                    // 12
   {ITEM_FOLDER,    "������ �����"},               // 13
   {ITEM_FOLDER,    "����� ���������"},            // 14
   {ITEM_BUTTON,    "�����"},                      // 15
   {ITEM_BUTTON,    "����������"},                 // 16
   {ITEM_FOLDER,    "����������"},                 // 17
   {ITEM_FOLDER,    "�����"},                      // 18
   {ITEM_FOLDER,    "��� ����������"},             // 19
   {ITEM_STRING,    "���:"},                       // 20
   {ITEM_IP_ADDRES, "IP:"},                        // 21
   {ITEM_IP_ADDRES, "Mask:"},                      // 22
   {ITEM_IP_ADDRES, "Gate:"},                      // 23
   {ITEM_IP_ADDRES, "DNS1:"},                      // 24
   {ITEM_IP_ADDRES, "DNS2:"},                      // 25
   {ITEM_IP_ADDRES, "NTP:"},                       // 26
   {ITEM_MSG, "������"},                           // 27
   {ITEM_MSG, "��������� ���������� � ������ ������ ������� ��������� ������� ���������"},      // 28
   {ITEM_MSG, "��� ����������� � �������. �������� �� ��������"},                               // 29
};                                                                                              
static MENU_ITEMS_RAM MenuItemsM[] = {
    {1, VAL_ANGLE_HOUR, (void*)&Params.Alpha.Angle},
    {2, VAL_ANGLE, (void*)&Params.Delta.Angle},
    {3, VAL_ANGLE, (void*)&Params.Gamma.Angle},
    {20, VAL_STRING, (void*)&Params.Local.Name},
    {21, VAL_IP_ADDRES, (void*)&Params.Local.IP},
    {22, VAL_IP_ADDRES, (void*)&Params.Local.Mask},
    {23, VAL_IP_ADDRES, (void*)&Params.Local.Gate},
    {24, VAL_IP_ADDRES, (void*)&Params.Local.DNS1},
    {25, VAL_IP_ADDRES, (void*)&Params.Local.DNS2},
    {26, VAL_IP_ADDRES, (void*)&Params.Local.NTP},
};
// ������ �����
const rom MENUS Menus[] = {
   {0, 11},     //����
   {0, 10},
   {10, 4},     //���������
   {10, 17},
   {10, 18},
   {10, 19},
   {10, 11},
   {4, 20},     //����
   {4, 21},
   {4, 22},
   {4, 23},
   {4, 24},
   {4, 25},
   {4, 26},
   {11, 12},    //����������
   {11, 13},
   {11, 14},
   {12, 1},    //�������
   {12, 2},
   {12, 3},
};






C_ROM char * MSG_NOMSG                      = "";                    
C_ROM char * MSG_MW_ALPHA                   = "�:";                  
C_ROM char * MSG_MW_DELTA                   = "�:";                  
C_ROM char * MSG_MW_GAMMA                   = "�:";                  
C_ROM char * MSG_MW_MENU                    = "����";                
C_ROM char * MSG_C_NET                      = "����";                
C_ROM char * MSG_MW_MODE_AUTO               = "���� ���������";      
C_ROM char * MSG_MW_MODE_MANUAL             = "������ ���������";    
C_ROM char * MSG_C_ALPHA                    = "�����";               
C_ROM char * MSG_C_DELTA                    = "������";              
C_ROM char * MSG_C_GAMMA                    = "�����";               
C_ROM char * MSG_M_SETTINGS                 = "���������";           
C_ROM char * MSG_M_S_OBSERV                 = "����������";          
C_ROM char * MSG_MO_GOTO                    = "�������";             
C_ROM char * MSG_MO_MANUAL                  = "������ �����";        
C_ROM char * MSG_MO_SPACECRAFT              = "����� ���������";     
C_ROM char * MSG_C_START                    = "�����";               
C_ROM char * MSG_C_CONTINUE                 = "����������";          
C_ROM char * MSG_S_MONTY                    = "����������";          
C_ROM char * MSG_S_DISPLAY                  = "�����";               
C_ROM char * MSG_SM_TYPE                    = "��� ����������";      
C_ROM char * MSG_SNL_NAME                   = "AS-CONTROL";          
C_ROM char * MSG_SN_NAME                    = "���:";                
C_ROM char * MSG_SN_IP                      = "IP:";                 
C_ROM char * MSG_SN_MASK                    = "Mask:";               
C_ROM char * MSG_SN_GATE                    = "Gate:";               
C_ROM char * MSG_SN_DNS1                    = "DNS1:";               
C_ROM char * MSG_SN_DNS2                    = "DNS2:";               
C_ROM char * MSG_SN_NTP                     = "NTP:";                
C_ROM char * MSG_C_ERROR                    = "������";              
C_ROM char * MSG_ERR_NOTREACHABLE0          = "��������� ����������";
C_ROM char * MSG_ERR_NOTREACHABLE1          = "� ������ ������ ���-";
C_ROM char * MSG_ERR_NOTREACHABLE2          = "���� ��������� ���";  
C_ROM char * MSG_ERR_NOTREACHABLE3          = "���� ���������";      
C_ROM char * MSG_ERR_NOCONNECTION0          = "��� ����������� �";   
C_ROM char * MSG_ERR_NOCONNECTION1          = "�������. �������� ��";
C_ROM char * MSG_ERR_NOCONNECTION2          = "��������";            
C_ROM char * MSG_ERR_NOCONNECTION3          = "";                    

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
BYTE ProcessKeys(KEYS_STR * KeyPressed, BYTE * PosX, BYTE MaxX, BYTE* PosY, BYTE MaxY, MENU_ID LastState, MENU_ID * State, BYTE * SelPosX, BYTE * SelPosY);
void IPtoText (DWORD IP, char * Text, BOOL ForEdit);
int SubStrToInt(const char* Text, int Beg, int * Val);
void TextToTimeD(char* TmpValue, BOOL TmpIsHours, float * TmpDoValue);
void NewProcessMenu(BYTE * ItemId, KEYS_STR * KeyPressed);
//void GetMsgFromROM(MSGS Msg_id, char* Msg);

void ProcessMenu( KEYS_STR * KeyPressed )
{
    static float Xa = 0.0;//91.3 * PI / 180.0;
    static float Xd = (float)(-33.1 * PI / 180.0);
    static float Xg = (float)(47.2 * PI / 180.0);
    static char TimeT[] = "23:56";    
    static char SecondT[] = ".00";
    

    static MENU_ID State = MAIN_WINDOW;
    static MENU_ID LastState = MAIN_WINDOW;

    // ��������: true- ����� ���, false-������ ���
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
    static BOOL Init = false;
    static int TimerCount = 0;
    static DWORD dwTime;
    static DateTime Date;
    static BYTE Id = 0;
    
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


    if(!Init){
        AppConfig.MyIPAddr.Val = 0x3701A8C0;
        AppConfig.MyMask.Val = 0x00FFFFFF;
        AppConfig.MyGateway.Val = 0x0101A8C0;
        AppConfig.PrimaryDNSServer.Val = 0x0101A8C0;
        Params.Local.IP = AppConfig.MyIPAddr.Val;
        Params.Local.Mask = AppConfig.MyMask.Val;
        Params.Local.Gate = AppConfig.MyGateway.Val;
        Params.Local.DNS1 = AppConfig.PrimaryDNSServer.Val;
        Params.Local.DNS2 = AppConfig.SecondaryDNSServer.Val;
        Params.Alpha.Angle = 0.0f;        
        Params.Delta.Angle = 0.0f; 
        Params.Gamma.Angle = 0.0f;
        Params.Alpha.TargetAngle = 0.0f;
        Params.Delta.TargetAngle = 0.0f;
        Params.Gamma.TargetAngle = 0.0f;
        Params.Alpha.StatusFlag.bits.Enable = 1;
        Params.Delta.StatusFlag.bits.Enable = 1;
        Params.Gamma.StatusFlag.bits.Enable = 0;
        //Params.Alpha.StatusFlag = 0;//|= AXIS_RUN;
        //Params.Delta.StatusFlag = 0;//|= AXIS_RUN;
        //Params.Gamma.StatusFlag = 0;// |= AXIS_RUN;
        Params.Local.ConnectFlag = 0;
        Params.NeedToUpdate.Val = 0;
        Params.Alpha.NeedToUpdate.Val = 0;
        strncpypgm2ram((char*)Params.Local.Name, MSG_SNL_NAME, sizeof(Params.Local.Name));
        Params.Alpha.IsModified.Val = 0x00;
        Params.Delta.IsModified.Val = 0x00;
        Params.Gamma.IsModified.Val = 0x00;
        Params.NeedToCommit.Val = 0x00;
        memset(TmpValue,0,sizeof(TmpValue));
        Params.Common.Flags.bits.NeedToRedrawMenus = true;

        Init = true;            
//         Params.Alpha.TargetAngle = 0.24f;
//         Params.NeedToCommit.bits.Alpha = 1;
//         Params.Alpha.NeedToCommit.bits.TargetAngle = 1;
    }
    //Params.Alpha.Angle += (2.0 * PI /(360.0 * 200.0 * 16.0))*13.333333333334/5.0;
    
    NewProcessMenu(&Id, KeyPressed);
    EndProcess = true;
    if(0)
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
                if(Params.Local.ConnectFlag){
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
                if(Params.Local.ConnectFlag){
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
                    if(Params.Local.ConnectFlag){
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
                    // �������� ������� �� ������� ��������� �� ����������
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
                    // ���-�� ���� ���������� ������ � ���� ���� �� ������
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
    if(Msg_id != MSG_NOMSG){
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

BYTE ProcessKeys(KEYS_STR * KeyPressed, BYTE* XPos, BYTE XMax, BYTE * YPos, BYTE YMax, MENU_ID LastState, MENU_ID * State, BYTE * XSelPos, BYTE * YSelPos)
{
    if(KeyPressed->keys.esc) { //ESC
        (*State) = LastState;
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
    Xg = X * 180/(PI); // � ����� �������
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
        } else sprintf (Text, "+%0.2d`%0.2d'%0.2d.%0.2d\"\0", Grad,Min,Sec,mSec);
    } else {
        if(hour){
            /*Grad /= 15;*/
            sprintf (Text, "-%0.2dh%0.2d'%0.2d.%0.2d\"\0", Grad,Min,Sec,mSec);
        } else sprintf (Text, "-%0.2d`%0.2d'%0.2d.%0.2d\"\0", Grad,Min,Sec,mSec);
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
// ���������� ����� � �������� ������� �� ������ �������, �� ����������� ������ int
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
            case '`':
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
    BYTE d = 1;//���� ������
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
            DY = 366; //366 ����           
        } else {
            DY = 365; //365 ����
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
    static MENU_ID State = SETTINGS;  // �� �����?
    BYTE Selected = 0;
    static int MenusLen = 0;
    static int RamMenusLen = 0;
    char * TmpString = NULL;
    DWORD * TmpDW = NULL;
    float * TmpFloat = NULL;
    char TmpMsg[20] = "";

    if(!Params.Common.Flags.bits.NeedToRedrawMenus && !KeyPressed->Val) 
        return;
    if(!MenusLen){
        MenusLen = sizeof(Menus)/sizeof(Menus[0]);
    }
    if(!RamMenusLen){
        RamMenusLen = sizeof(MenuItemsM)/sizeof(MenuItemsM[0]);
    }
    
     
    // ���������
    DrawRectangle(0,0,132,10,1);
    strncpypgm2ram((char*)&TmpMsg, MenuItems[*ItemId].Name, strlen(MenuItems[*ItemId].Name));
    OutTextXY(15,2,(const char*)TmpMsg,ARIAL_L,NORMAL);
   
    
    switch (MenuItems[*ItemId].Type) {
    case ITEM_FOLDER:
        // ������� ���������� ����� 
        for(i = 0; i < MenusLen; i++){
            if(Menus[i].Id == *ItemId) {
                MaxY++;
            }
        }
        Selected = ProcessKeys(KeyPressed, &PosX, MaxX, &PosY, MaxY, MAIN_WINDOW, &State, &SelPosX, &SelPosY);
        if(Selected == ESC){
            if(MStackHeap != 0) {
                MStackHeap--;
                *ItemId = MenuStack[MStackHeap].LastId;
                PosY = MenuStack[MStackHeap].LastPosY;
            }
            DisplayClear();
        }
        DrawScrollBar(PosY, MaxY);
        for(i = 0; i < MenusLen; i++){
            if(Menus[i].Id == *ItemId) {
                switch(MenuItems[Menus[i].BrunchId].Type){
                case ITEM_FOLDER:
                    DrawMenuLine(j, MenuItems[Menus[i].BrunchId].Name, NULL, PosX, PosY, SELECT_LINE|FONT_TYPE_B);
                    if((Selected == ENTER)&&(SelPosY == j)){                        
                        MenuStack[MStackHeap].LastId = *ItemId;
                        MenuStack[MStackHeap].LastPosY = SelPosY;
                        MStackHeap++;
                        *ItemId = Menus[i].BrunchId;
                        DisplayClear();
                        return;
                    }
                    break;
                case ITEM_STRING:
                case ITEM_IP_ADDRES:
                case ITEM_ANGLE:
                    TmpString = NULL;
                    for(k = 0; k < RamMenusLen; k++){
                        if(MenuItemsM[k].Id == Menus[i].BrunchId) {
                            switch(MenuItemsM[k].ValType){
                            case VAL_STRING:
                                TmpString = (char*)MenuItemsM[k].Value;
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
                    DrawMenuLine(j, MenuItems[Menus[i].BrunchId].Name, (const char*)TmpString, PosX, PosY, SELECT_LINE|FONT_TYPE_B);
                    if((Selected == ENTER)&&(SelPosY == j)){                        
                        MenuStack[MStackHeap].LastId = *ItemId;
                        MenuStack[MStackHeap].LastPosY = SelPosY;
                        MStackHeap++;
                        *ItemId = Menus[i].BrunchId;
                        DisplayClear();
                        return;
                    }
                    break;
                }
                j++;
            }
        }
        break;
    case ITEM_ANGLE:
        TmpString = NULL;
        for(k = 0; k < RamMenusLen; k++){
            if(MenuItemsM[k].Id == *ItemId) {
                switch(MenuItemsM[k].ValType){
                case VAL_STRING:
                    TmpString = (char*)MenuItemsM[k].Value;
                    break;
                case VAL_IP_ADDRES: 
                    // 255.255.255.255
                    TmpDW = (DWORD*)MenuItemsM[k].Value;
                    IPtoText(*TmpDW, (char*)TmpMsg, 0);
                    TmpString = (char*)&TmpMsg;
                    break;
                case VAL_ANGLE:  
                    // +90`59'59.99"
                    TmpFloat = (float*)MenuItemsM[k].Value;                                
                    XtoTimeString((char*)&TmpMsg, *TmpFloat, 0 ); 
                    TmpString = (char*)&TmpMsg;
                    break;
                case VAL_ANGLE_HOUR:
                    // +12`59'59.99"
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
        // ������� ���������� ��������
        if(MenuItems[*ItemId].Type == ITEM_ANGLE){
            MaxX = strlen(TmpString);
        }
        Selected = ProcessKeys(KeyPressed, &PosX, MaxX, &PosY, MaxY, MAIN_WINDOW, &State, &SelPosX, &SelPosY);
        if(Selected == ESC){
            if(MStackHeap != 0) {
                MStackHeap--;
                *ItemId = MenuStack[MStackHeap].LastId;
                PosY = MenuStack[MStackHeap].LastPosY;
            }
            DisplayClear();
        }
        DrawMenuLine(j, MenuItems[*ItemId].Name, (const char*)TmpString, PosX, PosY, SELECT_COLUMN|FONT_TYPE_B);
        if(Selected == ENTER){
            if(MStackHeap != 0) {
                MStackHeap--;
                *ItemId = MenuStack[MStackHeap].LastId;
                PosY = MenuStack[MStackHeap].LastPosY;
            }
            DisplayClear();
        }
        break;
    default:;
    }
    Params.Common.Flags.bits.NeedToRedrawMenus = 0; 
}

