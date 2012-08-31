#ifndef __DEVICE_CONTROL_H_
#define __DEVICE_CONTROL_H_
#include "GenericTypeDefs.h"

// Таймеры
typedef enum _TIMERS_ID{
    T1, T2, T3, T4, T5, T6, T7, T8, T9
} TIMERS_ID;
typedef enum _TMR_CLOCK_SOURCE {
    CLOCK_SOURCE_INTERNAL = 0, CLOCK_SOURCE_EXTERNAL = 1
} TMR_CLOCK_SOURCE;

typedef enum _TMR_GATED_MODE {
    GATED_DISABLE = 0, GATED_ENABLE = 1
}TMR_GATED_MODE;

typedef enum _TMR_PRESCALER {
    PRE_1_1 = 00, PRE_1_8 = 1, PRE_1_64 = 2, PRE_1_256 = 3
}TMR_PRESCALER;

typedef enum _SYS_IDLE {
    IDLE_ENABLE = 0, IDLE_DISABLE = 1
} SYS_IDLE;

typedef enum _TMR_BIT_MODE {
    BIT_16 = 0, BIT_32 = 1
}TMR_BIT_MODE;

typedef enum _TMR1_SYNC {
    SYNC_ENABLE = 1, SYNC_DISABLE = 0
} TMR1_SYNC;

int TimerInit(TIMERS_ID id, TMR_CLOCK_SOURCE source, TMR_GATED_MODE gated, TMR_PRESCALER pre, SYS_IDLE idle, TMR_BIT_MODE bit, TMR1_SYNC sync);
int TimerSetInt(TIMERS_ID id, BYTE Level, BOOL enabled);
int TimerSetValue(TIMERS_ID id, WORD TmrValue, WORD PRValue);
int TimerSetState(TIMERS_ID id, BOOL enabled);
// DMA
typedef enum _DMA_ID{
    DMA0, DMA1, DMA2, DMA3, DMA4, DMA5, DMA6, DMA7
} DMA_ID;

typedef enum _DMA_DATA_SIZE_BIT {
    SIZE_WORD = 0, SIZE_BYTE = 1
}DMA_DATA_SIZE_BIT;

typedef enum _DMA_TRANSFER_DIRECTION{
    DEVICE_TO_RAM = 0, RAM_TO_DEVICE = 1
} DMA_TRANSFER_DIRECTION;

typedef enum _DMA_COMPLETE_BLOCK_INT{
    FULL_BLOCK = 0, HALF_BLOCK = 1
}DMA_COMPLETE_BLOCK_INT;

typedef enum _DMA_NULL_DATA_MODE {
    NORMAL_OPS = 0, NULL_DATA_TO_DEVICE = 1
}DMA_NULL_DATA_MODE;

typedef enum _DMA_ADRESING_MODE {
    REG_INDIRECT_W_POST_INC = 0, REG_INDIRECT = 1, DEVICE_INDIRECT_ADDR = 2, 
}DMA_ADRESING_MODE;

typedef enum _DMA_OPERATION_MODE {
    CONTINUOUS = 0, ONE_SHOT = 1, CONTINUE_PP = 2, ONE_SHOT_PP = 3,
}DMA_OPERATION_MODE;

typedef enum _DMA_DEVICE_IRQ {
    IRQ_INT0 = 0, IRQ_IC1=1, IRQ_IC2=5, IRQ_OC1=2, IRQ_OC2=6, IRQ_TMR2=7, IRQ_TMR3=8, IRQ_SPI1=10, IRQ_SPI2=33, IRQ_UART1_RX, IRQ_UART1_TX, IRQ_UART2_RX, IRQ_UART2_TX, IRQ_ADC1, IRQ_ADC2
} DMA_DEVICE_IRQ;

int DMAInit(DMA_ID id, DMA_DATA_SIZE_BIT size, DMA_TRANSFER_DIRECTION dir, DMA_COMPLETE_BLOCK_INT half, DMA_NULL_DATA_MODE nullw, DMA_ADRESING_MODE addr, DMA_OPERATION_MODE mode);
int DMASelectDevice(DMA_ID id, DMA_DEVICE_IRQ irq, int DEVICE_REG);
int DMASelectBuffer(DMA_ID id, int DMAbufA, int DMAbufB, WORD Count);
int DMASetState(DMA_ID id, BOOL enabled, BOOL force);
int DMAGetPPState(DMA_ID id);
int DMASetInt(DMA_ID id, BYTE Level, BOOL enabled);

typedef enum _OC_WORK_MODE{
    OC_DISABLED = 0, ACT_LOW_ONE_SHOT = 1, ACT_HIGH_ONE_SHOT = 2, TOGGLE = 3, DELAYED_ONE_SHOT = 4, CONT_PULSE = 5, PWM = 6, PWM_W_FAULT_PROT = 7
} OC_WORK_MODE;

typedef enum _OC_ID{
    ID_OC1, ID_OC2, ID_OC3, ID_OC4, ID_OC5, ID_OC6, ID_OC7, ID_OC8
}OC_ID;

typedef enum _OC_TMR_SELECT {
    OC_TMR2 = 0, OC_TMR3 = 1
} OC_TMR_SELECT;

int OCInit(OC_ID id, SYS_IDLE idle, OC_TMR_SELECT tmr, OC_WORK_MODE ocm);
int OCSetInt(OC_ID id, BYTE Level, BOOL enabled);
int OCSetMode(OC_ID id,OC_WORK_MODE ocm);
int OCSetValue(OC_ID id, WORD ocr, WORD ocrs);

#endif //__DEVICE_CONTROL_H_
