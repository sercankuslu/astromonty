/**********************************************************************
* © 2005 Microchip Technology Inc.
*
* FileName:        spi1Drv.c
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
* RK	          04/06/06 	  First release of source file
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* ADDITIONAL NOTES.
**********************************************************************/
#if defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#elif defined(__PIC24H__)
#include "p24Hxxxx.h"
#endif

void cfgDma0SpiTx(void);
void cfgDma1SpiRx(void);
void cfgSPI1Master(void);

void ProcessSpiRxSamples(unsigned int * SpiRxBuffer);

unsigned int Spi1RxBuffA[16] __attribute__((space(dma)));
unsigned int Spi1RxBuffB[16] __attribute__((space(dma)));
unsigned int Spi1TxBuffA[16] __attribute__((space(dma)));
unsigned int Spi1TxBuffB[16] __attribute__((space(dma)));
/*=============================================================================
_DMA0Init(): Initialise DMA0 for SPI Data Transmission 
=============================================================================*/
// DMA0 configuration
// Direction: Read from DMA RAM and write to SPI buffer
// AMODE: Register Indirect with Post-Increment mode
// MODE: Continuous, Ping-Pong Enabled
// IRQ: SPI

void cfgDma0SpiTx(void)
{

	DMA0CON = 0x2002;					
	DMA0CNT = 15;						
	DMA0REQ = 0x00A;					

	DMA0PAD = (volatile unsigned int) &SPI1BUF;
	DMA0STA= __builtin_dmaoffset(Spi1TxBuffA);
	DMA0STB= __builtin_dmaoffset(Spi1TxBuffB);
	
	
	IFS0bits.DMA0IF  = 0;			// Clear DMA interrupt
	IEC0bits.DMA0IE  = 1;			// Enable DMA interrupt
	DMA0CONbits.CHEN = 1;			// Enable DMA Channel	
	
}

// DMA1 configuration
// Direction: Read from SPI buffer and write to DMA RAM 
// AMODE: Register Indirect with Post-Increment mode
// MODE: Continuous, Ping-Pong Enabled
// IRQ: SPI
void cfgDma1SpiRx(void)
{
	DMA1CON = 0x0002;				
	DMA1CNT = 15;						
	DMA1REQ = 0x00A;					

	DMA1PAD = (volatile unsigned int) &SPI1BUF;
	DMA1STA= __builtin_dmaoffset(Spi1RxBuffA);
	DMA1STB= __builtin_dmaoffset(Spi1RxBuffB);
	
	
	IFS0bits.DMA1IF  = 0;			// Clear DMA interrupt
	IEC0bits.DMA1IE  = 1;			// Enable DMA interrupt
	DMA1CONbits.CHEN = 1;			// Enable DMA Channel		
	
}

void cfgSpi1Master(void)
{

// Configure SPI2CON register to the following
// •	Idle state for clock is a low level (SPI1CON1bits.CKP=?)
// •	Data out on Active to Idle Edge (SPI1CON1bits.CKE=?)
// •	16-bit data transfer mode (SPI1CON1bits.MODE16=?)
// •	Enable Master mode (SPI1CON1bits.MSTEN=?)
// •	Set Primary Pre-scalar for 4:1 ratio (SPI1CON1bits.PPRE=?)
// •	Set Secondary Pre-scalar for 2:1 ratio (SPI1CON1bits.SPRE=?)
// •	Enable SDO output (SPI1CON1bits.DISSDO=?)
// •	Enable SCK output (SPI1CON1bits.DISSCK=?)
		SPI1CON1bits.CKP=0; 
		SPI1CON1bits.CKE=1; 
		SPI1CON1bits.MODE16=1; 
		SPI1CON1bits.MSTEN=1; 
		SPI1CON1bits.SPRE=6; 
		SPI1CON1bits.PPRE=2;
 		SPI1CON1bits.DISSDO=0;
		SPI1CON1bits.DISSCK = 0;	
	

// •	Enable SPI module (SPI1STATbits.SPIEN=?)
		SPI1STATbits.SPIEN=1; 


// Force First word after Enabling SPI
    	DMA0REQbits.FORCE=1;
    	while(DMA0REQbits.FORCE==1);

	
}


/*=============================================================================
Initialise SPI Buffer
=============================================================================*/

void initSPIBuff(void)
{
unsigned int i;
    for(i=0;i<16;i++)
        Spi1TxBuffA[i]=i;
    for(i=0;i<16;i++)
        Spi1TxBuffB[i]=16+i;        
        
   	for(i=0;i<16;i++){
        Spi1RxBuffA[i]=0xDEED;
        Spi1RxBuffB[i]=0xDEED;
		}	
}






/*=============================================================================
Interrupt Service Routines.
=============================================================================*/
unsigned int RxDmaBuffer = 0;

void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{

      IFS0bits.DMA0IF = 0;		// Clear the DMA0 Interrupt Flag;

}



void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)
{
	if(RxDmaBuffer == 0)
	{
		ProcessSpiRxSamples(&Spi1RxBuffA[0]);

	}
	else
	{
		ProcessSpiRxSamples(&Spi1RxBuffB[0]);
	}

	RxDmaBuffer ^= 1;

	tglPin();					// Toggle RA6	
    IFS0bits.DMA1IF = 0;		// Clear the DMA0 Interrupt Flag
}

void ProcessSpiRxSamples(unsigned int * SpiRxBuffer)
{
	/* Do something with SPI Samples */
}




