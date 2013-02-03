/*********************************************************************
 *
 *  SPI Flash Memory Driver
 *  - Tested with SST 25VF016B 
 *  - Expected compatibility with other SST (Microchip) SST25 series 
 *    devices
 *
 *********************************************************************
 * FileName:        SPIFlash.c
 * Dependencies:    SPIFlash.h
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
#define __SPIFLASH_C

#include "HardwareProfile.h"
#include "device_control.h"
#if defined(SPIFLASH_CS_TRIS)

#include "TCPIP Stack/TCPIP.h"

#define READ				0x03    // SPI Flash opcode: Read up up to 25MHz
#define READ_FAST			0x0B    // SPI Flash opcode: Read up to 50MHz with 1 dummy byte
#define ERASE_4K			0x20    // SPI Flash opcode: 4KByte sector erase
#define ERASE_32K			0x52    // SPI Flash opcode: 32KByte block erase
#define ERASE_SECTOR		0xD8    // SPI Flash opcode: 64KByte block erase
#define ERASE_ALL			0x60    // SPI Flash opcode: Entire chip erase
#define WRITE				0x02    // SPI Flash opcode: Write one byte (or a page of up to 256 bytes, depending on device)
#define WRITE_WORD_STREAM	0xAD    // SPI Flash opcode: Write continuous stream of 16-bit words (AAI mode); available on SST25VF016B (but not on SST25VF010A)
#define WRITE_BYTE_STREAM	0xAF    // SPI Flash opcode: Write continuous stream of bytes (AAI mode); available on SST25VF010A (but not on SST25VF016B)
#define RDSR				0x05    // SPI Flash opcode: Read Status Register
#define EWSR				0x50    // SPI Flash opcode: Enable Write Status Register
#define WRSR				0x01    // SPI Flash opcode: Write Status Register
#define WREN				0x06    // SPI Flash opcode: Write Enable
#define WRDI				0x04    // SPI Flash opcode: Write Disable / End AAI mode
#define RDID				0x90    // SPI Flash opcode: Read ID
#define JEDEC_ID			0x9F    // SPI Flash opcode: Read JEDEC ID
#define EBSY				0x70    // SPI Flash opcode: Enable write BUSY status on SO pin
#define DBSY				0x80    // SPI Flash opcode: Disable write BUSY status on SO pin

#define BUSY    0x01    // Mask for Status Register BUSY (Internal Write Operaiton in Progress status) bit
#define WEL     0x02    // Mask for Status Register WEL (Write Enable status) bit
#define BP0     0x04    // Mask for Status Register BP0 (Block Protect 0) bit
#define BP1     0x08    // Mask for Status Register BP1 (Block Protect 1) bit
#define BP2     0x10    // Mask for Status Register BP2 (Block Protect 2) bit
#define BP3     0x20    // Mask for Status Register BP3 (Block Protect 3) bit
#define AAI     0x40    // Mask for Status Register AAI (Auto Address Increment Programming status) bit
#define BPL     0x80    // Mask for Status Register BPL (BPx block protect bit read-only protect) bit

#if defined(__PIC24F__) || defined(__PIC24FK__)
    #define FLASH_PROPER_SPICON1  (0x001B | 0x0120)   // 1:1 primary prescale, 2:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
    #define FLASH_PROPER_SPICON1  (0x000F | 0x0120)   // 1:1 primary prescale, 5:1 secondary prescale, CKE=1, MASTER mode
    #define FLASH_16BIT_PROPER_SPICON1  (0x000F | 0x0520)   //16bit, 1:1 primary prescale, 5:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__dsPIC30F__)
    #define FLASH_PROPER_SPICON1  (0x0017 | 0x0120)   // 1:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__PIC32MX__)
    #define FLASH_PROPER_SPICON1  (_SPI2CON_ON_MASK | _SPI2CON_FRZ_MASK | _SPI2CON_CKE_MASK | _SPI2CON_MSTEN_MASK)
#else
    #define FLASH_PROPER_SPICON1  (0x20)              // SSPEN bit is set, SPI in master mode, FOSC/4, IDLE state is low level
#endif
#define SPI_ON_BIT SPIFLASH_SPISTATbits.SPIEN

// Maximum speed of SPI Flash part in Hz
// Should theoretically operate at 25MHz, but need to account for level-shifting delays
#define SPIFLASH_MAX_SPI_FREQ       (10000000ul)
#define WaitForDataByte()
#define ClearSPIDoneFlag()

// Internal pointer to address being written
static DWORD dwWriteAddr;
static BYTE FlashDeviceHandle;

// SPI Flash device capabilities
static union
{
	unsigned char v;
	struct
	{
		unsigned char bWriteWordStream : 1;	// Supports AAI Word opcode (0xAD)
		unsigned char bWriteByteStream : 1;	// Supports AAI Byte opcode (0xAF)
		unsigned char bPageProgram : 1;		// Supports Byte program opcode with up to 256 bytes/page (0x02)
		unsigned char filler : 5;
	} bits;
} deviceCaps;


static void _SendCmd(BYTE cmd);
static void _WaitWhileBusy(void);
void SPIFlashPageWrite(BYTE* vData, WORD wLen);
//static void _GetStatus(void);

int FlashSelect()
{
    SPIFLASH_CS_IO = 0;
    LED0_IO = 1;
    return 0;
}
int FlashRelease()
{    
    SPIFLASH_CS_IO = 1;
    LED0_IO = 0;
    return 0;
}
/*****************************************************************************
  Function:
    void SPIFlashInit(void)

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
static int SPIFlashInitialized = 0;  
void SPIFlashInit(void)
{
	BYTE Cmd = JEDEC_ID;
	BYTE Data[3];
    if(SPIFlashInitialized) return;
    SPIFlashInitialized = 1;
    SPIFLASH_CS_TRIS = 0;   // Drive SPI Flash chip select pin

    // Configure SPI
    SPIConfig Config;
    Config.SPICON1 = 0x17 | 0x100 | 0x20; // CKE = 1 MSTEB = 1;
    Config.SPICON2 = 0;
    Config.SPISTAT = 0;
    FlashDeviceHandle = SPIRegisterDevice(ID_SPI2, Config, FlashSelect, FlashRelease);

    // Read Device ID code to determine supported device capabilities/instructions
    {
            
        SPIReceiveData( FlashDeviceHandle, &Cmd, 1, Data, 3 );

        // Decode Device Capabilities Flags from Device ID
        deviceCaps.v = 0x00;
        switch(Data[2])
        {
            case 0x43:	// SST25LF020(A)	(2 Mbit)	0xAF, 14us, AAI Byte
            case 0x48:	// SST25VF512(A)	(512 Kbit)	0xAF, 14us, AAI Byte
            case 0x49:	// SST25VF010A		(1 Mbit)	0xAF, 14us, AAI Byte
                deviceCaps.bits.bWriteByteStream = 1;
                break;
            case 0x18:	// M25P128              (128 Mbit)
            case 0x4B:	// SST25VF064C		(64 Mbit)	0x02, 1.5ms/256 byte page, no AAI			
                deviceCaps.bits.bPageProgram = 1;
                break;

            //case 0x01:	// SST25WF512		(512 Kbit)	0xAD, 50us, AAI Word
            //case 0x02:	// SST25WF010		(1 Mbit)	0xAD, 50us, AAI Word
            //case 0x03:	// SST25WF020		(2 Mbit)	0xAD, 50us, AAI Word
            //case 0x04:	// SST25WF040		(4 Mbit)	0xAD, 50us, AAI Word
            //case 0x05:	// SST25WF080		(8 Mbit)	0xAD, 14us, AAI Word
            //case 0x41:	// SST25VF016B		(16 Mbit)	0xAD,  7us, AAI Word
            //case 0x4A:	// SST25VF032B		(32 Mbit)	0xAD,  7us, AAI Word
            //case 0x8C:	// SST25VF020B		(2 Mbit)	0xAD,  7us, AAI Word
            //case 0x8D:	// SST25VF040B		(4 Mbit)	0xAD,  7us, AAI Word
            //case 0x8E:	// SST25VF080B		(8 Mbit)	0xAD,  7us, AAI Word				
            // Assume AAI Word programming is supported for the above commented 
            // devices and unknown devices.
            default:
                deviceCaps.bits.bWriteWordStream = 1;
        }
    }

    // Clear any pre-existing AAI write mode
    // This may occur if the PIC is reset during a write, but the Flash is
    // not tied to the same hardware reset.
    Cmd = WRDI;
    SPISendCmd( FlashDeviceHandle, &Cmd, 1);

    // Execute Enable-Write-Status-Register (EWSR) instruction
    Cmd = EWSR;
    SPISendCmd( FlashDeviceHandle, &Cmd, 1);

    // Clear Write-Protect on all memory locations
    Cmd = WRSR;
    Data[0] = 0;
    SPISendData( FlashDeviceHandle, &Cmd, 1, Data, 1 );
}


/*****************************************************************************
  Function:
    void SPIFlashReadArray(DWORD dwAddress, BYTE *vData, WORD wLength)

  Description:
    Reads an array of bytes from the SPI Flash module.

  Precondition:
    SPIFlashInit has been called, and the chip is not busy (should be
    handled elsewhere automatically.)

  Parameters:
    dwAddress - Address from which to read
    vData - Where to store data that has been read
    wLength - Length of data to read

  Returns:
    None
  ***************************************************************************/
void SPIFlashReadArray(DWORD dwAddress, BYTE *vData, WORD wLength)
{
    BYTE Cmd[4];
    // Ignore operations when the destination is NULL or nothing to read
    if(vData == NULL || wLength == 0)
        return;

    Cmd[0] = READ;
    Cmd[1] = ((BYTE*)&dwAddress)[2];
    Cmd[2] = ((BYTE*)&dwAddress)[1];
    Cmd[3] = ((BYTE*)&dwAddress)[0];
    SPIReceiveData(FlashDeviceHandle, Cmd, 4, vData, wLength);
}

/*****************************************************************************
  Function:
    void SPIFlashBeginWrite(DWORD dwAddr)

  Summary:
    Prepares the SPI Flash module for writing.

  Description:
    Prepares the SPI Flash module for writing.  Subsequent calls to
    SPIFlashWrite or SPIFlashWriteArray will begin at this location and
    continue sequentially.

    SPI Flash

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    dwAddr - Address where the writing will begin

  Returns:
    None

  Remarks:
    Flash parts have large sector sizes, and can only erase entire sectors
    at once.  The SST parts for which this library was written have sectors
    that are 4kB in size.  Your application must ensure that writes begin on
    a sector boundary so that the SPIFlashWrite functions will erase the
    sector before attempting to write.  Entire sectors need not be written
    at once, so applications can begin writing to the front of a sector,
    perform other tasks, then later call SPIFlashBeginWrite and point to an
    address in this sector that has not yet been programmed.  However, care
    must taken to ensure that writes are not attempted on addresses that are
    not in the erased state.  The chip will provide no indication that the
    write has failed, and will silently ignore the command.
  ***************************************************************************/
void SPIFlashBeginWrite(DWORD dwAddr)
{
    dwWriteAddr = dwAddr;
}

/*****************************************************************************
  Function:
    void SPIFlashWrite(BYTE vData)

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
    SPIFlashInit and SPIFlashBeginWrite have been called, and the current
    address is either the front of a 4kB sector or has already been erased.

  Parameters:
    vData - The byte to write to the next memory location.

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPIFlashWrite(BYTE vData)
{
    BYTE Cmd[4];

    // If address is a boundary, erase a sector first
    if((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0u)
        SPIFlashEraseSector(dwWriteAddr);

    _WaitWhileBusy();

    Cmd[0] = WREN;
    SPISendCmd( FlashDeviceHandle, Cmd, 1);

    Cmd[0] = WRITE;
    Cmd[1] = ((BYTE*)&dwWriteAddr)[2];
    Cmd[2] = ((BYTE*)&dwWriteAddr)[1];
    Cmd[3] = ((BYTE*)&dwWriteAddr)[0];
    SPISendData(FlashDeviceHandle, Cmd, 4, &vData, 1);

}
/*****************************************************************************
  Function:
    void SPIFlashWriteArray(BYTE* vData, WORD wLen)

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
    SPIFlashInit and SPIFlashBeginWrite have been called, and the current
    address is either the front of a sector or has already been erased.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/


void SPIFlashWriteArray(BYTE* vData, WORD wLen)
{
    DWORD FreeInCurrentPage = 0;
    WORD CurrentLen = 0;
    WORD CurrentData = 0;
    WORD DataLen = wLen;
    BYTE Cmd[4];
    
    while(DataLen > 0){        
        if((dwWriteAddr & SPI_FLASH_SECTOR_MASK)==0){ // стереть сектор  TODO: стирать весь сектор автоматически плохо
            SPIFlashEraseSector(dwWriteAddr);
        }
        _WaitWhileBusy();

        Cmd[0] = WREN;
        SPISendCmd( FlashDeviceHandle,Cmd, 1);

        if (deviceCaps.bits.bPageProgram)
        {
            Cmd[0] = WRITE;
            FreeInCurrentPage = SPI_FLASH_PAGE_SIZE - (dwWriteAddr & SPI_FLASH_PAGE_MASK);
            if(DataLen <= FreeInCurrentPage){
                CurrentLen = DataLen;
            } else {
                CurrentLen = FreeInCurrentPage;
            }
        } else if(deviceCaps.bits.bWriteByteStream){
            CurrentLen = wLen;
            Cmd[0] = WRITE_BYTE_STREAM;
        } else {
            CurrentLen = wLen;
            Cmd[0] = WRITE;
        }         
        
        Cmd[1] = ((BYTE*)&dwWriteAddr)[2];
        Cmd[2] = ((BYTE*)&dwWriteAddr)[1];
        Cmd[3] = ((BYTE*)&dwWriteAddr)[0];
        SPISendData(FlashDeviceHandle, Cmd, 4, &vData[CurrentData], CurrentLen);
        DataLen -= CurrentLen;
        CurrentData += CurrentLen;
        dwWriteAddr += CurrentLen;
        _WaitWhileBusy();
    }
}



/*****************************************************************************
  Function:
    void SPIFlashEraseSector(DWORD dwAddr)

  Summary:
    Erases a sector.

  Description:
    This function erases a sector in the Flash part.  It is called
    internally by the SPIFlashWrite functions whenever a write is attempted
    on the first byte in a sector.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    dwAddr - The address of the sector to be erased.

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPIFlashEraseSector(DWORD dwAddr)
{
    BYTE Cmd[4];

    _WaitWhileBusy();

    Cmd[0] = WREN;
    SPISendCmd( FlashDeviceHandle, Cmd, 1);

    Cmd[0] = ERASE_SECTOR;
    Cmd[1] = ((BYTE*)&dwAddr)[2];
    Cmd[2] = ((BYTE*)&dwAddr)[1];
    Cmd[3] = ((BYTE*)&dwAddr)[0];
    SPISendCmd(FlashDeviceHandle, Cmd, 4);

}


/*****************************************************************************
  Function:
    static void _SendCmd(BYTE cmd)

  Summary:
    Sends a single-byte command to the SPI Flash part.

  Description:
    This function sends a single-byte command to the SPI Flash part.  It is
    used for commands such as WREN, WRDI, and EWSR that must have the chip
    select activated, then deactivated immediately after the command is
    transmitted.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    cmd - The single-byte command code to send

  Returns:
    None
  ***************************************************************************/
static void _SendCmd(BYTE cmd)
{    
    SPISendCmd( FlashDeviceHandle, &cmd, 1);
}


/*****************************************************************************
  Function:
    static void _WaitWhileBusy(void)

  Summary:
    Waits for the SPI Flash part to indicate it is idle.

  Description:
    This function waits for the SPI Flash part to indicate it is idle.  It is
    used in the programming functions to wait for operations to complete.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
static void _WaitWhileBusy(void)
{    
    BYTE Cmd = RDSR;
    BYTE Data;
    // Poll the BUSY bit
    do
    {
        SPIReceiveData( FlashDeviceHandle, &Cmd, 1, &Data, 1);
    } while(Data & BUSY);
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
    SPIFlashInit has been called.

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
//  SPIFLASH_CS_IO = 0;
//  ClearSPIDoneFlag();
//
//  // Send Read Status Register instruction
//  SPIFLASH_SSPBUF = RDSR;
//  WaitForDataByte();
//  Dummy = SPIFLASH_SSPBUF;
//
//  SPIFLASH_SSPBUF = 0x00;
//  WaitForDataByte();
//  *status = SPIFLASH_SSPBUF;
//  status++;
//
//  // Deactivate chip select
//  SPIFLASH_CS_IO = 1;
//
//  if(status == &statuses[10])
//      statuses[15] = 0;
//}

#endif //#if defined(SPIFLASH_CS_TRIS)

