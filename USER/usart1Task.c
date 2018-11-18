#include "usart1Task.h"
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
#include "comm.h"
#include "power.h"
#include "flash.h"
#include "dog.h"
#include "bleTask.h"


#ifdef USING_USART1
void vUsatt1SendAngleTask(void *pvParameters)
{
    S_FLOAT_XYZ * Q_ANGLE;
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        if( xQueueReceive(xAngleBuffSend, &(Q_ANGLE), ( portTickType ) 0  ) == pdPASS)
        {
            if( xSemaphoreTake( xSemaphoreUsart1SendOver, ( portTickType ) 0 ) == pdTRUE )
            {
                USART1_SendAngle(*Q_ANGLE);
            }
        }
        vTaskDelayUntil( &xLastWakeTime, 30/portTICK_RATE_MS );
    }
}
#endif
