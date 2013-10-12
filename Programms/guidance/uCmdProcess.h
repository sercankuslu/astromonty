#ifndef __uCMD_PROCESS_
#define __uCMD_PROCESS_

#ifdef __C30__
#   include "GenericTypeDefs.h"
#else
#   include "stdafx.h"
#endif

#include "..\dsPIC33\TCPIP Demo App\device_control.h"
#include "..\dsPIC33\TCPIP Demo App\OCTimer.h"


// структура DMA буфера для режима Delayed One-Shot в OC
typedef struct OC_BUF {
    WORD r;
    WORD rs;
} OC_BUF;


 typedef enum xCMD_STATE { // состояния
                           // приоритет|описание
     xCMD_STOP,            //10 остановлен
     xCMD_START,           //10 включение канала
     xCMD_ACCELERATE,      //10 разгоняется
     xCMD_RUN,             //10 движется с постоянной скоростью
     xCMD_DECELERATE,      //10 тормозит
     xCMD_SET_TIMER,       //10 установка таймера
     //xCMD_SET_TIMER_SOURCE,//10 установка источника сигнала для таймера
     xCMD_SET_DIRECTION,   //10 установка направления вращения
     //xCMD_SET_STEP,        //10 установка количества микрошагов
     xCMD_EMERGENCY_STOP,  //0 аварийная остановка
     xCMD_BREAK,           //5 отменить все команды до этой и выбрать следующую ( только mCMD )
     xCMD_ERROR            // ошибочное состояние
 }xCMD_STATE;


// элемент очереди микрокоманд (сопровождает данные в буфере DMA)
// одновременно в одном буфере может быть несколько команд
typedef struct xCMD_QUEUE{
    xCMD_STATE  State;                                  // команда
    DWORD       Value;                                  // параметры команды (количество шагов на выполнение команды)
} xCMD_QUEUE;

#define uCMD_QUEUE_SIZE 10
#define mCMD_QUEUE_SIZE 10

typedef struct MOTOR_CONFIG {
    WORD StepPerTurn;                                   // количество шагов двигателя на оборот (200 - ДШИ-200)
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

typedef struct CHANEL_CONFIG {
    MOUNT_CONFIG            MConfig;
    MOTOR_DRIVER_CONFIG     DrvConfig;
    OC_CONFIG               OCConfig;
    TIMERS_ID               TmrId;                      // номер таймера от которого работает канал (T2/T3)
    double                  dx;                         // шаг угла в градусах
    WORD                    wSTEPPulseWidth;            // Длительность ипульса ШАГ в интервалах таймера
    DMA_ID                  DmaId;
    DWORD                   AccBaseAddress;             // Адрес начала таблицы ускорения на внешней памяти
    DWORD                   AccRecordCount;             // Количество записей в таблице ускорения на внешней памяти
}CHANEL_CONFIG;

typedef struct mCMD_STATUS {
    WORD                    AccX;                       // координата X в формуле ускорения 
} mCMD_STATUS;

typedef struct OC_CHANEL_STATE{
        
    PRIORITY_QUEUE          uCmdQueue;                  // очередь микрокоманд
    LONG                    XPosition;                  // текущий номер шага TODO: поддержать переключение скоростей
    BYTE                    CurrentDirection;           // направление вращения при движении ( зависит значение вывода Dir )

    PRIORITY_QUEUE          mCmdQueue;                  // очередь миникоманд
    mCMD_STATUS             mCMD_Status;                // переменные, используемые при обработке миникоманд (предвыборка)

    xCMD_STATE              CurrentState;
    
    DWORD_VAL               T;
    BYTE                    Enable;                     // признак включенности
    
    CHANEL_CONFIG           Config;
    

}OC_CHANEL_STATE;



int uCmd_Init();
int uCmd_DMACallback(void*, BYTE*, WORD);
int uCmd_OCCallback(void * _This);
int uCmd_ICCallback(void * _This);
#endif //__uCMD_PROCESS_