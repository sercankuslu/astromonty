
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "threads.h"

#if defined(__C30__)
	void _ISR __attribute__((__no_auto_psv__)) _T6Interrupt(void)
	{
	   WORD t=100;
	   WORD t1=5;
	   WORD t2=6;
	   WORD t3=7;
	   WORD t4=8;
	   t4=t+t1+t2+t3+t4;
	   IFS2bits.T6IF=0;
	}

#endif
#define STACK_SIZE 256
typedef struct _TASKS_LIST
{
    BYTE    TaskID;
    WORD   	Stack[STACK_SIZE];
    WORD    W15;
    //WORD    ;
} TASKS_LIST;

//static WORD Stack1[STACK_SIZE];

#define TASK_COUNT 10
static TASKS_LIST Tasks[TASK_COUNT];

/*********************************************************************
 * Function: BYTE InitMultiTasking(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          (BYTE) 0 - if all OK
 *                         1 - error
 *
 * Side Effects:    Init Timer5 as interval counter
 *
 ********************************************************************/
BYTE InitMultiTasking(void)
{   
	WORD i;
	WORD j;
	for(i=0;i<TASK_COUNT;i++){
		for(j=0;j<STACK_SIZE;j++){
			Tasks[i].Stack[j]=0;	
		}
	} 
    T6CON = 0x8030;
    IEC2bits.T6IE = 1;
    IPC11bits.T6IP = 0x7;
    return 0;
}

BYTE StartProcess(void* process, WORD TimeNeeded)
{
    
    return 0;
}

BYTE StopProcess(BYTE Id)
{
    return 0;
}

BYTE StartCriticalSection(void)
{
    return 0;
}
BYTE EndCriticalSection(void)
{
    return 0;
}



