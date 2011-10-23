
#include "protocol.h"

typedef struct ST_CONNECTION {
    BYTE Mode;
}ST_CONNECTION;
#define MAX_CONNECTIONS 5
ST_CONNECTION Connection[MAX_CONNECTIONS];

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
BYTE ParseBlob(BYTE* pbBlock, BYTE bBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *pbAttributeLen, BYTE* pbMem, BYTE bMemLen, BYTE* bMemPos)
{
    BYTE i = 0;    
    //(*pbAttribute) = 0;
    BYTE bHead;
    BYTE j = 0;	
    bHead = sizeof(ST_ATTRIBUTE_TYPE) + sizeof(BYTE);
    for(i = 0; i < bBlockLen; i++){
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
        *pbAttributeLen = i+1;
        j++;
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

    memset(pbMem,0,sizeof(pbMem));
    memset(Data,0,sizeof(Data));
    res = ParseBlob(pbBlob, *pbBlobLen, Data, &bAttributeLen, pbMem, sizeof(pbMem), &bMemPos);
    if(res!=STR_OK) return res;  
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
		        	SendData[0].ulValueLen = sizeof(AppConfig.MyIPAddr);
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




