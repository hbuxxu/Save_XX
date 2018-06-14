/****************************************************************************
* �ļ���: AD7663.h
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2016.12.09    
* ����:      qgk
* ˵��:   
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
#ifndef __AD7663_H
#define __AD7663_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif
 
	 
#define AD7663_Con     ((uint16_t)0xFF00)  /*AD7663���ƶ˿� PE8-15  ����Ϊ���*/
 // ADת�����Ŀ�������	 
 #define AD7663_CVT      PEout(11)  	//PE11
 #define AD7663_nCS      PEout(10)		//PE10
 #define AD7663_nRD      PEout(9) 	  //PE9
 #define AD7663_Byte      PEout(8) 			//PE8
 // ģ�⿪�صĵ�ַѡ������ѡ��ͨ��
 #define ADG408_A2       GPIO_Pin_14	  //PE14
 #define ADG408_A1       GPIO_Pin_15	 //PE15
 #define ADG408_A0       GPIO_Pin_13  	//PE13
 #define ADG408_EN1     GPIO_Pin_12	  //PE12
 #define AD7663_Busy     GPIO_Pin_10   // PD10

extern uint16_t  ADC_Convert(uint8_t u8Channel); //AD7663ת������
extern void EXTI15_10_ISR(void);	 												 //AD7663BUSY������
extern void TIM2_ISR(void);           												 //��ʱ��2�жϷ�����
extern void TIM3_ISR(void);           												 //��ʱ��3�жϷ�����
// extern void ADC_7663_Gpioinit(void);										 //AD7663 �˿�����
// extern void ADC_7663_NVIC_Config(void);	 				   //�ж�����
extern void Delay_ms(uint16_t time_ms);  							 //ms��ʱ����
extern void ADC_7663_Init(void); //��ʼ������Ad7663
#ifdef __cplusplus
}
#endif
#endif /* __AD7663_H */
