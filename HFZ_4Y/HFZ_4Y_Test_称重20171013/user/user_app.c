/****************************************************************************
* ÎÄ¼şÃû: MAIN_APP.c
* ÎÄ¼şÀúÊ·:
* °æ±¾ºÅ:    V1.0
* ÈÕÆÚ  :    2016.02.16   
* ×÷Õß :     XX
* ËµÃ÷:      Ö÷º¯Êıµ÷ÓÃµÄ×Óº¯Êı  ¸÷¸ö¼ì²â×´Ì¬º¯Êı
*****************************************************************************/
/*=============================================================================
*******ĞŞ¸ÄÈÕÖ¾*******
1¡¢ÈÕ    ÆÚ£º
µØ    µã£º
ĞŞ ¸Ä ÈË£º
Ìí¼ÓÄÚÈİ£º
°æ±¾ºÅ:
2¡¢ÈÕ    ÆÚ£º
µØ    µã£º
ĞŞ ¸Ä ÈË£º
Ìí¼ÓÄÚÈİ£º
°æ±¾ºÅ:
================================================================================*/

#include "user_app.h"


// float VOLTAGE = 10*1000/65535;
/*******************************************************************************
* º¯ÊıÃû  : void IWDG_Configuration(void)
* ÃèÊö    : ¶ÀÁ¢¿´ÃÅ¹·ÅäÖÃº¯Êı
* ÊäÈë    : ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÎŞ
* ËµÃ÷    : 
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
* º¯ÊıÃû  : Fun_Adjust
* ÃèÊö    : ÏµÍ³±ê¶¨×Ó×´Ì¬
* ÊäÈë    : ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void Fun_Adjust(void)
{
		_union_u16_u8_type Voltage_Zero, //ÁãµãµçÑ¹ÉÏ´«±äÁ¿
																				 Voltage_Real, //¾ø¶ÔµçÑ¹ÉÏ´«±äÁ¿
																				 Value_Real;		 //ÊµÊ±²âÁ¿Öµ
		float Voltage_Relative;//Ïà¶ÔµçÑ¹
	  _unionType1 Parameter;  //ÁÙÊ±±ê¶¨Í¨µÀÏµÊı

		float AD_Data_sum=0.0;//AD×ª»»½á¹ûµÄºÍ ÇóÆ½¾ùÖµÊ¹ÓÃ
//     USER_SystemFlag.A.Flag_Sucess1 = 1;
//     USER_SystemFlag.A.Flag_Standalone = 1;
		while(System_State == Adjust_State)
		{
				W5500_Receive_Deal(); //Íø¿Ú²éÑ¯º¯Êı
				switch(AD_Channel_ID)
				{
						case Left_Weight_ID:
																								Voltage_Zero.u16_data = Zero_LeftWeight;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
																								memcpy(Parameter.Load_Flash,Left_Weight_par.Load_Flash, 17);
																								break;
						case Right_Wight_ID:
																								Voltage_Zero.u16_data = Zero_RightWight;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
																								memcpy(Parameter.Load_Flash,Right_Wight_par.Load_Flash, 17);
																								break;
						case Left_Brake_ID:
																								Voltage_Zero.u16_data = Zero_LeftBrake;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ3Í¨µÀ
																								memcpy(Parameter.Load_Flash,Left_Brake_par.Load_Flash, 17);
																								break;
						case Right_Brake_ID:
																								Voltage_Zero.u16_data = Zero_RightBrake;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ4Í¨µÀ
																								memcpy(Parameter.Load_Flash,Right_Brake_par.Load_Flash, 17);
																								break;
						case All_Weight_ID:
																								Voltage_Zero.u16_data = Zero_AllWeight;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ5Í¨µÀ
																								memcpy(Parameter.Load_Flash,All_Weight_par.Load_Flash, 17);
																								break;
						case FootPlat_ID:
																								Voltage_Zero.u16_data = Zero_FootPlat;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ6Í¨µÀ
																								memcpy(Parameter.Load_Flash,FootPlat_par.Load_Flash, 17);
																								break;
						case Left_Move_ID:
																								Voltage_Zero.u16_data = Zero_LeftMove;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ7Í¨µÀ
																								memcpy(Parameter.Load_Flash,Left_Move_par.Load_Flash, 17);
																								break;
						case Right_Move_ID:
																								Voltage_Zero.u16_data = Zero_RightMove;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ8Í¨µÀ
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
						/* ÉÏ´«±ê¶¨Êı¾İ */
						Tx_Buffer[0] = 8;  //Ö¡³¤¶È µÍÎ»
						Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
						Tx_Buffer[2] = 'C'; 
						Tx_Buffer[3] = AD_Channel_ID; 
						Tx_Buffer[4] = Voltage_Zero.u8_data[0];
						Tx_Buffer[5] = Voltage_Zero.u8_data[1];
						Tx_Buffer[6] = Voltage_Real.u8_data[0];
						Tx_Buffer[7] = Voltage_Real.u8_data[1];
						Tx_Buffer[8] = Value_Real.u8_data[0];
						Tx_Buffer[9] = Value_Real.u8_data[1];
						if(USER_SystemFlag.A.Flag_Online == 1) //ºÍÖ÷¿ØÁªÍø ÉÏ´«ÄÚÈİ
						{
								W5500_Send_Data(2,Tx_Buffer);
								if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍµ¥»úÁªÍø ÉÏ´«ÄÚÈİ
								{
										if(USER_SystemFlag.A.Flag_Sucess == 1) //ÎÕÊÖ¼ÓÃÜ
										{
												USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
										}
										USER_LEDFlag.A.LED_15 = 1;  				//ÊıÂë¹ÜÊı¾İÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
										USER_LEDFlag.A.LED_14 = 1;  				//ÊıÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
										FunW5500_SendLED(0,0,Value_Real.u16_data);
								}
						}
						else
						{
								if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍµ¥»úÁªÍø ÉÏ´«ÄÚÈİ
								{
										W5500_Send_Data(0,Tx_Buffer);
								}
						}
				}
		}
}

/*******************************************************************************
* º¯ÊıÃû  : Fun_Zero
* ÃèÊö    : Ä£ÄâÍ¨µÀµ÷Áãº¯Êı
* ÊäÈë    : Channel_ID µ÷ÁãÍ¨µÀ   state:ÊÇ·ñÖ±½Óµ÷Áã 1:Ö±½Óµ÷Áã
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void Fun_Zero(uint8_t Channel_ID,uint8_t state)
{
		float AD_Data_sum=0.0;//AD×ª»»½á¹ûµÄºÍ ÇóÆ½¾ùÖµÊ¹ÓÃ
		uint16_t Voltage_Zero=0;//ÁãµãµçÑ¹

		AD_Data_sum = ADShow_Adjust(Channel_ID);
		Voltage_Zero = AD_Data_sum*VOLTAGE; //Ä£ÄâÁ¿×ª»»³ÉµçÑ¹  ·Å´ó1000±¶
#ifdef _DEBUG //µ÷ÊÔÓÃº¯Êı¶Î
		printf("\r\n Voltage_Zero%u=%umv \r\n",Channel_ID,Voltage_Zero);	
#endif	
		switch(Channel_ID)
		{
				case Left_Weight_ID:
																						if(state == 1) Zero_LeftWeight = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
																					  if(Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftWeight = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
																						}
																						break;
				case Right_Wight_ID:
																						if(state == 1) Zero_RightWight = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
																						if(Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightWight = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
																						}																					
																						break;
				case Left_Brake_ID:
																						if(state == 1) Zero_LeftBrake = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ3Í¨µÀ
																						if(Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftBrake = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ3Í¨µÀ
																						}																									
																						break;
				case Right_Brake_ID:
																						if(state == 1) Zero_RightBrake = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ4Í¨µÀ
																						if(Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightBrake = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ4Í¨µÀ
																						}
																						break;
				case All_Weight_ID:
																						if(state == 1) Zero_AllWeight = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ5Í¨µÀ
																						if(Fun_VoltageConvertValue(All_Weight_ID,Voltage_Zero) < 50)
																						{
																								Zero_AllWeight = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ5Í¨µÀ
																						}																					
																						break;
				case FootPlat_ID:
																						if(state == 1) Zero_FootPlat = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ6Í¨µÀ
																						if(Fun_VoltageConvertValue(FootPlat_ID,Voltage_Zero) < 50)
																						{
																								Zero_FootPlat = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ6Í¨µÀ
																						}																						
																						break;
				case Left_Move_ID:
																						if(state == 1) Zero_LeftMove = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ7Í¨µÀ
																						if(Fun_VoltageConvertValue(Left_Move_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftMove = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ7Í¨µÀ
																						}
																						break;
				case Right_Move_ID:
																						if(state == 1) Zero_RightMove = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ8Í¨µÀ
																						if(Fun_VoltageConvertValue(Right_Move_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightMove = Voltage_Zero;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ8Í¨µÀ
																						}																						
																						break;
				default:
																						printf("\r\nError:The AD channel is false!!!Fun_Zero\r\n");
																						break;
		}
}

/*******************************************************************************
* º¯ÊıÃû  : Fun_StaticWeightTest
* ÃèÊö    : ¾²Ì¬ÂÖÖØ²âÊÔº¯Êı
* ÊäÈë    : ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void Fun_StaticWeightTest(void) 
{
		uint8_t Countdown_time;//µ¹¼ÆÊ±³õÊ¼Öµ
		uint16_t u16Count300=0; //300¸öµã´æ´¢¼ÆÊı
		uint16_t u16countMax=0; //¼ÆÊı×î´ó´æ´¢µãÊıÁ¿
		uint16_t Voltage_Relative;//Ïà¶ÔµçÑ¹
		float fl32Left_SumWeight=0,
					 fl32Right_SumWeight=0;//ÖØÁ¿ÇóºÍÁÙÊ±±äÁ¿
		float fl32ADSum_Channen1=0,
					 fl32ADSum_Channen2=0;//ÖØÁ¿ÇóºÍÁÙÊ±±äÁ¿

		u16countMax = 0;//³õÊ¼»¯		
		if(USER_SystemFlag.A.Flag_Online == 1)  //ÁªÍø ¸øÖ÷¿Ø·¢
		{
				W5500_Send_Data(2,con_TranMW);//ÉÏ´«MW ¾²Ì¬ÂÖÖØ¼ì²â
		}
    else
    {
        if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
        {
              W5500_Send_Data(0,con_TranMW);//ÉÏ´«MW ¾²Ì¬ÂÖÖØ¼ì²â
        }
    }
		Left_DynamicWeight = 0;
		Right_DynamicWeight = 0;  //¶¯Ì¬ÏÔÊ¾ÖµÇåÁã
		Left_StaticWeight = 0;
		Right_StaticWeight = 0; //³õÊ¼»¯ÖáÖØ½á¹û
		Countdown_time = SysParameter.Par0_WeightTestTime + 1; //µ¹¼ÆÊ±Ê±¼ä
// 		Countdown_time = 2;		
		USER_SystemFlag.A.Flag_1s = 1;//Ê×´Îµ÷ÓÃÉÏ´«µ¹¼ÆÊ±µÚÒ»¸öÊıÊ¹ÓÃ
		if(USER_SystemFlag.A.Flag_TestC1 == 1)
		{
				USER_SystemFlag.A.Flag_TestC = 1;
		}
		while(System_State == StaticWeight_State)
		{
				W5500_Receive_Deal(); //ÍøÂçÍ¨ĞÅ²éÑ¯´¦Àí
				if(IO_GetInStatus(IN_Weight_ID) == 1)
				{
						if(USER_SystemFlag.A.Flag_1s == 1)
						{
								TIM2_1ms = 0;//¶¨Ê±¼ÆÊıÇåÁã
								USER_SystemFlag.A.Flag_1s = 0;
								if(Countdown_time > 0) 
								{
										Countdown_time --;
										if(Countdown_time>0)
										{
												Tx_Buffer[0] = 3;  //Ö¡³¤¶È µÍÎ»
												Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
												Tx_Buffer[2] = 'M'; 
												Tx_Buffer[3] = '1'; 
												Tx_Buffer[4] = Countdown_time;
												 if(USER_SystemFlag.A.Flag_Online == 1)  //ÁªÍø ¸øÖ÷¿Ø·¢
												 {
															W5500_Send_Data(2,Tx_Buffer);
															 if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
															 {												
																		USER_LEDFlag.A.LED_15 = 1;  				//ÊıÂë¹ÜÊı¾İÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
																		USER_LEDFlag.A.LED_14 = 0;  				//ÊıÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
																		FunW5500_SendLED(0,LED_OFF,Countdown_time);
															 }
												 }
												 else
												 {
															 if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
															 {
																		W5500_Send_Data(0,Tx_Buffer);
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
								/*×óÖØ*/
							  fl32Left_SumWeight = ADShow_Samp(Left_Weight_ID);
								fl32Left_SumWeight =fl32Left_SumWeight*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
								if(fl32Left_SumWeight > Zero_LeftWeight)
								{
										Voltage_Relative = fl32Left_SumWeight - Zero_LeftWeight;
										Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
								}
								else 
								{
										Left_DynamicWeight = 0;
								}
								u16Left1_TemporaryWeight[u16Count300] = Left_DynamicWeight;
								/*ÓÒÖØ*/		
								fl32Right_SumWeight = ADShow_Samp(Right_Wight_ID);
								fl32Right_SumWeight = fl32Right_SumWeight*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
								if(fl32Right_SumWeight > Zero_RightWight)
								{
										Voltage_Relative = fl32Right_SumWeight - Zero_RightWight;
										Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
								}
								else 
								{
										Right_DynamicWeight = 0;
								}
								u16Right1_TemporaryWeight[u16Count300] = Right_DynamicWeight;				
								u16Count300 ++;
								if(u16Count300 >= 300) u16Count300 = 0;							
						}
				}
				else
				{
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
									USER_SystemFlag.A.Flag_500ms = 0;
									/*×óÖØ*/
									fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);
									fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
									if(fl32ADSum_Channen1 > Zero_LeftWeight)
									{
											Voltage_Relative = fl32ADSum_Channen1 - Zero_LeftWeight;
											Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
											Left_DynamicWeight = Fun_DealWeight(Left_DynamicWeight);
									}
									else 
									{
											Left_DynamicWeight = 0;
									}
									/*ÓÒÖØ*/
									fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
									fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
									if(fl32ADSum_Channen2 > Zero_RightWight)
									{
											Voltage_Relative = fl32ADSum_Channen2 - Zero_RightWight;
											Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
											Right_DynamicWeight = Fun_DealWeight(Right_DynamicWeight);
									}
									else 
									{
											Right_DynamicWeight = 0;
									}
									if(USER_SystemFlag.A.Flag_Online == 1) //ºÍÖ÷¿ØÁªÍø ÉÏ´«ÄÚÈİ
								 {
											if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍÊıÂë¹ÜÁªÍøÁªÍø ÉÏ´«ÄÚÈİ
											{
													if(USER_SystemFlag.A.Flag_Sucess == 1) //ÎÕÊÖ¼ÓÃÜ
													{
															USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
													}
													USER_LEDFlag.A.LED_15 = 1;  				//ÊıÂë¹ÜÊı¾İÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
													USER_LEDFlag.A.LED_14 = 1;  				//ÊıÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
													USER_LEDFlag.A.LED_Weight = 1;  			//ÖØÁ¿Ö¸Ê¾µÆ
													USER_LEDFlag.A.LED_Brake = 0;  			  //ÖÆ¶¯Ö¸Ê¾µÆ
													FunW5500_SendLED(0,Left_DynamicWeight,Right_DynamicWeight);	
											}
								 }
								 else
								 {
											if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍÊıÂë¹ÜÁªÍøÁªÍø ÉÏ´«ÄÚÈİ
											{
// 													FunW5500_SendGW(0,Left_DynamicWeight,Right_DynamicWeight);	
													W5500_Send_Data(0,con_TranT4); //»Ø´«³µÁ¾²»µ½Î»ĞÅºÅ
											}
								 }
					 }
						Countdown_time = SysParameter.Par0_WeightTestTime + 1; //µ¹¼ÆÊ±Ê±¼ä
						TIM2_1ms = 0;//¶¨Ê±¼ÆÊıÇåÁã
						USER_SystemFlag.A.Flag_1s = 1;//Ê×´Îµ÷ÓÃÉÏ´«µ¹¼ÆÊ±µÚÒ»¸öÊıÊ¹ÓÃ
						u16countMax = 0;//³õÊ¼»¯
				}
		}		
		USER_SystemFlag.A.Flag_TestC = 0;
		USER_SystemFlag.A.Flag_TestC1 = 0;
		#ifdef _DEBUG //µ÷ÊÔÓÃº¯Êı¶Î
		printf("\r\n%u\r\n",u16countMax);
		#endif		
		if(u16countMax >= 300) u16countMax = 300;
		fl32Left_SumWeight = 0;
		fl32Right_SumWeight = 0;
		for(u16Count300=4;u16Count300<u16countMax;u16Count300++) //
		{
				fl32Left_SumWeight += u16Left1_TemporaryWeight[u16Count300];
				fl32Right_SumWeight += u16Right1_TemporaryWeight[u16Count300];
		}
		Left_StaticWeight = fl32Left_SumWeight/(u16countMax-4);
		Right_StaticWeight = fl32Right_SumWeight/(u16countMax-4);
		Left_StaticWeight = Fun_DealWeight(Left_StaticWeight);
		Right_StaticWeight = Fun_DealWeight(Right_StaticWeight);
		
		Left_DynamicWeight1[0] = Left_StaticWeight;
		Right_DynamicWeight1[0] = Right_StaticWeight;
		
    #ifdef _DEBUG //µ÷ÊÔÓÃº¯Êı¶Î
		printf("\r\nL=%u,R=%u\r\n",Left_StaticWeight,Right_StaticWeight);		
    #endif
		 if(USER_SystemFlag.A.Flag_Online == 1)  //ÁªÍø ¸øÖ÷¿Ø·¢
		 {
					W5500_Send_Data(2,con_TranT0); //Õı³£¼ì²â½áÊø
					if(USER_SystemFlag.A.Flag_AutoD == 1)
					{
							USER_SystemFlag.A.Flag_AutoD = 0;
							FunW5500_SendDW(2);				//·¢ËÍµ±Ç°ÂÖÖØ
					}
					if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
					{								
								if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍÊıÂë¹ÜÁªÍøÁªÍø ÉÏ´«ÄÚÈİ
								{
										if(USER_SystemFlag.A.Flag_Sucess == 1) //ÎÕÊÖ¼ÓÃÜ
										{
												USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
										}
										USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
										USER_LEDFlag.A.LED_15 = 1;  				      //ÊıÂë¹ÜÊı¾İÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
										USER_LEDFlag.A.LED_14 = 1;  				      //ÊıÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
										USER_LEDFlag.A.LED_Weight = 1;  			//ÖØÁ¿Ö¸Ê¾µÆ
										USER_LEDFlag.A.LED_Brake = 0;  			  //ÖÆ¶¯Ö¸Ê¾µÆ
										FunW5500_SendLED(0,Left_StaticWeight,Right_StaticWeight);	
								}
				  }
		 }		
		 else
		 {
					if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
					{
								W5500_Send_Data(0,con_TranT0); //Õı³£¼ì²â½áÊø
				  }
		 }		 
		System_State = ReadData_State; //ÉÏ±ßµ¹¼ÆÊ±Ê±¼ä½áÊøÒ²ÓĞÕâÌõ×´Ì¬ÇĞ»»
}

/*******************************************************************************
* º¯ÊıÃû  : Fun_DynamicWeightTest
* ÃèÊö    : ¶¯Ì¬ÂÖÖØ²âÊÔº¯Êı
* ÊäÈë    : ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void Fun_DynamicWeightTest(void)   
{
			uint8_t Loop_Flag=1;
			uint8_t i,j,m,i_Flg;
			uint16_t u16Count100=0; //100¸öµã´æ´¢¼ÆÊı
			uint16_t u16countMax=0; //¼ÆÊı×î´ó´æ´¢µãÊıÁ¿
			uint16_t Voltage_Relative,Left_MaxWeight,Right_MaxWeight;  //Ïà¶ÔµçÑ¹  ×óÓÒÂÖÖØ×î´óÖµ
			uint8_t Max_Number=0,			             //ÅĞ¶ÏÊÇ·ñ²âÊÔÒ»Öá½áÊø
								  DynamicWeight_Count=0; //³ÆÖØ¼ì²âÖáÊı¼ÆÊı
			float fl32Left_SumWeight=0,
						 fl32Right_SumWeight=0;//ÖØÁ¿ÇóºÍÁÙÊ±±äÁ¿

		Left_MaxWeight = 0;
		Right_MaxWeight = 0;//³õÊ¼»¯
		for(i=0;i<6;i++) //»º´æ³õÊ¼»¯
		{
				Left_DynamicWeight1[i] = 0;
				Right_DynamicWeight1[i] = 0;
		}
		i_Flg = 0;
		con_TranMA[4] = 0x31;
		if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
		{
					W5500_Send_Data(0,con_TranMA);   //¼ì²â¿ªÊ¼ÉÏ´«¼ì²âµÚ1Öá
		}
		if(USER_SystemFlag.A.Flag_Online == 1)  //ÁªÍø ¸øÖ÷¿Ø·¢
		{
					W5500_Send_Data(2,con_TranMA);   //¼ì²â¿ªÊ¼ÉÏ´«¼ì²âµÚ1Öá
		}			
		if(USER_SystemFlag.A.Flag_TestC1 == 1)
		{
				USER_SystemFlag.A.Flag_TestC = 1;
		}
		while(System_State == DynamicWeight_State)
		{
				while(Loop_Flag == 1)
				{
						W5500_Receive_Deal(); //ÍøÂçÍ¨ĞÅ²éÑ¯´¦Àí
						if(USER_SystemFlag.A.Flag_10ms == 1)
						{
								USER_SystemFlag.A.Flag_10ms = 0;																	
								/* ×óÂÖÖØ¼ÆËã */
								fl32Left_SumWeight = ADShow_Samp(Left_Weight_ID);
								fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
								if(fl32Left_SumWeight > Zero_LeftWeight)
								{
										Voltage_Relative = fl32Left_SumWeight - Zero_LeftWeight;
										Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
								}
								else 
								{
										Left_DynamicWeight = 0;
								}
								u16Left1_TemporaryWeight[u16Count100] = Left_DynamicWeight;
								/* ÓÒÂÖÖØ¼ÆËã */
								fl32Right_SumWeight = ADShow_Samp(Right_Wight_ID);
								fl32Right_SumWeight = fl32Right_SumWeight*VOLTAGE;//ÇóµçÑ¹Öµ		
								if(fl32Right_SumWeight > Zero_RightWight)
								{
										Voltage_Relative = fl32Right_SumWeight - Zero_RightWight;
										Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
								}
								else 
								{
										Right_DynamicWeight = 0;
								}
								u16Right1_TemporaryWeight[u16Count100] = Right_DynamicWeight;
								//ÅĞ¶ÏÊÇ·ñ²É¼¯½áÊø
								if(Left_MaxWeight < Left_DynamicWeight)
								{
										Left_MaxWeight = Left_DynamicWeight;
										TIM2_count = 0;
										Max_Number = 3;
								}
								if(Right_MaxWeight < Right_DynamicWeight)
								{
										Right_MaxWeight = Right_DynamicWeight;
										TIM2_count = 0;
										Max_Number = 3;
								}
								if((Left_MaxWeight + Right_MaxWeight) > 50) //Èç¹û×î´óÖµ´óÓÚ50kg
								{
										if(Max_Number)
										{
												if((Left_DynamicWeight < (Left_MaxWeight/5)) || (Right_DynamicWeight < (Right_MaxWeight/5)))
												{
															Left_MaxWeight = 0; 
															Right_MaxWeight = 0;
												}
										}
										else
										{  
												if((Left_DynamicWeight+Right_DynamicWeight) < 20) 
												{																
														Loop_Flag = 0;
												}
												if(i_Flg == 1)
												{
														if( TIM2_count > 4000) //4Ãë
														{
																	Loop_Flag = 0;
														}
												}
												else 
												{ 
														TIM2_count = 0; 
														i_Flg = 1; 
												}			   
										}
								}
								Max_Number--;
								if( Max_Number > 5 ) 
								{ 
										Max_Number = 0; 
								}																				
								u16Count100 ++;
								if(u16Count100 >= 100) u16Count100 = 0;
								if(u16countMax < 100) u16countMax++;    //±£´æµÄÊı¾İ¼ÆÊı  ×î¶à¼ÆÊı100¸ö
						}
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
								/* ×óÂÖÖØ¼ÆËã */
								fl32Left_SumWeight = ADShow_Display(Left_Weight_ID);
								fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
								if(fl32Left_SumWeight > Zero_LeftWeight)
								{
										Voltage_Relative = fl32Left_SumWeight - Zero_LeftWeight;
										Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
										Left_DynamicWeight = Fun_DealWeight(Left_DynamicWeight);
								}
								else 
								{
										Left_DynamicWeight = 0;
								}								
								/* ÓÒÂÖÖØ¼ÆËã */
								fl32Right_SumWeight = ADShow_Display(Right_Wight_ID);
								fl32Right_SumWeight = fl32Right_SumWeight*VOLTAGE;//ÇóµçÑ¹Öµ		
								if(fl32Right_SumWeight > Zero_RightWight)
								{
										Voltage_Relative = fl32Right_SumWeight - Zero_RightWight;
										Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
										Right_DynamicWeight = Fun_DealWeight(Right_DynamicWeight);
								}
								else 
								{
										Right_DynamicWeight = 0;
								}
								if(USER_SystemFlag.A.Flag_Online == 1)         //ÁªÍø ¸øÖ÷¿Ø·¢
								{
											if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
											{
														USER_LEDFlag.A.LED_15 = 1;  				//ÊıÂë¹ÜÊı¾İÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
														USER_LEDFlag.A.LED_14 = 0;  				//ÊıÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
														FunW5500_SendLED(0,Left_DynamicWeight,Right_DynamicWeight);//ÉÏ´«³ÆÖØÖØÁ¿
											}
								}
						}
				}
				DataRank(u16Left1_TemporaryWeight,u16countMax);    //×óÂÖÖØÊı¾İÅÅĞò
				DataRank(u16Right1_TemporaryWeight,u16countMax);  //ÓÒÂÖÖØÊı¾İÅÅĞò
				
				i = u16countMax*6/10;
				j = u16countMax*9/10;
				if(i == j)
				{
						Left_MaxWeight = 0;
						Right_MaxWeight = 0;
				}
				else
				{
						fl32Left_SumWeight = 0;
						fl32Right_SumWeight = 0;
						for(m = i;m < j;m++)
						{
								fl32Left_SumWeight += u16Left1_TemporaryWeight[m];
								fl32Right_SumWeight += u16Right1_TemporaryWeight[m];
						}
						m = j - i;
						Left_MaxWeight = fl32Left_SumWeight / m;
						Right_MaxWeight = fl32Right_SumWeight / m;
						Left_MaxWeight = Fun_DealWeight(Left_MaxWeight);
						Right_MaxWeight = Fun_DealWeight(Right_MaxWeight);
						
						fl32Left_SumWeight = 0;
						fl32Right_SumWeight = 0;
				}
				Left_DynamicWeight1[DynamicWeight_Count] = Left_MaxWeight;
				Right_DynamicWeight1[DynamicWeight_Count] = Right_MaxWeight;				
				if(USER_SystemFlag.A.Flag_Online == 1)         //ÁªÍø ¸øÖ÷¿Ø·¢
				{
							if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
							{
										USER_LEDFlag.A.LED_15 = 1;  				//ÊıÂë¹ÜÊı¾İÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
										USER_LEDFlag.A.LED_14 = 0;  				//ÊıÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
										FunW5500_SendLED(0,Left_DynamicWeight1[DynamicWeight_Count],Right_DynamicWeight1[DynamicWeight_Count]);//ÉÏ´«ÂÖÖØ
							}
				}
				DynamicWeight_Count ++;
				if(DynamicWeight_Count < DynamicWeight_Number)
				{
						 con_TranMA[4] = DynamicWeight_Count + 0x31;
// 						 if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
// 						 {
// 									W5500_Send_Data(0,con_TranMA);   //¼ì²â¿ªÊ¼ÉÏ´«¼ì²âµÚ1Öá
// 						 }
						 if(USER_SystemFlag.A.Flag_Online == 1)  //ÁªÍø ¸øÖ÷¿Ø·¢
						 {
									W5500_Send_Data(2,con_TranMA);   //¼ì²â¿ªÊ¼ÉÏ´«¼ì²âµÚ1Öá
						 }			
						Loop_Flag = 1;
						Left_MaxWeight = 0;
						Right_MaxWeight = 0;
						Max_Number = 3;
						i_Flg = 0;
						u16countMax = 0;
				}
				else
				{
						USER_SystemFlag.A.Flag_TestC = 0;
						USER_SystemFlag.A.Flag_TestC1 = 0;
						Loop_Flag = 0;
						#ifdef _DEBUG //µ÷ÊÔÓÃº¯Êı¶Î
						for(i=0;i<DynamicWeight_Number;i++)
						{
								printf("\r\n%u L=%u,R=%u\r\n",i,Left_DynamicWeight1[i],Right_DynamicWeight1[i]);
						}
						#endif													
						if(USER_SystemFlag.A.Flag_Online == 1)  //ÁªÍø ¸øÖ÷¿Ø·¢
						{									
									W5500_Send_Data(2,con_TranT0);   //Õı³£¼ì²â½áÊø
									if(USER_SystemFlag.A.Flag_AutoD == 1)
									{
											USER_SystemFlag.A.Flag_AutoD = 0;
											FunW5500_SendDW(2);				//·¢ËÍµ±Ç°ÂÖÖØ
									}
									if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
									{								
												if(USER_SystemFlag.A.Flag_Sucess == 1) //ÎÕÊÖ¼ÓÃÜ
												{
														USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
												}
												USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
												USER_LEDFlag.A.LED_15 = 1;  				      //ÊıÂë¹ÜÊı¾İÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
												USER_LEDFlag.A.LED_14 = 1;  				      //ÊıÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
												USER_LEDFlag.A.LED_Weight = 1;  			//ÖØÁ¿Ö¸Ê¾µÆ
												USER_LEDFlag.A.LED_Brake = 0;  			  //ÖÆ¶¯Ö¸Ê¾µÆ
												FunW5500_SendLED(0,Left_DynamicWeight1[DynamicWeight_Count-1],Right_DynamicWeight1[DynamicWeight_Count-1]);	
									}	
						}		
						else
						{	
									if(USER_SystemFlag.A.Flag_Standalone == 1) //µ¥»úÁªÍø ¸øÊıÂë¹Ü·¢
									{
												W5500_Send_Data(0,con_TranT0);   //Õı³£¼ì²â½áÊø									
									}
						}
					  System_State = ReadData_State; //ÉÏ±ßµ¹¼ÆÊ±Ê±¼ä½áÊøÒ²ÓĞÕâÌõ×´Ì¬ÇĞ»»
				}
		}		
}

/*******************************************************************************
* º¯ÊıÃû  : Fun_MotorWeightTest
* ÃèÊö    : Ä¦ÍĞ³ÆÖØ²âÊÔº¯Êı
* ÊäÈë    : ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void Fun_MotorWeightTest(void)
{
		uint8_t Countdown_time;//µ¹¼ÆÊ±³õÊ¼Öµ
		uint16_t  u16Left0_TemporaryWeight[10] = {0},
								   u16Right0_TemporaryWeight[10] = {0};//×óÓÒÂÖÖØ1msÄ£ÄâÁ¿×ª»»½á¹û´æ´¢ ÁÙÊ±±äÁ¿

		uint8_t u8Count10=0;       //10¸öµã¼ÆÊı
		uint8_t u8Count101=0;     //10¸öµã¼ÆÊı   ÔÚ²»µ½Î»ÖĞÊ¹ÓÃ ÓÃÀ´ÏÔÊ¾ÊµÊ±ÖµÊ¹ÓÃ
		uint16_t u16Count300=0; //300¸öµã´æ´¢¼ÆÊı
		uint16_t u16countMax=0; //¼ÆÊı×î´ó´æ´¢µãÊıÁ¿
		uint16_t Voltage_Relative;//Ïà¶ÔµçÑ¹
		float u32Left_SumWeight=0.0,
					 u32Right_SumWeight=0.0;//ÖØÁ¿ÇóºÍÁÙÊ±±äÁ¿

		u16countMax = 0;//³õÊ¼»¯
		W5500_Send_Data(0,con_TranML);//ÉÏ´«MW ¾²Ì¬ÂÖÖØ¼ì²â
		Left_DynamicWeight = 0;
		Right_DynamicWeight = 0;  //¶¯Ì¬ÏÔÊ¾ÖµÇåÁã
		Motor_StaticWeight = 0; //³õÊ¼»¯Ä¦ÖØ½á¹û
		Countdown_time = SysParameter.Par0_WeightTestTime + 1; //µ¹¼ÆÊ±Ê±¼ä
		USER_SystemFlag.A.Flag_1s = 1;//Ê×´Îµ÷ÓÃÉÏ´«µ¹¼ÆÊ±µÚÒ»¸öÊıÊ¹ÓÃ
		while(System_State == StaticWeight_Motor_State)
		{
				W5500_Receive_Deal(); //ÍøÂçÍ¨ĞÅ²éÑ¯´¦Àí
				if(IO_GetInStatus(IN_Weight_ID) == 1)
				{
						u8Count101 = 0;//²»µ½Î»ÖĞ²É¼¯Ä£ÄâÁ¿Ê¹ÓÃ
						if(USER_SystemFlag.A.Flag_1s == 1)
						{
								TIM2_1ms = 0;//¶¨Ê±¼ÆÊıÇåÁã
								USER_SystemFlag.A.Flag_1s = 0;
								if(Countdown_time > 0) 
								{
										Countdown_time --;
										if(Countdown_time>0)
										{
												Tx_Buffer[0] = 3;  //Ö¡³¤¶È µÍÎ»
												Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
												Tx_Buffer[2] = 'M'; 
												Tx_Buffer[3] = '1'; 
												Tx_Buffer[4] = Countdown_time;
												W5500_Send_Data(0,Tx_Buffer);
										}
										else
										{
												System_State = Default_State;
										}
								}
						}
						if(USER_SystemFlag.A.Flag_1ms == 1)
						{
								USER_SystemFlag.A.Flag_1ms = 0;
							
								u16Left0_TemporaryWeight[u8Count10] = ADC_Convert(Left_Weight_ID);   //×óÖØ×ª»»½á¹û
								u32Left_SumWeight += u16Left0_TemporaryWeight[u8Count10];
							
								u16Right0_TemporaryWeight[u8Count10] = ADC_Convert(Right_Wight_ID); //ÓÒÖØ×ª»»½á¹û
								u32Right_SumWeight += u16Right0_TemporaryWeight[u8Count10];
							
								u8Count10 ++;
								if(u8Count10 >= 10)
								{
										u16countMax ++;
										/* ×óÂÖÖØ¼ÆËã */
										u32Left_SumWeight /= 10;
// 										u32Left_SumWeight += 0.5;
										u32Left_SumWeight = u32Left_SumWeight*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
// 										u32Left_SumWeight += 0.5;
										if(u32Left_SumWeight > Zero_LeftWeight)
										{
												Voltage_Relative = u32Left_SumWeight - Zero_LeftWeight;
												Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
										}
										else 
										{
												Left_DynamicWeight = 0;
										}
										u16Left1_TemporaryWeight[u16Count300] = Left_DynamicWeight;
										/* ÓÒÂÖÖØ¼ÆËã */
										u32Right_SumWeight /= 10;//È¡Æ½¾ùÖµ
// 										u32Right_SumWeight += 0.5;
										u32Right_SumWeight = u32Right_SumWeight*VOLTAGE;//ÇóµçÑ¹Öµ		
// 										u32Right_SumWeight += 0.5;
										if(u32Right_SumWeight > Zero_RightWight)
										{
												Voltage_Relative = u32Right_SumWeight - Zero_RightWight;
												Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
										}
										else 
										{
												Right_DynamicWeight = 0;
										}
										u16Right1_TemporaryWeight[u16Count300] = Right_DynamicWeight;
										
										
										u16Count300 ++;
										if(u16Count300 >= 300) u16Count300 = 0;
										u8Count10 = 0;
										u32Left_SumWeight = 0;
										u32Right_SumWeight = 0; //ÇóºÍ±äÁ¿ÇåÁã ÁÙÊ±±äÁ¿															
								}
						}
				}
				else
				{
						//ÊµÊ±ÏÔÊ¾Ê¹ÓÃ
						if(USER_SystemFlag.A.Flag_1ms == 1)
						{
								USER_SystemFlag.A.Flag_1ms = 0;
							
								u16Left0_TemporaryWeight[u8Count101] = ADC_Convert(Left_Weight_ID);   //×óÖØ×ª»»½á¹û
								u32Left_SumWeight += u16Left0_TemporaryWeight[u8Count101];
							
								u16Right0_TemporaryWeight[u8Count101] = ADC_Convert(Right_Wight_ID); //ÓÒÖØ×ª»»½á¹û
								u32Right_SumWeight += u16Right0_TemporaryWeight[u8Count101];
							
								u8Count101 ++;
								if(u8Count101 >= 10)
								{
										/* ×óÂÖÖØ¼ÆËã */
										u32Left_SumWeight /= 10;
// 										u32Left_SumWeight += 0.5;
										u32Left_SumWeight = u32Left_SumWeight*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
// 										u32Left_SumWeight += 0.5;
										if(u32Left_SumWeight > Zero_LeftWeight)
										{
												Voltage_Relative = u32Left_SumWeight - Zero_LeftWeight;
												Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
										}
										else 
										{
												Left_DynamicWeight = 0;
										}
										/* ÓÒÂÖÖØ¼ÆËã */
										u32Right_SumWeight /= 10;//È¡Æ½¾ùÖµ
// 										u32Right_SumWeight += 0.5;
										u32Right_SumWeight = u32Right_SumWeight*VOLTAGE;//ÇóµçÑ¹Öµ	
// 										u32Right_SumWeight += 0.5;			
										if(u32Right_SumWeight > Zero_RightWight)
										{
												Voltage_Relative = u32Right_SumWeight - Zero_RightWight;
												Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
										}
										else 
										{
												Right_DynamicWeight = 0;
										}
										u8Count101 = 0;
										u32Left_SumWeight = 0;
										u32Right_SumWeight = 0; //ÇóºÍ±äÁ¿ÇåÁã ÁÙÊ±±äÁ¿															
								}
						}
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
								W5500_Send_Data(0,con_TranT4); //»Ø´«³µÁ¾²»µ½Î»ĞÅºÅ								
						}						
						Countdown_time = SysParameter.Par0_WeightTestTime + 1; //µ¹¼ÆÊ±Ê±¼ä
						TIM2_1ms = 0;//¶¨Ê±¼ÆÊıÇåÁã
						USER_SystemFlag.A.Flag_1s = 1;//Ê×´Îµ÷ÓÃÉÏ´«µ¹¼ÆÊ±µÚÒ»¸öÊıÊ¹ÓÃ
						u8Count10 = 0;
						u16Count300 = 0;
						u16countMax = 0;//³õÊ¼»¯
				}
		}				
//		printf("\r\n%u\r\n",u16countMax);
		if(u16countMax >= 300) u16countMax = 300;
		u32Left_SumWeight = 0;
		u32Right_SumWeight = 0;
		for(u16Count300=4;u16Count300<u16countMax;u16Count300++) //
		{
				u32Left_SumWeight += u16Left1_TemporaryWeight[u16Count300];
				u32Right_SumWeight += u16Right1_TemporaryWeight[u16Count300];
		}
		Motor_StaticWeight = (float)u32Left_SumWeight/(u16countMax-4);
//		printf("\r\nMotor_StaticWeight=%u\r\n",Motor_StaticWeight);
		W5500_Send_Data(0,con_TranT0); //Õı³£¼ì²â½áÊø
		System_State = Default_State; //ÉÏ±ßµ¹¼ÆÊ±Ê±¼ä½áÊøÒ²ÓĞÕâÌõ×´Ì¬ÇĞ»»
}

/*******************************************************************************
* º¯ÊıÃû  : Fun_VoltageConvertValue
* ÃèÊö    : Ïà¶ÔµçÑ¹×ª»»³ÉÊµÊ±²âÁ¿Öµ
* ÊäÈë    : AD_ID:Ä£ÄâÁ¿Í¨µÀ      Voltage_Relative:Ïà¶ÔµçÑ¹
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÊµÊ±²âÁ¿Öµ
* ËµÃ÷    : 
*******************************************************************************/
uint16_t Fun_VoltageConvertValue(uint8_t AD_ID,uint16_t Voltage_Relative)
{
		uint16_t Value_Real,Value_Limit;
		_unionType1 Parameter;  //ÁÙÊ±±ê¶¨Í¨µÀÏµÊı
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
		switch(Parameter.Load_Parameter.Parameter_NB)  //ÄâºÏ´ÎÊı
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
		return (Value_Real);
}

/*******************************************************************************
* º¯ÊıÃû  : IO_GetInStatus
* ÃèÊö    : »ñÈ¡ÊäÈë¿Ú×´Ì¬
* ÊäÈë    : IO_ID:¿ªÈëÍ¨µÀ  
													 IN_Weight_ID     		//ÂÖÖØµ½Î»ĞÅºÅ¿ªÈëID
													 IN_Brake_ID						//ÖÆ¶¯µ½Î»ĞÅºÅ¿ªÈëID
													 IN_Raise_ID			  		//¾ÙÉıµ½Î»ĞÅºÅ¿ªÈëID
													 IN_Protect_ID     		//¸Ç°å±£»¤ĞÅºÅ¿ªÈëID
													 IN_LeftStop_ID      	//×óÍ£»úĞÅºÅ¿ªÈëID
													 IN_RightStop_ID    	//ÓÒÍ£»úĞÅºÅ¿ªÈëID
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÊµÊ±²âÁ¿Öµ
* ËµÃ÷    : 
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
* º¯ÊıÃû  : DataRank
* ÃèÊö    : Êı¾İÅÅĞò ÉıĞòÅÅÁĞ
* ÊäÈë    :   *Data Êı¾İ±äÁ¿   number ÅÅĞò×ÜÊıÁ¿
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  : ÊµÊ±²âÁ¿Öµ
* ËµÃ÷    : 
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
* º¯ÊıÃû  : Get_Parameter
* ÃèÊö    : ´Ó´æ´¢Ğ¾Æ¬»ñÈ¡²ÎÊı
* ÊäÈë    :  ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void Get_Parameter(void)
{
		uint8_t u8flash_buffer[21];//±ê¶¨ÏµÊı  ²ÎÊıÁÙÊ±´æ´¢±äÁ¿		
		/* ±ê¶¨ÏµÊı¶Á³öÀ´ */
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address,17);
		memcpy(Left_Weight_par.Load_Flash,u8flash_buffer,17);	 //°ÑÏµÊı±£´æµ½±äÁ¿
	
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address,17);
		memcpy(Right_Wight_par.Load_Flash,u8flash_buffer,17);	 //°ÑÏµÊı±£´æµ½±äÁ¿
	
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Brake_Flash_Address,17);
		memcpy(Left_Brake_par.Load_Flash,u8flash_buffer,17);	 //°ÑÏµÊı±£´æµ½±äÁ¿
	
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Brake_Flash_Address,17);
		memcpy(Right_Brake_par.Load_Flash,u8flash_buffer,17);	 //°ÑÏµÊı±£´æµ½±äÁ¿
	
		SPI_FLASH_BufferRead(u8flash_buffer,All_Weight_Flash_Address,17);
		memcpy(All_Weight_par.Load_Flash,u8flash_buffer,17);	 //°ÑÏµÊı±£´æµ½±äÁ¿
	
		SPI_FLASH_BufferRead(u8flash_buffer,FootPlat_Flash_Address,17);
		memcpy(FootPlat_par.Load_Flash,u8flash_buffer,17);	 //°ÑÏµÊı±£´æµ½±äÁ¿
	
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Move_Flash_Address,17);
		memcpy(Left_Move_par.Load_Flash,u8flash_buffer,17);	 //°ÑÏµÊı±£´æµ½±äÁ¿
		
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Move_Flash_Address,17);
		memcpy(Right_Move_par.Load_Flash,u8flash_buffer,17);	 //°ÑÏµÊı±£´æµ½±äÁ¿
	
	 //²ÎÊı¶ÁÈ¡
	 Fun_GetParameter();
	 //³ö³§²ÎÊı¶ÁÈ¡
	 SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,25);
	 if(Parameter_Buffer[24] != 0x55)//³ö³§±àºÅ
	 {
			 Parameter_Buffer[0] = 0;
			 Parameter_Buffer[1] = 0;
			 Parameter_Buffer[2] = 0;
			 Parameter_Buffer[3] = 0;		  
		 
			 Parameter_Buffer[4] = '0';
			 Parameter_Buffer[5] = '0';
			 Parameter_Buffer[6] = '0';
			 Parameter_Buffer[7] = '0';      
		 
			 Parameter_Buffer[8] = '0';
			 Parameter_Buffer[9] = '0';
			 Parameter_Buffer[10] = '0';
			 Parameter_Buffer[11] = '0';     
 		 
			 Parameter_Buffer[12] = '0';
			 Parameter_Buffer[13] = '0';   
		 
			 Parameter_Buffer[14] = '0';
			 Parameter_Buffer[15] = '0';
			 Parameter_Buffer[16] = '0';
			 Parameter_Buffer[17] = '0';   
			 
			 Parameter_Buffer[18] = '0';			
			 Parameter_Buffer[19] = '0';    
			 			 
			 Parameter_Buffer[20] = '0';
			 Parameter_Buffer[21] = '0';
			 Parameter_Buffer[22] = '0';
			 Parameter_Buffer[23] = '0';   
			 
			 Parameter_Buffer[24] = 0x55;
			 SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//²Á³öÉÈÇø
			 SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,25);//±£´æÏµÊı		
	 }	
	 
	 //IP²ÎÊı¶ÁÈ¡
	 SPI_FLASH_BufferRead(W5500_Parameter,ParameterIP_Flash_Address,27);
	 if(W5500_Parameter[26] != 0x55)//Ã»±£´æIP Ôò³õÊ¼»¯IP
	 {
			 W5500_Parameter[0] = 192;
			 W5500_Parameter[1] = 168;
			 W5500_Parameter[2] = 100;
			 W5500_Parameter[3] = 1;		  //Íø¹Ø
		 
			 W5500_Parameter[4] = 255;
			 W5500_Parameter[5] = 255;
			 W5500_Parameter[6] = 255;
			 W5500_Parameter[7] = 0;      //×ÓÍøÑÚÂë
		 
			 W5500_Parameter[8] = 192;
			 W5500_Parameter[9] = 168;
			 W5500_Parameter[10] = 100;
			 W5500_Parameter[11] = 3;     //Ö÷¿Ø°åIP
 		 
			 W5500_Parameter[12] = 0xC8;
			 W5500_Parameter[13] = 0x61;   //¶Ë¿ÚºÅ25032
		 
			 W5500_Parameter[14] = 0;
			 W5500_Parameter[15] = 0;
			 W5500_Parameter[16] = 0;
			 W5500_Parameter[17] = 0;   // ÊıÂë¹ÜIP
			 
			 W5500_Parameter[18] = 0;			
			 W5500_Parameter[19] = 0;    //ÊıÂë¹Ü¶Ë¿ÚºÅ
			 			 
			 W5500_Parameter[20] = 192;
			 W5500_Parameter[21] = 168;
			 W5500_Parameter[22] = 1;
			 W5500_Parameter[23] = 200;   //UDP¹ã²¥Ä¿µÄµØÖ·
			 
			 W5500_Parameter[24] = 0xC7;
			 W5500_Parameter[25] = 0x61;  //UDP¶Ë¿ÚºÅ
			 
			 W5500_Parameter[26] = 0x55;
			 SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//²Á³öÉÈÇø
			 SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//±£´æÏµÊı		
	 }		
}

/*******************************************************************************
* º¯ÊıÃû  : Fun_SignalTest
* ÃèÊö    : ĞÅºÅ²âÊÔ
* ÊäÈë    :  ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
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
						fl32ADSum_Channen8 = 0.0;  //Ïà¼ÓµÄºÍ
		uint16_t Voltage_Relative[8];//Ïà¶ÔµçÑ¹
		uint16_t DynamicData[8];     //¶¯Ì¬²âÁ¿Öµ
		uint8_t u8Count_100ms;  //ÉÏ´«ÆµÂÊ¼ÆÊı
		while(System_State == SignalTest_State)
		{
				W5500_Receive_Deal(); //ÍøÂçÍ¨ĞÅ²éÑ¯´¦Àí
				if(USER_SystemFlag.A.Flag_500ms == 1)
				{
						USER_SystemFlag.A.Flag_500ms = 0;						
// 						if(SysParameter.Par12_MachineType == 0)//ÂÖÖØÌ¨
						if(1)//Ä¬ÈÏÂÖÖØÌ¨
						{
								/* 1Í¨µÀ¼ÆËã */
								fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);

								fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
								if(fl32ADSum_Channen1 > Zero_LeftWeight)
								{
										Voltage_Relative[0] = fl32ADSum_Channen1 - Zero_LeftWeight;
										DynamicData[0] = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative[0]);
								}
								else 
								{
										DynamicData[0] = 0;
								}
								/* 2Í¨µÀ¼ÆËã */
								fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
								fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
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
								Tx_Buffer[3] = 2;    //Ä£ÄâÁ¿ÊıÁ¿
								Tx_Buffer[4] = 1;    //¿ªÈëÁ¿ÊıÁ¿
								/* 1Í¨µÀ */
								Tx_Buffer[5] = Left_Weight_ID;    
								Tx_Buffer[6] = Zero_LeftWeight%256;    
								Tx_Buffer[7] = Zero_LeftWeight/256;    
								Tx_Buffer[8] = (uint32_t)fl32ADSum_Channen1%256;    
								Tx_Buffer[9] = (uint32_t)fl32ADSum_Channen1/256;
								Tx_Buffer[10] = DynamicData[0]%256;
								Tx_Buffer[11] = DynamicData[0]/256;
								/* 2Í¨µÀ */
								Tx_Buffer[12] = Right_Wight_ID;    
								Tx_Buffer[13] = Zero_RightWight%256;    
								Tx_Buffer[14] = Zero_RightWight/256;    
								Tx_Buffer[15] = (uint32_t)fl32ADSum_Channen2%256;    
								Tx_Buffer[16] = (uint32_t)fl32ADSum_Channen2/256;
								Tx_Buffer[17] = DynamicData[1]%256;
								Tx_Buffer[18] = DynamicData[1]/256;
								/* ¿ªÈëÁ¿ */
								Tx_Buffer[19] = IN_Weight_ID;    
								Tx_Buffer[20] = IO_GetInStatus(IN_Weight_ID)+0x30; 
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
* º¯ÊıÃû  : Fun_Stable()
* ÃèÊö    : ´ı»ú×´Ì¬
* ÊäÈë    :  ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void Fun_Stable(void)
{
		float	fl32ADSum_Channen1 = 0.0,
						fl32ADSum_Channen2 = 0.0;
		uint16_t Voltage_Relative;									 //Ïà¶ÔµçÑ¹ã

		Fun_Zero(Left_Weight_ID,0);//ÅĞ¶ÏÖØÁ¿ÊÇ·ñµ÷Áã
		Fun_Zero(Right_Wight_ID,0);//ÅĞ¶ÏÖØÁ¿ÊÇ·ñµ÷Áã
		while(System_State == Default_State)
		{
				W5500_Receive_Deal(); //ÍøÂçÍ¨ĞÅ²éÑ¯´¦Àí
// 				if(SysParameter.Par12_MachineType == 0) //ÂÖÖØÌ¨
				if(1)//Ä¬ÈÏÂÖÖØÌ¨
				{							
							if(USER_SystemFlag.A.Flag_500ms == 1)
							{
										USER_SystemFlag.A.Flag_500ms = 0;
										/*×óÖØ*/
										fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);
										fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
										if(fl32ADSum_Channen1 > Zero_LeftWeight)
										{
												Voltage_Relative = fl32ADSum_Channen1 - Zero_LeftWeight;
												Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
												Left_DynamicWeight = Fun_DealWeight(Left_DynamicWeight);
										}
										else 
										{
												Left_DynamicWeight = 0;
										}
										/*ÓÒÖØ*/
										fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
										fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
										if(fl32ADSum_Channen2 > Zero_RightWight)
										{
												Voltage_Relative = fl32ADSum_Channen2 - Zero_RightWight;
												Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
												Right_DynamicWeight = Fun_DealWeight(Right_DynamicWeight);
										}
										else 
										{
												Right_DynamicWeight = 0;
										}
										if(USER_SystemFlag.A.Flag_Online == 1) //ºÍÖ÷¿ØÁªÍø ÉÏ´«ÄÚÈİ
									 {
// 												FunW5500_SendLED(2,Left_DynamicWeight,Right_DynamicWeight);	
												if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍÊıÂë¹ÜÁªÍøÁªÍø ÉÏ´«ÄÚÈİ
												{
														if(USER_SystemFlag.A.Flag_Sucess == 1) //ÎÕÊÖ¼ÓÃÜ
														{
																USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
														}
														USER_LEDFlag.A.LED_Test = 0;  					//¼ì²âÖ¸Ê¾µÆ
														USER_LEDFlag.A.LED_15 = 1;  				//ÊıÂë¹ÜÊı¾İÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
														USER_LEDFlag.A.LED_14 = 1;  				//ÊıÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
														USER_LEDFlag.A.LED_Weight = 1;  			//ÖØÁ¿Ö¸Ê¾µÆ
														USER_LEDFlag.A.LED_Brake = 0;  			  //ÖÆ¶¯Ö¸Ê¾µÆ
														FunW5500_SendLED(0,Left_DynamicWeight,Right_DynamicWeight);	
												}
									 }
									 else
									 {
												if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍÊıÂë¹ÜÁªÍøÁªÍø ÉÏ´«ÄÚÈİ
												{
														FunW5500_SendGW(0,Left_DynamicWeight,Right_DynamicWeight);	
												}
									 }
							}				
				}							
		}
}



/*******************************************************************************
* º¯ÊıÃû  : Deal_Data()
* ÃèÊö    : ÖÆ¶¯¹ı³ÌÊı¾İ´¦Àí
* ÊäÈë    :  Number:±£´æµÄ×î´óÊı¾İ,Data:Êı¾İ´æ´¢Êı×é,Command:×ÖÃüÁî
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
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
		 *Data++ = 10;                      //²ÉÑùÖÜÆÚ
		 for(i=0;i<Number;i++)
		 {
					aaa.u16_Data = u16Left_Data[i]; //×ó±ßÊı¾İ
				 *Data++ = aaa.u8_Data[0];
				 *Data++ = aaa.u8_Data[1];
			 
					aaa.u16_Data = u16Right_Data[i]; //ÓÒ±ßÊı¾İ
				 *Data++ = aaa.u8_Data[0];
				 *Data++ = aaa.u8_Data[1];
		 }
}
/*******************************************************************************
* º¯ÊıÃû  : FunW5500_SendGW()
* ÃèÊö    : ÉÏ´«ÂÖÖØ¶¯Ì¬Êı¾İ
* ÊäÈë    :  s:¶Ë¿Ú,  L_Data:×óÖØÊı¾İ,R_Data:ÓÒÖØÊı¾İ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void FunW5500_SendGW(uint8_t s,uint16_t L_Data,uint16_t R_Data)
{
		Tx_Buffer[0] = 14;  //Ö¡³¤¶È µÍÎ»
		Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
		Tx_Buffer[2] = 'G'; 
		Tx_Buffer[3] = 'W'; 
		/* ×óÖØ */
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
		/* ÓÒÖØ */
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
		W5500_Send_Data(s,Tx_Buffer);				//·¢ËÍµ±Ç°ÂÖÖØ
}
/*******************************************************************************
* º¯ÊıÃû  : FunW5500_SendLED()
* ÃèÊö    : ÁªÍøÊ±ÉÏ´«ÊıÂë¹ÜÏÔÊ¾
* ÊäÈë    :  s:¶Ë¿Ú,  L_Data:ÊıÂë¹Ü,R_Data:ÓÒÊıÂë¹Ü
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void FunW5500_SendLED(uint8_t s,uint16_t L_Data,uint16_t R_Data)
{
		_union_u16_u8_type u16Data;
		Tx_Buffer[0] = 8;    //Ö¡³¤¶È µÍÎ»
		Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
		Tx_Buffer[2] = 'N'; 
		Tx_Buffer[3] = 'H'; 
		/* ×óLED */
	  u16Data.u16_data = L_Data;
		Tx_Buffer[4] = u16Data.u8_data[0];
		Tx_Buffer[5] = u16Data.u8_data[1];
		/* ÓÒLED */
		u16Data.u16_data = R_Data;
		Tx_Buffer[6] = u16Data.u8_data[0];
		Tx_Buffer[7] = u16Data.u8_data[1];
	  /* Ö¸Ê¾µÆ */
		 u16Data.u16_data = USER_LEDFlag.Init;
		Tx_Buffer[8] = u16Data.u8_data[0];
		Tx_Buffer[9] = u16Data.u8_data[1];
		W5500_Send_Data(s,Tx_Buffer);				//·¢ËÍµ±Ç°ÂÖÖØ
}
/*******************************************************************************
* º¯ÊıÃû  : FunW5500_SendDW()
* ÃèÊö    : ÉÏ´«ÂÖÖØ½á¹ûÊı¾İ
* ÊäÈë    :  s:¶Ë¿Ú
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void FunW5500_SendDW(uint8_t s)
{
		uint8_t i;
		Tx_Buffer[0] = DynamicWeight_Number*12+3;  //Ö¡³¤¶È µÍÎ»
		Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
		Tx_Buffer[2] = 'D'; 
		Tx_Buffer[3] = 'W'; 
		Tx_Buffer[4] = DynamicWeight_Number;
		for(i=0;i<DynamicWeight_Number;i++)
		{
					/* ×óÖØ */
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
					/* ÓÒÖØ */
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
		W5500_Send_Data(s,Tx_Buffer);				//·¢ËÍµ±Ç°ÂÖÖØ
}
/*******************************************************************************
* º¯ÊıÃû  : FunW5500_TestC()
* ÃèÊö    : ÉÏ´«²âÊÔ¹ı³ÌÖĞ¼ì²âÊı¾İ
* ÊäÈë    :  s:¶Ë¿Ú
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void FunW5500_TestC(uint8_t s)
{
		_union_u16_u8_type u16Data;
	  float fl32Left_SumBrake,fl32Right_SumBrake,f32All_SumWeight;
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1,All_DynamicWeight1,Voltage_Relative;
	  if(USER_SystemFlag.A.Flag_TestC == 1)
		{
					/* ×óÖØ¼ÆËã */
					fl32Left_SumBrake = ADShow_Samp(Left_Weight_ID);
					fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //×ª»»³ÉµçÑ¹Öµ
					if(fl32Left_SumBrake > Zero_LeftWeight)
					{
							Voltage_Relative = fl32Left_SumBrake - Zero_LeftWeight;
							Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
					}
					else 
					{
							Left_DynamicBrake1 = 0;
					}
					/* ÓÒÖØ¼ÆËã */
					fl32Right_SumBrake = ADShow_Samp(Right_Wight_ID);//È¡Æ½¾ùÖµ
					fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//ÇóµçÑ¹Öµ		
					if(fl32Right_SumBrake > Zero_RightWight)
					{
							Voltage_Relative = fl32Right_SumBrake - Zero_RightWight;
							Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
					}
					else 
					{
							Right_DynamicBrake1 = 0;
					}
					Tx_Buffer[0] = 13;  //Ö¡³¤¶È µÍÎ»
					Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
					Tx_Buffer[2] = 'C'; 
					Tx_Buffer[3] = 'X'; 
					Tx_Buffer[4] = 2;
					Tx_Buffer[5] = 1;
					Tx_Buffer[6] = 0;
					//Ä£ÄâÁ¿
					u16Data.u16_data = Left_DynamicBrake1;
					Tx_Buffer[7] = Left_Weight_ID; //×óÖØ
					Tx_Buffer[8] = u16Data.u8_data[0];
					Tx_Buffer[9] = u16Data.u8_data[1];		
					u16Data.u16_data = Right_DynamicBrake1;
					Tx_Buffer[10] = Right_Wight_ID; //ÓÒÖØ
					Tx_Buffer[11] = u16Data.u8_data[0];
					Tx_Buffer[12] = u16Data.u8_data[1];
					//¿ªÈëÁ¿
					Tx_Buffer[13] = IN_Weight_ID;
					Tx_Buffer[14] = IO_GetInStatus(IN_Weight_ID)+0x30;
					W5500_Send_Data(s,Tx_Buffer);				//·¢ËÍµ±Ç°¼ì²â½á¹û
		}
}
/*******************************************************************************
* º¯ÊıÃû  : Fun_DealWeight()
* ÃèÊö    : ´¦ÀíÖØÁ¿·Ö±æÂÊº¯Êı
* ÊäÈë    :  ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
uint16_t Fun_DealWeight(uint16_t Weight)
{
		uint8_t Remainder;    //ÓàÊı
    uint16_t Quotient;      //ÉÌ
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
* º¯ÊıÃû  : Fun_DealParameter()
* ÃèÊö    : ¿ØÖÆ²ÎÊı±äÁ¿´¦Àíº¯Êı ²ÎÊı½ÓÊÕÊ±Ê¹ÓÃ
* ÊäÈë    :  ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :1:²ÎÊıÉèÖÃÕıÈ·  0:²ÎÊıÉèÖÃÊ§°Ü
* ËµÃ÷    : 
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
						case 10:  //¾²Ì¬³ÆÖØµ¹¼ÆÊ±3-9
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par0_WeightTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
													}
							            break;
						}
						case 11://µÚÈı¹öÍ²ÓĞÎŞ  0  1
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
						case 12://Ì¤°åÁ¦ÓĞÎŞ 0 1
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
						case 13://ÖÆ¶¯×¼±¸Ê±¼ä 2-9
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
													}
							            break;
						 }
						case 14://×èÖÍ²âÊÔÊ±¼ä 2-9Ãë
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
													}
							            break;
						}
						case 15://ÖÆ¶¯²âÊÔÊ±¼ä 2-9Ãë
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par5_BrakeTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
													}
							            break;
						 }
						case 16://ÂÖÖØÌ¨·Ö¶ÈÖµ  1 2 5
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
						case 17://×ó¶¯Ì¬Æ½¾ùÁãÖµ daN 0-30
						{
													if(parameter.u16_data > 30)
													{
															return 0;
													}
													else
													{
															SysParameter.Par7_LeftAverZero = parameter.u16_data; 
													}
							            break;
						 }
						case 18://ÓÒ¶¯Ì¬Æ½¾ùÁãÖµ daN 0-30
						{
													if(parameter.u16_data > 30)
													{
															return 0;
													}
													else
													{
															SysParameter.Par8_RightAverZero = parameter.u16_data; 
													}
							            break;
						 }
						case 19://×ó¶¯Ì¬×î´óÁãÖµ daN 0-30
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
						case 20://ÓÒ¶¯Ì¬×î´óÁãÖµ daN 0-30
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
						case 21://¼ÓÔØ¾ÙÉıÖØÁ¿·§Öµ 
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
						case 22://Ì¨¼ÜÀàĞÍ 0-3
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
						case 23://¼ÓÔØ¾ÙÉı¸ß¶È 0-900
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
						case 24://Ì¨¼Ü×î³¤¾ÙÉıÊ±¼ä20-90 ×î¸ß¿ÉÉèÖÃ90s
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
// 													break;
				}
		}
		return 1;
}
/*******************************************************************************
* º¯ÊıÃû  : Fun_GetParameter()
* ÃèÊö    : »ñÈ¡²ÎÊı´¦Àí  
* ÊäÈë    :  ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :1:²ÎÊıÉèÖÃÕıÈ·  0:²ÎÊıÉèÖÃÊ§°Ü
* ËµÃ÷    : 
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
			  //²ÎÊı1
				Parameter_Buffer[1] = 10;
				Parameter_Buffer[2] = 0;
				Parameter_Buffer[3] = 0;
				//²ÎÊı2
				Parameter_Buffer[4] = 11;
				Parameter_Buffer[5] = 0;
				Parameter_Buffer[6] = 0;
				//²ÎÊı3
				Parameter_Buffer[7] = 12;
				Parameter_Buffer[8] = 0;
				Parameter_Buffer[9] = 0;
				//²ÎÊı4
				Parameter_Buffer[10] = 13;
				Parameter_Buffer[11] = 0;
				Parameter_Buffer[12] = 0;
				//²ÎÊı5
				Parameter_Buffer[13] = 14;
				Parameter_Buffer[14] = 0;
				Parameter_Buffer[15] = 0;
				//²ÎÊı6
				Parameter_Buffer[16] = 15;
				Parameter_Buffer[17] = 0;
				Parameter_Buffer[18] = 0;
				//²ÎÊı7
				Parameter_Buffer[19] = 16;
				Parameter_Buffer[20] = 0;
				Parameter_Buffer[21] = 0;
				//²ÎÊı8
				Parameter_Buffer[22] = 17;
				Parameter_Buffer[23] = 0;
				Parameter_Buffer[24] = 0;
				//²ÎÊı9
				Parameter_Buffer[25] = 18;
				Parameter_Buffer[26] = 0;
				Parameter_Buffer[27] = 0;
				//²ÎÊı10
				Parameter_Buffer[28] = 19;
				Parameter_Buffer[29] = 0;
				Parameter_Buffer[30] = 0;
				//²ÎÊı11
				Parameter_Buffer[31] = 20;
				Parameter_Buffer[32] = 0;
				Parameter_Buffer[33] = 0;
				//²ÎÊı12
				Parameter_Buffer[34] = 21;
				Parameter_Buffer[35] = 0;
				Parameter_Buffer[36] = 0;
				//²ÎÊı13
				Parameter_Buffer[37] = 22;
				Parameter_Buffer[38] = 0;
				Parameter_Buffer[39] = 0;
				//²ÎÊı14
				Parameter_Buffer[40] = 23;
				Parameter_Buffer[41] = 0;
				Parameter_Buffer[42] = 0;
				//²ÎÊı15
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
								case 10:  //¾²Ì¬³ÆÖØµ¹¼ÆÊ±3-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par0_WeightTestTime = 3; //Ä¬ÈÏ3Ãë
																	Parameter_Buffer[2] = 3;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par0_WeightTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
															}
															break;
								}
								case 11://µÚÈı¹öÍ²ÓĞÎŞ  0  1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par1_RollerFlag = 1; //Ä¬ÈÏÓĞ
																	Parameter_Buffer[5] = 1;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par1_RollerFlag = parameter.u16_data; 
															}
															break;
								}
								case 12://Ì¤°åÁ¦ÓĞÎŞ 0 1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par2_FootPlatFlag = 1;//Ä¬ÈÏÓĞ
																	Parameter_Buffer[8] = 1;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par2_FootPlatFlag = parameter.u16_data; 
															}
															break;
								}
								case 13://ÖÆ¶¯×¼±¸Ê±¼ä 2-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par3_BrakeReadyTime = 3; //Ä¬ÈÏ3Ãë
																	Parameter_Buffer[11] = 3;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
															}
															break;
								 }
								case 14://×èÖÍ²âÊÔÊ±¼ä 2-9Ãë
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par4_ZuzhiTestTime = 5; //Ä¬ÈÏ5Ãë
																	Parameter_Buffer[14] = 5;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
															}
															break;
								}
								case 15://ÖÆ¶¯²âÊÔÊ±¼ä 2-9Ãë
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par5_BrakeTestTime = 8; //Ä¬ÈÏ8Ãë
																	Parameter_Buffer[17] = 8;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par5_BrakeTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
															}
															break;
								 }
								case 16://ÂÖÖØÌ¨·Ö¶ÈÖµ  1 2 5
								{
															if((parameter.u16_data==1) || (parameter.u16_data==2) || (parameter.u16_data==5))		
															{
																	SysParameter.Par6_Weight = parameter.u16_data; //													
															}
															else
															{
																	SysParameter.Par6_Weight = 1; //Ä¬ÈÏ1
																	Parameter_Buffer[20] = 1;
																	Edit_Flg = 1;
															}
															break;
								}
								case 17://×ó¶¯Ì¬Æ½¾ùÁãÖµ daN 0-30
								{
															if(parameter.u16_data > 30)
															{
																	SysParameter.Par7_LeftAverZero = 0; 
																	Parameter_Buffer[23] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par7_LeftAverZero = parameter.u16_data; 
															}
															break;
								 }
								case 18://ÓÒ¶¯Ì¬Æ½¾ùÁãÖµ daN 0-30
								{
															if(parameter.u16_data > 30)
															{
																	SysParameter.Par8_RightAverZero = 0; 
																	Parameter_Buffer[26] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par8_RightAverZero = parameter.u16_data; 
															}
															break;
								 }
								case 19://×ó¶¯Ì¬×î´óÁãÖµ daN 0-30
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
								case 20://ÓÒ¶¯Ì¬×î´óÁãÖµ daN 0-30
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
								case 21://¼ÓÔØ¾ÙÉıÖØÁ¿·§Öµ 
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
								case 22://Ì¨¼ÜÀàĞÍ 0-3
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
								case 23://¼ÓÔØ¾ÙÉı¸ß¶È 0-900
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
								case 24://Ì¨¼Ü×î³¤¾ÙÉıÊ±¼ä20-90 ×î¸ß¿ÉÉèÖÃ90s
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
								case 10:  //¾²Ì¬³ÆÖØµ¹¼ÆÊ±3-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par0_WeightTestTime = 3; //Ä¬ÈÏ3Ãë
																	Parameter_Buffer[i*3+2] = 3;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par0_WeightTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
															}
															break;
								}
								case 11://µÚÈı¹öÍ²ÓĞÎŞ  0  1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par1_RollerFlag = 1; //Ä¬ÈÏÓĞ
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
								case 12://Ì¤°åÁ¦ÓĞÎŞ 0 1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par2_FootPlatFlag = 1;//Ä¬ÈÏÓĞ
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
								case 13://ÖÆ¶¯×¼±¸Ê±¼ä 2-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par3_BrakeReadyTime = 3; //Ä¬ÈÏ3Ãë
																	Parameter_Buffer[i*3+2] = 3;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
															}
															break;
								 }
								case 14://×èÖÍ²âÊÔÊ±¼ä 2-9Ãë
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par4_ZuzhiTestTime = 5; //Ä¬ÈÏ5Ãë
																	Parameter_Buffer[i*3+2] = 5;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
															}
															break;
								}
								case 15://ÖÆ¶¯²âÊÔÊ±¼ä 2-9Ãë
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par5_BrakeTestTime = 8; //Ä¬ÈÏ8Ãë
																	Parameter_Buffer[i*3+2] = 8;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par5_BrakeTestTime = parameter.u16_data; //µ¹¼ÆÊ±3-9Ãë
															}
															break;
								 }
								case 16://ÂÖÖØÌ¨·Ö¶ÈÖµ  1 2 5
								{
															if((parameter.u16_data==1) || (parameter.u16_data==2) || (parameter.u16_data==5))		
															{
																	SysParameter.Par6_Weight = parameter.u16_data; //
															}
															else
															{
																	SysParameter.Par6_Weight = 1; //Ä¬ÈÏ1
																	Parameter_Buffer[i*3+2] = 1;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															break;
								}
								case 17://×ó¶¯Ì¬Æ½¾ùÁãÖµ daN 0-30
								{
															if(parameter.u16_data > 30)
															{
																	SysParameter.Par7_LeftAverZero = 0; 
																	Parameter_Buffer[i*3+2] = 0;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par7_LeftAverZero = parameter.u16_data; 
															}
															break;
								 }
								case 18://ÓÒ¶¯Ì¬Æ½¾ùÁãÖµ daN 0-30
								{
															if(parameter.u16_data > 30)
															{
																	SysParameter.Par8_RightAverZero = 0; 
																	Parameter_Buffer[i*3+2] = 0;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par8_RightAverZero = parameter.u16_data; 
															}
															break;
								 }
								case 19://×ó¶¯Ì¬×î´óÁãÖµ daN 0-30
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
								case 20://ÓÒ¶¯Ì¬×î´óÁãÖµ daN 0-30
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
								case 21://¼ÓÔØ¾ÙÉıÖØÁ¿·§Öµ 
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
								case 22://Ì¨¼ÜÀàĞÍ 0-3
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
								case 23://¼ÓÔØ¾ÙÉı¸ß¶È 0-900
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
								case 24://Ì¨¼Ü×î³¤¾ÙÉıÊ±¼ä20-90 ×î¸ß¿ÉÉèÖÃ90s
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
				SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//²Á³öÉÈÇø
				SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(u8ParameterN*3+1));//±£´æÏµÊı
		}
}
/*******************************************************************************
* º¯ÊıÃû  : W5500_ParameterReset
* ÃèÊö    : ÍøÂçIP²ÎÊıÇ¿ÖÆ¸´Î»
* ÊäÈë    :  ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void W5500_ParameterReset(void)
{
		 W5500_Parameter[0] = 192;
		 W5500_Parameter[1] = 168;
		 W5500_Parameter[2] = 100;
		 W5500_Parameter[3] = 1;		  //Íø¹Ø
	 
		 W5500_Parameter[4] = 255;
		 W5500_Parameter[5] = 255;
		 W5500_Parameter[6] = 255;
		 W5500_Parameter[7] = 0;      //×ÓÍøÑÚÂë
	 
		 W5500_Parameter[8] = 192;
		 W5500_Parameter[9] = 168;
		 W5500_Parameter[10] = 100;
		 W5500_Parameter[11] = 3;     //Ö÷¿Ø°åIP
	 
		 W5500_Parameter[12] = 0xC8;
		 W5500_Parameter[13] = 0x61;   //¶Ë¿ÚºÅ25032
	 
		 W5500_Parameter[14] = 0;
		 W5500_Parameter[15] = 0;
		 W5500_Parameter[16] = 0;
		 W5500_Parameter[17] = 0;   // ÊıÂë¹ÜIP
		 
		 W5500_Parameter[18] = 0;			
		 W5500_Parameter[19] = 0;    //ÊıÂë¹Ü¶Ë¿ÚºÅ
					 
		 W5500_Parameter[20] = 192;
		 W5500_Parameter[21] = 168;
		 W5500_Parameter[22] = 1;
		 W5500_Parameter[23] = 200;   //UDP¹ã²¥Ä¿µÄµØÖ·
		 
		 W5500_Parameter[24] = 0xC7;
		 W5500_Parameter[25] = 0x61;  //UDP¶Ë¿ÚºÅ
		 
		 W5500_Parameter[26] = 0x55;
		 SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//²Á³öÉÈÇø
		 SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//±£´æÏµÊı		
}
/*******************************************************************************
* º¯ÊıÃû  : Fun_ReadData()
* ÃèÊö    : ¶ÁÈ¡Êı¾İº¯Êı
* ÊäÈë    :  ÎŞ
* Êä³ö    : ÎŞ
* ·µ»ØÖµ  :ÎŞ
* ËµÃ÷    : 
*******************************************************************************/
void Fun_ReadData(void)
{
			uint8_t u8TimeCount_1s = 0;
			TIM2_1ms = 0;//¶¨Ê±¼ÆÊıÇåÁã
			USER_SystemFlag.A.Flag_1s = 0;
			while(System_State == ReadData_State)
			{
					W5500_Receive_Deal(); //ÍøÂçÍ¨ĞÅ²éÑ¯´¦Àí
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
