// #ifndef __OCTIMER_C
// #define __OCTIMER_C
// #endif

#include "stdafx.h"

#ifdef __C30__
#   include "TCPIP Stack/TCPIP.h"
#   include "math.h"
#   include "device_control.h"
#else
#   include <math.h>
#endif

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
//__attribute__((far)) APP_CONFIG AppConfig;
#else
RR rr1;
RR rr2;
RR rr3;
DWORD TickGet()
{
    return 0;
}
#endif
int InitRR(RR * rr);
int CacheNextCmd(RR * rr);
DWORD GetBigTmrValue(BYTE id);
int SetDirection(BYTE oc, BYTE Dir);
int CalculateBreakParam(double K, double B, double dx, GD_STATE State, int Direction, double Vbeg, double * Vend, double * deltaX, double * deltaT, LONG * Xbreak);
void CalculateParams(RR * rr);
BOOL IsDisableOC(BYTE oc);
DWORD GetInterval(DWORD T, double Xb, double dx, double a, double d);
WORD Acceleration(RR * rr, WORD* buf, WORD count);

double frac(double X);
double GM_Sidereal_Time (double jd);

#ifdef __C30__


#else
    WORD TMR2 = 0;
    WORD TMR3 = 0;
    WORD OC1R = 0;
#endif

int OCSetup(void)
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

                PORT1_ENABLE       = 0;// выход ENABLE
                PORT1_DIR           = 0;// выход DIR
                PORT1_STEP           = 0;// выход STEP

                PORT1_ENABLE_Tris = 0;// выход ENABLE
                PORT1_DIR_Tris    = 0;// выход DIR
                PORT1_STEP_Tris   = 0;// выход STEP
        }
        {
                PORT2_NULL_Tris   = 1; // вход NULL
                PORT2_POS_Tris    = 1; // вход POS
                PORT2_POS2_Tris   = 1; // вход POS2

                PORT2_ENABLE       = 0;// выход ENABLE
                PORT2_DIR           = 0;// выход DIR
                PORT2_STEP           = 0;// выход STEP

                PORT2_ENABLE_Tris = 0;// выход ENABLE
                PORT2_DIR_Tris    = 0;// выход DIR
                PORT2_STEP_Tris   = 0;// выход STEP
        }
        {
                PORT3_NULL_Tris   = 1; // вход NULL
                PORT3_POS_Tris    = 1; // вход POS
                PORT3_POS2_Tris   = 1; // вход POS2

                PORT3_ENABLE       = 0;// выход ENABLE
                PORT3_DIR           = 0;// выход DIR
                PORT3_STEP           = 0;// выход STEP

                PORT3_ENABLE_Tris = 0;// выход ENABLE
                PORT3_DIR_Tris    = 0;// выход DIR
                PORT3_STEP_Tris   = 0;// выход STEP
        }
#endif //#ifdef __C30__

        //TmrInit(2);
        rr1.Index = 0;
        rr1.TmrId = 2;
        InitRR(&rr1);
        rr2.Index = 1;
        rr2.TmrId = 2;
        InitRR(&rr2);
        rr3.Index = 2;
        rr3.TmrId = 2;
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
        TimerInit(T3, CLOCK_SOURCE_INTERNAL, GATED_DISABLE, PRE_1_256, IDLE_DISABLE, BIT_16, SYNC_DISABLE);
        TimerSetValue(T3, 0, 0xFFFF);
        OCSetCallback(T3, NULL);
        TimerSetInt(T3, 5, FALSE);

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

int InitRR(RR * rr)
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
    rr->NextCacheCmd = 0;
    rr->NextWriteCmd = 0;
    rr->Interval = 32768;
    //rr->LastCmdV = 0.0;
    //rr->LastCmdX = 0.0;
    
    CalculateParams(rr);

    return 0;
}
void CalculateParams(RR * rr)
{
    double I;
    I = ((rr->Mass*rr->Radius*rr->Radius/4) + (rr->Mass*rr->Length*rr->Length/12))/rr->Reduction;
#ifdef __C30__
    if(AppConfig.RRConfig[rr->Index].Control){
        rr->K = (AppConfig.RRConfig[rr->Index].K)/I;
        rr->B = AppConfig.RRConfig[rr->Index].B / I;
    else
#else
    {
        rr->K = (MY_DEFAULT_RR_PARA_K)/I;
        rr->B = MY_DEFAULT_RR_PARA_B / I;
    }
#endif
    rr->dx = 2.0 * PI /(rr->Reduction * rr->StepPerTurn * rr->uStepPerStep); // шаг перемещения в радианах
    rr->d = (-(rr->K)/(2.0 * rr->B * rr->TimerStep));
    rr->a = (4.0 * rr->B/(rr->K * rr->K));
    rr->e = (DWORD)(0.000160 / rr->TimerStep); //120us
}

DWORD GetInterval(DWORD T, double Xb, double dx, double a, double d)
{
    double D;
    double X = Xb * dx;
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
/*
Используем:
rr->CacheCmdCounter             счетчик оставшихся шагов в команде
rr->dx                          ширина шага
rr->a                           вспомогательная констата
rr->d
rr->e                           интервал в шагах таймера времени, необходимого для однократного вычисления
rr->TimerStep                   интервал таймера в секундах
rr->T1                          предыдущее значение функции ускорения
rr->XaccBeg                     координата функции ускорения
rr->Interval                    интервал
rr->T                           счетчик интервалов
 **/
WORD CalculateMove(RR * rr, BUF_TYPE* buf, WORD count)
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
        while(rr->Interval < rr->e >> m1){
            m1++;
        }
        m = 1 << (m1);        
        if(m == 1){ // вычисляем по одному шагу- времени много            
            switch(rr->CacheState){
                case ST_ACCELERATE:
                    rr->XaccBeg++;
                    rr->Interval = GetInterval(rr->T1, rr->XaccBeg, rr->dx, rr->a, rr->d);
                    rr->T1 += rr->Interval;
                    break;
                case ST_DECELERATE:
                    rr->XaccBeg--;
                    rr->Interval = GetInterval(rr->T1, rr->XaccBeg, rr->dx, rr->a, rr->d);
                    rr->T1 -= rr->Interval;
                    break;
                case ST_RUN:
                    rr->Interval = (DWORD)(dx_div_Ts / rr->CacheSpeed);
                    break;
                default:
                    return 0;
                    break;
            }
            rr->T.Val += rr->Interval;
            buf[i].R = rr->T.word.LW;
            buf[i].RS = (WORD)((rr->T.Val + rr->Interval / 2) & 0xFFFF);
            
        } else { // вычисляем по нескольку шагов- времени мало
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
                buf[i+j].R  = rr->T.word.LW;
                buf[i+j].RS = (WORD)((rr->T.Val + rr->Interval / 2) & 0xFFFF);
            }            
            
        }
        rr->CacheCmdCounter -= m;
        i += m;
    } 
    {   // вычисление скорости
        double OmKT = (1 - rr->K * rr->T1);
        rr->CacheSpeed = (rr->B * rr->T1*( 1.0 + OmKT))/(OmKT * OmKT); // V = BT/(1-KT)
    }
    return FreeData;
}
// Функция для вызова через DMASetCallback 
int Control(void * _This, WORD* Buf, WORD BufSize)
{
    RR * rr = (RR*)_This;
    WORD i = 0;
    WORD j = 0;
    for(i = 0; (i < BufSize)&&(rr->CacheState != ST_STOP);){
        if(j == BufSize) j = 0;
        j += CalculateMove(rr, (BUF_TYPE*)&Buf[j], (BufSize - j)/2);

        if(rr->CacheCmdCounter <= 0){
            CacheNextCmd(rr);
        }        
    }
    if(rr->CacheState == ST_STOP)
        return 1;
    return 0;
}




DWORD GetBigTmrValue(BYTE id)
{
    switch (id){
    case 2:
        Timer2Big.word.LW = TMR2;
        return Timer2Big.Val;
    case 3:
        Timer3Big.word.LW = TMR3;
        return Timer3Big.Val;
    default:
        return 0;
    }
}

BOOL IsDisableOC(BYTE oc)
{
#ifdef __C30__
    switch (oc){
    case 0:
        if(OC1CONbits.OCM == 0b000) return 1;
        break;
    case 1:
        if(OC2CONbits.OCM == 0b000) return 1;
        break;
    case 2:
        if(OC3CONbits.OCM == 0b000) return 1;
        break;
    case 3:
        if(OC4CONbits.OCM == 0b000) return 1;
        break;
    default :
    return 0;
    }
    return 0;
#else
    return oc;
#endif

}

int SetDirection(BYTE oc, BYTE Dir)
{
#ifdef __C30__
    switch (oc){
    case 0:
        PORT1_DIR = Dir;
        break;
    case 1:
        PORT2_DIR = Dir;
        break;
    case 2:
        PORT3_DIR = Dir;
        break;
    case 3:
        PORT4_DIR = Dir;
        break;
    default :
    return 0;
    }
    return 0;
#else
    return oc + Dir;
#endif
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

*/
// проверить параметры 
// поставить команду в очередь
// возврат: 
// 0: команда успешно поставлена в очередь
// 1: очередь переполнена. повторить позже
// 2: некорректные параметры команды
// 3: неизвестная команда
int PushCmd(RR* rr, GD_STATE cmd, STATE_VALUE Value)
{
    if(rr->CmdCount < CQ_SIZE){
        switch(cmd){
            case ST_STOP:
                if(Value.Null == NULL){
                    rr->CmdQueue[rr->NextWriteCmd].State = cmd;
                    rr->CmdQueue[rr->NextWriteCmd].Value.Null = NULL;
                } else return 2;
                break;
            case ST_ACCELERATE:
            case ST_DECELERATE:
                if((Value.Speed >= -rr->MaxSpeed )&&(Value.Speed <= rr->MaxSpeed)){
                    rr->CmdQueue[rr->NextWriteCmd].State = cmd;
                    rr->CmdQueue[rr->NextWriteCmd].Value = Value;
                } else return 2;
                break;
            case ST_RUN: // TODO: функцию сделать которая выяснит предельные углы
                if((Value.Angle >= rr->MinAngle )&&(Value.Angle <= rr->MaxAngle)){
                    rr->CmdQueue[rr->NextWriteCmd].State = cmd;
                    rr->CmdQueue[rr->NextWriteCmd].Value = Value;
                } else return 2;
                break;
            case ST_SET_DIRECTION:
                if((Value.Dir>=-1)&&(Value.Dir<=1)){
                    rr->CmdQueue[rr->NextWriteCmd].State = cmd;
                    rr->CmdQueue[rr->NextWriteCmd].Value = Value;
                } else return 2;
                break;
            case ST_SET_TIMER:
                if((Value.Timer == 2 )||(Value.Timer == 3)){
                    rr->CmdQueue[rr->NextWriteCmd].State = cmd;
                    rr->CmdQueue[rr->NextWriteCmd].Value = Value;
                } else return 2;
                break;
            case ST_SET_STEP:
                if((Value.Step == 1 )||(Value.Step == 2)||(Value.Step == 4 )||(Value.Step == 8)||(Value.Step == 16 )||(Value.Step == 32)){
                    rr->CmdQueue[rr->NextWriteCmd].State = cmd;
                    rr->CmdQueue[rr->NextWriteCmd].Value = Value;
                } else return 2;
                break;
            case ST_EMERGENCY_STOP:
                if(Value.Null == NULL){
                    rr->CmdQueue[rr->NextWriteCmd].State = cmd;
                    rr->CmdQueue[rr->NextWriteCmd].Value.Null = NULL;
                } else return 2;
                break;
            default:
                return 3;
                break;
        }
    
        rr->CmdCount++;
        rr->NextWriteCmd++;
        if(rr->NextWriteCmd >= CQ_SIZE){
            rr->NextWriteCmd -= CQ_SIZE;
        }
    } else return 1;
    return 0;
}

int ProcessCmd(RR * rr)
{
    STATE_VALUE Value;    
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
    return 0;
}













int CacheNextCmd(RR * rr)
{
    if(rr->CmdCount > 0){
        //double D;
        //double X;
        rr->CacheState = rr->CmdQueue[rr->NextCacheCmd].State;
        //rr->Vend  = rr->CmdQueue[rr->NextCacheCmd].Vend;
        //rr->deltaX  = rr->CmdQueue[rr->NextCacheCmd].deltaX;
        //rr->CacheDir = rr->CmdQueue[rr->NextCacheCmd].Direction;
        //rr->CacheCmdCounter = rr->CmdQueue[rr->NextCacheCmd].RunStep;
        switch(rr->CacheState){
            case ST_ACCELERATE:
            case ST_DECELERATE:/*
                if((rr->XaccBeg > 0)){
                    X = rr->XaccBeg * rr->dx;
                    D = X *(X + rr->a);
                    if(D >= 0.0){
                        rr->T1 = (DWORD)((X + sqrt(D))*rr->d);
                    }
                };*/
                break;
            case ST_RUN:
                rr->XCachePos = 0;
                rr->T1 = 0;
                break;
            default:;
        }
        rr->CmdCount--;
        rr->CmdQueue[rr->NextCacheCmd].State = ST_STOP;
        //rr->CmdQueue[rr->NextCacheCmd].Vend = 0.0;
        //rr->CmdQueue[rr->NextCacheCmd].deltaX = 0.0;
        //rr->CmdQueue[rr->NextCacheCmd].Direction = 1;
        //rr->CmdQueue[rr->NextCacheCmd].RunStep = 0;
        rr->NextCacheCmd++;
        if(rr->NextCacheCmd >= CQ_SIZE)rr->NextCacheCmd -= CQ_SIZE;
    } else {
        rr->CacheState = ST_STOP;
    }
    return 0;
}

int PushCmdToQueue(RR * rr, GD_STATE State, double Vend, double deltaX, int Direction )
{
    //LONG Xbreak;
    if(rr->CmdCount < CQ_SIZE){
        rr->CmdQueue[rr->NextWriteCmd].State = State;
        //rr->CmdQueue[rr->NextWriteCmd].Vend = Vend;
        //rr->CmdQueue[rr->NextWriteCmd].deltaX = deltaX;
        //if(Direction > 0)
            //rr->CmdQueue[rr->NextWriteCmd].Direction = 1;
        //else
          //  rr->CmdQueue[rr->NextWriteCmd].Direction = 0;
         //rr->CmdQueue[rr->NextWriteCmd].RunStep = 32000;
        /*
        CalculateBreakParam(rr->K, rr->B, rr->dx, State, rr->CmdQueue[rr->NextWriteCmd].Direction, rr->LastCmdV,
            &rr->CmdQueue[rr->NextWriteCmd].Vend,
            &rr->CmdQueue[rr->NextWriteCmd].deltaX,NULL, &Xbreak);
        rr->CmdQueue[rr->NextWriteCmd].RunStep = Xbreak;
        rr->LastCmdV = rr->CmdQueue[rr->NextWriteCmd].Vend;
        if(Direction > 0)
            rr->LastCmdX += rr->CmdQueue[rr->NextWriteCmd].deltaX;
        else
            rr->LastCmdX -= rr->CmdQueue[rr->NextWriteCmd].deltaX;
        */
        rr->NextWriteCmd++;
        if(rr->NextWriteCmd >= CQ_SIZE)rr->NextWriteCmd -= CQ_SIZE;
        rr->CmdCount++;
    } else return -1;
    if((rr->CmdCount>0)&&(rr->CacheState == ST_STOP)){
        CacheNextCmd(rr);
    }
    return 0;
}

int CalculateBreakParam(double K, double B, double dx, GD_STATE State, int Direction,
                        double Vbeg, double * Vend,
                        double * deltaX, double * deltaT, LONG * Xbreak)
{
    double VKpB = 0.0;
    double dTb = 0.0;
    double dTe = 0.0;
    LONG Xe;
    LONG Xb;
    LONG dX;
    LONG dX2;
    double XX = 0.0;
    double D = 0.0;
    double d = - K/(2.0 * B);
    double a = 4.0 * B/(K * K);
    double T2 = 0.0;
    double OmKT = 0.0;
    double XmX = 0.0;
    //rr->d

    switch (State) {
        case ST_ACCELERATE:
        case ST_DECELERATE:
            if(Vbeg!=0.0){
                VKpB = Vbeg * K + B;
                D = B * VKpB;
                dTb = ((VKpB - sqrt(D))/(K * VKpB));
                Xb = (LONG)((B * dTb * dTb) / ((1 - K * dTb)*dx));
            } else Xb = 0;
            if(*Vend != 0.0){
                VKpB = (*Vend) * K + B;
                D = B * VKpB;
                dTe = ((VKpB - sqrt(D))/(K * VKpB));
                Xe = (LONG)((B * dTe * dTe) / ((1 - K * dTe)*dx));
            } else Xe = 0;
            if(State != ST_DECELERATE){
                dX = Xe - Xb;
            } else {
                dX = Xb - Xe;
            }
            if(Direction >0 ){
                // количество радиан на которое сдвинулись
                XmX = (*deltaX);
            } else {
                XmX = (*deltaX);
            }
            dX2 = (LONG)(XmX/dx);

            if (dX2 < dX) {
                // если координата перемещения меньше, чем координата при заданной скорости
                (*Xbreak) = dX2;
                if(State != ST_DECELERATE){
                    XX = Xb * dx + XmX;
                } else {
                    XX = Xb * dx - XmX;
                }
                D = XX *(XX + a);
                if(D >= 0.0){
                    T2 = ((XX + sqrt(D))*d);
                }
                OmKT = (1 - K * T2);
                (*Vend) = (B * T2*( 1.0 + OmKT))/(OmKT * OmKT); // вычислить скорость  V = BT/(1-KT)  T =
                if (deltaT != NULL) {
                    if(T2 >= dTb)
                        *deltaT = (T2 - dTb);
                    else
                        *deltaT = (dTb - T2);
                }
            } else {
                // если координата перемещения больше, чем координата при заданной скорости
                if(Direction >0 ){
                    (*Xbreak) = dX;
                    (*deltaX) = dX * dx;
                } else {
                    (*Xbreak) = dX;
                    (*deltaX) = dX * dx;
                }
                if (deltaT != NULL) {
                    if(dTe >= dTb)
                        *deltaT = dTe - dTb;
                    else
                        *deltaT = dTb - dTe;
                }
            }
            break;
        case ST_RUN:
            {
                if(Direction >0 ){
                    // количество шагов на которое сдвинулись
                    XmX = (*deltaX);
                } else {
                    XmX = (*deltaX);
                }
                (*Xbreak) = (LONG)(XmX / dx);
                (*Vend) = Vbeg;
                if (deltaT != NULL) {
                    *deltaT = XmX/Vbeg;
                }
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

int GoToCmd(RR * rr, double VTarget, double XTarget, DWORD Tick)
{
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
    /*	
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
    }   */ 
    return 0;
}

double GetAngle(WORD n)
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
int GetStatus(WORD n){    
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


// Вычисление звездного времени
double GM_Sidereal_Time (double jd) {
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

double LM_Sidereal_Time (double jd, double longitude) {
    double GMST = GM_Sidereal_Time(jd);
    double LMST =  24.0*frac((GMST + longitude/15.0)/24.0);
    return LMST;
}

double frac(double X) {
    X = X - floor(X);
    if (X<0) X = X + 1.0;
    return X;
}
// день d, месяц m, год y, время в часах  
double JulDay (BYTE D, BYTE M,WORD Y,BYTE h, BYTE m, BYTE s){
    double JD = 0.0;
    double UT = h + m/60 + s/3600;
    if (Y < 1900) Y = Y + 1900;
    if (M <= 2) { M = M + 12; Y = Y - 1;}
    JD =  floor(365.25 * (Y + 4716)) + floor(30.6001 * (M + 1)) + D - 13 - 1524.5 + UT/24.0;
    return JD;
}

