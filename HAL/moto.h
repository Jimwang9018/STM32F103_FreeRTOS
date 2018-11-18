#ifndef _BSP_MOTO_H_
#define _BSP_MOTO_H_
#include "stm32f10x.h"

#define MotoPwmMax 999

typedef struct{
    void (*SetPWM)(int16_t motoPWM);
}MOTOTypeDef;

void MOTOSetup(void);

extern MOTOTypeDef MOTO;

#endif
