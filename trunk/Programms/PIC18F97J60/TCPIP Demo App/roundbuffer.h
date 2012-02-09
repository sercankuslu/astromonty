#include "stdafx.h"
#ifndef _ROUNDBUFFER_H_
#define _ROUNDBUFFER_H_


#if !defined(__C30__)
int PushAttr(ST_ATTRIBUTE Data, BYTE BufNumber);
int PopAttr(ST_ATTRIBUTE *Data, BYTE BufNumber);
BOOL IsDataInBuffer(BYTE BufNumber);
BOOL RoundBufferInit(void);
#endif
#endif //_ROUNDBUFFER_H_
