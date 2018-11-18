
#include "stm32f10x.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

#define portTICK_RATE_US	 ( ( portTickType ) 1 / configTICK_RATE_HZ )

/***************************************************************************
* @brief   : 毫秒延时
* @note    : 系统启动前不能调用
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
* @brief   : 微秒延时
* @note    : 系统启动前不能调用
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
* @brief   : 毫秒延时
* @note    : 用于系统启动前的延时
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

