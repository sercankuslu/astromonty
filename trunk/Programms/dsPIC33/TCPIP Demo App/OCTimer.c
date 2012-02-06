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



static DWORD_VAL Timer2Big;
static DWORD_VAL Timer3Big;
RR rr1;
RR rr2;
RR rr3;

int InitRR(RR * rr);
int SetRunState(RR * rr);

#ifdef __C30__
void __attribute__((__interrupt__,__no_auto_psv__)) _OC1Interrupt( void )
{       
    ProcessOC(&rr1);
    IFS0bits.OC1IF = 0; // Clear OC1 interrupt flag
    
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC2Interrupt( void )
{
    ProcessOC(&rr2);
    IFS0bits.OC2IF = 0; // Clear OC2 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC3Interrupt( void )
{
    ProcessOC(&rr3);
    IFS1bits.OC3IF = 0; // Clear OC3 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC4Interrupt( void )
{
    //ProcessOC(&rr4);
    IFS1bits.OC4IF = 0; // Clear OC4 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC5Interrupt( void )
{

    IFS2bits.OC5IF = 0; // Clear OC5 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC6Interrupt( void )
{

    IFS2bits.OC6IF = 0; // Clear OC6 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC7Interrupt( void )
{

    IFS2bits.OC7IF = 0; // Clear OC7 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC8Interrupt( void )
{    	
    IFS2bits.OC8IF = 0; // Clear OC1 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _T2Interrupt( void )
{
    Timer2Big.word.HW++;    	
    ProcessTimer(2, &rr1);
    ProcessTimer(2, &rr2);
    ProcessTimer(2, &rr3);
    IFS0bits.T2IF = 0; // Clear T2 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _T3Interrupt( void )
{
        Timer3Big.word.HW++;
    ProcessTimer(3, &rr1);
    ProcessTimer(3, &rr2);
    ProcessTimer(3, &rr3);
    IFS0bits.T3IF = 0; // Clear T3 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt( void )
{
    Control(&rr1);    
    IFS1bits.U2RXIF = 0;
} 
void __attribute__((__interrupt__,__no_auto_psv__)) _U2TXInterrupt( void )
{    
    Control(&rr2); 
    IFS1bits.U2TXIF = 0;
} 
void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt( void )
{    
    Control(&rr3);
    IFS0bits.U1RXIF = 0;
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
        MS1         = 1;    // выход MS1  
        MS2         = 1; 	// выход MS2  
        SLEEP       = 1; 	// выход SLEEP
        RESET       = 1; 	// выход RESET
        
        MS1_Tris    = 0; 	// выход MS1
        MS2_Tris    = 0; 	// выход MS2
        SLEEP_Tris  = 0;	// выход SLEEP		
        RESET_Tris  = 0;	// выход RESET	
        // инициализация прерывания для заполнения буфера

        IFS1bits.U2RXIF = 0;
        IPC7bits.U2RXIP = 6;		// Priority level 6
        IEC1bits.U2RXIE = 1;

        IFS1bits.U2TXIF = 0;
        IPC7bits.U2TXIP = 6;		// Priority level 6
        IEC1bits.U2TXIE = 1;

        IFS0bits.U1RXIF = 0;
        IPC2bits.U1RXIP = 6;		// Priority level 6
        IEC0bits.U1RXIE = 1;

        //Инициализация порта1
        {
                PORT1_NULL_Tris   = 1; // вход NULL
                PORT1_POS_Tris    = 1; // вход POS
                PORT1_POS2_Tris   = 1; // вход POS2	
                
                PORT1_ENABLE 	  = 0;// выход ENABLE   
                PORT1_DIR 		  = 0;// выход DIR      
                PORT1_STEP 		  = 0;// выход STEP	
        
                PORT1_ENABLE_Tris = 0;// выход ENABLE
                PORT1_DIR_Tris    = 0;// выход DIR
                PORT1_STEP_Tris   = 0;// выход STEP		
        }    
        {
                PORT2_NULL_Tris   = 1; // вход NULL
                PORT2_POS_Tris    = 1; // вход POS
                PORT2_POS2_Tris   = 1; // вход POS2	
                
                PORT2_ENABLE 	  = 0;// выход ENABLE   
                PORT2_DIR 		  = 0;// выход DIR      
                PORT2_STEP 		  = 0;// выход STEP	
        
                PORT2_ENABLE_Tris = 0;// выход ENABLE
                PORT2_DIR_Tris    = 0;// выход DIR
                PORT2_STEP_Tris   = 0;// выход STEP		
        }
        {
                PORT3_NULL_Tris   = 1; // вход NULL
                PORT3_POS_Tris    = 1; // вход POS
                PORT3_POS2_Tris   = 1; // вход POS2	
                
                PORT3_ENABLE 	  = 0;// выход ENABLE   
                PORT3_DIR 		  = 0;// выход DIR      
                PORT3_STEP 		  = 0;// выход STEP	
        
                PORT3_ENABLE_Tris = 0;// выход ENABLE
                PORT3_DIR_Tris    = 0;// выход DIR
                PORT3_STEP_Tris   = 0;// выход STEP		
        }
#endif //#ifdef __C30__

        TmrInit(2);
        InitRR(&rr1);  
        rr1.Index = 1;
        rr1.TmrId = 2;
         InitRR(&rr2);
         rr2.Index = 2;
         rr2.TmrId = 2;
//         InitRR(&rr3);
//         rr3.Index = 3;
//         rr3.TmrId = 2;
#ifdef __C30__
        IFS1bits.U2RXIF = 1;
        //IFS1bits.U2TXIF = 1;
        //IFS0bits.U1RXIF = 1;
        
        // инициализация OC1
        {
            OC1CONbits.OCM = 0b000;
            OC1CONbits.OCTSEL = 0;  	// выбрать Timer2
            IPC0bits.OC1IP = 7;		    // выбрать приоритет прерывания для OC1
            IFS0bits.OC1IF = 0;			// сбросить флаг прерывания
            IEC0bits.OC1IE = 1;			// разрешаем прерывания от OC1    
        }
        // инициализация OC2
        {
            OC2CONbits.OCM = 0b000;
            OC2CONbits.OCTSEL = 0;  	// выбрать Timer2
            IPC1bits.OC2IP = 7;		    // выбрать приоритет прерывания для OC2
            IFS0bits.OC2IF = 0;			// сбросить флаг прерывания
            IEC0bits.OC2IE = 1;			// разрешаем прерывания от OC2    
        }
        // инициализация OC3
        {
            OC3CONbits.OCM = 0b000;
            OC3CONbits.OCTSEL = 0;  	// выбрать Timer2
            IPC6bits.OC3IP = 7;		    // выбрать приоритет прерывания для OC3
            IFS1bits.OC3IF = 0;			// сбросить флаг прерывания
            IEC1bits.OC3IE = 1;			// разрешаем прерывания от OC3    
        }
        
        IFS0bits.OC1IF = 1;
        //IFS0bits.OC2IF = 1;
        //IFS1bits.OC3IF = 1;
        Timer2Big.Val = 0;
        TMR2 = 0;
        T2CONbits.TON = 1; 		// Start Timer        
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
                    T2CONbits.TON = 0; 		// Disable Timer
                    T2CONbits.TCS = 0; 		// Select internal instruction cycle clock
                    T2CONbits.TGATE = 0; 	// Disable Gated Timer mode
                    T2CONbits.TCKPS = 0b01; // Select 8:1 Prescaler 200ns
                    TMR2 = 0x00; 			// Clear timer register
                    PR2 = 0xFFFF; 			// Load the period value
                    IPC1bits.T2IP = 7; 	// Set Timer2 Interrupt Priority Level
                    IFS0bits.T2IF = 0; 		// Clear Timer2 Interrupt Flag
                    IEC0bits.T2IE = 1; 		// Enable Timer2 interrupt
                    //T2CONbits.TON = 1; 		// Start Timer   
                    #endif //#ifdef __C30__
                    break;
                case 3:		
                    #ifdef __C30__
                    T3CONbits.TON = 0; 		// Disable Timer
                    T3CONbits.TCS = 0; 		// Select internal instruction cycle clock
                    T3CONbits.TGATE = 0; 	// Disable Gated Timer mode
                    T3CONbits.TCKPS = 0b01; // Select 8:1 Prescaler 200ns
                    TMR3 = 0x00; 			// Clear timer register
                    PR3 = 0xFFFF; 			// Load the period value
                    IPC2bits.T3IP = 0; 		// Set Timer3 Interrupt Priority Level
                    IFS0bits.T3IF = 0; 		// Clear Timer3 Interrupt Flag
                    IEC0bits.T3IE = 1; 		// Enable Timer3 interrupt
                    T3CONbits.TON = 1; 		// Start Timer
                    #endif //#ifdef __C30__
                    break;
                default: return -1;
        }		
        return 0;
}

int InitRR(RR * rr)
{
    double I;
    rr->Mass = 500.0f;
    rr->Radius = 0.50f;
    rr->Length = 3.0f;
    rr->Reduction = 360.0f;
    rr->StepPerTurn = 200;
    rr->uStepPerStep = 16;
    I = ((rr->Mass*rr->Radius*rr->Radius/4) + (rr->Mass*rr->Length*rr->Length/12))/rr->Reduction;
    rr->K = (-0.257809479)/I;
    rr->B = 0.205858823 / I;
    rr->dx = 2.0 * PI /(rr->Reduction * rr->StepPerTurn * rr->uStepPerStep); // шаг перемещения в радианах
    rr->TimerStep = 0.0000002; // шаг таймера
    rr->Vend = 0 * Grad_to_Rad;
    rr->Xend = 0 * Grad_to_Rad; // здесь удвоенная координата. т.к. после ускорения сразу идет торможение
    rr->DataCount = 0;
    rr->NextWriteTo = 0;
    rr->NextReadFrom = 0;    
    rr->XaccBeg = 0;
    rr->XCachePos = 0;
    rr->CacheCmdCounter = 0;
    Timer2Big.word.LW = TMR2;
    rr->TimeBeg = /* Timer2Big.Val */ + (ARR_TYPE)(0.0000 * BUF_SIZE /rr->TimerStep);
    rr->CacheState = ST_STOP;    
    rr->RunState = ST_STOP;
    rr->T.Val = 0;
    rr->Interval = 0xFFFFFFFF;
    rr->CmdCount = 0;
    rr->CalcDir = 1;
    rr->RunDir = 1;
    rr->XPosition = 0;
    rr->NextCacheCmd = 0;
    rr->NextWriteCmd = 0;
    rr->NextExecuteCmd = 0;
    rr->RunCmdCounter = 0;
    rr->e = (ARR_TYPE)(0.000120 / rr->TimerStep); //70us
//     PushCmdToQueue(rr, ST_ACCELERATE, 20.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
//     PushCmdToQueue(rr, ST_RUN, 0.0, 45.0 * Grad_to_Rad, 1);
//     PushCmdToQueue(rr, ST_DECELERATE, 0.0 * Grad_to_Rad, 90.0 * Grad_to_Rad, 1);
//      PushCmdToQueue(rr, ST_ACCELERATE, 20.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, -1);
//      PushCmdToQueue(rr, ST_RUN, 0.0, 16.69 * Grad_to_Rad, -1);
//      PushCmdToQueue(rr, ST_DECELERATE, 0.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, -1);
    //PushCmdToQueue(rr, ST_ACCELERATE, 4.0 * Grad_to_Rad, -8.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(rr, ST_DECELERATE, 0.0 * Grad_to_Rad, -8.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(rr, ST_RUN, 8.0 * Grad_to_Rad, 5.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(rr, ST_ACCELERATE, 3.0 * Grad_to_Rad, 1.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(rr, ST_ACCELERATE, 5.0 * Grad_to_Rad, 2.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(rr, ST_ACCELERATE, 8.0 * Grad_to_Rad, 2.0 * Grad_to_Rad, 1);
//     PushCmdToQueue(rr, ST_RUN, 8.0 * Grad_to_Rad, 5.0 * Grad_to_Rad, 1);
//     PushCmdToQueue(rr, ST_DECELERATE, 0, 0, 1);
//     PushCmdToQueue(rr, ST_ACCELERATE, 8.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, -1);
//     PushCmdToQueue(rr, ST_RUN, 8.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, -1);
//     PushCmdToQueue(rr, ST_DECELERATE, 0, 0, -1);
    //PushCmdToQueue(rr, ST_STOP, 0, 0, 1 );
    //RunCmd(rr);
    //CacheNextCmd(rr);
    return 0;    
}
int Run(RR * rr)
{
    // x = V*T
    // T = X/V;
    WORD k = 0;
    WORD m = 32;
    WORD i;
    WORD j;
    WORD FreeData = BUF_SIZE - rr->DataCount;  
    double X = 0.0;    
    LONG Xb = rr->XCachePos;
    ARR_TYPE T = 0;
    ARR_TYPE T1 = 0;// = 0;
    ARR_TYPE T2 = 0;    
        
    for( i = 0; i < FreeData; i++) {
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;
        if(k == 0) {
            if(rr->Interval < rr->e){
                //if(m < 32) m++;
                m = 32;
                X += rr->dx*m;
            } else {
                //if( m > 1) m--;
                m = 1;
                X += rr->dx;
            }
            T2 = (ARR_TYPE)(X / (rr->Vend * rr->TimerStep));
            if(m > 1){
                rr->Interval = (T2 - T1) / m;
            } else {
                rr->Interval = (T2 - T1);
            }
        }
        T += rr->Interval;
        if(rr->CalcDir >= 0){
            Xb++;
        } else {
            Xb--;            
        }
        rr->CacheCmdCounter--;
        if(rr->CacheCmdCounter <= 0){
            FreeData = i;
            CacheNextCmd(rr);
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = rr->TimeBeg + T;
        rr->DataCount++;           
        T1 = T;            
    }

    rr->TimeBeg += T1;
    //rr->DataCount += FreeData;
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->XCachePos = Xb;
    return 0;
}
// разгон с текущей скорости до требуемой
int Acceleration(RR * rr)
{
    WORD j;        
    WORD FreeData;
    ARR_TYPE T = 0;
    ARR_TYPE T1 = 0;
    ARR_TYPE T2 = 0;    
    double X;       // временная переменная 
    ARR_TYPE Tb = 0;  
    double D;
    LONG Xb = rr->XCachePos;
    double a;    
    double d;
    WORD i = 0;
    WORD k = 0;
    WORD m = 1;
   

    X = rr->XaccBeg * rr->dx; 
    d = rr->K/(2.0 * rr->B * rr->TimerStep);    
    a = 4.0 * rr->B/(rr->K * rr->K);   

    if(rr->XaccBeg > 0){
        D = X *(X + a);
        if(D >= 0.0){
            T1 = (ARR_TYPE)((-X - sqrt(D))*d);
        }  
    } else T1 = 0;
    Tb = rr->TimeBeg - T1;
    T = T1;
    
    // оптимизировано 37 uSec (1431.5 тактов за шаг) при m=1
    // 4.86uSec  при m = 32
    FreeData = BUF_SIZE - rr->DataCount;
    for( i = 0; i < FreeData; i++) {
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;
        if(k == 0) {
            if(rr->Interval < rr->e){
                //if(m < 32) m++;
                m = 32;
                X += rr->dx*m;
            } else {
                //if( m > 1) m--;
                m = 1;
                X += rr->dx;
            } 
            D = X *(X + a);
            if(D >= 0.0){
                T2 = (ARR_TYPE)((-X - sqrt(D))*d);
            }
            if(m > 1){
                rr->Interval = (T2 - T1) / m;
            } else {
                rr->Interval = (T2 - T1);
            }
        }
        T += rr->Interval;
        if(rr->CalcDir >= 0){
            Xb++;
        } else {
            Xb--;            
        }
        rr->CacheCmdCounter--;
        if(rr->CacheCmdCounter <= 0){
            FreeData = i;
            CacheNextCmd(rr);
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = Tb + T;
        rr->DataCount++;
        T1 = T;        
    }
    if(rr->RunState == ST_STOP){            
        SetRunState(rr);
        ProcessOC(rr);
    }
    rr->TimeBeg = Tb + T1;
    rr->XaccBeg += FreeData; 
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->XCachePos = Xb;
    return 0;
}

// торможение с текущей скорости до требуемой
int Deceleration(RR * rr)
{    
    WORD j;        
    WORD FreeData = BUF_SIZE - rr->DataCount;
    ARR_TYPE T = 0;
    ARR_TYPE T1 = 0;
    ARR_TYPE T2 = 0;    
    double X;       // временная переменная 
    ARR_TYPE Tb = 0;  
    double D;      
    LONG Xb = rr->XCachePos; //(LONG)(rr->Xbeg/rr->dx);
    double dx;
    double a;
    double c;
    double d;    
    WORD i = 0;    
    WORD k = 0;
    WORD m = 1;    
    dx = rr->dx;
    X = rr->XaccBeg * rr->dx; 
    d = rr->K / (2.0 * rr->B * rr->TimerStep);
    c = rr->K*d;
    a = 4.0 * rr->B / (rr->K * rr->K);    

    X = rr->XaccBeg * rr->dx; 
    if(rr->XaccBeg > 0){        
        D = X *(X + a);
        if(D >= 0.0){
            T1 = (ARR_TYPE)((-X - sqrt(D)) * d);
        } 
    }
    Tb = rr->TimeBeg + T1;
    T = T1; 
    
    for( i = 0; i < FreeData; i++) {
        // "грубые" вычисления
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;
        if(k == 0) {
            if(rr->Interval < rr->e){
                m = 32;
                X -= dx * m;
            } else {
                //if( m > 1 ) m -= 8;
                m = 1;
                X -= dx;
            }             
            if(X<=0.0){
                //X=0.0;
                 FreeData = i;
                 CacheNextCmd(rr);
                 break;
            }
            D = X *(X + a);
            if(D >= 0.0){
                T2 = (ARR_TYPE)((-X - sqrt(D)) * d);
            }
            if(m > 1){
                rr->Interval = (T1 - T2) / m;
            } else {
                rr->Interval = (T1 - T2);
            }
        }
        T -= rr->Interval;
        if(rr->CalcDir >= 0){
            Xb++;
        } else {
            Xb--;            
        }
        rr->CacheCmdCounter--;
        if(rr->CacheCmdCounter <= 0){
            FreeData = i;
            CacheNextCmd(rr);
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = Tb - T;
        rr->DataCount++;
        T1 = T;
    }   
    rr->TimeBeg = Tb - T1;
    rr->XaccBeg -= FreeData; 
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->XCachePos = Xb;    
    return 0;
}

int Control(RR * rr)
{
    while((rr->DataCount <= BUF_SIZE_2)&&(rr->CacheState != ST_STOP)){
        switch(rr->CacheState){
        case ST_ACCELERATE:
            Acceleration(rr);
            break;
        case ST_RUN:
            Run(rr);
            break;
        case ST_DECELERATE:
            Deceleration(rr); 
            break;
        case ST_STOP:
        break;
        }
    }    
    return 0;
}

int CacheNextCmd(RR * rr)
{    
    if(rr->CmdCount > 0){
        rr->CacheState = rr->CmdQueue[rr->NextCacheCmd].State;
        rr->Vend  = rr->CmdQueue[rr->NextCacheCmd].Vend;
        rr->Xend  = rr->CmdQueue[rr->NextCacheCmd].Xend; 
        rr->CalcDir = rr->CmdQueue[rr->NextCacheCmd].Direction;        
        rr->CacheCmdCounter = rr->CmdQueue[rr->NextCacheCmd].RunStep;
        rr->NextCacheCmd++;
        if(rr->NextCacheCmd >= CQ_SIZE)rr->NextCacheCmd -= CQ_SIZE;        
    } else {
        rr->CacheState = ST_STOP;
    }
    return 0;
}

int SetRunState(RR * rr)
{         
    rr->RunDir = rr->CmdQueue[rr->NextExecuteCmd].Direction;
    rr->RunState = rr->CmdQueue[rr->NextExecuteCmd].State;
    rr->RunCmdCounter = rr->CmdQueue[rr->NextExecuteCmd].RunStep;
    if(rr->RunDir>=0){ // устанавливаем направление вращения
        SetDirection(rr->Index,1);
    } else {
        SetDirection(rr->Index,0);
    }
    return 0;
}



int PushCmdToQueue(RR * rr, GD_STATE State, double Vend, double Xend, int Direction )
{
    LONG Xbreak;
    WORD LastCmd;
    double Vbeg = 0.0;
    double Xbeg = 0.0;
    if(rr->CmdCount < CQ_SIZE){
        rr->CmdQueue[rr->NextWriteCmd].State = State;
        rr->CmdQueue[rr->NextWriteCmd].Vend = Vend;
        rr->CmdQueue[rr->NextWriteCmd].Xend = Xend;
        rr->CmdQueue[rr->NextWriteCmd].Direction = Direction;
        
        //CalculateBreakParam(rr, &Xbreak);

        if(rr->CmdCount > 0){
            if(rr->NextWriteCmd > 0){
                LastCmd = rr->NextWriteCmd - 1;
            } else {
                LastCmd = CQ_SIZE - 1;
            }
            Vbeg = rr->CmdQueue[LastCmd].Vend;
            Xbeg = rr->CmdQueue[LastCmd].Xend;
        }    
        CalculateBreakParam(rr, State, Direction, Vbeg, Xbeg, 
            &rr->CmdQueue[rr->NextWriteCmd].Vend, 
            &rr->CmdQueue[rr->NextWriteCmd].Xend, &Xbreak);
         rr->CmdQueue[rr->NextWriteCmd].RunStep = Xbreak;

        rr->NextWriteCmd++;
        if(rr->NextWriteCmd >= CQ_SIZE)rr->NextWriteCmd -= CQ_SIZE;       
        rr->CmdCount++;
    } else return -1;
    if((rr->CmdCount>0)&&(rr->CacheState == ST_STOP)){
        CacheNextCmd(rr);
    }
    return 0;
}

int ProcessOC(RR * rr)
{
    DWORD Timer = 0;
    if(rr->DataCount>0){
        if(rr->DataCount <= BUF_SIZE_2) {
#ifdef __C30__
            switch(rr->Index){
            case 1: 
                IFS1bits.U2RXIF = 1;    //  1 = вызов просчета буфера   	
                break;
            case 2: 
                IFS1bits.U2TXIF = 1;    //  1 = вызов просчета буфера   	
             break;
            case 3: 
                IFS0bits.U1RXIF = 1;    //  1 = вызов просчета буфера   	
                break;
            }
#else
            if(rr->DataCount == 0)
                Control(rr);
#endif
        }
    } else {
        rr->RunState = ST_STOP;
        DisableOC(rr->Index);
        return 0;
    }
    rr->T.Val = rr->IntervalArray[rr->NextReadFrom];
    rr->NextReadFrom++;
    if(rr->NextReadFrom >= BUF_SIZE) rr->NextReadFrom -= BUF_SIZE;
    
    SetOC(rr->Index, rr->T.word.LW);
    Timer = GetBigTmrValue(rr->TmrId);
    if((Timer <= rr->T.Val)&&(Timer + 0x00010020 >= rr->T.Val)){
        EnableOC(rr->Index);
    }
    if(rr->RunDir >= 0){
        rr->XPosition++;
    } else {
        rr->XPosition--;
    }
    rr->RunCmdCounter--;
    if(rr->RunCmdCounter <= 0 ){
        rr->NextExecuteCmd++;
        if(rr->NextExecuteCmd >= CQ_SIZE) rr->NextExecuteCmd -= CQ_SIZE;
        SetRunState(rr);
    }
    if(rr->DataCount > 0) rr->DataCount--;
    return 0;
}

int SetOC(BYTE oc, WORD LW)
{
    switch (oc){
    case 1:
#ifdef __C30__
        OC1CONbits.OCM = 0b000; // выключить модуль OC
        OC1RS = LW + 50; // записать значение OCxRS
#endif // __C30__
        OC1R = LW;		// записать значение OCxR
        break;
    case 2:
#ifdef __C30__
        OC2CONbits.OCM = 0b000; // выключить модуль OC
        OC2R = LW;		// записать значение OCxR
        OC2RS = LW + 50; // записать значение OCxRS
#endif // __C30__
        break; 
    case 3:
#ifdef __C30__
        OC3CONbits.OCM = 0b000; // выключить модуль OC
        OC3R = LW;		// записать значение OCxR
        OC3RS = LW + 50; // записать значение OCxRS
#endif // __C30__
        break; 
    case 4:
#ifdef __C30__
        OC4CONbits.OCM = 0b000; // выключить модуль OC
        OC4R = LW;		// записать значение OCxR
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
    case 1:
        OC1CONbits.OCM = 0b100; // выключить модуль OC	   	
        break;
    case 2:
        OC2CONbits.OCM = 0b100; // выключить модуль OC	   	
        break; 
    case 3:
        OC3CONbits.OCM = 0b100; // выключить модуль OC	   	
        break; 
    case 4:
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
    case 1:
        OC1CONbits.OCM = 0b000; // выключить модуль OC	
        IFS0bits.OC1IF = 0;     // Clear OC1 interrupt flag   	
        break;
    case 2:
        OC2CONbits.OCM = 0b000; // выключить модуль OC	 
        IFS0bits.OC2IF = 0; // Clear OC1 interrupt flag  	
        break; 
    case 3:
        OC3CONbits.OCM = 0b000; // выключить модуль OC	  
        IFS1bits.OC3IF = 0; // Clear OC1 interrupt flag 	
        break; 
    case 4:
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
    case 1:
        if(OC1CONbits.OCM == 0b000) return 1; 
        break;
    case 2:
        if(OC2CONbits.OCM == 0b000) return 1;
        break; 
    case 3:
        if(OC3CONbits.OCM == 0b000) return 1;
        break; 
    case 4:
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
    case 1:
        PORT1_DIR = Dir;
        break;
    case 2:
        PORT2_DIR = Dir;
        break; 
    case 3:
        PORT3_DIR = Dir;
        break; 
    case 4:
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


int CalculateBreakParam(RR * rr, GD_STATE State, int Direction, double Vbeg, double Xbeg, double * Vend, double * Xend, LONG * Xbreak)
{
    double VKpB;
    double dTb;
    double dTe;
    LONG Xe;
    LONG Xb;
    LONG dX;
    LONG dX2;
    double XX;
    double D;   
    double d = rr->K/(2.0 * rr->B);    
    double a = 4.0 * rr->B/(rr->K * rr->K);   
    double T2 = 0.0;
    double OmKT;
    double XmX;

    switch (State) {
        case ST_ACCELERATE:
        case ST_DECELERATE:
            if(Vbeg!=0.0){
                VKpB = Vbeg * rr->K + rr->B;
                D = rr->B * VKpB;
                dTb = ((-VKpB + sqrt(D))/(-rr->K * VKpB));
                Xb = (LONG)((rr->B * dTb * dTb) / ((1 - rr->K * dTb)*rr->dx));
            } else Xb = 0;

            if((*Vend) != 0.0){
                VKpB = (*Vend) * rr->K + rr->B;
                D = rr->B * VKpB;
                dTe = ((-VKpB + sqrt(D))/(-rr->K * VKpB));
                Xe = (LONG)((rr->B * dTe * dTe) / ((1 - rr->K * dTe)*rr->dx));
            } else Xe = 0;
            if(State != ST_DECELERATE){
                dX = Xe - Xb;
            } else {
                dX = Xb - Xe;
            }
            if(Direction >=0 ){   
                // количество шагов на которое сдвинулись                
                XmX = (*Xend) - Xbeg;                
            } else {                                
                XmX = Xbeg - (*Xend);
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
                    T2 = ((-XX - sqrt(D))*d);
                }
                OmKT = (1 - rr->K * T2);
                (*Vend) = (rr->B * T2*( 1.0 + OmKT))/(OmKT * OmKT); // вычислить скорость  V = BT/(1-KT)  T = 
            } else {
                // если координата перемещения больше, чем координата при заданной скорости  
                if(Direction >=0 ){
                    (*Xbreak) = dX;                    
                    (*Xend) = Xbeg + dX * rr->dx;
                } else {
                    (*Xbreak) = dX;
                    (*Xend) = Xbeg - dX * rr->dx;
                }
            }            
            break; 
        case ST_RUN: 
            if(rr->CmdCount == 0) {
                return -1;  // это ошибка: перед командой ST_RUN должна быть еще команда
                // т.к. увеличение счетчика после, то мы таким образом не исполняем команду
            } else {
                if(Direction >=0 ){   
                    // количество шагов на которое сдвинулись                
                    XmX = (*Xend) - Xbeg;                
                } else {                                
                    XmX = Xbeg - (*Xend);
                }  
                (*Xbreak) = (LONG)(XmX /rr->dx);               
                (*Vend) = Vbeg;                
            }
            break;
        case ST_BUFFER_FREE:
        case ST_STOP:
            (*Xbreak) = 0;
            (*Vend) = 0;
            (*Vend) = Xbeg;
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

int GoToCmd(RR * rr, double VTarget, double XTarget)
{
    if((rr == NULL) || (VTarget == 0.0) || (XTarget == 0.0)) 
        return -1;
    if((rr->RunState != ST_STOP)||(rr->CacheState != ST_STOP)){
        // прервать выполнение текущей команды и остановить движение
        //BreakCurrentCmd(rr);
        if(rr->CalcDir>=0){
            PushCmdToQueue(rr, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, rr->CalcDir);
        } else {
            PushCmdToQueue(rr, ST_DECELERATE, 0.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, rr->CalcDir);
        }
        // тут мы уже знаем какая координата будет
        // как-то нужно дождаться выполнения команды Deceleration
    }
    // V == 0;
    //X == rr->XPosition;
    // теперь два варианта : количество шагов до нужного положения     
    // 1. больше, чем разгон до максимума + торможение до нужной скорости
    //    => вычисляем сумму разгон+ торможение + движение по линейному закону
    // 2. меньше
    //    => вычисляем максимальную скорость до которой успеем разогнаться
    return 0;
}