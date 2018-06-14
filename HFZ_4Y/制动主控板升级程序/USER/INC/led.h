/*******************************************************************************
** 文件名: 		led.h
** 文件历史:
** 版本：  		V1.0
** 工作环境: 
** 作者: 		  qgk
** 生成日期:  2016.09.29    
** 修改日志：
*******************************************************************************/
#ifndef _LED_APP_H_
#define _LED_APP_H_
#ifdef __cplusplus
extern "C" 
{
    #endif
    #define RCC_GPIO_sysLED                              RCC_APB2Periph_GPIOD
    #define RCC_GPIO_LED                                 RCC_APB2Periph_GPIOC
    #define GPIO_LED_PORT                                GPIOC
    #define GPIO_LED_sysPORT                             GPIOD
    #define GPIO_SYSLED                                  GPIO_Pin_0   //系统运行指示灯 
    #define GPIO_LED1                                    GPIO_Pin_10  //指示灯1
    #define GPIO_LED2                                    GPIO_Pin_11  //指示灯2  
    #define GPIO_LED3                                    GPIO_Pin_12  //指示灯3  
    #define GPIO_LED_ALL                                 GPIO_LED1 |GPIO_LED2 |GPIO_LED3
    extern void LED_config(void);
    extern void LED1_on(void); 
    extern void LED1_off(void);
    void LED_Control(void);
    #ifdef __cplusplus
}
#endif
#endif 
