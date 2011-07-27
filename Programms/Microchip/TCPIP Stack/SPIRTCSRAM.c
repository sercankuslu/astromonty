/*********************************************************************
 *
 *  SPI Flash Memory Driver
 *  - Tested with SST 25VF016B 
 *  - Expected compatibility with other SST (Microchip) SST25 series 
 *    devices
 *
 *********************************************************************
 * FileName:        SPIRTCSRAM.c
 * Dependencies:    SPIRTCSRAM.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.11b or higher
 *					Microchip C30 v3.23 or higher
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
 * Author               		Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * E. Wood              		3/20/08 Original
 * Dave Collier/H. Schlunder	6/09/10	Update for SST25VF010A
********************************************************************/
#define __SPIRTCSRAM_C

#include "HardwareProfile.h"

#if defined(SPIRTCSRAM_CS_TRIS)

#include "TCPIPConfig.h"
#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/SPIRTCSRAM.h"


#define WRITEMASK				0x80
#define RTC_SECONDS				0x00
#define RTC_MINUTES				0x01
#define RTC_HOUR				0x02
#define RTC_DAY					0x03 	//ƒень недели
#define RTC_DATE				0x04	//день мес€ца
#define RTC_MONTH				0x05	//мес€ц + ¬ек
#define RTC_YEAR				0x06
#define RTC_SECONDS_ALARM1		0x07
#define RTC_MINUTES_ALARM1		0x08
#define RTC_HOUR_ALARM1			0x09
#define RTC_DAY_ALARM1			0x0A 	//ƒень недели
#define RTC_MINUTES_ALARM2		0x0B
#define RTC_HOUR_ALARM2			0x0C
#define RTC_DAY_ALARM2			0x0D	 	//ƒень недели
#define RTC_CONTROL				0x0E
#define RTC_STATUS				0x0F
#define RTC_CRYSTAL_AGGING		0x10
#define RTC_TEMP_MSB			0x11
#define RTC_TEMP_LSB			0x12
#define RTC_TEMP_CONTROL		0x13
#define SRAM_ADDRES				0x18
#define SRAM_DATA				0x19

#define SEC_IN_365_DAY 31536000
#define SEC_IN_366_DAY 31622400
#define SEC_1970_2000 978307200


#if defined(__PIC24F__) || defined(__PIC24FK__)
    #define RTC_PROPER_SPICON1  (0x001B | 0x0120)   // 1:1 primary prescale, 2:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
    #define RTC_PROPER_SPICON1  (0x0016 | 0x0020)   // 4:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__dsPIC30F__)
    #define RTC_PROPER_SPICON1  (0x0017 | 0x0120)   // 1:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__PIC32MX__)
    #define RTC_PROPER_SPICON1  (_SPI2CON_ON_MASK | _SPI2CON_FRZ_MASK | _SPI2CON_CKE_MASK | _SPI2CON_MSTEN_MASK)
#else
    #define RTC_PROPER_SPICON1  (0x20)              // SSPEN bit is set, SPI in master mode, FOSC/4, IDLE state is low level
#endif

// Maximum speed of SPI Flash part in Hz
// Should theoretically operate at 25MHz, but need to account for level-shifting delays
#define SPIRTCSRAM_MAX_SPI_FREQ       (16000000ul)

#if defined (__18CXX)
    #define ClearSPIDoneFlag()  {SPIRTCSRAM_SPI_IF = 0;}
    #define WaitForDataByte()   {while(!SPIRTCSRAM_SPI_IF); SPIRTCSRAM_SPI_IF = 0;}
    #define SPI_ON_BIT          (SPIRTCSRAM_SPICON1bits.SSPEN)
#elif defined(__C30__)
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while ((SPIRTCSRAM_SPISTATbits.SPITBF == 1) || (SPIRTCSRAM_SPISTATbits.SPIRBF == 0));
    }

    #define SPI_ON_BIT          (SPIRTCSRAM_SPISTATbits.SPIEN)
#elif defined( __PIC32MX__ )
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while (!SPIRTCSRAM_SPISTATbits.SPITBE || !SPIRTCSRAM_SPISTATbits.SPIRBF);
    }

    #define SPI_ON_BIT          (SPIRTCSRAM_SPICON1bits.ON)
#else
    #error Determine SPI flag mechanism
#endif

// Internal pointer to address being written
static DWORD dwWriteAddr;

// Seconds value obtained by last update
static DWORD dwRTCSeconds = 0;

// Tick count of last update
static DWORD dwRTCLastUpdateTick = 0;
// Time Struct
static volatile RTC_TIME Time;

static void _WaitWhileBusy(void);
static void spi_write(BYTE x);
static BYTE spi_read(void);


//static void _GetStatus(void);
static void Wait400ns()
{
    BYTE i;
    for(i=0;i<8;i++){Nop();}
}

static void WriteAddressReg(BYTE Addr)
{
    volatile BYTE Dummy;
    SPIRTCSRAM_CS_IO = 0;    
    Dummy = SRAM_ADDRES | WRITEMASK;
    spi_write(Dummy);
    spi_write(Addr);
    SPIRTCSRAM_CS_IO = 1;    
    Wait400ns();
}

static void spi_write(BYTE x){
    volatile BYTE Dummy;
    SPIRTCSRAM_SSPBUF = x;
    WaitForDataByte();
    Dummy = SPIRTCSRAM_SSPBUF;    
}


static BYTE spi_read(void){
    BYTE dat;
    SPIRTCSRAM_SSPBUF = 0x00;
    WaitForDataByte();
    dat = SPIRTCSRAM_SSPBUF;    
    return(dat);
}        

static void _WaitWhileBusy(void){
    BYTE x;   
    do {    
        Wait400ns();
        SPIRTCSRAM_CS_IO = 0;
        spi_write(0x0f);    //control/status register
        x = spi_read();
        SPIRTCSRAM_CS_IO = 1;
        x = (x & 0x04) >> 2;        
    } while(x);
}

/*****************************************************************************
  Function:
    void SPIRTCSRAMInit(void)

  Description:
    Initializes SPI Flash module.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is only called once during the lifetime of the application.

  Internal:
    This function sends WRDI to clear any pending write operation, and also
    clears the software write-protect on all memory locations.
  ***************************************************************************/
void SPIRTCSRAMInit(void)
{
	//BYTE i;
	//BYTE seconds;
	//BYTE minutes;
    //BYTE hours;
    BYTE temp_h;
    BYTE temp_l;
    //BYTE Data;
    //BYTE Address;
    //volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif
    
    
    SPIRTCSRAM_CS_IO = 1;
    SPIRTCSRAM_CS_TRIS = 0;   // Drive SPI Flash chip select pin

    SPIRTCSRAM_SCK_TRIS = 0;  // Set SCK pin as an output
    SPIRTCSRAM_SDI_TRIS = 1;  // Make sure SDI pin is an input
    SPIRTCSRAM_SDO_TRIS = 0;  // Set SDO pin as an output

    // Save SPI state (clock speed)
    SPICON1Save = SPIRTCSRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = RTC_PROPER_SPICON1;
    SPI_ON_BIT = 1;

    ClearSPIDoneFlag();
    #if defined(__C30__)
        SPIRTCSRAM_SPICON2 = 0;
        SPIRTCSRAM_SPISTAT = 0;    // clear SPI
        SPIRTCSRAM_SPISTATbits.SPIEN = 1;
    #elif defined(__C32__)
        SPIRTCSRAM_SPIBRG = (GetPeripheralClock()-1ul)/2ul/SPIRTCSRAM_MAX_SPI_FREQ;
    #elif defined(__18CXX)
        SPIRTCSRAM_SPISTATbits.CKE = 1;       // Transmit data on rising edge of clock
        SPIRTCSRAM_SPISTATbits.SMP = 0;       // Input sampled at middle of data output time
    #endif

    SPIRTCSRAM_CS_IO = 0;
    ClearSPIDoneFlag();
                   
        SPIRTCReadTime();   
              
        Wait400ns();        
        _WaitWhileBusy();         //check busy flag till clear        
        SPIRTCSRAM_CS_IO = 0;       //do temperature conversion
        spi_write(RTC_CONTROL | WRITEMASK);
        spi_write(0x24);
        SPIRTCSRAM_CS_IO = 1;
        Wait400ns();
        SPIRTCSRAM_CS_IO = 0;       
        spi_write(RTC_STATUS | WRITEMASK);
        spi_write(0x00);
        SPIRTCSRAM_CS_IO = 1;
        Wait400ns();        
        SPIRTCSRAM_CS_IO = 0;                     //read temp
        spi_write(RTC_TEMP_MSB);
        temp_h = spi_read();
        temp_l = spi_read();
    
        SPIRTCSRAM_CS_IO = 1;        

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
    
    //interrupt every second
    SPIRTCSetAlarm1PerSec();
    TRISAbits.TRISA13 = 1;
    INTCON2bits.INT2EP = 1; //1 = Interrupt on negative edge; 0 = Interrupt on positive edge
    IPC7bits.INT2IP = 7; //111 = Interrupt is priority 7 (highest priority interrupt)
    IEC1bits.INT2IE = 1; //1 = Interrupt request enabled
    
    
}


/*****************************************************************************
  Function:
    void SPIRTCSRAMReadArray(DWORD dwAddress, BYTE *vData, WORD wLength)

  Description:
    Reads an array of bytes from the SPI Flash module.

  Precondition:
    SPIRTCSRAMInit has been called, and the chip is not busy (should be
    handled elsewhere automatically.)

  Parameters:
    dwAddress - Address from which to read
    vData - Where to store data that has been read
    wLength - Length of data to read

  Returns:
    None
  ***************************************************************************/
void SPISRAMReadArray(DWORD dwAddress, BYTE *vData, WORD wLength)
{
    
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif
    //BYTE i;

    // Ignore operations when the destination is NULL or nothing to read
    if(vData == NULL || wLength == 0)
        return;

    // Save SPI state (clock speed)
    SPICON1Save = SPIRTCSRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = RTC_PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // Activate chip select
    SPIRTCSRAM_CS_IO = 0;
    ClearSPIDoneFlag();
    
    
    // Send address
    Dummy = ((BYTE*)&dwAddress)[0];
    WriteAddressReg(Dummy);

    SPIRTCSRAM_CS_IO = 0;
    spi_write(SRAM_DATA);
    // Read data
    while(wLength--)
    {        
        Dummy = spi_read();
        *vData++ = Dummy;
    }

    // Deactivate chip select
    SPIRTCSRAM_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}

/*****************************************************************************
  Function:
    void SPIRTCSRAMBeginWrite(DWORD dwAddr)

  Summary:
    Prepares the SPI Flash module for writing.

  Description:
    Prepares the SPI Flash module for writing.  Subsequent calls to
    SPIRTCSRAMWrite or SPIRTCSRAMWriteArray will begin at this location and
    continue sequentially.

    SPI Flash

  Precondition:
    SPIRTCSRAMInit has been called.

  Parameters:
    dwAddr - Address where the writing will begin

  Returns:
    None

  Remarks:
    Flash parts have large sector sizes, and can only erase entire sectors
    at once.  The SST parts for which this library was written have sectors
    that are 4kB in size.  Your application must ensure that writes begin on
    a sector boundary so that the SPIRTCSRAMWrite functions will erase the
    sector before attempting to write.  Entire sectors need not be written
    at once, so applications can begin writing to the front of a sector,
    perform other tasks, then later call SPIRTCSRAMBeginWrite and point to an
    address in this sector that has not yet been programmed.  However, care
    must taken to ensure that writes are not attempted on addresses that are
    not in the erased state.  The chip will provide no indication that the
    write has failed, and will silently ignore the command.
  ***************************************************************************/
void SPISRAMBeginWrite(DWORD dwAddr)
{
    dwWriteAddr = dwAddr;
}

/*****************************************************************************
  Function:
    void SPIRTCSRAMWrite(BYTE vData)

  Summary:
    Writes a byte to the SPI Flash part.

  Description:
    This function writes a byte to the SPI Flash part.  If the current
    address pointer indicates the beginning of a 4kB sector, the entire
    sector will first be erased to allow writes to proceed.  If the current
    address pointer indicates elsewhere, it will be assumed that the sector
    has already been erased.  If this is not true, the chip will silently
    ignore the write command.

  Precondition:
    SPIRTCSRAMInit and SPIRTCSRAMBeginWrite have been called, and the current
    address is either the front of a 4kB sector or has already been erased.

  Parameters:
    vData - The byte to write to the next memory location.

  Returns:
    None

  Remarks:
    See Remarks in SPIRTCSRAMBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPISRAMWrite(BYTE vData)
{
    //BYTE i;
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif

    // Save SPI state (clock speed)
    SPICON1Save = SPIRTCSRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = RTC_PROPER_SPICON1;
    SPI_ON_BIT = 1;

    
    // Activate the chip select
    SPIRTCSRAM_CS_IO = 0;
    ClearSPIDoneFlag();

    
    // Send address
    Dummy = ((BYTE*)&dwWriteAddr)[0];
    WriteAddressReg(Dummy);       
    
    SPIRTCSRAM_CS_IO = 0;
    Dummy = SRAM_DATA | WRITEMASK;
    spi_write(Dummy);
    spi_write(vData);
    dwWriteAddr++;

    // Deactivate chip select and wait for write to complete
    SPIRTCSRAM_CS_IO = 1;
    
    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}

/*****************************************************************************
  Function:
    void SPIRTCSRAMWriteArray(BYTE* vData, WORD wLen)

  Summary:
    Writes an array of bytes to the SPI Flash part.

  Description:
    This function writes an array of bytes to the SPI Flash part.  When the
    address pointer crosses a sector boundary (and has more data to write),
    the next sector will automatically be erased.  If the current address
    pointer indicates an address that is not a sector boundary and is not
    already erased, the chip will silently ignore the write command until the
    next sector boundary is crossed.

  Precondition:
    SPIRTCSRAMInit and SPIRTCSRAMBeginWrite have been called, and the current
    address is either the front of a sector or has already been erased.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    See Remarks in SPIRTCSRAMBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPISRAMWriteArray(BYTE* vData, WORD wLen)
{
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif
    BOOL isStarted;    
    //BYTE i;

	// Do nothing if no data to process
	if(wLen == 0u)
		return;

    // Save SPI state (clock speed)
    SPICON1Save = SPIRTCSRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = RTC_PROPER_SPICON1;
    SPI_ON_BIT = 1;
 
    isStarted = FALSE;

    // Loop over all remaining WORDs
    while(wLen > 0)
    {
                      
        if(!isStarted)
        {
            // Activate the chip select
            SPIRTCSRAM_CS_IO = 0;
            ClearSPIDoneFlag();

            // Send address
            Dummy = ((BYTE*)&dwWriteAddr)[0];
            WriteAddressReg(Dummy);       
            
            SPIRTCSRAM_CS_IO = 0;
            Dummy = SRAM_DATA | WRITEMASK;
            spi_write(Dummy);  
            isStarted = TRUE;
        }
        Dummy = *vData++;
        spi_write(Dummy);
        dwWriteAddr++;
        wLen--;
    }   
    SPIRTCSRAM_CS_IO = 1; 
    
    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}




/*****************************************************************************
  Function:
    static void _GetStatus()

  Summary:
    Reads the status register of the part.

  Description:
    This function reads the status register of the part.  It was written
    for debugging purposes, and is not needed for normal operation.  Place
    a breakpoint at the last instruction and check the "status" variable to
    see the result.

  Precondition:
    SPIRTCSRAMInit has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
//static void _GetStatus()
//{
//	volatile BYTE Dummy;
//  static BYTE statuses[16];
//  static BYTE *status = statuses;
//
//  // Activate chip select
//  SPIRTCSRAM_CS_IO = 0;
//  ClearSPIDoneFlag();
//
//  // Send Read Status Register instruction
//  SPIRTCSRAM_SSPBUF = RDSR;
//  WaitForDataByte();
//  Dummy = SPIRTCSRAM_SSPBUF;
//
//  SPIRTCSRAM_SSPBUF = 0x00;
//  WaitForDataByte();
//  *status = SPIRTCSRAM_SSPBUF;
//  status++;
//
//  // Deactivate chip select
//  SPIRTCSRAM_CS_IO = 1;
//
//  if(status == &statuses[10])
//      statuses[15] = 0;
//}

void SPIRTCWriteTime(void)
{
    
}
void SPIRTCReadTime()
{     
	BYTE s;
    BYTE m;
    BYTE h;
    BYTE D;
    BYTE M;
    BYTE Y;
    BYTE i;
    DWORD DY = 0;
    DWORD DM = 0;   
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif     
   
    BYTE seconds = 0;

    // Save SPI state (clock speed)
    SPICON1Save = SPIRTCSRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = RTC_PROPER_SPICON1;
    SPI_ON_BIT = 1;
     
    // Activate chip select
    SPIRTCSRAM_CS_IO = 0;
    ClearSPIDoneFlag();
    while(Time.b0.Val != seconds){
        dwRTCLastUpdateTick = TickGet();
        spi_write(RTC_SECONDS);
        Time.b0.Val = spi_read();
        Time.b1.Val = spi_read();
        Time.b2.Val = spi_read();
        Time.b3.Val = spi_read();
        Time.b4.Val = spi_read();
        Time.b5.Val = spi_read();
        Time.b6.Val = spi_read();
        SPIRTCSRAM_CS_IO = 1; 
        Wait400ns();
        SPIRTCSRAM_CS_IO = 0;
        spi_write(RTC_SECONDS);
        seconds = spi_read();
        SPIRTCSRAM_CS_IO = 1;         
    }  
    
    Y = Time.b6.years.Year10   * 10 + Time.b6.years.Year;
    M = Time.b5.month.Month10  * 10 + Time.b5.month.Month;
    D = Time.b4.date.Date10    * 10 + Time.b4.date.Date;
    h = Time.b2.hours.Hour10   * 10 + Time.b2.hours.Hour;
    m = Time.b1.minutes.Min10  * 10 + Time.b1.minutes.Min;
    s = Time.b0.seconds.Sec10  * 10 + Time.b0.seconds.Sec;
    for(i = 1;i<M;i++){
        switch (i){        
            case 4:case 6: case 9: case 11: 
                DM += 30;
            break;
            case 2:
                DM += 28;
                if((Y % 4) == 0){
                    DM++;                    
                }
            break;
            default:
            DM += 31;        
        }
    }
    for(i=0;i<Y;i++){
        DY+=365;
		if((i % 4) == 0){
            DY++;                    
        }
    }
    dwRTCSeconds = (((DY + DM + D) * 24 + h) * 60 + m)*60 + s + SEC_1970_2000;   
    RTCGetUTCSeconds();
	
    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}
/*****************************************************************************
  Function:
	DWORD RTCGetUTCSeconds(void)

  Summary:
	Obtains the current time from the RTC module.

  Description:
	This function obtains the current time as reported by the SNTP module.  
	Use this value for absolute time stamping.  The value returned is (by
	default) the number of seconds since 01-Jan-1970 00:00:00.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	The number of seconds since the Epoch.  (Default 01-Jan-1970 00:00:00)
  	
  Remarks:
	Do not use this function for time difference measurements.  The Tick
	module is more appropriate for those requirements.
  ***************************************************************************/
DWORD RTCGetUTCSeconds(void)
{
    DWORD dwTickDelta;
	DWORD dwTick;
    
    // Update the dwRTCSeconds variable with the number of seconds 
	// that has elapsed
	dwTick = TickGet();
	dwTickDelta = dwTick - dwRTCLastUpdateTick;
	while(dwTickDelta > TICK_SECOND)
	{
		dwRTCSeconds++;
		dwTickDelta -= TICK_SECOND;
	}	
	// Save the tick and residual fractional seconds for the next call
	dwRTCLastUpdateTick = dwTick - dwTickDelta;
    return dwRTCSeconds;
}    

void RTCSetUTCSeconds(DWORD Seconds)
{
    BYTE Y = 70;
    DWORD DY;
    BYTE HY;
    if(Seconds>=SEC_1970_2000){
        Seconds -= SEC_1970_2000;
        Y = 0; //2000
    }
    HY = 4;
    while(Seconds >= DY){
        if(HY == 4){
            DY = SEC_IN_366_DAY; //366 дней
            HY = 0;
        } else {
            DY = SEC_IN_365_DAY; //365 дней
        }
        HY++;
        Y++;
        Seconds -= DY;        
    }
}
     
#if __C30_VERSION__ >= 300
void _ISR __attribute__((__no_auto_psv__)) _INT2Interrupt(void)
#else
void _ISR _INT2Interrupt(void)
#endif
{
	// Increment internal high tick counter
	dwRTCSeconds++;

	// Reset interrupt flag
	 IFS1bits.INT2IF =  0;
}


void SPIRTCSetAlarm1PerSec(void)
{
	volatile BYTE Dummy;
	BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif     
	// Save SPI state (clock speed)
    SPICON1Save = SPIRTCSRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = RTC_PROPER_SPICON1;
    SPI_ON_BIT = 1;
     
    // Activate chip select
    SPIRTCSRAM_CS_IO = 0;
    ClearSPIDoneFlag();
        
    Dummy = RTC_SECONDS_ALARM1 | WRITEMASK;    
    spi_write(Dummy);
    spi_write(0x80);
    spi_write(0x80);
    spi_write(0x80);
    spi_write(0x80);
	SPIRTCSRAM_CS_IO = 1;
    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRTCSRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}
#endif //#if defined(SPIRTCSRAM_CS_TRIS)

