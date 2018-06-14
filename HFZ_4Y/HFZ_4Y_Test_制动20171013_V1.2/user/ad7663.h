/****************************************************************************
* 文件名: AD7663.h
* 文件历史:
* 版本号:    V1.0
* 日期:      2016.12.09    
* 作者:      qgk
* 说明:   
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
#ifndef __AD7663_H
#define __AD7663_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif
 
	 
#define AD7663_Con     ((uint16_t)0xFF00)  /*AD7663控制端口 PE8-15  配置为输出*/
 // AD转换器的控制引脚	 
 #define AD7663_CVT      PEout(11)  	//PE11
 #define AD7663_nCS      PEout(10)		//PE10
 #define AD7663_nRD      PEout(9) 	  //PE9
 #define AD7663_Byte      PEout(8) 			//PE8
 // 模拟开关的地址选择，用来选择通道
 #define ADG408_A2       GPIO_Pin_14	  //PE14
 #define ADG408_A1       GPIO_Pin_15	 //PE15
 #define ADG408_A0       GPIO_Pin_13  	//PE13
 #define ADG408_EN1     GPIO_Pin_12	  //PE12
 #define AD7663_Busy     GPIO_Pin_10   // PD10

extern uint16_t  ADC_Convert(uint8_t u8Channel); //AD7663转换函数
extern void EXTI15_10_ISR(void);	 												 //AD7663BUSY服务函数
extern void TIM2_ISR(void);           												 //定时器2中断服务函数
extern void TIM3_ISR(void);           												 //定时器3中断服务函数
// extern void ADC_7663_Gpioinit(void);										 //AD7663 端口配置
// extern void ADC_7663_NVIC_Config(void);	 				   //中断配置
extern void Delay_ms(uint16_t time_ms);  							 //ms延时函数
extern void ADC_7663_Init(void); //初始化配置Ad7663
#ifdef __cplusplus
}
#endif
#endif /* __AD7663_H */
