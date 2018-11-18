#ifndef __ADC_H
#define	__ADC_H

#include "stm32f10x.h"

typedef struct{
    __IO u16 convertedValue[2];
    void (*Init)(void);
}ADCTypeDef;

void ADCSetup(void);

extern ADCTypeDef ADC;


#endif /* __ADC_H */

