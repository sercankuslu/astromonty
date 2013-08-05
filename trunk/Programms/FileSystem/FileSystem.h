#pragma once

#include "resource.h"

#define MAX_FILE_NAME_LENGTH 32
#define uFS_SEEK_SET 0	// Beginning of file
#define uFS_SEEK_CUR 1	// Current position of the file pointer
#define uFS_SEEK_END 2	// End of file 
//#define uFS_MODE_READ 1
//#define uFS_MODE_WRITE 2
//#define uFS_MODE_APPEND 3
//#define uFS_MODE_NORMAL 0
//#define uFS_MODE_EXTENDED 1

enum ModeEnum
{
    uFS_MODE_NONE = 0, 
    uFS_MODE_READ, 
    uFS_MODE_WRITE, 
    uFS_MODE_APPEND,
    uFS_MODE_READ_PLUS, 
    uFS_MODE_WRITE_PLUS, 
    uFS_MODE_APPEND_PLUS
};
enum BinaryEnum { uFS_TEXT = 0, uFS_BINARY};
enum DirEnum { uFS_RECORD_IS_FILE = 0, uFS_RECORD_IS_DIR};

typedef union {
    BYTE Val;
    //BYTE_VAL VAL;
    struct {
        ModeEnum Mode:4; // r/w/a = 01/10/11        
        unsigned bit4:1;
        unsigned bit5:1;
        BinaryEnum Binary:1;
        DirEnum Dir:1;        
    } Enums;
       
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
uFS_FILE *  uFS_fopen ( const char * filename, const char * mode );
int uFS_fclose( uFS_FILE * stream);
int uFS_fwrite ( BYTE * ptr, DWORD size, DWORD count, uFS_FILE * stream );
int uFS_fread ( BYTE * ptr, DWORD size, DWORD count, uFS_FILE * stream );
int uFS_fseek ( uFS_FILE * stream, int offset, int origin );
int uFS_remove ( const char * filename );

