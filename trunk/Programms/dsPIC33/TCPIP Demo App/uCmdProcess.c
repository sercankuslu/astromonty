#include "stdafx.h"

#include "device_control.h"
#include "Queue.h"
#include "uCmdProcess.h"
#include <math.h>

#ifdef __C30__

//#   include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/SPIFlash.h"
#include "GenericTypeDefs.h"

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

xCMD_QUEUE      uCmdQueueValues1[uCMD_QUEUE_SIZE];     // values
QUEUE_ELEMENT   uCMDQueueKeys1[uCMD_QUEUE_SIZE];    // keys
xCMD_QUEUE      mCmdQueueValues1[mCMD_QUEUE_SIZE];     // values
QUEUE_ELEMENT   mCMDQueueKeys1[uCMD_QUEUE_SIZE];    // keys
OC_CHANEL_STATE OControll1;

int SetDirection(OC_ID id, BYTE Dir);

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
#endif


int uCmd_Init(void)
{
    // для тестов
    xCMD_QUEUE command;

    //                              
    // команда -> очередь команд -> мини команда -> очередь выборки -> микрокоманда -> очередь микрокоманд -> исполнение
    // 0. настроить очередь
    Queue_Init(&OControll1.uCmdQueue, uCMDQueueKeys1, uCMD_QUEUE_SIZE, (BYTE*)uCmdQueueValues1, sizeof(xCMD_QUEUE));
    Queue_Init(&OControll1.mCmdQueue, mCMDQueueKeys1, mCMD_QUEUE_SIZE, (BYTE*)mCmdQueueValues1, sizeof(xCMD_QUEUE));
    

    
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
    xCMD_QUEUE StopCommand;
    BYTE ProcessCmd = 1;

    while (ProcessCmd){

        if(Queue_First( &OCN->uCmdQueue, NULL, (BYTE**)&Value) != 0){
            // если нет команды, то останов
            StopCommand.State = xCMD_STOP;
            StopCommand.Value = (DWORD)0x0000;
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
            OCN->CurrentDirection = (BYTE)Value->Value;
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

        default:
            OCN->CurrentState = xCMD_ERROR;
            break;
        }
        if(ProcessCmd != 0)
            Queue_Delete(&OCN->uCmdQueue);
    }
    return 0;
}
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
    BYTE NeedStart = 0;

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
                OCN->mCMD_Status.AccX = (WORD)OCN->Config.AccRecordCount;
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
                BufPtr->r = (WORD)(*TmpBufPtr + OCN->T.Val);
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

                BufPtr->r = (WORD)(OCN->mCMD_Status.RUN_Interval + OCN->T.Val);
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
            // TODO: надо округлять десятичные
            OCN->mCMD_Status.RUN_Interval = (WORD)Value->Value;
            Queue_Delete(&OCN->mCmdQueue);
            break;
        case xCMD_DECELERATE:
            Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Value);
            Queue_Delete(&OCN->mCmdQueue);
            break;
//         case xCMD_SET_TIMER: // TODO: Возможно, в mCMD не нужно
//             Queue_Insert(&OCN->uCmdQueue, Priority, (BYTE*)&Value);
//             Queue_Delete(&OCN->mCmdQueue);
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
            if(OCN->CurrentState == xCMD_STOP){
                // запуск OC, если все выключено
                NeedStart = 1;
            }
            break;
        case xCMD_EMERGENCY_STOP:
            // все вырубить
            #ifdef __C30__
                PORT1_ENABLE = 1;
                PORT2_ENABLE = 1;
                PORT3_ENABLE = 1;
            #endif
            OCN->CurrentState = Value->State;
            OCSetMode(OCN->Config.OCConfig.Index, OC_DISABLED);
            DMADisable(OCN->Config.DmaId);
            ProcessCmd = 0;
            break;
        default:
            break;
        }
    }
    if(NeedStart){
        uCmd_OCCallback( _This );
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
    
    {
        WORD i;

        WORD Buf[128];
        WORD * Bufptr = Buf;
        volatile double Value = 0;
        SPIFlashBeginWrite(262144);   
        for(i = 0; i < 32000; i++){
            if(((i & 0x7f) == 0) && (i > 0)){
                SPIFlashWriteArray((BYTE*)Buf, 256);
                Bufptr = Buf;
            }
            Value = (GetInterval( ((double)i) * AppConfig.ChanellsConfig[0].dx, AppConfig.ChanellsConfig[0].U,AppConfig.ChanellsConfig[0].V)/0.0000002);            
            *Bufptr = (WORD)Value;
            Bufptr++;
        }
    }
}
