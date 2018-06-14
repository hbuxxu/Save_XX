/****************************************************************************
* 文件名: user_io.c
* 文件历史:
* 版本号:    V1.0
* 日期:      2017.02.09    
* 作者:      xx
* 说明:   继电器板开出控制配置  开入控制配置
*****************************************************************************/
/*=============================================================================
*******修改日志*******
1、日    期：
地    点：
修 改 人：
添加内容：
版本号:
================================================================================*/
#include "user_io.h"


/*******************************************************************************
* 函数名  : IO_GpioConfig
* 描述    : 开出配置函数
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 
*******************************************************************************/
void IO_GpioConfig(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
  
		/* 外部继电器板开出IO配置 */
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
		
		Left_Up = IO_CLOSE;                        //左台架举升  KO1
	  Left_Down = IO_OPEN;                   //左台架下降  KO2
	  Right_Up = IO_CLOSE;                      //右台架举升
	  Right_Down = IO_OPEN;                 //右台架下降	 
	  Clamp_All = IO_CLOSE;                     //台架夹紧
	  Protect_Overflow = IO_CLOSE;         //液压电机溢流电磁阀控制  
	  Pump_UpDown = IO_CLOSE;            //汽车制动举升器举降控制 OPEN:下降   COLSE:上升

	  Pump_Before = IO_CLOSE;               //摩托车 前夹紧  
	  Pump_Behind = IO_CLOSE;          	  //摩托车 后夹紧
		 
	  Left_Machine = IO_CLOSE;               //左电机
	  Right_Machine = IO_CLOSE;             //右电机
	  Machine_Switch = IO_CLOSE;           //星三角切换
	  Speed_Control = IO_CLOSE;             //制动台测速板控制信号st
	 
				
		/* 主板开入IO配置 */
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//PC13 14 15   （6 7 8通道）
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//PG10   5通道
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOG,&GPIO_InitStructure);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;//PE3 4 5 6  （2 3 4 1通道）
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOE,&GPIO_InitStructure);
		/* 主板开出IO配置 */
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//PE2  （内部1通道）
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOE,&GPIO_InitStructure);
		
		/* 运行指示灯配置 */		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;//PC10 11 12  （指示灯LD18 LD15 LD17）
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//PD0  （指示灯LD14）
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_Init(GPIOD,&GPIO_InitStructure);
		LED14=0;
		LED15=0;
		LED17=0;
		LED18=0;
		
		 Hydraumatic_Machine = IO_CLOSE; //液压电机开出
}
