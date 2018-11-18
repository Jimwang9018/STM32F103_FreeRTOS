#ifndef __MAIN_H
#define	__MAIN_H

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

#define INTERRUPT_PRIORITY_DMA1_Channel7    6
#define INTERRUPT_PRIORITY_DMA1_Channel2    4
#define INTERRUPT_PRIORITY_DMA1_Channel4    6
#define INTERRUPT_PRIORITY_USART2_RX        5
#define INTERRUPT_PRIORITY_USART1_RX        5
#define INTERRUPT_PRIORITY_USART3_RX        5

#define INTERRUPT_PRIORITY_TIM3       		 10
#define INTERRUPT_PRIORITY_EXIT       		 2

#define INTERRUPT_PRIORITY_USART3          7

#define PC_DEBUG

typedef struct {
    float X;
    float Y;
    float Z;
		int16_t V;
} S_FLOAT_XYZ;

extern xTaskHandle  vWIFITaskHandle;

#endif /* __LED_H */

