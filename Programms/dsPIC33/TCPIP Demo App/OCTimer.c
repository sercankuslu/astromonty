#define __OCTIMER_C
#include "TCPIP Stack/TCPIP.h"
#include "OCTimer.h"
#include "math.h"


static DWORD_VAL Timer2Big;
static DWORD_VAL Timer3Big;
static RR rr1;
//static RR rr2;
//static RR rr3;

int InitRR(RR * rr);

void __attribute__((__interrupt__,__no_auto_psv__)) _OC1Interrupt( void )
{       
   	if(rr1.DataCount>0){
       	rr1.DataCount--;
       	if((IFS1bits.U2RXIF != 1) && (rr1.DataCount <= BUF_SIZE_2)) {
            IFS1bits.U2RXIF = 1;    //  1 = вызов просчета буфера   	
       	}
   	} else {
   	    rr1.RunState = ST_STOP;
   	    OC1CONbits.OCM = 0b000; // выключить модуль OC
   	    IFS0bits.OC1IF = 0; // Clear OC1 interrupt flag
   	    return;
   	}
	rr1.T.Val = rr1.IntervalArray[rr1.NextReadFrom]; 	
   	rr1.NextReadFrom++;
   	if(rr1.NextReadFrom >= BUF_SIZE) rr1.NextReadFrom -= BUF_SIZE;
   	
   	OC1CONbits.OCM = 0b000; // выключить модуль OC	   	
   	OC1R = rr1.T.word.LW;		// записать значение OCxR
   	OC1RS = rr1.T.word.LW + 50; // записать значение OCxRS
   	Timer2Big.word.LW = TMR2;   	
    if((Timer2Big.Val <= rr1.T.Val)&&(Timer2Big.Val + 0x00010000 >= rr1.T.Val)){     
	    OC1CONbits.OCM = 0b100;	// включить модуль OC
	}        		

    IFS0bits.OC1IF = 0; // Clear OC1 interrupt flag
    
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC2Interrupt( void )
{

    IFS0bits.OC2IF = 0; // Clear OC2 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC3Interrupt( void )
{

    IFS1bits.OC3IF = 0; // Clear OC3 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC4Interrupt( void )
{

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
    Timer2Big.word.LW = TMR2;	
    
    if(OC1CONbits.OCM == 0b000){
    	if(rr1.RunState != ST_STOP){               	
            if((Timer2Big.Val <= rr1.T.Val)&&(Timer2Big.Val + 0x00010000 >= rr1.T.Val)){     
        	    OC1CONbits.OCM = 0b100;	// включить модуль OC
        	}        		
    	}
	}
    IFS0bits.T2IF = 0; // Clear T2 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _T3Interrupt( void )
{
	Timer3Big.word.HW++;
    Timer3Big.word.LW = TMR3;
    IFS0bits.T3IF = 0; // Clear T3 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt( void )
{
    Control(&rr1);    
    IFS1bits.U2RXIF = 0;
} 

int OCInit(void)
{
	//BYTE i;		
	
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
	IFS1bits.U2RXIF = 0;    //  1 = вызов 
	
	//Инициализация порта1
	{
    	
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
    
        TmrInit(2);
        InitRR(&rr1);        
        //InitRR(&rr2);
        //InitRR(&rr3);
        IFS1bits.U2RXIF = 1;
        
        // инициализация OC1
    	{
        	OC1CONbits.OCM = 0b000;
            OC1CONbits.OCTSEL = 0;  	// выбрать Timer2
            IPC0bits.OC1IP = 7;		    // выбрать приоритет прерывания для OC1
            IFS0bits.OC1IF = 0;			// сбросить флаг прерывания
            IEC0bits.OC1IE = 1;			// разрешаем прерывания от OC1    
        }
        IFS0bits.OC1IF = 1;
    }       
    
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
			T2CONbits.TON = 0; 		// Disable Timer
		    T2CONbits.TCS = 0; 		// Select internal instruction cycle clock
		    T2CONbits.TGATE = 0; 	// Disable Gated Timer mode
		    T2CONbits.TCKPS = 0b01; // Select 8:1 Prescaler 200ns
		    TMR2 = 0x00; 			// Clear timer register
		    PR2 = 0xFFFF; 			// Load the period value
		    IPC1bits.T2IP = 7; 	// Set Timer2 Interrupt Priority Level
		    IFS0bits.T2IF = 0; 		// Clear Timer2 Interrupt Flag
		    IEC0bits.T2IE = 1; 		// Enable Timer2 interrupt
		    T2CONbits.TON = 1; 		// Start Timer   
		    break;
		case 3:		
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
			break;
		default: return -1;
	}		
	return 0;
}

int InitRR(RR * rr)
{
    double I;
    rr->Mass = 500.0f;
    rr->Radius = 0.30f;
    rr->Length = 3.0f;
    rr->Reduction = 360.0f;
    I = ((rr->Mass*rr->Radius*rr->Radius/4) + (rr->Mass*rr->Length*rr->Length/12))/rr->Reduction;     
    rr->K = (-0.000349812 * 200 * 180/PI)/I;
    rr->B = 0.751428571 / I;
    rr->dx = PI/(180.0*200.0*16.0); // шаг перемещения в радианах
    rr->TimerStep = 0.0000002; // шаг таймера
    rr->Vend = 0 * Grad_to_Rad;
    rr->Xend = 0 * Grad_to_Rad; // здесь удвоенная координата. т.к. после ускорения сразу идет торможение
    rr->DataCount = 0;
    rr->NextWriteTo = 0;
    rr->NextReadFrom = 0;    
    rr->XaccBeg = 0;
    rr->Xbeg = 0;
    Timer2Big.word.LW = TMR2;
    rr->TimeBeg = Timer2Big.Val + (ARR_TYPE)(1.0 /* * BUF_SIZE*/ /rr->TimerStep);       
    rr->State = ST_STOP;    
    rr->RunState = ST_RUN;
    rr->T.Val = 0;
    rr->Interval = 0xFFFFFFFF;
    rr1.CmdCount = 0;
    rr->CalcDir = 1;
    rr1.NextReadCmd = 0;
    rr1.NextWriteCmd = 0;
    PushCmdToQueue(&rr1, ST_ACCELERATE, 1.0 * Grad_to_Rad, 0 );
    PushCmdToQueue(&rr1, ST_RUN, 0.0 * Grad_to_Rad, 5.0 * Grad_to_Rad );
    PushCmdToQueue(&rr1, ST_DECELERATE, 0, 0 );
    PushCmdToQueue(&rr1, ST_STOP, 0, 0 );
    SetNextState(&rr1);
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
    //X = rr->Vend * rr->TimeBeg;
    DWORD Xb = (DWORD)(rr->Xbeg/rr->dx);
    DWORD Xe = (DWORD)(rr->Xend/rr->dx);
    ARR_TYPE T = 0;
    ARR_TYPE T1 = 0;// = 0;
    ARR_TYPE T2 = 0;
    ARR_TYPE e;
    e = (ARR_TYPE)(0.000070 / rr->TimerStep); //70us
    if(rr->Vend == 0.0){
        if(rr->Interval > 0)
            rr->Vend = rr->dx / (rr->Interval*rr->TimerStep);
    }

    for( i = 0; i < FreeData; i++) {       
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;                                              
        if(k == 0) {
            if(rr->Interval < e){
                //if(m < 32) m++;
                m = 32;
            } else {
                //if( m > 1) m--;
                m = 1;
            } 
            X += rr->dx*m;
            T2 = (ARR_TYPE)(X / (rr->Vend * rr->TimerStep));
            rr->Interval = (T2 - T1) / m;            
        }             
        Xb+=rr->CalcDir;
        T += rr->Interval;
        if(((rr->CalcDir > 0)&&(Xe != 0)&&(Xb >= Xe))||
           ((rr->CalcDir < 0)&&(Xe != 0)&&(Xb <= Xe))){
            FreeData = i;
            SetNextState(rr);
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = rr->TimeBeg + T;                    
        T1 = T;            
    }

    rr->TimeBeg += T1;        
    rr->DataCount += FreeData;
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->Xbeg = Xb * rr->dx;
    return 0;
}
// разгон с текущей скорости до требуемой
//    ARR_TYPE    TimeBeg;  
//    DWORD       XaccBeg;                    //(желательно целое число шагов)


int Acceleration(RR * rr)
{
    WORD j;        
    WORD FreeData = BUF_SIZE - rr->DataCount;
    ARR_TYPE T = 0;
    ARR_TYPE T1 = 0;
    ARR_TYPE T2 = 0;    
    ARR_TYPE dT = 0;
    double X;       // временная переменная 
    ARR_TYPE Tb = 0;  
    double D;      
    LONG Xb = (LONG)(rr->Xbeg/rr->dx);
    LONG Xe = (LONG)(rr->Xend/rr->dx);
    double K = rr->K;
    double B = rr->B;
    double VKpB = 0.0; 
    
    double dx;
    double a;
    double c;
    double d;    
    WORD i = 0;
    ARR_TYPE e;
    WORD k = 0;
    WORD m = 1;    
    
    e = (ARR_TYPE)(0.000070 / rr->TimerStep); //70us
    dx = rr->dx;
    X = rr->XaccBeg * rr->dx; 
    d = K/(2.0 * B * rr->TimerStep);
    c = K*d;
    a = 4.0 * B/(K*K);   
//     Tb = rr->TimeBeg - rr->T1;       	
//     T1 = rr->T1;  
    if(rr->XaccBeg > 0){
        X = rr->XaccBeg * rr->dx; 
        D = X *(X + a);
        if(D >= 0.0){
            T1 = (ARR_TYPE)((-X - sqrt(D))*d);
        }  
    } else T1 = 0;
    Tb = rr->TimeBeg - T1;
    T = T1;

    // вычисление времени окончания
    if(rr->Vend != 0.0){       
        VKpB = rr->Vend * K + B;
        D = B * VKpB;
        dT = (ARR_TYPE)((-VKpB + sqrt(D))/(-K * VKpB * rr->TimerStep));
    }
    // оптимизировано 35 uSec (1431.5 тактов за шаг)
    for( i = 0; i < FreeData; i++) {       
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;                                              
        if(k == 0) {
            if(rr->Interval < e){
                //if(m < 32) m++;
                m = 32;
            } else {
                //if( m > 1) m--;
                m = 1;
            } 
            X += dx*m;
            D = X *(X + a);
            if(D >= 0.0){
                T2 = (ARR_TYPE)((-X - sqrt(D))*d);
            }                
            rr->Interval = (T2 - T1) / m;            
        }             
        Xb+=rr->CalcDir;
        T += rr->Interval;

        if(((dT != 0)&&(T >= dT))||
            ((rr->CalcDir > 0)&&(Xe != 0)&&(Xb >= Xe))||
            ((rr->CalcDir < 0)&&(Xe != 0)&&(Xb <= Xe))){
            FreeData = i;
            SetNextState(rr);
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = Tb + T;                    
        T1 = T;            
    }    
    rr->TimeBeg = Tb + T1;
    rr->XaccBeg += FreeData; 
    rr->DataCount += FreeData;
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->Xbeg = Xb * rr->dx;
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
    ARR_TYPE dT = 0;
    double X;       // временная переменная 
    ARR_TYPE Tb = 0;  
    double D;      
    LONG Xb = (LONG)(rr->Xbeg/rr->dx);
    LONG Xe = (LONG)(rr->Xend/rr->dx);
    double K = rr->K;
    double B = rr->B;
    double VKpB = 0.0; 

    double dx;
    double a;
    double c;
    double d;    
    WORD i = 0;
    ARR_TYPE e;
    WORD k = 0;
    WORD m = 1;
    e = (ARR_TYPE)(0.000070 / rr->TimerStep); //70us
    dx = rr->dx;
    X = rr->XaccBeg * rr->dx; 
    d = K/(2.0 * B * rr->TimerStep);
    c = K*d;
    a = 4.0 * B/(K*K);    

//     Tb = rr->TimeBeg + rr->T1;       	
//     T1 = rr->T1;
    X = rr->XaccBeg * rr->dx; 
    if(rr->XaccBeg > 0){        
        D = X *(X + a);
        if(D >= 0.0){
            T1 = (ARR_TYPE)((-X - sqrt(D))*d);
        } 
    }
    Tb = rr->TimeBeg + T1;
    T = T1; 


    if(rr->Vend != 0.0){        
        VKpB = rr->Vend * K + B;
        dT = (ARR_TYPE)((-VKpB + sqrt(B * VKpB))/(-K * VKpB * rr->TimerStep));
    }

    for( i = 0; i < FreeData; i++) {    
        // "грубые" вычисления
        j = rr->NextWriteTo + i;
        if(j >= BUF_SIZE) j -= BUF_SIZE;                                              
        if(k == 0) {
            if(rr->Interval < e){                
                m = 32;
                //if( m < 32 ) m += 8;
            } else {
                //if( m > 1 ) m -= 8;
                m = 1;
            } 
            X -= dx*m;
            if(X<=0.0){
                FreeData = i;
                SetNextState(rr);
                break;
            }
            D = X *(X + a);
            if(D >= 0.0){
                T2 = (ARR_TYPE)((-X - sqrt(D))*d);
            }                
            rr->Interval = (T1 - T2) / m;
        }             
        Xb += rr->CalcDir;
        T -= rr->Interval;
        if(((dT != 0)&&(T >= dT))||
            ((rr->CalcDir > 0)&&(Xe != 0)&&(Xb >= Xe))||
            ((rr->CalcDir < 0)&&(Xe != 0)&&(Xb <= Xe))){
            FreeData = i;
            SetNextState(rr);
            break;
        }
        k++;
        if(k >= m){
            T = T2;
            k = 0;
        } 
        rr->IntervalArray[j] = Tb - T;                    
        T1 = T;      
    }   
    rr->TimeBeg = Tb - T1;
    rr->XaccBeg -= FreeData; 
    rr->DataCount += FreeData;
    rr->NextWriteTo += FreeData;
    if(rr->NextWriteTo >= BUF_SIZE) rr->NextWriteTo -= BUF_SIZE;
    rr->Xbeg = Xb * rr->dx;
    return 0;
}

int Control(RR * rr)
{
	while((rr->DataCount <= BUF_SIZE_2)&&(rr->State != ST_STOP)){
		switch(rr->State){
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
		case ST_FREE:
		break;
	    }
	}    
    return 0;
}

int SetNextState(RR * rr)
{
    if(rr->CmdCount > 0){
        rr->State = rr->CmdQueue[rr->NextReadCmd].State;        
        rr->Vend  = rr->CmdQueue[rr->NextReadCmd].Vend;
        rr->Xend  = rr->CmdQueue[rr->NextReadCmd].Xend;
        rr->NextReadCmd++;
        if(rr->NextReadCmd >= CQ_SIZE)rr->NextReadCmd -= CQ_SIZE;
        rr->CmdCount--;
    } else {
        rr->State = ST_STOP;
    }
    return 0;
}

int PushCmdToQueue(RR * rr, GD_STATE State, double Vend, double Xend )
{
    if(rr->CmdCount < CQ_SIZE){
        rr->CmdQueue[rr->NextWriteCmd].State = State;
        rr->CmdQueue[rr->NextWriteCmd].Vend = Vend;
        rr->CmdQueue[rr->NextWriteCmd].Xend = Xend;
        rr->NextWriteCmd++;
        if(rr->NextWriteCmd >= CQ_SIZE)rr->NextWriteCmd -= CQ_SIZE;
        rr->CmdCount++;
    } else return -1;
}