/****************************************************************************
* 文件名:    sys_delay.c
* 文件历史:
* 版本号:    V1.0
* 日期:      2016.09.29    
* 作者:      qgk
* 说明:      延时函数
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
#include "sys_delay.h"
#include "W5500.h"
unsigned int uTimer3_Counter=0;             //延时定时变量
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 函数名  : NVIC_Configuration_TIM3
* 描  述  : STM32中断向量表配配置
* 输  入  : 无
* 输  出  : 无
* 返回值  : 无
* 说  明  : 
*******************************************************************************/
void NVIC_Configuration_TIM3(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;						//定义NVIC初始化结构体
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				//设置中断优先级组为1，优先组(可设0～4位)
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//设置中断向量号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;	//设置抢先优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//设置响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能NVIC
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* 函数名  : void DelayTimerInit_Config
* 描  述  : 精确Delay延时定时器初始化
* 输  入  : 无
* 输  出  : 无
* 返  回  : 无 
* 说  明  : 无
*******************************************************************************/
void DelayTimerInit_Config(void)
{
    Timer3_Init_Config();
}
/*******************************************************************************
* 函数名  : Timer3_Init_Config
* 描  述  : Timer3初始化配置
* 输  入  : 无
* 输  出  : 无
* 返  回  : 无 
* 说  明  : 无     调试中
*******************************************************************************/
void Timer3_Init_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	       	//使能Timer3时钟
    TIM_TimeBaseStructure.TIM_Period = 9;	                  	    //TIM_TimeBaseStructure.TIM_Period = 9;						
    TIM_TimeBaseStructure.TIM_Prescaler = 7200;                   //TIM_TimeBaseStructure.TIM_Prescaler = 7199;		
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	    	//设置时钟分割:TDTS = TIM_CKD_DIV1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				        //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); 				          //使能TIM2指定的中断
    TIM_Cmd(TIM3, ENABLE);  									                    //使能TIMx外设
}

/*******************************************************************************
* 函数名  : TIM3_IRQHandler
* 描  述  : 定时器3中断断服务函数
* 输  入  : 无
* 输  出  : 无
* 返  回  : 无 
* 说  明  : 无
*******************************************************************************/
void TIM3_IRQHandler(void)
{  
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {	
        uTimer3_Counter++;
        if(uTimer3_Counter>=100000)
        uTimer3_Counter=0;
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
    }
}
/*******************************************************************************
* 函数名  : TimeDelay_ms
* 描  述  : 延时函数(ms)
* 输  入  : d:延时系数，单位为毫秒
* 输  出  : 无
* 返  回  : 无 
* 说  明  : 延时是利用Timer3定时器产生的1毫秒的计数来实现的
*******************************************************************************/
void TimeDelay_ms(unsigned int d)
{
    uTimer3_Counter=0; 
    while((uTimer3_Counter!=0)&&(uTimer3_Counter<d));
}

/******************************************************************************
* 函数名  : unsigned int Get_ClockTick(void)
* 描述    : 获得ClockTick参数值
* 输入    : 无
* 输出    : 无
* 返回值  : ClockTick
* 说明    : 关闭LED
*******************************************************************************/
unsigned int Get_uTimer3_Counter(void)
{
    return uTimer3_Counter;
}


	void uTimer3_Counter_Reset(void)
  {
		uTimer3_Counter=0;
   }
