/*******************************************************************************
** �ļ���: 		W5500_app.h
** �ļ���ʷ:
** �汾��  		V1.0
** ��������: 
** ����: 		  qgk
** ��������:  2016.09.29    
** �޸���־��
*******************************************************************************/
#ifndef	_W5500_APP_H_
#define	_W5500_APP_H_

#include "stm32f10x.h"

#define   WEIGHT_ADDRESS  0x02   //�������ذ��ַ��
#define   BRAKE_ADDRESS    0x03   //�ƶ����ذ��ַ��

#define   CON_ADDRESS     WEIGHT_ADDRESS   //��ϵͳ���ذ��ַ��  ���տ�ͨ�����������޸�
#define   CON_START  					0x02   //��ʼλ
#define   CON_END      					0x03   //����λ

/**********���ʼ������***************/
extern void Load_Net_Parameters(void);
//extern void W5500_Initialization(void);
extern void W5500_Init_Config(void);   //���ú�W5500���� ������Ҫ��ǰ���úö�ʱ��2 �õ�����ʱ����  ��ʱ��2��AD7663������
extern void W5500_Reset(void);          //ǿ�Ƹ�λ
/*************************************/
extern void W5500_Receive_Deal(void);  //�������ݴ�����

extern void W5500_Send_Data(uint8_t s,uint8_t *Data); //��������

extern void IDtoMAC(u8 *ulp);  //STM32  IDת����MAC��ַ

#endif
