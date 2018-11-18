#include "fft.h"
#include "MPU9250.h"
#include "imu.h"
#include "stm32_dsp.h"
#include "math.h"
#include "delay.h"


long lBufInArray[NPT];
long lBufOutArray[NPT];
long lBufMagArray[NPT/2];

u8 fx = 0;

void GetPowerMag()
{
    signed short lX,lY;
    float X,Y,Mag;
    unsigned short i;
    for(i=0; i<NPT/2; i++)
    {
        lX  = (lBufOutArray[i] << 16) >> 16;
        lY  = (lBufOutArray[i] >> 16);
        X = NPT * ((float)lX) / 32768;
        Y = NPT * ((float)lY) / 32768;
        Mag = sqrt(X * X + Y * Y) / NPT;
        if(i == 0)
            lBufMagArray[i] = (unsigned short)(Mag * 32768);
        else
            lBufMagArray[i] = (unsigned short)(Mag * 65536);
    }
}


void fft()
{		
		long max = 0;
		for(u8 i=0; i<NPT-1; i++)
		{
				lBufInArray[i] = lBufInArray[i+1];
		}
		lBufInArray[NPT-1] = MPU6500_GYRO_LAST.Z << 16;
		
		cr4_fft_64_stm32(lBufOutArray, lBufInArray, NPT);
		GetPowerMag();
		
	
		for(u8 i=0; i<NPT/2; i++)
		{
				if(lBufMagArray[i] > 50)
				{
					if(lBufMagArray[i] > max)
					{
							max = lBufMagArray[i];
							fx = i;
					}	
				}
									
		}
		
		max = 0;
}

