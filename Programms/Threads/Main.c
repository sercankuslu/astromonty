
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "Threads.h"
#include <spi.h>
#include <dsp.h>
#include "SpiDriver.h"
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
    BYTE i=173;
    DWORD j=2020202020;
    double k=0.0;  
    double kk=0.0;   
    /*for(i=0;i<50;i++){
        j=i;
        //for(j=1;j<360;j++)
        //{*/
            k=(double)i*sin((double)j*PI/180.0);
            Nop();
            Nop();
            //kk=(double)i*sin((double)j*PI/180.0);
            /*
            if(k!=kk){
                Nop();
            }         
        //}                       
    }*/
    LATGbits.LATG1 ^= 1;
    return 7;
}
BYTE Test2(BYTE a)
{
    BYTE i=17;
    DWORD j=1124562;
    double k=0.0;  
    double kk=0.0;   
    /*
    for(i=0;i<50;i++){
        j=i;
        //for(j=1;j<360;j++)
        //{*/
            k=(double)i*sin((double)j*PI/180.0);
            Nop();
            Nop();
            //kk=(double)i*sin((double)j*PI/180.0);
            //if(k!=kk){
            //    Nop();
            //}         
        //}                       
    //}*/
    LATGbits.LATG0 ^= 1;
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
        Test2(32);
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

void CS_On()
{
    LATGbits.LATG9=0;
}
void CS_Off()
{
    LATGbits.LATG9=1;
    TRISGbits.TRISG9=0;    
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
    WORD T;
    BYTE DevId;
    BYTE REV=0;
    //DWORD* D;
    CLKDIV|=0x4; //PLLPRE 4 ; PLLPOST 2
    PLLFBD = 0x7f;
    LATFbits.LATF0=1;
    TRISFbits.TRISF0=0;   
    TRISGbits.TRISG0=0;
    TRISGbits.TRISG1=0;
    
    
    //SPI1_Init(D);
    //CS_Control(1);
    //SPI1_Init(&CS_On,&CS_Off);
    SPI_Init();
    DevId=SPI_RegDevice(2,2,8000000, &CS_On,&CS_Off); 
    SPI_Open(DevId);
    LATFbits.LATF0=1;
    for(T=0;T<65000;T++){
        Nop();
        Nop();
    }
    //bank3 0x12; EREVID
    //переключение на банк 3
    //1. по адресу 1F записать 0x03
    //команда 5F 1F
    SPI2BUF=0x5F;
    while ((SPI2STATbits.SPITBF == 1) || (SPI2STATbits.SPIRBF == 0));
    SPI2BUF=0x1F;
    while ((SPI2STATbits.SPITBF == 1) || (SPI2STATbits.SPIRBF == 0));
    //чтение регистра
    //2. 12 00  
    SPI2BUF=0x12;
    while ((SPI2STATbits.SPITBF == 1) || (SPI2STATbits.SPIRBF == 0));
    SPI2BUF=0x00;
    while ((SPI2STATbits.SPITBF == 1) || (SPI2STATbits.SPIRBF == 0));
    REV = SPI2BUF;
   	InitMultiTasking();
    StartProcess(&Process1,50);   
    StartProcess(&Process2,50); 
    //StartProcess(&Process3,100);   
    //StartProcess(&Process4,100); 
    StartMultiTasking();
    //Process1();
    //Process2();
loop: //	while(1)
    
        // Blink LED0 
        //LATAbits.LATA0 ^= 1;
       
	
	goto loop;
	
}

