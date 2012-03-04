#ifndef __OC_TIMER_H_
#define __OC_TIMER_H_
#include "..\..\guidance\stdafx.h"

#ifdef __C30__
#include "GenericTypeDefs.h"
#endif



#define OCM_MODULE_DISABLED         0x00 //000 Module Disabled Controlled by GPIO register —
#define OCM_ONE_SHOT_LOW            0x01 //001 Active-Low One-Shot 0 OCx rising edge
#define OCM_ONE_SHOT_HIGH           0x02 //010 Active-High One-Shot 1 OCx falling edge
#define OCM_TOGGLE                  0x03 //011 Toggle Current output is maintained OCx rising and falling edge
// используем
#define OCM_DELAYED_ONE_SHOT        0x04 //100 Delayed One-Shot 0 OCx falling edge
#define OCM_CONTINUOUS_PULSE        0x05 //101 Continuous Pulse 0 OCx falling edge

#define OCM_PWM_WO_FAULT            0x06 //110 PWM without Fault Protection ‘0’, if OCxR is zero ‘1’, if OCxR is non-zero No interrupt
#define OCM_PWM_W_FAULT             0x07 //111 PWM with Fault Protection ‘0’, if OCxR is zero ‘1’, if OCxR is non-zero OCFA falling edge for OC1 to OC4

// описание значений флага
#define OCT_USE_TMR3                0x01 // 0 - используется таймер 2; 1 - таймер 3
#define OCT_USE_PERIOD_BUFFER       0x02 // 1 - используем буффер периода. 0 - в буфере периода только 1 значение
#define OCT_USE_PULSE_BUFFER        0x04 // 1 - используем буффер пульса. 0 - в буфере пульса только 1 значение
#define OCT_USE_STEPS_LEFT          0x08 // 1 - используем StepsLeft(). 0 - Продолжающееся движение до достижения максимального значения
#define OCT_IS_USE                  0x80 // Этот таймер используется


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
#define PI 3.1415926535897932384626433832795
#define FREQ_STEP 20
#define BUF_SIZE 64
// половина BUF_SIZE
#define BUF_SIZE_2 32
// очередь команд
#define CQ_SIZE 10

//static double Accelerate[ACCELERATE_SIZE];
#define Grad_to_Rad 0.017453292519943295
#define Rad_to_Grad 57.295779513082323

// параметры 

typedef struct ARR_TYPE {
    DWORD FixedPoint;   // опорная точка
    DWORD Interval;     // Интервал от предыдущей точки
    WORD  Count;        // Количество интервалов
    WORD  Correction;   // остаток от деления полного интервала на 32
} ARR_TYPE;

typedef enum Cmd{ // команды
    CM_STOP,            // Остановиться (снижаем скорость до остановки)
    CM_RUN_WITH_SPEED,  // Двигаться с заданной скоростью до окончания
    CM_RUN_TO_POINT,    // Двигаться до указанного угла
}GD_CMD;

typedef enum State{     // состояния
    ST_STOP,            // остановлен
    ST_ACCELERATE,      // разгоняется
    ST_RUN,             // движется с постоянной скоростью
    ST_DECELERATE,      // тормозит
    ST_BUFFER_FREE      // сигнализирует об нехватке буфера ( проскок )
}GD_STATE;

// очередь команд. если значение равно 0, то оно либо не используется, либо заполняется автоматически
typedef struct CMD_QUEUE{
    GD_STATE State;
    double Vend;
    double Xend;
    INT Direction;
    LONG RunStep; //  количество шагов на выполнение команды
}Cmd_Queue;

typedef struct RR{

    // Время
    // IntervalArray
    // |      NextWriteTo
    // |      |      NextReadFrom
    // |      |      |
    // v      v      v
    // 0======-------=========
    //
    // окончание предыдущей операции ( исходные параметры операции)
    // | промежуточное состояние
    // | | Конец операции
    // | | |
    // v v v
    // -------========--------
    //                    
    //  
    DWORD                   Interval;
    ARR_TYPE                IntervalArray[BUF_SIZE];    // массив отсчетов времени (кольцевой буффер)
    WORD                    NextReadFrom;               // индекс массива времени. указывает на первый значащий элемент
    WORD                    NextWriteTo;                // индекс массива времени. указывает на первый свободный элемент
    WORD                    DataCount;                  // количество данных в массиве.
    DWORD_VAL               T;


    // команды
    GD_CMD                  Cmd;
    GD_STATE                CacheState;
    GD_STATE                RunState;

    Cmd_Queue               CmdQueue[CQ_SIZE];          // очередь команд
    WORD                    NextCacheCmd;
    WORD                    NextWriteCmd;
    WORD                    NextExecuteCmd;
    WORD                    CmdCount;

    // параметры исполнения
    LONG                    XPosition;
    int                     RunDir;                     // направление вращения при движении ( зависит значение вывода Dir )
    LONG                    RunCmdCounter;
    // параметры предпросчета
    LONG                    XCachePos;                  // текущее положение в просчете
    int                     CalcDir;                    // направление вращения при просчете
    LONG                    CacheCmdCounter;
    // исхoдные параметры:
    DWORD                   TimeBeg;
    LONG                    XaccBeg;                    //параметры функции ускорения (желательно целое число шагов)

    // параметры указывающие на момент окончания
    double                  Vend;                       //(надо знать скорость, на которой завершится ускорение)
    double                  Xend;


    // служебные (оптимизация)
    double                  dX_acc_dec_pos;                 // текущее положение в просчете в радианах
    DWORD                  d;
    DWORD                  a;
    // константы
    DWORD  e; // если интервал меньше этого значения, переходим на быстрые вычисления
    double K;
    double B;
    double TimerStep;
    double dx;
    double Mass;
    double Radius;
    double Length;
    double Reduction;
    WORD StepPerTurn;
    WORD uStepPerStep;
    BYTE Index;
    BYTE TmrId;
}RR;

#ifdef __C30__

#define MS1_Tris TRISGbits.TRISG12 // выход MS1
#define MS2_Tris TRISGbits.TRISG13 // выход MS2
#define SLEEP_Tris TRISGbits.TRISG14 // выход SLEEP
#define RESET_Tris TRISGbits.TRISG15 // выход RESET
#define MS1 LATGbits.LATG12 // выход MS1
#define MS2 LATGbits.LATG13 // выход MS2
#define SLEEP LATGbits.LATG14 // выход SLEEP
#define RESET LATGbits.LATG15 // выход RESET

//описание выводов port1
#define PORT1_NULL_Tris TRISAbits.TRISA0 // вход NULL
#define PORT1_POS_Tris TRISDbits.TRISD8 // вход POS
#define PORT1_POS2_Tris TRISEbits.TRISE0 // вход POS2
#define PORT1_NULL PORTAbits.RA0 // вход NULL
#define PORT1_POS PORTDbits.RD8 // вход POS
#define PORT1_POS2 PORTEbits.RE0 // вход POS2
#define PORT1_ENABLE_Tris TRISAbits.TRISA9 // выход ENABLE
#define PORT1_DIR_Tris TRISBbits.TRISB8 // выход DIR
#define PORT1_STEP_Tris TRISDbits.TRISD0 // выход STEP
#define PORT1_ENABLE LATAbits.LATA9 // выход ENABLE
#define PORT1_DIR LATBbits.LATB8 // выход DIR
#define PORT1_STEP LATDbits.LATD0 // выход STEP

//инициализация выводов port2
#define PORT2_NULL_Tris TRISAbits.TRISA1 // вход NULL
#define PORT2_POS_Tris TRISDbits.TRISD9 // вход POS
#define PORT2_POS2_Tris TRISEbits.TRISE1 // вход POS2
#define PORT2_NULL PORTAbits.RA1 // вход NULL
#define PORT2_POS PORTDbits.RD9 // вход POS
#define PORT2_POS2 PORTEbits.RE1 // вход POS2
#define PORT2_ENABLE_Tris TRISAbits.TRISA10 // выход ENABLE
#define PORT2_DIR_Tris TRISBbits.TRISB9 // выход DIR
#define PORT2_STEP_Tris TRISDbits.TRISD1 // выход STEP
#define PORT2_ENABLE LATAbits.LATA10 // выход ENABLE
#define PORT2_DIR LATBbits.LATB9 // выход DIR
#define PORT2_STEP LATDbits.LATD1 // выход STEP

//инициализация выводов port3
#define PORT3_NULL_Tris TRISAbits.TRISA2 // вход NULL
#define PORT3_POS_Tris TRISDbits.TRISD10 // вход POS
#define PORT3_POS2_Tris TRISEbits.TRISE2 // вход POS2
#define PORT3_NULL PORTAbits.RA2 // вход NULL
#define PORT3_POS PORTDbits.RD10 // вход POS
#define PORT3_POS2 PORTEbits.RE2 // вход POS2
#define PORT3_ENABLE_Tris TRISAbits.TRISA9 // выход ENABLE
#define PORT3_DIR_Tris TRISBbits.TRISB10 // выход DIR
#define PORT3_STEP_Tris TRISDbits.TRISD2 // выход STEP
#define PORT3_ENABLE LATAbits.LATA9 // выход ENABLE
#define PORT3_DIR LATBbits.LATB10 // выход DIR
#define PORT3_STEP LATDbits.LATD2 // выход STEP

//инициализация выводов port4
#define PORT4_NULL_Tris TRISAbits.TRISA3 // вход NULL
#define PORT4_POS_Tris TRISDbits.TRISD11 // вход POS
#define PORT4_POS2_Tris TRISEbits.TRISE3 // вход POS2
#define PORT4_NULL PORTAbits.RA3 // вход NULL
#define PORT4_POS PORTDbits.RD11 // вход POS
#define PORT4_POS2 PORTEbits.RE3 // вход POS2
#define PORT4_ENABLE_Tris TRISAbits.TRISA9 // выход ENABLE
#define PORT4_DIR_Tris TRISBbits.TRISB11 // выход DIR
#define PORT4_STEP_Tris TRISDbits.TRISD3 // выход STEP
#define PORT4_ENABLE LATAbits.LATA9 // выход ENABLE
#define PORT4_DIR LATBbits.LATB11 // выход DIR
#define PORT4_STEP LATDbits.LATD3 // выход STEP

//инициализация выводов port5
#define PORT5_NULL_Tris TRISAbits.TRISA4 // вход NULL
#define PORT5_POS_Tris TRISDbits.TRISD12 // вход POS
#define PORT5_POS2_Tris TRISEbits.TRISE4 // вход POS2
#define PORT5_NULL PORTAbits.RA4 // вход NULL
#define PORT5_POS PORTDbits.RD12 // вход POS
#define PORT5_POS2 PORTEbits.RE4 // вход POS2
#define PORT5_ENABLE_Tris TRISAbits.TRISA9 // выход ENABLE
#define PORT5_DIR_Tris TRISBbits.TRISB12 // выход DIR
#define PORT5_STEP_Tris TRISDbits.TRISD4 // выход STEP
#define PORT5_ENABLE LATAbits.LATA9 // выход ENABLE
#define PORT5_DIR LATBbits.LATB12 // выход DIR
#define PORT5_STEP LATDbits.LATD4 // выход STEP

//инициализация выводов port6
#define PORT6_NULL_Tris TRISAbits.TRISA5 // вход NULL
#define PORT6_POS_Tris TRISDbits.TRISD13 // вход POS
#define PORT6_POS2_Tris TRISEbits.TRISE5 // вход POS2
#define PORT6_NULL PORTAbits.RA5 // вход NULL
#define PORT6_POS PORTDbits.RD13 // вход POS
#define PORT6_POS2 PORTEbits.RE5 // вход POS2
#define PORT6_ENABLE_Tris TRISAbits.TRISA9 // выход ENABLE
#define PORT6_DIR_Tris TRISBbits.TRISB13 // выход DIR
#define PORT6_STEP_Tris TRISDbits.TRISD5 // выход STEP
#define PORT6_ENABLE LATAbits.LATA9 // выход ENABLE
#define PORT6_DIR LATBbits.LATB13 // выход DIR
#define PORT6_STEP LATDbits.LATD5 // выход STEP

//инициализация выводов port7
#define PORT7_NULL_Tris TRISAbits.TRISA6 // вход NULL
#define PORT7_POS_Tris TRISDbits.TRISD14 // вход POS
#define PORT7_POS2_Tris TRISEbits.TRISE6 // вход POS2
#define PORT7_NULL PORTAbits.RA6 // вход NULL
#define PORT7_POS PORTDbits.RD14 // вход POS
#define PORT7_POS2 PORTEbits.RE6 // вход POS2
#define PORT7_ENABLE_Tris TRISAbits.TRISA9 // выход ENABLE
#define PORT7_DIR_Tris TRISBbits.TRISB14 // выход DIR
#define PORT7_STEP_Tris TRISDbits.TRISD6 // выход STEP
#define PORT7_ENABLE LATAbits.LATA9 // выход ENABLE
#define PORT7_DIR LATBbits.LATB14 // выход DIR
#define PORT7_STEP LATDbits.LATD6 // выход STEP

//инициализация выводов port8
#define PORT8_NULL_Tris TRISAbits.TRISA7 // вход NULL
#define PORT8_POS_Tris TRISDbits.TRISD15 // вход POS
#define PORT8_POS2_Tris TRISEbits.TRISE7 // вход POS2
#define PORT8_NULL PORTAbits.RA7 // вход NULL
#define PORT8_POS PORTDbits.RD15 // вход POS
#define PORT8_POS2 PORTEbits.RE7 // вход POS2
#define PORT8_ENABLE_Tris TRISAbits.TRISA9 // выход ENABLE
#define PORT8_DIR_Tris TRISBbits.TRISB15 // выход DIR
#define PORT8_STEP_Tris TRISDbits.TRISD7 // выход STEP
#define PORT8_ENABLE LATAbits.LATA9 // выход ENABLE
#define PORT8_DIR LATBbits.LATB15 // выход DIR
#define PORT8_STEP LATDbits.LATD7 // выход STEP

#endif

int OCInit(void);
int TmrInit(BYTE Num);
//int OCTimerInit(BYTE num, DWORD Steps, DWORD * Periods, DWORD * Pulses, BYTE TmrNum,WORD ocm);
int Run(RR * rr);
int Acceleration(RR * rr);
int Deceleration(RR * rr);
int Control(RR * rr);

int CacheNextCmd(RR * rr);
int PushCmdToQueue(RR * rr, GD_STATE State, double Vend, double Xend, int Direction );
int ProcessTimer(BYTE id, RR * rr);
DWORD GetBigTmrValue(BYTE id);
int DisableOC(BYTE oc);
int EnableOC(BYTE oc);
BOOL IsDisableOC(BYTE oc);
int SetOC(BYTE oc, WORD LW);
int ProcessOC(RR * rr);
int SetDirection(BYTE oc, BYTE Dir);

int CalculateBreakParam(RR * rr, GD_STATE State, int Direction, double Vbeg, double Xbeg, double * Vend, double * Xend, LONG * Xbreak);

int GDateToJD(DateTime GDate, int * JDN, double * JD);
int JDToGDate(double JD, DateTime * GDate );
#endif //__OC_TIMER_H_

