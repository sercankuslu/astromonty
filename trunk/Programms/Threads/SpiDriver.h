#ifndef __SPI_DRIVER_H
#define __SPI_DRIVER_H

#include "GenericTypeDefs.h"
#include "Compiler.h"

#ifndef SPI_DEV_COUNT
#define SPI_DEV_COUNT = 6
#endif //SPI_DEV_COUNT
#ifndef SPI_PORT_COUNT
#define SPI_PORT_COUNT = 2
#endif //SPI_PORT_COUNT

#define SPI_DEV_ACTIVE   1
#define SPI_DEV_CURRENT  2

typedef struct _SPI_DEV_LIST
{
    BYTE    DevID;
    BYTE    Port;
    BYTE    Status;//SPI_DEV_ACTIVE|SPI_DEV_CURRENT
    BYTE    SPIParams; //пол€рность 0:0|0:1|1:0|1:1; скорость pri=1:1|4:1|16:1|64:1; sec= 1:1...8:1
    void*   DevSelect;  //‘ункци€ дл€ выбора устройства
    void*   DevDeselect; //‘ункци€ дл€ освобождени€ устройства
} SPI_DEVICE_LIST;

BYTE SPI_Init(void);

BYTE SPI_RegDevice(BYTE port, void* DevOn,void* DevOff); //возвращает DevId не требует захвата устройства

BOOL SPI1_Open(BYTE DevId);
void SPI1_Close(BYTE DevId);

BYTE SPI1_Write_Byte(BYTE* data);
BYTE SPI1_Write_WORD(WORD* data);
BYTE SPI1_Write_DWORD(DWORD* data);
WORD SPI1_Write_Aray(BYTE* data,WORD length);

BYTE SPI1_Read_Byte(BYTE* data);
BYTE SPI1_Read_WORD(WORD* data);
BYTE SPI1_Read_DWORD(DWORD* data);
WORD SPI1_Read_Aray(BYTE* data,WORD length);
#endif //__SPI_DRIVER_H
