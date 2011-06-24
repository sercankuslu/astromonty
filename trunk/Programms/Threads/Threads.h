#ifndef __THREADS_H
#define __THREADS_H

#define STACK_SIZE    128
#define TASK_ACTIVE   1
#define TASK_CURRENT  2
typedef struct _TASKS_LIST
{
    BYTE    TaskID;
    BYTE    Status;//TASK_ACTIVE|TASK_CURRENT
    WORD    Time;    
    WORD*   W15;
    WORD*   W14;
    WORD*   SPBeg;
    WORD*   SPEnd;
    DWORD*  PC;
    WORD   	Stack[STACK_SIZE];
    
    //WORD    ;
} TASKS_LIST;

//static WORD Stack1[STACK_SIZE];

#define TASK_COUNT 10
BYTE InitMultiTasking(void);
BYTE StartMultiTasking(void);
BYTE StartProcess(void* process, DWORD TimeNeeded);

#endif
