#ifndef _CONTROL_H
#define _CONTROL_H

#ifdef _WINDOWS
#   include "..\..\guidance\stdafx.h"
#else
#   include "GenericTypeDefs.h"
#endif


typedef struct AXIS_PARAM
{
    BYTE  NeedToUpdate;     // необходимо запросить с сервера указанные параметры
    BYTE  NeedToCommit;     // необходимо отправить на сервер указанные параметры

    double Angle;           // текущая координата в радианах (угол относительно весеннего равноденствия) то есть положение звезды    
    DWORD AbsSteps;         // текущий номер шага    

} AXIS_PARAM;

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
}NETWORK_SETTINGS;

typedef struct All_PARAMS
{
    // битовые карты -флаги 
    BYTE NeedToUpdate;   // необходимо запросить с сервера указанные параметры
    BYTE NeedToCommit;   // необходимо отправить на сервер указанные параметры

    AXIS_PARAM Alpha;
    AXIS_PARAM Delta;
    AXIS_PARAM Gamma;
    AXIS_PARAM TargetAlpha;
    AXIS_PARAM TargetDelta;
    AXIS_PARAM TargetGamma;

    NETWORK_SETTINGS Local;
    NETWORK_SETTINGS Remote;

} ALL_PARAMS;

typedef enum MENU_ID {
    MAIN_WINDOW, MENU, SETTINGS, OBSERV, O_GOTO, S_OBSERV, S_NETWORK, S_MONTY,S_DISPLAY, SM_TYPESELECT, SM_ALPHA, SM_DELTA, SM_GAMMA,EDIT_IP,EDIT_TIME,EDIT_ANGLE
} MENU_ID;

typedef enum SELECT_MODE {
    NO_SELECT, SELECT_LINE, SELECT_COLUMN
} SELECT_MODE;


void ProcessMenu( BYTE * KeyPressed);
#endif