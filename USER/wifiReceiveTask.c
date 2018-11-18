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

void vWifiReceiveTask(void *pvParameters)
{		
		if( xSemaphoreTake( Usart3.xSemaphoreUsart3ReadFull, portMAX_DELAY ) == pdTRUE )
		{
				
		}
		
}