// #ifndef __OCTIMER_C
// #define __OCTIMER_C
// #endif

#include "stdafx.h"

#ifdef __C30__
#   include "TCPIP Stack/TCPIP.h"
#   include "math.h"
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
int TmrInit(BYTE Num);
int CacheNextCmd(RR * rr);
DWORD GetBigTmrValue(BYTE id);
int DisableOC(BYTE oc);
int SetDirection(BYTE oc, BYTE Dir);
int SetOC(BYTE oc, WORD LW);
int EnableOC(BYTE oc);
BOOL IsDisableOC(BYTE oc);
int CalculateBreakParam(RR * rr, GD_STATE State, int Direction, double Vbeg, double * Vend, double * deltaX, double * deltaT, LONG * Xbreak);
int ProcessTimer(BYTE id, RR * rr);
void CalculateParams(RR * rr);
int BreakCurrentCmd(RR * rr);

double frac(double X);
double GM_Sidereal_Time (double jd);

#ifdef __C30__
void __attribute__((__interrupt__,__no_auto_psv__)) _OC1Interrupt( void )
{
    IFS0bits.OC1IF = 0; // Clear OC1 interrupt flag
    if(PORT1_DIR)
        M1.Val++;
    else
        M1.Val--;
    ProcessOC(&rr1);
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC2Interrupt( void )
{
    IFS0bits.OC2IF = 0; // Clear OC2 interrupt flag
    if(PORT1_DIR)
        M2.Val++;
    else
        M2.Val--;
    ProcessOC(&rr2);
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC3Interrupt( void )
{
    IFS1bits.OC3IF = 0; // Clear OC3 interrupt flag
    if(PORT1_DIR)
        M3.Val++;
    else
        M3.Val--;
    ProcessOC(&rr3);
}
void __attribute__((__interrupt__,__no_auto_psv__)) _T2Interrupt( void )
{
    IFS0bits.T2IF = 0; // Clear T2 interrupt flag
    Timer2Big.word.HW++;
    ProcessTimer(2, &rr1);
    ProcessTimer(2, &rr2);
    ProcessTimer(2, &rr3);
}
void __attribute__((__interrupt__,__no_auto_psv__)) _T3Interrupt( void )
{
    IFS0bits.T3IF = 0; // Clear T3 interrupt flag
    Timer3Big.word.HW++;
    ProcessTimer(3, &rr1);
    ProcessTimer(3, &rr2);
    ProcessTimer(3, &rr3);
}
void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt( void )
{
    IFS1bits.U2RXIF = 0;
    //Control(&rr1);
}
void __attribute__((__interrupt__,__no_auto_psv__)) _U2TXInterrupt( void )
{
    IFS1bits.U2TXIF = 0;
    //Control(&rr2);
}
void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt( void )
{
    IFS0bits.U1RXIF = 0;
    //Control(&rr3);
}
#else
    WORD TMR2 = 0;
    WORD TMR3 = 0;
    WORD OC1R = 0;
#endif

int OCInit(void)
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

        IFS1bits.U2RXIF = 0;
        IPC7bits.U2RXIP = 5;        // Priority level 6
        IEC1bits.U2RXIE = 1;

        IFS1bits.U2TXIF = 0;
        IPC7bits.U2TXIP = 5;        // Priority level 6
        IEC1bits.U2TXIE = 1;

        IFS0bits.U1RXIF = 0;
        IPC2bits.U1RXIP = 5;        // Priority level 6
        IEC0bits.U1RXIE = 1;

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

        TmrInit(2);
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

        // инициализация OC1
        {
            OC1CONbits.OCM = 0b000;
            OC1CONbits.OCTSEL = 0;      // выбрать Timer2
            IPC0bits.OC1IP = 6;            // выбрать приоритет прерывания для OC1
            IFS0bits.OC1IF = 0;            // сбросить флаг прерывания
            IEC0bits.OC1IE = 1;            // разрешаем прерывания от OC1
        }
        // инициализация OC2
        {
            OC2CONbits.OCM = 0b000;
            OC2CONbits.OCTSEL = 0;      // выбрать Timer2
            IPC1bits.OC2IP = 6;            // выбрать приоритет прерывания для OC2
            IFS0bits.OC2IF = 0;            // сбросить флаг прерывания
            IEC0bits.OC2IE = 1;            // разрешаем прерывания от OC2
        }
        // инициализация OC3
        {
            OC3CONbits.OCM = 0b000;
            OC3CONbits.OCTSEL = 0;      // выбрать Timer2
            IPC6bits.OC3IP = 6;            // выбрать приоритет прерывания для OC3
            IFS1bits.OC3IF = 0;            // сбросить флаг прерывания
            IEC1bits.OC3IE = 1;            // разрешаем прерывания от OC3
        }

        //IFS0bits.OC1IF = 1;
        //IFS0bits.OC2IF = 1;
        //IFS1bits.OC3IF = 1;
        ProcessOC(&rr1);
        ProcessOC(&rr2);
        ProcessOC(&rr3);
        //Timer2Big.Val = 0;
        //TMR2 = 0;
        //T2CONbits.TON = 1;         // Start Timer
#endif //#ifdef __C30__

    return 0;
}
/*
11 = 1:256
10 = 1:64
01 = 1:8
00 = 1:1
*/
int TmrInit(BYTE Num)
{
        switch(Num){
                case 2:
                    #ifdef __C30__
                    T2CONbits.TON = 0;         // Disable Timer
                    T2CONbits.TCS = 0;         // Select internal instruction cycle clock
                    T2CONbits.TGATE = 0;     // Disable Gated Timer mode
                    T2CONbits.TCKPS = 0b01; // Select 8:1 Prescaler 200ns
                    TMR2 = 0x00;             // Clear timer register
                    PR2 = 0xFFFF;             // Load the period value
                    IPC1bits.T2IP = 7;     // Set Timer2 Interrupt Priority Level
                    IFS0bits.T2IF = 0;         // Clear Timer2 Interrupt Flag
                    IEC0bits.T2IE = 1;         // Enable Timer2 interrupt
                    T2CONbits.TON = 1;         // Start Timer
                    #endif //#ifdef __C30__
                    break;
                case 3:
                    #ifdef __C30__
                    T3CONbits.TON = 0;         // Disable Timer
                    T3CONbits.TCS = 0;         // Select internal instruction cycle clock
                    T3CONbits.TGATE = 0;     // Disable Gated Timer mode
                    T3CONbits.TCKPS = 0b01; // Select 8:1 Prescaler 200ns
                    TMR3 = 0x00;             // Clear timer register
                    PR3 = 0xFFFF;             // Load the period value
                    IPC2bits.T3IP = 7;         // Set Timer3 Interrupt Priority Level
                    IFS0bits.T3IF = 0;         // Clear Timer3 Interrupt Flag
                    IEC0bits.T3IE = 1;         // Enable Timer3 interrupt
                    T3CONbits.TON = 1;         // Start Timer
                    #endif //#ifdef __C30__
                    break;
                default: return -1;
        }
        return 0;
}

int InitRR(RR * rr)
{   //AppConfig
    rr->Mass = AppConfig.RRConfig[rr->Index].Mass;
    rr->Radius = AppConfig.RRConfig[rr->Index].Radius;
    rr->Length = AppConfig.RRConfig[rr->Index].Length;
    rr->Reduction = AppConfig.RRConfig[rr->Index].Reduction;
    rr->StepPerTurn = AppConfig.RRConfig[rr->Index].StepPerTurn;
    rr->uStepPerStep = AppConfig.RRConfig[rr->Index].uStepPerStep; //16
    rr->TimerStep = AppConfig.RRConfig[rr->Index].TimerStep;//0.0000002; // шаг таймера
    rr->Vend = 0.0;
    rr->deltaX = 0.0;
    rr->DataCount = 0;
    rr->NextWriteTo = 0;
    rr->NextReadFrom = 0;
    rr->XaccBeg = 0;
    rr->XCachePos = 0;
    rr->CacheCmdCounter = 0;
    //Timer2Big.word.LW = TMR2;
    rr->TimeBeg = 0;
    rr->CacheState = ST_STOP;
    rr->RunState = ST_STOP;
    rr->T.Val = 0;
    rr->Tb = 0;
    rr->T1 = 0;
    rr->CmdCount = 0;
    rr->CacheDir = 1;
    rr->RunDir = 1;
    rr->XPosition = RRConfigRAM.RRSave[rr->Index].XPosition;
    rr->NextCacheCmd = 0;
    rr->NextWriteCmd = 0;
    rr->Interval = 32768;
    rr->LastCmdV = 0.0;
    rr->LastCmdX = 0.0;
    rr->VMax = AppConfig.RRConfig[rr->Index].VMax * Grad_to_Rad;
    CalculateParams(rr);

    return 0;
}
void CalculateParams(RR * rr)
{
    double I;
    I = ((rr->Mass*rr->Radius*rr->Radius/4) + (rr->Mass*rr->Length*rr->Length/12))/rr->Reduction;
    rr->K = (AppConfig.RRConfig[rr->Index].K)/I;
    rr->B = AppConfig.RRConfig[rr->Index].B / I;
    rr->dx = 2.0 * PI /(rr->Reduction * rr->StepPerTurn * rr->uStepPerStep); // шаг перемещения в радианах
    rr->d = (-(rr->K)/(2.0 * rr->B * rr->TimerStep));
    rr->a = (4.0 * rr->B/(rr->K * rr->K));
    rr->e = (DWORD)(0.000120 / rr->TimerStep); //70us
    rr->LastCmdX = rr->XPosition*rr->dx;

}
int Run(RR * rr)
{
    // x = V*T
    // T = X/V;
    DWORD FreeData;
    DWORD T2 = 0;
    LONG dT = 0;
    WORD i = 0;
    LONG m = 1;
    double X = 0.0;

    // оптимизировано 37 uSec (1431.5 тактов за шаг) при m=1
    // 3.30546875 uSec  при m = 32 (132.21875 тактов на шаг)
    if(BUF_SIZE - rr->DataCount >= 32){
        FreeData = 32;
    } else {
        FreeData = (BUF_SIZE - rr->DataCount);
    }
    if((DWORD)abs(rr->Interval)>= rr->e)
    {
        m = 1;
        if(rr->CacheCmdCounter < FreeData){
            FreeData = rr->CacheCmdCounter;
            rr->CacheCmdCounter = 0;
        } else {
            rr->CacheCmdCounter -= FreeData;
        }
        for( i = 0; i < FreeData; i++) {
            rr->XCachePos++;
            X = rr->XCachePos * rr->dx;
            T2 = (DWORD)(X / (rr->Vend * rr->TimerStep));
            dT = T2 - rr->T1;
            rr->T1 = T2;
            rr->Interval = dT;
            //rr->IntervalArray[rr->NextWriteTo].FixedPoint = T2; // будущая точка
            rr->IntervalArray[rr->NextWriteTo].Interval = rr->Interval;
            rr->IntervalArray[rr->NextWriteTo].Correction = 0;
            rr->IntervalArray[rr->NextWriteTo].Count = 1;
            rr->IntervalArray[rr->NextWriteTo].State = ST_RUN;
            rr->IntervalArray[rr->NextWriteTo].Flags.Dir = rr->CacheDir;
            rr->IntervalArray[rr->NextWriteTo].Flags.uSteps = rr->uStepPerStep;
            rr->DataCount++;
            rr->NextWriteTo++;
            if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
        }
    }else {
        m = rr->CacheCmdCounter;
        rr->CacheCmdCounter = 0;
        if( m > 0 ) {
            rr->XCachePos += m;
            X = rr->XCachePos * rr->dx;
            T2 = (DWORD)(X / (rr->Vend * rr->TimerStep));
            dT = T2 - rr->T1;
            rr->T1 = T2;
            rr->Interval = (dT / m);
            //rr->IntervalArray[rr->NextWriteTo].FixedPoint = T2; // будущая точка
            rr->IntervalArray[rr->NextWriteTo].Interval = rr->Interval;
            rr->IntervalArray[rr->NextWriteTo].Correction = dT % m;
            rr->IntervalArray[rr->NextWriteTo].Count = m;
            rr->IntervalArray[rr->NextWriteTo].State = ST_RUN;
            rr->IntervalArray[rr->NextWriteTo].Flags.Dir = rr->CacheDir;
            rr->IntervalArray[rr->NextWriteTo].Flags.uSteps = rr->uStepPerStep;
            rr->DataCount++;
            rr->NextWriteTo++;
            if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
        }

    }
    if(rr->CacheCmdCounter <= 0){
        CacheNextCmd(rr);
    }
    return 0;
}
// разгон с текущей скорости до требуемой
int Acceleration(RR * rr)
{
    DWORD FreeData;
    DWORD T2 = 0;
    LONG dT = 0;
    double D;
    double X = 0.0;
    WORD i = 0;
    LONG m = 1;

    // оптимизировано 37 uSec (1431.5 тактов за шаг) при m=1
    // 3.30546875 uSec  при m = 32 (132.21875 тактов на шаг)
     if(BUF_SIZE - rr->DataCount >= 32){
         FreeData = 32;
     } else {
         FreeData = (BUF_SIZE - rr->DataCount);
     }

    if(abs(rr->Interval)>= rr->e)
    {
        m = 1;
        if(FreeData < 32) return 0;
        if(rr->CacheCmdCounter < FreeData){
            FreeData = rr->CacheCmdCounter;
            rr->CacheCmdCounter = 0;
        } else {
            rr->CacheCmdCounter -= FreeData;
        }
        for( i = 0; i < FreeData; i++) {
            rr->XaccBeg++;
            X = rr->XaccBeg * rr->dx;
            D = X *(X + rr->a);
            if(D >= 0.0){
                T2 = (DWORD)((X + sqrt(D))*rr->d);
            }
            dT = T2 - rr->T1;
            rr->T1 = T2;
            rr->Interval = dT;
            //rr->IntervalArray[rr->NextWriteTo].FixedPoint = T2; // будущая точка
            rr->IntervalArray[rr->NextWriteTo].Interval = rr->Interval;
            rr->IntervalArray[rr->NextWriteTo].Correction = 0;
            rr->IntervalArray[rr->NextWriteTo].Count = 1;
            rr->IntervalArray[rr->NextWriteTo].State = ST_ACCELERATE;
            rr->IntervalArray[rr->NextWriteTo].Flags.Dir = rr->CacheDir;
            rr->IntervalArray[rr->NextWriteTo].Flags.uSteps = rr->uStepPerStep;
            rr->DataCount++;
            rr->NextWriteTo++;
            if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
        }
    }else {
        if(abs(rr->Interval) >= 200) m = 32;
        else m = 256;

        {
            if(rr->CacheCmdCounter >= m) {
                rr->CacheCmdCounter -= m;
            } else {
                m = rr->CacheCmdCounter;
                rr->CacheCmdCounter = 0;
            }
            if( m > 0 ) {
                rr->XaccBeg += m;
                X = rr->XaccBeg * rr->dx;
                D = X *(X + rr->a);
                if(D >= 0.0){
                    T2 = (DWORD)((X + sqrt(D))*rr->d);
                }
                dT = T2 - rr->T1;
                rr->T1 = T2;
                rr->Interval =(dT / m);
                //rr->IntervalArray[rr->NextWriteTo].FixedPoint = T2; // будущая точка
                rr->IntervalArray[rr->NextWriteTo].Interval = rr->Interval;
                rr->IntervalArray[rr->NextWriteTo].Correction = (dT % m);
                rr->IntervalArray[rr->NextWriteTo].Count = m;
                rr->IntervalArray[rr->NextWriteTo].State = ST_ACCELERATE;
                rr->IntervalArray[rr->NextWriteTo].Flags.Dir = rr->CacheDir;
                rr->IntervalArray[rr->NextWriteTo].Flags.uSteps = rr->uStepPerStep;
                rr->DataCount++;
                rr->NextWriteTo++;
                if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
            }
        }
    }
    if(rr->CacheCmdCounter <= 0){
        CacheNextCmd(rr);
    }

    return 0;
}

// торможение с текущей скорости до требуемой
int Deceleration(RR * rr)
{
    LONG FreeData;
    DWORD T2 = 0;
    LONG dT = 0;
    double D;
    double X = 0.0;
    WORD i = 0;
    LONG m = 1;

    //rr->Tb = rr->T1;
    // оптимизировано 37 uSec (1431.5 тактов за шаг) при m=1
    // 3.30546875 uSec  при m = 32 (132.21875 тактов на шаг)
    if(BUF_SIZE - rr->DataCount >= 32){
        FreeData = 32;
    } else {
        FreeData = (BUF_SIZE - rr->DataCount);
    }

    if(abs(rr->Interval)>= rr->e)
    {
        m = 1;
        if(FreeData < 32) return 0;
        if(rr->CacheCmdCounter < FreeData){
            FreeData = rr->CacheCmdCounter;
            rr->CacheCmdCounter = 0;
        } else {
            rr->CacheCmdCounter -= FreeData;
        }
        for( i = 0; i < FreeData; i++) {
            rr->XaccBeg--;
            X = rr->XaccBeg * rr->dx;
            D = X *(X + rr->a);
            if(D >= 0.0){
                T2 = (DWORD)((X + sqrt(D))*rr->d);
            }
            dT = rr->T1 - T2;
            rr->T1 = T2;
            rr->Interval = dT;
            //rr->IntervalArray[rr->NextWriteTo].FixedPoint = rr->Tb - T2; // будущая точка
            rr->IntervalArray[rr->NextWriteTo].Interval = rr->Interval;
            rr->IntervalArray[rr->NextWriteTo].Correction = 0;
            rr->IntervalArray[rr->NextWriteTo].Count = 1;
            rr->IntervalArray[rr->NextWriteTo].State = ST_DECELERATE;
            rr->IntervalArray[rr->NextWriteTo].Flags.Dir = rr->CacheDir;
            rr->IntervalArray[rr->NextWriteTo].Flags.uSteps = rr->uStepPerStep;
            rr->DataCount++;
            rr->NextWriteTo++;
            if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
        }
    }else {
        if(abs(rr->Interval) >= 200) m = 32;
        else m = 256;
        if(rr->CacheCmdCounter >= m) {
            rr->CacheCmdCounter -= m;
        } else {
            m = rr->CacheCmdCounter;
            rr->CacheCmdCounter = 0;
        }
        if(m > 0) {
            rr->XaccBeg -= m;
            X = rr->XaccBeg * rr->dx;
            D = X *(X + rr->a);
            if(D >= 0.0){
                T2 = (DWORD)((X + sqrt(D))*rr->d);
            }
            dT = rr->T1 - T2;
            rr->T1 = T2;
            rr->Interval = dT / m;
            //rr->IntervalArray[rr->NextWriteTo].FixedPoint = rr->Tb - T2; // будущая точка
            rr->IntervalArray[rr->NextWriteTo].Interval = rr->Interval;
            rr->IntervalArray[rr->NextWriteTo].Correction = dT % m;
            rr->IntervalArray[rr->NextWriteTo].Count = m;
            rr->IntervalArray[rr->NextWriteTo].State = ST_DECELERATE;
            rr->IntervalArray[rr->NextWriteTo].Flags.Dir = rr->CacheDir;
            rr->IntervalArray[rr->NextWriteTo].Flags.uSteps = rr->uStepPerStep;
            rr->DataCount++;
            rr->NextWriteTo++;
            if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
        }
    }
    if(rr->CacheCmdCounter <= 0){
        CacheNextCmd(rr);
    }
    return 0;
}

int Control(RR * rr)
{
    do{
        if((rr->DataCount < BUF_SIZE - 1)&&(rr->CacheState != ST_STOP))
        {
            switch(rr->CacheState){
            case ST_ACCELERATE:
                Acceleration(rr);
                break;
            case ST_RUN:
                Run(rr);
//                 rr->uStepPerStep = 4;
//                 CalculateParams(rr);
                break;
            case ST_DECELERATE:
                Deceleration(rr);
                break;
            case ST_STOP:
            break;
            }
        }
    } while ((rr->DataCount <= 1)&&(rr->CacheState != ST_STOP));
    // запуск
    if((rr->RunState == ST_STOP)&&(rr->DataCount > 1)){
        rr->T.Val = GetBigTmrValue(rr->TmrId);
#ifdef __C30__
        switch(rr->Index){
            case 0:
                IFS0bits.OC1IF = 1;
                break;
            case 1:
                IFS0bits.OC2IF = 1;
                break;
            case 2:
                IFS1bits.OC3IF = 1;
                break;
        }
#endif
    }
    return 0;
}

int TimerMonitor()
{
    Control(&rr1);
    Control(&rr2);
    Control(&rr3);
    return 0;
}


int ProcessOC(RR * rr)
{
    DWORD Timer = 0;
    if(rr->DataCount == 0) {
        rr->RunState = ST_STOP;
        DisableOC(rr->Index);
        return 0;
    }
    // задаём значение времени начала выполнения команды
    if(rr->RunState != rr->IntervalArray[rr->NextReadFrom].State){
        rr->RunState = rr->IntervalArray[rr->NextReadFrom].State;
        rr->TimeBeg = rr->T.Val;
    }
    // устанавливаем направление вращения
    if(rr->RunDir != rr->IntervalArray[rr->NextReadFrom].Flags.Dir){
        rr->RunDir = rr->IntervalArray[rr->NextReadFrom].Flags.Dir;
        SetDirection(rr->Index,rr->RunDir);
    }
    // прибавляем интервалы и корректируем
    // TODO: Сделать равномерное распределение коррекции
    //if(rr->uStepPerStep != rr->IntervalArray[rr->NextWriteTo].Flags.uSteps){
        // смена микрошага.
        // 16   * 1
        // 8    * 2     ( Interval << 1)
        // 4    * 4     ( Interval << 2)
        // 2    * 8     ( Interval << 3)
        // 1    * 16    ( Interval << 4)
        // пока что на ходу меняем только с 16 на 4

    {
        rr->T.Val += rr->IntervalArray[rr->NextReadFrom].Interval;
        if(rr->IntervalArray[rr->NextReadFrom].Correction > 0) {
            rr->T.Val++;
            rr->IntervalArray[rr->NextReadFrom].Correction--;
        }
        rr->IntervalArray[rr->NextReadFrom].Count--;
    }

    SetOC(rr->Index, rr->T.word.LW);
    Timer = GetBigTmrValue(rr->TmrId);
    if((Timer <= rr->T.Val)&&(Timer + 0x00010020 >= rr->T.Val)){
        EnableOC(rr->Index);
    }

    if(rr->RunDir > 0){
        rr->XPosition++;
    } else {
        rr->XPosition--;
    }
    if(rr->IntervalArray[rr->NextReadFrom].Count == 0) {
        rr->NextReadFrom++;
        if(rr->NextReadFrom >= BUF_SIZE) rr->NextReadFrom -= BUF_SIZE;
        if(rr->DataCount > 0) rr->DataCount--;

        //rr->CorrectionRate = rr->IntervalArray[rr->NextReadFrom].Count /  rr->IntervalArray[rr->NextReadFrom].Correction;
    }
    return 0;
}

int SetOC(BYTE oc, WORD LW)
{
    switch (oc){
    case 0:
#ifdef __C30__
        OC1CONbits.OCM = 0b000; // выключить модуль OC
        OC1RS = LW + 50; // записать значение OCxRS
#endif // __C30__
        OC1R = LW;        // записать значение OCxR
        break;
    case 1:
#ifdef __C30__
        OC2CONbits.OCM = 0b000; // выключить модуль OC
        OC2R = LW;        // записать значение OCxR
        OC2RS = LW + 50; // записать значение OCxRS
#endif // __C30__
        break;
    case 2:
#ifdef __C30__
        OC3CONbits.OCM = 0b000; // выключить модуль OC
        OC3R = LW;        // записать значение OCxR
        OC3RS = LW + 50; // записать значение OCxRS
#endif // __C30__
        break;
    case 3:
#ifdef __C30__
        OC4CONbits.OCM = 0b000; // выключить модуль OC
        OC4R = LW;        // записать значение OCxR
        OC4RS = LW + 50; // записать значение OCxRS
#endif // __C30__
        break;
    default :
        return -1;
    }
    return 0;
}
int EnableOC(BYTE oc)
{
#ifdef __C30__
    switch (oc){
    case 0:
        OC1CONbits.OCM = 0b100; // выключить модуль OC
        break;
    case 1:
        OC2CONbits.OCM = 0b100; // выключить модуль OC
        break;
    case 2:
        OC3CONbits.OCM = 0b100; // выключить модуль OC
        break;
    case 3:
        OC4CONbits.OCM = 0b100; // выключить модуль OC
        break;
    default :
    return -1;
    }
    return 0;
#else
    return oc;
#endif

}
int DisableOC(BYTE oc)
{
#ifdef __C30__
    switch (oc){
    case 0:
        OC1CONbits.OCM = 0b000; // выключить модуль OC
        IFS0bits.OC1IF = 0;     // Clear OC1 interrupt flag
        break;
    case 1:
        OC2CONbits.OCM = 0b000; // выключить модуль OC
        IFS0bits.OC2IF = 0; // Clear OC1 interrupt flag
        break;
    case 2:
        OC3CONbits.OCM = 0b000; // выключить модуль OC
        IFS1bits.OC3IF = 0; // Clear OC1 interrupt flag
        break;
    case 3:
        OC4CONbits.OCM = 0b000; // выключить модуль OC
        IFS1bits.OC4IF = 0; // Clear OC1 interrupt flag
        break;
    default :
    return -1;
    }
    return 0;
#else
    return oc;
#endif
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

int ProcessTimer(BYTE id, RR * rr)
{
    DWORD Timer = GetBigTmrValue(id);
    if(rr->TmrId == id){
        if(IsDisableOC(rr->Index)){
            if(rr->RunState != ST_STOP){
                if((Timer <= rr->T.Val)&&(Timer + 0x00010020 >= rr->T.Val)){
                    EnableOC(rr->Index);
                }
            }
        }
    }
    return 0;
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

int CacheNextCmd(RR * rr)
{
    if(rr->CmdCount > 0){
        double D;
        double X;
        rr->CacheState = rr->CmdQueue[rr->NextCacheCmd].State;
        rr->Vend  = rr->CmdQueue[rr->NextCacheCmd].Vend;
        rr->deltaX  = rr->CmdQueue[rr->NextCacheCmd].deltaX;
        rr->CacheDir = rr->CmdQueue[rr->NextCacheCmd].Direction;
        rr->CacheCmdCounter = rr->CmdQueue[rr->NextCacheCmd].RunStep;
        switch(rr->CacheState){
            case ST_ACCELERATE:
            case ST_DECELERATE:
                if((rr->XaccBeg > 0)){
                    X = rr->XaccBeg * rr->dx;
                    D = X *(X + rr->a);
                    if(D >= 0.0){
                        rr->T1 = (DWORD)((X + sqrt(D))*rr->d);
                    }
                    rr->Tb = rr->T1;
                };
                break;
            case ST_RUN:
                rr->XCachePos = 0;
                rr->T1 = 0;
                rr->Tb = 0;
                break;
            default:;
        }
        rr->CmdCount--;
        rr->CmdQueue[rr->NextCacheCmd].State = ST_STOP;
        rr->CmdQueue[rr->NextCacheCmd].Vend = 0.0;
        rr->CmdQueue[rr->NextCacheCmd].deltaX = 0.0;
        rr->CmdQueue[rr->NextCacheCmd].Direction = 1;
        rr->CmdQueue[rr->NextCacheCmd].RunStep = 0;
        rr->NextCacheCmd++;
        if(rr->NextCacheCmd >= CQ_SIZE)rr->NextCacheCmd -= CQ_SIZE;
    } else {
        rr->CacheState = ST_STOP;
    }
    return 0;
}

int PushCmdToQueue(RR * rr, GD_STATE State, double Vend, double deltaX, int Direction )
{
    LONG Xbreak;
    if(rr->CmdCount < CQ_SIZE){
        rr->CmdQueue[rr->NextWriteCmd].State = State;
        rr->CmdQueue[rr->NextWriteCmd].Vend = Vend;
        rr->CmdQueue[rr->NextWriteCmd].deltaX = deltaX;
        if(Direction > 0)
            rr->CmdQueue[rr->NextWriteCmd].Direction = 1;
        else
            rr->CmdQueue[rr->NextWriteCmd].Direction = 0;

        CalculateBreakParam(rr, State, rr->CmdQueue[rr->NextWriteCmd].Direction, rr->LastCmdV,
            &rr->CmdQueue[rr->NextWriteCmd].Vend,
            &rr->CmdQueue[rr->NextWriteCmd].deltaX,NULL, &Xbreak);
        rr->CmdQueue[rr->NextWriteCmd].RunStep = Xbreak;
        rr->LastCmdV = rr->CmdQueue[rr->NextWriteCmd].Vend;
        if(Direction > 0)
            rr->LastCmdX += rr->CmdQueue[rr->NextWriteCmd].deltaX;
        else
            rr->LastCmdX -= rr->CmdQueue[rr->NextWriteCmd].deltaX;

        rr->NextWriteCmd++;
        if(rr->NextWriteCmd >= CQ_SIZE)rr->NextWriteCmd -= CQ_SIZE;
        rr->CmdCount++;
    } else return -1;
    if((rr->CmdCount>0)&&(rr->CacheState == ST_STOP)){
        CacheNextCmd(rr);
    }
    return 0;
}

int CalculateBreakParam(RR * rr, GD_STATE State, int Direction,
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
    double d = -rr->K/(2.0 * rr->B);
    double a = 4.0 * rr->B/(rr->K * rr->K);
    double T2 = 0.0;
    double OmKT = 0.0;
    double XmX = 0.0;
    //rr->d

    switch (State) {
        case ST_ACCELERATE:
        case ST_DECELERATE:
            if(Vbeg!=0.0){
                VKpB = Vbeg * rr->K + rr->B;
                D = rr->B * VKpB;
                dTb = ((VKpB - sqrt(D))/(rr->K * VKpB));
                Xb = (LONG)((rr->B * dTb * dTb) / ((1 - rr->K * dTb)*rr->dx));
            } else Xb = 0;
            if(*Vend != 0.0){
                VKpB = (*Vend) * rr->K + rr->B;
                D = rr->B * VKpB;
                dTe = ((VKpB - sqrt(D))/(rr->K * VKpB));
                Xe = (LONG)((rr->B * dTe * dTe) / ((1 - rr->K * dTe)*rr->dx));
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
            dX2 = (LONG)(XmX/rr->dx);

            if (dX2 < dX) {
                // если координата перемещения меньше, чем координата при заданной скорости
                (*Xbreak) = dX2;
                if(State != ST_DECELERATE){
                    XX = Xb * rr->dx + XmX;
                } else {
                    XX = Xb * rr->dx - XmX;
                }
                D = XX *(XX + a);
                if(D >= 0.0){
                    T2 = ((XX + sqrt(D))*d);
                }
                OmKT = (1 - rr->K * T2);
                (*Vend) = (rr->B * T2*( 1.0 + OmKT))/(OmKT * OmKT); // вычислить скорость  V = BT/(1-KT)  T =
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
                    (*deltaX) = dX * rr->dx;
                } else {
                    (*Xbreak) = dX;
                    (*deltaX) = dX * rr->dx;
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
                (*Xbreak) = (LONG)(XmX /rr->dx);
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

    if(rr->LastCmdX == XTarget) return 0;
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
        CalculateBreakParam(rr, ST_ACCELERATE, Direction, rr->LastCmdV, &VendA, &Xa, &Ta, &Xbreak);
        VendD = VTarget;
        CalculateBreakParam(rr, ST_DECELERATE, Direction, VendA, &VendD, &Xd, &Td, &Xbreak);
        //T0 = ((double)(Tick - TickGet()))* 0.00000025;
        if(Direction){
            Trun = -(XTarget - rr->LastCmdX - Xa - Xd + VTarget * (T0 + Ta + Td) ) /(VTarget - rr->VMax);
        } else {
            Trun = -(XTarget - rr->LastCmdX + Xa + Xd + VTarget * (T0 + Ta + Td) ) /(VTarget + rr->VMax);
        }
        //if(Trun < 0.0) Trun = -Trun;
        {
            double Xrun = rr->VMax * Trun;
            //BreakCurrentCmd(rr);
            PushCmdToQueue(rr, ST_ACCELERATE, VendA, PI , Direction);
            PushCmdToQueue(rr, ST_RUN, VendA,  Xrun, Direction);
            PushCmdToQueue(rr, ST_DECELERATE, VendD, PI, Direction);
            if(VTarget != 0.0){
                PushCmdToQueue(rr, ST_RUN, VTarget,  PI, Direction);
            }
            PushCmdToQueue(rr, ST_STOP, 0.0, 0.0, Direction);
        }
    }
    return 0;
}
int BreakCurrentCmd(RR * rr)
{
    if(rr->RunState == ST_RUN){
        DisableOC(rr->Index);
        rr->RunState = ST_STOP;
        rr->NextWriteTo = rr->NextReadFrom + 1;
        rr->DataCount = 0;
        rr->LastCmdX = rr->XPosition * rr->dx;        
        rr->DataCount = 0;
        CacheNextCmd(rr);
    }
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

