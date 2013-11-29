#include "stdafx.h"
#include "Queue.h"
#ifdef __C30__
#   include "TCPIP Stack/TCPIP.h"
#endif

//****************

int Queue_Insert( PRIORITY_QUEUE * Queue, BYTE Key, BYTE * Value )
{
    volatile BYTE * Source;
    volatile BYTE * Dest;
    volatile WORD Count;
    if(Queue->Count >= Queue->Size)
        // очередь полна
        return -1;
    
    Queue->End->Key = Key; 
    Source = Value;
    Dest = Queue->End->Value;
    Count = Queue->ValueSize;
    memcpy((BYTE*)Dest, (BYTE*)Source, Count);
    //memcpy((BYTE*)Queue->End->Value, (BYTE*)Value, Queue->ValueSize);
    Queue->End++;
    if(Queue->End >= (Queue->Queue + Queue->Size))
        Queue->End = Queue->Queue;
    Queue->Count++;
    return 0;
}



// Ставит в очередь и возвращает указатель на буфер ( позволит уменьшит количество копирований, но есть риск считать неготовый буфер)
int Queue_GetEndBuffer( PRIORITY_QUEUE * Queue, BYTE Key, BYTE ** Value )
{
    if(Queue->Count >= Queue->Size)
        // очередь полна
        return -1;

    Queue->End->Key = Key;
    if(Value != NULL)
        *Value = Queue->End->Value;
    Queue->End++;
    if(Queue->End >= (Queue->Queue + Queue->Size))
        Queue->End = Queue->Queue;
    // если очередь полна, ставим флаг
    Queue->Count++;
    return 0;

}

int Queue_First( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value )
{
    if(Queue->Count == 0)      // Очередь пуста
        return -1;
    if(Key != NULL)
        *Key = Queue->Start->Key;
    if(Value != NULL)
        *Value = Queue->Start->Value;
    return 0;
}

QUEUE_ELEMENT * Queue_GetMin( PRIORITY_QUEUE * Queue)
{
    QUEUE_ELEMENT * j = NULL;
    BYTE MinKey = 255;
    QUEUE_ELEMENT * MinIndex = NULL;
    BYTE i = 0;

    if(Queue->Count == 0)      // Очередь пуста
        return NULL;

    j = Queue->Start;
    for(i = 0; i < Queue->Count; i++) {
        if (MinKey > j->Key) {
            MinKey = j->Key;
            MinIndex = j;
        }
        j++;
        if(j >= (Queue->Queue + Queue->Size) )
            j = Queue->Queue;
    }
    return MinIndex;
}

int Queue_Min( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value )
{
    QUEUE_ELEMENT * MinIndex = Queue_GetMin(Queue);
    if(MinIndex != NULL){
        if(Key != NULL)
            *Key = MinIndex->Key;
        if(Value != NULL)
            *Value = MinIndex->Value;
        return 0;
    }
    return -1;
}

int Queue_Extract( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE * Value )
{
    int i = 0;
    if(Queue->Count == 0)      // Очередь пуста
        return -1;

    if(Key != NULL)
        *Key = Queue->Start->Key;
    if(Value != NULL)
        memcpy((BYTE*)Value, (BYTE*)Queue->Start->Value, Queue->ValueSize);

    Queue->Start++;
    if(Queue->Start >= (Queue->Queue + Queue->Size) )
        Queue->Start = Queue->Queue;
    Queue->Count--;
    return 0;
}

int Queue_ExtractMin( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE * Value )
{
    QUEUE_ELEMENT * MinIndex = Queue_GetMin(Queue);
    if(MinIndex == NULL)
        return -1;

    while(Queue->Start != MinIndex){
        Queue->Start++;
        if(Queue->Start >= (Queue->Queue + Queue->Size) )
            Queue->Start = Queue->Queue;
        Queue->Count--;
    }

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

int Queue_Init(PRIORITY_QUEUE * Queue, QUEUE_ELEMENT * KeyBuf, BYTE KeyBufSize, BYTE * ValueBuf, WORD ValueSize )
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
    //Queue->QueueFull = 0;           // флаг, указывающий на полую очередь. 
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
    QUEUE_ELEMENT * MinIndex = Queue_GetMin(Queue);
    if(MinIndex == NULL)
        return -1;
    
    while(Queue->Start != MinIndex){
        Queue->Start++;
        if(Queue->Start >= (Queue->Queue + Queue->Size) )
            Queue->Start = Queue->Queue;
        Queue->Count--;
    }

    *Key = MinIndex->Key;
    *Value = MinIndex->Value;
    return 0;
    
}

int Queue_Delete( PRIORITY_QUEUE * Queue )
{
    if(Queue->Count == 0)      // Очередь пуста
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
    //BYTE k2 = 64;
    BYTE v1[] = {0,1,2,3,4,5};
    //BYTE v2[] = {2,2,2,2,2,2};
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


