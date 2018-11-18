#ifndef _POWER_H
#define _POWER_H

#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "imu.h"
#include "main.h"

#define ON  1
#define OFF 0

#define LOW     1
#define HIGH    0

enum BatterStatus {enough, low, veryLow};
enum ChargeMode {noCharge, charging, chargeOver, noNeedCharge};

typedef struct
{		
		u16	downTime;
		float	value;
		enum BatterStatus batterSatus;
    enum ChargeMode chargeMode;
    bool batterWarn;
    void (*Off)(void);
    void (*On)(void);
		void (*chargeTest)(void);
}POWERTypeDef;

void POWERSetup(void);

extern POWERTypeDef POWER;



#endif
