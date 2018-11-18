#include "moto.h"

MOTOTypeDef MOTO;

/***************************************************************************
* @brief   : 电机更新PWM值
* @note    :
* @param   : PWM值
* @retval  :
****************************************************************************/
static void SetPWM(int16_t motoPWM)
{
    if(motoPWM>MotoPwmMax)
        motoPWM = MotoPwmMax;

    if(motoPWM<0)
        motoPWM = 0;

    TIM2->CCR2 = motoPWM;
}

/***************************************************************************
* @brief   : 定时器2初始化
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void Tim2Init(void)
{
    TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  		    TIM_OCInitStructure;
    uint16_t PrescalerValue = 0;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    /* Compute the prescaler value */
    PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period        = 999;
    TIM_TimeBaseStructure.TIM_Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel */
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = 0;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;

    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

/***************************************************************************
* @brief   : MOTO GPIO属性配置
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_1 ;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    Tim2Init();
}

void MOTOSetup(void)
{
		Init();
		MOTO.SetPWM = SetPWM;
}

