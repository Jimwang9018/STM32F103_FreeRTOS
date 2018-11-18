#ifndef USART2_COMMS_H
#define USART2_COMMS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "imu.h"
#include "ble.h"

#define Usart2RecevieBuffLength 32
#define Usart2SendBuffLength    1024

typedef struct
{
    u8 data[Usart2SendBuffLength];
    u16 length;
} Usart2TxBuffTypeDef;

typedef struct
{
    u8 data[Usart2RecevieBuffLength];
    u16 length;
} Usart2RxBuffTypeDef;

typedef struct
{
    Usart2TxBuffTypeDef TxBuff;
    Usart2RxBuffTypeDef RxBuff;
    xSemaphoreHandle xSemaphoreUsart2ReadFull;
    xSemaphoreHandle xSemaphoreUsart2SendOver;
    void (*Init)( unsigned long baud);
		void (*Printf)( char *Data, ... );
} Usart2TypeDef;

void Usart2Setup(void);

extern Usart2TypeDef Usart2;

#endif


