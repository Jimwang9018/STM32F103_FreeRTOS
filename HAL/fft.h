#ifndef _FFT_H_
#define _FFT_H_
#include "stm32f10x.h"

#define NPT 64
#define PI2 6.28318530717959
#define Fs 40

void fft(void);

extern long lBufMagArray[NPT/2];
extern long max;
extern u8 fx;
#endif
