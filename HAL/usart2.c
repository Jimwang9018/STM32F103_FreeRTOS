 #include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

#include "ble.h"
#include "usart2.h"
#include "main.h"

Usart2TypeDef Usart2;

/*****************************************************************************
* @brief   : 串口DMA发送属性配置
* @note    :
* @param   : 串口发送缓存地址
* @retval  : 无
****************************************************************************/
static void DMATxConfig(u8 * pUsart2TxBuff )
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)(&USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (u32)pUsart2TxBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize            = 0;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal ;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);

    DMA_ClearFlag(DMA1_FLAG_GL7);
    DMA_Cmd (DMA1_Channel7,DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel                      = DMA1_Channel7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = INTERRUPT_PRIORITY_DMA1_Channel7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
}

/*****************************************************************************
* @brief   : 串口DMA接收属性配置
* @note    :
* @param   : 串口接收缓存地址
* @retval  : 无
****************************************************************************/
static void DMARxConfig(u8 * pUsart2RxBuff )
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Channel6, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)(&USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (u32)pUsart2RxBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize            = Usart2RecevieBuffLength;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal ;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
    DMA_ClearFlag(DMA1_FLAG_GL6);
    DMA_Cmd (DMA1_Channel6,ENABLE);

}

/*****************************************************************************
* @brief   : DMA发送中断函数
* @note    :
* @param   : 无
* @retval  : 无
****************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
    static signed portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    if(DMA_GetITStatus(DMA1_FLAG_TC7))
    {
        DMA_ClearFlag(DMA1_FLAG_GL7);
        DMA_Cmd(DMA1_Channel7, DISABLE);
        xSemaphoreGiveFromISR( Usart2.xSemaphoreUsart2SendOver, &xHigherPriorityTaskWoken );
    }
}


/*****************************************************************************
* @brief   : 串口接收IDLE中断
* @note    :
* @param   : 无
* @retval  : 无
****************************************************************************/
void USART2_IRQHandler(void)
{
    signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)                        // 总线空闲中断
    {
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA_ClearFlag( DMA1_FLAG_GL6 );
        Usart2.RxBuff.length = Usart2RecevieBuffLength - DMA_GetCurrDataCounter(DMA1_Channel6);
				//Usart2.RxBuff.length ++;
				//Usart2.RxBuff.data[Usart2.RxBuff.length] = '\0';
        DMA_SetCurrDataCounter(DMA1_Channel6,Usart2RecevieBuffLength);
        DMA_Cmd(DMA1_Channel6, ENABLE);
        USART_ReceiveData( USART2 ); 																	// Clear IDLE interrupt flag bit
        xSemaphoreGiveFromISR( Usart2.xSemaphoreUsart2ReadFull, &xHigherPriorityTaskWoken );
    }
}

/*****************************************************************************
* @brief   : 串口2初始化
* @note    :
* @param   : 波特率
* @retval  : 无
****************************************************************************/
static void Init( unsigned long baud)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    USART_InitStructure.USART_BaudRate              = baud;
    USART_InitStructure.USART_WordLength            = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;
    USART_InitStructure.USART_Parity                = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( USART2, &USART_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel                      = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = INTERRUPT_PRIORITY_USART2_RX;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

    USART_Cmd( USART2, ENABLE );
    DMATxConfig(Usart2.TxBuff.data);
    DMARxConfig(Usart2.RxBuff.data);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    vSemaphoreCreateBinary( Usart2.xSemaphoreUsart2ReadFull );
    vSemaphoreCreateBinary( Usart2.xSemaphoreUsart2SendOver );
}


/** \brief
 *  ????
 * \param
 * \param
 * \return
 *
 */
static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */



static void Printf( char *Data, ... )
{
    const char *s;
    int d;
    char buf[16];
    u8 * pBuff;
		pBuff = Usart2.TxBuff.data;
		Usart2.TxBuff.length = 0;
	
    va_list ap;
    va_start(ap, Data);

    while ( *Data != 0)
    {
        if ( *Data == 0x5c )  //'\'
        {
            switch ( *++Data )
            {
            case 'r':
                *pBuff++ = 0x0d;
				Usart2.TxBuff.length ++;								             
                Data ++;
                break;

            case 'n':
                *pBuff++ = 0x0a;
				Usart2.TxBuff.length ++;	              
                Data ++;
                break;

            default:
                Data ++;
                break;
            }
        }
        else if ( *Data == '%')
        {
            switch ( *++Data )
            {
            case 's':
                s = va_arg(ap, const char *);
                for ( ; *s; s++)
                {
                    *pBuff++ = 	*s;
					Usart2.TxBuff.length ++;             
                }
                Data++;
                break;

            case 'd':
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    *pBuff++ = 	*s;
					Usart2.TxBuff.length ++;                   
                }
                Data++;
                break;
            default:
                Data++;
                break;
            }
        } /* end of else if */
        else
        {
            *pBuff++ = 	*Data++;
            Usart2.TxBuff.length ++;
        }
    }
		
	if(Usart2.TxBuff.length >= Usart2SendBuffLength)
		Usart2.TxBuff.length = Usart2SendBuffLength;

    if(pdTRUE == xSemaphoreTake( Usart2.xSemaphoreUsart2SendOver, ( portTickType ) portMAX_DELAY ))
    {
        DMA_SetCurrDataCounter(DMA1_Channel7,Usart2.TxBuff.length);
        DMA_Cmd(DMA1_Channel7, ENABLE);
    }
}

/*****************************************************************************
* @brief   : USART2驱动安装
* @note    :
* @param   : 
* @retval  : 
****************************************************************************/
void Usart2Setup(void)
{
	Init(115200);
	Usart2.Printf = Printf;
}


