/**
  ******************************************************************************
  * @file    Project/Template/stm32f10x_conf.h 
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Library configuration file.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
//#include "stm32f10x_adc.h" 
//#include "stm32f10x_bkp.h" 
//#include "stm32f10x_can.h" 
//#include "stm32f10x_crc.h" 
//#include "stm32f10x_dac.h" 
//#include "stm32f10x_dbgmcu.h" 
//#include "stm32f10x_dma.h" 
#include "stm32f10x_exti.h" 
#include "stm32f10x_flash.h" 
//#include "stm32f10x_fsmc.h"	
#include "stdlib.h" 
#include "stm32f10x_gpio.h"
//#include "stm32f10x_i2c.h" 
#include "stm32f10x_iwdg.h" 
// #include "stm32f10x_pwr.h" 
#include "stm32f10x_rcc.h" 
//#include "stm32f10x_rtc.h" 
//#include "stm32f10x_sdio.h" 
#include "stm32f10x_spi.h" 
#include "stm32f10x_tim.h" 
#include "stm32f10x_usart.h" 
//#include "stm32f10x_wwdg.h" 
#include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */

#include "user_sys.h"						/* 位带驱动 */
#include "ad7663.h"		    //AD7663模拟转换芯片设置
#include "stdio.h"
#include "string.h"
#include "user_data.h"   //全局变量定义  参数设置头文件
#include "user_io.h"       //开入开出配置
#include "user_usart.h"  //调试串口配置
#include "flash_spi.h"    //W25Q64 flash存储芯片头文件

#include "W5500.h"	     //W5500驱动函数库
#include "W5500_APP.h" //W5500外部使用函数库
#include "user_app.h"
#include "Cabrate_Computer.h" //曲线拟合

#include "ad7663_app.h"		    //AD7663 队列采集
#include "HYAES_APP.h"		    //加密头文件

#include "SystemErr.h"         //故障头文件






/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed. 
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM32F10x_CONF_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
