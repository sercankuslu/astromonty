#ifndef _CONTROL_H
#define _CONTROL_H

#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "GenericTypeDefs.h"
#endif
// флаги для NeedToUpdate и NeedToCommit
#define PF_NEED_UPDATE      0x01
#define PF_NEED_COMMIT      0x02
#define PF_IS_MODIFIED      0x04
#define PF_ENABLE           0x08
#define PF_CAN_SELECTED     0x10
#define PF_HIDE_SCROLL      0x20
#define PF_IS_HIDE          0x40
#define PF_ALT_MENU         0x80

#define PF_UPDATED          0xFE 
#define PF_COMMITED         0xFD
#define PF_NO_MODIFY        0xFB
#define PF_DISABLE          0xF7
#define PF_CANT_SELECTED    0xEF
#define PF_SHOW_SCROLL      0xDF
#define PF_STD_HEADER       0xBF
#define PF_STD_FOOTER       0x7F

typedef BYTE PARAMS_FLAGS;
/*
typedef struct PARAMS_FLAGS {
    BYTE NeedToUpdate:1;    //0
    BYTE NeedToCommit:1;    //1
    BYTE IsModified:1;      //2
    BYTE Enable:1;          //3
    BYTE HideScroll:1;      //4
    BYTE AltHeader:1;       //5
    BYTE AltFooter:1;       //6
    BYTE CanSelected:1;         //7
}PARAMS_FLAGS;
 */
typedef struct AXIS_STATUS_FLAG_STRUCT {   
    BYTE Run:1;
    BYTE b2:7;  
} AXIS_STATUS_FLAG_STRUCT; 
typedef struct AXIS_PARAM
{   
    PARAMS_FLAGS AngleFlag;
    PARAMS_FLAGS SpeedFlag;
    PARAMS_FLAGS TargetAngleFlag;
    PARAMS_FLAGS TargetSpeedFlag;
    float Angle;           // текущая координата в радианах (угол относительно весеннего равноденствия) то есть положение звезды   
    float Speed;           // максимальная скорость наведения
    float TargetAngle;           // текущая координата в радианах (угол относительно весеннего равноденствия) то есть положение звезды    
    float TargetSpeed;           // скорость звезды
    AXIS_STATUS_FLAG_STRUCT  StatusFlag;       // флаг состояния оси
} AXIS_PARAM;
// флаги для NeedToUpdate и NeedToCommit

typedef struct NETWORK_SETTINGS
{
    PARAMS_FLAGS NetFlags;
    PARAMS_FLAGS StatusFlag;
    DWORD IP;
    DWORD Mask;
    DWORD Gate;
    DWORD DNS1;
    DWORD DNS2;    
    char  Name[16];    
    DWORD NTP;
    BYTE Status;
}NETWORK_SETTINGS;


typedef union COMMON_SETTINGS_FLAG_STRUCT
{
    BYTE Val;
    struct __PACKED
    {        
        BYTE Man_Auto:1;        // ручное = 1/ автоматическое = 0 наведение
        BYTE NeedToRedrawMenus:1;
        BYTE NeedToRedrawTime:1;
        BYTE b3:1;
        BYTE b4:1;
        BYTE b5:1;
        BYTE b6:1;
        BYTE b7:1;
    } bits;
} COMMON_SETTINGS_FLAG_STRUCT;
typedef struct COMMON_SETTINGS
{
    PARAMS_FLAGS MaxGuidanceSpeedFlag;
    COMMON_SETTINGS_FLAG_STRUCT Flags;
    double MaxGuidanceSpeed;              // максимальная скорость наведения в ручном/автоматическом режимах
    
} COMMON_SETTINGS;

typedef struct All_PARAMS
{
    PARAMS_FLAGS AlphaFlag;
    PARAMS_FLAGS DeltaFlag;
    PARAMS_FLAGS GammaFlag;
    PARAMS_FLAGS LocalFlag;
    PARAMS_FLAGS RemoteFlag;
    PARAMS_FLAGS CommonFlag;
    PARAMS_FLAGS MainMenuFlag;
    PARAMS_FLAGS MainMenu2Flag;

    AXIS_PARAM Alpha;
    AXIS_PARAM Delta;
    AXIS_PARAM Gamma;

    NETWORK_SETTINGS Local;
    NETWORK_SETTINGS Remote;

    COMMON_SETTINGS Common;

} ALL_PARAMS;

#define NO_SELECT 0x00
#define SELECT_LINE 0x01
#define SELECT_COLUMN 0x02
#define FONT_TYPE_B     0x04

typedef union KEYS_STR{
    BYTE Val;
    struct __PACKED
        {        
            BYTE up:1;
            BYTE down:1;
            BYTE left:1;
            BYTE right:1;
            BYTE esc:1;
            BYTE enter:1;
            BYTE b6:1;
            BYTE b7:1;
    } keys;
} KEYS_STR; 

#ifndef DATE_TIME_STRUCT
#define DATE_TIME_STRUCT
typedef struct DateTimeStruct {
    WORD Year;
    BYTE Month;
    BYTE Day;
    BYTE DayOfWeak;
    BYTE Hour;
    BYTE Min;
    BYTE Sec;
    double uSec;
} DateTime;
#endif
    
void ProcessMenu( KEYS_STR * KeyPressed);
void SecondsToTime(DWORD Seconds, DateTime * Date);
#endif
