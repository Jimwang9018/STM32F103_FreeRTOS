#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f10x.h"

//#define SPI_FLASH_PageSize      4096
#define     SPI_FLASH_PageSize              256
#define     SPI_FLASH_PerWritePageSize      256


#define     W25X_WriteEnable		        0x06
#define     W25X_WriteDisable		        0x04
#define     W25X_ReadStatusReg		        0x05
#define     W25X_WriteStatusReg		        0x01
#define     W25X_ReadData			        0x13
//#define W25X_ReadDataWith4ByteAddress			     0x13
#define     W25X_FastReadData		        0x0c
#define     W25X_FastReadDual		        0x3B
#define     W25X_PageProgram		        0x02
#define     W25X_BlockErase			        0xD8
#define     W25X_SectorErase		        0x20
#define     W25X_ChipErase			        0x60
#define     W25X_PowerDown			        0xB9
#define     W25X_ReleasePowerDown	        0xAB
#define     W25X_DeviceID			        0xAB
#define     W25X_ManufactDeviceID   	    0x90
#define     W25X_JedecDeviceID		        0x9F
#define     W25X_enter4ByteAddress          0xB7

#define     WIP_Flag                        0x01  /* Write In Progress (WIP) flag */

#define     Dummy_Byte                      0xFF

#define     SPI_FLASH_SPI                   SPI1
#define     SPI_FLASH_SPI_CLK               RCC_APB2Periph_SPI1
#define     SPI_FLASH_SPI_SCK_PIN           GPIO_Pin_5                  /* PA.05 */
#define     SPI_FLASH_SPI_SCK_GPIO_PORT     GPIOA                       /* GPIOA */
#define     SPI_FLASH_SPI_SCK_GPIO_CLK      RCC_APB2Periph_GPIOA
#define     SPI_FLASH_SPI_MISO_PIN          GPIO_Pin_6                  /* PA.06 */
#define     SPI_FLASH_SPI_MISO_GPIO_PORT    GPIOA                       /* GPIOA */
#define     SPI_FLASH_SPI_MISO_GPIO_CLK     RCC_APB2Periph_GPIOA
#define     SPI_FLASH_SPI_MOSI_PIN          GPIO_Pin_7                  /* PA.07 */
#define     SPI_FLASH_SPI_MOSI_GPIO_PORT    GPIOA                       /* GPIOA */
#define     SPI_FLASH_SPI_MOSI_GPIO_CLK     RCC_APB2Periph_GPIOA
#define     SPI_FLASH_CS_PIN                GPIO_Pin_4                  /* PC.04 */
#define     SPI_FLASH_CS_GPIO_PORT          GPIOA                       /* GPIOC */
#define     SPI_FLASH_CS_GPIO_CLK           RCC_APB2Periph_GPIOA

#define     SPI_FLASH_CS_LOW()              GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define     SPI_FLASH_CS_HIGH()             GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define     countof(a)                      (sizeof(a) / sizeof(*(a)))

typedef struct{
   void (*SectorErase4K)(u32 SectorAddr);
   void (*BlockErase32K)(u32 SectorAddr);
   void (*BlockErase64K)(u32 SectorAddr);
   void (*ChipErase)(void);
   void (*PageWrite)(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
   void (*BufferWrite)(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
   void (*BufferRead)(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
   u32  (*ReadID)(void);
   u32  (*ReadDeviceID)(void);
   void (*StartReadSequence)(u32 ReadAddr);
   u16  (*SendHalfWord)(u16 HalfWord);
   void (*PowerDown)(void);
   void (*WAKEUP)(void);
   void (*Enter4byteAddressMode)(void);
}FlashTypeDef;

void FLASHSetup(void);

extern FlashTypeDef Flash;

#endif /* __SPI_FLASH_H */

