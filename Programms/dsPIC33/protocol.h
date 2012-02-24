#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_
#ifdef _WINDOWS
#   include "..\guidance\stdafx.h"
#else
#   include "TCPIP Stack/TCPIP.h"
#endif
// 

typedef enum ST_RESULT {
    STR_OK,
    STR_NEED_ANSWER,
    STR_NEED_DISCONNECT,
    STR_FUNCTION_FAILED,
    STR_BUFFER_TOO_SMALL,
    STR_COMMAND_UNKNOWN,
    STR_DATA_CORRUPTED,
    STR_ATTR_NOT_FOUND
} ST_RESULT;

typedef enum ST_STATUS {
    STS_NO_CONNECT,
    STS_CONNECT_REQ,
    STS_AUTH_REQ,
    STS_CONNECTED
} ST_STATUS;

typedef enum ST_COMMANDS {
    STC_NO_COMMANDS,
    STC_REQEST_CONNECT,
    STC_REQEST_AUTH,
    STC_REQEST_DATA,
    STC_SEND_DATA,
    STC_EXECUTE_COMMAND
}ST_COMMANDS;

typedef enum ST_FLAGS {
    STF_OK,
    STF_ACCEPTED,
    STF_DECLINED,
    STF_AUTH_NEEDED,
    STF_AUTH_NOT_NEEDED,
    STF_TOO_MANY_DATA,
    STF_INCORRECT_COMMAND,
    STF_COMMAND_INCOMPLETE,
    STF_COMMAND_UNKNOWN,
    STF_DATA_TYPE_UNKNOWN,
    STF_DATA_ERROR,
    STF_DATA_READY
}ST_FLAGS;

typedef enum ST_ATTRIBUTE_TYPE {
    STA_NULL,
    STA_COMMAND, 
    STA_FLAG,
    STA_LOGIN,
    STA_PASSWORD,
    STA_NETWORK_ADDRESS,
    STA_NETWORK_MASK,
    STA_NETWORK_GATE,
    STA_NETWORK_DNS1,
    STA_NETWORK_DNS2,
    STA_NETWORK_MODE,
    STA_NETWORK_NAME,
    STA_NETWORK_MAC,
    STA_TIME_SNTP,
    STA_TIME_RTC,
    STA_TIME_SELECT,
    STA_ALPHA,
    STA_ALPHA_TARGET,
    STA_DELTA,
    STA_DELTA_TARGET,
    STA_GAMMA,
    STA_GAMMA_TARGET,
}ST_ATTRIBUTE_TYPE;

typedef enum STN_MODE {
    STM_MODE_AUTO,
    STM_MODE_DHCP,
    STM_MODE_STATIC
} STN_MODE;

#define MEM_BUFFER_LEN 32
#define MAX_ATTRIBUTE 8

typedef struct ST_ATTRIBUTE {
    ST_ATTRIBUTE_TYPE   type;
    BYTE                ulValueLen;  /* in bytes */
    void*               pValue;
} ST_ATTRIBUTE;
typedef ST_ATTRIBUTE*  ST_ATTRIBUTE_PTR;
#define IN_BUFFER 0
#define OUT_BUFFER 1
#define FALSE   0
#define TRUE    1

#ifdef _WINDOWS
typedef struct {
    char NetBIOSName[16];
    DWORD Time;
} AppConfigType;
#endif



//static BYTE SendAttributes();
#define MAX_BUFFER_LEN 64
ST_RESULT ProtocolInit();
ST_RESULT FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE bAttributeLen, BYTE* pbBlock, BYTE bBlockLen ,BYTE* pbBlockPos);
ST_RESULT ParseBlob(BYTE* pbBlock, BYTE bBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *pbAttributeLen, BYTE* pbMem, BYTE bMemLen, BYTE* bMemPos);
ST_RESULT FindParam(ST_ATTRIBUTE* pData, BYTE bDataLen, ST_ATTRIBUTE_TYPE bType, BYTE * Index);
ST_RESULT RunServer(BYTE bConnectionID, BYTE* pbBlob, BYTE* pbBlobLen);
ST_RESULT RunClient(BYTE* pbBlob, BYTE bBlobLen, BYTE *pbDataLength);
ST_RESULT CopyAttribute(ST_ATTRIBUTE pDest, ST_ATTRIBUTE pSource, BYTE *pbMem, BYTE bMemLen, BYTE* pMemPos );
BOOL IsClientConnected();
#endif //__PROTOCOL_H_
