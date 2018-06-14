/****************************************************************************
* �ļ���: AD7663.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2017.02.09    
* ����:      xx
* ˵��:     
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
#include "ad7663.h"

uint8_t  ADC_FinishFlg;  //ת����ɱ�־ TURE
// static uint16_t TIM2_count; //��ʱ��2������ʱ��������������1ms��ʱ��
// static uint16_t TIM2_count1 = 0;
// static uint16_t TIM2_count2 = 0;
// static uint16_t TIM2_count3 = 0;
/*******************************************************************************
* @��������	��ADC_7663_Gpioinit   
* @����˵�� ��ADת�����ų�ʼ��
* @������� ����
* @������� ����
* @���ز��� ����
*******************************************************************************/
void ADC_7663_Gpioinit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;	
	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_All;   //AD�ɼ����ݶ˿�  PF0-15
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = AD7663_Con;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = AD7663_Busy;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource10);	//PD10
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
		AD7663_nCS = 0;			//CS=0  ����ʹ��
		AD7663_CVT = 1;			//CNVST=1
		AD7663_nRD = 1;			//RD=1
		AD7663_Byte = 0;			//BYTE=0 
}
/*******************************************************************************
* ������  : ADC_7663_NVIC_Config
* ����    : AD�ɼ��ⲿ�ж�����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
/*============================================================================*/
void ADC_7663_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);				//ʹ��Timer2ʱ��
		TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;														//7200 - 1;//���÷�Ƶϵ�� 7200��Ƶ
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//��������  0.1ms����һ��
		TIM_TimeBaseStructure.TIM_Period = 10 - 1;																		//����ʱ�� 1ms
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//ʱ�ӷָ�
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;													//���ü�����ʼֵ
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); 				                //ʹ��TIM2ָ�����ж�
    TIM_Cmd(TIM2, ENABLE); 																												//ʹ��TIMx����
		//��ʱ��3����  ����UDP�㲥
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);				//ʹ��Timer3ʱ��
		TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;														//7200 - 1;//���÷�Ƶϵ�� 7200��Ƶ
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//��������  0.1ms����һ��
		TIM_TimeBaseStructure.TIM_Period = 10 - 1;																		//����ʱ�� 1ms
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//ʱ�ӷָ�
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;													//���ü�����ʼֵ
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); 				                //ʹ��TIM3ָ�����ж�
    TIM_Cmd(TIM3, DISABLE); 																												//ʹ��TIMx����
		
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8010000);
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Set);				           //�����ж����ȼ���Ϊ1��������(����0��4λ)
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				         //�����ж�������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM2_IRQChannelPreemptionPriority;	 //�����������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM2_IRQChannelSubPriority;			         //������Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				     //ʹ��NVIC
    NVIC_Init(&NVIC_InitStructure);
		
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				         //�����ж�������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_IRQChannelPreemptionPriority;	 //�����������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM3_IRQChannelSubPriority;			         //������Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				     //ʹ��NVIC
    NVIC_Init(&NVIC_InitStructure);
	
	  EXTI_ClearITPendingBit(EXTI_Line10);                                     	//ADת������ж�
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI15_10_IRQnChannelPreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI15_10_IRQnChannelSubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* @��������	��ADC_Convert
* @����˵�� ��ADת����ʼ
* @������� ��u8Channel��ͨ��ѡ��1-8
* @������� ����
* @���ز��� ��ת����� 
*******************************************************************************/
uint16_t  ADC_Convert(uint8_t u8Channel)
{	
// 		uint16_t u16Con_Channel[8] = {0x1000,0x3000,0xB000,0x9000,0xF000,0xD000,0x5000,0x7000};	//0-7ͨ��																								
		uint16_t u16ADC_Data;  //ADת�����
		uint16_t u16Channel,j,u16Channel_ID;
		switch(u8Channel)
		{
				case Left_Weight_ID:
																						u16Channel_ID = 0x1000;	//��Ӧģ��ͨ��1ͨ��
																						break;
				case Right_Wight_ID:
																						u16Channel_ID = 0x3000;	//��Ӧģ��ͨ��2ͨ��
																						break;
				case Left_Brake_ID:
																						u16Channel_ID = 0xB000;	//��Ӧģ��ͨ��3ͨ��
																						break;
				case Right_Brake_ID:
																						u16Channel_ID = 0x9000;	//��Ӧģ��ͨ��4ͨ��
																						break;
				case All_Weight_ID:
																						u16Channel_ID = 0xF000;	//��Ӧģ��ͨ��5ͨ��
																						break;
				case FootPlat_ID:
																						u16Channel_ID = 0xD000;	//��Ӧģ��ͨ��6ͨ��
																						break;
				case Left_Move_ID:
																						u16Channel_ID = 0x5000;	//��Ӧģ��ͨ��7ͨ��
																						break;
				case Right_Move_ID:
																						u16Channel_ID = 0x7000;	//��Ӧģ��ͨ��8ͨ��
																						break;
				default:
																						printf("\r\nError:The AD channel is false!!!\r\n");
																						u16Channel_ID = 0x1000; //Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																						break;				
		}
		
	  /* ����ͨ���� */
		u16Channel = GPIO_ReadOutputData(GPIOE);
    u16Channel &= 0x0FFF;	
    u16Channel |= u16Channel_ID;
    GPIO_Write(GPIOE, u16Channel);
		for( j = 0; j < 300; j++ ) ;	//��ʱ��֤ͨ���л��ɹ�
    /* ��λת���Ƿ���ɱ�־λ */
		ADC_FinishFlg = 0;
		/*--����ת��--*/  
    AD7663_CVT = 1;        //CNVST=1     �½�������ת��
    AD7663_nRD = 1;        //RD=1
    AD7663_CVT = 0;      // CNVST=0 ����ת��	 
    AD7663_nRD = 0;      // RD=0 ���Զ�ȡ����
		while(ADC_FinishFlg == 0) ;//�ж��Ƿ�ת�����
		u16ADC_Data = GPIO_ReadInputData(GPIOF);
		AD7663_nRD = 1;
		AD7663_CVT = 1;
		return u16ADC_Data;
}

/*******************************************************************************
* ������  : EXTI15_10_ISR
* ����    : AD�ɼ��ⲿ�жϴ�����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void EXTI15_10_ISR(void)
{
    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
    {					
				ADC_FinishFlg = 1;
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
}
/*******************************************************************************
* ������  : TIM2_ISR
* ����    : ��ʱ��2�жϷ�����
* ����    : ��
* ���    : ��   
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void TIM2_ISR(void)
{
// 		static uint16_t TIM2_count1 = 0;
// 		static uint16_t TIM2_count2 = 0;
// 		static uint16_t TIM2_count3 = 0;
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {	  
					TIM2_count_Delay ++;
					TIM2_count ++;
					TIM2_count1 ++;
					TIM2_count2 ++;
					TIM2_count3 ++;
					TIM2_count4 ++;
					TIM2_1ms ++;
					if(TIM2_count_Delay>=0xFFFFFFF0)
					{
							TIM2_count_Delay = 0;   //��ʱ����ʹ�õı���											
					}
					if(TIM2_count > 100000) //��ʱ֮������
					{							
							if(TIM2_count > 400000)
							{
									Left_Up = IO_CLOSE;
									Right_Up = IO_CLOSE;
									Left_Down = IO_CLOSE;
									Right_Down = IO_CLOSE;
									Clamp_All = IO_CLOSE;
									Protect_Overflow = IO_CLOSE; 
									Hydraumatic_Machine = IO_CLOSE;
									if(TIM2_count>=0xFFFFFFF0)
									{
											TIM2_count = 0;   //��ʱ����ʹ�õı���											
									}
							}
					}
					if(TIM2_count1 >= 1)//1ms
					{
							TIM2_count1 = 0;
							USER_SystemFlag.A.Flag_1ms = 1;
					}				
					if(TIM2_count2 >= 10)//10ms
					{
							TIM2_count2 = 0;
							USER_SystemFlag.A.Flag_10ms = 1;
					}			
					if(TIM2_count3 >= 100)//100ms
					{
							TIM2_count3 = 0;
							USER_SystemFlag.A.Flag_100ms = 1;
					}			
					if(TIM2_count4 >= 330)//500ms
					{
							TIM2_count4 = 0;
							USER_SystemFlag.A.Flag_500ms = 1;
// 							USER_SystemFlag.A.Flag_UDP = 1;
					}			
					if(TIM2_1ms >= 1000)//1000ms
					{
							TIM2_1ms = 0;
							USER_SystemFlag.A.Flag_1s = 1;
							LED14=~LED14;
							LED15=~LED15;
							LED17=~LED17;
							LED18=~LED18;
					}
         TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	
    }
}


/*******************************************************************************
* ������  : TIM3_ISR
* ����    : ��ʱ��3�жϷ����� �жϹ���ź�
* ����    : ��
* ���    : ��   
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void TIM3_ISR(void)
{
		static uint16_t TIM3_count1 = 0;
		static uint16_t TIM3_count2 = 0;
		static uint16_t TIM3_count3 = 0;
		static uint16_t TIM3_count4 = 0;
		static uint8_t u8Before_Status = 0;//IO���ϴ�״̬
		uint8_t u8Now_Status = 0;
		if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
		{
				TIM3_count1 ++;
				TIM3_count2 ++;
				TIM3_count3 ++;
				TIM3_count4 ++;
			  if(TIM3_count4 >= 2000)//2s
				{
						TIM3_count4 = 0;
						if(USER_SystemFlag.A.Flag_Online == 1)//�����������ɹ�
						{
								Network1_StopFlg ++;
								if(Network1_StopFlg >= NETWORK_MAX)
								{
										Network1_StopFlg = 0;
										USER_SystemFlag.A.Flag_Online = 0;
										USER_LEDFlag.A.LED_Online = 0;  				//����ָʾ��
										if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
										{
												USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
												USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
												FunW5500_SendLED(0,0,0);            //�ϴ�����ʾ��
										}
										Write_W5500_SOCK_1Byte(2,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
										Socket_Init(2);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
										S2_State=0;//��������״̬0x00,�˿�����ʧ��
								}
						}
            if(USER_SystemFlag.A.Flag_Standalone == 1) //������������ɹ�
						{
								Network0_StopFlg ++;
								if(Network0_StopFlg >= NETWORK_MAX)
								{
										Network0_StopFlg = 0;
										USER_SystemFlag.A.Flag_Standalone = 0;										
										Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
										Socket_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
										S0_State=0;//��������״̬0x00,�˿�����ʧ��
								}
						}
				}
				if(TIM3_count3 >= 20)
				{
						TIM3_count3 = 0;
						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
						{
								FunW5500_TestC(2);
						}
				}
				if(TIM3_count2 >= 7) //7ms�ж�һ�ι���ź��Ƿ��б仯
				{
						TIM3_count2 = 0;
// 						if(SysParameter.Par12_MachineType == 0)//����̨
						if(1)//����̨
						{
									u8Now_Status = IO_GetInStatus(IN_Weight_ID);
									if(u8Before_Status != u8Now_Status)
									{
											u8Before_Status = u8Now_Status;
											con_TranMI[0] = 4;
											con_TranMI[4] = IN_Weight_ID;
											con_TranMI[5] = u8Now_Status + 0x30;
											W5500_Send_Data(1,con_TranMI);
									}
						}
						else
						{
								 u8Now_Status = IO_GetInStatus(IN_Brake_ID);
								 if(u8Before_Status != u8Now_Status)
								 {
										 u8Before_Status = u8Now_Status;
										 con_TranMI[0] = 4;
										 con_TranMI[4] = IN_Brake_ID;
										 con_TranMI[5] = u8Now_Status + 0x30;
		// 								 con_TranMI[6] = IN_Protect_ID;
		// 								 con_TranMI[7] = IO_GetInStatus(IN_Protect_ID) + 0x30;
										 W5500_Send_Data(1,con_TranMI);
								 }
						}	
				}
				if(TIM3_count1 >= 500)   //ǿ�Ƹ�λ�ж�
				{
						TIM3_count1 = 0;
						if(GPIO_ReadInputDataBit(SYSTEM_REST_PORT,SYSTEM_REST) == Bit_RESET) //��λ����
						{
								Reset_Flag ++;
								if(Reset_Flag>=RESET_MAX)
								{
										//��λ����
										W5500_Reset();
										Reset_Flag = 0;
								}
						}
						else
						{
								Reset_Flag = 0;
						}
// 						USER_SystemFlag.A.Flag_UDP = 1;
// 						if(SysParameter.Par12_MachineType == 0)//����̨
						if(1)
						{
									con_TranMI[0] = 4; //֡��
									con_TranMI[4] = IN_Weight_ID;
									con_TranMI[5] = IO_GetInStatus(IN_Weight_ID) + 0x30;
									W5500_Send_Data(1,con_TranMI);
						}
						else
						{
								 con_TranMI[0] = 4; //֡��
								 con_TranMI[4] = IN_Brake_ID;
								 con_TranMI[5] = IO_GetInStatus(IN_Brake_ID) + 0x30;
// 								 con_TranMI[6] = IN_Protect_ID;
// 								 con_TranMI[7] = IO_GetInStatus(IN_Protect_ID) + 0x30;
								 W5500_Send_Data(1,con_TranMI);
						}	
				}
				TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
		}
}
/*******************************************************************************
* ������  : Delay
* ����    : ��ʱ����(ms)
* ����    : d:��ʱϵ������λΪ����  ���2000
* ���    : ��
* ����    : �� 
* ˵��    : ��ʱ������Timer2��ʱ��������1����ļ�����ʵ�ֵ�
*******************************************************************************/
void Delay_ms(uint16_t time_ms)
{
	TIM2_count_Delay=0; 
	while(TIM2_count_Delay < time_ms)
	{
			IWDG_ReloadCounter();   //���Ź�����
	}
}

/*******************************************************************************
* ������  : ADC_7663_Init
* ����    : ��ʼ������AD7663
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : ����������������AD7663
*******************************************************************************/
void ADC_7663_Init(void)
{
		ADC_7663_Gpioinit();
		ADC_7663_NVIC_Config();	
}
