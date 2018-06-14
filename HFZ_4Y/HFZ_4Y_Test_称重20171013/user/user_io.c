/****************************************************************************
* �ļ���: user_io.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2017.02.09    
* ����:      xx
* ˵��:   �̵����忪����������  �����������
*****************************************************************************/
/*=============================================================================
*******�޸���־*******
1����    �ڣ�
��    �㣺
�� �� �ˣ�
������ݣ�
�汾��:
================================================================================*/
#include "user_io.h"


/*******************************************************************************
* ������  : IO_GpioConfig
* ����    : �������ú���
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : 
*******************************************************************************/
void IO_GpioConfig(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
  
		/* �ⲿ�̵����忪��IO���� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;//PA8
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;//PC6 7 8 9
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
    GPIO_InitStructure.GPIO_Pin=0x01FC;//PG2-8
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOG,&GPIO_InitStructure);
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;//PD15
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOD,&GPIO_InitStructure);    
		
		Left_Up = IO_CLOSE;                        //��̨�ܾ���  KO1
	  Left_Down = IO_OPEN;                   //��̨���½�  KO2
	  Right_Up = IO_CLOSE;                      //��̨�ܾ���
	  Right_Down = IO_OPEN;                 //��̨���½�	 
	  Clamp_All = IO_CLOSE;                     //̨�ܼн�
	  Protect_Overflow = IO_CLOSE;         //Һѹ���������ŷ�����  
	  Pump_UpDown = IO_CLOSE;            //�����ƶ��������ٽ����� OPEN:�½�   COLSE:����

	  Pump_Before = IO_CLOSE;               //Ħ�г� ǰ�н�  
	  Pump_Behind = IO_CLOSE;          	  //Ħ�г� ��н�
		 
	  Left_Machine = IO_CLOSE;               //����
	  Right_Machine = IO_CLOSE;             //�ҵ��
	  Machine_Switch = IO_CLOSE;           //�������л�
	  Speed_Control = IO_CLOSE;             //�ƶ�̨���ٰ�����ź�st
	 
				
		/* ���忪��IO���� */
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//PC13 14 15   ��6 7 8ͨ����
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//PG10   5ͨ��
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOG,&GPIO_InitStructure);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;//PE3 4 5 6  ��2 3 4 1ͨ����
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOE,&GPIO_InitStructure);
		/* ���忪��IO���� */
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//PE2  ���ڲ�1ͨ����
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOE,&GPIO_InitStructure);
		
		/* ����ָʾ������ */		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;//PC10 11 12  ��ָʾ��LD18 LD15 LD17��
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//PD0  ��ָʾ��LD14��
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOD,&GPIO_InitStructure);
		LED14=0;
		LED15=0;
		LED17=0;
		LED18=0;
		
		 Hydraumatic_Machine = IO_CLOSE; //Һѹ�������
}
