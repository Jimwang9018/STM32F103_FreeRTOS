#ifndef __FLASH_FUNCTION_H
#define	__FLASH_FUNCTION_H
#include "stm32f10x.h"
#include "main.h"
#include <stdbool.h>
#include "esp8266.h"

#define			FLASH_READ_OVER			1
#define 		FLASH_KEEP_READ			0

#define 		brushDataStartAddress  					0x01fe0000  		//存储数据开始地址的地址
#define 		brushDataStartAddressLength					4

#define 		brushDataEndAddress							0x01fe1000			//存储数据结束地址的地址
#define 		brushDataEndAddressLength						4

#define 		gyroOffsetAddress    						0x01fe2000			//存储陀螺仪补偿数据的地址
#define 		gyroOffsetFlagAddress	    			0x01fe2100  		//存储陀螺仪补偿数据标志位的地址

#define		 	wifiNameAddress    							0x01fe3000			//存储WIFI连接信息地址
#define 		wifiNameLengthAddress						0x01fe3100			//存储WIFI连接标识地址

#define			wifiPasswordAddress							0x01fe3200
#define 		wifiPasswordLengthAddress				0x01fe3300

#define 		wifiIPAddress										0x01fe3400
#define			wifiIPLengthAddress							0x01fe3500

#define			wifiPortAddress									0x01fe3600
#define			wifiPortLengthAddress						0x01fe3700

#define			RegisterDataAddress							0x01fe3800

#define			userIDAddress										0x01fe4000

#define 		newOffsetFlag	 	1

//typedef union {
//	struct {
//		float x;
//		float y;
//		float z;
//	}F;
//	struct {
//		uint32_t x;
//		uint32_t y;
//		uint32_t z;
//	}T;
//}gyroOffsetTypeDef;

//typedef union	{
//	int16_t  t16;
//	u8 t8[2];
//}	angleTypeDef;

typedef struct{
	int16_t x;
	int16_t y;
	int16_t z;
} gyroOffsetTypeDef;

typedef struct
{
		uint32_t brushDataStart;
		uint32_t brushDataEnd;
		uint32_t brushDataStartOld;
		uint32_t brushDataEndOld;
		uint32_t userID;
		bool isEaraseFlash;
	
		gyroOffsetTypeDef gyroOffset;
		uint32_t (*getStartAddress)(void);
		void (*setStartAddress)(uint32_t  startAddress);
		uint32_t (*getEndAddress)(void);
		void (*setEndAddress)(uint32_t  endAddress);
		void (*writeData)(S_FLOAT_XYZ  Q_ANGLE);
		u8 (*readData)(void);
		void (*storgeOffsetData)(void);
		void (*storgeLinkData)();
		bool (*readLinkData)();
		void (*readOffsetData)(void);
		void (*setOffsetDataFlag)(void);
		u8 	(*readOffsetDataFlag)(void);
		void (*flashTest)(void);
		void (*eraseDataArea)();
		void (*eraseSendedData)(void);
		u8 	(*readRegisterData)(void);
		void (*saveRegisterData)(void);
		void (*storgeUserID)(void);
		void (*readUserID)(void);
} FlashFunTypeDef;

void FlashFunSetup(void);

extern FlashFunTypeDef FlashFun;

#endif
