#ifndef _TIM_H_
#define _TIM_H_
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"



void Tim3Init(u16 period_num);
void Tim4Init(u16 period_num);


#endif
