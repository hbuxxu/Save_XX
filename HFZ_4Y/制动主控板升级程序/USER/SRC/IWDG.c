#include "stm32f10x.h"
void IWDG_QConfiguration(void)
{
	  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	  IWDG_SetPrescaler(IWDG_Prescaler_32);
	  IWDG_SetReload(0X7ff);
	  IWDG_ReloadCounter();
 }
