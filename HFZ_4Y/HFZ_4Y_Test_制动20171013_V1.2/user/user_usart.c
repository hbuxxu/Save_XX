/**********************************************************************************
* �ļ���  ��Usart.c
* ��  ��  ��usart ����������         
* ��汾  ��ST_v3.5
**********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "user_usart.h"

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(User_USART, (uint8_t) ch);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(User_USART, USART_FLAG_TC) == RESET)
    {
    }
    return ch;
}
/****************************************************************************
* ��    �ƣ�void Usart2_Init(void)
* ��    �ܣ�����2��ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void Usart_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	
	/* ���ô���1 ����ʹ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   //PA9  USART1_TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   //PA10  USART1_RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE); //ʹ��USART1ʱ��		
		/*������9600 8λ 1λֹͣλ ����żУ�� ��ֹӲ��������*/
		USART_InitStructure.USART_BaudRate=115200;
		USART_InitStructure.USART_WordLength=USART_WordLength_8b;
		USART_InitStructure.USART_StopBits=USART_StopBits_1;
		USART_InitStructure.USART_Parity=USART_Parity_No;
		USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
		USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
		USART_Init(USART1,&USART_InitStructure);
	  /*ʹ��USART1�����ж�*/
		// USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	  /*ʹ��USART1����*/
	  USART_Cmd(User_USART,ENABLE);


		/* ���ô���2 ����ʹ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;   //PA2  USART2_TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;   //PA3  USART2_RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); //ʹ��USART2ʱ��		
		/*������9600 8λ 1λֹͣλ ����żУ�� ��ֹӲ��������*/
		USART_InitStructure.USART_BaudRate=9600;
		USART_InitStructure.USART_WordLength=USART_WordLength_8b;
		USART_InitStructure.USART_StopBits=USART_StopBits_1;
		USART_InitStructure.USART_Parity=USART_Parity_No;
		USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
		USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
		USART_Init(USART2,&USART_InitStructure);
	  /*ʹ��USART1�����ж�*/
		// USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	  /*ʹ��USART1����*/
	  USART_Cmd(User_USART,ENABLE);
}
