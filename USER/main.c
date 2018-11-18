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
#include "ledTask.h"
#include "imuTask.h"
#include "keyTask.h"
#include "bleReceiveTask.h"
#include "powerTask.h"
#include "usart1Task.h"
#include "w25q256.h"
#include "motoTask.h"
#include "wifiSendTask.h"
#include "esp8266.h"
#include "usart3.h"
#include "flashFunction.h"

xTaskHandle  vWIFITaskHandle;

void prvSetupHardware( void );

/*-----------------------------------------------------------*/

int main(void)
{
    prvSetupHardware();

    xTaskCreate( vIMUTask, ( signed portCHAR * ) "IMU", 512, NULL, tskIDLE_PRIORITY+15, NULL );
//    xTaskCreate( vBLESendTask, ( signed portCHAR * ) "BLESENDDATA", configMIDMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+6, NULL );
//    xTaskCreate( vLEDTask, ( signed portCHAR * ) "LEDMOTO", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+10, NULL );
//    xTaskCreate( vKeyOnTask, ( signed portCHAR * ) "KEY", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+4, NULL );
//    xTaskCreate( vBLEReceiveTask, ( signed portCHAR * ) "COMM", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+8, NULL );
//    xTaskCreate( vPowerTask, ( signed portCHAR * ) "POWER", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
//    xTaskCreate( vMotoTask, ( signed portCHAR * ) "MOTO", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+9, NULL );
//    xTaskCreate( vWifiSendTask, ( signed portCHAR * ) "WIFI", 512, NULL, tskIDLE_PRIORITY+1, NULL );
		//xTaskCreate( vWifiReceiveTask, ( signed portCHAR * ) "WIFI", 1024, NULL, tskIDLE_PRIORITY+1, NULL );
	
    vTaskStartScheduler();
    while(1);
		return 0;
}

/*-----------------------------------------------------------*/

void prvSetupHardware( void )
{		
		Tim3Init(2500); //5000
    Tim4Init(2500);
		
		ADCSetup();
//		KEYSetup();  				//按键驱动安装放在按键任务里面完成
//		LEDSetup();
		Usart1Setup();
//		Usart2Setup();
//		Usart3Setup();
		IIC_Init();
//		BLESetup();
//		MOTOSetup();
		POWERSetup();
//		FLASHSetup();
//		FlashFunSetup();
		IMUSetup();
//		WIFISetup();	
	printf("OK\r\n");
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}



