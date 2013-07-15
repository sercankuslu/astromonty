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
#define uFS_MAX_OPENED_FILE 10
#define uFS_INVALID_HANDLE 0xFFFF
#define uFS_ERROR_FREE_RECORD 0xFF
#define uFS_ERROR_NO_ERROR 0x00


typedef WORD IdAddrSectors[3]; 

BYTE FS[256*SECTORS_COUNT];


//FS_FREE 
typedef struct _FREE_SECTORS{
    BYTE AvailableSectors;
    BYTE ReleasedSectors;
} FREE_SECTORS;

FREE_SECTORS    FreeSectors[SECTORS_COUNT/8];


uFS_FILE OpenFileArray[uFS_MAX_OPENED_FILE + 3];

int uFS_Read(DWORD Addr, BYTE * Dest, DWORD Count);
int uFS_Write(DWORD Addr, BYTE * Source, DWORD Count);
WORD GetIndirectSectorNumber(WORD SectorNumber, WORD WordPosition);
int SetIndirectSectorNumber(WORD SectorNumber, WORD WordPosition, WORD Number);
int uFS_GetSectorAddr( INODE_RECORD Inode, DWORD Addr, DWORD * SectorAddr, IdAddrSectors Indirect  );
int uFS_GetAddr(INODE_RECORD Inode, DWORD Addr, DWORD * DataAddr );
int uFS_AddSector(INODE_RECORD * Inode, DWORD * SectorAddr);
int uFS_AddSectorFromRange(INODE_RECORD * Inode, DWORD * SectorAddr, WORD BeginSector, WORD EndSector);
int uFS_ReadData(DWORD Addr, BYTE * Dest, int Count);
int uFS_WriteData(DWORD Addr, BYTE * Source, int Count);
int uFSReadFile(INODE_RECORD Inode, DWORD Address, BYTE * Dest, int Count);
int uFSWriteFile(INODE_RECORD * Inode, DWORD Address, BYTE * Source, int Count);
int uFS_ClaimFreeSectorFromRange(WORD * ClaimedSector, WORD BeginSector, WORD EndSector);
int uFS_ClaimFreeSector(WORD * ClaimedSector);
int uFS_ReleaseSector(WORD ReleasedSector);
BYTE bBitCount(BYTE x);
BYTE wBitCount(WORD x);
DWORD uFS_GetFreeSectorCount();
DWORD uFS_GetFreeSectorCountInRange(WORD BeginSector, WORD EndSector);
DWORD uFS_GetReleasedSectorCount();
DWORD uFS_GetReleasedSectorCountInRange(WORD BeginSector, WORD EndSector);


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



int uFS_GetSectorAddr( INODE_RECORD Inode, WORD Addr, WORD * SectorAddr, IdAddrSectors Indirect  )
{
    if(Addr << DATA_SECTOR_SIZE_BIT >= Inode.dwSize) 
        return -1; // EOF

    if(Indirect != NULL){
        Indirect[0] = INVALID_SECTOR_NUMBER;
        Indirect[1] = INVALID_SECTOR_NUMBER;
        Indirect[2] = INVALID_SECTOR_NUMBER;
    }
    WORD SectorNumber = Addr;
    if(SectorNumber < 9){
        // Прямая адресация
        if(Inode.pwTable[SectorNumber] == INVALID_SECTOR_NUMBER)
            return -1;
        *SectorAddr = (Inode.pwTable[SectorNumber]);
        if(*SectorAddr == INVALID_SECTOR_NUMBER)
            return -1;
        return 0;
    } else SectorNumber -= 9;

    if(SectorNumber < INDERECT_ADDR_x1_SIZE){

        // Косвенная адресация
        // Inode.pwTable[9]-->table1[128]-->data[256]

        if(Inode.pwTable[9] == INVALID_SECTOR_NUMBER)
            return -1;
        *SectorAddr = (GetIndirectSectorNumber(Inode.pwTable[9], SectorNumber));
        if(*SectorAddr == INVALID_SECTOR_NUMBER)
            return -1;
        if(Indirect != NULL){
            Indirect[0] = Inode.pwTable[9];
        }
        return 0;

    } else SectorNumber -= INDERECT_ADDR_x1_SIZE;

    if(SectorNumber < INDERECT_ADDR_x2_SIZE){ 

        // Двойная косвенная адресация
        // Inode.pwTable[10]-->table1[128]-->table2[128]-->data[256]

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
        if(Indirect != NULL){
            Indirect[0] = Inode.pwTable[10];
            Indirect[1] = Addr2;
        }
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
        if(Indirect != NULL){
            Indirect[0] = Inode.pwTable[11];
            Indirect[1] = Addr2;
            Indirect[2] = Addr3;
        }
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
    int Err = uFS_GetSectorAddr(Inode, Addr, &SectorAddr, NULL);
    if(Err == 0){
        *DataAddr = (SectorAddr  << DATA_SECTOR_SIZE_BIT) | (Addr & SECTOR_MASK);
    }
    return Err;
}

int uFS_ExpandFile(INODE_RECORD * Inode, DWORD NewSize)
{
    DWORD PhysicalAddr = 0;
    int Err = 0;
    int WritedCnt = 0;
    int Count = NewSize - Inode->dwSize;   
    if (Count < 0)
        return -1;
    // TODO : сделать увеличение размеров файла без записи в каждый сектор
    return WritedCnt;
}

// ***************************************************************************
// добавляет сектор к файлу
int uFS_AddSector(INODE_RECORD * Inode, DWORD * SectorAddr)
{
    return uFS_AddSectorFromRange(Inode, SectorAddr, 0, SECTORS_COUNT - 1 );
}

// ***************************************************************************
// добавляет сектор к файлу из дапазона
int uFS_AddSectorFromRange(INODE_RECORD * Inode, DWORD * SectorAddr, WORD BeginSector, WORD EndSector)             // Inode файла    
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
            if(uFS_ClaimFreeSectorFromRange(&Inode->pwTable[SectorNumber], BeginSector, EndSector) != 0)
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
            if(uFS_ClaimFreeSectorFromRange(&Inode->pwTable[9], BeginSector, EndSector) != 0)
                return -1; // нет свободных секторов
        }
        DWORD Table1 = (GetIndirectSectorNumber(Inode->pwTable[9], SectorNumber));
        if(Table1 == INVALID_SECTOR_NUMBER){
            WORD Number = INVALID_SECTOR_NUMBER;
            if(uFS_ClaimFreeSectorFromRange(&Number, BeginSector, EndSector) != 0)
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
            if(uFS_ClaimFreeSectorFromRange(&Inode->pwTable[10], BeginSector, EndSector) != 0)
                return -1; // нет свободных секторов
        }

        WORD Table1 = SectorNumber >> TABLE_SECTOR_SIZE_BIT;
        WORD Table2 = SectorNumber & TABLE_SECTOR_MASK;

        WORD Addr1 = (GetIndirectSectorNumber(Inode->pwTable[10], Table1));

        if(Addr1 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSectorFromRange(&Addr1, BeginSector, EndSector) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Inode->pwTable[10], Table1, Addr1);
        }

        WORD Addr2 = (GetIndirectSectorNumber(Addr1, Table2));
        if(Addr2 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSectorFromRange(&Addr2, BeginSector, EndSector) != 0)
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
            if(uFS_ClaimFreeSectorFromRange(&Inode->pwTable[11], BeginSector, EndSector) != 0)
                return -1; // нет свободных секторов
        }

        WORD Table1 = SectorNumber >> (TABLE_SECTOR_SIZE_BIT * 2);
        WORD Table2 = (SectorNumber >> TABLE_SECTOR_SIZE_BIT) & TABLE_SECTOR_MASK;
        WORD Table3 = SectorNumber & TABLE_SECTOR_MASK;

        WORD Addr1 = (GetIndirectSectorNumber(Inode->pwTable[11], Table1));

        if(Addr1 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSectorFromRange(&Addr1, BeginSector, EndSector) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Inode->pwTable[11], Table1, Addr1);
        }

        WORD Addr2 = (GetIndirectSectorNumber(Addr1, Table2));
        if(Addr2 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSectorFromRange(&Addr2, BeginSector, EndSector) != 0)
                return -1; // нет свободных секторов
            SetIndirectSectorNumber(Addr1, Table2, Addr2);
        }

        WORD Addr3 = (GetIndirectSectorNumber(Addr2, Table3));
        if(Addr3 == INVALID_SECTOR_NUMBER){
            if(uFS_ClaimFreeSectorFromRange(&Addr3, BeginSector, EndSector) != 0)
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
    INODE_RECORD * Inode,           // Inode файла
    DWORD Address,                  // Адрес внутри файла
    BYTE * Source,                  // Адрес буфера
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



// резервирует свободный сектор из заданного диапазона
int uFS_ClaimFreeSectorFromRange(WORD * ClaimedSector, WORD BeginSector, WORD EndSector)
{
    WORD Beg = BeginSector >> 3;
    WORD End = EndSector >> 3;
    if(End > sizeof(FreeSectors)/sizeof(FREE_SECTORS)){
        End = sizeof(FreeSectors)/sizeof(FREE_SECTORS);
    }
    for(int i = Beg >> 3; i < End >> 3; i++){
        if(FreeSectors[i].AvailableSectors > 0){         
            int j = 0;
            while(!((FreeSectors[i].AvailableSectors >> j) & 0x1)){
                j++;
            }
            FreeSectors[i].AvailableSectors &= (~(1 << j));
            *ClaimedSector = (i << 3) + j;
            return 0;
        }
    }
    return -1;
}

// резервирует свободный сектор
int uFS_ClaimFreeSector(WORD * ClaimedSector)
{
    return uFS_ClaimFreeSectorFromRange(ClaimedSector, 0, sizeof(FreeSectors)/sizeof(FREE_SECTORS));
}

// освобождает сектор
int uFS_ReleaseSector(WORD ReleasedSector)
{   
    int i = ReleasedSector >> 3;  
    int j = ReleasedSector & 0x07;
    if(FreeSectors[i].ReleasedSectors > 0){  
        FreeSectors[i].ReleasedSectors &= (~(1 << j));
        return 0;
    }
    return -1;
}


// ***************************************************************************

int uFS_ReleaseFileSector(INODE_RECORD Inode, WORD SectorIndex)
{   
    IdAddrSectors Indirect;
    WORD SectorAddr = 0;

    uFS_GetSectorAddr( Inode, SectorIndex, &SectorAddr, Indirect );
    if(uFS_ReleaseSector(SectorAddr) != 0)
        return -1;  // ошибка освобождения сектора   
    
    if(SectorAddr & SECTOR_MASK == 0){
        // первый сектор. если удаляем с конца файла, то последий не удаленный. нужно удалить сектора таблиц        
        if(Indirect[2] == INVALID_SECTOR_NUMBER){
            // прямая, косвенная или двойная косвенная адресация
            if(Indirect[1] == INVALID_SECTOR_NUMBER){
                // прямая или косвенная адресация
                if(Indirect[0] == INVALID_SECTOR_NUMBER){
                    // прямая адресация
                } else {
                    // косвенная адресация
                    uFS_ReleaseSector(Indirect[0]);
                }
            } else {
                // двойная косвенная адресация
                uFS_ReleaseSector(Indirect[1]);
                if(Indirect[0] & TABLE_SECTOR_MASK ==0 ){
                    uFS_ReleaseSector(Indirect[0]);
                }
            }
        } else {
            // тройная косвенная адресация
            uFS_ReleaseSector(Indirect[2]);
            if(Indirect[1] & TABLE_SECTOR_MASK ==0 ){
                uFS_ReleaseSector(Indirect[1]);
                if(Indirect[0] & TABLE_SECTOR_MASK ==0 ){
                    uFS_ReleaseSector(Indirect[0]);
                }
            }
        }
    }

    return 0;


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

DWORD uFS_GetFreeSectorCount()
{
    return uFS_GetFreeSectorCountInRange(0, SECTORS_COUNT - 1);
}

DWORD uFS_GetFreeSectorCountInRange(WORD BeginSector, WORD EndSector)
{
    DWORD count = 0;
    WORD Beg = BeginSector >> 3;
    WORD End = EndSector >> 3;
    if(End > sizeof(FreeSectors)/sizeof(FREE_SECTORS)){
        End = sizeof(FreeSectors)/sizeof(FREE_SECTORS);
    }    
    for(int i = Beg; i < End; i++){
        count += bBitCount(FreeSectors[i].AvailableSectors);
    }
    return count;
}

DWORD uFS_GetReleasedSectorCount()
{
    return uFS_GetReleasedSectorCountInRange(0, SECTORS_COUNT - 1);
}

DWORD uFS_GetReleasedSectorCountInRange(WORD BeginSector, WORD EndSector)
{
    DWORD count = 0;
    WORD Beg = BeginSector >> 3;
    WORD End = EndSector >> 3;
    if(End > sizeof(FreeSectors)/sizeof(FREE_SECTORS)){
        End = sizeof(FreeSectors)/sizeof(FREE_SECTORS);
    }    
    for(int i = Beg; i < End; i++){
        count += bBitCount(FreeSectors[i].ReleasedSectors);
    }
    return count;
}

int uFS_Init()
{
    for ( int i = 0; i < uFS_MAX_OPENED_FILE + 3; i++){
        
        OpenFileArray[i].ErrorState = uFS_ERROR_FREE_RECORD;
        OpenFileArray[i].Flags.Val = 0;
        OpenFileArray[i].DataPointer = 0;

        OpenFileArray[i].Inode.dwSize = 0;
        OpenFileArray[i].Inode.wFlags = 0;
        OpenFileArray[i].Inode.wInodeID = 0;
        memset(OpenFileArray[i].Inode.pwTable, 0xFF, 12);
        
        OpenFileArray[i].Dir.bNameLength = 0;
        memset(OpenFileArray[i].Dir.pcFileName, 0, MAX_FILE_NAME_LENGTH);
        OpenFileArray[i].Dir.wInodeID = 0;
    }
    uFS_FILE fs_root;
    fs_root.Inode.dwSize = 0;
    fs_root.Inode.wInodeID = 1;
    fs_root.Inode.wFlags = 0;
    fs_root.Flags.bits.RE = 1;
    fs_root.Flags.bits.WE = 0;
    fs_root.DataPointer = 0;
    memset(fs_root.Inode.pwTable, INVALID_SECTOR_NUMBER, sizeof(fs_root.Inode.pwTable));
    if(uFS_ReadData(0x000020, (BYTE*)&fs_root.Inode, sizeof(INODE_RECORD)) == 0){
        return -1;
    }
    uFS_fseek(&fs_root, 0x40, SEEK_SET);

    for(int i = 0; i < 3; i++)  {
        OpenFileArray[i].ErrorState = uFS_ERROR_NO_ERROR;
        OpenFileArray[i].Flags.bits.Binary = 1;
        OpenFileArray[i].Flags.bits.RE = 1;
        OpenFileArray[i].Flags.bits.WE = 1;
        OpenFileArray[i].DataPointer = 0;
        if(uFS_fread((BYTE*)&OpenFileArray[i].Inode, sizeof(INODE_RECORD), 1, &fs_root) < 0){
            return -1;
        }
        OpenFileArray[i].Dir.wInodeID = OpenFileArray[i].Inode.wInodeID;
    }
    {
        strcpy (OpenFileArray[0].Dir.pcFileName,".inodes");
        OpenFileArray[0].Dir.bNameLength = strlen(OpenFileArray[0].Dir.pcFileName);        

        strcpy (OpenFileArray[1].Dir.pcFileName,".free");
        OpenFileArray[1].Dir.bNameLength = strlen(OpenFileArray[1].Dir.pcFileName);        

        strcpy (OpenFileArray[2].Dir.pcFileName,"/");
        OpenFileArray[2].Dir.bNameLength = strlen(OpenFileArray[2].Dir.pcFileName);        
    }
    //uFS_ReleaseFileSector(&OpenFileArray[0].Inode, 0);
    //DWORD SectorAddr;
    //IdAddrSectors Indirect;

    //uFS_GetSectorAddr( OpenFileArray[0].Inode, 32762, &SectorAddr, Indirect );
    //uFS_GetSectorAddr( OpenFileArray[0].Inode, 29000, &SectorAddr, NULL );

    return 0;
}

/*
"r"     read: Open file for input operations. The file must exist.
"w"     write: Create an empty file for output operations. If a file with the same name already exists, 
        its contents are discarded and the file is treated as a new empty file.
"a"     append: Open file for output at the end of a file. Output operations always 
        write data at the end of the file, expanding it. Repositioning operations (fseek, fsetpos, rewind) 
        are ignored. The file is created if it does not exist.
"r+"    read/update: Open a file for update (both for input and output). The file must exist.
"w+"    write/update: Create an empty file and open it for update (both for input and output). 
        If a file with the same name already exists its contents are discarded and the file is treated as a new empty file.
"a+"    append/update: Open a file for update (both for input and output) with all output 
        operations writing data at the end of the file. Repositioning operations 
        (fseek, fsetpos, rewind) affects the next input operations, but output operations 
        (move the position back to the end of file. The file is created if it does not exist.
text:
r, r+, rt, rt+, r+t
w, w+, wt, wt+, w+t
a, a+, at, at+, a+t

binary:
rb, rb+, r+b
wb, wb+, w+b
ab, ab+, a+b

/folder/filename.bin
./filename.bin

Если файл на запись, выделяем буфер
*/

uFS_FILE *  FS_fopen ( const char * filename, const char * mode )   //Open file
{
    int i;
    //FILESTATE State;
    uFS_FILE * Stream = NULL;
    
    if ((filename == NULL)||(mode == NULL)) {
        return NULL;
    }

    for (i = 0; i < uFS_MAX_OPENED_FILE + 3; i++){
        if(OpenFileArray[i].ErrorState == uFS_ERROR_FREE_RECORD){
            Stream = &OpenFileArray[i];
        }
    }

    if(Stream == NULL)
        return NULL;
    
    Stream->Flags.Val = 0;

    for (i = 0; i < 3; i++)
    {
        switch (mode[i])
        {
        // взаимоисключающие
        case 'r':
            Stream->Flags.bits.RE = 1;
            Stream->Flags.bits.WE = 0;
            Stream->Flags.bits.Renew = 0;
            Stream->Flags.bits.WrToEnd = 0;
            Stream->Flags.bits.New = 0;
            break;
        case 'w':
            Stream->Flags.bits.WE = 1;
            Stream->Flags.bits.RE = 0;
            Stream->Flags.bits.Renew = 1;
            Stream->Flags.bits.WrToEnd = 0;
            Stream->Flags.bits.New = 1;
            break;
        case 'a':
            Stream->Flags.bits.WE = 1;
            Stream->Flags.bits.RE = 0;
            Stream->Flags.bits.Renew = 0;
            Stream->Flags.bits.WrToEnd = 1;
            Stream->Flags.bits.New = 1;
            break;
        // модификаторы
        case '+':
            Stream->Flags.bits.WE = 1;
            Stream->Flags.bits.RE = 1;
            break;
        case 'b':
            Stream->Flags.bits.Binary = 1;
            break;
        case 't':
            Stream->Flags.bits.Binary = 0;
            break;
        // конец строки
        case '\0':
            i = 3;
            break;
        default:
            return NULL;
        }
    }
    return NULL;
}


// Wite block of data from stream
int uFS_fwrite ( BYTE * ptr, DWORD size, DWORD count, uFS_FILE * stream )
{
    if(stream->Flags.bits.WE == 0) 
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
    if(stream->Flags.bits.RE == 0) 
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
    //if(stream->Flags.bits.RE == 1) 
    //    return -1;

    switch (origin) {
    case SEEK_SET:
        if(offset <0)
            return -1;
        if(offset < (int)stream->Inode.dwSize){
            stream->DataPointer = offset;
        } else {
           
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
        } else {
            // TODO: увеличение файла с помощью fseek
        }
        break;
    default:
        return -1;
    }
    return 0;
}
//*********************************************************************************
int CreateFS()
{
    uFS_FILE fs_root;
    uFS_FILE fs_inode;
    uFS_FILE fs_free;
    uFS_FILE fs_dir;
    BYTE Buf[256];

    fs_root.Inode.dwSize = 0;
    fs_root.Inode.wInodeID = 1;
    fs_root.Inode.wFlags = 0;
    fs_root.Flags.bits.RE = 1;
    fs_root.Flags.bits.WE = 1;
    fs_root.DataPointer = 0;
    memset(fs_root.Inode.pwTable, INVALID_SECTOR_NUMBER, sizeof(fs_root.Inode.pwTable));

    fs_inode.Inode.dwSize = 0;
    fs_inode.Inode.wInodeID = 2;
    fs_inode.Inode.wFlags = 0;
    fs_inode.Flags.bits.RE = 1;
    fs_inode.Flags.bits.WE = 1;
    fs_inode.DataPointer = 0;
    memset(fs_inode.Inode.pwTable, INVALID_SECTOR_NUMBER, sizeof(fs_inode.Inode.pwTable));

    fs_free.Inode.dwSize = 0;
    fs_free.Inode.wInodeID = 3;
    fs_free.Inode.wFlags = 0;
    fs_free.Flags.bits.RE = 1;
    fs_free.Flags.bits.WE = 1;
    fs_free.DataPointer = 0;
    memset(fs_free.Inode.pwTable, INVALID_SECTOR_NUMBER, sizeof(fs_free.Inode.pwTable));

    fs_dir.Inode.dwSize = 0;
    fs_dir.Inode.wInodeID = 4;
    fs_dir.Inode.wFlags = 0;
    fs_dir.Flags.bits.RE = 1;
    fs_dir.Flags.bits.WE = 1;
    fs_dir.DataPointer = 0;
    memset(fs_dir.Inode.pwTable, INVALID_SECTOR_NUMBER, sizeof(fs_dir.Inode.pwTable));

    memset(Buf, 0xFF, sizeof(Buf));
    
    // создаём файл первого сектора
    uFS_fwrite (  Buf, 1, 0x40, &fs_root );
    
    // создаём файл Инодов
    for(int i = 0; i < 128; i++){
        uFS_fwrite (  Buf, 256, 1, &fs_inode );
    }
        
    // создаём файл корневой директории
    uFS_fwrite (  Buf, 1, 0, &fs_dir );

    // создаём и записываем файл свободного места
    uFS_fwrite (  (BYTE*)FreeSectors, 1, 256, &fs_free );    

    // пишем Inode таблицы инодов    
    uFS_fwrite (  (BYTE*)&fs_inode.Inode, 1, sizeof(fs_inode.Inode), &fs_root );
    
    // пишем Inode файла свободного места    
    uFS_fwrite (  (BYTE*)&fs_free.Inode, 1, sizeof(fs_free.Inode), &fs_root );

    // пишем Inode файла корневой директории
    uFS_fwrite (  (BYTE*)&fs_dir.Inode, 1, sizeof(fs_dir.Inode), &fs_root );
    
    uFS_fseek( &fs_root, 0x20, SEEK_SET);
     // пишем Inode файла корневой директории
    uFS_fwrite (  (BYTE*)&fs_root.Inode, 1, sizeof(fs_root.Inode), &fs_root );

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

        int k =0;
        int t = ~(k - 1) & k;

        //INODE_RECORD Inode;
        file.Inode.dwSize = 0;
        file.Inode.wInodeID = 0;
        file.Inode.wFlags = 0;
        file.Flags.bits.RE = 1;
        file.Flags.bits.WE = 1;
        file.DataPointer = 0;

        
        memset(FS,0xFF,sizeof(FS));
        memset(Data1,0x00,sizeof(Data1));
        memset((void*)FreeSectors, 0xFF, sizeof(FreeSectors));
        CreateFS();

        if(uFS_Init())
            return 1;



        for(int i = 0; i < 12; i++){
             file.Inode.pwTable[i] = INVALID_SECTOR_NUMBER;
        }
        for(DWORD i = 0; i < sizeof(Data)/sizeof(Data[0]); i++){
            Data[i] = (WORD)i;
        }

        /*Count = SECTORS_COUNT - uFS_GetFreeSectorCount();
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
        uFS_fwrite (  (BYTE*)&file.Inode, 1, sizeof(file.Inode), &file );*/

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
