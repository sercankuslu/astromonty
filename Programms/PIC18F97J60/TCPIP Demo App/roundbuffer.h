#include "stdafx.h"
#ifndef _ROUNDBUFFER_H_
#define _ROUNDBUFFER_H_

typedef enum RB_RV {
    RB_OK,
    RB_NO_MEMORY,
    RB_NO_DATA,
}RB_RV;

#if !defined(__C30__)
RB_RV PushAttr(ST_ATTRIBUTE Data, BYTE BufNumber);
RB_RV PopAttr(ST_ATTRIBUTE *Data, BYTE BufNumber);
BOOL IsDataInBuffer(BYTE BufNumber);
RB_RV RoundBufferInit(void);
#endif
#endif //_ROUNDBUFFER_H_
