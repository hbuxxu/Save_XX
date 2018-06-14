/*******************************************************************************
** 文件名: 		W5500_app.h
** 文件历史:
** 版本：  		V1.0
** 工作环境: 
** 作者: 		  qgk
** 生成日期:  2016.09.29    
** 修改日志：
*******************************************************************************/
#ifndef	_W5500_APP_H_
#define	_W5500_APP_H_

#include "stm32f10x.h"

#define   WEIGHT_ADDRESS  0x02   //轴重主控板地址码
#define   BRAKE_ADDRESS    0x03   //制动主控板地址码

#define   CON_ADDRESS     WEIGHT_ADDRESS   //本系统主控板地址码  最终可通过参数设置修改
#define   CON_START  					0x02   //起始位
#define   CON_END      					0x03   //结束位

/**********需初始化函数***************/
extern void Load_Net_Parameters(void);
//extern void W5500_Initialization(void);
extern void W5500_Init_Config(void);   //配置好W5500函数 不过需要提前配置好定时器2 用到了延时函数  定时器2在AD7663中配置
extern void W5500_Reset(void);          //强制复位
/*************************************/
extern void W5500_Receive_Deal(void);  //接收数据处理函数

extern void W5500_Send_Data(uint8_t s,uint8_t *Data); //发送数据

extern void IDtoMAC(u8 *ulp);  //STM32  ID转化成MAC地址

#endif
