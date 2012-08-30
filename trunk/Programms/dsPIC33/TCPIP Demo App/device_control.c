#include "device_control.h"
#include "TCPIP Stack/TCPIP.h"
int InitTimer(TIMERS_ID id, TMR_CLOCK_SOURCE source, TMR_GATED_MODE gated, TMR_PRESCALER pre, TMR_IDLE idle, TMR_BIT_MODE bit, TMR1_SYNC sync)
{
    WORD Config = 0;
    
    switch(idle){
        case IDLE_ENABLE: //Config &= 0xDFFF;
            break;
        case IDLE_DISABLE: Config |= 0x2000; 
            break;
    }

    switch(source){
        case CLOCK_SOURCE_INTERNAL: //Config &= 0xFFFD;
            break;
        case CLOCK_SOURCE_EXTERNAL: Config |= 0x0002;
            break;
    }
    switch(gated){
        case GATED_DISABLE: //Config &= 0xFFFB;
            break;
        case GATED_ENABLE:  Config |= 0x0004;
            break;
    }
    
    switch(bit){
        case BIT_16: //Config &= 0xFFF7;
            break;
        case BIT_32: Config |= 0x0008;
            break;
    }
    switch(pre){
        case PRE_1_1:   Config |= 0x0000;
            break;
        case PRE_1_8:   Config |= 0x0010;
            break;
        case PRE_1_64:  Config |= 0x0020;
            break;
        case PRE_1_256: Config |= 0x0030;
            break;
    }
    switch(sync){
        case SYNC_ENABLE:  Config |= 0x0004;
            break;
        case SYNC_DISABLE: //Config &= 0xFFFB;
            break;
    }
    switch(id){
        case T1: T1CON = Config;
            break;
        case T2: T2CON = Config;
            break;
        case T3: T3CON = Config;
            break;
        case T4: T4CON = Config;
            break;
        case T5: T5CON = Config;
            break;
        case T6: T6CON = Config;
            break;
        case T7: T7CON = Config;
            break;
        case T8: T8CON = Config;
            break;
        case T9: T9CON = Config;
            break;
        default:
        return -1;
    }
    return 0;
}
int SetTimerInt(TIMERS_ID id, BYTE Level, BOOL enabled)
{
    switch(id){
        case T1:
            IPC0bits.T1IP = Level & 0x07;     // Set Timer1 Interrupt Priority Level
            IFS0bits.T1IF = 0;         // Clear Timer1 Interrupt Flag
            IEC0bits.T1IE = enabled;   // Enable Timer1 interrupt
            break;
        case T2: 
            IPC1bits.T2IP = Level & 0x07;     // Set Timer2 Interrupt Priority Level
            IFS0bits.T2IF = 0;         // Clear Timer2 Interrupt Flag
            IEC0bits.T2IE = enabled;   // Enable Timer2 interrupt
            break;
        case T3: 
            IPC2bits.T3IP = Level & 0x07;     // Set Timer3 Interrupt Priority Level
            IFS0bits.T3IF = 0;         // Clear Timer3 Interrupt Flag
            IEC0bits.T3IE = enabled;   // Enable Timer3 interrupt
            break;
        case T4: 
            IPC6bits.T4IP = Level & 0x07;     // Set Timer4 Interrupt Priority Level
            IFS1bits.T4IF = 0;         // Clear Timer4 Interrupt Flag
            IEC1bits.T4IE = enabled;   // Enable Timer4 interrupt
            break;
        case T5: 
            IPC7bits.T5IP = Level & 0x07;     // Set Timer5 Interrupt Priority Level
            IFS1bits.T5IF = 0;         // Clear Timer5 Interrupt Flag
            IEC1bits.T5IE = enabled;   // Enable Timer5 interrupt
            break;
        case T6: 
            IPC11bits.T6IP = Level & 0x07;     // Set Timer6 Interrupt Priority Level
            IFS2bits.T6IF = 0;         // Clear Timer6 Interrupt Flag
            IEC2bits.T6IE = enabled;   // Enable Timer6 interrupt
            break;
        case T7: 
            IPC12bits.T7IP = Level & 0x07;     // Set Timer7 Interrupt Priority Level
            IFS3bits.T7IF = 0;         // Clear Timer7 Interrupt Flag
            IEC3bits.T7IE = enabled;   // Enable Timer7 interrupt
            break;
        case T8: 
            IPC12bits.T8IP = Level & 0x07;     // Set Timer8 Interrupt Priority Level
            IFS3bits.T8IF = 0;         // Clear Timer8 Interrupt Flag
            IEC3bits.T8IE = enabled;   // Enable Timer8 interrupt
            break;
        case T9: 
            IPC13bits.T9IP = Level & 0x07;     // Set Timer9 Interrupt Priority Level
            IFS3bits.T9IF = 0;         // Clear Timer9 Interrupt Flag
            IEC3bits.T9IE = enabled;   // Enable Timer9 interrupt
            break;
        default:
        return -1;
    }
    return 0;
}
int SetTimerValue(TIMERS_ID id, WORD TmrValue, WORD PRValue)
{
    switch(id){
        case T1:
            TMR1 = TmrValue;
            PR1  = PRValue;
            break;
        case T2: 
            TMR2 = TmrValue;
            PR2  = PRValue;
            break;
        case T3: 
            TMR3 = TmrValue;
            PR3  = PRValue;
            break;
        case T4: 
            TMR4 = TmrValue;
            PR4  = PRValue;
            break;
        case T5: 
            TMR5 = TmrValue;
            PR5  = PRValue;
            break;
        case T6: 
            TMR6 = TmrValue;
            PR6  = PRValue;
            break;
        case T7: 
            TMR7 = TmrValue;
            PR7  = PRValue;
            break;
        case T8: 
            TMR8 = TmrValue;
            PR8  = PRValue;
            break;
        case T9: 
            TMR9 = TmrValue;
            PR9  = PRValue;
            break;
        default:
        return -1;
    }
    return 0;
}
int SetTimerState(TIMERS_ID id, BOOL enabled)
{
        
    switch(id){
        case T1: T1CONbits.TON = enabled;
            break;
        case T2: T2CONbits.TON = enabled;
            break;
        case T3: T3CONbits.TON = enabled;
            break;
        case T4: T4CONbits.TON = enabled;
            break;
        case T5: T5CONbits.TON = enabled;
            break;
        case T6: T6CONbits.TON = enabled;
            break;
        case T7: T7CONbits.TON = enabled;
            break;
        case T8: T8CONbits.TON = enabled;
            break;
        case T9: T9CONbits.TON = enabled;
            break;
        default:
        return -1;
    }
    return 0;
}

int DMAInit(DMA_ID id, DMA_DATA_SIZE_BIT bit, DMA_TRANSFER_DIRECTION dir, DMA_COMPLETE_BLOCK_INT half, DMA_NULL_DATA_MODE nullw, DMA_ADRESING_MODE addr, DMA_OPERATION_MODE mode)
{
    WORD Config = 0;
    switch(bit){
        case SIZE_WORD: Config |= 0x0000;
            break;
        case SIZE_BYTE: Config |= 0x4000;
            break;
    }
    switch(dir){
        case DEVICE_TO_RAM: Config |= 0x0000;
            break;
        case RAM_TO_DEVICE: Config |= 0x2000;
            break;    
    }
    switch(half){
        case FULL_BLOCK: Config |= 0x0000;
            break;
        case HALF_BLOCK: Config |= 0x1000;
            break;    
    }
    switch(nullw){
        case NORMAL_OPS: Config |= 0x0000;
            break;
        case NULL_DATA_TO_DEVICE: Config |= 0x0800;
            break;    
    }
    switch(addr){
        case REG_INDIRECT_W_POST_INC:   Config |= 0x0000;
            break;
        case REG_INDIRECT:              Config |= 0x0010;
            break;
        case DEVICE_INDIRECT_ADDR:      Config |= 0x0020;
            break;    
    }
    switch(mode){
        case CONTINUOUS:  Config |= 0x0000;
            break;
        case ONE_SHOT:    Config |= 0x0001;
            break;
        case CONTINUE_PP: Config |= 0x0002;
            break;    
        case ONE_SHOT_PP: Config |= 0x0003;
            break;    
    }
    switch(id){
        case DMA0: DMA0CON = Config;
            break;
        case DMA1: DMA1CON = Config;
            break;
        case DMA2: DMA2CON = Config;
            break;
        case DMA3: DMA3CON = Config;
            break;
        case DMA4: DMA4CON = Config;
            break;
        case DMA5: DMA5CON = Config;
            break;
        case DMA6: DMA6CON = Config;
            break;
        case DMA7: DMA7CON = Config;
            break;
        default:
        return -1;
    }
    return 0;
}
int DMASelectDevice(DMA_ID id, DMA_DEVICE_IRQ irq, WORD * DEVICE_REG)
{
    switch(id){
        case DMA0: 
            DMA0REQ = (BYTE)irq;
            DMA0PAD = (int)DEVICE_REG;
            break;
        case DMA1: 
            DMA1REQ = (BYTE)irq;
            DMA1PAD = (int)DEVICE_REG;
            break;
        case DMA2: 
            DMA2REQ = (BYTE)irq;
            DMA2PAD = (int)DEVICE_REG;
            break;
        case DMA3: 
            DMA3REQ = (BYTE)irq;
            DMA3PAD = (int)DEVICE_REG;
            break;
        case DMA4: 
            DMA4REQ = (BYTE)irq;
            DMA4PAD = (int)DEVICE_REG;
            break;
        case DMA5: 
            DMA5REQ = (BYTE)irq;
            DMA5PAD = (int)DEVICE_REG;
            break;
        case DMA6: 
            DMA6REQ = (BYTE)irq;
            DMA6PAD = (int)DEVICE_REG;
            break;
        case DMA7: 
            DMA7REQ = (BYTE)irq;
            DMA7PAD = (int)DEVICE_REG;
            break;
        default:
        return -1;
    }
    return 0;
}
int DMASelectBuffer(DMA_ID id, int DMAbufA, int DMAbufB, WORD Count)
{
     switch(id){
        case DMA0: 
            DMA0STA = DMAbufA;
            DMA0STB = DMAbufB;
            DMA0CNT = Count;
            break;
        case DMA1: 
            DMA1STA = DMAbufA;
            DMA1STB = DMAbufB;
            DMA1CNT = Count;
            break;
        case DMA2: 
            DMA2STA = DMAbufA;
            DMA2STB = DMAbufB;
            DMA2CNT = Count;
            break;
        case DMA3: 
            DMA3STA = DMAbufA;
            DMA3STB = DMAbufB;
            DMA3CNT = Count;
            break;
        case DMA4: 
            DMA4STA = DMAbufA;
            DMA4STB = DMAbufB;
            DMA4CNT = Count;
            break;
        case DMA5: 
            DMA5STA = DMAbufA;
            DMA5STB = DMAbufB;
            DMA5CNT = Count;
            break;
        case DMA6: 
            DMA6STA = DMAbufA;
            DMA6STB = DMAbufB;
            DMA6CNT = Count;
            break;
        case DMA7: 
            DMA7STA = DMAbufA;
            DMA7STB = DMAbufB;
            DMA7CNT = Count;
            break;
        default:
        return -1;
    }
    return 0;
}
int DMASetState(DMA_ID id, BOOL enabled, BOOL force)
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
int DMAGetPPState(DMA_ID id)
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

int DMASetInt(DMA_ID id, BYTE Level, BOOL enabled)
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
