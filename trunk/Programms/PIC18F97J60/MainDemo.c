/*********************************************************************
 *
 *  Main Application Entry Point and TCP/IP Stack Demo
 *  Module for Microchip TCP/IP Stack
 *   -Demonstrates how to call and use the Microchip TCP/IP stack
 *	 -Reference: Microchip TCP/IP Stack Help (TCPIP Stack Help.chm)
 *
 *********************************************************************
 * FileName:        MainDemo.c
 * Dependencies:    TCPIP.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.11b or higher
 *					Microchip C30 v3.24 or higher
 *					Microchip C18 v3.36 or higher
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
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
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
 * Nilesh Rajbharti		4/19/01		Original (Rev. 1.0)
 * Nilesh Rajbharti		2/09/02		Cleanup
 * Nilesh Rajbharti		5/22/02		Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti		7/9/02		Rev 2.1 (See version.log for detail)
 * Nilesh Rajbharti		4/7/03		Rev 2.11.01 (See version log for detail)
 * Howard Schlunder		10/1/04		Beta Rev 0.9 (See version log for detail)
 * Howard Schlunder		10/8/04		Beta Rev 0.9.1 Announce support added
 * Howard Schlunder		11/29/04	Beta Rev 0.9.2 (See version log for detail)
 * Howard Schlunder		2/10/05		Rev 2.5.0
 * Howard Schlunder		1/5/06		Rev 3.00
 * Howard Schlunder		1/18/06		Rev 3.01 ENC28J60 fixes to TCP, 
 *									UDP and ENC28J60 files
 * Howard Schlunder		3/01/06		Rev. 3.16 including 16-bit micro support
 * Howard Schlunder		4/12/06		Rev. 3.50 added LCD for Explorer 16
 * Howard Schlunder		6/19/06		Rev. 3.60 finished dsPIC30F support, added PICDEM.net 2 support
 * Howard Schlunder		8/02/06		Rev. 3.75 added beta DNS, NBNS, and HTTP client (GenericTCPClient.c) services
 * Howard Schlunder		12/28/06	Rev. 4.00RC added SMTP, Telnet, substantially modified TCP layer
 * Howard Schlunder		04/09/07	Rev. 4.02 added TCPPerformanceTest, UDPPerformanceTest, Reboot and fixed some bugs
 * Howard Schlunder		xx/xx/07	Rev. 4.03
 * HSchlunder & EWood	08/27/07	Rev. 4.11
 * HSchlunder & EWood	10/08/07	Rev. 4.13
 * HSchlunder & EWood	11/06/07	Rev. 4.16
 * HSchlunder & EWood	11/08/07	Rev. 4.17
 * HSchlunder & EWood	11/12/07	Rev. 4.18
 * HSchlunder & EWood	02/11/08	Rev. 4.19
 * HSchlunder & EWood   04/26/08    Rev. 4.50 Moved most code to other files for clarity
 * KHesky               07/07/08    Added ZG2100-specific support
 * HSchlunder & EWood   07/24/08    Rev. 4.51
 * Howard Schlunder		11/10/08    Rev. 4.55
 * Howard Schlunder		04/14/09    Rev. 5.00
 * Howard Schlunder		07/10/09    Rev. 5.10
 * Howard Schlunder		11/18/09    Rev. 5.20
 * Howard Schlunder		04/28/10    Rev. 5.25
 * Howard Schlunder		10/19/10	Rev. 5.31
 ********************************************************************/
/*
 * This macro uniquely defines this file as the main entry point.
 * There should only be one such definition in the entire project,
 * and this file must define the AppConfig variable as described below.
 */
#define THIS_IS_STACK_APPLICATION

// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"
#include "pcf8535.h"
#include <i2c.h>



#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
#include "TCPIP Stack/ZeroconfLinkLocal.h"
#endif
#if defined(STACK_USE_ZEROCONF_MDNS_SD)
#include "TCPIP Stack/ZeroconfMulticastDNS.h"
#endif

// Include functions specific to this stack application
#include "MainDemo.h"
//#include "pcf8535.h"
// Used for Wi-Fi assertions
#define WF_MODULE_NUMBER   WF_MODULE_MAIN_DEMO

// Declare AppConfig structure and some other supporting stack variables
APP_CONFIG AppConfig;
static unsigned short wOriginalAppConfigChecksum;	// Checksum of the ROM defaults for AppConfig
BYTE AN0String[8];

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
#if defined(WF_CS_TRIS)
    static void WF_Connect(void);
#endif

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
		//LED0_IO ^= 1;
	}
	
	#if defined(HI_TECH_C)
	void interrupt HighISR(void)
	#else
	#pragma interruptlow HighISR
	void HighISR(void)
	#endif
	{
	    #if defined(STACK_USE_UART2TCP_BRIDGE)
			UART2TCPBridgeISR();
		#endif

		#if defined(WF_CS_TRIS)
			WFEintISR();
		#endif // WF_CS_TRIS
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

void LCDSendData(unsigned char add1,unsigned char* I2C_Send, unsigned char size);
void LCDSendCommand(unsigned char add1,unsigned char* wrptr, unsigned char size);
void LCDClearData(unsigned char add1);
void LCDSetXY(unsigned char add1, unsigned char X,unsigned char Y);

//unsigned char I2C_Send[21] = "MICROCHIP:I2C_MASTER" ;
unsigned char I2C_Recv[21];
static unsigned char I2C_Send[] =
{	
	modeLCD_CMD_TILL_STOP
	,
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_COMMAND_PAGE
	| paramLCD_PAGE_DISPLAY_SETTINGS
	,
	cmdLCD_BIAS_SYSTEM
	| paramLCD_BIAS_9 // Bias 1/9, MUX 1:65
	,
	cmdLCD_MULTILPEX_RATE
	| paramLCD_MUX_65
	,
	cmdLCD_DISPLAY_CONTROL
	| paramLCD_NORMAL
	,
	cmdLCD_EXT_DISPLAY_CTL
	| paramLCD_XDIRECTION_REVERSE
	| paramLCD_YDIRECTION_REVERSE 
	,
	cmdLCD_DISPLAY_SIZE
	| paramLCD_LARGE_DISPLAY
	,
	
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_COMMAND_PAGE
	| paramLCD_PAGE_HV_GEN
	,
	cmdLCD_HV_GEN_CONTROL
	| paramLCD_HV_ENABLED
	| paramLCD_VLCD_LOW
	,
	cmdLCD_HV_GEN_STAGES
	| paramLCD_HV_MUL5
	,
	cmdLCD_TEMP_COEFF
	| paramLCD_TC_274
	,
	cmdLCD_VLCD_CONTROL
	| 51
	,
	
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_COMMAND_PAGE
	| paramLCD_PAGE_SPECIAL_FEATURE
	,
	cmdLCD_STATE_CONTROL
	| 0
	,
	cmdLCD_OSC_SETTING
	| paramLCD_OSC_INTERNAL
	,
	cmdLCD_COG_TCP
	| paramLCD_BOT_ROW_SWAP_ENABLED
	| paramLCD_TOP_ROW_SWAP_ENABLED
	,
	
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_FUNCTION_SET
	| paramLCD_ACTIVE
	| paramLCD_HORIZONTAL_ADDRESSING
	, 
	cmdLCD_RAM_PAGE
	| paramLCD_PAGE0
	,
	cmdLCD_SET_Y
	| 0
	, 
	cmdLCD_SET_X
	| 0, 
		
};
static unsigned char I2C_Home[] =
{
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_SET_Y
	| 0
	,
	cmdLCD_SET_X
	| 0
	,
};


static unsigned char I2C_Send1[]={0x80,0xE0,0xB8,0x17,0x9F,0xFC,0xE0,0x80, //A
								0x81,0xFF,0xFF,0x89,0x89,0xFF,0x76,0x00, //B
								0x3C,0x7E,0xC3,0x81,0x81,0x82,0x47,0x00, //C
								0x81,0xFF,0xFF,0x81,0x81,0xC3,0x7E,0x3C, //D
								0x81,0xFF,0xFF,0x89,0x9D,0xC3, //E
								0x81,0xFF,0xFF,0x89,0x1D,0x03,0x00, //F
								0x3C,0x7E,0xC3,0x81,0xA1,0xE2,0x67,0x20, //G
								0x81,0xFF,0xFF,0x89,0x08,0x89,0xFF,0xFF,0x81, //H
								0x81,0xFF,0xFF,0x81, //I
								0x60,0xE0,0x81,0xFF,0x7F,0x01, //J
								0x81,0xFF,0xFF,0x99,0x3C,0xE3,0xC1,0x81, //K
								0x81,0xFF,0xFF,0x81,0x80,0xC0,0xE0, //L
								0x81,0xFF,0x87,0x3F,0xF8,0x3C,0x87,0xFF,0xFF,0x81, //M
								0x81,0xFF,0x83,0x0E,0x1C,0x71,0xFF,0x01, //N
								0x3C,0x7E,0xC3,0x81,0x81,0xC3,0x7E,0x3C, //O
								0x81,0xFF,0xFF,0x91,0x11,0x11,0x1F,0x0E, //P
								0x3C,0x7E,0xC3,0x81,0xA1,0xC3,0xFE,0xBC, //Q 
};
static unsigned char I2C_Send2[]={0x81,0xFF,0xFF,0x89,0x39,0xF7,0xCC,0x80, //R
								0xCE,0x5F,0x99,0xFA,0x73, //S
								0x03,0x81,0xFF,0xFF,0x81,0x03, //T
								0x01,0x7F,0xFF,0x81,0x80,0x81,0x7F,0x01, //U
								0x01,0x07,0x3F,0xF9,0xC0,0x39,0x07,0x01, //V
								0x01,0x07,0x3F,0xF8,0xE1,0x1B,0x1F,0xF9,0xE0,0x1D,0x03,0x01, //W
								0x81,0xC3,0xAF,0x3D,0xF4,0xC3,0x81, //X
								0x01,0x03,0x8F,0xFD,0xF0,0x8D,0x03,0x01, //Y
								0x80,0xC7,0xF1,0xBD,0x8F,0xC3,0xE1, //Z

};
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
	static DWORD t1 = 0;
	static DWORD dwLastIP = 0;
	BYTE b = 0;
	unsigned char sync_mode=0;
	unsigned char slew=0; 
	unsigned char add1; 
	unsigned char w; 
	unsigned char i; 
	unsigned char data; 
	unsigned char status; 
	unsigned char length;
	
	OSCTUNE = 0x40;
	OSCCON = 0x02;

	TRISAbits.TRISA0=0;
	TRISAbits.TRISA1=0;
	LATAbits.LATA0 =0;
	LATAbits.LATA1 =0;
	//DelayMs(1000);
	//LATAbits.LATA0 =1;
	//DelayMs(1000);
	//LATAbits.LATA1 =1;

	TRISCbits.TRISC3=1;
	TRISCbits.TRISC4=1;
	LATGbits.LATG4 =0;
	LATCbits.LATC5 =1;
	TRISCbits.TRISC5=0;	 //LCD reset
	TRISGbits.TRISG4=0;
	LATCbits.LATC5 =0;
	DelayMs(500);
	LATCbits.LATC5 =1;
	//LATGbits.LATG4 =1;
	

	// Initialize stack-related hardware components that may be 
	// required by the UART configuration routines
	
    TickInit();
	//DelayMs(1000);
	//LATAbits.LATA0 =0;
	//LATAbits.LATA1 =0;

	//DelayMs(1000);
	// вывели lcd из ресета

	for(w=0;w<20;w++)
    I2C_Recv[w]=0;

    add1=PCF8535_BUS_ADDRESS;        //address of the device (slave) under communication

    CloseI2C();    //close i2c if was operating earlier


	//---INITIALISE THE I2C MODULE FOR MASTER MODE WITH 100KHz ---
    sync_mode = MASTER;
    slew = SLEW_OFF;
    OpenI2C(sync_mode,slew);
    SSPADD=0x0A;             //400kHz Baud clock(9) @8MHz
//check for bus idle condition in multi master communication
    IdleI2C();

//---START I2C---
    StartI2C();

	LCDSendCommand(add1,I2C_Send,sizeof(I2C_Send));
	LCDClearData(add1);
	for(i=7;i>0;i--){
		LCDSetXY(add1,0,i);
		LCDSendData(add1,I2C_Send1,sizeof(I2C_Send1));
	}
	
	
	//LCDSetXY(add1,0,5);
	//LCDSendData(add1,I2C_Send2,sizeof(I2C_Send2));

    while(1)
    {
        // Blink LED0 (right most one) every second.
		/*
        if(TickGet() - t >= TICK_SECOND/2ul)
        {
            t = TickGet();
            LATGbits.LATG4 ^= 1;
        }
		*/
		
		t++;
		if(t>=200){
			t=0;
			LATGbits.LATG4 ^= 1;
			//LATAbits.LATA1 ^= 1;
			t1++;
			if(t1>=255){
				t1=0;
			}
			//lcd_set_contrast(t1);
			
		}
			
	}
}


void LCDSendData(unsigned char add1,unsigned char* wrptr, unsigned char size)
{
	unsigned char sync_mode=0;
	unsigned char slew=0; 
	unsigned char i; 
	unsigned char w; 
	unsigned char data; 
	unsigned char status; 
	unsigned char length;

	RestartI2C();
    IdleI2C();

	//****write the address of the device for communication***
    data = SSPBUF;        //read any previous stored content in buffer to clear buffer full status
    do
    {
    status = WriteI2C( add1 | 0x00 );    //write the address of slave
        if(status == -1)        //check if bus collision happened
        {
            data = SSPBUF;        //upon bus collision detection clear the buffer,
            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
			LATAbits.LATA1 =1;
        }
    }
    while(status!=0);        //write untill successful communication
	//R/W BIT IS '0' FOR FURTHER WRITE TO SLAVE
	do
    {
    status = WriteI2C(modeLCD_DATA_TILL_STOP);    //write the address of slave
        if(status == -1)        //check if bus collision happened
        {
            data = SSPBUF;        //upon bus collision detection clear the buffer,
            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
			LATAbits.LATA1 =1;
        }
    }
    while(status!=0);        //write untill successful communication
	//***WRITE THE THE DATA TO BE SENT FOR SLAVE***
	//write string of data to be transmitted to slave
   	for (i=0;i<size;i++ )                 // transmit data 
	{
	  	if ( SSP1CON1bits.SSPM3 )      // if Master transmitter then execute the following
		{
			//temp = putcI2C1 ( *wrptr );
			//if (temp ) return ( temp );   	
			if ( WriteI2C1( *wrptr ) )    // write 1 byte
			{
			  return ( -3 );             // return with write collision error
			}
			IdleI2C1();                  // test for idle condition
			if ( SSP1CON2bits.ACKSTAT )  // test received ack bit state
			{
			  return ( -2 );             // bus device responded with  NOT ACK
			}                            // terminate putsI2C1() function
		}	  
	wrptr ++;                        // increment pointer
	}            
    
	
//---TERMINATE COMMUNICATION FROM MASTER SIDE---
     IdleI2C();
}
void LCDSendCommand(unsigned char add1,unsigned char* wrptr, unsigned char size)
{
	unsigned char sync_mode=0;
	unsigned char slew=0; 
	unsigned char i; 
	unsigned char w; 
	unsigned char data; 
	unsigned char status; 
	unsigned char length;
	//****write the address of the device for communication***
  	RestartI2C();
    IdleI2C();

    data = SSPBUF;        //read any previous stored content in buffer to clear buffer full status
    do
    {
    status = WriteI2C( add1 | 0x00 );    //write the address of slave
        if(status == -1)        //check if bus collision happened
        {
            data = SSPBUF;        //upon bus collision detection clear the buffer,
            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
			LATAbits.LATA1 =1;
        }
    }
    while(status!=0);        //write untill successful communication
	//R/W BIT IS '0' FOR FURTHER WRITE TO SLAVE
	do
    {
    status = WriteI2C(modeLCD_CMD_TILL_STOP);    //write the address of slave
        if(status == -1)        //check if bus collision happened
        {
            data = SSPBUF;        //upon bus collision detection clear the buffer,
            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
			LATAbits.LATA1 =1;
        }
    }
    while(status!=0);        //write untill successful communication
	//***WRITE THE THE DATA TO BE SENT FOR SLAVE***
	//write string of data to be transmitted to slave
   	for (i=0;i<size;i++ )                 // transmit data 
	{
	  	if ( SSP1CON1bits.SSPM3 )      // if Master transmitter then execute the following
		{
			//temp = putcI2C1 ( *wrptr );
			//if (temp ) return ( temp );   	
			if ( WriteI2C1( *wrptr ) )    // write 1 byte
			{
			  return ( -3 );             // return with write collision error
			}
			IdleI2C1();                  // test for idle condition
			if ( SSP1CON2bits.ACKSTAT )  // test received ack bit state
			{
			  return ( -2 );             // bus device responded with  NOT ACK
			}                            // terminate putsI2C1() function
		}	  
	wrptr ++;                        // increment pointer
	}            
    
	
//---TERMINATE COMMUNICATION FROM MASTER SIDE---
     IdleI2C();
}

void LCDClearData(unsigned char add1)
{
	unsigned char sync_mode=0;
	unsigned char slew=0; 
	unsigned char i; 
	unsigned char j; 
	unsigned char w; 
	unsigned char data; 
	unsigned char status; 
	unsigned char length;
		
	for(j=0;j<6;j++){
		LCDSetXY(add1,0,j);
		RestartI2C();
	    IdleI2C();
	
		//****write the address of the device for communication***
	    data = SSPBUF;        //read any previous stored content in buffer to clear buffer full status
	    do
	    {
	    status = WriteI2C( add1 | 0x00 );    //write the address of slave
	        if(status == -1)        //check if bus collision happened
	        {
	            data = SSPBUF;        //upon bus collision detection clear the buffer,
	            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
				LATAbits.LATA1 =1;
	        }
	    }
	    while(status!=0);        //write untill successful communication
		//R/W BIT IS '0' FOR FURTHER WRITE TO SLAVE
		do
	    {
	    status = WriteI2C(modeLCD_DATA_TILL_STOP);    //write the address of slave
	        if(status == -1)        //check if bus collision happened
	        {
	            data = SSPBUF;        //upon bus collision detection clear the buffer,
	            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
				LATAbits.LATA1 =1;
	        }
	    }
	    while(status!=0);        //write untill successful communication
		//***WRITE THE THE DATA TO BE SENT FOR SLAVE***
		//write string of data to be transmitted to slave
	   	for (i=0;i<128;i++ )                 // transmit data 
		{
		  	if ( SSP1CON1bits.SSPM3 )      // if Master transmitter then execute the following
			{
				//temp = putcI2C1 ( *wrptr );
				//if (temp ) return ( temp );   	
				if ( WriteI2C1( 0 ) )    // write 1 byte
				{
				  return ( -3 );             // return with write collision error
				}
				IdleI2C1();                  // test for idle condition
				if ( SSP1CON2bits.ACKSTAT )  // test received ack bit state
				{
				  return ( -2 );             // bus device responded with  NOT ACK
				}                            // terminate putsI2C1() function
			}	  	          
		}         
	    
		
	//---TERMINATE COMMUNICATION FROM MASTER SIDE---
	     IdleI2C();
	}
}
void LCDSetXY(unsigned char add1, unsigned char X,unsigned char Y)
{
	unsigned char temp[3];

	temp[0]= cmdLCD_DEFAULT_PAGE;
	temp[1]= cmdLCD_SET_Y | Y;
	temp[2]= cmdLCD_SET_X | X;
	LCDSendCommand(add1,temp,3);
}
