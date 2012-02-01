// protocoltest.cpp: определяет точку входа для консольного приложения.

#include "stdafx.h"
#include "protocol.h"
#include "roundbuffer.h"
#include <memory.h>

int _tmain(int argc, _TCHAR* argv[])
{
	BYTE D[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I',0};
	BYTE T[30];
	memset(T,0,sizeof(T));
	ST_ATTRIBUTE A = {
		0x1A, sizeof(D), D 
	};
	ST_ATTRIBUTE B = {
		0, 0, T
	};
	for(int i = 0; i<200;i++){
		
		PopAttr(&B);
	}

	return 0;
}

