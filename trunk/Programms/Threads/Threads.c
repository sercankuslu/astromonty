
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "threads.h"
static TASKS_LIST Tasks[TASK_COUNT];
static BYTE CurrentTask;

#if defined(__C30__)
	//void _ISR __attribute__((__no_auto_psv__)) _T6Interrupt(void)
	void _ISR __attribute__((__no_auto_psv__,__interrupt__(
                        save(WREG3,WREG4,WREG5,WREG6,WREG7,
                         WREG8,WREG9,WREG10,WREG11,WREG12,WREG13))
                         //[, irq(irqid)]
                         //[, altirq(altirqid)]                         
             ))  _T6Interrupt(void)

	{  
       /*     	
	   asm volatile ("push.w w0 \n"
	                 "push.w w1 \n"
	                 "push.w w2 \n"
	                 "push.w w3 \n"
	                 "push.w w4 \n"
	                 "push.w w5 \n"
	                 "push.w w6 \n"
	                 "push.w w7 \n"
	                 "push.w w8 \n"
	                 "push.w w9 \n"
	                 "push.w w10 \n"
	                 "push.w w11 \n"
	                 "push.w w12 \n"
	                 "push.w w13 \n"	                 
                     );
       */
       //asm volatile ("mov.w w15,[%0] \n": /* no outputs */ : "r"(&Tasks[0].W15));
       /* 
	   asm volatile ("mov.w w6, 0x1234");
	   asm volatile (
	                 "pop.w w13 \n"
	                 "pop.w w12 \n"
	                 "pop.w w11 \n"
	                 "pop.w w10 \n"
	                 "pop.w w9 \n"
	                 "pop.w w8 \n"
	                 "pop.w w7 \n"
	                 "pop.w w6 \n"
	                 "pop.w w5 \n"
	                 "pop.w w4 \n"
	                 "pop.w w3 \n"
	                 "pop.w w2 \n"
	                 "pop.w w1 \n"
                     "pop.w w0");        
       */
        static WORD wreg14;        
        static BYTE i;
        asm volatile ("mov.w w14, %0 \n" :"=r"(wreg14));  
                     
        for(i=0;i<TASK_COUNT;i++){
            if(Tasks[i].Status==(TASK_ACTIVE + TASK_CURRENT)){
                Tasks[i].Status=TASK_ACTIVE;
                Tasks[i].W14 = wreg14;                               
            }    
        }  
        CurrentTask++;
        while(Tasks[CurrentTask].Status != TASK_ACTIVE){        
            CurrentTask++;
            if(CurrentTask>TASK_COUNT){
                CurrentTask=0;
            }
        }               
        Tasks[CurrentTask].Status = TASK_ACTIVE+TASK_CURRENT;
        
        wreg14 = Tasks[CurrentTask].W14;
        asm volatile ("mov.w %0, w14 \n" 
                      "mov.w %0, w15 \n": : "r"(wreg14));  
               
	    IFS2bits.T6IF=0;
	}

#endif



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
			Tasks[i].Stack[j]=0xFFFF;	
		}
		Tasks[i].TaskID=i;
		Tasks[i].Status = 0;
		Tasks[i].SPBeg = Tasks[i].Stack;
		Tasks[i].W15 = Tasks[i].SPBeg;
		Tasks[i].SPEnd = Tasks[i].SPBeg + STACK_SIZE;
	} 
	for(i=0;i<TASK_COUNT;i++){
		for(j=0;j<STACK_SIZE;j++){
			Tasks[i].Stack[j]=0x0000;	
		}
	}     
    return 0;
}

BYTE StartMultiTasking(void){
    T6CON = 0x8030;
    IEC2bits.T6IE = 1;
    IPC11bits.T6IP = 0x01;
    return 0;
}

/*********************************************************************
 * Function: StartProcess(void* process, WORD TimeNeeded)
 *
 * PreCondition:    InitMultiTasking
 *
 * Input:           void* process - function addres
 *                  WORD TimeNeeded - time
 *
 * Output:          (BYTE) 0 - if all OK
 *                         1 - error
 *
 * Side Effects:    None
 *
 ********************************************************************/
BYTE StartProcess(void* process, DWORD TimeNeeded)
{
    BYTE i;    
    BYTE j; 
    for(i=0;i<TASK_COUNT;i++){
        if(Tasks[i].Status==0){
            Tasks[i].Status = TASK_ACTIVE;
            Tasks[i].PC = (WORD*)process+1;
            Tasks[i].Time = TimeNeeded;            
            
            /*
            Tasks[i].Stack[0] = Tasks[i].SPEnd;
            Tasks[i].Stack[1] = Tasks[i].PC;
            Tasks[i].Stack[2] = 0;
            Tasks[i].Stack[3] = &Tasks[i].Stack[1];
            */
            
            Tasks[i].W15 = Tasks[i].W14;
            j=0;
            Tasks[i].Stack[j++] = 0x021E;            
            Tasks[i].Stack[j++] = 0x0000;
            Tasks[i].Stack[j++] = 0x47F0;
            Tasks[i].Stack[j++] = Tasks[i].PC;
            for(j=4;j<(16+3);j++){
                Tasks[i].Stack[j] = 0x0000;
            }                        
            Tasks[i].Stack[j++] = &Tasks[i].Stack[3];
            Tasks[i].W14 = &Tasks[i].Stack[j];
            return 0;
        }         
    }
    return 1;
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



