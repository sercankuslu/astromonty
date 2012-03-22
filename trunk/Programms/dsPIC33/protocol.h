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
    STR_TOO_MANY_ATTRIBUTES,
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
    STC_CONNECT_ANSWER,
    STC_REQEST_AUTH,
    STC_AUTH_ANSWER,
    STC_REQEST_DATA,
    STC_SEND_DATA,
    STC_EXECUTE_COMMAND,
    STC_EXECUTE_ANSWER
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
    STF_DATA_READY,
    STF_NO_DATA
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
    STA_DELTA,
    STA_GAMMA,
    STA_ABS_TARGET,
    STA_ASTRO_TARGET,
    STA_SPEED,
    STA_START,            
    STA_STOP,    
    STA_EMERGENCY_STOP,
    STA_MESSAGE
}ST_ATTRIBUTE_TYPE;

typedef enum STN_MODE {
    STM_MODE_AUTO,
    STM_MODE_DHCP,
    STM_MODE_STATIC
} STN_MODE;

#define MEM_BUFFER_LEN 32
#define MAX_ATTRIBUTE 8
#define FALSE   0
#define TRUE    1

#define MAX_BUFFER_LEN 64
ST_RESULT SetClientDisconnect();
ST_RESULT ProtocolInit();
ST_RESULT RunServer(BYTE bConnectionID, BYTE* pbBlob, int pbBlobSize, int* pbBlobLen);
ST_RESULT RunClient(BYTE* pbBlob, int bBlobLen, int *pbDataLength);

ST_RESULT BeginCommand(BYTE * pBuffer, int bBufLen, ST_COMMANDS Value);
ST_RESULT AddAttribute(BYTE * pBuffer, int bBufLen, ST_ATTRIBUTE_TYPE Attribute, BYTE bValueLen, BYTE * Value);
ST_RESULT AddFlag(BYTE * pBuffer, int bBufLen, ST_FLAGS Value);
ST_RESULT FindAttribute(BYTE * pBuffer, int bBufLen, ST_ATTRIBUTE_TYPE Attribute, BYTE * bValueLen, BYTE ** Value);
ST_RESULT GetNextAttribute(BYTE * pBuffer, int bBufLen, BYTE ** pPointer, ST_ATTRIBUTE_TYPE * Attribute, BYTE * bValueLen, BYTE ** Value);
ST_RESULT CheckBlob(BYTE * pBuffer, int bBufLen);
ST_RESULT GetCommandLength(BYTE * pbBlob, int bBlobLen, int * pbDataLength);
#endif //__PROTOCOL_H_
