#ifndef USART1_COMMS_H
#define USART1_COMMS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "imu.h"
#include "ble.h"

#define Usart1RecevieBuffLength 32
#define Usart1SendBuffLength    32

typedef struct
{
    u8 data[Usart1SendBuffLength];
    u8 length;
} Usart1TxBuffTypeDef;

typedef struct
{
    u8 data[Usart1RecevieBuffLength];
    u8 length;
} Usart1RxBuffTypeDef;

typedef struct
{
    Usart1TxBuffTypeDef TxBuff;
    Usart1RxBuffTypeDef RxBuff;
    xSemaphoreHandle xSemaphoreUsart1ReadFull;
    xSemaphoreHandle xSemaphoreUsart1SendOver;
    void (*Init)( unsigned long baud);
		void (*Printf)( char *Data, ... );
		void (*sendAngle)(S_FLOAT_XYZ Q_ANGLE);
} Usart1TypeDef;

void Usart1Setup(void);

extern Usart1TypeDef Usart1;

#endif


