#ifndef _CONTROL_H
#define _CONTROL_H

#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "GenericTypeDefs.h"
#endif
// флаги дл€ NeedToUpdate и NeedToCommit
#define C_ANGLE 0x01
#define C_STEPS 0x02
#define T_ANGLE 0x04
#define T_STEPS 0x08
#define FLAG  0x80

#define AXIS_ENABLE 0x80
#define AXIS_RUN    0x01

typedef struct AXIS_PARAM
{
    BYTE  NeedToUpdate;     // необходимо запросить с сервера указанные параметры
    BYTE  NeedToCommit;     // необходимо отправить на сервер указанные параметры

    double Angle;           // текуща€ координата в радианах (угол относительно весеннего равноденстви€) то есть положение звезды    
    DWORD  AbsSteps;         // текущий номер шага    
    double TargetAngle;           // текуща€ координата в радианах (угол относительно весеннего равноденстви€) то есть положение звезды    
    DWORD  TargetAbsSteps;         // текущий номер шага    
    BYTE   StatusFlag;       // флаг состо€ни€ оси
} AXIS_PARAM;
// флаги дл€ NeedToUpdate и NeedToCommit
#define NS_IP_ADDR 0x01
#define NS_IP_MASK 0x02
#define NS_IP_GATE 0x04
#define NS_IP_DNS1 0x08
#define NS_IP_DNS2 0x10
#define NS_IP_NAME 0x20
#define NS_IP_NTP  0x40
#define NS_IP_FLAG 0x80
typedef struct NETWORK_SETTINGS
{
    BYTE  NeedToUpdate;     // необходимо запросить с сервера указанные параметры
    BYTE  NeedToCommit;     // необходимо отправить на сервер указанные параметры
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

typedef enum MENU_ID {
    MAIN_WINDOW, MENU, SETTINGS, OBSERV, O_GOTO, S_OBSERV, S_NETWORK, S_MONTY,S_DISPLAY, SM_TYPESELECT, SM_ALPHA, SM_DELTA, SM_GAMMA,EDIT_IP,EDIT_TIME,EDIT_ANGLE, ERROR_COORDINATE
} MENU_ID;
#define NO_SELECT 0x00
#define SELECT_LINE 0x01
#define SELECT_COLUMN 0x02
#define FONT_TYPE_B     0x04

void ProcessMenu( BYTE * KeyPressed);
void ExecuteCommands( void );
#endif
