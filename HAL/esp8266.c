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
    GPIO_ResetBits( GPIOB, GPIO_Pin_13 );		/* 拉低WiFi模块的片选引脚	*/
    GPIO_SetBits( GPIOB, GPIO_Pin_3 );			/* 拉高WiFi模块的复位重启引脚	*/
}

/********************************************************
 * 函数名：Enable
 * 描述  ：使能/禁用WF-ESP8266模块
 * 输入  ：enumChoose = ENABLE，使能模块
 *         enumChoose = DISABLE，禁用模块
 * 返回  : 无
 * 调用  ：被外部调用
 ********************************************************/
static void Enable ( FunctionalState enumChoose )
{	
    if ( enumChoose == ENABLE )
        ESP8266_CH_HIGH_LEVEL();
    else
        ESP8266_CH_LOW_LEVEL();
}


/*************************************************************
 * 函数名：CmdInput
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 *************************************************************/
static bool CmdInput ( char * cmd, char * reply1, char * reply2, u32 waittime )
{
    //Usart3.RxBuff.length = 0;   //从新开始接收新的数据包
		Usart3.Printf( "%s\r\n", cmd );	
	
    if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      	//不需要接收数据
        return true;
	
    delayMs ( waittime );                 													//延时
		
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
 * 函数名：LongCmdInput
 * 描述  ：应答超过200字节的指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 ************************************************************/
static bool LongAckCmdInput ( char * cmd, char * reply1, char * reply2, u32 waittime )
{
    char  *pStr ;
		char	*pBuff;
		//Usart3.RxBuff.length = 0; 
    Usart3.Printf("%s\r\n", cmd );
   
    if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
        return true;

    delayMs (waittime);                 //延时
		
		if( xSemaphoreTake( Usart3.xSemaphoreUsart3ReadFull, ( portTickType ) 0 ) == pdTRUE )
		{
				Usart3.RxBuff.data[Usart3.RxBuff.length] = '\0';
				pBuff = (char *)Usart3.RxBuff.data;	                         //做分割，显示路由器信息
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
 * 函数名：Rest
 * 描述  ：重启WF-ESP8266模块
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被ESP8266_AT_Test调用
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
 * 函数名：ESP8266_AT_Test
 * 描述  ：对WF-ESP8266模块进行AT测试启动
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
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
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择WF-ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
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
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
static bool JoinAP ( char * pSSID, char * pPassWord )
{
    char cCmd [120];

    sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );

    return CmdInput ( cCmd, "OK", NULL, 7000 );

}


/*
 * 函数名：ESP8266_BuildAP
 * 描述  ：WF-ESP8266模块创建WiFi热点
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 *       ：enunPsdMode，WiFi加密方式代号字符串
 * 返回  : 1，创建成功
 *         0，创建失败
 * 调用  ：被外部调用
 */
static bool BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
    char cCmd [120];

    sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode );

    return CmdInput ( cCmd, "OK", 0, 1000 );

}


/*
 * 函数名：ESP8266_Enable_MultipleId
 * 描述  ：WF-ESP8266模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
static bool EnableMultipleId ( FunctionalState enumEnUnvarnishTx )
{
    char cStr [20];

    sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );

    return CmdInput ( cStr, "OK", 0, 500 );

}


/*
 * 函数名：ESP8266_Link_Server
 * 描述  ：WF-ESP8266模块连接外部服务器
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
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
 * 函数名：ESP8266_StartOrShutServer
 * 描述  ：WF-ESP8266模块开启或关闭服务器模式
 * 输入  ：enumMode，开启/关闭
 *       ：pPortNum，服务器端口号字符串
 *       ：pTimeOver，服务器超时时间字符串，单位：秒
 * 返回  : 1，操作成功
 *         0，操作失败
 * 调用  ：被外部调用
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
 * 函数名：ESP8266_UnvarnishSend
 * 描述  ：配置WF-ESP8266模块进入透传发送
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
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
 * 函数名：ESP8266_SendString
 * 描述  ：WF-ESP8266模块发送字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 *       ：pStr，要发送的字符串
 *       ：ulStrLength，要发送的字符串的字节数
 *       ：ucId，哪个ID发送的字符串
 * 返回  : 1，发送成功
 *         0，发送失败
 * 调用  ：被外部调用
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
