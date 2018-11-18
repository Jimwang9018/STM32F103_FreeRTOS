#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include <stdio.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

#include "usart1.h"
#include "delay.h"
#include "led.h"
#include "I2C.h"
#include "MPU9250.h"
#include "imu.h"
#include "tim.h"
#include "ADC.h"
#include "moto.h"
#include "ble.h"
#include "key.h"
#include "power.h"
#include "w25q256.h"
#include "esp8266.h"


POWERTypeDef POWER;

/** \brief 电源管理初始化
 *
 * \param
 * \param
 * \return
 *
 */
static void Init(void)
{
    /**< 开第二个稳压芯片使能IO */
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  	/**< 充电完成测试引脚 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
			/**< 正在充电测试引脚 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
}


static void PVDConfiguration(void)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure EXTI Line16(PVD Output) to generate an interrupt on rising and
     falling edges */
		EXTI_ClearITPendingBit(EXTI_Line16); 
		EXTI_InitStructure.EXTI_Line = EXTI_Line16;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	
		NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
		PWR_PVDLevelConfig(PWR_PVDLevel_2V9);
		PWR_PVDCmd(ENABLE);
}

void PVD_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line16) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line16);
		
		if(POWER.chargeMode == noCharge || POWER.chargeMode == chargeOver)
		{
				POWER.batterSatus = veryLow;
				vTaskSuspendAll();
				POWER.Off();
		}
		
	}
		
}

/** \brief 关闭电源
 *
 * \param
 * \param
 * \return
 *
 */
static void Off(void)
{		
		
		GPIO_ResetBits(GPIOB, GPIO_Pin_15); //EN
		setupDelayMs(50);
		GPIO_SetBits(GPIOB, GPIO_Pin_15);
    //BLE_CLK(OFF);
		Flash.PowerDown();
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    MOTO.SetPWM(0);
    MPU6500_setSleepEnabled(1);
		ESP8266_RST_LOW_LEVEL();
//		offClock();
    setupDelayMs(500);
		
		
//		//NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP,DISABLE);
//    //NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT,DISABLE);
//	
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//		__WFI();
    PWR_WakeUpPinCmd(ENABLE);
    PWR_EnterSTANDBYMode();

}



/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
static void On(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
static void chargeTest(void)
{
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0 && POWER.value <= 4.1)
		POWER.chargeMode = charging;
	else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0)
		POWER.chargeMode = chargeOver;
	else POWER.chargeMode = noCharge;
}

/** \brief
 *
 * \param
 * \param
 * \return
 *
 */
void POWERSetup(void)
{
		Init();
		PVDConfiguration();
		POWER.Off = Off;
		POWER.On = On;
		POWER.chargeTest = chargeTest;
}

