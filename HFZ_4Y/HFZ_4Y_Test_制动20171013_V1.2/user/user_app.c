/****************************************************************************
* �ļ���: MAIN_APP.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����  :    2016.02.16   
* ���� :     XX
* ˵��:      ���������õ��Ӻ���  �������״̬����
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

#include "user_app.h"


// float VOLTAGE = 10*1000/65535;

/*******************************************************************************
* ������  : void IWDG_Configuration(void)
* ����    : �������Ź����ú���
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : 
*******************************************************************************/
void IWDG_Configuration(void)
{
    	//write 0x5555
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	//40K/256=156HZ(6.4ms)
	IWDG_SetPrescaler(IWDG_Prescaler_256);

	//relead time 2s/6.4MS=781 .it can't be bigger than 0xfff
	IWDG_SetReload(312);

	//reload IWDG
	IWDG_ReloadCounter();

	//IWDG enablie
	IWDG_Enable();
}

/*******************************************************************************
* ������  : Fun_Adjust
* ����    : ϵͳ�궨��״̬
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : 
*******************************************************************************/
void Fun_Adjust(void)
{
		_union_u16_u8_type Voltage_Zero, //����ѹ�ϴ�����
																				 Voltage_Real, //���Ե�ѹ�ϴ�����
																				 Value_Real;		 //ʵʱ����ֵ
		float Voltage_Relative;//��Ե�ѹ
	  _unionType1 Parameter;  //��ʱ�궨ͨ��ϵ��
		float AD_Data_sum=0.0;//ADת������ĺ� ��ƽ��ֵʹ��
//     USER_SystemFlag.A.Flag_Sucess1 = 1;
//     USER_SystemFlag.A.Flag_Standalone = 1;
		while(System_State == Adjust_State)
		{
				W5500_Receive_Deal(); //���ڲ�ѯ����
				switch(AD_Channel_ID)
				{
						case Left_Weight_ID:
																								Voltage_Zero.u16_data = Zero_LeftWeight;	//��Ӧģ��ͨ��1ͨ��
																								memcpy(Parameter.Load_Flash,Left_Weight_par.Load_Flash, 17);
																								break;
						case Right_Wight_ID:
																								Voltage_Zero.u16_data = Zero_RightWight;	//��Ӧģ��ͨ��2ͨ��
																								memcpy(Parameter.Load_Flash,Right_Wight_par.Load_Flash, 17);
																								break;
						case Left_Brake_ID:
																								Voltage_Zero.u16_data = Zero_LeftBrake;	//��Ӧģ��ͨ��3ͨ��
																								memcpy(Parameter.Load_Flash,Left_Brake_par.Load_Flash, 17);
																								break;
						case Right_Brake_ID:
																								Voltage_Zero.u16_data = Zero_RightBrake;	//��Ӧģ��ͨ��4ͨ��
																								memcpy(Parameter.Load_Flash,Right_Brake_par.Load_Flash, 17);
																								break;
						case All_Weight_ID:
																								Voltage_Zero.u16_data = Zero_AllWeight;	//��Ӧģ��ͨ��5ͨ��
																								memcpy(Parameter.Load_Flash,All_Weight_par.Load_Flash, 17);
																								break;
						case FootPlat_ID:
																								Voltage_Zero.u16_data = Zero_FootPlat;	//��Ӧģ��ͨ��6ͨ��
																								memcpy(Parameter.Load_Flash,FootPlat_par.Load_Flash, 17);
																								break;
						case Left_Move_ID:
																								Voltage_Zero.u16_data = Zero_LeftMove;	//��Ӧģ��ͨ��7ͨ��
																								memcpy(Parameter.Load_Flash,Left_Move_par.Load_Flash, 17);
																								break;
						case Right_Move_ID:
																								Voltage_Zero.u16_data = Zero_RightMove;	//��Ӧģ��ͨ��8ͨ��
																								memcpy(Parameter.Load_Flash,Right_Move_par.Load_Flash, 17);
																								break;
						default:
																								printf("\r\nError:The AD channel is false!!!Fun_Adjust\r\n");
																								break;
				}	
				if(USER_SystemFlag.A.Flag_100ms == 1)
				{
						USER_SystemFlag.A.Flag_100ms = 0;
						AD_Data_sum = ADShow_Adjust(AD_Channel_ID);
						Voltage_Real.u16_data = AD_Data_sum*VOLTAGE;
						if(Voltage_Real.u16_data > Voltage_Zero.u16_data)
						{
								Voltage_Relative = Voltage_Real.u16_data - Voltage_Zero.u16_data;
								Value_Real.u16_data = Fun_VoltageConvertValue(AD_Channel_ID,Voltage_Relative);				
						}
						else
						{
								Value_Real.u16_data = 0;
						}
						/* �ϴ��궨���� */
						Tx_Buffer[0] = 8;  //֡���� ��λ
						Tx_Buffer[1] = 0;  //֡���� ��λ
						Tx_Buffer[2] = 'C'; 
						Tx_Buffer[3] = AD_Channel_ID; 
						Tx_Buffer[4] = Voltage_Zero.u8_data[0];
						Tx_Buffer[5] = Voltage_Zero.u8_data[1];
						Tx_Buffer[6] = Voltage_Real.u8_data[0];
						Tx_Buffer[7] = Voltage_Real.u8_data[1];
						Tx_Buffer[8] = Value_Real.u8_data[0];
						Tx_Buffer[9] = Value_Real.u8_data[1];
						if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
						{
								W5500_Send_Data(2,Tx_Buffer);
								if(USER_SystemFlag.A.Flag_Standalone == 1) //�͵������� �ϴ�����
								{
										USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
										USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
										FunW5500_SendLED(0,0,Value_Real.u16_data);
								}
						}
						else
						{
								if(USER_SystemFlag.A.Flag_Standalone == 1) //�͵������� �ϴ�����
								{
										W5500_Send_Data(0,Tx_Buffer);
								}
						}
				}				
		}
}

/*******************************************************************************
* ������  : Fun_Zero
* ����    : ģ��ͨ�����㺯��
* ����    : Channel_ID ����ͨ��  state:�Ƿ�ֱ�ӵ��� 1:ֱ�ӵ���
* ���    : ��
* ����ֵ  : ��
* ˵��    : 
*******************************************************************************/
void Fun_Zero(uint8_t Channel_ID,uint8_t state)
{
		float AD_Data_sum=0.0;//ADת������ĺ� ��ƽ��ֵʹ��
		uint16_t Voltage_Zero=0;//����ѹ

		AD_Data_sum = ADShow_Adjust(Channel_ID);
		Voltage_Zero = AD_Data_sum*VOLTAGE; //ģ����ת���ɵ�ѹ  �Ŵ�1000��
#ifdef _DEBUG //�����ú�����
		printf("\r\n Voltage_Zero%u=%umv \r\n",Channel_ID,Voltage_Zero);	
#endif	
		switch(Channel_ID)
		{
				case Left_Weight_ID:
																						if(state == 1) Zero_LeftWeight = Voltage_Zero;	//��Ӧģ��ͨ��1ͨ��
																					  if(Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftWeight = Voltage_Zero;	//��Ӧģ��ͨ��1ͨ��
																						}
																						break;
				case Right_Wight_ID:
																						if(state == 1) Zero_RightWight = Voltage_Zero;	//��Ӧģ��ͨ��2ͨ��
																						if(Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightWight = Voltage_Zero;	//��Ӧģ��ͨ��2ͨ��
																						}																					
																						break;
				case Left_Brake_ID:
																						if(state == 1) Zero_LeftBrake = Voltage_Zero;	//��Ӧģ��ͨ��3ͨ��
																						if(Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftBrake = Voltage_Zero;	//��Ӧģ��ͨ��3ͨ��
																						}																									
																						break;
				case Right_Brake_ID:
																						if(state == 1) Zero_RightBrake = Voltage_Zero;	//��Ӧģ��ͨ��4ͨ��
																						if(Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightBrake = Voltage_Zero;	//��Ӧģ��ͨ��4ͨ��
																						}
																						break;
				case All_Weight_ID:
																						if(state == 1) Zero_AllWeight = Voltage_Zero;	//��Ӧģ��ͨ��5ͨ��
																						if(Fun_VoltageConvertValue(All_Weight_ID,Voltage_Zero) < 50)
																						{
																								Zero_AllWeight = Voltage_Zero;	//��Ӧģ��ͨ��5ͨ��
																						}																					
																						break;
				case FootPlat_ID:
																						if(state == 1) Zero_FootPlat = Voltage_Zero;	//��Ӧģ��ͨ��6ͨ��
																						if(Fun_VoltageConvertValue(FootPlat_ID,Voltage_Zero) < 50)
																						{
																								Zero_FootPlat = Voltage_Zero;	//��Ӧģ��ͨ��6ͨ��
																						}																						
																						break;
				case Left_Move_ID:
																						if(state == 1) Zero_LeftMove = Voltage_Zero;	//��Ӧģ��ͨ��7ͨ��
																						if(Fun_VoltageConvertValue(Left_Move_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftMove = Voltage_Zero;	//��Ӧģ��ͨ��7ͨ��
																						}
																						break;
				case Right_Move_ID:
																						if(state == 1) Zero_RightMove = Voltage_Zero;	//��Ӧģ��ͨ��8ͨ��
																						if(Fun_VoltageConvertValue(Right_Move_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightMove = Voltage_Zero;	//��Ӧģ��ͨ��8ͨ��
																						}																						
																						break;
				default:
																						printf("\r\nError:The AD channel is false!!!Fun_Zero\r\n");
																						break;
		}
}

/*******************************************************************************
* ������  : Fun_AllWeightTest
* ����    : ���ظ���̨���ز��Ժ���
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : 
*******************************************************************************/
void Fun_AllWeightTest(void)
{
		uint8_t Countdown_time;//����ʱ��ʼֵ

		uint16_t u16Count300=0; 																		 //300����洢����
		uint16_t u16countMax=0; 																		 //�������洢������
		uint16_t Voltage_Relative;																		 //��Ե�ѹ
		float fl32Left_SumWeight=0.0;													     //���������ʱ����


		u16countMax = 0;//��ʼ��
		
		
		if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
		{
				W5500_Send_Data(2,con_TranMF1);//�ϴ�MF1���ϳ��ؼ��
        if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
        {
            if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
            {
                USER_LEDFlag.A.LED_Online = 1;  //����ָʾ��
            }
            USER_LEDFlag.A.LED_Test = 1;  			//���ָʾ��
            USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
            USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
            USER_LEDFlag.A.LED_Weight = 0;  		//����ָʾ��
            USER_LEDFlag.A.LED_Brake = 1;  			//�ƶ�ָʾ��
            FunW5500_SendLED(0,0,0);            //�ϴ�ָʾ��
            Delay_ms(10);
        }
		}
    else
    {
        if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
        {
//             W5500_Send_Data(0,con_TranMF1);//�ϴ�MF1���ϳ��ؼ��
            Delay_ms(10);
        }
    }
		All_DynamicWeight = 0;  //���ϳ���̨��̬��ʾ����
		AllNone_StaticWeight = 0;                  //��ʼ�����ظ������ؽ��
		Countdown_time = SysParameter.Par0_WeightTestTime + 1; //����ʱʱ��
		USER_SystemFlag.A.Flag_1s = 1;//�״ε����ϴ�����ʱ��һ����ʹ��
		while(System_State == StaticWeight_None_State)
		{
				W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 1)
				{
						if(USER_SystemFlag.A.Flag_1s == 1)
						{
								TIM2_1ms = 0;//��ʱ��������
								USER_SystemFlag.A.Flag_1s = 0;
								if(Countdown_time > 0) 
								{
										Countdown_time --;
										if(Countdown_time>0)
										{
												Tx_Buffer[0] = 3;  //֡���� ��λ
												Tx_Buffer[1] = 0;  //֡���� ��λ
												Tx_Buffer[2] = 'M'; 
												Tx_Buffer[3] = '1'; 
												Tx_Buffer[4] = Countdown_time;
												
												if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
												{
														W5500_Send_Data(2,Tx_Buffer);//
                            if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                            {
                                if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
                                {
                                    USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
                                }
                                USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
                                USER_LEDFlag.A.LED_15 = 1;  				    //����������Ƿ�ı� 1��  0����
                                USER_LEDFlag.A.LED_14 = 1;  				    //�����ָʾ���Ƿ�ı� 1��  0����
                                USER_LEDFlag.A.LED_Weight = 0;  			  //����ָʾ��
                                USER_LEDFlag.A.LED_Brake = 1;  			    //�ƶ�ָʾ��
                                FunW5500_SendLED(0,LED_OFF,Countdown_time);//�ϴ�����ʱ
                            }
												}
                        else
                        {
                            if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                            {
//                                 W5500_Send_Data(0,Tx_Buffer);//
                            }
                        }
										}
										else
										{
												System_State = Default_State;
										}
								}
						}						
						if(USER_SystemFlag.A.Flag_10ms == 1)
						{
								USER_SystemFlag.A.Flag_10ms = 0;					
								u16countMax ++;
// 										if(u16countMax >= 300) u16countMax=300;
								/* ����̨���ؼ��� */
								fl32Left_SumWeight = ADShow_Samp(All_Weight_ID);
								fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32Left_SumWeight > Zero_AllWeight)
								{
										Voltage_Relative = fl32Left_SumWeight - Zero_AllWeight;
										u16Left1_TemporaryWeight[u16Count300] = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
								}
								else 
								{
										u16Left1_TemporaryWeight[u16Count300] = 0;
								}															
								u16Count300 ++;
								if(u16Count300 >= 300) u16Count300 = 0;										
						}
						if(USER_SystemFlag.A.Flag_500ms == 1) //��ʱ����Ҫ�ϴ�����
						{
									USER_SystemFlag.A.Flag_500ms = 0;		
                   /* ���� */
                  fl32Left_SumWeight = ADShow_Display(All_Weight_ID);//ȡƽ��ֵ
                  fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE;//���ѹֵ		
                  if(fl32Left_SumWeight > Zero_AllWeight)
                  {
                      Voltage_Relative = fl32Left_SumWeight - Zero_AllWeight;
                      All_DynamicWeight = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
                  }
                  else 
                  {
                      All_DynamicWeight = 0;
                  }
									if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
									{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
// 	  											FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//�ϴ����ϳ�������
												}
									}
									else
									{
											if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
											{
// 													FunW5500_SendGF(0,All_DynamicWeight); //�ϴ����ϳ���
											}
									}
						 }
				}
				else
				{						
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
                 /* ���� */
                fl32Left_SumWeight = ADShow_Display(All_Weight_ID);//ȡƽ��ֵ
                fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE;//���ѹֵ		
                if(fl32Left_SumWeight > Zero_AllWeight)
                {
                    Voltage_Relative = fl32Left_SumWeight - Zero_AllWeight;
                    All_DynamicWeight = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
                }
                else 
                {
                    All_DynamicWeight = 0;
                }
                if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
                {                   
                    if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                    {
                        if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
                        {
                            USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
                        }
                        USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
                        USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                        USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
                        USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
                        USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
                        FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//�ϴ����ϳ�������
                    }
                }
                else
                {
                    if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                    {  
                        W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�
                    }
                }                         												
						}						
						Countdown_time = SysParameter.Par0_WeightTestTime + 1; //����ʱʱ��
						TIM2_1ms = 0;//��ʱ��������
						USER_SystemFlag.A.Flag_1s = 1;//�״ε����ϴ�����ʱ��һ����ʹ��
						u16Count300 = 0;
						u16countMax = 0;//��ʼ��
				}
		}				
		#ifdef _DEBUG //�����ú�����
		printf("\r\n%u\r\n",u16countMax);
		#endif		
		if(u16countMax >= 300) u16countMax = 300;
		DataRank(u16Left1_TemporaryWeight,u16countMax); //����
		#ifdef _DEBUG //�����ú�����
		for(u16Count300=0;u16Count300<u16countMax;u16Count300++)
		{
				printf("\r\nALLWeight=%u\r\n",u16Left1_TemporaryWeight[u16Count300]);
		}
		#endif					
		fl32Left_SumWeight = 0.0;
		if(u16countMax>280)
		{
				for(u16Count300=100;u16Count300<250;u16Count300++) //
				{
						fl32Left_SumWeight += u16Left1_TemporaryWeight[u16Count300];
				}
				AllNone_StaticWeight = fl32Left_SumWeight/150;
	  }
		else
		{
				for(u16Count300=20;u16Count300<(u16countMax-20);u16Count300++) //
				{
						fl32Left_SumWeight += u16Left1_TemporaryWeight[u16Count300];
				}
				AllNone_StaticWeight = fl32Left_SumWeight/(u16countMax-40);
		}
		
		AllNone_StaticWeight = Fun_DealWeight(AllNone_StaticWeight);
		#ifdef _DEBUG //�����ú�����
		printf("\r\nAllNone_StaticWeight=%u\r\n",AllNone_StaticWeight);
		#endif	
		 if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		 {
						W5500_Send_Data(2,con_TranT0);          //����������
						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
						{
								USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
								USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
	  						FunW5500_SendLED(0,LED_OFF,AllNone_StaticWeight);//�ϴ����ϳ�������
						}
						if(USER_SystemFlag.A.Flag_AutoD == 1)
						{
								USER_SystemFlag.A.Flag_AutoD = 0;
								FunW5500_SendDF(2,1,AllNone_StaticWeight);	//DF1
						}
		 }
		 else
		 {
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{
								FunW5500_SendGF(0,AllNone_StaticWeight);
								Delay_ms(20);
// 								W5500_Send_Data(0,con_TranT0); //����������
					}
		 }		
		System_State = ReadData_State; //�ϱߵ���ʱʱ�����Ҳ������״̬�л�
}


/*******************************************************************************
* ������  : Fun_VoltageConvertValue
* ����    : ��Ե�ѹת����ʵʱ����ֵ
* ����    : AD_ID:ģ����ͨ��      Voltage_Relative:��Ե�ѹ
* ���    : ��
* ����ֵ  : ʵʱ����ֵ
* ˵��    : 
*******************************************************************************/
uint16_t Fun_VoltageConvertValue(uint8_t AD_ID,uint16_t Voltage_Relative)
{
		uint16_t Value_Real,Value_Limit;
		_unionType1 Parameter;  //��ʱ�궨ͨ��ϵ��
		float Voltage_Relative1;
		Voltage_Relative1 = Voltage_Relative;
		switch(AD_ID)
		{
				case Left_Weight_ID:
																						Value_Limit = Left_Weight_Limit;
																						memcpy(Parameter.Load_Flash,Left_Weight_par.Load_Flash, 17);
																						break;
				case Right_Wight_ID:
																					  Value_Limit = Right_Wight_Limit;
																						memcpy(Parameter.Load_Flash,Right_Wight_par.Load_Flash, 17);
																						break;
				case Left_Brake_ID:
																					  Value_Limit = Left_Brake_Limit;
																						memcpy(Parameter.Load_Flash,Left_Brake_par.Load_Flash, 17);
																						break;
				case Right_Brake_ID:
																					  Value_Limit = Right_Brake_Limit;
																						memcpy(Parameter.Load_Flash,Right_Brake_par.Load_Flash, 17);
																						break;
				case All_Weight_ID:
																					  Value_Limit = All_Weight_Limit;
																						memcpy(Parameter.Load_Flash,All_Weight_par.Load_Flash, 17);
																						break;
				case FootPlat_ID:
																					  Value_Limit = FootPlat_Limit;
																					  memcpy(Parameter.Load_Flash,FootPlat_par.Load_Flash, 17);
																						break;
				case Left_Move_ID:
																					  Value_Limit = Left_Move_Limit;
																					  memcpy(Parameter.Load_Flash,Left_Move_par.Load_Flash, 17);
																						break;
				case Right_Move_ID:
																					  Value_Limit = Right_Move_Limit;
																					  memcpy(Parameter.Load_Flash,Right_Move_par.Load_Flash, 17);
																						break;
				default:
																						printf("\r\nError:The AD channel ID is false!!!AdConverValue\r\n");																				
																						break;	
		}
		switch(Parameter.Load_Parameter.Parameter_NB)  //��ϴ���
		{
					case 1:
												Value_Real = Parameter.Load_Parameter.Parameter_C1.float_data * Voltage_Relative1 
																					 + Parameter.Load_Parameter.Parameter_C0.float_data + 0.5;
												break;
					case 2:
												Value_Real = Parameter.Load_Parameter.Parameter_C1.float_data * Voltage_Relative1 
																					 + Parameter.Load_Parameter.Parameter_C2.float_data * Voltage_Relative1 * Voltage_Relative1
																					 + Parameter.Load_Parameter.Parameter_C0.float_data + 0.5;
												break;
					case 3:
												Value_Real = Parameter.Load_Parameter.Parameter_C1.float_data * Voltage_Relative1 
																					 + Parameter.Load_Parameter.Parameter_C2.float_data * Voltage_Relative1 * Voltage_Relative1
																					 + Parameter.Load_Parameter.Parameter_C3.float_data * Voltage_Relative1 * Voltage_Relative1 * Voltage_Relative1
																					 + Parameter.Load_Parameter.Parameter_C0.float_data + 0.5;
												break;
					default:
												printf("\r\nError:The Parameter_NB is false!!!AdConverValue\r\n");
												break;																			
		}
		if(Value_Real > Value_Limit) Value_Real = Value_Limit;
		switch(AD_ID)
		{
				case Left_Brake_ID:
													 if(Value_Real>u16Left_BrakeZero)
													 {
															Value_Real -= u16Left_BrakeZero;
													 }
													 else
													 {
															Value_Real = 0;
													 }
													 break;
				case Right_Brake_ID:
													 if(Value_Real>u16Right_BrakeZero)
													 {
															Value_Real -= u16Right_BrakeZero;
													 }
													 else
													 {
															Value_Real = 0;
													 }
													 break;
				default:																			
													 break;	
		}
		return (Value_Real);
}

/*******************************************************************************
* ������  : IO_GetInStatus
* ����    : ��ȡ�����״̬
* ����    : IO_ID:����ͨ��  
													 IN_Weight_ID     		//���ص�λ�źſ���ID
													 IN_Brake_ID						//�ƶ���λ�źſ���ID
													 IN_Raise_ID			  		//������λ�źſ���ID
													 IN_Protect_ID     		//�ǰ屣���źſ���ID
													 IN_LeftStop_ID      	//��ͣ���źſ���ID
													 IN_RightStop_ID    	//��ͣ���źſ���ID
* ���    : ��
* ����ֵ  : ʵʱ����ֵ
* ˵��    : 
*******************************************************************************/
uint8_t IO_GetInStatus(uint8_t IO_ID)
{
		uint8_t INstatus=0;
		switch(IO_ID)
		{
				case IN_Weight_ID:
																				if(PEin(3) == 1)	INstatus = 1;
																				else INstatus = 0;
																				break;
				case IN_Brake_ID:
																				if(PEin(4) == 1)	INstatus = 1;
																				else INstatus = 0;
																				break;
				case IN_Raise_ID:
																				if(PEin(5) == 1)	INstatus = 1;
																				else INstatus = 0;
																				break;
				case IN_Protect_ID:
																				if(PGin(10) == 1)	INstatus = 1;
																				else INstatus = 0;
																				break;
				case IN_LeftStop_ID:
																				if(PCin(13) == 1)	INstatus = 1;
																				else INstatus = 0;
																				break;				
				case IN_RightStop_ID:
																				if(PCin(14) == 1)	INstatus = 1;
																				else INstatus = 0;
																				break;
				default:
										break;
		}
	  return (INstatus);
}
/*******************************************************************************
* ������  : DataRank
* ����    : �������� ��������
* ����    :   *Data ���ݱ���   number ����������
* ���    : ��
* ����ֵ  : ʵʱ����ֵ
* ˵��    : 
*******************************************************************************/
void DataRank(uint16_t *Data,uint16_t number)
{
		uint16_t u16Data_temp;
		uint16_t i,j;
		for(j=1;j<(number-1);j++)
		{
				for(i=0;i<(number-j);i++)
				{
						if(*(Data+i)>*(Data+i+1))
						{
								u16Data_temp = *(Data+i);
								*(Data+i) = *(Data+i+1);
								*(Data+i+1) = u16Data_temp;
						}
				}
		}
}
/*******************************************************************************
* ������  : Get_Parameter
* ����    : �Ӵ洢оƬ��ȡ����
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Get_Parameter(void)
{
		uint8_t u8flash_buffer[21];//�궨ϵ��  ������ʱ�洢����		
		/* �궨ϵ�������� */
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address,17);
		memcpy(Left_Weight_par.Load_Flash,u8flash_buffer,17);	 //��ϵ�����浽����
	
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address,17);
		memcpy(Right_Wight_par.Load_Flash,u8flash_buffer,17);	 //��ϵ�����浽����
	
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Brake_Flash_Address,17);
		memcpy(Left_Brake_par.Load_Flash,u8flash_buffer,17);	 //��ϵ�����浽����
	
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Brake_Flash_Address,17);
		memcpy(Right_Brake_par.Load_Flash,u8flash_buffer,17);	 //��ϵ�����浽����
	
		SPI_FLASH_BufferRead(u8flash_buffer,All_Weight_Flash_Address,17);
		memcpy(All_Weight_par.Load_Flash,u8flash_buffer,17);	 //��ϵ�����浽����
	
		SPI_FLASH_BufferRead(u8flash_buffer,FootPlat_Flash_Address,17);
		memcpy(FootPlat_par.Load_Flash,u8flash_buffer,17);	 //��ϵ�����浽����
	
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Move_Flash_Address,17);
		memcpy(Left_Move_par.Load_Flash,u8flash_buffer,17);	 //��ϵ�����浽����
		
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Move_Flash_Address,17);
		memcpy(Right_Move_par.Load_Flash,u8flash_buffer,17);	 //��ϵ�����浽����
	
		//������ȡ
	 Fun_GetParameter();
	 //����������ȡ
	 SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,25);
	 if(Parameter_Buffer[24] != 0x55)//û������� ���ʼ��IP
	 {
			 Parameter_Buffer[0] = 0;
			 Parameter_Buffer[1] = 0;
			 Parameter_Buffer[2] = 0;
			 Parameter_Buffer[3] = 0;		  //����
		 
			 Parameter_Buffer[4] = '0';
			 Parameter_Buffer[5] = '0';
			 Parameter_Buffer[6] = '0';
			 Parameter_Buffer[7] = '0';      //��������
		 
			 Parameter_Buffer[8] = '0';
			 Parameter_Buffer[9] = '0';
			 Parameter_Buffer[10] = '0';
			 Parameter_Buffer[11] = '0';     //���ذ�IP
 		 
			 Parameter_Buffer[12] = '0';
			 Parameter_Buffer[13] = '0';   //�˿ں�25032
		 
			 Parameter_Buffer[14] = '0';
			 Parameter_Buffer[15] = '0';
			 Parameter_Buffer[16] = '0';
			 Parameter_Buffer[17] = '0';   // �����IP
			 
			 Parameter_Buffer[18] = '0';			
			 Parameter_Buffer[19] = '0';    //����ܶ˿ں�
			 			 
			 Parameter_Buffer[20] = '0';
			 Parameter_Buffer[21] = '0';
			 Parameter_Buffer[22] = '0';
			 Parameter_Buffer[23] = '0';   //UDP�㲥Ŀ�ĵ�ַ
			 
			 Parameter_Buffer[24] = 0x55;
			 SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//��������
			 SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,25);//����ϵ��		
	 }	
	 
	 //IP������ȡ
	 SPI_FLASH_BufferRead(W5500_Parameter,ParameterIP_Flash_Address,27);
	 if(W5500_Parameter[26] != 0x55)//û����IP ���ʼ��IP
	 {
			 W5500_Parameter[0] = 192;
			 W5500_Parameter[1] = 168;
			 W5500_Parameter[2] = 100;
			 W5500_Parameter[3] = 1;		  //����
		 
			 W5500_Parameter[4] = 255;
			 W5500_Parameter[5] = 255;
			 W5500_Parameter[6] = 255;
			 W5500_Parameter[7] = 0;      //��������
		 
			 W5500_Parameter[8] = 192;
			 W5500_Parameter[9] = 168;
			 W5500_Parameter[10] = 100;
			 W5500_Parameter[11] = 4;     //���ذ�IP
 		 
			 W5500_Parameter[12] = 0xC8;
			 W5500_Parameter[13] = 0x61;   //�˿ں�25032
		 
			 W5500_Parameter[14] = 0;
			 W5500_Parameter[15] = 0;
			 W5500_Parameter[16] = 0;
			 W5500_Parameter[17] = 0;   // �����IP
			 
			 W5500_Parameter[18] = 0;			
			 W5500_Parameter[19] = 0;    //����ܶ˿ں�
			 			 
			 W5500_Parameter[20] = 192;
			 W5500_Parameter[21] = 168;
			 W5500_Parameter[22] = 1;
			 W5500_Parameter[23] = 200;   //UDP�㲥Ŀ�ĵ�ַ
			 
			 W5500_Parameter[24] = 0xC7;
			 W5500_Parameter[25] = 0x61;  //UDP�˿ں�
			 
			 W5500_Parameter[26] = 0x55;
			 SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//��������
			 SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//����ϵ��		
	 }	
		
}

/*******************************************************************************
* ������  : Fun_SignalTest
* ����    : �źŲ���
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_SignalTest(void)
{
		float	fl32ADSum_Channen1 = 0.0,
						fl32ADSum_Channen2 = 0.0,
						fl32ADSum_Channen3 = 0.0,
						fl32ADSum_Channen4 = 0.0,
						fl32ADSum_Channen5 = 0.0,
						fl32ADSum_Channen6 = 0.0,
						fl32ADSum_Channen7 = 0.0,
						fl32ADSum_Channen8 = 0.0;  //��ӵĺ�
		uint16_t Voltage_Relative[8];//��Ե�ѹ
		uint16_t DynamicData[8];     //��̬����ֵ
		uint8_t u8Count_100ms;  //�ϴ�Ƶ�ʼ���
		while(System_State == SignalTest_State)
		{
				W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
				if(USER_SystemFlag.A.Flag_500ms == 1)
				{
						USER_SystemFlag.A.Flag_500ms = 0;
						
// 						if(SysParameter.Par12_MachineType == 0)//����̨
						if(0)//Ĭ���ƶ�̨
						{
								/* 1ͨ������ */
								fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);

								fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen1 > Zero_LeftWeight)
								{
										Voltage_Relative[0] = fl32ADSum_Channen1 - Zero_LeftWeight;
										DynamicData[0] = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative[0]);
								}
								else 
								{
										DynamicData[0] = 0;
								}
								/* 2ͨ������ */
								fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
								fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen2 > Zero_RightWight)
								{
										Voltage_Relative[1] = fl32ADSum_Channen1 - Zero_RightWight;
										DynamicData[1] = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative[1]);
								}
								else 
								{
										DynamicData[1] = 0;
								}
								Tx_Buffer[0] = 19;
								Tx_Buffer[1] = 0;
								Tx_Buffer[2] = 'C';
								Tx_Buffer[3] = 2;    //ģ��������
								Tx_Buffer[4] = 1;    //����������
								/* 1ͨ�� */
								Tx_Buffer[5] = Left_Weight_ID;    
								Tx_Buffer[6] = Zero_LeftWeight%256;    
								Tx_Buffer[7] = Zero_LeftWeight/256;    
								Tx_Buffer[8] = (uint32_t)fl32ADSum_Channen1%256;    
								Tx_Buffer[9] = (uint32_t)fl32ADSum_Channen1/256;
								Tx_Buffer[10] = DynamicData[0]%256;
								Tx_Buffer[11] = DynamicData[0]/256;
								/* 2ͨ�� */
								Tx_Buffer[12] = Right_Wight_ID;    
								Tx_Buffer[13] = Zero_RightWight%256;    
								Tx_Buffer[14] = Zero_RightWight/256;    
								Tx_Buffer[15] = (uint32_t)fl32ADSum_Channen2%256;    
								Tx_Buffer[16] = (uint32_t)fl32ADSum_Channen2/256;
								Tx_Buffer[17] = DynamicData[1]%256;
								Tx_Buffer[18] = DynamicData[1]/256;
								/* ������ */
								Tx_Buffer[19] = IN_Weight_ID;    
								Tx_Buffer[20] = IO_GetInStatus(IN_Weight_ID)+0x30; 
						}
						else if((SysParameter.Par12_MachineType == 1) || (SysParameter.Par12_MachineType == 2))//��ͨ�ƶ�̨�����������ƶ�̨
						{
								/* 3ͨ������ */
								fl32ADSum_Channen3 = ADShow_Display(Left_Brake_ID);
								fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen3 > Zero_LeftBrake)
								{
										Voltage_Relative[2] = fl32ADSum_Channen3 - Zero_LeftBrake;
										DynamicData[2] = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative[2]);
								}
								else 
								{
										DynamicData[2] = 0;
								}
								/* 4ͨ������ */
								fl32ADSum_Channen4 = ADShow_Display(Right_Brake_ID);
								fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen4 > Zero_RightBrake)
								{
										Voltage_Relative[3] = fl32ADSum_Channen4 - Zero_RightBrake;
										DynamicData[3] = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative[3]);
								}
								else 
								{
										DynamicData[3] = 0;
								}
								/* 5ͨ������ */
								fl32ADSum_Channen5 = ADShow_Display(All_Weight_ID);
								fl32ADSum_Channen5 = fl32ADSum_Channen5*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen5 > Zero_AllWeight)
								{
										Voltage_Relative[4] = fl32ADSum_Channen5 - Zero_AllWeight;
										DynamicData[4] = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative[4]);
								}
								else 
								{
										DynamicData[4] = 0;
								}
								/* 6ͨ������ */
								fl32ADSum_Channen6 = ADShow_Display(FootPlat_ID);
								fl32ADSum_Channen6 = fl32ADSum_Channen6*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen6 > Zero_FootPlat)
								{
										Voltage_Relative[5] = fl32ADSum_Channen6 - Zero_FootPlat;
										DynamicData[5] = Fun_VoltageConvertValue(FootPlat_ID,Voltage_Relative[5]);
								}
								else 
								{
										DynamicData[5] = 0;
								}
								Tx_Buffer[0] = 41;
								Tx_Buffer[1] = 0;
								Tx_Buffer[2] = 'C';
								Tx_Buffer[3] = 4;    //ģ��������
								Tx_Buffer[4] = 5;    //����������
								/* 3ͨ�� */
								Tx_Buffer[5] = Left_Brake_ID;    
								Tx_Buffer[6] = Zero_LeftBrake%256;    
								Tx_Buffer[7] = Zero_LeftBrake/256;    
								Tx_Buffer[8] = (uint32_t)fl32ADSum_Channen3%256;    
								Tx_Buffer[9] = (uint32_t)fl32ADSum_Channen3/256;
								Tx_Buffer[10] = DynamicData[2]%256;
								Tx_Buffer[11] = DynamicData[2]/256;
								/* 4ͨ�� */
								Tx_Buffer[12] = Right_Brake_ID;    
								Tx_Buffer[13] = Zero_RightBrake%256;    
								Tx_Buffer[14] = Zero_RightBrake/256;    
								Tx_Buffer[15] = (uint32_t)fl32ADSum_Channen4%256;    
								Tx_Buffer[16] = (uint32_t)fl32ADSum_Channen4/256;
								Tx_Buffer[17] = DynamicData[3]%256;
								Tx_Buffer[18] = DynamicData[3]/256;
								/* 5ͨ�� */
								Tx_Buffer[19] = All_Weight_ID;    
								Tx_Buffer[20] = Zero_AllWeight%256;    
								Tx_Buffer[21] = Zero_AllWeight/256;    
								Tx_Buffer[22] = (uint32_t)fl32ADSum_Channen5%256;    
								Tx_Buffer[23] = (uint32_t)fl32ADSum_Channen5/256;
								Tx_Buffer[24] = DynamicData[4]%256;
								Tx_Buffer[25] = DynamicData[4]/256;
								/* 6ͨ�� */
								Tx_Buffer[26] = FootPlat_ID;    
								Tx_Buffer[27] = Zero_FootPlat%256;    
								Tx_Buffer[28] = Zero_FootPlat/256;    
								Tx_Buffer[29] = (uint32_t)fl32ADSum_Channen6%256;    
								Tx_Buffer[30] = (uint32_t)fl32ADSum_Channen6/256;
								Tx_Buffer[31] = DynamicData[5]%256;
								Tx_Buffer[32] = DynamicData[5]/256;
								/* ������ */										
								Tx_Buffer[33] = IN_Brake_ID;    
								Tx_Buffer[34] = IO_GetInStatus(IN_Brake_ID)+0x30;   
								
								Tx_Buffer[35] = IN_Raise_ID;
								Tx_Buffer[36] = IO_GetInStatus(IN_Raise_ID)+0x30;
								
								Tx_Buffer[37] = IN_Protect_ID;
								Tx_Buffer[38] = IO_GetInStatus(IN_Protect_ID)+0x30;
								
								Tx_Buffer[39] = IN_LeftStop_ID;
								Tx_Buffer[40] = IO_GetInStatus(IN_LeftStop_ID)+0x30;
								
								Tx_Buffer[41] = IN_RightStop_ID;
								Tx_Buffer[42] = IO_GetInStatus(IN_RightStop_ID)+0x30;
						}
						else if(SysParameter.Par12_MachineType == 3)
						{
								/* 3ͨ������ */
								fl32ADSum_Channen3 = ADShow_Display(Left_Brake_ID);
								fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen3 > Zero_LeftBrake)
								{
										Voltage_Relative[2] = fl32ADSum_Channen3 - Zero_LeftBrake;
										DynamicData[2] = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative[2]);
								}
								else 
								{
										DynamicData[2] = 0;
								}
								/* 4ͨ������ */
								fl32ADSum_Channen4 = ADShow_Display(Right_Brake_ID);
								fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen4 > Zero_RightBrake)
								{
										Voltage_Relative[3] = fl32ADSum_Channen4 - Zero_RightBrake;
										DynamicData[3] = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative[3]);
								}
								else 
								{
										DynamicData[3] = 0;
								}
								/* 5ͨ������ */
								fl32ADSum_Channen5 = ADShow_Display(All_Weight_ID);
								fl32ADSum_Channen5 = fl32ADSum_Channen5*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen5 > Zero_AllWeight)
								{
										Voltage_Relative[4] = fl32ADSum_Channen5 - Zero_AllWeight;
										DynamicData[4] = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative[4]);
								}
								else 
								{
										DynamicData[4] = 0;
								}
								/* 6ͨ������ */
								fl32ADSum_Channen6 = ADShow_Display(FootPlat_ID);
								fl32ADSum_Channen6 = fl32ADSum_Channen6*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen6 > Zero_FootPlat)
								{
										Voltage_Relative[5] = fl32ADSum_Channen6 - Zero_FootPlat;
										DynamicData[5] = Fun_VoltageConvertValue(FootPlat_ID,Voltage_Relative[5]);
								}
								else 
								{
										DynamicData[5] = 0;
								}
								/* 7ͨ������ */
								fl32ADSum_Channen7 = ADShow_Display(Left_Move_ID);
								fl32ADSum_Channen7 = fl32ADSum_Channen7*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen7 > Zero_LeftMove)
								{
										Voltage_Relative[6] = fl32ADSum_Channen7 - Zero_LeftMove;
										DynamicData[6] = Fun_VoltageConvertValue(Left_Move_ID,Voltage_Relative[6]);
								}
								else 
								{
										DynamicData[6] = 0;
								}
								/* 8ͨ������ */
								fl32ADSum_Channen8 = ADShow_Display(Right_Move_ID);
								fl32ADSum_Channen8 = fl32ADSum_Channen8*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen8 > Zero_RightMove)
								{
										Voltage_Relative[7] = fl32ADSum_Channen8 - Zero_RightMove;
										DynamicData[7] = Fun_VoltageConvertValue(Right_Move_ID,Voltage_Relative[7]);
								}
								else 
								{
										DynamicData[7] = 0;
								}
								Tx_Buffer[0] = 55;
								Tx_Buffer[1] = 0;
								Tx_Buffer[2] = 'C';
								Tx_Buffer[3] = 6;    //ģ��������
								Tx_Buffer[4] = 5;    //����������
								/* 3ͨ�� */
								Tx_Buffer[5] = Left_Brake_ID;    
								Tx_Buffer[6] = Zero_LeftBrake%256;    
								Tx_Buffer[7] = Zero_LeftBrake/256;    
								Tx_Buffer[8] = (uint32_t)fl32ADSum_Channen3%256;    
								Tx_Buffer[9] = (uint32_t)fl32ADSum_Channen3/256;
								Tx_Buffer[10] = DynamicData[2]%256;
								Tx_Buffer[11] = DynamicData[2]/256;
								/* 4ͨ�� */
								Tx_Buffer[12] = Right_Brake_ID;    
								Tx_Buffer[13] = Zero_RightBrake%256;    
								Tx_Buffer[14] = Zero_RightBrake/256;    
								Tx_Buffer[15] = (uint32_t)fl32ADSum_Channen4%256;    
								Tx_Buffer[16] = (uint32_t)fl32ADSum_Channen4/256;
								Tx_Buffer[17] = DynamicData[3]%256;
								Tx_Buffer[18] = DynamicData[3]/256;
								/* 5ͨ�� */
								Tx_Buffer[19] = All_Weight_ID;    
								Tx_Buffer[20] = Zero_AllWeight%256;    
								Tx_Buffer[21] = Zero_AllWeight/256;    
								Tx_Buffer[22] = (uint32_t)fl32ADSum_Channen5%256;    
								Tx_Buffer[23] = (uint32_t)fl32ADSum_Channen5/256;
								Tx_Buffer[24] = DynamicData[4]%256;
								Tx_Buffer[25] = DynamicData[4]/256;
								/* 6ͨ�� */
								Tx_Buffer[26] = FootPlat_ID;    
								Tx_Buffer[27] = Zero_FootPlat%256;    
								Tx_Buffer[28] = Zero_FootPlat/256;    
								Tx_Buffer[29] = (uint32_t)fl32ADSum_Channen6%256;    
								Tx_Buffer[30] = (uint32_t)fl32ADSum_Channen6/256;
								Tx_Buffer[31] = DynamicData[5]%256;
								Tx_Buffer[32] = DynamicData[5]/256;
								/* 7ͨ�� */
								Tx_Buffer[33] = Left_Move_ID;    
								Tx_Buffer[34] = Zero_LeftMove%256;    
								Tx_Buffer[35] = Zero_LeftMove/256;    
								Tx_Buffer[36] = (uint32_t)fl32ADSum_Channen7%256;    
								Tx_Buffer[37] = (uint32_t)fl32ADSum_Channen7/256;
								Tx_Buffer[38] = DynamicData[6]%256;
								Tx_Buffer[39] = DynamicData[6]/256;
								/* 8ͨ�� */
								Tx_Buffer[40] = Right_Move_ID;    
								Tx_Buffer[41] = Zero_RightMove%256;    
								Tx_Buffer[42] = Zero_RightMove/256;    
								Tx_Buffer[43] = (uint32_t)fl32ADSum_Channen8%256;    
								Tx_Buffer[44] = (uint32_t)fl32ADSum_Channen8/256;
								Tx_Buffer[45] = DynamicData[7]%256;
								Tx_Buffer[46] = DynamicData[7]/256;
								/* ������ */										
								Tx_Buffer[47] = IN_Brake_ID;    
								Tx_Buffer[48] = IO_GetInStatus(IN_Brake_ID)+0x30;   
								
								Tx_Buffer[49] = IN_Raise_ID;
								Tx_Buffer[50] = IO_GetInStatus(IN_Raise_ID)+0x30;
								
								Tx_Buffer[51] = IN_Protect_ID;
								Tx_Buffer[52] = IO_GetInStatus(IN_Protect_ID)+0x30;
								
								Tx_Buffer[53] = IN_LeftStop_ID;
								Tx_Buffer[54] = IO_GetInStatus(IN_LeftStop_ID)+0x30;
								
								Tx_Buffer[55] = IN_RightStop_ID;
								Tx_Buffer[56] = IO_GetInStatus(IN_RightStop_ID)+0x30;
						}																																																																		
				}
				if(USER_SystemFlag.A.Flag_100ms == 1)
				{
						USER_SystemFlag.A.Flag_100ms = 0;
						u8Count_100ms ++;
						if(u8Count_100ms >= Signal_TransferTime)
						{
								W5500_Send_Data(2,Tx_Buffer);
								u8Count_100ms = 0;
						}
				}
		}
}

/*******************************************************************************
* ������  : Fun_DynamoStart
* ����    : �������
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_DynamoStart(void)  //û�и�ȡֵ����
{
		float  fl32ADSum_Channen3 = 0.0,
						 fl32ADSum_Channen4 = 0.0;
		uint16_t Voltage_Relative;		//��Ե�ѹ
        uint8_t u8Protect_CountFlg = 0;    //����������ֵ����
		uint8_t u8Protect_CountFlg2 = 0;  //�ҵ��������ֵ����
		uint16_t u16Protect_Limit1; //�������ǰ��ͬ�ı�����ֵ
		
//		Pump_UpDown = IO_OPEN;  //��������֤�����������½�
		if(USER_LEDFlag.A.LED_Down!=1)
		{
				Pump_UpDown = IO_OPEN;				
        USER_LEDFlag.A.LED_Down = 1;         //������
        USER_LEDFlag.A.LED_Up = 1;           //������
				Delay_ms(2000);
		}    
// 		W5500_Send_Data(0,con_TranMD);  //�ϴ�MD����������״̬
		/* ������ֵ���� */
		if(Left_StaticWeight<StaticWeight_Init)
		{
				Left_StaticWeight = StaticWeight_Init;
// 				W5500_Send_Data(0,con_TranT9);//����̫Сʹ��Ĭ������
		}
		if(Right_StaticWeight<StaticWeight_Init)
		{
				Right_StaticWeight = StaticWeight_Init;
// 				W5500_Send_Data(0,con_TranT9);//����̫Сʹ��Ĭ������
		}
		if(System_State == BrakeTest_Full_State)
		{
         if(AllFull_StaticWeight > 100)
         {
            u16Protect_Limit = AllFull_StaticWeight*4/2;//������ֵΪ���ص�40%
         }
         else
         {
            u16Protect_Limit = (Left_StaticWeight + Right_StaticWeight)*4/2;//������ֵΪ���ص�40%
         }
		}
		else
		{
				 u16Protect_Limit = (Left_StaticWeight + Right_StaticWeight)*4/2;//������ֵΪ���ص�40%
		}
		if(u16Protect_Limit < (SysParameter.Par13_UpHeight*10))
		{
			u16Protect_Limit = SysParameter.Par13_UpHeight*10;
		}
		u16Protect_Limit1 = u16Protect_Limit;
        if(u16Protect_Limit1 < 2000) u16Protect_Limit1 = 2000;
		
		//���������
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط� 
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{
// 								USER_LEDFlag.A.LED_TJUp = 1;  					//̨�ܾ�ָʾ��
// 								USER_LEDFlag.A.LED_TJDown = 0;  		//̨�ܽ�ָʾ��
								USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
								USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
								FunW5500_SendLED(0,0,0);//�ϴ��ƶ���
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{						
// 							 FunW5500_SendGB(0,0,0); //�ϴ��ƶ���
					}
		}  		
		if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
		{
				Left_DynamicBrake = 0;
				Right_DynamicBrake = 0;
				if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
				{
						W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
				}
				if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
				{
						W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
				}
				System_State = Default_State;          //����Ĭ��״̬ �ƶ�����
				Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
				Left_Machine = IO_CLOSE;							//����ֹͣ
				Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
				Machine_Switch = IO_CLOSE;					//������
				USER_SystemFlag.A.Flag_Ldynamo = 0;
				USER_SystemFlag.A.Flag_Rdynamo = 0;
				USER_SystemFlag.A.Flag_Switch = 0;
				USER_SystemFlag.A.Flag_ST = 0;
				SysParameter.DynamoStart_Flag = 0;
				return;
		}
		if(USER_SystemFlag.A.Flag_TestC1 == 1)
		{
				USER_SystemFlag.A.Flag_TestC = 1;
		}
		//�ƶ���ǿ������
		Fun_Zero(Left_Brake_ID,1);//�ж����ƶ��Ƿ����
		Fun_Zero(Right_Brake_ID,1);//�ж����ƶ��Ƿ����
		Delay_ms(2000);        // 20180503
		
		Speed_Control = IO_OPEN; 	//���ٰ�St�ź�����
		USER_SystemFlag.A.Flag_ST = 1;//ͣ����������־
		Left_Machine = IO_OPEN;			//��������
		USER_SystemFlag.A.Flag_Ldynamo = 1;//����������־
		TIM2_count=0; 
		while(TIM2_count < 1000)  		//1S��ʱ����
		{
				W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
						{
								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
						{
								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
						}
						System_State = Default_State;          //����Ĭ��״̬ �ƶ�����
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
					  return;
				}
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;
						/* 3ͨ�����Ƽ��� */
						fl32ADSum_Channen3 = ADShow_Samp(Left_Brake_ID);
						fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32ADSum_Channen3 > Zero_LeftBrake)
						{
								Voltage_Relative = fl32ADSum_Channen3 - Zero_LeftBrake;
								Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Left_DynamicBrake = 0;
						}
						/* 4ͨ�����Ƽ��� */
						fl32ADSum_Channen4 = ADShow_Samp(Right_Brake_ID);
						fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32ADSum_Channen4 > Zero_RightBrake)
						{
								Voltage_Relative = fl32ADSum_Channen4 - Zero_RightBrake;
								Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Right_DynamicBrake = 0;
						}																				
						if(Left_DynamicBrake > u16Protect_Limit1)  //���ͱ���
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX1)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;     //�������״̬
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Machine_Switch = IO_CLOSE;					//������			
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT1(2,Left_DynamicBrake); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																FunW5500_SendLED(0,Left_DynamicBrake,0);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake); //�������ʹ�
													}
										}
										return;
								}
						 }		
						 else
						 {
								 u8Protect_CountFlg = 0;
						 }
				}
		}
		Right_Machine = IO_OPEN;			//�ҵ������
		USER_SystemFlag.A.Flag_Rdynamo = 1;//�ҵ��������־
		TIM2_count=0; 
		while(TIM2_count < 1000)  		//1S��ʱ����
		{
				W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
						{
								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
						{
								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
						}
						System_State = Default_State;          //����Ĭ��״̬ �ƶ�����
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
					  return;
				}
				if(USER_SystemFlag.A.Flag_10ms == 1) //������
				{
						USER_SystemFlag.A.Flag_10ms = 0;
						/* 3ͨ�����Ƽ��� */
						fl32ADSum_Channen3 = ADShow_Samp(Left_Brake_ID);
						fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32ADSum_Channen3 > Zero_LeftBrake)
						{
								Voltage_Relative = fl32ADSum_Channen3 - Zero_LeftBrake;
								Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Left_DynamicBrake = 0;
						}
						/* 4ͨ�����Ƽ��� */
						fl32ADSum_Channen4 = ADShow_Samp(Right_Brake_ID);
						fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32ADSum_Channen4 > Zero_RightBrake)
						{
								Voltage_Relative = fl32ADSum_Channen4 - Zero_RightBrake;
								Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Right_DynamicBrake = 0;
						}															
						if(Left_DynamicBrake > u16Protect_Limit1)  //���ͱ���
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;          //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;					//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT1(2,Left_DynamicBrake); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																FunW5500_SendLED(0,Left_DynamicBrake,0);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake); //�������ʹ�
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake > u16Protect_Limit1)  //���ͱ���
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX1)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;         //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;						//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT2(2,Right_DynamicBrake); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																FunW5500_SendLED(0,0,Right_DynamicBrake);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake); //�������ʹ�
													}
										}
										return;
								}
						}
						else
						{
								u8Protect_CountFlg2 = 0;
						}						
				}
		}
		Machine_Switch = IO_OPEN;			//������
		USER_SystemFlag.A.Flag_Switch = 1;//������������־
		fl32ADSum_Channen3 = 0.0;
		fl32ADSum_Channen4 = 0.0;
		TIM2_count=0; 
		while(TIM2_count < 1000)  		//1S��ʱ����
		{
				W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
						{
								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
						{
								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
						}
						System_State = Default_State;          //����Ĭ��״̬ �ƶ�����
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
					  return;
				}
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;
					   /* 3ͨ�����Ƽ��� */
						fl32ADSum_Channen3 = ADShow_Samp(Left_Brake_ID);
						fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32ADSum_Channen3 > Zero_LeftBrake)
						{
								Voltage_Relative = fl32ADSum_Channen3 - Zero_LeftBrake;
								Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Left_DynamicBrake = 0;
						}
						/* 4ͨ�����Ƽ��� */
						fl32ADSum_Channen4 = ADShow_Samp(Right_Brake_ID);
						fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32ADSum_Channen4 > Zero_RightBrake)
						{
								Voltage_Relative = fl32ADSum_Channen4 - Zero_RightBrake;
								Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Right_DynamicBrake = 0;
						}										
						if(Left_DynamicBrake > u16Protect_Limit1)  //���ͱ���
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;          //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;					//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT1(2,Left_DynamicBrake); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																FunW5500_SendLED(0,Left_DynamicBrake,0);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake); //�������ʹ�
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake > u16Protect_Limit1)  //���ͱ���
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;         //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;						//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT2(2,Right_DynamicBrake); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																FunW5500_SendLED(0,0,Right_DynamicBrake);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake); //�������ʹ�
													}
										}
										return;
								}
						}
						else
						{
								u8Protect_CountFlg2 = 0;
						}
				}
		}
		Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ �����������Ӳͣ���ж�
		USER_SystemFlag.A.Flag_ST = 0;    //ͣ����������־
// 		System_State = Default_State;         //����Ĭ��״̬ �ƶ�����
		SysParameter.DynamoStart_Flag = 1;//����ʱ����Ӳͣ���ж�
}
/*******************************************************************************
* ������  : Fun_Stable()
* ����    : ����״̬
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_Stable(void)
{
		float	fl32ADSum_Channen1 = 0.0,
						fl32ADSum_Channen2 = 0.0,
						fl32ADSum_Channen3 = 0.0,
						fl32ADSum_Channen4 = 0.0,
						fl32ADSum_Channen5 = 0.0;      //�ۼӺ�
		uint16_t Voltage_Relative;									 //��Ե�ѹ
// 		Fun_Zero(Left_Weight_ID,0);//�ж������Ƿ����
// 		Fun_Zero(Right_Wight_ID,0);//�ж������Ƿ����
		Fun_Zero(Left_Brake_ID,1);//�ж����ƶ��Ƿ����
		Fun_Zero(Right_Brake_ID,1);//�ж����ƶ��Ƿ����
		
		Fun_Zero(All_Weight_ID,0);//�жϸ��ϳ����Ƿ����
		while(System_State == Default_State)
		{
				W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
// 				if(SysParameter.Par12_MachineType == 0) //����̨
// 				{
// 						if(USER_SystemFlag.A.Flag_500ms == 1)
// 						{
// 									USER_SystemFlag.A.Flag_500ms = 0;
// 									/*����*/
// 									fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);
// 									fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //ת���ɵ�ѹֵ
// 									if(fl32ADSum_Channen1 > Zero_LeftWeight)
// 									{
// 											Voltage_Relative = fl32ADSum_Channen1 - Zero_LeftWeight;
// 											Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
// 									}
// 									else 
// 									{
// 											Left_DynamicWeight = 0;
// 									}
// 									/*����*/
// 									fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
// 									fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //ת���ɵ�ѹֵ
// 									if(fl32ADSum_Channen2 > Zero_RightWight)
// 									{
// 											Voltage_Relative = fl32ADSum_Channen2 - Zero_RightWight;
// 											Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
// 									}
// 									else 
// 									{
// 											Right_DynamicWeight = 0;
// 									}											
// 									if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
// 									{									
// 											if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
// 											{
// 													if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
// 													{
// 															USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
// 													}
// 													USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
// 													USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
// 													USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
// 													USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
// 													FunW5500_SendLED(0,Left_DynamicWeight,Right_DynamicWeight);	
// 											}
// 									}
// 									else
// 									{
// 											if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
// 											{
// 													FunW5500_SendGW(0,Left_DynamicWeight,Right_DynamicWeight); //�ϴ���̬��ʾ����
// 											}											
// 									}
// 						}			
// 				}
// 				else  //�ƶ�̨
				{						
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
								/* ���Ƽ��� */
								fl32ADSum_Channen3 = ADShow_Display(Left_Brake_ID);
								fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32ADSum_Channen3 > Zero_LeftBrake)
								{
										Voltage_Relative = fl32ADSum_Channen3 - Zero_LeftBrake;
										Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
										Left_DynamicBrake += 5;
										Left_DynamicBrake /= 10;
										Left_DynamicBrake *= 10;   //����daN
								}
								else 
								{
										Left_DynamicBrake = 0;
								}
							
								/* ���Ƽ��� */
								fl32ADSum_Channen4 = ADShow_Display(Right_Brake_ID);//ȡƽ��ֵ
								fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE;//���ѹֵ		
								if(fl32ADSum_Channen4 > Zero_RightBrake)
								{
										Voltage_Relative = fl32ADSum_Channen4 - Zero_RightBrake;
										Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
										Right_DynamicBrake += 5;
										Right_DynamicBrake /= 10;
										Right_DynamicBrake *= 10;  //����daN
								}
								else 
								{
										Right_DynamicBrake = 0;
								}							
								if(SysParameter.Par12_MachineType > 1)//�����ƶ�̨ʱ�и��ϳ���
								{
										/* ���� */
										fl32ADSum_Channen5 = ADShow_Display(All_Weight_ID);//ȡƽ��ֵ
										fl32ADSum_Channen5 = fl32ADSum_Channen5*VOLTAGE;//���ѹֵ		
										if(fl32ADSum_Channen5 > Zero_AllWeight)
										{
												Voltage_Relative = fl32ADSum_Channen5 - Zero_AllWeight;
												All_DynamicWeight = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
										}
										else 
										{
												All_DynamicWeight = 0;
										}
								 }	
								 if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
								 {
											if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
											{
													if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
													{
															USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
													}
                          USER_LEDFlag.A.LED_Test = 0;  					//���ָʾ��
													USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
													USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
													USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
													USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
													FunW5500_SendLED(0,Left_DynamicBrake,Right_DynamicBrake);	
											}
// 											FunW5500_SendLED(2,Left_DynamicBrake,Right_DynamicBrake);	
								 }
								 else
								 {
											if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
											{
													FunW5500_SendGB(0,Left_DynamicBrake,Right_DynamicBrake);	
											}
								 }
						}																
				}				
		}
}
/*******************************************************************************
* ������  : Fun_TestZZ()
* ����    : ���Ͳ���
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_TestZZ(void)
{
		uint8_t Countdown_time;//����ʱ��ʼֵ
		uint8_t u8Left_flg=0,        //��ͣ����־
							  u8Right_flg=0;			//��ͣ����־
		uint16_t u16Count800=0; //800����洢����
		uint16_t u16countMax=0; //�������洢������
		uint16_t Voltage_Relative;//��Ե�ѹ
		float fl32Left_SumBrake=0,
					 fl32Right_SumBrake=0;//�ƶ������ʱ����
		uint8_t u8Protect_CountFlg = 0;    //����������ֵ����
		uint8_t u8Protect_CountFlg2 = 0;  //�ҵ��������ֵ����
    
    float fl32Left_SumBrake1=0,
					  fl32Right_SumBrake1=0;																	  //�ƶ������ʱ����		�ϴ���ʾʹ�� 			
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1;			
		u16countMax = 0;//��ʼ��		
		if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
		{
				W5500_Send_Data(2,con_TranMZ);//�ϴ����ͼ��
        if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
        {
              if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
              {
                  USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
              }
              USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
              USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
              USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��					
              USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
              USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
// 							FunW5500_SendLED(0,0,0);//�ϴ��ƶ���
        }
		}
    else
    {
        if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
        {
            W5500_Send_Data(0,con_TranMZ);//�ϴ����ͼ��
        }
    }
		/***�������****/
		Fun_DynamoStart();
		if(System_State != Retardant_State)
		{
				return;
		}
		/*******************/
		Left_DynamicBrake = 0;
		Right_DynamicBrake = 0;  //��̬��ʾֵ����
		Left_DynamicBrake1 = 0;
		Right_DynamicBrake1 = 0;
		Countdown_time = SysParameter.Par4_ZuzhiTestTime + 1; //����ʱʱ��
// 	Countdown_time = 2;		
		USER_SystemFlag.A.Flag_1s = 1;//�״ε����ϴ�����ʱ��һ����ʹ��
		
		while(System_State == Retardant_State)
		{
				W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
						{
								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
						{
								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
						}
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						u8Left_flg = 1;	
						u8Right_flg = 1;
				}
				if(SysParameter.Par1_RollerFlag == 1)//Ӳͣ����Ч
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_flg == 0)) //��ͣ���ź�
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_flg = 1;											
										Left_Machine = IO_CLOSE;     						//��ͣ��
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													W5500_Send_Data(2,con_TranT7); 		 //���������쳣
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
		// 														W5500_Send_Data(0,con_TranT7); 		 //���������쳣
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
		// 													 W5500_Send_Data(0,con_TranT7); 		 //���������쳣
													}
										}  																
										#ifdef _DEBUG //�����ú�����
										printf("\r\nError:Left pulse is false!!!T7\r\n");
										#endif	
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_flg == 0)) //��ͣ���ź�
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_flg = 1;
										Right_Machine = IO_CLOSE;     							//��ͣ��						
										USER_SystemFlag.A.Flag_Rdynamo = 0;		
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													W5500_Send_Data(2,con_TranT8);  		//���������쳣
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
		// 													W5500_Send_Data(0,con_TranT8);  		//���������쳣
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
		// 													 W5500_Send_Data(0,con_TranT8);  		//���������쳣
													}
										}  
										#ifdef _DEBUG //�����ú�����
										printf("\r\nError:Left pulse is false!!!T8\r\n");
										#endif
								}								
						}
						if((u8Left_flg == 1) && (u8Right_flg == 1))
						{
								Machine_Switch = IO_CLOSE;						//������
								USER_SystemFlag.A.Flag_Switch = 0;
								System_State = Default_State;
								return;
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//��ʱ��������
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
										Tx_Buffer[0] = 3;  //֡���� ��λ
										Tx_Buffer[1] = 0;  //֡���� ��λ
										Tx_Buffer[2] = 'M'; 
										Tx_Buffer[3] = '1'; 
										Tx_Buffer[4] = Countdown_time;
									  if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
									  {
												W5500_Send_Data(2,Tx_Buffer);
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
															USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
															USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
															FunW5500_SendLED(0,LED_OFF,Countdown_time);
												}
									  }
										else
										{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
															W5500_Send_Data(0,Tx_Buffer);//
												}
										}
								}
								else
								{
										System_State = Default_State;
// 										Left_Machine = IO_CLOSE;							//����ֹͣ
// 										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
// 										Machine_Switch = IO_CLOSE;						//������
								}
						}
				}
				if(USER_SystemFlag.A.Flag_500ms == 1)  //��ʾʹ��
				{
						USER_SystemFlag.A.Flag_500ms = 0;
						/* ���ƶ����� */
						fl32Left_SumBrake1 = ADShow_Display(Left_Brake_ID);
						fl32Left_SumBrake1 = fl32Left_SumBrake1*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32Left_SumBrake1 > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake1 - Zero_LeftBrake;
								Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake += 5;
								Left_DynamicBrake /= 10;
								Left_DynamicBrake *= 10;   //����daN
						}
						else 
						{
								Left_DynamicBrake = 0;
						}
						/* ���ƶ����� */
						fl32Right_SumBrake1 = ADShow_Display(Right_Brake_ID);//ȡƽ��ֵ
						fl32Right_SumBrake1 = fl32Right_SumBrake1*VOLTAGE;//���ѹֵ		
						if(fl32Right_SumBrake1 > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake1 - Zero_RightBrake;
								Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake += 5;
								Right_DynamicBrake /= 10;
								Right_DynamicBrake *= 10;  //����daN
						}
						else 
						{
								Right_DynamicBrake = 0;
						}
				}
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;		  
						u16countMax ++;
// 							if(u16countMax >= 300) u16countMax=300;
						/* ���ƶ����� */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //����daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						if(u16Count800 < 800)
						{
								u16Left_Data[u16Count800] = Left_DynamicBrake1;
						}
						/* ���ƶ����� */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//ȡƽ��ֵ
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//���ѹֵ		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //����daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if(u16Count800 < 800)
						{
								u16Right_Data[u16Count800] = Right_DynamicBrake1;
						}					
						if(Left_DynamicBrake1 > u16Protect_Limit)  //���ͱ���
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = Default_State;          //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;					//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
//										W5500_Send_Data(0,con_TranT1);  //�������ʹ�
										SysParameter.DynamoStart_Flag = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT1(2,Left_DynamicBrake1); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{                                
                              USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                              USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
                              FunW5500_SendLED(0,Left_DynamicBrake1,0);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake1); //�������ʹ�
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake1 > u16Protect_Limit)  //���ͱ���
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = Default_State;         //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;						//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
//										W5500_Send_Data(0,con_TranT2);  //�������ʹ�
										SysParameter.DynamoStart_Flag = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT2(2,Right_DynamicBrake1); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																FunW5500_SendLED(0,0,Right_DynamicBrake1);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake1); //�������ʹ�
													}
										}
										return;
								}
						}
						else
						{
								u8Protect_CountFlg2 = 0;
						}							
						u16Count800 ++;
				}
		}	
		if(u16countMax >= 800) u16countMax = 800;
		Deal_Data(u16countMax,u8Tran_CurrentZ,'Z');  //�������ݱ��浽�ϴ����ݵ�Ԫ
		Fun_DealZZ(u16countMax); //���ͼ�����
		if(SysParameter.Retardant_Flag == 1)  							  //���������ͺ���ֹͣ
		{
				USER_SystemFlag.A.Flag_TestC = 0;
				USER_SystemFlag.A.Flag_TestC1 = 0;
				Speed_Control = IO_CLOSE; 												 //���ٰ�ֹͣ
				Left_Machine = IO_CLOSE;													 //����ֹͣ
				Right_Machine = IO_CLOSE;												 //�ҵ��ֹͣ
				Machine_Switch = IO_CLOSE;											 //������
				USER_SystemFlag.A.Flag_Ldynamo = 0;
				USER_SystemFlag.A.Flag_Rdynamo = 0;
				USER_SystemFlag.A.Flag_Switch = 0;
				USER_SystemFlag.A.Flag_ST = 0;
				SysParameter.DynamoStart_Flag = 0;   				 //���ش���״̬���ж�Ӳ��ͣ�����
				if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
				{
							W5500_Send_Data(2,con_TranT0);									 //�ϴ�������
							if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
							{									
// 										FunW5500_SendLED(0,Left_BrakeAve,Right_BrakeAve);//�ϴ����Ͳ��Խ��
							}
							if(USER_SystemFlag.A.Flag_AutoD == 1)
							{
									USER_SystemFlag.A.Flag_AutoD = 0;
									W5500_Send_Data(2,u8Tran_CurrentZ);				//���͵�ǰ���͹�������
							}						
				}
				else
				{
							if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
							{						
									 FunW5500_SendGB(0,Left_BrakeAve,Right_BrakeAve); //��ʾ���Ͳ��Խ��
									 Delay_ms(20);
									 W5500_Send_Data(0,con_TranT0);									 //�ϴ�������
							}
				} 
				System_State = ReadData_State;          //����ȡ��״̬
		}
		else  //��������ƶ�����
		{
				SysParameter.DynamoStart_Flag = 1;   		//���ش���״̬���ж�Ӳ��ͣ�����
				System_State = BrakeTest_None_State;          //��������ƶ�����
				Fun_TestZC();
		}
}
/*******************************************************************************
* ������  : Fun_TestZC()
* ����    : ����+�����ƶ�����
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_TestZC(void)
{
		uint8_t Countdown_time;																						//����ʱ��ʼֵ
		uint8_t u8Left_Stopflg=0,        																		//��ͣ����־
							  u8Right_Stopflg=0, 																				//��ͣ����־
								u8Left_Judgeflg=0,																				//��ͣ���жϱ�־
								u8Right_Judgeflg=0;																			//��ͣ���жϱ�־
	 uint16_t	 u16Left_Number=0,																			//��ͣ���½�����
								 u16Right_Number=0;																		//��ͣ���½�����

		uint16_t u16Count800_Left=0, 															    //800����洢�������
								  u16Count800_Right=0;															  //800����洢�����ұ�
		uint16_t u16countMax=0, 																					//�������洢������
									u16Brake_LeftBefor=0,
									u16Brake_RightBefor=0,															//ǰһ���ƶ�������
									u16Brake_LeftMAX=0,
									u16Brake_RightMAX=0,																//�ƶ������ֵ
									u16Brake_Protect20=0,																//�ƶ�����ͣ����ʼ��ֵ 20%
									u16Brake_Protect60=0;																//�ƶ�����ͣ����ʼ��ֵ 60%
		uint16_t   u16Brake_Stop70=0,																			//�ƶ�������ͣ����ֵ
									 u16Brake_Stop90=0;                                      //�ƶ������߱�����ֵ
		uint16_t Voltage_Relative;																					//��Ե�ѹ
		float fl32Left_SumBrake=0,
					 fl32Right_SumBrake=0;																			//�ƶ������ʱ����
									 
	  float fl32Left_SumBrake1=0,
					  fl32Right_SumBrake1=0;																	  //�ƶ������ʱ����		�ϴ���ʾʹ�� 			
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1;
		
		uint16_t i;
		
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranMB);								   //�ϴ������ƶ�����
					Delay_ms(20);
					W5500_Send_Data(2,con_TranM2);								   //�ϴ�M2��
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{
								USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
								USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
								FunW5500_SendLED(0,0,0);//�ϴ��ƶ���
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{													 
							 W5500_Send_Data(0,con_TranM2);								   //�ϴ�M2��
					}
		}  
	  u16Brake_Protect20 = Left_StaticWeight + Right_StaticWeight;
		if(u16Brake_Protect20 < 500) u16Brake_Protect20 = 500;
		u16Brake_Protect60 = u16Brake_Protect20 * 3;
		
		u16Brake_Stop70 = u16Brake_Protect20 * 4;  //80%
		u16Brake_Stop90 = (float)u16Brake_Protect20 * 4.6;  //92%
		
		Left_DynamicBrake = 0;
		Right_DynamicBrake = 0;  																				//��̬��ʾֵ����
		Left_DynamicBrake1 = 0;
		Right_DynamicBrake1 = 0;  														
		Countdown_time = SysParameter.Par5_BrakeTestTime + 1; //�ƶ�����ʱ��	
		USER_SystemFlag.A.Flag_1s = 1;															//�״ε����ϴ�����ʱ��һ����ʹ��
		while(System_State == BrakeTest_None_State)
		{
				W5500_Receive_Deal(); 																			//����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
// 						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
// 						{
// 								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
// 						}
// 						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
// 						{
// 								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
// 						}
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						u8Left_Judgeflg = 3;							//ͣ���½�10�������ɼ�
						u8Left_Stopflg = 1;
						u8Right_Judgeflg = 3;							//ͣ���½�10�������ɼ�			
						u8Right_Stopflg = 1;
						//ͣ������  ͣ��ʱ��
						Left_StopFlag = '0';
						Right_StopFlag = '0';  
						Left_StopTime.u16_data = u16Count800_Left;
						Right_StopTime.u16_data = u16Count800_Right;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//Ӳͣ����Ч
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //��ͣ���ź�
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 3;							//ͣ���½�10�������ɼ�
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '1';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //��ͣ���ź�
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 3;							//ͣ���½�10�������ɼ�
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '1';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//��ʱ��������
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
// 										Tx_Buffer[0] = 3;  //֡���� ��λ
// 										Tx_Buffer[1] = 0;  //֡���� ��λ
// 										Tx_Buffer[2] = 'M'; 
// 										Tx_Buffer[3] = '1'; 
// 										Tx_Buffer[4] = Countdown_time;
// // 										W5500_Send_Data(0,Tx_Buffer);
								}
								else
								{
										System_State = Default_State;										
										Left_Machine = IO_CLOSE;													 //����ֹͣ
										Right_Machine = IO_CLOSE;												 //�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;											 //������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0';
										Right_StopFlag = '0';  
										Left_StopTime.u16_data = u16Count800_Left;
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_500ms == 1) //��ʱ
				{
							USER_SystemFlag.A.Flag_500ms = 0;			
							/* ���ƶ����� */
							fl32Left_SumBrake1 = ADShow_Display(Left_Brake_ID);
							fl32Left_SumBrake1 = fl32Left_SumBrake1*VOLTAGE; //ת���ɵ�ѹֵ
							if(fl32Left_SumBrake1 > Zero_LeftBrake)
							{
									Voltage_Relative = fl32Left_SumBrake1 - Zero_LeftBrake;
									Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
									Left_DynamicBrake += 5;
									Left_DynamicBrake /= 10;
									Left_DynamicBrake *= 10;   //����daN
							}
							else 
							{
									Left_DynamicBrake = 0;
							}
							/* ���ƶ����� */
							fl32Right_SumBrake1 = ADShow_Display(Right_Brake_ID);//ȡƽ��ֵ
							fl32Right_SumBrake1 = fl32Right_SumBrake1*VOLTAGE;//���ѹֵ		
							if(fl32Right_SumBrake1 > Zero_RightBrake)
							{
									Voltage_Relative = fl32Right_SumBrake1 - Zero_RightBrake;
									Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
									Right_DynamicBrake += 5;
									Right_DynamicBrake /= 10;
									Right_DynamicBrake *= 10;  //����daN
							}
							else 
							{
									Right_DynamicBrake = 0;
							}
							if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
							{
										if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
										{
													USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
													USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
													FunW5500_SendLED(0,Left_DynamicBrake,Right_DynamicBrake);//�ϴ��ƶ���
										}
							}
							else
							{
									if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
									{
											FunW5500_SendGB(0,Left_DynamicBrake,Right_DynamicBrake); //�ϴ��ƶ���
									}
							}
				 }
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;						
						/* ���ƶ����� */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //����daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						if((u16Count800_Left < 800) && (u8Left_Judgeflg < 10))
						{
								if(Left_DynamicBrake1 < u16Brake_Stop90)
								{
										u16Left_Data[u16Count800_Left++] = Left_DynamicBrake1;
								}	
								if(u16Brake_LeftMAX <= Left_DynamicBrake1)  //���ֵ�����½������ж�
								{
										u16Brake_LeftMAX = Left_DynamicBrake1;
										u16Left_Number = 0;
								}
								else
								{
										u16Left_Number ++;
								}
								if((Left_DynamicBrake1 > u16Brake_Stop70)&&(u8Left_Judgeflg < 4))  //����ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}	
						}
						/* ���ƶ����� */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//ȡƽ��ֵ
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//���ѹֵ		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //����daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if((u16Count800_Right < 800) && (u8Right_Judgeflg < 10))
						{
								if(Right_DynamicBrake1 < u16Brake_Stop90)
								{
										u16Right_Data[u16Count800_Right++] = Right_DynamicBrake1;
								}								
								if(u16Brake_RightMAX <= Right_DynamicBrake1)  //���ֵ�����½������ж�
								{
										u16Brake_RightMAX = Right_DynamicBrake1;
										u16Right_Number = 0;
								}
								else
								{
										u16Right_Number ++;
								}
								if((Right_DynamicBrake1 > u16Brake_Stop70)&&(u8Right_Judgeflg < 4))  //����ͣ��
								{
										Right_Machine = IO_CLOSE;							//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  	
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}	
						}							

						//��ͣ���ж�
						if(u8Left_Judgeflg == 0)  // <20%�ж�
						{
								u16Left_Number = 0;
								if(Left_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Left_Judgeflg = 1;												
								}
						}
						else if(u8Left_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Left_Number > con_LDN20)  //ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;   
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0'; 
										Left_StopTime.u16_data = u16Count800_Left;
								}
								else
								{
										if(Left_DynamicBrake1 > u16Brake_Protect60)
										{
												u8Left_Judgeflg = 2;
												u16Left_Number = 0;
										}
								}
						}
						else if(u8Left_Judgeflg == 2) // >60%�ж�
						{										
								if(u16Left_Number > con_LDN60)  //ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;  		
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						else if(u8Left_Judgeflg == 3)  //Ӳͣ��10���½������ɼ�
						{
								if(u16Left_Number > con_LDNStop)   //�ж��½�ʮ����
								{
										u8Left_Judgeflg = 4;    
								}
						}								
						else if(u8Left_Judgeflg == 4)   //����ͣ������������
						{
								if(Left_DynamicBrake1 < 200)
								{
										if(u16Count800_Left<800)
										{
												u16Left_Data[u16Count800_Left++] = 0;
										}
										u8Left_Judgeflg = 10;
								}
						}
						if(u8Left_Judgeflg == 10)
						{
								Left_Machine = IO_CLOSE;													 //����ֹͣ
								USER_SystemFlag.A.Flag_Ldynamo = 0;
						}
						
						//��ͣ���ж�
						if(u8Right_Judgeflg == 0)  // <20%�ж�
						{
								u16Right_Number = 0;
								if(Right_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Right_Judgeflg = 1;												
								}
						}
						else if(u8Right_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Right_Number > con_RDN20)  //ͣ��
								{
										Right_Machine = IO_CLOSE;								//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;   
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
								else
								{
										if(Right_DynamicBrake1 > u16Brake_Protect60)
										{
												u8Right_Judgeflg = 2;
												u16Right_Number = 0;
										}
								}
						}
						else if(u8Right_Judgeflg == 2) // >60%�ж�
						{										
								if(u16Right_Number > con_RDN60)  //ͣ��
								{
										Right_Machine = IO_CLOSE;							//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  		
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
						else if(u8Right_Judgeflg == 3)  //Ӳͣ��10���½������ɼ�
						{
								if(u16Right_Number > con_RDNStop)   
								{
										u8Right_Judgeflg = 4;    
								}
						}	
						else if(u8Right_Judgeflg == 4)   //����ͣ������������
						{
								if(Right_DynamicBrake1 < 200)
								{
										if(u16Count800_Right<800)
										{
												u16Right_Data[u16Count800_Right++] = 0;
										}
										u8Right_Judgeflg = 10;
								}
						}
						if(u8Right_Judgeflg == 10)
						{
								Right_Machine = IO_CLOSE;													 //�ҵ��ֹͣ
								USER_SystemFlag.A.Flag_Rdynamo = 0;
						}
						if((u8Left_Judgeflg == 10) && (u8Right_Judgeflg == 10))
						{
								Machine_Switch = IO_CLOSE;												//�������л��ر�
								USER_SystemFlag.A.Flag_Switch = 0;
								System_State = Default_State;
						}
				}
		}	
		USER_SystemFlag.A.Flag_TestC = 0;
		USER_SystemFlag.A.Flag_TestC1 = 0;
		SysParameter.DynamoStart_Flag = 0;   						 //���ش���״̬���ж�Ӳ��ͣ�����
		if(u16Count800_Left < u16Count800_Right)       //�����������ͬ����
		{
				for(i=u16Count800_Left;i<u16Count800_Right;i++)
				{
						u16Left_Data[i] = 0;
				}
				u16countMax = u16Count800_Right;
		}
		else
		{
				if(u16Count800_Left == u16Count800_Right)
				{
						u16countMax = u16Count800_Right;
				}
				else
				{
						for(i=u16Count800_Right;i<u16Count800_Left;i++)
						{
								u16Right_Data[i] = 0;
						}
						u16countMax = u16Count800_Left;
				}
		}		
// 		DataRank(u16Left_Data,u16Count800_Left); //����
// 		DataRank(u16Right_Data,u16Count800_Right); //����
		#ifdef _DEBUG //�����ú�����
		for(i=0;i<u16Count800_Left;i++)
		{
				printf("\r\nu16Left_Data=%u\r\n",u16Left_Data[i]);
		}
		for(i=0;i<u16Count800_Left;i++)
		{
				printf("\r\nu16Right_Data=%u\r\n",u16Right_Data[i]);
		}		
		#endif
		Deal_Data(u16countMax,u8Tran_CurrentC,'C');  //�����ƶ����ݱ��浽�ϴ����ݵ�Ԫ
		Fun_DealCC(u16Count800_Left,u16Count800_Right);
		System_State = ReadData_State; 	   //�����ȡ����״̬
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranT0);									 //�ϴ�������
					if(USER_SystemFlag.A.Flag_AutoD == 1)
					{
							USER_SystemFlag.A.Flag_AutoD = 0;
							W5500_Send_Data(2,u8Tran_CurrentZ);				//���͵�ǰ���͹�������
							W5500_Send_Data(2,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
					}										
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{												 
							 FunW5500_SendGB(0,Left_BrakeSum,Right_BrakeSum);   //�ϴ��ƶ������ս��
							 Delay_ms(20);
							 W5500_Send_Data(0,con_TranT0);									 //�ϴ�������
					}
		}  
}

/*******************************************************************************
* ������  : Fun_TestCC()
* ����    : �����ƶ�����
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_TestCC(void)
{
		uint8_t Countdown_time;																						//����ʱ��ʼֵ
		uint8_t u8Left_Stopflg=0,        																		//��ͣ����־
							  u8Right_Stopflg=0, 																				//��ͣ����־
								u8Left_Judgeflg=0,																				//��ͣ���жϱ�־
								u8Right_Judgeflg=0;																			//��ͣ���жϱ�־
	 uint16_t	 u16Left_Number=0,																			//��ͣ���½�����
								 u16Right_Number=0;																		//��ͣ���½�����

		uint16_t u16Count800_Left=0, 															    //800����洢�������
								  u16Count800_Right=0;															  //800����洢�����ұ�
		uint16_t u16countMax=0, 																					//�������洢������
									u16Brake_LeftBefor=0,
									u16Brake_RightBefor=0,															//ǰһ���ƶ�������
									u16Brake_LeftMAX=0,
									u16Brake_RightMAX=0,																//�ƶ������ֵ
									u16Brake_Protect20=0,																//�ƶ�����ͣ����ʼ��ֵ 20%
									u16Brake_Protect60=0;																//�ƶ�����ͣ����ʼ��ֵ 60%
	  uint16_t   u16Brake_Stop70=0,																			//�ƶ�������ͣ����ֵ
									 u16Brake_Stop90=0;                                      //�ƶ������߱�����ֵ
		uint16_t Voltage_Relative;																					//��Ե�ѹ
		float fl32Left_SumBrake=0,
					 fl32Right_SumBrake=0;																			//�ƶ������ʱ����
									 
	  float fl32Left_SumBrake1=0,
					  fl32Right_SumBrake1=0;																	  //�ƶ������ʱ����		�ϴ���ʾʹ�� 		
						
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1;	
		uint16_t i;
		uint8_t u8Protect_CountFlg = 0;    //����������ֵ����
		uint8_t u8Protect_CountFlg2 = 0;  //�ҵ��������ֵ����

		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranMB);								   //�ϴ������ƶ�����
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{
                if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
                {
                    USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
                }
                USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
                USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
                USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��					
								USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
								USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
// 							FunW5500_SendLED(0,0,0);//�ϴ��ƶ���
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{													 
							 W5500_Send_Data(0,con_TranMB);								   //�ϴ������ƶ�����
					}
		} 
		/***�������****/
		Fun_DynamoStart();
		if(System_State != BrakeTest_None_State)
		{
				return;
		}
		/*****�ƶ�׼������ʱ*******/		
		Left_DynamicBrake1 = 0;
		Right_DynamicBrake1 = 0;
		Countdown_time = SysParameter.Par3_BrakeReadyTime + 1; //�ƶ�����ʱ��	
		USER_SystemFlag.A.Flag_1s = 1;															//�״ε����ϴ�����ʱ��һ����ʹ��
		while(System_State == BrakeTest_None_State)
		{
				W5500_Receive_Deal(); 																			//����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
						{
								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
						{
								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
						}
						System_State = Default_State;          //����Ĭ��״̬ �ƶ�����
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						return;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//Ӳͣ����Ч
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //��ͣ���ź�
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Ldynamo = 0;
		// 								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
		// 								{
		// 										W5500_Send_Data(0,con_TranT7); //
		// 								}
										if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
										{
												W5500_Send_Data(2,con_TranT7); //�ش��������쳣
										}
								}
								
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //��ͣ���ź�
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Rdynamo = 0;
		// 								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
		// 								{
		// 										W5500_Send_Data(0,con_TranT8); //
		// 								}
										if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
										{
												W5500_Send_Data(2,con_TranT8); //�ش���������λ�ź�	
										}
								}
						}
						if((u8Left_Stopflg == 1) && (u8Right_Stopflg == 1))
						{
								Machine_Switch = IO_CLOSE;     							//�������л�
								USER_SystemFlag.A.Flag_Switch = 0;
								SysParameter.DynamoStart_Flag = 0;       //����
								System_State = Default_State;    
								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
								{
										W5500_Send_Data(0,con_TranT7); //
										W5500_Send_Data(0,con_TranT8); //
								}
								if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
								{
// 										W5500_Send_Data(2,con_TranT7); //�ش���������λ�ź�	
// 										W5500_Send_Data(2,con_TranT8); //�ش���������λ�ź�	
								}
								return;
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//��ʱ��������
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
										Tx_Buffer[0] = 3;  //֡���� ��λ
										Tx_Buffer[1] = 0;  //֡���� ��λ
										Tx_Buffer[2] = 'M'; 
										Tx_Buffer[3] = '1'; 
										Tx_Buffer[4] = Countdown_time;										
										if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
										{
												W5500_Send_Data(2,Tx_Buffer);//
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
														FunW5500_SendLED(0,LED_OFF,Countdown_time);
												}
										}
										else
										{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
															W5500_Send_Data(0,Tx_Buffer);//
												}
										}
								}
								else
								{
										break;		//����ѭ��								
								}
						}
				}
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;		
						/* ���ƶ����� */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //����daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						/* ���ƶ����� */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//ȡƽ��ֵ
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//���ѹֵ		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //����daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if(Left_DynamicBrake1 > u16Protect_Limit)  //���ͱ���
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;          //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;					//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT1(2,Left_DynamicBrake1); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{                                
                              USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                              USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
                              FunW5500_SendLED(0,Left_DynamicBrake1,0);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake1); //�������ʹ�
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake1 > u16Protect_Limit)  //���ͱ���
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;         //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;						//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT2(2,Right_DynamicBrake1); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																FunW5500_SendLED(0,0,Right_DynamicBrake1);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake1); //�������ʹ�
													}
										}
										return;
								}
						}
						else
						{
								u8Protect_CountFlg2 = 0;
						}
			  }						
		}
		/***********��**************/
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranM2);								   //�ϴ���
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{

					}					
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{						
							 W5500_Send_Data(0,con_TranM2);								   //�ϴ���
					}
		}  
		/*****************************************************/
		fl32Left_SumBrake = 0;
		fl32Right_SumBrake = 0; //��ͱ������� ��ʱ����		
	  u16Brake_Protect20 = Left_StaticWeight + Right_StaticWeight;
		if(u16Brake_Protect20 < 500) u16Brake_Protect20 = 500;
		u16Brake_Protect60 = u16Brake_Protect20 * 3;
		
		u16Brake_Stop70 = u16Brake_Protect20 * 4;  //80%
		u16Brake_Stop90 = (float)u16Brake_Protect20 * 4.6;  //92%
		
		Left_DynamicBrake = 0;
		Right_DynamicBrake = 0;  																				//��̬��ʾֵ����
		Countdown_time = SysParameter.Par5_BrakeTestTime + 1; //�ƶ�����ʱ��	
		USER_SystemFlag.A.Flag_1s = 1;															//�״ε����ϴ�����ʱ��һ����ʹ��
		while(System_State == BrakeTest_None_State)
		{
				W5500_Receive_Deal(); 				//����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
// 						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
// 						{
// 								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
// 						}
// 						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
// 						{
// 								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
// 						}
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						u8Left_Judgeflg = 3;							//ͣ���½�10�������ɼ�
						u8Left_Stopflg = 1;
						u8Right_Judgeflg = 3;							//ͣ���½�10�������ɼ�			
						u8Right_Stopflg = 1;
						//ͣ������  ͣ��ʱ��
						Left_StopFlag = '0';
						Right_StopFlag = '0';  
						Left_StopTime.u16_data = u16Count800_Left;
						Right_StopTime.u16_data = u16Count800_Right;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//Ӳͣ����Ч
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //��ͣ���ź�
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 3;							//ͣ���½�10�������ɼ�
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '1'; 
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //��ͣ���ź�
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 3;							//ͣ���½�10�������ɼ�
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '1';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//��ʱ��������
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
// 										Tx_Buffer[0] = 3;  //֡���� ��λ
// 										Tx_Buffer[1] = 0;  //֡���� ��λ
// 										Tx_Buffer[2] = 'M'; 
// 										Tx_Buffer[3] = '1'; 
// 										Tx_Buffer[4] = Countdown_time;
// 										W5500_Send_Data(0,Tx_Buffer);
								}
								else
								{
										System_State = Default_State;										
										Left_Machine = IO_CLOSE;													 //����ֹͣ
										Right_Machine = IO_CLOSE;												 //�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;											 //������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_500ms == 1) //�ϴ��ƶ���
				{
							USER_SystemFlag.A.Flag_500ms = 0;				
							/* ���ƶ����� */
							fl32Left_SumBrake1 = ADShow_Display(Left_Brake_ID);
							fl32Left_SumBrake1 = fl32Left_SumBrake1*VOLTAGE; //ת���ɵ�ѹֵ
							if(fl32Left_SumBrake1 > Zero_LeftBrake)
							{
									Voltage_Relative = fl32Left_SumBrake1 - Zero_LeftBrake;
									Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
									Left_DynamicBrake += 5;
									Left_DynamicBrake /= 10;
									Left_DynamicBrake *= 10;   //����daN
							}
							else 
							{
									Left_DynamicBrake = 0;
							}
							/* ���ƶ����� */
							fl32Right_SumBrake1 = ADShow_Display(Right_Brake_ID);//ȡƽ��ֵ
							fl32Right_SumBrake1 = fl32Right_SumBrake1*VOLTAGE;//���ѹֵ		
							if(fl32Right_SumBrake1 > Zero_RightBrake)
							{
									Voltage_Relative = fl32Right_SumBrake1 - Zero_RightBrake;
									Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
									Right_DynamicBrake += 5;
									Right_DynamicBrake /= 10;
									Right_DynamicBrake *= 10;  //����daN
							}
							else 
							{
									Right_DynamicBrake = 0;
							}
							if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
							{
										if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
										{
													USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
													USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
 	  											FunW5500_SendLED(0,Left_DynamicBrake,Right_DynamicBrake);//�ϴ��ƶ���
										}
							}
							else
							{
									if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
									{
 													FunW5500_SendGB(0,Left_DynamicBrake,Right_DynamicBrake);//�ϴ��ƶ���
									}
							}
				 }					
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;										
						/* ���ƶ����� */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //����daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						if((u16Count800_Left < 800) && (u8Left_Judgeflg < 10))
						{
								if(Left_DynamicBrake1 < u16Brake_Stop90)
								{
										u16Left_Data[u16Count800_Left++] = Left_DynamicBrake1;
								}								
								if(u16Brake_LeftMAX <= Left_DynamicBrake1)  //���ֵ�����½������ж�
								{
										u16Brake_LeftMAX = Left_DynamicBrake1;
										u16Left_Number = 0;
								}
								else
								{
										u16Left_Number ++;
								}
								if((Left_DynamicBrake1 > u16Brake_Stop70)&&(u8Left_Judgeflg < 4))  //����ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0'; 
										Left_StopTime.u16_data = u16Count800_Left;
								}		
						}
						/* ���ƶ����� */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//ȡƽ��ֵ
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//���ѹֵ		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //����daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if((u16Count800_Right < 800) && (u8Right_Judgeflg < 10))
						{
								if(Right_DynamicBrake1 < u16Brake_Stop90)
								{
										u16Right_Data[u16Count800_Right++] = Right_DynamicBrake1;
								}														
								if(u16Brake_RightMAX <= Right_DynamicBrake1)  //���ֵ�����½������ж�
								{
										u16Brake_RightMAX = Right_DynamicBrake1;
										u16Right_Number = 0;
								}
								else
								{
										u16Right_Number ++;
								}
								if((Right_DynamicBrake1 > u16Brake_Stop70)&&(u8Right_Judgeflg < 4))  //����ͣ��
								{
										Right_Machine = IO_CLOSE;							//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  	
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}	
						}							
						//��ͣ���ж�
						if(u8Left_Judgeflg == 0)  // <20%�ж�
						{
								u16Left_Number = 0;
								if(Left_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Left_Judgeflg = 1;												
								}
						}
						else if(u8Left_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Left_Number > con_LDN20)  //ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;   
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}
								else
								{
										if(Left_DynamicBrake1 > u16Brake_Protect60)
										{
												u8Left_Judgeflg = 2;
												u16Left_Number = 0;
										}
								}
						}
						else if(u8Left_Judgeflg == 2) // >60%�ж�
						{										
								if(u16Left_Number > con_LDN60)  //ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;  		
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						else if(u8Left_Judgeflg == 3)  //Ӳͣ��10���½������ɼ�
						{
								if(u16Left_Number > con_LDNStop)   //�ж��½�ʮ����
								{
										u8Left_Judgeflg = 4;    
								}
						}								
						else if(u8Left_Judgeflg == 4)   //����ͣ������������
						{
								if(Left_DynamicBrake1 < 200)
								{
										if(u16Count800_Left<800)
										{
												u16Left_Data[u16Count800_Left++] = 0;
										}
										u8Left_Judgeflg = 10;
								}
						}
						if(u8Left_Judgeflg == 10)
						{
								Left_Machine = IO_CLOSE;													 //����ֹͣ
								USER_SystemFlag.A.Flag_Ldynamo = 0;
						}
						
						//��ͣ���ж�
						if(u8Right_Judgeflg == 0)  // <20%�ж�
						{
								u16Right_Number = 0;
								if(Right_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Right_Judgeflg = 1;												
								}
						}
						else if(u8Right_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Right_Number > con_RDN20)  //ͣ��
								{
										Right_Machine = IO_CLOSE;								//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;   
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
								else
								{
										if(Right_DynamicBrake1 > u16Brake_Protect60)
										{
												u8Right_Judgeflg = 2;
												u16Right_Number = 0;
										}
								}
						}
						else if(u8Right_Judgeflg == 2) // >60%�ж�
						{										
								if(u16Right_Number > con_RDN60)  //ͣ��
								{
										Right_Machine = IO_CLOSE;							//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;									
								}
						}
						else if(u8Right_Judgeflg == 3)  //Ӳͣ��10���½������ɼ�
						{
								if(u16Right_Number > con_RDNStop)   
								{
										u8Right_Judgeflg = 4;    
								}
						}	
						else if(u8Right_Judgeflg == 4)   //����ͣ������������
						{
								if(Right_DynamicBrake1 < 200)
								{
										if(u16Count800_Right<800)
										{
												u16Right_Data[u16Count800_Right++] = 0;
										}
										u8Right_Judgeflg = 10;
								}
						}
						if(u8Right_Judgeflg == 10)
						{
								Right_Machine = IO_CLOSE;													 //�ҵ��ֹͣ
						}
						if((u8Left_Judgeflg == 10) && (u8Right_Judgeflg == 10))
						{
								Machine_Switch = IO_CLOSE;												//�������л��ر�
								USER_SystemFlag.A.Flag_Switch = 0;
								System_State = Default_State;
						}
				}
		}	
		USER_SystemFlag.A.Flag_TestC = 0;
		USER_SystemFlag.A.Flag_TestC1 = 0;
		SysParameter.DynamoStart_Flag = 0;   						 //���ش���״̬���ж�Ӳ��ͣ�����
		if(u16Count800_Left < u16Count800_Right)       //�����������ͬ����
		{
				for(i=u16Count800_Left;i<u16Count800_Right;i++)
				{
						u16Left_Data[i] = 0;
				}
				u16countMax = u16Count800_Right;
		}
		else
		{
				if(u16Count800_Left == u16Count800_Right)
				{
						u16countMax = u16Count800_Right;
				}
				else
				{
						for(i=u16Count800_Right;i<u16Count800_Left;i++)
						{
								u16Right_Data[i] = 0;
						}
						u16countMax = u16Count800_Left;
				}
		}		
// 		DataRank(u16Left_Data,u16Count800_Left); //����
// 		DataRank(u16Right_Data,u16Count800_Right); //����
		#ifdef _DEBUG //�����ú�����
		for(i=0;i<u16Count800_Left;i++)
		{
				printf("\r\nu16Left_Data=%u\r\n",u16Left_Data[i]);
		}
		for(i=0;i<u16Count800_Left;i++)
		{
				printf("\r\nu16Right_Data=%u\r\n",u16Right_Data[i]);
		}		
		#endif
		Deal_Data(u16countMax,u8Tran_CurrentC,'C');  //�����ƶ����ݱ��浽�ϴ����ݵ�Ԫ
		Fun_DealCC(u16Count800_Left,u16Count800_Right);
		System_State = ReadData_State; 	   //�����ȡ����״̬
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranT0);									 //�ϴ�������
					if(USER_SystemFlag.A.Flag_AutoD == 1)
					{
							USER_SystemFlag.A.Flag_AutoD = 0;
							W5500_Send_Data(2,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
					}					
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{												 
							 FunW5500_SendGB(0,Left_BrakeSum,Right_BrakeSum);   //�ϴ��ƶ������ս��
							 Delay_ms(20);
							 W5500_Send_Data(0,con_TranT0);									 //�ϴ�������
					}
		}  
}

/*******************************************************************************
* ������  : Fun_AllWeightTestJJ()
* ����    : ���ؾ������
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_AllWeightTestJJ(void)
{
		uint8_t Countdown_time;//����ʱ��ʼֵ
		uint16_t u16Count300=0; 																	 //300����洢����
// 		uint16_t u16countMax=0; 																 //�������洢������
		uint16_t Voltage_Relative;																 //��Ե�ѹ
		float fl32Left_SumWeight=0.0,													     //���������ʱ����
					  fl32Left_SumWeight1=0.0;													 //���������ʱ���� ������ʾʹ��
		uint16_t All_DynamicWeight1;                               //����������ʱ����
		uint8_t u8Flg1 = 0,																								 //����״̬�л���ʶ
								u8Flg2 = 0;

		TIM2_count = 0;																										 //��ʱ����
// 		u16countMax = 0;																							   //��ʼ��		
		All_DynamicWeight = 0;  																		   //���ϳ���̨��̬��ʾ����
		AllFull_StaticWeight = 0;                  								     //��ʼ�����ظ������ؽ��
		Countdown_time = SysParameter.Par0_WeightTestTime + 1; //����ʱʱ��
// 		USER_SystemFlag.A.Flag_1s = 1;											   //�״ε����ϴ�����ʱ��һ����ʹ��
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranMF2);							 //�ϴ�MF2���ؾ������
					if(USER_SystemFlag.A.Flag_Standalone == 1)   //�������� ������ܷ�
					{               
                if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
                {
                    USER_LEDFlag.A.LED_Online = 1;  	//����ָʾ��
                }
                USER_LEDFlag.A.LED_Test = 1;  			  //���ָʾ��
                USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
                USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
								USER_LEDFlag.A.LED_TJUp = 1;  			  //̨�ܾ�ָʾ��
								USER_LEDFlag.A.LED_TJDown = 0;  		  //̨�ܽ�ָʾ��						
								USER_LEDFlag.A.LED_15 = 1;  				  //����������Ƿ�ı� 1��  0����
								USER_LEDFlag.A.LED_14 = 1;  				  //�����ָʾ���Ƿ�ı� 1��  0����
								FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//�ϴ����ϳ�������
                Delay_ms(10);
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{						
// 							 W5500_Send_Data(0,con_TranMF2);								   //�ϴ�MF2���ؾ������
							 Delay_ms(20);
// 							 FunW5500_SendGF(0,All_DynamicWeight); //�ϴ����ϳ���
               Delay_ms(10);
					}
		}
		while(System_State == StaticWeight_Full_State)
		{
				W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 1)
				{
						if((USER_SystemFlag.A.Flag_1s == 1) && (u8Flg2 == 1))
						{
								TIM2_1ms = 0;//��ʱ��������
								USER_SystemFlag.A.Flag_1s = 0;
								if(Countdown_time > 0) 
								{
										Countdown_time --;
										if(Countdown_time>0)
										{
												Tx_Buffer[0] = 3;  //֡���� ��λ
												Tx_Buffer[1] = 0;  //֡���� ��λ
												Tx_Buffer[2] = 'M'; 
												Tx_Buffer[3] = '1'; 
												Tx_Buffer[4] = Countdown_time;												
												if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
												{
														W5500_Send_Data(2,Tx_Buffer);//
                            if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                            {               
                                  if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
                                  {
                                      USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
                                  }
                                  USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
                                  USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
                                  USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
                                  USER_LEDFlag.A.LED_TJUp = 1;  			//̨�ܾ�ָʾ��
                                  USER_LEDFlag.A.LED_TJDown = 0;  		//̨�ܽ�ָʾ��						
                                  USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                                  USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
                                  FunW5500_SendLED(0,LED_OFF,Countdown_time);//�ϴ�����ʱ
                            }
												}
                        else
                        {
                            if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                            {
//                                 W5500_Send_Data(0,Tx_Buffer);//
                            }
                        }
										}
										else
										{
												Left_Up = IO_CLOSE;  			  //��̨�ܾٹر�
												Left_Down = IO_CLOSE; 		//��̨�ܽ��ر�
												System_State = Default_State;
										}
								}
						}
						if(USER_SystemFlag.A.Flag_10ms == 1)
						{
								USER_SystemFlag.A.Flag_10ms = 0;							
								/* ����̨���ؼ��� */
								fl32Left_SumWeight = ADShow_Samp(All_Weight_ID);
								fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE; //ת���ɵ�ѹֵ
								if(fl32Left_SumWeight > Zero_AllWeight)
								{
										Voltage_Relative = fl32Left_SumWeight - Zero_AllWeight;
										All_DynamicWeight1 = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
								}
								else 
								{
										All_DynamicWeight1 = 0;
								}
								if((u8Flg2 == 1) && (u16Count300<100))
								{
										u16Left1_TemporaryWeight[u16Count300++] = All_DynamicWeight1;		
										if(u16Count300 >= 100) u16Count300 = 0;
								}										
								if((All_DynamicWeight1 < SysParameter.Par11_MaxWeight) && (u8Flg1 == 0)) //����С�ڼ��ط�ֵ
								{
										u8Flg1 = 1;
										Left_Up = IO_OPEN;  					//��̨�ܾٴ�
										Left_Down = IO_CLOSE; 		//��̨�ܽ��ر�
								}
								else
								{
										if(u8Flg2 == 0)
										{
												//������λ���������ﵽ���ط�ֵ
												if((IO_GetInStatus(IN_Raise_ID) == 1) || (All_DynamicWeight1 >= SysParameter.Par11_MaxWeight))
												{
														if(All_DynamicWeight1 >= SysParameter.Par11_MaxWeight)
														{
																Left_Up = IO_CLOSE;  			  //��̨�ܾٹر�
																Left_Down = IO_CLOSE; 		//��̨�ܽ��ر�
																System_State = Default_State;
																AllFull_StaticWeight = All_DynamicWeight1;
														}
														else
														{
																u8Flg2 = 1;
																USER_SystemFlag.A.Flag_1s = 1;			//�״ε����ϴ�����ʱ��һ����ʹ��
														}
												}
												if(TIM2_count > (SysParameter.Par14_UpTime*1000))//��ʱ��ֹͣ����
												{
														u8Flg2 = 1;
														USER_SystemFlag.A.Flag_1s = 1;		  //�״ε����ϴ�����ʱ��һ����ʹ��
														Left_Up = IO_CLOSE;  			  //��̨�ܾٹر�
														Left_Down = IO_CLOSE; 		//��̨�ܽ��ر�
														if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
														{
// 																		W5500_Send_Data(0,con_TranT3);		 //�ϴ�������ʱ ����λ
														}
														if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
														{
																W5500_Send_Data(2,con_TranT3);		 //�ϴ�������ʱ ����λ
														}																
												}
										}
								}																		
						}
						if((USER_SystemFlag.A.Flag_500ms == 1) && (u8Flg2 == 0))
						{
									USER_SystemFlag.A.Flag_500ms = 0;
									/* ����̨���ؼ��� */
									fl32Left_SumWeight1 = ADShow_Display(All_Weight_ID);
									fl32Left_SumWeight1 = fl32Left_SumWeight1*VOLTAGE; //ת���ɵ�ѹֵ
									if(fl32Left_SumWeight1 > Zero_AllWeight)
									{
											Voltage_Relative = fl32Left_SumWeight1 - Zero_AllWeight;
											All_DynamicWeight = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
									}
									else 
									{
											All_DynamicWeight = 0;
									}														
									All_DynamicWeight = Fun_DealWeight(All_DynamicWeight);
									if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
									{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
                              if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
                              {
                                  USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
                              }
                              USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
                              USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
                              USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
															USER_LEDFlag.A.LED_TJUp = 1;  					//̨�ܾ�ָʾ��
															USER_LEDFlag.A.LED_TJDown = 0;  		//̨�ܽ�ָʾ��
															USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
															USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
	  													FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//�ϴ����ϳ�������
												}
									}
									else
									{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
// 														 FunW5500_SendGF(0,All_DynamicWeight); //�ϴ����ϳ���
												}
									}
						}
				}
				else
				{
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
                /* ����̨���ؼ��� */
                fl32Left_SumWeight1 = ADShow_Display(All_Weight_ID);
                fl32Left_SumWeight1 = fl32Left_SumWeight1*VOLTAGE; //ת���ɵ�ѹֵ
                if(fl32Left_SumWeight1 > Zero_AllWeight)
                {
                    Voltage_Relative = fl32Left_SumWeight1 - Zero_AllWeight;
                    All_DynamicWeight = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
                }
                else 
                {
                    All_DynamicWeight = 0;
                }														
                All_DynamicWeight = Fun_DealWeight(All_DynamicWeight);
                if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
                {
                    if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                    {
                          if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
                          {
                              USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
                          }
                          USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
                          USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
                          USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
                          USER_LEDFlag.A.LED_TJUp = 1;  					//̨�ܾ�ָʾ��
                          USER_LEDFlag.A.LED_TJDown = 0;  		//̨�ܽ�ָʾ��
                          USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                          USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
                          FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//�ϴ����ϳ�������
                    }
										
                }
                else
                {
                    if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                    {
                          W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�
                    }					
                }
						}						
						Countdown_time = SysParameter.Par0_WeightTestTime + 1; //����ʱʱ��
						TIM2_1ms = 0;//��ʱ��������
						USER_SystemFlag.A.Flag_1s = 1;//�״ε����ϴ�����ʱ��һ����ʹ��
						u16Count300 = 0;
// 						u16countMax = 0;//��ʼ��
						fl32Left_SumWeight1 =0;
						TIM2_count = 0;
						u8Flg1 = 0;
						u8Flg2 = 0;
						Left_Up = IO_CLOSE;  			//��̨�ܾٹر�
						Left_Down = IO_OPEN; 		//��̨�ܽ���
				}
		}				
// 		DataRank(u16Left1_TemporaryWeight,u16countMax); //����
		if(u8Flg2 == 1)
		{
				fl32Left_SumWeight = 0.0;
				for(u16Count300=0;u16Count300<100;u16Count300++) //
				{
						fl32Left_SumWeight += u16Left1_TemporaryWeight[u16Count300];
				}
				AllFull_StaticWeight = fl32Left_SumWeight/100;
		}
		AllFull_StaticWeight = Fun_DealWeight(AllFull_StaticWeight);
	  #ifdef _DEBUG //�����ú�����
		printf("\r\nAllFull_StaticWeight=%u\r\n",AllFull_StaticWeight);
		#endif	
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		 {
						W5500_Send_Data(2,con_TranTF); 	//���ؾ������
						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
						{                  
									USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
	  							FunW5500_SendLED(0,LED_OFF,AllFull_StaticWeight);//�ϴ����ϳ�������
						}
						if(USER_SystemFlag.A.Flag_AutoD == 1)
						{
								USER_SystemFlag.A.Flag_AutoD = 0;
								FunW5500_SendDF(2,2,AllFull_StaticWeight);
						}
		 }
		 else
		 {
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{
								FunW5500_SendGF(0,AllFull_StaticWeight);
								Delay_ms(20);
// 								W5500_Send_Data(0,con_TranTF); 	//���ؾ������
					}
		 }				
		System_State = ReadData_State; 					//�ϱߵ���ʱʱ�����Ҳ������״̬�л�
}


/*******************************************************************************
* ������  : Fun_TestJJ()
* ����    : �����ƶ�����
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_TestJJ(void)
{
		uint8_t Countdown_time;																						//����ʱ��ʼֵ
		uint8_t u8Left_Stopflg=0,        																		//��ͣ����־
							  u8Right_Stopflg=0, 																				//��ͣ����־
								u8Left_Judgeflg=0,																				//��ͣ���жϱ�־
								u8Right_Judgeflg=0;																			//��ͣ���жϱ�־
	 uint16_t	 u16Left_Number=0,																			//��ͣ���½�����
								 u16Right_Number=0;																		//��ͣ���½�����

		uint16_t u16Count800_Left=0, 															    //800����洢�������
								  u16Count800_Right=0;															  //800����洢�����ұ�
		uint16_t u16countMax=0, 																					//�������洢������
									u16Brake_LeftBefor=0,
									u16Brake_RightBefor=0,															//ǰһ���ƶ�������
									u16Brake_LeftMAX=0,
									u16Brake_RightMAX=0,																//�ƶ������ֵ
									u16Brake_Protect20=0,																//�ƶ�����ͣ����ʼ��ֵ 20%
									u16Brake_Protect60=0;																//�ƶ�����ͣ����ʼ��ֵ 60%
		uint16_t   u16Brake_Stop70=0,																			//�ƶ�������ͣ����ֵ
									 u16Brake_Stop90=0;                                      //�ƶ������߱�����ֵ
		uint16_t Voltage_Relative;																					//��Ե�ѹ
		float fl32Left_SumBrake=0,
					 fl32Right_SumBrake=0;																			//�ƶ������ʱ����
									 
		float fl32Left_SumBrake1=0,
					  fl32Right_SumBrake1=0;																	  //�ƶ������ʱ����		�ϴ���ʾʹ�� 			
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1;							 
									 
		uint16_t i;
		uint8_t u8Protect_CountFlg = 0;    //����������ֵ����
		uint8_t u8Protect_CountFlg2 = 0;  //�ҵ��������ֵ����

		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranMJ);													//�ϴ������ƶ����
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{
                if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
                {
                    USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
                }
                USER_LEDFlag.A.LED_Test = 1;  			  //���ָʾ��
                USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
                USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��					
								USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
								USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
// 								FunW5500_SendLED(0,0,0);//�ϴ��ƶ���
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{													 
							 W5500_Send_Data(0,con_TranMJ);													//�ϴ������ƶ����
					}
		} 
		/***�������****/
		Fun_DynamoStart();
		if(System_State != BrakeTest_Full_State)
		{
				return;
		}
		/*****�ƶ�׼������ʱ*******/	
		Left_DynamicBrake1 = 0;
		Right_DynamicBrake1 = 0;
		Countdown_time = SysParameter.Par3_BrakeReadyTime + 1; //�ƶ�׼��ʱ��	
		USER_SystemFlag.A.Flag_1s = 1;															//�״ε����ϴ�����ʱ��һ����ʹ��
		while(System_State == BrakeTest_Full_State)
		{
				W5500_Receive_Deal(); 							//����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
						{
								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
						{
								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
						}
						System_State = Default_State;          //����Ĭ��״̬ �ƶ�����
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						return;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//Ӳͣ����Ч
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //��ͣ���ź�
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Ldynamo = 0;
		// 								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
		// 								{
		// 										W5500_Send_Data(0,con_TranT7); //
		// 								}
										if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
										{
												W5500_Send_Data(2,con_TranT7); //�ش���������λ�ź�	
										}
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //��ͣ���ź�
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Rdynamo = 0;
		// 								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
		// 								{
		// 										W5500_Send_Data(0,con_TranT8); //
		// 								}
										if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
										{
												W5500_Send_Data(2,con_TranT8); //�ش���������λ�ź�	
										}
								}
						}
						if((u8Left_Stopflg == 1) && (u8Right_Stopflg == 1))
						{
								Machine_Switch = IO_CLOSE;     							//�������л�
								USER_SystemFlag.A.Flag_Switch = 0;
								SysParameter.DynamoStart_Flag = 0;       //����
								System_State = Default_State;     
								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
								{
										W5500_Send_Data(0,con_TranT7); //
										W5500_Send_Data(0,con_TranT8); //
								}
								if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
								{
// 										W5500_Send_Data(2,con_TranT7); //�ش���������λ�ź�	
// 										W5500_Send_Data(2,con_TranT8); //�ش���������λ�ź�	
								}
								return;
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//��ʱ��������
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
										Tx_Buffer[0] = 3;  //֡���� ��λ
										Tx_Buffer[1] = 0;  //֡���� ��λ
										Tx_Buffer[2] = 'M'; 
										Tx_Buffer[3] = '1'; 
										Tx_Buffer[4] = Countdown_time;									
										if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
										{
												W5500_Send_Data(2,Tx_Buffer);//
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
														USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
														USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
														FunW5500_SendLED(0,LED_OFF,Countdown_time);
												}
										}
										else
										{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
												{
															W5500_Send_Data(0,Tx_Buffer);//
												}
										}
								}
								else
								{
										break;		//����ѭ��								
								}
						}
				}
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;		
					  /* ���ƶ����� */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //����daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						/* ���ƶ����� */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//ȡƽ��ֵ
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//���ѹֵ		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //����daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if(Left_DynamicBrake1 > u16Protect_Limit)  //���ͱ���
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;          //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;					//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT1(2,Left_DynamicBrake1); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																FunW5500_SendLED(0,Left_DynamicBrake1,0);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake1); //�������ʹ�
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake1 > u16Protect_Limit)  //���ͱ���
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX)  //3�㱣��ͣ��
								{
										System_State = ReadData_State;         //����Ĭ��״̬ �ƶ�����
										Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
										Left_Machine = IO_CLOSE;							//����ֹͣ
										Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;						//������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
										{
													FunW5500_SendT2(2,Right_DynamicBrake1); //�������ʹ�
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{
																FunW5500_SendLED(0,0,Right_DynamicBrake1);//�ϴ�������
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake1); //�������ʹ�
													}
										}
										return;
								}
						}
						else
						{
								u8Protect_CountFlg2 = 0;
						}
			  }						
		}
		/***********��**************/
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranM2);								   //�ϴ���
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{

					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{						
							 W5500_Send_Data(0,con_TranM2);								   //�ϴ���
					}
		}  
		/*****************************************************/
// 	  u16Brake_Protect20 = Left_StaticWeight + Right_StaticWeight;
		u16Brake_Protect20 = AllFull_StaticWeight;
		if(u16Brake_Protect20 < 500) u16Brake_Protect20 = 500;
		u16Brake_Protect60 = u16Brake_Protect20 * 3;
		
		u16Brake_Stop70 = u16Brake_Protect20 * 4;  //80%
		u16Brake_Stop90 = (float)u16Brake_Protect20 * 4.6;  //92%
		
		Left_DynamicBrake = 0;
		Right_DynamicBrake = 0;  																				//��̬��ʾֵ����
		Countdown_time = SysParameter.Par5_BrakeTestTime + 1; //�ƶ�����ʱ��	
		USER_SystemFlag.A.Flag_1s = 1;															//�״ε����ϴ�����ʱ��һ����ʹ��
		while(System_State == BrakeTest_Full_State)
		{
				W5500_Receive_Deal(); 																			//����ͨ�Ų�ѯ����
				if(IO_GetInStatus(IN_Brake_ID) == 0) //�жϵ�λ�ź��Ƿ�λ
				{
// 						if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
// 						{
// 								W5500_Send_Data(0,con_TranT4); //�ش���������λ�ź�	
// 						}
// 						if(USER_SystemFlag.A.Flag_Online == 1)  //���� �����ط�
// 						{
// 								W5500_Send_Data(2,con_TranT4); //�ش���������λ�ź�	
// 						}
						Speed_Control = IO_CLOSE; 						//���ٰ�ֹͣ
						Left_Machine = IO_CLOSE;							//����ֹͣ
						Right_Machine = IO_CLOSE;						//�ҵ��ֹͣ
						Machine_Switch = IO_CLOSE;					//������
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						u8Left_Judgeflg = 3;							//ͣ���½�10�������ɼ�
						u8Left_Stopflg = 1;
						u8Right_Judgeflg = 3;							//ͣ���½�10�������ɼ�			
						u8Right_Stopflg = 1;
						//ͣ������  ͣ��ʱ��
						Left_StopFlag = '0';
						Right_StopFlag = '0';  
						Left_StopTime.u16_data = u16Count800_Left;
						Right_StopTime.u16_data = u16Count800_Right;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//Ӳͣ����Ч
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //��ͣ���ź�
						{		
							  if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //��ͣ��
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 3;							//ͣ���½�10�������ɼ�
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '1';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //��ͣ���ź�
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //��ͣ��								
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 3;							//ͣ���½�10�������ɼ�
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '1';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//��ʱ��������
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
// 										Tx_Buffer[0] = 3;  //֡���� ��λ
// 										Tx_Buffer[1] = 0;  //֡���� ��λ
// 										Tx_Buffer[2] = 'M'; 
// 										Tx_Buffer[3] = '1'; 
// 										Tx_Buffer[4] = Countdown_time;
// 										W5500_Send_Data(0,Tx_Buffer);
								}
								else
								{
										System_State = Default_State;										
										Left_Machine = IO_CLOSE;													 //����ֹͣ
										Right_Machine = IO_CLOSE;												 //�ҵ��ֹͣ
										Machine_Switch = IO_CLOSE;											 //������
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0';
										Right_StopFlag = '0';  
										Left_StopTime.u16_data = u16Count800_Left;
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_500ms == 1) //�ϴ��ƶ���
				{
							USER_SystemFlag.A.Flag_500ms = 0;	
							/* ���ƶ����� */
							fl32Left_SumBrake1 = ADShow_Display(Left_Brake_ID);
							fl32Left_SumBrake1 = fl32Left_SumBrake1*VOLTAGE; //ת���ɵ�ѹֵ
							if(fl32Left_SumBrake1 > Zero_LeftBrake)
							{
									Voltage_Relative = fl32Left_SumBrake1 - Zero_LeftBrake;
									Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
									Left_DynamicBrake += 5;
									Left_DynamicBrake /= 10;
									Left_DynamicBrake *= 10;   //����daN
							}
							else 
							{
									Left_DynamicBrake = 0;
							}
							/* ���ƶ����� */
							fl32Right_SumBrake1 = ADShow_Display(Right_Brake_ID);//ȡƽ��ֵ
							fl32Right_SumBrake1 = fl32Right_SumBrake1*VOLTAGE;//���ѹֵ		
							if(fl32Right_SumBrake1 > Zero_RightBrake)
							{
									Voltage_Relative = fl32Right_SumBrake1 - Zero_RightBrake;
									Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
									Right_DynamicBrake += 5;
									Right_DynamicBrake /= 10;
									Right_DynamicBrake *= 10;  //����daN
							}
							else 
							{
									Right_DynamicBrake = 0;
							}
							if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
							{
										if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
										{
													USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
													USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
 	  											FunW5500_SendLED(0,Left_DynamicBrake,Right_DynamicBrake);//�ϴ��ƶ���
										}
							}
							else
							{
									if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
									{
 													FunW5500_SendGB(0,Left_DynamicBrake,Right_DynamicBrake);//�ϴ��ƶ���
									}
							}
				 }
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;					
						/* ���ƶ����� */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //ת���ɵ�ѹֵ
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //����daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						if((u16Count800_Left < 800) && (u8Left_Judgeflg < 10))
						{
								if(Left_DynamicBrake1 < u16Brake_Stop90)
								{
										u16Left_Data[u16Count800_Left++] = Left_DynamicBrake1;
								}	
								if(u16Brake_LeftMAX <= Left_DynamicBrake1)  //���ֵ�����½������ж�
								{
										u16Brake_LeftMAX = Left_DynamicBrake1;
										u16Left_Number = 0;
								}
								else
								{
										u16Left_Number ++;
								}
								if((Left_DynamicBrake1 > u16Brake_Stop70)&&(u8Left_Judgeflg < 4))  //����ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0'; 
										Left_StopTime.u16_data = u16Count800_Left;
								}	
						}
						/* ���ƶ����� */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//ȡƽ��ֵ
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//���ѹֵ		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //����daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if((u16Count800_Right < 800) && (u8Right_Judgeflg < 10))
						{
								if(Right_DynamicBrake1 < u16Brake_Stop90)
								{
										u16Right_Data[u16Count800_Right++] = Right_DynamicBrake1;
								}									
								if(u16Brake_RightMAX <= Right_DynamicBrake1)  //���ֵ�����½������ж�
								{
										u16Brake_RightMAX = Right_DynamicBrake1;
										u16Right_Number = 0;
								}
								else
								{
										u16Right_Number ++;
								}
								if((Right_DynamicBrake1 > u16Brake_Stop70)&&(u8Right_Judgeflg < 4))  //����ͣ��
								{
										Right_Machine = IO_CLOSE;							//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  	
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
							}	
						}							

						//��ͣ���ж�
						if(u8Left_Judgeflg == 0)  // <20%�ж�
						{
								u16Left_Number = 0;
								if(Left_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Left_Judgeflg = 1;												
								}
						}
						else if(u8Left_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Left_Number > con_LDN20)  //ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;   
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}
								else
								{
										if(Left_DynamicBrake1 > u16Brake_Protect60)
										{
												u8Left_Judgeflg = 2;
												u16Left_Number = 0;
										}
								}
						}
						else if(u8Left_Judgeflg == 2) // >60%�ж�
						{										
								if(u16Left_Number > con_LDN60)  //ͣ��
								{
										Left_Machine = IO_CLOSE;			//����ֹͣ
										USER_SystemFlag.A.Flag_Ldynamo = 0;						
										u8Left_Judgeflg = 4;  		
										//ͣ������  ͣ��ʱ��
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						else if(u8Left_Judgeflg == 3)  //Ӳͣ��10���½������ɼ�
						{
								if(u16Left_Number > con_LDNStop)   //�ж��½�ʮ����
								{
										u8Left_Judgeflg = 4;    
								}
						}								
						else if(u8Left_Judgeflg == 4)   //����ͣ������������
						{
								if(Left_DynamicBrake1 < 200)
								{
										if(u16Count800_Left<800)
										{
												u16Left_Data[u16Count800_Left++] = 0;
										}
										u8Left_Judgeflg = 10;
								}
						}
						if(u8Left_Judgeflg == 10)
						{
								Left_Machine = IO_CLOSE;													 //����ֹͣ
								USER_SystemFlag.A.Flag_Ldynamo = 0;
						}
						
						//��ͣ���ж�
						if(u8Right_Judgeflg == 0)  // <20%�ж�
						{
								u16Right_Number = 0;
								if(Right_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Right_Judgeflg = 1;												
								}
						}
						else if(u8Right_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Right_Number > con_RDN20)  //ͣ��
								{
										Right_Machine = IO_CLOSE;								//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;   
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
								else
								{
										if(Right_DynamicBrake1 > u16Brake_Protect60)
										{
												u8Right_Judgeflg = 2;
												u16Right_Number = 0;
										}
								}
						}
						else if(u8Right_Judgeflg == 2) // >60%�ж�
						{										
								if(u16Right_Number > con_RDN60)  //ͣ��
								{
										Right_Machine = IO_CLOSE;							//�ҵ��ֹͣ
										USER_SystemFlag.A.Flag_Rdynamo = 0;									
										u8Right_Judgeflg = 4;  		
										//ͣ������  ͣ��ʱ��
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
						else if(u8Right_Judgeflg == 3)  //Ӳͣ��10���½������ɼ�
						{
								if(u16Right_Number > con_RDNStop)   
								{
										u8Right_Judgeflg = 4;    
								}
						}	
						else if(u8Right_Judgeflg == 4)   //����ͣ������������
						{
								if(Right_DynamicBrake1 < 200)
								{
										if(u16Count800_Right<800)
										{
												u16Right_Data[u16Count800_Right++] = 0;
										}
										u8Right_Judgeflg = 10;
								}
						}							
						if(u8Right_Judgeflg == 10)
						{
								Right_Machine = IO_CLOSE;													 //�ҵ��ֹͣ							
								USER_SystemFlag.A.Flag_Rdynamo = 0;
						}
						if((u8Left_Judgeflg == 10) && (u8Right_Judgeflg == 10))
						{
								Machine_Switch = IO_CLOSE;												//�������л��ر�
								USER_SystemFlag.A.Flag_Switch = 0;
								System_State = Default_State;
						}
				}
		}	
		USER_SystemFlag.A.Flag_TestC = 0;
		USER_SystemFlag.A.Flag_TestC1 = 0;
		SysParameter.DynamoStart_Flag = 0;   						 //���ش���״̬���ж�Ӳ��ͣ�����
		if(u16Count800_Left < u16Count800_Right)       //�����������ͬ����
		{
				for(i=u16Count800_Left;i<u16Count800_Right;i++)
				{
						u16Left_Data[i] = 0;
				}
				u16countMax = u16Count800_Right;
		}
		else
		{
				if(u16Count800_Left == u16Count800_Right)
				{
						u16countMax = u16Count800_Right;
				}
				else
				{
						for(i=u16Count800_Right;i<u16Count800_Left;i++)
						{
								u16Right_Data[i] = 0;
						}
						u16countMax = u16Count800_Left;
				}
		}
		Deal_Data(u16countMax,u8Tran_CurrentJ,'J');  //�����ƶ����ݱ��浽�ϴ����ݵ�Ԫ
		Fun_DealJJ(u16Count800_Left,u16Count800_Right);
		System_State = ReadData_State; 	   //�����ȡ����״̬
		if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
		{
					W5500_Send_Data(2,con_TranT0);									 //�ϴ�������
					if(USER_SystemFlag.A.Flag_AutoD == 1)
					{
							USER_SystemFlag.A.Flag_AutoD = 0;
							W5500_Send_Data(2,u8Tran_CurrentJ);				//���͵�ǰ�����ƶ���������
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
					{												 
							 FunW5500_SendGB(0,Left_BrakeSumJJ,Right_BrakeSumJJ);   //�ϴ��ƶ������ս��
							 Delay_ms(20);
							 W5500_Send_Data(0,con_TranT0);									 //�ϴ�������
					}
		}  
}


/*******************************************************************************
* ������  : Deal_Data()
* ����    : �ƶ��������ݴ���
* ����    :  Number:������������,Data:���ݴ洢����,Command:������
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Deal_Data(uint16_t Number,uint8_t *Data,uint8_t Command)
{
		union{
          u8   u8_Data[2];
          u16  u16_Data;
     }aaa;
		 uint16_t i;
		 aaa.u16_Data = Number*4+5;
		 *Data++ = aaa.u8_Data[0];
		 *Data++ = aaa.u8_Data[1];
		 *Data++ = 'D';
		 *Data++ = Command;
		 aaa.u16_Data = Number;
		 *Data++ = aaa.u8_Data[0];
		 *Data++ = aaa.u8_Data[1];
		 *Data++ = 10;                      //��������
		 for(i=0;i<Number;i++)
		 {
					aaa.u16_Data = u16Left_Data[i]; //�������
				 *Data++ = aaa.u8_Data[0];
				 *Data++ = aaa.u8_Data[1];
			 
					aaa.u16_Data = u16Right_Data[i]; //�ұ�����
				 *Data++ = aaa.u8_Data[0];
				 *Data++ = aaa.u8_Data[1];
		 }
}
/*******************************************************************************
* ������  : FunW5500_SendGW()
* ����    : �ϴ����ض�̬����
* ����    :  s:�˿�,  L_Data:��������,R_Data:��������
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendGW(uint8_t s,uint16_t L_Data,uint16_t R_Data)
{
		Tx_Buffer[0] = 14;  //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'G'; 
		Tx_Buffer[3] = 'W'; 
		/* ���� */
		Tx_Buffer[4] = L_Data/10000; 
		if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
		else Tx_Buffer[4] = 0x20;
		Tx_Buffer[5] = L_Data%10000/1000; 
		if((Tx_Buffer[5] == 0) && (Tx_Buffer[4] == 0x20)) Tx_Buffer[5] = 0x20;
		else Tx_Buffer[5] += 0x30;
		Tx_Buffer[6] = L_Data%1000/100; 
		if((Tx_Buffer[6] == 0) && (Tx_Buffer[5] == 0x20)) Tx_Buffer[6] = 0x20;
		else Tx_Buffer[6] += 0x30;
		Tx_Buffer[7] = L_Data%100/10; 
		if((Tx_Buffer[7] == 0) && (Tx_Buffer[6] == 0x20)) Tx_Buffer[7] = 0x20;
		else Tx_Buffer[7] += 0x30;
		Tx_Buffer[8] = L_Data%10 + 0x30; 
		Tx_Buffer[9] = 0x20; 
		/* ���� */
		Tx_Buffer[10] = R_Data/10000; 
		if(Tx_Buffer[10] > 0) Tx_Buffer[10] += 0x30;
		else Tx_Buffer[10] = 0x20;
		Tx_Buffer[11] = R_Data%10000/1000; 
		if((Tx_Buffer[11] == 0) && (Tx_Buffer[10] == 0x20)) Tx_Buffer[11] = 0x20;
		else Tx_Buffer[11] += 0x30;
		Tx_Buffer[12] = R_Data%1000/100; 
		if((Tx_Buffer[12] == 0) && (Tx_Buffer[11] == 0x20)) Tx_Buffer[12] = 0x20;
		else Tx_Buffer[12] += 0x30;
		Tx_Buffer[13] = R_Data%100/10; 
		if((Tx_Buffer[13] == 0) && (Tx_Buffer[12] == 0x20)) Tx_Buffer[13] = 0x20;
		else Tx_Buffer[13] += 0x30;
		Tx_Buffer[14] = R_Data%10 + 0x30; 
		Tx_Buffer[15] = 0x20; 
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ����
}
/*******************************************************************************
* ������  : FunW5500_SendGB()
* ����    : �ϴ��ƶ���̬����
* ����    :  s:�˿�,  L_Data:��������,R_Data:���ƶ���
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendGB(uint8_t s,uint16_t L_Data,uint16_t R_Data)
{
		Tx_Buffer[0] = 14;  //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'G'; 
		Tx_Buffer[3] = 'B'; 
		/* ���� */
		Tx_Buffer[4] = L_Data/10000; 
		if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
		else Tx_Buffer[4] = 0x20;
		Tx_Buffer[5] = L_Data%10000/1000; 
		if((Tx_Buffer[5] == 0) && (Tx_Buffer[4] == 0x20)) Tx_Buffer[5] = 0x20;
		else Tx_Buffer[5] += 0x30;
		Tx_Buffer[6] = L_Data%1000/100; 
		if((Tx_Buffer[6] == 0) && (Tx_Buffer[5] == 0x20)) Tx_Buffer[6] = 0x20;
		else Tx_Buffer[6] += 0x30;
		Tx_Buffer[7] = L_Data%100/10; 
		if((Tx_Buffer[7] == 0) && (Tx_Buffer[6] == 0x20)) Tx_Buffer[7] = 0x20;
		else Tx_Buffer[7] += 0x30;
		Tx_Buffer[8] = L_Data%10 + 0x30; 
		Tx_Buffer[9] = 0x20; 
		/* ���� */
		Tx_Buffer[10] = R_Data/10000; 
		if(Tx_Buffer[10] > 0) Tx_Buffer[10] += 0x30;
		else Tx_Buffer[10] = 0x20;
		Tx_Buffer[11] = R_Data%10000/1000; 
		if((Tx_Buffer[11] == 0) && (Tx_Buffer[10] == 0x20)) Tx_Buffer[11] = 0x20;
		else Tx_Buffer[11] += 0x30;
		Tx_Buffer[12] = R_Data%1000/100; 
		if((Tx_Buffer[12] == 0) && (Tx_Buffer[11] == 0x20)) Tx_Buffer[12] = 0x20;
		else Tx_Buffer[12] += 0x30;
		Tx_Buffer[13] = R_Data%100/10; 
		if((Tx_Buffer[13] == 0) && (Tx_Buffer[12] == 0x20)) Tx_Buffer[13] = 0x20;
		else Tx_Buffer[13] += 0x30;
		Tx_Buffer[14] = R_Data%10 + 0x30; 
		Tx_Buffer[15] = 0x20; 
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ����
}
/*******************************************************************************
* ������  : FunW5500_SendGF()
* ����    : �ϴ�����������̬����
* ����    :  s:�˿�,  ALL_Data:��������
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendGF(uint8_t s,uint16_t ALL_Data)
{
		Tx_Buffer[0] = 8;  //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'G'; 
		Tx_Buffer[3] = 'F'; 
		/* �������� */
		Tx_Buffer[4] = ALL_Data/10000; 
		if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
		else Tx_Buffer[4] = 0x20;
		Tx_Buffer[5] = ALL_Data%10000/1000; 
		if((Tx_Buffer[5] == 0) && (Tx_Buffer[4] == 0x20)) Tx_Buffer[5] = 0x20;
		else Tx_Buffer[5] += 0x30;
		Tx_Buffer[6] = ALL_Data%1000/100; 
		if((Tx_Buffer[6] == 0) && (Tx_Buffer[5] == 0x20)) Tx_Buffer[6] = 0x20;
		else Tx_Buffer[6] += 0x30;
		Tx_Buffer[7] = ALL_Data%100/10; 
		if((Tx_Buffer[7] == 0) && (Tx_Buffer[6] == 0x20)) Tx_Buffer[7] = 0x20;
		else Tx_Buffer[7] += 0x30;
		Tx_Buffer[8] = ALL_Data%10 + 0x30; 
		Tx_Buffer[9] = 0x20; 
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ����
}
/*******************************************************************************
* ������  : FunW5500_SendDF()
* ����    : �ϴ�����������̬����
* ����    :  s:�˿�,  ALL_Data:��������
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendDF(uint8_t s,uint8_t a,uint16_t ALL_Data)
{
		Tx_Buffer[0] = 8;  //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'D'; 
		Tx_Buffer[3] = 'F'; 
		Tx_Buffer[4] = a+0x30;
		/* �������� */
		Tx_Buffer[5] = ALL_Data/10000; 
		if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
		else Tx_Buffer[5] = 0x20;
		Tx_Buffer[6] = ALL_Data%10000/1000; 
		if((Tx_Buffer[6] == 0) && (Tx_Buffer[5] == 0x20)) Tx_Buffer[6] = 0x20;
		else Tx_Buffer[6] += 0x30;
		Tx_Buffer[7] = ALL_Data%1000/100; 
		if((Tx_Buffer[7] == 0) && (Tx_Buffer[6] == 0x20)) Tx_Buffer[7] = 0x20;
		else Tx_Buffer[7] += 0x30;
		Tx_Buffer[8] = ALL_Data%100/10; 
		if((Tx_Buffer[8] == 0) && (Tx_Buffer[7] == 0x20)) Tx_Buffer[8] = 0x20;
		else Tx_Buffer[8] += 0x30;
		Tx_Buffer[9] = ALL_Data%10 + 0x30; 
		Tx_Buffer[10] = 0x20; 
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ����
}
/*******************************************************************************
* ������  : FunW5500_SendLED()
* ����    : ����ʱ�ϴ��������ʾ
* ����    :  s:�˿�,  L_Data:�����,R_Data:�������
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendLED(uint8_t s,uint16_t L_Data,uint16_t R_Data)
{
		_union_u16_u8_type u16Data;
		Tx_Buffer[0] = 8;    //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'N'; 
		Tx_Buffer[3] = 'H'; 
		/* ��LED */
	  u16Data.u16_data = L_Data;
		Tx_Buffer[4] = u16Data.u8_data[0];
		Tx_Buffer[5] = u16Data.u8_data[1];
		/* ��LED */
		u16Data.u16_data = R_Data;
		Tx_Buffer[6] = u16Data.u8_data[0];
		Tx_Buffer[7] = u16Data.u8_data[1];
	  /* ָʾ�� */
		 u16Data.u16_data = USER_LEDFlag.Init;
		Tx_Buffer[8] = u16Data.u8_data[0];
		Tx_Buffer[9] = u16Data.u8_data[1];
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ����
}
/*******************************************************************************
* ������  : FunW5500_SendDW()
* ����    : �ϴ����ؽ������
* ����    :  s:�˿�
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendDW(uint8_t s)
{
		uint8_t i;
		Tx_Buffer[0] = DynamicWeight_Number*12+3;  //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'D'; 
		Tx_Buffer[3] = 'W'; 
		Tx_Buffer[4] = DynamicWeight_Number;
		for(i=0;i<DynamicWeight_Number;i++)
		{
					/* ���� */
					Tx_Buffer[5+i*12] = Left_DynamicWeight1[i]/10000; 
					if(Tx_Buffer[5+i*12] > 0) Tx_Buffer[5+i*12] += 0x30;
					else Tx_Buffer[5+i*12] = 0x20;
					Tx_Buffer[6+i*12] = Left_DynamicWeight1[i]%10000/1000; 
					if((Tx_Buffer[6+i*12] == 0) && (Tx_Buffer[5+i*12] == 0x20)) Tx_Buffer[6+i*12] = 0x20;
					else Tx_Buffer[6+i*12] += 0x30;
					Tx_Buffer[7+i*12] = Left_DynamicWeight1[i]%1000/100; 
					if((Tx_Buffer[7+i*12] == 0) && (Tx_Buffer[6+i*12] == 0x20)) Tx_Buffer[7+i*12] = 0x20;
					else Tx_Buffer[7+i*12] += 0x30;
					Tx_Buffer[8+i*12] = Left_DynamicWeight1[i]%100/10; 
					if((Tx_Buffer[8+i*12] == 0) && (Tx_Buffer[7+i*12] == 0x20)) Tx_Buffer[8+i*12] = 0x20;
					else Tx_Buffer[8+i*12] += 0x30;
					Tx_Buffer[9+i*12] = Left_DynamicWeight1[i]%10 + 0x30; 
					Tx_Buffer[10+i*12] = 0x20; 
					/* ���� */
					Tx_Buffer[11+i*12] = Right_DynamicWeight1[i]/10000; 
					if(Tx_Buffer[11+i*12] > 0) Tx_Buffer[11+i*12] += 0x30;
					else Tx_Buffer[11+i*12] = 0x20;
					Tx_Buffer[12+i*12] = Right_DynamicWeight1[i]%10000/1000; 
					if((Tx_Buffer[12+i*12] == 0) && (Tx_Buffer[11+i*12] == 0x20)) Tx_Buffer[12+i*12] = 0x20;
					else Tx_Buffer[12+i*12] += 0x30;
					Tx_Buffer[13+i*12] = Right_DynamicWeight1[i]%1000/100; 
					if((Tx_Buffer[13+i*12] == 0) && (Tx_Buffer[12+i*12] == 0x20)) Tx_Buffer[13+i*12] = 0x20;
					else Tx_Buffer[13+i*12] += 0x30;
					Tx_Buffer[14+i*12] = Right_DynamicWeight1[i]%100/10; 
					if((Tx_Buffer[14+i*12] == 0) && (Tx_Buffer[13+i*12] == 0x20)) Tx_Buffer[14+i*12] = 0x20;
					else Tx_Buffer[14+i*12] += 0x30;
					Tx_Buffer[15+i*12] = Right_DynamicWeight1[i]%10 + 0x30; 
					Tx_Buffer[16+i*12] = 0x20; 
	  }
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ����
}
/*******************************************************************************
* ������  : FunW5500_SendT1()
* ����    : �ϴ������Ͳ���ʧ��
* ����    :  s:�˿�,  ALL_Data:����
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendT1(uint8_t s,uint16_t ALL_Data)
{
		Tx_Buffer[0] = 8;  //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'T'; 
		Tx_Buffer[3] = '1'; 
		/* ������ */
		Tx_Buffer[4] = ALL_Data/10000; 
		if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
		else Tx_Buffer[4] = 0x20;
		Tx_Buffer[5] = ALL_Data%10000/1000; 
		if((Tx_Buffer[5] == 0) && (Tx_Buffer[4] == 0x20)) Tx_Buffer[5] = 0x20;
		else Tx_Buffer[5] += 0x30;
		Tx_Buffer[6] = ALL_Data%1000/100; 
		if((Tx_Buffer[6] == 0) && (Tx_Buffer[5] == 0x20)) Tx_Buffer[6] = 0x20;
		else Tx_Buffer[6] += 0x30;
		Tx_Buffer[7] = ALL_Data%100/10; 
		if((Tx_Buffer[7] == 0) && (Tx_Buffer[6] == 0x20)) Tx_Buffer[7] = 0x20;
		else Tx_Buffer[7] += 0x30;
		Tx_Buffer[8] = ALL_Data%10 + 0x30; 
		Tx_Buffer[9] = 0x20; 
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ����
}
/*******************************************************************************
* ������  : FunW5500_SendT2()
* ����    : �ϴ������Ͳ���ʧ��
* ����    :  s:�˿�,  ALL_Data:����
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendT2(uint8_t s,uint16_t ALL_Data)
{
		Tx_Buffer[0] = 8;  //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'T'; 
		Tx_Buffer[3] = '2'; 
		/* ������ */
		Tx_Buffer[4] = ALL_Data/10000; 
		if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
		else Tx_Buffer[4] = 0x20;
		Tx_Buffer[5] = ALL_Data%10000/1000; 
		if((Tx_Buffer[5] == 0) && (Tx_Buffer[4] == 0x20)) Tx_Buffer[5] = 0x20;
		else Tx_Buffer[5] += 0x30;
		Tx_Buffer[6] = ALL_Data%1000/100; 
		if((Tx_Buffer[6] == 0) && (Tx_Buffer[5] == 0x20)) Tx_Buffer[6] = 0x20;
		else Tx_Buffer[6] += 0x30;
		Tx_Buffer[7] = ALL_Data%100/10; 
		if((Tx_Buffer[7] == 0) && (Tx_Buffer[6] == 0x20)) Tx_Buffer[7] = 0x20;
		else Tx_Buffer[7] += 0x30;
		Tx_Buffer[8] = ALL_Data%10 + 0x30; 
		Tx_Buffer[9] = 0x20; 
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ����
}
/*******************************************************************************
* ������  : Fun_ReadData()
* ����    : ��ȡ���ݺ���
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_ReadData(void)
{
			uint8_t u8TimeCount_1s = 0;
			TIM2_1ms = 0;//��ʱ��������
			USER_SystemFlag.A.Flag_1s = 0;
			while(System_State == ReadData_State)
			{
					W5500_Receive_Deal(); //����ͨ�Ų�ѯ����
					if(USER_SystemFlag.A.Flag_1s == 1)
					{
							 USER_SystemFlag.A.Flag_1s = 0;
							 u8TimeCount_1s ++;
					}
					if(u8TimeCount_1s > 8)
					{
								System_State = Default_State;
					}
			}
}
/*******************************************************************************
* ������  : Fun_DealZZ()
* ����    : �������ݴ�����
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_DealZZ(uint16_t Number)
{
		float fl32Left_Sum = 0,
					 fl32Right_Sum = 0; //��
	  uint16_t i_count;  //����ʹ��
		for(i_count=0;i_count<Number;i_count++)
		{
					fl32Left_Sum += u16Left_Data[i_count];
					fl32Right_Sum += u16Right_Data[i_count];
		}
		Left_BrakeAve = fl32Left_Sum/Number;
		Left_BrakeAve = (Left_BrakeAve+5)/10;
		Left_BrakeAve = Left_BrakeAve*10;
		
		Right_BrakeAve = fl32Right_Sum/Number;
		Right_BrakeAve = (Right_BrakeAve+5)/10;
		Right_BrakeAve = Right_BrakeAve*10;
}
/*******************************************************************************
* ������  : Fun_DealCC()
* ����    : �����ƶ����ݴ�����
* ����    :  Left_Number:���ƶ�����������  Right_Number:���ƶ�����������
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_DealCC(uint16_t Left_Number,uint16_t Right_Number)
{
		float fl32Left_Brake = 0,
					  fl32Right_Brake = 0;
	  uint16_t i,j,MaxNumber,MinNumber;
		uint16_t temp = 0;
		/****�������ƽ��*****/
		if( Left_Number >= 4 )  //4��ƽ��
		{
				for( i = 0; i < Left_Number - 4; i++ )
			  {
						 fl32Left_Brake = 0;
						 fl32Left_Brake += u16Left_Data[i];
						 fl32Left_Brake += u16Left_Data[i+1];
						 fl32Left_Brake += u16Left_Data[i+2];
					   fl32Left_Brake += u16Left_Data[i+3];
						 u16Left_Data[i] = fl32Left_Brake/4;
						 u16Left_Data[i] += 5;
						 u16Left_Data[i] = u16Left_Data[i]/10;
						 u16Left_Data[i] = u16Left_Data[i]*10;
				}
				for( i = 0; i < 4; i++ )
				{
						 fl32Left_Brake = 0;
						 for( j = Left_Number - 4 + i; j < Left_Number; j++ )
						 {
									fl32Left_Brake += u16Left_Data[j]; 
						 }
						 u16Left_Data[Left_Number - 4 + i] = fl32Left_Brake / (4-i)/10;
						 u16Left_Data[Left_Number - 4 + i] = u16Left_Data[Left_Number - 4 + i]*10;		//ʹ��ʾ������ϴ�һ�£����һλ����Ϊ��
				}
// 				Left_Number -= 3;
		}
    /****�ұ�����ƽ��*****/
		if( Right_Number >= 4 )  //4��ƽ��
		{
				for( i = 0; i < Right_Number - 4; i++ )
			  {
						 fl32Right_Brake = 0;
						 fl32Right_Brake += u16Right_Data[i];
						 fl32Right_Brake += u16Right_Data[i+1];
						 fl32Right_Brake += u16Right_Data[i+2];
					   fl32Right_Brake += u16Right_Data[i+3];
						 u16Right_Data[i] = fl32Right_Brake/4;
						 u16Right_Data[i] += 5;
						 u16Right_Data[i] = u16Right_Data[i]/10;
						 u16Right_Data[i] = u16Right_Data[i]*10;
				}
				for( i = 0; i < 4; i++ )
				{
						 fl32Right_Brake = 0;
						 for( j = Right_Number - 4 + i; j < Right_Number; j++ )
						 {
									fl32Right_Brake += u16Right_Data[j]; 
						 }
						 u16Right_Data[Right_Number - 4 + i] = fl32Right_Brake / (4-i)/10;
						 u16Right_Data[Right_Number - 4 + i] = u16Right_Data[Right_Number - 4 + i]*10;		//ʹ��ʾ������ϴ�һ�£����һλ����Ϊ��
				}
// 				Right_Number -= 3;
		}
	  if(Left_Number < Right_Number)
		{
				MaxNumber = Right_Number;
				MinNumber = Left_Number;
				for(i = MinNumber;i < MaxNumber;i++)
				{
						u16Left_Data[i] = 0;
				}
		}
		else
		{
				MaxNumber = Left_Number;
				MinNumber = Right_Number;
				for(i = MinNumber;i < MaxNumber;i++)
				{
						u16Right_Data[i] = 0;
				}
		}
		/*********�ƺ�***************/
		Left_BrakeSum = 0;          //���ƶ������ֵ  �����ƶ�
		Right_BrakeSum = 0;         //���ƶ������ֵ  �����ƶ�
		for(i = 0;i < MaxNumber;i++)
		{
				 if(Left_BrakeSum < u16Left_Data[i])  Left_BrakeSum = u16Left_Data[i];
				 if(Right_BrakeSum < u16Right_Data[i])  Right_BrakeSum = u16Right_Data[i];
		}
		/*********�Ʋ�***************/
		Left_BrakeSub = 0;          //���Ʋ����ֵ  �����ƶ�
		Right_BrakeSub = 0;         //���Ʋ����ֵ  �����ƶ�
		for(i = 0;i < MinNumber;i++)
		{
				 if(u16Right_Data[i] < u16Left_Data[i])
				 {
							if(temp < (u16Left_Data[i] - u16Right_Data[i]))
							{
									temp = u16Left_Data[i] - u16Right_Data[i];
									Left_BrakeSub = u16Left_Data[i];
									Right_BrakeSub = u16Right_Data[i];
							}
				 }
				 else
				 {
							if(temp < (u16Right_Data[i] - u16Left_Data[i]))
							{
									temp = u16Right_Data[i] - u16Left_Data[i];
									Left_BrakeSub = u16Left_Data[i];
									Right_BrakeSub = u16Right_Data[i];
							}
				 }
		}
		
}
/*******************************************************************************
* ������  : Fun_DealJJ()
* ����    : �����ƶ����ݴ�����
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void Fun_DealJJ(uint16_t Left_Number,uint16_t Right_Number)
{
		float fl32Left_Brake = 0,
					  fl32Right_Brake = 0;
	  uint16_t i,j,MaxNumber,MinNumber;
		uint16_t temp = 0;
		/****�������ƽ��*****/
		if( Left_Number >= 4 )  //4��ƽ��
		{
				for( i = 0; i < Left_Number - 4; i++ )
			  {
						 fl32Left_Brake = 0;
						 fl32Left_Brake += u16Left_Data[i];
						 fl32Left_Brake += u16Left_Data[i+1];
						 fl32Left_Brake += u16Left_Data[i+2];
					   fl32Left_Brake += u16Left_Data[i+3];
						 u16Left_Data[i] = fl32Left_Brake/4;
						 u16Left_Data[i] += 5;
						 u16Left_Data[i] = u16Left_Data[i]/10;
						 u16Left_Data[i] = u16Left_Data[i]*10;
				}
				for( i = 0; i < 4; i++ )
				{
						 fl32Left_Brake = 0;
						 for( j = Left_Number - 4 + i; j < Left_Number; j++ )
						 {
									fl32Left_Brake += u16Left_Data[j]; 
						 }
						 u16Left_Data[Left_Number - 4 + i] = fl32Left_Brake / (4-i)/10;
						 u16Left_Data[Left_Number - 4 + i] = u16Left_Data[Left_Number - 4 + i]*10;		//ʹ��ʾ������ϴ�һ�£����һλ����Ϊ��
				}
// 				Left_Number -= 3;
		}
    /****�ұ�����ƽ��*****/
		if( Right_Number >= 4 )  //4��ƽ��
		{
				for( i = 0; i < Right_Number - 4; i++ )
			  {
						 fl32Right_Brake = 0;
						 fl32Right_Brake += u16Right_Data[i];
						 fl32Right_Brake += u16Right_Data[i+1];
						 fl32Right_Brake += u16Right_Data[i+2];
					   fl32Right_Brake += u16Right_Data[i+3];
						 u16Right_Data[i] = fl32Right_Brake/4;
						 u16Right_Data[i] += 5;
						 u16Right_Data[i] = u16Right_Data[i]/10;
						 u16Right_Data[i] = u16Right_Data[i]*10;
				}
				for( i = 0; i < 4; i++ )
				{
						 fl32Right_Brake = 0;
						 for( j = Right_Number - 4 + i; j < Right_Number; j++ )
						 {
									fl32Right_Brake += u16Right_Data[j]; 
						 }
						 u16Right_Data[Right_Number - 4 + i] = fl32Right_Brake / (4-i)/10;
						 u16Right_Data[Right_Number - 4 + i] = u16Right_Data[Right_Number - 4 + i]*10;		//ʹ��ʾ������ϴ�һ�£����һλ����Ϊ��
				}
// 				Right_Number -= 3;
		}
	  if(Left_Number < Right_Number)
		{
				MaxNumber = Right_Number;
				MinNumber = Left_Number;
				for(i = MinNumber;i < MaxNumber;i++)
				{
						u16Left_Data[i] = 0;
				}
		}
		else
		{
				MaxNumber = Left_Number;
				MinNumber = Right_Number;
				for(i = MinNumber;i < MaxNumber;i++)
				{
						u16Right_Data[i] = 0;
				}
		}
		/*********�ƺ�***************/
		Left_BrakeSumJJ = 0;          //���ƶ������ֵ  �����ƶ�
		Right_BrakeSumJJ = 0;         //���ƶ������ֵ  �����ƶ�
		for(i = 0;i < MaxNumber;i++)
		{
				 if(Left_BrakeSumJJ < u16Left_Data[i])  Left_BrakeSumJJ = u16Left_Data[i];
				 if(Right_BrakeSumJJ < u16Right_Data[i])  Right_BrakeSumJJ = u16Right_Data[i];
		}
		/*********�Ʋ�***************/
		Left_BrakeSubJJ = 0;          //���Ʋ����ֵ  �����ƶ�
		Right_BrakeSubJJ = 0;         //���Ʋ����ֵ  �����ƶ�
		for(i = 0;i < MinNumber;i++)
		{
				 if(u16Right_Data[i] < u16Left_Data[i])
				 {
							if(temp < (u16Left_Data[i] - u16Right_Data[i]))
							{
									temp = u16Left_Data[i] - u16Right_Data[i];
									Left_BrakeSubJJ = u16Left_Data[i];
									Right_BrakeSubJJ = u16Right_Data[i];
							}
				 }
				 else
				 {
							if(temp < (u16Right_Data[i] - u16Left_Data[i]))
							{
									temp = u16Right_Data[i] - u16Left_Data[i];
									Left_BrakeSubJJ = u16Left_Data[i];
									Right_BrakeSubJJ = u16Right_Data[i];
							}
				 }
		}
}
/*******************************************************************************
* ������  : Fun_DealWeight()
* ����    : ���������ֱ��ʺ���
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
uint16_t Fun_DealWeight(uint16_t Weight)
{
		uint8_t Remainder;    //����
    uint16_t Quotient;      //��
		uint16_t aaa=0;
		if(SysParameter.Par6_Weight == 2)
		 {
				 Quotient = Weight / 2;
				 Remainder = Weight % 2;
				 if(Remainder > 0)
				 {
						aaa = (Quotient +1) * 2;																										 
				 }										
				 else					 
				 {
						aaa = Weight;
				 }
		 }
		 else if(SysParameter.Par6_Weight == 5)		 
		 {
				 Quotient = Weight / 5;
				 Remainder = Weight % 5;
				 if(Remainder < 3)
				 {
						aaa = Quotient * 5;																										 
				 }
				 else
				 {
						aaa = (Quotient +1) * 5;
				 }																									
		 }
		 else
		 {
					aaa = Weight;
		 }
		 return(aaa);
}
/*******************************************************************************
* ������  : Fun_DealDCData()
* ����    : �ϴ��������Խ��
* ����    :  
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
uint8_t Fun_DealDCData(uint8_t n,uint16_t data)
{
		uint8_t a1,a2,a3,a4,a5;
		a5 = data/10000; 
		if(a5 > 0) a5 += 0x30;
		else a5 = 0x20;
		a4 = data%10000/1000; 
		if((a4 == 0) && (a5 == 0x20)) a4 = 0x20;
		else a4 += 0x30;
		a3 = data%1000/100; 
		if((a3 == 0) && (a4 == 0x20)) a3 = 0x20;
		else a3 += 0x30;
		a2 = data%100/10; 
		if((a2 == 0) && (a3 == 0x20)) a2 = 0x20;
		else a2 += 0x30;
		a1 = data%10 + 0x30; 
	  switch(n)
		{
				case 1: return(a1); 
				case 2: return(a2); 
				case 3: return(a3); 
				case 4: return(a4); 
				case 5: return(a5); 
				default: return(0); 
		}
}
/*******************************************************************************
* ������  : FunW5500_SendDC()
* ����    : �ϴ��������Խ��
* ����    :  s:�˿�
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_SendDC(uint8_t s)
{
		Tx_Buffer[0] = 62;  //֡���� ��λ
		Tx_Buffer[1] = 0;    //֡���� ��λ
		Tx_Buffer[2] = 'D'; 
		Tx_Buffer[3] = 'D'; 
		/*������*/
		Tx_Buffer[4] = Fun_DealDCData(5,Left_BrakeAve);
		Tx_Buffer[5] = Fun_DealDCData(4,Left_BrakeAve);
		Tx_Buffer[6] = Fun_DealDCData(3,Left_BrakeAve);
		Tx_Buffer[7] = Fun_DealDCData(2,Left_BrakeAve);
		Tx_Buffer[8] = Fun_DealDCData(1,Left_BrakeAve);
		Tx_Buffer[9] = 0x20;
		/*������*/
		Tx_Buffer[10] = Fun_DealDCData(5,Right_BrakeAve);
		Tx_Buffer[11] = Fun_DealDCData(4,Right_BrakeAve);
		Tx_Buffer[12] = Fun_DealDCData(3,Right_BrakeAve);
		Tx_Buffer[13] = Fun_DealDCData(2,Right_BrakeAve);
		Tx_Buffer[14] = Fun_DealDCData(1,Right_BrakeAve);
		Tx_Buffer[15] = 0x20;
		/*������ƺ�*/
		Tx_Buffer[16] = Fun_DealDCData(5,Left_BrakeSum);
		Tx_Buffer[17] = Fun_DealDCData(4,Left_BrakeSum);
		Tx_Buffer[18] = Fun_DealDCData(3,Left_BrakeSum);
		Tx_Buffer[19] = Fun_DealDCData(2,Left_BrakeSum);
		Tx_Buffer[20] = Fun_DealDCData(1,Left_BrakeSum);
		Tx_Buffer[21] = 0x20;
		/*�ҿ����ƺ�*/
		Tx_Buffer[22] = Fun_DealDCData(5,Right_BrakeSum);
		Tx_Buffer[23] = Fun_DealDCData(4,Right_BrakeSum);
		Tx_Buffer[24] = Fun_DealDCData(3,Right_BrakeSum);
		Tx_Buffer[25] = Fun_DealDCData(2,Right_BrakeSum);
		Tx_Buffer[26] = Fun_DealDCData(1,Right_BrakeSum);
		Tx_Buffer[27] = 0x20;
		/*������Ʋ�*/
		Tx_Buffer[28] = Fun_DealDCData(5,Left_BrakeSub);
		Tx_Buffer[29] = Fun_DealDCData(4,Left_BrakeSub);
		Tx_Buffer[30] = Fun_DealDCData(3,Left_BrakeSub);
		Tx_Buffer[31] = Fun_DealDCData(2,Left_BrakeSub);
		Tx_Buffer[32] = Fun_DealDCData(1,Left_BrakeSub);
		Tx_Buffer[33] = 0x20;
		/*�ҿ����Ʋ�*/
		Tx_Buffer[34] = Fun_DealDCData(5,Right_BrakeSub);
		Tx_Buffer[35] = Fun_DealDCData(4,Right_BrakeSub);
		Tx_Buffer[36] = Fun_DealDCData(3,Right_BrakeSub);
		Tx_Buffer[37] = Fun_DealDCData(2,Right_BrakeSub);
		Tx_Buffer[38] = Fun_DealDCData(1,Right_BrakeSub);
		Tx_Buffer[39] = 0x20;
		/*������ƺ�*/
		Tx_Buffer[40] = Fun_DealDCData(5,Left_BrakeSumJJ);
		Tx_Buffer[41] = Fun_DealDCData(4,Left_BrakeSumJJ);
		Tx_Buffer[42] = Fun_DealDCData(3,Left_BrakeSumJJ);
		Tx_Buffer[43] = Fun_DealDCData(2,Left_BrakeSumJJ);
		Tx_Buffer[44] = Fun_DealDCData(1,Left_BrakeSumJJ);
		Tx_Buffer[45] = 0x20;
		/*�Ҽ����ƺ�*/
		Tx_Buffer[46] = Fun_DealDCData(5,Right_BrakeSumJJ);
		Tx_Buffer[47] = Fun_DealDCData(4,Right_BrakeSumJJ);
		Tx_Buffer[48] = Fun_DealDCData(3,Right_BrakeSumJJ);
		Tx_Buffer[49] = Fun_DealDCData(2,Right_BrakeSumJJ);
		Tx_Buffer[50] = Fun_DealDCData(1,Right_BrakeSumJJ);
		Tx_Buffer[51] = 0x20;
		/*������Ʋ�*/
		Tx_Buffer[52] = Fun_DealDCData(5,Left_BrakeSubJJ);
		Tx_Buffer[53] = Fun_DealDCData(4,Left_BrakeSubJJ);
		Tx_Buffer[54] = Fun_DealDCData(3,Left_BrakeSubJJ);
		Tx_Buffer[55] = Fun_DealDCData(2,Left_BrakeSubJJ);
		Tx_Buffer[56] = Fun_DealDCData(1,Left_BrakeSubJJ);
		Tx_Buffer[57] = 0x20;
		/*�Ҽ����Ʋ�*/
		Tx_Buffer[58] = Fun_DealDCData(5,Right_BrakeSubJJ);
		Tx_Buffer[59] = Fun_DealDCData(4,Right_BrakeSubJJ);
		Tx_Buffer[60] = Fun_DealDCData(3,Right_BrakeSubJJ);
		Tx_Buffer[61] = Fun_DealDCData(2,Right_BrakeSubJJ);
		Tx_Buffer[62] = Fun_DealDCData(1,Right_BrakeSubJJ);
		Tx_Buffer[63] = 0x20;
		W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ�����
}
/*******************************************************************************
* ������  : FunW5500_TestC()
* ����    : �ϴ����Թ����м������
* ����    :  s:�˿�
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void FunW5500_TestC(uint8_t s)
{
		_union_u16_u8_type u16Data;
	  float fl32Left_SumBrake,fl32Right_SumBrake,f32All_SumWeight;
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1,All_DynamicWeight1,Voltage_Relative;
	  if(USER_SystemFlag.A.Flag_TestC == 1)
		{
					/* ���ƶ����� */
					fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
					fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //ת���ɵ�ѹֵ
					if(fl32Left_SumBrake > Zero_LeftBrake)
					{
							Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
							Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
							Left_DynamicBrake1 += 5;
							Left_DynamicBrake1 /= 10;
							Left_DynamicBrake1 *= 10;   //����daN
					}
					else 
					{
							Left_DynamicBrake1 = 0;
					}
					/* ���ƶ����� */
					fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//ȡƽ��ֵ
					fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//���ѹֵ		
					if(fl32Right_SumBrake > Zero_RightBrake)
					{
							Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
							Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
							Right_DynamicBrake1 += 5;
							Right_DynamicBrake1 /= 10;
							Right_DynamicBrake1 *= 10;  //����daN
					}
					else 
					{
							Right_DynamicBrake1 = 0;
					}
					/* ���ؼ��� */
					f32All_SumWeight = ADShow_Samp(All_Weight_ID);//ȡƽ��ֵ
					f32All_SumWeight = f32All_SumWeight*VOLTAGE;//���ѹֵ		
					if(f32All_SumWeight > Zero_AllWeight)
					{
							Voltage_Relative = f32All_SumWeight - Zero_AllWeight;
							All_DynamicWeight1 = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
					}
					else 
					{
							All_DynamicWeight1 = 0;
					}
					
					Tx_Buffer[0] = 28;  //֡���� ��λ
					Tx_Buffer[1] = 0;    //֡���� ��λ
					Tx_Buffer[2] = 'C'; 
					Tx_Buffer[3] = 'X'; 
					Tx_Buffer[4] = 3;
					Tx_Buffer[5] = 3;
					Tx_Buffer[6] = 4;
					//ģ����
					u16Data.u16_data = Left_DynamicBrake1;
					Tx_Buffer[7] = Left_Brake_ID; //����
					Tx_Buffer[8] = u16Data.u8_data[0];
					Tx_Buffer[9] = u16Data.u8_data[1];		
					u16Data.u16_data = Right_DynamicBrake1;
					Tx_Buffer[10] = Right_Brake_ID; //����
					Tx_Buffer[11] = u16Data.u8_data[0];
					Tx_Buffer[12] = u16Data.u8_data[1];
					u16Data.u16_data = All_DynamicWeight1;
					Tx_Buffer[13] = All_Weight_ID; //����
					Tx_Buffer[14] = u16Data.u8_data[0];
					Tx_Buffer[15] = u16Data.u8_data[1];
					//������
					Tx_Buffer[16] = IN_Brake_ID;
					Tx_Buffer[17] = IO_GetInStatus(IN_Brake_ID)+0x30;
					Tx_Buffer[18] = IN_LeftStop_ID;
					Tx_Buffer[19] = IO_GetInStatus(IN_LeftStop_ID)+0x30;
					Tx_Buffer[20] = IN_RightStop_ID;
					Tx_Buffer[21] = IO_GetInStatus(IN_RightStop_ID)+0x30;
					//������
					Tx_Buffer[22] = 1;
					Tx_Buffer[23] = USER_SystemFlag.A.Flag_Ldynamo+0x30;
					Tx_Buffer[24] = 2;
					Tx_Buffer[25] = USER_SystemFlag.A.Flag_Rdynamo+0x30;
					Tx_Buffer[26] = 3;
					Tx_Buffer[27] = USER_SystemFlag.A.Flag_Switch+0x30;
					Tx_Buffer[28] = 4;
					Tx_Buffer[29] = USER_SystemFlag.A.Flag_ST+0x30;		
					W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ�����
		}
}
/*******************************************************************************
* ������  : Fun_DealParameter()
* ����    : ���Ʋ������������� ��������ʱʹ��
* ����    :  ��
* ���    : ��
* ����ֵ  :1:����������ȷ  0:��������ʧ��
* ˵��    : 
*******************************************************************************/
uint8_t Fun_DealParameter(void)
{
		uint8_t u8ParameterN,u8ParameterID;
		uint8_t i;
		_union_u16_u8_type parameter;
		u8ParameterN = Parameter_Buffer[0];
		if((u8ParameterN<1) || (u8ParameterN>15))
		{
				return 0;
		}
		for(i=0;i <u8ParameterN;i++)
		{
				u8ParameterID = Parameter_Buffer[i*3+1];
				parameter.u8_data[0] = Parameter_Buffer[i*3+2];
				parameter.u8_data[1] = Parameter_Buffer[i*3+3];
				switch(u8ParameterID)
				{
						case 10:  //��̬���ص���ʱ3-9
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par0_WeightTestTime = parameter.u16_data; //����ʱ3-9��
													}
							            break;
						}
						case 11://������Ͳ����  0  1
						{
													if(parameter.u16_data > 1)
													{
															return 0;
													}
													else
													{
															SysParameter.Par1_RollerFlag = parameter.u16_data; 
													}
							            break;
						}
						case 12://̤�������� 0 1
						{
													if(parameter.u16_data > 1)
													{
															return 0;
													}
													else
													{
															SysParameter.Par2_FootPlatFlag = parameter.u16_data; 
													}
							            break;
						}
						case 13://�ƶ�׼��ʱ�� 2-9
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //����ʱ3-9��
													}
							            break;
						 }
						case 14://���Ͳ���ʱ�� 2-9��
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //����ʱ3-9��
													}
							            break;
						}
						case 15://�ƶ�����ʱ�� 2-9��
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par5_BrakeTestTime = parameter.u16_data; //����ʱ3-9��
													}
							            break;
						 }
						case 16://����̨�ֶ�ֵ  1 2 5
						{
													if((parameter.u16_data==1) || (parameter.u16_data==2) || (parameter.u16_data==5))		
													{
															SysParameter.Par6_Weight = parameter.u16_data; //
													}
													else
													{
															return 0;
													}
							            break;
						}
						case 17://��̬ƽ����ֵ daN 0-30
						{
													if(parameter.u16_data > 200)
													{
															return 0;
													}
													else
													{
															SysParameter.Par7_LeftAverZero = parameter.u16_data; 
															u16Left_BrakeZero = SysParameter.Par7_LeftAverZero*10;
													}
							            break;
						 }
						case 18://�Ҷ�̬ƽ����ֵ daN 0-30
						{
													if(parameter.u16_data > 200)
													{
															return 0;
													}
													else
													{
															SysParameter.Par8_RightAverZero = parameter.u16_data; 
															u16Right_BrakeZero = SysParameter.Par8_RightAverZero*10;
													}
							            break;
						 }
						case 19://��̬�����ֵ daN 0-30
						{
													if(parameter.u16_data > 30)
													{
															return 0;
													}
													else
													{
															SysParameter.Par9_LeftMaxZero = parameter.u16_data; 
													}
							            break;
						 }
						case 20://�Ҷ�̬�����ֵ daN 0-30
						{
													if(parameter.u16_data > 30)
													{
															return 0;
													}
													else
													{
															SysParameter.Par10_RightMaxZero = parameter.u16_data; 
													}
							            break;
						 }
						case 21://���ؾ���������ֵ 
						{
// 													if(parameter.u16_data > 13000)
// 													{
// 															return 0;
// 													}
// 													else
// 													{
// 															SysParameter.Par11_MaxWeight = parameter.u16_data; 
// 													}
													SysParameter.Par11_MaxWeight = 11500; 
							            break;
						}
						case 22://̨������ 0-3
						{
													if(parameter.u16_data > 3)
													{
															return 0;
													}
													else
													{
															SysParameter.Par12_MachineType = parameter.u16_data; 
													}
							            break;
						}
						case 23://���ؾ����߶� 0-900
						{
													if(parameter.u16_data > 900)
													{
															return 0;
													}
													else
													{
															SysParameter.Par13_UpHeight = parameter.u16_data; 
													}
							            break;
						}
						case 24://̨�������ʱ��20-90 ��߿�����90s
						{
													if((parameter.u16_data<20) || (parameter.u16_data>90))
													{
															return 0;
													}
													else
													{
															SysParameter.Par14_UpTime = parameter.u16_data; //
													}
							            break;
						}
						default :
													return 0;
//													break;
				}
		}
		return 1;
}
/*******************************************************************************
* ������  : Fun_GetParameter()
* ����    : ��ȡ��������  
* ����    :  ��
* ���    : ��
* ����ֵ  :1:����������ȷ  0:��������ʧ��
* ˵��    : 
*******************************************************************************/
void Fun_GetParameter(void)
{
		uint8_t u8ParameterN,u8ParameterID;
		uint8_t i;
		uint8_t Edit_Flg=0;
		_union_u16_u8_type parameter;
		
		SPI_FLASH_BufferRead(Parameter_Buffer,ParameterAll_Flash_Address,100);
		u8ParameterN = Parameter_Buffer[0];
		if((u8ParameterN<1) || (u8ParameterN>15))
		{
				Parameter_Buffer[0] = 15;
				u8ParameterN = Parameter_Buffer[0];
			  //����1
				Parameter_Buffer[1] = 10;
				Parameter_Buffer[2] = 0;
				Parameter_Buffer[3] = 0;
				//����2
				Parameter_Buffer[4] = 11;
				Parameter_Buffer[5] = 0;
				Parameter_Buffer[6] = 0;
				//����3
				Parameter_Buffer[7] = 12;
				Parameter_Buffer[8] = 0;
				Parameter_Buffer[9] = 0;
				//����4
				Parameter_Buffer[10] = 13;
				Parameter_Buffer[11] = 0;
				Parameter_Buffer[12] = 0;
				//����5
				Parameter_Buffer[13] = 14;
				Parameter_Buffer[14] = 0;
				Parameter_Buffer[15] = 0;
				//����6
				Parameter_Buffer[16] = 15;
				Parameter_Buffer[17] = 0;
				Parameter_Buffer[18] = 0;
				//����7
				Parameter_Buffer[19] = 16;
				Parameter_Buffer[20] = 0;
				Parameter_Buffer[21] = 0;
				//����8
				Parameter_Buffer[22] = 17;
				Parameter_Buffer[23] = 0;
				Parameter_Buffer[24] = 0;
				//����9
				Parameter_Buffer[25] = 18;
				Parameter_Buffer[26] = 0;
				Parameter_Buffer[27] = 0;
				//����10
				Parameter_Buffer[28] = 19;
				Parameter_Buffer[29] = 0;
				Parameter_Buffer[30] = 0;
				//����11
				Parameter_Buffer[31] = 20;
				Parameter_Buffer[32] = 0;
				Parameter_Buffer[33] = 0;
				//����12
				Parameter_Buffer[34] = 21;
				Parameter_Buffer[35] = 0;
				Parameter_Buffer[36] = 0;
				//����13
				Parameter_Buffer[37] = 22;
				Parameter_Buffer[38] = 0;
				Parameter_Buffer[39] = 0;
				//����14
				Parameter_Buffer[40] = 23;
				Parameter_Buffer[41] = 0;
				Parameter_Buffer[42] = 0;
				//����15
				Parameter_Buffer[43] = 24;
				Parameter_Buffer[44] = 0;
				Parameter_Buffer[45] = 0;
				for(i=0;i <u8ParameterN;i++)
				{
						u8ParameterID = Parameter_Buffer[i*3+1];
						parameter.u8_data[0] = Parameter_Buffer[i*3+2];
						parameter.u8_data[1] = Parameter_Buffer[i*3+3];
						switch(u8ParameterID)
						{
								case 10:  //��̬���ص���ʱ3-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par0_WeightTestTime = 3; //Ĭ��3��
																	Parameter_Buffer[2] = 3;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par0_WeightTestTime = parameter.u16_data; //����ʱ3-9��
															}
															break;
								}
								case 11://������Ͳ����  0  1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par1_RollerFlag = 1; //Ĭ����
																	Parameter_Buffer[5] = 1;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par1_RollerFlag = parameter.u16_data; 
															}
															break;
								}
								case 12://̤�������� 0 1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par2_FootPlatFlag = 1;//Ĭ����
																	Parameter_Buffer[8] = 1;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par2_FootPlatFlag = parameter.u16_data; 
															}
															break;
								}
								case 13://�ƶ�׼��ʱ�� 2-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par3_BrakeReadyTime = 3; //Ĭ��3��
																	Parameter_Buffer[11] = 3;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //����ʱ3-9��
															}
															break;
								 }
								case 14://���Ͳ���ʱ�� 2-9��
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par4_ZuzhiTestTime = 5; //Ĭ��5��
																	Parameter_Buffer[14] = 5;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //����ʱ3-9��
															}
															break;
								}
								case 15://�ƶ�����ʱ�� 2-9��
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par5_BrakeTestTime = 8; //Ĭ��8��
																	Parameter_Buffer[17] = 8;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par5_BrakeTestTime = parameter.u16_data; //����ʱ3-9��
															}
															break;
								 }
								case 16://����̨�ֶ�ֵ  1 2 5
								{
															if((parameter.u16_data==1) || (parameter.u16_data==2) || (parameter.u16_data==5))		
															{
																	SysParameter.Par6_Weight = parameter.u16_data; //													
															}
															else
															{
																	SysParameter.Par6_Weight = 1; //Ĭ��1
																	Parameter_Buffer[20] = 1;
																	Edit_Flg = 1;
															}
															break;
								}
								case 17://��̬ƽ����ֵ daN 0-200
								{
															if(parameter.u16_data > 200)
															{
																	SysParameter.Par7_LeftAverZero = 0; 
																	Parameter_Buffer[23] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par7_LeftAverZero = parameter.u16_data; 
																	u16Left_BrakeZero = 0;
															}
															break;
								 }
								case 18://�Ҷ�̬ƽ����ֵ daN 0-200
								{
															if(parameter.u16_data > 200)
															{
																	SysParameter.Par8_RightAverZero = 0; 
																	Parameter_Buffer[26] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par8_RightAverZero = parameter.u16_data; 
																	u16Right_BrakeZero = 0;
															}
															break;
								 }
								case 19://��̬�����ֵ daN 0-30
								{
															if(parameter.u16_data > 30)
															{
																	SysParameter.Par9_LeftMaxZero = 0; 
																	Parameter_Buffer[29] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par9_LeftMaxZero = parameter.u16_data; 
															}
															break;
								 }
								case 20://�Ҷ�̬�����ֵ daN 0-30
								{
															if(parameter.u16_data > 30)
															{
																	SysParameter.Par10_RightMaxZero = 0;
																	Parameter_Buffer[32] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par10_RightMaxZero = parameter.u16_data; 
															}
															break;
								 }
								case 21://���ؾ���������ֵ 
								{
		// 													if(parameter.u16_data > 13000)
		// 													{
		// 															SysParameter.Par11_MaxWeight = 11500; 
		// 															Edit_Flg = 1;
		// 													}
		// 													else
		// 													{
		// 															SysParameter.Par11_MaxWeight = parameter.u16_data; 
		// 													}
															SysParameter.Par11_MaxWeight = 11500; 
															Parameter_Buffer[35] = 0xEC;
															Parameter_Buffer[36] = 0x2C;
															break;
								}
								case 22://̨������ 0-3
								{
															if(parameter.u16_data > 3)
															{
																	SysParameter.Par12_MachineType = 2; 
																	Parameter_Buffer[38] = 2;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par12_MachineType = parameter.u16_data; 
															}
															break;
								}
								case 23://���ؾ����߶� 0-900
								{
															if(parameter.u16_data > 900)
															{
																	SysParameter.Par13_UpHeight = 600; 
																	Parameter_Buffer[41] = 0x58;
																	Parameter_Buffer[42] = 0x02;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par13_UpHeight = parameter.u16_data; 
															}
															break;
								}
								case 24://̨�������ʱ��20-90 ��߿�����90s
								{
															if((parameter.u16_data<20) || (parameter.u16_data>90))
															{
																	SysParameter.Par14_UpTime = 20; //
																	Parameter_Buffer[44] = 20;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par14_UpTime = parameter.u16_data; //
															}
															break;
								}
								default :																		
															break;
						}
				}
		}
		else
		{
				for(i=0;i <u8ParameterN;i++)
				{
						u8ParameterID = Parameter_Buffer[i*3+1];
						parameter.u8_data[0] = Parameter_Buffer[i*3+2];
						parameter.u8_data[1] = Parameter_Buffer[i*3+3];
						switch(u8ParameterID)
						{
								case 10:  //��̬���ص���ʱ3-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par0_WeightTestTime = 3; //Ĭ��3��
																	Parameter_Buffer[i*3+2] = 3;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par0_WeightTestTime = parameter.u16_data; //����ʱ3-9��
															}
															break;
								}
								case 11://������Ͳ����  0  1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par1_RollerFlag = 1; //Ĭ����
																	Parameter_Buffer[i*3+2] = 1;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par1_RollerFlag = parameter.u16_data; 
															}
															break;
								}
								case 12://̤�������� 0 1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par2_FootPlatFlag = 1;//Ĭ����
																	Parameter_Buffer[i*3+2] = 1;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par2_FootPlatFlag = parameter.u16_data; 
															}
															break;
								}
								case 13://�ƶ�׼��ʱ�� 2-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par3_BrakeReadyTime = 3; //Ĭ��3��
																	Parameter_Buffer[i*3+2] = 3;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //����ʱ3-9��
															}
															break;
								 }
								case 14://���Ͳ���ʱ�� 2-9��
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par4_ZuzhiTestTime = 5; //Ĭ��5��
																	Parameter_Buffer[i*3+2] = 5;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //����ʱ3-9��
															}
															break;
								}
								case 15://�ƶ�����ʱ�� 2-9��
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par5_BrakeTestTime = 8; //Ĭ��8��
																	Parameter_Buffer[i*3+2] = 8;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par5_BrakeTestTime = parameter.u16_data; //����ʱ3-9��
															}
															break;
								 }
								case 16://����̨�ֶ�ֵ  1 2 5
								{
															if((parameter.u16_data==1) || (parameter.u16_data==2) || (parameter.u16_data==5))		
															{
																	SysParameter.Par6_Weight = parameter.u16_data; //
															}
															else
															{
																	SysParameter.Par6_Weight = 1; //Ĭ��1
																	Parameter_Buffer[i*3+2] = 1;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															break;
								}
								case 17://��̬ƽ����ֵ daN 0-200
								{
															if(parameter.u16_data > 200)
															{
																	SysParameter.Par7_LeftAverZero = 0; 
																	Parameter_Buffer[i*3+2] = 0;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par7_LeftAverZero = parameter.u16_data; 
																	u16Left_BrakeZero = SysParameter.Par7_LeftAverZero*10;
															}
															break;
								 }
								case 18://�Ҷ�̬ƽ����ֵ daN 0-30
								{
															if(parameter.u16_data > 200)
															{
																	SysParameter.Par8_RightAverZero = 0; 
																	Parameter_Buffer[i*3+2] = 0;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par8_RightAverZero = parameter.u16_data; 
																	u16Right_BrakeZero = SysParameter.Par8_RightAverZero*10;
															}
															break;
								 }
								case 19://��̬�����ֵ daN 0-30
								{
															if(parameter.u16_data > 30)
															{
																	SysParameter.Par9_LeftMaxZero = 0; 
																	Parameter_Buffer[i*3+2] = 0;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par9_LeftMaxZero = parameter.u16_data; 
															}
															break;
								 }
								case 20://�Ҷ�̬�����ֵ daN 0-30
								{
															if(parameter.u16_data > 30)
															{
																	SysParameter.Par10_RightMaxZero = 0;
																	Parameter_Buffer[i*3+2] = 0;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par10_RightMaxZero = parameter.u16_data; 
															}
															break;
								 }
								case 21://���ؾ���������ֵ 
								{
		// 													if(parameter.u16_data > 13000)
		// 													{
		// 															SysParameter.Par11_MaxWeight = 11500; 
		// 															Edit_Flg = 1;
		// 													}
		// 													else
		// 													{
		// 															SysParameter.Par11_MaxWeight = parameter.u16_data; 
		// 													}
															SysParameter.Par11_MaxWeight = 11500; 
															Parameter_Buffer[i*3+2] = 0xEC;
															Parameter_Buffer[i*3+3] = 0x2C;
															break;
								}
								case 22://̨������ 0-3
								{
															if(parameter.u16_data > 3)
															{
																	SysParameter.Par12_MachineType = 2; 
																	Parameter_Buffer[i*3+2] = 2;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par12_MachineType = parameter.u16_data; 
															}
															break;
								}
								case 23://���ؾ����߶� 0-900
								{
															if(parameter.u16_data > 900)
															{
																	SysParameter.Par13_UpHeight = 600; 
																	Parameter_Buffer[i*3+2] = 0x58;
																	Parameter_Buffer[i*3+3] = 0x02;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par13_UpHeight = parameter.u16_data; 
															}
															break;
								}
								case 24://̨�������ʱ��20-90 ��߿�����90s
								{
															if((parameter.u16_data<20) || (parameter.u16_data>90))
															{
																	SysParameter.Par14_UpTime = 20; //
																	Parameter_Buffer[i*3+2] = 20;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par14_UpTime = parameter.u16_data; //
															}
															break;
								}
								default :																		
															break;
						}
				}
		}
		if(Edit_Flg == 1)
		{
				SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//��������
				SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(u8ParameterN*3+1));//����ϵ��
		}
}
/*******************************************************************************
* ������  : W5500_ParameterReset
* ����    : ����IP����ǿ�Ƹ�λ
* ����    :  ��
* ���    : ��
* ����ֵ  :��
* ˵��    : 
*******************************************************************************/
void W5500_ParameterReset(void)
{
		 W5500_Parameter[0] = 192;
		 W5500_Parameter[1] = 168;
		 W5500_Parameter[2] = 100;
		 W5500_Parameter[3] = 1;		  //����
	 
		 W5500_Parameter[4] = 255;
		 W5500_Parameter[5] = 255;
		 W5500_Parameter[6] = 255;
		 W5500_Parameter[7] = 0;      //��������
	 
		 W5500_Parameter[8] = 192;
		 W5500_Parameter[9] = 168;
		 W5500_Parameter[10] = 100;
		 W5500_Parameter[11] = 4;     //���ذ�IP
	 
		 W5500_Parameter[12] = 0xC8;
		 W5500_Parameter[13] = 0x61;   //�˿ں�25032
	 
		 W5500_Parameter[14] = 0;
		 W5500_Parameter[15] = 0;
		 W5500_Parameter[16] = 0;
		 W5500_Parameter[17] = 0;   // �����IP
		 
		 W5500_Parameter[18] = 0;			
		 W5500_Parameter[19] = 0;    //����ܶ˿ں�
					 
		 W5500_Parameter[20] = 192;
		 W5500_Parameter[21] = 168;
		 W5500_Parameter[22] = 1;
		 W5500_Parameter[23] = 200;   //UDP�㲥Ŀ�ĵ�ַ
		 
		 W5500_Parameter[24] = 0xC7;
		 W5500_Parameter[25] = 0x61;  //UDP�˿ں�
		 
		 W5500_Parameter[26] = 0x55;
		 SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//��������
		 SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//����ϵ��		
}
