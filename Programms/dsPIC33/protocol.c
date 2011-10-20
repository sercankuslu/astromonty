#include "protocol.h"

typedef struct ST_CONNECTION {
    BYTE Mode;
}ST_CONNECTION;
#define MAX_CONNECTIONS 5
ST_CONNECTION Connection[MAX_CONNECTIONS];

BYTE FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE bAttribute, BYTE* Block, WORD* wBlockLen)
{
    BYTE i = 0;
    BYTE* BlockPtr = Block;    
    for(i = 0; i < bAttribute; i++){
        memcpy(BlockPtr, &pAttribute[i].type, sizeof(ST_ATTRIBUTE_TYPE));
        BlockPtr+=sizeof(ST_ATTRIBUTE_TYPE);
        if(BlockPtr >= Block+wBlockLen) return STR_BUFFER_TOO_SMALL;
        memcpy(BlockPtr, &pAttribute[i].ulValueLen, sizeof(WORD));
        BlockPtr+=sizeof(WORD);
        if(BlockPtr >= Block+wBlockLen) return STR_BUFFER_TOO_SMALL;
        if(pAttribute[i].ulValueLen!=0){
            memcpy(BlockPtr, pAttribute[i].pValue, pAttribute[i].ulValueLen);
            BlockPtr+=pAttribute[i].ulValueLen;  
            if(BlockPtr >= Block+wBlockLen) return STR_BUFFER_TOO_SMALL;
        }     
    }
    *wBlockLen = BlockPtr - Block;
    return STR_OK;
}
BYTE ParseBlob(BYTE* Block, WORD ulBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *pbAttribute, BYTE** pbMemPtr, WORD wMemLen)
{
    BYTE i = 0;
    BYTE* BlockPtr = Block;
    *pbAttribute=0;
    for(; BlockPtr < Block+ulBlockLen; i++){
        memcpy(&pAttribute[i].type, BlockPtr, sizeof(ST_ATTRIBUTE_TYPE)); 
        BlockPtr+=sizeof(ST_ATTRIBUTE_TYPE);
        if(BlockPtr >= Block+ulBlockLen) return STR_FUNCTOIN_FAILED;
        memcpy(&pAttribute[i].ulValueLen, BlockPtr, sizeof(WORD));           
        BlockPtr+=sizeof(WORD);
        if(BlockPtr >= Block+ulBlockLen) return STR_FUNCTOIN_FAILED;
        if(pAttribute[i].ulValueLen!=0){ // если длинна данных равна 0, то не считываем данные
            memcpy((*pbMemPtr), BlockPtr, pAttribute[i].ulValueLen);
            pAttribute[i].pValue = (*pbMemPtr);
            (*pbMemPtr)+=pAttribute[i].ulValueLen;
            BlockPtr+=pAttribute[i].ulValueLen;  
            if(BlockPtr >= Block+ulBlockLen) return STR_FUNCTOIN_FAILED;
        } 
        else pAttribute[i].pValue = NULL;            
        *pbAttribute++;
    }   
    return STR_OK;
}
BYTE Init()
{
    BYTE i;
    for(i=0;i<MAX_CONNECTIONS;i++){
        Connection[i].Mode = STS_NO_CONNECT;
    }
}
BYTE findparam(BYTE* pbData, BYTE bDataLen, ST_ATTRIBUTE_TYPE Type)
{
    BYTE i;
    for(i = 0; i < bDataLen; i++) {
        if(pbData[i].type==Type){
            return i;			            
        }  
    }   
    return 255;
} 
BYTE ProcessClients(BYTE bConnectionID, BYTE* pbBlob, WORD* pwBlobLen)
{
    BYTE Mem[64];
    BYTE* MemPtr = Mem;
    memset(Mem,0,sizeof(Mem));
    ST_ATTRIBUTE Data[MAX_ATTRIBUTE];
    BOOL NeedAnswer = false;
    BYTE Answers = 0;
    static ST_ATTRIBUTE Answer[] = {
        {STA_FLAG, &Answers, sizeof(BYTE) }
    };
    memset(Data,0,sizeof(Data));
    BYTE bAttributeLen=0;
    BYTE res = 0;
    res = ParseBlob(pbBlob, *pwBlobLen, Data, &bAttributeLen, &MemPtr);
    if(res!=STR_OK) return res;
    BYTE i;
    BYTE Command=0;
    BYTE* pbBlobPtr = pbBlob; // нужно для последовательного формирования блоба
    WORD  wBlobLen;
    // ищем командный атрибут    

    i = findparam(Data, bAttributeLen, STA_COMMAND); 
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
        NeedAnswer = true;
        break;                        
    case STS_AUTH_REQ:
        if(Command == STC_REQEST_AUTH) {
            BYTE login = findparam(Data, bAttributeLen, STA_LOGIN);             
            BYTE password = findparam(Data, bAttributeLen, STA_PASSWORD); 
            if((login == 255)||(password == 255)) {
                Answers = STF_COMMAND_INCOMPLETE;
                Connection[bConnectionID].Mode = STS_NO_CONNECT;                
            } else {
                if(1){ // проверка подлиннности                
                    Answers = STF_ACCEPTED;
                    Connection[bConnectionID].Mode = STS_CONNECTED;
                } else {
                    Answers = STF_DECLINED; 
                    Connection[bConnectionID].Mode = STS_NO_CONNECT;                
                }
            }
            NeedAnswer = true;            
        } else {
            Answers = STF_DECLINED; 
            Connection[bConnectionID].Mode = STS_NO_CONNECT;
        }
        break;   
    case STS_CONNECTED:
        switch(Command){
        case STC_REQEST_DATA:
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
        res = STR_COMMAND_UNKNOWN;  
    }
    if(NeedAnswer) {        
        res = FormBlob(Answer, 1, &pbBlobPtr, &wBlobLen);
        pbBlobPtr+=wBlobLen; // увеличиваем указатель
        *pwBlobLen+=wBlobLen; // увеличиваем размер
        if(res!=STR_OK) return res;
        if(Connection[bConnectionID].Mode == STS_NO_CONNECT) {
            res = STR_NEED_DISCONNECT;
        } else {
            res = STR_NEED_ANSWER;
        }
    }
    return res;
}




