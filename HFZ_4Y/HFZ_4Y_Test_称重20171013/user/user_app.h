/****************************************************************************
* �ļ���: USER_APP.h
* �ļ���ʷ:
* �汾��:    V1.0
* ����:       2016.02.16    
* ����:      XX
* ˵��:   ���������õ��Ӻ���  �������״̬����
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
#ifndef __USER_APP_H
#define __USER_APP_H

#include "stm32f10x.h"



#ifdef __cplusplus
 extern "C" {
#endif

#define Con_AvgNum   10   //�������� 1msһ����
#define DISPFREQ 100     //��ʾˢ��Ƶ��  ���ô���256
#define	 LED_OFF 							0xFFFF //�����ȫ�� 
#define  VOLTAGE   0.1525902189   //10*1000/65535 ת���ɵ�ѹֵ  1000��
	 
/* ����ʹ�ã���ʹ��ʱ��ע�͵� */	 
#define _DEBUG     		 	 
	 
	
extern void IWDG_Configuration(void);   //�������Ź����ú���
extern void Fun_Stable(void);  													//����״̬
extern void Fun_ReadData(void);                      //��ȡ���ݺ���
extern void Fun_Adjust(void);															//�궨����
extern void Fun_SignalTest(void); 											//�źŲ��Ժ���
extern void Fun_Zero(uint8_t Channel_ID,uint8_t state);    		//����	 
extern uint8_t IO_GetInStatus(uint8_t IO_ID);		//�������ź�״̬	 
extern void Fun_StaticWeightTest(void);						//��̬���غ���
extern void Fun_DynamicWeightTest(void);				//��̬���غ���	 
extern void Fun_MotorWeightTest(void);    				//Ħ�г����غ���
	 
extern void DataRank(uint16_t *Data,uint16_t number);	 //��������  ����
extern void Get_Parameter(void); //��ȡ����
extern uint16_t Fun_VoltageConvertValue(uint8_t AD_ID,uint16_t Voltage_Relative);//��Ե�ѹת���ɲ���ֵ	 
extern void Deal_Data(uint16_t Number,uint8_t *Data,uint8_t Command);  //���ݴ���
extern void FunW5500_SendGW(uint8_t s,uint16_t L_Data,uint16_t R_Data);   //���͵�ǰ����
extern void FunW5500_SendLED(uint8_t s,uint16_t L_Data,uint16_t R_Data);   //�������ʾ
extern void FunW5500_SendDW(uint8_t s);  //���ؽ������
extern void FunW5500_TestC(uint8_t s);
extern uint16_t Fun_DealWeight(uint16_t Weight);
extern uint8_t Fun_DealParameter(void);
extern void Fun_GetParameter(void);
extern void W5500_ParameterReset(void); //ǿ��IP��λ

#ifdef __cplusplus
}
#endif
#endif /* __USER_APP_H */
