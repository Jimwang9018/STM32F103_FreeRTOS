#ifndef __KEY_H
#define	__KEY_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f10x.h"
#include <stdbool.h>

#define getKeyValue()  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEYON	 	0x01
#define KEYOFF  0X00

#define ON	1
#define OFF	0

typedef struct{
    u8 status;
    //xSemaphoreHandle xSemaphoreKeyOn;
    void (*Init)(void);
}KEYTypeDef;

void KEYSetup(void);

extern KEYTypeDef KEY;

#endif
