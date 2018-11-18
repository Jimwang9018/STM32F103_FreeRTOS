#include "stm32f10x.h"
#include "w25q256.h"
#include "main.h"
#include "imu.h"
#include "ADC.h"
#include "flashFunction.h"
#include "esp8266.h"
#include "usart1.h"

FlashFunTypeDef FlashFun;

#define		TestAddress	 0x00000000

static void flashTest(void)
{
		uint8_t address[4] = "haha";
		uint8_t read[4];
		Flash.SectorErase4K(TestAddress); 
		Flash.BufferWrite(address, TestAddress, 4);
		Flash.BufferRead(read, gyroOffsetFlagAddress, 1);
		Usart1.Printf("%d",read[0]);
		
}


/***************************************************************************
* @brief   : 返回存储数据起始地址
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static uint32_t getStartAddress(void)
{
    uint32_t startAddress = 0;
    uint8_t address[4];
		Flash.BufferRead(address, brushDataStartAddress, brushDataStartAddressLength);
    startAddress |= ((address[0])<<24);
    startAddress |= ((address[1])<<16);
    startAddress |= ((address[2])<<8);
    startAddress |= (address[3]);
    return startAddress;
}

/***************************************************************************
* @brief   : 保存起始地址
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void setStartAddress(uint32_t  startAddress)
{
    uint8_t address[4];
    Flash.SectorErase4K(brushDataStartAddress); 
    address[0] = (uint8_t)((startAddress & 0xff000000)>>24);
    address[1] = (uint8_t)((startAddress & 0xff0000)>>16);
    address[2] = (uint8_t)((startAddress & 0xff00)>>8);
    address[3] = (uint8_t)(startAddress & 0xff);
    Flash.BufferWrite(address, brushDataStartAddress, brushDataStartAddressLength);
}

/***************************************************************************
* @brief   : 返回存储数据的结束地址
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static uint32_t getEndAddress(void)
{
    uint32_t endAddress = 0;
    uint8_t address[4];
    Flash.BufferRead(address, brushDataEndAddress, brushDataEndAddressLength); 
    endAddress |= (((uint32_t)address[0])<<24);
    endAddress |= (((uint32_t)address[1])<<16);
    endAddress |= (((uint32_t)address[2])<<8);
    endAddress |= ((uint32_t)address[3]);
    return endAddress;
}

/***************************************************************************
* @brief   : 保存数据存储的结束地址
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void setEndAddress(uint32_t  endAddress)
{
    uint8_t address[4];
    Flash.SectorErase4K(brushDataEndAddress);
    address[0] = (uint8_t)((endAddress & 0xff000000)>>24);
    address[1] = (uint8_t)((endAddress & 0xff0000)>>16);
    address[2] = (uint8_t)((endAddress & 0xff00)>>8);
    address[3] = (uint8_t)(endAddress & 0xff);
    Flash.BufferWrite(address, brushDataEndAddress, brushDataEndAddressLength);
}

/***************************************************************************
* @brief   : 写数据到存储芯片
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void writeData(S_FLOAT_XYZ  Q_ANGLE)
{
    int _temp;
    u8 tempData[8];

    _temp = (int)(Q_ANGLE.X*100);
    tempData[0]	=	BYTE1(_temp);
    tempData[1]	=	BYTE0(_temp);
    _temp = (int)(Q_ANGLE.Y*100);
    tempData[2]	=	BYTE1(_temp);
    tempData[3]	=	BYTE0(_temp);
		_temp = (int)(Q_ANGLE.V);
		tempData[4]  =  BYTE1(_temp);
    tempData[5]  =  BYTE0(_temp);
    _temp = (int)(ADC.convertedValue[0]);
    tempData[6]	=	BYTE1(_temp);
    tempData[7]	=	BYTE0(_temp);

    Flash.BufferWrite(tempData, FlashFun.brushDataEnd, 8);
    FlashFun.brushDataEnd += 8;
}

/***************************************************************************
* @brief   : 上传数据到服务器
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static u8 readData()
{
		static int32_t num = 0;
		u8 * pTemp;

		pTemp = &(Usart3.TxBuff.data[12]);
		num = FlashFun.brushDataEnd - FlashFun.brushDataStart;
	
		if(num <= 1000 && num > 0)
		{
				Flash.BufferRead(pTemp , FlashFun.brushDataStart, num);
//				for(u16 i=0; i<num; i++)
//				{
//						Usart3.TxBuff.data[12+i] = pTemp[i];
//				}
				FlashFun.brushDataStart += num;
				Usart3.TxBuff.length = num;
				return FLASH_READ_OVER;
		}else if(num > 1000)
		{
				Flash.BufferRead(pTemp, FlashFun.brushDataStart, 1000);
//				for(u16 i=0; i<1000; i++)
//				{
//						Usart3.TxBuff.data[12+i] = pTemp[i];
//				}
				FlashFun.brushDataStart += 1000;
				Usart3.TxBuff.length = 1000;
				return FLASH_KEEP_READ;
		}else
		{
				return FLASH_READ_OVER;
		}
 
}

/***************************************************************************
* @brief   : 删除已经发送的数据
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void eraseSendedData(void)
{
		static int32_t num = 0;
		u16 eraseNum = 0;
	
		num = FlashFun.brushDataEndOld - FlashFun.brushDataStartOld;

		if(num > 0)
		{
				eraseNum = num/4096 + 1;
				for(u16 i=0; i<eraseNum; i++)
				{
						Flash.SectorErase4K(FlashFun.brushDataStartOld);
						FlashFun.brushDataStartOld += 4096;
				}
		}
				
		FlashFun.brushDataEnd = 0;
		FlashFun.brushDataEndOld = 0;
		FlashFun.brushDataStart = 0;
		FlashFun.brushDataStartOld = 0;
		FlashFun.setStartAddress(0);
		FlashFun.setEndAddress(0);
}

/***************************************************************************
* @brief   : 保存补偿数据到FLASH
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void storgeOffsetData(void)
{		
		u8 temp[6];
		temp[0] = BYTE1(FlashFun.gyroOffset.x);
		temp[1] = BYTE0(FlashFun.gyroOffset.x);
		temp[2] = BYTE1(FlashFun.gyroOffset.y);
		temp[3] = BYTE0(FlashFun.gyroOffset.y);
		temp[4] = BYTE1(FlashFun.gyroOffset.z);
		temp[5] = BYTE0(FlashFun.gyroOffset.z);
		Flash.BufferWrite(temp, gyroOffsetAddress, 6);
}

/***************************************************************************
* @brief   : 读取补偿数据
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void readOffsetData(void)
{
		u8 temp[6];
    Flash.BufferRead(temp, gyroOffsetAddress, 6); 
		FlashFun.gyroOffset.x |= (((int16_t)temp[0])<<8);
		FlashFun.gyroOffset.x |= ((int16_t)temp[1]);
		FlashFun.gyroOffset.y |= (((int16_t)temp[2])<<8);
		FlashFun.gyroOffset.y |= ((int16_t)temp[3]);
		FlashFun.gyroOffset.z |= (((int16_t)temp[4])<<8);
		FlashFun.gyroOffset.z |= ((int16_t)temp[5]);
}

/***************************************************************************
* @brief   : 设置存储标志位
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void setOffsetDataFlag(void)
{
		u8	gyroOffsetFlag = newOffsetFlag;
		Flash.BufferWrite(&gyroOffsetFlag, gyroOffsetFlagAddress, 1);
}

/***************************************************************************
* @brief   : 读取存储标志位
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static u8 readOffsetDataFlag(void)
{
		u8	gyroOffsetFlag;
		Flash.BufferRead(&gyroOffsetFlag, gyroOffsetFlagAddress, 1);
		return gyroOffsetFlag;
}


/***************************************************************************
* @brief   : 保存用户ID到FLASH
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void storgeUserID(void)
{		
		u8 temp[4];
		Flash.SectorErase4K(userIDAddress);
		temp[0] = BYTE3(FlashFun.userID);
		temp[1] = BYTE2(FlashFun.userID);
		temp[2] = BYTE1(FlashFun.userID);
		temp[3] = BYTE0(FlashFun.userID);
		Flash.BufferWrite(temp, userIDAddress, 4);
}

/***************************************************************************
* @brief   : 读用户ID
* @note    :
* @param   :
* @retval  :
****************************************************************************/
static void readUserID(void)
{
		u8 temp[4];
    Flash.BufferRead(temp, userIDAddress, 4); 
		FlashFun.userID |= ((uint32_t)temp[0])<<24;
		FlashFun.userID |= ((uint32_t)temp[1])<<16;
		FlashFun.userID |= ((uint32_t)temp[2])<<8;
		FlashFun.userID |= ((uint32_t)temp[3]);
}


static void storgeLinkData()
{
		u8 length[2];
		u16 temp;
	
		Flash.SectorErase4K(wifiNameAddress);
	
		temp = WIFI.linkData.nameLength;
		length[1] = BYTE1(temp);
		length[0] = BYTE0(temp);
		Flash.BufferWrite((u8*)WIFI.linkData.name, wifiNameAddress, WIFI.linkData.nameLength);
		Flash.BufferWrite(length, wifiNameLengthAddress,	2);
	
		temp = WIFI.linkData.passwordLength;
		length[1] = BYTE1(temp);
		length[0] = BYTE0(temp);
		Flash.BufferWrite((u8*)WIFI.linkData.password, wifiPasswordAddress, WIFI.linkData.passwordLength);
		Flash.BufferWrite(length, wifiPasswordLengthAddress,	2);
	
		temp = WIFI.linkData.serverIPLength;
		length[1] = BYTE1(temp);
		length[0] = BYTE0(temp);
		Flash.BufferWrite((u8*)WIFI.linkData.serverIP, wifiIPAddress, WIFI.linkData.serverIPLength);
		Flash.BufferWrite(length, wifiIPLengthAddress,	2);
		
		temp = WIFI.linkData.serverPortLength;
		length[1] = BYTE1(temp);
		length[0] = BYTE0(temp);
		Flash.BufferWrite((u8*)WIFI.linkData.serverPort, wifiPortAddress, WIFI.linkData.serverPortLength);
		Flash.BufferWrite(length, wifiPortLengthAddress ,	2);
}

static bool readLinkData()
{
		u8 length[2];
		Flash.BufferRead(length, wifiNameLengthAddress,	2);
		WIFI.linkData.nameLength |= length[1]<<8;
		WIFI.linkData.nameLength |= length[0];
		
		Flash.BufferRead(length, wifiPasswordLengthAddress,	2);
		WIFI.linkData.passwordLength |= length[1]<<8;
		WIFI.linkData.passwordLength |= length[0];
	
		Flash.BufferRead(length, wifiIPLengthAddress,	2);
		WIFI.linkData.serverIPLength |= length[1]<<8;
		WIFI.linkData.serverIPLength |= length[0];
	
		Flash.BufferRead(length, wifiPortLengthAddress,	2);
		WIFI.linkData.serverPortLength |= length[1]<<8;
		WIFI.linkData.serverPortLength |= length[0];
		
		if(WIFI.linkData.nameLength  == 0 || WIFI.linkData.nameLength  == 0xffff || WIFI.linkData.serverIPLength  == 0 || WIFI.linkData.serverPortLength == 0)
		{
				return false;				
		}else
		{
				Flash.BufferRead((u8*)WIFI.linkData.name, wifiNameAddress, WIFI.linkData.nameLength);
				Flash.BufferRead((u8*)WIFI.linkData.password, wifiPasswordAddress, WIFI.linkData.passwordLength);
				Flash.BufferRead((u8*)WIFI.linkData.serverIP, wifiIPAddress, WIFI.linkData.serverIPLength);
				Flash.BufferRead((u8*)WIFI.linkData.serverPort, wifiPortAddress, WIFI.linkData.serverPortLength);
				return true;
		}
}

static void eraseDataArea()
{
		u32 address = 0;
		for(u16 i=0; i<500; i++)
		{
				Flash.BlockErase64K(address);
				address += 64*1024;
		}
		FlashFun.setStartAddress(0);
		FlashFun.setEndAddress(0);
}

static void saveRegisterData(void)
{
		u8 temp = 1;
		Flash.BufferWrite(&temp, RegisterDataAddress,	1);
}

static u8 readRegisterData(void)
{
		u8 temp;
		Flash.BufferRead(&temp, RegisterDataAddress, 1);
		return temp;
}

/***************************************************************************
* @brief   : 初始化程序
* @note    :
* @param   :
* @retval  :
****************************************************************************/
void FlashFunSetup(void)
{
	FlashFun.getEndAddress 			= getEndAddress;
	FlashFun.getStartAddress 		= getStartAddress;
	FlashFun.readOffsetData 		= readOffsetData;
	FlashFun.readOffsetDataFlag = readOffsetDataFlag;
	FlashFun.setEndAddress 			= setEndAddress;
	FlashFun.setOffsetDataFlag 	= setOffsetDataFlag;
	FlashFun.setStartAddress 		= setStartAddress;
	FlashFun.storgeOffsetData 	= storgeOffsetData;
	FlashFun.readData 					= readData;
	FlashFun.writeData 					= writeData;
	FlashFun.storgeLinkData 		= storgeLinkData;
	FlashFun.flashTest 					= flashTest;
	FlashFun.readLinkData				= readLinkData;
	FlashFun.brushDataStart			= getStartAddress();
	FlashFun.brushDataStartOld  = FlashFun.brushDataStart;
	FlashFun.brushDataEnd				= getEndAddress();
	FlashFun.brushDataEndOld		= FlashFun.brushDataEnd;
	FlashFun.eraseDataArea 			= eraseDataArea;
	FlashFun.eraseSendedData		= eraseSendedData;
	FlashFun.saveRegisterData		= saveRegisterData;
	FlashFun.readRegisterData		= readRegisterData;
	FlashFun.storgeUserID       = storgeUserID;
	FlashFun.readUserID					= readUserID;
	FlashFun.userID = 0;
}
