#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_
#include "TCPIP Stack/TCPIP.h"

// 
#define STR_OK                              0
#define STR_NEED_ANSWER                     1
#define STR_NEED_DISCONNECT                 2
#define STR_FUNCTION_FAILED                 6
#define STR_BUFFER_TOO_SMALL                7
#define STR_COMMAND_UNKNOWN                 8
#define STR_DATA_CORRUPTED					9
// 
#define STS_NO_CONNECT                      0
#define STS_CONNECT_REQ                     1
#define STS_AUTH_REQ                        2
#define STS_CONNECTED                       3

#define STA_COMMAND                         0x01    
#   define STC_REQEST_CONNECT               0x80    
#   define STC_REQEST_AUTH                  0x81    
#   define STC_REQEST_DATA                  0x40    
#   define STC_SEND_DATA                    0x41    

#define STA_FLAG                            0x02    
#   define STF_ACCEPTED                     0x81    
#   define STF_DECLINED                     0x82    
#   define STF_AUTH_NEEDED                  0x83    
#   define STF_AUTH_NOT_NEEDED              0x84    
#   define STF_TOO_MANY_DATA                0x40    
#   define STF_INCORRECT_COMMAND            0x41    
#   define STF_COMMAND_INCOMPLETE           0x42    
#   define STF_COMMAND_UNKNOWN              0x43
#   define STF_DATA_TYPE_UNKNOWN           	0x44
#   define STF_DATA_ERROR		           	0x45
#   define STF_DATA_READY                   0x46
#define STA_LOGIN                           0x03    
#define STA_PASSWORD                        0x04    

#define STA_NETWORK_ADDRESS  		         5   // 

/*


#define STA_NETWORK_MASK                     2   //
#define STA_NETWORK_DNS1                     3   //
#define STA_NETWORK_DNS2                     4   //
#define STA_NETWORK_GATE                     5   //
#define STA_NETWORK_MODE                     6   //
#   define STM_MODE_AUTO                     7   // 
#   define STM_MODE_DHCP                     8   // 
#   define STM_MODE_STATIC                   9   // 
#define STA_NETWORK_NAME                     10  //
#define STA_NETWORK_MAC                      11  //
#define STA_TIME_SNTP                        12   // 
#define STA_TIME_RTC                         13   // 
#define STA_TIME_SELECT                      14   // 
//
#define STA_ALPHA_GRAD_CURRENT               15   //
#define STA_ALPHA_GRAD_TARGET                16   //
#define STA_ALPHA_GRAD_MIN                   17   //
#define STA_ALPHA_GRAD_MAX                   18   //
#define STA_ALPHA_STEP_CURRENT               19   //
#define STA_ALPHA_STEP_TARGET                20   //
#define STA_ALPHA_STEP_MIN                   21   //
#define STA_ALPHA_STEP_MAX                   22   //
#define STA_ALPHA_DIRECTION                  23   //
#define STA_ALPHA_CONFIG                     24   //
#define STA_DELTA_CURRENT                    25   //
#define STA_DELTA_TARGET                     26   //
#define STA_DELTA_NUL                        27   //
*/
#define MEM_BUFFER_LEN 64
#define MAX_ATTRIBUTE 10
typedef BYTE ST_ATTRIBUTE_TYPE;
typedef struct ST_ATTRIBUTE {
    ST_ATTRIBUTE_TYPE   type;
    BYTE                ulValueLen;  /* in bytes */
    void*               pValue;    
} ST_ATTRIBUTE;
typedef ST_ATTRIBUTE*  ST_ATTRIBUTE_PTR;

#define FALSE	0
#define TRUE    1
//static BYTE SendAttributes();
#define MAX_BUFFER_LEN 64
BYTE FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE bAttributeLen, BYTE* pbBlock, BYTE bBlockLen ,BYTE* pbBlockPos);
BYTE ParseBlob(BYTE* pbBlock, BYTE bBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE *pbAttributeLen, BYTE* pbMem, BYTE bMemLen, BYTE* bMemPos);
BYTE ProcessClients(BYTE bConnectionID, BYTE* pbBlob, BYTE* pbBlobLen);
BYTE FindParam(ST_ATTRIBUTE* pData, BYTE bDataLen, ST_ATTRIBUTE_TYPE bType);
BYTE RunClient(BYTE* pbBlob, BYTE bBlobLen, BYTE *pbDataLength);
#endif