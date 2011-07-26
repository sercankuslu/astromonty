/*********************************************************************
 *
 *  SPI Flash Memory Driver Header
 *	- Tested to be compatible with SST25VF016B
 *  - Expected compatibility with other SST (Microchip) SST25 series 
 *    devices
 *
 *********************************************************************
 * FileName:        SPIFlash.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * E. Wood				3/20/08	Original
********************************************************************/
#ifndef __SPIRTSSRAM_H
#define __SPIRTSSRAM_H

#include "HardwareProfile.h"

//#define SPI_FLASH_SECTOR_SIZE		(262144ul)
//#define SPI_FLASH_PAGE_SIZE			(256ul)		// SST has no page boundary requirements

//#define SPI_FLASH_SECTOR_MASK		(SPI_FLASH_SECTOR_SIZE - 1)
typedef union
{
    UINT8 Val;
    struct  __PACKED
    {
         UINT8 Sec:4;
         UINT8 Sec10:4;
    } seconds;
} SECONDS;

typedef union
{
    UINT8 Val;
    struct  __PACKED
    {
         UINT8 Min:4;
         UINT8 Min10:4;
    } minutes;
} MINUTES;

typedef union
{
    UINT8 Val;
    struct __PACKED
    {
         UINT8 Hour:4;
         UINT8 Hour10:2; //AM/PM  (0/1)
         UINT8 Mode:1; //12/24  (1/0)
         UINT8 :1;
    } hours;
} HOURS;

typedef union
{
    UINT8 Val;
    struct __PACKED
    {
         UINT8 Day:3;
        
    } day;
} DAY;

typedef union
{    
    UINT8 Val;
    struct __PACKED
    {
         UINT8 Date:4;
         UINT8 Date10:4;
    } date;
} DATE;

typedef union
{    
    UINT8 Val;
    struct __PACKED
    {
         UINT8 Month:4;
         UINT8 Month10:1;
         UINT8 :2;            //12/24  (1/0)
         UINT8 Century:1;
    } month;
} MONTH;

typedef union
{    
    UINT8 Val;
    struct __PACKED
    {
         UINT8 Year:4;
         UINT8 Year10:4;
    } years;
} YEARS;

typedef struct
{
    SECONDS b0;
    MINUTES b1;
    HOURS   b2;
    DAY     b3;
    DATE    b4;
    MONTH   b5;
    YEARS   b6;
} RTC_TIME;

typedef struct
{
    SECONDS b0;
    MINUTES b1;
    HOURS   b2;
    DAY     b3;
    DATE    b4;
    MONTH   b5;
    YEARS   b6;
} RTC_ALARM1;

#if defined(SPIRTCSRAM_CS_TRIS)
	void SPIRTCSRAMInit(void);	
	//SRAM
	void SPISRAMReadArray(DWORD dwAddress, BYTE *vData, WORD wLength);
	void SPISRAMBeginWrite(DWORD dwAddr);
	void SPISRAMWrite(BYTE vData);
	void SPISRAMWriteArray(BYTE *vData, WORD wLength);	
	//RTC
	void SPIRTCWriteTime(void);
    void SPIRTCReadTime(void);
    DWORD RTCGetUTCSeconds(void);
    void RTCSetUTCSeconds(DWORD Seconds);
    void SPIRTCSetAlarm1PerSec(void);
	//void SPIRTCWriteAlarm(RTC_ALARM *vData);
    //void SPIRTCReadAlarm(RTC_ALARM *vData);
#else
	// If you get any of these linker errors, it means that you either have an 
	// error in your HardwareProfile.h or TCPIPConfig.h definitions.  The code 
	// is attempting to call a function that can't possibly work because you 
	// have not specified what pins and SPI module the physical SPI Flash chip 
	// is connected to.  Alternatively, if you don't have an SPI Flash chip, it 
	// means you have enabled a stack feature that requires SPI Flash hardware.
	// In this case, you need to edit TCPIPConfig.h and disable this stack 
	// feature.  The linker error tells you which object file this error was 
	// generated from.  It should be a clue as to what feature you need to 
	// disable.
	void You_cannot_call_the_SPISRAMInit_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPISRAMReadArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPISRAMBeginWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPISRAMWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPISRAMWriteArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	#define SPISRAMInit()				You_cannot_call_the_SPIFlashInit_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPISRAMReadArray(a,b,c)	You_cannot_call_the_SPIFlashReadArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPISRAMBeginWrite(a)		You_cannot_call_the_SPIFlashBeginWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPISRAMWrite(a)			You_cannot_call_the_SPIFlashWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPISRAMWriteArray(a,b)		You_cannot_call_the_SPIFlashWriteArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
#endif

#endif
