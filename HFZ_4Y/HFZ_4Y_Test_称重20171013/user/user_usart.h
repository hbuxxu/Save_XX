/****************************************************************************
* �ļ���: USER_USART.h
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2016.12.09    
* ����:     XX
* ˵��:   ��������
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
#ifndef __USER_USART_H
#define __USER_USART_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif


#define User_USART  USART1  // USART1 DB9�˿�  USART2 �̶���
	 
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
