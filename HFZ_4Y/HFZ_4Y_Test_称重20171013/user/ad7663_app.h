/****************************************************************************
* �ļ���: AD7663_APP.h
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2017.09.15    
* ����:      XX
* ˵��:   AD�ɼ�ѭ��������ʽģ��
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
#ifndef __AD7663_APP_H
#define __AD7663_APP_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif
/***********��ʱ��4�ж����������޸�*********************/
#define 		Tim4_ms 		1	  //��ʱ����ʱʱ��  ������ʱ�� ��λms
// #define 		NVIC_PriorityGroup_Set 			NVIC_PriorityGroup_1
#define    TIM4_IRQChannelPreemptionPriority   					 1   //�����������ȼ� �����ж���һ���ж�������
#define    TIM4_IRQChannelSubPriority   											 3	 	//������Ӧ���ȼ�

/***********ѭ������ʹ�ñ���*********************/
#define MaxCabItem    		2         //���ɼ�ͨ����
#define DLLength    			1000      //ѭ�����г���
#define Window_Display    900       //��ʾ�������� �����������ʾ���ϴ�
#define Window_Adjust    	330       //��ʾ�������� ���ڱ궨�����ϴ�
#define Window_Samp  		  10        //��ʾ�������� ������Ч���ݲɼ�
	 

extern void TIM4_NVIC_Config(void);  							//��ʱ��4����	 
extern void InitADScreen(void);  									//��ʼ�����б���
extern uint16_t ADShow_Display(uint8_t chm);	    //�������ʾʹ��
extern uint16_t ADShow_Adjust(uint8_t chm); 		  //�궨ʹ��
extern uint16_t ADShow_Samp(uint8_t chm); 		    //ʵ�ʲ�������ʹ��
extern void TIM4_ISR(void);	 											//�жϷ������
	 

#ifdef __cplusplus
}
#endif
#endif /* __AD7663_APP_H */
