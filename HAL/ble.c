/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */
#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>

/* Demo application includes. */
#include "ble.h"
#include "imu.h"
#include "led.h"
#include "ADC.h"
#include "usart2.h"
#include "delay.h"
#include "fft.h"

BLETypeDef BLE;

/***************************************************************************
* @brief   : 蓝牙相关引脚初始化
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_15;       //EN
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB, GPIO_Pin_15);
	
		GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_15;       //SWDIO
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		


		GPIO_SetBits(GPIOB, GPIO_Pin_15);
		setupDelayMs(50);
		GPIO_ResetBits(GPIOB, GPIO_Pin_15); //EN
	
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);

}

static bool IsLink(void)
{
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
				return true;
		else 
				return false;
}

/***************************************************************************
* @brief   :  蓝牙发送角度数据
* @note    :
* @param   :  Q_ANGLE陀螺仪计算出来的角度
* @retval  :
****************************************************************************/
static void SendData(S_FLOAT_XYZ Q_ANGLE)
{
    int _temp;
    u8 sum;

    Usart2.TxBuff.data[0]   =  0x88;
    Usart2.TxBuff.data[1]   =  0x01;
    Usart2.TxBuff.data[2]   =  0x0D;
    Usart2.TxBuff.data[4]   =  0x31;
    _temp = (int)(Q_ANGLE.X*100);
    Usart2.TxBuff.data[5]   =  BYTE1(_temp);
    Usart2.TxBuff.data[6]   =  BYTE0(_temp);
    _temp = (int)(Q_ANGLE.Y*100);
    Usart2.TxBuff.data[7]   =  BYTE1(_temp);
    Usart2.TxBuff.data[8]   =  BYTE0(_temp);
	
		_temp = (int)(fx);
		Usart2.TxBuff.data[9]   =  BYTE1(_temp);
    Usart2.TxBuff.data[10]  =  BYTE0(_temp);
		fx = 0;
    _temp = (int)(ADC.convertedValue[0]);
    Usart2.TxBuff.data[11]   =  BYTE1(_temp);
    Usart2.TxBuff.data[12]  =  BYTE0(_temp);
	
    sum  = 0;
    sum += Usart2.TxBuff.data[0];
    sum += Usart2.TxBuff.data[1];
    sum += Usart2.TxBuff.data[2];
    for(int i=4; i<13; i++)
    {
        sum += Usart2.TxBuff.data[i];
    }
    Usart2.TxBuff.data[3] = sum;

    DMA_SetCurrDataCounter(DMA1_Channel7,Usart2.TxBuff.data[2]);
    DMA_Cmd(DMA1_Channel7, ENABLE);

}

/***************************************************************************
* @brief   :  牙刷发送指令到手机
* @note    :
* @param   :  CmdToApp待发送的指令
* @retval  :
****************************************************************************/
const u8 bleCmdBrushOn[6]    = {0x88,0x01,0x06,0xA1,0x11,0x01};      /**< 牙刷发送到手机的指令 */
const u8 bleCmdBrushOff[6]   = {0x88,0x01,0x06,0xA2,0x11,0x02};      /**< 牙刷发送到手机的指令 */
const u8 bleCmdBindingSuccess[6]   = {0x88,0x01,0x06,0xA3,0x11,0x03};      /**< 牙刷发送到手机的指令 */
const u8 bleCmdReceiveAPNameSuccess[6]   = {0x88,0x01,0x06,0xC6,0x27,0x10};      /**< 牙刷发送到手机的指令 */
const u8 bleCmdReceiveAPPasswordSuccess[6]   = {0x88,0x01,0x06,0xC7,0x27,0x11};      /**< 牙刷发送到手机的指令 */


static void SendCmd(u8 CmdToApp)
{
		switch(CmdToApp)
		{
			case BURSH_ON : {
					for(u8 i=0; i<6; i++)
					{
							Usart2.TxBuff.data[i] = bleCmdBrushOn[i];
					}
			}break;
			
			case BURSH_OFF : {
					for(u8 i=0; i<6; i++)
					{
							Usart2.TxBuff.data[i] = bleCmdBrushOff[i];
					}
			}break;
			
			case BINDING_SUCCESS : {
					for(u8 i=0; i<6; i++)
					{
							Usart2.TxBuff.data[i] = bleCmdBindingSuccess[i];
					}
			}break;
			
			default : break;
		}
		
		 DMA_SetCurrDataCounter(DMA1_Channel7,6);
     DMA_Cmd(DMA1_Channel7, ENABLE);

}

/***************************************************************************
* @brief   :  牙刷发送反馈指令到手机
* @note    :
* @param   :  CmdToApp待发送的指令
* @retval  :
****************************************************************************/
const u8 bleAppAckCmdBrushOn[6]     = {0x88,0x11,0x06,0xB1,0x11,0x01};      /**< 手机发送过来的指令 */
const u8 bleAppAckCmdBrushOff[6]    = {0x88,0x11,0x06,0xB2,0x11,0x02};      /**< 手机发送过来的指令 */

static void SendAck(u8 ack)
{
    if(ack == ACK_ON)
    {
        for(u8 i=0; i<6; i++)
        {
             Usart2.TxBuff.data[i] = bleAppAckCmdBrushOn[i];
        }
        DMA_SetCurrDataCounter(DMA1_Channel7,bleAppAckCmdBrushOn[2]);
        DMA_Cmd(DMA1_Channel7, ENABLE);
    }
    else if(ack == ACK_OFF)
    {
        for(u8 i=0; i<6; i++)
        {
             Usart2.TxBuff.data[i] = bleAppAckCmdBrushOff[i];
        }
        DMA_SetCurrDataCounter(DMA1_Channel7,bleAppAckCmdBrushOff[2]);
        DMA_Cmd(DMA1_Channel7, ENABLE);
    }

}

/***************************************************************************
* @brief   :  CRC校验函数
* @note    :
* @param   :  pBuff待校验的数据地址
* @retval  :  成功返回1，错误返回0
****************************************************************************/
static bool CRCCheck(u8 * pBuff)
{
    u8 sum = 0;
    sum += pBuff[0];
    sum += pBuff[1];
    sum += pBuff[2];

    for(u8 i = 4; i < pBuff[2] ; i ++)
    {
        sum += pBuff[i];
    }

    if(sum == pBuff[3])
    {
        return true;
    }
    else
    {
        return false;
    }
}

/***************************************************************************
* @brief   :  蓝牙接收数据预处理
* @note    :
* @param   :
* @retval  :
****************************************************************************/
const u8 commandAck[] = {"TTM:OK\r\n\0"}; /**< 蓝牙模块连接成功后返回的数据 */

static bool APPbuffDataPrepare(u8 * pBuff)
{
    if(pBuff[0] == 0x88)
    {
        if(CRCCheck(pBuff) == 0)
				{
            return false;
				}
					
        if((pBuff[4] & 0xF0) == 0x10 )
        {
            BLE.command = basedControl;
        }
        else if((pBuff[4] & 0xF0) == 0x20)
        {
            BLE.command = wifiControl;
        }
        else if((pBuff[4] & 0xF0) == 0x30)
        {
            BLE.command = dataTransmission;
        }
        else if((pBuff[4] & 0xF0) == 0x50)
        {
            BLE.command = flashControl;
        }
				else
				{
						BLE.command = noControl;
				}
        return true;
    }
    return false;
}


static void SendErrData(u8 errData)
{	
    u8 sum;

    Usart2.TxBuff.data[0]   =  0x88;
    Usart2.TxBuff.data[1]   =  0x01;
    Usart2.TxBuff.data[2]   =  0x06;
    Usart2.TxBuff.data[4]   =  0x41;
		Usart2.TxBuff.data[5]   =  errData;

    sum  = 0;
    sum += Usart2.TxBuff.data[0];
    sum += Usart2.TxBuff.data[1];
    sum += Usart2.TxBuff.data[2];
		sum += Usart2.TxBuff.data[4];
		sum += Usart2.TxBuff.data[5];
    Usart2.TxBuff.data[3] = sum;

    DMA_SetCurrDataCounter(DMA1_Channel7,Usart2.TxBuff.data[2]);
    DMA_Cmd(DMA1_Channel7, ENABLE);

}

static void SendSetWIFIData(u8 cmd)
{
		u8 sum;

    Usart2.TxBuff.data[0]   =  0x88;
    Usart2.TxBuff.data[1]   =  0x01;
    Usart2.TxBuff.data[2]   =  0x06;
    Usart2.TxBuff.data[4]   =  0x27;
		Usart2.TxBuff.data[5]   =  cmd;

    sum  = 0;
    sum += Usart2.TxBuff.data[0];
    sum += Usart2.TxBuff.data[1];
    sum += Usart2.TxBuff.data[2];
		sum += Usart2.TxBuff.data[4];
		sum += Usart2.TxBuff.data[5];
    Usart2.TxBuff.data[3] = sum;

    DMA_SetCurrDataCounter(DMA1_Channel7,Usart2.TxBuff.data[2]);
    DMA_Cmd(DMA1_Channel7, ENABLE);
}


/***************************************************************************
* @brief   :  安装
* @note    :
* @param   :
* @retval  :
****************************************************************************/
void BLESetup(void)
{
		Init();
		BLE.SendData = SendData;
		BLE.SendCmd = SendCmd;
		BLE.SendAck = SendAck;
		BLE.CRCCheck = CRCCheck;
		BLE.APPbuffDataPrepare = APPbuffDataPrepare;
		BLE.SendErrData = SendErrData;
		BLE.IsLink = IsLink;
		BLE.SendSetWIFIData = SendSetWIFIData;
}


