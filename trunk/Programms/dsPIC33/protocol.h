#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_
#include "TCPIP Stack/TCPIP.h"
#define STR_OK                              0
#define STR_FUNCTOIN_FAILED                 6

// ������ � ������������
#define STA_COMMAND                         0x01    // �������
#   define STC_REQEST_CONNECT               0x80    // ������ �����������
#   define STC_REQEST_AUTH                  0x81    // ������ �������� �����������
#   define STC_REQEST_DATA                  0x40    // ������ ������
#   define STC_SEND_DATA                    0x41    // �������� ������

#define STA_FLAG                            0x02    // ���� �����������
#   define STF_ACCEPTED                     0x81    // ����������� �����������
#   define STF_DECLINED                     0x82    // ����������� ���������
#   define STF_AUTH_NEEDED                  0x83    // ��������� �������� �����������
#   define STF_AUTH_NOT_NEEDED              0x84    // �������� ����������� �� ��������� 
#   define STF_TOO_MANY_DATA                0x40    // ������� ����� ���������� 
#   define STF_INCORRECT_COMMAND            0x41    // ������������ ��������

#define STA_LOGIN                           0x03    // �����
#define STA_PASSWORD                        0x04    // ������

/*
// ���������
#define STA_NETWORK_ADDRESS  		         1   // 
#define STA_NETWORK_MASK                     2   //
#define STA_NETWORK_DNS1                     3   //
#define STA_NETWORK_DNS2                     4   //
#define STA_NETWORK_GATE                     5   //
#define STA_NETWORK_MODE                     6   //
#   define STM_MODE_AUTO                     7   // ���� � �������������� ������ ( DHCP ������ ����������, ���� ��� DHCP � ����)
#   define STM_MODE_DHCP                     8   // ����� ���������� �� DHCP �������
#   define STM_MODE_STATIC                   9   // �������� ����������� �����
#define STA_NETWORK_NAME                     10  //
#define STA_NETWORK_MAC                      11  //
#define STA_TIME_SNTP                        12   // ����� �� SNTP ������
#define STA_TIME_RTC                         13   // ����� �� RTC ������
#define STA_TIME_SELECT                      14   // 
// ���������� �����������
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
typedef unsigned short ST_ATTRIBUTE_TYPE;

typedef struct ST_ATTRIBUTE {
    ST_ATTRIBUTE_TYPE   type;
    void*               pValue;    
    WORD                ulValueLen;  /* in bytes */
} ST_ATTRIBUTE;
typedef ST_ATTRIBUTE*  ST_ATTRIBUTE_PTR;
/* �������� �����������:
 * 1. ������������ ����� �� ��������� TCP
 * 2. ��������� ������� STA_CONNECT_COMMAND = STF_REQEST_CONNECT
 *    ���� ����� �������� ������, ������ ������ ����� � ��������� STA_CONNECT_FLAG = STF_NEEDAUTH
 * 2.1 � ����� �� ���� ������ ���� ��������� ��������: 
 *      STA_CONNECT_COMMAND = STC_REQEST_AUTH,STA_CONNECT_LOGIN, STA_CONNECT_PASSWORD 
 * 2.2 � ������ ������, ������ ������ ������� STA_CONNECT_FLAG = STF_ACCEPTED
 * 3. ������ ������
 * 3.1 STA_CONNECT_COMMAND
 */
/*
typedef struct ST_PACKET {
    WORD                wAttributeCount;        // ���������� ���������
    ST_ATTRIBUTE*       Attributes;             // ������ ���������
    WORD                DataLen;                // ������ ������� ������(����� �������� ���� ������, �������� � ��������)
    BYTE*               Data;                   // ������ ������
} ST_PACKET;
*/
static BYTE SendAttributes();
#define MAX_BUFFER_LEN 255
BYTE FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE ulAttribute, BYTE* Block, BYTE* ulBlockLen);
BYTE ParseBlob(BYTE* Block, BYTE ulBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE ulAttribute, BYTE** Mem);
#endif