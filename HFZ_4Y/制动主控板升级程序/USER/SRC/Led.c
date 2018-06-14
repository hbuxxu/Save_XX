/****************************************************************************
* �ļ���:   led.c
* �ļ���ʷ:
* �汾��:   V1.0
* ����:     2016.09.29    
* ����:     qgk
* ˵��:     Led�ƿ��Ʋ���
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
#include "stm32f10x.h"
#include "led.h"
#include "sys_delay.h"
/****************************************************************************
* ��    �ƣ�LED_config
* ��    �ܣ���ʼ��LED�ƹܽ�
* ��ڲ�������
* ���ڲ�������
* ˵    ������
****************************************************************************/ 
void LED_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_GPIO_LED |RCC_GPIO_sysLED| RCC_APB2Periph_AFIO , ENABLE); 
    GPIO_InitStructure.GPIO_Pin = GPIO_LED_ALL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_LED_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_SYSLED;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_LED_sysPORT, &GPIO_InitStructure);
}
/****************************************************************************
* ��    �ƣLED1_on
* ��    �ܣ�����ָʾ��
* ��ڲ�������
* ���ڲ�������
* ˵    ��������LED1
****************************************************************************/ 
void LED1_on(void)
{
    GPIO_ResetBits(GPIO_LED_PORT,GPIO_LED1);
}

/****************************************************************************
* ��    �ƣ�void LED_Spark(void)
* ��    �ܣ�����ָʾ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����Ϩ��LED1
****************************************************************************/ 
void LED1_off(void)
{
    GPIO_SetBits(GPIO_LED_PORT,GPIO_LED1);	
}



void LED_Control(void)
{   
	 if(Get_uTimer3_Counter()==100)
		LED1_on(); 
   if(Get_uTimer3_Counter()>=200)
	 {
    LED1_off();
		uTimer3_Counter_Reset(); 
	 }
}
