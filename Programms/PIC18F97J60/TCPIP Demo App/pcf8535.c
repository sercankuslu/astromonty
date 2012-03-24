#include "stdafx.h"
#include "PCF8535.h"

#ifndef _WINDOWS
#include "TCPIP Stack/TCPIP.h"
#include <i2c.h>
#endif

#ifdef _WINDOWS
struct LATCbits {
    BYTE LATC5;
}LATCbits;
struct TRISCbits {
    BYTE TISC5;
}TRISCbits;
#   define LCD_RESET LATCbits.LATC5
#   define LCD_RESET_TRIS TRISCbits.TRISC5
#endif

// LCD controller init sequence
static BYTE INIT_SEQUENCE[] =
{
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_COMMAND_PAGE
	| paramLCD_PAGE_DISPLAY_SETTINGS
	,
	cmdLCD_BIAS_SYSTEM
	| paramLCD_BIAS_9 // Bias 1/9, MUX 1:65
	,
	cmdLCD_MULTILPEX_RATE
	| paramLCD_MUX_65
	,
	cmdLCD_DISPLAY_CONTROL
	| paramLCD_NORMAL
	,
	cmdLCD_EXT_DISPLAY_CTL
	| paramLCD_XDIRECTION_REVERSE
	| paramLCD_YDIRECTION_NORMAL
	,
	cmdLCD_DISPLAY_SIZE
	| paramLCD_LARGE_DISPLAY
	,
	
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_COMMAND_PAGE
	| paramLCD_PAGE_HV_GEN
	,
	cmdLCD_HV_GEN_CONTROL
	| paramLCD_HV_ENABLED
	| paramLCD_VLCD_LOW
	,
	cmdLCD_HV_GEN_STAGES
	| paramLCD_HV_MUL5
	,
	cmdLCD_TEMP_COEFF
	| paramLCD_TC_274
	,
	cmdLCD_VLCD_CONTROL
	| 55
	,
	
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_COMMAND_PAGE
	| paramLCD_PAGE_SPECIAL_FEATURE
	,
	cmdLCD_STATE_CONTROL
	| 0
	,
	cmdLCD_OSC_SETTING
	| paramLCD_OSC_INTERNAL
	,
	cmdLCD_COG_TCP
	| paramLCD_BOT_ROW_SWAP_ENABLED
	| paramLCD_TOP_ROW_SWAP_ENABLED
	,
	
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_FUNCTION_SET
	| paramLCD_ACTIVE
	| paramLCD_HORIZONTAL_ADDRESSING
	,
	cmdLCD_RAM_PAGE
	| paramLCD_PAGE0
	,
	cmdLCD_SET_Y
	| 0
	,
	cmdLCD_SET_X
	| 0
	,
};

BYTE I2C_Recv[21];
static char START = 0;
void pcfLCDInit(BYTE addr)
{
#ifndef _WINDOWS
	WORD i;
	BYTE sync_mode=0;
	BYTE slew=0; 

	// Set lcd reset pin as output
	// Set lcd reset pin low
	LCD_RESET_TRIS = 0;
	LCD_RESET = 0;
	DelayMs(500); // tW(RESL)
	LCD_RESET = 1;
	DelayMs(500); // tW(RESH)
	LCD_RESET = 0;
	DelayMs(1); // tW(RESL)
	LCD_RESET = 1;
	DelayMs(3); // tR(OP)
	
	//***************************************************
	//* Lcd init commands *
	//***************************************************
    for(i=0;i<20;i++)
    I2C_Recv[i]=0;

    addr=PCF8535_BUS_ADDRESS;        //address of the device (slave) under communication

    CloseI2C();    //close i2c if was operating earlier

	//---INITIALISE THE I2C MODULE FOR MASTER MODE WITH 100KHz ---
    sync_mode = MASTER;
    slew = SLEW_OFF;

    OpenI2C1(sync_mode,slew);

    SSPADD=0x0A;             //400kHz Baud clock(9) @8MHz
    //check for bus idle condition in multi master communication
    IdleI2C();

    //---START I2C---
    //StartI2C();
    LCDSendCommand(addr,(BYTE*)INIT_SEQUENCE,sizeof(INIT_SEQUENCE));
#else 
    UNUSED(addr); // заглушка
#endif
}

void LCDSetContrast(BYTE contrast)
{
#ifdef _WINDOWS
    UNUSED(contrast); // заглушка
#endif
/*	if(lcd_start_write(modeLCD_CMD_TILL_STOP)){
		WriteI2C1(cmdLCD_DEFAULT_PAGE);
		WriteI2C1(cmdLCD_COMMAND_PAGE | paramLCD_PAGE_HV_GEN);
	
		if(contrast > 128)
		{
			WriteI2C1(cmdLCD_HV_GEN_CONTROL
			| paramLCD_HV_ENABLED
			| paramLCD_VLCD_HIGH
			);
			contrast -= 128;
		}
		else
		{
			WriteI2C1(cmdLCD_HV_GEN_CONTROL
			| paramLCD_HV_ENABLED
			| paramLCD_VLCD_LOW
			);
		}
		WriteI2C1(cmdLCD_VLCD_CONTROL | contrast >> 1);
		StopI2C1();
	}*/
}

int LCDSendData(BYTE add1,BYTE* wrptr, BYTE size)
{
#ifndef _WINDOWS
	BYTE data; 
	BYTE status; 
	BYTE i;
	BYTE_VAL d;
	BYTE_VAL d1;
	
	RestartI2C();
        IdleI2C();

	//****write the address of the device for communication***
    data = SSPBUF;        //read any previous stored content in buffer to clear buffer full status
    do
    {
    status = WriteI2C( add1 | 0x00 );    //write the address of slave
        if(status == -1)        //check if bus collision happened
        {
            data = SSPBUF;        //upon bus collision detection clear the buffer,
            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
			//LATAbits.LATA1 =1;
        }
    }
    while(status!=0);        //write untill successful communication
	//R/W BIT IS '0' FOR FURTHER WRITE TO SLAVE
	do
    {
    status = WriteI2C(modeLCD_DATA_TILL_STOP);    //write the address of slave
        if(status == -1)        //check if bus collision happened
        {
            data = SSPBUF;        //upon bus collision detection clear the buffer,
            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
			//LATAbits.LATA1 =1;
        }
    }
    while(status!=0);        //write untill successful communication
	//***WRITE THE THE DATA TO BE SENT FOR SLAVE***
	//write string of data to be transmitted to slave
   	for (i=0;i<size;i++ )                 // transmit data 
	{
	  	if ( SSP1CON1bits.SSPM3 )      // if Master transmitter then execute the following
		{
			//temp = putcI2C1 ( *wrptr );
			//if (temp ) return ( temp );   
			d.Val = *wrptr;
                        // swapping bits
                        d1.bits.b0 = d.bits.b7; 
                        d1.bits.b1 = d.bits.b6; 
                        d1.bits.b2 = d.bits.b5; 
                        d1.bits.b3 = d.bits.b4; 
                        d1.bits.b4 = d.bits.b3; 
                        d1.bits.b5 = d.bits.b2; 
                        d1.bits.b6 = d.bits.b1; 
                        d1.bits.b7 = d.bits.b0; 
//
			if ( WriteI2C( d1.Val ) )    // write 1 byte
			{
			  return ( -3 );             // return with write collision error
			}
			IdleI2C();                  // test for idle condition
			if ( SSP1CON2bits.ACKSTAT )  // test received ack bit state
			{
			  return ( -2 );             // bus device responded with  NOT ACK
			}                            // terminate putsI2C1() function
		}	  
	wrptr ++;                        // increment pointer
	}            
    
	
//---TERMINATE COMMUNICATION FROM MASTER SIDE---
     IdleI2C();
#else
    UNUSED(add1);
    UNUSED(wrptr);
    UNUSED(size);
#endif
     return 0;
}
/************************************************************
************************************************************/
int LCDSendCommand(BYTE add1,BYTE* wrptr, BYTE size)
{
#ifdef _WINDOWS
    UNUSED(add1);
    UNUSED(wrptr);
    UNUSED(size);
#else
    BYTE i;
	BYTE data; 
	BYTE status; 
	
	//****write the address of the device for communication***
	if(START){
  		RestartI2C();
	} 
	else {
		StartI2C();
		START=1;
	}
    IdleI2C();

    data = SSPBUF;        //read any previous stored content in buffer to clear buffer full status
    do
    {
    status = WriteI2C( add1 | 0x00 );    //write the address of slave
        if(status == -1)        //check if bus collision happened
        {
            data = SSPBUF;        //upon bus collision detection clear the buffer,
            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
			//LATAbits.LATA1 =1;
        }
    }
    while(status!=0);        //write untill successful communication
	//R/W BIT IS '0' FOR FURTHER WRITE TO SLAVE
	do
    {
    status = WriteI2C(modeLCD_CMD_TILL_STOP);    //write the address of slave
        if(status == -1)        //check if bus collision happened
        {
            data = SSPBUF;        //upon bus collision detection clear the buffer,
            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
			//LATAbits.LATA1 =1;
        }
    }
    while(status!=0);        //write untill successful communication
	//***WRITE THE THE DATA TO BE SENT FOR SLAVE***
	//write string of data to be transmitted to slave
   	for (i=0;i<size;i++ )                 // transmit data 
	{
	  	if ( SSP1CON1bits.SSPM3 )      // if Master transmitter then execute the following
		{
			//temp = putcI2C1 ( *wrptr );
			//if (temp ) return ( temp );   	
			if ( WriteI2C1( *wrptr ) )    // write 1 byte
			{
			  return ( -3 );             // return with write collision error
			}
			IdleI2C1();                  // test for idle condition
			if ( SSP1CON2bits.ACKSTAT )  // test received ack bit state
			{
			  return ( -2 );             // bus device responded with  NOT ACK
			}                            // terminate putsI2C1() function
		}	  
	wrptr ++;                        // increment pointer
	}            
    IdleI2C();
#endif
    return 0;
}
/************************************************************
************************************************************/

int LCDClearData(BYTE add1)
{
#ifdef _WINDOWS
    UNUSED(add1);
#else
	BYTE i; 
	BYTE j; 
	BYTE data; 
	BYTE status; 	
		
	for(j=0;j<7;j++){
		LCDSetXY(add1,0,j);
		RestartI2C();
	    IdleI2C();
	
		//****write the address of the device for communication***
	    data = SSPBUF;        //read any previous stored content in buffer to clear buffer full status
	    do
	    {
	    status = WriteI2C( add1 | 0x00 );    //write the address of slave
	        if(status == -1)        //check if bus collision happened
	        {
	            data = SSPBUF;        //upon bus collision detection clear the buffer,
	            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
				//LATAbits.LATA1 =1;
	        }
	    }
	    while(status!=0);        //write untill successful communication
		//R/W BIT IS '0' FOR FURTHER WRITE TO SLAVE
		do
	    {
	    status = WriteI2C(modeLCD_DATA_TILL_STOP);    //write the address of slave
	        if(status == -1)        //check if bus collision happened
	        {
	            data = SSPBUF;        //upon bus collision detection clear the buffer,
	            SSPCON1bits.WCOL=0;    // clear the bus collision status bit
				//LATAbits.LATA1 =1;
	        }
	    }
	    while(status!=0);        //write untill successful communication
		//***WRITE THE THE DATA TO BE SENT FOR SLAVE***
		//write string of data to be transmitted to slave
	   	for (i=0;i<128;i++ )                 // transmit data 
		{
		  	if ( SSP1CON1bits.SSPM3 )      // if Master transmitter then execute the following
			{
				//temp = putcI2C1 ( *wrptr );
				//if (temp ) return ( temp );   	
				if ( WriteI2C1( 0 ) )    // write 1 byte
				{
				  return ( -3 );             // return with write collision error
				}
				IdleI2C1();                  // test for idle condition
				if ( SSP1CON2bits.ACKSTAT )  // test received ack bit state
				{
				  return ( -2 );             // bus device responded with  NOT ACK
				}                            // terminate putsI2C1() function
			}	  	          
		}         
	    
		
	//---TERMINATE COMMUNICATION FROM MASTER SIDE---
	     IdleI2C();
	}
#endif
	return 0;
}
/************************************************************
************************************************************/
void LCDSetXY(BYTE add1, BYTE X,BYTE Y)
{
	BYTE temp[3];

	temp[0]= cmdLCD_DEFAULT_PAGE;
	temp[1]= cmdLCD_SET_Y | Y;
	temp[2]= cmdLCD_SET_X | X;
	LCDSendCommand(add1,temp,3);
}
