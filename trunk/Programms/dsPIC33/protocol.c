#include "protocol.h"
typdef struct ST_CONNECTION {
    BYTE Flag;
}ST_CONNECTION;
ST_CONNECTION Connection[5];

BYTE FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE ulAttribute, BYTE* Block, BYTE* ulBlockLen)
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
BYTE ParseBlob(BYTE* Block, BYTE ulBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *ulAttribute, BYTE** MemPtr)
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
        } else pAttribute[i].pValue = NULL;            
        *ulAttribute++;
    }    
}
BYTE ProcessAttributes(BYTE ConnectionID, BYTE* Blob, WORD* BlobLen)
{
    BYTE Mem[64];
     BYTE* MemPtr = Mem;
    memset(Mem,0,sizeof(Mem));
     ST_ATTRIBUTE Data[10];
    memset(Data,0,sizeof(Data));
    BYTE ulAttributeLen=0;
     BYTE res = 0;

    res = ParseBlob(Blob, BlobLen, Data, &ulAttributeLen, &MemPtr);

}




