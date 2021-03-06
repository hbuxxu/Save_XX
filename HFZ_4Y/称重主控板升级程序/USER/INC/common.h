/*******************************************************************************
** 文件名: 		common.h
** 文件历史:
** 版本：  		V1.0
** 工作环境: 
** 作者: 		  qgk
** 生成日期:  2016.09.29    
** 修改日志：
*******************************************************************************/
/* 防止重定义 ----------------------------------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* 包含头文件 *****************************************************************/
#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"

/* 类型声明 -----------------------------------------------------------------*/
typedef  void (*pFunction)(void);
//* 宏 ------------------------------------------------------------------------*/
#define CMD_STRING_SIZE       128
#define ApplicationAddress        0x8010000 
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
#define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
#define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
#define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
#define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
#define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
#define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
#define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
#define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
#error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif
#define FLASH_IMAGE_SIZE                   (uint32_t) (FLASH_SIZE - (ApplicationAddress - 0x08000000))
#define SerialPutString(x) Serial_PutString((uint8_t*)(x))
/* 函数声明 ------------------------------------------------------------------*/
uint32_t GetIntegerInput(int32_t * num);
void SerialPutChar(uint8_t c);
void Serial_PutString(uint8_t *s);
uint32_t FLASH_PagesMask(__IO uint32_t Size);
void FLASH_DisableWriteProtectionPages(void);
void Main_Menu(void);
extern uint32_t JumpAddress;
extern pFunction Jump_To_Application;

#endif  /* _COMMON_H */

/*******************************文件结束***************************************/
