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

/*typedef union {
    BYTE Val;
    struct {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    } FREE_bits;
} FS_FREE;
*/
//FS_FREE 
BYTE    FreeSectors[8192];

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

#define SECTOR_SIZE 256
#define SECTOR_SIZE_WORD 128
#define SECTOR_SIZE_MASK 0x00FF
#define SECTOR_SIZE_BIT 8
#define INDERECT_ADDR_x1_SIZE SECTOR_SIZE_WORD
#define INDERECT_ADDR_x2_SIZE SECTOR_SIZE_WORD*SECTOR_SIZE_WORD
#define INDERECT_ADDR_x3_SIZE SECTOR_SIZE_WORD*SECTOR_SIZE_WORD*SECTOR_SIZE_WORD
// ***************************************************************************
int uFS_Read(DWORD Addr, BYTE * Dest, DWORD Count)
{
    memcpy(Dest, &FS[Addr],Count);
    return 0;
}

// ***************************************************************************
WORD GetIndirectSectorNumber(WORD SectorNumber, WORD WordPosition)
{   
    DWORD AddrInSector = (SectorNumber << SECTOR_SIZE_BIT) | ((WordPosition << 1) & SECTOR_SIZE_MASK);
    WORD DataAddr;
    uFS_Read(AddrInSector, (BYTE*)&DataAddr, sizeof(WORD));
    return DataAddr;
}

// ***************************************************************************
int uFS_GetSectorAddr(
    INODE_RECORD Inode,             // Inode файла
    DWORD Addr, DWORD * SectorAddr )
{
    if(Addr >= Inode.dwSize) 
        return -1; // EOF

    WORD SectorNumber = (Addr & (~SECTOR_SIZE_MASK)) >> SECTOR_SIZE_BIT;
    if(SectorNumber < 9){
        // Прямая адресация
        *SectorAddr = (Inode.pwTable[SectorNumber] << SECTOR_SIZE_BIT);
        return 0;
    } else SectorNumber -= 9;

    if(SectorNumber < INDERECT_ADDR_x1_SIZE){

        // Косвенная адресация
        // Inode.pwTable[9]-->table1[]-->data
        //               table1[128]  data[256]
        // SectorNumber: 0000000      00000000

        *SectorAddr = (GetIndirectSectorNumber(Inode.pwTable[9], SectorNumber));
        return 0;

    } else SectorNumber -= INDERECT_ADDR_x1_SIZE;

    if(SectorNumber < INDERECT_ADDR_x2_SIZE){ 

        // Двойная косвенная адресация
        // Inode.pwTable[10]-->table1[]-->table2[]-->data
        //               table1[128]  table2[128]  data[256]
        // SectorNumber: 0000000      0000000      00000000

        WORD Table1 = SectorNumber >> (SECTOR_SIZE_BIT - 1);
        WORD Table2 = SectorNumber & (SECTOR_SIZE_MASK >> 1);
        WORD Addr2 = GetIndirectSectorNumber(Inode.pwTable[10], Table1);
        *SectorAddr = GetIndirectSectorNumber(Addr2, Table2);
        return 0;
    } else SectorNumber -= INDERECT_ADDR_x2_SIZE;

    if(SectorNumber < INDERECT_ADDR_x3_SIZE){

        // Тройная косвенная адресация
        // Inode.pwTable[10]-->table1[]-->table2[]-->table3[]-->data
        //               table1[128]  table2[128]  table3[128]  data[256]
        // SectorNumber: 0000000      0000000      0000000      00000000

        WORD Table1 = SectorNumber >> ((SECTOR_SIZE_BIT - 1) * 2);
        WORD Table2 = (SectorNumber >> (SECTOR_SIZE_BIT - 1)) & (SECTOR_SIZE_MASK >> 1);
        WORD Table3 = SectorNumber & (SECTOR_SIZE_MASK >> 1);
        WORD Addr2 = GetIndirectSectorNumber(Inode.pwTable[11], Table1);
        WORD Addr3 = GetIndirectSectorNumber(Addr2, Table2);
        *SectorAddr = GetIndirectSectorNumber(Addr3, Table3);
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
    *DataAddr = (SectorAddr  << SECTOR_SIZE_BIT) | (Addr & SECTOR_SIZE_MASK);
    return Err;
}


// ***************************************************************************
DWORD uFS_AddSector(INODE_RECORD Inode)             // Inode файла    
{
    // Определить текущее количество секторов в файле
    WORD SectorNumber = (Inode.dwSize & (~SECTOR_SIZE_MASK)) >> SECTOR_SIZE_BIT;
    // Определить старший тип адресации
    return 0;
}

int uFS_ReadData(DWORD Addr, BYTE * Dest, int Count)
{
    return 0;
}

int uFSReadFile(
    INODE_RECORD Inode,             // Inode файла
    DWORD Address,                  // Адрес внутри файла
    BYTE * Dest,                    // Адрес буфера
    int Count                       // Количество в байтах
    )                               // Возвращает количество реально считанных байт или значение < 0 при ошибке
{
    return 0;
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




int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;

    // инициализировать MFC, а также печать и сообщения об ошибках про сбое
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO. Измените код ошибки соответственно своим потребностям
        _tprintf(_T("Критическая ошибка: сбой при инициализации MFC\n"));
        nRetCode = 1;
    }
    else
    {
        INODE_RECORD Inode;
        Inode.dwSize = 4229376+256;
        Inode.wInodeID = 10;
        Inode.wFlags = 0;
        
        memset(FS,0xFF,sizeof(FS));
        memset((void*)FreeSectors, 0xFF, sizeof(FreeSectors));
        
        DWORD Count = uFS_GetFreeSectorCount();
        for(int i = 0; i < 12; i++){
             if(uFS_ClaimFreeSector(&Inode.pwTable[i]) != 0) 
                 break;
        }

        // косвенная адресация
        WORD * wFS = (WORD*)&FS[Inode.pwTable[9]*256];
        for(int i = 0; i < 128; i++){
            if(uFS_ClaimFreeSector(&wFS[i]) != 0) break;
        }

        // двойная косвенная адресация
        wFS = (WORD*)&FS[Inode.pwTable[10]*256];
        for(int i = 0; i < 128; i++){
            if(uFS_ClaimFreeSector(&wFS[i]) != 0) break;
            WORD * wFS2 = (WORD*)&FS[wFS[i]*256];
            for(int j = 0; j < 128; j++){
                if(uFS_ClaimFreeSector(&wFS2[j]) != 0) break;
                memset(&FS[wFS2[j]*256],0x00,256);
            }
        }
        // 4229376
        // тройная косвенная адресация
         wFS = (WORD*)&FS[Inode.pwTable[11]*256];
         for(int i = 0; i < 2; i++){
             if(uFS_ClaimFreeSector(&wFS[i]) != 0) break;
             WORD * wFS2 = (WORD*)&FS[wFS[i]*256];
             for(int j = 0; j < 128; j++){
                 if(uFS_ClaimFreeSector(&wFS2[j]) != 0) break;
                 WORD * wFS3 = (WORD*)&FS[wFS2[j]*256];
                 for(int k = 0; k < 128; k++){
                     if(uFS_ClaimFreeSector(&wFS3[k]) != 0) break;
                     memset(&FS[wFS3[k]*256],0x00,256);
                 }
             }
         }
        Count = uFS_GetFreeSectorCount();
        DWORD PhysicalAddr = 0;
        BYTE Data[256];
        uFS_GetAddr(Inode, 2400 , &PhysicalAddr);
        uFS_GetAddr(Inode,  4229376, &PhysicalAddr);
        uFS_Read(PhysicalAddr, Data, 256);
    }
return nRetCode;
}
