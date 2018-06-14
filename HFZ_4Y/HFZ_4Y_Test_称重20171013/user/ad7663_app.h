/****************************************************************************
* 文件名: AD7663_APP.h
* 文件历史:
* 版本号:    V1.0
* 日期:      2017.09.15    
* 作者:      XX
* 说明:   AD采集循环队列形式模板
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
#ifndef __AD7663_APP_H
#define __AD7663_APP_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif
/***********定时器4中断向量设置修改*********************/
#define 		Tim4_ms 		1	  //定时器定时时间  即采样时间 单位ms
// #define 		NVIC_PriorityGroup_Set 			NVIC_PriorityGroup_1
#define    TIM4_IRQChannelPreemptionPriority   					 1   //设置抢先优先级 根据中断组一共有多少设置
#define    TIM4_IRQChannelSubPriority   											 3	 	//设置响应优先级

/***********循环队列使用变量*********************/
#define MaxCabItem    		2         //最大采集通道数
#define DLLength    			1000      //循环队列长度
#define Window_Display    900       //显示滑动窗口 用于数码管显示和上传
#define Window_Adjust    	330       //显示滑动窗口 用于标定数据上传
#define Window_Samp  		  10        //显示滑动窗口 用于有效数据采集
	 

extern void TIM4_NVIC_Config(void);  							//定时器4配置	 
extern void InitADScreen(void);  									//初始化所有变量
extern uint16_t ADShow_Display(uint8_t chm);	    //数码管显示使用
extern uint16_t ADShow_Adjust(uint8_t chm); 		  //标定使用
extern uint16_t ADShow_Samp(uint8_t chm); 		    //实际采样曲线使用
extern void TIM4_ISR(void);	 											//中断服务程序
	 

#ifdef __cplusplus
}
#endif
#endif /* __AD7663_APP_H */
