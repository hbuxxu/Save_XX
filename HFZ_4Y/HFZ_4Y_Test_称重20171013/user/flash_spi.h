/****************************************************************************
* �ļ���: FLASH_SPI.h
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2017.02.14    
* ����:     XX
* ˵��:   W25Q64�洢оƬ������
W25Q64һ��8M�ֽ�������
					 ��Ϊ128���飬ÿ���С64K
ÿ���ַ�Ϊ16��������ÿ������4K
ÿ�����ٿɲ���1��������Ҳ�ɲ���1���飬����������
ҳ���һ�ο�д256���ֽ�

*****************************************************************************/
/*=============================================================================
*******�޸���־*******
1����    �ڣ�
��    �㣺
�� �� �ˣ�
������ݣ�
�汾��:
2����    �ڣ�
��    �㣺
�� �� �ˣ�
������ݣ�
�汾��:
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
/* Select SPI FLASH: Chip Select pin low Ƭѡ�ź� �͵�ƽ��Ч */
#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)

/* Select SPI FLASH: WP pin low   д����  �͵�ƽ��Ч  �͵�ƽ���޷�����д���� */
#define SPI_FLASH_WP_LOW()       GPIO_ResetBits(SPI_FLASH_WP_GPIO_PORT, SPI_FLASH_WP_PIN)
/* Deselect SPI FLASH: WP pin high */
#define SPI_FLASH_WP_HIGH()      GPIO_SetBits(SPI_FLASH_WP_GPIO_PORT, SPI_FLASH_WP_PIN)

/* Select SPI FLASH: HOLD pin low  �����������  �͵�ƽ��Ч ����һ���ⲿָ�� */
#define SPI_FLASH_HOLD_LOW()       GPIO_ResetBits(SPI_FLASH_HOLD_GPIO_PORT, SPI_FLASH_HOLD_PIN)
/* Deselect SPI FLASH: WP pin high */
#define SPI_FLASH_HOLD_HIGH()      GPIO_SetBits(SPI_FLASH_HOLD_GPIO_PORT, SPI_FLASH_HOLD_PIN)


/* Private typedef -----------------------------------------------------------*/
//#define SPI_FLASH_PageSize      4096  ��ʵ
#define SPI_FLASH_PageSize      256  									 //ÿҳ�ֽ���
#define SPI_FLASH_PerWritePageSize      256       //ÿҳ��д�ֽ���

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
#define W25X_ReleasePowerDown	0xAB   //�ͷŵ���
#define W25X_DeviceID										0xAB   //����ID
#define W25X_ManufactDeviceID			0x90 
#define W25X_JedecDeviceID					0x9F 

#define WIP_Flag                							0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte              						0xA5  //д����ʹ�� ��ʵ������

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
// #define  sFLASH_ID                      0xEF3015
// #define  W25Q16_FLASH_ID        0xEF3015
// #define  W25Q32_FLASH_ID        0xEF3016

#define  W25Q64_FLASH_ID        0xEF4017



/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
/* �ⲿ���ú��� */
extern void SPI_FLASH_Init(void); //W25Q64��ʼ��
extern void SPI_FLASH_SectorErase_A(u32 SectorAddr); //��������  4K  �������0-2047
extern void SPI_FLASH_SectorWrite(u8* pBuffer, u32 SectorAddr, u16 NumByteToWrite);//������̣���д0-4096�ֽ����� ��ַλ0-2047
extern void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead); //�����ݣ���ַλ��ʼ�ֽڵ�ַ

void SPI_FLASH_SectorErase(u32 SectorAddr); //��������  4K 
void SPI_FLASH_BulkErase(void);	//оƬ��������
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);  //ҳ��� ����д256�ֽ�����
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite); //û����  дһ����
u32 SPI_FLASH_ReadID(void);
u32 SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);    //����ģʽ
void SPI_Flash_WAKEUP(void);

/*----- Low layer function -----*/
u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);  //дʹ��
void SPI_FLASH_WriteDisable(void); //д��ֹ
void SPI_FLASH_WaitForWriteEnd(void);
uint8_t SPI_Flash_Check(void); //����ID��ʵ

	 
#ifdef __cplusplus
}
#endif
#endif /* __FLASH_SPI_H */
