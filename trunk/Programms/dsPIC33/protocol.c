#include "protocol.h"

typedef struct ST_CONNECTION {
    BYTE Mode;
}ST_CONNECTION;
#define MAX_CONNECTIONS 5
ST_CONNECTION Connection[MAX_CONNECTIONS];

BYTE FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE bAttributeLen, BYTE* pbBlock, BYTE* pbBlockLen)
{
    BYTE i = 0;
    BYTE* BlockPtr = pbBlock;
    BYTE* pbBlockLast = pbBlock + (*pbBlockLen); 
    BYTE  bHead = sizeof(ST_ATTRIBUTE_TYPE) + sizeof(BYTE);   
    for(i = 0; i < bAttributeLen; i++){
	    if(pbBlock + bHead >= pbBlockLast) return STR_BUFFER_TOO_SMALL;
        memcpy(BlockPtr, &pAttribute[i], bHead);
        BlockPtr += bHead;  
        if(pAttribute[i].ulValueLen!=0){
	        if(pbBlock + pAttribute[i].ulValueLen >= pbBlockLast) return STR_BUFFER_TOO_SMALL; 
            memcpy(BlockPtr, pAttribute[i].pValue, pAttribute[i].ulValueLen);
            BlockPtr += pAttribute[i].ulValueLen;
        }     
    }
    *pbBlockLen = BlockPtr - pbBlock;
    return STR_OK;
}
BYTE ParseBlob(BYTE* pbBlock, BYTE bBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *pbAttribute, BYTE** pbMemPtr, BYTE* pbMemEnd)
{
    BYTE i = 0;
    BYTE* BlockPtr = pbBlock;
    BYTE* pbBlockLast = pbBlock + bBlockLen;
    *pbAttribute=0;
    BYTE bHead = sizeof(ST_ATTRIBUTE_TYPE) + sizeof(BYTE);
    for(i = 0; BlockPtr < pbBlockLast; i++){
        if(BlockPtr + bHead >= pbBlockLast) return STR_DATA_CORRUPTED;
        memcpy(&pAttribute[i], BlockPtr, bHead); 
        BlockPtr += bHead;
        if(pAttribute[i].ulValueLen != 0){ 
            if(pbMemPtr + pAttribute[i].ulValueLen >= pbMemEnd) return 
                STR_BUFFER_TOO_SMALL;
            if(BlockPtr + pAttribute[i].ulValueLen >= pbBlockLast) return
                STR_DATA_CORRUPTED;
            memcpy((*pbMemPtr), BlockPtr, pAttribute[i].ulValueLen);
            pAttribute[i].pValue = (*pbMemPtr);
            (*pbMemPtr) += pAttribute[i].ulValueLen;
            BlockPtr += pAttribute[i].ulValueLen;  
        } 
        else pAttribute[i].pValue = NULL;            
        *pbAttribute++;
    }   
    return STR_OK;
}
BYTE Init()
{
    BYTE i;
    for(i = 0; i < MAX_CONNECTIONS; i++){
        Connection[i].Mode = STS_NO_CONNECT;
    }
}
BYTE findparam(ST_ATTRIBUTE* pData, BYTE bDataLen, ST_ATTRIBUTE_TYPE bType)
{
    BYTE i;
    for(i = 0; i < bDataLen; i++) {
        if(pData[i].type == bType){
            return i;			            
        }  
    }   
    return 255;
} 
BYTE ProcessClients(BYTE bConnectionID, BYTE* pbBlob, BYTE* pbBlobLen)
{
    BYTE res = 0;
    BYTE i = 0;
    BYTE Command = 0;
    BOOL NeedAnswer = FALSE;
    BYTE Answers = 0;

    BYTE bAttributeLen = 0;
    BYTE bBlobLen = 0;
    BYTE pbMem[MEM_BUFFER_LEN];
    ST_ATTRIBUTE Data[MAX_ATTRIBUTE];
    BYTE* pbBlobPtr = pbBlob; // ����� ��� ����������������� ������������ �����
    BYTE* pbMemPtr = pbMem;    

    
    ST_ATTRIBUTE Answer[] = {
        {STA_FLAG,  sizeof(BYTE),&Answers }
    };

    memset(pbMem,0,sizeof(pbMem));
    memset(Data,0,sizeof(Data));
    res = ParseBlob(pbBlob, bBlobLen, Data, &bAttributeLen, &pbMemPtr, pbMem + sizeof(pbMem));
    if(res!=STR_OK) return res;
    // ���� ��������� �������    
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
        NeedAnswer = TRUE;
        break;                        
    case STS_AUTH_REQ:
        if(Command == STC_REQEST_AUTH) {
            BYTE login = findparam(Data, bAttributeLen, STA_LOGIN);             
            BYTE password = findparam(Data, bAttributeLen, STA_PASSWORD); 
            if((login == 255)||(password == 255)) {
                Answers = STF_COMMAND_INCOMPLETE;
                Connection[bConnectionID].Mode = STS_NO_CONNECT;                
            } else {
                if(1){ // �������� ������������                
                    Answers = STF_ACCEPTED;
                    Connection[bConnectionID].Mode = STS_CONNECTED;
                } else {
                    Answers = STF_DECLINED; 
                    Connection[bConnectionID].Mode = STS_NO_CONNECT;                
                }
            }
            NeedAnswer = FALSE;            
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
        res = FormBlob(Answer, 1, &pbBlobPtr, &bBlobLen);
        pbBlobPtr+=bBlobLen; // ����������� ���������
        *pbBlobLen+=bBlobLen; // ����������� ������
        if(res!=STR_OK) return res;
        if(Connection[bConnectionID].Mode == STS_NO_CONNECT) {
            res = STR_NEED_DISCONNECT;
        } else {
            res = STR_NEED_ANSWER;
        }
    }
    return res;
}




