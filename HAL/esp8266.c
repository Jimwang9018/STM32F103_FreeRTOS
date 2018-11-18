#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "esp8266.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "flashFunction.h"

WIFITypeDef   WIFI;

static void GPIOConfig( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_3 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    GPIO_ResetBits( GPIOB, GPIO_Pin_13 );		/* ����WiFiģ���Ƭѡ����	*/
    GPIO_SetBits( GPIOB, GPIO_Pin_3 );			/* ����WiFiģ��ĸ�λ��������	*/
}

/********************************************************
 * ��������Enable
 * ����  ��ʹ��/����WF-ESP8266ģ��
 * ����  ��enumChoose = ENABLE��ʹ��ģ��
 *         enumChoose = DISABLE������ģ��
 * ����  : ��
 * ����  �����ⲿ����
 ********************************************************/
static void Enable ( FunctionalState enumChoose )
{	
    if ( enumChoose == ENABLE )
        ESP8266_CH_HIGH_LEVEL();
    else
        ESP8266_CH_LOW_LEVEL();
}


/*************************************************************
 * ��������CmdInput
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 *************************************************************/
static bool CmdInput ( char * cmd, char * reply1, char * reply2, u32 waittime )
{
    //Usart3.RxBuff.length = 0;   //���¿�ʼ�����µ����ݰ�
		Usart3.Printf( "%s\r\n", cmd );	
	
    if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      	//����Ҫ��������
        return true;
	
    delayMs ( waittime );                 													//��ʱ
		
		if( xSemaphoreTake( Usart3.xSemaphoreUsart3ReadFull, ( portTickType ) 0 ) == pdTRUE )
		{
				if(Usart3.RxBuff.length >= 255)
							Usart3.RxBuff.length = 255;
				
				Usart3.RxBuff.data[Usart3.RxBuff.length] = '\0';							
				//Usart2.Printf( "%s", Usart3.RxBuff.data );
				//delayMs(50);
				//printf("%s", Usart3.RxBuff.data );
		
    if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
				return ( ( bool ) strstr ( (const char *)Usart3.RxBuff.data, reply1 ) ||
                 ( bool ) strstr ( (const char *)Usart3.RxBuff.data, reply2 ) );
		
    else if ( reply1 != 0 )
				return ( ( bool ) strstr ( (const char *)Usart3.RxBuff.data, reply1 ) );	
    
		else
			return ( ( bool ) strstr ( (const char *)Usart3.RxBuff.data, reply2 ) );
	}
	else
	{
			return false;
	}
		
}

/************************************************************
 * ��������LongCmdInput
 * ����  ��Ӧ�𳬹�200�ֽڵ�ָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 ************************************************************/
static bool LongAckCmdInput ( char * cmd, char * reply1, char * reply2, u32 waittime )
{
    char  *pStr ;
		char	*pBuff;
		//Usart3.RxBuff.length = 0; 
    Usart3.Printf("%s\r\n", cmd );
   
    if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
        return true;

    delayMs (waittime);                 //��ʱ
		
		if( xSemaphoreTake( Usart3.xSemaphoreUsart3ReadFull, ( portTickType ) 0 ) == pdTRUE )
		{
				Usart3.RxBuff.data[Usart3.RxBuff.length] = '\0';
				pBuff = (char *)Usart3.RxBuff.data;	                         //���ָ��ʾ·������Ϣ
				pStr = strtok ( pBuff, "+");
				while (  pStr != NULL  ) 
				{
						Usart2.Printf("%s", pStr);
						pStr = strtok ( NULL, "+" );
						delayMs(50);
				}
			delayMs(200);	
    if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
        return ( ( bool ) strstr ( (const char *)Usart3.RxBuff.data, reply1 ) ||
                 ( bool ) strstr ( (const char *)Usart3.RxBuff.data, reply2 ) );

    else if ( reply1 != 0 )
			return ( ( bool ) strstr ( (const char *)Usart3.RxBuff.data, reply1 ) );
			
    else
        return ( ( bool ) strstr ( (const char *)Usart3.RxBuff.data, reply2 ) );
	}
	else
	{
			return false;
	}

}


/**********************************************************
 * ��������Rest
 * ����  ������WF-ESP8266ģ��
 * ����  ����
 * ����  : ��
 * ����  ����ESP8266_AT_Test����
 **********************************************************/
static void Rest ( void )
{
#if 0
    CmdInput ( "AT+RST", "OK", "ready", 2500 );

#else
    ESP8266_RST_LOW_LEVEL();
    delayMs ( 500 );
    ESP8266_RST_HIGH_LEVEL();

#endif

}

/**********************************************************
 * ��������ESP8266_AT_Test
 * ����  ����WF-ESP8266ģ�����AT��������
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 **********************************************************/
static bool ATTest ( void )
{
		u8 num = 0;
		bool temp;
	
    ESP8266_RST_HIGH_LEVEL();
    delayMs ( 1000 );
		
		do
		{
				temp = CmdInput ( "AT", "OK", NULL, 200 );
				num++;
				if(temp == false)
				{
						Rest ();
				}
				
		}while((temp == false) && (num <= 3));
		
		if(num <= 3)
		{
			return true;
		}else{
			return false;
		}
		
}


/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
 */
static bool NetModeChoose ( ENUM_Net_ModeTypeDef enumMode )
{
    switch ( enumMode )
    {
    case STA:
        return CmdInput ( "AT+CWMODE=1", "OK", NULL, 2500 );

    case AP:
        return CmdInput ( "AT+CWMODE=2", "OK", NULL, 2500 );

    case STA_AP:
        return CmdInput ( "AT+CWMODE=3", "OK", NULL, 2500 );

    default:
        return false;
    }

}


/*
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
static bool JoinAP ( char * pSSID, char * pPassWord )
{
    char cCmd [120];

    sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );

    return CmdInput ( cCmd, "OK", NULL, 7000 );

}


/*
 * ��������ESP8266_BuildAP
 * ����  ��WF-ESP8266ģ�鴴��WiFi�ȵ�
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 *       ��enunPsdMode��WiFi���ܷ�ʽ�����ַ���
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
static bool BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
    char cCmd [120];

    sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode );

    return CmdInput ( cCmd, "OK", 0, 1000 );

}


/*
 * ��������ESP8266_Enable_MultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
static bool EnableMultipleId ( FunctionalState enumEnUnvarnishTx )
{
    char cStr [20];

    sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );

    return CmdInput ( cStr, "OK", 0, 500 );

}


/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
static bool LinkServer ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
    char cStr [100] = { 0 }, cCmd [120];

    switch (  enumE )
    {
    case enumTCP:
        sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
        break;

    case enumUDP:
        sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
        break;

    default:
        break;
    }

    if ( id < 5 )
        sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

    else
        sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

    return CmdInput ( cCmd, "OK", "ALREAY CONNECT", 500 );

}


/*
 * ��������ESP8266_StartOrShutServer
 * ����  ��WF-ESP8266ģ�鿪����رշ�����ģʽ
 * ����  ��enumMode������/�ر�
 *       ��pPortNum���������˿ں��ַ���
 *       ��pTimeOver����������ʱʱ���ַ�������λ����
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
static bool StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
    char cCmd1 [120], cCmd2 [120];

    if ( enumMode )
    {
        sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );

        sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

        return ( CmdInput ( cCmd1, "OK", 0, 500 ) &&
                 CmdInput ( cCmd2, "OK", 0, 500 ) );
    }

    else
    {
        sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

        return CmdInput ( cCmd1, "OK", 0, 500 );
    }

}


/*
 * ��������ESP8266_UnvarnishSend
 * ����  ������WF-ESP8266ģ�����͸������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
static u8 UnvarnishSend ( void )
{
	bool temp;

	temp = CmdInput ( "AT+CIPMODE=1", "OK", 0, 500 );
		
	if(temp == false )
	{
			return 0x16;
	}
		
	temp = CmdInput ( "AT+CIPSEND", "\r\n", ">", 500 );

	if(temp == false )
	{
			return 0x16;
	}
	
	return 0;  
}

static bool OutUnvarnish ( void )
{
    Usart3.Printf("+++");
		delayMs(500);
		Usart3.Printf("+++");
		delayMs(500);
		CmdInput("AT+CWQAP", "OK", NULL, 500);
		CmdInput("AT+CWAUTOCONN=0", "OK", NULL, 500);
		return true;
}


/*
 * ��������ESP8266_SendString
 * ����  ��WF-ESP8266ģ�鷢���ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 *       ��pStr��Ҫ���͵��ַ���
 *       ��ulStrLength��Ҫ���͵��ַ������ֽ���
 *       ��ucId���ĸ�ID���͵��ַ���
 * ����  : 1�����ͳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
static bool SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
    char cStr [20];
    bool bRet = false;

    if ( enumEnUnvarnishTx )
		{
				Usart3.Printf("%s", pStr);
		}
    else
    {
        if ( ucId < 5 )
            sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

        else
            sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );

        CmdInput ( cStr, "> ", 0, 1000 );

        bRet = CmdInput ( pStr, "SEND OK", 0, 1000 );
    }

    return bRet;

}


static u8 Init(void)
{
    Enable( ENABLE );
		OutUnvarnish();
		if(ATTest() == false)
		{
				return 0x11;
		}
			
		if(NetModeChoose(STA) == false)
		{
				return 0x12;
		}
			
		return 0;
}


static u8 ListAP(void)
{
	if(LongAckCmdInput( "AT+CWLAP", "CWLAP", NULL, 5000 ) == false)
    return 0x13;
	
	return 0;
}


static u8 AddAP()
{    
    bool temp1=0;
	
		temp1 = JoinAP ( WIFI.linkData.name, WIFI.linkData.password );
				
		if(temp1 == false)
				return 0x14;
		
		return 0;
}

static u8 AddServer()
{
		bool temp;
	
		temp = LinkServer ( enumTCP, WIFI.linkData.serverIP, WIFI.linkData.serverPort, Single_ID );
				
		if(temp == false)
				return 0x15;
		
		return 0;
}

static u8 CreatLink(void)
{
		u8 temp;
		if(FlashFun.readLinkData(WIFI.linkData) == false)
		{
				return 0x17;
		}	
		temp = AddAP();
		if(temp != 0)
		{
			return temp;
		}
		
		temp = AddServer();
		if(temp != 0)
		{
			return temp;
		}
		return temp;
}


const u8 registerData[10] = {0x88, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00};

static void RegisterToServer(void)
{
		Usart3.TxBuff.length = 10;
		for(u8 i=0; i<10; i++)
		{
				Usart3.TxBuff.data[i] = registerData[i];
		}
		
		Usart3.TxBuff.data[1] = BYTE3(FlashFun.userID);
		Usart3.TxBuff.data[2] = BYTE2(FlashFun.userID);
		Usart3.TxBuff.data[3] = BYTE1(FlashFun.userID);
		Usart3.TxBuff.data[4] = BYTE0(FlashFun.userID);
		
		if(pdTRUE == xSemaphoreTake( Usart3.xSemaphoreUsart3SendOver, ( portTickType ) 5000 ))
    {
        DMA_SetCurrDataCounter(DMA1_Channel2, Usart3.TxBuff.length);
        DMA_Cmd(DMA1_Channel2, ENABLE);
    }
}

static void SendDataToServer(void)
{
		static u16 packID = 0;
		u16 temp;
	
		packID ++;
		Usart3.TxBuff.data[0] = 0x88;
		Usart3.TxBuff.data[1] = BYTE3(FlashFun.userID);
		Usart3.TxBuff.data[2] = BYTE2(FlashFun.userID);
		Usart3.TxBuff.data[3] = BYTE1(FlashFun.userID);
		Usart3.TxBuff.data[4] = BYTE0(FlashFun.userID);
		Usart3.TxBuff.data[5] = 0x00;
		Usart3.TxBuff.data[6] = 0x02;
		Usart3.TxBuff.data[7] = 0x00;
		Usart3.TxBuff.data[8] = 0x01;
		Usart3.TxBuff.data[9] = 0x64;
		temp = packID;
		Usart3.TxBuff.data[10] = BYTE1(temp);
		Usart3.TxBuff.data[11] = BYTE0(temp);
	
		Usart3.TxBuff.length += 12;
	
		if(pdTRUE == xSemaphoreTake( Usart3.xSemaphoreUsart3SendOver, ( portTickType ) 5000 ))
    {
        DMA_SetCurrDataCounter(DMA1_Channel2, Usart3.TxBuff.length);
        DMA_Cmd(DMA1_Channel2, ENABLE);
    }
		
}


void WIFISetup(void)
{
		WIFI.xSemaphoreInitWIFI = NULL;
		WIFI.xSemaphoreListAP = NULL;
		WIFI.xSemaphoreAddAP = NULL;
		WIFI.xSemaphoreRegisterServer = NULL;
		vSemaphoreCreateBinary(WIFI.xSemaphoreInitWIFI);
		vSemaphoreCreateBinary(WIFI.xSemaphoreListAP);
		vSemaphoreCreateBinary(WIFI.xSemaphoreAddAP);
		vSemaphoreCreateBinary(WIFI.xSemaphoreRegisterServer);
		xSemaphoreTake( WIFI.xSemaphoreInitWIFI, portMAX_DELAY );
		xSemaphoreTake( WIFI.xSemaphoreListAP, portMAX_DELAY );
		xSemaphoreTake( WIFI.xSemaphoreAddAP, portMAX_DELAY );
		xSemaphoreTake( WIFI.xSemaphoreRegisterServer, portMAX_DELAY );
		GPIOConfig();
		WIFI.ATTest 						= ATTest;
		WIFI.BuildAP 						= BuildAP;
		WIFI.CmdInput 					= CmdInput;
		WIFI.LongAckCmdInput 		= LongAckCmdInput;
		WIFI.Enable 						= Enable;
		WIFI.EnableMultipleId 	= EnableMultipleId;
		WIFI.UnvarnishSend 			= UnvarnishSend;
		WIFI.StartOrShutServer 	= StartOrShutServer;
		WIFI.SendString 				= SendString;
		WIFI.Rest 							= Rest;
		WIFI.NetModeChoose 			= NetModeChoose;
		WIFI.LinkServer 				= LinkServer;
		WIFI.JoinAP 						= JoinAP;	
		WIFI.Init 							= Init;
		WIFI.ListAP 						= ListAP;
		WIFI.AddAP					 		= AddAP;
		WIFI.AddServer					= AddServer;
		WIFI.CreatLink 					= CreatLink;
		WIFI.RegisterToServer		= RegisterToServer;
		WIFI.SendDataToServer   = SendDataToServer;
		WIFI.OutUnvarnish       = OutUnvarnish;
		WIFI.tranModeTimeCont  = 0;

}










/***********************************************************************************************************************************
************************************************************************************************************************************
************************************************************************************************************************************/
