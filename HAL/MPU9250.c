#include "MPU9250.h"
#include "I2C.h"
#include "delay.h"
#include "flash.h"
#include "w25q256.h"
#include "flashFunction.h"

u8						mpu6500_buffer[14];					//iic读取后存放数据
S_INT16_XYZ		GYRO_OFFSET,ACC_OFFSET;			//零漂
u8						GYRO_OFFSET_OK = 1;
u8						ACC_OFFSET_OK = 1;
S_INT16_XYZ		MPU6500_ACC_LAST,MPU6500_GYRO_LAST;		//最新一次读取值

/**************************实现函数********************************************
//将iic读取到得数据分拆,放入相应寄存器
*******************************************************************************/
void MPU6500_Dataanl(void)
{
		static u8 offsetFlag = 0;
		static u8 fristReadFlag = 0;
	
    MPU6500_ACC_LAST.X=((((int16_t)mpu6500_buffer[0]) << 8) | mpu6500_buffer[1]);
    MPU6500_ACC_LAST.Y=((((int16_t)mpu6500_buffer[2]) << 8) | mpu6500_buffer[3]);
    MPU6500_ACC_LAST.Z=((((int16_t)mpu6500_buffer[4]) << 8) | mpu6500_buffer[5]);
    //跳过温度ADC
    MPU6500_GYRO_LAST.X=((((int16_t)mpu6500_buffer[8]) << 8) | mpu6500_buffer[9]) - FlashFun.gyroOffset.x;	
    MPU6500_GYRO_LAST.Y=((((int16_t)mpu6500_buffer[10]) << 8) | mpu6500_buffer[11]) - FlashFun.gyroOffset.y;	
    MPU6500_GYRO_LAST.Z=((((int16_t)mpu6500_buffer[12]) << 8) | mpu6500_buffer[13]) - FlashFun.gyroOffset.z;
    if(MPU6500_GYRO_LAST.X < 5 && MPU6500_GYRO_LAST.X > -5 ) MPU6500_GYRO_LAST.X =0;
    if(MPU6500_GYRO_LAST.Y < 5 && MPU6500_GYRO_LAST.Y > -5) MPU6500_GYRO_LAST.Y =0;
    if(MPU6500_GYRO_LAST.Z < 5 && MPU6500_GYRO_LAST.Z > -5) MPU6500_GYRO_LAST.Z =0;
		
		if(fristReadFlag == 0)
		{
				offsetFlag = FlashFun.readOffsetDataFlag();
				fristReadFlag = 1;
		}
		
		if(offsetFlag != newOffsetFlag)
		{ 
				static int32_t	tempgx = 0,tempgy = 0,tempgz = 0;
				static uint8_t cnt_g = 0;
				
				if(cnt_g==0)
        {		Flash.SectorErase4K(gyroOffsetAddress);
            FlashFun.gyroOffset.x = 0;
						FlashFun.gyroOffset.y = 0;
						FlashFun.gyroOffset.z = 0;
            tempgx = 0;
            tempgy = 0;
            tempgz = 0;
            cnt_g = 1;
        }
			  tempgx += MPU6500_GYRO_LAST.X;
        tempgy += MPU6500_GYRO_LAST.Y;
        tempgz += MPU6500_GYRO_LAST.Z;
				if(cnt_g>=200)
        {		
            FlashFun.gyroOffset.x = tempgx/cnt_g;
            FlashFun.gyroOffset.y = tempgy/cnt_g;
            FlashFun.gyroOffset.z = tempgz/cnt_g;
            cnt_g = 0;
						offsetFlag = newOffsetFlag;
						Flash.SectorErase4K(gyroOffsetAddress);
            FlashFun.storgeOffsetData();
						FlashFun.setOffsetDataFlag();
            return;
        }
        cnt_g++;	
		}
	
}


/**************************实现函数********************************************
//将iic读取到得数据分拆,放入相应寄存器,更新MPU6500_Last
*******************************************************************************/
void MPU6500_Read(void)
{
    IICreadBytes(devAddr,MPU6500_RA_ACCEL_XOUT_H,14,mpu6500_buffer);
}
/**************************实现函数********************************************
*函数原型:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的1个位
输入	dev  目标设备地址
reg	   寄存器地址
bitNum  要修改目标字节的bitNum位
data  为0 时，目标位将被清0 否则将被置位
返回   成功 为1
失败为0
*******************************************************************************/
void MPU6500_IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
{
    u8 b;
    IICreadBytes(dev, reg, 1, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    IICwriteBytes(dev, reg, 1, &b);
}
/**************************实现函数********************************************
*函数原型:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的多个位
输入	dev  目标设备地址
reg	   寄存器地址
bitStart  目标字节的起始位
length   位长度
data    存放改变目标字节位的值
返回   成功 为1
失败为0
*******************************************************************************/
void MPU6500_IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
{
    u8 b,mask;
    IICreadBytes(dev, reg, 1, &b);
    mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
    data <<= (8 - length);
    data >>= (7 - bitStart);
    b &= mask;
    b |= data;
    IICwriteBytes(dev, reg, 1, &b);
}
/**************************实现函数********************************************
*函数原型:		void MPU6500_setClockSource(uint8_t source)
*功　　能:	    设置  MPU6500 的时钟源
* CLK_SEL | Clock Source
* --------+--------------------------------------
* 0       | Internal oscillator
* 1       | PLL with X Gyro reference
* 2       | PLL with Y Gyro reference
* 3       | PLL with Z Gyro reference
* 4       | PLL with external 32.768kHz reference
* 5       | PLL with external 19.2MHz reference
* 6       | Reserved
* 7       | Stops the clock and keeps the timing generator in reset
*******************************************************************************/
void MPU6500_setClockSource(uint8_t source)
{
    MPU6500_IICwriteBits(devAddr, MPU6500_RA_PWR_MGMT_1, MPU6500_PWR1_CLKSEL_BIT, MPU6500_PWR1_CLKSEL_LENGTH, source);
}
/** Set full-scale gyroscope range.
* @param range New full-scale gyroscope range value
* @see getFullScaleRange()
* @see MPU6500_GYRO_FS_250
* @see MPU6500_RA_GYRO_CONFIG
* @see MPU6500_GCONFIG_FS_SEL_BIT
* @see MPU6500_GCONFIG_FS_SEL_LENGTH
*/
void MPU6500_setFullScaleGyroRange(uint8_t range)
{
    MPU6500_IICwriteBits(devAddr, MPU6500_RA_GYRO_CONFIG, MPU6500_GCONFIG_FS_SEL_BIT, MPU6500_GCONFIG_FS_SEL_LENGTH, range);
}

/**************************实现函数********************************************
*函数原型:		void MPU6500_setFullScaleAccelRange(uint8_t range)
*功　　能:	    设置  MPU6500 加速度计的最大量程
*******************************************************************************/
void MPU6500_setFullScaleAccelRange(uint8_t range)
{
    MPU6500_IICwriteBits(devAddr, MPU6500_RA_ACCEL_CONFIG, MPU6500_ACONFIG_AFS_SEL_BIT, MPU6500_ACONFIG_AFS_SEL_LENGTH, range);
}
/**************************实现函数********************************************
*函数原型:		void MPU6500_setSleepEnabled(uint8_t enabled)
*功　　能:	    设置  MPU6500 是否进入睡眠模式
enabled =1   睡觉
enabled =0   工作
*******************************************************************************/
void MPU6500_setSleepEnabled(uint8_t enabled)
{
    MPU6500_IICwriteBit(devAddr, MPU6500_RA_PWR_MGMT_1, MPU6500_PWR1_SLEEP_BIT, enabled);
}

/**************************实现函数********************************************
*函数原型:		void MPU6500_setI2CMasterModeEnabled(uint8_t enabled)
*功　　能:	    设置 MPU6500 是否为AUX I2C线的主机
*******************************************************************************/
void MPU6500_setI2CMasterModeEnabled(uint8_t enabled) {
    MPU6500_IICwriteBit(devAddr, MPU6500_RA_USER_CTRL, MPU6500_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**************************实现函数********************************************
*函数原型:		void MPU6500_setI2CBypassEnabled(uint8_t enabled)
*功　　能:	    设置 MPU6500 是否为AUX I2C线的主机
*******************************************************************************/
void MPU6500_setI2CBypassEnabled(uint8_t enabled)
{
    MPU6500_IICwriteBit(devAddr, MPU6500_RA_INT_PIN_CFG, MPU6500_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

void MPU6500_setDLPF(uint8_t mode)
{
    MPU6500_IICwriteBits(devAddr, MPU6500_RA_CONFIG, MPU6500_CFG_DLPF_CFG_BIT, MPU6500_CFG_DLPF_CFG_LENGTH, mode);
}

void Delay_ms_mpu(u16 nms)
{
    u8 i;
    for(i=0; i<nms; i++)
    {
        u16 delay_cnt = 50000;
        if(delay_cnt)
            delay_cnt--;
    }
}

/**************************实现函数********************************************
*函数原型:		void MPU6500_initialize(void)
*功　　能:	    初始化 	MPU6500 以进入可用状态。
*******************************************************************************/
void MPU6500_Init(void)
{
    u8 aa = 0x00; //03
    MPU6500_setSleepEnabled(0); //进入工作状态
    Delay_ms_mpu(200);
    MPU6500_setClockSource(MPU6500_CLOCK_PLL_XGYRO); //设置时钟  0x6b   0x01
    Delay_ms_mpu(50);
    IICreadBytes(0xd0,0x1a,1,&aa);//44hz滤波
    Delay_ms_mpu(50);
    MPU6500_setFullScaleGyroRange(MPU6500_GYRO_FS_2000);//陀螺仪最大量程 +-500度每秒
    Delay_ms_mpu(50);
    MPU6500_setFullScaleAccelRange(MPU6500_ACCEL_FS_4);	//加速度度最大量程 +-4G
    Delay_ms_mpu(50);
    MPU6500_setDLPF(MPU6500_DLPF_BW_42);
    Delay_ms_mpu(50);
    MPU6500_setI2CMasterModeEnabled(0);	 //不让MPU6500 控制AUXI2C
    Delay_ms_mpu(50);
    MPU6500_setI2CBypassEnabled(1);	 //主控制器的I2C与	MPU6500的AUXI2C	直通。控制器可以直接访问HMC5883L
    Delay_ms_mpu(50);

}



