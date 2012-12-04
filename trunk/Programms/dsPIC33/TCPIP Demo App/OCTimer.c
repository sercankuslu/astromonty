// #ifndef __OCTIMER_C
// #define __OCTIMER_C
// #endif

#include "stdafx.h"

#ifdef __C30__
#   include "TCPIP Stack/TCPIP.h"
#   include "math.h"
//#   include "device_control.h"
#else
#   include <math.h>
#endif
#include "device_control.h"
#include "OCTimer.h"


#define abs(x) (x<0? -x : x)

static DWORD_VAL Timer2Big;
static DWORD_VAL Timer3Big;
static MOTOR_POSITION M1;
static MOTOR_POSITION M2;
static MOTOR_POSITION M3;
#ifdef __C30__
__attribute__((far)) RR rr1;
__attribute__((far)) RR rr2;
__attribute__((far)) RR rr3;
__attribute__((far)) RAMSaveConfig RRConfigRAM;
__attribute__((far)) APP_CONFIG AppConfig;
#else
RR rr1;
RR rr2;
RR rr3;
DWORD TickGet()
{
    return 0;
}
#endif
TIMERS_CON TimerConfig[2];

int InitRR(RR * rr);
int CacheNextCmd(RR * rr);
int SetDirection(BYTE oc, BYTE Dir);
int CalculateBreakParam(RR * rr, GD_STATE State, double Vbeg, double * Vend, double * deltaX, double * deltaT, LONG * Xbreak);
void CalculateParams(RR * rr);
DWORD GetInterval(DWORD T, DWORD Xa, double dx, double a, double d);

double frac(double X);
double GM_Sidereal_Time (double jd);
int RunControl(void * _This, DMA_ID id);

int PushRunCmd(RR * rr, GD_STATE State, BYTE Direction, TIMERS_ID Timer,DWORD RunStep);
Run_Cmd_Queue PopRunCmd(RR * rr);
int SearchHighPriorityRunCmd(RR * rr);


#ifdef __C30__


#else
    //WORD TMR2 = 0;
    //WORD TMR3 = 0;
    //WORD OC1R = 0;
#endif
//------------------------------------------------------------------------------------------------
int SetDirection(BYTE oc, BYTE Dir)
//------------------------------------------------------------------------------------------------
{
    return 0;
}
//------------------------------------------------------------------------------------------------
int OCSetup(void)
//------------------------------------------------------------------------------------------------
{
        //BYTE i;
    #ifdef __C30__
        // MS1 = 1; MS2 = 1; 1/16
        // MS1 = 0; MS2 = 1; 1/4
        // MS1 = 1; MS2 = 0; 1/2
        // MS1 = 0; MS2 = 0; 1/1
        MS1         = 1;    // выход MS1
        MS2         = 1;     // выход MS2
        SLEEP       = 1;     // выход SLEEP
        RESET       = 1;     // выход RESET

        MS1_Tris    = 0;     // выход MS1
        MS2_Tris    = 0;     // выход MS2
        SLEEP_Tris  = 0;    // выход SLEEP
        RESET_Tris  = 0;    // выход RESET
        // инициализация прерывания для заполнения буфера
        /*
        IFS1bits.U2RXIF = 0;
        IPC7bits.U2RXIP = 5;        // Priority level 6
        IEC1bits.U2RXIE = 1;

        IFS1bits.U2TXIF = 0;
        IPC7bits.U2TXIP = 5;        // Priority level 6
        IEC1bits.U2TXIE = 1;

        IFS0bits.U1RXIF = 0;
        IPC2bits.U1RXIP = 5;        // Priority level 6
        IEC0bits.U1RXIE = 1;
        */
        //Инициализация порта1
        {
                PORT1_NULL_Tris   = 1; // вход NULL
                PORT1_POS_Tris    = 1; // вход POS
                PORT1_POS2_Tris   = 1; // вход POS2

                PORT1_ENABLE      = 0;// выход ENABLE
                PORT1_DIR         = 0;// выход DIR
                PORT1_STEP        = 0;// выход STEP

                PORT1_ENABLE_Tris = 0;// выход ENABLE
                PORT1_DIR_Tris    = 0;// выход DIR
                PORT1_STEP_Tris   = 0;// выход STEP
        }
        {
                PORT2_NULL_Tris   = 1; // вход NULL
                PORT2_POS_Tris    = 1; // вход POS
                PORT2_POS2_Tris   = 1; // вход POS2

                PORT2_ENABLE      = 0;// выход ENABLE
                PORT2_DIR         = 0;// выход DIR
                PORT2_STEP        = 0;// выход STEP

                PORT2_ENABLE_Tris = 0;// выход ENABLE
                PORT2_DIR_Tris    = 0;// выход DIR
                PORT2_STEP_Tris   = 0;// выход STEP
        }
        {
                PORT3_NULL_Tris   = 1; // вход NULL
                PORT3_POS_Tris    = 1; // вход POS
                PORT3_POS2_Tris   = 1; // вход POS2

                PORT3_ENABLE      = 0;// выход ENABLE
                PORT3_DIR         = 0;// выход DIR
                PORT3_STEP        = 0;// выход STEP

                PORT3_ENABLE_Tris = 0;// выход ENABLE
                PORT3_DIR_Tris    = 0;// выход DIR
                PORT3_STEP_Tris   = 0;// выход STEP
        }
    #endif //#ifdef __C30__

        //TmrInit(2);
        rr1.Index = ID_OC1;
        rr1.TmrId = TIMER2;
        InitRR(&rr1);
        rr2.Index = ID_OC2;
        rr2.TmrId = TIMER2;
        InitRR(&rr2);
        rr3.Index = ID_OC3;
        rr3.TmrId = TIMER3;
        InitRR(&rr3);
        M1.Val = 0;
        M2.Val = 0;
        M3.Val = 0;
    #ifdef __C30__
        //IFS1bits.U2RXIF = 1;
        //IFS1bits.U2TXIF = 1;
        //IFS0bits.U1RXIF = 1;
    /*
        // инициализация OC1        
        OCSetInt(ID_OC1, 6, TRUE);
        OCSetCallback(ID_OC1, NULL);
        OCInit(ID_OC1, IDLE_DISABLE, OC_TMR2, OC_DISABLED);
        
        // инициализация OC2        
	    OCSetInt(ID_OC2, 6, TRUE);
	    OCSetCallback(ID_OC2, NULL);
	    OCInit(ID_OC2, IDLE_DISABLE, OC_TMR2, OC_DISABLED);
        
        // инициализация OC3
        OCSetInt(ID_OC3, 6, TRUE);
        OCSetCallback(ID_OC3, NULL);
	    OCInit(ID_OC3, IDLE_DISABLE, OC_TMR2, OC_DISABLED);
	    
        // Initialize Timer2
        TimerInit(T2, CLOCK_SOURCE_INTERNAL, GATED_DISABLE, PRE_1_1, IDLE_DISABLE, BIT_16, SYNC_DISABLE);
        TimerSetValue(T2, 0, 0xFFFF);
        OCSetCallback(T2, NULL);
        TimerSetInt(T2, 5, FALSE);

        // Initialize Timer3
        TimerInit(TIMER3, CLOCK_SOURCE_INTERNAL, GATED_DISABLE, PRE_1_256, IDLE_DISABLE, BIT_16, SYNC_DISABLE);
        TimerSetValue(TIMER3, 0, 0xFFFF);
        OCSetCallback(TIMER3, NULL);
        TimerSetInt(TIMER3, 5, FALSE);

        // Setup and Enable DMA Channel
        DMAInit(DMA0, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
        DMASelectDevice(DMA0, IRQ_OC1, (int)&OC1R);
        DMASetBufferSize(DMA0, 64);
        DMASetCallback(DMA0, (ROM void*)fillingBufferR, (ROM void*)fillingBufferR);
        DMASetInt(DMA0, 5, TRUE);
        DMAPrepBuffer(DMA0);
        DMASetState(DMA0, TRUE, FALSE);
        
        DMAInit(DMA1, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
        DMASelectDevice(DMA1, IRQ_OC1, (int)&OC1RS);
        DMASetBufferSize(DMA1, 64);
        DMASetCallback(DMA1, (ROM void*)fillingBufferRS, (ROM void*)fillingBufferRS);
        DMASetInt(DMA1, 5, TRUE);
        DMAPrepBuffer(DMA1);
        DMASetState(DMA1, TRUE, FALSE); 
        
        DMAInit(DMA2, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
        DMASelectDevice(DMA2, IRQ_OC2, (int)&OC2R);
        DMASetBufferSize(DMA2, 64);
        DMASetCallback(DMA2, (ROM void*)fillingBufferR1, (ROM void*)fillingBufferR1);
        DMASetInt(DMA2, 5, TRUE);
        DMAPrepBuffer(DMA2);
        DMASetState(DMA2, TRUE, FALSE);
        
        DMAInit(DMA3, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
        DMASelectDevice(DMA3, IRQ_OC2, (int)&OC2RS);
        DMASetBufferSize(DMA3, 64);
        DMASetCallback(DMA3, (ROM void*)fillingBufferRS1, (ROM void*)fillingBufferRS1);
        DMASetInt(DMA3, 5, TRUE);
        DMAPrepBuffer(DMA3);
        DMASetState(DMA3, TRUE, FALSE);      
      
        T2CONbits.TON = 1;
        T3CONbits.TON = 1;
        
        
        */
        
        //ProcessOC(&rr1);
        //ProcessOC(&rr2);
        //ProcessOC(&rr3);
        //Timer2Big.Val = 0;
        //TMR2 = 0;
        //T2CONbits.TON = 1;         // Start Timer
    #endif //#ifdef __C30__

    return 0;
}
//------------------------------------------------------------------------------------------------
int InitRR(RR * rr)
//------------------------------------------------------------------------------------------------
{   //AppConfig
    #ifdef __C30__
    if(AppConfig.RRConfig[rr->Index].Control){
        rr->Mass = AppConfig.RRConfig[rr->Index].Mass;
        rr->Radius = AppConfig.RRConfig[rr->Index].Radius;
        rr->Length = AppConfig.RRConfig[rr->Index].Length;
        rr->Reduction = AppConfig.RRConfig[rr->Index].Reduction;
        rr->StepPerTurn = AppConfig.RRConfig[rr->Index].StepPerTurn;
        rr->uStepPerStep = AppConfig.RRConfig[rr->Index].uStepPerStep; //16
        rr->TimerStep = AppConfig.RRConfig[rr->Index].TimerStep;//0.0000002; // шаг таймера
        //rr->VMax = AppConfig.RRConfig[rr->Index].VMax * Grad_to_Rad;
        rr->XPosition = RRConfigRAM.RRSave[rr->Index].XPosition;
    } else 
    #endif
    {
        rr->Mass = MY_DEFAULT_RR_PARA_Mass;
        rr->Radius = MY_DEFAULT_RR_PARA_Radius;
        rr->Length = MY_DEFAULT_RR_PARA_Length;
        rr->Reduction = MY_DEFAULT_RR_PARA_Rdct;
        rr->StepPerTurn = MY_DEFAULT_RR_PARA_SPT;
        rr->uStepPerStep = MY_DEFAULT_RR_PARA_uSPS; //16
        rr->TimerStep = MY_DEFAULT_RR_PARA_TimerStep;//0.0000002; // шаг таймера
        //rr->VMax = MY_DEFAULT_RR_PARA_VMax* Grad_to_Rad;
        rr->XPosition = 0;
    }
    rr->Vend = 0.0;
    rr->deltaX = 0.0;
    //rr->DataCount = 0;
    //rr->NextWriteTo = 0;
    //rr->NextReadFrom = 0;
    //rr->XaccBeg = 0;
    rr->XCachePos = 0;
    rr->CacheCmdCounter = 0;
    //Timer2Big.word.LW = TMR2;
    //rr->TimeBeg = 0;
    rr->CacheState = ST_STOP;
    rr->RunState = ST_STOP;
    rr->T.Val = 0;
    rr->T1 = 0;
    rr->CmdCount = 0;
    rr->CacheDir = 1;
    rr->RunDir = 1;    
    rr->CmdBeginQueue = 0;
    rr->CmdEndQueue = 0;
    rr->Interval = 32768;
    //rr->LastCmdV = 0.0;
    //rr->LastCmdX = 0.0;
    
    CalculateParams(rr);

    return 0;
}
//------------------------------------------------------------------------------------------------
void CalculateParams(RR * rr)
//------------------------------------------------------------------------------------------------
{
    double I;
    
    I = ((rr->Mass*rr->Radius*rr->Radius/4) + (rr->Mass*rr->Length*rr->Length/12))/rr->Reduction;
#ifdef __C30__
    if(AppConfig.RRConfig[rr->Index].Control){
        rr->K = AppConfig.RRConfig[rr->Index].K / I;
        rr->B = AppConfig.RRConfig[rr->Index].B / I;
    } else
#endif
    {
        // данные TODO: надо бы их в AppConfig.RRConfig[rr->Index] добавить
        double F1 = 100;
        double F2 = 1000;
        double P1 = 0.76;
        double P2 = 0.46;
        // предварительные вычисления. здесь характеристики двигателей.
        double V1 = F1/rr->StepPerTurn; // скорость в оборотах в секунду 
        double V2 = F2/rr->StepPerTurn;
        double A1 = (P1/I)*Rad_to_Grad;
        double A2 = (P2/I)*Rad_to_Grad;
        // получение констант
        rr->K = (A2 - A1)/(2 *(V2 - V1)); // размерность 1/сек
        rr->B = (A1 - V1 * rr->K*2)/2;      // размерность 1/сек^2
    }
    rr->dx = 360.0 /(rr->Reduction * rr->StepPerTurn * rr->uStepPerStep); // шаг перемещения в градусах
    rr->d = (-(rr->K)/(2.0 * rr->B * rr->TimerStep));
    rr->a = (4.0 * rr->B/(rr->K * rr->K));
    rr->OneStepCalcTime = (DWORD)(0.000160 / rr->TimerStep); //160us
}
//------------------------------------------------------------------------------------------------
// вычисление скорости ( только для ускорения или торможения )
// V = BT(2-KT)/(1-KT)^2 => V = U(BTU+1), U = 1/(1+KT)
// T                          значение функции ускорения T=(-Kx+sqrt((Kx)^2+4Bx))/2B оптимиз: 
//                            T = d(x+sqrt(x(x+a)), d = -k/2BTs, a = 4B/K^2; Ts - TimerStep
//------------------------------------------------------------------------------------------------
double GetSpeed(double T, double K, double B)
//------------------------------------------------------------------------------------------------
{
    double U = 1.0/(1.0 - K * T);
    return (U *(B * T * U + 1.0));
}
//------------------------------------------------------------------------------------------------
// Вычисление интервала между значением T и значением функции, вычесленной на основании X
// T                        предыдущее значение функции ускорения T=(-Kx+sqrt((Kx)^2+4Bx))/2B оптимиз: 
//                          T = d(x+sqrt(x(x+a)), d = -k/2BTs, a = 4B/K^2; Ts - TimerStep
// Xa                       текущее значение координаты X в алгоритме ускорения
// dx                       шаг двигателя в радианах
// a                        константа
// d                        константа
//------------------------------------------------------------------------------------------------
DWORD GetInterval(DWORD T, DWORD Xa, double dx, double a, double d)
//------------------------------------------------------------------------------------------------
{
    double D;
    double X = Xa * dx;
    DWORD R = 0;
    D = X *(X + a);
    if(D >= 0.0){
        R =(DWORD)((X + sqrt(D))* d);
        if(R >= T) {
            return R - T; 
            
        } else {
            return T - R;
        }
    }
    return 0;
}


// Функция для вызова через DMASetCallback 
//------------------------------------------------------------------------------------------------
int Control(void * _This, WORD* Buf, WORD BufSize)
//------------------------------------------------------------------------------------------------
{
    RR * rr = (RR*)_This;
    WORD i = 0;
    WORD j = 0;
    for(i = 0; (i < BufSize)&&(rr->CacheState != ST_STOP);){
        if(j == BufSize) j = 0;
        j += CalculateMove(rr, (BUF_TYPE*)&Buf[j], (BufSize - j)/2);
        PushRunCmd(rr, rr->CacheState, rr->CacheDir, rr->TmrId, j);
        if(rr->CacheCmdCounter <= 0){
            CacheNextCmd(rr);
        }        
    }
    if(rr->CacheState == ST_STOP)
        return 1;
    return 0;
}

//------------------------------------------------------------------------------------------------
/*
Вычисление и заполнение буфера одной командой. (ST_ACCELERATE,ST_DECELERATE,ST_RUN)
Используем:
rr->CacheState                  текущая команда
rr->CacheCmdCounter             счетчик оставшихся шагов в команде
rr->dx                          ширина шага
rr->a                           вспомогательная констата 4B/k^2
rr->d                           вспомогательная констата -k/2BTs
rr->e                           период времени, необходимого для однократного вычисления(в шагах таймера)
rr->TimerStep                   интервал таймера в секундах
rr->T1                          предыдущее значение T(времени) функции ускорения
rr->XaccBeg                     координата X функции ускорения
rr->Interval                    интервал
rr->T                           счетчик интервалов
rr->CacheSpeed                  скорость
*/
//------------------------------------------------------------------------------------------------
WORD CalculateMove(RR * rr, BUF_TYPE* buf, WORD count)
//------------------------------------------------------------------------------------------------
{
    WORD FreeData;
    DWORD dT = 0;
    LONG Corr = 0;
    WORD i = 0;
    WORD j = 0;
    WORD m = 1;
    BYTE m1 = 0;    
    double dx_div_Ts = rr->dx / rr->TimerStep;
    // оптимизировано 37 uSec (1431.5 тактов за шаг) при m=1
    // 3.30546875 uSec  при m = 32 (132.21875 тактов на шаг)

    // если CacheCmdCounter < Count => FreeData = CacheCmdCounter
    // если CacheCmdCounter >= Count => FreeData = Count

    if(rr->CacheCmdCounter < count) {
        FreeData = (WORD)rr->CacheCmdCounter;        
    } else {
        FreeData = count;
    }
       
    for(i = 0; i < FreeData;){
        m1 = 3;
        while(rr->Interval < rr->OneStepCalcTime >> m1){
            m1++;
        }
        m = 1 << m1; //
        { // вычисляем по нескольку шагов- времени мало(#me: времени всегда мало)
            // если m + i  <  FreeData => m = m
            // если m + i  >= FreeData => m = FreeData - i;
            if(m + i > FreeData){
                m = FreeData - i;
            }             
            switch(rr->CacheState){
                case ST_ACCELERATE:
                    rr->XaccBeg += m;
                    dT = GetInterval(rr->T1, rr->XaccBeg, rr->dx, rr->a, rr->d);
                    rr->T1 += dT;
                    break;
                case ST_DECELERATE:
                    rr->XaccBeg -= m;
                    dT = GetInterval(rr->T1, rr->XaccBeg, rr->dx, rr->a, rr->d);
                    rr->T1 -= dT;
                    break;
                case ST_RUN:
                    dT = (DWORD)(m * dx_div_Ts / rr->CacheSpeed);
                    break;
                default:
                    return 0;
                    break;
            }            
            rr->Interval =(dT / m);
            Corr = (dT % m);
            for( j = 0; j < m; j++) {
                rr->T.Val += rr->Interval;
                if(Corr>0) {
                    rr->T.Val += 1;
                    Corr--;
                }
                buf[i+j].R  = (WORD)(rr->T.Val >> TimerConfig[rr->TmrId].SHRcount);
                buf[i+j].RS = (WORD)((rr->T.Val + rr->Interval / 2) >> TimerConfig[rr->TmrId].SHRcount);
            }
        }
        rr->CacheCmdCounter -= m;
        i += m;
    } 
    {   // вычисление скорости        
        rr->CacheSpeed = GetSpeed(rr->T1 * rr->TimerStep, rr->K, rr->B);
    }
    return FreeData;
}
//------------------------------------------------------------------------------------------------
int RunControl(void * _This, DMA_ID id)
//------------------------------------------------------------------------------------------------
{
    RR * rr = (RR*)_This;
    if(DMAGetPPState(id) != 0) {
        return 1; // полушаг
    }
    if(rr->RunQueue[rr->RunBeginQueue].RunStep == 0){
        //NextRunCmd(rr); // очистка и переключение команды
        SetDirection( rr->Index, rr->RunQueue[rr->RunBeginQueue].Direction);
        OCSetTmr((OC_ID)rr->Index, (OC_TMR_SELECT)rr->RunQueue[rr->RunBeginQueue].Timer);
    } 
    else {
        rr->RunQueue[rr->RunBeginQueue].RunStep--;
    }
    
    return 0;
}



/*
перечень команд
    1 остановка
    2 ускорение
    3 торможение
    4 движение
    5 медленное ускорение 
    6 медленное торможение
    7 медленное движение
    8 медленное движение без ограничения (до максимального значения)
    9 экстренная остановка
   10 смена шага
   11 смена направления (только при полной остановке)
    
требование к диспетчеру команд:
    1. очередная смена команды
    2. прерывание команд с низким приоритетом командами с высоким приоритетом
    3. принудительная смена команды на торможение
    4. принудительная остановка
    5. при любом прерывании команд очередь очищается
    6. все команды должны быть совместимы. т.е. незавершенную команду можно прервать любой другой, подходящей по смыслу.
        например: ST_ACCELERATE можно прервать ST_RUN или ST_DECELERATE и все необходимые переменные будут иметь правильные значения
    7. при невысоких скоростях уменьшить размер буфера (при переключении на tmr3 сделать размер буфера = 16 циклов)
    8. при высоких скоростях установить большой размер буфера (при переключении на tmr2 сделать размер буфера = 128 циклов)

приоритет команд:
А. низкий приоритет    (прерывается командами с высоким и обычным приоритетом)
    1. движение без ограничения (до максимального значения)

Б. обычный приоритет   (прерывается только командами с высоким приоритетом)
    1. ускорение
    2. торможение
    3. движение
    4. остановка

    5. смена направления
    6. смена таймера

В. высокий приоритет
    1. экстренная остановка
    2. смена шага
    

1.  Сканируем очередь 
2.  Если есть команда с высоким приоритетом, начинаем выполнять её. 
    при этом удаляем все предыдущие команды из очереди.
3.  Выполняем команды с одинаковым приоритетом в порядке очереди.

выполнение команды
1.  выполняем предварительные вычисления (CalculateBreakParam)
2.  
//------------------------------------------------------------------------------------------------
*/
// проверить параметры 
// поставить команду в очередь
// возврат: 
// 0: команда успешно поставлена в очередь
// 1: очередь переполнена. повторить позже
// 2: некорректные параметры команды
// 3: неизвестная команда
//------------------------------------------------------------------------------------------------
int PushCmd(RR* rr, GD_STATE cmd, STATE_VALUE Value)
//------------------------------------------------------------------------------------------------
{
    if(rr->CmdCount < CQ_SIZE){
        switch(cmd){
            case ST_STOP:
                if(Value.Null == NULL){
                    rr->CmdQueue[rr->CmdEndQueue].State = cmd;
                    rr->CmdQueue[rr->CmdEndQueue].Value.Null = NULL;
                } else return 2;
                break;
            case ST_ACCELERATE:
            case ST_DECELERATE:
                if((Value.Speed >= -rr->MaxSpeed )&&(Value.Speed <= rr->MaxSpeed)){
                    rr->CmdQueue[rr->CmdEndQueue].State = cmd;
                    rr->CmdQueue[rr->CmdEndQueue].Value = Value;
                } else return 2;
                break;
            case ST_RUN: // TODO: функцию сделать которая выяснит предельные углы
                if((Value.Angle >= rr->MinAngle )&&(Value.Angle <= rr->MaxAngle)){
                    rr->CmdQueue[rr->CmdEndQueue].State = cmd;
                    rr->CmdQueue[rr->CmdEndQueue].Value = Value;
                } else return 2;
                break;
            case ST_SET_DIRECTION:
                if((Value.Dir>=-1)&&(Value.Dir<=1)){
                    rr->CmdQueue[rr->CmdEndQueue].State = cmd;
                    rr->CmdQueue[rr->CmdEndQueue].Value = Value;
                } else return 2;
                break;
            case ST_SET_TIMER:
                if((Value.Timer == 2 )||(Value.Timer == 3)){
                    rr->CmdQueue[rr->CmdEndQueue].State = cmd;
                    rr->CmdQueue[rr->CmdEndQueue].Value = Value;
                } else return 2;
                break;
            case ST_SET_STEP:
                if((Value.Step == 1 )||(Value.Step == 2)||(Value.Step == 4 )||(Value.Step == 8)||(Value.Step == 16 )||(Value.Step == 32)){
                    rr->CmdQueue[rr->CmdEndQueue].State = cmd;
                    rr->CmdQueue[rr->CmdEndQueue].Value = Value;
                } else return 2;
                break;
            case ST_EMERGENCY_STOP:
                if(Value.Null == NULL){
                    rr->CmdQueue[rr->CmdEndQueue].State = cmd;
                    rr->CmdQueue[rr->CmdEndQueue].Value.Null = NULL;
                } else return 2;
                break;
            default:
                return 3;
                break;
        }
    
        rr->CmdCount++;
        rr->CmdEndQueue++;
        if(rr->CmdEndQueue >= CQ_SIZE){
            rr->CmdEndQueue -= CQ_SIZE;
        }
    } else return 1;
    return 0;
}

BOOL NeedBreakCmd(GD_STATE CurrentCmd, GD_STATE NextCmd){
    switch(CurrentCmd){
        case ST_CONTINOUS:
            if(NextCmd != ST_CONTINOUS) return true;
            else return false;
        break;
        case ST_STOP:
        case ST_ACCELERATE:
        case ST_DECELERATE:
        case ST_RUN:
            if(NextCmd == ST_EMERGENCY_STOP) return true;
            else return false;
        break;
        //case 
    }
    return false;
}

int ProcessCmd(RR * rr)
{
    /*
    STATE_VALUE Value;   
    double Vend = 10.0;
    double Vbeg = 0.0;
    double DeltaX = 5.0;
    double DeltaT = 0;
    LONG XBreak = 0;
    CalculateBreakParam(rr, ST_ACCELERATE, Vbeg, &Vend, &DeltaX, &DeltaT, &XBreak);
    Vbeg = 10.0;
    Vend = 0.0;
    DeltaX = 5.0;
    CalculateBreakParam(rr, ST_DECELERATE, Vbeg, &Vend, &DeltaX, &DeltaT, &XBreak);
    Value.Speed = 10.0 * Grad_to_Rad;
    PushCmd(rr, ST_ACCELERATE, Value);
    Value.Speed = 1.0 * Grad_to_Rad;
    PushCmd(rr, ST_DECELERATE, Value);
    Value.Angle = 52.0 * Grad_to_Rad;
    PushCmd(rr, ST_RUN, Value);
    Value.Timer = 2;
    PushCmd(rr, ST_SET_TIMER, Value);
    Value.Dir = 1;
    PushCmd(rr, ST_SET_DIRECTION, Value);
    Value.Step = 8;
    PushCmd(rr, ST_SET_STEP, Value);
    Value.Null = NULL;
    PushCmd(rr, ST_EMERGENCY_STOP, Value);
    PushCmd(rr, ST_STOP, Value);   
    Value.Speed = 10.0 * Grad_to_Rad;
    PushCmd(rr, ST_ACCELERATE, Value);
    Value.Speed = 1.0 * Grad_to_Rad;
    PushCmd(rr, ST_DECELERATE, Value);
    Value.Angle = 52.0 * Grad_to_Rad;
    PushCmd(rr, ST_RUN, Value);
    RunControl((void*)rr, DMA0);
    //TimerSetValue(T1,0,0);
    */
    PushRunCmd(rr, ST_ACCELERATE, 0, TIMER2, 256);
    PushRunCmd(rr, ST_EMERGENCY_STOP, 0, TIMER2, 0);
    Run_Cmd_Queue CurrCmd = PopRunCmd(rr);
    if(CurrCmd.State == ST_ACCELERATE) return 1;
    return 0;
}
//
//
//      BeginQueue
//      |      EndQueue
//      |      |
//      v      v
//  ----*******------
// |                 |
//  -----------------
//------------------------------------------------------------------------------------------------
int PushRunCmd(RR * rr, GD_STATE State, BYTE Direction, TIMERS_ID Timer, DWORD RunStep)
//------------------------------------------------------------------------------------------------
{
    if(rr->RunCount < CQ_SIZE){
        rr->RunQueue[rr->RunEndQueue].State = State;
        rr->RunQueue[rr->RunEndQueue].Direction = Direction;
        rr->RunQueue[rr->RunEndQueue].Timer = Timer;
        rr->RunQueue[rr->RunEndQueue].RunStep = RunStep;
        rr->RunEndQueue++;
        if (rr->RunEndQueue >= CQ_SIZE){
            rr->RunEndQueue -= CQ_SIZE;
        }
        rr->RunCount++;
        return 0;
    } else {
        return 1;
    }
}
//------------------------------------------------------------------------------------------------
Run_Cmd_Queue PopRunCmd(RR * rr)
//------------------------------------------------------------------------------------------------
{   
    if(rr->RunCount>0){
        BYTE NewBeginQueue = SearchHighPriorityRunCmd(rr);
        Run_Cmd_Queue CurrCmd = rr->RunQueue[NewBeginQueue];
        while(rr->RunBeginQueue != NewBeginQueue){
            rr->RunQueue[rr->RunBeginQueue].Direction = 0;
            rr->RunQueue[rr->RunBeginQueue].RunStep = 0;
            rr->RunQueue[rr->RunBeginQueue].State = ST_STOP;
            rr->RunQueue[rr->RunBeginQueue].Timer = TIMER2;
            rr->RunBeginQueue++;
            if (rr->RunBeginQueue >= CQ_SIZE){
                rr->RunBeginQueue -= CQ_SIZE;
            }
            rr->RunCount--;
        }
        return CurrCmd;
    }
}
//------------------------------------------------------------------------------------------------
int SearchHighPriorityRunCmd(RR * rr)
//------------------------------------------------------------------------------------------------
// Ищем в очереди исполнения высоко приоритетные задачи
{
    GD_STATE CurrentState = rr->RunQueue[rr->RunBeginQueue].State;
    BYTE QueuePos = rr->RunBeginQueue;
    BYTE i = 0;
    while(i < rr->RunCount){
        QueuePos++;
        if (QueuePos >= CQ_SIZE){
            QueuePos -= CQ_SIZE;
        }
        i++;
        switch(rr->RunQueue[QueuePos].State){
            case ST_CONTINOUS:       // сопровождение
            case ST_STOP:            // остановлен
            break;
            case ST_ACCELERATE:      // разгоняется
            case ST_RUN:             // движется с постоянной скоростью
            case ST_DECELERATE:      // тормозит
            case ST_SET_TIMER:       // установка таймера
            case ST_SET_DIRECTION:   // установка направления вращения
                if((CurrentState == ST_CONTINOUS)||(CurrentState == ST_STOP)){
                    return QueuePos;
                }
            break;
            case ST_SET_STEP:        // установка количества микрошагов
            case ST_EMERGENCY_STOP:  // аварийная остановка
                if(CurrentState != ST_EMERGENCY_STOP){
                    return QueuePos;
                }
            break;
            default:
            break;
        }
    }
    return rr->RunBeginQueue;
}


//------------------------------------------------------------------------------------------------
int CacheNextCmd(RR * rr)
//------------------------------------------------------------------------------------------------
{
    double dx_div_Ts;
    if(rr->CmdCount > 0){
        //double D;
        //double X;
        rr->CacheState = rr->CmdQueue[rr->CmdBeginQueue].State;
        //rr->Vend  = rr->CmdQueue[rr->CmdBeginQueue].Vend;
        //rr->deltaX  = rr->CmdQueue[rr->CmdBeginQueue].deltaX;
        //rr->CacheDir = rr->CmdQueue[rr->CmdBeginQueue].Direction;
        //rr->CacheCmdCounter = rr->CmdQueue[rr->CmdBeginQueue].RunStep;
        switch(rr->CacheState){
            case ST_ACCELERATE:
            case ST_DECELERATE:
                rr->Interval = GetInterval(rr->T1, rr->XaccBeg, rr->dx, rr->a, rr->d);
                rr->T1 += rr->Interval;
                break;
            case ST_RUN: //TODO:
                dx_div_Ts = rr->dx / rr->TimerStep;
                rr->Interval = (DWORD)(dx_div_Ts / rr->CacheSpeed);
                // вычисление скорости        
                rr->CacheSpeed = GetSpeed(rr->T1 * rr->TimerStep, rr->K, rr->B);
                break;
            default:;
        }
        rr->CmdCount--;
        rr->CmdQueue[rr->CmdBeginQueue].State = ST_STOP;
        //rr->CmdQueue[rr->CmdBeginQueue].Vend = 0.0;
        //rr->CmdQueue[rr->CmdBeginQueue].deltaX = 0.0;
        //rr->CmdQueue[rr->CmdBeginQueue].Direction = 1;
        //rr->CmdQueue[rr->CmdBeginQueue].RunStep = 0;
        rr->CmdBeginQueue++;
        if(rr->CmdBeginQueue >= CQ_SIZE)rr->CmdBeginQueue -= CQ_SIZE;
    } else {
        rr->CacheState = ST_STOP;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
void GetAlgParams(double V, double K, double B, double dx, double * T, LONG * X)
//------------------------------------------------------------------------------------------------
{
    double VKpB = 0.0;
    double D = 0.0;

    VKpB = V * K + B;
    D = B * VKpB;
    (*T) = ((VKpB - sqrt(D))/(K * VKpB));
    (*X) = (LONG)((B * (*T) * (*T)) / ((1 - K * (*T))*dx));
}
//------------------------------------------------------------------------------------------------
// используем
// rr->K,// rr->B// rr->dx// rr->a// rr->d
//------------------------------------------------------------------------------------------------
int CalculateBreakParam(RR * rr, GD_STATE State, double Vbeg, double * Vend, double * deltaX, double * deltaT, LONG * Xbreak)
//------------------------------------------------------------------------------------------------
{
    double dTb = 0.0;
    double dTe = 0.0;
    LONG Xe;
    LONG Xb;
    LONG dX;
    LONG dX2;
    DWORD XX = 0;
    double T2 = 0.0;

    switch (State) {
        case ST_ACCELERATE:
        case ST_DECELERATE:
            if(Vbeg!=0.0){
                GetAlgParams(Vbeg, rr->K, rr->B, rr->dx, &dTb, &Xb);
            } else Xb = 0;
            if(*Vend != 0.0){
                GetAlgParams(*Vend, rr->K, rr->B, rr->dx, &dTe, &Xe);
            } else Xe = 0;            
            dX = abs((Xe - Xb));
            dX2 = (LONG)(abs((*deltaX)/rr->dx));

            if (dX2 < dX) {
                // если координата перемещения меньше, чем координата при заданной скорости
                (*Xbreak) = dX2;
                if(State != ST_DECELERATE){
                    XX = Xb + dX2;
                } else {
                    XX = Xb - dX2;
                }
                T2 = (double)(GetInterval(0, XX, rr->dx, rr->a, rr->d) * rr->TimerStep);
                (*Vend) = GetSpeed(T2 , rr->K, rr->B);     

                if (deltaT != NULL) {
                    (*deltaT) = abs((T2 - dTb));
                }
            } else {
                (*Xbreak) = dX;
                (*deltaX) = dX * rr->dx;
                if (deltaT != NULL) {
                    (*deltaT) = abs((dTe - dTb));
                }
            }
            break;
        case ST_RUN: 
            (*Xbreak) = (LONG)(abs((*deltaX)) / rr->dx);
            (*Vend) = Vbeg;
            if (deltaT != NULL) {
                *deltaT = abs((*deltaX))/Vbeg;
            }
            break;

        case ST_STOP:
            (*Xbreak) = 0;
            (*Vend) = 0;
            (*Vend) = Vbeg;
            if (deltaT != NULL) {
                *deltaT = 0.0;
            }
        default:
            break;
    }
    return 0;
}

//------------------------------------------------------------------------------------------------
//int GoToCmd(RR * rr, double VTarget, double XTarget, DWORD Tick)
//------------------------------------------------------------------------------------------------
/*{
    BYTE Direction = 1;
    BYTE TargetDirection = 1;
    LONG Xbreak;
    double Xa = 180.0 * Grad_to_Rad;
    double Ta = 0.0;
    double Td = 0.0;
    double Xd = 180.0 * Grad_to_Rad;
    double T0 = 0.0;
    double Trun = 0.0;
    double VendA = 0.0;
    double VendD = 0.0;	
    // 1. больше, чем разгон до максимума + торможение до нужной скорости
    //    => вычисляем сумму разгон+ торможение + движение по линейному закону
    // 2. меньше
    //    => вычисляем максимальную скорость до которой успеем разогнаться
    //
    // TODO: если двигаемся, нужно остановиться
    // TODO: выяснить направление движения
    // TODO: определить модель привода ( догоняем цель или ждем )
    VendD = VTarget;
    / *	
    if(rr->LastCmdX == XTarget) return 0;
    //BreakCurrentCmd(rr);
    if(rr->LastCmdX < XTarget) {
        Direction = 1;
        Xa = (XTarget - rr->LastCmdX)/2.0;
    } else {
        Direction = 0;
        Xa = (rr->LastCmdX - XTarget)/2.0;
    }
    //if(rr->LastCmdX < XTarget)
    {

        Xd = Xa;
        VendA = rr->VMax;
        CalculateBreakParam(rr->K, rr->B, rr->dx, ST_ACCELERATE, Direction, rr->LastCmdV, &VendA, &Xa, &Ta, &Xbreak);
        
        CalculateBreakParam(rr->K, rr->B, rr->dx, ST_DECELERATE, Direction, VendA, &VendD, &Xd, &Td, &Xbreak);
        //T0 = ((double)(Tick - TickGet()))* 0.00000025;
        if(Direction){
            Trun = -(XTarget - rr->LastCmdX - Xa - Xd + VTarget * (T0 + Ta + Td) ) /(VTarget - rr->VMax);
        } else {
            Trun = -(XTarget - rr->LastCmdX + Xa + Xd + VTarget * (T0 + Ta + Td) ) /(VTarget + rr->VMax);
        }
        //if(Trun < 0.0) Trun = -Trun;
        {
            double Xrun = rr->VMax * Trun;            
            PushCmdToQueue(rr, ST_ACCELERATE, VendA, PI , Direction);
            PushCmdToQueue(rr, ST_RUN, VendA,  Xrun, Direction);
            PushCmdToQueue(rr, ST_DECELERATE, 0.0, PI, Direction);
            if(VTarget != 0.0){
	            if(VTarget>=0){
					TargetDirection = 1;		
				} else {
					TargetDirection = 0;
					VTarget = -VTarget;
				}
	            PushCmdToQueue(rr, ST_ACCELERATE, VTarget, PI , TargetDirection);
                PushCmdToQueue(rr, ST_RUN, VTarget,  PI, TargetDirection);
                PushCmdToQueue(rr, ST_DECELERATE, 0.0, PI, TargetDirection);
            }
            PushCmdToQueue(rr, ST_STOP, 0.0, 0.0, Direction);
        }
    }   * / 
    return 0;
}*/
//------------------------------------------------------------------------------------------------
// информационные функции для Web интерфейса
//------------------------------------------------------------------------------------------------
double GetAngle(WORD n)
//------------------------------------------------------------------------------------------------
{
    double X = 0.0;
    switch(n){
        case 0: X = rr1.XPosition * rr1.dx;
            if(X > PI2) X -= PI2;
            if(X < 0.0) X += PI2;
        break;
        case 1: X = rr2.XPosition * rr2.dx;
        break;
        case 2: X = rr3.XPosition * rr3.dx;
        break;
    }
    return X;
}
//------------------------------------------------------------------------------------------------
int GetStatus(WORD n){    
//------------------------------------------------------------------------------------------------
    switch(n){
        case 0: return (rr1.RunState == ST_STOP)?0:1;            
        break;
        case 1: return (rr2.RunState == ST_STOP)?0:1;
        break;
        case 2: return (rr3.RunState == ST_STOP)?0:1;
        break;
    }
    return 3;
}

//------------------------------------------------------------------------------------------------
// Вычисление звездного времени
//------------------------------------------------------------------------------------------------
double GM_Sidereal_Time (double jd) {
//------------------------------------------------------------------------------------------------
    double t_eph = 0.0;
    double ut = 0.0;
    double MJD0 = 0.0;
    double MJD = 0.0;

    MJD = jd - 2400000.5;
    MJD0 = floor(MJD);
    ut = (MJD - MJD0)*24.0;
    t_eph  = (MJD0-51544.5)/36525.0;
    return  6.697374558 + 1.0027379093*ut + (8640184.812866 + (0.093104 - 0.0000062*t_eph)*t_eph)*t_eph/3600.0;
}

//------------------------------------------------------------------------------------------------
double LM_Sidereal_Time (double jd, double longitude) {
//------------------------------------------------------------------------------------------------
    double GMST = GM_Sidereal_Time(jd);
    double LMST =  24.0*frac((GMST + longitude/15.0)/24.0);
    return LMST;
}
//------------------------------------------------------------------------------------------------
double frac(double X) {
//------------------------------------------------------------------------------------------------
    X = X - floor(X);
    if (X<0) X = X + 1.0;
    return X;
}
//------------------------------------------------------------------------------------------------
// день d, месяц m, год y, время в часах  
//------------------------------------------------------------------------------------------------
double JulDay (BYTE D, BYTE M,WORD Y,BYTE h, BYTE m, BYTE s){
//------------------------------------------------------------------------------------------------
    double JD = 0.0;
    double UT = h + m/60 + s/3600;
    if (Y < 1900) Y = Y + 1900;
    if (M <= 2) { M = M + 12; Y = Y - 1;}
    JD =  floor(365.25 * (Y + 4716)) + floor(30.6001 * (M + 1)) + D - 13 - 1524.5 + UT/24.0;
    return JD;
}
/*
int GDateToJD(DateTime GDate, int * JDN, double * JD)
{
    double  a;
    double  y;
    double  m;

    a = (14-GDate.Month)/12;
    y = GDate.Year + 4800 - a;
    m = GDate.Month + 12 * a - 3;
    (*JDN) = (int)(GDate.Day + (int)((153*m + 2)/5) + (int)(365 * y) + (int)(y / 4)-(int)(y / 100)+(int)(y / 400) - 32045);
    (*JD) = (double)(*JDN) + (double)(((double)GDate.Hour - 12.0)/24.0) + (double)((double)GDate.Min/1440.0) + (double)((double)GDate.Sec/86400.0);
    return 0;
}

int JDToGDate(double JD, DateTime * GDate )
{
    int a;
    double b;
    double c;
    double d;
    double e;
    double m;
    double s;

    a = (int)(JD) + 32044;
    b = (4.0*a + 3.0)/146097.0;
    c = a - 146097.0*b/4.0;
    d = (4.0*c+3.0)/1461.0;
    e = c - 1461.0 * d / 4.0;
    m = (5.0* e + 2.0)/153.0;
    s = JD - (int)(JD);

    GDate->Day = (BYTE)(e - (int)((153.0 * m + 2.0)/5.0) + 1);
    GDate->Month = (BYTE)(m + 3 - 12 * (int)(m / 10.0));
    GDate->Year = (WORD)(100 * b + d - 4800 + (m/10));
    GDate->Hour = (BYTE)(s * 24 + 12);
    GDate->Min = (BYTE)((s * 24 + 12 - GDate->Hour) * 60);
    GDate->Sec = (BYTE)(((s * 24 + 12 - GDate->Hour) * 60 - GDate->Min) * 60);
    return 0;
}
*/
