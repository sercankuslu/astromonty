
#include "stdafx.h"
#include "roundbuffer.h"

#define ATTR_SIZE 16
#define MEM_SIZE 64
#if defined(__18CXX)
#pragma udata ATTRIBUTE = 0x500
#endif
static ST_ATTRIBUTE AttrBuffer[ATTR_SIZE];
static BYTE MemoryBuffer[64];
#pragma udata

static WORD MemBegin = 0;
static WORD MemEnd = 0;
static WORD MemAllocated = 0;
static WORD MemFree = MEM_SIZE;
static BYTE AttrBegin = 0;
static BYTE AttrEnd = 0;
static BYTE AttrFree = ATTR_SIZE;

int MoveDataToBuffer(BYTE* D, WORD Count);
int MoveDataFromBuffer(BYTE* D, WORD Count);

int PushAttr( ST_ATTRIBUTE Attr )
{
	WORD C = 0;
	if (AttrFree == 0) return -1; // no memory	
	if(MoveDataToBuffer((BYTE*)Attr.pValue, Attr.ulValueLen)==0){
		AttrBuffer[AttrEnd].type = Attr.type;
		AttrBuffer[AttrEnd].ulValueLen = Attr.ulValueLen;
		AttrBuffer[AttrEnd].pValue = NULL;
		
		AttrEnd++;
		if (AttrEnd >= ATTR_SIZE) {
			AttrEnd = 0;
		}
		AttrFree--;
	} else return -1;
	return 0;
}

int PopAttr( ST_ATTRIBUTE* Attr )
{
	WORD C = 0;
	if (AttrFree == 0) return -1; // no memory	
	if(MoveDataFromBuffer((BYTE*)Attr->pValue, AttrBuffer[AttrBegin].ulValueLen)==0){
		Attr->type = AttrBuffer[AttrBegin].type;
		Attr->ulValueLen = AttrBuffer[AttrBegin].ulValueLen;
		AttrBegin++;
		if (AttrBegin >= ATTR_SIZE) {
			AttrBegin = 0;
		}
		AttrFree++;
	} else return -1;
	return 0;
}
int MoveDataToBuffer(BYTE* D, WORD Count)
{
	WORD C = 0;
	if (Count > MemFree) return -1; // no memory
	if (Count + MemEnd > MEM_SIZE)	{
		C = MEM_SIZE - MemEnd;
		memcpy(&MemoryBuffer[MemEnd], &D[0], C);
		memcpy(&MemoryBuffer[0], &D[C], Count - C);
		MemEnd = Count - C;		
	} else {		
		memcpy(&MemoryBuffer[MemEnd], &D[0], Count);
		MemEnd+=Count;		
	}
	if(MemEnd >= MEM_SIZE) MemEnd = 0;
	MemFree-=Count;
	return 0;
}

int MoveDataFromBuffer(BYTE* D, WORD Count)
{
	WORD C = 0;
	if (Count > MEM_SIZE - MemFree) return -1; // no data in memory
	if (Count + MemBegin > MEM_SIZE)	{
		C = MEM_SIZE - MemBegin;
		memcpy(&D[0], &MemoryBuffer[MemBegin], C);
		memcpy(&D[C], &MemoryBuffer[0], Count - C);
		MemBegin = Count - C;		
	} else {		
		memcpy(&D[0], &MemoryBuffer[MemBegin], Count);
		MemBegin+=Count;
	}
	if(MemBegin >= MEM_SIZE) MemBegin = 0;
	MemFree+=Count;	
	return 0;
}

//  [000000________00000]