// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


typedef unsigned char BYTE;
typedef bool BOOL;
typedef unsigned short int WORD;
typedef unsigned long DWORD;

typedef union
{
	DWORD Val;	
} DWORD_VAL;

typedef DWORD_VAL IP_ADDR;

typedef struct {
	BYTE v[6];
} MAC_ADDR;
typedef struct {
	IP_ADDR		MyIPAddr;               // IP address
	IP_ADDR		MyMask;                 // Subnet mask
	IP_ADDR		MyGateway;              // Default Gateway
	IP_ADDR		PrimaryDNSServer;       // Primary DNS Server
	IP_ADDR		SecondaryDNSServer;     // Secondary DNS Server
	IP_ADDR		DefaultIPAddr;          // Default IP address
	IP_ADDR		DefaultMask;            // Default subnet mask
	BYTE		NetBIOSName[16];        // NetBIOS name
	struct
	{
		unsigned char : 6;
		unsigned char bIsDHCPEnabled : 1;
		unsigned char bInConfigMode : 1;
	} Flags;                            // Flag structure
	MAC_ADDR	MyMACAddr;              // Application MAC address
} APP_CONFIG;

DWORD SNTPGetUTCSeconds();


// TODO. ”становите здесь ссылки на дополнительные заголовки, требующиес€ дл€ программы
