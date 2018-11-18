#ifndef _BLE_BLE_H
#define _BLE_BLE_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "imu.h"
#include <stdbool.h>


#define ON          1
#define OFF         0
#define ACK_ON      1
#define ACK_OFF     0
#define BURSH_ON    1
#define BURSH_OFF   0
#define	BINDING_SUCCESS							2


#define BLE_EN(a) if(a)     GPIO_ResetBits(GPIOB, GPIO_Pin_15);\
                            else  GPIO_SetBits(GPIOB, GPIO_Pin_15);

#define BLE_CLK(a) if(a)   GPIO_ResetBits(GPIOB, GPIO_Pin_14);\
                            else  GPIO_SetBits(GPIOB, GPIO_Pin_14);


/**<功能指令，分别为：空指令、基本控制、WIFI控制、数据传输  */
enum CommandID { noControl, basedControl, wifiControl, dataTransmission, errorData, flashControl};

typedef struct{
    bool isConnect;
    enum CommandID command;
    void (*SendData)(S_FLOAT_XYZ Q_ANGLE);
    void (*SendCmd)(u8 CmdToApp);
    void (*SendAck)(u8 ack);
    bool (*CRCCheck)(u8 * pBuff);
    bool (*APPbuffDataPrepare)(u8 * pBuff);
		void (*SendErrData)(u8 errData);
		bool (*IsLink)(void);
		void (*SendSetWIFIData)(u8 cmd);
}BLETypeDef;

void BLESetup(void);

extern BLETypeDef BLE;


#endif
