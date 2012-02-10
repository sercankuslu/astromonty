#include "stdafx.h"
#include "font.h"
#ifndef _WINDOWS
#include "GenericTypeDefs.h"
#pragma romdata overlay FONT_SECTION =0x00AC00
static rom 
#else
static
#endif
    WORD Arial_Data[] = {
    0x0,0x0,0x0,0x0,0x0,    	
	0x02F,
	0x003,0x00,0x000,
	0x024,0x07E,0x024,0x024,0x07E,0x024,
	0x024,0x04A,0x0FF,0x052,0x024,
	0x002,0x025,0x012,0x008,0x004,0x012,0x029,0x010,
	0x010,0x02A,0x025,0x02D,0x012,0x038,0x024,
	0x003,
	0x07E,0x0C3,
	0x0C3,0x07E,
	0x00A,0x00A,0x004,0x01F,0x004,0x00A,0x00A,
	0x008,0x01C,0x008,
	0x100,0x080,
	0x008,0x008,0x008,
	0x080,
	0x030,0x00C,0x003,
	0x01E,0x021,0x021,0x01E,
	0x002,0x03F,
	0x022,0x031,0x029,0x026,
	0x012,0x021,0x025,0x01A,
	0x018,0x014,0x012,0x03F,0x010,
	0x017,0x025,0x025,0x019,
	0x01E,0x025,0x025,0x019,
	0x001,0x039,0x005,0x003,
	0x01A,0x025,0x025,0x01A,
	0x026,0x029,0x029,0x01E,
	0x024,
	0x040,0x024,
	0x008,0x014,0x022,
	0x014,0x014,0x014,0x014,
	0x022,0x014,0x008,
	0x002,0x029,0x006,
	0x038,0x044,0x092,0x0AA,0x0BA,0x0A2,0x09C,0x040,
	0x03E,0x009,0x009,0x03E,
	0x03F,0x025,0x025,0x01A,
	0x01E,0x021,0x021,0x012,
	0x03F,0x021,0x021,0x01E,
	0x03F,0x025,0x025,0x021,
	0x03F,0x005,0x005,0x001,
	0x01E,0x021,0x029,0x03A,
	0x03F,0x004,0x004,0x03F,
	0x021,0x03F,0x021,
	0x010,0x020,0x020,0x01F,
	0x03F,0x004,0x00A,0x031,
	0x03F,0x020,0x020,0x020,
	0x03F,0x002,0x004,0x002,0x03F,
	0x03F,0x018,0x006,0x03F,
	0x00C,0x012,0x021,0x021,0x012,0x00C,
	0x03F,0x009,0x009,0x006,
	0x00C,0x012,0x021,0x029,0x012,0x02C,
	0x03F,0x009,0x009,0x036,
	0x012,0x025,0x029,0x012,
	0x001,0x001,0x03F,0x001,0x001,
	0x01F,0x020,0x020,0x01F,
	0x003,0x00C,0x030,0x00C,0x003,
	0x003,0x00C,0x030,0x00C,0x003,0x00C,0x030,0x00C,0x003,
	0x021,0x012,0x00C,0x012,0x021,
	0x003,0x004,0x038,0x004,0x003,
	0x021,0x031,0x029,0x025,0x023,0x021,
	0x0FF,0x081,
	0x003,0x03C,0x0C0,
	0x081,0x0FF,
	0x004,0x002,0x001,0x002,0x004,
	0x080,0x080,0x080,0x080,0x080,
	0x001,0x002,
	0x012,0x02A,0x02A,0x02A,0x03C,
	0x03F,0x024,0x024,0x018,
	0x01C,0x022,0x022,0x022,
	0x018,0x024,0x024,0x03F,
	0x01C,0x02A,0x02A,0x02A,0x004,
	0x004,0x03F,0x005,
	0x09C,0x0A2,0x0A2,0x07E,
	0x03F,0x004,0x004,0x038,
	0x03D,
	0x080,0x27D,
	0x03F,0x008,0x014,0x022,
	0x03F,
	0x03E,0x002,0x03E,0x002,0x03C,
	0x03E,0x002,0x002,0x03C,
	0x01C,0x022,0x022,0x01C,
	0x0FE,0x022,0x022,0x01C,
	0x01C,0x022,0x022,0x0FE,
	0x03E,0x004,0x002,0x002,
	0x024,0x02A,0x02A,0x012,
	0x002,0x01F,0x022,
	0x01E,0x020,0x020,0x03E,
	0x006,0x018,0x020,0x018,0x006,
	0x01E,0x020,0x01E,0x020,0x01E,
	0x032,0x00C,0x00C,0x032,
	0x09E,0x0A0,0x0A0,0x07E,
	0x032,0x02A,0x026,0x022,
	0x010,0x06E,0x000,
	0x0FF,
	0x081,0x06E,0x010,
	0x008,0x004,0x004,0x008,0x004,
	0x03E,0x02B,0x02A,0x023,0x022,
	0x01C,0x02B,0x02A,0x02B,0x004,
	0x03E,0x009,0x009,0x03E,
	0x03F,0x025,0x025,0x025,0x019,
	0x03F,0x025,0x025,0x025,0x01A,
	0x03F,0x001,0x001,0x001,0x001,
	0x060,0x03E,0x021,0x021,0x03E,0x060,
	0x03F,0x025,0x025,0x021,0x021,
	0x031,0x00A,0x004,0x03F,0x004,0x00A,0x031,
	0x021,0x025,0x025,0x01A,
	0x03F,0x010,0x008,0x004,0x03F,
	0x03F,0x010,0x009,0x004,0x03F,
	0x03F,0x004,0x00A,0x031,
	0x020,0x03F,0x001,0x001,0x03F,
	0x03F,0x002,0x004,0x002,0x03F,
	0x03F,0x004,0x004,0x03F,
	0x00C,0x012,0x021,0x021,0x012,0x00C,
	0x03F,0x001,0x001,0x03F,
	0x03F,0x009,0x009,0x006,
	0x01E,0x021,0x021,0x021,
	0x001,0x001,0x03F,0x001,0x001,
	0x007,0x028,0x028,0x028,0x01F,
	0x00E,0x011,0x03F,0x011,0x00E,
	0x031,0x00A,0x004,0x00A,0x031,
	0x03F,0x020,0x020,0x03F,0x060,
	0x007,0x008,0x008,0x03F,
	0x03F,0x020,0x03F,0x020,0x03F,
	0x03F,0x020,0x03F,0x020,0x03F,0x060,
	0x001,0x03F,0x024,0x024,0x018,
	0x03F,0x024,0x024,0x018,0x03F,
	0x03F,0x024,0x024,0x018,
	0x012,0x021,0x025,0x025,0x01E,
	0x03F,0x008,0x01E,0x021,0x021,0x01E,
	0x036,0x009,0x009,0x009,0x03F,
	0x012,0x02A,0x02A,0x02A,0x03C,
	0x01E,0x025,0x025,0x019,
	0x03E,0x02A,0x02A,0x02A,0x014,
	0x03E,0x002,0x002,0x002,
	0x060,0x03C,0x022,0x03E,0x060,
	0x01C,0x02A,0x02A,0x02A,0x024,
	0x036,0x008,0x03E,0x008,0x036,
	0x022,0x02A,0x02A,0x014,
	0x03E,0x010,0x008,0x03E,
	0x03C,0x011,0x009,0x03C,
	0x03E,0x008,0x008,0x036,
	0x020,0x03E,0x002,0x002,0x03E,
	0x03E,0x004,0x008,0x004,0x03E,
	0x03E,0x008,0x008,0x03E,
	0x01C,0x022,0x022,0x01C,
	0x03E,0x002,0x002,0x03E,
	0x03E,0x012,0x012,0x00C,
	0x01C,0x022,0x022,0x022,
	0x03E,0x002,0x03E,0x002,0x03C,
	0x09E,0x0A0,0x0A0,0x07E,
	0x01C,0x022,0x07F,0x022,0x01C,
	0x032,0x00C,0x00C,0x032,
	0x03E,0x020,0x020,0x03E,0x060,
	0x00E,0x010,0x010,0x03E,
	0x03E,0x020,0x03E,0x020,0x03E,
	0x03E,0x020,0x03E,0x020,0x03E,0x060,
	0x002,0x03E,0x024,0x024,0x018,
	0x03E,0x024,0x024,0x018,0x03E,
	0x03E,0x024,0x024,0x018,
	0x014,0x022,0x02A,0x02A,0x01C,
	0x03E,0x008,0x01C,0x022,0x022,0x01C,
	0x02C,0x012,0x012,0x012,0x03E,
};
#ifndef _WINDOWS
static rom 
#else
static
#endif
    WORD Arial_B_Data[]={
    0x000,0x000,0x000,0x000,0x000,    
    0x0DF,0x0DE,
    0x00B,0x007,0x000,0x00B,0x007,
    0x0E4,0x03C,0x027,0x0E4,0x03C,0x027,
    0x09C,0x132,0x3FF,0x132,0x0E4,
    0x00F,0x009,0x0CF,0x030,0x00C,0x0F3,0x090,0x0F0,
    0x060,0x0F6,0x09F,0x099,0x0F7,0x0E6,0x0B0,
    0x00B,0x007,
    0x078,0x1FE,0x303,
    0x303,0x1FE,0x078,
    0x00A,0x007,0x00A,
    0x010,0x010,0x07C,0x010,0x010,
    0x2C0,0x1C0,
    0x010,0x010,0x010,
    0x0C0,0x0C0,
    0x0C0,0x03C,0x003,
    0x07E,0x0FF,0x081,0x0FF,0x07E,
    0x00C,0x006,0x0FF,0x0FF,
    0x0C2,0x0E3,0x0B1,0x09F,0x08E,
    0x042,0x0C3,0x089,0x0FF,0x076,
    0x030,0x02C,0x022,0x0FF,0x0FF,0x020,
    0x05C,0x0CF,0x08B,0x0F9,0x071,
    0x07E,0x0FF,0x089,0x0FB,0x072,
    0x001,0x0F1,0x0FD,0x00F,0x003,
    0x076,0x0FF,0x089,0x0FF,0x076,
    0x04E,0x0DF,0x091,0x0FF,0x07E,
    0x0CC,0x0CC,
    0x2CC,0x1CC,
    0x018,0x018,0x024,0x024,0x042,
    0x028,0x028,0x028,0x028,0x008,
    0x042,0x024,0x024,0x018,0x018,
    0x002,0x0D3,0x0D9,0x00F,0x006,
    0x078,0x186,0x172,0x28D,0x285,0x2C5,0x2FD,0x28D,0x142,0x13C,
    0x0E0,0x0FC,0x03F,0x023,0x03F,0x0FC,0x0E0,
    0x0FF,0x0FF,0x089,0x089,0x0FF,0x076,
    0x03C,0x07E,0x0C3,0x081,0x081,0x0C3,0x042,
    0x0FF,0x0FF,0x081,0x081,0x0FF,0x07E,
    0x0FF,0x0FF,0x089,0x089,0x089,0x081,
    0x0FF,0x0FF,0x009,0x009,0x009,0x001,
    0x03C,0x07E,0x0C3,0x081,0x091,0x0F3,0x072,
    0x0FF,0x0FF,0x008,0x008,0x0FF,0x0FF,
    0x0FF,0x0FF,
    0x040,0x0C0,0x080,0x0FF,0x07F,
    0x0FF,0x0FF,0x018,0x03E,0x0F7,0x0C1,
    0x0FF,0x0FF,0x080,0x080,0x080,0x080,
    0x0FF,0x0FF,0x00F,0x07C,0x0E0,0x07C,0x00F,0x0FF,0x0FF,
    0x0FF,0x0FF,0x00C,0x030,0x0FF,0x0FF,
    0x03C,0x07E,0x0C3,0x081,0x0C3,0x07E,0x03C,
    0x0FF,0x0FF,0x011,0x011,0x01F,0x00E,
    0x03C,0x07E,0x0C3,0x0A1,0x0C3,0x0FE,0x13C,0x100,
    0x0FF,0x0FF,0x011,0x031,0x07F,0x0CE,0x080,
    0x04E,0x0CF,0x099,0x099,0x0F3,0x072,
    0x001,0x001,0x0FF,0x0FF,0x001,0x001,
    0x07F,0x0FF,0x080,0x080,0x0FF,0x07F,
    0x003,0x03F,0x0FC,0x0C0,0x0FC,0x03F,0x003,
    0x003,0x03F,0x0FC,0x0E0,0x03E,0x007,0x03E,0x0E0,0x0FC,0x03F,0x003,
    0x0C3,0x0E7,0x03C,0x03C,0x0E7,0x0C3,
    0x003,0x00F,0x0F8,0x0F8,0x00F,0x003,
    0x0C1,0x0E1,0x0B1,0x099,0x08D,0x087,0x083,
    0x3FF,0x3FF,0x201,
    0x003,0x03C,0x0C0,
    0x201,0x3FF,0x3FF,
    0x008,0x00E,0x003,0x00E,0x008,
    0x200,0x200,0x200,0x200,0x200,0x200,
    0x001,0x003,0x002,
    0x068,0x0F4,0x094,0x0FC,0x0F8,
    0x0FF,0x0FF,0x084,0x084,0x0FC,0x078,
    0x078,0x0FC,0x084,0x0CC,0x048,
    0x078,0x0FC,0x084,0x084,0x0FF,0x0FF,
    0x078,0x0FC,0x094,0x094,0x0DC,0x058,
    0x004,0x0FE,0x0FF,0x005,0x001,
    0x178,0x2FC,0x284,0x284,0x3FC,0x1FC,
    0x0FF,0x0FF,0x004,0x004,0x0FC,0x0F8,
    0x0FD,0x0FD,
    0x200,0x3FD,0x1FD,
    0x0FF,0x0FF,0x030,0x078,0x0EC,0x084,
    0x0FE,0x0FF,
    0x0FC,0x0FC,0x004,0x004,0x0FC,0x0FC,0x004,0x004,0x0FC,0x0F8,
    0x0FC,0x0FC,0x004,0x004,0x0FC,0x0F8,
    0x078,0x0FC,0x084,0x084,0x0FC,0x078,
    0x3FC,0x3FC,0x084,0x084,0x0FC,0x078,
    0x078,0x0FC,0x084,0x084,0x3FC,0x3FC,
    0x0FC,0x0FC,0x008,0x004,0x004,0x004,
    0x058,0x0DC,0x0B4,0x0B4,0x0EC,0x068,
    0x004,0x07E,0x0FF,0x084,
    0x07C,0x0FC,0x080,0x080,0x0FC,0x0FC,
    0x03C,0x0FC,0x0C0,0x0FC,0x03C,
    0x01C,0x0FC,0x0E0,0x078,0x00C,0x078,0x0E0,0x0FC,0x01C,
    0x0CC,0x0FC,0x030,0x0FC,0x0CC,
    0x204,0x23C,0x3F8,0x1C0,0x0F8,0x03C,0x004,
    0x0C4,0x0E4,0x0B4,0x09C,0x08C,
    0x020,0x1FE,0x000,0x201,
    0x3FF,
    0x201,0x3DF,0x1FE,0x020,
    0x010,0x008,0x008,0x010,0x008,
    0x0FE,0x0FF,0x092,0x092,0x083,0x082,
    0x078,0x0FD,0x094,0x094,0x0DD,0x058,
    0x0E0,0x0FC,0x03F,0x023,0x03F,0x0FC,0x0E0,
    0x0FF,0x0FF,0x089,0x089,0x089,0x0F9,0x070,
    0x0FF,0x0FF,0x089,0x089,0x0FF,0x076,
    0x0FF,0x0FF,0x001,0x001,0x001,
    0x380,0x0FF,0x0FF,0x081,0x0FF,0x0FF,0x380,
    0x0FF,0x0FF,0x089,0x089,0x081,
    0x081,0x0F3,0x07E,0x008,0x0FF,0x0FF,0x008,0x07E,0x0F3,0x081,
    0x042,0x0C3,0x089,0x089,0x0FF,0x076,
    0x0FF,0x0FF,0x060,0x030,0x018,0x0FF,0x0FF,
    0x0FF,0x0FF,0x060,0x033,0x018,0x0FF,0x0FF,
    0x0FF,0x0FF,0x008,0x07E,0x0F3,0x081,
    0x080,0x0FF,0x07F,0x001,0x001,0x0FF,0x0FF,
    0x0FF,0x0FF,0x00F,0x07C,0x0E0,0x07C,0x00F,0x0FF,0x0FF,
    0x0FF,0x0FF,0x008,0x008,0x0FF,0x0FF,
    0x03C,0x07E,0x0C3,0x081,0x0C3,0x07E,0x03C,
    0x0FF,0x0FF,0x001,0x001,0x001,0x0FF,0x0FF,
    0x0FF,0x0FF,0x011,0x011,0x01F,0x00E,
    0x03C,0x07E,0x0C3,0x081,0x081,0x0C3,0x042,
    0x001,0x001,0x0FF,0x0FF,0x001,0x001,
    0x003,0x08F,0x0FC,0x078,0x01F,0x007,
    0x03C,0x07E,0x042,0x0FF,0x0FF,0x042,0x07E,0x03C,
    0x0C3,0x0E7,0x03C,0x03C,0x0E7,0x0C3,
    0x0FF,0x0FF,0x080,0x080,0x0FF,0x0FF,0x380,
    0x00F,0x01F,0x010,0x010,0x010,0x0FF,0x0FF,
    0x0FF,0x0FF,0x080,0x080,0x0FF,0x0FF,0x080,0x080,0x0FF,0x0FF,
    0x0FF,0x0FF,0x080,0x080,0x0FF,0x0FF,0x080,0x080,0x0FF,0x0FF,0x380,
    0x001,0x001,0x0FF,0x0FF,0x088,0x088,0x088,0x0F8,0x070,
    0x0FF,0x0FF,0x088,0x088,0x088,0x0F8,0x070,0x0FF,0x0FF,
    0x0FF,0x0FF,0x088,0x088,0x088,0x0F8,0x070,
    0x024,0x066,0x0C3,0x089,0x0CB,0x07E,0x03C,
    0x0FF,0x0FF,0x018,0x03C,0x07E,0x0C3,0x081,0x0C3,0x07E,0x03C,
    0x080,0x0CE,0x07F,0x031,0x011,0x0FF,0x0FF,
    0x068,0x0F4,0x094,0x0FC,0x0F8,
    0x07E,0x0FD,0x085,0x085,0x0FD,0x079,
    0x0FC,0x0FC,0x094,0x094,0x0FC,0x068,
    0x0FC,0x0FC,0x004,0x004,
    0x380,0x0C0,0x0FC,0x084,0x0FC,0x0FC,0x380,
    0x078,0x0FC,0x094,0x094,0x0DC,0x058,
    0x0C4,0x0EC,0x010,0x0FC,0x0FC,0x010,0x0EC,0x0C4,
    0x048,0x084,0x094,0x0FC,0x068,
    0x0FC,0x0FC,0x060,0x018,0x0FC,0x0FC,
    0x0FC,0x0FD,0x061,0x019,0x0FD,0x0FC,
    0x0FC,0x0FC,0x010,0x078,0x0EC,0x084,
    0x080,0x0FC,0x07C,0x004,0x0FC,0x0FC,
    0x0FC,0x0FC,0x030,0x0C0,0x030,0x0FC,0x0FC,
    0x0FC,0x0FC,0x010,0x010,0x0FC,0x0FC,
    0x078,0x0FC,0x084,0x084,0x0FC,0x078,
    0x0FC,0x0FC,0x004,0x004,0x0FC,0x0FC,
    0x3FC,0x3FC,0x084,0x084,0x0FC,0x078,
    0x078,0x0FC,0x084,0x0CC,0x048,
    0x004,0x0FC,0x0FC,0x004,
    0x204,0x23C,0x3F8,0x1C0,0x0F8,0x03C,0x004,
    0x078,0x0FC,0x084,0x3FF,0x3FF,0x084,0x0FC,0x078,
    0x0CC,0x0FC,0x030,0x0FC,0x0CC,
    0x0FC,0x0FC,0x080,0x080,0x0FC,0x0FC,0x380,
    0x01C,0x03C,0x020,0x0FC,0x0FC,
    0x0FC,0x0FC,0x080,0x0FC,0x0FC,0x080,0x0FC,0x0FC,
    0x0FC,0x0FC,0x080,0x0FC,0x0FC,0x080,0x0FC,0x0FC,0x380,
    0x004,0x0FC,0x0FC,0x090,0x0F0,0x060,
    0x0FC,0x0FC,0x090,0x0F0,0x060,0x0FC,0x0FC,
    0x0FC,0x0FC,0x090,0x0F0,0x060,
    0x048,0x0CC,0x094,0x0FC,0x078,
    0x0FC,0x0FC,0x010,0x078,0x0FC,0x084,0x0FC,0x078,
    0x080,0x0D8,0x07C,0x024,0x0FC,0x0FC,


};
#ifndef _WINDOWS
static rom 
#else
static
#endif
WORD Arial_cp1251[257] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  5,  6,  9, 15, 20, 28, 35, 36, 38, 40, 47, 50, 52, 55, 56,
     59, 63, 65, 69, 73, 78, 82, 86, 90, 94, 98, 99,101,104,108,111,
    114,122,126,130,134,138,142,146,150,154,157,161,165,169,174,178,
    184,188,194,198,202,207,211,216,225,230,235,241,243,246,248,253,
    258,260,265,269,273,277,282,285,289,293,294,296,300,301,306,310,
    314,318,322,326,330,333,337,342,347,351,355,359,362,363,366,371,
    371,371,371,371,371,371,371,371,371,371,371,371,371,371,371,371,
    371,371,371,371,371,371,371,371,371,371,371,371,371,371,371,371,
    371,371,371,371,371,371,371,371,371,376,376,376,376,376,376,376,
    376,376,376,376,376,376,376,376,376,381,381,381,381,381,381,381,
    381,385,390,395,400,406,411,418,422,427,432,436,441,446,450,456,
    460,464,468,473,478,483,488,493,497,502,508,513,518,522,527,533,
    538,543,547,552,556,561,566,571,575,579,583,587,592,597,601,605,
    609,613,617,622,626,631,635,640,644,649,655,660,665,669,674,680,685
};
#ifndef _WINDOWS
static rom 
#else
static
#endif
WORD Arial_B_cp1251[257]={
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  5,  7, 12, 18, 23, 31, 38, 40, 43, 46, 49, 54, 56, 59, 61,
     64, 69, 73, 78, 83, 89, 94, 99,104,109,114,116,118,123,128,133,
    138,148,155,161,168,174,180,186,193,199,201,206,212,218,227,233,
    240,246,254,261,267,273,279,286,297,303,309,316,319,322,325,330,
    336,339,344,350,355,361,367,372,378,384,386,389,395,397,407,413,
    419,425,431,437,443,447,453,458,467,472,479,484,488,489,493,498,
    498,498,498,498,498,498,498,498,498,498,498,498,498,498,498,498,
    498,498,498,498,498,498,498,498,498,498,498,498,498,498,498,498, 
    498,498,498,498,498,498,498,498,498,504,504,504,504,504,504,504,
    504,504,504,504,504,504,504,504,504,510,510,510,510,510,510,510,
    510,517,524,530,535,542,547,557,563,570,577,583,590,599,605,612,
    619,625,632,638,644,652,658,665,672,682,693,702,711,718,725,735,
    742,747,753,759,763,770,776,784,789,795,801,807,813,820,826,832,
    838,844,849,853,860,868,873,880,885,893,902,908,915,920,925,933,939  
};
#ifndef _WINDOWS
#pragma romdata
#endif
WORD* GetSymbolImage(BYTE symbol,WORD* OutWordCount,BYTE CFont){
    static WORD Image[12];
    BYTE i;        
    WORD Begin =  0;
    WORD End   =  0;    
    WORD Size =  0;
    switch(CFont){
        case 0:
            Begin = Arial_cp1251[symbol];
            End   = Arial_cp1251[(symbol+1)];    
        break;
        case 1:
            Begin = Arial_B_cp1251[symbol];
            End   = Arial_B_cp1251[(symbol+1)];    
        break;
        default:
            End   = 0;
            Begin = 0;
    }
    Size  =  End - Begin;
    //Image[0]=0;
    if(Size<1){
        OutWordCount[0] = 0;
        return 0;
    } 
     switch(CFont){
        case 0:
            for(i=0;i<Size;i++){
                Image[i]= Arial_Data[Begin+i];
            }
        break;
        case 1:
            for(i=0;i<Size;i++){
                Image[i]= Arial_B_Data[Begin+i];
            }
        break;
     }   
    OutWordCount[0] = Size;
    return Image;//&ArialData[Begin];    
}

