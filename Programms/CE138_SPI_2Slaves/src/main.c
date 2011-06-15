/**********************************************************************
* © 2005 Microchip Technology Inc.
*
* FileName:        main.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx/PIC24Hxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
* Tested On:	   dsPIC33FJ256GP710
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* David B. Martin	11/23/06	Initial Release
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ADDITIONAL NOTES: 
* The Processor starts with the External Crystal without PLL enabled and then the Clock is switched to PLL Mode.
**********************************************************************/
#if defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#elif defined(__PIC24H__)
#include "p24Hxxxx.h"
#endif


_FOSCSEL(FNOSC_FRC);					
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT);	// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
													// OSC2 Pin Function: OSC2 is Clock Output
													// Primary Oscillator Mode: XT Crystal
_FWDT(FWDTEN_OFF); 			            			// Watchdog Timer Enabled/disabled by user software

const unsigned long CRC_32_TABLE [4] = {0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EE8D};


void write_SPI1(short command)
{	
	short temp;	

	PORTGbits.RG15 = 0;		// lower the slave select line
	temp = SPI2BUF;			// dummy read of the SPI1BUF register to clear the SPIRBF flag
	SPI2BUF = command;		// write the data out to the SPI peripheral
    while (!SPI2STATbits.SPIRBF)	// wait for the data to be sent out
		;
	PORTGbits.RG15 = 1;		// raise the slave select line
}
void write_SPI2(short command)
{	
	short temp;	

	PORTGbits.RG13 = 0;		// lower the slave select line
	temp = SPI2BUF;			// dummy read of the SPI1BUF register to clear the SPIRBF flag
	SPI2BUF = command;		// write the data out to the SPI peripheral
    while (!SPI2STATbits.SPIRBF)	// wait for the data to be sent out
		;
	PORTGbits.RG13 = 1;		// raise the slave select line
}	
void delay(void)
{	
	short temp;	
	for (temp=0;temp<255;temp++)
		;
}	

void Init_SPI(void)
{

	// setup the SPI peripheral
	SPI2STAT = 0x0;  // disable the SPI module (just in case)
	SPI2CON1 = 0x0161;	// FRAMEN = 0, SPIFSD = 0, DISSDO = 0, MODE16 = 0; SMP = 0; CKP = 1; CKE = 1; SSEN = 0; MSTEN = 1; SPRE = 0b000, PPRE = 0b01
	SPI2CON1bits.CKE = 0x01;
	SPI2CON1bits.CKP = 0x00;
	SPI2STAT = 0x8000; // enable the SPI module

}
int main(void)
{

	short i,j;
	
// Configure Oscillator to operate the device at 40Mhz
// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
// Fosc= 8M*40/(2*2)=80Mhz for 8M input clock
	PLLFBD=38;					// M=40
	CLKDIVbits.PLLPOST=0;		// N1=2
	CLKDIVbits.PLLPRE=0;		// N2=2
	OSCTUN=0;					// Tune FRC oscillator, if FRC is used

// Disable Watch Dog Timer
	RCONbits.SWDTEN=0;

// Clock switching to incorporate PLL
	__builtin_write_OSCCONH(0x03);		// Initiate Clock Switch to Primary
													// Oscillator with PLL (NOSC=0b011)
	__builtin_write_OSCCONL(0x01);		// Start clock switching
	while (OSCCONbits.COSC != 0b011);	// Wait for Clock switch to occur	

// Wait for PLL to lock
	while(OSCCONbits.LOCK!=1) {};


	TRISA = 0x0;				// set to all outputs

	// setup the SPI1 port
	// setup the I/O pins properly (SDI1 as input, SDO1 as output, RG3 as output)
	PORTG = 0xFFFF;
	TRISG = 0x5F5F;
	PORTF = 0x0;
	TRISF = 0x5F;
	AD1PCFGLbits.PCFG2 = 1;		// set the RB0 pin to Digital Mode (all others to analog) 
	TRISF = 0x0080;				// set PORTF to output except for SDI1
	
	
	

	while(1)
	{
		//demonstrate the SPI peripheral in Master Mode CKE = 0, CKP = 0
	
			Init_SPI();
	
		for (i=0;i<255;i++)
			{
				write_SPI1(i);
				delay();
			} 

		for (i=0;i<255;i++)
			{
				write_SPI2(i);
				delay();
			} 

		for (i=0;i<255;i++)
			{
				write_SPI1(i);
				delay();
				write_SPI2(i);
				delay();
			} 


	}
}
/******************************************************************************/

