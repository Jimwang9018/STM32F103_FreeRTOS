#ifndef _IMU_TASK_H
#define _IMU_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

extern xQueueHandle xAngleBuffSend;
extern xQueueHandle xAngleBuffStorage;

void vIMUTask(void *pvParameters);

#endif
