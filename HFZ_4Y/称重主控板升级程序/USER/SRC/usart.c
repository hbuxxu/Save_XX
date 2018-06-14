/****************************************************************************
* �ļ���:    usart.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2016.09.29    
* ����:      qgk
* ˵��:      ���ڳ�ʼ��
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
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/****************************************************************************
* ��    �ƣ�void Usart1_Init(void)
* ��    �ܣ�����1��ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void Usart1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA , ENABLE);	 //ʹ�ܴ���1ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 //USART1 TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //A�˿� 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 //USART1 RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //���ÿ�©����
    GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 //A�˿� 
    USART_InitStructure.USART_BaudRate = 115200;						//����115200bps
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure);							//���ô��ڲ�������   
    USART_Cmd(USART1, ENABLE);	
}
