#ifndef __OC_TIMER_H_
#define __OC_TIMER_H_
#include "..\..\guidance\stdafx.h"

#ifdef __C30__
#include "GenericTypeDefs.h"
#endif

typedef union MOTOR_POSITION{
    BYTE Val;
    struct __16
    {        
        BYTE Step:4;
        BYTE b8:4;
    } b16;
    struct __8
    {        
        BYTE b0:1;
        BYTE Step:3;
        BYTE b8:4;
    } b8;
    struct __4
    {        
        BYTE b0:2;
        BYTE Step:2;
        BYTE b8:4;
    } b4;
    struct __2
    {        
        BYTE b0:3;
        BYTE Step:1;
        BYTE b8:4;
    } b2;
    struct __1
    {        
        BYTE b0:4;
        BYTE Step:1;
        BYTE b8:3;
    } b1;
} MOTOR_POSITION; 
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
#define PI2 6.283185307179586476925286766559
#define FREQ_STEP 20
#define BUF_SIZE 128
// половина BUF_SIZE
#define BUF_SIZE_2 64
// очередь команд
#define CQ_SIZE 10

//static double Accelerate[ACCELERATE_SIZE];
#define Grad_to_Rad 0.017453292519943295
#define Rad_to_Grad 57.295779513082323

typedef union _STATE_VALUE{
    double Speed;
    double Angle;
    int Dir;
    BYTE Timer;
    BYTE Step;
    void* Null;
}STATE_VALUE;

// параметры 

typedef enum GD_STATE { // состояния
    ST_STOP,            // остановлен
    ST_ACCELERATE,      // разгоняется
    ST_RUN,             // движется с постоянной скоростью
    ST_DECELERATE,      // тормозит
    ST_SET_TIMER,       // установка таймера
    ST_SET_DIRECTION,   // установка направления вращения
    ST_SET_STEP,        // установка количества микрошагов
    ST_EMERGENCY_STOP,  // аварийная остановка
}GD_STATE;

// очередь команд. если значение равно 0, то оно либо не используется, либо заполняется автоматически
typedef struct CMD_QUEUE{
    GD_STATE    State;      // команда
    STATE_VALUE Value;      // значение параметра
    //BYTE        Direction;  // направление движения в команде
    //double      Vend;       // скорость, которая будет достигнута
    //double      deltaX;     // расстояние, которое будет пройдено после выполнения команды
    //DWORD       RunStep;    // количество шагов на выполнение команды
}Cmd_Queue;

typedef struct RR{

    // очередь команды        
    Cmd_Queue               CmdQueue[CQ_SIZE];          // очередь команд
    BYTE                    NextCacheCmd;               // указатель на начало очереди
    BYTE                    NextWriteCmd;               // указатель на конец очереди
    BYTE                    CmdCount;                   // количество команд в очереди

    // параметры исполнения
    GD_STATE                RunState;                   // тип команды, выполняемой в данное время
    LONG                    XPosition;                  // текущий номер шага TODO: поддержать переключение скоростей
    BYTE                    RunDir;                     // направление вращения при движении ( зависит значение вывода Dir )    
    DWORD_VAL               T;

    // параметры предпросчета
    GD_STATE                CacheState;                 // тип команды, кешируемой в данное время
    LONG                    XCachePos;                  // текущее положение в просчете
    double                  CacheSpeed;                 // текущая скорость
    BYTE                    CacheDir;                   // направление вращения при просчете
    DWORD                   CacheCmdCounter;            // количество шагов до окончания команды
    DWORD                   Interval;                   // текущее значение интервала (число со знаком) почему?
    DWORD                   XaccBeg;                   // параметры функции ускорения
    double                  d;                          // константы для минимизации вычислений
    double                  a;  
    double                  c;
    DWORD                   T1;                         // значение времени, полученное в предыдущем вызове Accelerate/Deccelerate

    // параметры указывающие на момент окончания
    double                  Vend;                       // скорость завершения команды
    double                  deltaX;                     // координата завершения команды

    LONG                    XMinPosition;               // минимальное значение номера шага (положение датчика 0)
    LONG                    XMaxPosition;               // максимальное значение номера шага
    LONG                    XZenitPosition;             // номер шага в зените
    LONG                    XParkPosition;              // значение номера шага парковочной позиции        
    double                  MaxSpeed;
    double                  MaxAngle;
    double                  MinAngle;

    // служебные (оптимизация)
    
    // параметры математики
    //WORD   uSec;                                        // 1 микросекунда в тактах таймера
    DWORD  e;                                           // количество периодов таймера, необходимое для однократного вычисления
    double K;                                           // Kx + B ( K - тангенс угла наклона графика зависимости мощьности двигателя от скорости вращения
    double B;                                           // B - константа, мощъность двигателя в Hm скорость в радианах в сек 
    double TimerStep;                                   // шаг таймера в секундах (25ns)
    double dx;                                          // шаг угла в радианах
    double Mass;                                        // масса монтировки
    double Radius;                                      // радиус оси/трубы телескопа
    double Length;                                      // длинна оси (эквивалентная)
    double Reduction;                                   // коэффициент редукции червячной пары (1/360)
    WORD StepPerTurn;                                   // количество шагов двигателя на оборот (200 - ДШИ-200)
    WORD uStepPerStep;                                  // количество микрошагов на шаг (16)
    BYTE Index;                                         // номер канала
    BYTE TmrId;                                         // номер таймера
    BYTE Enable;                                        // признак включенности
    
}RR;

typedef struct {
    WORD R;
    WORD RS;
} BUF_TYPE;

typedef  struct 
#ifdef __C30__
__attribute__((__packed__))
#endif 
{
    DWORD Timestamp;									// метка времени UTC
    LONG XPosition;                  					// текущий номер шага TODO: поддержать переключение скоростей
    WORD uStepPerStep;                                  // текущее количество микрошагов на шаг (16)
}RRRAMSave;


typedef struct
#ifdef __C30__
__attribute__((__packed__))
#endif 
{
    RRRAMSave RRSave[3];
}RAMSaveConfig;

#ifdef __C30__

#define MS1_Tris            TRISGbits.TRISG12   // выход MS1
#define MS2_Tris            TRISGbits.TRISG13   // выход MS2
#define SLEEP_Tris          TRISGbits.TRISG14   // выход SLEEP
#define RESET_Tris          TRISGbits.TRISG15   // выход RESET
#define MS1                 LATGbits.LATG12     // выход MS1
#define MS2                 LATGbits.LATG13     // выход MS2
#define SLEEP               LATGbits.LATG14     // выход SLEEP
#define RESET               LATGbits.LATG15     // выход RESET

//описание выводов port1
#define PORT1_NULL_Tris     TRISAbits.TRISA0    // вход NULL
#define PORT1_POS_Tris      TRISDbits.TRISD8    // вход POS
#define PORT1_POS2_Tris     TRISEbits.TRISE0    // вход POS2
#define PORT1_NULL          PORTAbits.RA0       // вход NULL
#define PORT1_POS           PORTDbits.RD8       // вход POS
#define PORT1_POS2          PORTEbits.RE0       // вход POS2
#define PORT1_ENABLE_Tris   TRISAbits.TRISA9    // выход ENABLE
#define PORT1_DIR_Tris      TRISBbits.TRISB8    // выход DIR
#define PORT1_STEP_Tris     TRISDbits.TRISD0    // выход STEP
#define PORT1_ENABLE        LATAbits.LATA9      // выход ENABLE
#define PORT1_DIR           LATBbits.LATB8      // выход DIR
#define PORT1_STEP          LATDbits.LATD0      // выход STEP

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

int OCSetup(void);
int PushCmdToQueue(RR * rr, GD_STATE State, double Vend, double Xend, int Direction );
int Control(void * _This, WORD* Buf, WORD BufSize);
int GDateToJD(DateTime GDate, int * JDN, double * JD);
int JDToGDate(double JD, DateTime * GDate );
int GoToCmd(RR * rr, double VTarget, double XTarget, DWORD Tick);
double GetAngle(WORD n);
int GetStatus(WORD n);
double JulDay (BYTE D, BYTE M,WORD Y,BYTE h, BYTE m, BYTE s);
double LM_Sidereal_Time (double jd, double longitude);
int ProcessCmd(RR * rr);
#endif //__OC_TIMER_H_

