#ifndef _CONTROL_H
#define _CONTROL_H

#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "GenericTypeDefs.h"
#endif
// флаги дл€ NeedToUpdate и NeedToCommit


typedef union {
    BYTE Val;
    struct __PACKED
    {        
        BYTE Angle:1;
        BYTE AbsStep:1;
        BYTE TargetAngle:1;
        BYTE TargetAbsStep:1;
        BYTE Flag:1;
        BYTE b5:1;
        BYTE b6:1;
        BYTE b7:1;
    } bits;
} AXIS_FLAG_STRUCT; 
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
    AXIS_FLAG_STRUCT  NeedToUpdate;     // необходимо запросить с сервера указанные параметры
    AXIS_FLAG_STRUCT  NeedToCommit;     // необходимо отправить на сервер указанные параметры
    AXIS_FLAG_STRUCT  IsModified;

    double Angle;           // текуща€ координата в радианах (угол относительно весеннего равноденстви€) то есть положение звезды    
    DWORD  AbsSteps;         // текущий номер шага    
    double TargetAngle;           // текуща€ координата в радианах (угол относительно весеннего равноденстви€) то есть положение звезды    
    DWORD  TargetAbsSteps;         // текущий номер шага    
    AXIS_STATUS_FLAG_STRUCT  StatusFlag;       // флаг состо€ни€ оси
} AXIS_PARAM;
// флаги дл€ NeedToUpdate и NeedToCommit

typedef union {
    BYTE Val;
    struct __PACKED
    {        
        BYTE IP:1;
        BYTE Mask:1;
        BYTE Gate:1;
        BYTE DNS1:1;
        BYTE DNS2:1;
        BYTE Name:1;
        BYTE NTP:1;
        BYTE Flag:1;
    } bits;
} NET_FLAG_STRUCT;
typedef struct NETWORK_SETTINGS
{
    NET_FLAG_STRUCT  NeedToUpdate;     // необходимо запросить с сервера указанные параметры
    NET_FLAG_STRUCT  NeedToCommit;     // необходимо отправить на сервер указанные параметры
    NET_FLAG_STRUCT  IsModified;
    DWORD IP;
    DWORD Mask;
    DWORD Gate;
    DWORD DNS1;
    DWORD DNS2;    
    char  Name[16];    
    DWORD NTP;
    BYTE ConnectFlag;
}NETWORK_SETTINGS;
// флаги дл€ NeedToUpdate и NeedToCommit
#define ALPHA           0x01
#define DELTA           0x02
#define GAMMA           0x04
#define NET_LOCAL       0x08
#define NET_REMOTE      0x10

typedef struct All_PARAMS
{
    // битовые карты -флаги 
    BYTE NeedToUpdate;   // необходимо запросить с сервера указанные параметры
    BYTE NeedToCommit;   // необходимо отправить на сервер указанные параметры    
    
    AXIS_PARAM Alpha;
    AXIS_PARAM Delta;
    AXIS_PARAM Gamma;

    NETWORK_SETTINGS Local;
    NETWORK_SETTINGS Remote;

} ALL_PARAMS;

#define NO_SELECT 0x00
#define SELECT_LINE 0x01
#define SELECT_COLUMN 0x02
#define FONT_TYPE_B     0x04

typedef union {
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
void ExecuteCommands( void );
void SecondsToTime(DWORD Seconds, DateTime * Date);
#endif
