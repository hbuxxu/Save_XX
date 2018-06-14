/**********************************************************************************
* 文件名  ：Usart.c
* 描  述  ：usart 驱动函数库         
* 库版本  ：ST_v3.5
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
* 名    称：void Usart2_Init(void)
* 功    能：串口2初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void Usart_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	
	/* 配置串口1 调试使用 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   //PA9  USART1_TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   //PA10  USART1_RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE); //使能USART1时钟		
		/*波特率9600 8位 1位停止位 无奇偶校验 禁止硬件流控制*/
		USART_InitStructure.USART_BaudRate=115200;
		USART_InitStructure.USART_WordLength=USART_WordLength_8b;
		USART_InitStructure.USART_StopBits=USART_StopBits_1;
		USART_InitStructure.USART_Parity=USART_Parity_No;
		USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
		USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
		USART_Init(USART1,&USART_InitStructure);
	  /*使能USART1接收中断*/
		// USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	  /*使能USART1外设*/
	  USART_Cmd(User_USART,ENABLE);


		/* 配置串口2 调试使用 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;   //PA2  USART2_TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;   //PA3  USART2_RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); //使能USART2时钟		
		/*波特率9600 8位 1位停止位 无奇偶校验 禁止硬件流控制*/
		USART_InitStructure.USART_BaudRate=9600;
		USART_InitStructure.USART_WordLength=USART_WordLength_8b;
		USART_InitStructure.USART_StopBits=USART_StopBits_1;
		USART_InitStructure.USART_Parity=USART_Parity_No;
		USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
		USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
		USART_Init(USART2,&USART_InitStructure);
	  /*使能USART1接收中断*/
		// USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	  /*使能USART1外设*/
	  USART_Cmd(User_USART,ENABLE);
}
