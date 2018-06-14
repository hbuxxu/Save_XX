/****************************************************************************
* 文件名: FLASH_SPI.h
* 文件历史:
* 版本号:    V1.0
* 日期:      2017.02.14    
* 作者:     XX
* 说明:   W25Q64存储芯片驱动库
W25Q64一共8M字节容量，
					 分为128个块，每块大小64K
每块又分为16个扇区，每个扇区4K
每次最少可擦除1个扇区，也可擦除1个块，或整个擦除
页编程一次可写256个字节

*****************************************************************************/
/*=============================================================================
*******修改日志*******
1、日    期：
地    点：
修 改 人：
添加内容：
版本号:
2、日    期：
地    点：
修 改 人：
添加内容：
版本号:
================================================================================*/
#ifndef __FLASH_SPI_H
#define __FLASH_SPI_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define SPI_FLASH_SPI                           				SPI3
#define SPI_FLASH_SPI_CLK                       		RCC_APB1Periph_SPI3
#define SPI_FLASH_SPI_SCK_PIN                  GPIO_Pin_3                  /* PB.03 */
#define SPI_FLASH_SPI_SCK_GPIO_PORT    GPIOB                      /* GPIOB */
#define SPI_FLASH_SPI_SCK_GPIO_CLK       RCC_APB2Periph_GPIOB

#define SPI_FLASH_SPI_MISO_PIN                  GPIO_Pin_4                  /* PA.04 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT    GPIOB                       /* GPIOB */
#define SPI_FLASH_SPI_MISO_GPIO_CLK       RCC_APB2Periph_GPIOB

#define SPI_FLASH_SPI_MOSI_PIN                   GPIO_Pin_5                  /* PA.05 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT    GPIOB                       /* GPIOB */
#define SPI_FLASH_SPI_MOSI_GPIO_CLK       RCC_APB2Periph_GPIOB

#define SPI_FLASH_CS_PIN                        			GPIO_Pin_15                  /* PG15 */
#define SPI_FLASH_CS_GPIO_PORT                GPIOG                       /* GPIOG */
#define SPI_FLASH_CS_GPIO_CLK                   RCC_APB2Periph_GPIOG

#define SPI_FLASH_WP_PIN                        			GPIO_Pin_7               
#define SPI_FLASH_WP_GPIO_PORT               GPIOB                      
#define SPI_FLASH_WP_GPIO_CLK                  RCC_APB2Periph_GPIOB

#define SPI_FLASH_HOLD_PIN                      		GPIO_Pin_6               
#define SPI_FLASH_HOLD_GPIO_PORT           GPIOB                      
#define SPI_FLASH_HOLD_GPIO_CLK              RCC_APB2Periph_GPIOB


/* Exported macro ------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low 片选信号 低电平有效 */
#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)

/* Select SPI FLASH: WP pin low   写保护  低电平有效  低电平后无法进行写操作 */
#define SPI_FLASH_WP_LOW()       GPIO_ResetBits(SPI_FLASH_WP_GPIO_PORT, SPI_FLASH_WP_PIN)
/* Deselect SPI FLASH: WP pin high */
#define SPI_FLASH_WP_HIGH()      GPIO_SetBits(SPI_FLASH_WP_GPIO_PORT, SPI_FLASH_WP_PIN)

/* Select SPI FLASH: HOLD pin low  防误操作引脚  低电平有效 忽略一切外部指令 */
#define SPI_FLASH_HOLD_LOW()       GPIO_ResetBits(SPI_FLASH_HOLD_GPIO_PORT, SPI_FLASH_HOLD_PIN)
/* Deselect SPI FLASH: WP pin high */
#define SPI_FLASH_HOLD_HIGH()      GPIO_SetBits(SPI_FLASH_HOLD_GPIO_PORT, SPI_FLASH_HOLD_PIN)


/* Private typedef -----------------------------------------------------------*/
//#define SPI_FLASH_PageSize      4096  核实
#define SPI_FLASH_PageSize      256  									 //每页字节数
#define SPI_FLASH_PerWritePageSize      256       //每页可写字节数

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable								0x06 
#define W25X_WriteDisable							0x04 
#define W25X_ReadStatusReg					0x05 
#define W25X_WriteStatusReg					0x01 
#define W25X_ReadData									0x03 
#define W25X_FastReadData						0x0B 
#define W25X_FastReadDual						0x3B 
#define W25X_PageProgram						0x02 
#define W25X_BlockErase								0xD8 
#define W25X_SectorErase							0x20 
#define W25X_ChipErase									0xC7 
#define W25X_PowerDown								0xB9 
#define W25X_ReleasePowerDown	0xAB   //释放掉电
#define W25X_DeviceID										0xAB   //器件ID
#define W25X_ManufactDeviceID			0x90 
#define W25X_JedecDeviceID					0x9F 

#define WIP_Flag                							0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte              						0xA5  //写数据使用 无实际意义

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
// #define  sFLASH_ID                      0xEF3015
// #define  W25Q16_FLASH_ID        0xEF3015
// #define  W25Q32_FLASH_ID        0xEF3016

#define  W25Q64_FLASH_ID        0xEF4017



/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
/* 外部调用函数 */
extern void SPI_FLASH_Init(void); //W25Q64初始化
extern void SPI_FLASH_SectorErase_A(u32 SectorAddr); //扇区擦除  4K  输入参数0-2047
extern void SPI_FLASH_SectorWrite(u8* pBuffer, u32 SectorAddr, u16 NumByteToWrite);//扇区编程，可写0-4096字节数据 地址位0-2047
extern void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead); //读数据，地址位起始字节地址

void SPI_FLASH_SectorErase(u32 SectorAddr); //扇区擦除  4K 
void SPI_FLASH_BulkErase(void);	//芯片整个擦除
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);  //页编程 最多可写256字节数据
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite); //没看懂  写一个块
u32 SPI_FLASH_ReadID(void);
u32 SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);    //掉电模式
void SPI_Flash_WAKEUP(void);

/*----- Low layer function -----*/
u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);  //写使能
void SPI_FLASH_WriteDisable(void); //写禁止
void SPI_FLASH_WaitForWriteEnd(void);
uint8_t SPI_Flash_Check(void); //器件ID核实

	 
#ifdef __cplusplus
}
#endif
#endif /* __FLASH_SPI_H */
