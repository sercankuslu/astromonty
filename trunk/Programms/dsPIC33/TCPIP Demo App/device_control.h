#ifndef __DEVICE_CONTROL_H_
#define __DEVICE_CONTROL_H_
#ifdef __C30__
#   include "GenericTypeDefs.h"
// блокировки
#define SET_INT_LOCK(x) SRbits.IPL = x
#define SAVE_INT_LOCK SRbits.IPL
#define RESTORE_INT_LOCK(x) SRbits.IPL = x
#else
#   include "stdafx.h"
// блокировки
#define SET_INT_LOCK(x)
#define SAVE_INT_LOCK 4
#define RESTORE_INT_LOCK(x)
typedef struct 
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
} _IFS0bits;
#endif 

#define LOCK(x,y) {\
    BYTE K = SAVE_INT_LOCK; \
    SET_INT_LOCK(x);\
    y;\
    RESTORE_INT_LOCK(K);\
}

#define OC_DMA_INT_LEVEL        6
#define OC_INT_LEVEL            7

#define OC1_DMA_ID              DMA0
#define OC2_DMA_ID              DMA1

#define OC1_DMA_BUF_LEN         256     //x2
#define OC2_DMA_BUF_LEN         256     //x2

#define SPI1_DMA_SEND_ID        DMA2
#define SPI1_DMA_RECEIVE_ID     DMA3
#define SPI1_DMA_INT_LEVEL      5
#define SPI1_DMA_BUF_LEN        256
#define SPI1_INT_LEVEL          6

#define SPI2_DMA_SEND_ID        DMA4
#define SPI2_DMA_RECEIVE_ID     DMA5
#define SPI2_DMA_INT_LEVEL      5
#define SPI2_DMA_BUF_LEN        256
#define SPI2_INT_LEVEL          6

#define OC_INT_LOCK         SAVE_INT_LOCK; SET_INT_LOCK(OC_INT_LEVEL)
#define OC_DMA_INT_LOCK     SAVE_INT_LOCK; SET_INT_LOCK(OC_DMA_INT_LEVEL)

// Таймеры
typedef enum _TIMERS_ID{
    TIMER1 = 0, TIMER2 = 1, TIMER3 = 2, TIMER4 = 3, TIMER5 = 4, TIMER6 = 5, TIMER7 = 6, TIMER8 = 7, TIMER9 = 8
} TIMERS_ID;
typedef enum _TMR_CLOCK_SOURCE {
    CLOCK_SOURCE_INTERNAL = 0, CLOCK_SOURCE_EXTERNAL = 1
} TMR_CLOCK_SOURCE;

typedef enum _TMR_GATED_MODE {
    GATED_DISABLE = 0, GATED_ENABLE = 1
}TMR_GATED_MODE;

typedef enum _TMR_PRESCALER {
    PRE_1_1 = 0, PRE_1_8 = 1, PRE_1_64 = 2, PRE_1_256 = 3
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

typedef struct _TMRConfigType{
    int (*CallbackFunc)(void);
} TMRConfigType;

int TimerInit(TIMERS_ID id, TMR_CLOCK_SOURCE source, TMR_GATED_MODE gated, TMR_PRESCALER pre, SYS_IDLE idle, TMR_BIT_MODE bit, TMR1_SYNC sync);
int TimerSetInt(TIMERS_ID id, BYTE Level, BOOL enabled);
int TimerSetValue(TIMERS_ID id, WORD TmrValue, WORD PRValue);
int TimerSetCallback(TIMERS_ID id, int (*CallbackFunc)(void));
void TimerSetState(TIMERS_ID id, BOOL enabled);
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
    IRQ_INT0 = 0, IRQ_IC1=1, IRQ_OC1=2, IRQ_IC2=5,  IRQ_OC2=6, IRQ_TMR2=7, IRQ_TMR3=8, IRQ_SPI1=10, IRQ_SPI2=33, IRQ_UART1_RX, IRQ_UART1_TX, IRQ_UART2_RX, IRQ_UART2_TX, IRQ_ADC1, IRQ_ADC2
} DMA_DEVICE_IRQ;

typedef struct _DMAConfigType {
    int (*fillingBufferAFunc)(void*, BYTE*, WORD);
    int (*fillingBufferBFunc)(void*, BYTE*, WORD);
    BYTE* BufA;
    BYTE* BufB;
    WORD Count;
    void* _This;
    BYTE SelectBuffer;      // номер буфера
} DMAConfigType;

WORD DMACreateConfig(DMA_DATA_SIZE_BIT size, DMA_TRANSFER_DIRECTION dir, DMA_COMPLETE_BLOCK_INT half, DMA_NULL_DATA_MODE nullw, DMA_ADRESING_MODE addr, DMA_OPERATION_MODE mode);
int DMAInit();
int DMASetConfig(DMA_ID id, WORD Config);
int DMASelectDevice(DMA_ID id, DMA_DEVICE_IRQ irq, int DEVICE_REG);
int DMASetDataCount(DMA_ID id, WORD Count);
int DMAGetBuffers(DMA_ID id, BYTE** BufferA, BYTE** BufferB);
int DMASetBuffers(DMA_ID id, BYTE* BufferA, BYTE* BufferB);
BYTE* DMAGetBuffer(WORD Count);
int DMASetCallback(DMA_ID id, void* _This, int (*fillingBufAFunc)(void*, BYTE*, WORD), int (*fillingBufBFunc)(void*, BYTE*, WORD));
int DMAPrepBuffer(DMA_ID id);
int DMAEnable(DMA_ID id);
int DMADisable(DMA_ID id);
BYTE DMACheck(DMA_ID id);
int DMAGetPPState(DMA_ID id);
int DMAForceTransfer(DMA_ID id);
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

typedef struct _OCConfigType{
    int (*CallbackFunc)(void * );
    void* _This;
} OCConfigType;

WORD OCCreateConfig(SYS_IDLE idle, OC_TMR_SELECT tmr, OC_WORK_MODE ocm);
int OCInit(OC_ID id, WORD Config);
int OCSetInt(OC_ID id, BYTE Level, BOOL enabled);
int OCSetMode(OC_ID id,OC_WORK_MODE ocm);
int OCSetCallback(OC_ID id, void* _This,  int (*CallbackFunc)(void*));
int OCSetValue(OC_ID id, WORD ocr, WORD ocrs);
int OCSetTmr(OC_ID id, OC_TMR_SELECT tmr);



typedef enum _SPI_ID{
    ID_SPI1, ID_SPI2, ID_SPI3, ID_SPI4, ID_SPI5, ID_SPI6
}SPI_ID;

typedef enum _SPI_CLOCK_MODE{
    MODE0, MODE1, MODE2, MODE3
}SPI_CLOCK_MODE;

typedef enum _SPI_CLOCK_POLARITY{
    ACTIVE_HIGH, ACTIVE_LOW
} SPI_CLOCK_POLARITY;

typedef enum _SPI_CLOCK_EDGE{
    IDLE_TO_ACTIVE, ACTIVE_TO_IDLE
} SPI_CLOCK_EDGE;

typedef enum _SPI_MODE{
    SLAVE = 0, MASTER = 1
}SPI_MODE;

typedef enum _SPI_DATA_SIZE_BIT {
    SPI_SIZE_BYTE = 0, SPI_SIZE_WORD = 1
}SPI_DATA_SIZE_BIT;

typedef enum _SPI_INPUT_PHASE{
    MIDDLE_PHASE = 0, END_PHASE = 1
}SPI_INPUT_PHASE;

typedef struct _SPIConfig{
    WORD SPISTAT;
    WORD SPICON1;
    WORD SPICON2; //фреймы не поддерживаем
} SPIConfig;

typedef enum _SPI_CLOCK_PRIMARY_PRESCALER{
    PPRE_64_1, PPRE_16_1, PPRE_4_1,PPRE_1_1
} SPI_CLOCK_PRIMARY_PRESCALER;

typedef enum _SPI_CLOCK_SECONDARY_PRESCALER{
    SPRE_8_1, SPRE_7_1, SPRE_6_1, SPRE_5_1, SPRE_4_1, SPRE_3_1, SPRE_2_1, SPRE_1_1,
} SPI_CLOCK_SECONDARY_PRESCALER;

typedef enum _Eflag
{
    TRANSFER_STOP = 0,
    SEND_DATA,
    SEND_CMD,
    SEND_END,
    RECEIVE_DATA,
    RECEIVE_CONTINUE,
    RECEIVE_END,
} EFlag;

typedef struct _SPI_STATUS 
{
    BYTE Busy;              // флаг занятости устройства
    BYTE TransferComplete;
    BYTE CurrentDevice;     
    BYTE LastIntLevel;
    int SPIDataCount;       // количество получаемых данных
    EFlag Flag;
    WORD DMASendCfg;
    WORD DMAReceiveCfg;
    BYTE* DMASendBuf;
    BYTE* DMAReceiveBuf;
    WORD DMASendBufLen;
    WORD DMAReceiveBufLen;
    BYTE* DataReceiveBuf;
    WORD DataReceiveLen;
    BYTE * DataReceiveSource;
}SPI_STATUS;

typedef struct _DEVICE_REG{
    BYTE DeviceId;
    SPI_ID id;
    SPI_STATUS * SPIStatus;
    SPIConfig Config;
    int (*DeviceSelect)(void);
    int (*DeviceRelease)(void);
    BYTE OldIntLevel;
} DEVICE_REG;

SPIConfig SPI_CreateParams(SPI_MODE Mode, SPI_CLOCK_POLARITY CKP, SPI_CLOCK_EDGE CKE, SPI_CLOCK_PRIMARY_PRESCALER PPRE, SPI_CLOCK_SECONDARY_PRESCALER SPRE,SYS_IDLE Idle,SPI_DATA_SIZE_BIT DataSize, SPI_INPUT_PHASE InputPhase);
int SPIInit();
DEVICE_REG * SPIRegisterDevice(SPI_ID id, SPIConfig Config, int (*DeviceSelect)(void), int (*DeviceRelease)(void));
WORD SPISendData( DEVICE_REG * DeviceHandle, BYTE* Cmd, WORD CmdLen, BYTE* Data, WORD DataLen );
WORD SPIReceiveData( DEVICE_REG * DeviceHandle, BYTE* Cmd, WORD CmdLen, BYTE* Data, WORD DataLen, BYTE WaitData);
WORD SPISendCmd( DEVICE_REG * DeviceHandle, BYTE* Cmd, WORD CmdLen);
void SPILock(SPI_ID id);
void SPIUnlock(SPI_ID id);
void SPIRelease(SPI_ID id);
#endif //__DEVICE_CONTROL_H_
