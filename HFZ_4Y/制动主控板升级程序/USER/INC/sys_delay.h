/*******************************************************************************
** �ļ���: 		sys_delay.h
** �ļ���ʷ:
** �汾��  		V1.0
** ��������: 
** ����: 		  qgk
** ��������:  2016.09.29    
** �޸���־��
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
    extern unsigned int W5500_Send_Delay_Counter; //W5500������ʱ��������(ms) 
    #ifdef __cplusplus
}
#endif

#endif /* __SYS_DELAY_H */
