/****************************************************************************
* 文件名: USER_APP.h
* 文件历史:
* 版本号:    V1.0
* 日期:       2016.02.16    
* 作者:      XX
* 说明:   主函数调用的子函数  各个检测状态函数
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
#ifndef __USER_APP_H
#define __USER_APP_H

#include "stm32f10x.h"



#ifdef __cplusplus
 extern "C" {
#endif

//#define Con_AvgNum   				10   //采样点数
//#define DISPFREQ 							100     //显示刷新频率  不得大于256
#define	 LED_OFF 							0xFFFF //数码管全灭
#define  VOLTAGE   						0.1525902189   //10*1000/65535 转换成电压值  1000倍

#define  PROTECT_MAX	 		3  //阻滞保护点数
#define  PROTECT_MAX1	 		30  //阻滞保护点数	 电机启动瞬间
	 
/* 调试使用，不使用时需注释掉 */	 
// #define _DEBUG     		 	 
	 
extern void IWDG_Configuration(void);   //独立看门狗配置函数	 
extern void Fun_Stable(void);  													//待机状态
extern void Fun_ReadData(void);                      //读取数据函数
extern void Fun_Adjust(void);															//标定函数
extern void Fun_SignalTest(void); 											//信号测试函数
extern void Fun_Zero(uint8_t Channel_ID,uint8_t state);    		//调零	 
extern uint8_t IO_GetInStatus(uint8_t IO_ID);		//读开入信号状态	 
extern void Fun_DynamoStart(void);									//电机启动函数
// extern void Fun_StaticWeightTest(void);						//静态称重函数
// extern void Fun_DynamicWeightTest(void);				//动态称重函数	 
// extern void Fun_MotorWeightTest(void);    				//摩托车称重函数
extern void Fun_AllWeightTest(void);					 				//空载复合称重函数
extern void Fun_TestZZ(void);														//阻滞测试函数
extern void Fun_TestZC(void);														//阻滞+空载制动之空载制动函数
extern void Fun_TestCC(void);													//空载制动测试函数
extern void Fun_AllWeightTestJJ(void); 						//加载举升完成函数
extern void Fun_TestJJ(void);													  //加载制动测试函数 
	 
extern void DataRank(uint16_t *Data,uint16_t number);	 //数据排序  升序
extern void Get_Parameter(void); //获取参数
extern uint16_t Fun_VoltageConvertValue(uint8_t AD_ID,uint16_t Voltage_Relative);//相对电压转换成测量值	 
extern void Deal_Data(uint16_t Number,uint8_t *Data,uint8_t Command);  //数据处理
extern void FunW5500_SendGW(uint8_t s,uint16_t L_Data,uint16_t R_Data);   //发送当前轮重
extern void FunW5500_SendLED(uint8_t s,uint16_t L_Data,uint16_t R_Data);   //数码管显示
extern void FunW5500_SendDW(uint8_t s);  //轮重结果数据
extern void FunW5500_SendGB(uint8_t s,uint16_t L_Data,uint16_t R_Data);     //发送当前制动显示
extern void FunW5500_SendGF(uint8_t s,uint16_t ALL_Data);                           //发送复合称重
extern void FunW5500_SendDF(uint8_t s,uint8_t a,uint16_t ALL_Data);                           //发送复合轴重结果
extern void FunW5500_SendT1(uint8_t s,uint16_t ALL_Data);                             //左阻滞失败
extern void FunW5500_SendT2(uint8_t s,uint16_t ALL_Data);                             //右阻滞失败
extern void FunW5500_SendDC(uint8_t s);                                                          //上传单机检测结果
extern void Fun_DealZZ(uint16_t Number);      //处理阻滞数据
extern void Fun_DealCC(uint16_t Left_Number,uint16_t Right_Number);
extern void Fun_DealJJ(uint16_t Left_Number,uint16_t Right_Number);
extern uint16_t Fun_DealWeight(uint16_t Weight);
extern void FunW5500_TestC(uint8_t s);

extern uint8_t Fun_DealParameter(void);
extern void Fun_GetParameter(void);
extern void W5500_ParameterReset(void); //强制IP复位

#ifdef __cplusplus
}
#endif
#endif /* __USER_APP_H */
