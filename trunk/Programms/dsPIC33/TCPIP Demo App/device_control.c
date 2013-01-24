#include "stdafx.h"
#include "device_control.h"

#ifdef __C30__
#include "TCPIP Stack/TCPIP.h"
#   define INTERRUPT void __attribute__((__interrupt__,__no_auto_psv__))
#   define DMAOFFSET(x) (WORD)__builtin_dmaoffset(x)
#endif
#ifdef _WIN32
#   define INTERRUPT void
#   define DMAOFFSET(x) (WORD)x
#   define Nop()
#endif

#ifdef _WIN32
WORD T1CON;
WORD T2CON;
WORD T3CON;
WORD T4CON;
WORD T5CON;
WORD T6CON;
WORD T7CON;
WORD T8CON;
WORD T9CON;

WORD TMR1;
WORD TMR2;
WORD TMR3;
WORD TMR4;
WORD TMR5;
WORD TMR6;
WORD TMR7;
WORD TMR8;
WORD TMR9;

WORD PR1;
WORD PR2;
WORD PR3;
WORD PR4;
WORD PR5;
WORD PR6;
WORD PR7;
WORD PR8;
WORD PR9;

WORD OC1CON;
WORD OC2CON;
WORD OC3CON;
WORD OC4CON;
WORD OC5CON;
WORD OC6CON;
WORD OC7CON;
WORD OC8CON;

WORD OC1R;
WORD OC2R;
WORD OC3R;
WORD OC4R;
WORD OC5R;
WORD OC6R;
WORD OC7R;
WORD OC8R;

WORD OC1RS;
WORD OC2RS;
WORD OC3RS;
WORD OC4RS;
WORD OC5RS;
WORD OC6RS;
WORD OC7RS;
WORD OC8RS;

WORD DMA0CON;
WORD DMA1CON;
WORD DMA2CON;
WORD DMA3CON;
WORD DMA4CON;
WORD DMA5CON;
WORD DMA6CON;
WORD DMA7CON;

WORD DMA0REQ;
WORD DMA1REQ;
WORD DMA2REQ;
WORD DMA3REQ;
WORD DMA4REQ;
WORD DMA5REQ;
WORD DMA6REQ;
WORD DMA7REQ;

WORD DMA0PAD;
WORD DMA1PAD;
WORD DMA2PAD;
WORD DMA3PAD;
WORD DMA4PAD;
WORD DMA5PAD;
WORD DMA6PAD;
WORD DMA7PAD;

WORD DMA0STA;
WORD DMA1STA;
WORD DMA2STA;
WORD DMA3STA;
WORD DMA4STA;
WORD DMA5STA;
WORD DMA6STA;
WORD DMA7STA;

WORD DMA0STB;
WORD DMA1STB;
WORD DMA2STB;
WORD DMA3STB;
WORD DMA4STB;
WORD DMA5STB;
WORD DMA6STB;
WORD DMA7STB;

WORD DMA0CNT;
WORD DMA1CNT;
WORD DMA2CNT;
WORD DMA3CNT;
WORD DMA4CNT;
WORD DMA5CNT;
WORD DMA6CNT;
WORD DMA7CNT;

WORD SPI1BUF;
WORD SPI2BUF;

typedef struct 
{
    __EXTENSION BYTE SPIRBF:1;
    __EXTENSION BYTE SPITBF:1;
    __EXTENSION BYTE b2:4;
    __EXTENSION BYTE SPIROV:1;
    __EXTENSION BYTE b7:6;
    __EXTENSION BYTE SPISIDL:1;
    __EXTENSION BYTE b15:1;
    __EXTENSION BYTE SPIEN:1;
} SPISTATbits;

typedef struct 
{
    __EXTENSION BYTE PPRE:2;
    __EXTENSION BYTE SPRE:3;
    __EXTENSION BYTE MSTEN:1;
    __EXTENSION BYTE CKP:1;
    __EXTENSION BYTE SSEN:1;
    __EXTENSION BYTE CKE:1;
    __EXTENSION BYTE SMP:1;
    __EXTENSION BYTE MODE16:1;
    __EXTENSION BYTE DISSDO:1;
    __EXTENSION BYTE DISSCK:1;
    __EXTENSION BYTE b13:1;    
    __EXTENSION BYTE b14:1;
    __EXTENSION BYTE b15:1;
} SPICON1bits;

WORD SPI1STAT;
WORD SPI2STAT;
WORD SPI1CON1;
WORD SPI2CON1;
SPISTATbits SPI1STATbits;
SPISTATbits SPI2STATbits;
SPICON1bits SPI1CON1bits;
SPICON1bits SPI2CON1bits;

typedef struct 
{
    __EXTENSION BYTE b0:1;
    __EXTENSION BYTE TCS:1;
    __EXTENSION BYTE TSYNC:1;
    __EXTENSION BYTE T32:1;
    __EXTENSION BYTE TCKPS:2;
    __EXTENSION BYTE TGATE:1;
    __EXTENSION BYTE b7:1;
    __EXTENSION BYTE b8:1;
    __EXTENSION BYTE b9:1;
    __EXTENSION BYTE b10:1;
    __EXTENSION BYTE b11:1;
    __EXTENSION BYTE b12:1;
    __EXTENSION BYTE TSIDL:1;
    __EXTENSION BYTE b14:1;
    __EXTENSION BYTE TON:1;
} TIMER_CONbits;

TIMER_CONbits T1CONbits;
TIMER_CONbits T2CONbits;
TIMER_CONbits T3CONbits;
TIMER_CONbits T4CONbits;
TIMER_CONbits T5CONbits;
TIMER_CONbits T6CONbits;
TIMER_CONbits T7CONbits;
TIMER_CONbits T8CONbits;
TIMER_CONbits T9CONbits;
typedef struct 
{
    __EXTENSION BYTE OCM:3;
    __EXTENSION BYTE OCTSEL:1;
    __EXTENSION BYTE OCFLT:1;
    __EXTENSION BYTE b4:8;
    __EXTENSION BYTE OCSIDL:1;
    __EXTENSION BYTE b14:2;
} OC_CONbits;
OC_CONbits OC1CONbits;
OC_CONbits OC2CONbits;
OC_CONbits OC3CONbits;
OC_CONbits OC4CONbits;
OC_CONbits OC5CONbits;
OC_CONbits OC6CONbits;
OC_CONbits OC7CONbits;
OC_CONbits OC8CONbits;

typedef struct 
{
    __EXTENSION BYTE MODE:2;
    __EXTENSION BYTE b2:2;
    __EXTENSION BYTE AMODE:2;
    __EXTENSION BYTE b6:5;
    __EXTENSION BYTE NULLW:1;
    __EXTENSION BYTE HALF:1;
    __EXTENSION BYTE DIR:1;
    __EXTENSION BYTE SIZE:1;
    __EXTENSION BYTE CHEN:1;
} DMA_CONbits;
DMA_CONbits DMA0CONbits;
DMA_CONbits DMA1CONbits;
DMA_CONbits DMA2CONbits;
DMA_CONbits DMA3CONbits;
DMA_CONbits DMA4CONbits;
DMA_CONbits DMA5CONbits;
DMA_CONbits DMA6CONbits;
DMA_CONbits DMA7CONbits;

typedef struct 
{
    __EXTENSION BYTE IRQSEL0:1;
    __EXTENSION BYTE IRQSEL1:1;
    __EXTENSION BYTE IRQSEL2:1;
    __EXTENSION BYTE IRQSEL3:1;
    __EXTENSION BYTE IRQSEL4:1;
    __EXTENSION BYTE IRQSEL5:1;
    __EXTENSION BYTE IRQSEL6:1;
    __EXTENSION BYTE b7:8;
    __EXTENSION BYTE FORCE:1;
} DMA_REQbits;

DMA_REQbits DMA0REQbits;
DMA_REQbits DMA1REQbits;
DMA_REQbits DMA2REQbits;
DMA_REQbits DMA3REQbits;
DMA_REQbits DMA4REQbits;
DMA_REQbits DMA5REQbits;
DMA_REQbits DMA6REQbits;
DMA_REQbits DMA7REQbits;

struct 
{
    __EXTENSION BYTE PPST0:1;
    __EXTENSION BYTE PPST1:1;
    __EXTENSION BYTE PPST2:1;
    __EXTENSION BYTE PPST3:1;
    __EXTENSION BYTE PPST4:1;
    __EXTENSION BYTE PPST5:1;
    __EXTENSION BYTE PPST6:1;
    __EXTENSION BYTE PPST7:1;
    __EXTENSION BYTE LSTCH:4;
    __EXTENSION BYTE b12:4;
} DMACS1bits;

struct 
{
    __EXTENSION BYTE INT0IF:1;
    __EXTENSION BYTE IC1IF:1;
    __EXTENSION BYTE OC1IF:1;
    __EXTENSION BYTE T1IF:1;
    __EXTENSION BYTE DMA0IF:1;
    __EXTENSION BYTE IC2IF:1;
    __EXTENSION BYTE OC2IF:1;
    __EXTENSION BYTE T2IF:1;
    __EXTENSION BYTE T3IF:1;
    __EXTENSION BYTE SPI1EIF:1;
    __EXTENSION BYTE SPI1IF:1;
    __EXTENSION BYTE U1RXIF:1;
    __EXTENSION BYTE U1TXIF:1;
    __EXTENSION BYTE AD1IF:1;
    __EXTENSION BYTE DMA1IF:1;
    __EXTENSION BYTE b16:1;
} IFS0bits;
struct 
{
    __EXTENSION BYTE SI2C1IF:1;
    __EXTENSION BYTE MI2C1IF:1;
    __EXTENSION BYTE b2:1;
    __EXTENSION BYTE CNIIF:1;
    __EXTENSION BYTE INT1IF:1;
    __EXTENSION BYTE AD2IF:1;
    __EXTENSION BYTE IC7IF:1;
    __EXTENSION BYTE IC8IF:1;
    __EXTENSION BYTE DMA2IF:1;
    __EXTENSION BYTE OC3IF:1;
    __EXTENSION BYTE OC4IF:1;
    __EXTENSION BYTE T4IF:1;
    __EXTENSION BYTE T5IF:1;
    __EXTENSION BYTE INT2IF:1;
    __EXTENSION BYTE U2RXIF:1;
    __EXTENSION BYTE U2TXIF:1;    
} IFS1bits;
struct 
{
    __EXTENSION BYTE SPI2EIF:1;
    __EXTENSION BYTE SPI2IF:1;
    __EXTENSION BYTE C1RXIF:1;
    __EXTENSION BYTE C1IF:1;
    __EXTENSION BYTE DMA3IF:1;
    __EXTENSION BYTE IC3IF:1;
    __EXTENSION BYTE IC4IF:1;
    __EXTENSION BYTE IC5IF:1;
    __EXTENSION BYTE IC6IF:1;
    __EXTENSION BYTE OC5IF:1;
    __EXTENSION BYTE OC6IF:1;
    __EXTENSION BYTE OC7IF:1;
    __EXTENSION BYTE OC8IF:1;
    __EXTENSION BYTE b13:1;
    __EXTENSION BYTE DMA4IF:1;
    __EXTENSION BYTE T6IF:1;
} IFS2bits;
struct 
{
    __EXTENSION BYTE T7IF:1;
    __EXTENSION BYTE SI2C2IF:1;
    __EXTENSION BYTE MI2C2IF:1;
    __EXTENSION BYTE T8IF:1;
    __EXTENSION BYTE T9IF:1;
    __EXTENSION BYTE INT3IF:1;
    __EXTENSION BYTE INT4IF:1;
    __EXTENSION BYTE C2RXIF:1;
    __EXTENSION BYTE C2IF:1;
    __EXTENSION BYTE b9:1;
    __EXTENSION BYTE b10:1;
    __EXTENSION BYTE DCIEIF:1;
    __EXTENSION BYTE DCIIF:1;
    __EXTENSION BYTE DMA5IF:1;
    __EXTENSION BYTE b14:1;
    __EXTENSION BYTE b15:1;
}IFS3bits;
struct 
{
    __EXTENSION BYTE b0:1;
    __EXTENSION BYTE U1EIF:1;
    __EXTENSION BYTE U2EIF:1;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE DMA6IF:1;
    __EXTENSION BYTE DMA7IF:1;
    __EXTENSION BYTE C1TXIF:1;
    __EXTENSION BYTE C2TXIF:1;
    __EXTENSION BYTE b8:1;
    __EXTENSION BYTE b9:1;
    __EXTENSION BYTE b10:1;
    __EXTENSION BYTE b11:1;
    __EXTENSION BYTE b12:1;
    __EXTENSION BYTE b13:1;
    __EXTENSION BYTE b14:1;
    __EXTENSION BYTE b15:1;
}IFS4bits;

struct 
{
    __EXTENSION BYTE INT0IE:1;
    __EXTENSION BYTE IC1IE:1;
    __EXTENSION BYTE OC1IE:1;
    __EXTENSION BYTE T1IE:1;
    __EXTENSION BYTE DMA0IE:1;
    __EXTENSION BYTE IC2IE:1;
    __EXTENSION BYTE OC2IE:1;
    __EXTENSION BYTE T2IE:1;
    __EXTENSION BYTE T3IE:1;
    __EXTENSION BYTE SPI1EIE:1;
    __EXTENSION BYTE SPI1IE:1;
    __EXTENSION BYTE U1RXIE:1;
    __EXTENSION BYTE U1TXIE:1;
    __EXTENSION BYTE AD1IE:1;
    __EXTENSION BYTE DMA1IE:1;
    __EXTENSION BYTE b15:1;
} IEC0bits;
struct 
{
    __EXTENSION BYTE SI2C1IE:1;
    __EXTENSION BYTE MI2C1IE:1;
    __EXTENSION BYTE b2:1;
    __EXTENSION BYTE CNIIE:1;
    __EXTENSION BYTE INT1IE:1;
    __EXTENSION BYTE AD2IE:1;
    __EXTENSION BYTE IC7IE:1;
    __EXTENSION BYTE IC8IE:1;
    __EXTENSION BYTE DMA2IE:1;
    __EXTENSION BYTE OC3IE:1;
    __EXTENSION BYTE OC4IE:1;
    __EXTENSION BYTE T4IE:1;
    __EXTENSION BYTE T5IE:1;
    __EXTENSION BYTE INT2IE:1;
    __EXTENSION BYTE U2RXIE:1;
    __EXTENSION BYTE U2TXIE:1;    
} IEC1bits;
struct 
{
    __EXTENSION BYTE SPI2EIE:1;
    __EXTENSION BYTE SPI2IE:1;
    __EXTENSION BYTE C1RXIE:1;
    __EXTENSION BYTE C1IE:1;
    __EXTENSION BYTE DMA3IE:1;
    __EXTENSION BYTE IC3IE:1;
    __EXTENSION BYTE IC4IE:1;
    __EXTENSION BYTE IC5IE:1;
    __EXTENSION BYTE IC6IE:1;
    __EXTENSION BYTE OC5IE:1;
    __EXTENSION BYTE OC6IE:1;
    __EXTENSION BYTE OC7IE:1;
    __EXTENSION BYTE OC8IE:1;
    __EXTENSION BYTE b13:1;
    __EXTENSION BYTE DMA4IE:1;
    __EXTENSION BYTE T6IE:1;
} IEC2bits;
struct 
{
    __EXTENSION BYTE T7IE:1;
    __EXTENSION BYTE SI2C2IE:1;
    __EXTENSION BYTE MI2C2IE:1;
    __EXTENSION BYTE T8IE:1;
    __EXTENSION BYTE T9IE:1;
    __EXTENSION BYTE INT3IE:1;
    __EXTENSION BYTE INT4IE:1;
    __EXTENSION BYTE C2RXIE:1;
    __EXTENSION BYTE C2IE:1;
    __EXTENSION BYTE b9:1;
    __EXTENSION BYTE b10:1;
    __EXTENSION BYTE DCIEIE:1;
    __EXTENSION BYTE DCIIE:1;
    __EXTENSION BYTE DMA5IE:1;
    __EXTENSION BYTE b14:1;
    __EXTENSION BYTE b15:1;
}IEC3bits;
struct 
{
    __EXTENSION BYTE b0:1;
    __EXTENSION BYTE U1EIE:1;
    __EXTENSION BYTE U2EIE:1;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE DMA6IE:1;
    __EXTENSION BYTE DMA7IE:1;
    __EXTENSION BYTE C1TXIE:1;
    __EXTENSION BYTE C2TXIE:1;
    __EXTENSION BYTE b8:1;
    __EXTENSION BYTE b9:1;
    __EXTENSION BYTE b10:1;
    __EXTENSION BYTE b11:1;
    __EXTENSION BYTE b12:1;
    __EXTENSION BYTE b13:1;
    __EXTENSION BYTE b14:1;
    __EXTENSION BYTE b15:1;
}IEC4bits;
struct _IPC0bits
{
    __EXTENSION BYTE INT0IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE IC1IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE OC1IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE T1IP:3;
    __EXTENSION BYTE b7:1;
} IPC0bits;
struct _IPC1bits
{
    __EXTENSION BYTE DMA0IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE IC2IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE OC2IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE T2IP:3;
    __EXTENSION BYTE b7:1;
} IPC1bits;
struct _IPC2bits
{
    __EXTENSION BYTE T3IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE SPI1EIP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE SPI1IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE U1RXIP:3;
    __EXTENSION BYTE b7:1;
} IPC2bits;
struct _IPC3bits
{
    __EXTENSION BYTE U1TXIP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE AD1IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE DMA1IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE b6:4;
} IPC3bits;
struct _IPC4bits
{
    __EXTENSION BYTE SI2C1IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE MI2C1IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE b5:4;
    __EXTENSION BYTE U1RXIP:3;
    __EXTENSION BYTE b7:1;
} IPC4bits;
struct _IPC5bits
{
    __EXTENSION BYTE INT1IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE AD2IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE IC7IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE IC8IP:3;
    __EXTENSION BYTE b7:1;
} IPC5bits;
struct _IPC6bits
{
    __EXTENSION BYTE DMA2IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE OC3IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE OC4IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE T4IP:3;
    __EXTENSION BYTE b7:1;
} IPC6bits;
struct _IPC7bits
{
    __EXTENSION BYTE T5IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE INT2IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE U2RXIP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE U2TXIP:3;
    __EXTENSION BYTE b7:1;
} IPC7bits;
struct _IPC8bits
{
    __EXTENSION BYTE SPI2EIP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE SPI2IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE C1RXIP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE C1IP:3;
    __EXTENSION BYTE b7:1;
} IPC8bits;
struct _IPC9bits
{
    __EXTENSION BYTE DMA3IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE IC3IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE IC4IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE IC5IP:3;
    __EXTENSION BYTE b7:1;
} IPC9bits;
struct _IPC10bits
{
    __EXTENSION BYTE IC6IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE OC5IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE OC6IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE OC7IP:3;
    __EXTENSION BYTE b7:1;
} IPC10bits;
struct _IPC11bits
{
    __EXTENSION BYTE OC8IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE b2:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE DMA4IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE T6IP:3;
    __EXTENSION BYTE b7:1;
} IPC11bits;
struct _IPC12bits
{
    __EXTENSION BYTE T7IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE SI2C2IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE MI2C2IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE T8IP:3;
    __EXTENSION BYTE b7:1;
} IPC12bits;
struct _IPC13bits
{
    __EXTENSION BYTE T9IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE INT3IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE INT4IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE C2RXIP:3;
    __EXTENSION BYTE b7:1;
} IPC13bits;
struct _IPC14bits
{
    __EXTENSION BYTE C2IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE b2:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE b4IP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE DCEIIP:3;
    __EXTENSION BYTE b7:1;
} IPC14bits;
struct _IPC15bits
{
    __EXTENSION BYTE DCIIP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE DMA5IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE b4:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE b6:3;
    __EXTENSION BYTE b7:1;
} IPC15bits;
struct _IPC16bits
{
    __EXTENSION BYTE b0:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE U1EIP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE U2EIP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE b6:3;
    __EXTENSION BYTE b7:1;
} IPC16bits;
struct _IPC17bits
{
    __EXTENSION BYTE DMA6IP:3;
    __EXTENSION BYTE b1:1;
    __EXTENSION BYTE DMA7IP:3;
    __EXTENSION BYTE b3:1;
    __EXTENSION BYTE C1TXIP:3;
    __EXTENSION BYTE b5:1;
    __EXTENSION BYTE C2TXIP:3;
    __EXTENSION BYTE b7:1;
} IPC17bits;

struct _SRbits
    {
    __EXTENSION BYTE C:1;
    __EXTENSION BYTE Z:1;
    __EXTENSION BYTE OV:1;
    __EXTENSION BYTE N:1;
    __EXTENSION BYTE RA:1;
    __EXTENSION BYTE IPL:3;

    __EXTENSION BYTE DC:1;
    __EXTENSION BYTE DA:1;
    __EXTENSION BYTE SAB:1;
    __EXTENSION BYTE OAB:1;
    __EXTENSION BYTE SB:1;
    __EXTENSION BYTE SA:1;
    __EXTENSION BYTE OB:1;
    __EXTENSION BYTE OA:1;
    } SRbits;
#endif //_WIN32


/************************************************************************************************
                                 |========| ==== ==\     /== =======| ======\
                                 ||  ||  ||  ||   ||\   /||   ||   ||  ||   ||
                                     ||      ||   ||\\ //||   || |     ||   ||
                                     ||      ||   || \V/ ||   ||=|     ||===<
                                     ||      ||   ||  V  ||   || |     ||   \\
                                     ||      ||   ||     ||   ||   ||  ||   ||
                                    ====    ==== ====   ==== =======| ==|   \==
************************************************************************************************/
TMRConfigType TMRConfig[9];
//------------------------------------------------------------------------------------------------
int TimerInit(TIMERS_ID id, TMR_CLOCK_SOURCE source, TMR_GATED_MODE gated, TMR_PRESCALER pre, SYS_IDLE idle, TMR_BIT_MODE bit, TMR1_SYNC sync)
//------------------------------------------------------------------------------------------------
{
    WORD Config = 0;
    Config |= ((WORD)idle)   << 13;
    Config |= ((WORD)gated)  << 6;
    Config |= ((WORD)pre)    << 4;
    Config |= ((WORD)bit)    << 3;
    Config |= ((WORD)sync)   << 2;
    Config |= ((WORD)source) << 1;

    switch(id){
        case TIMER1: T1CON = Config;
            break;
        case TIMER2: T2CON = Config;
            break;
        case TIMER3: T3CON = Config;
            break;
        case TIMER4: T4CON = Config;
            break;
        case TIMER5: T5CON = Config;
            break;
        case TIMER6: T6CON = Config;
            break;
        case TIMER7: T7CON = Config;
            break;
        case TIMER8: T8CON = Config;
            break;
        case TIMER9: T9CON = Config;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int TimerSetInt(TIMERS_ID id, BYTE Level, BOOL enabled)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case TIMER1:
            IPC0bits.T1IP = Level & 0x07;     // Set Timer1 Interrupt Priority Level
            IFS0bits.T1IF = 0;         // Clear Timer1 Interrupt Flag
            IEC0bits.T1IE = enabled;   // Enable Timer1 interrupt
            break;
        case TIMER2:
            IPC1bits.T2IP = Level & 0x07;     // Set Timer2 Interrupt Priority Level
            IFS0bits.T2IF = 0;         // Clear Timer2 Interrupt Flag
            IEC0bits.T2IE = enabled;   // Enable Timer2 interrupt
            break;
        case TIMER3:
            IPC2bits.T3IP = Level & 0x07;     // Set Timer3 Interrupt Priority Level
            IFS0bits.T3IF = 0;         // Clear Timer3 Interrupt Flag
            IEC0bits.T3IE = enabled;   // Enable Timer3 interrupt
            break;
        case TIMER4:
            IPC6bits.T4IP = Level & 0x07;     // Set Timer4 Interrupt Priority Level
            IFS1bits.T4IF = 0;         // Clear Timer4 Interrupt Flag
            IEC1bits.T4IE = enabled;   // Enable Timer4 interrupt
            break;
        case TIMER5:
            IPC7bits.T5IP = Level & 0x07;     // Set Timer5 Interrupt Priority Level
            IFS1bits.T5IF = 0;         // Clear Timer5 Interrupt Flag
            IEC1bits.T5IE = enabled;   // Enable Timer5 interrupt
            break;
        case TIMER6:
            IPC11bits.T6IP = Level & 0x07;     // Set Timer6 Interrupt Priority Level
            IFS2bits.T6IF = 0;         // Clear Timer6 Interrupt Flag
            IEC2bits.T6IE = enabled;   // Enable Timer6 interrupt
            break;
        case TIMER7:
            IPC12bits.T7IP = Level & 0x07;     // Set Timer7 Interrupt Priority Level
            IFS3bits.T7IF = 0;         // Clear Timer7 Interrupt Flag
            IEC3bits.T7IE = enabled;   // Enable Timer7 interrupt
            break;
        case TIMER8:
            IPC12bits.T8IP = Level & 0x07;     // Set Timer8 Interrupt Priority Level
            IFS3bits.T8IF = 0;         // Clear Timer8 Interrupt Flag
            IEC3bits.T8IE = enabled;   // Enable Timer8 interrupt
            break;
        case TIMER9:
            IPC13bits.T9IP = Level & 0x07;     // Set Timer9 Interrupt Priority Level
            IFS3bits.T9IF = 0;         // Clear Timer9 Interrupt Flag
            IEC3bits.T9IE = enabled;   // Enable Timer9 interrupt
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int TimerSetValue(TIMERS_ID id, WORD TmrValue, WORD PRValue)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case TIMER1:
            TMR1 = TmrValue;
            PR1  = PRValue;
            break;
        case TIMER2:
            TMR2 = TmrValue;
            PR2  = PRValue;
            break;
        case TIMER3:
            TMR3 = TmrValue;
            PR3  = PRValue;
            break;
        case TIMER4:
            TMR4 = TmrValue;
            PR4  = PRValue;
            break;
        case TIMER5:
            TMR5 = TmrValue;
            PR5  = PRValue;
            break;
        case TIMER6:
            TMR6 = TmrValue;
            PR6  = PRValue;
            break;
        case TIMER7:
            TMR7 = TmrValue;
            PR7  = PRValue;
            break;
        case TIMER8:
            TMR8 = TmrValue;
            PR8  = PRValue;
            break;
        case TIMER9:
            TMR9 = TmrValue;
            PR9  = PRValue;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
void TimerSetState(TIMERS_ID id, BOOL enabled)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case TIMER1: T1CONbits.TON = enabled;
            break;
        case TIMER2: T2CONbits.TON = enabled;
            break;
        case TIMER3: T3CONbits.TON = enabled;
            break;
        case TIMER4: T4CONbits.TON = enabled;
            break;
        case TIMER5: T5CONbits.TON = enabled;
            break;
        case TIMER6: T6CONbits.TON = enabled;
            break;
        case TIMER7: T7CONbits.TON = enabled;
            break;
        case TIMER8: T8CONbits.TON = enabled;
            break;
        case TIMER9: T9CONbits.TON = enabled;
            break;        
    }
}
//------------------------------------------------------------------------------------------------
int TimerSetCallback(TIMERS_ID id, int (*CallbackFunc)(void))
//------------------------------------------------------------------------------------------------
{
    TMRConfig[id].CallbackFunc = CallbackFunc;
    return 0;
}
/*
//------------------------------------------------------------------------------------------------
INTERRUPT _T1Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER1;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS0bits.T1IF = 0; // Clear TIMER1 interrupt flag
}
*/
//------------------------------------------------------------------------------------------------
INTERRUPT _T2Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER2;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS0bits.T2IF = 0; // Clear TIMER2 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _T3Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER3;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS0bits.T3IF = 0; // Clear TIMER3 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _T4Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER4;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS1bits.T4IF = 0; // Clear TIMER4 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _T5Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER5;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS1bits.T5IF = 0; // Clear TIMER5 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _T6Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER6;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS2bits.T6IF = 0; // Clear TIMER6 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _T7Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER7;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS3bits.T7IF = 0; // Clear TIMER7 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _T8Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER8;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS3bits.T8IF = 0; // Clear TIMER8 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _T9Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = TIMER9;
    if(TMRConfig[id].CallbackFunc){
        TMRConfig[id].CallbackFunc();
    }
    IFS3bits.T9IF = 0; // Clear TIMER9 interrupt flag
}
//************************************************************************************************
//
//                              Direct Memory Access (DMA)
//
//************************************************************************************************

// управляющие переменные
DMAConfigType DMAConfig[8];
#ifdef __C30__
    //unsigned int DMABuffer[1024]  __attribute__((space(dma)));
    BYTE DMA0BufferA[DMA0BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA0BufferB[DMA0BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA1BufferA[DMA1BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA1BufferB[DMA1BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA2BufferA[DMA2BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA2BufferB[DMA2BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA3BufferA[DMA3BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA3BufferB[DMA3BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA4BufferA[DMA4BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA4BufferB[DMA4BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA5BufferA[DMA5BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA5BufferB[DMA5BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA6BufferA[DMA6BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA6BufferB[DMA6BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA7BufferA[DMA7BUF_SIZE] __attribute__((space(dma)));
    BYTE DMA7BufferB[DMA7BUF_SIZE] __attribute__((space(dma)));
#else
    //unsigned int DMABuffer[1024];
    BYTE DMA0BufferA[DMA0BUF_SIZE];
    BYTE DMA0BufferB[DMA0BUF_SIZE];
    BYTE DMA1BufferA[DMA1BUF_SIZE];
    BYTE DMA1BufferB[DMA1BUF_SIZE];
    BYTE *DMA2BufferA;//[DMA2BUF_SIZE];
    BYTE *DMA2BufferB;//[DMA2BUF_SIZE];
    BYTE *DMA3BufferA;//[DMA3BUF_SIZE];
    BYTE *DMA3BufferB;//[DMA3BUF_SIZE];
    BYTE *DMA4BufferA;//[DMA4BUF_SIZE];
    BYTE *DMA4BufferB;//[DMA4BUF_SIZE];
    BYTE *DMA5BufferA;//[DMA5BUF_SIZE];
    BYTE *DMA5BufferB;//[DMA5BUF_SIZE];
    BYTE DMA6BufferA[DMA6BUF_SIZE];
    BYTE DMA6BufferB[DMA6BUF_SIZE];
    BYTE DMA7BufferA[DMA7BUF_SIZE];
    BYTE DMA7BufferB[DMA7BUF_SIZE];

#endif
static int BufferBusySize = 0;
//------------------------------------------------------------------------------------------------
WORD DMACreateConfig(DMA_DATA_SIZE_BIT size, DMA_TRANSFER_DIRECTION dir, DMA_COMPLETE_BLOCK_INT half, DMA_NULL_DATA_MODE nullw, DMA_ADRESING_MODE addr, DMA_OPERATION_MODE mode)
//------------------------------------------------------------------------------------------------
{
    WORD Config = 0;//SIZE_BYTE, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, ONE_SHOT
    Config |= ((WORD)size)  << 14;  // 1 
    Config |= ((WORD)dir)   << 13;  // 1
    Config |= ((WORD)half)  << 12;  // 0
    Config |= ((WORD)nullw) << 11;  // 0
    Config |= ((WORD)addr)  << 4;   // 0
    Config |= ((WORD)mode)  << 0;   // 1
    return Config;
}
//------------------------------------------------------------------------------------------------
int DMAInit(DMA_ID id, WORD Config)
//------------------------------------------------------------------------------------------------
{
    
    switch(id){
        case DMA0: 
            DMA0CON = Config;
            DMAConfig[id].BufA = DMA0BufferA;
            DMAConfig[id].BufB = DMA0BufferB;
            DMA0STA = DMAOFFSET(DMA0BufferA);
            DMA0STB = DMAOFFSET(DMA0BufferB);
            break;
        case DMA1: 
            DMA1CON = Config;
            DMAConfig[id].BufA = DMA1BufferA;
            DMAConfig[id].BufB = DMA1BufferB;
            DMA1STA = DMAOFFSET(DMA1BufferA);
            DMA1STB = DMAOFFSET(DMA1BufferB);
            break;
        case DMA2: 
            DMA2CON = Config;
            DMAConfig[id].BufA = DMA2BufferA;
            DMAConfig[id].BufB = DMA2BufferB;
            DMA2STA = DMAOFFSET(DMA2BufferA);
            DMA2STB = DMAOFFSET(DMA2BufferB);
            break;
        case DMA3: 
            DMA3CON = Config;
            DMAConfig[id].BufA = DMA3BufferA;
            DMAConfig[id].BufB = DMA3BufferB;
            DMA3STA = DMAOFFSET(DMA3BufferA);
            DMA3STB = DMAOFFSET(DMA3BufferB);
            break;
        case DMA4: 
            DMA4CON = Config;
            DMAConfig[id].BufA = DMA4BufferA;
            DMAConfig[id].BufB = DMA4BufferB;
            DMA4STA = DMAOFFSET(DMA4BufferA);
            DMA4STB = DMAOFFSET(DMA4BufferB);
            break;
        case DMA5: 
            DMA5CON = Config;
            DMAConfig[id].BufA = DMA5BufferA;
            DMAConfig[id].BufB = DMA5BufferB;
            DMA5STA = DMAOFFSET(DMA5BufferA);
            DMA5STB = DMAOFFSET(DMA5BufferB);
            break;
        case DMA6: 
            DMA6CON = Config;
            DMAConfig[id].BufA = DMA6BufferA;
            DMAConfig[id].BufB = DMA6BufferB;
            DMA6STA = DMAOFFSET(DMA6BufferA);
            DMA6STB = DMAOFFSET(DMA6BufferB);
            break;
        case DMA7: 
            DMA7CON = Config;
            DMAConfig[id].BufA = DMA7BufferA;
            DMAConfig[id].BufB = DMA7BufferB;
            DMA7STA = DMAOFFSET(DMA7BufferA);
            DMA7STB = DMAOFFSET(DMA7BufferB);
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int DMASetConfig(DMA_ID id, WORD Config)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case DMA0: 
            DMA0CON = Config;
            break;
        case DMA1: 
            DMA1CON = Config;
            break;
        case DMA2: 
            DMA2CON = Config;
            break;
        case DMA3: 
            DMA3CON = Config;
            break;
        case DMA4: 
            DMA4CON = Config;
            break;
        case DMA5: 
            DMA5CON = Config;
            break;
        case DMA6: 
            DMA6CON = Config;
            break;
        case DMA7: 
            DMA7CON = Config;
            break;
        default:
            return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int DMASelectDevice(DMA_ID id, DMA_DEVICE_IRQ irq, int DEVICE_REG)
//------------------------------------------------------------------------------------------------
{    
    switch(id){
        case DMA0:
            DMA0REQ = (BYTE)irq;
            DMA0PAD = DEVICE_REG;
            break;
        case DMA1:
            DMA1REQ = (BYTE)irq;
            DMA1PAD = DEVICE_REG;
            break;
        case DMA2:
            DMA2REQ = (BYTE)irq;
            DMA2PAD = DEVICE_REG;
            break;
        case DMA3:
            DMA3REQ = (BYTE)irq;
            DMA3PAD = DEVICE_REG;
            break;
        case DMA4:
            DMA4REQ = (BYTE)irq;
            DMA4PAD = DEVICE_REG;
            break;
        case DMA5:
            DMA5REQ = (BYTE)irq;
            DMA5PAD = DEVICE_REG;
            break;
        case DMA6:
            DMA6REQ = (BYTE)irq;
            DMA6PAD = DEVICE_REG;
            break;
        case DMA7:
            DMA7REQ = (BYTE)irq;
            DMA7PAD = DEVICE_REG;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int DMASetDataCount(DMA_ID id, WORD Count)
//------------------------------------------------------------------------------------------------
{
    DMAConfig[id].Count = Count;
    switch(id){
        case DMA0: DMA0CNT = Count-1;
            break;
        case DMA1: DMA1CNT = Count-1;
            break;
        case DMA2: DMA2CNT = Count-1;
            break;
        case DMA3: DMA3CNT = Count-1;
            break;
        case DMA4: DMA4CNT = Count-1;
            break;
        case DMA5: DMA5CNT = Count-1;
            break;
        case DMA6: DMA6CNT = Count-1;
            break;
        case DMA7: DMA7CNT = Count-1;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int DMASetCallback(DMA_ID id, void* _This,  int (*fillingBufAFunc)(void*, BYTE*, WORD ), int (*fillingBufBFunc)(void*, BYTE*, WORD ))
//------------------------------------------------------------------------------------------------
{
    DMAConfig[id].fillingBufferAFunc = fillingBufAFunc;
    DMAConfig[id].fillingBufferBFunc = fillingBufBFunc; 
    DMAConfig[id]._This = _This;
    return 0;
}
//------------------------------------------------------------------------------------------------
int DMAPrepBuffer(DMA_ID id)
//------------------------------------------------------------------------------------------------
{    
    BYTE BufBEna = 0;
    if(DMAConfig[id].fillingBufferAFunc){
        DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
    }
    switch(id){
        case DMA0: BufBEna = DMA0CONbits.MODE & 2;
            break;
        case DMA1: BufBEna = DMA1CONbits.MODE & 2;
            break;
        case DMA2: BufBEna = DMA2CONbits.MODE & 2;
            break;
        case DMA3: BufBEna = DMA3CONbits.MODE & 2;
            break;
        case DMA4: BufBEna = DMA4CONbits.MODE & 2;
            break;
        case DMA5: BufBEna = DMA5CONbits.MODE & 2;
            break;
        case DMA6: BufBEna = DMA6CONbits.MODE & 2;
            break;
        case DMA7: BufBEna = DMA7CONbits.MODE & 2;
            break;
        default:
        return -1;
    }
    if(BufBEna && DMAConfig[id].fillingBufferBFunc){
        DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int DMASetState(DMA_ID id, BOOL enabled, BOOL force)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case DMA0:
            DMA0CONbits.CHEN = enabled;
            DMA0REQbits.FORCE = force;
            break;
        case DMA1:
            DMA1CONbits.CHEN = enabled;
            DMA1REQbits.FORCE = force;
            break;
        case DMA2:
            DMA2CONbits.CHEN = enabled;
            DMA2REQbits.FORCE = force;
            break;
        case DMA3:
            DMA3CONbits.CHEN = enabled;
            DMA3REQbits.FORCE = force;
            break;
        case DMA4:
            DMA4CONbits.CHEN = enabled;
            DMA4REQbits.FORCE = force;
            break;
        case DMA5:
            DMA5CONbits.CHEN = enabled;
            DMA5REQbits.FORCE = force;
            break;
        case DMA6:
            DMA6CONbits.CHEN = enabled;
            DMA6REQbits.FORCE = force;
            break;
        case DMA7:
            DMA7CONbits.CHEN = enabled;
            DMA7REQbits.FORCE = force;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int DMAGetPPState(DMA_ID id)
//------------------------------------------------------------------------------------------------
{
    int state = 0;
    switch(id){
        case DMA0: state = DMACS1bits.PPST0;
            break;
        case DMA1: state = DMACS1bits.PPST1;
            break;
        case DMA2: state = DMACS1bits.PPST2;
            break;
        case DMA3: state = DMACS1bits.PPST3;
            break;
        case DMA4: state = DMACS1bits.PPST4;
            break;
        case DMA5: state = DMACS1bits.PPST5;
            break;
        case DMA6: state = DMACS1bits.PPST6;
            break;
        case DMA7: state = DMACS1bits.PPST7;
            break;
        default:
        return -1;
    }
    return state;
}
//------------------------------------------------------------------------------------------------
int DMASetInt(DMA_ID id, BYTE Level, BOOL enabled)
//------------------------------------------------------------------------------------------------
{
     switch(id){
        case DMA0:
            IFS0bits.DMA0IF = 0;    // Clear the DMA interrupt flag
            IEC0bits.DMA0IE = enabled;    // Enable DMA interrupt
            IPC1bits.DMA0IP = Level;
            break;
        case DMA1:
            IFS0bits.DMA1IF = 0;    // Clear the DMA interrupt flag
            IEC0bits.DMA1IE = enabled;    // Enable DMA interrupt
            IPC3bits.DMA1IP = Level;
            break;
        case DMA2:
            IFS1bits.DMA2IF = 0;    // Clear the DMA interrupt flag
            IEC1bits.DMA2IE = enabled;    // Enable DMA interrupt
            IPC6bits.DMA2IP = Level;
            break;
        case DMA3:
            IFS2bits.DMA3IF = 0;    // Clear the DMA interrupt flag
            IEC2bits.DMA3IE = enabled;    // Enable DMA interrupt
            IPC9bits.DMA3IP = Level;
            break;
        case DMA4:
            IFS2bits.DMA4IF = 0;    // Clear the DMA interrupt flag
            IEC2bits.DMA4IE = enabled;    // Enable DMA interrupt
            IPC11bits.DMA4IP = Level;
            break;
        case DMA5:
            IFS3bits.DMA5IF = 0;    // Clear the DMA interrupt flag
            IEC3bits.DMA5IE = enabled;    // Enable DMA interrupt
            IPC15bits.DMA5IP = Level;
            break;
        case DMA6:
            IFS4bits.DMA6IF = 0;    // Clear the DMA interrupt flag
            IEC4bits.DMA6IE = enabled;    // Enable DMA interrupt
            IPC17bits.DMA6IP = Level;
            break;
        case DMA7:
            IFS4bits.DMA7IF = 0;    // Clear the DMA interrupt flag
            IEC4bits.DMA7IE = enabled;    // Enable DMA interrupt
            IPC17bits.DMA7IP = Level;
            break;
        default:
        return -1;
    }
    return 0;
}
int DMAForceTransfer(DMA_ID id)
{
    switch(id){
        case DMA0: DMA0REQbits.FORCE = 1;
            break;
        case DMA1: DMA1REQbits.FORCE = 1;
            break;
        case DMA2: DMA2REQbits.FORCE = 1;
            break;
        case DMA3: DMA3REQbits.FORCE = 1;
            break;
        case DMA4: DMA4REQbits.FORCE = 1;
            break;
        case DMA5: DMA5REQbits.FORCE = 1;
            break;
        case DMA6: DMA6REQbits.FORCE = 1;
            break;
        case DMA7: DMA7REQbits.FORCE = 1;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
INTERRUPT _DMA0Interrupt(void)
//------------------------------------------------------------------------------------------------
{
    DMA_ID id = DMA0;
    if(DMAConfig[id].fillingBufferAFunc){
        if(DMACS1bits.PPST0){
            DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
        } else {
            DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
        }
    }     
    IFS0bits.DMA0IF = 0; // Clear the DMA0 Interrupt Flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _DMA1Interrupt(void)
//------------------------------------------------------------------------------------------------
{    
    DMA_ID id = DMA1;
    if(DMAConfig[id].fillingBufferBFunc){
        if(DMACS1bits.PPST1){
            DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
        } else {
            DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
        }
    }
    IFS0bits.DMA1IF = 0; // Clear the DMA0 Interrupt Flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _DMA2Interrupt(void)
//------------------------------------------------------------------------------------------------
{
    DMA_ID id = DMA2;
    if(DMAConfig[id].fillingBufferAFunc){
        if(DMACS1bits.PPST2){
            DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
        } else {
            DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
        }
    }     
    IFS1bits.DMA2IF = 0; // Clear the DMA0 Interrupt Flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _DMA3Interrupt(void)
//------------------------------------------------------------------------------------------------
{    
    DMA_ID id = DMA3;
    if(DMAConfig[id].fillingBufferBFunc){
        if(DMACS1bits.PPST3){
            DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
        } else {
            DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
        }
    }
    IFS2bits.DMA3IF = 0; // Clear the DMA0 Interrupt Flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _DMA4Interrupt(void)
//------------------------------------------------------------------------------------------------
{
    DMA_ID id = DMA4;
    if(DMAConfig[id].fillingBufferAFunc){
        if(DMACS1bits.PPST4){
            DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
        } else {
            DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
        }
    }     
    IFS2bits.DMA4IF = 0; // Clear the DMA0 Interrupt Flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _Interrupt61(void)//_DMA5Interrupt(void) // ошибка в конфигурации?
//------------------------------------------------------------------------------------------------
{    
    DMA_ID id = DMA5;
    if(DMAConfig[id].fillingBufferBFunc){
        if(DMACS1bits.PPST5){
            DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
        } else {
            DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
        }
    }
    IFS3bits.DMA5IF = 0; // Clear the DMA0 Interrupt Flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _DMA6Interrupt(void)
//------------------------------------------------------------------------------------------------
{
    DMA_ID id = DMA6;
    if(DMAConfig[id].fillingBufferAFunc){
        if(DMACS1bits.PPST6){
            DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
        } else {
            DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
        }
    }     
    IFS4bits.DMA6IF = 0; // Clear the DMA0 Interrupt Flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _DMA7Interrupt(void)
//------------------------------------------------------------------------------------------------
{    
    DMA_ID id = DMA7;
    if(DMAConfig[id].fillingBufferBFunc){
        if(DMACS1bits.PPST7){
            DMAConfig[id].fillingBufferAFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufA, DMAConfig[id].Count);
        } else {
            DMAConfig[id].fillingBufferBFunc(DMAConfig[id]._This, (BYTE*)DMAConfig[id].BufB, DMAConfig[id].Count);
        }
    }
    IFS4bits.DMA7IF = 0; // Clear the DMA0 Interrupt Flag
}
//************************************************************************************************
//
//                              Output Compare
//
//************************************************************************************************
OCConfigType OCConfig[8];
//------------------------------------------------------------------------------------------------
int OCInit(OC_ID id, SYS_IDLE idle, OC_TMR_SELECT tmr, OC_WORK_MODE ocm)
//------------------------------------------------------------------------------------------------
{
    WORD Config = 0;
    Config |= ((WORD)idle)  << 13;
    Config |= ((WORD)tmr)   << 3;
    Config |= ((WORD)ocm)   << 0;
    switch(id){
        case ID_OC1: OC1CON = Config;
            break;
        case ID_OC2: OC2CON = Config;
            break;
        case ID_OC3: OC3CON = Config;
            break;
        case ID_OC4: OC4CON = Config;
            break;
        case ID_OC5: OC5CON = Config;
            break;
        case ID_OC6: OC6CON = Config;
            break;
        case ID_OC7: OC7CON = Config;
            break;
        case ID_OC8: OC8CON = Config;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int OCSetTmr(OC_ID id, OC_TMR_SELECT tmr)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case ID_OC1: OC1CONbits.OCTSEL = tmr;
            break;
        case ID_OC2: OC2CONbits.OCTSEL = tmr;
            break;
        case ID_OC3: OC3CONbits.OCTSEL = tmr;
            break;
        case ID_OC4: OC4CONbits.OCTSEL = tmr;
            break;
        case ID_OC5: OC5CONbits.OCTSEL = tmr;
            break;
        case ID_OC6: OC6CONbits.OCTSEL = tmr;
            break;
        case ID_OC7: OC7CONbits.OCTSEL = tmr;
            break;
        case ID_OC8: OC8CONbits.OCTSEL = tmr;
            break;
        default:
            return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int OCSetValue(OC_ID id, WORD ocr, WORD ocrs)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case ID_OC1:
            OC1R  = ocr;
            OC1RS = ocrs;
            break;
        case ID_OC2:
            OC2R  = ocr;
            OC2RS = ocrs;
            break;
        case ID_OC3:
            OC3R  = ocr;
            OC3RS = ocrs;
            break;
        case ID_OC4:
            OC4R  = ocr;
            OC4RS = ocrs;
            break;
        case ID_OC5:
            OC5R  = ocr;
            OC5RS = ocrs;
            break;
        case ID_OC6:
            OC6R  = ocr;
            OC6RS = ocrs;
            break;
        case ID_OC7:
            OC7R  = ocr;
            OC7RS = ocrs;
            break;
        case ID_OC8:
            OC8R  = ocr;
            OC8RS = ocrs;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int OCSetInt(OC_ID id, BYTE Level, BOOL enabled)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case ID_OC1:
            IPC0bits.OC1IP = Level;
            IFS0bits.OC1IF = 0;
            IEC0bits.OC1IE = enabled;
            break;
        case ID_OC2:
            IPC1bits.OC2IP = Level;
            IFS0bits.OC2IF = 0;
            IEC0bits.OC2IE = enabled;
            break;
        case ID_OC3:
            IPC6bits.OC3IP = Level;
            IFS1bits.OC3IF = 0;
            IEC1bits.OC3IE = enabled;
            break;
        case ID_OC4:
            IPC6bits.OC4IP = Level;
            IFS1bits.OC4IF = 0;
            IEC1bits.OC4IE = enabled;
            break;
        case ID_OC5:
            IPC10bits.OC5IP = Level;
            IFS2bits.OC5IF = 0;
            IEC2bits.OC5IE = enabled;
            break;
        case ID_OC6:
            IPC10bits.OC6IP = Level;
            IFS2bits.OC6IF = 0;
            IEC2bits.OC6IE = enabled;
            break;
        case ID_OC7:
            IPC10bits.OC7IP = Level;
            IFS2bits.OC7IF = 0;
            IEC2bits.OC7IE = enabled;
            break;
        case ID_OC8:
            IPC11bits.OC8IP = Level;
            IFS2bits.OC8IF = 0;
            IEC2bits.OC8IE = enabled;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int OCSetMode(OC_ID id, OC_WORK_MODE ocm)
//------------------------------------------------------------------------------------------------
{
    switch(id){
        case ID_OC1: OC1CONbits.OCM = (BYTE)ocm;
            break;
        case ID_OC2: OC2CONbits.OCM = (BYTE)ocm;
            break;
        case ID_OC3: OC3CONbits.OCM = (BYTE)ocm;
            break;
        case ID_OC4: OC4CONbits.OCM = (BYTE)ocm;
            break;
        case ID_OC5: OC5CONbits.OCM = (BYTE)ocm;
            break;
        case ID_OC6: OC6CONbits.OCM = (BYTE)ocm;
            break;
        case ID_OC7: OC7CONbits.OCM = (BYTE)ocm;
            break;
        case ID_OC8: OC8CONbits.OCM = (BYTE)ocm;
            break;
        default:
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------
int OCSetCallback(OC_ID id, int (*CallbackFunc)(void))
//------------------------------------------------------------------------------------------------
{
    OCConfig[id].CallbackFunc = CallbackFunc;
    return 0;
}
//------------------------------------------------------------------------------------------------
INTERRUPT _OC1Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = ID_OC1;
    if(OCConfig[id].CallbackFunc){
        OCConfig[id].CallbackFunc();
    }
    IFS0bits.OC1IF = 0; // Clear OC1 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _OC2Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = ID_OC2;
    if(OCConfig[id].CallbackFunc){
        OCConfig[id].CallbackFunc();
    }
    IFS0bits.OC2IF = 0; // Clear OC2 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _OC3Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = ID_OC3;
    if(OCConfig[id].CallbackFunc){
        OCConfig[id].CallbackFunc();
    }
    IFS1bits.OC3IF = 0; // Clear OC3 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _OC4Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = ID_OC4;
    if(OCConfig[id].CallbackFunc){
        OCConfig[id].CallbackFunc();
    }
    IFS1bits.OC4IF = 0; // Clear OC4 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _OC5Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = ID_OC5;
    if(OCConfig[id].CallbackFunc){
        OCConfig[id].CallbackFunc();
    }
    IFS2bits.OC5IF = 0; // Clear OC5 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _OC6Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = ID_OC6;
    if(OCConfig[id].CallbackFunc){
        OCConfig[id].CallbackFunc();
    }
    IFS2bits.OC6IF = 0; // Clear OC6 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _OC7Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = ID_OC7;
    if(OCConfig[id].CallbackFunc){
        OCConfig[id].CallbackFunc();
    }
    IFS2bits.OC7IF = 0; // Clear OC7 interrupt flag
}
//------------------------------------------------------------------------------------------------
INTERRUPT _OC8Interrupt( void )
//------------------------------------------------------------------------------------------------
{
    int id = ID_OC8;
    if(OCConfig[id].CallbackFunc){
        OCConfig[id].CallbackFunc();
    }
    IFS2bits.OC8IF = 0; // Clear OC8 interrupt flag
}
/************************************************************************************************
                                     @@@@@  @@@@@@  @@@@
                                    @@   @@  @@  @@  @@
                                    @@       @@  @@  @@
                                      @@@    @@@@@   @@
                                         @@  @@      @@
                                    @@   @@  @@      @@
                                     @@@@@  @@@@    @@@@
************************************************************************************************/
// 1. Отправка команды
// 
// 2. Отправка команды + отправка BYTE
// 3. Отправка команды + отправка WORD
// 4. Отправка команды + отправка DWORD
// 5. Отправка команды + отправка QWORD
// 6. Отправка команды + отправка BYTE* через DMA
// 7. Отправка команды + отправка WORD* через DMA
// 
// 8.  Отправка команды + получение BYTE
// 9.  Отправка команды + получение WORD
// 10. Отправка команды + получение DWORD
// 11. Отправка команды + получение QWORD
// 12. Отправка команды + получение BYTE* через DMA
// 13. Отправка команды + получение WORD* через DMA



typedef enum _Eflag
{
    SEND_DATA = 0,
    SEND_FINAL_PART,
    SEND_END,
    RECEIVE_DATA,
    RECEIVE_FINAL_PART,
    RECEIVE_END
} EFlag;

typedef struct _SPIData{
    WORD len;
    WORD pos;
    BYTE* val;
    EFlag Flag;
} SPIData;

typedef struct _DEVICE_REG{
    SPI_ID id;
    SPIConfig Config;
    int (*DeviceSelect)(void);
    int (*DeviceRelease)(void);
    BYTE OldIntLevel;
} DEVICE_REG;

typedef struct _SPI_STATUS 
{
    BYTE CurrentDevice;
    int SPIDataCount;
    BYTE Busy;
}SPI_STATUS;
#define MAX_SPI_DEVICES 10
DEVICE_REG SPIDeviceList[MAX_SPI_DEVICES];
static BYTE DeviceCount = 0;

SPI_STATUS SPIStatus[2];
int SPICallBack(void* _This, BYTE* DMABuff, WORD BufSize);
void SPIDeviceLock(SPI_ID id)
{
    while(SPIStatus[id].Busy){
        Nop();
        Nop();
    };
    SPIStatus[id].Busy = 1;
}
void SPIDeviceRelease(SPI_ID id)
{
    SPIStatus[id].Busy = 0;
}

BYTE SPIRegisterDevice(SPI_ID id, SPIConfig Config, int (*DeviceSelect)(void), int (*DeviceRelease)(void))
{
    SPIDeviceList[DeviceCount].Config.SPICON1 = Config.SPICON1;
    SPIDeviceList[DeviceCount].Config.SPISTAT = Config.SPISTAT;
    SPIDeviceList[DeviceCount].DeviceSelect = DeviceSelect;
    SPIDeviceList[DeviceCount].DeviceRelease = DeviceRelease;
    SPIDeviceList[DeviceCount].id = id;
    SPIDeviceList[DeviceCount].OldIntLevel = 0;
    return DeviceCount++;
}

//------------------------------------------------------------------------------------------------
int SPIInit()
//------------------------------------------------------------------------------------------------
{
    BYTE SPI_id = 0;
    BYTE i = 0;
    for (SPI_id = 0; SPI_id < 2; SPI_id++) {
        SPIStatus[SPI_id].CurrentDevice = 0;
        SPIStatus[SPI_id].SPIDataCount = 0;
        SPIStatus[SPI_id].Busy = 0;
    }

    DeviceCount = 0;
    for(i = 0; i < MAX_SPI_DEVICES; i++){
        SPIDeviceList[i].Config.SPICON1 = 0;
        SPIDeviceList[i].Config.SPISTAT = 0;
        SPIDeviceList[i].DeviceSelect = NULL;
        SPIDeviceList[i].DeviceRelease = NULL;
        SPIDeviceList[i].id = ID_SPI1;
        SPIDeviceList[i].OldIntLevel = 0;
    }
    DMASelectDevice(DMA7, IRQ_SPI1, (int)&SPI1BUF);
    DMASetCallback(DMA7, (void*)&(SPIStatus[ID_SPI1]), SPICallBack, SPICallBack);
    DMASetInt(DMA7, 5, TRUE);

    DMASelectDevice(DMA6, IRQ_SPI2, (int)&SPI2BUF);
    DMASetCallback(DMA6, (void*)&(SPIStatus[ID_SPI2]), SPICallBack, SPICallBack);
    DMASetInt(DMA6, 5, TRUE);

    return 0;
}
//------------------------------------------------------------------------------------------------
int SPICallBack(void* _This, BYTE* DMABuff, WORD BufSize)
//------------------------------------------------------------------------------------------------
{
    SPI_STATUS* Status = (SPI_STATUS*)_This;
    //Status->CurrentDevice;
    SPIDeviceRelease(ID_SPI1);
    return 0;
}


//------------------------------------------------------------------------------------------------
INTERRUPT _SPI1Interrupt(void)
//------------------------------------------------------------------------------------------------
{    
    IFS0bits.SPI1IF = 0;
}

#if defined(__C30__)
    static inline __attribute__((__always_inline__)) void WaitForDataReady( void )
    {
        while ((SPI1STATbits.SPITBF == 1) || (SPI1STATbits.SPIRBF == 0));
    }
#endif
#ifdef _WIN32
    #define WaitForDataReady( void )     
#endif

// Send BYTE Array to Device
// Internal!!!
//------------------------------------------------------------------------------------------------
WORD SPISendByteArrayDMA(SPI_ID SPI_id, BYTE* val, WORD len)
//------------------------------------------------------------------------------------------------
{
    //WORD Config;
    //Config = DMACreateConfig(SIZE_BYTE, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, ONE_SHOT);

    if(SPI_id == ID_SPI1) {
        DMASetConfig(DMA7, 0x4001);
        DMASetDataCount(DMA7, len);
        memcpy(DMA7BufferA, val, len);
        DMASetState(DMA7, TRUE, FALSE);
    } else if(SPI_id == ID_SPI2){
        DMASetConfig(DMA6, 0x4001);
        DMASetDataCount(DMA6, len);
        memcpy(DMA6BufferA, val, len);
        DMASetState(DMA6, TRUE, FALSE);
    } else return 0;
    return len;
}
//------------------------------------------------------------------------------------------------
WORD SPIReceiveByteArrayDMA(SPI_ID SPI_id, BYTE* val, WORD len)
//------------------------------------------------------------------------------------------------
{
    SPIData DataToSend;
    WORD BufA;
    WORD BufB;
    WORD Config;
    WORD DMABuffSize;
    DMA_ID DMA_id;

    if(SPI_id == ID_SPI1) {
        DMABuffSize = DMA7BUF_SIZE;
        DMA_id = DMA7;
    } else if(SPI_id == ID_SPI2){
        DMABuffSize = DMA6BUF_SIZE;
        DMA_id = DMA6;
    } else return 0;

    if(len <= DMABuffSize*2){ // данные меньше 256 байт получаем одним куском
        DMABuffSize = len;
        Config = DMACreateConfig(SIZE_BYTE, DEVICE_TO_RAM, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, ONE_SHOT);
    } else {
        Config = DMACreateConfig(SIZE_BYTE, DEVICE_TO_RAM, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
    }

    DataToSend.val = val;
    DataToSend.len = len;
    DataToSend.pos = 0;
    DataToSend.Flag = RECEIVE_DATA;
    SPIStatus[SPI_id].SPIDataCount = len;
    DMAInit(DMA_id, Config);
    if(SPI_id == ID_SPI1){
        DMASelectDevice(DMA_id, IRQ_SPI1, (int)&SPI1BUF);
    } else if(SPI_id == ID_SPI2){
        DMASelectDevice(DMA_id, IRQ_SPI2, (int)&SPI2BUF);
    }
    DMASetDataCount(DMA_id, DMABuffSize);

    DMASetCallback(DMA_id, (void*)&DataToSend, SPICallBack, SPICallBack);
    DMASetInt(DMA_id, 5, TRUE); //TODO: уровень обработчика DMA
    DMAPrepBuffer(DMA_id);
    // получение данных
    WaitForDataReady();
    DMASetState(DMA_id, TRUE, TRUE);

    while(DataToSend.Flag < RECEIVE_FINAL_PART);
    return len;
}

//------------------------------------------------------------------------------------------------
// получение данных через SPI
// регулярная отправка без блокировок и выбора устройства
//------------------------------------------------------------------------------------------------
WORD SPI1GetArray(BYTE *val, WORD len)
//------------------------------------------------------------------------------------------------
{
    volatile BYTE Dummy;
    BYTE MODE16 = SPI1CON1bits.MODE16;
    WaitForDataReady();
    Dummy = SPI1BUF;
    if(!val) return 0;
    if(MODE16) {
        WORD_VAL wv;
        // Read the data, 2 bytes at a time, for as long as possible
        SPI1BUF = 0x0000;    // Send a dummy WORD to generate 32 clocks
        while(len >= 2) {
            WaitForDataReady();      // Wait until WORD is transmitted
            wv.Val = SPI1BUF;
            len -= 2;
            if(len >= 2)
                SPI1BUF = 0x0000;    // Send a dummy WORD to generate 32 clocks
            *(val++) = wv.v[1];
            *(val++) = wv.v[0];
        };
    }
    // Read the data
    if(len) {
        if(MODE16){
            SPI1STATbits.SPIEN = 0;
            SPI1CON1bits.MODE16 = 0;
            SPI1STATbits.SPIEN = 1;
        }
        SPI1BUF = 0x00;    // Send a dummy BYTE to generate 16 clocks
        while( len ) {
            WaitForDataReady();      // Wait until BYTE is transmitted
            *(val++) = SPI1BUF;            
            len--;
            if(len >= 1)
                SPI1BUF = 0x00;    // Send a dummy WORD to generate 32 clocks
        };
    }
    if(MODE16) {
        SPI1STATbits.SPIEN = 0;
        SPI1CON1bits.MODE16 = 1;
        SPI1STATbits.SPIEN = 1;
    }
    return len;
}//end 
//------------------------------------------------------------------------------------------------
// отправка данных по SPI
// регулярная отправка без блокировок и выбора устройства
//------------------------------------------------------------------------------------------------
WORD SPI1SendArray(BYTE *val, WORD len)
//------------------------------------------------------------------------------------------------
{    
    volatile BYTE Dummy;
    BYTE MODE16 = SPI1CON1bits.MODE16;
    if(MODE16) {
        WORD_VAL wv;
        // Send the data, 2 bytes at a time, for as long as possible
        while(len > 1) {
            wv.v[1] = *val++;
            wv.v[0] = *val++;
            len -= 2;            
            WaitForDataReady();      // Wait until WORD is transmitted
            Dummy = SPI1BUF;
            SPI1BUF = wv.Val;       // Start sending the WORD
        };
        WaitForDataReady();      // Wait until WORD is transmitted
        Dummy = SPI1BUF;
    }
    // Send the data, one byte at a time
    if(len) {
        if(MODE16){
            SPI1STATbits.SPIEN = 0;
            SPI1CON1bits.MODE16 = 0;
            SPI1STATbits.SPIEN = 1;
        }
        while(len) {
            len--;                  
            WaitForDataReady();      // Wait until byte is transmitted
            Dummy = SPI1BUF;
            SPI1BUF = *val++;       // Start sending the byte
        };
        WaitForDataReady();      // Wait until WORD is transmitted
        Dummy = SPI1BUF;
    }
    if(MODE16) {
        SPI1STATbits.SPIEN = 0;
        SPI1CON1bits.MODE16 = 1;
        SPI1STATbits.SPIEN = 1;
    }
    return len;
}

SPIConfig SPI_CreateParams( SPI_MODE Mode, SPI_CLOCK_MODE ClockMode, DWORD DeviceSpeed,SYS_IDLE Idle,SPI_DATA_SIZE_BIT DataSize, SPI_INPUT_PHASE InputPhase)
{
    SPIConfig Config;
    Config.SPISTAT = 0;
    Config.SPISTAT |= ((WORD)Idle) << 13;
    Config.SPICON1 = 0;
    Config.SPICON1 |= ((WORD)Mode) << 5;
    Config.SPICON1 |= ((WORD)ClockMode & 1) << 6;
    Config.SPICON1 |= ((WORD)ClockMode & 2) << 8;
    Config.SPICON1 |= ((WORD)InputPhase) << 9;
    Config.SPICON1 |= ((WORD)DataSize) << 10;
    return Config;
}
//________________________________________________________________________________________________
// 
// передача данных с помощью DMA
// 1. Определить порт SPI
// 2. Блокирование прерываний уровня порта SPI
// 3. Проверка счетчика SPIxDataCount. Ожидание, если не 0
// 4. настройка порта SPI
// 5. инициализация DMA (передача функций Select и Release и т.д.)
// 6. запуск передачи
// 7. ожидание передачи всех данных в буфер DMA
// 8. снятие блокировки прерывания выполняется счетчиком SPIxDataCounter()
// 9. выход.

WORD SPISendData( BYTE DeviceHandle, BYTE* Cmd, WORD CmdLen, BYTE* Data, WORD DataLen )
{
    // 1. Определить порт SPI
    SPI_ID SPI_id = SPIDeviceList[DeviceHandle].id;
    WORD DataSent = 0;
    // блокирование устройства
    SPIDeviceLock(SPI_id);
    // 4. настройка порта SPI
    switch(SPI_id){
        case ID_SPI1:
            SPI1CON1 = SPIDeviceList[DeviceHandle].Config.SPICON1;
            SPI1STAT = SPIDeviceList[DeviceHandle].Config.SPISTAT;
            break;
        case ID_SPI2:
            SPI2CON1 = SPIDeviceList[DeviceHandle].Config.SPICON1;
            SPI2STAT = SPIDeviceList[DeviceHandle].Config.SPISTAT;
            break;
        default:
            SET_INT_LOCK(SPIDeviceList[DeviceHandle].OldIntLevel);
            return 0 ;
    }
    // 5. отправка команды
    SPIDeviceList[DeviceHandle].DeviceSelect();
    SPI1SendArray(Cmd, CmdLen);
    // 6. Отправка данных
    DataSent = SPISendByteArrayDMA(SPI_id, Data, DataLen);
    // 8. снятие блокировки прерывания выполняется счетчиком SPIxDataCounter()
    // 9. выход.
    return DataSent;
}
int SPIReceiveData( BYTE DeviceHandle, BYTE* Cmd, WORD CmdLen, BYTE* val, WORD len );

/*
int SPI1SendData( WORD SPI_para, BYTE* Cmd, WORD CmdLen, BYTE* val, WORD len, int (*DeviceSelect)(void), int (*DeviceRelease)(void) )
{
    BYTE TmpInt;
    // установка текущего приоритета CPU на 4
    TmpInt = SAVE_INT_LOCK;
    SPIData DataStruct;
    WORD BufA;
    WORD BufB;
    WORD Config;
    if(len + CmdLen <= 128){
        Config = DMACreateConfig(SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, ONE_SHOT);
    } else {
        if(len + CmdLen <= 256){
            Config = DMACreateConfig(SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, ONE_SHOT_PP);
        } else {
            Config = DMACreateConfig(SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);
        }
    }
    if(len & 0x0001){
        // Если длинна нечетная, то отправим 1 байт отдельно в конце
        len -= 1;
    }
    DataStruct.Data1 = Cmd;
    DataStruct.Data1Len = CmdLen;
    DataStruct.Data2 = val;
    DataStruct.Data2Len = len;
    DataStruct.pos = 0;
    DataStruct.Flag = 0;
    SPI1DataCount = CmdLen + len;
    DMAInit(DMA7, Config);
    DMASelectDevice(DMA7, IRQ_TMR2, (int)&OC1R);
    DMASetDataCount(DMA7, 64);

    DMASetCallback(DMA7, (void*)&DataStruct, SPIPPCallBack, SPIPPCallBack);
    DMASetInt(DMA7, 5, TRUE);
    DMAPrepBuffer(DMA7);
    SET_INT_LOCK(4);
    // выбор устройства
    DeviceSelect();
    
//    //Setup for SPI1 Master mode:
//    // Interrupt Controller Settings
//    IFS0bits.SPI1IF = 0;
//    IEC0bits.SPI1IE = 1;
//    IPC2bits.SPI1IP = 5;
//    SPI1CON1 = 0x0F;
//    SPI1CON2 = 0;
//    SPI1CON1bits.CKE = 1;
//    SPI1CON1bits.MSTEN = 1;
//    SPI1STATbits.SPIEN = 1;
// Initialize Output Compare Module in Contionous pulse mode
    OCInit(ID_OC1, IDLE_DISABLE, OC_TMR2, OC_DISABLED);
    //OCSetValue(ID_OC1, 100, 125);
    OCSetInt(ID_OC1, 6, TRUE);
    OCSetCallback(ID_OC1, SPI1DataCounter);
    OCSetMode(ID_OC1, TOGGLE);
// Initialize Timer2
    TimerInit(TIMER2, CLOCK_SOURCE_INTERNAL, GATED_DISABLE, PRE_1_8, IDLE_DISABLE, BIT_16, SYNC_DISABLE);
    TimerSetValue(TIMER2, 0, 0x0100);
    TimerSetCallback(TIMER2, NULL);
    TimerSetInt(TIMER2, 5, FALSE);    
    
    // отправка команды
    // отправка данных
    ClearSPIDoneFlag();
    DMASetState(DMA7, TRUE, TRUE);
    T2CONbits.TON = 1;
    while(SPI1DataCount>0){
        Nop();
        Nop();
    }
    // освобождение устройства
    DeviceRelease();
    // восстановление приоритета CPU
    RESTORE_INT_LOCK(TmpInt);
    return 0;
}
*/
