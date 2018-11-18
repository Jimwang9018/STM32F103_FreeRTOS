#include "ledTask.h"
#include "stm32f10x.h"
#include "ble.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "main.h"
#include "moto.h"
#include "power.h"
#include "key.h"
#include "ledTask.h"
#include "usart1.h"
#include <stdbool.h>
#include "esp8266.h"

void  vLEDTask(void *pvParameters)
{
		vTaskDelay( 100/portTICK_RATE_MS );
    while(1)
    {			
        if(BLE.isConnect == false)																						//蓝牙提示
        {
						if(BLE.IsLink() == true)
						{
								LED.GreenLedOff();
								LED.RedLedOff();
								for(u8 i=0; i<6; i++)
								{
										LED.BlueLedBlink();
										vTaskDelay( 250/portTICK_RATE_MS );
								}
								POWER.downTime = 0;
								LED.BlueLedOff();
								BLE.isConnect = true;
						}
        }else{
						if(BLE.IsLink() == false)
						{
								BLE.isConnect = false;
						}
				}
				
				if(POWER.chargeMode == noCharge  )					//刷牙提示
        {
            if(KEY.status == ON)
            {
                LED.GreenLedOn();
            } else {              
                LED.GreenLedOn();
            }
        }
				else if(POWER.chargeMode == charging)                           //充电提示
        {
						LED.RedLedOn();
        }
				
//				if(WIFI.isSendData == true)
//				{
////						LED.GreenLedOff();
////						LED.RedLedOff();
//				}
				
				static u8 fristFlag = 0;
        if(POWER.batterWarn == ON)          														// 电压低提示
        {
						
						if(POWER.batterSatus == low && fristFlag == 0)
						{
								LED.GreenLedOff();
								LED.BlueLedOff();
								for(u8 i=0; i<5; i++)
								{
										LED.RedLedOn();
										vTaskDelay( 250/portTICK_RATE_MS );
										LED.RedLedOff();
										vTaskDelay( 250/portTICK_RATE_MS );
								}  
								fristFlag = 1;
						}
						
						if(POWER.batterSatus == veryLow)
						{
								LED.GreenLedOff();
								LED.BlueLedOff();
								for(u8 i=0; i<5; i++)
								{
										LED.RedLedOn();
										vTaskDelay( 250/portTICK_RATE_MS );
										LED.RedLedOff();
										vTaskDelay( 250/portTICK_RATE_MS );
								} 
						}
                    
        }
				
        
        vTaskDelay( 10/portTICK_RATE_MS );
    }

}

///*-----------------------------------------------------------*/
