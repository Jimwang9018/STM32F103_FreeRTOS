#ifndef _FLASH_TASK_H
#define _FLASH_TASK_H

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
#include "ledTask.h"
#include "imuTask.h"
#include "keyTask.h"
#include "bleReceiveTask.h"
#include "powerTask.h"
#include "usart1Task.h"
#include "main.h"


#endif
