#include "esp8266.h"
#include "wifiSendTask.h"
#include "powerTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "usart3.h"
#include <string.h>
#include <stdio.h>
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "main.h"
#include "esp8266.h"
#include "flashFunction.h"

void vWifiSendTask(void *pvParameters)
{		
		Usart3Setup();
		WIFISetup();
		WIFI.Init();
		
    while(1)
    {
        if(WIFI.mode == WIFI_SET_MODE)
				{
					switch(WIFI.setStep)
					{
							case SET_STEP_LIST_AP :
							{
									if( xSemaphoreTake( WIFI.xSemaphoreListAP, portMAX_DELAY ) == pdTRUE )	
									{
											WIFI.ListAP();
											WIFI.setStep = SET_STEP_LINK;
									}
							}	break;
							case SET_STEP_LINK :
							{
									if( xSemaphoreTake( WIFI.xSemaphoreAddAP, portMAX_DELAY ) == pdTRUE )
									{
											WIFI.AddAPLinkServer();
											FlashFun.storgeLinkData(WIFI.linkData);
									}
							} break;
							case SET_OVER :
							{

							}
							
					}
						
				}else
				{
						
				}
    }
}
