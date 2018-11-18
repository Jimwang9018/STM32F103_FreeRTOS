#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
#include "led.h"
#include "main.h"
#include <stdbool.h>

KEYTypeDef KEY;

/** \brief GPIO属性配置
 *
 * \param
 * \param
 * \return
 *
 */
static void GPIOConfig(void)
{
    //EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
   // NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INTERRUPT_PRIORITY_EXIT;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

}

/** \brief 按键外部中断
 *
 * \param
 * \param
 * \return
 *
 */
//void EXTI0_IRQHandler(void)
//{		
//		signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
//		EXTI_ClearITPendingBit(EXTI_Line0);
//    xSemaphoreGiveFromISR( KEY.xSemaphoreKeyOn, &xHigherPriorityTaskWoken );
//}

/** \brief 按键初始化
 *
 * \param
 * \param
 * \return
 *
 */
static void Init(void)
{
    GPIOConfig();
    //vSemaphoreCreateBinary( KEY.xSemaphoreKeyOn );
}


void KEYSetup(void)
{
		Init();
}

