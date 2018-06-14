/****************************************************************************
* 文件名: USER_USART.h
* 文件历史:
* 版本号:    V1.0
* 日期:      2016.12.09    
* 作者:     XX
* 说明:   串口设置
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
#ifndef __USER_USART_H
#define __USER_USART_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif


#define User_USART  USART1  // USART1 DB9端口  USART2 绿端子
	 
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */	 

extern void Usart_Init(void);
	 
#ifdef __cplusplus
}
#endif
#endif /* __USER_USART_H */
