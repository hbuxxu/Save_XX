/*******************************************************************************
** 文件名: 		sys_delay.h
** 文件历史:
** 版本：  		V1.0
** 工作环境: 
** 作者: 		  qgk
** 生成日期:  2016.09.29    
** 修改日志：
*******************************************************************************/
#ifndef __SYS_DELAY_H
#define __SYS_DELAY_H

#ifdef __cplusplus
extern "C" 
{
   #endif
	 #include "stdint.h"
    extern void Delay_nNULL(uint32_t nCount);	 
    extern void TimeDelay_ms(unsigned int d);
    extern void NVIC_Configuration_TIM3(void);	 
    extern void Timer3_Init_Config(void);
    extern void DelayTimerInit_Config(void);
	  extern unsigned int Get_uTimer3_Counter(void);
	  extern void uTimer3_Counter_Reset(void);
    extern unsigned int W5500_Send_Delay_Counter; //W5500发送延时计数变量(ms) 
    #ifdef __cplusplus
}
#endif

#endif /* __SYS_DELAY_H */
