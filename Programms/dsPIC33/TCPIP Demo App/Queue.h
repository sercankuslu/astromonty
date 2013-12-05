#ifndef __QUEUE_H_
#define __QUEUE_H_

#ifdef __C30__
#   include "GenericTypeDefs.h"
#else
#   include "stdafx.h"
#endif

//
//
//  -------*******--------
//  ^                    |
//  |____________________|

typedef struct QUEUE_ELEMENT {
    BYTE Key;
    BYTE * Value;
} QUEUE_ELEMENT;

typedef struct PRIORITY_QUEUE {
    QUEUE_ELEMENT * Queue;
    BYTE * Values;
    QUEUE_ELEMENT * Start;
    QUEUE_ELEMENT * End;
    //BYTE QueueFull;
    BYTE Count;
    BYTE Size;
    WORD ValueSize;
} PRIORITY_QUEUE;

int Queue_Insert(PRIORITY_QUEUE * Queue, BYTE Key, BYTE * Value);               // добавляет пару (k,v) в хранилище; Значение из массива Value копируется во внутреннее хранилище
int Queue_GetEndBuffer( PRIORITY_QUEUE * Queue, BYTE Key, BYTE ** Value );      // добавляет пару (k,v) в хранилище и возвращает указатель на буфер ( позволит уменьшит количество копирований, но есть риск считать неготовый буфер)
int Queue_Min(PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value);               // возвращает пару (k,v) с минимальным значением ключа k. в Value передаётся указатель на запись во внутреннем хранилище
int Queue_Extract(PRIORITY_QUEUE * Queue, BYTE * Key, BYTE * Value);            // возвращает пару (k,v) удаляя её из хранилища. Значение из внутреннего хранилища копируется в массив Value
int Queue_ExtractMin( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE * Value );       // возвращает пару (k,v) с минимальным значением ключа k. удаляя из хранилища все пары, находящиеся раньше пары с минимальным значением k,
                                                                                // Значение из внутреннего хранилища копируется в массив Value  
int Queue_ExtractAllToMin(PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value);    // возвращает пару (k,v) с минимальным значением ключа k. удаляя из хранилища все пары, находящиеся раньше пары с минимальным значением k
int Queue_Init(PRIORITY_QUEUE * Queue, QUEUE_ELEMENT * KeyBuf, BYTE KeyBufSize, BYTE * ValueBuf, WORD ValueSize );         // инициирует очередь
int Queue_Delete(PRIORITY_QUEUE * Queue);                                       // удаляет пару из начала очереди
int Queue_Revert( PRIORITY_QUEUE * Queue );                                     // Удаляет с хвоста очереди ( отменяет добавление )
int Queue_Selftest ();
int Queue_First(PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value);             // Возрвращает первый в очереди элемент ( самый старый )
int Queue_Last( PRIORITY_QUEUE * Queue, BYTE * Key, BYTE ** Value );            // Возрвращает последний в очереди элемент ( самый новый )
/*
// Example
QUEUE_ELEMENT q[5];
PRIORITY_QUEUE Q;
BYTE Values[5*6];
BYTE k1 = 128;
BYTE v1[] = {1,1,1,1,1,1};
BYTE K = 0;
BYTE V[] = {0,0,0,0,0,0};

// Init
Queue_Init(&Q, q, 5, Values, 6);
// add values
Queue_Insert( &Q, k1, v1);
Queue_Insert( &Q, k1, v1);
Queue_Insert( &Q, k1, v1);
// get min value
Queue_Min(&Q, &K, V );
// remove all values before high priority
Queue_ExtractAllToMin(&Q, &K, V);

*/

#endif //__QUEUE_H_
