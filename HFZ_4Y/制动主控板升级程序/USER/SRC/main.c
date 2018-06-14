/****************************************************************************
* 文件名:    main.c
* 文件历史:
* 版本号:    V1.0
* 日期:      2017.09.29    
* 作者:      qgk
* 实验平台：整备质量主控板 + 测试软件
* 说明:      升级程序
*****************************************************************************/
/*=============================================================================
*******修改日志*******
1、日    期：
地    点：
修 改 人：
添加内容：
版本号:
2、日    期：
地    点：
修 改 人：
添加内容：
版本号:
================================================================================*/
#include "common.h" 
#include "IWDG.h"
#include "usart.h"    
#include "sys_delay.h"
#include "W5500.h"	    
#include "W5500_app.h"	
#include "led.h"
#include <string.h>  
/**********************函数定义区******************************/     
void NVIC_Configuration(void);   
void Usart_Init(void);
void FLASH_IO_Init(void);
void Upgrate(void);
/**********************变量定义区******************************/    

/****************************************************************************/

/****************************************************************************
* 名    称：int main(void)
* 功    能：主函数
* 入口参数：无
* 出口参数：无
* 说    明：无
* 调用方法：无 
****************************************************************************/ 
int main(void)
{
    /*外部中断初始化*/	
    NVIC_Configuration();	
		SPI_FLASH_Init();
    /*LED 初始化*/
    LED_config();        
    /*串口初始化*/
    Usart_Init();	    	
    /*精确Delay延时定时器初始化*/
    DelayTimerInit_Config();    
    /*W5500 SPI初始化配置*/
    W5500_SPI_Configuration(); 
    /*W5500 GPIO初始化配置*/
    W5500_GPIO_Configuration();	
    /*装载网络参数*/
    Load_Net_Parameters();
    /*W5500初始化*/
    W5500_Initialization();
	  IWDG_QConfiguration();
    while (1)
    {  
			/*升级功能函数*/
    Upgrate();
		IWDG_ReloadCounter();	
		LED_Control();
    }
}
/****************************************************************************
* 名    称：Upgrate
* 功    能：升级功能函数
* 入口参数：无
* 出口参数：无
* 说    明：无
* 调用方法：无 
****************************************************************************/ 
    void Upgrate(void)
{
	     NVIC_InitTypeDef NVIC_InitStructure;
			 /*W5500数据处理*/
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
        /*否则执行用户程序*/
        if(wUp_grade==6)
        {   
            FLASH_Lock();
            /*判断用户是否已经下载程序，因为正常情况下此地址是栈地址。*/ 
            if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
            {		 
						 SerialPutString("Execute user Program\r\n\n"); /*跳转至用户代码*/
              __disable_irq();
							TIM_Cmd(TIM3, DISABLE);  									                    //失能TIMx外设
							TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	                  //清标志位
                /* 关闭外部中断*/
                NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
                NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
                NVIC_Init(&NVIC_InitStructure);			
							  EXTI_ClearITPendingBit(EXTI_Line5);
 							  USART_Cmd(USART1, DISABLE);                      /* 关闭串口*/
								USART_ClearFlag(USART1,USART_FLAG_RXNE);
                USART_ClearITPendingBit(USART1,USART_IT_RXNE); 
                JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
                Jump_To_Application = (pFunction) JumpAddress;
                /*初始化用户程序的堆栈指针*/
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
* 名    称：void Usart_Init(void)
* 功    能：串口初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：  添加串口初始化函数
* 调用方法：
****************************************************************************/
void Usart_Init(void)
{  
    /*串口1初始化*/
    Usart1_Init();
}

/****************************************************************************
* 名    称：void NVIC_Configuration(void)
* 功    能：中断源配置函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无   
****************************************************************************/
void NVIC_Configuration(void)
{
    /* 优先级组1 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	
    /*定时器3中断源配置*/
    NVIC_Configuration_TIM3();
    /*W5500 接收中断引脚中断源配置*/
    W5500_NVIC_Configuration();	
}


