#include "imu.h"
#include "MPU9250.h"
#include "math.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "FlashFunction.h"

#define RtA 		57.324841				//弧度到角度
#define AtR    	0.0174533				//度到角度
#define Acc_G 	0.0011963				//加速度变成G
#define Gyro_G 	0.0609756				//角速度变成度250：0.0152672//改
#define Gyro_Gr	0.0010653				//	0.0002663
#define FILTER_NUM 30
#define MOVE_FILTER_NUM 3

S_INT16_XYZ ACC_AVG;			//平均值滤波后的ACC


/*-----------------------------------------------------------*/
int16_t	ACC_X_BUF[FILTER_NUM],ACC_Y_BUF[FILTER_NUM],ACC_Z_BUF[FILTER_NUM];	//加速度滑动窗口滤波数组

void Prepare_Data(void)
{
    static uint8_t filter_cnt=0;
    int32_t temp1=0,temp2=0,temp3=0;
    uint8_t i;

    MPU6500_Read();
    MPU6500_Dataanl();

    ACC_X_BUF[filter_cnt] = MPU6500_ACC_LAST.X;//更新滑动窗口数组
    ACC_Y_BUF[filter_cnt] = MPU6500_ACC_LAST.Y;
    ACC_Z_BUF[filter_cnt] = MPU6500_ACC_LAST.Z;
    for(i=0; i<FILTER_NUM; i++)
    {
        temp1 += ACC_X_BUF[i];
        temp2 += ACC_Y_BUF[i];
        temp3 += ACC_Z_BUF[i];
    }
    ACC_AVG.X = temp1 / FILTER_NUM;
    ACC_AVG.Y = temp2 / FILTER_NUM;
    ACC_AVG.Z = temp3 / FILTER_NUM;
    filter_cnt++;
    if(filter_cnt==FILTER_NUM)	filter_cnt=0;

}



/*-----------------------------------------------------------*/

#define Kp 10.0f                        // proportional gain governs rate of convergence to accelerometer/magnetometer 10
#define Ki 0.001f                          // integral gain governs rate of convergence of gyroscope biases 0.001
#define halfT 0.005f                   // half the sample period采样周期的一半

float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    // quaternion elements representing the estimated orientation
float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error

void Get_Attitude(S_FLOAT_XYZ * pANGLE)
{
    float gx,gy,gz,ax,ay,az;
    gx = MPU6500_GYRO_LAST.X*Gyro_Gr;
    gy = MPU6500_GYRO_LAST.Y*Gyro_Gr;
    gz = MPU6500_GYRO_LAST.Z*Gyro_Gr;
    ax = ACC_AVG.X;
    ay = ACC_AVG.Y;
    az = ACC_AVG.Z;

    float norm;
    //float hx, hy, hz, bx, bz;
    float vx, vy, vz;// wx, wy, wz;
    float ex, ey, ez;

    // 先把这些用得到的值算好
    float q0q0 = q0*q0;
    float q0q1 = q0*q1;
    float q0q2 = q0*q2;
//  float q0q3 = q0*q3;
    float q1q1 = q1*q1;
//  float q1q2 = q1*q2;
    float q1q3 = q1*q3;
    float q2q2 = q2*q2;
    float q2q3 = q2*q3;
    float q3q3 = q3*q3;

    if(ax*ay*az==0)
        return ;

    norm = sqrt(ax*ax + ay*ay + az*az);       //acc数据归一化
    ax = ax /norm;
    ay = ay / norm;
    az = az / norm;

    // estimated direction of gravity and flux (v and w)              估计重力方向和流量/变迁
    vx = 2*(q1q3 - q0q2);												//四元素中xyz的表示
    vy = 2*(q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3 ;

    // error is sum of cross product between reference direction of fields and direction measured by sensors
    ex = (ay*vz - az*vy) ;                           					 //向量外积在相减得到差分就是误差
    ey = (az*vx - ax*vz) ;
    ez = (ax*vy - ay*vx) ;

    exInt = exInt + ex * Ki;								  //对误差进行积分
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    // adjusted gyroscope measurements
    gx = gx + Kp*ex + exInt;					   							//将误差PI后补偿到陀螺仪，即补偿零点漂移
    gy = gy + Kp*ey + eyInt;
    gz = gz + Kp*ez + ezInt;				   							//这里的gz由于没有观测者进行矫正会产生漂移，表现出来的就是积分自增或自减

    // integrate quaternion rate and normalise						   //四元素的微分方程
    q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
    q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
    q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
    q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

    // normalise quaternion
    norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;

 //(*pANGLE).Z = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2*q2 - 2 * q3* q3 + 1)* 57.3; // yaw
    (*pANGLE).Y  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch
    (*pANGLE).X = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll

}

/*-----------------------------------------------------------*/

void IMUSetup()
{
		MPU6500_Init();
		FlashFun.readOffsetData();
}


