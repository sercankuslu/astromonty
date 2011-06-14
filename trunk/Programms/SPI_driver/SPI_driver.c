#include "SPI_driver.h"

#define BYTE unsigned short
#define WORD unsigned int

typedef struct
{	
	unsigned short CS;
	unsigned short mode;
	unsigned short ;
} SPI_PARAMS;

static void SPIInit(BYTE SPInum);
static void SPISetParam(BYTE SPInum,WORD para);
static WORD SPIRead(BYTE SPInum,WORD count,BYTE* buffer);
static WORD SPIRead(BYTE SPInum,WORD count,WORD* buffer);
static WORD SPIWrite(BYTE SPInum,WORD count,BYTE* buffer);
static WORD SPIWrite(BYTE SPInum,WORD count,WORD* buffer);
static WORD SPIReadDMA(BYTE SPInum,WORD count,BYTE* buffer);
static WORD SPIWriteDMA(BYTE SPInum,WORD count,BYTE* buffer);
static BYTE SPILock(BYTE SPInum);
static void SPIUnLock(BYTE SPInum);

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

