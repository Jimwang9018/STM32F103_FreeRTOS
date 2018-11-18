#include "stm32f10x.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/** \brief 定时器3初始化
 *
 * \param
 * \param
 * \return
 *
 */
void Tim3Init(u16 period_num)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period            = period_num;            //装载值
    //prescaler is 1200,that is 72000000/72/500=2000Hz;
    TIM_TimeBaseStructure.TIM_Prescaler         = 72-1;                  //分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;          //or TIM_CKD_DIV2 or TIM_CKD_DIV4
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3,TIM_FLAG_Update);


    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;	    	//配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse           = 0;						//设置初始PWM脉冲宽度为0
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_Low;  	    //当定时器计数值小于CCR1_Val时为低电平
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);	 					    //使能通道3
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);				    //使能预装载
    TIM_ARRPreloadConfig(TIM3, ENABLE);			 					    //使能TIM3重载寄存器ARR

    TIM_Cmd(TIM3,ENABLE);
		
}


/** \brief 定时器4初始化
 *
 * \param
 * \param
 * \return
 *
 */
void Tim4Init(u16 period_num)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    TIM_DeInit(TIM4);
    TIM_TimeBaseStructure.TIM_Period            = period_num;               //装载值
    //prescaler is 1200,that is 72000000/72/500=2000Hz;
    TIM_TimeBaseStructure.TIM_Prescaler         = 72-1;                     //分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;             //or TIM_CKD_DIV2 or TIM_CKD_DIV4
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4,TIM_FLAG_Update);

    TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM1;	    	//配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState         = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse               = 0;						//设置初始PWM脉冲宽度为0
    TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_Low;  	    //当定时器计数值小于CCR1_Val时为低电平
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 							//使能通道3
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);						//使能预装载
    
	TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM1;	    	//配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState         = TIM_OutputState_Enable;	//使能输出
    TIM_OCInitStructure.TIM_Pulse               = 0;						//设置初始PWM脉冲宽度为0
    TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_Low;  	    //当定时器计数值小于CCR1_Val时为低电平
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);	 							//使能通道3
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);						//使能预装载
    TIM_ARRPreloadConfig(TIM4, ENABLE);			 							//使能TIM3重载寄存器ARR

    TIM_Cmd(TIM4,ENABLE);

}


