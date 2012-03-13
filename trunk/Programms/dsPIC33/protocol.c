#include "stdafx.h"
#include "protocol.h"
#include "..\dsPIC33\TCPIP Demo App\OCTimer.h"
#ifndef __C30__
#   include "..\PIC18F97J60\TCPIP Demo App\roundbuffer.h"
#endif
#ifdef USE_PROTOCOL_CLIENT
#define MAX_CONNECTIONS 1
#else
#define MAX_CONNECTIONS 5
#endif

extern RR rr1;
extern RR rr2;
extern RR rr3;

typedef struct ST_CONNECTION {
    ST_STATUS Mode;
}ST_CONNECTION;

#ifndef __C30__
static ST_COMMANDS  connectreq = STC_REQEST_CONNECT;
static ST_COMMANDS  authreq = STC_REQEST_AUTH;
static ST_COMMANDS  datareq = STC_REQEST_DATA;

static ST_ATTRIBUTE RequestConnect[] = {
    {STA_COMMAND, sizeof(BYTE), &connectreq},
};
static char Login[] = "root";
static char Password[] = "pass";
static ST_ATTRIBUTE RequestAuth[] = {
    {STA_COMMAND,  sizeof(BYTE), &authreq},
    {STA_LOGIN,  0, 0},
    {STA_PASSWORD,  0, 0},
};
static ST_ATTRIBUTE RequestData[] = {
    {STA_COMMAND,  sizeof(BYTE), &datareq},
    {STA_TIME_SNTP,  0, NULL},
};
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
    static ST_COMMANDS Command = STC_NO_COMMANDS;
    BOOL NeedAnswer = FALSE;
    ST_FLAGS Answers = STF_OK;
    BOOL AuthChecked = TRUE;

    BYTE bAttributeLen = 0;
    BYTE pbMem[MEM_BUFFER_LEN];
    ST_ATTRIBUTE Data[MAX_ATTRIBUTE];
    BYTE bMemPos = 0;
    int bBlobPos = 0;
    float d1;
    float d2;
    float d3;

    ST_ATTRIBUTE Answer[] = {
        {STA_FLAG,  sizeof(BYTE),&Answers }
    };
    ST_ATTRIBUTE SendData[] = {
        {STA_NULL, 0, 0 }
    };
    if(*pbBlobLen == 0) return STR_OK;
    if((*pbBlobLen < 0)||(*pbBlobLen >= pbBlobSize)) {
        Connections[bConnectionID].Mode = STS_NO_CONNECT;
        return STR_NEED_DISCONNECT;
    }
    memset(pbMem,0,sizeof(pbMem));
    memset(Data,0,sizeof(Data));
    
    res = ParseBlob(pbBlob, *pbBlobLen, Data, &bAttributeLen, pbMem, sizeof(pbMem), &bMemPos);
    if(res != STR_OK) {
        Connections[bConnectionID].Mode = STS_NO_CONNECT;
        return res;  
    }
    
    res = FindParam(Data, bAttributeLen, STA_COMMAND, &i); 
    if(res == STR_ATTR_NOT_FOUND) return STR_COMMAND_UNKNOWN;
    
    Command = (ST_COMMANDS)*((BYTE*)Data[i].pValue);	
   
    switch(Connections[bConnectionID].Mode){
    case STS_NO_CONNECT:    	
        if(Command == STC_REQEST_CONNECT) {
            Answers = STF_AUTH_NEEDED;
            Connections[bConnectionID].Mode = STS_AUTH_REQ;
        } else {
            Answers = STF_DECLINED; 
        }
        NeedAnswer = TRUE;
        break;
    case STS_AUTH_REQ:
        if(Command == STC_REQEST_AUTH) {
            BYTE login;
            BYTE password;
            res = FindParam(Data, bAttributeLen, STA_LOGIN,&login);
            if(res != STR_OK){
                Answers = STF_COMMAND_INCOMPLETE;
                Connections[bConnectionID].Mode = STS_NO_CONNECT;
                NeedAnswer = TRUE;
                break; 
            }
            res = FindParam(Data, bAttributeLen, STA_PASSWORD, &password); 
            if(res != STR_OK){
                Answers = STF_COMMAND_INCOMPLETE;
                Connections[bConnectionID].Mode = STS_NO_CONNECT;
                NeedAnswer = TRUE;
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
        NeedAnswer = TRUE;
        break;   
    case STS_CONNECTED:
        LED1_IO = 1;        
        switch(Command){
            case STC_REQEST_DATA:
                for(j = 0; j < bAttributeLen; j++){
                    Answers = STF_NO_DATA;
                    switch (Data[j].type){                    
                    case STA_NETWORK_NAME:  
                        SendData[0].type = STA_NETWORK_NAME;
                        SendData[0].ulValueLen = sizeof(AppConfig.NetBIOSName);
                        SendData[0].pValue = AppConfig.NetBIOSName;
                        res = FormBlob(SendData, 1, pbBlob, MAX_BUFFER_LEN, &bBlobPos);
                        if(res!=STR_OK){
                            Answers = STF_DATA_ERROR;
                            i = bAttributeLen; // interrupt process
                            *pbBlobLen = 0;
                            bBlobPos = 0;	
                        }else {
                            Answers = STF_DATA_READY;
                            *pbBlobLen = bBlobPos;
                        }	
                        break;
                    case STA_TIME_SNTP:
                        {                            
                            static BYTE vTime[11];                            
                            ULtoA(CPUSPEED.Val, vTime);                                    
                            SendData[0].type = STA_TIME_SNTP;
                            SendData[0].ulValueLen = sizeof(vTime);
                            SendData[0].pValue = vTime;
                            res = FormBlob(SendData, 1, pbBlob, MAX_BUFFER_LEN, &bBlobPos);
                            if(res!=STR_OK){
                                Answers = STF_DATA_ERROR;
                                i = bAttributeLen; // interrupt process
                                *pbBlobLen = 0;
                                bBlobPos = 0;	
                            }else {
                                Answers = STF_DATA_READY;
                                *pbBlobLen = bBlobPos;
                            }
                        }
                        break;
                    case STA_ALPHA:
                        {   
                            d1 = (float)(rr1.XPosition * rr1.dx);
                            SendData[0].type = STA_ALPHA;
                            SendData[0].ulValueLen = sizeof(float);
                            SendData[0].pValue = &d1;
                            res = FormBlob(SendData, 1, pbBlob, MAX_BUFFER_LEN, &bBlobPos);
                            if(res!=STR_OK){
                                Answers = STF_DATA_ERROR;
                                i = bAttributeLen; // interrupt process
                                *pbBlobLen = 0;
                                bBlobPos = 0;	
                            }else {
                                Answers = STF_DATA_READY;
                                *pbBlobLen = bBlobPos;
                            }
                        }
                        break;
                    case STA_DELTA:
                        {   
                            d2 = (float)(rr2.XPosition * rr2.dx);
                            SendData[0].type = STA_DELTA;
                            SendData[0].ulValueLen = sizeof(float);
                            SendData[0].pValue = &d2;
                            res = FormBlob(SendData, 1, pbBlob, MAX_BUFFER_LEN, &bBlobPos);
                            if(res!=STR_OK){
                                Answers = STF_DATA_ERROR;
                                i = bAttributeLen; // interrupt process
                                *pbBlobLen = 0;
                                bBlobPos = 0;	
                            }else {
                                Answers = STF_DATA_READY;
                                *pbBlobLen = bBlobPos;
                            }
                        }
                        break;
                    case STA_GAMMA:
                        {   
                            d3 = (float)(rr3.XPosition * rr3.dx);
                            SendData[0].type = STA_GAMMA;
                            SendData[0].ulValueLen = sizeof(float);
                            SendData[0].pValue = &d3;
                            res = FormBlob(SendData, 1, pbBlob, MAX_BUFFER_LEN, &bBlobPos);
                            if(res!=STR_OK){
                                Answers = STF_DATA_ERROR;
                                i = bAttributeLen; // interrupt process
                                *pbBlobLen = 0;
                                bBlobPos = 0;	
                            }else {
                                Answers = STF_DATA_READY;
                                *pbBlobLen = bBlobPos;
                            }
                        }
                        break;
                    default:
                        break;                        
                    };
                }
                NeedAnswer = TRUE;
                res = STR_OK;
                break;
            case STC_SEND_DATA:
                res = STR_OK;
                break;
            case STC_EXECUTE_COMMAND: // *************************************************************************
                {
                    for(j = 0; j < bAttributeLen; j++){
                        switch (Data[j].type){
                        case STA_ALPHA_START:
                            {
                                float t = 0.0;
                                //BYTE l = 0;
                                res = FindParam(Data, bAttributeLen, STA_ALPHA_TARGET, &k);
                                if(res != STR_OK) {
                                    break;
                                }
                                memcpy(&t, Data[k].pValue, Data[k].ulValueLen);
                                rr1.XPosition = 0;                                 
                                PushCmdToQueue(&rr1, ST_ACCELERATE, 10 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);
                                PushCmdToQueue(&rr1, ST_RUN, 0.0, t, 1);
                                PushCmdToQueue(&rr1, ST_DECELERATE, 0.0 * Grad_to_Rad, 180.0 * Grad_to_Rad, 1);                                
                                Answers = STF_OK;
                            }
                            break;
                        default:;

                        };
                    }
                         
                }
                NeedAnswer = TRUE;
                res = STR_OK;
                break;
            default :
                Answers = STF_COMMAND_UNKNOWN;
                }
        break;
    default:        
        Connections[bConnectionID].Mode = STS_NO_CONNECT;
        res = STR_FUNCTION_FAILED;  
    }    
    if(NeedAnswer) {
        res = FormBlob(Answer, 1, pbBlob, MAX_BUFFER_LEN, &bBlobPos);
        *pbBlobLen = bBlobPos;
        if(res!=STR_OK) return res;
        if(Connections[bConnectionID].Mode == STS_NO_CONNECT) {
            res = STR_NEED_DISCONNECT;
        } else {
            res = STR_NEED_ANSWER;
        }
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
    ST_ATTRIBUTE Data[MAX_ATTRIBUTE];
    BYTE bAttributeLen = 0;
    BYTE pbMem[MEM_BUFFER_LEN];
    BYTE bMemPos = 0;	
    int bBlockPos = 0;    
    ST_RESULT  res = STR_OK;
    BYTE AttrLen;
    BYTE j;
    BYTE* Answer; 
    BOOL DoWork = TRUE;
    if(*pbDataLength < 0){
        ST_STATE = ST_REQUEST_CONNECT;
        ClientConnected = FALSE;
        return STR_NEED_DISCONNECT;
    }
      
    while(DoWork){
        DoWork = FALSE;
        switch(ST_STATE){
        case ST_REQUEST_CONNECT:
            RoundBufferInit();
            AttrLen = 1;
            res = FormBlob(RequestConnect, AttrLen, pbBlob, bBlobLen, &bBlockPos);
            // TODO: обработка ошибок
            res = STR_NEED_ANSWER;
            ST_STATE = ST_WAIT_CONNECT;
            break;
        case ST_WAIT_CONNECT:
            if(*pbDataLength==0) break;
            res = ParseBlob(pbBlob, *pbDataLength, Data, &bAttributeLen, pbMem, (BYTE)sizeof(pbMem), &bMemPos);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            } 
            res = FindParam(Data, bAttributeLen, STA_FLAG, &j);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }
            if((Data[j].type == STA_FLAG)&&
                (Data[j].ulValueLen == sizeof(BYTE)))
            {
                Answer = (BYTE*)Data[j].pValue;
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
            AttrLen = 3;
            res = FindParam(RequestAuth, AttrLen, STA_LOGIN, &j);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }            
            RequestAuth[j].pValue = Login;
            RequestAuth[j].ulValueLen = (BYTE)strlen((char*)Login);
            res = FindParam(RequestAuth, AttrLen, STA_PASSWORD, &j);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }                        
            RequestAuth[j].pValue = Password;
            RequestAuth[j].ulValueLen = (BYTE)strlen((char*)Password);
            res = FormBlob(RequestAuth, AttrLen, pbBlob, bBlobLen, &bBlockPos);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }                
            // TODO: обработка ошибок
            res = STR_NEED_ANSWER; 
            ST_STATE = ST_WAIT_AUTH;
            break;
        case ST_WAIT_AUTH:
            if(*pbDataLength==0) break;
            res = ParseBlob(pbBlob, *pbDataLength, Data, &bAttributeLen, pbMem, (BYTE)sizeof(pbMem), &bMemPos);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            } 
            res = FindParam(Data, bAttributeLen, STA_FLAG, &j);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            }
            if((Data[j].type == STA_FLAG)&&
                (Data[j].ulValueLen == sizeof(BYTE)))
            {
                Answer = (BYTE*)Data[j].pValue;
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
            if(*pbDataLength > 0){
                res = ParseBlob(pbBlob, *pbDataLength, Data, &bAttributeLen, pbMem, sizeof(pbMem), &bMemPos);
                if(res != STR_OK){
                    ST_STATE = ST_REQUEST_CONNECT;
                } else {                    
                    for(j = 0; j < bAttributeLen; j++){
                        switch (Data[j].type){
                        case STA_COMMAND:                        
                        case STA_LOGIN:
                        case STA_PASSWORD:
                            break;
                        case STA_FLAG:
                            Answer = (BYTE*)Data[j].pValue;
                            if(*Answer != STF_DATA_READY){
                                // это ошибка - выходим
                                j = bAttributeLen;
                            }
                            break;
                        case STA_TIME_SNTP:
                        case STA_ALPHA:
                        case STA_DELTA:
                        case STA_GAMMA:
                        default:
                            if(PushAttr(Data[j],IN_BUFFER) != RB_OK){
                                // переполнение буфера. отключаемся
                                ST_STATE = ST_REQUEST_CONNECT;
                                return STR_NEED_DISCONNECT;
                            }                        
                        };
                    }
                }
            }
            // отправка данных
            if(IsDataInBuffer(OUT_BUFFER)){
                // если в очереди много запросов, формируем большой пакет из них
                //res = FormBlob(&RequestData[0], 1, pbBlob, bBlobLen, &bBlockPos);
                BOOL CommandExist = FALSE;
                do{    
                    BYTE m[10];
                    ST_ATTRIBUTE_TYPE Type;                    
                    RequestData[1].pValue = (void*)&m;
                    if(GetNextAttrType(OUT_BUFFER, &Type) != RB_OK) {
                        break;
                    }                    
                    if(Type == STA_COMMAND){
                        if(CommandExist) {
                            break;
                        }
                        CommandExist = TRUE;
                    }                    
                    if(PopAttr(&RequestData[1], OUT_BUFFER) == RB_OK){                        
                        res = FormBlob(&RequestData[1], 1, pbBlob, bBlobLen, &bBlockPos);
                    }
                    if(res!=STR_OK) {
                        ST_STATE = ST_REQUEST_CONNECT;
                        return STR_NEED_DISCONNECT; 
                    }
                }while(IsDataInBuffer(OUT_BUFFER));
                res = STR_NEED_ANSWER; 
                ST_STATE = ST_CONNECTED;
            }
            break;
        default: 
            ST_STATE = ST_REQUEST_CONNECT;
        };        
    }
    if(ST_STATE == ST_CONNECTED){        
        ClientConnected = 1;
    } else ClientConnected = 0;

    if(res == STR_NEED_ANSWER){
        *pbDataLength = bBlockPos;
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
