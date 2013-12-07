#ifndef __uCMD_PROCESS_
#define __uCMD_PROCESS_

#ifdef __C30__
#   include "GenericTypeDefs.h"
#include "device_control.h"
#include "Queue.h"
#else
#include "stdafx.h"
#include "Queue.h"
#include "device_control.h"
#endif

#define Grad_to_Rad 0.017453292519943295
#define Rad_to_Grad 57.295779513082323

//#include "..\dsPIC33\TCPIP Demo App\OCTimer.h"


// структура DMA буфера для режима Delayed One-Shot в OC
typedef struct _OC_BUF {
    WORD r;
    WORD rs;
} OC_BUF;

typedef enum _COMMAND { 
    CMD_STOP = 0x00,                        // остановка
    CMD_GO_TO_POSITION,                     // перейти на позицию; параметры: a, d
    CMD_GO_TO_POSITION_AND_GUIDE_STAR,      // перейти на позицию и сопровождать звезду ( обычное часовое ведение) a, d
    CMD_GO_TO_POSITION_AND_GUIDE_SAT,       // перейти на позицию и сопровождать спутник ( нужны параметры движения спутника) a, d, a1, d1
    CMD_SET_GUIDE_TIME,                     // установить время сопровождения объекта
    CMD_GO_HOME,                            // перевести телескоп в положение парковки  
    CMD_OPEN_ROOF,                          // открыть купол ( посылает пакет на заданный URL) 
    CMD_CLOSE_ROOF,                         // закрыть купол
    CMD_CREATE_SNAPSHOT,                    // подает команду на камеру
    CMD_GET_SNAPSHOT,                       // запрашивает фотографию у камеры 
    CMD_SLOW_GO_HOME,                       // медленное возвращение на парковочную позицию после возобновления электропитания
    CMD_SET_AXIS,                           // калибровка ( устанавливает параметры, как текущее положение телескопа) a, d
    CMD_SET_GUIDE_SPEED,                    // калибровка ( устанавливает скорость сопровождения звезды) guidespeed
    CMD_SET_AIM_SPEED,                      // калибровка ( устанавливает скорость наведения) runspeed
    CMD_EMG_STOP = 0xFF,                    // аварийная остановка

} COMMAND;

 typedef enum _xCMD_STATE { // состояния
                           // приоритет|описание
     xCMD_STOP,            //10 остановлен
     //xCMD_START,           //10 включение канала
     xCMD_ACCELERATE,      //10 разгоняется
     xCMD_SET_SPEED,       //10 устанавливает точное значение интервала для xCMD_RUN (double)
     xCMD_RUN,             //10 движется с постоянной скоростью
     xCMD_DECELERATE,      //10 тормозит
     xCMD_SET_TIMER,       //10 установка таймера
     //xCMD_SET_TIMER_SOURCE,//10 установка источника сигнала для таймера
     xCMD_SET_DIRECTION,   //10 установка направления вращения
     //xCMD_SET_STEP,        //10 установка количества микрошагов
     xCMD_EMERGENCY_STOP,  //0 аварийная остановка
     //xCMD_BREAK,           //5 отменить все команды до этой и выбрать следующую ( только mCMD )
    xCMD_SLOW_RUN,        //режим ведения ( скорость такова, что буфер (64 шага) занимают много времени (при часовом ведении это 9,6 сек))
     xCMD_ERROR            // ошибочное состояние
 }xCMD_STATE;


// элемент очереди микрокоманд (сопровождает данные в буфере DMA)
// одновременно в одном буфере может быть несколько команд
typedef struct _xCMD_QUEUE{
    xCMD_STATE  State;                                  // команда
    DWORD       Value;                                  // параметры команды (количество шагов на выполнение команды)
} xCMD_QUEUE;

typedef struct _CMD_QUEUE{
    COMMAND     Command;                                // команда
    LONG       a;                                      // углы в шагах
    LONG       d;
    double       a1;                                     // скорость наведения
    double       d1;                                     // скорость сопровождения

} CMD_QUEUE;

#define BUFFER_QUEUE_SIZE 16
#define CMD_QUEUE_SIZE 10
#define uCMD_QUEUE_SIZE 24
#define mCMD_QUEUE_SIZE 10

typedef struct MOTOR_CONFIG {
    WORD                    StepPerTurn;                // количество шагов двигателя на оборот (200 - ДШИ-200)
    double                  K;                          // Kx + B ( K - тангенс угла наклона графика зависимости мощности двигателя от скорости вращения
    double                  B;                          // B - константа, мощность двигателя в Hm скорость в радианах в сек 
} MOTOR_CONFIG;

typedef struct MOTOR_DRIVER_CONFIG{
    WORD uStepPerStep;                                  // количество микрошагов на шаг (16)
    double dSTEPPulseWidth;                             // Длительность ипульса в секундах 2us    
}MOTOR_DRIVER_CONFIG;

typedef struct MOUNT_CONFIG {
    double Mass;                                        // масса монтировки
    double Radius;                                      // радиус оси/трубы телескопа
    double Length;                                      // длинна оси (эквивалентная)
    double Reduction;                                   // коэффициент редукции червячной пары (1/360)
} MOUNT_CONFIG;

typedef struct OC_CONFIG {
    OC_ID                   Index;                      // номер канала (номер OC)
    OC_WORK_MODE            WorkMode;                   // режим работы канала OC 
    WORD                    OCxCon;                     // Конфигурация канала
    // TODO: этим двум параметрам тут не место
    // TMR_PRESCALER Tmr2divide;                                    // делитель таймера 2(0 => 1, 3 => 8, 6 => 64, 8 => 256)
    // TMR_PRESCALER Tmr3divide;                                    // делитель таймера 3(0 => 1, 3 => 8, 6 => 64, 8 => 256)


    /*double                  d;                          // константы для минимизации вычислений
    double                  a;  
    double                  c;
    LONG                    XMinPosition;               // минимальное значение номера шага (положение датчика 0)
    LONG                    XMaxPosition;               // максимальное значение номера шага
    LONG                    XZenitPosition;             // номер шага в зените
    LONG                    XParkPosition;              // значение номера шага парковочной позиции        
    double                  MaxSpeed;
    double                  MaxAngle;
    double                  MinAngle;*/
}OC_CONFIG;

typedef struct DMA_CONFIG {
    DMA_ID                  DmaId;
    WORD                    DmaCfg;
    WORD                    DMABufSize;
} DMA_CONFIG;

typedef struct CHANEL_CONFIG {
    MOUNT_CONFIG            MntConfig;
    MOTOR_CONFIG            MConfig;
    MOTOR_DRIVER_CONFIG     DrvConfig;
    OC_CONFIG               OCConfig;
    DMA_CONFIG              DMAConfig;
    TIMERS_ID               TmrId;                      // номер таймера от которого работает канал (T2/T3)
    double                  dx;                         // шаг угла в градусах
    WORD                    wSTEPPulseWidth;            // Длительность ипульса ШАГ в интервалах таймера
    DWORD                   AccBaseAddress;             // Адрес начала таблицы ускорения на внешней памяти
    DWORD                   AccRecordCount;             // Количество записей в таблице ускорения на внешней памяти
    double                  K;                          // приведенные параметры двигателя
    double                  B;
    double                  Xmax;                       
    double                  Vmax;                       
    double                  Tmax;
    double                  GuideSpeed;                 // Скорость сопровождения звезды
}CHANEL_CONFIG;

typedef struct mCMD_STATUS {
    WORD                    AccX;                       // координата X в формуле ускорения 
    WORD                    RUN_Interval;             // текущий интервал
    xCMD_STATE              State;
    BYTE                    Priority;
} mCMD_STATUS;

typedef struct OC_CHANEL_STATE{
        
    PRIORITY_QUEUE          uCmdQueue;                  // очередь микрокоманд
    PRIORITY_QUEUE          BufferQueue;                  // очередь микрокоманд
    LONG                    XPosition;                  // текущий номер шага TODO: поддержать переключение скоростей
    BYTE                    CurrentDirection;           // направление вращения при движении ( зависит значение вывода Dir )    

    PRIORITY_QUEUE          mCmdQueue;                  // очередь миникоманд
    mCMD_STATUS             mCMD_Status;                // переменные, используемые при обработке миникоманд (предвыборка)

    xCMD_STATE              CurrentState;
    
    WORD                    T;
    BYTE                    Enable;                     // признак включенности
    BYTE                    Pulse;                      // флаг для Togle режима в OC (микрокоманды)
    
    CHANEL_CONFIG *         Config;
    

}OC_CHANEL_STATE;

#ifdef _WINDOWS_
#if !defined _APP_CONFIG_TYPE
#define _APP_CONFIG_TYPE
typedef struct { 
    DWORD_VAL MyIPAddr;
    DWORD_VAL MyMask;
    DWORD_VAL MyGateway;
    DWORD_VAL PrimaryDNSServer;
    DWORD_VAL SecondaryDNSServer;
    CHANEL_CONFIG    ChanellsConfig[3];      // Конфигурация каналов
    char NetBIOSName[16];
    DWORD Time;
} APP_CONFIG;
#endif //#if !defined _APP_CONFIG_TYPE
#endif // _WINDOWS_

int Cmd_Init(void);
int Cmd_Process(void);
void uCmd_DefaultConfig(CHANEL_CONFIG * Config, BYTE Number);
void CreateAccTable();
LONG GetAngle(BYTE ch);
DWORD GetStepsPerGrad(BYTE ch);
BYTE GetStatus(BYTE ch);
#endif //__uCMD_PROCESS_
