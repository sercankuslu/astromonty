#include "stdafx.h"
#include "protocol.h"
#include "..\dsPIC33\TCPIP Demo App\OCTimer.h"
#ifndef __C30__

#   include "..\PIC18F97J60\TCPIP Demo App\Control.h"
extern ALL_PARAMS Params;
#endif

#ifdef USE_PROTOCOL_CLIENT
#define MAX_CONNECTIONS 1
#endif
#ifdef USE_PROTOCOL_SERVER
#define MAX_CONNECTIONS 5
extern RR rr1;
extern RR rr2;
extern RR rr3;
#endif




typedef struct ST_CONNECTION {
    ST_STATUS Mode;
}ST_CONNECTION;

#ifndef __C30__
static char Login[] = "root";
static char Password[] = "pass";

#endif
#ifndef __18CXX
ST_CONNECTION Connections[MAX_CONNECTIONS];
#endif
#ifndef _WINDOWS
extern DWORD_VAL CPUSPEED;
#   define ULtoA(x,y) ultoa(x, y)
#else
#   define ULtoA(x,y) ultoa(CPUSPEED.Val, (char*)vTime,10)
   DWORD_VAL CPUSPEED;  
   BYTE LED1_IO;
   AppConfigType AppConfig;

#endif
   static BOOL ClientConnected = FALSE;
   //static BOOL ServerConnected = FALSE;

#ifndef __18CXX
ST_RESULT ProtocolInit()
{
    BYTE i;
    for(i = 0; i < MAX_CONNECTIONS; i++){
        Connections[i].Mode = STS_NO_CONNECT;
    }    
    return STR_OK;
}
#endif

/******************************************************************************
*
*
*
*
*
*
******************************************************************************/
#ifdef USE_PROTOCOL_SERVER
ST_RESULT RunServer(BYTE bConnectionID, BYTE* pbBlob, int pbBlobSize, int* pbBlobLen)
{
    ST_RESULT res = STR_OK;
    ST_COMMANDS Command = STC_NO_COMMANDS;    
    ST_FLAGS Answers = STF_OK;
    BOOL AuthChecked = TRUE;
    float d1;
    float Target = 0.0;
    float Speed = 0.0;
    BYTE Len = 0;
    BYTE * Answer = NULL;
    char * pLogin = NULL;
    char * pPassword = NULL;
    BYTE * pPointer;
    ST_ATTRIBUTE_TYPE Attribute = STA_NULL;
    void * Value = NULL;
    static BYTE pbOutBlob[64];
    RR * rr = NULL;
    
    if(*pbBlobLen == 0) return STR_OK;
    if((*pbBlobLen < 0)||(*pbBlobLen >= pbBlobSize)) {
        Connections[bConnectionID].Mode = STS_NO_CONNECT;
        return STR_NEED_DISCONNECT;
    }
       
    res = CheckBlob(pbBlob, *pbBlobLen);
    if(res != STR_OK) {
        Connections[bConnectionID].Mode = STS_NO_CONNECT;
        return res;  
    }
    res = FindAttribute(pbBlob, *pbBlobLen, STA_COMMAND, &Len, &Answer);
    if(res != STR_OK){
        Connections[bConnectionID].Mode = STS_NO_CONNECT;
        return STR_COMMAND_UNKNOWN;
    }
    if(Len == sizeof(BYTE)){
        Command = (ST_COMMANDS)*Answer;  
    }
   
    switch(Connections[bConnectionID].Mode) {
    case STS_NO_CONNECT:
        res = BeginCommand(pbOutBlob, sizeof(pbOutBlob), STC_CONNECT_ANSWER);
        if(res != STR_OK){
            Answers = STF_DECLINED;
            Connections[bConnectionID].Mode = STS_NO_CONNECT;
            break;
        }
        if(Command == STC_REQEST_CONNECT) {
            Answers = STF_AUTH_NEEDED;
            Connections[bConnectionID].Mode = STS_AUTH_REQ;
        } else {
            Answers = STF_DECLINED; 
        }
        
        break;
    case STS_AUTH_REQ:
        res = BeginCommand(pbOutBlob, sizeof(pbOutBlob), STC_AUTH_ANSWER);
        if(res != STR_OK){
            Answers = STF_DECLINED;
            Connections[bConnectionID].Mode = STS_NO_CONNECT;
            break;
        }
        if(Command == STC_REQEST_AUTH) {
            res = FindAttribute(pbBlob, *pbBlobLen, STA_LOGIN, &Len, (BYTE**)&pLogin);
            if(res != STR_OK){
                Connections[bConnectionID].Mode = STS_NO_CONNECT;
                Answers = STF_COMMAND_INCOMPLETE;                
                break;
            }
            res = FindAttribute(pbBlob, *pbBlobLen, STA_PASSWORD, &Len, (BYTE**)&pPassword);
            if(res != STR_OK){
                Connections[bConnectionID].Mode = STS_NO_CONNECT;
                Answers = STF_COMMAND_INCOMPLETE;
                break;
            }
            {
                //strcmp();
                if(AuthChecked){ // authority check
                    Answers = STF_ACCEPTED;
                    Connections[bConnectionID].Mode = STS_CONNECTED;
                } else {
                    Answers = STF_DECLINED; 
                    Connections[bConnectionID].Mode = STS_NO_CONNECT;
                }
            }
        } else {
            Answers = STF_DECLINED; 
            Connections[bConnectionID].Mode = STS_NO_CONNECT;
        }
        break;   
    case STS_CONNECTED:
        LED1_IO = 1;

        switch(Command){
            case STC_REQEST_DATA:
                pPointer = NULL;
                res = BeginCommand(pbOutBlob, sizeof(pbOutBlob), STC_SEND_DATA);
                if(res != STR_OK){
                    Answers = STF_DATA_ERROR;
                    Connections[bConnectionID].Mode = STS_NO_CONNECT;
                    break;
                }
                do{
                    res = GetNextAttribute(pbBlob, *pbBlobLen, &pPointer, &Attribute, &Len, (BYTE**)&Value);
                    if(res == STR_OK) {
                        switch (Attribute){
                        case STA_ALPHA:
                            {   
                                d1 = (float)(rr1.XPosition * rr1.dx);                                 
                                res = AddAttribute(pbOutBlob, sizeof(pbOutBlob), STA_ALPHA, sizeof(float), (BYTE*)&d1);
                                if(res != STR_OK){
                                    Answers = STF_DATA_ERROR;
                                    Connections[bConnectionID].Mode = STS_NO_CONNECT;
                                    break;
                                }
                                Answers = STF_DATA_READY;                                    
                            }
                            break;
                        case STA_DELTA:
                            {   
                                d1 = (float)(rr2.XPosition * rr2.dx);                                 
                                res = AddAttribute(pbOutBlob, sizeof(pbOutBlob), STA_DELTA, sizeof(float), (BYTE*)&d1);
                                if(res != STR_OK){
                                    Answers = STF_DATA_ERROR;
                                    Connections[bConnectionID].Mode = STS_NO_CONNECT;
                                    break;
                                }
                                Answers = STF_DATA_READY;                                    
                            }
                            break;
                        case STA_GAMMA:
                            {   
                                d1 = (float)(rr3.XPosition * rr3.dx);                                 
                                res = AddAttribute(pbOutBlob, sizeof(pbOutBlob), STA_GAMMA, sizeof(float), (BYTE*)&d1);
                                if(res != STR_OK){
                                    Answers = STF_DATA_ERROR;
                                    Connections[bConnectionID].Mode = STS_NO_CONNECT;
                                    break;
                                }
                                Answers = STF_DATA_READY;                                    
                            }
                            break;
                        default:;
                        }
                    }
                } while (res != STR_ATTR_NOT_FOUND);               
                res = STR_OK;
                break;
            case STC_SEND_DATA:
                res = STR_OK;
                break;
            case STC_EXECUTE_COMMAND: // *************************************************************************
                {
                    pPointer = NULL;
                    res = BeginCommand(pbOutBlob, sizeof(pbOutBlob), STC_EXECUTE_ANSWER);
                    if(res != STR_OK){
                        Answers = STF_DATA_ERROR;
                        Connections[bConnectionID].Mode = STS_NO_CONNECT;
                        break;
                    }
                    Target = 0.0;
                    Speed = 0.0;
                    do{              
                        res = GetNextAttribute(pbBlob, *pbBlobLen, &pPointer, &Attribute, &Len, (BYTE**)&Value);
                        switch (Attribute){
                        case STA_ALPHA:
                            rr = &rr1;                                                       
                            break;
                        case STA_DELTA:
                            rr = &rr2;                            
                            break;
                        case STA_GAMMA:
                            rr = &rr3;                            
                            break;
                        case STA_ABS_TARGET:
                            Target = *((float*)Value);
                            break;
                        case STA_ASTRO_TARGET:
                            Target = *((float*)Value);
                            //StarToAbs( Target, &Target);
                            break;
                        case STA_SPEED:
                            Speed = *((float*)Value);
                            break;
                        case STA_START:
                            GoToCmd(rr, Speed, Target, 0); 
                            break;
                        default: break;
                        }                        
                    } while(res != STR_ATTR_NOT_FOUND);
                }              
                res = STR_OK;
                break;
            default :
                res = BeginCommand(pbOutBlob, sizeof(pbOutBlob), STC_NO_COMMANDS);
                if(res != STR_OK){
                    Answers = STF_DECLINED;
                    Connections[bConnectionID].Mode = STS_NO_CONNECT;
                    break;
                }
                Answers = STF_COMMAND_UNKNOWN;
            }
        break;
    default:
        Connections[bConnectionID].Mode = STS_NO_CONNECT;
        res = STR_FUNCTION_FAILED;  
    }   
    res = AddFlag(pbOutBlob, sizeof(pbOutBlob), Answers);
    if(res != STR_OK){        
        Connections[bConnectionID].Mode = STS_NO_CONNECT;
        return res;
    }
    res = GetCommandLength(pbOutBlob, sizeof(pbOutBlob), pbBlobLen);
    if(res == STR_OK) {
        memcpy( pbBlob, pbOutBlob, *pbBlobLen);
    } else return res;
   
    if(Connections[bConnectionID].Mode == STS_NO_CONNECT) {
        res = STR_NEED_DISCONNECT;
    } else {
        res = STR_NEED_ANSWER;
    }
    return res;
}
#endif //USE_PROTOCOL_SERVER

/******************************************************************************
* Function				RunClient 
* Params:
* 	pbBlob				data buffer
*	bBlobLen			maximum size of data buffer
*	pbDataLength		reseived/sended data in buffer
* return:			
*	STR_OK  			if all ok
*	STR_NEED_ANSWER		if need send buffer
*	STR_NEED_DISCONNECT	if need disconnect socket
******************************************************************************/
#ifdef USE_PROTOCOL_CLIENT
ST_RESULT  RunClient(BYTE* pbBlob, int bBlobLen, int *pbDataLength)
{
    static  enum ST_STATE{
        ST_REQUEST_CONNECT = 0,
        ST_WAIT_CONNECT,
        ST_REQUEST_AUTH,
        ST_WAIT_AUTH,
        ST_CONNECTED
    } ST_STATE = ST_REQUEST_CONNECT;
    ST_RESULT  res = STR_OK;
    BYTE* Answer; 
    BOOL DoWork = TRUE;
    BYTE Len = 0;
    float * fData = NULL;
    BYTE bNull = 0;
    BYTE * pPointer;
    ST_ATTRIBUTE_TYPE Attribute = STA_NULL;
    void * Value = NULL;    
    ST_COMMANDS Command = STC_NO_COMMANDS;
    
    if(*pbDataLength < 0){
        ST_STATE = ST_REQUEST_CONNECT;
        ClientConnected = FALSE;
        return STR_NEED_DISCONNECT;
    }
    if(*pbDataLength > 0){        
        res = CheckBlob(pbBlob, bBlobLen);
        if(res != STR_OK) {
            ST_STATE = ST_REQUEST_CONNECT;
            ClientConnected = FALSE;
            return STR_NEED_DISCONNECT;
        }
        res = FindAttribute(pbBlob, bBlobLen, STA_COMMAND, &Len, &Answer);
        if(res != STR_OK){
            ST_STATE = ST_REQUEST_CONNECT;
            ClientConnected = FALSE;       
            return STR_COMMAND_UNKNOWN;
        }
        if(Len == sizeof(BYTE)){
            Command = (ST_COMMANDS)*Answer;  
        }
    } else 
        Command = STC_NO_COMMANDS;
   
    while(DoWork){
        DoWork = FALSE;
        switch(ST_STATE){
        case ST_REQUEST_CONNECT:
            res = BeginCommand(pbBlob, bBlobLen, STC_REQEST_CONNECT);
            if(res != STR_OK) 
                return STR_NEED_DISCONNECT;            
            res = STR_NEED_ANSWER;
            ST_STATE = ST_WAIT_CONNECT;
            break;
        case ST_WAIT_CONNECT:
            if(Command != STC_CONNECT_ANSWER){
                return STR_OK;
            }
            res = FindAttribute(pbBlob, bBlobLen, STA_FLAG, &Len, &Answer);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }
            if(Len == sizeof(BYTE))
            {
                switch(*Answer){
                case STF_AUTH_NEEDED:
                    ST_STATE = ST_REQUEST_AUTH;
                    DoWork = TRUE;
                    break;
                case STF_AUTH_NOT_NEEDED:
                case STF_ACCEPTED:
                    ST_STATE = ST_CONNECTED;
                    break;
                default:
                    ST_STATE = ST_REQUEST_CONNECT;
                    break;
                }
            }
            break;
        case ST_REQUEST_AUTH:            
            res = BeginCommand(pbBlob, bBlobLen, STC_REQEST_AUTH);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }           
            res = AddAttribute(pbBlob, bBlobLen, STA_LOGIN, (BYTE)sizeof(Login), (BYTE*)Login);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }           
            res = AddAttribute(pbBlob, bBlobLen, STA_PASSWORD, (BYTE)sizeof(Password), (BYTE*)Password);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }
            res = STR_NEED_ANSWER; 
            ST_STATE = ST_WAIT_AUTH;
            break;
        case ST_WAIT_AUTH:
            if(Command != STC_AUTH_ANSWER){
                return STR_OK;
            }
            res = FindAttribute(pbBlob, bBlobLen, STA_FLAG, &Len, &Answer);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }           
            if(Len == sizeof(BYTE)){                
                switch(*Answer){                
                case STF_ACCEPTED:
                    ST_STATE = ST_CONNECTED;
                    break;
                default:
                    ST_STATE = ST_REQUEST_CONNECT;
                    break;
                }
            }
            break;
        case ST_CONNECTED:
            // получение данных
            pPointer = NULL;
            switch(Command) {
            case STC_SEND_DATA:
                do{
                    res = GetNextAttribute(pbBlob, bBlobLen, &pPointer, &Attribute, &Len, (BYTE**)&fData);                    
                    if(res == STR_OK) {
                        if(Len == sizeof(float)){
                            switch (Attribute){
                            case STA_ALPHA:
                                Params.Alpha.Angle = *fData;                                
                                Params.Alpha.AngleFlag.IsModified = 1;
                                break;
                            case STA_DELTA:
                                Params.Delta.Angle = *fData;
                                Params.Delta.AngleFlag.IsModified = 1;                               
                                break;
                            case STA_GAMMA:
                                Params.Gamma.Angle = *fData;
                                Params.Gamma.AngleFlag.IsModified = 1;                                
                                break;
                            }
                        }
                    }
                } while (res != STR_ATTR_NOT_FOUND);
                if(res == STR_ATTR_NOT_FOUND) res = STR_OK;
                break;
            case STC_CONNECT_ANSWER:
            case STC_AUTH_ANSWER:
                ST_STATE = ST_REQUEST_CONNECT;
                return STR_NEED_DISCONNECT;
                break;
            default: ;
            }
            // отправка команд
            if(Params.AlphaFlag.NeedToCommit||Params.DeltaFlag.NeedToCommit||Params.GammaFlag.NeedToCommit){
                res = BeginCommand(pbBlob, bBlobLen, STC_EXECUTE_COMMAND);
                if(res != STR_OK){
                    ST_STATE = ST_REQUEST_CONNECT;
                    break;
                }
                do {
                    if(Params.AlphaFlag.NeedToCommit){
                        if(Params.Alpha.TargetAngleFlag.NeedToCommit){
                            // выбираем канал ALPHA
                            res = AddAttribute(pbBlob, bBlobLen, STA_ALPHA, 0, &bNull); 
                            if(res != STR_OK){
                                ST_STATE = ST_REQUEST_CONNECT;
                                break;
                            }
                            Value = &Params.Alpha.TargetAngle;
                            Params.Alpha.TargetAngleFlag.NeedToCommit = 0;
                            
                        }
                        Params.AlphaFlag.NeedToCommit = 0;
                    } else
                    if(Params.DeltaFlag.NeedToCommit){
                        if(Params.Delta.TargetAngleFlag.NeedToCommit){
                            // выбираем канал ALPHA
                            res = AddAttribute(pbBlob, bBlobLen, STA_DELTA, 0, &bNull); 
                            if(res != STR_OK){
                                ST_STATE = ST_REQUEST_CONNECT;
                                break;
                            }
                            Value = &Params.Delta.TargetAngle;
                            res = STR_NEED_ANSWER; 
                            Params.Delta.TargetAngleFlag.NeedToCommit = 0;
                        }
                        Params.DeltaFlag.NeedToCommit = 0;
                    } else
                    if(Params.DeltaFlag.NeedToCommit){
                        if(Params.Gamma.TargetAngleFlag.NeedToCommit){
                            // выбираем канал ALPHA
                            res = AddAttribute(pbBlob, bBlobLen, STA_GAMMA, 0, &bNull); 
                            if(res != STR_OK){
                                ST_STATE = ST_REQUEST_CONNECT;
                                break;
                            }
                            Value = &Params.Gamma.TargetAngle;
                            res = STR_NEED_ANSWER; 
                            Params.Gamma.TargetAngleFlag.NeedToCommit = 0;
                            
                        }
                        Params.GammaFlag.NeedToCommit = 0;
                    }
                    // задаем координаты
                    res = AddAttribute(pbBlob, bBlobLen, STA_ABS_TARGET, (BYTE)sizeof(float), (BYTE*)Value);
                    if(res != STR_OK){
                        ST_STATE = ST_REQUEST_CONNECT;
                        break;
                    }
                    // подтверждаем начало выполнения
                    res = AddAttribute(pbBlob, bBlobLen, STA_START, 0, &bNull);
                    if(res != STR_OK){
                        ST_STATE = ST_REQUEST_CONNECT;
                        break;
                    }
                } while (Params.AlphaFlag.NeedToCommit||Params.DeltaFlag.NeedToCommit||Params.GammaFlag.NeedToCommit);
                res = STR_NEED_ANSWER;
                break;
            } 
            // отправка запросов на данные 
            // все переменные, полученные из блоба должны быть сохранены в других местах!!!            
            if(Params.AlphaFlag.NeedToUpdate||Params.DeltaFlag.NeedToUpdate||Params.GammaFlag.NeedToUpdate){
                res = BeginCommand(pbBlob, bBlobLen, STC_REQEST_DATA);
                if(res != STR_OK){
                    ST_STATE = ST_REQUEST_CONNECT;
                    break;
                }        
                if(Params.AlphaFlag.NeedToUpdate){
                    if(Params.Alpha.AngleFlag.NeedToUpdate){ 
                        res = AddAttribute(pbBlob, bBlobLen, STA_ALPHA, 0, &bNull);
                        if(res != STR_OK){
                            ST_STATE = ST_REQUEST_CONNECT;
                            break;
                        } 
                        Params.Alpha.AngleFlag.NeedToUpdate = 0;
                    }
                    Params.AlphaFlag.NeedToUpdate = 0;
                }
                if(Params.DeltaFlag.NeedToUpdate){
                    if(Params.Delta.AngleFlag.NeedToUpdate){ 
                        res = AddAttribute(pbBlob, bBlobLen, STA_DELTA, 0, &bNull);
                        if(res != STR_OK){
                            ST_STATE = ST_REQUEST_CONNECT;
                            break;
                        } 
                        Params.Delta.AngleFlag.NeedToUpdate = 0;
                    }
                    Params.DeltaFlag.NeedToUpdate = 0;
                }
                if(Params.GammaFlag.NeedToUpdate){
                    if(Params.Gamma.AngleFlag.NeedToUpdate){ 
                        res = AddAttribute(pbBlob, bBlobLen, STA_GAMMA, 0, &bNull);
                        if(res != STR_OK){
                            ST_STATE = ST_REQUEST_CONNECT;
                            break;
                        } 
                        Params.Gamma.AngleFlag.NeedToUpdate = 0;
                    }
                    Params.GammaFlag.NeedToUpdate = 0;
                }
                res = STR_NEED_ANSWER;
            }
            
            break;
        default: 
            ST_STATE = ST_REQUEST_CONNECT;
        };        
    }
    if(ST_STATE == ST_CONNECTED){        
        if(!Params.Local.Status)
            Params.Local.StatusFlag.IsModified = 1;
        Params.Local.Status = 1;
    } else {
        ClientConnected = 0;
        if(Params.Local.Status)
            Params.Local.StatusFlag.IsModified = 1;
        Params.Local.Status = 0;    
    }

    if(res == STR_NEED_ANSWER){
        res = AddFlag(pbBlob, bBlobLen, STF_OK);
        if(res != STR_OK){
            ST_STATE = ST_REQUEST_CONNECT;
            return res;
        }
        res = GetCommandLength(pbBlob, bBlobLen, pbDataLength);
        res = STR_NEED_ANSWER;
    } else {
        *pbDataLength = 0;
    }
    return res;
}
#endif // USE_PROTOCOL_CLIENT

ST_RESULT BeginCommand(BYTE * pBuffer, int bBufLen, ST_COMMANDS Value)
{
    // очищаем буфер
    memset(pBuffer, 0, bBufLen);    
    // записываем код команды
    return AddAttribute(pBuffer, bBufLen, STA_COMMAND, 1, (BYTE*)&Value);    
}
ST_RESULT AddFlag(BYTE * pBuffer, int bBufLen, ST_FLAGS Value)
{
    return AddAttribute(pBuffer, bBufLen, STA_FLAG, 1, (BYTE*)&Value);    
}
ST_RESULT AddAttribute(BYTE * pBuffer, int bBufLen, ST_ATTRIBUTE_TYPE Attribute, BYTE bValueLen, BYTE * Value)
{
    BYTE * pPointer = NULL;
    //TODO: сделать проверки
    if(bBufLen <= 0) return STR_BUFFER_TOO_SMALL;
    pPointer = pBuffer + pBuffer[0] + 1;
    // записываем код команды
    *pPointer++ = (BYTE)Attribute;    
    *pPointer++ = (BYTE)bValueLen;    
    memcpy(pPointer, Value, bValueLen);
    // в первом байте хранится размер всего блока
    pBuffer[0] += sizeof((BYTE)Attribute) + sizeof((BYTE)bValueLen) + bValueLen;
    return STR_OK;
}
ST_RESULT CheckBlob(BYTE * pBuffer, int bBufLen)
{
    BYTE i;    
    if((bBufLen < 0 ) || (pBuffer[0] > bBufLen)) return STR_DATA_CORRUPTED;
    if(bBufLen == 0) return STR_BUFFER_TOO_SMALL;
    for(i = 1; i < pBuffer[0] + 1; i++){
        i++;
        i += pBuffer[i];
        if((i > pBuffer[0] + 1)||(i > bBufLen)) return STR_DATA_CORRUPTED;
    }
    return STR_OK;
}
ST_RESULT FindAttribute(BYTE * pBuffer, int bBufLen, ST_ATTRIBUTE_TYPE Attribute, BYTE * bValueLen, BYTE ** Value)
{
    BYTE i;
    if((bBufLen < 0 ) || (pBuffer[0] > bBufLen)) return STR_DATA_CORRUPTED;
    for(i = 1; i < pBuffer[0] + 1; i++){
        if(pBuffer[i] == Attribute){
            *bValueLen = pBuffer[i + 1];
            *Value = &pBuffer[i + 2];
            return STR_OK;
        }
        i++;
        i += pBuffer[i];
        if((i > pBuffer[0] + 1)||(i > bBufLen)) return STR_DATA_CORRUPTED;
    }
    return STR_ATTR_NOT_FOUND;
}
ST_RESULT GetCommandLength(BYTE * pbBlob, int bBlobLen, int * pbDataLength)
{
    if(bBlobLen <= 0) {
        *pbDataLength = 0;
        return STR_BUFFER_TOO_SMALL;
    }
    *pbDataLength = (int)pbBlob[0] + 1;
    return STR_OK;
}
ST_RESULT GetNextAttribute(BYTE * pBuffer, int bBufLen, BYTE ** pPointer, ST_ATTRIBUTE_TYPE * Attribute, BYTE * bValueLen, BYTE ** Value)
{
    if(bBufLen <= 0) return STR_BUFFER_TOO_SMALL;
    if(*pPointer == NULL) { // начинаем с начала
        *pPointer = pBuffer + 1;
    }
    if(*pPointer >= pBuffer + pBuffer[0]) {
        return STR_ATTR_NOT_FOUND;
    }
    // Читаем команду
    *Attribute = (ST_ATTRIBUTE_TYPE)**pPointer;
    (*pPointer)++;
    *bValueLen = **pPointer;
    (*pPointer)++;
    *Value = *pPointer;    
    (*pPointer) += *bValueLen;
    return STR_OK;
}

ST_RESULT SetClientDisconnect()
{
    return STR_OK;
}
