#include "stdafx.h"
#include "../../dsPIC33/protocol.h"
#include "roundbuffer.h"

#define ATTR_BUFF_SIZE 8
#define MEM_BUF_SIZE 32
#define BUFFERS_COUNT 2

typedef struct MEMORY_BUFFER {
        WORD MemBegin;
        WORD MemEnd;
        WORD MemFree;
        WORD MemSize;
        BYTE* MemoryBuffer;
} MEMORY_BUFFER;

typedef struct ATTR_BUFFER {
        BYTE AttrBegin;
        BYTE AttrEnd;
        BYTE AttrFree;
        BYTE AttrSize;	
        ST_ATTRIBUTE* AttrBuffer;
        MEMORY_BUFFER MemoryBuffer;	
} ATTR_BUFFER;

#if defined(__18CXX)
#pragma udata ATTRIBUTE = 0x800
#endif
static ST_ATTRIBUTE AttrBuffer1[ATTR_BUFF_SIZE];
static ST_ATTRIBUTE AttrBuffer2[ATTR_BUFF_SIZE];
static BYTE MemoryBuffer1[MEM_BUF_SIZE];
static BYTE MemoryBuffer2[MEM_BUF_SIZE];
#if defined(__18CXX)
//#pragma udata MEMORY = 0x600
#pragma udata
#endif


/*static WORD MemBegin = 0;
static WORD MemEnd = 0;
static WORD MemFree = MEM_SIZE;
static BYTE AttrBegin = 0;
static BYTE AttrEnd = 0;
static BYTE AttrFree = ATTR_SIZE;
*/

static ATTR_BUFFER RoundBuffers[BUFFERS_COUNT]; 
static BOOL IsInit = FALSE;
RB_RV MoveDataToBuffer(BYTE* D, WORD Count, MEMORY_BUFFER* MemBuf);
RB_RV MoveDataFromBuffer(BYTE* D, WORD Count, MEMORY_BUFFER* MemBuf);

RB_RV RoundBufferInit(void)
{
    BYTE i;
    for(i = 0; i< BUFFERS_COUNT; i++){
        RoundBuffers[i].AttrBegin = 0;
        RoundBuffers[i].AttrEnd = 0;
        RoundBuffers[i].AttrFree = ATTR_BUFF_SIZE;
        RoundBuffers[i].AttrSize = ATTR_BUFF_SIZE;
        RoundBuffers[i].MemoryBuffer.MemBegin = 0;
        RoundBuffers[i].MemoryBuffer.MemEnd = 0;
        RoundBuffers[i].MemoryBuffer.MemFree = MEM_BUF_SIZE;
        RoundBuffers[i].MemoryBuffer.MemSize = MEM_BUF_SIZE;
    }
    RoundBuffers[0].AttrBuffer = AttrBuffer1;
    RoundBuffers[1].AttrBuffer = AttrBuffer2;
    memset(AttrBuffer1, 0 , ATTR_BUFF_SIZE*sizeof(ST_ATTRIBUTE));
    memset(AttrBuffer2, 0 , ATTR_BUFF_SIZE*sizeof(ST_ATTRIBUTE));
    RoundBuffers[0].MemoryBuffer.MemoryBuffer = MemoryBuffer1;
    RoundBuffers[1].MemoryBuffer.MemoryBuffer = MemoryBuffer2;
    memset(MemoryBuffer1, 0 , MEM_BUF_SIZE);
    memset(MemoryBuffer2, 0 , MEM_BUF_SIZE);
    return RB_OK;
}

BOOL IsDataInBuffer(BYTE BufNumber)
{
    if(!IsInit) {
        RoundBufferInit();
        IsInit = TRUE;
    }	
    return (RoundBuffers[BufNumber].AttrFree < RoundBuffers[BufNumber].AttrSize)?1:0;
}
RB_RV PushAttr( ST_ATTRIBUTE Attr , BYTE BufNumber)
{
    ATTR_BUFFER* Attr_buf = &RoundBuffers[BufNumber];
    RB_RV rv = RB_OK;
    if (Attr_buf->AttrFree == 0) return RB_NO_MEMORY; // no memory
    Attr_buf->AttrBuffer[Attr_buf->AttrEnd].type = Attr.type;
    Attr_buf->AttrBuffer[Attr_buf->AttrEnd].ulValueLen = Attr.ulValueLen;
    Attr_buf->AttrBuffer[Attr_buf->AttrEnd].pValue = NULL;
    if(Attr.ulValueLen > 0){
        rv = MoveDataToBuffer((BYTE*)Attr.pValue, Attr.ulValueLen, &Attr_buf->MemoryBuffer);
        if(rv != RB_OK)
            return rv;
    }	
    Attr_buf->AttrEnd++;
    if (Attr_buf->AttrEnd >= Attr_buf->AttrSize) {
        Attr_buf->AttrEnd = 0;
    }
    if(Attr_buf->AttrFree>0)
        Attr_buf->AttrFree--;
    return RB_OK;
}

RB_RV PopAttr( ST_ATTRIBUTE* Attr, BYTE BufNumber )
{
    ATTR_BUFFER* Attr_buf = &RoundBuffers[BufNumber];
    RB_RV rv = RB_OK;
    if (Attr_buf->AttrFree == Attr_buf->AttrSize) return RB_NO_DATA; // no data
    
    Attr->type = Attr_buf->AttrBuffer[Attr_buf->AttrBegin].type;
    Attr->ulValueLen = Attr_buf->AttrBuffer[Attr_buf->AttrBegin].ulValueLen;	
    if(Attr->ulValueLen>0){
        rv = MoveDataFromBuffer((BYTE*)Attr->pValue, Attr_buf->AttrBuffer[Attr_buf->AttrBegin].ulValueLen, &Attr_buf->MemoryBuffer);
        if(rv!=RB_OK)
            return rv;
    }
    Attr_buf->AttrBegin++;
    if (Attr_buf->AttrBegin >= Attr_buf->AttrSize) {
        Attr_buf->AttrBegin = 0;
    }
    Attr_buf->AttrFree++;    
    return RB_OK;
}
RB_RV MoveDataToBuffer(BYTE* D, WORD Count, MEMORY_BUFFER* MemBuf)
{
    WORD C = 0;
    if (Count > MemBuf->MemFree) return RB_NO_MEMORY; // no memory
    if (Count + MemBuf->MemEnd > MemBuf->MemSize) {
        C = MemBuf->MemSize - MemBuf->MemEnd;
        memcpy((void*)&MemBuf->MemoryBuffer[MemBuf->MemEnd], (void*)&D[0], C);
        memcpy((void*)&MemBuf->MemoryBuffer[0], (void*)&D[C], Count - C);
        MemBuf->MemEnd = Count - C;
    } else {
        memcpy((void*)&MemBuf->MemoryBuffer[MemBuf->MemEnd], (void*)&D[0], Count);
        MemBuf->MemEnd+=Count;
    }
    if(MemBuf->MemEnd >= MemBuf->MemSize) MemBuf->MemEnd = 0;
    MemBuf->MemFree-=Count;
    return RB_OK;
}

RB_RV MoveDataFromBuffer(BYTE* D, WORD Count, MEMORY_BUFFER* MemBuf)
{
    WORD C = 0;
    if (Count > MemBuf->MemSize - MemBuf->MemFree) return RB_NO_DATA; // no data in memory
    if (Count + MemBuf->MemBegin > MemBuf->MemSize) {
        C = MemBuf->MemSize - MemBuf->MemBegin;
        memcpy((void*)&D[0], (void*)&MemBuf->MemoryBuffer[MemBuf->MemBegin], C);
        memcpy((void*)&D[C], (void*)&MemBuf->MemoryBuffer[0], Count - C);
        MemBuf->MemBegin = Count - C;
    } else {
        memcpy((void*)&D[0], (void*)&MemBuf->MemoryBuffer[MemBuf->MemBegin], Count);
        MemBuf->MemBegin+=Count;
    }
    if(MemBuf->MemBegin >= MemBuf->MemSize) MemBuf->MemBegin = 0;
    MemBuf->MemFree+=Count;
    return RB_OK;
}


// запрос данных
// SendData(type) -> помещаем в очередь запросов
// ReciveData()   <- забираем из очереди ответов
// Отправка данных
// SendData()     -> помещаем в очередь запросов
