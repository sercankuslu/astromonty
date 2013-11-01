/*********************************************************************
 *
 *  SPI RTC/SRAM Memory Driver
 *  - Tested with Maxim DS3234S
 *
 *********************************************************************
 * FileName:        SPIRTCSRAM.c
 * Dependencies:    SPIRTCSRAM.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.11b or higher
 *					Microchip C30 v3.23 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         
 *
 *
 * Author               		Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * D.Mosh              		   02/08/11 Original 
 ********************************************************************/
#ifndef __SPIRTSSRAM_H
#define __SPIRTSSRAM_H

#include "HardwareProfile.h"

typedef union
{
    BYTE Val;
    struct  _SECONDS {
        BYTE Sec:4;
        BYTE Sec10:3;
        BYTE Alarm:1;
    } bits;
} SECONDS;

typedef union
{
    BYTE Val;
    struct  _MINUTES {
        BYTE Min:4;
        BYTE Min10:3;
        BYTE Alarm:1;
    } bits;
} MINUTES;

typedef union
{
    BYTE Val;
    struct _HOURS {
        BYTE Hour:4;
        BYTE Hour10:2; //AM/PM  (0/1)
        BYTE Mode:1; //12/24  (1/0)
        BYTE Alarm:1;
    } bits;
} HOURS;

typedef union
{
    BYTE Val;
    struct _DAY {
        BYTE Day:3;
        BYTE b3:2;
        BYTE AlarmDY_DT:1;
        BYTE Alarm:1;
    } bits;
} DAY;

typedef union
{
    BYTE Val;
    struct _DATE{
        BYTE Date:4;
        BYTE Date10:2;
        BYTE AlarmDY_DT:1;
        BYTE Alarm:1;
    } bits;
} DATE;

typedef union
{
    BYTE Val;
    struct _MONTH {
        BYTE Month:4;
        BYTE Month10:1;
        BYTE :2;            //12/24  (1/0)
        BYTE Century:1;
    } bits;
} MONTH;

typedef union
{
    BYTE Val;
    struct _YEARS {
        BYTE Year:4;
        BYTE Year10:4;
    } bits;
} YEARS;

typedef struct _RTC_TIME
{
    SECONDS Seconds;
    MINUTES Minutes;
    HOURS   Hours;
    DAY     Day;
    DATE    Date;
    MONTH   Month;
    YEARS   Years;
} RTC_TIME;

typedef struct _RTC_ALARM1
{
    SECONDS Seconds;
    MINUTES Minutes;
    HOURS   Hours;
    DATE    Date;
} RTC_ALARM1;

typedef struct _RTC_ALARM2
{

    MINUTES Minutes;
    HOURS   Hours;
    DATE    Date;
} RTC_ALARM2;



#if defined(SPIRTCSRAM_CS_TRIS)
    void SPIRTCSRAMInit(void);
    void SPI_RTCReadRegister(BYTE bAddress, BYTE *vData);
    void SPI_RTCWriteRegister(BYTE bAddress, BYTE vData);
    //SRAM
    void SPI_RTC_SRAMReadArray(BYTE bAddress, BYTE *vData, WORD wLength, WAIT_READY WaitData);
    void SPI_RTC_SRAMWriteArray(BYTE bAddress, BYTE *vData, WORD wLength);
    //RTC    
    void SPI_RTCReadTime(RTC_TIME * Time);
    void SPI_RTCWriteTime(RTC_TIME Time);
    DWORD SPI_RTCGetUTCSeconds(void);
    DWORD GetTimeFromRTC();
    void SetRTCTimeFromUTC(DWORD Seconds);
    void SetTime();
	//void SPIRTCWriteAlarm(RTC_ALARM *vData);
    //void SPIRTCReadAlarm(RTC_ALARM *vData);
#endif

#endif
