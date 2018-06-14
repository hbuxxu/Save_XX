/****************************************************************************
* �ļ���:    sys_delay.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2016.09.29    
* ����:      qgk
* ˵��:      ��ʱ����
*****************************************************************************/
/*=============================================================================
*******�޸���־*******
1����    �ڣ�
��    �㣺
�� �� �ˣ�
������ݣ�
�汾��:
2����    �ڣ�
��    �㣺
�� �� �ˣ�
������ݣ�
�汾��:
================================================================================*/
#include "stm32f10x.h"
#include "sys_delay.h"
#include "W5500.h"
unsigned int uTimer3_Counter=0;             //��ʱ��ʱ����
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ������  : NVIC_Configuration_TIM3
* ��  ��  : STM32�ж�������������
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : ��
* ˵  ��  : 
*******************************************************************************/
void NVIC_Configuration_TIM3(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;						//����NVIC��ʼ���ṹ��
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				//�����ж����ȼ���Ϊ1��������(����0��4λ)
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//�����ж�������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;	//�����������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//������Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��NVIC
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* ������  : void DelayTimerInit_Config
* ��  ��  : ��ȷDelay��ʱ��ʱ����ʼ��
* ��  ��  : ��
* ��  ��  : ��
* ��  ��  : �� 
* ˵  ��  : ��
*******************************************************************************/
void DelayTimerInit_Config(void)
{
    Timer3_Init_Config();
}
/*******************************************************************************
* ������  : Timer3_Init_Config
* ��  ��  : Timer3��ʼ������
* ��  ��  : ��
* ��  ��  : ��
* ��  ��  : �� 
* ˵  ��  : ��     ������
*******************************************************************************/
void Timer3_Init_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	       	//ʹ��Timer3ʱ��
    TIM_TimeBaseStructure.TIM_Period = 9;	                  	    //TIM_TimeBaseStructure.TIM_Period = 9;						
    TIM_TimeBaseStructure.TIM_Prescaler = 7200;                   //TIM_TimeBaseStructure.TIM_Prescaler = 7199;		
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	    	//����ʱ�ӷָ�:TDTS = TIM_CKD_DIV1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				        //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); 				          //ʹ��TIM2ָ�����ж�
    TIM_Cmd(TIM3, ENABLE);  									                    //ʹ��TIMx����
}

/*******************************************************************************
* ������  : TIM3_IRQHandler
* ��  ��  : ��ʱ��3�ж϶Ϸ�����
* ��  ��  : ��
* ��  ��  : ��
* ��  ��  : �� 
* ˵  ��  : ��
*******************************************************************************/
void TIM3_IRQHandler(void)
{  
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {	
        uTimer3_Counter++;
        if(uTimer3_Counter>=100000)
        uTimer3_Counter=0;
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
    }
}
/*******************************************************************************
* ������  : TimeDelay_ms
* ��  ��  : ��ʱ����(ms)
* ��  ��  : d:��ʱϵ������λΪ����
* ��  ��  : ��
* ��  ��  : �� 
* ˵  ��  : ��ʱ������Timer3��ʱ��������1����ļ�����ʵ�ֵ�
*******************************************************************************/
void TimeDelay_ms(unsigned int d)
{
    uTimer3_Counter=0; 
    while((uTimer3_Counter!=0)&&(uTimer3_Counter<d));
}

/******************************************************************************
* ������  : unsigned int Get_ClockTick(void)
* ����    : ���ClockTick����ֵ
* ����    : ��
* ���    : ��
* ����ֵ  : ClockTick
* ˵��    : �ر�LED
*******************************************************************************/
unsigned int Get_uTimer3_Counter(void)
{
    return uTimer3_Counter;
}


	void uTimer3_Counter_Reset(void)
  {
		uTimer3_Counter=0;
   }
