/****************************************************************************
* �ļ���: AD7663_app.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2017.09.15    
* ����:      xx
* ˵��:     AD�ɼ�ѭ��������ʽģ��
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
#include "ad7663_app.h"		    //AD7663 ���вɼ�

/**ѭ������ʹ�ñ���**/
uint16_t AD_SampBuff[MaxCabItem][DLLength];  //����ͨ��ģ�����ɼ�������
uint16_t AD_Ind[MaxCabItem];                             //����ͨ����ǰλ��ָ������
uint32_t AD_SumDisplay[MaxCabItem];  							//����ͨ��ģ�����ɼ��� �����������ʾ
uint32_t AD_SumAdjust[MaxCabItem];  								//����ͨ��ģ�����ɼ���  ���ڱ궨�����ϴ�
uint32_t AD_SumSamp[MaxCabItem];  								//����ͨ��ģ�����ɼ���  ������Ч���ݲɼ�


/*******************************************************************************
* @��������	��TIM4_NVIC_Config
* @����˵�� ����ʱ��4���� �ж�����
* @������� ����
* @������� ����
* @���ز��� ����
*******************************************************************************/
void TIM4_NVIC_Config(void)
{
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);				//ʹ��Timer2ʱ��
		TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;														//7200 - 1;//���÷�Ƶϵ�� 7200��Ƶ
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//��������  0.1ms����һ��
		TIM_TimeBaseStructure.TIM_Period = Tim4_ms*10 - 1;																		//����ʱ�� 1ms
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//ʱ�ӷָ�
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;													//���ü�����ʼֵ
		TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE ); 				                //ʹ��TIM2ָ�����ж�
    TIM_Cmd(TIM4, ENABLE); 																												//ʹ��TIMx����
	
// 		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);              
// 	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Set);				      //�����ж����ȼ���Ϊ1��������(����0��4λ) 
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;				         //�����ж�������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM4_IRQChannelPreemptionPriority;	 //�����������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM4_IRQChannelSubPriority;			                     //������Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				                                                             //ʹ��NVIC
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* @��������	��InitADList
* @����˵�� �� ��ʼ������ͨ������ֵ
* @������� ��chm:ͨ����  InitValue:ADģ�����ɼ�ֵ��0-65535��
* @������� ����
* @���ز��� ����
*******************************************************************************/
void InitADList(uint8_t chm,uint16_t InitValue)
{
		uint16_t i;
		AD_Ind[chm] = 0;
		AD_SumDisplay[chm] = Window_Display * InitValue;
		AD_SumAdjust[chm] = Window_Adjust * InitValue;
		AD_SumSamp[chm] = Window_Samp * InitValue;
	  for(i = 0;i < DLLength;i++)
		{
				AD_SampBuff[chm][i] = InitValue;
		}		
}
/*******************************************************************************
* @��������	��InitADScreen
* @����˵�� �� ��ʼ������ͨ��
* @������� ����
* @������� ����
* @���ز��� ����
*******************************************************************************/
void InitADScreen(void)
{
		uint8_t i;
		for(i = 0;i < MaxCabItem;i++)
		{
				InitADList(i,0);
		}
}
/*******************************************************************************
* @��������	��ADGet_Cycle_Queue
* @����˵�� �� ��ȡ������ָ������Ԫ��ֵ����
* @������� ��chm:ͨ����     Index:ָ�����
* @������� ����
* @���ز��� ����
*******************************************************************************/
uint16_t ADGet_Cycle_Queue(uint8_t chm,int16_t Index)
{
		if(Index >= 0)
		{
				Index %= DLLength;
		}
		else
		{
				while(Index < 0)
				{
						Index += DLLength;
				}
		}
		return(AD_SampBuff[chm][Index]);
}
/*******************************************************************************
* @��������	��ADGet_Data
* @����˵�� �� ��ʱ�ɼ�AD����  ��ʱ�жϵ���
* @������� ��chm:ͨ����  
* @������� ����
* @���ز��� ����
*******************************************************************************/
void ADGet_Data(uint8_t chm)
{
		uint8_t chm1;
		switch(chm)
		{
				case Left_Weight_ID:
																						chm1 = 0;	//��Ӧģ��ͨ��1ͨ��
																						break;
				case Right_Wight_ID:
																						chm1 = 1;	//��Ӧģ��ͨ��2ͨ��
																						break;
				case Left_Brake_ID:
																						chm1 = 2;	//��Ӧģ��ͨ��3ͨ��
																						break;
				case Right_Brake_ID:
																						chm1 = 3;	//��Ӧģ��ͨ��4ͨ��
																						break;
				case All_Weight_ID:
																						chm1 = 4;	//��Ӧģ��ͨ��5ͨ��
																						break;
				case FootPlat_ID:
																						chm1 = 5;	//��Ӧģ��ͨ��6ͨ��
																						break;
				case Left_Move_ID:
																						chm1 = 6;	//��Ӧģ��ͨ��7ͨ��
																						break;
				case Right_Move_ID:
																						chm1 = 7;	//��Ӧģ��ͨ��8ͨ��
																						break;
				default:
																						printf("\r\nError:The ADGet_Data channel is false!!!\r\n");
																						chm1 = 0; //Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																						break;				
		}
		AD_SampBuff[chm1][AD_Ind[chm1]] = ADC_Convert(chm);  //�ɼ�ģ����  ADC_Convert�����Լ���ģ���޸�ͨ����
		/* �ۼӺ� */
		AD_SumDisplay[chm1] += ADGet_Cycle_Queue(chm1,AD_Ind[chm1]) - ADGet_Cycle_Queue(chm1,AD_Ind[chm1]-Window_Display);
		AD_SumAdjust[chm1] += ADGet_Cycle_Queue(chm1,AD_Ind[chm1]) - ADGet_Cycle_Queue(chm1,AD_Ind[chm1]-Window_Adjust);
		AD_SumSamp[chm1] += ADGet_Cycle_Queue(chm1,AD_Ind[chm1]) - ADGet_Cycle_Queue(chm1,AD_Ind[chm1]-Window_Samp);
		AD_Ind[chm1] ++;  //ָ���1
		if(AD_Ind[chm1] >= DLLength)  //ָ��ѭ��
		{
				AD_Ind[chm1] = 0;
		}
}

/*******************************************************************************
* @��������	��ADShow_Display
* @����˵�� �� �ϴ���ʾֵ
* @������� ��chm:ͨ����  
* @������� ����
* @���ز��� ����
*******************************************************************************/
uint16_t ADShow_Display(uint8_t chm)
{
		float temp;
		uint16_t temp1;
		switch(chm)
		{
				case Left_Weight_ID:
																						chm = 0;	//��Ӧģ��ͨ��1ͨ��
																						break;
				case Right_Wight_ID:
																						chm = 1;	//��Ӧģ��ͨ��2ͨ��
																						break;
				case Left_Brake_ID:
																						chm = 2;	//��Ӧģ��ͨ��3ͨ��
																						break;
				case Right_Brake_ID:
																						chm = 3;	//��Ӧģ��ͨ��4ͨ��
																						break;
				case All_Weight_ID:
																						chm = 4;	//��Ӧģ��ͨ��5ͨ��
																						break;
				case FootPlat_ID:
																						chm = 5;	//��Ӧģ��ͨ��6ͨ��
																						break;
				case Left_Move_ID:
																						chm = 6;	//��Ӧģ��ͨ��7ͨ��
																						break;
				case Right_Move_ID:
																						chm = 7;	//��Ӧģ��ͨ��8ͨ��
																						break;
				default:
																						printf("\r\nError:The ADGet_Data channel is false!!!\r\n");
																						chm = 0; //Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																						break;				
		}
	  temp = (float)AD_SumDisplay[chm] / Window_Display;
		temp1 = temp + 0.5;
		return(temp1);
}
/*******************************************************************************
* @��������	��ADShow_Adjust
* @����˵�� �� �ϴ��궨ֵ
* @������� ��chm:ͨ����  
* @������� ����
* @���ز��� ����
*******************************************************************************/
uint16_t ADShow_Adjust(uint8_t chm)
{
		float temp;
		uint16_t temp1;
		switch(chm)
		{
				case Left_Weight_ID:
																						chm = 0;	//��Ӧģ��ͨ��1ͨ��
																						break;
				case Right_Wight_ID:
																						chm = 1;	//��Ӧģ��ͨ��2ͨ��
																						break;
				case Left_Brake_ID:
																						chm = 2;	//��Ӧģ��ͨ��3ͨ��
																						break;
				case Right_Brake_ID:
																						chm = 3;	//��Ӧģ��ͨ��4ͨ��
																						break;
				case All_Weight_ID:
																						chm = 4;	//��Ӧģ��ͨ��5ͨ��
																						break;
				case FootPlat_ID:
																						chm = 5;	//��Ӧģ��ͨ��6ͨ��
																						break;
				case Left_Move_ID:
																						chm = 6;	//��Ӧģ��ͨ��7ͨ��
																						break;
				case Right_Move_ID:
																						chm = 7;	//��Ӧģ��ͨ��8ͨ��
																						break;
				default:
																						printf("\r\nError:The ADGet_Data channel is false!!!\r\n");
																						chm = 0; //Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																						break;				
		}
	  temp = (float)AD_SumAdjust[chm] / Window_Adjust;
		temp1 = temp + 0.5;		
		return(temp1);
}
/*******************************************************************************
* @��������	��ADShow_Samp
* @����˵�� �� ʵ�ʲ�����Чֵ
* @������� ��chm:ͨ����  
* @������� ����
* @���ز��� ����
*******************************************************************************/
uint16_t ADShow_Samp(uint8_t chm)
{
		float temp;
		uint16_t temp1;
		switch(chm)
		{
				case Left_Weight_ID:
																						chm = 0;	//��Ӧģ��ͨ��1ͨ��
																						break;
				case Right_Wight_ID:
																						chm = 1;	//��Ӧģ��ͨ��2ͨ��
																						break;
				case Left_Brake_ID:
																						chm = 2;	//��Ӧģ��ͨ��3ͨ��
																						break;
				case Right_Brake_ID:
																						chm = 3;	//��Ӧģ��ͨ��4ͨ��
																						break;
				case All_Weight_ID:
																						chm = 4;	//��Ӧģ��ͨ��5ͨ��
																						break;
				case FootPlat_ID:
																						chm = 5;	//��Ӧģ��ͨ��6ͨ��
																						break;
				case Left_Move_ID:
																						chm = 6;	//��Ӧģ��ͨ��7ͨ��
																						break;
				case Right_Move_ID:
																						chm = 7;	//��Ӧģ��ͨ��8ͨ��
																						break;
				default:
																						printf("\r\nError:The ADGet_Data channel is false!!!\r\n");
																						chm = 0; //Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																						break;				
		}
	  temp = (float)AD_SumSamp[chm] / Window_Samp;
		temp1 = temp + 0.5;
		return(temp1);
}
/*******************************************************************************
* @��������	��TIM4_ISR
* @����˵�� ����ʱ���жϷ���������ʱ�ɼ�ͨ��ģ��������ڶ�����
* @������� ����
* @������� ����
* @���ز��� ����
*******************************************************************************/
void TIM4_ISR(void)
{
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
		{
				ADGet_Data(Left_Brake_ID);
				ADGet_Data(Right_Brake_ID);         //�����Լ�����Ҫ���ѭ���ɼ���ͨ������
				ADGet_Data(All_Weight_ID);
				TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		}
}




