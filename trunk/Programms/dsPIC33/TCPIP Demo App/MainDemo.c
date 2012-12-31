/*********************************************************************
 *
 *  Main Application Entry Point and TCP/IP Stack Demo
 *  Module for Microchip TCP/IP Stack
 *   -Demonstrates how to call and use the Microchip TCP/IP stack
 *     -Reference: Microchip TCP/IP Stack Help (TCPIP Stack Help.chm)
 *
 *********************************************************************
 * FileName:        MainDemo.c
 * Dependencies:    TCPIP.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.11b or higher
 *                    Microchip C30 v3.24 or higher
 *                    Microchip C18 v3.36 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *        ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *        used in conjunction with a Microchip ethernet controller for
 *        the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author              Date         Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti        4/19/01        Original (Rev. 1.0)
 * Nilesh Rajbharti        2/09/02        Cleanup
 * Nilesh Rajbharti        5/22/02        Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti        7/9/02        Rev 2.1 (See version.log for detail)
 * Nilesh Rajbharti        4/7/03        Rev 2.11.01 (See version log for detail)
 * Howard Schlunder        10/1/04        Beta Rev 0.9 (See version log for detail)
 * Howard Schlunder        10/8/04        Beta Rev 0.9.1 Announce support added
 * Howard Schlunder        11/29/04    Beta Rev 0.9.2 (See version log for detail)
 * Howard Schlunder        2/10/05        Rev 2.5.0
 * Howard Schlunder        1/5/06        Rev 3.00
 * Howard Schlunder        1/18/06        Rev 3.01 ENC28J60 fixes to TCP, 
 *                                    UDP and ENC28J60 files
 * Howard Schlunder        3/01/06        Rev. 3.16 including 16-bit micro support
 * Howard Schlunder        4/12/06        Rev. 3.50 added LCD for Explorer 16
 * Howard Schlunder        6/19/06        Rev. 3.60 finished dsPIC30F support, added PICDEM.net 2 support
 * Howard Schlunder        8/02/06        Rev. 3.75 added beta DNS, NBNS, and HTTP client (GenericTCPClient.c) services
 * Howard Schlunder        12/28/06    Rev. 4.00RC added SMTP, Telnet, substantially modified TCP layer
 * Howard Schlunder        04/09/07    Rev. 4.02 added TCPPerformanceTest, UDPPerformanceTest, Reboot and fixed some bugs
 * Howard Schlunder        xx/xx/07    Rev. 4.03
 * HSchlunder & EWood    08/27/07    Rev. 4.11
 * HSchlunder & EWood    10/08/07    Rev. 4.13
 * HSchlunder & EWood    11/06/07    Rev. 4.16
 * HSchlunder & EWood    11/08/07    Rev. 4.17
 * HSchlunder & EWood    11/12/07    Rev. 4.18
 * HSchlunder & EWood    02/11/08    Rev. 4.19
 * HSchlunder & EWood   04/26/08    Rev. 4.50 Moved most code to other files for clarity
 * KHesky               07/07/08    Added ZG2100-specific support
 * HSchlunder & EWood   07/24/08    Rev. 4.51
 * Howard Schlunder        11/10/08    Rev. 4.55
 * Howard Schlunder        04/14/09    Rev. 5.00
 * Howard Schlunder        07/10/09    Rev. 5.10
 * Howard Schlunder        11/18/09    Rev. 5.20
 * Howard Schlunder        04/28/10    Rev. 5.25
 * Howard Schlunder        10/19/10    Rev. 5.31
 ********************************************************************/
/*
 * This macro uniquely defines this file as the main entry point.
 * There should only be one such definition in the entire project,
 * and this file must define the AppConfig variable as described below.
 */
#define THIS_IS_STACK_APPLICATION
#define USE_PROTOCOL_SERVER

// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"
#include <i2c.h>

#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
#include "TCPIP Stack/ZeroconfLinkLocal.h"
#endif
#if defined(STACK_USE_ZEROCONF_MDNS_SD)
#include "TCPIP Stack/ZeroconfMulticastDNS.h"
#endif

// Include functions specific to this stack application
#include "MainDemo.h"
#include "TCPIP Stack/SPIRTCSRAM.h"
#include "OCTimer.h"
#include "../protocol.h"
#include "device_control.h"


// Used for Wi-Fi assertions
#define WF_MODULE_NUMBER   WF_MODULE_MAIN_DEMO

// Declare AppConfig structure and some other supporting stack variables
APP_CONFIG AppConfig;

static unsigned short wOriginalAppConfigChecksum;    // Checksum of the ROM defaults for AppConfig
BYTE AN0String[8];
DWORD_VAL CPUSPEED;

extern RR rr1;
extern RR rr2;
extern RR rr3;
extern RAMSaveConfig RRConfigRAM;
// Use UART2 instead of UART1 for stdout (printf functions).  Explorer 16 
// serial port hardware is on PIC UART2 module.
#if defined(EXPLORER_16) || defined(PIC24FJ256DA210_DEV_BOARD)
    int __C30_UART = 2;
#endif


// Private helper functions.
// These may or may not be present in all applications.
static void InitAppConfig(void);
static void InitializeBoard(void);
static void ProcessIO(void);
void FanControl();

#if defined(WF_CS_TRIS)
    static void WF_Connect(void);
#endif
/*
void __attribute__((__interrupt__,__no_auto_psv__)) _T7Interrupt( void )
{
    static BYTE Up = 0;    
    //T7CONbits.TON = 0;
    if(Up == 1){
        LATBbits.LATB4 = 1;
        Up = 0;
        PR7 = 2000;
    } else {
        LATBbits.LATB4 = 0;
        Up = 1;
        PR7 = 1000;
    }
    //T7CONbits.TON = 1;
    //LATBbits.LATB4 ^= 1;
    IFS3bits.T7IF = 0;
}
*/
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
    TickUpdate();
}
    
#if defined(HI_TECH_C)
void interrupt HighISR(void)
#else
#pragma interruptlow HighISR
void HighISR(void)
#endif
{
    Nop();    
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
    while(1){
        Nop();
        Nop();
    }
}
void _ISR __attribute__((__no_auto_psv__)) _StackError(void)
{
    while(1){
        Nop();
        Nop();
    }
}
void _ISR __attribute__((__no_auto_psv__)) _MathError(void)
{
    while(1){
        Nop();
        Nop();
    }
}  
void _ISR __attribute__((__no_auto_psv__)) _OscillatorFail(void)
{
    while(1){
        Nop();
        Nop();
    }
}   
void _ISR __attribute__((__no_auto_psv__)) _DMACError(void)
{
    while(1){
        Nop();
        Nop();
    }
} /*
void __attribute__((__interrupt__,__no_auto_psv__)) _T6Interrupt( void )
{    
    //static WORD T;
    //static WORD T1;
    //T  = TMR8;
    //T1 = TMR9HLD;
    //TMR9HLD = 0x0000;
    //TMR8 = 0x0000;
    //TMR6 = 0x7FFF;
    //CPUSPEED.word.LW = T;
    //CPUSPEED.word.HW = T1;
    IFS2bits.T6IF = 0; // Clear T6 interrupt flag
    TimerMonitor();
}   
*/ 
#elif defined(__C32__)
    void _general_exception_handler(unsigned cause, unsigned status)
    {
        Nop();
        Nop();
    }
#endif

/*
unsigned int BufferA[128] __attribute__((space(dma)));
unsigned int BufferAS[128] __attribute__((space(dma)));
unsigned int BufferB[128] __attribute__((space(dma)));
unsigned int BufferBS[128] __attribute__((space(dma)));
*/
// DMA Interrupt Handler
void fillingBufferR(void* _This, WORD * Buf, WORD Count)
{
    static int T = 0;
    int i = 0; 
    WORD Interval;
    for(i = 0; i < Count; i++){
        Interval = (T++ + 1)*256;
        Buf[i] = (int)Interval;
        i++;
        Buf[i] = (int)Interval+128;
    }
}
void fillingBufferRS(void* _This, WORD * Buf, WORD Count)
{
    static int T = 0;
    int i = 0;    
    for(i = 0; i < Count; i++){
        Buf[i] = (int)(T+1)*200+25;
        T++;
    }
}
void fillingBufferR1(void* _This, WORD * Buf, WORD Count)
{
    static int T = 0;
    int i = 0; 
    WORD Interval;
    for(i = 0; i < Count; i++){
        Interval = (T++ + 1)*256;
        Buf[i] = (int)Interval;
        i++;
        Buf[i] = (int)Interval+128;
    }
}
void fillingBufferRS1(void* _This, WORD * Buf, WORD Count)
{
    static int T = 0;
    int i = 0;    
    for(i = 0; i < Count; i++){
        Buf[i] = (int)(T+1)*200+25;
        T++;
    }
}

void FillBufSPI(void* _This, WORD * Buf, WORD Count)
{
    static int T = 322;
    int i = 0;   
    int t = 0;
    if(t < Count){
	    
    } 
    for(i = 0; i < Count; i++){
        Buf[i] = (int)(T+1)*200+25;
        T++;
    }
}
int ENCSelect()
{
    LATFbits.LATF12 = 0;
    return 0;
}
int ENCRelease()
{
    LATFbits.LATF12 = 1;
    return 0;
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
        
    static DWORD t = 0;
    static DWORD d = 0;
    static DWORD dwLastIP = 0;
    static int TimeAdjusted = 0;
    //void (*mas[8])(WORD) = {f0,f1,f2,f3,f4,f5,f6,f7};    
    //WORD Count = 10;
    //DMA_ID id = 6;
    //(mas[id])(Count);
    WORD Cmd[135];
    int i = 0;
    for(i = 0; i < 135; i++){
	    Cmd[i] = i;
    }
    BYTE Data[] = { 0x10,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
    //memset(Cmd,0x12,sizeof(Cmd));
    SPIConfig Config;
    volatile int k = 0;
    k = SPIRegisterDevice(ID_SPI1, Config, ENCSelect, ENCRelease);
    k = SPIRegisterDevice(ID_SPI1, Config, ENCSelect, ENCRelease);
    k = SPIRegisterDevice(ID_SPI1, Config, ENCSelect, ENCRelease);
    k = SPIRegisterDevice(ID_SPI1, Config, ENCSelect, ENCRelease);
    memcpy(Cmd,Data,sizeof(Data));
    SPI1PutArray(Cmd, sizeof(Cmd));
    //SPI1SendData( 0, (BYTE*)&Cmd, sizeof(Cmd), (BYTE*)Data, sizeof(Data), ENCSelect, ENCRelease);
    //BYTE sync_mode=0;
        //BYTE slew=0; 
    // указатель на функцию
    //int (*p)(void); // объявление
    //p = OCSetup;    // присвоение
    //p();			  // вызов

    //volatile DWORD UTCT;
    LATFbits.LATF4 = 0;
    TRISFbits.TRISF4 = 0;  
//    CloseI2C1(); 
//	sync_mode = 0;
//    slew = 0;
//    OpenI2C1(sync_mode,slew);
//    I2C1BRG=32;             //1 MHz Baud clock(32) @40MHz
//   	IdleI2C1();
        
    // Initialize application specific hardware
    // Для работы A13  его нужно отключить от ADC
    {
        TRISAbits.TRISA12 = 1;
        AD1CON1 = 0;
        AD2CON1 = 0;
        AD1PCFGH = 0xFFFF;
        AD1PCFGL = 0xFFFF;
        AD2PCFGL = 0xFFFF;
    }
        
    //BYTE res;
    //char bfr[64];
    //BYTE length=0;
    //AppConfig.MyIPAddr.Val = 0x12345678;
    //while(1){
    //    res = RunClient(bfr, sizeof(bfr), &length);        
    //    res = ProcessClients(0, bfr, &length);            
    //}
    if(0){
            #ifdef NO_DEFINED
        WORD Buf[256];
        WORD BufSize = 256;
        WORD* BufA;
        WORD* BufB;
        //double k=1.23;
        //volatile double B;
        //B = exp(k);
        //if(B > 0.0){
            //    OCSetup();
        //}
        DMAInit(DMA0, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
        DMASelectDevice(DMA0, IRQ_OC1, (int)&OC1R);
        DMASetBufferSize(DMA0, 64);
        BufA = DMAGetBuffer(DMA0);
        BufB = DMAGetBuffer(DMA0);
        DMASetBuffers(DMA0,BufA,BufB);
        
        DMASetCallback(DMA0, NULL, (ROM void*)fillingBufferR, (ROM void*)fillingBufferR);
        DMASetInt(DMA0, 5, TRUE);
        DMAPrepBuffer(DMA0);
        DMASetState(DMA0, TRUE, FALSE);
         
        /*PushCmdToQueue(&rr1, ST_ACCELERATE, 10.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
        PushCmdToQueue(&rr1, ST_RUN, 10.0 * Grad_to_Rad, 15.0 * Grad_to_Rad, 1);
        PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
        PushCmdToQueue(&rr1, ST_STOP, 0.0 * Grad_to_Rad, 0.0 * Grad_to_Rad, 1);
        for(;;){
            if(Control(&rr1, Buf, BufSize))
                break;
        }*/
        while(1){
            Nop();
        }
        #endif
    }
    if(0){
#ifdef NO_DEFINED
            DWORD i = 0;
            WORD sz = 256;
        {
                WORD* BufA;
                WORD* BufB;
            // Initialize Output Compare Module in Contionous pulse mode
            OCInit(ID_OC1, IDLE_DISABLE, OC_TMR2, OC_DISABLED);
            OCSetValue(ID_OC1, 100, 125);
            OCSetInt(ID_OC1, 6, TRUE);
            OCSetCallback(ID_OC1, NULL);
            OCSetMode(ID_OC1, TOGGLE);
            
            // Initialize Timer2
            TimerInit(T2, CLOCK_SOURCE_INTERNAL, GATED_DISABLE, PRE_1_8, IDLE_DISABLE, BIT_16, SYNC_DISABLE);
            TimerSetValue(T2, 0, 0xFFFF);
            TimerSetCallback(T2, NULL);
            TimerSetInt(T2, 5, FALSE);

            // Initialize Timer3
            TimerInit(T3, CLOCK_SOURCE_INTERNAL, GATED_DISABLE, PRE_1_8, IDLE_DISABLE, BIT_16, SYNC_DISABLE);
            TimerSetValue(T3, 0, 0xFFFF);
            OCSetCallback(T3, NULL);
            TimerSetInt(T3, 5, FALSE);
           
            // Setup and Enable DMA Channel
            DMAInit(DMA0, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
            DMASelectDevice(DMA0, IRQ_OC1, (int)&OC1R);
            DMASetBufferSize(DMA0, 128);
                BufA = DMAGetBuffer(DMA0);
                BufB = DMAGetBuffer(DMA0);
                DMASetBuffers(DMA0,BufA,BufB);
            DMASetCallback(DMA0, NULL, (ROM void*)fillingBufferR, (ROM void*)fillingBufferR);
            DMASetInt(DMA0, 5, TRUE);
            DMAPrepBuffer(DMA0);
            DMASetState(DMA0, TRUE, FALSE);
            
            DMAInit(DMA1, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
            DMASelectDevice(DMA1, IRQ_OC2, (int)&OC2R);
            DMASetBufferSize(DMA1, 128);
                BufA = DMAGetBuffer(DMA1);
                BufB = DMAGetBuffer(DMA1);
                DMASetBuffers(DMA1,BufA,BufB);
            DMASetCallback(DMA1, NULL, (ROM void*)fillingBufferR1, (ROM void*)fillingBufferR1);
            DMASetInt(DMA1, 5, TRUE);
            DMAPrepBuffer(DMA1);
            DMASetState(DMA1, TRUE, FALSE);            
            
            // Initialize Output Compare Module in Contionous pulse mode
            OCInit(ID_OC2, IDLE_DISABLE, OC_TMR3, OC_DISABLED);
            OCSetValue(ID_OC2, 100, 125);
            OCSetInt(ID_OC2, 6, TRUE);
            OCSetCallback(ID_OC2, NULL);            
            OCSetMode(ID_OC2, TOGGLE);
            /*
            // Setup and Enable DMA Channel
            DMAInit(DMA2, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
            DMASelectDevice(DMA2, IRQ_OC2, (int)&OC2R);
            DMASetBufferSize(DMA2, 64);
            DMASetCallback(DMA2, NULL, (ROM void*)fillingBufferR1, (ROM void*)fillingBufferR1);
            DMASetInt(DMA2, 5, TRUE);
            DMAPrepBuffer(DMA2);
            DMASetState(DMA2, TRUE, FALSE);
            
            DMAInit(DMA3, SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
            DMASelectDevice(DMA3, IRQ_OC2, (int)&OC2RS);
            DMASetBufferSize(DMA3, 64);
            DMASetCallback(DMA3, NULL, (ROM void*)fillingBufferRS1, (ROM void*)fillingBufferRS1);
            DMASetInt(DMA3, 5, TRUE);
            DMAPrepBuffer(DMA3);
            DMASetState(DMA3, TRUE, FALSE);      
            */
            // Enable Timer
            //TimerSetState(T2, TRUE);
            //TimerSetState(T3, TRUE);
            T2CONbits.TON = 1;
            T3CONbits.TON = 1;
            /*
            DMAForceTransfer(DMA0);
            DMAForceTransfer(DMA1);
            DMAForceTransfer(DMA2);
            DMAForceTransfer(DMA3);
            */

        }
        while(1){	       	
                if(i++ > 100000){
                        if(sz == 32) 
                                sz = 128; 
                        else 
                                sz = 32;
                        //DMASetState(DMA0, FALSE, FALSE);
                        //DMASetState(DMA1, FALSE, FALSE);
                        DMASetBufferSize(DMA0, sz);
                        DMASetBufferSize(DMA1, sz);
                        //DMAForceTransfer(DMA0);
                        //DMAForceTransfer(DMA1);
                        //DMASetState(DMA0, TRUE, FALSE);
                        //DMASetState(DMA1, TRUE, FALSE);
                        
                        
                        i = 0;
                }
           Nop();
           Nop();
        }
#endif
    }
    
    if(0){
        OCSetup();
        //вызов предпросчета двигателей
        {
            T6CON = 0x0030; //0.000000025*256 = 0.0000064 = 6.4us
            TMR6 = 0x0000;  //
            PR6 = 10;//;    //0.0000064 * 10 = 0.000064 = 64us
            IFS2bits.T6IF = 0;
            IEC2bits.T6IE = 1;
            IPC11bits.T6IP = 5;    
            T6CONbits.TON = 1;
        } 
//        PushCmdToQueue(&rr1, ST_ACCELERATE, 20.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
//        PushCmdToQueue(&rr1, ST_RUN, 20.0,  45.0 * Grad_to_Rad, 1);
//        PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1); 
//        //Control(&rr1);
//        PushCmdToQueue(&rr2, ST_ACCELERATE, 18.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
//        PushCmdToQueue(&rr2, ST_RUN, 18.0,  45.0 * Grad_to_Rad, 1);
//        PushCmdToQueue(&rr2, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);         
//        //Control(&rr2);
//        
//        
//        PushCmdToQueue(&rr3, ST_ACCELERATE, 10.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
//        PushCmdToQueue(&rr3, ST_RUN, 10.0,  45.0 * Grad_to_Rad, 1);
//        PushCmdToQueue(&rr3, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1); 
        //GoToCmd(&rr1, -0.00417807934636275010445197530291 * Grad_to_Rad, 90 * Grad_to_Rad, 0);
        //Control(&rr3);
        
        while(1){
            //TimerMonitor();
            Nop();
            ////Control(&rr1);
            //Control(&rr2);
            //Control(&rr3);
        }
     }   
    InitializeBoard();
    // calculate CPU speed  
    if(0){
        T6CON = 0x0002;
        T8CON = 0x0008;
        T9CON = 0x0000;
        PR8=0xFFFF;
        PR9=0xFFFF;
        TMR6 = 0x7FFF;
        TMR8 = 0x0000;
        TMR9 = 0x0000;    
        CPUSPEED.Val = 0;
        IFS2bits.T6IF = 0;
        IEC2bits.T6IE = 1;
        IPC11bits.T6IP = 7;
        T6CONbits.TON = 1;
        T8CONbits.TON = 1;    
    }
    
    
#if defined(USE_LCD)
    // Initialize and display the stack version on the LCD
    LCDInit();
    DelayMs(100);
    strcpypgm2ram((char*)LCDText, "TCPStack " TCPIP_STACK_VERSION "  "
        "                ");
    LCDUpdate();
#endif

    // Initialize stack-related hardware components that may be 
    // required by the UART configuration routines
    TickInit();
#if defined(STACK_USE_MPFS) || defined(STACK_USE_MPFS2)
    MPFSInit();
#endif

    // Initialize Stack and application related NV variables into AppConfig.
    InitAppConfig();

    // Initiates board setup process if button is depressed 
    // on startup
    #ifdef bad
    if(BUTTON0_IO == 0u)
    {
    #if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)
    // Invalidate the EEPROM contents if BUTTON0 is held down for more than 4 seconds
    DWORD StartTime = TickGet();
    LED_PUT(0x00);
            
    while(BUTTON0_IO == 0u)
    {
        if(TickGet() - StartTime > 4*TICK_SECOND)
        {
            #if defined(EEPROM_CS_TRIS)
            XEEBeginWrite(0x0000);
            XEEWrite(0xFF);
            XEEWrite(0xFF);
            XEEEndWrite();
            #elif defined(SPIFLASH_CS_TRIS)
            SPIFlashBeginWrite(0x0000);
            SPIFlashWrite(0xFF);
            SPIFlashWrite(0xFF);
            #endif
            
            #if defined(STACK_USE_UART)
            putrsUART("\r\n\r\nBUTTON0 held for more than 4 seconds.  Default settings restored.\r\n\r\n");
            #endif

            LED_PUT(0x0F);
            while((LONG)(TickGet() - StartTime) <= (LONG)(9*TICK_SECOND/2));
            LED_PUT(0x00);
            while(BUTTON0_IO == 0u);
            Reset();
            break;
        }
    }
    #endif

    #if defined(STACK_USE_UART)
        DoUARTConfig();
    #endif
    }
    #endif //bad
    // Initialize core stack layers (MAC, ARP, TCP, UDP) and
    // application modules (HTTP, SNMP, etc.)
    StackInit();

    #if defined(WF_CS_TRIS)
    WF_Connect();
    #endif

    // Initialize any application-specific modules or functions/
    // For this demo application, this only includes the
    // UART 2 TCP Bridge
    #if defined(STACK_USE_UART2TCP_BRIDGE)
    UART2TCPBridgeInit();
    #endif

    #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
    ZeroconfLLInitialize();
    #endif

    #if defined(STACK_USE_ZEROCONF_MDNS_SD)
    mDNSInitialize(MY_DEFAULT_HOST_NAME);
    mDNSServiceRegister(
        (const char *) "DemoWebServer",    // base name of the service
        "_http._tcp.local",                // type of the service
        80,                                // TCP or UDP port, at which this service is available
        ((const BYTE *)"path=/index.htm"),    // TXT info
        1,                                    // auto rename the service when if needed
        NULL,                                // no callback function
        NULL                                // no application context
        );

    mDNSMulticastFilterRegister();            
    #endif

    if(0){
            LoadRRConfig();
            OCSetup();
            //вызов предпросчета двигателей
            {
                T6CON = 0x0030; //0.000000025*256 = 0.0000064 = 6.4us
                TMR6 = 0x0000;  //
                PR6 = 10;//;    //0.0000064 * 10 = 0.000064 = 64us
                IFS2bits.T6IF = 0;
                IEC2bits.T6IE = 1;
                IPC11bits.T6IP = 5;    
                T6CONbits.TON = 1;
            } 
    }
    //Вентиляторы        
    {
        TRISBbits.TRISB4 = 0;
        T7CON = 0x0030; //0.000000025*256 = 0.0000064
        TMR7 = 0x0000;
        PR7 = 16384;//512;
        IFS3bits.T7IF = 0;
        IEC3bits.T7IE = 1;
        IPC12bits.T7IP = 4;    
        T7CONbits.TON = 1;
    } 
    /*
    PushCmdToQueue(&rr1, ST_ACCELERATE, 10.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr1, ST_RUN, 10.0,  90.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1); 
    //Control(&rr1);
    
    PushCmdToQueue(&rr2, ST_ACCELERATE, 10.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr2, ST_RUN, 10.0,  45.0 * Grad_to_Rad, 1);
    PushCmdToQueue(&rr2, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);         
    */
    //Control(&rr2);
    //0.004166667
    //GoToCmd(&rr1, 0.0 * Grad_to_Rad, 90.0 * Grad_to_Rad, 0);
    //GoToCmd(&rr2, 0.0 * Grad_to_Rad, 10.0 * Grad_to_Rad, 0);
    
    //PushCmdToQueue(&rr3, ST_ACCELERATE, 10.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(&rr3, ST_RUN, 10.0,  45.0 * Grad_to_Rad, 1);
    //PushCmdToQueue(&rr3, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1); 
    //Control(&rr3); 
    /*
    while(1){
        Nop();
    }*/
    // Now that all items are initialized, begin the co-operative
    // multitasking loop.  This infinite loop will continuously 
    // execute all stack-related tasks, as well as your own
    // application's functions.  Custom functions should be added
    // at the end of this loop.
    // Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot
    // or part of it) and returns so that other tasks can do their
    // job.
    // If a task needs very long time to do its job, it must be broken
    // down into smaller pieces so that other tasks can have CPU time.
    while(1)
    {
        //Control(&rr1);
        //Control(&rr2);
        //Control(&rr3);
        // Blink LED0 (right most one) every second.
        
        //if(TickGet() - d >= TICK_SECOND/16000ul)
        //{
        //    d = TickGet();
        //       LATDbits.LATD0 ^= 1; // выход STEP
            
        //}
        //TimerMonitor();
        //FanControl();        
        if(PORTAbits.RA13 != t){
            t = PORTAbits.RA13;
                LED0_IO = t;
                //if(!TimeAdjusted){
                //    TimeAdjusted = AdjustLocalRTCTime(); 
                //    LED2_IO = TimeAdjusted;           
                //}
                RRConfigRAM.RRSave[0].XPosition = rr1.XPosition;
                RRConfigRAM.RRSave[1].XPosition = rr2.XPosition;                
                   RRConfigRAM.RRSave[2].XPosition = rr3.XPosition;
                SaveRRConfig();
        }
        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();

        // This tasks invokes each of the core stack application tasks
        StackApplications();

        #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
        ZeroconfLLProcess();
        #endif

        #if defined(STACK_USE_ZEROCONF_MDNS_SD)
        mDNSProcess();
    // Use this function to exercise service update function
    // HTTPUpdateRecord();
        #endif

    // Process application specific tasks here.
    // For this demo app, this will include the Generic TCP 
    // client and servers, and the SNMP, Ping, and SNMP Trap
    // demos.  Following that, we will process any IO from
    // the inputs on the board itself.
    // Any custom modules or processing you need to do should
    // go here.
    #if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE)
    GenericTCPClient();
    #endif
    
    #if defined(STACK_USE_GENERIC_TCP_SERVER_EXAMPLE)
    GenericTCPServer();
    #endif
    
    #if defined(STACK_USE_SMTP_CLIENT)
    SMTPDemo();
    #endif
    
    #if defined(STACK_USE_ICMP_CLIENT)
    PingDemo();
    #endif
    
    #if defined(STACK_USE_SNMP_SERVER) && !defined(SNMP_TRAP_DISABLED)
    //User should use one of the following SNMP demo
    // This routine demonstrates V1 or V2 trap formats with one variable binding.
    SNMPTrapDemo();
    
    #if defined(SNMP_STACK_USE_V2_TRAP) || defined(SNMP_V1_V2_TRAP_WITH_SNMPV3)
    //This routine provides V2 format notifications with multiple (3) variable bindings
    //User should modify this routine to send v2 trap format notifications with the required varbinds.
    //SNMPV2TrapDemo();
    #endif 
    if(gSendTrapFlag)
        SNMPSendTrap();
    #endif
    
    #if defined(STACK_USE_BERKELEY_API)
    //BerkeleyTCPClientDemo();
        BerkeleyTCPServerDemo();
    //BerkeleyUDPClientDemo();
    #endif

    ProcessIO();

        // If the local IP address has changed (ex: due to DHCP lease change)
        // write the new IP address to the LCD display, UART, and Announce 
        // service
    if(dwLastIP != AppConfig.MyIPAddr.Val)
    {
        dwLastIP = AppConfig.MyIPAddr.Val;
            
        #if defined(STACK_USE_UART)
            putrsUART((ROM char*)"\r\nNew IP Address: ");
        #endif

        DisplayIPValue(AppConfig.MyIPAddr);

        #if defined(STACK_USE_UART)
            putrsUART((ROM char*)"\r\n");
        #endif


        #if defined(STACK_USE_ANNOUNCE)
            AnnounceIP();
        #endif

            #if defined(STACK_USE_ZEROCONF_MDNS_SD)
            mDNSFillHostRecord();
        #endif
    }
    }
}
int AdjustLocalRTCTime()
{
    DWORD RTCSeconds;
    DWORD SNTPSeconds;
#if defined(STACK_USE_SNTP_CLIENT)&&defined(SPIRTCSRAM_CS_TRIS)
    //имеет смысл только при работающем модуле SNTP
    if(SNTPIsTimeValid())
    {
        //RTCSeconds = RTCGetUTCSeconds();  
        SNTPSeconds = SNTPGetUTCSeconds();
        //if((RTCSeconds>SNTPSeconds+2)||(RTCSeconds<SNTPSeconds-2)){
            SetTimeFromUTC(SNTPSeconds); 
        //}
        return 1;
    }    
#endif
    return 0;
}

DWORD UTCGetTime(void)
{
#if defined(STACK_USE_SNTP_CLIENT)&&defined(SPIRTCSRAM_CS_TRIS)
    //if(RTCIsTimeValid()){
    //    return RTCGetUTCSeconds();
    //} else
    if(SNTPIsTimeValid()) {
        //получаем время из SNTP модуля
        return SNTPGetUTCSeconds();
    } else {
        return RTCGetUTCSeconds();
    }
#endif
}


#if defined(WF_CS_TRIS)
/*****************************************************************************
 * FUNCTION: WF_Connect
 *
 * RETURNS:  None
 *
 * PARAMS:   None
 *
 *  NOTES:   Connects to an 802.11 network.  Customize this function as needed 
 *           for your application.
 *****************************************************************************/
static void WF_Connect(void)
{
    UINT8 ConnectionProfileID;
    UINT8 channelList[] = MY_DEFAULT_CHANNEL_LIST;
    #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
    BOOL  PsPollEnabled;
    #endif
    
    /* create a Connection Profile */
    WF_CPCreate(&ConnectionProfileID);

    #if defined(STACK_USE_UART)
    putrsUART("Set SSID (");
    putsUART(AppConfig.MySSID);
    putrsUART(")\r\n");
    #endif
    WF_CPSetSsid(ConnectionProfileID, 
                 AppConfig.MySSID, 
                 AppConfig.SsidLength);

    #if defined(STACK_USE_UART)
    putrsUART("Set Network Type\r\n");
    #endif
    WF_CPSetNetworkType(ConnectionProfileID, MY_DEFAULT_NETWORK_TYPE);
    
    #if defined(STACK_USE_UART)
    putrsUART("Set Scan Type\r\n");
    #endif
    WF_CASetScanType(MY_DEFAULT_SCAN_TYPE);
    
    #if defined(STACK_USE_UART)
    putrsUART("Set Channel List\r\n");
    #endif    
    WF_CASetChannelList(channelList, sizeof(channelList));
    
    #if defined(STACK_USE_UART)
    putrsUART("Set list retry count\r\n");
    #endif
    WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT);

    #if defined(STACK_USE_UART)        
    putrsUART("Set Event Notify\r\n");    
    #endif
    WF_CASetEventNotificationAction(MY_DEFAULT_EVENT_NOTIFICATION_LIST);
    
#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
    PsPollEnabled = (MY_DEFAULT_PS_POLL == WF_ENABLED);
    if (!PsPollEnabled)
    {    
        /* disable low power (PS-Poll) mode */
        #if defined(STACK_USE_UART)
        putrsUART("Disable PS-Poll\r\n");        
        #endif
        WF_PsPollDisable();
    }    
    else
    {
        /* Enable low power (PS-Poll) mode */
        #if defined(STACK_USE_UART)
        putrsUART("Enable PS-Poll\r\n");        
        #endif
        WF_PsPollEnable(TRUE);
    }    
#endif

    #if defined(STACK_USE_UART)
    putrsUART("Set Beacon Timeout\r\n");
    #endif
    WF_CASetBeaconTimeout(40);
    
    /* Set Security */
    #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (Open)\r\n");
        #endif
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40)
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (WEP40)\r\n");
        #endif
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104)
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (WEP104)\r\n");
        #endif
    #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_KEY 
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (WPA with key)\r\n");
        #endif
    #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_KEY 
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (WPA2 with key)\r\n");
        #endif
    #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_PASS_PHRASE
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (WPA with pass phrase)\r\n");
        #endif
    #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_PASS_PHRASE
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (WPA2 with pass phrase)\r\n");    
        #endif
    #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (WPA with key, auto-select)\r\n");
        #endif
    #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE
        #if defined(STACK_USE_UART)
        putrsUART("Set Security (WPA with pass phrase, auto-select)\r\n");
        #endif
    #endif /* MY_DEFAULT_WIFI_SECURITY_MODE */

    WF_CPSetSecurity(ConnectionProfileID,
                     AppConfig.SecurityMode,
                     AppConfig.WepKeyIndex,   /* only used if WEP enabled */
                     AppConfig.SecurityKey,
                     AppConfig.SecurityKeyLength);
    #if defined(STACK_USE_UART)                     
    putrsUART("Start WiFi Connect\r\n");        
    #endif
    WF_CMConnect(ConnectionProfileID);
}   
#endif /* WF_CS_TRIS */

// Writes an IP address to the LCD display and the UART as available
void DisplayIPValue(IP_ADDR IPVal)
{
//    printf("%u.%u.%u.%u", IPVal.v[0], IPVal.v[1], IPVal.v[2], IPVal.v[3]);
    BYTE IPDigit[4];
    BYTE i;
#ifdef USE_LCD
    BYTE j;
    BYTE LCDPos=16;
#endif

    for(i = 0; i < sizeof(IP_ADDR); i++)
    {
        uitoa((WORD)IPVal.v[i], IPDigit);

        #if defined(STACK_USE_UART)
            putsUART((char *) IPDigit);
        #endif

        #ifdef USE_LCD
            for(j = 0; j < strlen((char*)IPDigit); j++)
            {
                LCDText[LCDPos++] = IPDigit[j];
            }
            if(i == sizeof(IP_ADDR)-1)
                break;
            LCDText[LCDPos++] = '.';
        #else
            if(i == sizeof(IP_ADDR)-1)
                break;
        #endif

        #if defined(STACK_USE_UART)
            while(BusyUART());
            WriteUART('.');
        #endif
    }

    #ifdef USE_LCD
        if(LCDPos < 32u)
            LCDText[LCDPos] = 0;
        LCDUpdate();
    #endif
}

// Processes A/D data from the potentiometer
static void ProcessIO(void)
{
#if defined(__C30__) || defined(__C32__)
    // Convert potentiometer result into ASCII string
    uitoa((WORD)ADC1BUF0, AN0String);
#else
    // AN0 should already be set up as an analog input
    ADCON0bits.GO = 1;

    // Wait until A/D conversion is done
    while(ADCON0bits.GO);

    // AD converter errata work around (ex: PIC18F87J10 A2)
    #if !defined(__18F87J50) && !defined(_18F87J50) && !defined(__18F87J11) && !defined(_18F87J11) 
    {
        BYTE temp = ADCON2;
        ADCON2 |= 0x7;    // Select Frc mode by setting ADCS0/ADCS1/ADCS2
        ADCON2 = temp;
    }
    #endif

    // Convert 10-bit value into ASCII string
    uitoa(*((WORD*)(&ADRESL)), AN0String);
#endif
}


/****************************************************************************
  Function:
    static void InitializeBoard(void)

  Description:
    This routine initializes the hardware.  It is a generic initialization
    routine for many of the Microchip development boards, using definitions
    in HardwareProfile.h to determine specific initialization.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void InitializeBoard(void)
{    
    // LEDs
    LED0_TRIS = 0;
    LED1_TRIS = 0;
    LED2_TRIS = 0;
    LED3_TRIS = 0;
    LED4_TRIS = 0;
    LED5_TRIS = 0;
    LED6_TRIS = 0;
    LED7_TRIS = 0;
    LED_PUT(0x00);

#if defined(__18CXX)
    // Enable 4x/5x/96MHz PLL on PIC18F87J10, PIC18F97J60, PIC18F87J50, etc.
    OSCTUNE = 0x40;

    // Set up analog features of PORTA

    // PICDEM.net 2 board has POT on AN2, Temp Sensor on AN3
    #if defined(PICDEMNET2)
        ADCON0 = 0x09;        // ADON, Channel 2
        ADCON1 = 0x0B;        // Vdd/Vss is +/-REF, AN0, AN1, AN2, AN3 are analog
    #elif defined(PICDEMZ)
        ADCON0 = 0x81;        // ADON, Channel 0, Fosc/32
        ADCON1 = 0x0F;        // Vdd/Vss is +/-REF, AN0, AN1, AN2, AN3 are all digital
    #elif defined(__18F87J11) || defined(_18F87J11) || defined(__18F87J50) || defined(_18F87J50)
        ADCON0 = 0x01;        // ADON, Channel 0, Vdd/Vss is +/-REF
        WDTCONbits.ADSHR = 1;
        ANCON0 = 0xFC;        // AN0 (POT) and AN1 (temp sensor) are anlog
        ANCON1 = 0xFF;
        WDTCONbits.ADSHR = 0;        
    #else
        ADCON0 = 0x01;        // ADON, Channel 0
        ADCON1 = 0x0E;        // Vdd/Vss is +/-REF, AN0 is analog
    #endif
    ADCON2 = 0xBE;        // Right justify, 20TAD ACQ time, Fosc/64 (~21.0kHz)


    // Enable internal PORTB pull-ups
    INTCON2bits.RBPU = 0;

    // Configure USART
    TXSTA = 0x20;
    RCSTA = 0x90;

    // See if we can use the high baud rate setting
    #if ((GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1) <= 255
        SPBRG = (GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1;
        TXSTAbits.BRGH = 1;
    #else    // Use the low baud rate setting
        SPBRG = (GetPeripheralClock()+8*BAUD_RATE)/BAUD_RATE/16 - 1;
    #endif


    // Enable Interrupts
    RCONbits.IPEN = 1;        // Enable interrupt priorities
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

    // Do a calibration A/D conversion
    #if defined(__18F87J10) || defined(__18F86J15) || defined(__18F86J10) || defined(__18F85J15) || defined(__18F85J10) || defined(__18F67J10) || defined(__18F66J15) || defined(__18F66J10) || defined(__18F65J15) || defined(__18F65J10) || defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60) || \
         defined(_18F87J10) ||  defined(_18F86J15) || defined(_18F86J10)  ||  defined(_18F85J15) ||  defined(_18F85J10) ||  defined(_18F67J10) ||  defined(_18F66J15) ||  defined(_18F66J10) ||  defined(_18F65J15) ||  defined(_18F65J10) ||  defined(_18F97J60) ||  defined(_18F96J65) ||  defined(_18F96J60) ||  defined(_18F87J60) ||  defined(_18F86J65) ||  defined(_18F86J60) ||  defined(_18F67J60) ||  defined(_18F66J65) ||  defined(_18F66J60)
        ADCON0bits.ADCAL = 1;
        ADCON0bits.GO = 1;
        while(ADCON0bits.GO);
        ADCON0bits.ADCAL = 0;
    #elif defined(__18F87J11) || defined(__18F86J16) || defined(__18F86J11) || defined(__18F67J11) || defined(__18F66J16) || defined(__18F66J11) || \
           defined(_18F87J11) ||  defined(_18F86J16) ||  defined(_18F86J11) ||  defined(_18F67J11) ||  defined(_18F66J16) ||  defined(_18F66J11) || \
          defined(__18F87J50) || defined(__18F86J55) || defined(__18F86J50) || defined(__18F67J50) || defined(__18F66J55) || defined(__18F66J50) || \
           defined(_18F87J50) ||  defined(_18F86J55) ||  defined(_18F86J50) ||  defined(_18F67J50) ||  defined(_18F66J55) ||  defined(_18F66J50)
        ADCON1bits.ADCAL = 1;
        ADCON0bits.GO = 1;
        while(ADCON0bits.GO);
        ADCON1bits.ADCAL = 0;
    #endif

#else    // 16-bit C30 and and 32-bit C32
    #if defined(__PIC32MX__)
    {
        // Enable multi-vectored interrupts
        INTEnableSystemMultiVectoredInt();
        
        // Enable optimal performance
        SYSTEMConfigPerformance(GetSystemClock());
        mOSCSetPBDIV(OSC_PB_DIV_1);                // Use 1:1 CPU Core:Peripheral clocks
        
        // Disable JTAG port so we get our I/O pins back, but first
        // wait 50ms so if you want to reprogram the part with 
        // JTAG, you'll still have a tiny window before JTAG goes away.
        // The PIC32 Starter Kit debuggers use JTAG and therefore must not 
        // disable JTAG.
        DelayMs(50);
        #if !defined(__MPLAB_DEBUGGER_PIC32MXSK) && !defined(__MPLAB_DEBUGGER_FS2)
            DDPCONbits.JTAGEN = 0;
        #endif
        LED_PUT(0x00);                // Turn the LEDs off
        
        CNPUESET = 0x00098000;        // Turn on weak pull ups on CN15, CN16, CN19 (RD5, RD7, RD13), which is connected to buttons on PIC32 Starter Kit boards
    }
    #endif

    #if defined(__dsPIC33F__) || defined(__PIC24H__)
        //PLLFBD = 0x7f;                // Multiply by 40 for 160MHz VCO output (8MHz XT oscillator)
        //CLKDIV = 0x0004;            // FRC: divide by 2, PLLPOST: divide by 2, PLLPRE: divide by 2
        //OSCCON = 0x0301;    
        {    
             // Configure PLL prescaler, PLL postscaler, PLL divisor
            PLLFBD=0x7E; // M = 128
            CLKDIVbits.PLLPOST = 0;// N2 = 2
            CLKDIVbits.PLLPRE = 3; // N1 = 5
            // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
            __builtin_write_OSCCONH(0x03);
            __builtin_write_OSCCONL(0x01);
            // Wait for Clock switch to occur
            while (OSCCONbits.COSC!= 0b011);
            // Wait for PLL to lock
            while(OSCCONbits.LOCK!= 1) {};
         }
        // Port I/O
        AD1PCFGHbits.PCFG23 = 1;    // Make RA7 (BUTTON1) a digital input
        AD1PCFGHbits.PCFG20 = 1;    // Make RA12 (INT1) a digital input for MRF24WB0M PICtail Plus interrupt

        // ADC
        AD1CHS0 = 0;                // Input to AN0 (potentiometer)
        AD1PCFGLbits.PCFG5 = 0;        // Disable digital input on AN5 (potentiometer)
        AD1PCFGLbits.PCFG4 = 0;        // Disable digital input on AN4 (TC1047A temp sensor)
    #else    //defined(__PIC24F__) || defined(__PIC32MX__)
        #if defined(__PIC24F__)
            CLKDIVbits.RCDIV = 0;        // Set 1:1 8MHz FRC postscalar
        #endif
        
        // ADC
        #if defined(__PIC24FJ256DA210__) || defined(__PIC24FJ256GB210__)
            // Disable analog on all pins
            ANSA = 0x0000;
            ANSB = 0x0000;
            ANSC = 0x0000;
            ANSD = 0x0000;
            ANSE = 0x0000;
            ANSF = 0x0000;
            ANSG = 0x0000;
        #else
            AD1CHS = 0;                    // Input to AN0 (potentiometer)
            AD1PCFGbits.PCFG4 = 0;        // Disable digital input on AN4 (TC1047A temp sensor)
            #if defined(__32MX460F512L__) || defined(__32MX795F512L__)    // PIC32MX460F512L and PIC32MX795F512L PIMs has different pinout to accomodate USB module
                AD1PCFGbits.PCFG2 = 0;        // Disable digital input on AN2 (potentiometer)
            #else
                AD1PCFGbits.PCFG5 = 0;        // Disable digital input on AN5 (potentiometer)
            #endif
        #endif
    #endif

    // ADC
    AD1CON1 = 0x84E4;            // Turn on, auto sample start, auto-convert, 12 bit mode (on parts with a 12bit A/D)
    AD1CON2 = 0x0404;            // AVdd, AVss, int every 2 conversions, MUXA only, scan
    AD1CON3 = 0x1003;            // 16 Tad auto-sample, Tad = 3*Tcy
    #if defined(__32MX460F512L__) || defined(__32MX795F512L__)    // PIC32MX460F512L and PIC32MX795F512L PIMs has different pinout to accomodate USB module
        AD1CSSL = 1<<2;                // Scan pot
    #else
        AD1CSSL = 1<<5;                // Scan pot
    #endif

    // UART
    #if defined(STACK_USE_UART)
        UARTTX_TRIS = 0;
        UARTRX_TRIS = 1;
        UMODE = 0x8000;            // Set UARTEN.  Note: this must be done before setting UTXEN

        #if defined(__C30__)
            USTA = 0x0400;        // UTXEN set
            #define CLOSEST_UBRG_VALUE ((GetPeripheralClock()+8ul*BAUD_RATE)/16/BAUD_RATE-1)
            #define BAUD_ACTUAL (GetPeripheralClock()/16/(CLOSEST_UBRG_VALUE+1))
        #else    //defined(__C32__)
            USTA = 0x00001400;        // RXEN set, TXEN set
            #define CLOSEST_UBRG_VALUE ((GetPeripheralClock()+8ul*BAUD_RATE)/16/BAUD_RATE-1)
            #define BAUD_ACTUAL (GetPeripheralClock()/16/(CLOSEST_UBRG_VALUE+1))
        #endif
    
        #define BAUD_ERROR ((BAUD_ACTUAL > BAUD_RATE) ? BAUD_ACTUAL-BAUD_RATE : BAUD_RATE-BAUD_ACTUAL)
        #define BAUD_ERROR_PRECENT    ((BAUD_ERROR*100+BAUD_RATE/2)/BAUD_RATE)
        #if (BAUD_ERROR_PRECENT > 3)
            #warning UART frequency error is worse than 3%
        #elif (BAUD_ERROR_PRECENT > 2)
            #warning UART frequency error is worse than 2%
        #endif
    
        UBRG = CLOSEST_UBRG_VALUE;
    #endif

#endif

// Deassert all chip select lines so there isn't any problem with 
// initialization order.  Ex: When ENC28J60 is on SPI2 with Explorer 16, 
// MAX3232 ROUT2 pin will drive RF12/U2CTS ENC28J60 CS line asserted, 
// preventing proper 25LC256 EEPROM operation.
#if defined(ENC_CS_TRIS)
    ENC_CS_IO = 1;
    ENC_CS_TRIS = 0;
#endif
#if defined(ENC100_CS_TRIS)
    ENC100_CS_IO = (ENC100_INTERFACE_MODE == 0);
    ENC100_CS_TRIS = 0;
#endif
#if defined(EEPROM_CS_TRIS)
    EEPROM_CS_IO = 1;
    EEPROM_CS_TRIS = 0;
#endif
#if defined(SPIRAM_CS_TRIS)
    SPIRAM_CS_IO = 1;
    SPIRAM_CS_TRIS = 0;
#endif
#if defined(SPIFLASH_CS_TRIS)
    SPIFLASH_CS_IO = 1;
    SPIFLASH_CS_TRIS = 0;
#endif
#if defined(SPIRTCRAM_CS_TRIS)
    SPIRTCRAM_CS_IO = 1;
    SPIRTCRAM_CS_TRIS = 0;
#endif
#if defined(WF_CS_TRIS)
    WF_CS_IO = 1;
    WF_CS_TRIS = 0;
#endif

#if defined(PIC24FJ64GA004_PIM)
    __builtin_write_OSCCONL(OSCCON & 0xBF);  // Unlock PPS

    // Remove some LED outputs to regain other functions
    LED1_TRIS = 1;        // Multiplexed with BUTTON0
    LED5_TRIS = 1;        // Multiplexed with EEPROM CS
    LED7_TRIS = 1;        // Multiplexed with BUTTON1
    
    // Inputs
    RPINR19bits.U2RXR = 19;            //U2RX = RP19
    RPINR22bits.SDI2R = 20;            //SDI2 = RP20
    RPINR20bits.SDI1R = 17;            //SDI1 = RP17
    
    // Outputs
    RPOR12bits.RP25R = U2TX_IO;        //RP25 = U2TX  
    RPOR12bits.RP24R = SCK2OUT_IO;     //RP24 = SCK2
    RPOR10bits.RP21R = SDO2_IO;        //RP21 = SDO2
    RPOR7bits.RP15R = SCK1OUT_IO;     //RP15 = SCK1
    RPOR8bits.RP16R = SDO1_IO;        //RP16 = SDO1
    
    AD1PCFG = 0xFFFF;                //All digital inputs - POT and Temp are on same pin as SDO1/SDI1, which is needed for ENC28J60 commnications

    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS
#endif

#if defined(__PIC24FJ256DA210__)
    __builtin_write_OSCCONL(OSCCON & 0xBF);  // Unlock PPS

    // Inputs
    RPINR19bits.U2RXR = 11;    // U2RX = RP11
    RPINR20bits.SDI1R = 0;    // SDI1 = RP0
    RPINR0bits.INT1R = 34;    // Assign RE9/RPI34 to INT1 (input) for MRF24WB0M Wi-Fi PICtail Plus interrupt
    
    // Outputs
    RPOR8bits.RP16R = 5;    // RP16 = U2TX
    RPOR1bits.RP2R = 8;     // RP2 = SCK1
    RPOR0bits.RP1R = 7;        // RP1 = SDO1

    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS
#endif

#if defined(__PIC24FJ256GB110__) || defined(__PIC24FJ256GB210__)
    __builtin_write_OSCCONL(OSCCON & 0xBF);  // Unlock PPS
    
    // Configure SPI1 PPS pins (ENC28J60/ENCX24J600/MRF24WB0M or other PICtail Plus cards)
    RPOR0bits.RP0R = 8;        // Assign RP0 to SCK1 (output)
    RPOR7bits.RP15R = 7;    // Assign RP15 to SDO1 (output)
    RPINR20bits.SDI1R = 23;    // Assign RP23 to SDI1 (input)

    // Configure SPI2 PPS pins (25LC256 EEPROM on Explorer 16)
    RPOR10bits.RP21R = 11;    // Assign RG6/RP21 to SCK2 (output)
    RPOR9bits.RP19R = 10;    // Assign RG8/RP19 to SDO2 (output)
    RPINR22bits.SDI2R = 26;    // Assign RG7/RP26 to SDI2 (input)
    
    // Configure UART2 PPS pins (MAX3232 on Explorer 16)
    #if !defined(ENC100_INTERFACE_MODE) || (ENC100_INTERFACE_MODE == 0) || defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)
    RPINR19bits.U2RXR = 10;    // Assign RF4/RP10 to U2RX (input)
    RPOR8bits.RP17R = 5;    // Assign RF5/RP17 to U2TX (output)
    #endif
    
    // Configure INT1 PPS pin (MRF24WB0M Wi-Fi PICtail Plus interrupt signal when in SPI slot 1)
    RPINR0bits.INT1R = 33;    // Assign RE8/RPI33 to INT1 (input)

    // Configure INT3 PPS pin (MRF24WB0M Wi-Fi PICtail Plus interrupt signal when in SPI slot 2)
    RPINR1bits.INT3R = 40;    // Assign RC3/RPI40 to INT3 (input)

    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS
#endif

#if defined(__PIC24FJ256GA110__)
    __builtin_write_OSCCONL(OSCCON & 0xBF);  // Unlock PPS
    
    // Configure SPI2 PPS pins (25LC256 EEPROM on Explorer 16 and ENC28J60/ENCX24J600/MRF24WB0M or other PICtail Plus cards)
    // Note that the ENC28J60/ENCX24J600/MRF24WB0M PICtails SPI PICtails must be inserted into the middle SPI2 socket, not the topmost SPI1 slot as normal.  This is because PIC24FJ256GA110 A3 silicon has an input-only RPI PPS pin in the ordinary SCK1 location.  Silicon rev A5 has this fixed, but for simplicity all demos will assume we are using SPI2.
    RPOR10bits.RP21R = 11;    // Assign RG6/RP21 to SCK2 (output)
    RPOR9bits.RP19R = 10;    // Assign RG8/RP19 to SDO2 (output)
    RPINR22bits.SDI2R = 26;    // Assign RG7/RP26 to SDI2 (input)
    
    // Configure UART2 PPS pins (MAX3232 on Explorer 16)
    RPINR19bits.U2RXR = 10;    // Assign RF4/RP10 to U2RX (input)
    RPOR8bits.RP17R = 5;    // Assign RF5/RP17 to U2TX (output)
    
    // Configure INT3 PPS pin (MRF24WB0M PICtail Plus interrupt signal)
    RPINR1bits.INT3R = 36;    // Assign RA14/RPI36 to INT3 (input)

    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS
#endif


#if defined(DSPICDEM11)
    // Deselect the LCD controller (PIC18F252 onboard) to ensure there is no SPI2 contention
    LCDCTRL_CS_TRIS = 0;
    LCDCTRL_CS_IO = 1;

    // Hold the codec in reset to ensure there is no SPI2 contention
    CODEC_RST_TRIS = 0;
    CODEC_RST_IO = 0;
#endif

#if defined(SPIRAM_CS_TRIS)
    SPIRAMInit();
#endif
#if defined(EEPROM_CS_TRIS)
    XEEInit();
#endif
#if defined(SPIFLASH_CS_TRIS)
    SPIFlashInit();
#endif
#if defined(SPIRTCSRAM_CS_TRIS)
    SPIRTCSRAMInit();
#endif
}

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
// MAC Address Serialization using a MPLAB PM3 Programmer and 
// Serialized Quick Turn Programming (SQTP). 
// The advantage of using SQTP for programming the MAC Address is it
// allows you to auto-increment the MAC address without recompiling 
// the code for each unit.  To use SQTP, the MAC address must be fixed
// at a specific location in program memory.  Uncomment these two pragmas
// that locate the MAC address at 0x1FFF0.  Syntax below is for MPLAB C 
// Compiler for PIC18 MCUs. Syntax will vary for other compilers.
//#pragma romdata MACROM=0x1FFF0
static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
//#pragma romdata

static void InitAppConfig(void)
{
#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS) || defined(SPIRTCSRAM_CS_TRIS)
    unsigned char vNeedToSaveDefaults = 0;
#endif
    //static WORD SizeOfAppCfg;
    //SizeOfAppCfg = sizeof(AppConfig);
    BYTE i;
    while(1)
    {
        // Start out zeroing all AppConfig bytes to ensure all fields are 
        // deterministic for checksum generation        
        memset((void*)&AppConfig, 0x00, sizeof(AppConfig));    
        AppConfig.Flags.bIsDHCPEnabled = TRUE;
        AppConfig.Flags.bInConfigMode = TRUE;
        memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
//        {
//            _prog_addressT MACAddressAddress;
//            MACAddressAddress.next = 0x157F8;
//            _memcpy_p2d24((char*)&AppConfig.MyMACAddr, MACAddressAddress, sizeof(AppConfig.MyMACAddr));
//        }
        AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
        AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
        AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
        AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
        AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
        AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
        AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
        for(i = 0; i < 3; i++ ){
            AppConfig.RRConfig[i].K     = MY_DEFAULT_RR_PARA_K;
            AppConfig.RRConfig[i].B     = MY_DEFAULT_RR_PARA_B;
            AppConfig.RRConfig[i].Mass     = MY_DEFAULT_RR_PARA_Mass;
            AppConfig.RRConfig[i].Radius = MY_DEFAULT_RR_PARA_Radius;
            AppConfig.RRConfig[i].Length = MY_DEFAULT_RR_PARA_Length;
            AppConfig.RRConfig[i].Reduction = MY_DEFAULT_RR_PARA_Rdct;
            AppConfig.RRConfig[i].TimerStep = MY_DEFAULT_RR_PARA_TimerStep;
            AppConfig.RRConfig[i].StepPerTurn = MY_DEFAULT_RR_PARA_SPT;
            AppConfig.RRConfig[i].uStepPerStep = MY_DEFAULT_RR_PARA_uSPS;
            AppConfig.RRConfig[i].XMinPosition = MY_DEFAULT_RR_PARA_XMinPos;
            AppConfig.RRConfig[i].XMaxPosition = MY_DEFAULT_RR_PARA_XMaxPos;
            AppConfig.RRConfig[i].XParkPosition = MY_DEFAULT_RR_PARA_XParkPos;
            AppConfig.RRConfig[i].VMax = MY_DEFAULT_RR_PARA_VMax;
            AppConfig.RRConfig[i].Control = 1;
        }
    
        // SNMP Community String configuration
        #if defined(STACK_USE_SNMP_SERVER)
        {
            BYTE i;
            static ROM char * ROM cReadCommunities[] = SNMP_READ_COMMUNITIES;
            static ROM char * ROM cWriteCommunities[] = SNMP_WRITE_COMMUNITIES;
            ROM char * strCommunity;
            
            for(i = 0; i < SNMP_MAX_COMMUNITY_SUPPORT; i++)
            {
                // Get a pointer to the next community string
                strCommunity = cReadCommunities[i];
                if(i >= sizeof(cReadCommunities)/sizeof(cReadCommunities[0]))
                    strCommunity = "";
    
                // Ensure we don't buffer overflow.  If your code gets stuck here, 
                // it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h 
                // is either too small or one of your community string lengths 
                // (SNMP_READ_COMMUNITIES) are too large.  Fix either.
                if(strlenpgm(strCommunity) >= sizeof(AppConfig.readCommunity[0]))
                    while(1);
                
                // Copy string into AppConfig
                strcpypgm2ram((char*)AppConfig.readCommunity[i], strCommunity);
    
                // Get a pointer to the next community string
                strCommunity = cWriteCommunities[i];
                if(i >= sizeof(cWriteCommunities)/sizeof(cWriteCommunities[0]))
                    strCommunity = "";
    
                // Ensure we don't buffer overflow.  If your code gets stuck here, 
                // it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h 
                // is either too small or one of your community string lengths 
                // (SNMP_WRITE_COMMUNITIES) are too large.  Fix either.
                if(strlenpgm(strCommunity) >= sizeof(AppConfig.writeCommunity[0]))
                    while(1);
    
                // Copy string into AppConfig
                strcpypgm2ram((char*)AppConfig.writeCommunity[i], strCommunity);
            }
        }
        #endif
    
        // Load the default NetBIOS Host Name
        memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
        FormatNetBIOSName(AppConfig.NetBIOSName);
    
        #if defined(WF_CS_TRIS)
            // Load the default SSID Name
            WF_ASSERT(sizeof(MY_DEFAULT_SSID_NAME) <= sizeof(AppConfig.MySSID));
            memcpypgm2ram(AppConfig.MySSID, (ROM void*)MY_DEFAULT_SSID_NAME, sizeof(MY_DEFAULT_SSID_NAME));
            AppConfig.SsidLength = sizeof(MY_DEFAULT_SSID_NAME) - 1;
    
            AppConfig.SecurityMode = MY_DEFAULT_WIFI_SECURITY_MODE;
            AppConfig.WepKeyIndex  = MY_DEFAULT_WEP_KEY_INDEX;
            
            #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
                memset(AppConfig.SecurityKey, 0x00, sizeof(AppConfig.SecurityKey));
                AppConfig.SecurityKeyLength = 0;
    
            #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40
                memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_40, sizeof(MY_DEFAULT_WEP_KEYS_40) - 1);
                AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_40) - 1;
    
            #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104
                memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_104, sizeof(MY_DEFAULT_WEP_KEYS_104) - 1);
                AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_104) - 1;
    
            #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_KEY)       || \
                  (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_KEY)      || \
                  (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY)
                memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_PSK, sizeof(MY_DEFAULT_PSK) - 1);
                AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_PSK) - 1;
    
            #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_PASS_PHRASE)     || \
                  (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_PASS_PHRASE)    || \
                  (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)
                memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_PSK_PHRASE, sizeof(MY_DEFAULT_PSK_PHRASE) - 1);
                AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_PSK_PHRASE) - 1;
    
            #else 
                #error "No security defined"
            #endif /* MY_DEFAULT_WIFI_SECURITY_MODE */
    
        #endif

        // Compute the checksum of the AppConfig defaults as loaded from ROM
        wOriginalAppConfigChecksum = CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig));

        #if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)|| defined(SPIRTCSRAM_CS_TRIS)
        {
            NVM_VALIDATION_STRUCT NVMValidationStruct;

            // Check to see if we have a flag set indicating that we need to 
            // save the ROM default AppConfig values.
            if(vNeedToSaveDefaults)
                SaveAppConfig(&AppConfig);
        
            // Read the NVMValidation record and AppConfig struct out of EEPROM/Flash
            #if defined(EEPROM_CS_TRIS)
            {
                XEEReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
                XEEReadArray(sizeof(NVMValidationStruct), (BYTE*)&AppConfig, sizeof(AppConfig));
            }
            //#elif defined(SPIRTCSRAM_CS_TRIS)
            //{
            //    SPISRAMReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
            //    SPISRAMReadArray(sizeof(NVMValidationStruct), (BYTE*)&AppConfig, sizeof(AppConfig));
            //}
            #elif defined(SPIFLASH_CS_TRIS)
            {
                SPIFlashReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
                SPIFlashReadArray(sizeof(NVMValidationStruct), (BYTE*)&AppConfig, sizeof(AppConfig));
            }            
            #endif
    
            // Check EEPROM/Flash validitity.  If it isn't valid, set a flag so 
            // that we will save the ROM default values on the next loop 
            // iteration.
            if((NVMValidationStruct.wConfigurationLength != sizeof(AppConfig)) ||
               (NVMValidationStruct.wOriginalChecksum != wOriginalAppConfigChecksum) ||
               (NVMValidationStruct.wCurrentChecksum != CalcIPChecksum((BYTE*)&AppConfig, sizeof(AppConfig))))
            {
                // Check to ensure that the vNeedToSaveDefaults flag is zero, 
                // indicating that this is the first iteration through the do 
                // loop.  If we have already saved the defaults once and the 
                // EEPROM/Flash still doesn't pass the validity check, then it 
                // means we aren't successfully reading or writing to the 
                // EEPROM/Flash.  This means you have a hardware error and/or 
                // SPI configuration error.
                if(vNeedToSaveDefaults)
                {
                    while(1);
                }
                
                // Set flag and restart loop to load ROM defaults and save them
                vNeedToSaveDefaults = 1;
                continue;
            }
            
            // If we get down here, it means the EEPROM/Flash has valid contents 
            // and either matches the ROM defaults or previously matched and 
            // was run-time reconfigured by the user.  In this case, we shall 
            // use the contents loaded from EEPROM/Flash.
            break;
        }
        #endif
        break;
    }
}

#if defined(EEPROM_CS_TRIS) || defined(SPIFLASH_CS_TRIS)|| defined(SPIRTCSRAM_CS_TRIS)
void SaveAppConfig(const APP_CONFIG *ptrAppConfig)
 {
    NVM_VALIDATION_STRUCT NVMValidationStruct;

    // Ensure adequate space has been reserved in non-volatile storage to 
    // store the entire AppConfig structure.  If you get stuck in this while(1) 
    // trap, it means you have a design time misconfiguration in TCPIPConfig.h.
    // You must increase MPFS_RESERVE_BLOCK to allocate more space.
    #if (defined(STACK_USE_MPFS) || defined(STACK_USE_MPFS2)) && !defined(SPIRTCSRAM_CS_TRIS)
        if(sizeof(NVMValidationStruct) + sizeof(AppConfig) > MPFS_RESERVE_BLOCK)
            while(1);
    #endif

    // Get proper values for the validation structure indicating that we can use 
    // these EEPROM/Flash contents on future boot ups
    NVMValidationStruct.wOriginalChecksum = wOriginalAppConfigChecksum;
    NVMValidationStruct.wCurrentChecksum = CalcIPChecksum((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
    NVMValidationStruct.wConfigurationLength = sizeof(APP_CONFIG);

    // Write the validation struct and current AppConfig contents to EEPROM/Flash
    #if defined(EEPROM_CS_TRIS)
        XEEBeginWrite(0x0000);
        XEEWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
        XEEWriteArray((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
    //#elif defined(SPIRTCSRAM_CS_TRIS)
    //    SPISRAMBeginWrite(0x0000);
    //    SPISRAMWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
    //    SPISRAMWriteArray((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));        
    #else
        SPIFlashBeginWrite(0x0000);
        SPIFlashWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
        SPIFlashWriteArray((BYTE*)ptrAppConfig, sizeof(APP_CONFIG));
        
    #endif
}
#endif

void SaveRRConfig()
 {
    NVM_VALIDATION_STRUCT NVMValidationStruct;

    // Ensure adequate space has been reserved in non-volatile storage to 
    // store the entire AppConfig structure.  If you get stuck in this while(1) 
    // trap, it means you have a design time misconfiguration in TCPIPConfig.h.
    // You must increase MPFS_RESERVE_BLOCK to allocate more space.
    if(sizeof(NVMValidationStruct) + sizeof(RAMSaveConfig) > 256)
            while(1);
        
    // Get proper values for the validation structure indicating that we can use 
    // these EEPROM/Flash contents on future boot ups
    NVMValidationStruct.wOriginalChecksum = 0;
    NVMValidationStruct.wCurrentChecksum = CalcIPChecksum((BYTE*)&RRConfigRAM, sizeof(RAMSaveConfig));
    NVMValidationStruct.wConfigurationLength = sizeof(RAMSaveConfig);

    // Write the validation struct and current AppConfig contents to CMOS RAM    
    #if defined(SPIRTCSRAM_CS_TRIS)
        SPISRAMBeginWrite(0x0000);
        SPISRAMWriteArray((BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
        SPISRAMWriteArray((BYTE*)&RRConfigRAM, sizeof(RAMSaveConfig));        
    #endif
}

void LoadRRConfig()
{
    NVM_VALIDATION_STRUCT NVMValidationStruct;
    #if defined(SPIRTCSRAM_CS_TRIS)
    {
        SPISRAMReadArray(0x0000, (BYTE*)&NVMValidationStruct, sizeof(NVMValidationStruct));
        SPISRAMReadArray(sizeof(NVMValidationStruct), (BYTE*)&RRConfigRAM, sizeof(RAMSaveConfig));
    }
    #endif
    if((NVMValidationStruct.wConfigurationLength != sizeof(RAMSaveConfig)) ||       
       (NVMValidationStruct.wCurrentChecksum != CalcIPChecksum((BYTE*)&RRConfigRAM, sizeof(RAMSaveConfig))))
    {
        BYTE i;        
        for(i = 0; i<3;i++){
            RRConfigRAM.RRSave[i].Timestamp = 0;
            RRConfigRAM.RRSave[i].XPosition = 0;
            RRConfigRAM.RRSave[i].uStepPerStep = 16;
        }
    }    
}

