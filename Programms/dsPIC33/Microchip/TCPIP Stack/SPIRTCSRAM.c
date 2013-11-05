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
 *                  Microchip C30 v3.23 or higher
 *                  Microchip C18 v3.30 or higher
 *                  HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         
 *
 *
 * Author                       Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * D.Mosh                      02/08/11 Original 
 ********************************************************************/
#define __SPIRTCSRAM_C

#include "HardwareProfile.h"

#if defined(SPIRTCSRAM_CS_TRIS)

#include "TCPIPConfig.h"
#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/SPIRTCSRAM.h"
#include "device_control.h"


#define WRITEMASK           0x80
#define RTC_SECONDS         0x00
#define RTC_MINUTES         0x01
#define RTC_HOUR            0x02
#define RTC_DAY             0x03    //ƒень недели
#define RTC_DATE            0x04    //день мес€ца
#define RTC_MONTH           0x05    //мес€ц + ¬ек
#define RTC_YEAR            0x06
#define RTC_SECONDS_ALARM1  0x07
#define RTC_MINUTES_ALARM1  0x08
#define RTC_HOUR_ALARM1     0x09
#define RTC_DAY_ALARM1      0x0A    //ƒень недели
#define RTC_MINUTES_ALARM2  0x0B
#define RTC_HOUR_ALARM2     0x0C
#define RTC_DAY_ALARM2      0x0D        //ƒень недели
#define RTC_CONTROL         0x0E
#define RTC_STATUS          0x0F
#define RTC_CRYSTAL_AGGING  0x10
#define RTC_TEMP_MSB        0x11
#define RTC_TEMP_LSB        0x12
#define RTC_TEMP_CONTROL    0x13
#define SRAM_ADDRES         0x18
#define SRAM_DATA           0x19

#define SEC_IN_365_DAY 31536000
#define SEC_IN_366_DAY 31622400
#define SEC_1970_2000 978307200
#define SEC_IN_4_YEAR 126230400
#define SEC_IN_31DAY    2678400
#define SEC_IN_30DAY    2592000
#define SEC_IN_29DAY    2505600
#define SEC_IN_28DAY    2419200
#define SEC_IN_WEEK      604800
#define SEC_IN_DAY        86400
#define SEC_IN_HOUR        3600
  

// Internal pointer to address being written
//static DWORD dwWriteAddr;

// Seconds value obtained by last update
static DWORD dwRTCSeconds = 0;

// Tick count of last update
//static DWORD dwRTCLastUpdateTick = 0;
// Time Struct

static DWORD EPOCH2000 = (86400ul * (365ul * 30ul + 7ul));

static BYTE bIsRTCTimeValid = 0;
//static int SPIRTCInitialized = 0;
static DEVICE_REG * RTCDeviceHandle;
static void Wait400ns()
{
    BYTE i;
    for(i=0;i<8;i++){Nop();}
}
int RTCSelect()
{
    SPIRTCSRAM_CS_IO = 0;
    LED1_IO = 1;
     Wait400ns();
    return 0;
}
int RTCRelease()
{    
    SPIRTCSRAM_CS_IO = 1;
    LED1_IO = 0;
    Wait400ns();
    return 0;
}
void SPI_RTCReadRegister(BYTE bAddress, BYTE *vData);
void SPI_RTCReadAll(BYTE * Value, BYTE Len);

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
static int SPIRTCSRAMInitialized = 0;
void SPIRTCSRAMInit(void)
{
    SPIConfig Config;  
    BYTE vData = 0xFF;
    if(SPIRTCSRAMInitialized) return;
    SPIRTCSRAMInitialized = 1;

    SPIRTCSRAM_CS_TRIS = 0;
    SPIRTCSRAM_CS_IO = 1;
    // частота 2ћ√ц 0x0016 | 0x0020
    Config = SPI_CreateParams(MASTER, ACTIVE_HIGH, IDLE_TO_ACTIVE, PPRE_4_1, SPRE_3_1, IDLE_ENABLE, SPI_SIZE_BYTE, MIDDLE_PHASE);
    RTCDeviceHandle = SPIRegisterDevice(SPIRTCSRAM_SPI_ID, Config, RTCSelect, RTCRelease);

    while(vData & 0x80){
        SPI_RTCReadRegister(RTC_STATUS, &vData);
        SPI_RTCWriteRegister(RTC_STATUS, 0x38);
    }
    
    SPI_RTCWriteRegister(RTC_CONTROL, 0x18);
    Wait400ns();

    //SPI_RTCReadTime(&Time);
    GetTimeFromRTC();
    
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
void SPI_RTC_SRAMReadArray(BYTE bAddress, BYTE *vData, WORD wLength, WAIT_READY WaitData)
{
    BYTE Cmd[2];
    // Ignore operations when the destination is NULL or nothing to read
    if(vData == NULL || wLength == 0)
        return;

    Cmd[0] = SRAM_ADDRES | WRITEMASK;
    Cmd[1] = bAddress;
    SPISendCmd(RTCDeviceHandle, Cmd, 2);

    Cmd[0] = SRAM_DATA;
    SPIReceiveData(RTCDeviceHandle, Cmd, 1, vData, wLength, WaitData);
}
/*****************************************************************************
  Function:
    void SPIReadRegister(BYTE bAddress, BYTE *vData)

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
void SPI_RTCReadRegister(BYTE bAddress, BYTE *vData)
{
    BYTE Cmd;
    // Ignore operations when the destination is NULL or nothing to read
    if(vData == NULL)
        return;

    Cmd = bAddress;
    SPIReceiveData(RTCDeviceHandle, &Cmd, 1, vData, 1, 1);
}


void SPI_RTCWriteRegister(BYTE bAddress, BYTE vData)
{
    BYTE Cmd;
    Cmd = bAddress | WRITEMASK;
    SPISendData(RTCDeviceHandle, &Cmd, 1, &vData, 1);
}


/*****************************************************************************
  Function:
    void SPIRTCSRAMWriteArray(BYTE* vData, WORD wLen)

  Summary:
    Writes an array of bytes to the SPI Flash part.

  Description:
    This function writes an array of bytes to the SPI SRAM part. 

  Precondition:
    SPIRTCSRAMInit and SPIRTCSRAMBeginWrite have been called.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    See Remarks in SPIRTCSRAMBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPI_RTC_SRAMWriteArray(BYTE bAddress, BYTE *vData, WORD wLength)
{
    BYTE Cmd[2];
    // Ignore operations when the destination is NULL or nothing to read
    if(vData == NULL || wLength == 0)
        return;

    Cmd[0] = SRAM_ADDRES | WRITEMASK;
    Cmd[1] = bAddress;
    SPISendCmd(RTCDeviceHandle, Cmd, 2);

    Cmd[0] = SRAM_DATA | WRITEMASK;
    SPISendData(RTCDeviceHandle, Cmd, 1, vData, wLength);
}

/*****************************************************************************
  Function:
    void SPIRTCWriteTime(void)

  Summary:
    Writes Time to the SPI RTC part.

  Description:
    This function writes Time to the SPI SRAM part. 

  Precondition:
    SPIRTCSRAMInit have been called.

  Parameters:
    None
    
  Returns:
    None

  Remarks:
    
  ***************************************************************************/

void SPI_RTCWriteTime(RTC_TIME Time)
{
    BYTE Cmd;

    Cmd = RTC_SECONDS | WRITEMASK;
    SPISendData(RTCDeviceHandle, &Cmd, 1, (BYTE*)&Time, sizeof(RTC_TIME));
}
/*****************************************************************************
  Function:
    void SPIRTCReadTime(void)

  Summary:
    Read Time to the SPI RTC part.

  Description:
    Read Time to the SPI RTC part.

  Precondition:
    SPIRTCSRAMInit have been called.

  Parameters:
    
  Returns:
    None

  Remarks:
   
  ***************************************************************************/
void SPI_RTCReadTime(RTC_TIME * Time)
{
    BYTE Cmd;
    if(Time == NULL) return;

    Cmd = RTC_SECONDS;
    SPIReceiveData(RTCDeviceHandle, &Cmd, 1, (BYTE*)Time, sizeof(RTC_TIME), 1);
}

void SPI_RTCReadAll(BYTE * Value, BYTE Len)
{
    BYTE Cmd;  

    Cmd = RTC_SECONDS;
    SPIReceiveData(RTCDeviceHandle, &Cmd, 1, Value, Len, 1);
}

/*****************************************************************************
  Function:
    DWORD GetTimeFromRTC(void)

  Summary:
    Return RTC local Time in seconds. 

  Description:    

  Precondition:
    SPIRTCSRAMInit have been called.

  Parameters:
  
  Returns:
    The number of seconds since the Epoch.  (Default 01-Jan-1970 00:00:00)

  Remarks:   
  ***************************************************************************/
DWORD GetTimeFromRTC()
{ 
    BYTE Y;
    BYTE M;
    BYTE D;
    BYTE h;
    BYTE m;
    BYTE s;
    BYTE i;
    DWORD DY = 0;
    DWORD DM = 0;   
    RTC_TIME Time;

    SPI_RTCReadTime(&Time);
    Y = Time.Month.bits.Century * 100 + Time.Years.bits.Year10 * 10 + Time.Years.bits.Year;
    M = Time.Month.bits.Month10 * 10 + Time.Month.bits.Month;
    D = Time.Date.bits.Date10    * 10 + Time.Date.bits.Date;
    h = Time.Hours.bits.Hour10   * 10 + Time.Hours.bits.Hour;
    m = Time.Minutes.bits.Min10  * 10 + Time.Minutes.bits.Min;
    s = Time.Seconds.bits.Sec10  * 10 + Time.Seconds.bits.Sec;
    
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
    for(i=0;i < Y;i++){
        if((i % 4) == 0){
            DY+=366;
        } else {
            DY+=365;
        }
    }   
    DY = DY + DM + D - 1;
    dwRTCSeconds = ((DY * 24 + h) * 60 + m)*60 + s + EPOCH2000;
    //dwRTCLastUpdateTick = TickGet();*/
    return dwRTCSeconds; //dwRTCSeconds;
}
/*****************************************************************************
  Function:
    void SetTimeFromUTC(DWORD Seconds)

  Summary:
    
  Description:
    
  Precondition:
    SPIRTCSRAMInit have been called.

  Parameters:
    
  Returns:
    None

  Remarks:
 ***************************************************************************/
void SetRTCTimeFromUTC(DWORD Seconds)
{ 
    //01-Jan-2000 00:00:00
    
    DWORD Y = 0;
    BYTE M = 1;
    BYTE D = 1;
    BYTE d = 1;//день недели
    BYTE h = 0;
    BYTE m = 0;
    BYTE s = 0;
    DWORD Days; 
    DWORD Secs = Seconds - EPOCH2000;
    RTC_TIME Time;

    // узнаем количество дней, прошедших с 2000 года
    Days = Secs/86400;
    Secs = Secs % 86400;
    Y = Days / 365ul;
    Days = Days % 365ul - (Y-1) / 4;
    //if(Days> 60) Days -= 1; 
    while((M<=12)&&(Days>31)){
        switch(M){
        case 1:case 3: case 5:case 7:
        case 8:case 10: case 12:
            if(Days > 31){
                Days -= 31;
                M++;
            }
            break;
        case 2:
            if(Y % 4 == 0){
                if(Days > 29){
                    Days -= 29;
                    M++;
                }
            }else{
                if(Days > 28){
                    Days -= 28;
                    M++;
                }
            }
            break;
        default:
            if(Days > 30){
                Days -= 30;
                M++;
            }
            break;
        }
    }    

    h = (BYTE)(Secs / 3600);
    Secs = Secs % 3600;
    m = (BYTE)(Secs / 60);
    s = Secs % 60;
   
   
    Time.Seconds.bits.Sec10 = s / 10;
    Time.Seconds.bits.Sec =   s % 10;
    Time.Minutes.bits.Min10 = m / 10;
    Time.Minutes.bits.Min =   m % 10;
    Time.Hours.bits.Hour10  = h / 10;
    Time.Hours.bits.Hour  =   h % 10;
    Time.Day.bits.Day       = d;
    Time.Date.bits.Date10   = D / 10;
    Time.Date.bits.Date   =   D % 10;
    Time.Month.bits.Month10 = M / 10;
    Time.Month.bits.Month =   M % 10;
    Time.Month.bits.Century = Y / 100;    
    Time.Years.bits.Year10 =  (Y % 100) / 10;
    Time.Years.bits.Year =    (Y % 100) % 10; 
    SPI_RTCWriteTime(Time);
}
     



BYTE RTCIsTimeValid(void){
    return bIsRTCTimeValid;
}
//03.08.2011 14:49:36, sunday 
void RTCGetFormatTime(BYTE* time){    
    /*
     //static BYTE time[30];
     BYTE i = 0;
     SPIRTCReadTime();
     
     time[i++] = Time.b4.date.Date10 + '0';
     time[i++] = Time.b4.date.Date   + '0';
     time[i++] = '.';     
     time[i++] = Time.b5.month.Month10 + '0';
     time[i++] = Time.b5.month.Month + '0';
     time[i++] = '.';     
     if(Time.b5.month.Century == 1){
         time[i++] = '1';
         time[i++] = '9';         
     } else {
         time[i++] = '2';
         time[i++] = '0';         
     }
     time[i++] = Time.b6.years.Year10 + '0';
     time[i++] = Time.b6.years.Year   + '0';
     time[i++] = ' ';     
     time[i++] = Time.b2.hours.Hour10 + '0';
     time[i++] = Time.b2.hours.Hour   + '0';
     time[i++] = ':';     
     time[i++] = Time.b1.minutes.Min10 + '0';
     time[i++] = Time.b1.minutes.Min   + '0';
     time[i++] = ':';     
     time[i++] = Time.b0.seconds.Sec10 + '0';
     time[i++] = Time.b0.seconds.Sec   + '0';
     time[i++] = ',';     
     time[i++] = ' ';
     switch(Time.b3.day.Day){
         case 1:
            memcpy((void*)&time[i], "Sunday", 6);
            i+=6;
         break;
         case 2:
            memcpy((void*)&time[i], "Monday", 6);
            i+=6;
         break;
         case 3:
            memcpy((void*)&time[i], "Tuesday", 7);
            i+=7;
         break;
         case 4:
            memcpy(&(time[i]),      "Wednesday", 9);
            i+=9;
         break;
         case 5:
            memcpy((void*)&time[i], "Thursday", 8);
            i+=8;
         break;
         case 6:
            memcpy((void*)&time[i], "Friday", 6);
            i+=6;
         break;
         case 7:
            memcpy((void*)&time[i], "Saturday", 8);
            i+=8;
         break;
         default:
            time[i] = 0;
     }
     time[i++] = 0;     
     */
}
#endif //#if defined(SPIRTCSRAM_CS_TRIS)

