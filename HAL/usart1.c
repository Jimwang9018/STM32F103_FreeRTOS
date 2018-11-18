#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>

#include "usart1.h"
#include "main.h"

Usart1TypeDef Usart1;

/*****************************************************************************
* @brief   : 串口DMA发送属性配置
* @note    :
* @param   : 串口发送缓存地址
* @retval  : 无
****************************************************************************/
static void DMATxConfig(u8 * pUsart1TxBuff )
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)(&USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (u32)pUsart1TxBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize            = 0;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_ClearFlag(DMA1_FLAG_GL4);
    DMA_Cmd (DMA1_Channel4,DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel                      = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = INTERRUPT_PRIORITY_DMA1_Channel4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
}


/*****************************************************************************
* @brief   : 串口DMA接收属性配置
* @note    :
* @param   : 串口接收缓存地址
* @retval  : 无
****************************************************************************/
static void DMARxConfig(u8 * pUsart1RxBuff )
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (u32)(&USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (u32)pUsart1RxBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize            = Usart1RecevieBuffLength;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal ;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_ClearFlag(DMA1_FLAG_GL5);
    DMA_Cmd (DMA1_Channel5,ENABLE);

}

/*****************************************************************************
* @brief   : DMA发送中断函数
* @note    :
* @param   : 无
* @retval  : 无
****************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
    static signed portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    if(DMA_GetITStatus(DMA1_FLAG_TC4))
    {
        DMA_ClearFlag(DMA1_FLAG_GL4);
        DMA_Cmd(DMA1_Channel4, DISABLE);
        xSemaphoreGiveFromISR( Usart1.xSemaphoreUsart1SendOver, &xHigherPriorityTaskWoken );
    }
}

/*****************************************************************************
* @brief   : 串口接收IDLE中断
* @note    :
* @param   : 无
* @retval  : 无
****************************************************************************/
void USART1_IRQHandler(void)
{
    signed portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)             // 总线空闲中断
    {
        DMA_Cmd(DMA1_Channel5, DISABLE);
        DMA_ClearFlag( DMA1_FLAG_GL5 );
        Usart1.RxBuff.length = Usart1RecevieBuffLength - DMA_GetCurrDataCounter(DMA1_Channel5);
        DMA_SetCurrDataCounter(DMA1_Channel5,Usart1.RxBuff.length);
        DMA_Cmd(DMA1_Channel5, ENABLE);
        USART_ReceiveData( USART1 ); 																	// Clear IDLE interrupt flag bit
        xSemaphoreGiveFromISR( Usart1.xSemaphoreUsart1ReadFull, &xHigherPriorityTaskWoken );
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

     RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );

    /* Configure USART1 Rx (PA10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    /* Configure USART1 Tx (PA9) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    USART_InitStructure.USART_BaudRate              = baud;
    USART_InitStructure.USART_WordLength            = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;
    USART_InitStructure.USART_Parity                = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( USART1, &USART_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel                      = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = INTERRUPT_PRIORITY_USART1_RX;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    USART_Cmd( USART1, ENABLE );
    DMATxConfig(Usart1.TxBuff.data);
    DMARxConfig(Usart1.RxBuff.data);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

    vSemaphoreCreateBinary( Usart1.xSemaphoreUsart1ReadFull );
    vSemaphoreCreateBinary( Usart1.xSemaphoreUsart1SendOver );
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
		pBuff = Usart1.TxBuff.data;
		Usart1.TxBuff.length = 0;
	
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
								Usart1.TxBuff.length ++;								             
                Data ++;
                break;

            case 'n':
                *pBuff++ = 0x0a;
								Usart1.TxBuff.length ++;	              
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
										Usart1.TxBuff.length ++;             
                }
                Data++;
                break;

            case 'd':
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++)
                {
                    *pBuff++ = 	*s;
										Usart1.TxBuff.length ++;                   
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
            Usart1.TxBuff.length ++;
        }
    }

    DMA_SetCurrDataCounter(DMA1_Channel4, Usart1.TxBuff.length);
    DMA_Cmd(DMA1_Channel4, ENABLE);
}


static void sendAngle(S_FLOAT_XYZ Q_ANGLE)
{
    int _temp;
    uint8_t sum;
    Usart1.TxBuff.data[0]=0x88;
    Usart1.TxBuff.data[1]=0xAF;
    Usart1.TxBuff.data[2]=0x1C;
    Usart1.TxBuff.data[3]=0;
    Usart1.TxBuff.data[4]=0;
    Usart1.TxBuff.data[5]=0;
    Usart1.TxBuff.data[6]=0;
    Usart1.TxBuff.data[7]=0;
    Usart1.TxBuff.data[8]=0;
    Usart1.TxBuff.data[9]=0;
    Usart1.TxBuff.data[10]=0;
    Usart1.TxBuff.data[11]=0;
    Usart1.TxBuff.data[12]=0;
    Usart1.TxBuff.data[13]=0;
    Usart1.TxBuff.data[14]=0;
    Usart1.TxBuff.data[15]=0;
    Usart1.TxBuff.data[16]=0;
    Usart1.TxBuff.data[17]=0;
    Usart1.TxBuff.data[18]=0;
    Usart1.TxBuff.data[19]=0;
    Usart1.TxBuff.data[20]=0;
    _temp = (int)(Q_ANGLE.X*100);
    Usart1.TxBuff.data[21]=BYTE1(_temp);
    Usart1.TxBuff.data[22]=BYTE0(_temp);
    _temp = (int)(Q_ANGLE.Y*100);
    Usart1.TxBuff.data[23]=BYTE1(_temp);
    Usart1.TxBuff.data[24]=BYTE0(_temp);
    _temp = (int)(Q_ANGLE.Z*10);
    Usart1.TxBuff.data[25]=BYTE1(_temp);
    Usart1.TxBuff.data[26]=BYTE0(_temp);
    Usart1.TxBuff.data[27]=0;
    Usart1.TxBuff.data[28]=0;
    Usart1.TxBuff.data[29]=0;
    Usart1.TxBuff.data[30]=0;

    sum = 0;
    for(int i=0; i<31; i++)
        sum += Usart1.TxBuff.data[i];
    Usart1.TxBuff.data[31] = sum;

    DMA_SetCurrDataCounter(DMA1_Channel4,32);
    DMA_Cmd(DMA1_Channel4, ENABLE);

//	DMA_Cmd(DMA1_Channel4,DISABLE);
//	DMA_SetCurrDataCounter(DMA1_Channel4,32);
//	DMA_Cmd (DMA1_Channel4,ENABLE);
//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

//	for(int i=0; i<32; i++)
//	{
//		xSerialPutChar( NULL, usart1SendBuff[i], 0 );
//	}

}

/*****************************************************************************
* @brief   : USART2驱动安装
* @note    :
* @param   : 
* @retval  : 
****************************************************************************/
void Usart1Setup(void)
{
		Init(115200);
		Usart1.Printf = Printf;
		Usart1.sendAngle =	sendAngle;
//	 GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_ResetBits(GPIOA, GPIO_Pin_9);
}

int fputc(int ch, FILE *f)
{
	/* 将Printf内容发往串口 */
	USART_SendData(USART1, (unsigned char) ch);
	while (!(USART1->SR & USART_FLAG_TXE));

	return (ch);
}
