#ifndef __SPI_DRIVER_H
#define __SPI_DRIVER_H

#include "GenericTypeDefs.h"
#include "Compiler.h"

BYTE SPI1_Init(BYTE CS_LAT); //возвращает DevId не требует захвата устройства

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
