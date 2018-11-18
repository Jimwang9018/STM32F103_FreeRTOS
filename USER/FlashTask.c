#include "FlashTask.h"
#include "w25q256.h"
#include "imu.h"
#include "flashFunction.h"

void vFlashTask(void)
{
		S_FLOAT_XYZ * Q_ANGLE;
    portTickType xLastWakeTime = xTaskGetTickCount();
		while(1)
		{
				if( xQueueReceive(xAngleBuffStorage, Q_ANGLE, ( portTickType ) 0  ) == pdPASS)
				{
						FlashFun.writeData(*Q_ANGLE);
				}
				vTaskDelayUntil( &xLastWakeTime, 40/portTICK_RATE_MS ); 
		}
		
}
