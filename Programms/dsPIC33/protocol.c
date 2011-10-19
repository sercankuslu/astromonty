#include "protocol.h"

typedef struct ST_CONNECTION {
    BYTE Flag;
}ST_CONNECTION;

ST_CONNECTION Connection[5];

BYTE FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE ulAttribute, BYTE* Block, WORD* ulBlockLen)
{
    BYTE i = 0;
    BYTE* BlockPtr = Block;    
    for(i = 0; i < ulAttribute; i++){
        memcpy(BlockPtr, &pAttribute[i].type, sizeof(ST_ATTRIBUTE_TYPE));
        BlockPtr+=sizeof(ST_ATTRIBUTE_TYPE);
        memcpy(BlockPtr, &pAttribute[i].ulValueLen, sizeof(WORD));
        BlockPtr+=sizeof(WORD);
        if(pAttribute[i].ulValueLen!=0){
            memcpy(BlockPtr, pAttribute[i].pValue, pAttribute[i].ulValueLen);
            BlockPtr+=pAttribute[i].ulValueLen;  
        }     
    }
    *ulBlockLen = BlockPtr - Block;
    return STR_OK;
}
BYTE ParseBlob(BYTE* Block, WORD ulBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *ulAttribute, BYTE** MemPtr)
{
    BYTE i = 0;
    BYTE* BlockPtr = Block;
    *ulAttribute=0;
    for(; BlockPtr < Block+ulBlockLen; i++){
        memcpy(&pAttribute[i].type, BlockPtr, sizeof(ST_ATTRIBUTE_TYPE)); 
        BlockPtr+=sizeof(ST_ATTRIBUTE_TYPE);
        memcpy(&pAttribute[i].ulValueLen, BlockPtr, sizeof(WORD));   
        BlockPtr+=sizeof(WORD);
        if(pAttribute[i].ulValueLen!=0){
            memcpy((*MemPtr), BlockPtr, pAttribute[i].ulValueLen);
            pAttribute[i].pValue = (*MemPtr);
            (*MemPtr)+=pAttribute[i].ulValueLen;
            BlockPtr+=pAttribute[i].ulValueLen;  
        } 
        else pAttribute[i].pValue = NULL;            
        *ulAttribute++;
    }    
}
BYTE ProcessAttributes(BYTE ConnectionID, BYTE* Blob, WORD* BlobLen)
{
    BYTE Mem[64];
    BYTE* MemPtr = Mem;
    memset(Mem,0,sizeof(Mem));
    ST_ATTRIBUTE Data[MAX_ATTRIBUTE];
    memset(Data,0,sizeof(Data));
    BYTE ulAttributeLen=0;
    BYTE res = 0;
    res = ParseBlob(Blob, *BlobLen, Data, &ulAttributeLen, &MemPtr);
    BYTE i;
    BYTE Command=0;
    for(i = 0; i < MAX_ATTRIBUTE; i++) {
		if(Data[i].type==STA_COMMAND){
			Command = *((BYTE*)Data[i].pValue);
			switch(Command) {
				case STC_REQEST_CONNECT: 
				Nop();
				break;
				default : ;	
			}			
			break;
		}  else {
		Nop();
		}  
    }

}




