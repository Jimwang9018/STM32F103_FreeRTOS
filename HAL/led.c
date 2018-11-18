#include "stm32f10x.h"
#include "stm32f10x_it.h"

#include "delay.h"
#include "led.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

LEDTypeDef LED;

const uint16_t indexWave[88] = {
    10,  10,  20,  20,  30,  40,  50,  60,  70,  80,
    90, 100, 110, 130, 150, 170, 190, 210, 230, 260,
    300, 320, 350, 390, 440, 500, 570, 650, 740, 840,
    950,1070,1200,1340,1490,1650,1820,2000,2190,2380,
    2420,2460,2520,2550,2550,2520,
    2460,2420,2380,2190,2000,1820,1650,1490,1340,1200,
    1070, 950, 840, 740, 650, 570, 500, 440, 390, 350,
    320, 300, 260, 230, 210, 190, 170, 150, 130, 110,
    100,  90,  80,  70,  60,  50,  40,  30,  20,  20,
    10,  10
};


/***************************************************************************
* @brief   : LED GPIO属性配置
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     =  GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
    GPIO_InitStructure.GPIO_Pin     =  GPIO_Pin_5 ;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    ledBlue(OFF);
}

/***************************************************************************
* @brief   : 绿灯呼吸
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void GreenLedBreath(void)
{
//    static uint8_t pwm_index ;					/**< 用于PWM查表 */

//    TIM3->CCR2 = indexWave[pwm_index];			/**< 根据PWM表修改定时器的比较寄存器值*/
//    if( ++pwm_index >=  88)						/**< 若PWM脉冲表已经输出完成一遍，重置PWM查表标志 */
//    {
//        pwm_index=0;
//    }
//		
		static u8 cont;
    static uint8_t pwm_index ;
    if(++cont == 3)
    {
        cont = 0;
        TIM3->CCR2 = indexWave[pwm_index]/20;
        if( ++pwm_index >=  88)
        {
            pwm_index=0;
        }
    }
}

/***************************************************************************
* @brief   : 绿灯常亮
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void GreenLedOn(void)
{
    TIM3->CCR2 = 600;
		TIM4->CCR4 = 600;
		TIM4->CCR3 = 600;
}

/***************************************************************************
* @brief   : 绿灯熄灭
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void GreenLedOff(void)
{
    TIM3->CCR2 = 0;
}

/***************************************************************************
* @brief   : 红灯呼吸
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void RedLedBreath(void)
{
    static u8 cont;
    static uint8_t pwm_index ;
    if(++cont == 3)
    {
        cont = 0;
        TIM4->CCR4 = indexWave[pwm_index];
        if( ++pwm_index >=  88)
        {
            pwm_index=0;
        }
    }
}

/***************************************************************************
* @brief   : 红灯亮
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void RedLedOn(void)
{
    TIM4->CCR4 = 2550;
		TIM3->CCR2 = 127;
		TIM4->CCR3 = 0;
}

/***************************************************************************
* @brief   : 红灯熄灭
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void RedLedOff(void)
{
    TIM4->CCR4 = 0;
		TIM3->CCR2 = 0;
}


//static void BlueLedBreath(void)
//{
//    static u8 cont;
//    static uint8_t pwm_index ;
//    if(++cont == 3)
//    {
//        cont = 0;
//        TIM4->CCR4 = indexWave[pwm_index];
//        if( ++pwm_index >=  88)
//        {
//            pwm_index=0;
//        }
//    }
//}
/***************************************************************************
* @brief   : 蓝灯闪烁
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void BlueLedBlink(void)
{
		TIM4->CCR4 = 0;
		TIM3->CCR2 = 0;
    if(TIM4->CCR3 == 2550)
        TIM4->CCR3 = 0;
    else
        TIM4->CCR3 = 2550;
}

/***************************************************************************
* @brief   : 蓝灯灭
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void BlueLedOff(void)
{
    TIM4->CCR3 = 0;
}

/***************************************************************************
* @brief   : 
* @note    :
* @param   :
* @retval  :
****************************************************************************/
void LEDSetup(void)
{
		GPIOConfig();
		LED.BlueLedBlink = BlueLedBlink;
		LED.BlueLedOff = BlueLedOff;
		LED.GreenLedBreath = GreenLedBreath;
		LED.GreenLedOff = GreenLedOff;
		LED.GreenLedOn = GreenLedOn;
		LED.RedLedBreath = RedLedBreath;
		LED.RedLedOff = RedLedOff;
		LED.RedLedOn = RedLedOn;
}



