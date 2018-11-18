#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "esp8266.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "wifiFunction.h"

WIFIFunTypeDef WIFIFun;

static void Init(void)
{
    WIFI.Enable( ENABLE );
    WIFI.ATTest();
    WIFI.NetModeChoose ( STA );
}

/*-----------------------------------------------------------*/

static void ListAP(void)
{
    WIFI.LongAckCmdInput( "AT+CWLAP", "OK", 0, 5000 );
}

/*-----------------------------------------------------------*/

static void AddAPLinkServer(void)
{
    char  * pStrDelimiter [4], * pBuf, * pStr;
    u8 uc;
    do
    {
        pBuf = linkData;
        uc = 0;
        while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
        {
            pStrDelimiter [ uc ++ ] = pStr;
            pBuf = NULL;
        }

    } while ( ! JoinAP ( pStrDelimiter [0], pStrDelimiter [1] ) );

    EnableMultipleId ( ENABLE );
    PC_Usart ( "\r\n added AP\r\n" );
}

/*-----------------------------------------------------------*/

void AddServer(void)
{
    char  * pStrDelimiter [2], * pBuf, * pStr;
    u8 uc;
    do
    {
        pBuf = wifiConfig.serverIP;
        uc = 0;
        while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
        {
            pStrDelimiter [ uc ++ ] = pStr;
            pBuf = NULL;
        }

    } while ( ! ( ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_0 ) &&
                  ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_1 ) &&
                  ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_2 ) &&
                  ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_3 ) &&
                  ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_4 ) ) );

}

void WIFISendTest(void)
{
    char cStrInput [] = "wifi test\r\n", * pStrDelimiter [2], * pBuf, * pStr;
    u8 uc,ul;
    for ( uc = 0; uc < 5; uc ++ )
    {
        //PC_Usart ( "\r\n请输入端口ID%d要发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n", uc );
        PC_Usart("please input char\r\n");
        ul = strlen ( cStrInput );

        ESP8266_SendString ( DISABLE, cStrInput, ul, ( ENUM_ID_NO_TypeDef ) uc );

    }

}
