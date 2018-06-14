/****************************************************************************
* 文件名: USER_IO.h
* 文件历史:
* 版本号:    V1.0
* 日期:      2016.12.09    
* 作者:     XX
* 说明:   开入开出配置
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
#ifndef __USER_IO_H
#define __USER_IO_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define IO_OPEN 0
#define IO_CLOSE 1	 
	 
#define Left_Up                 PGout(3)//   PDout(15)     //左台架举升  KO1
#define Left_Down               PGout(4)//PGout(2)       //左台架下降  KO2
#define Right_Up                  PGout(3)       //右台架举升
#define Right_Down             PGout(4)       //右台架下降	 
#define Clamp_All                 PGout(5)      //台架夹紧
#define Protect_Overflow     PGout(6)      //液压电机溢流电磁阀控制  
#define Pump_UpDown         PGout(7)     //汽车制动举升器举降控制 OPEN:下降   COLSE:上升

#define Pump_Before           PGout(8)     //摩托车 前夹紧  
#define Pump_Behind           PCout(6)  	  //摩托车 后夹紧
	 
#define Left_Machine           PCout(8)     //左电机
#define Right_Machine         PCout(9)     //右电机
#define Machine_Switch       PCout(7)     //星三角切换
#define Speed_Control         PAout(8)     //制动台测速板控制信号st

#define Hydraumatic_Machine        PEout(2)     //液压电机开出

#define LED14 PDout(0) 
#define LED15 PCout(11)
#define LED17 PCout(12)
#define LED18 PCout(10)
	 
extern void IO_GpioConfig(void);
#ifdef __cplusplus
}
#endif
#endif /* __USER_IO_H */
