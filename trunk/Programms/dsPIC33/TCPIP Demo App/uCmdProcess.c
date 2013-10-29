#include "stdafx.h"



#ifdef __C30__

//#   include "TCPIP Stack/TCPIP.h"
#include "GenericTypeDefs.h"

#else
void FS_WriteArray(DWORD Addr, BYTE* val, WORD len);
void FS_ReadArray(DWORD Addr, BYTE* val, WORD len);
#   define SPIFlashReadArray(dwAddress, vData, wLen, WaitData)  FS_ReadArray(dwAddress, vData, wLen)
//#   define SPIFlashReadArray(dwAddress, vData, wLen, WaitData) FS_ReadArray(dwAddress, vData, wLen)

unsigned char FileSystem[64*1024*256];

#endif

#include "device_control.h"
#include "Queue.h"
#include "uCmdProcess.h"
#include "TCPIP Stack/SPIFlash.h"


xCMD_QUEUE      uCmdQueueValues1[uCMD_QUEUE_SIZE];     // values
QUEUE_ELEMENT   uCMDQueueKeys1[uCMD_QUEUE_SIZE];    // keys
xCMD_QUEUE      mCmdQueueValues1[mCMD_QUEUE_SIZE];     // values
QUEUE_ELEMENT   mCMDQueueKeys1[uCMD_QUEUE_SIZE];    // keys
OC_CHANEL_STATE OControll1;



int SetDirection(OC_ID id, BYTE Dir);

int uCmd_Init(void)
{
    // для тестов
    xCMD_QUEUE command;

    //                              
    // команда -> очередь команд -> мини команда -> очередь выборки -> микрокоманда -> очередь микрокоманд -> исполнение
    // 0. настроить очередь
    Queue_Init(&OControll1.uCmdQueue, uCMDQueueKeys1, uCMD_QUEUE_SIZE, (BYTE*)uCmdQueueValues1, sizeof(xCMD_QUEUE));
    Queue_Init(&OControll1.mCmdQueue, mCMDQueueKeys1, mCMD_QUEUE_SIZE, (BYTE*)mCmdQueueValues1, sizeof(xCMD_QUEUE));
    // 1. считать настройки из flash
    // 2. настроить OC, DMA
    // 3. 
    // 

    
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
    
    /*
    BYTE Buf[256];
    WORD * k = (WORD*)FileSystem;
    for (int i = 0; i < 256; i++) {
        *k++ = i;
    }
    OC1.Config.wSTEPPulseWidth = 10;
    OC1.Config.AccRecordCount = 256;

    command.State = xCMD_START;
    command.Value = (DWORD)0x0000;    
    Queue_Insert(&OC1.mCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_ACCELERATE;
    command.Value = (DWORD)32;    
    Queue_Insert(&OC1.mCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_DECELERATE;
    command.Value = (DWORD)0;    
    Queue_Insert(&OC1.mCmdQueue, 10, (BYTE*)&command);
    command.State = xCMD_STOP;
    command.Value = (DWORD)0x0000;    
    Queue_Insert(&OC1.mCmdQueue, 10, (BYTE*)&command);

    uCmd_DMACallback( (void*)&OC1, Buf, 256);
    */
    return 0;
}

// для команд, находящихся в буфере,
// приоритетная выборка не производится
int uCmd_OCCallback( void * _This )
{
    OC_CHANEL_STATE * OCN = (OC_CHANEL_STATE*)_This;
    xCMD_QUEUE * Value = NULL;
    BYTE ProcessCmd = 1;
    xCMD_QUEUE StopCommand;
    StopCommand.State = xCMD_STOP;
    StopCommand.Value = (DWORD)0x0000;    

    while (ProcessCmd){

        if(Queue_First( &OCN->uCmdQueue, NULL, (BYTE**)&Value) != 0){
            // если нет команды, то останов
            Value = &StopCommand;
            ProcessCmd = 0;
        }

        switch (Value->State) {

        case xCMD_ACCELERATE:
        case xCMD_RUN:
        case xCMD_DECELERATE: // значение Value - количество шагов, которое загружено в буфер
            (OCN->CurrentDirection == 0) ? OCN->XPosition++ : OCN->XPosition--;
            if(Value->Value > 0){
                Value->Value--;
                ProcessCmd = 0;
                OCN->CurrentState = Value->State;
            }
            break;
        case xCMD_SET_TIMER: // значение Value - значение типа OC_TMR_SELECT - номер таймера
            OCSetTmr(OCN->Config.OCConfig.Index, (OC_TMR_SELECT)Value->Value);
            OCN->CurrentState = Value->State;
            break;

        case xCMD_SET_DIRECTION: // значение Value - направление движения
            OCN->CurrentDirection = Value->Value;
            OCN->CurrentState = Value->State;
            SetDirection(OCN->Config.OCConfig.Index, (BYTE)Value->Value);            
            break;

        case xCMD_STOP: // остановка модуля
            OCN->CurrentState = Value->State;
            OCSetMode(OCN->Config.OCConfig.Index, OC_DISABLED);
            DMADisable(OCN->Config.DmaId);
            break;

        case xCMD_START: // запуск модуля
            DMAEnable(OCN->Config.DmaId);
            DMAForceTransfer(OCN->Config.DmaId);
            OCSetMode(OCN->Config.OCConfig.Index, OCN->Config.OCConfig.WorkMode);
            //ProcessCmd = 0;
            break;

        case xCMD_EMERGENCY_STOP:
            OCN->CurrentState = Value->State;
            OCSetMode(OCN->Config.OCConfig.Index, OC_DISABLED);
            DMADisable(OCN->Config.DmaId);
            ProcessCmd = 0;
            break;

        default:
            OCN->CurrentState = xCMD_ERROR;
            break;
        }
        if(ProcessCmd != 0)
            Queue_Delete(&OCN->uCmdQueue);
    }
    return 0;
}
#define WAIT_READ_COMPLETE 0
#define NO_WAIT_READ_COMPLETE 0
int uCmd_DMACallback( void * _This, BYTE* Buf, WORD BufLen)
{
#define TMP_SIZE 8
#define TMP_SIZE_MASK 7
#define bTMP_SIZE 16
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
    WORD Pulse = OCN->Config.wSTEPPulseWidth;
    OC_BUF * BufPtr;
    WORD DataSize = 0;
    BYTE Buf1 = 0;

    BufPtr = (OC_BUF*)Buf;

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
//             Value->Value;  // тут номер AccX до которого надо дойти
//             OCN->mCMD_Status.AccX; // тут текущий номер AccX
//             OCN->Config.AccBaseAddress; // адрес таблицы разгона
//             OCN->Config.AccRecordCount; // количество записей в таблице разгона

            // проверка на некорректные данные
            if(OCN->mCMD_Status.AccX > OCN->Config.AccRecordCount){
                OCN->mCMD_Status.AccX = OCN->Config.AccRecordCount;
            }
            if(Value->Value > OCN->Config.AccRecordCount){
                Value->Value = OCN->Config.AccRecordCount;
            }
            // получение размеров данных
            DataSize = (WORD)Value->Value - (WORD)OCN->mCMD_Status.AccX;
            if (DataSize > BufLen / 4) {
                DataSize = BufLen / 4;
            }

            Addr = OCN->Config.AccBaseAddress + OCN->mCMD_Status.AccX * sizeof(WORD);

            // получили данные в буфер1
            SPIFlashReadArray(Addr , (BYTE*)TmpBuf1, bTMP_SIZE, WAIT_READ_COMPLETE);
            Buf1 = 0;
            for (i = 0; i < DataSize; i++) {

                if((i & TMP_SIZE_MASK) == 0 ){

                    if(Buf1 == 0){
                        Addr += bTMP_SIZE;
                        // запросили следующие данные в буфер2
                        SPIFlashReadArray(Addr, (BYTE*)TmpBuf2, bTMP_SIZE, NO_WAIT_READ_COMPLETE);
                        TmpBufPtr = TmpBuf1;
                        Buf1 = 1;
                    } else {
                        Addr += bTMP_SIZE;
                        // запросили следующие данные в буфер1
                        SPIFlashReadArray(Addr, (BYTE*)TmpBuf1, bTMP_SIZE, NO_WAIT_READ_COMPLETE);
                        TmpBufPtr = TmpBuf2;
                        Buf1 = 0;
                    }
                }
                BufPtr->r = *TmpBufPtr + OCN->T.Val;
                BufPtr->rs = BufPtr->r + Pulse;
                BufPtr++;
                TmpBufPtr++;
            }
            if(DataSize > 0){
                OCN->mCMD_Status.AccX += DataSize;
                Command.State = xCMD_ACCELERATE;
                Command.Value = (DWORD)DataSize; 
                Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Command);
                BufLen -= DataSize * 4;
            } 
            if(OCN->mCMD_Status.AccX >= Value->Value) {
                Queue_Delete(&OCN->mCmdQueue);
            }
            
            if(BufLen == 0){
                ProcessCmd = 0; // буфер заполнен ( следующую команду выбирать не нужно)
            }
            break;
        case xCMD_RUN:
            // получение размеров данных
            DataSize = (WORD)Value->Value;
            if (DataSize > BufLen / 4) {
                DataSize = BufLen / 4;
            }
            Buf1 = 0;
            for (i = 0; i < DataSize; i++) {

                BufPtr->r = OCN->mCMD_Status.RUN_Interval + OCN->T.Val;
                BufPtr->rs = BufPtr->r + Pulse;
                BufPtr++;
                TmpBufPtr++;
            }
            if(DataSize > 0){
                OCN->mCMD_Status.AccX += DataSize;
                Command.State = xCMD_RUN;
                Command.Value = (DWORD)DataSize; 
                Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Command);
                BufLen -= DataSize * 4;
            } 
            if(OCN->mCMD_Status.AccX >= Value->Value) {
                Queue_Delete(&OCN->mCmdQueue);
            }

            if(BufLen == 0){
                ProcessCmd = 0; // буфер заполнен ( следующую команду выбирать не нужно)
            }
            
            break;
        case xCMD_SET_SPEED: //задаёт значение интервала для xCMD_RUN ( в случае, если не задано, используется последнее значение xCMD_ACCELERATE/xCMD_DECELERATE)
            // для uCMD не используется
            OCN->mCMD_Status.RUN_Interval = (WORD)Value->Value;
            Queue_Delete(&OCN->mCmdQueue);
            break;
        case xCMD_DECELERATE:
            Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Value);
            Queue_Delete(&OCN->mCmdQueue);
            break;
        case xCMD_SET_TIMER:
            Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Value);
            Queue_Delete(&OCN->mCmdQueue);
            break;
        case xCMD_SET_DIRECTION:
            Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Value);
            Queue_Delete(&OCN->mCmdQueue);
            break;
        case xCMD_STOP:
            Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Value);
            Queue_Delete(&OCN->mCmdQueue);
            ProcessCmd = 0;
            break;
        case xCMD_START:
            Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Value);
            Queue_Delete(&OCN->mCmdQueue);
            break;
        case xCMD_EMERGENCY_STOP:
            // все вырубить
            break;
        default:
            break;
        }
    }
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


//-------------------------------------------------------------------------
void FS_WriteArray(DWORD Addr, BYTE* val, WORD len)
    //------------------------------------------------------------------------------------------------
{
#ifdef __C30__
    SPIFlashBeginWrite(Addr);
    SPIFlashWriteArray(val, len);
#else
    memcpy(&FileSystem[Addr], val, len);
#endif

}

//------------------------------------------------------------------------------------------------
void FS_ReadArray(DWORD Addr, BYTE* val, WORD len)
    //------------------------------------------------------------------------------------------------
{
#ifdef __C30__
    SPIFlashReadArray(Addr, val, len, 1);
#else
    memcpy(val, &FileSystem[Addr], len);
#endif
}
void uCmd_DefaultConfig(CHANEL_CONFIG * Config, BYTE Number)
{
    Config->AccBaseAddress = 0x40000;
    Config->AccRecordCount = 32000; // 10 градусов
    switch(Number){
    case 0:
        Config->DmaId = DMA4;
        Config->OCConfig.Index = ID_OC1;
        break;
    case 1:
        Config->DmaId = DMA5;
        Config->OCConfig.Index = ID_OC2;
        break;
    case 2:
        Config->DmaId = DMA6;
        Config->OCConfig.Index = ID_OC3;
        break;
    }
    Config->DrvConfig.dSTEPPulseWidth = 0.000002; // 2 uS
    Config->wSTEPPulseWidth = (WORD)(Config->DrvConfig.dSTEPPulseWidth / 0.0000016);
    Config->DrvConfig.uStepPerStep = 16;
    Config->MntConfig.Length = 2.0;
    Config->MntConfig.Mass = 500.0;
    Config->MntConfig.Radius = 0.3;
    Config->MntConfig.Reduction = 360.0;
    Config->OCConfig.WorkMode = TOGGLE;
    Config->TmrId = TIMER2;
    Config->MConfig.StepPerTurn = 200;
    //TODO: Вычислить К и B на основании других параметров
    Config->MConfig.K = -3.384858359;
    Config->MConfig.B = 40.27981447;
    Config->dx = 360.0/(Config->MntConfig.Reduction * Config->DrvConfig.uStepPerStep * Config->MConfig.StepPerTurn);
}

