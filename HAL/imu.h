#ifndef _IMU_H_
#define _IMU_H_
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "main.h"

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))


int16_t MoveAcc(void);
void Prepare_Data(void);
void Get_Attitude(S_FLOAT_XYZ * Q_ANGLE);
signed portBASE_TYPE xImuPutAngle( S_FLOAT_XYZ Q_ANGLE, portTickType xBlockTime );
void IMUSetup(void);

#endif
