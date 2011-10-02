#define __OCTIMER_C
#include "TCPIP Stack/TCPIP.h"
#include "OCTimer.h"

int TimerOverflow(BYTE TmrNum);
int UpdateOCTimer(BYTE Num);
int EnableOCTimer(BYTE Num);
int DisableOCTimer(BYTE Num);

void __attribute__((__interrupt__,__no_auto_psv__)) _OC1Interrupt( void )
{
    UpdateOCTimer(1);
    IFS0bits.OC1IF = 0; // Clear OC1 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC2Interrupt( void )
{
    UpdateOCTimer(2);
    IFS0bits.OC2IF = 0; // Clear OC2 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC3Interrupt( void )
{
    UpdateOCTimer(3);
    IFS1bits.OC3IF = 0; // Clear OC3 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC4Interrupt( void )
{
    UpdateOCTimer(4);
    IFS1bits.OC4IF = 0; // Clear OC4 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC5Interrupt( void )
{
    UpdateOCTimer(5);
    IFS2bits.OC5IF = 0; // Clear OC5 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC6Interrupt( void )
{
    UpdateOCTimer(6);
    IFS2bits.OC6IF = 0; // Clear OC6 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC7Interrupt( void )
{
    UpdateOCTimer(7);
    IFS2bits.OC7IF = 0; // Clear OC7 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _OC8Interrupt( void )
{
    UpdateOCTimer(8);
    IFS2bits.OC8IF = 0; // Clear OC1 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _T2Interrupt( void )
{
	TimerOverflow(2);
    IFS0bits.T2IF = 0; // Clear T2 interrupt flag
}
void __attribute__((__interrupt__,__no_auto_psv__)) _T3Interrupt( void )
{
	TimerOverflow(3);
    IFS0bits.T3IF = 0; // Clear T3 interrupt flag
}

int OCInit(void)
{
	BYTE i;	
	for(i = 0; i < 8; i++){
		OCTimers[i].Flags = 0;			
		OCTimers[i].PeriodsLeft = 0;
		OCTimers[i].OCxCON = 0;
		OCTimers[i].StepsLeft.Val = 0;
		OCTimers[i].Period.Val = 0;
		OCTimers[i].Pulse.Val = 0;
		OCTimers[i].Periods = NULL; 
		OCTimers[i].Pulses = NULL;
		
	}
	// MS1 = 1; MS2 = 1; 1/16
	MS1 = 1; 		 // выход MS1  
	MS2 = 1; 		 // выход MS2  
	SLEEP = 0; 		 // выход SLEEP
	RESET = 0; 		 // выход RESET
	
	MS1_Tris = 0; 	 // выход MS1
	MS2_Tris = 0; 	 // выход MS2
	SLEEP_Tris = 0;	 // выход SLEEP		
	RESET_Tris = 0;	 // выход RESET	
	
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
		    T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler 25ns
		    TMR2 = 0x00; 			// Clear timer register
		    PR2 = 0xFFFF; 			// Load the period value
		    IPC1bits.T2IP = 0x01; 	// Set Timer2 Interrupt Priority Level
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

int UpdateOCTimer(BYTE Num)
{
	return 0;
}
int TimerOverflow(BYTE TmrNum)
{
	BYTE i;
	for(i=0;i<8;i++){
		if(OCTimers[i].Flags&&OC_TIMER_IS_USE){
			switch(TmrNum){
			case 3:
				if(OCTimers[i].Flags&OC_TIMER_USE_TMR3){
					if(OCTimers[i].Period.word.HW != 0){
						OCTimers[i].Period.word.HW--;
						if(OCTimers[i].Period.word.HW ==0) EnableOCTimer(i);
					}	
				} 
			break;
			case 2:
				if(!OCTimers[i].Flags&OC_TIMER_USE_TMR3){
					if(OCTimers[i].Period.word.HW != 0){
						OCTimers[i].Period.word.HW--;
						if(OCTimers[i].Period.word.HW ==0) EnableOCTimer(i);
					}	
				} 
			break;
			default:
				return -1;				
			}	
		}
	}	
	return 0;
}

int OCTimerInit(BYTE num,  	// номер таймера	
	DWORD Steps,			// количество шагов, на которые нужно сдвинуться	
	DWORD * Periods,		// указатель на массив периодов
	DWORD * Pulses,			// указатель на массив пульсов
	BYTE BufferLen,			// размер буфера периодов
	BYTE TmrNum,			// Номер таймера (2 или 3)
	BYTE ocm				// настройки
	)
{
	BYTE i = num -1;
	
	switch(num){
	case 1:
		OCTimers[i].Periods = Periods;
		OCTimers[i].Pulses  = Pulses;
		
		PORT1_NULL_Tris   = 1; // вход NULL
		PORT1_POS_Tris    = 1; // вход POS
		PORT1_POS2_Tris   = 1; // вход POS2	
		
		PORT1_ENABLE 	  = 0;// выход ENABLE   
		PORT1_DIR 		  = 0;// выход DIR      
		PORT1_STEP 		  = 0;// выход STEP	

		PORT1_ENABLE_Tris = 0;// выход ENABLE
		PORT1_DIR_Tris    = 0;// выход DIR
		PORT1_STEP_Tris   = 0;// выход STEP		
				
	break;
	}
	return 0;		
}

int EnableOCTimer(BYTE Num)
{
	return 0;
}

int DisableOCTimer(BYTE Num)
{
	return 0;
}
