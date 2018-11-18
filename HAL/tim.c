#include "stm32f10x.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/** \brief ��ʱ��3��ʼ��
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
    TIM_TimeBaseStructure.TIM_Period            = period_num;            //װ��ֵ
    //prescaler is 1200,that is 72000000/72/500=2000Hz;
    TIM_TimeBaseStructure.TIM_Prescaler         = 72-1;                  //��Ƶϵ��
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;          //or TIM_CKD_DIV2 or TIM_CKD_DIV4
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3,TIM_FLAG_Update);


    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;	    	//����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;	//ʹ�����
    TIM_OCInitStructure.TIM_Pulse           = 0;						//���ó�ʼPWM������Ϊ0
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_Low;  	    //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);	 					    //ʹ��ͨ��3
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);				    //ʹ��Ԥװ��
    TIM_ARRPreloadConfig(TIM3, ENABLE);			 					    //ʹ��TIM3���ؼĴ���ARR

    TIM_Cmd(TIM3,ENABLE);
		
}


/** \brief ��ʱ��4��ʼ��
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
    TIM_TimeBaseStructure.TIM_Period            = period_num;               //װ��ֵ
    //prescaler is 1200,that is 72000000/72/500=2000Hz;
    TIM_TimeBaseStructure.TIM_Prescaler         = 72-1;                     //��Ƶϵ��
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;             //or TIM_CKD_DIV2 or TIM_CKD_DIV4
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4,TIM_FLAG_Update);

    TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM1;	    	//����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState         = TIM_OutputState_Enable;	//ʹ�����
    TIM_OCInitStructure.TIM_Pulse               = 0;						//���ó�ʼPWM������Ϊ0
    TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_Low;  	    //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);	 							//ʹ��ͨ��3
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);						//ʹ��Ԥװ��
    
	TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM1;	    	//����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState         = TIM_OutputState_Enable;	//ʹ�����
    TIM_OCInitStructure.TIM_Pulse               = 0;						//���ó�ʼPWM������Ϊ0
    TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_Low;  	    //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);	 							//ʹ��ͨ��3
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);						//ʹ��Ԥװ��
    TIM_ARRPreloadConfig(TIM4, ENABLE);			 							//ʹ��TIM3���ؼĴ���ARR

    TIM_Cmd(TIM4,ENABLE);

}


