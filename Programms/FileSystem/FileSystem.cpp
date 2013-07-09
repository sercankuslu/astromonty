// FileSystem.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "FileSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Единственный объект приложения

CWinApp theApp;

using namespace std;

BYTE FS[256*65536];


//FS_FREE 
BYTE    FreeSectors[8192];
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
    } FILESTATEbits;
}FILESTATE;

typedef struct _DIR_RECORD
{
    WORD wInodeID;                  // Индекс инода
    BYTE bNameLength;               // Длинна имени файла
    char * pcFileName;              // Имя файла
} DIR_RECORD;

typedef struct _INODE_RECORD
{
    WORD wInodeID;                  // Индекс инода        
    WORD wFlags;                    // флаги
    DWORD dwSize;                   // размер файла
    WORD pwTable[12];               // таблица размещения секторов 
                                    // [0..8] прямая адресация
                                    // [9] косвенная
                                    // [10] двойная косвенная
                                    // [11] тройная косвенная
} INODE_RECORD;

// запись сотояния файла
typedef struct _uFS_FILE
{
    INODE_RECORD Inode;
    DIR_RECORD Dir;
    DWORD DataPointer;
    FILESTATE Flags;
} uFS_FILE;

#define SECTORS_COUNT 65536
#define SECTOR_SIZE 256
#define TABLE_SECTOR_SIZE 128
#define SECTOR_MASK 0x00FF
#define TABLE_SECTOR_MASK 0x007F
#define TABLE_SECTOR_SIZE_BIT 7
#define DATA_SECTOR_SIZE_BIT 8
#define INDERECT_ADDR_x1_SIZE TABLE_SECTOR_SIZE
#define INDERECT_ADDR_x2_SIZE TABLE_SECTOR_SIZE*TABLE_SECTOR_SIZE
#define INDERECT_ADDR_x3_SIZE TABLE_SECTOR_SIZE*TABLE_SECTOR_SIZE*TABLE_SECTOR_SIZE
#define INVALID_SECTOR_NUMBER 0xFFFF

int uFS_ClaimFreeSector(WORD * ClaimedSector);
// ***************************************************************************
int uFS_Read(DWORD Addr, BYTE * Dest, DWORD Count)
{
    memcpy(Dest, &FS[Addr],Count);
    return 0;
}
int uFS_Write(DWORD Addr, BYTE * Source, DWORD Count)
{
    memcpy(&FS[Addr], Source, Count);
    return 0;
}
// ***************************************************************************
WORD GetIndirectSectorNumber(WORD SectorNumber, WORD WordPosition)
{   
    DWORD AddrInSector = (SectorNumber << DATA_SECTOR_SIZE_BIT) | ((WordPosition << 1) & SECTOR_MASK);
    WORD DataAddr;
    uFS_Read(AddrInSector, (BYTE*)&DataAddr, sizeof(WORD));
    return DataAddr;
}
// ***************************************************************************
int SetIndirectSectorNumber(WORD SectorNumber, WORD WordPosition, WORD Number)
{   
    DWORD AddrInSector = (SectorNumber << DATA_SECTOR_SIZE_BIT) | ((WordPosition << 1) & SECTOR_MASK);
    return uFS_Write(AddrInSector, (BYTE*)&Number, sizeof(WORD));
}

// ***************************************************************************
int uFS_GetSectorAddr(
    INODE_RECORD Inode,             // Inode файла
    DWORD Addr, DWORD * SectorAddr )
{
    if(Addr >= Inode.dwSize) 
        return -1; // EOF

    WORD SectorNumber = (Addr & (~SECTOR_MASK)) >> DATA_SECTOR_SIZE_BIT;
    if(SectorNumber < 9){
        // Прямая адресация
        if(Inode.pwTable[SectorNumber] == INVALID_SECTOR_NUMBER)
            return -1;
        *SectorAddr = (Inode.pwTable[SectorNumber]);
        return 0;
    } else SectorNumber -= 9;

    if(SectorNumber < INDERECT_ADDR_x1_SIZE){

        // Косвенная адресация
        // Inode.pwTable[9]-->table1[]-->data
        //               table1[128]  data[256]
        // SectorNumber: 0000000      00000000
        //               0x7F         0xFF
        if(Inode.pwTable[9] == INVALID_SECTOR_NUMBER)
            return -1;
        *SectorAddr = (GetIndirectSectorNumber(Inode.pwTable[9], SectorNumber));
        return 0;

    } else SectorNumber -= INDERECT_ADDR_x1_SIZE;

    if(SectorNumber < INDERECT_ADDR_x2_SIZE){ 

        // Двойная косвенная адресация
        // Inode.pwTable[10]-->table1[]-->table2[]-->data
        //               table1[128]  table2[128]  data[256]
        // SectorNumber: 0000000      0000000      00000000
        //               0x7F         0x7F         0xFF
        if(Inode.pwTable[10] == INVALID_SECTOR_NUMBER)
            return -1;
        WORD Table1 = SectorNumber >> TABLE_SECTOR_SIZE_BIT;
        WORD Table2 = SectorNumber & TABLE_SECTOR_MASK;
        WORD Addr2 = GetIndirectSectorNumber(Inode.pwTable[10], Table1);
        if(Addr2 == INVALID_SECTOR_NUMBER)
            return -1;
        *SectorAddr = GetIndirectSectorNumber(Addr2, Table2);
        if(*SectorAddr == INVALID_SECTOR_NUMBER)
            return -1;
        return 0;
    } else SectorNumber -= INDERECT_ADDR_x2_SIZE;

    if(SectorNumber < INDERECT_ADDR_x3_SIZE){

        // Тройная косвенная адресация
        // Inode.pwTable[10]-->table1[]-->table2[]-->table3[]-->data
        //               table1[128]  table2[128]  table3[128]  data[256]
        // SectorNumber: 0000000      0000000      0000000      00000000
        //               0x7F         0x7F         0x7F         0xFF

        WORD Table1 = SectorNumber >> (TABLE_SECTOR_SIZE_BIT * 2);
        WORD Table2 = (SectorNumber >> TABLE_SECTOR_SIZE_BIT) & TABLE_SECTOR_MASK;
        WORD Table3 = SectorNumber & TABLE_SECTOR_MASK;
        if(Inode.pwTable[11] == INVALID_SECTOR_NUMBER)
            return -1;
        WORD Addr2 = GetIndirectSectorNumber(Inode.pwTable[11], Table1);
        if(Addr2 == INVALID_SECTOR_NUMBER)
            return -1;
        WORD Addr3 = GetIndirectSectorNumber(Addr2, Table2);
        if(Addr3 == INVALID_SECTOR_NUMBER)
            return -1;
        *SectorAddr = GetIndirectSectorNumber(Addr3, Table3);
        if(*SectorAddr == INVALID_SECTOR_NUMBER)
            return -1;
        return 0;
    }
    return -1;
}


// ***************************************************************************
int uFS_GetAddr(
    INODE_RECORD Inode,             // Inode файла
    DWORD Addr, DWORD * DataAddr )
{
    DWORD SectorAddr;
    int Err = uFS_GetSectorAddr(Inode, Addr, &SectorAddr);
    if(Err == 0){
        *DataAddr = (SectorAddr  << DATA_SECTOR_SIZE_BIT) | (Addr & SECTOR_MASK);
    }
    return Err;
}


// ***************************************************************************
int uFS_AddSector(INODE_RECORD * Inode, DWORD * SectorAddr)             // Inode файла    
{

    if((Inode->dwSize & SECTOR_MASK) != 0 ){ // Если адрес не кратный сектору, то новый сектор не нужен
        return 0;
    }

    // Определить текущее количество секторов в файле
    WORD SectorNumber = (Inode->dwSize & (~SECTOR_MASK)) >> DATA_SECTOR_SIZE_BIT;

    // Определить тип адресации    
    if(SectorNumber < 9){

        // Прямая адресация
        if(Inode->pwTable[SectorNumber] == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSector(&Inode->pwTable[SectorNumber]) != 0)
                    return -1; // нет свободных секторов
            *SectorAddr = (Inode->pwTable[SectorNumber] << DATA_SECTOR_SIZE_BIT);
            return 0;
        } else return 1;

    } else SectorNumber -= 9;

    if(SectorNumber < INDERECT_ADDR_x1_SIZE){

        // Косвенная адресация
        // Inode.pwTable[9]-->table1[]-->data
        //               table1[128]  data[256]
        // SectorNumber: 0000000      00000000
        // 
        if((SectorNumber == 0) && (Inode->pwTable[9] == INVALID_SECTOR_NUMBER )){
            if(uFS_ClaimFreeSector(&Inode->pwTable[9]) != 0)
                return -1; // нет свободных секторов
        }
        DWORD Table1 = (GetIndirectSectorNumber(Inode->pwTable[9], SectorNumber));
        if(Table1 == INVALID_SECTOR_NUMBER){
            WORD Number = INVALID_SECTOR_NUMBER;
            if(uFS_ClaimFreeSector(&Number) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Inode->pwTable[9], SectorNumber, Number);
            *SectorAddr = Number  << DATA_SECTOR_SIZE_BIT;
        }
        return 0;

    } else SectorNumber -= INDERECT_ADDR_x1_SIZE;

    if(SectorNumber < INDERECT_ADDR_x2_SIZE){ 

        // Двойная косвенная адресация
        // Inode.pwTable[10]-->table1[]-->table2[]-->data
        //               table1[128]  table2[128]  data[256]
        // SectorNumber: 0000000      0000000      00000000

        if((SectorNumber == 0) && (Inode->pwTable[10] == INVALID_SECTOR_NUMBER )){
            if(uFS_ClaimFreeSector(&Inode->pwTable[10]) != 0)
                return -1; // нет свободных секторов
        }

        WORD Table1 = SectorNumber >> TABLE_SECTOR_SIZE_BIT;
        WORD Table2 = SectorNumber & TABLE_SECTOR_MASK;

        WORD Addr1 = (GetIndirectSectorNumber(Inode->pwTable[10], Table1));

        if(Addr1 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSector(&Addr1) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Inode->pwTable[10], Table1, Addr1);
        }

        WORD Addr2 = (GetIndirectSectorNumber(Addr1, Table2));
        if(Addr2 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSector(&Addr2) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Addr1, Table2, Addr2);
        }
        *SectorAddr = Addr2  << DATA_SECTOR_SIZE_BIT;
        return 0;

    } else SectorNumber -= INDERECT_ADDR_x2_SIZE;

    if(SectorNumber < INDERECT_ADDR_x3_SIZE){

        // Тройная косвенная адресация
        // Inode.pwTable[10]-->table1[]-->table2[]-->table3[]-->data
        //               table1[128]  table2[128]  table3[128]  data[256]
        // SectorNumber: 0000000      0000000      0000000      00000000

        if((SectorNumber == 0) && (Inode->pwTable[11] == INVALID_SECTOR_NUMBER )){
            if(uFS_ClaimFreeSector(&Inode->pwTable[11]) != 0)
                return -1; // нет свободных секторов
        }

        WORD Table1 = SectorNumber >> (TABLE_SECTOR_SIZE_BIT * 2);
        WORD Table2 = (SectorNumber >> TABLE_SECTOR_SIZE_BIT) & TABLE_SECTOR_MASK;
        WORD Table3 = SectorNumber & TABLE_SECTOR_MASK;

        WORD Addr1 = (GetIndirectSectorNumber(Inode->pwTable[11], Table1));

        if(Addr1 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSector(&Addr1) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Inode->pwTable[11], Table1, Addr1);
        }

        WORD Addr2 = (GetIndirectSectorNumber(Addr1, Table2));
        if(Addr2 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSector(&Addr2) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Addr1, Table2, Addr2);
        }

        WORD Addr3 = (GetIndirectSectorNumber(Addr2, Table3));
        if(Addr3 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSector(&Addr3) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Addr2, Table3, Addr3);
        }
        *SectorAddr = Addr3  << DATA_SECTOR_SIZE_BIT;

        return 0;
    }
    return -1;

}

// читает данные с квантованием по размеру сектора.
// возвращает количество реально прочитанных байт
int uFS_ReadData(DWORD Addr, BYTE * Dest, int Count)
{
    int CountToRead = SECTOR_SIZE - (Addr & SECTOR_MASK);
    if(Addr == ((INVALID_SECTOR_NUMBER << DATA_SECTOR_SIZE_BIT) | SECTOR_MASK)){
        return -1;
    }
    if(Count == 0) 
        return 0;
    if(Count >= CountToRead){
        uFS_Read(Addr, Dest, CountToRead);
        return CountToRead;
    } else {
        uFS_Read(Addr, Dest, Count);
        return Count;
    }
    return 0;
}

// Записывает данные с квантованием по размеру сектора.
// возвращает количество реально записанных байт
int uFS_WriteData(DWORD Addr, BYTE * Source, int Count)
{
    int CountToWrite = SECTOR_SIZE - (Addr & SECTOR_MASK);
    if(Addr == ((INVALID_SECTOR_NUMBER << DATA_SECTOR_SIZE_BIT) | SECTOR_MASK)){
        return -1;
    }
    if(Count == 0) 
        return 0;
    if(Count >= CountToWrite){
        uFS_Write(Addr, Source, CountToWrite);
        return CountToWrite;
    } else {
        uFS_Write(Addr, Source, Count);
        return Count;
    }
    return 0;
}

// читает данные из файла
// возвращает количество прочитанных байт в случае успеха или код ошибки
int uFSReadFile(
    INODE_RECORD Inode,             // Inode файла
    DWORD Address,                  // Адрес внутри файла
    BYTE * Dest,                    // Адрес буфера
    int Count                       // Количество в байтах
    )                               // Возвращает количество реально считанных байт или значение < 0 при ошибке
{
    DWORD PhysicalAddr = 0;
    int Err = 0;
    int ReadedCnt = 0;
    
    if(Count + Address > Inode.dwSize){
        Count = Inode.dwSize - Address;
    }
    if (Count < 0)
        return -1;
    
    while(Count != ReadedCnt){

        Err = uFS_GetAddr(Inode, Address + ReadedCnt , &PhysicalAddr);
        if(Err != 0){
            return Err;
        }

        int Cnt = uFS_ReadData(PhysicalAddr, &Dest[ReadedCnt], Count - ReadedCnt);
        if(Cnt < 0 ){
            return Cnt;
        } 
        ReadedCnt += Cnt;
    }

    return ReadedCnt;
}

// читает данные из файла
// возвращает количество прочитанных байт в случае успеха или код ошибки
int uFSWriteFile(
    INODE_RECORD * Inode,             // Inode файла
    DWORD Address,                  // Адрес внутри файла
    BYTE * Source,                    // Адрес буфера
    int Count                       // Количество в байтах
    )                               // Возвращает количество реально считанных байт или значение < 0 при ошибке
{
    DWORD PhysicalAddr = 0;
    int Err = 0;
    int WritedCnt = 0;
       
    if (Count < 0)
        return -1;

    while(Count != WritedCnt){

        Err = uFS_AddSector(Inode, &PhysicalAddr);
        if(Err != 0){
            return Err;
        }
        PhysicalAddr |= (Address & SECTOR_MASK);
        int Cnt = uFS_WriteData(PhysicalAddr, &Source[WritedCnt], Count - WritedCnt);
        if(Cnt < 0 ){
            return Cnt;
        } 
        WritedCnt += Cnt;
        Inode->dwSize += Cnt;
    }

    return WritedCnt;
}

int uFS_ClaimFreeSector(WORD * ClaimedSector)
{
    for(int i = 0; i < sizeof(FreeSectors); i++){
        if(FreeSectors[i] > 0){         
            int j = 0;
            while(!((FreeSectors[i] >> j) & 0x1)){
                j++;
            }
            FreeSectors[i] &= (~(1 << j));
            *ClaimedSector = i * 8 + j;
            return 0;
        }
    }
    return -1;
}

BYTE bc[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
BYTE bBitCount(BYTE x)
{
    return  bc[x & 0x0F] + bc[(x >> 4) & 0x0F];
}

BYTE wBitCount(WORD x)
{
    return  bc[x & 0x000F] + bc[(x >> 4) & 0x000F] + bc[(x >> 8) & 0x000F] + bc[(x >> 12) & 0x000F];
}

/*
BYTE bBitCount(BYTE x)
{
    x -=  (x>>1) & 0x55;
    x  = ((x>>2) & 0x33) + (x & 0x33);
    x  = ((x>>4) + x) & 0x0f;
    //x *= 0x01;
    return  x;
}

WORD wBitCount(WORD x)
{
    x -=  (x>>1) & 0x5555;
    x  = ((x>>2) & 0x3333) + (x & 0x3333);
    x  = ((x>>4) + x) & 0x0f0f;
    x *= 0x0101;
    return  x >> 8;
}
*/

DWORD uFS_GetFreeSectorCount()
{
    DWORD count = 0;
    WORD * wFS = (WORD*)FreeSectors;
    for(int i = 0; i < sizeof(FreeSectors)/2; i++){
        count += wBitCount(wFS[i]);
    }
    return count;
}

// Wite block of data from stream
int uFS_fwrite ( BYTE * ptr, DWORD size, DWORD count, uFS_FILE * stream )
{
    if(stream->Flags.FILESTATEbits.WE == 0) 
        return -1;

    int Cnt = uFSWriteFile(&stream->Inode, stream->DataPointer, (BYTE*)ptr, count*size);
    if(Cnt != 0){
        stream->DataPointer += Cnt;
        return Cnt;
    }
    return -1;
}

// Read block of data from stream
int uFS_fread ( BYTE * ptr, DWORD size, DWORD count, uFS_FILE * stream )
{
    if(stream->Flags.FILESTATEbits.RE == 0) 
        return -1;

    int Cnt = uFSReadFile(stream->Inode, stream->DataPointer, (BYTE*)ptr, count*size);
    if(Cnt != 0){
        stream->DataPointer += Cnt;
        return Cnt;
    }
    return -1;
}

#define SEEK_SET 0	// Beginning of file
#define SEEK_CUR 1	// Current position of the file pointer
#define SEEK_END 2	// End of file 

int uFS_fseek ( uFS_FILE * stream, int offset, int origin )
{
    if(stream->Flags.FILESTATEbits.RE == 1) 
        return -1;

    switch (origin) {
    case SEEK_SET:
        if(offset <0)
            return -1;
        if(offset < (int)stream->Inode.dwSize){
            stream->DataPointer = offset;
        }
    	break;
    case SEEK_CUR: 
        {
            int res = stream->DataPointer + offset;
            if(res <= 0){
                stream->DataPointer = 0;
            } else {
                if(res > (int)stream->Inode.dwSize){
                    stream->DataPointer = stream->Inode.dwSize;
                } else 
                    stream->DataPointer = res;
            }
            break;
        }
    case SEEK_END:
        if (offset < 0) {
            int res = stream->DataPointer + offset;
            if(res <= 0){
                stream->DataPointer = 0;
            } else {
                stream->DataPointer = res;
            }
        }
        break;
    default:
        return -1;
    }
    return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;
    DWORD PhysicalAddr = 0;
    WORD Data[65536];
    WORD Data1[65536];
    int Count = 0;
    // инициализировать MFC, а также печать и сообщения об ошибках про сбое
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO. Измените код ошибки соответственно своим потребностям
        _tprintf(_T("Критическая ошибка: сбой при инициализации MFC\n"));
        nRetCode = 1;
    }
    else
    {
        uFS_FILE file;
        //INODE_RECORD Inode;
        file.Inode.dwSize = 0;
        file.Inode.wInodeID = 0;
        file.Inode.wFlags = 0;
        file.Flags.FILESTATEbits.RE = 1;
        file.Flags.FILESTATEbits.WE = 1;
        file.DataPointer = 0;

        
        memset(FS,0xFF,sizeof(FS));
        memset(Data1,0x00,sizeof(Data1));
        memset((void*)FreeSectors, 0xFF, sizeof(FreeSectors));
        for(int i = 0; i < 12; i++){
             file.Inode.pwTable[i] = INVALID_SECTOR_NUMBER;
        }
        for(DWORD i = 0; i < sizeof(Data)/sizeof(Data[0]); i++){
            Data[i] = (WORD)i;
        }

        Count = SECTORS_COUNT - uFS_GetFreeSectorCount();
        file.Inode.wInodeID++;
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );
        file.Inode.wInodeID++;
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );
        file.Inode.wInodeID++;
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );
        file.Inode.wInodeID++;
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );
        file.Inode.wInodeID++;
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );
        file.Inode.wInodeID++;
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );
        file.Inode.wInodeID++;
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );
        file.Inode.wInodeID++;
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );

        //uFSReadFile(Inode, 0, (BYTE*)Data1, sizeof(Data1));
        Count = SECTORS_COUNT - uFS_GetFreeSectorCount();
       
//         while(Count < SECTORS_COUNT){
//             if(uFSWriteFile(&Inode, 0, (BYTE*)Data, sizeof(Data)) < 0)
//                 break;
//             if(uFSReadFile(Inode, 0, (BYTE*)Data1, sizeof(Data1)) < 0)
//                 break;
//             if(memcmp(Data, Data1, sizeof(Data))){
//                 break;
//             };
//             Count = SECTORS_COUNT - uFS_GetFreeSectorCount();
//         }
//         Count = SECTORS_COUNT - uFS_GetFreeSectorCount();
    }
return nRetCode;
}
