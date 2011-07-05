
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "Threads.h"
#include <spi.h>
#include <dsp.h>
//#include "timer.h"

//
// PIC18 Interrupt Service Routines
// 
// NOTE: Several PICs, including the PIC18F4620 revision A3 have a RETFIE FAST/MOVFF bug
// The interruptlow keyword is used to work around the bug when using C18
#if defined(__18CXX)
	#if defined(HI_TECH_C)
	void interrupt low_priority LowISR(void)
	#else
	#pragma interruptlow LowISR
	void LowISR(void)
	#endif
	{
	    //TickUpdate();
	}
	
	#if defined(HI_TECH_C)
	void interrupt HighISR(void)
	#else
	#pragma interruptlow HighISR
	void HighISR(void)
	#endif
	{
	    
	}

	#if !defined(HI_TECH_C)
	#pragma code lowVector=0x18
	void LowVector(void){_asm goto LowISR _endasm}
	#pragma code highVector=0x8
	void HighVector(void){_asm goto HighISR _endasm}
	#pragma code // Return to default code section
	#endif

// C30 and C32 Exception Handlers
// If your code gets here, you either tried to read or write
// a NULL pointer, or your application overflowed the stack
// by having too many local variables or parameters declared.
#elif defined(__C30__)
	void _ISR __attribute__((__no_auto_psv__)) _AddressError(void)
	{
	    Nop();
		Nop();
	}
	void _ISR __attribute__((__no_auto_psv__)) _StackError(void)
	{
	    Nop();
		Nop();
	}
	
#elif defined(__C32__)
	void _general_exception_handler(unsigned cause, unsigned status)
	{
		Nop();
		Nop();
	}
#endif
BYTE Test(BYTE a)
{
    BYTE i;
    DWORD j;
    double k=0.0;  
    double kk=0.0;   
    for(i=0;i<50;i++){
        for(j=1;j<360;j++)
        {
            k=(double)i*sin((double)j*PI/180.0);
            Nop();
            Nop();
            kk=(double)i*sin((double)j*PI/180.0);
            if(k!=kk){
                Nop();
            }         
        }                       
    }
    return 7;
}
void Process1 (void) __attribute__ ((noreturn));
void Process1 (void)
{    
    while(1){
        Nop();
        Nop();        
        Test(43);
    }
}
void Process2 (void) __attribute__ ((noreturn));
void Process2 (void)
{    
    while(1){
        Nop();
        Nop();
        Test(32);
    }
}
void Process3 (void) __attribute__ ((noreturn));
void Process3 (void)
{    
    while(1){
        Nop();
        Nop();        
        Test(43);
    }
}
void Process4 (void) __attribute__ ((noreturn));
void Process4 (void)
{    
    while(1){
        Nop();
        Nop();
        Test(32);
    }
}
//
// Main application entry point.
//
#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{
    BYTE T;
    T=LATAbits.LATA0;
   	InitMultiTasking();
    StartProcess(&Process1,100);   
    StartProcess(&Process2,100); 
    StartProcess(&Process3,100);   
    StartProcess(&Process4,100); 
    StartMultiTasking();
    //Process1();
    //Process2();
loop: //	while(1)
    
        // Blink LED0 
        //LATAbits.LATA0 ^= 1;
       
	
	goto loop;
	
}

