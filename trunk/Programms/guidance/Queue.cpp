#include "stdafx.h"
#include "Queue.h"

int Queue_Insert( PRIORITY_QUEUE * Queue, BYTE Key, BYTE * Value )
{
    if(Queue->Count < Queue->Size){
        Queue->End->Key = Key; 
        memcpy(Queue->End->Value, Value, Queue->ValueSize);
        Queue->Count++;
        Queue->End++;
        if(Queue->End >= (Queue->Queue + Queue->Size) )
            Queue->End = Queue->Queue;        
    } else 
        return -1;
    return 0;
}

int Queue_First( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value )
{
    if(Queue->Count == 0 )
        return -1;
    if((Key == NULL)&&(Value == NULL)) 
        return -1;
    if(Key != NULL)
        *Key = Queue->Start->Key;    
    if(Value != NULL)
        *Value = Queue->Start->Value;
    return 0;
}

int Queue_Min( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value )
{
    QUEUE_ELEMENT * j = NULL;
    BYTE MinKey = 255;
    QUEUE_ELEMENT * MinIndex = NULL;

    if(Queue->Count == 0)
        return -1;
    if((Key == NULL)&&(Value == NULL)) 
        return -1;

    j = Queue->Start;

    for(BYTE i = 0; i < Queue->Count; i++) {
        if (MinKey > j->Key) {
            MinKey = j->Key;
            MinIndex = j;
        }
        j++;
        if(j >= (Queue->Queue + Queue->Size) )
            j = Queue->Queue;
                    
    }
    if(Key != NULL)
        *Key = MinIndex->Key;        
    if(Value != NULL)
        *Value = MinIndex->Value;
    return 0;
}

int Queue_Extract( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE * Value )
{
    if(Queue->Count == 0)
        return -1;
    if((Key == NULL)&&(Value == NULL)) 
        return -1;

    if(Key != NULL)
        *Key = Queue->Start->Key;
    if(Value != NULL)
        memcpy(Value, Queue->Start->Value, Queue->ValueSize);

    Queue->Start++;
    if(Queue->Start >= (Queue->Queue + Queue->Size) )
        Queue->Start = Queue->Queue;
    Queue->Count--;    

    return 0;
}

int Queue_Init(PRIORITY_QUEUE * Queue, QUEUE_ELEMENT * KeyBuf, BYTE KeyBufSize, BYTE * ValueBuf, BYTE ValueSize )
{
    QUEUE_ELEMENT * j = NULL;
    BYTE * TmpVal = NULL;

    Queue->Queue = KeyBuf;          // буфер ключей очереди
    Queue->Count = 0;               // количество занятых элементов
    Queue->Start = Queue->Queue;               // указатель на точку очереди из которой забираются элементы
    Queue->End = Queue->Queue;                 // указатель на точку очереди в которую записываются элементы
    Queue->Size = KeyBufSize;       // размер буфера очереди
    Queue->Values = ValueBuf;       // буфер значений
    Queue->ValueSize = ValueSize;   // размер одного элемента 

    j = Queue->Queue;
    TmpVal = ValueBuf;
    while (!(j >= (Queue->Queue + Queue->Size))) {
        j->Value = TmpVal;
        TmpVal += ValueSize;
        j++;
    }
    return 0;
}

int Queue_ExtractAllToMin( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value )
{
    if(Queue_Min( Queue, Key, Value) == 0){
        while(Queue->Start->Key != *Key){
            Queue_Delete(Queue);
        }
        return 0;
    }
    return -1;
}

int Queue_Delete( PRIORITY_QUEUE * Queue )
{
    if(Queue->Count == 0)
        return -1;

    Queue->Start++;
    if(Queue->Start >= (Queue->Queue + Queue->Size) )
        Queue->Start = Queue->Queue;
    Queue->Count--;    
    return 0;
}


int Queue_Selftest ()
{
    QUEUE_ELEMENT q[5];
    PRIORITY_QUEUE Q;
    BYTE Values[5*6];
    BYTE k1 = 128;
    BYTE k2 = 64;
    BYTE v1[] = {0,1,2,3,4,5};
    BYTE v2[] = {2,2,2,2,2,2};
    BYTE K = 0;
    BYTE V[] = {0,0,0,0,0,0};
    BYTE* V1 = NULL;
    BYTE i = 0;
    int Error = 0;
    Queue_Init(&Q, q, 5, Values, 6);

    // добавляем 5 значений в очередь
    for (i = 0; i < 5; i++) {
        if(Queue_Insert(&Q, k1, v1))
            Error++;
    }
    // пробуем добавить еще одно
    if(Queue_Insert(&Q, k1, v1) == 0)
        Error++;    

    {    
        if(Queue_Min(&Q, &K, &V1))
            Error++;
        if(memcmp(v1, V1, 6))
            Error++;
    }
    {    
        if(Queue_First(&Q, &K, &V1))
            Error++;
        if(memcmp(v1, V1, 6))
            Error++;
    }

    // получаем 5 значений из очереди
    for (i = 0; i < 5; i++) {
        if(Queue_Extract(&Q, &K, V))
            Error++;
        if(memcmp(v1, V, 6))
            Error++;
    }

    return Error;
}


