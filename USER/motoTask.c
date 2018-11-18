#include "FreeRTOS.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"
#include "main.h"
#include "moto.h"
#include "power.h"
#include "key.h"
#include "motoTask.h"

void  vMotoTask(void *pvParameters)
{
    while(1)
    {
        if(POWER.chargeMode == noCharge )								    //À¢—¿Ã· æ
        {
            if(KEY.status == ON)
            {
								MOTO.SetPWM(800);
            } else {
                MOTO.SetPWM(0);
            }
        }
        vTaskDelay( 50/portTICK_RATE_MS );
    }

}
