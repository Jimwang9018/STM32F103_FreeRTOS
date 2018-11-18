#include "bleSendTask.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "usart2.h"
#include "key.h"
#include "imuTask.h"
#include "flashFunction.h" 
#include <stdbool.h>
#include "MPU9250.h"
#include "imu.h"
#include "fft.h"
#include "usart1.h"


void vBLESendTask(void *pvParameters)
{
    S_FLOAT_XYZ * Q_ANGLE;
    portTickType xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
			if(KEY.status == ON)
			{
					if( xQueueReceive(xAngleBuffSend, &(Q_ANGLE), ( portTickType ) 0  ) == pdPASS)
					{
							if(BLE.IsLink() == true)												/*蓝牙连接上时通过蓝牙传输数据*/
							{
									if( xSemaphoreTake( Usart2.xSemaphoreUsart2SendOver, ( portTickType ) 0 ) == pdTRUE )
									{
											BLE.SendData(*Q_ANGLE);
									}
							}
							
					}
					
					if( xQueueReceive(xAngleBuffStorage, &(Q_ANGLE), ( portTickType ) 0  ) == pdPASS)
					{
						
							FlashFun.writeData(*Q_ANGLE);
					}
					
			}
			
       vTaskDelayUntil( &xLastWakeTime, 65/portTICK_RATE_MS ); 
    }
}

