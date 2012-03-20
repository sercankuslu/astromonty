#include "stdafx.h"
#include "protocol.h"
#include "..\dsPIC33\TCPIP Demo App\OCTimer.h"
#ifndef __C30__

#   include "..\PIC18F97J60\TCPIP Demo App\Control.h"
#endif

#ifdef USE_PROTOCOL_CLIENT
#define MAX_CONNECTIONS 1
#else
#define MAX_CONNECTIONS 5
#endif

extern RR rr1;
extern RR rr2;
extern RR rr3;
extern ALL_PARAMS Params;

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
/******************************************************************************
*
*
*
*
*
*
******************************************************************************/
ST_RESULT FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE bAttributeLen, BYTE* pbBlock, int bBlockLen ,int* pbBlockPos)
{
    BYTE i = 0;  
    int j = (*pbBlockPos);
    for(i = 0; i < bAttributeLen; i++){
        pbBlock[j++] = (BYTE)pAttribute[i].type;
        pbBlock[j++] = pAttribute[i].ulValueLen;
        if(pAttribute[i].ulValueLen!=0){
            if(*pbBlockPos + pAttribute[i].ulValueLen > bBlockLen) 
                return STR_BUFFER_TOO_SMALL; 
            memcpy(&pbBlock[j], pAttribute[i].pValue, pAttribute[i].ulValueLen);
            j += pAttribute[i].ulValueLen;
        }
    }
    (*pbBlockPos) = j;
    return STR_OK;
}
/******************************************************************************
*
*
*
*
*
*
******************************************************************************/
ST_RESULT ParseBlob(BYTE* pbBlock, int bBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *pbAttributeLen, BYTE* pbMem, BYTE bMemLen, BYTE* bMemPos)
{
    BYTE i = 0;    
    //(*pbAttribute) = 0;
    BYTE bHead;
    BYTE j = 0;
    bHead = sizeof(BYTE) + sizeof(BYTE);
    for(i = 0; i < bBlockLen; ){
        if(i + bHead > bBlockLen) return STR_DATA_CORRUPTED;
        pAttribute[j].type = (ST_ATTRIBUTE_TYPE)pbBlock[i++];
        pAttribute[j].ulValueLen = pbBlock[i++];
        if(pAttribute[j].ulValueLen != 0){ 
            if((*bMemPos + pAttribute[j].ulValueLen) > bMemLen) 
                return STR_BUFFER_TOO_SMALL;
            if(i + pAttribute[j].ulValueLen > bBlockLen) 
                return STR_DATA_CORRUPTED;
            memcpy((void*)&pbMem[*bMemPos], (void*)&pbBlock[i], pAttribute[j].ulValueLen);
            pAttribute[j].pValue = &pbMem[*bMemPos];
            *bMemPos += pAttribute[j].ulValueLen;
            i += pAttribute[j].ulValueLen;
        } 
        else pAttribute[j].pValue = NULL;
        *pbAttributeLen = ++j;
        if( j >= MAX_ATTRIBUTE) 
        	return STR_TOO_MANY_ATTRIBUTES;
    }   
    return STR_OK;
}
/******************************************************************************
*
*
*
*
*
*
******************************************************************************/
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
ST_RESULT FindParam(ST_ATTRIBUTE* pData, BYTE bDataLen, ST_ATTRIBUTE_TYPE bType, BYTE * Index)
{
    BYTE i;
    for(i = 0; i < bDataLen; i++) {
        if(pData[i].type == bType){
            (*Index) = i;
            return STR_OK;
        }
    }
    return STR_ATTR_NOT_FOUND;
} 

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
    BYTE i = 0;
    BYTE j = 0;
    BYTE k = 0;
    ST_COMMANDS Command = STC_NO_COMMANDS;    
    ST_FLAGS Answers = STF_OK;
    BOOL AuthChecked = TRUE;
    BYTE bAttributeLen = 0;       
    float d1;
    float d2;
    float d3;
    BYTE Len = 0;
    BYTE * Answer = NULL;
    char * pLogin = NULL;
    char * pPassword = NULL;
    BYTE * pPointer;
    ST_ATTRIBUTE_TYPE Attribute = STA_NULL;
    void * Value = NULL;
    void * OutValue = NULL;
    ST_FLAGS Flag = STF_OK;
    static BYTE pbOutBlob[64];
    
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
   
    switch(Connections[bConnectionID].Mode){
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
#ifdef _DISABLE_
            case STC_EXECUTE_COMMAND: // *************************************************************************
                {
                    for(j = 0; j < bAttributeLen; j++){
                        switch (Data[j].type){
                        case STA_ALPHA_START:
                            {
                                res = FindParam(Data, bAttributeLen, STA_ALPHA_TARGET, &k);
                                if(res != STR_OK) {
                                    break;
                                }
                                memcpy(&d1, Data[k].pValue, Data[k].ulValueLen);
                                // TODO: искать скорость наведения в атрибутах
                                GoToCmd(&rr1, 1.0 * Grad_to_Rad, d1, 0);                                                               
                                Answers = STF_OK;
                            }
                            break;
                        case STA_DELTA_START:
                            {
                                res = FindParam(Data, bAttributeLen, STA_DELTA_TARGET, &k);
                                if(res != STR_OK) {
                                    break;
                                }
                                memcpy(&d2, Data[k].pValue, Data[k].ulValueLen);

                                GoToCmd(&rr2, 1.0 * Grad_to_Rad, d2, 0);                                                               
                                Answers = STF_OK;
                            }
                            break;
                        case STA_GAMMA_START:
                            {
                                res = FindParam(Data, bAttributeLen, STA_GAMMA_TARGET, &k);
                                if(res != STR_OK) {
                                    break;
                                }
                                memcpy(&d3, Data[k].pValue, Data[k].ulValueLen);
                                GoToCmd(&rr3, 1.0 * Grad_to_Rad, d3, 0);                                                               
                                Answers = STF_OK;
                            }
                            break;
                        default:;

                        };
                    }
                         
                }                
                res = STR_OK;
                break;
#endif
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
    int bBlockPos = 0;    
    ST_RESULT  res = STR_OK;
    BYTE AttrLen;
    BYTE j;
    BYTE* Answer; 
    BOOL DoWork = TRUE;
    BYTE Len = 0;
    BYTE * Str = NULL;
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
    res = CheckBlob(pbBlob, bBlobLen);
    if(res != STR_OK) {
        ST_STATE = ST_REQUEST_CONNECT;
        ClientConnected = FALSE;
        return STR_NEED_DISCONNECT;
    }

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
            if(*pbDataLength==0) break;            
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
            if(*pbDataLength==0) break;            
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
            res = FindAttribute(pbBlob, bBlobLen, STA_COMMAND, &Len, &Answer);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }
            if(Len == sizeof(BYTE)){
                Command = (ST_COMMANDS)*Answer;  
            }
            if(Command == STC_SEND_DATA) {
                do{
                    res = GetNextAttribute(pbBlob, bBlobLen, &pPointer, &Attribute, &Len, (BYTE**)&fData);                    
                    if(res == STR_OK) {
                        if(Len == sizeof(float)){
                            switch (Attribute){
                            case STA_ALPHA:
                                Params.Alpha.Angle = *fData;
                                Params.Alpha.IsModified.bits.Angle = 1;                                
                                break;
                            case STA_DELTA:
                                Params.Delta.Angle = *fData;
                                Params.Delta.IsModified.bits.Angle = 1;                               
                                break;
                            case STA_GAMMA:
                                Params.Gamma.Angle = *fData;
                                Params.Gamma.IsModified.bits.Angle = 1;                                
                                break;
                            }
                        }
                    }
                } while (res != STR_ATTR_NOT_FOUND);
            }
            
            // отправка данных 
            // все переменные, полученные из блоба должны быть сохранены в других местах!!!
            if(Params.NeedToUpdate.Val > 0){
                res = BeginCommand(pbBlob, bBlobLen, STC_REQEST_DATA);
                if(res != STR_OK){
                    ST_STATE = ST_REQUEST_CONNECT;
                    break;
                }        
                if(Params.NeedToUpdate.bits.Alpha){
                    if(Params.Alpha.NeedToUpdate.bits.Angle){ 
                        res = AddAttribute(pbBlob, bBlobLen, STA_ALPHA, 0, &bNull);
                        if(res != STR_OK){
                            ST_STATE = ST_REQUEST_CONNECT;
                            break;
                        } 
                        Params.Alpha.NeedToUpdate.bits.Angle = 0;
                        Params.NeedToUpdate.bits.Alpha = 0;
                    }
                }
                if(Params.NeedToUpdate.bits.Delta){
                    if(Params.Delta.NeedToUpdate.bits.Angle){ 
                        res = AddAttribute(pbBlob, bBlobLen, STA_DELTA, 0, &bNull);
                        if(res != STR_OK){
                            ST_STATE = ST_REQUEST_CONNECT;
                            break;
                        } 
                        Params.Delta.NeedToUpdate.bits.Angle = 0;
                        Params.NeedToUpdate.bits.Delta = 0;
                    }
                }
                if(Params.NeedToUpdate.bits.Gamma){
                    if(Params.Gamma.NeedToUpdate.bits.Angle){ 
                        res = AddAttribute(pbBlob, bBlobLen, STA_GAMMA, 0, &bNull);
                        if(res != STR_OK){
                            ST_STATE = ST_REQUEST_CONNECT;
                            break;
                        } 
                        Params.Gamma.NeedToUpdate.bits.Angle = 0;
                        Params.NeedToUpdate.bits.Gamma = 0;
                    }
                }
                res = STR_NEED_ANSWER;
            }
            
            break;
        default: 
            ST_STATE = ST_REQUEST_CONNECT;
        };        
    }
    if(ST_STATE == ST_CONNECTED){        
        if(!Params.Local.ConnectFlag)
            Params.Local.IsModified.bits.Flag = 1;
        Params.Local.ConnectFlag = 1;
    } else {
        ClientConnected = 0;
        if(Params.Local.ConnectFlag)
            Params.Local.IsModified.bits.Flag = 1;
        Params.Local.ConnectFlag = 0;    
    }

    if(res == STR_NEED_ANSWER){
        res = AddFlag(pbBlob, bBlobLen, STF_OK);
        if(res != STR_OK){
            ST_STATE = ST_REQUEST_CONNECT;
            return res;
        }
        res = GetCommandLength(pbBlob, bBlobLen, pbDataLength);
    } else {
        *pbDataLength = 0;
    }
    return res;
}
#endif // USE_PROTOCOL_CLIENT

ST_RESULT  CopyAttribute(ST_ATTRIBUTE pDest, ST_ATTRIBUTE pSource, BYTE *pbMem, BYTE bMemLen, BYTE* bMemPos )
{
        pDest.type = pSource.type;
        pDest.ulValueLen = pSource.ulValueLen;
        
        if((*bMemPos + pDest.ulValueLen) > bMemLen) 
                return STR_BUFFER_TOO_SMALL;	
        memcpy(&pbMem[*bMemPos], pSource.pValue, pSource.ulValueLen);
        pDest.pValue = &pbMem[*bMemPos];
        *bMemPos += pDest.ulValueLen;
        return STR_OK;
}

BOOL IsClientConnected()
{
    return ClientConnected;
}

void SetClientDisconnect()
{
    ClientConnected = FALSE;
}

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
    pBuffer[0] += sizeof(BYTE(Attribute)) + sizeof((BYTE)bValueLen) + bValueLen;
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


#ifdef __DISABLE__
void ExecuteCommands()
{
    static BYTE datareq = STC_REQEST_DATA;
    static BYTE execreq = STC_EXECUTE_COMMAND;
    static ST_ATTRIBUTE RequestData[] = {
        {STA_COMMAND,  sizeof(BYTE), &datareq},
        {STA_NULL,  0, NULL},
    };
    static ST_ATTRIBUTE Data = {
        STA_NULL,  0, NULL
    };
    
    if(Params.NeedToCommit.Val > 0){
        if(Params.NeedToCommit.bits.Alpha){
            if(Params.Alpha.NeedToCommit.bits.TargetAngle){
                RequestData[0].pValue = &execreq;
                rv = PushAttr(RequestData[0], OUT_BUFFER);
                if(rv!=RB_OK) return;
                RequestData[1].type = STA_ALPHA_START;
                RequestData[1].pValue = NULL;
                RequestData[1].ulValueLen = 0;
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                if(rv!=RB_OK) return;
                RequestData[1].type = STA_ALPHA_TARGET;
                RequestData[1].pValue = &Params.Alpha.TargetAngle;
                RequestData[1].ulValueLen = sizeof(float);
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                if(rv!=RB_OK) return;
                Params.Alpha.NeedToCommit.bits.TargetAngle = 0;
            }
        }
        if(Params.NeedToCommit.bits.Delta){
            if(Params.Delta.NeedToCommit.bits.TargetAngle){
                RequestData[0].pValue = &execreq;
                rv = PushAttr(RequestData[0], OUT_BUFFER);
                if(rv!=RB_OK) return;
                RequestData[1].type = STA_DELTA_START;
                RequestData[1].pValue = NULL;
                RequestData[1].ulValueLen = 0;
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                if(rv!=RB_OK) return;
                RequestData[1].type = STA_DELTA_TARGET;
                RequestData[1].pValue = &Params.Delta.TargetAngle;
                RequestData[1].ulValueLen = sizeof(float);
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                if(rv!=RB_OK) return;
                Params.Delta.NeedToCommit.bits.TargetAngle = 0;
            }
        }
        if(Params.NeedToCommit.bits.Gamma){
            if(Params.Gamma.NeedToCommit.bits.TargetAngle){
                RequestData[0].pValue = &execreq;
                rv = PushAttr(RequestData[0], OUT_BUFFER);
                if(rv!=RB_OK) return;
                RequestData[1].type = STA_GAMMA_START;
                RequestData[1].pValue = NULL;
                RequestData[1].ulValueLen = 0;
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                if(rv!=RB_OK) return;
                RequestData[1].type = STA_GAMMA_TARGET;
                RequestData[1].pValue = &Params.Gamma.TargetAngle;
                RequestData[1].ulValueLen = sizeof(float);
                rv = PushAttr(RequestData[1], OUT_BUFFER);
                if(rv!=RB_OK) return;
                Params.Gamma.NeedToCommit.bits.TargetAngle = 0;
            }
        }
        Params.NeedToCommit.bits.Alpha = 0;
        Params.NeedToCommit.bits.Delta = 0;
        Params.NeedToCommit.bits.Gamma = 0;
    } else
        

}
#endif //#ifdef __DISABLE__