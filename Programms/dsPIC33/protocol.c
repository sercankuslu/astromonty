
#include "protocol.h"

typedef struct ST_CONNECTION {
    BYTE Mode;
}ST_CONNECTION;
#define MAX_CONNECTIONS 5
ST_CONNECTION Connection[MAX_CONNECTIONS];
static BYTE connectreq = STC_REQEST_CONNECT;
static BYTE authreq = STC_REQEST_AUTH;
static BYTE datareq = STC_REQEST_DATA;
static ST_ATTRIBUTE RequestConnect[] = {
    {STA_COMMAND, sizeof(BYTE), &connectreq},
};
static char* Login = "root";
static char* Password = "pass";
static ST_ATTRIBUTE RequestAuth[] = {
    {STA_COMMAND,  sizeof(BYTE), &authreq},
    {STA_LOGIN,  0, 0},
    {STA_PASSWORD,  0, 0},
};
static ST_ATTRIBUTE RequestData[] = {
    {STA_COMMAND,  sizeof(BYTE), &datareq},
    {STA_NETWORK_ADDRESS,  0, NULL},
};

#if defined(__18CXX)
#pragma udata ATTRIBUTE =0x500
#endif
static ST_ATTRIBUTE ReceivedData[16];
#if defined(__18CXX)
#pragma udata MEMORY =0x540
#endif
static BYTE ReceivedMemory[64];
#pragma udata
static BYTE ReceivedDataCount;
static BYTE ReceivedMemoryLen;

/******************************************************************************
*
*
*
*
*
*
******************************************************************************/
BYTE FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE bAttributeLen, BYTE* pbBlock, BYTE bBlockLen ,BYTE* pbBlockPos)
{
    BYTE i = 0;  
    BYTE j = (*pbBlockPos);
    for(i = 0; i < bAttributeLen; i++){
        pbBlock[j++] = pAttribute[i].type;
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
BYTE ParseBlob(BYTE* pbBlock, BYTE bBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *pbAttributeLen, BYTE* pbMem, BYTE bMemLen, BYTE* bMemPos)
{
    BYTE i = 0;    
    //(*pbAttribute) = 0;
    BYTE bHead;
    BYTE j = 0;
    bHead = sizeof(ST_ATTRIBUTE_TYPE) + sizeof(BYTE);
    for(i = 0; i < bBlockLen; ){
        if(i + bHead > bBlockLen) return STR_DATA_CORRUPTED;
        pAttribute[j].type = pbBlock[i++];
        pAttribute[j].ulValueLen = pbBlock[i++];
        if(pAttribute[j].ulValueLen != 0){ 
            if((*bMemPos + pAttribute[j].ulValueLen) > bMemLen) 
                return STR_BUFFER_TOO_SMALL;
            if(i + pAttribute[j].ulValueLen > bBlockLen) 
                return STR_DATA_CORRUPTED;
            memcpy(&pbMem[*bMemPos], &pbBlock[i], pAttribute[j].ulValueLen);
            pAttribute[j].pValue = &pbMem[*bMemPos];
            *bMemPos += pAttribute[j].ulValueLen;
            i += pAttribute[j].ulValueLen;
        } 
        else pAttribute[j].pValue = NULL;
        *pbAttributeLen = ++j;
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
BYTE Init()
{
    BYTE i;
    for(i = 0; i < MAX_CONNECTIONS; i++){
        Connection[i].Mode = STS_NO_CONNECT;
    }
    return 0;
}
/******************************************************************************
*
*
*
*
*
*
******************************************************************************/
BYTE FindParam(ST_ATTRIBUTE* pData, BYTE bDataLen, ST_ATTRIBUTE_TYPE bType)
{
    BYTE i;
    for(i = 0; i < bDataLen; i++) {
        if(pData[i].type == bType){
            return i;
        }
    }
    return 255;
} 

/******************************************************************************
*
*
*
*
*
*
******************************************************************************/
BYTE RunServer(BYTE bConnectionID, BYTE* pbBlob, BYTE* pbBlobLen)
{
    BYTE res = 0;
    BYTE i = 0;
    BYTE j = 0;
    BYTE Command = 0;
    BOOL NeedAnswer = FALSE;
    BYTE Answers = 0;

    BYTE bAttributeLen = 0;
    BYTE pbMem[MEM_BUFFER_LEN];
    ST_ATTRIBUTE Data[MAX_ATTRIBUTE];
    BYTE bMemPos = 0;
    BYTE bBlobPos = 0;

    ST_ATTRIBUTE Answer[] = {
        {STA_FLAG,  sizeof(BYTE),&Answers }
    };
    ST_ATTRIBUTE SendData[] = {
        {0,  0, 0 }
    };
    if(*pbBlobLen == 0) return STR_OK;
    memset(pbMem,0,sizeof(pbMem));
    memset(Data,0,sizeof(Data));
    res = ParseBlob(pbBlob, *pbBlobLen, Data, &bAttributeLen, pbMem, sizeof(pbMem), &bMemPos);
    if(res!=STR_OK) return res;  
    i = FindParam(Data, bAttributeLen, STA_COMMAND); 
    if(i==255) return STR_COMMAND_UNKNOWN;
    Command = *((BYTE*)Data[i].pValue);	
    switch(Connection[bConnectionID].Mode){
    case STS_NO_CONNECT:
    	
        if(Command == STC_REQEST_CONNECT) {
            Answers = STF_AUTH_NEEDED;
            Connection[bConnectionID].Mode = STS_AUTH_REQ;
        } else {
            Answers = STF_DECLINED; 
        }
        NeedAnswer = TRUE;
        break;
    case STS_AUTH_REQ:
        if(Command == STC_REQEST_AUTH) {
            BYTE login = FindParam(Data, bAttributeLen, STA_LOGIN);
            BYTE password = FindParam(Data, bAttributeLen, STA_PASSWORD); 
            if((login == 255)||(password == 255)) {
                Answers = STF_COMMAND_INCOMPLETE;
                Connection[bConnectionID].Mode = STS_NO_CONNECT;
            } else {
                //strcmp();
                if(1){ // authority check
                    Answers = STF_ACCEPTED;
                    Connection[bConnectionID].Mode = STS_CONNECTED;
                } else {
                    Answers = STF_DECLINED; 
                    Connection[bConnectionID].Mode = STS_NO_CONNECT;
                }
            }
        } else {
            Answers = STF_DECLINED; 
            Connection[bConnectionID].Mode = STS_NO_CONNECT;
        }
        NeedAnswer = TRUE;
        break;   
    case STS_CONNECTED:
    	LED1_IO = 1;
        switch(Command){
	    case STC_REQEST_DATA:
	        for(j = 0; j < bAttributeLen; j++){
	            switch (Data[j].type){
	    case STA_COMMAND:
	    case STA_FLAG:
	    case STA_LOGIN:
	    case STA_PASSWORD:
	        break;
	    case STA_NETWORK_ADDRESS:  
	        SendData[0].type = STA_NETWORK_ADDRESS;
	        SendData[0].ulValueLen = sizeof(AppConfig.MyIPAddr.Val);
	        SendData[0].pValue = &AppConfig.MyIPAddr.Val;
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
	    default:
	        Answers = STF_DATA_TYPE_UNKNOWN;
	        i = bAttributeLen; // interrupt process
	        *pbBlobLen = 0;
	        bBlobPos = 0;
            };
        }
        NeedAnswer = TRUE;
        res = STR_OK;
        break;
    case STC_SEND_DATA:
        res = STR_OK;
        break;
    default :
        Answers = STF_COMMAND_UNKNOWN;
        }
        break;
    default:
        res = STR_FUNCTION_FAILED;  
    }
    if(NeedAnswer) {
        res = FormBlob(Answer, 1, pbBlob, MAX_BUFFER_LEN, &bBlobPos);
        *pbBlobLen = bBlobPos;
        if(res!=STR_OK) return res;
        if(Connection[bConnectionID].Mode == STS_NO_CONNECT) {
            res = STR_NEED_DISCONNECT;
        } else {
            res = STR_NEED_ANSWER;
        }
    }
    return res;
}


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
BYTE RunClient(BYTE* pbBlob, BYTE bBlobLen, BYTE *pbDataLength)
{
    static  enum {
        ST_REQUEST_CONNECT = 0,
        ST_WAIT_CONNECT,
        ST_REQUEST_AUTH,
        ST_WAIT_AUTH,
        ST_REQUEST_DATA,
        ST_WAIT_DATA,
        ST_SEND_DATA,
        ST_WAIT_ANSWER
    } ST_STATE = ST_REQUEST_CONNECT;
    ST_ATTRIBUTE Data[MAX_ATTRIBUTE];
    BYTE bAttributeLen = 0;
    BYTE pbMem[MEM_BUFFER_LEN];
    BYTE bMemPos = 0;	
    BYTE bBlockPos = NULL;    
    static BYTE res = STR_OK;
    BYTE AttrLen;
    BYTE j;
    BYTE* Answer;   
    while(1){        
        switch(ST_STATE){
        case ST_REQUEST_CONNECT:
        	ReceivedDataCount = 0;
			ReceivedMemoryLen = 0;
            AttrLen = 1;
            res = FormBlob(RequestConnect, AttrLen, pbBlob, bBlobLen, &bBlockPos);
            res = STR_NEED_ANSWER;
            ST_STATE++;
            break;
        case ST_WAIT_CONNECT:
            if(*pbDataLength==0) break;
            res = ParseBlob(pbBlob, *pbDataLength, Data, &bAttributeLen, pbMem, sizeof(pbMem), &bMemPos);
            if(res != STR_OK){
                ST_STATE = ST_REQUEST_CONNECT;
                break;
            } 
            j = FindParam(Data,bAttributeLen,STA_FLAG);
            if(j==255){
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
            break;
        case STF_AUTH_NOT_NEEDED:
        case STF_ACCEPTED:
            ST_STATE = ST_REQUEST_DATA;
            break;
        default:
            ST_STATE = ST_REQUEST_CONNECT;
            break;
                }
            }
            break;
        case ST_REQUEST_AUTH:
            AttrLen = 3;
            j = FindParam(RequestAuth,AttrLen,STA_LOGIN);
            RequestAuth[j].pValue = Login;
            RequestAuth[j].ulValueLen = strlen(Login);
            j = FindParam(RequestAuth,AttrLen,STA_PASSWORD);
            RequestAuth[j].pValue = Password;
            RequestAuth[j].ulValueLen = strlen(Password);
            res = FormBlob(RequestAuth, AttrLen, pbBlob, bBlobLen, &bBlockPos);
            res = STR_NEED_ANSWER; 
            ST_STATE++;
            break;
        case ST_WAIT_AUTH:
            if(*pbDataLength==0) break;
            res = ParseBlob(pbBlob, *pbDataLength, Data, &bAttributeLen, pbMem, sizeof(pbMem), &bMemPos);
            if(res != STR_OK){
                ST_STATE = 0;
            } else {
                ST_STATE++;
            }
            break;
        case ST_REQUEST_DATA:
            AttrLen = 2;
            res = FormBlob(RequestData, AttrLen, pbBlob, bBlobLen, &bBlockPos);
            res = STR_NEED_ANSWER; 
            ST_STATE++;
            break;
        case ST_WAIT_DATA:
            if(*pbDataLength==0) break;
            res = ParseBlob(pbBlob, *pbDataLength, Data, &bAttributeLen, pbMem, sizeof(pbMem), &bMemPos);
            if(res != STR_OK){
                ST_STATE = 0;
            } else {
                ST_STATE = ST_REQUEST_DATA;
                //j = FindParam(Data,bAttributeLen,STA_NETWORK_ADDRESS);
                //if(j!=255)
                //	CopyAttribute(ReceivedData[0], Data[j], ReceivedMemory, sizeof(ReceivedMemory), ReceivedDataCount );
            }

            break;
        default: 
            ST_STATE = ST_REQUEST_CONNECT;
        };
        break;
    }
    if(res == STR_NEED_ANSWER){
        *pbDataLength = bBlockPos;
    } else {
        *pbDataLength = 0;
    }
    return res;
}
BYTE SendData(ST_ATTRIBUTE_PTR pData, BYTE bCount)
{
	return 0;
}

BOOL IsDataReady(void)
{
	return 0;
}

BYTE CopyAttribute(ST_ATTRIBUTE pDest, ST_ATTRIBUTE pSource, BYTE *pbMem, BYTE bMemLen, BYTE* bMemPos )
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


// помещает данные в буфер, возвращает указатель на них
void* Push(BYTE* pValue, BYTE bValueLen)
{
	return NULL;
}
//возвращает данные из буфера и освобождает буфер
void Pop(BYTE** pValue, BYTE* pbValueLen)
{
    

}

// при приходе новых данных, ищется в наличии атрибут с таким же типом данных, освобождается занимаемая им память и записывается новое значение
// должны быть метки времени для каждой записи. слишком старые данные надо удалять
