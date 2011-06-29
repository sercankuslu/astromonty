#ifndef PCF8535_H__
#define PCF8535_H__
#include "TCPIP Stack/TCPIP.h"
#include "GenericTypeDefs.h"
/*
PCF8535 lcd controller control codes
*/

#define PCF8535_BUS_ADDRESS 0x78
#define LCD_RESET LATCbits.LATC5 
#define LCD_SIZE_X 133
#define LCD_SIZE_Y 64

void LCDInit(BYTE addr);
void LCDSetContrast(BYTE contrast);
int LCDSendData(BYTE add1,BYTE* wrptr, BYTE size);
int LCDSendCommand(BYTE add1,BYTE* wrptr, BYTE size);
int LCDClearData(BYTE add1);
void LCDSetXY(BYTE add1, BYTE X,BYTE Y);

// Co, D/~C bits
enum
{
	modeLCD_CMD_TILL_STOP = 0x00,
	modeLCD_CMD_SINGLE = 0x80,
	modeLCD_DATA_TILL_STOP = 0x40,
	modeLCD_DATA_SINGLE = 0xC0
};

enum
{
	cmdLCD_DEFAULT_PAGE = 0x01,
	// Function and RAM command page
	cmdLCD_COMMAND_PAGE = 0x08,
	paramLCD_PAGE_FUNCTION_AND_RAM = 0x07, //H[2:0] = 111
	paramLCD_PAGE_DISPLAY_SETTINGS = 0x06, //H[2:0] = 110
	paramLCD_PAGE_HV_GEN = 0x05, //H[2:0] = 101
	paramLCD_PAGE_SPECIAL_FEATURE = 0x03, //H[2:0] = 011
	
	cmdLCD_FUNCTION_SET = 0x10,
	paramLCD_POWER_DOWN = (1<<2), // PD = 0
	paramLCD_ACTIVE = (0<<2), // PD = 1
	paramLCD_HORIZONTAL_ADDRESSING = (0<<1), // V = 0
	paramLCD_VERTICAL_ADDRESSING = (1<<1), // V = 1
	
	cmdLCD_RAM_PAGE = 0x20,
	paramLCD_PAGE0 = (0<<2), // XM0 = 0
	paramLCD_PAGE1 = (1<<2), // XM0 = 0
	
	cmdLCD_SET_Y = 0x40,
	
	cmdLCD_SET_X = 0x80,
	
	// Display settings command page
	cmdLCD_DISPLAY_CONTROL = 0x04,
	paramLCD_BLANK = (0<<1),//|(0<<0), // D = 0, E = 0
	paramLCD_NORMAL = (1<<1),//|(0<<0), // D = 1, E = 0
	paramLCD_ALL_ON = (0<<1)|(1),//<<0), // D = 0, E = 1
	paramLCD_INVERSE = (1<<1)|(1),//<<0), // D = 1, E = 1
	
	cmdLCD_EXT_DISPLAY_CTL = 0x08,
	paramLCD_XDIRECTION_REVERSE = 0,//(0<<2), // MX = 0
	paramLCD_XDIRECTION_NORMAL = (1<<2), // MX = 1
	paramLCD_YDIRECTION_REVERSE = 0,//(0<<1), // MY = 0
	paramLCD_YDIRECTION_NORMAL = (1<<1), // MY = 1
	
	cmdLCD_BIAS_SYSTEM = 0x10,
	paramLCD_BIAS_11 = 0, // Bias 1/11, MUX 1:100
	paramLCD_BIAS_10 = 1, // Bias 1/10, MUX 1:80
	paramLCD_BIAS_9 = 2, // Bias 1/9, MUX 1:65
	paramLCD_BIAS_8 = 3, // Bias 1/8, MUX 1:49
	paramLCD_BIAS_7 = 4, // Bias 1/7, MUX 1:34
	paramLCD_BIAS_6 = 5, // Bias 1/6, MUX 1:26
	paramLCD_BIAS_5 = 6, // Bias 1/5, MUX 1:17
	paramLCD_BIAS_4 = 7, // Bias 1/4, MUX 1:9
	
	cmdLCD_DISPLAY_SIZE = 0x20,
	paramLCD_SMALL_DISPLAY = 0,//(0<<2), // IB = 0
	paramLCD_LARGE_DISPLAY = (1<<2), // IB = 1
	
	cmdLCD_MULTILPEX_RATE = 0x80,
	paramLCD_MUX_17 = 0, // MUX 1:17
	paramLCD_MUX_26 = 1, // MUX 1:26
	paramLCD_MUX_34 = 2, // MUX 1:34
	paramLCD_MUX_49 = 3, // MUX 1:49
	paramLCD_MUX_65 = 4, // MUX 1:65
	
	// HV-gen command page
	cmdLCD_HV_GEN_CONTROL = 0x04,
	paramLCD_HV_DISABLED = 0,//(0<<0), // HVE = 0
	paramLCD_HV_ENABLED = 1,//(1<<0), // HVE = 1
	paramLCD_VLCD_LOW = (0<<1), // PRS = 0
	paramLCD_VLCD_HIGH = (1<<1), // PRS = 1
	
	cmdLCD_HV_GEN_STAGES = 0x08,
	paramLCD_HV_MUL2 = 0, // S1 = 0, S0 = 0
	paramLCD_HV_MUL3 = 1, // S1 = 0, S0 = 1
	paramLCD_HV_MUL4 = 2, // S1 = 1, S0 = 0
	paramLCD_HV_MUL5 = 3, // S1 = 1, S0 = 1
	cmdLCD_TEMP_COEFF = 0x10,
	paramLCD_TC_0 = 0, // TC = 0
	paramLCD_TC_044 = 1, // TC = -0.44e-3 1/C
	paramLCD_TC_110 = 2, // TC = -1.10e-3 1/C
	paramLCD_TC_145 = 3, // TC = -1.45e-3 1/C
	paramLCD_TC_191 = 4, // TC = -1.91-3 1/C
	paramLCD_TC_215 = 5, // TC = -2.15e-3 1/C
	paramLCD_TC_232 = 6, // TC = -2.32e-3 1/C
	paramLCD_TC_274 = 7, // TC = -2.74e-3 1/C
	cmdLCD_TEMP_MEAS_CTL = 0x20,
	paramLCD_START_MEASURE = 1,//(1<<0), // SM = 1
	cmdLCD_VLCD_CONTROL = 0x80,
	
	// Special feature command page
	cmdLCD_STATE_CONTROL = 0x04,
	paramLCD_DISPLAY_OFF = (1<<1), // DOF = 1
	paramLCD_DIRECT_MODE = 1,//(1<<0), // DM = 1
	cmdLCD_OSC_SETTING = 0x08,
	paramLCD_OSC_INTERNAL = (0<<1), // EC = 0
	paramLCD_OSC_EXTERNAL = (1<<1), // EC = 1
	cmdLCD_COG_TCP = 0x40,
	paramLCD_BOT_ROW_SWAP_DISABLED = 0,//(0<<3), // BRS = 0
	paramLCD_BOT_ROW_SWAP_ENABLED = (1<<3), // BRS = 0
	paramLCD_TOP_ROW_SWAP_DISABLED = 0,//(0<<4), // TRS = 0
	paramLCD_TOP_ROW_SWAP_ENABLED = (1<<4) // TRS = 0
}; 

#endif // PCF8535_H__
