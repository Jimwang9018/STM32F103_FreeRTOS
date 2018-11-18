#ifndef USART3_COMMS_H
#define USART3_COMMS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x.h"

#define Usart3SendBuffLength        1024
#define Usart3RecevieBuffLength     1024

typedef struct
{
    u8 data[Usart3SendBuffLength];
    u16 length;
} Usart3TxBuffTypeDef;

typedef struct
{
    u8 data[Usart3RecevieBuffLength];
    u16 length;
} Usart3RxBuffTypeDef;

typedef struct
{
    Usart3TxBuffTypeDef TxBuff;
    Usart3RxBuffTypeDef RxBuff;
    xSemaphoreHandle xSemaphoreUsart3ReadFull;
    xSemaphoreHandle xSemaphoreUsart3SendOver;
    void (*Usart3Init)( unsigned long baud);
		void (*Printf)( char *Data, ... );
} Usart3TypeDef;

extern Usart3TypeDef Usart3;

void Usart3Setup(void);

void USART3_printf( char *Data, ... );

#endif



