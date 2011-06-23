#include "PCF8535.h"
#include "TCPIP Stack/TCPIP.h"
#include <i2c.h>
#define uint8_t unsigned short
#define uint16_t unsigned int
#define LCD_RESET LATCbits.LATC5 
#define LCD_SIZE_X 133
#define LCD_SIZE_Y 64
void lcd_set_contrast(uint8_t contrast);
// Start write transaction:
// Generate START condition, send device address and control byte
// return non-zero on success, 0 if NACK received
static uint8_t lcd_start_write(uint8_t control_byte)
{
	StartI2C1();
	//SSP1CON2bits.SEN = 1;
	if(WriteI2C1(PCF8535_BUS_ADDRESS)!= 0)
		return 0;
	if(WriteI2C1(control_byte)!= 0)
		return 0;
	return 1;
}

// Send [size] bytes to LCD
// return non-zero on success, 0 if NACK received
uint8_t lcd_send_array(uint8_t *data, uint16_t size)
{
	do{
		if(WriteI2C1(*data++)!= 0)
			return 0;
	}
	while (--size);
	return 1;
}

// LCD controller init sequence
static uint8_t lcd_init_sequence[] =
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
	| paramLCD_ALL_ON
	,
	cmdLCD_EXT_DISPLAY_CTL
	| paramLCD_XDIRECTION_NORMAL
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
	| paramLCD_HV_MUL3
	,
	cmdLCD_TEMP_COEFF
	| paramLCD_TC_274
	,
	cmdLCD_VLCD_CONTROL
	| 51
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

static uint8_t lcd_set_home_sequence[] =
{
	cmdLCD_DEFAULT_PAGE
	,
	cmdLCD_SET_Y
	| 0
	,
	cmdLCD_SET_X
	| 0
	,
};
void lcd_home()
{
	lcd_start_write(modeLCD_CMD_TILL_STOP);
	lcd_send_array(lcd_set_home_sequence, sizeof(lcd_set_home_sequence));
	StopI2C();
}

void lcd_init()
{
	uint16_t i;
	// Set lcd reset pin as output
	// Set lcd reset pin low
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
	if(lcd_start_write(modeLCD_CMD_TILL_STOP)==1){
		
		if(lcd_send_array(lcd_init_sequence, sizeof(lcd_init_sequence))==1){
			LATAbits.LATA0 =1;
		}
	}
	StopI2C1();	
		// Now lcd address pointer set to row 0, col. 0.
		// fill whole screen with black pattern
	if(lcd_start_write(modeLCD_DATA_TILL_STOP)==1){
		// lcd address pointer autoincrements,
		// so we can just draw LCD_SIZE_X*LCD_SIZE_Y points.
		// one byte fills 8 dots on screen,
		LATAbits.LATA1 =1;
		//for(i = 0; i < LCD_SIZE_X * LCD_SIZE_Y / 16; ++i)
		//{
			WriteI2C(0x1f);
			WriteI2C(0x05);
			WriteI2C(0x07);
			WriteI2C(0x00);
			WriteI2C(0x1f);
			WriteI2C(0x04);
			WriteI2C(0x1F);
		//}
		
	}
	StopI2C1();
	
}
/*
static uint8_t lcd_init_sequence1[] =
{
	0x01,0x10,//    ; page 7, PD=0, V=0
	0x0E,0x12,//    ; page 6, bias = 1/9
	0x07,0x0E,//    ; D=1, E=1, mirror X,Y
	0x84,0x01,//    ; mux=1/65
	0x0D,0x08,//
	0x12,0xBC,//    ; Vdd x 2, TC=2, Vlcd ~7V
	0x05,0x01,//    ; PRS=0, HVE=1
	0x0B,0x58,//    ; page 3, TRS=1, BRS=1
};

void lcd_init2()
{
	uint16_t i;
	// Set lcd reset pin as output
	// Set lcd reset pin low
	LCD_RESET = 0;
	DelayMs(1); // tW(RESL)
	LCD_RESET = 1;
	DelayMs(5); // tW(RESH)
	LCD_RESET = 0;
	DelayMs(1); // tW(RESL)
	LCD_RESET = 0;
	DelayMs(3); // tR(OP)
	
	//***************************************************
	//* Lcd init commands *
	//***************************************************
	
	if(lcd_start_write(modeLCD_CMD_TILL_STOP)==0){
		LATGbits.LATG4 =1;
		lcd_send_array(lcd_init_sequence1, sizeof(lcd_init_sequence1));
		
	}
	StopI2C1();
	DelayMs(1000);
	LATGbits.LATG4 =0;
	DelayMs(1000);
		// Now lcd address pointer set to row 0, col. 0.
		// fill whole screen with black pattern
	if(lcd_start_write(modeLCD_DATA_TILL_STOP)==0){
		// lcd address pointer autoincrements,
		// so we can just draw LCD_SIZE_X*LCD_SIZE_Y points.
		// one byte fills 8 dots on screen,
		LATGbits.LATG4 =1;
		//for(i = 0; i < LCD_SIZE_X * LCD_SIZE_Y / 16; ++i)
		//{
			WriteI2C1(0x1f);
			WriteI2C1(0x05);
			WriteI2C1(0x07);
			WriteI2C1(0x00);
			WriteI2C1(0x1f);
			WriteI2C1(0x04);
			WriteI2C1(0x1F);
		//}		
	}
	StopI2C1();
	DelayMs(1000);
	LATGbits.LATG4 =0;
	DelayMs(1000);
}
*/
void lcd_set_contrast(uint8_t contrast)
{
	if(lcd_start_write(modeLCD_CMD_TILL_STOP)){
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
	}
}
