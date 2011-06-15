#include "SPI_driver.h"

#define BYTE unsigned short
#define WORD unsigned int

typedef struct
{	
	unsigned short CS;
	unsigned short mode;
	unsigned short a;
} SPI_PARAMS;

static void SPIInit(BYTE SPInum);
static void SPISetParam(BYTE SPInum,WORD para);
static WORD SPIRead(BYTE SPInum,WORD count,BYTE* buffer);
static WORD SPIReadw(BYTE SPInum,WORD count,WORD* buffer);
static WORD SPIWrite(BYTE SPInum,WORD count,BYTE* buffer);
static WORD SPIWritew(BYTE SPInum,WORD count,WORD* buffer);
static WORD SPIReadDMA(BYTE SPInum,WORD count,BYTE* buffer);
static WORD SPIWriteDMA(BYTE SPInum,WORD count,BYTE* buffer);
static BYTE SPILock(BYTE SPInum);
static void SPIUnLock(BYTE SPInum);

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
/* Insert ISR Code Here*/
/* Clear Timer1 interrupt */
//IFS0bits.T1IF = 0;
}

#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{
	while(1)
    {
	}
}

