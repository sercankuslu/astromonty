#ifndef __SPI_DRIVER_H
#define __SPI_DRIVER_H

#include "GenericTypeDefs.h"
#include "Compiler.h"

#ifndef SPI_DEV_COUNT
#define SPI_DEV_COUNT  6
#endif //SPI_DEV_COUNT
#ifndef SPI_PORT_COUNT
#define SPI_PORT_COUNT  2
#endif //SPI_PORT_COUNT

#define SPI_DEV_ACTIVE   1
#define SPI_DEV_CURRENT  2

#define SPI_ENABLE              (1<<15)
#define SPI_STOP_IDLE           (1<<13)
//flags
#define SPI_NEW_DATA_RECEIVED   (1<<6)
#define SPI_DATA_TRANSFERRED    (1<<1)
#define SPI_DATA_RECEIVED       (1<<1) 
/*
SPIxSTAT
bit 15 SPIEN: SPIx Enable bit
    1 = Enables module and configures SCKx, SDOx, SDIx and SSx as serial port pins
    0 = Disables module
bit 14 Unimplemented: Read as С0Т
bit 13 SPISIDL: Stop in Idle Mode bit
    1 = Discontinue module operation when device enters Idle mode
    0 = Continue module operation in Idle mode
bit 12-7 Unimplemented: Read as С0Т
bit 6 SPIROV: Receive Overflow Flag bit
    1 = A new byte/word is completely received and discarded. The user software has not read the
        previous data in the SPIxBUF register
    0 = No overflow has occurred
bit 5-2 Unimplemented: Read as С0Т
bit 1 SPITBF: SPIx Transmit Buffer Full Status bit
    1 = Transmit not yet started, SPIxTXB is full
    0 = Transmit started, SPIxTXB is empty
        Automatically set in hardware when CPU writes SPIxBUF location, loading SPIxTXB.
        Automatically cleared in hardware when SPIx module transfers data from SPIxTXB to SPIxSR.
bit 0 SPIRBF: SPIx Receive Buffer Full Status bit
    1 = Receive complete, SPIxRXB is full
    0 = Receive is not complete, SPIxRXB is empty
        Automatically set in hardware when SPIx transfers data from SPIxSR to SPIxRXB.
        Automatically cleared in hardware when core reads SPIxBUF location, reading SPIxRXB.
*/
#define SPI_DISABLE_SCK     (1<<12)
#define SPI_DISABLE_SDO     (1<<11)
#define SPI_ENABLE_SS       (1<<7)
#define SPI_MODE_16_8       (1<<10)
#define SPI_CLK_EDGE_HL     (1<<8)
#define SPI_CLK_POLAR_H     (1<<6)
#define SPI_MASTER          (1<<5)
/*  
SPIXCON1
bit 15-13 Unimplemented: Read as С0Т
bit 12 DISSCK: Disable SCKx pin bit (SPI Master mode only)
    1 = Internal SPI clock is disabled, pin functions as I/O
    0 = Internal SPI clock is enabled
bit 11 DISSDO: Disable SDOx pin bit
    1 = SDOx pin is not used by the module; pin functions as I/O
    0 = SDOx pin is controlled by the module
bit 10 MODE16: Word/Byte Communication Select bit
    1 = Communication is word-wide (16 bits)
    0 = Communication is byte-wide (8 bits)
bit 9 SMP: SPIx Data Input Sample Phase bit(1)
    Master mode:
    1 = Input data sampled at end of data output time
    0 = Input data sampled at middle of data output time
    Slave mode:
    SMP bit must be cleared when SPIx module is used in Slave mode.
bit 8 CKE: SPIx Clock Edge Select bit(2)
    1 = Serial output data changes on transition from active clock state to Idle clock state (refer to bit 6)
    0 = Serial output data changes on transition from Idle clock state to active clock state (refer to bit 6)
bit 7 SSEN: Slave Select Enable bit (Slave mode)
    1 = SSx pin is used for Slave mode
    0 = SSx pin is not used by the module. Pin controlled by port function
bit 6 CKP: Clock Polarity Select bit
    1 = Idle state for clock is a high level; active state is a low level
    0 = Idle state for clock is a low level; active state is a high level
bit 5 MSTEN: Master Mode Enable bit
    1 = Master mode
    0 = Slave mode
bit 4-2 SPRE<2:0>: Secondary Prescale bits (Master mode)(3)
    111 = Secondary prescale 1:1
    110 = Secondary prescale 2:1
    Х
    Х
    Х
    000 = Secondary prescale 8:1
bit 1-0 PPRE<1:0>: Primary Prescale bits (Master mode)(3)
    11 = Primary prescale 1:1
    10 = Primary prescale 4:1
    01 = Primary prescale 16:1
    00 = Primary prescale 64:1        
*/
#define SPI_FRAME_ENABLE        (1<<15)
#define SPI_FSYNC_SLAVE         (1<<14)
#define SPI_FSYNC_POLAR_HIGH    (1<<13)
#define SPI_FSYNC_COIN_FIRST    (1<<1)
/*
SPIxCON2
bit 15 FRMEN: Framed SPIx Support bit
    1 = Framed SPIx support enabled (SSx pin is used as frame sync pulse input/output)
    0 = Framed SPIx support disabled
bit 14 SPIFSD: Frame Sync Pulse Direction Control bit
    1 = Frame sync pulse input (slave)
    0 = Frame sync pulse output (master)
bit 13 FRMPOL: Frame Sync Pulse Polarity bit
    1 = Frame sync pulse is active-high
    0 = Frame sync pulse is active-low
bit 12-2 Unimplemented: Read as С0Т
bit 1 FRMDLY: Frame Sync Pulse Edge Select bit
    1 = Frame sync pulse coincides with first bit clock
    0 = Frame sync pulse precedes first bit clock
bit 0 Unimplemented: This bit must not be set to С1Т by the user application
*/
#define GetPeripheralClock() 40000000.0
typedef struct _SPI_DEV_LIST
{
    BYTE    DevID;
    BYTE    Port;
    BYTE    Status;     //SPI_DEV_ACTIVE|SPI_DEV_CURRENT
    WORD    SPIParams;  //(SPIXCON1) Master/slave, пол€рность 0:0|0:1|1:0|1:1;
                        // скорость pri=1:1|4:1|16:1|64:1; sec= 1:1...8:1
    void*   DevSelect;  //‘ункци€ дл€ выбора устройства
    void*   DevDeselect; //‘ункци€ дл€ освобождени€ устройства
} SPI_DEVICE_LIST;

BYTE SPI_Init(void);

//регистраци€ устройства
BYTE SPI_RegDevice(BYTE port, BYTE Mode, DWORD Speed, void* DevOn,void* DevOff);

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


