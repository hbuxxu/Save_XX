/****************************************************************************
* 文件名:   led.c
* 文件历史:
* 版本号:   V1.0
* 日期:     2016.09.29    
* 作者:     qgk
* 说明:     Led灯控制部分
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
#include "stm32f10x.h"
#include "led.h"
#include "sys_delay.h"
/****************************************************************************
* 名    称：LED_config
* 功    能：初始化LED灯管脚
* 入口参数：无
* 出口参数：无
* 说    明：无
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
* 名    称ED1_on
* 功    能：控制指示灯
* 入口参数：无
* 出口参数：无
* 说    明：点亮LED1
****************************************************************************/ 
void LED1_on(void)
{
    GPIO_ResetBits(GPIO_LED_PORT,GPIO_LED1);
}

/****************************************************************************
* 名    称：void LED_Spark(void)
* 功    能：控制指示灯
* 入口参数：无
* 出口参数：无
* 说    明：熄灭LED1
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
