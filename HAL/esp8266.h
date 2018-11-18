#ifndef __ESP8266_CONFIG_H
#define	__ESP8266_CONFIG_H

#include "stm32f10x.h"
#include <stdbool.h>
#include "usart3.h"
#include "usart2.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define 		WIFI_STANDBY_MODE		0 
#define 		WIFI_SET_MODE       1
#define 		WIFI_TRANS_MODE     2

#define			OPEN_WIFI_SUCCESS 	 			0x01
#define			LIST_AP_SUCCESS    				0x02
#define			ADD_AP_SUCCESS     				0x03
#define			LINK_SERVER_SUCCESS 			0x04
#define			SET_UNVARNISH_SUCCESS			0x05
#define			REIGSTER_SERVER_SUCCESS 	0x06
#define			TYR_AGAIN 								0x07
#define			WIFI_SET_OVER							0x08
#define 		RECEIVE_APNAME_SUCCESS 				0x10
#define			RECEIVE_APPASSWORD_SUCCESS		0x11
#define 		RECEIVE_SERVERIP_SUCCESS 			0x12
#define			RECEIVE_SERVERPORT_SUCCESS		0x13

#define 		SET_STEP_LIST_AP		0
#define 		SET_STEP_LINK				1
#define			SET_REIGSTER_SERVER			2
#define 		SET_OVER						3
#define			ABNORMAL_EXIT     4

#define     ESP8266_CH_HIGH_LEVEL()             GPIO_SetBits( GPIOB, GPIO_Pin_13 )
#define     ESP8266_CH_LOW_LEVEL()              GPIO_ResetBits( GPIOB, GPIO_Pin_13 )

#define     ESP8266_RST_HIGH_LEVEL()            GPIO_SetBits( GPIOB, GPIO_Pin_3 )
#define     ESP8266_RST_LOW_LEVEL()             GPIO_ResetBits( GPIOB, GPIO_Pin_3 )

#define 		BUF_MAX_LEN     1024                                     //最大接收缓存字节数
#define 		BUF_MAX_LEN     1024                                     //最大发送缓存字节数

//* 数据类型 */
typedef enum {
    STA,
    AP,
    STA_AP
} ENUM_Net_ModeTypeDef;

typedef enum {
    enumTCP,
    enumUDP,
} ENUM_NetPro_TypeDef;

typedef enum {
    Multiple_ID_0 = 0,
    Multiple_ID_1 = 1,
    Multiple_ID_2 = 2,
    Multiple_ID_3 = 3,
    Multiple_ID_4 = 4,
    Single_ID     = 5,
} ENUM_ID_NO_TypeDef;

typedef enum {
    OPEN = 0,
    WEP = 1,
    WPA_PSK = 2,
    WPA2_PSK = 3,
    WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;




typedef struct{
		char name[20];
		char password[20];
		char serverIP[20];
		char serverPort[20];
		u8 nameLength;
		u8 passwordLength;
		u8 serverIPLength;
		u8 serverPortLength;
} LinkDataTypeDef;

typedef struct
{
		xSemaphoreHandle xSemaphoreInitWIFI;
		xSemaphoreHandle xSemaphoreListAP;
		xSemaphoreHandle xSemaphoreAddAP;
		xSemaphoreHandle xSemaphoreRegisterServer;
		u8 mode;
		u8 setStep;
		u8 RxStep;
		u8 APPOverFlag;
		u8 tranModeTimeCont;
		bool isSendData;
		LinkDataTypeDef  linkData;
		bool active;
    void (*GPIOConfig)( void );
    void (*Enable) (FunctionalState enumChoose);
    void (*Rest) ( void );
    bool (*ATTest) ( void );
    bool (*CmdInput) ( char * cmd, char * reply1, char * reply2, u32 waittime );
    bool (*LongAckCmdInput) ( char * cmd, char * reply1, char * reply2, u32 waittime );
    bool (*NetModeChoose) ( ENUM_Net_ModeTypeDef enumMode );
    bool (*JoinAP) ( char * pSSID, char * pPassWord );
    bool (*BuildAP) ( char * pSSID, char * pPassWord, char * enunPsdMode );
    bool (*EnableMultipleId) ( FunctionalState enumEnUnvarnishTx );
    bool (*LinkServer) ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
    bool (*StartOrShutServer) ( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
    u8 (*UnvarnishSend) ( void );
		bool (*OutUnvarnish) ( void );
    bool (*SendString) ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );
		u8 (*Init)(void);
		u8 (*ListAP)(void);
		u8 (*AddAP)();
		u8 (*AddServer)();
		u8 (*CreatLink)(void);
		void (*RegisterToServer)(void);
		void (*SendDataToServer)(void);
		
} WIFITypeDef;

void WIFISetup(void);

extern WIFITypeDef  WIFI;



#endif    /* __WIFI_CONFIG_H */

