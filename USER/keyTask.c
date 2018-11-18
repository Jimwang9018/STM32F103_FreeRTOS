#include "keyTask.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

#include "usart1.h"
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

void vKeyOnTask(void *pvParameters)
{
	
    portTickType xLastWakeTime;
		xLastWakeTime = xTaskGetTickCount();
    vTaskDelay( 500/portTICK_RATE_MS );     /**< 等待系统稳定再开启按键 */  
		KEYSetup();
		static u8 keyStep = 0;
	
    while(1)
    {
       if( (getKeyValue() == KEYON) && (keyStep == 0) && (POWER.chargeMode == noCharge || POWER.chargeMode == chargeOver ))
			 {
					 vTaskDelayUntil( &xLastWakeTime, 5/portTICK_RATE_MS );
					 if( getKeyValue() == KEYON )
					 {
							keyStep = 1;
					 }						 
			 }
			 
			 if((getKeyValue() == KEYOFF) && keyStep == 1)
			 {
					 vTaskDelayUntil( &xLastWakeTime, 10/portTICK_RATE_MS );
					 if((getKeyValue() == KEYOFF) && keyStep == 1 )
					 {
							keyStep = 0;
							if(KEY.status == OFF)
                   KEY.status = ON;
              else if(KEY.status == ON)
                   KEY.status = OFF;
							
							if(KEY.status == ON)
              {		
										
                   if( xSemaphoreTake( Usart2.xSemaphoreUsart2SendOver, ( portTickType ) 50 ) == pdTRUE )
                   {
												vTaskDelay( 70/portTICK_RATE_MS );
                        BLE.SendCmd(BURSH_ON);
                   }
               }
               else if(KEY.status == OFF)
               {
										
                   if( xSemaphoreTake( Usart2.xSemaphoreUsart2SendOver, ( portTickType ) 50 ) == pdTRUE )
                   {
												vTaskDelay( 70/portTICK_RATE_MS );
                        BLE.SendCmd(BURSH_OFF);
                    }
							 }

					 }
			 }
       
       vTaskDelayUntil( &xLastWakeTime, 20/portTICK_RATE_MS );     
    }

}

