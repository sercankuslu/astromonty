#pragma once

#include "resource.h"

#define MAX_FILE_NAME_LENGTH 32

/*
 *        создать, если нет
 *        | удалить файл, если есть/не удалять файл
 *        | | текстовый(0)/бинарный(1)
 *        | | | запись в конец/запись по указателю
 *        | | | | чтение разрешено
 *        | | | | | запись разрешена
 *        | | | | | |
 *   |7|6|5|4|3|2|1|0|
 */
typedef union {
    BYTE Val;
    //BYTE_VAL VAL;
    struct {
        unsigned RE:1;
        unsigned WE:1;
        unsigned WrToEnd:1;
        unsigned Binary:1;
        unsigned Renew:1;
        unsigned New:1;
        unsigned b6:1;
        unsigned b7:1;
    } bits;
}FILESTATE;

typedef struct _INODE_RECORD
{
    WORD wInodeID;                  // Индекс инода        
    WORD wFlags;                    // флаги
    DWORD dwSize;                   // размер файла
    WORD wSectorsCount;             // количество секторов
    WORD pwTable[11];               // таблица размещения секторов 
                                    // [0..7] прямая адресация
                                    // [8] косвенная
                                    // [9] двойная косвенная
                                    // [10] тройная косвенная
} INODE_RECORD;

typedef struct _DIR_RECORD
{
    WORD wInodeID;                          // Индекс инода
    BYTE bNameLength;                       // Длинна имени файла
    char pcFileName[MAX_FILE_NAME_LENGTH];  // Имя файла
} DIR_RECORD;

// запись сотояния файла
typedef struct _uFS_FILE
{
    INODE_RECORD Inode;
    DIR_RECORD Dir;
    DWORD DataPointer;
    FILESTATE Flags;
    BYTE ErrorState;
} uFS_FILE;


int uFS_Init();
int CreateFS();
uFS_FILE *  FS_fopen ( const char * filename, const char * mode );
int uFS_fwrite ( BYTE * ptr, DWORD size, DWORD count, uFS_FILE * stream );
int uFS_fread ( BYTE * ptr, DWORD size, DWORD count, uFS_FILE * stream );
int uFS_fseek ( uFS_FILE * stream, int offset, int origin );


