#ifndef _CONTROL_H
#define _CONTROL_H

#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "GenericTypeDefs.h"
#endif
// флаги для NeedToUpdate и NeedToCommit

typedef struct PARAMS_FLAGS {
    BYTE NeedToUpdate:1;
    BYTE NeedToCommit:1;
    BYTE IsModified:1;
    BYTE Enable;
    BYTE b5:4;
}PARAMS_FLAGS;
 
typedef union {
    BYTE Val;
    struct __PACKED
    {        
        BYTE Enable:1;
        BYTE Run:1;
        BYTE b2:1;
        BYTE b3:1;
        BYTE b4:1;
        BYTE b5:1;
        BYTE b6:1;
        BYTE b7:1;
    } bits;
} AXIS_STATUS_FLAG_STRUCT; 
typedef struct AXIS_PARAM
{   
    PARAMS_FLAGS AngleFlag;
    PARAMS_FLAGS SpeedFlag;
    PARAMS_FLAGS TargetAngleFlag;
    PARAMS_FLAGS TargetSpeedFlag;
    float Angle;           // текущая координата в радианах (угол относительно весеннего равноденствия) то есть положение звезды   
    BYTE  Speed;           // номер позиции скорости
    float TargetAngle;           // текущая координата в радианах (угол относительно весеннего равноденствия) то есть положение звезды    
    float TargetSpeed;           // текущая координата в радианах (угол относительно весеннего равноденствия) то есть положение звезды    
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
