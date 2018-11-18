#include "bleReceiveTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

#include "usart2.h"
#include "delay.h"
#include "led.h"
#include "I2C.h"
#include "MPU9250.h"
#include "imu.h"
#include "tim.h"
#include "ADC.h"
#include "moto.h"
#include "ble.h"
#include "key.h"
#include "power.h"
#include "flash.h"
#include "bleSendTask.h"
#include "wifiSendTask.h"
#include "main.h"
#include "esp8266.h"
#include "w25q256.h"
#include "flashFunction.h"

void basedControlHandle(void);
void wifiControlHandle(void);
void flashControlHandle(void);


void vBLEReceiveTask(void *pvParameters)
{
		xSemaphoreTake( Usart2.xSemaphoreUsart2ReadFull, ( portTickType ) 0 );		/**< 先读取一次信号量*/
	
    while(1)
    {
        if( xSemaphoreTake( Usart2.xSemaphoreUsart2ReadFull, portMAX_DELAY ) == pdTRUE )
        {		
						
            if(BLE.APPbuffDataPrepare(Usart2.RxBuff.data) == true)
            {
                switch(BLE.command)
                {
										case basedControl:
												basedControlHandle();
												break;
																		
										case wifiControl:
												wifiControlHandle();																														
												break;
										
										case flashControl:										
												flashControlHandle();
												break;
																			
										default :
												break;
                }
            }
        }
    }

}

void basedControlHandle()
{
		u8 key = 0;
		key = Usart2.RxBuff.data[4] & 0x0f;
	
		if(key == 0x01)
		{
				if(Usart2.RxBuff.data[5] == 0x01)                  	/**< 手机发送指令开启刷牙 */
				{
						if( xSemaphoreTake( Usart2.xSemaphoreUsart2SendOver, ( portTickType ) 10 ) == pdTRUE )
						{
								BLE.SendAck(ACK_ON);
						}
						KEY.status = ON;

				} else if(Usart2.RxBuff.data[5] == 0x02)                   /**< 手机发送指令关闭刷牙 */
				{
						if( xSemaphoreTake( Usart2.xSemaphoreUsart2SendOver, ( portTickType ) 10 ) == pdTRUE )
						{
								BLE.SendAck(ACK_OFF);
						}
						KEY.status = OFF;
				}
		}else if(key == 0x02){
				FlashFun.userID |= Usart2.RxBuff.data[8] << 24;
				FlashFun.userID |= Usart2.RxBuff.data[7] << 16;
				FlashFun.userID |= Usart2.RxBuff.data[6] << 8;
				FlashFun.userID |= Usart2.RxBuff.data[5];
			
				FlashFun.storgeUserID();
				BLE.SendCmd(BINDING_SUCCESS);
		}
		
}


void wifiControlHandle()
{
		u8 key = 0;

		key = Usart2.RxBuff.data[4] & 0x0f;
		if(key == 0x01 )								
		{
				if(Usart2.RxBuff.data[5] == 0x01)		//开启WIFI
				{															
						WIFI.mode = WIFI_SET_MODE;
						WIFI.setStep = SET_STEP_LIST_AP;
						xSemaphoreGive(WIFI.xSemaphoreInitWIFI);
				} else if(Usart2.RxBuff.data[5] == 0x02 )
				{
						WIFI.setStep = SET_OVER;																								
				}
										
		}									
		else if(key == 0x02 )									//接收WIFI名字
		{						
				char * pLinkDataBuff;
				pLinkDataBuff = WIFI.linkData.name;
				for(u8 i = 0; i < Usart2.RxBuff.data[2]-5; i++)
				{
						*pLinkDataBuff = (char)Usart2.RxBuff.data[5+i];
						pLinkDataBuff ++;
						WIFI.linkData.nameLength ++;
				}
				BLE.SendSetWIFIData(RECEIVE_APNAME_SUCCESS);		/**< 返回接收AP name成功指令到手机 */						
		}
												
		else if(key == 0x03 )
		{
				char * pLinkDataBuff;
				pLinkDataBuff = WIFI.linkData.password;
				for(u8 i = 0; i < Usart2.RxBuff.data[2]-5; i++)
				{
						*pLinkDataBuff = (char)Usart2.RxBuff.data[5+i];
						pLinkDataBuff ++;
						WIFI.linkData.passwordLength ++;
				}			
				xSemaphoreGive(WIFI.xSemaphoreAddAP);
				BLE.SendSetWIFIData(RECEIVE_APPASSWORD_SUCCESS); /**< 返回接收AP password成功指令到手机 */	
		}
												
		else if(key == 0x04 )
		{
				char * pLinkDataBuff;
				pLinkDataBuff = WIFI.linkData.serverIP;
				for(u8 i = 0; i < Usart2.RxBuff.data[2]-5; i++)
				{
						*pLinkDataBuff = (char)Usart2.RxBuff.data[5+i];
						pLinkDataBuff ++;
						WIFI.linkData.serverIPLength ++;
				}
				BLE.SendSetWIFIData(RECEIVE_SERVERIP_SUCCESS); /**< 返回接收server IP成功指令到手机 */	
		}
												
		else if(key == 0x05 )
		{
				char * pLinkDataBuff;
				pLinkDataBuff = WIFI.linkData.serverPort;
				for(u8 i = 0; i < Usart2.RxBuff.data[2]-5; i++)
				{
						*pLinkDataBuff = (char)Usart2.RxBuff.data[5+i];
						pLinkDataBuff ++;
						WIFI.linkData.serverPortLength ++;
				}
				xSemaphoreGive(WIFI.xSemaphoreRegisterServer);
				BLE.SendSetWIFIData(RECEIVE_SERVERPORT_SUCCESS); /**< 返回接收server port成功指令到手机 */	
		}else if(key == 0x08 )
		{
				if(Usart2.RxBuff.data[5] == 0x01)
				{
						WIFI.APPOverFlag = 1;
						xSemaphoreGive(WIFI.xSemaphoreAddAP);	
				}
		}
														
}


void flashControlHandle()
{
		if(Usart2.RxBuff.data[5] == 0x01)
		{		
				FlashFun.isEaraseFlash = true;
				printf("earase start\r\n");
				FlashFun.eraseDataArea();
				printf("erase ok\r\n");
				FlashFun.isEaraseFlash = false;
		}
}
