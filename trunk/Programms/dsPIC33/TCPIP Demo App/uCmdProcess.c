#include "stdafx.h"

#include "device_control.h"
#include "Queue.h"
#include "uCmdProcess.h"
#include <math.h>

#ifdef __C30__

#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/SPIFlash.h"
#include "GenericTypeDefs.h"
extern APP_CONFIG AppConfig;
#else
typedef enum _WAIT_READY
{
    NO_WAIT_READ_COMPLETE = 0,
    WAIT_READ_COMPLETE = 1,
} WAIT_READY;

void SPIFlashReadArray(DWORD dwAddress, BYTE* vData, WORD wLen, WAIT_READY WaitData);
void SPIFlashWriteArray(BYTE* vData, WORD wLen);
void SPIFlashBeginWrite(DWORD Addr);

APP_CONFIG AppConfig;

DWORD dwFlashAddr = 0;
unsigned char FileSystem[64*1024*256];

#endif
QUEUE_ELEMENT   CMDQueueKeys[CMD_QUEUE_SIZE];
CMD_QUEUE       CMDQueueValues[CMD_QUEUE_SIZE];
PRIORITY_QUEUE  CmdQueue;                  // очередь команд
xCMD_QUEUE      uCmdQueueValues1[uCMD_QUEUE_SIZE];     // values
QUEUE_ELEMENT   uCMDQueueKeys1[uCMD_QUEUE_SIZE];    // keys
xCMD_QUEUE      mCmdQueueValues1[mCMD_QUEUE_SIZE];     // values
QUEUE_ELEMENT   mCMDQueueKeys1[uCMD_QUEUE_SIZE];    // keys
xCMD_QUEUE      uCmdQueueValues2[uCMD_QUEUE_SIZE];     // values
QUEUE_ELEMENT   uCMDQueueKeys2[uCMD_QUEUE_SIZE];    // keys
xCMD_QUEUE      mCmdQueueValues2[mCMD_QUEUE_SIZE];     // values
QUEUE_ELEMENT   mCMDQueueKeys2[uCMD_QUEUE_SIZE];    // keys
OC_CHANEL_STATE OControll1;
OC_CHANEL_STATE OControll2;
//OC_CHANEL_STATE OControll3;

int uCmd_Init(void);
int xCmdStart(BYTE Id);
int SetDirection(OC_ID id, BYTE Dir);
int mCmd_DMACallback( void * _This, BYTE* Buf, WORD BufLen);
int uCmd_OCCallback(void * _This);
int uCmd_ICCallback(void * _This);
double GetInterval(double X, double U, double V);
int Aim(OC_CHANEL_STATE * OCN, LONG destang, BYTE priority);

#ifndef __C30__
void SPIFlashReadArray(DWORD dwAddress, BYTE* vData, WORD wLen, WAIT_READY WaitData)
{
    memcpy(vData, &FileSystem[dwAddress], wLen);
}
void SPIFlashBeginWrite(DWORD Addr)
{
    dwFlashAddr = Addr;
}
void SPIFlashWriteArray(BYTE* vData, WORD wLen)
{
    memcpy(&FileSystem[dwFlashAddr], vData, wLen);
}

extern WORD OC1R;
extern WORD OC2R;

#endif


int xCmdStart(BYTE Id)
{
    OC_CHANEL_STATE * OCN;
    if(Id == 0){
        OCN = &OControll1;
    } else if(Id == 1){
        OCN = &OControll2;
    } else return 0;

    if((OCN->CurrentState == xCMD_STOP) || (OCN->CurrentState == xCMD_EMERGENCY_STOP)){
        DMAPrepBuffer(OCN->Config->DmaId);
        OCSetMode(OCN->Config->OCConfig.Index, OCN->Config->OCConfig.WorkMode);
        DMAEnable(OCN->Config->DmaId);
        DMAForceTransfer(OCN->Config->DmaId);
        return 0;
    } else return -1;
}

int uCmd_Init(void)
{
    // для тестов
    //xCMD_QUEUE command;
    WORD DMAcfg;
    BYTE * Buf1;
    BYTE * Buf2;
    BYTE i = 0;
    OC_CHANEL_STATE * nOC;

    //                              
    // команда -> очередь команд -> мини команда -> очередь выборки -> микрокоманда -> очередь микрокоманд -> исполнение
    // 0. настроить очередь
    Queue_Init(&OControll1.uCmdQueue, uCMDQueueKeys1, uCMD_QUEUE_SIZE, (BYTE*)uCmdQueueValues1, sizeof(xCMD_QUEUE));
    Queue_Init(&OControll1.mCmdQueue, mCMDQueueKeys1, mCMD_QUEUE_SIZE, (BYTE*)mCmdQueueValues1, sizeof(xCMD_QUEUE));
    Queue_Init(&OControll2.uCmdQueue, uCMDQueueKeys2, uCMD_QUEUE_SIZE, (BYTE*)uCmdQueueValues2, sizeof(xCMD_QUEUE));
    Queue_Init(&OControll2.mCmdQueue, mCMDQueueKeys2, mCMD_QUEUE_SIZE, (BYTE*)mCmdQueueValues2, sizeof(xCMD_QUEUE));

    OControll1.Config = &AppConfig.ChanellsConfig[0];
    OControll1.CurrentDirection = 0;
    OControll2.Config = &AppConfig.ChanellsConfig[1];
    OControll2.CurrentDirection = 0;
    OControll1.Pulse = 0;
    OControll2.Pulse = 0;
    //uCmd_DefaultConfig(&AppConfig.ChanellsConfig[0], 0);
#ifdef __C30__
    T2CONbits.TON = 0;
    T3CONbits.TON = 0;
#else
    CreateAccTable();
#endif 


    TimerInit(TIMER2, CLOCK_SOURCE_INTERNAL, GATED_DISABLE, PRE_1_8, IDLE_ENABLE, BIT_16, SYNC_DISABLE);
    TimerSetValue(TIMER2, 0x0000, 0xFFFF);
    TimerSetCallback(TIMER2, NULL);
    TimerSetInt(TIMER2, 0, FALSE);

    TimerInit(TIMER3, CLOCK_SOURCE_INTERNAL, GATED_DISABLE, PRE_1_64, IDLE_ENABLE, BIT_16, SYNC_DISABLE);
    TimerSetValue(TIMER3, 0x0000, 0xFFFF);
    TimerSetCallback(TIMER3, NULL);
    TimerSetInt(TIMER3, 0, FALSE);

    //TimerSetState(TIMER3, TRUE); // сначала медленный потом быстрый
    //TimerSetState(TIMER2, TRUE);
#ifdef __C30__
    T2CONbits.TON = 1;
    T3CONbits.TON = 1;
#endif

    DMAInit();
    DMAcfg = DMACreateConfig(SIZE_WORD, RAM_TO_DEVICE, FULL_BLOCK, NORMAL_OPS, REG_INDIRECT_W_POST_INC, CONTINUE_PP);

    OCInit(ID_OC1, IDLE_DISABLE, OC_TMR2, OC_DISABLED);
    OCSetInt(ID_OC1, OC1_INT_LEVEL, TRUE);

    DMASetConfig(OC1_DMA_ID, DMAcfg);
    Buf1 = DMAGetBuffer(OC1_DMA_BUF_LEN);
    Buf2 = DMAGetBuffer(OC1_DMA_BUF_LEN);
    DMASelectDevice(OC1_DMA_ID, IRQ_OC1, (int)&OC1R);
    DMASetInt(OC1_DMA_ID, OC1_DMA_INT_LEVEL, TRUE);
    DMASetBuffers(OC1_DMA_ID, Buf1, Buf2);
    DMASetDataCount(OC1_DMA_ID, OC1_DMA_BUF_LEN);

    OCInit(ID_OC2, IDLE_DISABLE, OC_TMR2, OC_DISABLED);
    OCSetInt(ID_OC2, OC2_INT_LEVEL, TRUE);

    DMASetConfig(OC2_DMA_ID, DMAcfg);
    Buf1 = DMAGetBuffer(OC2_DMA_BUF_LEN);
    Buf2 = DMAGetBuffer(OC2_DMA_BUF_LEN);
    DMASelectDevice(OC2_DMA_ID, IRQ_OC2, (int)&OC2R);
    DMASetInt(OC2_DMA_ID, OC2_DMA_INT_LEVEL, TRUE);
    DMASetBuffers(OC2_DMA_ID, Buf1, Buf2);
    DMASetDataCount(OC2_DMA_ID, OC2_DMA_BUF_LEN);



    for (i = 0; i < 2; i++){
        if(i == 0){
            nOC = &OControll1;
        } else {
            nOC = &OControll2;
        }

        if(nOC->Config->OCConfig.Index == ID_OC1){
            OCSetCallback(ID_OC1, (void*)&OControll1,  uCmd_OCCallback);
            DMASetCallback(OC1_DMA_ID, (void*)&OControll1, mCmd_DMACallback, mCmd_DMACallback);
        } else 
        if(nOC->Config->OCConfig.Index == ID_OC2){
            OCSetCallback(ID_OC2, (void*)&OControll2,  uCmd_OCCallback);
            DMASetCallback(OC2_DMA_ID, (void*)&OControll2, mCmd_DMACallback, mCmd_DMACallback);
        }
    }

    /*
    command.State = xCMD_STOP;
    command.Value = (DWORD)0x0000;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_SET_TIMER;
    command.Value = (DWORD)OC_TMR3;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_START;
    command.Value = (DWORD)0x0000;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_ACCELERATE;
    command.Value = (DWORD)0x0001;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_RUN;
    command.Value = (DWORD)0x0001;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_DECELERATE;
    command.Value = (DWORD)0x0001;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_STOP;
    command.Value = (DWORD)0x0000;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);

    uCmd_OCCallback((void*)&OControll1);
    uCmd_OCCallback((void*)&OControll1);
    uCmd_OCCallback((void*)&OControll1);
    uCmd_OCCallback((void*)&OControll1);
    command.State = xCMD_START;
    command.Value = (DWORD)0x0000;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_ACCELERATE;
    command.Value = (DWORD)0x0000;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_DECELERATE;
    command.Value = (DWORD)0x0001;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_STOP;
    command.Value = (DWORD)0x0000;    
    Queue_Insert(&OControll1.uCmdQueue, 10, (BYTE*)&command);
    uCmd_OCCallback((void*)&OControll1);
    uCmd_OCCallback((void*)&OControll1);
    uCmd_OCCallback((void*)&OControll1);
    uCmd_OCCallback((void*)&OControll1);

    */

    /*
    // миникоманды ( заполнение буфера DMA )
    BYTE Buf[256];
    WORD * k = (WORD*)FileSystem;
    for (int i = 256; i > 0 ; i--) {
        *(k++) = i * 100;
    }
    OControll1.Config.wSTEPPulseWidth = 10;
    OControll1.Config.AccRecordCount = 256;
    */
//     command.State = xCMD_START;
//     command.Value = (DWORD)0x0000;
//     Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);
    //command.State = xCMD_ACCELERATE;
    //command.Value = (DWORD)8;          // дойти до 8 шага в разгоне
    //Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);
//     command.State = xCMD_ACCELERATE;
//     command.Value = (DWORD)60000;          // разгон до максимальной скорости
//     Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);
//     command.State = xCMD_RUN;
//     command.Value = (DWORD)1152000;          // пройти 360 градусов на скорости
//     Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);
//     command.State = xCMD_DECELERATE;
//     command.Value = (DWORD)0;           // дойти до 0 шага в торможении
//     Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);
//     //DMAPrepBuffer(OControll1.Config->DmaId);
//     command.State = xCMD_STOP;
//     command.Value = (DWORD)0x0000;
//     Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);
    //uCmd_DMACallback( (void*)&OControll1, Buf, 256);
    /*
    command.State = xCMD_SET_DIRECTION;
    command.Value = (DWORD)1;
    Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);

    command.State = xCMD_SET_TIMER;
    command.Value = (DWORD)OC_TMR3;
    Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);

    command.State = xCMD_SET_SPEED;//
    command.Value = (DWORD)46746;       // задать интервал (часовое ведение)
    Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);

    command.State = xCMD_RUN;
    command.Value = (DWORD)1152000;         // пройти 100 шагов на скорости
    Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);

    command.State = xCMD_STOP;
    Queue_Insert(&OControll1.mCmdQueue, 10, (BYTE*)&command);
    */
    /*
    for (int j = 0; j < (32 + 10 + 32 + 100); j++) {
        uCmd_DMACallback( (void*)&OControll1, Buf, 256);
        for (int i = 0; i < 64; i++) {
            uCmd_OCCallback((void*)&OControll1);
        }
    }*/
    return 0;
}

// для команд, находящихся в буфере,
// приоритетная выборка не производится
int uCmd_OCCallback( void * _This )
{
    OC_CHANEL_STATE * OCN = (OC_CHANEL_STATE*)_This;
    xCMD_QUEUE * Value = NULL;
    xCMD_QUEUE StopCommand;
    BYTE ProcessCmd = 1;

    //if((OCN->Pulse) && (OCN->CurrentState != xCMD_STOP)){
    //    OCN->Pulse = 0;
    //    return 0;
    // }
    
    switch(OCN->Config->OCConfig.Index){
    case ID_OC1: 
#ifdef __C30__
        if(PORTDbits.RD0 == 1) return 0; 
#endif
        break;
    case ID_OC2: 
#ifdef __C30__
        if(PORTDbits.RD1 == 1) return 0; 
#endif
        break;
    default:
        break;
    }

    while (ProcessCmd){

        if(Queue_First( &OCN->uCmdQueue, NULL, (BYTE**)&Value) != 0){
            // если нет команды, то останов
            if((OCN->CurrentState != xCMD_STOP) && (OCN->CurrentState != xCMD_EMERGENCY_STOP)){
                StopCommand.State = xCMD_STOP;
                StopCommand.Value = (DWORD)0x0000;
                Value = &StopCommand;
                ProcessCmd = 0;
            } else {
                return -1;
            }
        }

        switch (Value->State) {

        case xCMD_ACCELERATE:
        case xCMD_RUN:
        case xCMD_DECELERATE: // значение Value - количество шагов, которое загружено в буфер
            if(Value->Value > 0){
                (OCN->CurrentDirection == 0) ? OCN->XPosition++ : OCN->XPosition--;
                Value->Value--;
                ProcessCmd = 0;
                OCN->CurrentState = Value->State;
                OCN->Pulse = 1;
            } else {
                Queue_Delete(&OCN->uCmdQueue);
            }
            break;
        case xCMD_SET_TIMER: // значение Value - значение типа OC_TMR_SELECT - номер таймера
            OCSetTmr(OCN->Config->OCConfig.Index, (OC_TMR_SELECT)Value->Value);
            OCN->CurrentState = Value->State;
            Queue_Delete(&OCN->uCmdQueue);
            break;

        case xCMD_SET_DIRECTION: // значение Value - направление движения
            OCN->CurrentDirection = (BYTE)Value->Value;
            OCN->CurrentState = Value->State;
            SetDirection(OCN->Config->OCConfig.Index, (BYTE)Value->Value);
            Queue_Delete(&OCN->uCmdQueue);
            break;

        case xCMD_STOP: // остановка модуля
            OCN->CurrentState = Value->State;
            OCSetMode(OCN->Config->OCConfig.Index, OC_DISABLED);
            DMADisable(OCN->Config->DmaId);
            Queue_Delete(&OCN->uCmdQueue);
            break;

//         case xCMD_START: // запуск модуля
//             OCSetMode(OCN->Config->OCConfig.Index, OCN->Config->OCConfig.WorkMode);
//             DMAEnable(OCN->Config->DmaId);
//             DMAForceTransfer(OCN->Config->DmaId);
//             Queue_Delete(&OCN->uCmdQueue);
//             ProcessCmd = 0;
//             break;

        default:
            OCN->CurrentState = xCMD_ERROR;
            Queue_Delete(&OCN->uCmdQueue);
            break;
        }
    }
    return 0;
}
int mCmd_DMACallback( void * _This, BYTE* Buf, WORD BufLen)
{
#define TMP_SIZE 32
#define TMP_SIZE_MASK TMP_SIZE-1
#define bTMP_SIZE TMP_SIZE*2

    OC_CHANEL_STATE * OCN = (OC_CHANEL_STATE*)_This;
    xCMD_QUEUE * Value = NULL;
    BYTE ProcessCmd = 1;
    BYTE Priority = 0;
    xCMD_QUEUE Command;
    WORD TmpBuf1[TMP_SIZE];
    WORD TmpBuf2[TMP_SIZE];
    WORD * TmpBufPtr = NULL;
    DWORD Addr = 0;
    WORD i = 0;
    //WORD j = 0;
    WORD Pulse = OCN->Config->wSTEPPulseWidth;
    WORD * BufPtr;
    //OC_BUF * BufPtr;
    DWORD DataSize = 0;
    BYTE BufLoad = 0;
    BYTE BufProcess = 0;
    //BYTE NeedStart = 0;
    WORD BufCount = BufLen / sizeof(OC_BUF); // свободное мето в буфере в элементах ( (WORD)r + (WORD)rs )
    WORD Buf1Cnt = 0;           // количество данных в буфере 1
    WORD Buf2Cnt = 0;           // количество данных в буфере 2
    WORD TmpSize2 = 0;
    WORD TmpSize = 0;              
    BYTE FirstPass = 1;         // первый проход
    BYTE Buf1Ready = 0;         // признак готовности буфера
    BYTE Buf2Ready = 0;
    WAIT_READY Buf1Wait = NO_WAIT_READ_COMPLETE;
    WAIT_READY Buf2Wait = NO_WAIT_READ_COMPLETE;


    BufPtr = (WORD*)Buf;

    while (ProcessCmd){

        if(Queue_ExtractAllToMin( &OCN->mCmdQueue, &Priority, (BYTE**)&Value ) != 0){
            // если нет команды, то останов
            Command.State = xCMD_STOP;
            Command.Value = (DWORD)0x0000; 
            Value = &Command;
            ProcessCmd = 0;
        }

        switch (Value->State) {

        case xCMD_ACCELERATE:
            // проверка на некорректные данные 
            if(OCN->mCMD_Status.AccX > OCN->Config->AccRecordCount){
                OCN->mCMD_Status.AccX = (WORD)OCN->Config->AccRecordCount;
            }
            if(Value->Value > OCN->Config->AccRecordCount){
                Value->Value = OCN->Config->AccRecordCount;
            }
            
            if(Value->Value < OCN->mCMD_Status.AccX){
                Value->State = xCMD_DECELERATE;
                break;
             }

            // получение количества данных
            DataSize = Value->Value - OCN->mCMD_Status.AccX;
            if (DataSize > BufCount) {
                DataSize = BufCount;
            } 
            // получение адреса
            Addr = OCN->Config->AccBaseAddress + (OCN->mCMD_Status.AccX) * sizeof(WORD);

            BufLoad = 0;
            BufProcess = 0;
            Buf1Ready = 0;
            Buf2Ready = 0;
            Buf2Wait = NO_WAIT_READ_COMPLETE;
            Buf1Wait = NO_WAIT_READ_COMPLETE;
            FirstPass = 1;
            // вычисляем количество полных буферов
            if (DataSize > 0)
            {   
                TmpSize2 = DataSize;

                while(TmpSize2 > 0){
                    if((BufLoad == 0)  && (Buf1Ready == 0) && (TmpSize2 > 0)){
                        if(TmpSize2 > TMP_SIZE){
                            Buf1Cnt = TMP_SIZE;
                        } else {
                            Buf1Cnt = TmpSize2;
                        }
                        TmpSize2 -= Buf1Cnt;
                        if(TmpSize2 == 0) {
                            Buf1Wait = WAIT_READ_COMPLETE;
                            Buf1Ready = 1;
                        }
                        SPIFlashReadArray(Addr, (BYTE*)TmpBuf1, Buf1Cnt * sizeof(WORD), Buf1Wait);
                        Addr += Buf1Cnt * sizeof(WORD);
                        if(FirstPass != 1 ) Buf2Ready = 1; // готовы данные во втором буфере
                        BufLoad = 1;
                    }  
                    if((((BufLoad == 1) && (Buf2Ready == 0)) || (FirstPass == 1)) && (TmpSize2 > 0)){
                        if(TmpSize2 > TMP_SIZE){
                            Buf2Cnt = TMP_SIZE;
                        } else {
                            Buf2Cnt = TmpSize2;
                        }
                        TmpSize2 -= Buf2Cnt;
                        if(TmpSize2 == 0) {
                            Buf2Wait = WAIT_READ_COMPLETE;
                            Buf2Ready = 1;
                        }
                        SPIFlashReadArray(Addr, (BYTE*)TmpBuf2, Buf2Cnt * sizeof(WORD) , Buf2Wait);
                        Addr += Buf2Cnt * sizeof(WORD);
                        FirstPass = 0;      // первый проход
                        Buf1Ready = 1;      // готовы данные в первом буфере
                        BufLoad = 0;
                    }
                    while(Buf1Ready||Buf2Ready){
                        if(Buf1Ready && (BufProcess == 0)){
                            TmpBufPtr = TmpBuf1;
                            Buf1Ready = 0;
                            TmpSize = Buf1Cnt;
                            BufProcess = 1;
                        } else
                        if(Buf2Ready && (BufProcess == 1)){
                            TmpBufPtr = TmpBuf2;
                            Buf2Ready = 0;
                            TmpSize = Buf2Cnt;
                            BufProcess = 0;
                        }
                        while(TmpSize > 0){
                            OCN->T.Val += (WORD)(*(TmpBufPtr++));       // на flash хранятся интервалы ( в TmpBufPtr)
                            *(BufPtr++) = OCN->T.word.LW;
                            *(BufPtr++) = OCN->T.word.LW + Pulse;
                            TmpSize--;
                        }
                    }
                }
            }

            if(DataSize > 0){
                OCN->mCMD_Status.AccX += DataSize;
                Command.State = xCMD_ACCELERATE;
                Command.Value = (DWORD)DataSize; 
                Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Command);
                BufCount -= DataSize;
                OCN->mCMD_Status.RUN_Interval = (WORD)(*(TmpBufPtr - 1));   // эта переменная указывает на значение последнего интервала
            } 
            if(OCN->mCMD_Status.AccX >= Value->Value) {
                Queue_Delete(&OCN->mCmdQueue);
            }

            if(BufCount == 0){
                ProcessCmd = 0; // буфер заполнен ( следующую команду выбирать не нужно)
            }
            break;
        case xCMD_RUN:
            // получение размеров данных
            DataSize = (DWORD)Value->Value;
            if (DataSize > BufCount) {
                DataSize = BufCount;
            }
            if (DataSize > 0){
                BufLoad = 0;
                for (i = 0; i < DataSize; i++) {

                    OCN->T.Val += (WORD)(OCN->mCMD_Status.RUN_Interval);
                    *(BufPtr++) = OCN->T.word.LW;
                    *(BufPtr++) = OCN->T.word.LW + Pulse;
                }
            }
            if(DataSize > 0){
                Command.State = xCMD_RUN;
                Command.Value = (DWORD)DataSize; 
                Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Command);
                BufCount -= DataSize;
                Value->Value -= DataSize;
            } 
            if(Value->Value == 0) {
                Queue_Delete(&OCN->mCmdQueue);
            }

            if(BufCount == 0){
                ProcessCmd = 0; // буфер заполнен ( следующую команду выбирать не нужно)
            }
            
            break;
        case xCMD_SET_SPEED: //задаёт значение интервала для xCMD_RUN ( в случае, если не задано, используется последнее значение xCMD_ACCELERATE/xCMD_DECELERATE)
            // для uCMD не используется
            // TODO: надо округлять десятичные
            OCN->mCMD_Status.RUN_Interval = (WORD)Value->Value;
            Queue_Delete(&OCN->mCmdQueue);
            break;
        case xCMD_DECELERATE:
            // TODO::
            // проверка на некорректные данные 
            if(OCN->mCMD_Status.AccX > OCN->Config->AccRecordCount){
                OCN->mCMD_Status.AccX = (WORD)OCN->Config->AccRecordCount;
            }
            if(Value->Value > OCN->Config->AccRecordCount){
                Value->Value = OCN->Config->AccRecordCount;
            }

//             if(Value->Value > OCN->mCMD_Status.AccX){
//                 Value->State = xCMD_ACCELERATE;
//                 break;
//             }

            // получение количества данных
            DataSize = (DWORD)OCN->mCMD_Status.AccX - Value->Value;
            if (DataSize > BufCount) {
                DataSize = BufCount;
            }
            // получение адреса
            Addr = OCN->Config->AccBaseAddress + ((DWORD)OCN->mCMD_Status.AccX) * sizeof(WORD);

            BufLoad = 0;
            BufProcess = 0;
            FirstPass = 1;
            Buf1Ready = 0;
            Buf2Ready = 0;
            Buf2Wait = NO_WAIT_READ_COMPLETE;
            Buf1Wait = NO_WAIT_READ_COMPLETE;

            // вычисляем количество полных буферов
            if (DataSize > 0)
            {
                TmpSize2 = DataSize;

                while(TmpSize2 > 0){
                    if((BufLoad == 0)  && (Buf1Ready == 0) && (TmpSize2 > 0)){
                        if(TmpSize2 > TMP_SIZE){
                            Buf1Cnt = TMP_SIZE;
                        } else {
                            Buf1Cnt = TmpSize2;
                        }
                        Addr -= Buf1Cnt * sizeof(WORD);
                        TmpSize2 -= Buf1Cnt;
                        if(TmpSize2 == 0) {
                            Buf1Wait = WAIT_READ_COMPLETE;
                            Buf1Ready = 1;
                        }
                        SPIFlashReadArray(Addr, (BYTE*)TmpBuf1, Buf1Cnt * sizeof(WORD), Buf1Wait);
                        if(FirstPass != 1 ) Buf2Ready = 1; // готовы данные во втором буфере
                        BufLoad = 1;
                    }  
                    if((((BufLoad == 1) && (Buf2Ready == 0)) || (FirstPass == 1)) && (TmpSize2 > 0)){
                        if(TmpSize2 > TMP_SIZE){
                            Buf2Cnt = TMP_SIZE;
                        } else {
                            Buf2Cnt = TmpSize2;
                        }
                        Addr -= Buf2Cnt * sizeof(WORD);
                        TmpSize2 -= Buf2Cnt;
                        if(TmpSize2 == 0) {
                            Buf2Wait = WAIT_READ_COMPLETE;
                            Buf2Ready = 1;
                        }
                        SPIFlashReadArray(Addr, (BYTE*)TmpBuf2, Buf2Cnt * sizeof(WORD) , Buf2Wait);
                        FirstPass = 0;      // первый проход
                        Buf1Ready = 1;      // готовы данные в первом буфере
                        BufLoad = 0;
                    }
                    while(Buf1Ready||Buf2Ready){
                        if(Buf1Ready && (BufProcess == 0)){
                            TmpBufPtr = TmpBuf1 + Buf1Cnt;
                            Buf1Ready = 0;
                            TmpSize = Buf1Cnt;
                            BufProcess = 1;
                        } else 
                        if(Buf2Ready && (BufProcess == 1)){
                            TmpBufPtr = TmpBuf2 + Buf2Cnt;
                            Buf2Ready = 0;
                            TmpSize = Buf2Cnt;
                            BufProcess = 0;
                        }
                        while(TmpSize > 0){
                            OCN->T.Val += (WORD)(*(--TmpBufPtr));       // на flash хранятся интервалы ( в TmpBufPtr)
                            *(BufPtr++) = OCN->T.word.LW;
                            *(BufPtr++) = OCN->T.word.LW + Pulse;
                            TmpSize--;
                        }
                    }
                }
            }

            if(DataSize > 0){
                OCN->mCMD_Status.AccX -= DataSize;
                Command.State = xCMD_DECELERATE;
                Command.Value = (DWORD)DataSize; 
                Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Command);
                BufCount -= DataSize;
                OCN->mCMD_Status.RUN_Interval = (WORD)(*(TmpBufPtr));   // эта переменная указывает на значение последнего интервала
            } 
            if(OCN->mCMD_Status.AccX <= Value->Value) {
                Queue_Delete(&OCN->mCmdQueue);
            }

            if(BufCount == 0){
                ProcessCmd = 0; // буфер заполнен ( следующую команду выбирать не нужно)
            }
            break;
         case xCMD_SET_TIMER: // TODO: Возможно, в mCMD не нужно
             Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)Value);
             Queue_Delete(&OCN->mCmdQueue);
            break;
        case xCMD_SET_DIRECTION:
            Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)Value);
            Queue_Delete(&OCN->mCmdQueue);
            break;
        //case xCMD_BREAK:
        //    Queue_Delete(&OCN->mCmdQueue);
        //    break;
        case xCMD_STOP:
            Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)Value);
            Queue_Delete(&OCN->mCmdQueue);
            ProcessCmd = 0;
            break;
//         case xCMD_START:
//             Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)Value);
//             Queue_Delete(&OCN->mCmdQueue);
//             if(OCN->CurrentState == xCMD_STOP){
//                 // запуск OC, если все выключено
//                 NeedStart = 1;
//             }
//             break;
        case xCMD_EMERGENCY_STOP:
            // все вырубить
            #ifdef __C30__
                PORT1_ENABLE = 1;
                PORT2_ENABLE = 1;
                PORT3_ENABLE = 1;
            #endif
            OCN->CurrentState = Value->State;
            OCSetMode(OCN->Config->OCConfig.Index, OC_DISABLED);
            DMADisable(OCN->Config->DmaId);
            ProcessCmd = 0;
            break;
        default:
            break;
        }
    }
//     if(NeedStart){
//         uCmd_OCCallback( _This );
//     }
    return 0;
}

//------------------------------------------------------------------------------------------------
int SetDirection(OC_ID id, BYTE Dir)
//------------------------------------------------------------------------------------------------
{
#ifdef __C30__
    switch ( id ) {
    case ID_OC1:
        PORT1_DIR = Dir;
        break;
    case ID_OC2: 
        PORT2_DIR = Dir;
        break;
    case ID_OC3: 
        PORT3_DIR = Dir;
        break;
    case ID_OC4: 
        PORT4_DIR = Dir;
        break;
    case ID_OC5: 
        PORT5_DIR = Dir;
        break;
    case ID_OC6: 
        PORT6_DIR = Dir;
        break;
    case ID_OC7: 
        PORT7_DIR = Dir;
        break;
    case ID_OC8:
        PORT8_DIR = Dir;
    	break;
    }
#endif
    return 0;
}

void uCmd_DefaultConfig(CHANEL_CONFIG * Config, BYTE Number)
{
    
    Config->AccBaseAddress = 0x40000;
    Config->AccRecordCount = 32000; // 10 градусов
    switch(Number){
    case 0:
        Config->DmaId = OC1_DMA_ID;
        Config->OCConfig.Index = ID_OC1;
        break;
    case 1:
        Config->DmaId = OC2_DMA_ID;
        Config->OCConfig.Index = ID_OC2;
        break;
    case 2:
        Config->DmaId = OC1_DMA_ID;  // без разницы DMA у OC3 нет
        Config->OCConfig.Index = ID_OC3;
        break;
    }
    Config->DrvConfig.dSTEPPulseWidth = 0.000002; // 2 uS
    Config->wSTEPPulseWidth = 20;//(WORD)(Config->DrvConfig.dSTEPPulseWidth / 0.0000016);
    Config->DrvConfig.uStepPerStep = 16;
    Config->MntConfig.Length = 2.0;
    Config->MntConfig.Mass = 500.0;
    Config->MntConfig.Radius = 0.3;
    Config->MntConfig.Reduction = 360.0;
    Config->OCConfig.WorkMode = TOGGLE;
    Config->TmrId = TIMER2;
    Config->MConfig.StepPerTurn = 200;
    //TODO: Вычислить К и B на основании других параметров
    

    //TODO: как-то учесть трение
    {
        double I;
        // линия на графике двигателя (F1,P1)(F2,P2)
        double F1 = 100;
        double F2 = 1000;
        double P1 = 0.76;
        double P2 = 0.46;
        I = (((Config->MntConfig.Mass*Config->MntConfig.Radius*Config->MntConfig.Radius/4) + (Config->MntConfig.Mass*Config->MntConfig.Length*Config->MntConfig.Length/12))/Config->MntConfig.Reduction)/Rad_to_Grad;

        Config->MConfig.K = (P2 - P1)/(F2-F1);
        Config->MConfig.B = P1 - F1 * Config->MConfig.K;
        // предварительные вычисления. здесь характеристики двигателей.
        Config->K = Config->MConfig.K * Config->MConfig.StepPerTurn / I;  // размерность 1/сек
        Config->B = Config->MConfig.B / I; // размерность 1/сек^2
        Config->V = Config->K / Config->B;                                  // V = K/B
        Config->U = 2.0/(Config->V*Config->V*Config->B);                    // U = 2/(V^2B)
    }
    //rr->d = (-(rr->K)/(2.0 * rr->B * rr->TimerStep));
    //rr->a = (4.0 * rr->B/(rr->K * rr->K));
    //rr->OneStepCalcTime = (DWORD)(0.000160 / rr->TimerStep); //160us
    Config->dx = 360.0/(Config->MntConfig.Reduction * Config->DrvConfig.uStepPerStep * Config->MConfig.StepPerTurn);
}

double GetInterval(double X, double U, double V)
{
    //double X = AccX * Config->dx;
    return V * (-sqrt(X * (X + U)) - X);
}

void CreateAccTable()
{
    
    
        WORD i;
        
        WORD Buf[128];
        WORD * Bufptr = Buf;
        volatile DWORD Value = 0;
        volatile DWORD Value1 = 0;
        SPIFlashBeginWrite(262144);
        for(i = 0; i < 64000; i++){
            if(((i & 0x7f) == 0) && (i > 0)){
                SPIFlashWriteArray((BYTE*)Buf, 256);
                Bufptr = Buf;
                //Nop();
            }
            Value = (DWORD)(GetInterval( ((double)i) * AppConfig.ChanellsConfig[0].dx + AppConfig.ChanellsConfig[0].dx, AppConfig.ChanellsConfig[0].U,AppConfig.ChanellsConfig[0].V)/0.0000002);
            *Bufptr = (WORD)(Value - Value1);
            Value1 = Value;
            Bufptr++;
        }
    
}
LONG GetAngle(BYTE ch)
{
    volatile LONG value = 0;
    switch (ch)
    {
    case 0: value = OControll1.XPosition;
        break;
    case 1: value = OControll2.XPosition;
        break;
    case 3: value = 0;
        break;
    default:
        value = 0;
    }
    return value;
}

DWORD GetStepsPerGrad(BYTE ch)
{
    volatile DWORD value = 0;
    switch (ch)
    {
    case 0: value = (OControll1.Config->MntConfig.Reduction * OControll1.Config->DrvConfig.uStepPerStep * OControll1.Config->MConfig.StepPerTurn)/360;
        break;
    case 1: value = (OControll2.Config->MntConfig.Reduction * OControll2.Config->DrvConfig.uStepPerStep * OControll2.Config->MConfig.StepPerTurn)/360;
        break;
    case 3: value = 0;
        break;
    default:
        value = 0;
    }
    return value;
}
BYTE GetStatus(BYTE ch){
    volatile BYTE value = 0;
    switch (ch)
    {
    case 0: value = (OControll1.CurrentState);
        break;
    case 1: value = (OControll2.CurrentState);
        break;
    case 3: value = 0;//(OControll3.CurrentState);
        break;
    default:
        value = 0;
    }
    return value;
}

int Cmd_Init(void)
{
    Queue_Init(&CmdQueue, CMDQueueKeys, CMD_QUEUE_SIZE, (BYTE*)CMDQueueValues, sizeof(CMD_QUEUE));
    return uCmd_Init();
}

int Cmd_Process()
{
    CMD_QUEUE * Value = NULL;
    //CMD_QUEUE StopCommand;
    xCMD_QUEUE mCmd;
    //BYTE ProcessCmd = 1;
    BYTE Priority = 50;
    static double GuideTime = 100.0; // 100 секунд

    if(Queue_ExtractAllToMin( &CmdQueue, &Priority, (BYTE**)&Value ) != 0){
        // если нет команды, выход
        return -1;
    }

    switch (Value->Command) {

    case CMD_STOP:                                  // остановка
        mCmd.State = xCMD_STOP;
        Queue_Insert(&OControll1.mCmdQueue, Priority, (BYTE*)&mCmd);
        Queue_Insert(&OControll2.mCmdQueue, Priority, (BYTE*)&mCmd);
        Queue_Delete(&CmdQueue);
        break;
    case CMD_GO_TO_POSITION:                        // перейти на позицию; параметры: a, d,
        // останавливаем
        if((OControll1.CurrentState != xCMD_STOP) || (OControll2.CurrentState != xCMD_STOP)) {

            if(OControll1.CurrentState != xCMD_STOP){
                mCmd.State = xCMD_DECELERATE;
                mCmd.Value = 0;
                Queue_Insert(&OControll1.mCmdQueue, Priority - 1, (BYTE*)&mCmd);
            }
            if(OControll2.CurrentState != xCMD_STOP){
                mCmd.State = xCMD_DECELERATE;
                mCmd.Value = 0;
                Queue_Insert(&OControll2.mCmdQueue, Priority - 1, (BYTE*)&mCmd);
            }
            break;
        }
        // вычисляем и запускаем
        if(Aim(&OControll1, (LONG)(Value->a), Priority) == 0){
            xCmdStart(0);
        }
        if(Aim(&OControll2, (LONG)(Value->d), Priority) == 0){
            xCmdStart(1);
        }

        Queue_Delete(&CmdQueue);
        break;
    case CMD_GO_TO_POSITION_AND_GUIDE_STAR:         // перейти на позицию и сопровождать звезду ( обычное часовое ведение) a, d,
        Queue_Delete(&CmdQueue);
        break;
    case CMD_SET_GUIDE_TIME:                        // установить время сопровождения объекта a ( в сек )
        GuideTime = (double)Value->a;
        Queue_Delete(&CmdQueue);
        break;
    case CMD_GO_HOME:                               // перевести телескоп в положение парковки  
        Queue_Delete(&CmdQueue);
        break;
    case CMD_SLOW_GO_HOME:                          // медленное возвращение на парковочную позицию после возобновления электропитания
        Queue_Delete(&CmdQueue);
        break;
    case CMD_SET_AXIS:                              // калибровка ( устанавливает текущее положение телескопа) a, d
        OControll1.XPosition = Value->a;
        OControll2.XPosition = Value->d;
        Queue_Delete(&CmdQueue);
        break;
    case CMD_SET_GUIDE_SPEED:                       // калибровка ( устанавливает скорость сопровождения звезды) guidespeed
        OControll1.Config->GuideSpeed = Value->a1;
        OControll2.Config->GuideSpeed = Value->d1;
        Queue_Delete(&CmdQueue);
        break;
    case CMD_SET_AIM_SPEED:                         // калибровка ( устанавливает скорость наведения) runspeed
        Queue_Delete(&CmdQueue);
        break;
    case CMD_EMG_STOP:                              // аварийная остановка
        mCmd.State = xCMD_EMERGENCY_STOP;
        Queue_Insert(&OControll1.mCmdQueue, 0, (BYTE*)&mCmd);
        Queue_Insert(&OControll2.mCmdQueue, 0, (BYTE*)&mCmd);
        Queue_Delete(&CmdQueue);
        break;
    default:
        Queue_Delete(&CmdQueue);
        break;
    }
    return 0;
}

int Aim(OC_CHANEL_STATE * OCN, LONG destang, BYTE Priority)
{
    
    LONG diff;
    LONG diff_2;
    xCMD_QUEUE mCmd;
    /*
    if(destang < 0) {
        destang += 1152000;
    } else if(destang > 1152000){
        destang -= 1152000;
    }
    if (OCN->XPosition < 0) {
        OCN->XPosition += 1152000;
    }else if(OCN->XPosition > 1152000){
        OCN->XPosition -= 1152000;
    }*/
    
    diff = destang - OCN->XPosition;

    // определяем направление движения
    if(diff < 0){
        diff = -diff;
        mCmd.State = xCMD_SET_DIRECTION;
        mCmd.Value = 1;
        Queue_Insert(&OCN->mCmdQueue, Priority - 1, (BYTE*)&mCmd);
    } else {
        mCmd.State = xCMD_SET_DIRECTION;
        mCmd.Value = 0;
        Queue_Insert(&OCN->mCmdQueue, Priority - 1, (BYTE*)&mCmd);
    }
    /*
    mCmd.State = xCMD_SET_SPEED;
    mCmd.Value = 156;
    Queue_Insert(&OCN->mCmdQueue, Priority, (BYTE*)&mCmd);
    mCmd.State = xCMD_RUN;
    mCmd.Value = diff;
    Queue_Insert(&OCN->mCmdQueue, Priority, (BYTE*)&mCmd);
    */
    
    diff_2 = diff / 2;
    if(diff_2 < OCN->Config->AccRecordCount){
        mCmd.State = xCMD_ACCELERATE;
        mCmd.Value = diff_2;
        Queue_Insert(&OCN->mCmdQueue, Priority, (BYTE*)&mCmd);
        mCmd.State = xCMD_DECELERATE;
        mCmd.Value = 0;
        Queue_Insert(&OCN->mCmdQueue, Priority, (BYTE*)&mCmd);
        mCmd.State = xCMD_STOP;
        Queue_Insert(&OCN->mCmdQueue, Priority + 1, (BYTE*)&mCmd);
    } else {
        mCmd.State = xCMD_ACCELERATE;
        mCmd.Value = OCN->Config->AccRecordCount;
        Queue_Insert(&OCN->mCmdQueue, Priority, (BYTE*)&mCmd);
        mCmd.State = xCMD_RUN;
        mCmd.Value = diff - OCN->Config->AccRecordCount * 2;
        Queue_Insert(&OCN->mCmdQueue, Priority, (BYTE*)&mCmd);
        mCmd.State = xCMD_DECELERATE;
        mCmd.Value = 0;
        Queue_Insert(&OCN->mCmdQueue, Priority, (BYTE*)&mCmd);
        mCmd.State = xCMD_STOP;
        Queue_Insert(&OCN->mCmdQueue, Priority + 1, (BYTE*)&mCmd);
    }
    return 0;
}

