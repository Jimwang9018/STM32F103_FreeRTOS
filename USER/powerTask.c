#include "powerTask.h"
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
#include "esp8266.h"
#include "flashFunction.h"

void vPowerTask(void *pvParameters)
{
		u16 wifiTransCont = 0;
    while(1)
    {
        if(KEY.status == ON  || WIFI.active == true || FlashFun.isEaraseFlash == true)
        {
            POWER.downTime = 0;
        } else if(++POWER.downTime >= 120)
        {
            vTaskSuspendAll();
						POWER.Off();
        }
				
				if(WIFI.mode == WIFI_TRANS_MODE)	/*WIFI¥´ ‰≥¨ ±º‡≤‚*/
				{
						WIFI.tranModeTimeCont++;
						if(WIFI.tranModeTimeCont >= 30)
						{
								WIFI.tranModeTimeCont =0;
								WIFI.active = false;
								FlashFun.isEaraseFlash = false;
						}
						
						if(WIFI.isSendData == true)
						{
							wifiTransCont ++;
							if(wifiTransCont >= 240)
							{
									wifiTransCont = 0;
									WIFI.active = false;
									FlashFun.isEaraseFlash = false;
							}
						}
				}
				
				
				
				POWER.value = (float)ADC.convertedValue[1] * 0.001611;  //µÁ—πºÏ≤‚
				POWER.chargeTest();
				if(POWER.chargeMode == noCharge || POWER.chargeMode == chargeOver)
				{
						if(POWER.value <= 3.68)
						{
								POWER.batterSatus = low;
								POWER.batterWarn = ON;
						}

						if(POWER.value <= 3.50)
						{
								POWER.batterSatus = veryLow;
								POWER.batterWarn = ON;
						}
					
						if(POWER.batterSatus == veryLow)
						{
								vTaskDelay( 3000/portTICK_RATE_MS );
								POWER.chargeTest();
								if(POWER.chargeMode == charging)
								{
										POWER.batterSatus = enough;
								}else{
										vTaskSuspendAll();
										POWER.Off();
								}
								
						}
				}
				

				vTaskDelay(1000/portTICK_RATE_MS );
    }
}

