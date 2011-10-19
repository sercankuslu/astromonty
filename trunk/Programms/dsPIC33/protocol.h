#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_
#include "TCPIP Stack/TCPIP.h"
#define STR_OK                              0
#define STR_FUNCTOIN_FAILED                 6

// работа с подключением
#define STA_COMMAND                         0x01    // команды
#   define STC_REQEST_CONNECT               0x80    // запрос подключения
#   define STC_REQEST_AUTH                  0x81    // запрос проверки подлинности
#   define STC_REQEST_DATA                  0x40    // запрос данных
#   define STC_SEND_DATA                    0x41    // отправка данных

#define STA_FLAG                            0x02    // флаг подключения
#   define STF_ACCEPTED                     0x81    // подключение установлено
#   define STF_DECLINED                     0x82    // подключение отклонено
#   define STF_AUTH_NEEDED                  0x83    // требуется проверка подлинности
#   define STF_AUTH_NOT_NEEDED              0x84    // проверка подлинности не требуется 
#   define STF_TOO_MANY_DATA                0x40    // слишком много параметров 
#   define STF_INCORRECT_COMMAND            0x41    // неправильная комманда

#define STA_LOGIN                           0x03    // логин
#define STA_PASSWORD                        0x04    // пароль

/*
// параметры
#define STA_NETWORK_ADDRESS  		         1   // 
#define STA_NETWORK_MASK                     2   //
#define STA_NETWORK_DNS1                     3   //
#define STA_NETWORK_DNS2                     4   //
#define STA_NETWORK_GATE                     5   //
#define STA_NETWORK_MODE                     6   //
#   define STM_MODE_AUTO                     7   // сеть в автоматическом режиме ( DHCP сервер включается, если нет DHCP в сети)
#   define STM_MODE_DHCP                     8   // адрес получается от DHCP сервера
#   define STM_MODE_STATIC                   9   // настроен статический адрес
#define STA_NETWORK_NAME                     10  //
#define STA_NETWORK_MAC                      11  //
#define STA_TIME_SNTP                        12   // время из SNTP модуля
#define STA_TIME_RTC                         13   // время из RTC модуля
#define STA_TIME_SELECT                      14   // 
// управление двигателями
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
/* алгоритм подключения:
 * 1. установление связи по протоколу TCP
 * 2. отправить атрибут STA_CONNECT_COMMAND = STF_REQEST_CONNECT
 *    если нужна проверка пароля, сервер пришлёт пакет с атрибутом STA_CONNECT_FLAG = STF_NEEDAUTH
 * 2.1 в ответ на этот запрос надо отправить атрибуты: 
 *      STA_CONNECT_COMMAND = STC_REQEST_AUTH,STA_CONNECT_LOGIN, STA_CONNECT_PASSWORD 
 * 2.2 в случае успеха, сервер пришлёт атрибут STA_CONNECT_FLAG = STF_ACCEPTED
 * 3. запрос данных
 * 3.1 STA_CONNECT_COMMAND
 */
/*
typedef struct ST_PACKET {
    WORD                wAttributeCount;        // количество атрибутов
    ST_ATTRIBUTE*       Attributes;             // массив атрибутов
    WORD                DataLen;                // размер массива данных(сумма размеров всех данных, входящих в атрибуты)
    BYTE*               Data;                   // массив данных
} ST_PACKET;
*/
static BYTE SendAttributes();
#define MAX_BUFFER_LEN 255
BYTE FormBlob(ST_ATTRIBUTE_PTR pAttribute, BYTE ulAttribute, BYTE* Block, BYTE* ulBlockLen);
BYTE ParseBlob(BYTE* Block, BYTE ulBlockLen, ST_ATTRIBUTE_PTR pAttribute, BYTE ulAttribute, BYTE** Mem);
#endif