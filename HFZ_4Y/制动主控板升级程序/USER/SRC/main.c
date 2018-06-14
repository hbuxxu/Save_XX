/****************************************************************************
* �ļ���:    main.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2017.09.29    
* ����:      qgk
* ʵ��ƽ̨�������������ذ� + �������
* ˵��:      ��������
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
#include "common.h" 
#include "IWDG.h"
#include "usart.h"    
#include "sys_delay.h"
#include "W5500.h"	    
#include "W5500_app.h"	
#include "led.h"
#include <string.h>  
/**********************����������******************************/     
void NVIC_Configuration(void);   
void Usart_Init(void);
void FLASH_IO_Init(void);
void Upgrate(void);
/**********************����������******************************/    

/****************************************************************************/

/****************************************************************************
* ��    �ƣ�int main(void)
* ��    �ܣ�������
* ��ڲ�������
* ���ڲ�������
* ˵    ������
* ���÷������� 
****************************************************************************/ 
int main(void)
{
    /*�ⲿ�жϳ�ʼ��*/	
    NVIC_Configuration();	
		SPI_FLASH_Init();
    /*LED ��ʼ��*/
    LED_config();        
    /*���ڳ�ʼ��*/
    Usart_Init();	    	
    /*��ȷDelay��ʱ��ʱ����ʼ��*/
    DelayTimerInit_Config();    
    /*W5500 SPI��ʼ������*/
    W5500_SPI_Configuration(); 
    /*W5500 GPIO��ʼ������*/
    W5500_GPIO_Configuration();	
    /*װ���������*/
    Load_Net_Parameters();
    /*W5500��ʼ��*/
    W5500_Initialization();
	  IWDG_QConfiguration();
    while (1)
    {  
			/*�������ܺ���*/
    Upgrate();
		IWDG_ReloadCounter();	
		LED_Control();
    }
}
/****************************************************************************
* ��    �ƣ�Upgrate
* ��    �ܣ��������ܺ���
* ��ڲ�������
* ���ڲ�������
* ˵    ������
* ���÷������� 
****************************************************************************/ 
    void Upgrate(void)
{
	     NVIC_InitTypeDef NVIC_InitStructure;
			 /*W5500���ݴ���*/
				W5500_DataHandling();
        if(wUp_grade==0)
        {
            SerialPutString("\r\n======================================================================");
            SerialPutString("\r\n=              **************************                            =");
            SerialPutString("\r\n=                                                                    =");
            SerialPutString("\r\n=     In-Application Programming Application  (Version 1.0.0)        =");
            SerialPutString("\r\n=                                                                    =");
            SerialPutString("\r\n=                             By QI                                  =");
            SerialPutString("\r\n======================================================================");
            SerialPutString("\r\n\r\n");
            wUp_grade=1;
        }
        /*����ִ���û�����*/
        if(wUp_grade==6)
        {   
            FLASH_Lock();
            /*�ж��û��Ƿ��Ѿ����س�����Ϊ��������´˵�ַ��ջ��ַ��*/ 
            if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
            {		 
						 SerialPutString("Execute user Program\r\n\n"); /*��ת���û�����*/
              __disable_irq();
							TIM_Cmd(TIM3, DISABLE);  									                    //ʧ��TIMx����
							TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	                  //���־λ
                /* �ر��ⲿ�ж�*/
                NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
                NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
                NVIC_Init(&NVIC_InitStructure);			
							  EXTI_ClearITPendingBit(EXTI_Line5);
 							  USART_Cmd(USART1, DISABLE);                      /* �رմ���*/
								USART_ClearFlag(USART1,USART_FLAG_RXNE);
                USART_ClearITPendingBit(USART1,USART_IT_RXNE); 
                JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
                Jump_To_Application = (pFunction) JumpAddress;
                /*��ʼ���û�����Ķ�ջָ��*/
                __set_MSP(*(__IO uint32_t*) ApplicationAddress);
                Jump_To_Application();
             }
            else
            {
                SerialPutString(" no user Program\r\n\n");
                wUp_grade=7;
            }
        }
			}
/****************************************************************************
* ��    �ƣ�void Usart_Init(void)
* ��    �ܣ����ڳ�ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����  ��Ӵ��ڳ�ʼ������
* ���÷�����
****************************************************************************/
void Usart_Init(void)
{  
    /*����1��ʼ��*/
    Usart1_Init();
}

/****************************************************************************
* ��    �ƣ�void NVIC_Configuration(void)
* ��    �ܣ��ж�Դ���ú���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�������   
****************************************************************************/
void NVIC_Configuration(void)
{
    /* ���ȼ���1 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	
    /*��ʱ��3�ж�Դ����*/
    NVIC_Configuration_TIM3();
    /*W5500 �����ж������ж�Դ����*/
    W5500_NVIC_Configuration();	
}


