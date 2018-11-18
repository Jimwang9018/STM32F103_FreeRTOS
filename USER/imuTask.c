#include "imuTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

#include "imu.h"
#include "flash.h"
#include "MPU9250.h"
#include "w25q256.h"
#include "flashFunction.h"
#include "usart1.h"
#include "fft.h"
#include "key.h"

xQueueHandle xAngleBuffSend;
xQueueHandle xAngleBuffStorage;

void vIMUTask(void *pvParameters)
{
    static S_FLOAT_XYZ Q_ANGLE;
    static S_FLOAT_XYZ *pQ_ANGLE;
    portTickType xLastWakeTime;
		static u8 timeCont;
		xLastWakeTime = xTaskGetTickCount();
    MPU6500_Init();
		FlashFun.readOffsetData();

    xAngleBuffSend      = xQueueCreate( 1, ( unsigned portBASE_TYPE ) sizeof(S_FLOAT_XYZ*) );
    xAngleBuffStorage   = xQueueCreate( 1, ( unsigned portBASE_TYPE ) sizeof(S_FLOAT_XYZ*) );
		
    while(1)
    {
			
        pQ_ANGLE = &Q_ANGLE;
        taskENTER_CRITICAL();
        {
            Prepare_Data();
            Get_Attitude(&Q_ANGLE);						
        }
        taskEXIT_CRITICAL();
				
				if(KEY.status == ON)
				{
						if(++timeCont >=2)
						{
								timeCont = 0;
								fft();
						}
				}
				
				
        xQueueSend( xAngleBuffStorage, (void *)&pQ_ANGLE, ( portTickType ) 0  );     /**<  */

        xQueueSend( xAngleBuffSend, (void *)&pQ_ANGLE, ( portTickType ) 0  );        /**<  */

        vTaskDelayUntil( &xLastWakeTime, 10/portTICK_RATE_MS );
    }

	Usart1.sendAngle(Q_ANGLE);
}

