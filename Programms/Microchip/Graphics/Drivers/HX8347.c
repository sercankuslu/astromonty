/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  HIMAX HX8347 controller driver
 *****************************************************************************
 * FileName:        HX8347.c
 * Dependencies:    Graphics.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright � 2009 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS� WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok     05/26/09
 *****************************************************************************/
#include "Graphics/Graphics.h"

// Color
WORD    _color;

// Clipping region control
SHORT   _clipRgn;

// Clipping region borders
SHORT   _clipLeft;
SHORT   _clipTop;
SHORT   _clipRight;
SHORT   _clipBottom;

/////////////////////// LOCAL FUNCTIONS PROTOTYPES ////////////////////////////
void    SetReg(BYTE index, BYTE value);
void    PutImage1BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch);
void    PutImage4BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch);
void    PutImage8BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch);
void    PutImage16BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch);

void    PutImage1BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch);
void    PutImage4BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch);
void    PutImage8BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch);
void    PutImage16BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch);



/*********************************************************************
* Macros:  WritePixel(data)
*
* Overview: Writes data
*
* PreCondition: none
*
* Input: data
*
* Output: none
*
* Side Effects: none
*
********************************************************************/

#ifdef USE_16BIT_PMP
#define WritePixel(data)  DeviceWrite(data)
#else
#define WritePixel(data) \
    DeviceWrite( ((WORD_VAL) data).v[1] & 0xF8 );        \
    DeviceWrite( ((data >> 3) & 0xFC) );                 \
    DeviceWrite( (((WORD_VAL) data).v[0] << 3) & 0xF8 );
#endif

/*********************************************************************
* Function:  void SetAddress(SHORT x, SHORT y)
*
* Overview: Writes address pointer.
*
* PreCondition: none
*
* Input: x - horizontal position
*        y - vertical position
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
inline void SetAddress(SHORT x, SHORT y)
{
	DisplaySetCommand();
    DeviceWrite(0x02);
	DisplaySetData();
    DeviceWrite(((WORD_VAL) (WORD) x).v[1]);
	DisplaySetCommand();
    DeviceWrite(0x03);
	DisplaySetData();
    DeviceWrite(((WORD_VAL) (WORD) x).v[0]);
	DisplaySetCommand();
    DeviceWrite(0x06);
	DisplaySetData();
    DeviceWrite(((WORD_VAL) (WORD) y).v[1]);
	DisplaySetCommand();
    DeviceWrite(0x07);
	DisplaySetData();
    DeviceWrite(((WORD_VAL) (WORD) y).v[0]);
	DisplaySetCommand();
    DeviceWrite(0x22);
}

/*********************************************************************
* Function:  void  SetReg(BYTE index, BYTE value)
*
* PreCondition: none
*
* Input: index - register number
*        value - value to be set
*
* Output: none
*
* Side Effects: none
*
* Overview: sets graphics controller register
*
* Note: none
*
********************************************************************/
void SetReg(BYTE index, BYTE value)
{
	DisplayEnable();
	DisplaySetCommand();
	DeviceWrite(index);
	DisplaySetData();
	DeviceWrite(value);
	DisplayDisable();
}

/*********************************************************************
* Function:  void ResetDevice()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: resets LCD, initializes PMP
*
* Note: none
*
********************************************************************/
void ResetDevice(void)
{
	// Initialize the device
	DeviceInit();

#if (DISPLAY_CONTROLLER == HX8347A)	
    // Gamma for CMO 2.8
    SetReg(0x46, 0x95);     //
    SetReg(0x47, 0x51);     //
    SetReg(0x48, 0x00);     //
    SetReg(0x49, 0x36);     //
    SetReg(0x4A, 0x11);     //
    SetReg(0x4B, 0x66);     //
    SetReg(0x4C, 0x14);     //
    SetReg(0x4D, 0x77);     //
    SetReg(0x4E, 0x13);     //
    SetReg(0x4F, 0x4C);     //
    SetReg(0x50, 0x46);     //
    SetReg(0x51, 0x46);     //

    // Display
    SetReg(0x90, 0x7F);     // SAP=0111 1111
    SetReg(0x01, 0x06);     // IDMON=0, INVON=1, NORON=1, PTLON=0
    #if (DISP_ORIENTATION == 0)
    SetReg(0x16, 0xC8);     // MY=1, MX=1, MV=0, BGR=1
    #else
    SetReg(0x16, 0xA8);     // MY=1, MX=0, MV=1, BGR=1
    #endif
    SetReg(0x23, 0x95);     // N_DC=1001 0101
    SetReg(0x24, 0x95);     // P_DC=1001 0101
    SetReg(0x25, 0xFF);     // I_DC=1111 1111
    SetReg(0x27, 0x06);     // N_BP=0000 0110
    SetReg(0x28, 0x06);     // N_FP=0000 0110
    SetReg(0x29, 0x06);     // P_BP=0000 0110
    SetReg(0x2A, 0x06);     // P_FP=0000 0110
    SetReg(0x2C, 0x06);     // I_BP=0000 0110
    SetReg(0x2D, 0x06);     // I_FP=0000 0110
    SetReg(0x3A, 0x01);     // N_RTN=0000, N_NW=001
    SetReg(0x3B, 0x01);     // P_RTN=0000, P_NW=001
    SetReg(0x3C, 0xF0);     // I_RTN=1111, I_NW=000
    SetReg(0x3D, 0x00);     // DIV=00
    DelayMs(20);

    SetReg(0x10, 0xA6);     // SS=0,GS=0 CSEL=110

    // Power Supply Setting
    SetReg(0x19, 0x49);     // OSCADJ=10 0000, OSD_EN=1 //60Hz
    SetReg(0x93, 0x0C);     // RADJ=1100,
    DelayMs(10);

    SetReg(0x20, 0x40);     // BT=0100
    SetReg(0x1D, 0x07);     // VC1=111
    SetReg(0x1E, 0x00);     // VC3=000
    SetReg(0x1F, 0x04);     // VRH=0100          4.12V
    SetReg(0x44, 0x4D);     // VCM=101 0000   3.21V
    SetReg(0x45, 0x11);     // VDV=1 0001           -1.19V
    DelayMs(10);

    SetReg(0x1C, 0x04);     // AP=100
    DelayMs(20);
    SetReg(0x43, 0x80);     //set VCOMG=1
    DelayMs(5);
    SetReg(0x1B, 0x18);     // GASENB=0, PON=1, DK=1, XDK=0, DDVDH_TRI=0, STB=0
    DelayMs(40);

    SetReg(0x1B, 0x10);     // GASENB=0, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    DelayMs(40);

    // Display ON Setting
    SetReg(0x26, 0x04);     // GON=0, DTE=0, D=01
    DelayMs(40);
    SetReg(0x26, 0x24);     // GON=1, DTE=0, D=01
    SetReg(0x26, 0x2C);     // GON=1, DTE=0, D=11
    DelayMs(40);

    SetReg(0x26, 0x3C);     // GON=1, DTE=1, D=11
    SetReg(0x35, 0x38);     // EQS=38h
    SetReg(0x36, 0x78);     // EQP=78h
    SetReg(0x3E, 0x38);     // SON=38h
    SetReg(0x40, 0x0F);     // GDON=0Fh
    SetReg(0x41, 0xF0);     // GDOFF

    // Set spulse & rpulse
    SetReg(0x57, 0x02);     // Test mode='1'
    SetReg(0x56, 0x84);     // set Rpulse='1000',spulse='0100'
    SetReg(0x57, 0x00);     // Test mode= '0'
    #if (DISP_ORIENTATION == 0)
    SetReg(0x04, 0x00);
    SetReg(0x05, 0xEF);
    SetReg(0x08, 0x01);
    SetReg(0x09, 0x3F);
    #else
    SetReg(0x04, 0x01);
    SetReg(0x05, 0x3F);
    SetReg(0x08, 0x00);
    SetReg(0x09, 0xEF);
    #endif
    DelayMs(20);

#elif (DISPLAY_CONTROLLER == HX8347D)	

    // Driving ability setting
	SetReg(0xEA,0x00);   // PTBA[15:8]
	SetReg(0xEB,0x20);   // PTBA[7:0]
	SetReg(0xEC,0x0C);   // STBA[15:8]
	SetReg(0xED,0xC4);   // STBA[7:0]
	SetReg(0xE8,0x40);   // OPON[7:0]
	SetReg(0xE9,0x38);   // OPON1[7:0]
	SetReg(0xF1,0x01);   // OTPS1B
	SetReg(0xF2,0x10);   // GEN
	SetReg(0x27,0xA3);   // 
    
    // Gamma 2.8 setting 
	SetReg(0x40,0x00);   // 
	SetReg(0x41,0x00);   // 
	SetReg(0x42,0x01);   // 
	SetReg(0x43,0x13);   // 
	SetReg(0x44,0x10);   // 
	SetReg(0x45,0x26);   // 
	SetReg(0x46,0x08);   // 
	SetReg(0x47,0x51);   // 
	SetReg(0x48,0x02);   // 
	SetReg(0x49,0x12);   // 
	SetReg(0x4A,0x18);   // 
	SetReg(0x4B,0x19);   // 
	SetReg(0x4C,0x14);   // 
	
	SetReg(0x50,0x19);   // 
	SetReg(0x51,0x2F);   // 
	SetReg(0x52,0x2C);   // 
	SetReg(0x53,0x3E);   // 
	SetReg(0x54,0x3F);   //          
	SetReg(0x55,0x3F);   // 
	SetReg(0x56,0x2E);   // 
	SetReg(0x57,0x77);   // 
	SetReg(0x58,0x0B);   // 
	SetReg(0x59,0x06);   //          
	SetReg(0x5A,0x07);   // 
	SetReg(0x5B,0x0D);   // 
	SetReg(0x5C,0x1D);   // 
	SetReg(0x5D,0xCC);   // 

    // Window setting
#if (DISP_ORIENTATION == 0)
    SetReg(0x04,0x00);
    SetReg(0x05,0xEF);
    SetReg(0x08,0x01);
    SetReg(0x09,0x3F);
#else
    SetReg(0x04,0x01);
    SetReg(0x05,0x3F);
    SetReg(0x08,0x00);
    SetReg(0x09,0xEF);
#endif

    // Display Setting
	//SetReg(0x01,0x06);   // IDMON=0, INVON=1, NORON=1, PTLON=0 

#if (DISP_ORIENTATION == 0)
	SetReg(0x16,0x08);   // MY=0, MX=0, MV=0, BGR=1
#else
	SetReg(0x16,0x68);   // MY=0, MX=1, MV=1, BGR=1
#endif


    // Power Voltage Setting
	SetReg(0x1B,0x1B);   // VRH = 4.65
	SetReg(0x1A,0x01);   // BT
	SetReg(0x24,0x2F);   // VMH
	SetReg(0x25,0x57);   // VML
	
	// Vcom offset
	SetReg(0x23,0x8D);   // FLICKER ADJUST

    // Power ON Setting
	SetReg(0x18,0x36);   // 
	SetReg(0x19,0x01);   //  
	SetReg(0x01,0x00);   // 
	SetReg(0x1F,0x88);   // 	
	DelayMs(5); 
	SetReg(0x1F,0x80);   // 	
	DelayMs(5); 
	SetReg(0x1F,0x90);   // 	
	DelayMs(5); 
	SetReg(0x1F,0xD0);   // 	
	DelayMs(5); 

    // 65K Color Selection
	SetReg(0x17,0x05);   // 	
    
    // Set Panel
  	SetReg(0x36,0x00);   // 	

    // Display ON Setting
	SetReg(0x28,0x38);   // 	
	DelayMs( 40); 
	SetReg(0x28,0x3C);   // 	
#endif

}

/*********************************************************************
* Function: void PutPixel(SHORT x, SHORT y)
*
* PreCondition: none
*
* Input: x,y - pixel coordinates
*
* Output: none
*
* Side Effects: none
*
* Overview: puts pixel
*
* Note: none
*
********************************************************************/
void PutPixel(SHORT x, SHORT y)
{
    if(_clipRgn)
    {
        if(x < _clipLeft)
            return;
        if(x > _clipRight)
            return;
        if(y < _clipTop)
            return;
        if(y > _clipBottom)
            return;
    }

    DisplayEnable;
    SetAddress(x, y);
    WritePixel(_color);
    DisplayDisable();
}

/*********************************************************************
* Function: WORD Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
*
* PreCondition: none
*
* Input: left,top - top left corner coordinates,
*        right,bottom - bottom right corner coordinates
*
* Output: none
*
* Side Effects: none
*
* Overview: draws rectangle filled with current color
*
* Note: none
*
********************************************************************/
WORD Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
{
    register SHORT  x, y;

    #ifndef USE_NONBLOCKING_CONFIG
    while(IsDeviceBusy() != 0);

    /* Ready */
    #else
    if(IsDeviceBusy() != 0)
        return (0);
    #endif
    if(_clipRgn)
    {
        if(left < _clipLeft)
            left = _clipLeft;
        if(right > _clipRight)
            right = _clipRight;
        if(top < _clipTop)
            top = _clipTop;
        if(bottom > _clipBottom)
            bottom = _clipBottom;
    }

    DisplayEnable;
    for(y = top; y < bottom + 1; y++)
    {
        SetAddress(left, y);
        for(x = left; x < right + 1; x++)
        {
            WritePixel(_color);
        }
    }

    DisplayDisable();
    return (1);
}

/*********************************************************************
* Function: void ClearDevice(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: clears screen with current color 
*
* Note: none
*
********************************************************************/
void ClearDevice(void)
{
    DWORD   counter;

    DisplayEnable;
    SetAddress(0, 0);
    for(counter = 0; counter < (DWORD) (GetMaxX() + 1) * (GetMaxY() + 1); counter++)
    {
        WritePixel(_color);
    }

    DisplayDisable();
}

/*********************************************************************
* Function: WORD PutImage(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner,
*        bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
#ifdef USE_DRV_PUTIMAGE

/* */
WORD PutImage(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    FLASH_BYTE  *flashAddress;
    BYTE        colorDepth;
    WORD        colorTemp;

        #ifndef USE_NONBLOCKING_CONFIG
    while(IsDeviceBusy() != 0);

    /* Ready */
        #else
    if(IsDeviceBusy() != 0)
        return (0);
        #endif

    // Save current color
    colorTemp = _color;

    switch(*((SHORT *)bitmap))
    {
                #ifdef USE_BITMAP_FLASH

        case FLASH:

            // Image address
            flashAddress = ((BITMAP_FLASH *)bitmap)->address;

            // Read color depth
            colorDepth = *(flashAddress + 1);

            // Draw picture
            switch(colorDepth)
            {
                case 1:     PutImage1BPP(left, top, flashAddress, stretch); break;
                case 4:     PutImage4BPP(left, top, flashAddress, stretch); break;
                case 8:     PutImage8BPP(left, top, flashAddress, stretch); break;
                case 16:    PutImage16BPP(left, top, flashAddress, stretch); break;
            }

            break;
                #endif
                #ifdef USE_BITMAP_EXTERNAL

        case EXTERNAL:

            // Get color depth
            ExternalMemoryCallback(bitmap, 1, 1, &colorDepth);

            // Draw picture
            switch(colorDepth)
            {
                case 1:     PutImage1BPPExt(left, top, bitmap, stretch); break;
                case 4:     PutImage4BPPExt(left, top, bitmap, stretch); break;
                case 8:     PutImage8BPPExt(left, top, bitmap, stretch); break;
                case 16:    PutImage16BPPExt(left, top, bitmap, stretch); break;
                default:    break;
            }

            break;
                #endif

        default:
            break;
    }

    // Restore current color
    _color = colorTemp;
    return (1);
}

    #ifdef USE_BITMAP_FLASH

/*********************************************************************
* Function: void PutImage1BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner,
*        bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage1BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    BYTE                temp;
    WORD                sizeX, sizeY;
    WORD                x, y;
    BYTE                stretchX, stretchY;
    WORD                pallete[2];
    BYTE                mask;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    pallete[0] = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    pallete[1] = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    DisplayEnable;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            SetAddress((WORD) left, (WORD) top);
            mask = 0;
            for(x = 0; x < sizeX; x++)
            {

                // Read 8 pixels from flash
                if(mask == 0)
                {
                    temp = *flashAddress;
                    flashAddress++;
                    mask = 0x80;
                }

                // Set color
                if(mask & temp)
                {
                    SetColor(pallete[1]);
                }
                else
                {
                    SetColor(pallete[0]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }

                // Shift to the next pixel
                mask >>= 1;
            }

            top++;
        }
    }

    DisplayDisable();
}

/*********************************************************************
* Function: void PutImage4BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs 16 color image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage4BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    WORD                sizeX, sizeY;
    register WORD       x, y;
    BYTE                temp;
    register BYTE       stretchX, stretchY;
    WORD                pallete[16];
    WORD                counter;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    // Read pallete
    for(counter = 0; counter < 16; counter++)
    {
        pallete[counter] = *((FLASH_WORD *)flashAddress);
        flashAddress += 2;
    }

    DisplayEnable;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            SetAddress(left, top);
            for(x = 0; x < sizeX; x++)
            {

                // Read 2 pixels from flash
                if(x & 0x0001)
                {

                    // second pixel in byte
                    SetColor(pallete[temp >> 4]);
                }
                else
                {
                    temp = *flashAddress;
                    flashAddress++;

                    // first pixel in byte
                    SetColor(pallete[temp & 0x0f]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }
    }

    DisplayDisable();
}

/*********************************************************************
* Function: void PutImage8BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs 256 color image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage8BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_BYTE *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    WORD                sizeX, sizeY;
    WORD                x, y;
    BYTE                temp;
    BYTE                stretchX, stretchY;
    WORD                pallete[256];
    WORD                counter;

    // Move pointer to size information
    flashAddress = bitmap + 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;

    // Read pallete
    for(counter = 0; counter < 256; counter++)
    {
        pallete[counter] = *((FLASH_WORD *)flashAddress);
        flashAddress += 2;
    }

    DisplayEnable;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            SetAddress(left, top);
            for(x = 0; x < sizeX; x++)
            {

                // Read pixels from flash
                temp = *flashAddress;
                flashAddress++;

                // Set color
                SetColor(pallete[temp]);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }
    }

    DisplayDisable();
}

/*********************************************************************
* Function: void PutImage16BPP(SHORT left, SHORT top, FLASH_BYTE* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs hicolor image starting from left,top coordinates
*
* Note: image must be located in flash
*
********************************************************************/
void PutImage16BPP(SHORT left, SHORT top, FLASH_BYTE *bitmap, BYTE stretch)
{
    register FLASH_WORD *flashAddress;
    register FLASH_WORD *tempFlashAddress;
    WORD                sizeX, sizeY;
    register WORD       x, y;
    WORD                temp;
    register BYTE       stretchX, stretchY;

    // Move pointer to size information
    flashAddress = (FLASH_WORD *)bitmap + 1;

    // Read image size
    sizeY = *flashAddress;
    flashAddress++;
    sizeX = *flashAddress;
    flashAddress++;

    DisplayEnable;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            flashAddress = tempFlashAddress;
            SetAddress(left, top);
            for(x = 0; x < sizeX; x++)
            {

                // Read pixels from flash
                temp = *flashAddress;
                flashAddress++;

                // Set color
                SetColor(temp);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }
    }

    DisplayDisable();
}

    #endif
    #ifdef USE_BITMAP_EXTERNAL

/*********************************************************************
* Function: void PutImage1BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage1BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[2];
    BYTE            lineBuffer[((GetMaxX() + 1) / 8) + 1];
    BYTE            *pData;
    SHORT           byteWidth;

    BYTE            temp;
    BYTE            mask;
    WORD            sizeX, sizeY;
    WORD            x, y;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (2 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 2 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 2 * sizeof(WORD);

    // Line width in bytes
    byteWidth = bmp.width >> 3;
    if(bmp.width & 0x0007)
        byteWidth++;

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        DisplayEnable;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            SetAddress(left, top);
            mask = 0;
            for(x = 0; x < sizeX; x++)
            {

                // Read 8 pixels from flash
                if(mask == 0)
                {
                    temp = *pData++;
                    mask = 0x80;
                }

                // Set color
                if(mask & temp)
                {
                    SetColor(pallete[1]);
                }
                else
                {
                    SetColor(pallete[0]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }

                // Shift to the next pixel
                mask >>= 1;
            }

            top++;
        }

        DisplayDisable();
    }
}

/*********************************************************************
* Function: void PutImage4BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage4BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[16];
    BYTE            lineBuffer[((GetMaxX() + 1) / 2) + 1];
    BYTE            *pData;
    SHORT           byteWidth;

    BYTE            temp;
    WORD            sizeX, sizeY;
    WORD            x, y;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (16 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 16 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 16 * sizeof(WORD);

    // Line width in bytes
    byteWidth = bmp.width >> 1;
    if(bmp.width & 0x0001)
        byteWidth++;

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        DisplayEnable;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            SetAddress(left, top);

            for(x = 0; x < sizeX; x++)
            {

                // Read 2 pixels from flash
                if(x & 0x0001)
                {

                    // second pixel in byte
                    SetColor(pallete[temp >> 4]);
                }
                else
                {
                    temp = *pData++;

                    // first pixel in byte
                    SetColor(pallete[temp & 0x0f]);
                }

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }

        DisplayDisable();
    }
}

/*********************************************************************
* Function: void PutImage8BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage8BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            pallete[256];
    BYTE            lineBuffer[(GetMaxX() + 1)];
    BYTE            *pData;

    BYTE            temp;
    WORD            sizeX, sizeY;
    WORD            x, y;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Get pallete (256 entries)
    ExternalMemoryCallback(bitmap, sizeof(BITMAP_HEADER), 256 * sizeof(WORD), pallete);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER) + 256 * sizeof(WORD);

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, sizeX, lineBuffer);
        memOffset += sizeX;
        DisplayEnable;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            SetAddress(left, top);

            for(x = 0; x < sizeX; x++)
            {
                temp = *pData++;
                SetColor(pallete[temp]);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }

        DisplayDisable();
    }
}

/*********************************************************************
* Function: void PutImage16BPPExt(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* PreCondition: none
*
* Input: left,top - left top image corner, bitmap - image pointer,
*        stretch - image stretch factor
*
* Output: none
*
* Side Effects: none
*
* Overview: outputs monochrome image starting from left,top coordinates
*
* Note: image must be located in external memory
*
********************************************************************/
void PutImage16BPPExt(SHORT left, SHORT top, void *bitmap, BYTE stretch)
{
    register DWORD  memOffset;
    BITMAP_HEADER   bmp;
    WORD            lineBuffer[(GetMaxX() + 1)];
    WORD            *pData;
    WORD            byteWidth;

    WORD            temp;
    WORD            sizeX, sizeY;
    WORD            x, y;
    BYTE            stretchX, stretchY;

    // Get bitmap header
    ExternalMemoryCallback(bitmap, 0, sizeof(BITMAP_HEADER), &bmp);

    // Set offset to the image data
    memOffset = sizeof(BITMAP_HEADER);

    // Get size
    sizeX = bmp.width;
    sizeY = bmp.height;

    byteWidth = sizeX << 1;

    for(y = 0; y < sizeY; y++)
    {

        // Get line
        ExternalMemoryCallback(bitmap, memOffset, byteWidth, lineBuffer);
        memOffset += byteWidth;
        DisplayEnable;
        for(stretchY = 0; stretchY < stretch; stretchY++)
        {
            pData = lineBuffer;
            SetAddress(left, top);

            for(x = 0; x < sizeX; x++)
            {
                temp = *pData++;
                SetColor(temp);

                // Write pixel to screen
                for(stretchX = 0; stretchX < stretch; stretchX++)
                {
                    WritePixel(_color);
                }
            }

            top++;
        }

        DisplayDisable();
    }
}

    #endif
#endif // USE_DRV_PUTIMAGE
