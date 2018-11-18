#ifndef _KALMAN_H
#define _KALMAN_H

#include "matrix.h"

#define   LENGTH      1*1
#define   ORDER       1
#define   N           100
#define   SEED        1567

extern volatile unsigned int step;

//================================================//
//==               最优值结构体                 ==//
//================================================//
typedef struct  _tOptimal
{
  float XNowOpt[LENGTH];
  float XPreOpt[LENGTH];
}tOptimal;

void   KalMan_PramInit(void);
float KalMan_Update(float *Z);


void step_count_process(void);

#endif
