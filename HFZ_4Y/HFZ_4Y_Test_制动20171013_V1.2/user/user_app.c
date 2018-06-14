/****************************************************************************
* 文件名: MAIN_APP.c
* 文件历史:
* 版本号:    V1.0
* 日期  :    2016.02.16   
* 作者 :     XX
* 说明:      主函数调用的子函数  各个检测状态函数
*****************************************************************************/
/*=============================================================================
*******修改日志*******
1、日    期：
地    点：
修 改 人：
添加内容：
版本号:
2、日    期：
地    点：
修 改 人：
添加内容：
版本号:
================================================================================*/

#include "user_app.h"


// float VOLTAGE = 10*1000/65535;

/*******************************************************************************
* 函数名  : void IWDG_Configuration(void)
* 描述    : 独立看门狗配置函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 
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
* 函数名  : Fun_Adjust
* 描述    : 系统标定子状态
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void Fun_Adjust(void)
{
		_union_u16_u8_type Voltage_Zero, //零点电压上传变量
																				 Voltage_Real, //绝对电压上传变量
																				 Value_Real;		 //实时测量值
		float Voltage_Relative;//相对电压
	  _unionType1 Parameter;  //临时标定通道系数
		float AD_Data_sum=0.0;//AD转换结果的和 求平均值使用
//     USER_SystemFlag.A.Flag_Sucess1 = 1;
//     USER_SystemFlag.A.Flag_Standalone = 1;
		while(System_State == Adjust_State)
		{
				W5500_Receive_Deal(); //网口查询函数
				switch(AD_Channel_ID)
				{
						case Left_Weight_ID:
																								Voltage_Zero.u16_data = Zero_LeftWeight;	//对应模拟通道1通道
																								memcpy(Parameter.Load_Flash,Left_Weight_par.Load_Flash, 17);
																								break;
						case Right_Wight_ID:
																								Voltage_Zero.u16_data = Zero_RightWight;	//对应模拟通道2通道
																								memcpy(Parameter.Load_Flash,Right_Wight_par.Load_Flash, 17);
																								break;
						case Left_Brake_ID:
																								Voltage_Zero.u16_data = Zero_LeftBrake;	//对应模拟通道3通道
																								memcpy(Parameter.Load_Flash,Left_Brake_par.Load_Flash, 17);
																								break;
						case Right_Brake_ID:
																								Voltage_Zero.u16_data = Zero_RightBrake;	//对应模拟通道4通道
																								memcpy(Parameter.Load_Flash,Right_Brake_par.Load_Flash, 17);
																								break;
						case All_Weight_ID:
																								Voltage_Zero.u16_data = Zero_AllWeight;	//对应模拟通道5通道
																								memcpy(Parameter.Load_Flash,All_Weight_par.Load_Flash, 17);
																								break;
						case FootPlat_ID:
																								Voltage_Zero.u16_data = Zero_FootPlat;	//对应模拟通道6通道
																								memcpy(Parameter.Load_Flash,FootPlat_par.Load_Flash, 17);
																								break;
						case Left_Move_ID:
																								Voltage_Zero.u16_data = Zero_LeftMove;	//对应模拟通道7通道
																								memcpy(Parameter.Load_Flash,Left_Move_par.Load_Flash, 17);
																								break;
						case Right_Move_ID:
																								Voltage_Zero.u16_data = Zero_RightMove;	//对应模拟通道8通道
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
						/* 上传标定数据 */
						Tx_Buffer[0] = 8;  //帧长度 低位
						Tx_Buffer[1] = 0;  //帧长度 高位
						Tx_Buffer[2] = 'C'; 
						Tx_Buffer[3] = AD_Channel_ID; 
						Tx_Buffer[4] = Voltage_Zero.u8_data[0];
						Tx_Buffer[5] = Voltage_Zero.u8_data[1];
						Tx_Buffer[6] = Voltage_Real.u8_data[0];
						Tx_Buffer[7] = Voltage_Real.u8_data[1];
						Tx_Buffer[8] = Value_Real.u8_data[0];
						Tx_Buffer[9] = Value_Real.u8_data[1];
						if(USER_SystemFlag.A.Flag_Online == 1) //和主控联网 上传内容
						{
								W5500_Send_Data(2,Tx_Buffer);
								if(USER_SystemFlag.A.Flag_Standalone == 1) //和单机联网 上传内容
								{
										USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
										USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
										FunW5500_SendLED(0,0,Value_Real.u16_data);
								}
						}
						else
						{
								if(USER_SystemFlag.A.Flag_Standalone == 1) //和单机联网 上传内容
								{
										W5500_Send_Data(0,Tx_Buffer);
								}
						}
				}				
		}
}

/*******************************************************************************
* 函数名  : Fun_Zero
* 描述    : 模拟通道调零函数
* 输入    : Channel_ID 调零通道  state:是否直接调零 1:直接调零
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void Fun_Zero(uint8_t Channel_ID,uint8_t state)
{
		float AD_Data_sum=0.0;//AD转换结果的和 求平均值使用
		uint16_t Voltage_Zero=0;//零点电压

		AD_Data_sum = ADShow_Adjust(Channel_ID);
		Voltage_Zero = AD_Data_sum*VOLTAGE; //模拟量转换成电压  放大1000倍
#ifdef _DEBUG //调试用函数段
		printf("\r\n Voltage_Zero%u=%umv \r\n",Channel_ID,Voltage_Zero);	
#endif	
		switch(Channel_ID)
		{
				case Left_Weight_ID:
																						if(state == 1) Zero_LeftWeight = Voltage_Zero;	//对应模拟通道1通道
																					  if(Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftWeight = Voltage_Zero;	//对应模拟通道1通道
																						}
																						break;
				case Right_Wight_ID:
																						if(state == 1) Zero_RightWight = Voltage_Zero;	//对应模拟通道2通道
																						if(Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightWight = Voltage_Zero;	//对应模拟通道2通道
																						}																					
																						break;
				case Left_Brake_ID:
																						if(state == 1) Zero_LeftBrake = Voltage_Zero;	//对应模拟通道3通道
																						if(Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftBrake = Voltage_Zero;	//对应模拟通道3通道
																						}																									
																						break;
				case Right_Brake_ID:
																						if(state == 1) Zero_RightBrake = Voltage_Zero;	//对应模拟通道4通道
																						if(Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightBrake = Voltage_Zero;	//对应模拟通道4通道
																						}
																						break;
				case All_Weight_ID:
																						if(state == 1) Zero_AllWeight = Voltage_Zero;	//对应模拟通道5通道
																						if(Fun_VoltageConvertValue(All_Weight_ID,Voltage_Zero) < 50)
																						{
																								Zero_AllWeight = Voltage_Zero;	//对应模拟通道5通道
																						}																					
																						break;
				case FootPlat_ID:
																						if(state == 1) Zero_FootPlat = Voltage_Zero;	//对应模拟通道6通道
																						if(Fun_VoltageConvertValue(FootPlat_ID,Voltage_Zero) < 50)
																						{
																								Zero_FootPlat = Voltage_Zero;	//对应模拟通道6通道
																						}																						
																						break;
				case Left_Move_ID:
																						if(state == 1) Zero_LeftMove = Voltage_Zero;	//对应模拟通道7通道
																						if(Fun_VoltageConvertValue(Left_Move_ID,Voltage_Zero) < 50)
																						{
																								Zero_LeftMove = Voltage_Zero;	//对应模拟通道7通道
																						}
																						break;
				case Right_Move_ID:
																						if(state == 1) Zero_RightMove = Voltage_Zero;	//对应模拟通道8通道
																						if(Fun_VoltageConvertValue(Right_Move_ID,Voltage_Zero) < 50)
																						{
																								Zero_RightMove = Voltage_Zero;	//对应模拟通道8通道
																						}																						
																						break;
				default:
																						printf("\r\nError:The AD channel is false!!!Fun_Zero\r\n");
																						break;
		}
}

/*******************************************************************************
* 函数名  : Fun_AllWeightTest
* 描述    : 空载复合台称重测试函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void Fun_AllWeightTest(void)
{
		uint8_t Countdown_time;//倒计时初始值

		uint16_t u16Count300=0; 																		 //300个点存储计数
		uint16_t u16countMax=0; 																		 //计数最大存储点数量
		uint16_t Voltage_Relative;																		 //相对电压
		float fl32Left_SumWeight=0.0;													     //重量求和临时变量


		u16countMax = 0;//初始化
		
		
		if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
		{
				W5500_Send_Data(2,con_TranMF1);//上传MF1复合称重检测
        if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
        {
            if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
            {
                USER_LEDFlag.A.LED_Online = 1;  //联网指示灯
            }
            USER_LEDFlag.A.LED_Test = 1;  			//检测指示灯
            USER_LEDFlag.A.LED_15 = 0;  				//数码管数据是否改变 1变  0不变
            USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
            USER_LEDFlag.A.LED_Weight = 0;  		//重量指示灯
            USER_LEDFlag.A.LED_Brake = 1;  			//制动指示灯
            FunW5500_SendLED(0,0,0);            //上传指示灯
            Delay_ms(10);
        }
		}
    else
    {
        if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
        {
//             W5500_Send_Data(0,con_TranMF1);//上传MF1复合称重检测
            Delay_ms(10);
        }
    }
		All_DynamicWeight = 0;  //复合称重台动态显示重量
		AllNone_StaticWeight = 0;                  //初始化空载复合轴重结果
		Countdown_time = SysParameter.Par0_WeightTestTime + 1; //倒计时时间
		USER_SystemFlag.A.Flag_1s = 1;//首次调用上传倒计时第一个数使用
		while(System_State == StaticWeight_None_State)
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 1)
				{
						if(USER_SystemFlag.A.Flag_1s == 1)
						{
								TIM2_1ms = 0;//定时计数清零
								USER_SystemFlag.A.Flag_1s = 0;
								if(Countdown_time > 0) 
								{
										Countdown_time --;
										if(Countdown_time>0)
										{
												Tx_Buffer[0] = 3;  //帧长度 低位
												Tx_Buffer[1] = 0;  //帧长度 高位
												Tx_Buffer[2] = 'M'; 
												Tx_Buffer[3] = '1'; 
												Tx_Buffer[4] = Countdown_time;
												
												if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
												{
														W5500_Send_Data(2,Tx_Buffer);//
                            if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
                            {
                                if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
                                {
                                    USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
                                }
                                USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
                                USER_LEDFlag.A.LED_15 = 1;  				    //数码管数据是否改变 1变  0不变
                                USER_LEDFlag.A.LED_14 = 1;  				    //数码管指示灯是否改变 1变  0不变
                                USER_LEDFlag.A.LED_Weight = 0;  			  //重量指示灯
                                USER_LEDFlag.A.LED_Brake = 1;  			    //制动指示灯
                                FunW5500_SendLED(0,LED_OFF,Countdown_time);//上传倒计时
                            }
												}
                        else
                        {
                            if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
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
								/* 复合台称重计算 */
								fl32Left_SumWeight = ADShow_Samp(All_Weight_ID);
								fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE; //转换成电压值
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
						if(USER_SystemFlag.A.Flag_500ms == 1) //暂时不需要上传重量
						{
									USER_SystemFlag.A.Flag_500ms = 0;		
                   /* 复重 */
                  fl32Left_SumWeight = ADShow_Display(All_Weight_ID);//取平均值
                  fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE;//求电压值		
                  if(fl32Left_SumWeight > Zero_AllWeight)
                  {
                      Voltage_Relative = fl32Left_SumWeight - Zero_AllWeight;
                      All_DynamicWeight = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
                  }
                  else 
                  {
                      All_DynamicWeight = 0;
                  }
									if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
									{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
// 	  											FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//上传复合称重重量
												}
									}
									else
									{
											if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
											{
// 													FunW5500_SendGF(0,All_DynamicWeight); //上传复合称重
											}
									}
						 }
				}
				else
				{						
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
                 /* 复重 */
                fl32Left_SumWeight = ADShow_Display(All_Weight_ID);//取平均值
                fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE;//求电压值		
                if(fl32Left_SumWeight > Zero_AllWeight)
                {
                    Voltage_Relative = fl32Left_SumWeight - Zero_AllWeight;
                    All_DynamicWeight = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
                }
                else 
                {
                    All_DynamicWeight = 0;
                }
                if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
                {                   
                    if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
                    {
                        if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
                        {
                            USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
                        }
                        USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
                        USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
                        USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
                        USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
                        USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯
                        FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//上传复合称重重量
                    }
                }
                else
                {
                    if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
                    {  
                        W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号
                    }
                }                         												
						}						
						Countdown_time = SysParameter.Par0_WeightTestTime + 1; //倒计时时间
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 1;//首次调用上传倒计时第一个数使用
						u16Count300 = 0;
						u16countMax = 0;//初始化
				}
		}				
		#ifdef _DEBUG //调试用函数段
		printf("\r\n%u\r\n",u16countMax);
		#endif		
		if(u16countMax >= 300) u16countMax = 300;
		DataRank(u16Left1_TemporaryWeight,u16countMax); //排序
		#ifdef _DEBUG //调试用函数段
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
		#ifdef _DEBUG //调试用函数段
		printf("\r\nAllNone_StaticWeight=%u\r\n",AllNone_StaticWeight);
		#endif	
		 if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		 {
						W5500_Send_Data(2,con_TranT0);          //正常检测结束
						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
						{
								USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
								USER_LEDFlag.A.LED_14 = 0;  				//数码管指示灯是否改变 1变  0不变
	  						FunW5500_SendLED(0,LED_OFF,AllNone_StaticWeight);//上传复合称重重量
						}
						if(USER_SystemFlag.A.Flag_AutoD == 1)
						{
								USER_SystemFlag.A.Flag_AutoD = 0;
								FunW5500_SendDF(2,1,AllNone_StaticWeight);	//DF1
						}
		 }
		 else
		 {
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{
								FunW5500_SendGF(0,AllNone_StaticWeight);
								Delay_ms(20);
// 								W5500_Send_Data(0,con_TranT0); //正常检测结束
					}
		 }		
		System_State = ReadData_State; //上边倒计时时间结束也有这条状态切换
}


/*******************************************************************************
* 函数名  : Fun_VoltageConvertValue
* 描述    : 相对电压转换成实时测量值
* 输入    : AD_ID:模拟量通道      Voltage_Relative:相对电压
* 输出    : 无
* 返回值  : 实时测量值
* 说明    : 
*******************************************************************************/
uint16_t Fun_VoltageConvertValue(uint8_t AD_ID,uint16_t Voltage_Relative)
{
		uint16_t Value_Real,Value_Limit;
		_unionType1 Parameter;  //临时标定通道系数
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
		switch(Parameter.Load_Parameter.Parameter_NB)  //拟合次数
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
* 函数名  : IO_GetInStatus
* 描述    : 获取输入口状态
* 输入    : IO_ID:开入通道  
													 IN_Weight_ID     		//轮重到位信号开入ID
													 IN_Brake_ID						//制动到位信号开入ID
													 IN_Raise_ID			  		//举升到位信号开入ID
													 IN_Protect_ID     		//盖板保护信号开入ID
													 IN_LeftStop_ID      	//左停机信号开入ID
													 IN_RightStop_ID    	//右停机信号开入ID
* 输出    : 无
* 返回值  : 实时测量值
* 说明    : 
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
* 函数名  : DataRank
* 描述    : 数据排序 升序排列
* 输入    :   *Data 数据变量   number 排序总数量
* 输出    : 无
* 返回值  : 实时测量值
* 说明    : 
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
* 函数名  : Get_Parameter
* 描述    : 从存储芯片获取参数
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Get_Parameter(void)
{
		uint8_t u8flash_buffer[21];//标定系数  参数临时存储变量		
		/* 标定系数读出来 */
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address,17);
		memcpy(Left_Weight_par.Load_Flash,u8flash_buffer,17);	 //把系数保存到变量
	
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address,17);
		memcpy(Right_Wight_par.Load_Flash,u8flash_buffer,17);	 //把系数保存到变量
	
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Brake_Flash_Address,17);
		memcpy(Left_Brake_par.Load_Flash,u8flash_buffer,17);	 //把系数保存到变量
	
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Brake_Flash_Address,17);
		memcpy(Right_Brake_par.Load_Flash,u8flash_buffer,17);	 //把系数保存到变量
	
		SPI_FLASH_BufferRead(u8flash_buffer,All_Weight_Flash_Address,17);
		memcpy(All_Weight_par.Load_Flash,u8flash_buffer,17);	 //把系数保存到变量
	
		SPI_FLASH_BufferRead(u8flash_buffer,FootPlat_Flash_Address,17);
		memcpy(FootPlat_par.Load_Flash,u8flash_buffer,17);	 //把系数保存到变量
	
		SPI_FLASH_BufferRead(u8flash_buffer,Left_Move_Flash_Address,17);
		memcpy(Left_Move_par.Load_Flash,u8flash_buffer,17);	 //把系数保存到变量
		
		SPI_FLASH_BufferRead(u8flash_buffer,Right_Move_Flash_Address,17);
		memcpy(Right_Move_par.Load_Flash,u8flash_buffer,17);	 //把系数保存到变量
	
		//参数读取
	 Fun_GetParameter();
	 //出厂参数读取
	 SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,25);
	 if(Parameter_Buffer[24] != 0x55)//没保存出厂 则初始化IP
	 {
			 Parameter_Buffer[0] = 0;
			 Parameter_Buffer[1] = 0;
			 Parameter_Buffer[2] = 0;
			 Parameter_Buffer[3] = 0;		  //网关
		 
			 Parameter_Buffer[4] = '0';
			 Parameter_Buffer[5] = '0';
			 Parameter_Buffer[6] = '0';
			 Parameter_Buffer[7] = '0';      //子网掩码
		 
			 Parameter_Buffer[8] = '0';
			 Parameter_Buffer[9] = '0';
			 Parameter_Buffer[10] = '0';
			 Parameter_Buffer[11] = '0';     //主控板IP
 		 
			 Parameter_Buffer[12] = '0';
			 Parameter_Buffer[13] = '0';   //端口号25032
		 
			 Parameter_Buffer[14] = '0';
			 Parameter_Buffer[15] = '0';
			 Parameter_Buffer[16] = '0';
			 Parameter_Buffer[17] = '0';   // 数码管IP
			 
			 Parameter_Buffer[18] = '0';			
			 Parameter_Buffer[19] = '0';    //数码管端口号
			 			 
			 Parameter_Buffer[20] = '0';
			 Parameter_Buffer[21] = '0';
			 Parameter_Buffer[22] = '0';
			 Parameter_Buffer[23] = '0';   //UDP广播目的地址
			 
			 Parameter_Buffer[24] = 0x55;
			 SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//擦出扇区
			 SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,25);//保存系数		
	 }	
	 
	 //IP参数读取
	 SPI_FLASH_BufferRead(W5500_Parameter,ParameterIP_Flash_Address,27);
	 if(W5500_Parameter[26] != 0x55)//没保存IP 则初始化IP
	 {
			 W5500_Parameter[0] = 192;
			 W5500_Parameter[1] = 168;
			 W5500_Parameter[2] = 100;
			 W5500_Parameter[3] = 1;		  //网关
		 
			 W5500_Parameter[4] = 255;
			 W5500_Parameter[5] = 255;
			 W5500_Parameter[6] = 255;
			 W5500_Parameter[7] = 0;      //子网掩码
		 
			 W5500_Parameter[8] = 192;
			 W5500_Parameter[9] = 168;
			 W5500_Parameter[10] = 100;
			 W5500_Parameter[11] = 4;     //主控板IP
 		 
			 W5500_Parameter[12] = 0xC8;
			 W5500_Parameter[13] = 0x61;   //端口号25032
		 
			 W5500_Parameter[14] = 0;
			 W5500_Parameter[15] = 0;
			 W5500_Parameter[16] = 0;
			 W5500_Parameter[17] = 0;   // 数码管IP
			 
			 W5500_Parameter[18] = 0;			
			 W5500_Parameter[19] = 0;    //数码管端口号
			 			 
			 W5500_Parameter[20] = 192;
			 W5500_Parameter[21] = 168;
			 W5500_Parameter[22] = 1;
			 W5500_Parameter[23] = 200;   //UDP广播目的地址
			 
			 W5500_Parameter[24] = 0xC7;
			 W5500_Parameter[25] = 0x61;  //UDP端口号
			 
			 W5500_Parameter[26] = 0x55;
			 SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//擦出扇区
			 SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//保存系数		
	 }	
		
}

/*******************************************************************************
* 函数名  : Fun_SignalTest
* 描述    : 信号测试
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
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
						fl32ADSum_Channen8 = 0.0;  //相加的和
		uint16_t Voltage_Relative[8];//相对电压
		uint16_t DynamicData[8];     //动态测量值
		uint8_t u8Count_100ms;  //上传频率计数
		while(System_State == SignalTest_State)
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(USER_SystemFlag.A.Flag_500ms == 1)
				{
						USER_SystemFlag.A.Flag_500ms = 0;
						
// 						if(SysParameter.Par12_MachineType == 0)//轮重台
						if(0)//默认制动台
						{
								/* 1通道计算 */
								fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);

								fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen1 > Zero_LeftWeight)
								{
										Voltage_Relative[0] = fl32ADSum_Channen1 - Zero_LeftWeight;
										DynamicData[0] = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative[0]);
								}
								else 
								{
										DynamicData[0] = 0;
								}
								/* 2通道计算 */
								fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
								fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //转换成电压值
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
								Tx_Buffer[3] = 2;    //模拟量数量
								Tx_Buffer[4] = 1;    //开入量数量
								/* 1通道 */
								Tx_Buffer[5] = Left_Weight_ID;    
								Tx_Buffer[6] = Zero_LeftWeight%256;    
								Tx_Buffer[7] = Zero_LeftWeight/256;    
								Tx_Buffer[8] = (uint32_t)fl32ADSum_Channen1%256;    
								Tx_Buffer[9] = (uint32_t)fl32ADSum_Channen1/256;
								Tx_Buffer[10] = DynamicData[0]%256;
								Tx_Buffer[11] = DynamicData[0]/256;
								/* 2通道 */
								Tx_Buffer[12] = Right_Wight_ID;    
								Tx_Buffer[13] = Zero_RightWight%256;    
								Tx_Buffer[14] = Zero_RightWight/256;    
								Tx_Buffer[15] = (uint32_t)fl32ADSum_Channen2%256;    
								Tx_Buffer[16] = (uint32_t)fl32ADSum_Channen2/256;
								Tx_Buffer[17] = DynamicData[1]%256;
								Tx_Buffer[18] = DynamicData[1]/256;
								/* 开入量 */
								Tx_Buffer[19] = IN_Weight_ID;    
								Tx_Buffer[20] = IO_GetInStatus(IN_Weight_ID)+0x30; 
						}
						else if((SysParameter.Par12_MachineType == 1) || (SysParameter.Par12_MachineType == 2))//普通制动台和气动加载制动台
						{
								/* 3通道计算 */
								fl32ADSum_Channen3 = ADShow_Display(Left_Brake_ID);
								fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen3 > Zero_LeftBrake)
								{
										Voltage_Relative[2] = fl32ADSum_Channen3 - Zero_LeftBrake;
										DynamicData[2] = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative[2]);
								}
								else 
								{
										DynamicData[2] = 0;
								}
								/* 4通道计算 */
								fl32ADSum_Channen4 = ADShow_Display(Right_Brake_ID);
								fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen4 > Zero_RightBrake)
								{
										Voltage_Relative[3] = fl32ADSum_Channen4 - Zero_RightBrake;
										DynamicData[3] = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative[3]);
								}
								else 
								{
										DynamicData[3] = 0;
								}
								/* 5通道计算 */
								fl32ADSum_Channen5 = ADShow_Display(All_Weight_ID);
								fl32ADSum_Channen5 = fl32ADSum_Channen5*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen5 > Zero_AllWeight)
								{
										Voltage_Relative[4] = fl32ADSum_Channen5 - Zero_AllWeight;
										DynamicData[4] = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative[4]);
								}
								else 
								{
										DynamicData[4] = 0;
								}
								/* 6通道计算 */
								fl32ADSum_Channen6 = ADShow_Display(FootPlat_ID);
								fl32ADSum_Channen6 = fl32ADSum_Channen6*VOLTAGE; //转换成电压值
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
								Tx_Buffer[3] = 4;    //模拟量数量
								Tx_Buffer[4] = 5;    //开入量数量
								/* 3通道 */
								Tx_Buffer[5] = Left_Brake_ID;    
								Tx_Buffer[6] = Zero_LeftBrake%256;    
								Tx_Buffer[7] = Zero_LeftBrake/256;    
								Tx_Buffer[8] = (uint32_t)fl32ADSum_Channen3%256;    
								Tx_Buffer[9] = (uint32_t)fl32ADSum_Channen3/256;
								Tx_Buffer[10] = DynamicData[2]%256;
								Tx_Buffer[11] = DynamicData[2]/256;
								/* 4通道 */
								Tx_Buffer[12] = Right_Brake_ID;    
								Tx_Buffer[13] = Zero_RightBrake%256;    
								Tx_Buffer[14] = Zero_RightBrake/256;    
								Tx_Buffer[15] = (uint32_t)fl32ADSum_Channen4%256;    
								Tx_Buffer[16] = (uint32_t)fl32ADSum_Channen4/256;
								Tx_Buffer[17] = DynamicData[3]%256;
								Tx_Buffer[18] = DynamicData[3]/256;
								/* 5通道 */
								Tx_Buffer[19] = All_Weight_ID;    
								Tx_Buffer[20] = Zero_AllWeight%256;    
								Tx_Buffer[21] = Zero_AllWeight/256;    
								Tx_Buffer[22] = (uint32_t)fl32ADSum_Channen5%256;    
								Tx_Buffer[23] = (uint32_t)fl32ADSum_Channen5/256;
								Tx_Buffer[24] = DynamicData[4]%256;
								Tx_Buffer[25] = DynamicData[4]/256;
								/* 6通道 */
								Tx_Buffer[26] = FootPlat_ID;    
								Tx_Buffer[27] = Zero_FootPlat%256;    
								Tx_Buffer[28] = Zero_FootPlat/256;    
								Tx_Buffer[29] = (uint32_t)fl32ADSum_Channen6%256;    
								Tx_Buffer[30] = (uint32_t)fl32ADSum_Channen6/256;
								Tx_Buffer[31] = DynamicData[5]%256;
								Tx_Buffer[32] = DynamicData[5]/256;
								/* 开入量 */										
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
								/* 3通道计算 */
								fl32ADSum_Channen3 = ADShow_Display(Left_Brake_ID);
								fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen3 > Zero_LeftBrake)
								{
										Voltage_Relative[2] = fl32ADSum_Channen3 - Zero_LeftBrake;
										DynamicData[2] = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative[2]);
								}
								else 
								{
										DynamicData[2] = 0;
								}
								/* 4通道计算 */
								fl32ADSum_Channen4 = ADShow_Display(Right_Brake_ID);
								fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen4 > Zero_RightBrake)
								{
										Voltage_Relative[3] = fl32ADSum_Channen4 - Zero_RightBrake;
										DynamicData[3] = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative[3]);
								}
								else 
								{
										DynamicData[3] = 0;
								}
								/* 5通道计算 */
								fl32ADSum_Channen5 = ADShow_Display(All_Weight_ID);
								fl32ADSum_Channen5 = fl32ADSum_Channen5*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen5 > Zero_AllWeight)
								{
										Voltage_Relative[4] = fl32ADSum_Channen5 - Zero_AllWeight;
										DynamicData[4] = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative[4]);
								}
								else 
								{
										DynamicData[4] = 0;
								}
								/* 6通道计算 */
								fl32ADSum_Channen6 = ADShow_Display(FootPlat_ID);
								fl32ADSum_Channen6 = fl32ADSum_Channen6*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen6 > Zero_FootPlat)
								{
										Voltage_Relative[5] = fl32ADSum_Channen6 - Zero_FootPlat;
										DynamicData[5] = Fun_VoltageConvertValue(FootPlat_ID,Voltage_Relative[5]);
								}
								else 
								{
										DynamicData[5] = 0;
								}
								/* 7通道计算 */
								fl32ADSum_Channen7 = ADShow_Display(Left_Move_ID);
								fl32ADSum_Channen7 = fl32ADSum_Channen7*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen7 > Zero_LeftMove)
								{
										Voltage_Relative[6] = fl32ADSum_Channen7 - Zero_LeftMove;
										DynamicData[6] = Fun_VoltageConvertValue(Left_Move_ID,Voltage_Relative[6]);
								}
								else 
								{
										DynamicData[6] = 0;
								}
								/* 8通道计算 */
								fl32ADSum_Channen8 = ADShow_Display(Right_Move_ID);
								fl32ADSum_Channen8 = fl32ADSum_Channen8*VOLTAGE; //转换成电压值
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
								Tx_Buffer[3] = 6;    //模拟量数量
								Tx_Buffer[4] = 5;    //开入量数量
								/* 3通道 */
								Tx_Buffer[5] = Left_Brake_ID;    
								Tx_Buffer[6] = Zero_LeftBrake%256;    
								Tx_Buffer[7] = Zero_LeftBrake/256;    
								Tx_Buffer[8] = (uint32_t)fl32ADSum_Channen3%256;    
								Tx_Buffer[9] = (uint32_t)fl32ADSum_Channen3/256;
								Tx_Buffer[10] = DynamicData[2]%256;
								Tx_Buffer[11] = DynamicData[2]/256;
								/* 4通道 */
								Tx_Buffer[12] = Right_Brake_ID;    
								Tx_Buffer[13] = Zero_RightBrake%256;    
								Tx_Buffer[14] = Zero_RightBrake/256;    
								Tx_Buffer[15] = (uint32_t)fl32ADSum_Channen4%256;    
								Tx_Buffer[16] = (uint32_t)fl32ADSum_Channen4/256;
								Tx_Buffer[17] = DynamicData[3]%256;
								Tx_Buffer[18] = DynamicData[3]/256;
								/* 5通道 */
								Tx_Buffer[19] = All_Weight_ID;    
								Tx_Buffer[20] = Zero_AllWeight%256;    
								Tx_Buffer[21] = Zero_AllWeight/256;    
								Tx_Buffer[22] = (uint32_t)fl32ADSum_Channen5%256;    
								Tx_Buffer[23] = (uint32_t)fl32ADSum_Channen5/256;
								Tx_Buffer[24] = DynamicData[4]%256;
								Tx_Buffer[25] = DynamicData[4]/256;
								/* 6通道 */
								Tx_Buffer[26] = FootPlat_ID;    
								Tx_Buffer[27] = Zero_FootPlat%256;    
								Tx_Buffer[28] = Zero_FootPlat/256;    
								Tx_Buffer[29] = (uint32_t)fl32ADSum_Channen6%256;    
								Tx_Buffer[30] = (uint32_t)fl32ADSum_Channen6/256;
								Tx_Buffer[31] = DynamicData[5]%256;
								Tx_Buffer[32] = DynamicData[5]/256;
								/* 7通道 */
								Tx_Buffer[33] = Left_Move_ID;    
								Tx_Buffer[34] = Zero_LeftMove%256;    
								Tx_Buffer[35] = Zero_LeftMove/256;    
								Tx_Buffer[36] = (uint32_t)fl32ADSum_Channen7%256;    
								Tx_Buffer[37] = (uint32_t)fl32ADSum_Channen7/256;
								Tx_Buffer[38] = DynamicData[6]%256;
								Tx_Buffer[39] = DynamicData[6]/256;
								/* 8通道 */
								Tx_Buffer[40] = Right_Move_ID;    
								Tx_Buffer[41] = Zero_RightMove%256;    
								Tx_Buffer[42] = Zero_RightMove/256;    
								Tx_Buffer[43] = (uint32_t)fl32ADSum_Channen8%256;    
								Tx_Buffer[44] = (uint32_t)fl32ADSum_Channen8/256;
								Tx_Buffer[45] = DynamicData[7]%256;
								Tx_Buffer[46] = DynamicData[7]/256;
								/* 开入量 */										
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
* 函数名  : Fun_DynamoStart
* 描述    : 电机启动
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_DynamoStart(void)  //没有改取值点数
{
		float  fl32ADSum_Channen3 = 0.0,
						 fl32ADSum_Channen4 = 0.0;
		uint16_t Voltage_Relative;		//相对电压
        uint8_t u8Protect_CountFlg = 0;    //左电机保护阀值计数
		uint8_t u8Protect_CountFlg2 = 0;  //右电机保护阀值计数
		uint16_t u16Protect_Limit1; //电机启动前不同的保护阀值
		
//		Pump_UpDown = IO_OPEN;  //保护，保证出车举升器下降
		if(USER_LEDFlag.A.LED_Down!=1)
		{
				Pump_UpDown = IO_OPEN;				
        USER_LEDFlag.A.LED_Down = 1;         //出车降
        USER_LEDFlag.A.LED_Up = 1;           //出车举
				Delay_ms(2000);
		}    
// 		W5500_Send_Data(0,con_TranMD);  //上传MD进入电机启动状态
		/* 保护阀值设置 */
		if(Left_StaticWeight<StaticWeight_Init)
		{
				Left_StaticWeight = StaticWeight_Init;
// 				W5500_Send_Data(0,con_TranT9);//轮重太小使用默认轮重
		}
		if(Right_StaticWeight<StaticWeight_Init)
		{
				Right_StaticWeight = StaticWeight_Init;
// 				W5500_Send_Data(0,con_TranT9);//轮重太小使用默认轮重
		}
		if(System_State == BrakeTest_Full_State)
		{
         if(AllFull_StaticWeight > 100)
         {
            u16Protect_Limit = AllFull_StaticWeight*4/2;//保护阀值为轮重的40%
         }
         else
         {
            u16Protect_Limit = (Left_StaticWeight + Right_StaticWeight)*4/2;//保护阀值为轮重的40%
         }
		}
		else
		{
				 u16Protect_Limit = (Left_StaticWeight + Right_StaticWeight)*4/2;//保护阀值为轮重的40%
		}
		if(u16Protect_Limit < (SysParameter.Par13_UpHeight*10))
		{
			u16Protect_Limit = SysParameter.Par13_UpHeight*10;
		}
		u16Protect_Limit1 = u16Protect_Limit;
        if(u16Protect_Limit1 < 2000) u16Protect_Limit1 = 2000;
		
		//数码管清零
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发 
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{
// 								USER_LEDFlag.A.LED_TJUp = 1;  					//台架举指示灯
// 								USER_LEDFlag.A.LED_TJDown = 0;  		//台架降指示灯
								USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
								USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
								FunW5500_SendLED(0,0,0);//上传制动力
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{						
// 							 FunW5500_SendGB(0,0,0); //上传制动力
					}
		}  		
		if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
		{
				Left_DynamicBrake = 0;
				Right_DynamicBrake = 0;
				if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
				{
						W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
				}
				if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
				{
						W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
				}
				System_State = Default_State;          //进入默认状态 制动待机
				Speed_Control = IO_CLOSE; 						//测速板停止
				Left_Machine = IO_CLOSE;							//左电机停止
				Right_Machine = IO_CLOSE;						//右电机停止
				Machine_Switch = IO_CLOSE;					//星三角
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
		//制动力强制清零
		Fun_Zero(Left_Brake_ID,1);//判断左制动是否调零
		Fun_Zero(Right_Brake_ID,1);//判断右制动是否调零
		Delay_ms(2000);        // 20180503
		
		Speed_Control = IO_OPEN; 	//测速板St信号启动
		USER_SystemFlag.A.Flag_ST = 1;//停机板启动标志
		Left_Machine = IO_OPEN;			//左电机启动
		USER_SystemFlag.A.Flag_Ldynamo = 1;//左电机启动标志
		TIM2_count=0; 
		while(TIM2_count < 1000)  		//1S延时启动
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
						{
								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						{
								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
						}
						System_State = Default_State;          //进入默认状态 制动待机
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
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
						/* 3通道左制计算 */
						fl32ADSum_Channen3 = ADShow_Samp(Left_Brake_ID);
						fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //转换成电压值
						if(fl32ADSum_Channen3 > Zero_LeftBrake)
						{
								Voltage_Relative = fl32ADSum_Channen3 - Zero_LeftBrake;
								Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Left_DynamicBrake = 0;
						}
						/* 4通道右制计算 */
						fl32ADSum_Channen4 = ADShow_Samp(Right_Brake_ID);
						fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //转换成电压值
						if(fl32ADSum_Channen4 > Zero_RightBrake)
						{
								Voltage_Relative = fl32ADSum_Channen4 - Zero_RightBrake;
								Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Right_DynamicBrake = 0;
						}																				
						if(Left_DynamicBrake > u16Protect_Limit1)  //阻滞保护
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX1)  //3点保护停机
								{
										System_State = ReadData_State;     //进入读数状态
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Machine_Switch = IO_CLOSE;					//星三角			
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT1(2,Left_DynamicBrake); //左轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																FunW5500_SendLED(0,Left_DynamicBrake,0);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake); //左轮阻滞大
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
		Right_Machine = IO_OPEN;			//右电机启动
		USER_SystemFlag.A.Flag_Rdynamo = 1;//右电机启动标志
		TIM2_count=0; 
		while(TIM2_count < 1000)  		//1S延时启动
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
						{
								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						{
								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
						}
						System_State = Default_State;          //进入默认状态 制动待机
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
					  return;
				}
				if(USER_SystemFlag.A.Flag_10ms == 1) //不启用
				{
						USER_SystemFlag.A.Flag_10ms = 0;
						/* 3通道左制计算 */
						fl32ADSum_Channen3 = ADShow_Samp(Left_Brake_ID);
						fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //转换成电压值
						if(fl32ADSum_Channen3 > Zero_LeftBrake)
						{
								Voltage_Relative = fl32ADSum_Channen3 - Zero_LeftBrake;
								Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Left_DynamicBrake = 0;
						}
						/* 4通道右制计算 */
						fl32ADSum_Channen4 = ADShow_Samp(Right_Brake_ID);
						fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //转换成电压值
						if(fl32ADSum_Channen4 > Zero_RightBrake)
						{
								Voltage_Relative = fl32ADSum_Channen4 - Zero_RightBrake;
								Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Right_DynamicBrake = 0;
						}															
						if(Left_DynamicBrake > u16Protect_Limit1)  //阻滞保护
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3点保护停机
								{
										System_State = ReadData_State;          //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;					//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT1(2,Left_DynamicBrake); //左轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																FunW5500_SendLED(0,Left_DynamicBrake,0);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake); //左轮阻滞大
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake > u16Protect_Limit1)  //阻滞保护
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX1)  //3点保护停机
								{
										System_State = ReadData_State;         //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;						//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT2(2,Right_DynamicBrake); //右轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																FunW5500_SendLED(0,0,Right_DynamicBrake);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake); //右轮阻滞大
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
		Machine_Switch = IO_OPEN;			//星三角
		USER_SystemFlag.A.Flag_Switch = 1;//星三角启动标志
		fl32ADSum_Channen3 = 0.0;
		fl32ADSum_Channen4 = 0.0;
		TIM2_count=0; 
		while(TIM2_count < 1000)  		//1S延时启动
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
						{
								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						{
								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
						}
						System_State = Default_State;          //进入默认状态 制动待机
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
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
					   /* 3通道左制计算 */
						fl32ADSum_Channen3 = ADShow_Samp(Left_Brake_ID);
						fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //转换成电压值
						if(fl32ADSum_Channen3 > Zero_LeftBrake)
						{
								Voltage_Relative = fl32ADSum_Channen3 - Zero_LeftBrake;
								Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Left_DynamicBrake = 0;
						}
						/* 4通道右制计算 */
						fl32ADSum_Channen4 = ADShow_Samp(Right_Brake_ID);
						fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE; //转换成电压值
						if(fl32ADSum_Channen4 > Zero_RightBrake)
						{
								Voltage_Relative = fl32ADSum_Channen4 - Zero_RightBrake;
								Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
						}
						else 
						{
								Right_DynamicBrake = 0;
						}										
						if(Left_DynamicBrake > u16Protect_Limit1)  //阻滞保护
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3点保护停机
								{
										System_State = ReadData_State;          //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;					//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT1(2,Left_DynamicBrake); //左轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																FunW5500_SendLED(0,Left_DynamicBrake,0);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake); //左轮阻滞大
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake > u16Protect_Limit1)  //阻滞保护
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX)  //3点保护停机
								{
										System_State = ReadData_State;         //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;						//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT2(2,Right_DynamicBrake); //右轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																FunW5500_SendLED(0,0,Right_DynamicBrake);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake); //右轮阻滞大
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
		Speed_Control = IO_CLOSE; 						//测速板停止 进入正常检测硬停机判断
		USER_SystemFlag.A.Flag_ST = 0;    //停机板启动标志
// 		System_State = Default_State;         //进入默认状态 制动待机
		SysParameter.DynamoStart_Flag = 1;//待机时进行硬停机判断
}
/*******************************************************************************
* 函数名  : Fun_Stable()
* 描述    : 待机状态
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_Stable(void)
{
		float	fl32ADSum_Channen1 = 0.0,
						fl32ADSum_Channen2 = 0.0,
						fl32ADSum_Channen3 = 0.0,
						fl32ADSum_Channen4 = 0.0,
						fl32ADSum_Channen5 = 0.0;      //累加和
		uint16_t Voltage_Relative;									 //相对电压
// 		Fun_Zero(Left_Weight_ID,0);//判断重量是否调零
// 		Fun_Zero(Right_Wight_ID,0);//判断重量是否调零
		Fun_Zero(Left_Brake_ID,1);//判断左制动是否调零
		Fun_Zero(Right_Brake_ID,1);//判断右制动是否调零
		
		Fun_Zero(All_Weight_ID,0);//判断复合称重是否调零
		while(System_State == Default_State)
		{
				W5500_Receive_Deal(); //网络通信查询处理
// 				if(SysParameter.Par12_MachineType == 0) //轮重台
// 				{
// 						if(USER_SystemFlag.A.Flag_500ms == 1)
// 						{
// 									USER_SystemFlag.A.Flag_500ms = 0;
// 									/*左重*/
// 									fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);
// 									fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //转换成电压值
// 									if(fl32ADSum_Channen1 > Zero_LeftWeight)
// 									{
// 											Voltage_Relative = fl32ADSum_Channen1 - Zero_LeftWeight;
// 											Left_DynamicWeight = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
// 									}
// 									else 
// 									{
// 											Left_DynamicWeight = 0;
// 									}
// 									/*右重*/
// 									fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
// 									fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //转换成电压值
// 									if(fl32ADSum_Channen2 > Zero_RightWight)
// 									{
// 											Voltage_Relative = fl32ADSum_Channen2 - Zero_RightWight;
// 											Right_DynamicWeight = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
// 									}
// 									else 
// 									{
// 											Right_DynamicWeight = 0;
// 									}											
// 									if(USER_SystemFlag.A.Flag_Online == 1) //和主控联网 上传内容
// 									{									
// 											if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
// 											{
// 													if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
// 													{
// 															USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
// 													}
// 													USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
// 													USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
// 													USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
// 													USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯
// 													FunW5500_SendLED(0,Left_DynamicWeight,Right_DynamicWeight);	
// 											}
// 									}
// 									else
// 									{
// 											if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
// 											{
// 													FunW5500_SendGW(0,Left_DynamicWeight,Right_DynamicWeight); //上传动态显示轮重
// 											}											
// 									}
// 						}			
// 				}
// 				else  //制动台
				{						
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
								/* 左制计算 */
								fl32ADSum_Channen3 = ADShow_Display(Left_Brake_ID);
								fl32ADSum_Channen3 = fl32ADSum_Channen3*VOLTAGE; //转换成电压值
								if(fl32ADSum_Channen3 > Zero_LeftBrake)
								{
										Voltage_Relative = fl32ADSum_Channen3 - Zero_LeftBrake;
										Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
										Left_DynamicBrake += 5;
										Left_DynamicBrake /= 10;
										Left_DynamicBrake *= 10;   //保留daN
								}
								else 
								{
										Left_DynamicBrake = 0;
								}
							
								/* 右制计算 */
								fl32ADSum_Channen4 = ADShow_Display(Right_Brake_ID);//取平均值
								fl32ADSum_Channen4 = fl32ADSum_Channen4*VOLTAGE;//求电压值		
								if(fl32ADSum_Channen4 > Zero_RightBrake)
								{
										Voltage_Relative = fl32ADSum_Channen4 - Zero_RightBrake;
										Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
										Right_DynamicBrake += 5;
										Right_DynamicBrake /= 10;
										Right_DynamicBrake *= 10;  //保留daN
								}
								else 
								{
										Right_DynamicBrake = 0;
								}							
								if(SysParameter.Par12_MachineType > 1)//加载制动台时有复合称重
								{
										/* 复重 */
										fl32ADSum_Channen5 = ADShow_Display(All_Weight_ID);//取平均值
										fl32ADSum_Channen5 = fl32ADSum_Channen5*VOLTAGE;//求电压值		
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
								 if(USER_SystemFlag.A.Flag_Online == 1) //和主控联网 上传内容
								 {
											if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
											{
													if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
													{
															USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
													}
                          USER_LEDFlag.A.LED_Test = 0;  					//检测指示灯
													USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
													USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
													USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
													USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯
													FunW5500_SendLED(0,Left_DynamicBrake,Right_DynamicBrake);	
											}
// 											FunW5500_SendLED(2,Left_DynamicBrake,Right_DynamicBrake);	
								 }
								 else
								 {
											if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
											{
													FunW5500_SendGB(0,Left_DynamicBrake,Right_DynamicBrake);	
											}
								 }
						}																
				}				
		}
}
/*******************************************************************************
* 函数名  : Fun_TestZZ()
* 描述    : 阻滞测试
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_TestZZ(void)
{
		uint8_t Countdown_time;//倒计时初始值
		uint8_t u8Left_flg=0,        //左停机标志
							  u8Right_flg=0;			//右停机标志
		uint16_t u16Count800=0; //800个点存储计数
		uint16_t u16countMax=0; //计数最大存储点数量
		uint16_t Voltage_Relative;//相对电压
		float fl32Left_SumBrake=0,
					 fl32Right_SumBrake=0;//制动求和临时变量
		uint8_t u8Protect_CountFlg = 0;    //左电机保护阀值计数
		uint8_t u8Protect_CountFlg2 = 0;  //右电机保护阀值计数
    
    float fl32Left_SumBrake1=0,
					  fl32Right_SumBrake1=0;																	  //制动求和临时变量		上传显示使用 			
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1;			
		u16countMax = 0;//初始化		
		if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
		{
				W5500_Send_Data(2,con_TranMZ);//上传阻滞检测
        if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
        {
              if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
              {
                  USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
              }
              USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
              USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
              USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯					
              USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
              USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
// 							FunW5500_SendLED(0,0,0);//上传制动力
        }
		}
    else
    {
        if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
        {
            W5500_Send_Data(0,con_TranMZ);//上传阻滞检测
        }
    }
		/***启动电机****/
		Fun_DynamoStart();
		if(System_State != Retardant_State)
		{
				return;
		}
		/*******************/
		Left_DynamicBrake = 0;
		Right_DynamicBrake = 0;  //动态显示值清零
		Left_DynamicBrake1 = 0;
		Right_DynamicBrake1 = 0;
		Countdown_time = SysParameter.Par4_ZuzhiTestTime + 1; //倒计时时间
// 	Countdown_time = 2;		
		USER_SystemFlag.A.Flag_1s = 1;//首次调用上传倒计时第一个数使用
		
		while(System_State == Retardant_State)
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
						{
								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						{
								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
						}
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						u8Left_flg = 1;	
						u8Right_flg = 1;
				}
				if(SysParameter.Par1_RollerFlag == 1)//硬停机有效
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_flg == 0)) //左停机信号
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_flg = 1;											
										Left_Machine = IO_CLOSE;     						//左停机
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													W5500_Send_Data(2,con_TranT7); 		 //左轮脉冲异常
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
		// 														W5500_Send_Data(0,con_TranT7); 		 //左轮脉冲异常
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
		// 													 W5500_Send_Data(0,con_TranT7); 		 //左轮脉冲异常
													}
										}  																
										#ifdef _DEBUG //调试用函数段
										printf("\r\nError:Left pulse is false!!!T7\r\n");
										#endif	
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_flg == 0)) //右停机信号
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_flg = 1;
										Right_Machine = IO_CLOSE;     							//右停机						
										USER_SystemFlag.A.Flag_Rdynamo = 0;		
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													W5500_Send_Data(2,con_TranT8);  		//右轮脉冲异常
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
		// 													W5500_Send_Data(0,con_TranT8);  		//右轮脉冲异常
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
		// 													 W5500_Send_Data(0,con_TranT8);  		//右轮脉冲异常
													}
										}  
										#ifdef _DEBUG //调试用函数段
										printf("\r\nError:Left pulse is false!!!T8\r\n");
										#endif
								}								
						}
						if((u8Left_flg == 1) && (u8Right_flg == 1))
						{
								Machine_Switch = IO_CLOSE;						//星三角
								USER_SystemFlag.A.Flag_Switch = 0;
								System_State = Default_State;
								return;
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
										Tx_Buffer[0] = 3;  //帧长度 低位
										Tx_Buffer[1] = 0;  //帧长度 高位
										Tx_Buffer[2] = 'M'; 
										Tx_Buffer[3] = '1'; 
										Tx_Buffer[4] = Countdown_time;
									  if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
									  {
												W5500_Send_Data(2,Tx_Buffer);
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
															USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
															USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
															FunW5500_SendLED(0,LED_OFF,Countdown_time);
												}
									  }
										else
										{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
															W5500_Send_Data(0,Tx_Buffer);//
												}
										}
								}
								else
								{
										System_State = Default_State;
// 										Left_Machine = IO_CLOSE;							//左电机停止
// 										Right_Machine = IO_CLOSE;						//右电机停止
// 										Machine_Switch = IO_CLOSE;						//星三角
								}
						}
				}
				if(USER_SystemFlag.A.Flag_500ms == 1)  //显示使用
				{
						USER_SystemFlag.A.Flag_500ms = 0;
						/* 左制动计算 */
						fl32Left_SumBrake1 = ADShow_Display(Left_Brake_ID);
						fl32Left_SumBrake1 = fl32Left_SumBrake1*VOLTAGE; //转换成电压值
						if(fl32Left_SumBrake1 > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake1 - Zero_LeftBrake;
								Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake += 5;
								Left_DynamicBrake /= 10;
								Left_DynamicBrake *= 10;   //保留daN
						}
						else 
						{
								Left_DynamicBrake = 0;
						}
						/* 右制动计算 */
						fl32Right_SumBrake1 = ADShow_Display(Right_Brake_ID);//取平均值
						fl32Right_SumBrake1 = fl32Right_SumBrake1*VOLTAGE;//求电压值		
						if(fl32Right_SumBrake1 > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake1 - Zero_RightBrake;
								Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake += 5;
								Right_DynamicBrake /= 10;
								Right_DynamicBrake *= 10;  //保留daN
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
						/* 左制动计算 */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //转换成电压值
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //保留daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						if(u16Count800 < 800)
						{
								u16Left_Data[u16Count800] = Left_DynamicBrake1;
						}
						/* 右制动计算 */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//取平均值
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//求电压值		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //保留daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if(u16Count800 < 800)
						{
								u16Right_Data[u16Count800] = Right_DynamicBrake1;
						}					
						if(Left_DynamicBrake1 > u16Protect_Limit)  //阻滞保护
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3点保护停机
								{
										System_State = Default_State;          //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;					//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
//										W5500_Send_Data(0,con_TranT1);  //左轮阻滞大
										SysParameter.DynamoStart_Flag = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT1(2,Left_DynamicBrake1); //左轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{                                
                              USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
                              USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
                              FunW5500_SendLED(0,Left_DynamicBrake1,0);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake1); //左轮阻滞大
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake1 > u16Protect_Limit)  //阻滞保护
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX)  //3点保护停机
								{
										System_State = Default_State;         //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;						//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
//										W5500_Send_Data(0,con_TranT2);  //右轮阻滞大
										SysParameter.DynamoStart_Flag = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT2(2,Right_DynamicBrake1); //右轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																FunW5500_SendLED(0,0,Right_DynamicBrake1);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake1); //右轮阻滞大
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
		Deal_Data(u16countMax,u8Tran_CurrentZ,'Z');  //阻滞数据保存到上传数据单元
		Fun_DealZZ(u16countMax); //阻滞计算结果
		if(SysParameter.Retardant_Flag == 1)  							  //测试完阻滞后电机停止
		{
				USER_SystemFlag.A.Flag_TestC = 0;
				USER_SystemFlag.A.Flag_TestC1 = 0;
				Speed_Control = IO_CLOSE; 												 //测速板停止
				Left_Machine = IO_CLOSE;													 //左电机停止
				Right_Machine = IO_CLOSE;												 //右电机停止
				Machine_Switch = IO_CLOSE;											 //星三角
				USER_SystemFlag.A.Flag_Ldynamo = 0;
				USER_SystemFlag.A.Flag_Rdynamo = 0;
				USER_SystemFlag.A.Flag_Switch = 0;
				USER_SystemFlag.A.Flag_ST = 0;
				SysParameter.DynamoStart_Flag = 0;   				 //返回待机状态不判断硬件停机与否
				if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
				{
							W5500_Send_Data(2,con_TranT0);									 //上传检测完成
							if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
							{									
// 										FunW5500_SendLED(0,Left_BrakeAve,Right_BrakeAve);//上传阻滞测试结果
							}
							if(USER_SystemFlag.A.Flag_AutoD == 1)
							{
									USER_SystemFlag.A.Flag_AutoD = 0;
									W5500_Send_Data(2,u8Tran_CurrentZ);				//发送当前阻滞过程数据
							}						
				}
				else
				{
							if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
							{						
									 FunW5500_SendGB(0,Left_BrakeAve,Right_BrakeAve); //显示阻滞测试结果
									 Delay_ms(20);
									 W5500_Send_Data(0,con_TranT0);									 //上传检测完成
							}
				} 
				System_State = ReadData_State;          //进入取数状态
		}
		else  //进入空载制动测试
		{
				SysParameter.DynamoStart_Flag = 1;   		//返回待机状态不判断硬件停机与否
				System_State = BrakeTest_None_State;          //进入空载制动测试
				Fun_TestZC();
		}
}
/*******************************************************************************
* 函数名  : Fun_TestZC()
* 描述    : 阻滞+空载制动测试
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_TestZC(void)
{
		uint8_t Countdown_time;																						//倒计时初始值
		uint8_t u8Left_Stopflg=0,        																		//左停机标志
							  u8Right_Stopflg=0, 																				//右停机标志
								u8Left_Judgeflg=0,																				//左停机判断标志
								u8Right_Judgeflg=0;																			//右停机判断标志
	 uint16_t	 u16Left_Number=0,																			//左停机下降点数
								 u16Right_Number=0;																		//右停机下降点数

		uint16_t u16Count800_Left=0, 															    //800个点存储计数左边
								  u16Count800_Right=0;															  //800个点存储计数右边
		uint16_t u16countMax=0, 																					//计数最大存储点数量
									u16Brake_LeftBefor=0,
									u16Brake_RightBefor=0,															//前一次制动力数据
									u16Brake_LeftMAX=0,
									u16Brake_RightMAX=0,																//制动力最大值
									u16Brake_Protect20=0,																//制动力软停机开始阀值 20%
									u16Brake_Protect60=0;																//制动力软停机开始阀值 60%
		uint16_t   u16Brake_Stop70=0,																			//制动力立刻停机阈值
									 u16Brake_Stop90=0;                                      //制动力曲线保护阈值
		uint16_t Voltage_Relative;																					//相对电压
		float fl32Left_SumBrake=0,
					 fl32Right_SumBrake=0;																			//制动求和临时变量
									 
	  float fl32Left_SumBrake1=0,
					  fl32Right_SumBrake1=0;																	  //制动求和临时变量		上传显示使用 			
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1;
		
		uint16_t i;
		
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranMB);								   //上传空载制动测试
					Delay_ms(20);
					W5500_Send_Data(2,con_TranM2);								   //上传M2踩
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{
								USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
								USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
								FunW5500_SendLED(0,0,0);//上传制动力
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{													 
							 W5500_Send_Data(0,con_TranM2);								   //上传M2踩
					}
		}  
	  u16Brake_Protect20 = Left_StaticWeight + Right_StaticWeight;
		if(u16Brake_Protect20 < 500) u16Brake_Protect20 = 500;
		u16Brake_Protect60 = u16Brake_Protect20 * 3;
		
		u16Brake_Stop70 = u16Brake_Protect20 * 4;  //80%
		u16Brake_Stop90 = (float)u16Brake_Protect20 * 4.6;  //92%
		
		Left_DynamicBrake = 0;
		Right_DynamicBrake = 0;  																				//动态显示值清零
		Left_DynamicBrake1 = 0;
		Right_DynamicBrake1 = 0;  														
		Countdown_time = SysParameter.Par5_BrakeTestTime + 1; //制动测试时间	
		USER_SystemFlag.A.Flag_1s = 1;															//首次调用上传倒计时第一个数使用
		while(System_State == BrakeTest_None_State)
		{
				W5500_Receive_Deal(); 																			//网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
// 						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
// 						{
// 								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
// 						}
// 						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
// 						{
// 								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
// 						}
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						u8Left_Judgeflg = 3;							//停机下降10个点数采集
						u8Left_Stopflg = 1;
						u8Right_Judgeflg = 3;							//停机下降10个点数采集			
						u8Right_Stopflg = 1;
						//停机类型  停机时刻
						Left_StopFlag = '0';
						Right_StopFlag = '0';  
						Left_StopTime.u16_data = u16Count800_Left;
						Right_StopTime.u16_data = u16Count800_Right;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//硬停机有效
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //左停机信号
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //左停机
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 3;							//停机下降10个点数采集
										//停机类型  停机时刻
										Left_StopFlag = '1';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //右停机信号
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //右停机
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 3;							//停机下降10个点数采集
										//停机类型  停机时刻
										Right_StopFlag = '1';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
// 										Tx_Buffer[0] = 3;  //帧长度 低位
// 										Tx_Buffer[1] = 0;  //帧长度 高位
// 										Tx_Buffer[2] = 'M'; 
// 										Tx_Buffer[3] = '1'; 
// 										Tx_Buffer[4] = Countdown_time;
// // 										W5500_Send_Data(0,Tx_Buffer);
								}
								else
								{
										System_State = Default_State;										
										Left_Machine = IO_CLOSE;													 //左电机停止
										Right_Machine = IO_CLOSE;												 //右电机停止
										Machine_Switch = IO_CLOSE;											 //星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										//停机类型  停机时刻
										Left_StopFlag = '0';
										Right_StopFlag = '0';  
										Left_StopTime.u16_data = u16Count800_Left;
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_500ms == 1) //暂时
				{
							USER_SystemFlag.A.Flag_500ms = 0;			
							/* 左制动计算 */
							fl32Left_SumBrake1 = ADShow_Display(Left_Brake_ID);
							fl32Left_SumBrake1 = fl32Left_SumBrake1*VOLTAGE; //转换成电压值
							if(fl32Left_SumBrake1 > Zero_LeftBrake)
							{
									Voltage_Relative = fl32Left_SumBrake1 - Zero_LeftBrake;
									Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
									Left_DynamicBrake += 5;
									Left_DynamicBrake /= 10;
									Left_DynamicBrake *= 10;   //保留daN
							}
							else 
							{
									Left_DynamicBrake = 0;
							}
							/* 右制动计算 */
							fl32Right_SumBrake1 = ADShow_Display(Right_Brake_ID);//取平均值
							fl32Right_SumBrake1 = fl32Right_SumBrake1*VOLTAGE;//求电压值		
							if(fl32Right_SumBrake1 > Zero_RightBrake)
							{
									Voltage_Relative = fl32Right_SumBrake1 - Zero_RightBrake;
									Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
									Right_DynamicBrake += 5;
									Right_DynamicBrake /= 10;
									Right_DynamicBrake *= 10;  //保留daN
							}
							else 
							{
									Right_DynamicBrake = 0;
							}
							if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
							{
										if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
										{
													USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
													USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
													FunW5500_SendLED(0,Left_DynamicBrake,Right_DynamicBrake);//上传制动力
										}
							}
							else
							{
									if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
									{
											FunW5500_SendGB(0,Left_DynamicBrake,Right_DynamicBrake); //上传制动力
									}
							}
				 }
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;						
						/* 左制动计算 */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //转换成电压值
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //保留daN
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
								if(u16Brake_LeftMAX <= Left_DynamicBrake1)  //最大值用于下降点数判断
								{
										u16Brake_LeftMAX = Left_DynamicBrake1;
										u16Left_Number = 0;
								}
								else
								{
										u16Left_Number ++;
								}
								if((Left_DynamicBrake1 > u16Brake_Stop70)&&(u8Left_Judgeflg < 4))  //立刻停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;
										//停机类型  停机时刻
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}	
						}
						/* 右制动计算 */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//取平均值
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//求电压值		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //保留daN
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
								if(u16Brake_RightMAX <= Right_DynamicBrake1)  //最大值用于下降点数判断
								{
										u16Brake_RightMAX = Right_DynamicBrake1;
										u16Right_Number = 0;
								}
								else
								{
										u16Right_Number ++;
								}
								if((Right_DynamicBrake1 > u16Brake_Stop70)&&(u8Right_Judgeflg < 4))  //立刻停机
								{
										Right_Machine = IO_CLOSE;							//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  	
										//停机类型  停机时刻
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}	
						}							

						//左停机判断
						if(u8Left_Judgeflg == 0)  // <20%判断
						{
								u16Left_Number = 0;
								if(Left_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Left_Judgeflg = 1;												
								}
						}
						else if(u8Left_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Left_Number > con_LDN20)  //停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;   
										//停机类型  停机时刻
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
						else if(u8Left_Judgeflg == 2) // >60%判断
						{										
								if(u16Left_Number > con_LDN60)  //停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;  		
										//停机类型  停机时刻
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						else if(u8Left_Judgeflg == 3)  //硬停机10个下降点数采集
						{
								if(u16Left_Number > con_LDNStop)   //判断下降十个点
								{
										u8Left_Judgeflg = 4;    
								}
						}								
						else if(u8Left_Judgeflg == 4)   //保存停机后整条曲线
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
								Left_Machine = IO_CLOSE;													 //左电机停止
								USER_SystemFlag.A.Flag_Ldynamo = 0;
						}
						
						//右停机判断
						if(u8Right_Judgeflg == 0)  // <20%判断
						{
								u16Right_Number = 0;
								if(Right_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Right_Judgeflg = 1;												
								}
						}
						else if(u8Right_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Right_Number > con_RDN20)  //停机
								{
										Right_Machine = IO_CLOSE;								//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;   
										//停机类型  停机时刻
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
						else if(u8Right_Judgeflg == 2) // >60%判断
						{										
								if(u16Right_Number > con_RDN60)  //停机
								{
										Right_Machine = IO_CLOSE;							//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  		
										//停机类型  停机时刻
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
						else if(u8Right_Judgeflg == 3)  //硬停机10个下降点数采集
						{
								if(u16Right_Number > con_RDNStop)   
								{
										u8Right_Judgeflg = 4;    
								}
						}	
						else if(u8Right_Judgeflg == 4)   //保存停机后整条曲线
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
								Right_Machine = IO_CLOSE;													 //右电机停止
								USER_SystemFlag.A.Flag_Rdynamo = 0;
						}
						if((u8Left_Judgeflg == 10) && (u8Right_Judgeflg == 10))
						{
								Machine_Switch = IO_CLOSE;												//星三角切还关闭
								USER_SystemFlag.A.Flag_Switch = 0;
								System_State = Default_State;
						}
				}
		}	
		USER_SystemFlag.A.Flag_TestC = 0;
		USER_SystemFlag.A.Flag_TestC1 = 0;
		SysParameter.DynamoStart_Flag = 0;   						 //返回待机状态不判断硬件停机与否
		if(u16Count800_Left < u16Count800_Right)       //填补数据左右相同数量
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
// 		DataRank(u16Left_Data,u16Count800_Left); //排序
// 		DataRank(u16Right_Data,u16Count800_Right); //排序
		#ifdef _DEBUG //调试用函数段
		for(i=0;i<u16Count800_Left;i++)
		{
				printf("\r\nu16Left_Data=%u\r\n",u16Left_Data[i]);
		}
		for(i=0;i<u16Count800_Left;i++)
		{
				printf("\r\nu16Right_Data=%u\r\n",u16Right_Data[i]);
		}		
		#endif
		Deal_Data(u16countMax,u8Tran_CurrentC,'C');  //空载制动数据保存到上传数据单元
		Fun_DealCC(u16Count800_Left,u16Count800_Right);
		System_State = ReadData_State; 	   //进入读取数据状态
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranT0);									 //上传检测完成
					if(USER_SystemFlag.A.Flag_AutoD == 1)
					{
							USER_SystemFlag.A.Flag_AutoD = 0;
							W5500_Send_Data(2,u8Tran_CurrentZ);				//发送当前阻滞过程数据
							W5500_Send_Data(2,u8Tran_CurrentC);				//发送当前空载制动过程数据
					}										
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{												 
							 FunW5500_SendGB(0,Left_BrakeSum,Right_BrakeSum);   //上传制动力最终结果
							 Delay_ms(20);
							 W5500_Send_Data(0,con_TranT0);									 //上传检测完成
					}
		}  
}

/*******************************************************************************
* 函数名  : Fun_TestCC()
* 描述    : 空载制动测试
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_TestCC(void)
{
		uint8_t Countdown_time;																						//倒计时初始值
		uint8_t u8Left_Stopflg=0,        																		//左停机标志
							  u8Right_Stopflg=0, 																				//右停机标志
								u8Left_Judgeflg=0,																				//左停机判断标志
								u8Right_Judgeflg=0;																			//右停机判断标志
	 uint16_t	 u16Left_Number=0,																			//左停机下降点数
								 u16Right_Number=0;																		//右停机下降点数

		uint16_t u16Count800_Left=0, 															    //800个点存储计数左边
								  u16Count800_Right=0;															  //800个点存储计数右边
		uint16_t u16countMax=0, 																					//计数最大存储点数量
									u16Brake_LeftBefor=0,
									u16Brake_RightBefor=0,															//前一次制动力数据
									u16Brake_LeftMAX=0,
									u16Brake_RightMAX=0,																//制动力最大值
									u16Brake_Protect20=0,																//制动力软停机开始阀值 20%
									u16Brake_Protect60=0;																//制动力软停机开始阀值 60%
	  uint16_t   u16Brake_Stop70=0,																			//制动力立刻停机阈值
									 u16Brake_Stop90=0;                                      //制动力曲线保护阈值
		uint16_t Voltage_Relative;																					//相对电压
		float fl32Left_SumBrake=0,
					 fl32Right_SumBrake=0;																			//制动求和临时变量
									 
	  float fl32Left_SumBrake1=0,
					  fl32Right_SumBrake1=0;																	  //制动求和临时变量		上传显示使用 		
						
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1;	
		uint16_t i;
		uint8_t u8Protect_CountFlg = 0;    //左电机保护阀值计数
		uint8_t u8Protect_CountFlg2 = 0;  //右电机保护阀值计数

		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranMB);								   //上传空载制动测试
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{
                if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
                {
                    USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
                }
                USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
                USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
                USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯					
								USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
								USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
// 							FunW5500_SendLED(0,0,0);//上传制动力
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{													 
							 W5500_Send_Data(0,con_TranMB);								   //上传空载制动测试
					}
		} 
		/***启动电机****/
		Fun_DynamoStart();
		if(System_State != BrakeTest_None_State)
		{
				return;
		}
		/*****制动准备倒计时*******/		
		Left_DynamicBrake1 = 0;
		Right_DynamicBrake1 = 0;
		Countdown_time = SysParameter.Par3_BrakeReadyTime + 1; //制动测试时间	
		USER_SystemFlag.A.Flag_1s = 1;															//首次调用上传倒计时第一个数使用
		while(System_State == BrakeTest_None_State)
		{
				W5500_Receive_Deal(); 																			//网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
						{
								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						{
								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
						}
						System_State = Default_State;          //进入默认状态 制动待机
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						return;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//硬停机有效
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //左停机信号
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //左停机
										USER_SystemFlag.A.Flag_Ldynamo = 0;
		// 								if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
		// 								{
		// 										W5500_Send_Data(0,con_TranT7); //
		// 								}
										if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
										{
												W5500_Send_Data(2,con_TranT7); //回传左脉冲异常
										}
								}
								
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //右停机信号
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //右停机
										USER_SystemFlag.A.Flag_Rdynamo = 0;
		// 								if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
		// 								{
		// 										W5500_Send_Data(0,con_TranT8); //
		// 								}
										if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
										{
												W5500_Send_Data(2,con_TranT8); //回传车辆不到位信号	
										}
								}
						}
						if((u8Left_Stopflg == 1) && (u8Right_Stopflg == 1))
						{
								Machine_Switch = IO_CLOSE;     							//星三角切换
								USER_SystemFlag.A.Flag_Switch = 0;
								SysParameter.DynamoStart_Flag = 0;       //无用
								System_State = Default_State;    
								if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
								{
										W5500_Send_Data(0,con_TranT7); //
										W5500_Send_Data(0,con_TranT8); //
								}
								if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
								{
// 										W5500_Send_Data(2,con_TranT7); //回传车辆不到位信号	
// 										W5500_Send_Data(2,con_TranT8); //回传车辆不到位信号	
								}
								return;
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
										Tx_Buffer[0] = 3;  //帧长度 低位
										Tx_Buffer[1] = 0;  //帧长度 高位
										Tx_Buffer[2] = 'M'; 
										Tx_Buffer[3] = '1'; 
										Tx_Buffer[4] = Countdown_time;										
										if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
										{
												W5500_Send_Data(2,Tx_Buffer);//
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
														FunW5500_SendLED(0,LED_OFF,Countdown_time);
												}
										}
										else
										{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
															W5500_Send_Data(0,Tx_Buffer);//
												}
										}
								}
								else
								{
										break;		//跳出循环								
								}
						}
				}
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;		
						/* 左制动计算 */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //转换成电压值
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //保留daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						/* 右制动计算 */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//取平均值
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//求电压值		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //保留daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if(Left_DynamicBrake1 > u16Protect_Limit)  //阻滞保护
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3点保护停机
								{
										System_State = ReadData_State;          //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;					//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT1(2,Left_DynamicBrake1); //左轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{                                
                              USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
                              USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
                              FunW5500_SendLED(0,Left_DynamicBrake1,0);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake1); //左轮阻滞大
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake1 > u16Protect_Limit)  //阻滞保护
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX)  //3点保护停机
								{
										System_State = ReadData_State;         //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;						//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT2(2,Right_DynamicBrake1); //右轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																FunW5500_SendLED(0,0,Right_DynamicBrake1);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake1); //右轮阻滞大
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
		/***********踩**************/
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranM2);								   //上传踩
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{

					}					
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{						
							 W5500_Send_Data(0,con_TranM2);								   //上传踩
					}
		}  
		/*****************************************************/
		fl32Left_SumBrake = 0;
		fl32Right_SumBrake = 0; //求和变量清零 临时变量		
	  u16Brake_Protect20 = Left_StaticWeight + Right_StaticWeight;
		if(u16Brake_Protect20 < 500) u16Brake_Protect20 = 500;
		u16Brake_Protect60 = u16Brake_Protect20 * 3;
		
		u16Brake_Stop70 = u16Brake_Protect20 * 4;  //80%
		u16Brake_Stop90 = (float)u16Brake_Protect20 * 4.6;  //92%
		
		Left_DynamicBrake = 0;
		Right_DynamicBrake = 0;  																				//动态显示值清零
		Countdown_time = SysParameter.Par5_BrakeTestTime + 1; //制动测试时间	
		USER_SystemFlag.A.Flag_1s = 1;															//首次调用上传倒计时第一个数使用
		while(System_State == BrakeTest_None_State)
		{
				W5500_Receive_Deal(); 				//网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
// 						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
// 						{
// 								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
// 						}
// 						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
// 						{
// 								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
// 						}
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						u8Left_Judgeflg = 3;							//停机下降10个点数采集
						u8Left_Stopflg = 1;
						u8Right_Judgeflg = 3;							//停机下降10个点数采集			
						u8Right_Stopflg = 1;
						//停机类型  停机时刻
						Left_StopFlag = '0';
						Right_StopFlag = '0';  
						Left_StopTime.u16_data = u16Count800_Left;
						Right_StopTime.u16_data = u16Count800_Right;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//硬停机有效
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //左停机信号
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //左停机
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 3;							//停机下降10个点数采集
										//停机类型  停机时刻
										Left_StopFlag = '1'; 
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //右停机信号
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //右停机
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 3;							//停机下降10个点数采集
										//停机类型  停机时刻
										Right_StopFlag = '1';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
// 										Tx_Buffer[0] = 3;  //帧长度 低位
// 										Tx_Buffer[1] = 0;  //帧长度 高位
// 										Tx_Buffer[2] = 'M'; 
// 										Tx_Buffer[3] = '1'; 
// 										Tx_Buffer[4] = Countdown_time;
// 										W5500_Send_Data(0,Tx_Buffer);
								}
								else
								{
										System_State = Default_State;										
										Left_Machine = IO_CLOSE;													 //左电机停止
										Right_Machine = IO_CLOSE;												 //右电机停止
										Machine_Switch = IO_CLOSE;											 //星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_500ms == 1) //上传制动力
				{
							USER_SystemFlag.A.Flag_500ms = 0;				
							/* 左制动计算 */
							fl32Left_SumBrake1 = ADShow_Display(Left_Brake_ID);
							fl32Left_SumBrake1 = fl32Left_SumBrake1*VOLTAGE; //转换成电压值
							if(fl32Left_SumBrake1 > Zero_LeftBrake)
							{
									Voltage_Relative = fl32Left_SumBrake1 - Zero_LeftBrake;
									Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
									Left_DynamicBrake += 5;
									Left_DynamicBrake /= 10;
									Left_DynamicBrake *= 10;   //保留daN
							}
							else 
							{
									Left_DynamicBrake = 0;
							}
							/* 右制动计算 */
							fl32Right_SumBrake1 = ADShow_Display(Right_Brake_ID);//取平均值
							fl32Right_SumBrake1 = fl32Right_SumBrake1*VOLTAGE;//求电压值		
							if(fl32Right_SumBrake1 > Zero_RightBrake)
							{
									Voltage_Relative = fl32Right_SumBrake1 - Zero_RightBrake;
									Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
									Right_DynamicBrake += 5;
									Right_DynamicBrake /= 10;
									Right_DynamicBrake *= 10;  //保留daN
							}
							else 
							{
									Right_DynamicBrake = 0;
							}
							if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
							{
										if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
										{
													USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
													USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
 	  											FunW5500_SendLED(0,Left_DynamicBrake,Right_DynamicBrake);//上传制动力
										}
							}
							else
							{
									if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
									{
 													FunW5500_SendGB(0,Left_DynamicBrake,Right_DynamicBrake);//上传制动力
									}
							}
				 }					
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;										
						/* 左制动计算 */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //转换成电压值
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //保留daN
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
								if(u16Brake_LeftMAX <= Left_DynamicBrake1)  //最大值用于下降点数判断
								{
										u16Brake_LeftMAX = Left_DynamicBrake1;
										u16Left_Number = 0;
								}
								else
								{
										u16Left_Number ++;
								}
								if((Left_DynamicBrake1 > u16Brake_Stop70)&&(u8Left_Judgeflg < 4))  //立刻停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;
										//停机类型  停机时刻
										Left_StopFlag = '0'; 
										Left_StopTime.u16_data = u16Count800_Left;
								}		
						}
						/* 右制动计算 */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//取平均值
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//求电压值		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //保留daN
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
								if(u16Brake_RightMAX <= Right_DynamicBrake1)  //最大值用于下降点数判断
								{
										u16Brake_RightMAX = Right_DynamicBrake1;
										u16Right_Number = 0;
								}
								else
								{
										u16Right_Number ++;
								}
								if((Right_DynamicBrake1 > u16Brake_Stop70)&&(u8Right_Judgeflg < 4))  //立刻停机
								{
										Right_Machine = IO_CLOSE;							//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  	
										//停机类型  停机时刻
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}	
						}							
						//左停机判断
						if(u8Left_Judgeflg == 0)  // <20%判断
						{
								u16Left_Number = 0;
								if(Left_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Left_Judgeflg = 1;												
								}
						}
						else if(u8Left_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Left_Number > con_LDN20)  //停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;   
										//停机类型  停机时刻
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
						else if(u8Left_Judgeflg == 2) // >60%判断
						{										
								if(u16Left_Number > con_LDN60)  //停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;  		
										//停机类型  停机时刻
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						else if(u8Left_Judgeflg == 3)  //硬停机10个下降点数采集
						{
								if(u16Left_Number > con_LDNStop)   //判断下降十个点
								{
										u8Left_Judgeflg = 4;    
								}
						}								
						else if(u8Left_Judgeflg == 4)   //保存停机后整条曲线
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
								Left_Machine = IO_CLOSE;													 //左电机停止
								USER_SystemFlag.A.Flag_Ldynamo = 0;
						}
						
						//右停机判断
						if(u8Right_Judgeflg == 0)  // <20%判断
						{
								u16Right_Number = 0;
								if(Right_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Right_Judgeflg = 1;												
								}
						}
						else if(u8Right_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Right_Number > con_RDN20)  //停机
								{
										Right_Machine = IO_CLOSE;								//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;   
										//停机类型  停机时刻
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
						else if(u8Right_Judgeflg == 2) // >60%判断
						{										
								if(u16Right_Number > con_RDN60)  //停机
								{
										Right_Machine = IO_CLOSE;							//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  
										//停机类型  停机时刻
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;									
								}
						}
						else if(u8Right_Judgeflg == 3)  //硬停机10个下降点数采集
						{
								if(u16Right_Number > con_RDNStop)   
								{
										u8Right_Judgeflg = 4;    
								}
						}	
						else if(u8Right_Judgeflg == 4)   //保存停机后整条曲线
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
								Right_Machine = IO_CLOSE;													 //右电机停止
						}
						if((u8Left_Judgeflg == 10) && (u8Right_Judgeflg == 10))
						{
								Machine_Switch = IO_CLOSE;												//星三角切还关闭
								USER_SystemFlag.A.Flag_Switch = 0;
								System_State = Default_State;
						}
				}
		}	
		USER_SystemFlag.A.Flag_TestC = 0;
		USER_SystemFlag.A.Flag_TestC1 = 0;
		SysParameter.DynamoStart_Flag = 0;   						 //返回待机状态不判断硬件停机与否
		if(u16Count800_Left < u16Count800_Right)       //填补数据左右相同数量
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
// 		DataRank(u16Left_Data,u16Count800_Left); //排序
// 		DataRank(u16Right_Data,u16Count800_Right); //排序
		#ifdef _DEBUG //调试用函数段
		for(i=0;i<u16Count800_Left;i++)
		{
				printf("\r\nu16Left_Data=%u\r\n",u16Left_Data[i]);
		}
		for(i=0;i<u16Count800_Left;i++)
		{
				printf("\r\nu16Right_Data=%u\r\n",u16Right_Data[i]);
		}		
		#endif
		Deal_Data(u16countMax,u8Tran_CurrentC,'C');  //空载制动数据保存到上传数据单元
		Fun_DealCC(u16Count800_Left,u16Count800_Right);
		System_State = ReadData_State; 	   //进入读取数据状态
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranT0);									 //上传检测完成
					if(USER_SystemFlag.A.Flag_AutoD == 1)
					{
							USER_SystemFlag.A.Flag_AutoD = 0;
							W5500_Send_Data(2,u8Tran_CurrentC);				//发送当前空载制动过程数据
					}					
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{												 
							 FunW5500_SendGB(0,Left_BrakeSum,Right_BrakeSum);   //上传制动力最终结果
							 Delay_ms(20);
							 W5500_Send_Data(0,con_TranT0);									 //上传检测完成
					}
		}  
}

/*******************************************************************************
* 函数名  : Fun_AllWeightTestJJ()
* 描述    : 加载举升检测
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_AllWeightTestJJ(void)
{
		uint8_t Countdown_time;//倒计时初始值
		uint16_t u16Count300=0; 																	 //300个点存储计数
// 		uint16_t u16countMax=0; 																 //计数最大存储点数量
		uint16_t Voltage_Relative;																 //相对电压
		float fl32Left_SumWeight=0.0,													     //重量求和临时变量
					  fl32Left_SumWeight1=0.0;													 //重量求和临时变量 用于显示使用
		uint16_t All_DynamicWeight1;                               //举升重量临时变量
		uint8_t u8Flg1 = 0,																								 //举升状态切换标识
								u8Flg2 = 0;

		TIM2_count = 0;																										 //超时计数
// 		u16countMax = 0;																							   //初始化		
		All_DynamicWeight = 0;  																		   //复合称重台动态显示重量
		AllFull_StaticWeight = 0;                  								     //初始化空载复合轴重结果
		Countdown_time = SysParameter.Par0_WeightTestTime + 1; //倒计时时间
// 		USER_SystemFlag.A.Flag_1s = 1;											   //首次调用上传倒计时第一个数使用
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranMF2);							 //上传MF2加载举升检测
					if(USER_SystemFlag.A.Flag_Standalone == 1)   //单机联网 给数码管发
					{               
                if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
                {
                    USER_LEDFlag.A.LED_Online = 1;  	//联网指示灯
                }
                USER_LEDFlag.A.LED_Test = 1;  			  //检测指示灯
                USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
                USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯
								USER_LEDFlag.A.LED_TJUp = 1;  			  //台架举指示灯
								USER_LEDFlag.A.LED_TJDown = 0;  		  //台架降指示灯						
								USER_LEDFlag.A.LED_15 = 1;  				  //数码管数据是否改变 1变  0不变
								USER_LEDFlag.A.LED_14 = 1;  				  //数码管指示灯是否改变 1变  0不变
								FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//上传复合称重重量
                Delay_ms(10);
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{						
// 							 W5500_Send_Data(0,con_TranMF2);								   //上传MF2加载举升检测
							 Delay_ms(20);
// 							 FunW5500_SendGF(0,All_DynamicWeight); //上传复合称重
               Delay_ms(10);
					}
		}
		while(System_State == StaticWeight_Full_State)
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 1)
				{
						if((USER_SystemFlag.A.Flag_1s == 1) && (u8Flg2 == 1))
						{
								TIM2_1ms = 0;//定时计数清零
								USER_SystemFlag.A.Flag_1s = 0;
								if(Countdown_time > 0) 
								{
										Countdown_time --;
										if(Countdown_time>0)
										{
												Tx_Buffer[0] = 3;  //帧长度 低位
												Tx_Buffer[1] = 0;  //帧长度 高位
												Tx_Buffer[2] = 'M'; 
												Tx_Buffer[3] = '1'; 
												Tx_Buffer[4] = Countdown_time;												
												if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
												{
														W5500_Send_Data(2,Tx_Buffer);//
                            if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
                            {               
                                  if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
                                  {
                                      USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
                                  }
                                  USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
                                  USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
                                  USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯
                                  USER_LEDFlag.A.LED_TJUp = 1;  			//台架举指示灯
                                  USER_LEDFlag.A.LED_TJDown = 0;  		//台架降指示灯						
                                  USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
                                  USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
                                  FunW5500_SendLED(0,LED_OFF,Countdown_time);//上传倒计时
                            }
												}
                        else
                        {
                            if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
                            {
//                                 W5500_Send_Data(0,Tx_Buffer);//
                            }
                        }
										}
										else
										{
												Left_Up = IO_CLOSE;  			  //左台架举关闭
												Left_Down = IO_CLOSE; 		//左台架降关闭
												System_State = Default_State;
										}
								}
						}
						if(USER_SystemFlag.A.Flag_10ms == 1)
						{
								USER_SystemFlag.A.Flag_10ms = 0;							
								/* 复合台称重计算 */
								fl32Left_SumWeight = ADShow_Samp(All_Weight_ID);
								fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE; //转换成电压值
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
								if((All_DynamicWeight1 < SysParameter.Par11_MaxWeight) && (u8Flg1 == 0)) //重量小于加载阀值
								{
										u8Flg1 = 1;
										Left_Up = IO_OPEN;  					//左台架举打开
										Left_Down = IO_CLOSE; 		//左台架降关闭
								}
								else
								{
										if(u8Flg2 == 0)
										{
												//举升到位或者重量达到加载阀值
												if((IO_GetInStatus(IN_Raise_ID) == 1) || (All_DynamicWeight1 >= SysParameter.Par11_MaxWeight))
												{
														if(All_DynamicWeight1 >= SysParameter.Par11_MaxWeight)
														{
																Left_Up = IO_CLOSE;  			  //左台架举关闭
																Left_Down = IO_CLOSE; 		//左台架降关闭
																System_State = Default_State;
																AllFull_StaticWeight = All_DynamicWeight1;
														}
														else
														{
																u8Flg2 = 1;
																USER_SystemFlag.A.Flag_1s = 1;			//首次调用上传倒计时第一个数使用
														}
												}
												if(TIM2_count > (SysParameter.Par14_UpTime*1000))//超时后停止举升
												{
														u8Flg2 = 1;
														USER_SystemFlag.A.Flag_1s = 1;		  //首次调用上传倒计时第一个数使用
														Left_Up = IO_CLOSE;  			  //左台架举关闭
														Left_Down = IO_CLOSE; 		//左台架降关闭
														if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
														{
// 																		W5500_Send_Data(0,con_TranT3);		 //上传举升超时 不到位
														}
														if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
														{
																W5500_Send_Data(2,con_TranT3);		 //上传举升超时 不到位
														}																
												}
										}
								}																		
						}
						if((USER_SystemFlag.A.Flag_500ms == 1) && (u8Flg2 == 0))
						{
									USER_SystemFlag.A.Flag_500ms = 0;
									/* 复合台称重计算 */
									fl32Left_SumWeight1 = ADShow_Display(All_Weight_ID);
									fl32Left_SumWeight1 = fl32Left_SumWeight1*VOLTAGE; //转换成电压值
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
									if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
									{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
                              if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
                              {
                                  USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
                              }
                              USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
                              USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
                              USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯
															USER_LEDFlag.A.LED_TJUp = 1;  					//台架举指示灯
															USER_LEDFlag.A.LED_TJDown = 0;  		//台架降指示灯
															USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
															USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
	  													FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//上传复合称重重量
												}
									}
									else
									{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
// 														 FunW5500_SendGF(0,All_DynamicWeight); //上传复合称重
												}
									}
						}
				}
				else
				{
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
                /* 复合台称重计算 */
                fl32Left_SumWeight1 = ADShow_Display(All_Weight_ID);
                fl32Left_SumWeight1 = fl32Left_SumWeight1*VOLTAGE; //转换成电压值
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
                if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
                {
                    if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
                    {
                          if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
                          {
                              USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
                          }
                          USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
                          USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
                          USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯
                          USER_LEDFlag.A.LED_TJUp = 1;  					//台架举指示灯
                          USER_LEDFlag.A.LED_TJDown = 0;  		//台架降指示灯
                          USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
                          USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
                          FunW5500_SendLED(0,LED_OFF,All_DynamicWeight);//上传复合称重重量
                    }
										
                }
                else
                {
                    if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
                    {
                          W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号
                    }					
                }
						}						
						Countdown_time = SysParameter.Par0_WeightTestTime + 1; //倒计时时间
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 1;//首次调用上传倒计时第一个数使用
						u16Count300 = 0;
// 						u16countMax = 0;//初始化
						fl32Left_SumWeight1 =0;
						TIM2_count = 0;
						u8Flg1 = 0;
						u8Flg2 = 0;
						Left_Up = IO_CLOSE;  			//左台架举关闭
						Left_Down = IO_OPEN; 		//左台架降打开
				}
		}				
// 		DataRank(u16Left1_TemporaryWeight,u16countMax); //排序
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
	  #ifdef _DEBUG //调试用函数段
		printf("\r\nAllFull_StaticWeight=%u\r\n",AllFull_StaticWeight);
		#endif	
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		 {
						W5500_Send_Data(2,con_TranTF); 	//加载举升完成
						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
						{                  
									USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
									USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
	  							FunW5500_SendLED(0,LED_OFF,AllFull_StaticWeight);//上传复合称重重量
						}
						if(USER_SystemFlag.A.Flag_AutoD == 1)
						{
								USER_SystemFlag.A.Flag_AutoD = 0;
								FunW5500_SendDF(2,2,AllFull_StaticWeight);
						}
		 }
		 else
		 {
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{
								FunW5500_SendGF(0,AllFull_StaticWeight);
								Delay_ms(20);
// 								W5500_Send_Data(0,con_TranTF); 	//加载举升完成
					}
		 }				
		System_State = ReadData_State; 					//上边倒计时时间结束也有这条状态切换
}


/*******************************************************************************
* 函数名  : Fun_TestJJ()
* 描述    : 加载制动测试
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_TestJJ(void)
{
		uint8_t Countdown_time;																						//倒计时初始值
		uint8_t u8Left_Stopflg=0,        																		//左停机标志
							  u8Right_Stopflg=0, 																				//右停机标志
								u8Left_Judgeflg=0,																				//左停机判断标志
								u8Right_Judgeflg=0;																			//右停机判断标志
	 uint16_t	 u16Left_Number=0,																			//左停机下降点数
								 u16Right_Number=0;																		//右停机下降点数

		uint16_t u16Count800_Left=0, 															    //800个点存储计数左边
								  u16Count800_Right=0;															  //800个点存储计数右边
		uint16_t u16countMax=0, 																					//计数最大存储点数量
									u16Brake_LeftBefor=0,
									u16Brake_RightBefor=0,															//前一次制动力数据
									u16Brake_LeftMAX=0,
									u16Brake_RightMAX=0,																//制动力最大值
									u16Brake_Protect20=0,																//制动力软停机开始阀值 20%
									u16Brake_Protect60=0;																//制动力软停机开始阀值 60%
		uint16_t   u16Brake_Stop70=0,																			//制动力立刻停机阈值
									 u16Brake_Stop90=0;                                      //制动力曲线保护阈值
		uint16_t Voltage_Relative;																					//相对电压
		float fl32Left_SumBrake=0,
					 fl32Right_SumBrake=0;																			//制动求和临时变量
									 
		float fl32Left_SumBrake1=0,
					  fl32Right_SumBrake1=0;																	  //制动求和临时变量		上传显示使用 			
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1;							 
									 
		uint16_t i;
		uint8_t u8Protect_CountFlg = 0;    //左电机保护阀值计数
		uint8_t u8Protect_CountFlg2 = 0;  //右电机保护阀值计数

		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranMJ);													//上传加载制动检测
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{
                if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
                {
                    USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
                }
                USER_LEDFlag.A.LED_Test = 1;  			  //检测指示灯
                USER_LEDFlag.A.LED_Weight = 0;  			//重量指示灯
                USER_LEDFlag.A.LED_Brake = 1;  			  //制动指示灯					
								USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
								USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
// 								FunW5500_SendLED(0,0,0);//上传制动力
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{													 
							 W5500_Send_Data(0,con_TranMJ);													//上传加载制动检测
					}
		} 
		/***启动电机****/
		Fun_DynamoStart();
		if(System_State != BrakeTest_Full_State)
		{
				return;
		}
		/*****制动准备倒计时*******/	
		Left_DynamicBrake1 = 0;
		Right_DynamicBrake1 = 0;
		Countdown_time = SysParameter.Par3_BrakeReadyTime + 1; //制动准备时间	
		USER_SystemFlag.A.Flag_1s = 1;															//首次调用上传倒计时第一个数使用
		while(System_State == BrakeTest_Full_State)
		{
				W5500_Receive_Deal(); 							//网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
						{
								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
						}
						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						{
								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
						}
						System_State = Default_State;          //进入默认状态 制动待机
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						return;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//硬停机有效
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //左停机信号
						{
								if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //左停机
										USER_SystemFlag.A.Flag_Ldynamo = 0;
		// 								if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
		// 								{
		// 										W5500_Send_Data(0,con_TranT7); //
		// 								}
										if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
										{
												W5500_Send_Data(2,con_TranT7); //回传车辆不到位信号	
										}
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //右停机信号
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //右停机
										USER_SystemFlag.A.Flag_Rdynamo = 0;
		// 								if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
		// 								{
		// 										W5500_Send_Data(0,con_TranT8); //
		// 								}
										if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
										{
												W5500_Send_Data(2,con_TranT8); //回传车辆不到位信号	
										}
								}
						}
						if((u8Left_Stopflg == 1) && (u8Right_Stopflg == 1))
						{
								Machine_Switch = IO_CLOSE;     							//星三角切换
								USER_SystemFlag.A.Flag_Switch = 0;
								SysParameter.DynamoStart_Flag = 0;       //无用
								System_State = Default_State;     
								if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
								{
										W5500_Send_Data(0,con_TranT7); //
										W5500_Send_Data(0,con_TranT8); //
								}
								if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
								{
// 										W5500_Send_Data(2,con_TranT7); //回传车辆不到位信号	
// 										W5500_Send_Data(2,con_TranT8); //回传车辆不到位信号	
								}
								return;
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
										Tx_Buffer[0] = 3;  //帧长度 低位
										Tx_Buffer[1] = 0;  //帧长度 高位
										Tx_Buffer[2] = 'M'; 
										Tx_Buffer[3] = '1'; 
										Tx_Buffer[4] = Countdown_time;									
										if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
										{
												W5500_Send_Data(2,Tx_Buffer);//
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
														USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
														USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
														FunW5500_SendLED(0,LED_OFF,Countdown_time);
												}
										}
										else
										{
												if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
												{
															W5500_Send_Data(0,Tx_Buffer);//
												}
										}
								}
								else
								{
										break;		//跳出循环								
								}
						}
				}
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;		
					  /* 左制动计算 */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //转换成电压值
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //保留daN
						}
						else 
						{
								Left_DynamicBrake1 = 0;
						}
						/* 右制动计算 */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//取平均值
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//求电压值		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //保留daN
						}
						else 
						{
								Right_DynamicBrake1 = 0;
						}
						if(Left_DynamicBrake1 > u16Protect_Limit)  //阻滞保护
						{
								u8Protect_CountFlg ++;
								if(u8Protect_CountFlg >= PROTECT_MAX)  //3点保护停机
								{
										System_State = ReadData_State;          //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;					//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT1(2,Left_DynamicBrake1); //左轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																FunW5500_SendLED(0,Left_DynamicBrake1,0);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT1(0,Left_DynamicBrake1); //左轮阻滞大
													}
										}
										return;
								}
						}	
						else
						{
								u8Protect_CountFlg = 0;
						}
						if(Right_DynamicBrake1 > u16Protect_Limit)  //阻滞保护
						{
								u8Protect_CountFlg2 ++;
								if(u8Protect_CountFlg2 >= PROTECT_MAX)  //3点保护停机
								{
										System_State = ReadData_State;         //进入默认状态 制动待机
										Speed_Control = IO_CLOSE; 						//测速板停止
										Left_Machine = IO_CLOSE;							//左电机停止
										Right_Machine = IO_CLOSE;						//右电机停止
										Machine_Switch = IO_CLOSE;						//星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
										{
													FunW5500_SendT2(2,Right_DynamicBrake1); //右轮阻滞大
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{
																FunW5500_SendLED(0,0,Right_DynamicBrake1);//上传阻滞力
													}
										}
										else
										{
													if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
													{						
															 FunW5500_SendT2(0,Right_DynamicBrake1); //右轮阻滞大
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
		/***********踩**************/
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranM2);								   //上传踩
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{

					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{						
							 W5500_Send_Data(0,con_TranM2);								   //上传踩
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
		Right_DynamicBrake = 0;  																				//动态显示值清零
		Countdown_time = SysParameter.Par5_BrakeTestTime + 1; //制动测试时间	
		USER_SystemFlag.A.Flag_1s = 1;															//首次调用上传倒计时第一个数使用
		while(System_State == BrakeTest_Full_State)
		{
				W5500_Receive_Deal(); 																			//网络通信查询处理
				if(IO_GetInStatus(IN_Brake_ID) == 0) //判断到位信号是否到位
				{
// 						if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
// 						{
// 								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号	
// 						}
// 						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
// 						{
// 								W5500_Send_Data(2,con_TranT4); //回传车辆不到位信号	
// 						}
						Speed_Control = IO_CLOSE; 						//测速板停止
						Left_Machine = IO_CLOSE;							//左电机停止
						Right_Machine = IO_CLOSE;						//右电机停止
						Machine_Switch = IO_CLOSE;					//星三角
						USER_SystemFlag.A.Flag_Ldynamo = 0;
						USER_SystemFlag.A.Flag_Rdynamo = 0;
						USER_SystemFlag.A.Flag_Switch = 0;
						USER_SystemFlag.A.Flag_ST = 0;
						SysParameter.DynamoStart_Flag = 0;
						u8Left_Judgeflg = 3;							//停机下降10个点数采集
						u8Left_Stopflg = 1;
						u8Right_Judgeflg = 3;							//停机下降10个点数采集			
						u8Right_Stopflg = 1;
						//停机类型  停机时刻
						Left_StopFlag = '0';
						Right_StopFlag = '0';  
						Left_StopTime.u16_data = u16Count800_Left;
						Right_StopTime.u16_data = u16Count800_Right;
				}
				if(SysParameter.Par1_RollerFlag == 1)									//硬停机有效
				{
						if((IO_GetInStatus(IN_LeftStop_ID) == 1) && (u8Left_Stopflg == 0)) //左停机信号
						{		
							  if(Left_DynamicBrake1>u16Protect_Limit)
								{
										u8Left_Stopflg = 1;											
										Left_Machine = IO_CLOSE;   //左停机
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 3;							//停机下降10个点数采集
										//停机类型  停机时刻
										Left_StopFlag = '1';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						if((IO_GetInStatus(IN_RightStop_ID) == 1) && (u8Right_Stopflg == 0)) //右停机信号
						{
								if(Right_DynamicBrake1>u16Protect_Limit)
								{
										u8Right_Stopflg = 1;
										Right_Machine = IO_CLOSE;   //右停机								
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 3;							//停机下降10个点数采集
										//停机类型  停机时刻
										Right_StopFlag = '1';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_1s == 1)
				{
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 0;
						if(Countdown_time > 0) 
						{
								Countdown_time --;
								if(Countdown_time>0)
								{
// 										Tx_Buffer[0] = 3;  //帧长度 低位
// 										Tx_Buffer[1] = 0;  //帧长度 高位
// 										Tx_Buffer[2] = 'M'; 
// 										Tx_Buffer[3] = '1'; 
// 										Tx_Buffer[4] = Countdown_time;
// 										W5500_Send_Data(0,Tx_Buffer);
								}
								else
								{
										System_State = Default_State;										
										Left_Machine = IO_CLOSE;													 //左电机停止
										Right_Machine = IO_CLOSE;												 //右电机停止
										Machine_Switch = IO_CLOSE;											 //星三角
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										USER_SystemFlag.A.Flag_Switch = 0;
										USER_SystemFlag.A.Flag_ST = 0;
										//停机类型  停机时刻
										Left_StopFlag = '0';
										Right_StopFlag = '0';  
										Left_StopTime.u16_data = u16Count800_Left;
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
				}
				if(USER_SystemFlag.A.Flag_500ms == 1) //上传制动力
				{
							USER_SystemFlag.A.Flag_500ms = 0;	
							/* 左制动计算 */
							fl32Left_SumBrake1 = ADShow_Display(Left_Brake_ID);
							fl32Left_SumBrake1 = fl32Left_SumBrake1*VOLTAGE; //转换成电压值
							if(fl32Left_SumBrake1 > Zero_LeftBrake)
							{
									Voltage_Relative = fl32Left_SumBrake1 - Zero_LeftBrake;
									Left_DynamicBrake = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
									Left_DynamicBrake += 5;
									Left_DynamicBrake /= 10;
									Left_DynamicBrake *= 10;   //保留daN
							}
							else 
							{
									Left_DynamicBrake = 0;
							}
							/* 右制动计算 */
							fl32Right_SumBrake1 = ADShow_Display(Right_Brake_ID);//取平均值
							fl32Right_SumBrake1 = fl32Right_SumBrake1*VOLTAGE;//求电压值		
							if(fl32Right_SumBrake1 > Zero_RightBrake)
							{
									Voltage_Relative = fl32Right_SumBrake1 - Zero_RightBrake;
									Right_DynamicBrake = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
									Right_DynamicBrake += 5;
									Right_DynamicBrake /= 10;
									Right_DynamicBrake *= 10;  //保留daN
							}
							else 
							{
									Right_DynamicBrake = 0;
							}
							if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
							{
										if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
										{
													USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
													USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
 	  											FunW5500_SendLED(0,Left_DynamicBrake,Right_DynamicBrake);//上传制动力
										}
							}
							else
							{
									if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
									{
 													FunW5500_SendGB(0,Left_DynamicBrake,Right_DynamicBrake);//上传制动力
									}
							}
				 }
				if(USER_SystemFlag.A.Flag_10ms == 1)
				{
						USER_SystemFlag.A.Flag_10ms = 0;					
						/* 左制动计算 */
						fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
						fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //转换成电压值
						if(fl32Left_SumBrake > Zero_LeftBrake)
						{
								Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
								Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
								Left_DynamicBrake1 += 5;
								Left_DynamicBrake1 /= 10;
								Left_DynamicBrake1 *= 10;   //保留daN
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
								if(u16Brake_LeftMAX <= Left_DynamicBrake1)  //最大值用于下降点数判断
								{
										u16Brake_LeftMAX = Left_DynamicBrake1;
										u16Left_Number = 0;
								}
								else
								{
										u16Left_Number ++;
								}
								if((Left_DynamicBrake1 > u16Brake_Stop70)&&(u8Left_Judgeflg < 4))  //立刻停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;
										//停机类型  停机时刻
										Left_StopFlag = '0'; 
										Left_StopTime.u16_data = u16Count800_Left;
								}	
						}
						/* 右制动计算 */
						fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//取平均值
						fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//求电压值		
						if(fl32Right_SumBrake > Zero_RightBrake)
						{
								Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
								Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
								Right_DynamicBrake1 += 5;
								Right_DynamicBrake1 /= 10;
								Right_DynamicBrake1 *= 10;  //保留daN
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
								if(u16Brake_RightMAX <= Right_DynamicBrake1)  //最大值用于下降点数判断
								{
										u16Brake_RightMAX = Right_DynamicBrake1;
										u16Right_Number = 0;
								}
								else
								{
										u16Right_Number ++;
								}
								if((Right_DynamicBrake1 > u16Brake_Stop70)&&(u8Right_Judgeflg < 4))  //立刻停机
								{
										Right_Machine = IO_CLOSE;							//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;  	
										//停机类型  停机时刻
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
							}	
						}							

						//左停机判断
						if(u8Left_Judgeflg == 0)  // <20%判断
						{
								u16Left_Number = 0;
								if(Left_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Left_Judgeflg = 1;												
								}
						}
						else if(u8Left_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Left_Number > con_LDN20)  //停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;
										u8Left_Judgeflg = 4;   
										//停机类型  停机时刻
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
						else if(u8Left_Judgeflg == 2) // >60%判断
						{										
								if(u16Left_Number > con_LDN60)  //停机
								{
										Left_Machine = IO_CLOSE;			//左电机停止
										USER_SystemFlag.A.Flag_Ldynamo = 0;						
										u8Left_Judgeflg = 4;  		
										//停机类型  停机时刻
										Left_StopFlag = '0';
										Left_StopTime.u16_data = u16Count800_Left;
								}
						}
						else if(u8Left_Judgeflg == 3)  //硬停机10个下降点数采集
						{
								if(u16Left_Number > con_LDNStop)   //判断下降十个点
								{
										u8Left_Judgeflg = 4;    
								}
						}								
						else if(u8Left_Judgeflg == 4)   //保存停机后整条曲线
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
								Left_Machine = IO_CLOSE;													 //左电机停止
								USER_SystemFlag.A.Flag_Ldynamo = 0;
						}
						
						//右停机判断
						if(u8Right_Judgeflg == 0)  // <20%判断
						{
								u16Right_Number = 0;
								if(Right_DynamicBrake1 > u16Brake_Protect20)
								{
										u8Right_Judgeflg = 1;												
								}
						}
						else if(u8Right_Judgeflg == 1) // 20%< <60%
						{										
								if(u16Right_Number > con_RDN20)  //停机
								{
										Right_Machine = IO_CLOSE;								//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;
										u8Right_Judgeflg = 4;   
										//停机类型  停机时刻
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
						else if(u8Right_Judgeflg == 2) // >60%判断
						{										
								if(u16Right_Number > con_RDN60)  //停机
								{
										Right_Machine = IO_CLOSE;							//右电机停止
										USER_SystemFlag.A.Flag_Rdynamo = 0;									
										u8Right_Judgeflg = 4;  		
										//停机类型  停机时刻
										Right_StopFlag = '0';  
										Right_StopTime.u16_data = u16Count800_Right;
								}
						}
						else if(u8Right_Judgeflg == 3)  //硬停机10个下降点数采集
						{
								if(u16Right_Number > con_RDNStop)   
								{
										u8Right_Judgeflg = 4;    
								}
						}	
						else if(u8Right_Judgeflg == 4)   //保存停机后整条曲线
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
								Right_Machine = IO_CLOSE;													 //右电机停止							
								USER_SystemFlag.A.Flag_Rdynamo = 0;
						}
						if((u8Left_Judgeflg == 10) && (u8Right_Judgeflg == 10))
						{
								Machine_Switch = IO_CLOSE;												//星三角切还关闭
								USER_SystemFlag.A.Flag_Switch = 0;
								System_State = Default_State;
						}
				}
		}	
		USER_SystemFlag.A.Flag_TestC = 0;
		USER_SystemFlag.A.Flag_TestC1 = 0;
		SysParameter.DynamoStart_Flag = 0;   						 //返回待机状态不判断硬件停机与否
		if(u16Count800_Left < u16Count800_Right)       //填补数据左右相同数量
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
		Deal_Data(u16countMax,u8Tran_CurrentJ,'J');  //加载制动数据保存到上传数据单元
		Fun_DealJJ(u16Count800_Left,u16Count800_Right);
		System_State = ReadData_State; 	   //进入读取数据状态
		if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
		{
					W5500_Send_Data(2,con_TranT0);									 //上传检测完成
					if(USER_SystemFlag.A.Flag_AutoD == 1)
					{
							USER_SystemFlag.A.Flag_AutoD = 0;
							W5500_Send_Data(2,u8Tran_CurrentJ);				//发送当前加载制动过程数据
					}
		}
		else
		{
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{												 
							 FunW5500_SendGB(0,Left_BrakeSumJJ,Right_BrakeSumJJ);   //上传制动力最终结果
							 Delay_ms(20);
							 W5500_Send_Data(0,con_TranT0);									 //上传检测完成
					}
		}  
}


/*******************************************************************************
* 函数名  : Deal_Data()
* 描述    : 制动过程数据处理
* 输入    :  Number:保存的最大数据,Data:数据存储数组,Command:字命令
* 输出    : 无
* 返回值  :无
* 说明    : 
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
		 *Data++ = 10;                      //采样周期
		 for(i=0;i<Number;i++)
		 {
					aaa.u16_Data = u16Left_Data[i]; //左边数据
				 *Data++ = aaa.u8_Data[0];
				 *Data++ = aaa.u8_Data[1];
			 
					aaa.u16_Data = u16Right_Data[i]; //右边数据
				 *Data++ = aaa.u8_Data[0];
				 *Data++ = aaa.u8_Data[1];
		 }
}
/*******************************************************************************
* 函数名  : FunW5500_SendGW()
* 描述    : 上传轮重动态数据
* 输入    :  s:端口,  L_Data:左重数据,R_Data:右重数据
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendGW(uint8_t s,uint16_t L_Data,uint16_t R_Data)
{
		Tx_Buffer[0] = 14;  //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'G'; 
		Tx_Buffer[3] = 'W'; 
		/* 左重 */
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
		/* 右重 */
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
		W5500_Send_Data(s,Tx_Buffer);				//发送当前轮重
}
/*******************************************************************************
* 函数名  : FunW5500_SendGB()
* 描述    : 上传制动动态数据
* 输入    :  s:端口,  L_Data:左制数据,R_Data:右制动力
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendGB(uint8_t s,uint16_t L_Data,uint16_t R_Data)
{
		Tx_Buffer[0] = 14;  //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'G'; 
		Tx_Buffer[3] = 'B'; 
		/* 左重 */
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
		/* 右重 */
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
		W5500_Send_Data(s,Tx_Buffer);				//发送当前轮重
}
/*******************************************************************************
* 函数名  : FunW5500_SendGF()
* 描述    : 上传复合重量动态数据
* 输入    :  s:端口,  ALL_Data:复合重量
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendGF(uint8_t s,uint16_t ALL_Data)
{
		Tx_Buffer[0] = 8;  //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'G'; 
		Tx_Buffer[3] = 'F'; 
		/* 复合重量 */
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
		W5500_Send_Data(s,Tx_Buffer);				//发送当前轮重
}
/*******************************************************************************
* 函数名  : FunW5500_SendDF()
* 描述    : 上传复合重量动态数据
* 输入    :  s:端口,  ALL_Data:复合重量
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendDF(uint8_t s,uint8_t a,uint16_t ALL_Data)
{
		Tx_Buffer[0] = 8;  //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'D'; 
		Tx_Buffer[3] = 'F'; 
		Tx_Buffer[4] = a+0x30;
		/* 复合重量 */
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
		W5500_Send_Data(s,Tx_Buffer);				//发送当前轮重
}
/*******************************************************************************
* 函数名  : FunW5500_SendLED()
* 描述    : 联网时上传数码管显示
* 输入    :  s:端口,  L_Data:数码管,R_Data:右数码管
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendLED(uint8_t s,uint16_t L_Data,uint16_t R_Data)
{
		_union_u16_u8_type u16Data;
		Tx_Buffer[0] = 8;    //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'N'; 
		Tx_Buffer[3] = 'H'; 
		/* 左LED */
	  u16Data.u16_data = L_Data;
		Tx_Buffer[4] = u16Data.u8_data[0];
		Tx_Buffer[5] = u16Data.u8_data[1];
		/* 右LED */
		u16Data.u16_data = R_Data;
		Tx_Buffer[6] = u16Data.u8_data[0];
		Tx_Buffer[7] = u16Data.u8_data[1];
	  /* 指示灯 */
		 u16Data.u16_data = USER_LEDFlag.Init;
		Tx_Buffer[8] = u16Data.u8_data[0];
		Tx_Buffer[9] = u16Data.u8_data[1];
		W5500_Send_Data(s,Tx_Buffer);				//发送当前轮重
}
/*******************************************************************************
* 函数名  : FunW5500_SendDW()
* 描述    : 上传轮重结果数据
* 输入    :  s:端口
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendDW(uint8_t s)
{
		uint8_t i;
		Tx_Buffer[0] = DynamicWeight_Number*12+3;  //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'D'; 
		Tx_Buffer[3] = 'W'; 
		Tx_Buffer[4] = DynamicWeight_Number;
		for(i=0;i<DynamicWeight_Number;i++)
		{
					/* 左重 */
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
					/* 右重 */
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
		W5500_Send_Data(s,Tx_Buffer);				//发送当前轮重
}
/*******************************************************************************
* 函数名  : FunW5500_SendT1()
* 描述    : 上传左阻滞测试失败
* 输入    :  s:端口,  ALL_Data:阻滞
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendT1(uint8_t s,uint16_t ALL_Data)
{
		Tx_Buffer[0] = 8;  //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'T'; 
		Tx_Buffer[3] = '1'; 
		/* 左阻滞 */
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
		W5500_Send_Data(s,Tx_Buffer);				//发送当前轮重
}
/*******************************************************************************
* 函数名  : FunW5500_SendT2()
* 描述    : 上传右阻滞测试失败
* 输入    :  s:端口,  ALL_Data:阻滞
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendT2(uint8_t s,uint16_t ALL_Data)
{
		Tx_Buffer[0] = 8;  //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'T'; 
		Tx_Buffer[3] = '2'; 
		/* 右阻滞 */
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
		W5500_Send_Data(s,Tx_Buffer);				//发送当前轮重
}
/*******************************************************************************
* 函数名  : Fun_ReadData()
* 描述    : 读取数据函数
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_ReadData(void)
{
			uint8_t u8TimeCount_1s = 0;
			TIM2_1ms = 0;//定时计数清零
			USER_SystemFlag.A.Flag_1s = 0;
			while(System_State == ReadData_State)
			{
					W5500_Receive_Deal(); //网络通信查询处理
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
* 函数名  : Fun_DealZZ()
* 描述    : 阻滞数据处理函数
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_DealZZ(uint16_t Number)
{
		float fl32Left_Sum = 0,
					 fl32Right_Sum = 0; //和
	  uint16_t i_count;  //计数使用
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
* 函数名  : Fun_DealCC()
* 描述    : 空载制动数据处理函数
* 输入    :  Left_Number:左制动力数据数量  Right_Number:右制动力数据数量
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_DealCC(uint16_t Left_Number,uint16_t Right_Number)
{
		float fl32Left_Brake = 0,
					  fl32Right_Brake = 0;
	  uint16_t i,j,MaxNumber,MinNumber;
		uint16_t temp = 0;
		/****左边数据平滑*****/
		if( Left_Number >= 4 )  //4点平滑
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
						 u16Left_Data[Left_Number - 4 + i] = u16Left_Data[Left_Number - 4 + i]*10;		//使显示结果和上传一致，最后一位保持为零
				}
// 				Left_Number -= 3;
		}
    /****右边数据平滑*****/
		if( Right_Number >= 4 )  //4点平滑
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
						 u16Right_Data[Right_Number - 4 + i] = u16Right_Data[Right_Number - 4 + i]*10;		//使显示结果和上传一致，最后一位保持为零
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
		/*********制和***************/
		Left_BrakeSum = 0;          //左制动力最大值  空载制动
		Right_BrakeSum = 0;         //右制动力最大值  空载制动
		for(i = 0;i < MaxNumber;i++)
		{
				 if(Left_BrakeSum < u16Left_Data[i])  Left_BrakeSum = u16Left_Data[i];
				 if(Right_BrakeSum < u16Right_Data[i])  Right_BrakeSum = u16Right_Data[i];
		}
		/*********制差***************/
		Left_BrakeSub = 0;          //左制差最大值  空载制动
		Right_BrakeSub = 0;         //右制差最大值  空载制动
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
* 函数名  : Fun_DealJJ()
* 描述    : 加载制动数据处理函数
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void Fun_DealJJ(uint16_t Left_Number,uint16_t Right_Number)
{
		float fl32Left_Brake = 0,
					  fl32Right_Brake = 0;
	  uint16_t i,j,MaxNumber,MinNumber;
		uint16_t temp = 0;
		/****左边数据平滑*****/
		if( Left_Number >= 4 )  //4点平滑
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
						 u16Left_Data[Left_Number - 4 + i] = u16Left_Data[Left_Number - 4 + i]*10;		//使显示结果和上传一致，最后一位保持为零
				}
// 				Left_Number -= 3;
		}
    /****右边数据平滑*****/
		if( Right_Number >= 4 )  //4点平滑
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
						 u16Right_Data[Right_Number - 4 + i] = u16Right_Data[Right_Number - 4 + i]*10;		//使显示结果和上传一致，最后一位保持为零
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
		/*********制和***************/
		Left_BrakeSumJJ = 0;          //左制动力最大值  空载制动
		Right_BrakeSumJJ = 0;         //右制动力最大值  空载制动
		for(i = 0;i < MaxNumber;i++)
		{
				 if(Left_BrakeSumJJ < u16Left_Data[i])  Left_BrakeSumJJ = u16Left_Data[i];
				 if(Right_BrakeSumJJ < u16Right_Data[i])  Right_BrakeSumJJ = u16Right_Data[i];
		}
		/*********制差***************/
		Left_BrakeSubJJ = 0;          //左制差最大值  空载制动
		Right_BrakeSubJJ = 0;         //右制差最大值  空载制动
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
* 函数名  : Fun_DealWeight()
* 描述    : 处理重量分辨率函数
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
uint16_t Fun_DealWeight(uint16_t Weight)
{
		uint8_t Remainder;    //余数
    uint16_t Quotient;      //商
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
* 函数名  : Fun_DealDCData()
* 描述    : 上传单机测试结果
* 输入    :  
* 输出    : 无
* 返回值  :无
* 说明    : 
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
* 函数名  : FunW5500_SendDC()
* 描述    : 上传单机测试结果
* 输入    :  s:端口
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_SendDC(uint8_t s)
{
		Tx_Buffer[0] = 62;  //帧长度 低位
		Tx_Buffer[1] = 0;    //帧长度 高位
		Tx_Buffer[2] = 'D'; 
		Tx_Buffer[3] = 'D'; 
		/*左阻滞*/
		Tx_Buffer[4] = Fun_DealDCData(5,Left_BrakeAve);
		Tx_Buffer[5] = Fun_DealDCData(4,Left_BrakeAve);
		Tx_Buffer[6] = Fun_DealDCData(3,Left_BrakeAve);
		Tx_Buffer[7] = Fun_DealDCData(2,Left_BrakeAve);
		Tx_Buffer[8] = Fun_DealDCData(1,Left_BrakeAve);
		Tx_Buffer[9] = 0x20;
		/*右阻滞*/
		Tx_Buffer[10] = Fun_DealDCData(5,Right_BrakeAve);
		Tx_Buffer[11] = Fun_DealDCData(4,Right_BrakeAve);
		Tx_Buffer[12] = Fun_DealDCData(3,Right_BrakeAve);
		Tx_Buffer[13] = Fun_DealDCData(2,Right_BrakeAve);
		Tx_Buffer[14] = Fun_DealDCData(1,Right_BrakeAve);
		Tx_Buffer[15] = 0x20;
		/*左空载制和*/
		Tx_Buffer[16] = Fun_DealDCData(5,Left_BrakeSum);
		Tx_Buffer[17] = Fun_DealDCData(4,Left_BrakeSum);
		Tx_Buffer[18] = Fun_DealDCData(3,Left_BrakeSum);
		Tx_Buffer[19] = Fun_DealDCData(2,Left_BrakeSum);
		Tx_Buffer[20] = Fun_DealDCData(1,Left_BrakeSum);
		Tx_Buffer[21] = 0x20;
		/*右空载制和*/
		Tx_Buffer[22] = Fun_DealDCData(5,Right_BrakeSum);
		Tx_Buffer[23] = Fun_DealDCData(4,Right_BrakeSum);
		Tx_Buffer[24] = Fun_DealDCData(3,Right_BrakeSum);
		Tx_Buffer[25] = Fun_DealDCData(2,Right_BrakeSum);
		Tx_Buffer[26] = Fun_DealDCData(1,Right_BrakeSum);
		Tx_Buffer[27] = 0x20;
		/*左空载制差*/
		Tx_Buffer[28] = Fun_DealDCData(5,Left_BrakeSub);
		Tx_Buffer[29] = Fun_DealDCData(4,Left_BrakeSub);
		Tx_Buffer[30] = Fun_DealDCData(3,Left_BrakeSub);
		Tx_Buffer[31] = Fun_DealDCData(2,Left_BrakeSub);
		Tx_Buffer[32] = Fun_DealDCData(1,Left_BrakeSub);
		Tx_Buffer[33] = 0x20;
		/*右空载制差*/
		Tx_Buffer[34] = Fun_DealDCData(5,Right_BrakeSub);
		Tx_Buffer[35] = Fun_DealDCData(4,Right_BrakeSub);
		Tx_Buffer[36] = Fun_DealDCData(3,Right_BrakeSub);
		Tx_Buffer[37] = Fun_DealDCData(2,Right_BrakeSub);
		Tx_Buffer[38] = Fun_DealDCData(1,Right_BrakeSub);
		Tx_Buffer[39] = 0x20;
		/*左加载制和*/
		Tx_Buffer[40] = Fun_DealDCData(5,Left_BrakeSumJJ);
		Tx_Buffer[41] = Fun_DealDCData(4,Left_BrakeSumJJ);
		Tx_Buffer[42] = Fun_DealDCData(3,Left_BrakeSumJJ);
		Tx_Buffer[43] = Fun_DealDCData(2,Left_BrakeSumJJ);
		Tx_Buffer[44] = Fun_DealDCData(1,Left_BrakeSumJJ);
		Tx_Buffer[45] = 0x20;
		/*右加载制和*/
		Tx_Buffer[46] = Fun_DealDCData(5,Right_BrakeSumJJ);
		Tx_Buffer[47] = Fun_DealDCData(4,Right_BrakeSumJJ);
		Tx_Buffer[48] = Fun_DealDCData(3,Right_BrakeSumJJ);
		Tx_Buffer[49] = Fun_DealDCData(2,Right_BrakeSumJJ);
		Tx_Buffer[50] = Fun_DealDCData(1,Right_BrakeSumJJ);
		Tx_Buffer[51] = 0x20;
		/*左加载制差*/
		Tx_Buffer[52] = Fun_DealDCData(5,Left_BrakeSubJJ);
		Tx_Buffer[53] = Fun_DealDCData(4,Left_BrakeSubJJ);
		Tx_Buffer[54] = Fun_DealDCData(3,Left_BrakeSubJJ);
		Tx_Buffer[55] = Fun_DealDCData(2,Left_BrakeSubJJ);
		Tx_Buffer[56] = Fun_DealDCData(1,Left_BrakeSubJJ);
		Tx_Buffer[57] = 0x20;
		/*右加载制差*/
		Tx_Buffer[58] = Fun_DealDCData(5,Right_BrakeSubJJ);
		Tx_Buffer[59] = Fun_DealDCData(4,Right_BrakeSubJJ);
		Tx_Buffer[60] = Fun_DealDCData(3,Right_BrakeSubJJ);
		Tx_Buffer[61] = Fun_DealDCData(2,Right_BrakeSubJJ);
		Tx_Buffer[62] = Fun_DealDCData(1,Right_BrakeSubJJ);
		Tx_Buffer[63] = 0x20;
		W5500_Send_Data(s,Tx_Buffer);				//发送当前检测结果
}
/*******************************************************************************
* 函数名  : FunW5500_TestC()
* 描述    : 上传测试过程中检测数据
* 输入    :  s:端口
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void FunW5500_TestC(uint8_t s)
{
		_union_u16_u8_type u16Data;
	  float fl32Left_SumBrake,fl32Right_SumBrake,f32All_SumWeight;
		uint16_t Left_DynamicBrake1,Right_DynamicBrake1,All_DynamicWeight1,Voltage_Relative;
	  if(USER_SystemFlag.A.Flag_TestC == 1)
		{
					/* 左制动计算 */
					fl32Left_SumBrake = ADShow_Samp(Left_Brake_ID);
					fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //转换成电压值
					if(fl32Left_SumBrake > Zero_LeftBrake)
					{
							Voltage_Relative = fl32Left_SumBrake - Zero_LeftBrake;
							Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Brake_ID,Voltage_Relative);
							Left_DynamicBrake1 += 5;
							Left_DynamicBrake1 /= 10;
							Left_DynamicBrake1 *= 10;   //保留daN
					}
					else 
					{
							Left_DynamicBrake1 = 0;
					}
					/* 右制动计算 */
					fl32Right_SumBrake = ADShow_Samp(Right_Brake_ID);//取平均值
					fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//求电压值		
					if(fl32Right_SumBrake > Zero_RightBrake)
					{
							Voltage_Relative = fl32Right_SumBrake - Zero_RightBrake;
							Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Brake_ID,Voltage_Relative);
							Right_DynamicBrake1 += 5;
							Right_DynamicBrake1 /= 10;
							Right_DynamicBrake1 *= 10;  //保留daN
					}
					else 
					{
							Right_DynamicBrake1 = 0;
					}
					/* 复重计算 */
					f32All_SumWeight = ADShow_Samp(All_Weight_ID);//取平均值
					f32All_SumWeight = f32All_SumWeight*VOLTAGE;//求电压值		
					if(f32All_SumWeight > Zero_AllWeight)
					{
							Voltage_Relative = f32All_SumWeight - Zero_AllWeight;
							All_DynamicWeight1 = Fun_VoltageConvertValue(All_Weight_ID,Voltage_Relative);
					}
					else 
					{
							All_DynamicWeight1 = 0;
					}
					
					Tx_Buffer[0] = 28;  //帧长度 低位
					Tx_Buffer[1] = 0;    //帧长度 高位
					Tx_Buffer[2] = 'C'; 
					Tx_Buffer[3] = 'X'; 
					Tx_Buffer[4] = 3;
					Tx_Buffer[5] = 3;
					Tx_Buffer[6] = 4;
					//模拟量
					u16Data.u16_data = Left_DynamicBrake1;
					Tx_Buffer[7] = Left_Brake_ID; //左制
					Tx_Buffer[8] = u16Data.u8_data[0];
					Tx_Buffer[9] = u16Data.u8_data[1];		
					u16Data.u16_data = Right_DynamicBrake1;
					Tx_Buffer[10] = Right_Brake_ID; //右制
					Tx_Buffer[11] = u16Data.u8_data[0];
					Tx_Buffer[12] = u16Data.u8_data[1];
					u16Data.u16_data = All_DynamicWeight1;
					Tx_Buffer[13] = All_Weight_ID; //复重
					Tx_Buffer[14] = u16Data.u8_data[0];
					Tx_Buffer[15] = u16Data.u8_data[1];
					//开入量
					Tx_Buffer[16] = IN_Brake_ID;
					Tx_Buffer[17] = IO_GetInStatus(IN_Brake_ID)+0x30;
					Tx_Buffer[18] = IN_LeftStop_ID;
					Tx_Buffer[19] = IO_GetInStatus(IN_LeftStop_ID)+0x30;
					Tx_Buffer[20] = IN_RightStop_ID;
					Tx_Buffer[21] = IO_GetInStatus(IN_RightStop_ID)+0x30;
					//开出量
					Tx_Buffer[22] = 1;
					Tx_Buffer[23] = USER_SystemFlag.A.Flag_Ldynamo+0x30;
					Tx_Buffer[24] = 2;
					Tx_Buffer[25] = USER_SystemFlag.A.Flag_Rdynamo+0x30;
					Tx_Buffer[26] = 3;
					Tx_Buffer[27] = USER_SystemFlag.A.Flag_Switch+0x30;
					Tx_Buffer[28] = 4;
					Tx_Buffer[29] = USER_SystemFlag.A.Flag_ST+0x30;		
					W5500_Send_Data(s,Tx_Buffer);				//发送当前检测结果
		}
}
/*******************************************************************************
* 函数名  : Fun_DealParameter()
* 描述    : 控制参数变量处理函数 参数接收时使用
* 输入    :  无
* 输出    : 无
* 返回值  :1:参数设置正确  0:参数设置失败
* 说明    : 
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
						case 10:  //静态称重倒计时3-9
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par0_WeightTestTime = parameter.u16_data; //倒计时3-9秒
													}
							            break;
						}
						case 11://第三滚筒有无  0  1
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
						case 12://踏板力有无 0 1
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
						case 13://制动准备时间 2-9
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //倒计时3-9秒
													}
							            break;
						 }
						case 14://阻滞测试时间 2-9秒
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //倒计时3-9秒
													}
							            break;
						}
						case 15://制动测试时间 2-9秒
						{
													if((parameter.u16_data<3) || (parameter.u16_data>9))
													{
															return 0;
													}
													else
													{
															SysParameter.Par5_BrakeTestTime = parameter.u16_data; //倒计时3-9秒
													}
							            break;
						 }
						case 16://轮重台分度值  1 2 5
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
						case 17://左动态平均零值 daN 0-30
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
						case 18://右动态平均零值 daN 0-30
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
						case 19://左动态最大零值 daN 0-30
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
						case 20://右动态最大零值 daN 0-30
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
						case 21://加载举升重量阀值 
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
						case 22://台架类型 0-3
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
						case 23://加载举升高度 0-900
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
						case 24://台架最长举升时间20-90 最高可设置90s
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
* 函数名  : Fun_GetParameter()
* 描述    : 获取参数处理  
* 输入    :  无
* 输出    : 无
* 返回值  :1:参数设置正确  0:参数设置失败
* 说明    : 
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
			  //参数1
				Parameter_Buffer[1] = 10;
				Parameter_Buffer[2] = 0;
				Parameter_Buffer[3] = 0;
				//参数2
				Parameter_Buffer[4] = 11;
				Parameter_Buffer[5] = 0;
				Parameter_Buffer[6] = 0;
				//参数3
				Parameter_Buffer[7] = 12;
				Parameter_Buffer[8] = 0;
				Parameter_Buffer[9] = 0;
				//参数4
				Parameter_Buffer[10] = 13;
				Parameter_Buffer[11] = 0;
				Parameter_Buffer[12] = 0;
				//参数5
				Parameter_Buffer[13] = 14;
				Parameter_Buffer[14] = 0;
				Parameter_Buffer[15] = 0;
				//参数6
				Parameter_Buffer[16] = 15;
				Parameter_Buffer[17] = 0;
				Parameter_Buffer[18] = 0;
				//参数7
				Parameter_Buffer[19] = 16;
				Parameter_Buffer[20] = 0;
				Parameter_Buffer[21] = 0;
				//参数8
				Parameter_Buffer[22] = 17;
				Parameter_Buffer[23] = 0;
				Parameter_Buffer[24] = 0;
				//参数9
				Parameter_Buffer[25] = 18;
				Parameter_Buffer[26] = 0;
				Parameter_Buffer[27] = 0;
				//参数10
				Parameter_Buffer[28] = 19;
				Parameter_Buffer[29] = 0;
				Parameter_Buffer[30] = 0;
				//参数11
				Parameter_Buffer[31] = 20;
				Parameter_Buffer[32] = 0;
				Parameter_Buffer[33] = 0;
				//参数12
				Parameter_Buffer[34] = 21;
				Parameter_Buffer[35] = 0;
				Parameter_Buffer[36] = 0;
				//参数13
				Parameter_Buffer[37] = 22;
				Parameter_Buffer[38] = 0;
				Parameter_Buffer[39] = 0;
				//参数14
				Parameter_Buffer[40] = 23;
				Parameter_Buffer[41] = 0;
				Parameter_Buffer[42] = 0;
				//参数15
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
								case 10:  //静态称重倒计时3-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par0_WeightTestTime = 3; //默认3秒
																	Parameter_Buffer[2] = 3;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par0_WeightTestTime = parameter.u16_data; //倒计时3-9秒
															}
															break;
								}
								case 11://第三滚筒有无  0  1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par1_RollerFlag = 1; //默认有
																	Parameter_Buffer[5] = 1;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par1_RollerFlag = parameter.u16_data; 
															}
															break;
								}
								case 12://踏板力有无 0 1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par2_FootPlatFlag = 1;//默认有
																	Parameter_Buffer[8] = 1;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par2_FootPlatFlag = parameter.u16_data; 
															}
															break;
								}
								case 13://制动准备时间 2-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par3_BrakeReadyTime = 3; //默认3秒
																	Parameter_Buffer[11] = 3;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //倒计时3-9秒
															}
															break;
								 }
								case 14://阻滞测试时间 2-9秒
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par4_ZuzhiTestTime = 5; //默认5秒
																	Parameter_Buffer[14] = 5;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //倒计时3-9秒
															}
															break;
								}
								case 15://制动测试时间 2-9秒
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par5_BrakeTestTime = 8; //默认8秒
																	Parameter_Buffer[17] = 8;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par5_BrakeTestTime = parameter.u16_data; //倒计时3-9秒
															}
															break;
								 }
								case 16://轮重台分度值  1 2 5
								{
															if((parameter.u16_data==1) || (parameter.u16_data==2) || (parameter.u16_data==5))		
															{
																	SysParameter.Par6_Weight = parameter.u16_data; //													
															}
															else
															{
																	SysParameter.Par6_Weight = 1; //默认1
																	Parameter_Buffer[20] = 1;
																	Edit_Flg = 1;
															}
															break;
								}
								case 17://左动态平均零值 daN 0-200
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
								case 18://右动态平均零值 daN 0-200
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
								case 19://左动态最大零值 daN 0-30
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
								case 20://右动态最大零值 daN 0-30
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
								case 21://加载举升重量阀值 
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
								case 22://台架类型 0-3
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
								case 23://加载举升高度 0-900
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
								case 24://台架最长举升时间20-90 最高可设置90s
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
								case 10:  //静态称重倒计时3-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par0_WeightTestTime = 3; //默认3秒
																	Parameter_Buffer[i*3+2] = 3;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par0_WeightTestTime = parameter.u16_data; //倒计时3-9秒
															}
															break;
								}
								case 11://第三滚筒有无  0  1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par1_RollerFlag = 1; //默认有
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
								case 12://踏板力有无 0 1
								{
															if(parameter.u16_data > 1)
															{
																	SysParameter.Par2_FootPlatFlag = 1;//默认有
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
								case 13://制动准备时间 2-9
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par3_BrakeReadyTime = 3; //默认3秒
																	Parameter_Buffer[i*3+2] = 3;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par3_BrakeReadyTime = parameter.u16_data; //倒计时3-9秒
															}
															break;
								 }
								case 14://阻滞测试时间 2-9秒
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par4_ZuzhiTestTime = 5; //默认5秒
																	Parameter_Buffer[i*3+2] = 5;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par4_ZuzhiTestTime = parameter.u16_data; //倒计时3-9秒
															}
															break;
								}
								case 15://制动测试时间 2-9秒
								{
															if((parameter.u16_data<3) || (parameter.u16_data>9))
															{
																	SysParameter.Par5_BrakeTestTime = 8; //默认8秒
																	Parameter_Buffer[i*3+2] = 8;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															else
															{
																	SysParameter.Par5_BrakeTestTime = parameter.u16_data; //倒计时3-9秒
															}
															break;
								 }
								case 16://轮重台分度值  1 2 5
								{
															if((parameter.u16_data==1) || (parameter.u16_data==2) || (parameter.u16_data==5))		
															{
																	SysParameter.Par6_Weight = parameter.u16_data; //
															}
															else
															{
																	SysParameter.Par6_Weight = 1; //默认1
																	Parameter_Buffer[i*3+2] = 1;
																	Parameter_Buffer[i*3+3] = 0;
																	Edit_Flg = 1;
															}
															break;
								}
								case 17://左动态平均零值 daN 0-200
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
								case 18://右动态平均零值 daN 0-30
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
								case 19://左动态最大零值 daN 0-30
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
								case 20://右动态最大零值 daN 0-30
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
								case 21://加载举升重量阀值 
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
								case 22://台架类型 0-3
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
								case 23://加载举升高度 0-900
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
								case 24://台架最长举升时间20-90 最高可设置90s
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
				SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//擦出扇区
				SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(u8ParameterN*3+1));//保存系数
		}
}
/*******************************************************************************
* 函数名  : W5500_ParameterReset
* 描述    : 网络IP参数强制复位
* 输入    :  无
* 输出    : 无
* 返回值  :无
* 说明    : 
*******************************************************************************/
void W5500_ParameterReset(void)
{
		 W5500_Parameter[0] = 192;
		 W5500_Parameter[1] = 168;
		 W5500_Parameter[2] = 100;
		 W5500_Parameter[3] = 1;		  //网关
	 
		 W5500_Parameter[4] = 255;
		 W5500_Parameter[5] = 255;
		 W5500_Parameter[6] = 255;
		 W5500_Parameter[7] = 0;      //子网掩码
	 
		 W5500_Parameter[8] = 192;
		 W5500_Parameter[9] = 168;
		 W5500_Parameter[10] = 100;
		 W5500_Parameter[11] = 4;     //主控板IP
	 
		 W5500_Parameter[12] = 0xC8;
		 W5500_Parameter[13] = 0x61;   //端口号25032
	 
		 W5500_Parameter[14] = 0;
		 W5500_Parameter[15] = 0;
		 W5500_Parameter[16] = 0;
		 W5500_Parameter[17] = 0;   // 数码管IP
		 
		 W5500_Parameter[18] = 0;			
		 W5500_Parameter[19] = 0;    //数码管端口号
					 
		 W5500_Parameter[20] = 192;
		 W5500_Parameter[21] = 168;
		 W5500_Parameter[22] = 1;
		 W5500_Parameter[23] = 200;   //UDP广播目的地址
		 
		 W5500_Parameter[24] = 0xC7;
		 W5500_Parameter[25] = 0x61;  //UDP端口号
		 
		 W5500_Parameter[26] = 0x55;
		 SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//擦出扇区
		 SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//保存系数		
}
