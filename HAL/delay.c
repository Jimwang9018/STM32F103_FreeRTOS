
#include "stm32f10x.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

#define portTICK_RATE_US	 ( ( portTickType ) 1 / configTICK_RATE_HZ )

/***************************************************************************
* @brief   : ������ʱ
* @note    : ϵͳ����ǰ���ܵ���
* @param   :
* @retval  :
****************************************************************************/
void delayMs(u16 nms)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil( &xLastWakeTime, ( nms / portTICK_RATE_MS ) );
}

/***************************************************************************
* @brief   : ΢����ʱ
* @note    : ϵͳ����ǰ���ܵ���
* @param   :
* @retval  :
****************************************************************************/
void delayUs(u32 nus)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil( &xLastWakeTime, ( nus / portTICK_RATE_US ) );
}

/***************************************************************************
* @brief   : ������ʱ
* @note    : ����ϵͳ����ǰ����ʱ
* @param   :
* @retval  :
****************************************************************************/
void setupDelayMs(uint16_t nms)
{
    uint16_t i;
    for(i=0; i<nms; i++)
    {
        uint16_t delay_cnt = 50000;
        if(delay_cnt)
            delay_cnt--;
    }
}

