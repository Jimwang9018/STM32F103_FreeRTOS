#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

#define ON	1
#define OFF	0

#define ledBlue(a)	if (a)	\
					GPIO_ResetBits(GPIOB,GPIO_Pin_8);\
					else		\
					GPIO_SetBits(GPIOB,GPIO_Pin_8)

typedef struct{
    void (*GreenLedBreath)(void);
    void (*GreenLedOn)(void);
    void (*GreenLedOff)(void);
    void (*RedLedBreath)(void);
    void (*RedLedOn)(void);
    void (*RedLedOff)(void);
    void (*BlueLedBlink)(void);
    void (*BlueLedOff)(void);
}LEDTypeDef;

void LEDSetup(void);

extern LEDTypeDef LED;


#endif /* __LED_H */
