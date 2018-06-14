/***********************************************************************************************************
*文件名：Cabrate_Computer.h
*文件历史：无
*版本号：VER1.0
*日期：2017.6.23
*作者：阚春辉
*说明：曲线拟合算法，与上位机同，C版本
***********************************************************************************************************/

#ifndef _CABRATE_COMPUTER_
#define _CABRATE_COMPUTER_

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f10x.h" 
//=数据结构
#define  MaxCabPoint   7     //最大点数
typedef struct
{
    float Vlt[MaxCabPoint];    //电压值
    float Val[MaxCabPoint];    //标准量值
    float Cof[MaxCabPoint];    //标定系数
    int   CabPointNum;//有效标定点数  
    int   NbofVar;//拟合次数
    int   Chm;//通道号
    int   MinValue;//最小值   
    int   MaxValue;//最大值
}_ADCab;
extern    uint8_t  CabItemIndex;//标定通道号
extern    _ADCab*  CabBuf[16];

extern  uint8_t  Cabrate_Computer(void);//标定系数计算
	
#ifdef __cplusplus
}
#endif

#endif 

