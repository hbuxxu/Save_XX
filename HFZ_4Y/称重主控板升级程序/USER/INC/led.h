/*******************************************************************************
** �ļ���: 		led.h
** �ļ���ʷ:
** �汾��  		V1.0
** ��������: 
** ����: 		  qgk
** ��������:  2016.09.29    
** �޸���־��
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
    #define GPIO_SYSLED                                  GPIO_Pin_0   //ϵͳ����ָʾ�� 
    #define GPIO_LED1                                    GPIO_Pin_10  //ָʾ��1
    #define GPIO_LED2                                    GPIO_Pin_11  //ָʾ��2  
    #define GPIO_LED3                                    GPIO_Pin_12  //ָʾ��3  
    #define GPIO_LED_ALL                                 GPIO_LED1 |GPIO_LED2 |GPIO_LED3
    extern void LED_config(void);
    extern void LED1_on(void); 
    extern void LED1_off(void);
    void LED_Control(void);
    #ifdef __cplusplus
}
#endif
#endif 
