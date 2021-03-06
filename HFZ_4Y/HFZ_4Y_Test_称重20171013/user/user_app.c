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
										if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
										{
												USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
										}
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
* 输入    : Channel_ID 调零通道   state:是否直接调零 1:直接调零
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
* 函数名  : Fun_StaticWeightTest
* 描述    : 静态轮重测试函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void Fun_StaticWeightTest(void) 
{
		uint8_t Countdown_time;//倒计时初始值
		uint16_t u16Count300=0; //300个点存储计数
		uint16_t u16countMax=0; //计数最大存储点数量
		uint16_t Voltage_Relative;//相对电压
		float fl32Left_SumWeight=0,
					 fl32Right_SumWeight=0;//重量求和临时变量
		float fl32ADSum_Channen1=0,
					 fl32ADSum_Channen2=0;//重量求和临时变量

		u16countMax = 0;//初始化		
		if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
		{
				W5500_Send_Data(2,con_TranMW);//上传MW 静态轮重检测
		}
    else
    {
        if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
        {
              W5500_Send_Data(0,con_TranMW);//上传MW 静态轮重检测
        }
    }
		Left_DynamicWeight = 0;
		Right_DynamicWeight = 0;  //动态显示值清零
		Left_StaticWeight = 0;
		Right_StaticWeight = 0; //初始化轴重结果
		Countdown_time = SysParameter.Par0_WeightTestTime + 1; //倒计时时间
// 		Countdown_time = 2;		
		USER_SystemFlag.A.Flag_1s = 1;//首次调用上传倒计时第一个数使用
		if(USER_SystemFlag.A.Flag_TestC1 == 1)
		{
				USER_SystemFlag.A.Flag_TestC = 1;
		}
		while(System_State == StaticWeight_State)
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(IO_GetInStatus(IN_Weight_ID) == 1)
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
															W5500_Send_Data(2,Tx_Buffer);
															 if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
															 {												
																		USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
																		USER_LEDFlag.A.LED_14 = 0;  				//数码管指示灯是否改变 1变  0不变
																		FunW5500_SendLED(0,LED_OFF,Countdown_time);
															 }
												 }
												 else
												 {
															 if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
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
								/*左重*/
							  fl32Left_SumWeight = ADShow_Samp(Left_Weight_ID);
								fl32Left_SumWeight =fl32Left_SumWeight*VOLTAGE; //转换成电压值
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
								/*右重*/		
								fl32Right_SumWeight = ADShow_Samp(Right_Wight_ID);
								fl32Right_SumWeight = fl32Right_SumWeight*VOLTAGE; //转换成电压值
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
									/*左重*/
									fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);
									fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //转换成电压值
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
									/*右重*/
									fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
									fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //转换成电压值
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
									if(USER_SystemFlag.A.Flag_Online == 1) //和主控联网 上传内容
								 {
											if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
											{
													if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
													{
															USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
													}
													USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
													USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
													USER_LEDFlag.A.LED_Weight = 1;  			//重量指示灯
													USER_LEDFlag.A.LED_Brake = 0;  			  //制动指示灯
													FunW5500_SendLED(0,Left_DynamicWeight,Right_DynamicWeight);	
											}
								 }
								 else
								 {
											if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
											{
// 													FunW5500_SendGW(0,Left_DynamicWeight,Right_DynamicWeight);	
													W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号
											}
								 }
					 }
						Countdown_time = SysParameter.Par0_WeightTestTime + 1; //倒计时时间
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 1;//首次调用上传倒计时第一个数使用
						u16countMax = 0;//初始化
				}
		}		
		USER_SystemFlag.A.Flag_TestC = 0;
		USER_SystemFlag.A.Flag_TestC1 = 0;
		#ifdef _DEBUG //调试用函数段
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
		
    #ifdef _DEBUG //调试用函数段
		printf("\r\nL=%u,R=%u\r\n",Left_StaticWeight,Right_StaticWeight);		
    #endif
		 if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
		 {
					W5500_Send_Data(2,con_TranT0); //正常检测结束
					if(USER_SystemFlag.A.Flag_AutoD == 1)
					{
							USER_SystemFlag.A.Flag_AutoD = 0;
							FunW5500_SendDW(2);				//发送当前轮重
					}
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{								
								if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
								{
										if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
										{
												USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
										}
										USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
										USER_LEDFlag.A.LED_15 = 1;  				      //数码管数据是否改变 1变  0不变
										USER_LEDFlag.A.LED_14 = 1;  				      //数码管指示灯是否改变 1变  0不变
										USER_LEDFlag.A.LED_Weight = 1;  			//重量指示灯
										USER_LEDFlag.A.LED_Brake = 0;  			  //制动指示灯
										FunW5500_SendLED(0,Left_StaticWeight,Right_StaticWeight);	
								}
				  }
		 }		
		 else
		 {
					if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
					{
								W5500_Send_Data(0,con_TranT0); //正常检测结束
				  }
		 }		 
		System_State = ReadData_State; //上边倒计时时间结束也有这条状态切换
}

/*******************************************************************************
* 函数名  : Fun_DynamicWeightTest
* 描述    : 动态轮重测试函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void Fun_DynamicWeightTest(void)   
{
			uint8_t Loop_Flag=1;
			uint8_t i,j,m,i_Flg;
			uint16_t u16Count100=0; //100个点存储计数
			uint16_t u16countMax=0; //计数最大存储点数量
			uint16_t Voltage_Relative,Left_MaxWeight,Right_MaxWeight;  //相对电压  左右轮重最大值
			uint8_t Max_Number=0,			             //判断是否测试一轴结束
								  DynamicWeight_Count=0; //称重检测轴数计数
			float fl32Left_SumWeight=0,
						 fl32Right_SumWeight=0;//重量求和临时变量

		Left_MaxWeight = 0;
		Right_MaxWeight = 0;//初始化
		for(i=0;i<6;i++) //缓存初始化
		{
				Left_DynamicWeight1[i] = 0;
				Right_DynamicWeight1[i] = 0;
		}
		i_Flg = 0;
		con_TranMA[4] = 0x31;
		if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
		{
					W5500_Send_Data(0,con_TranMA);   //检测开始上传检测第1轴
		}
		if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
		{
					W5500_Send_Data(2,con_TranMA);   //检测开始上传检测第1轴
		}			
		if(USER_SystemFlag.A.Flag_TestC1 == 1)
		{
				USER_SystemFlag.A.Flag_TestC = 1;
		}
		while(System_State == DynamicWeight_State)
		{
				while(Loop_Flag == 1)
				{
						W5500_Receive_Deal(); //网络通信查询处理
						if(USER_SystemFlag.A.Flag_10ms == 1)
						{
								USER_SystemFlag.A.Flag_10ms = 0;																	
								/* 左轮重计算 */
								fl32Left_SumWeight = ADShow_Samp(Left_Weight_ID);
								fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE; //转换成电压值
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
								/* 右轮重计算 */
								fl32Right_SumWeight = ADShow_Samp(Right_Wight_ID);
								fl32Right_SumWeight = fl32Right_SumWeight*VOLTAGE;//求电压值		
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
								//判断是否采集结束
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
								if((Left_MaxWeight + Right_MaxWeight) > 50) //如果最大值大于50kg
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
														if( TIM2_count > 4000) //4秒
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
								if(u16countMax < 100) u16countMax++;    //保存的数据计数  最多计数100个
						}
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
								/* 左轮重计算 */
								fl32Left_SumWeight = ADShow_Display(Left_Weight_ID);
								fl32Left_SumWeight = fl32Left_SumWeight*VOLTAGE; //转换成电压值
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
								/* 右轮重计算 */
								fl32Right_SumWeight = ADShow_Display(Right_Wight_ID);
								fl32Right_SumWeight = fl32Right_SumWeight*VOLTAGE;//求电压值		
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
								if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
								{
											if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
											{
														USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
														USER_LEDFlag.A.LED_14 = 0;  				//数码管指示灯是否改变 1变  0不变
														FunW5500_SendLED(0,Left_DynamicWeight,Right_DynamicWeight);//上传称重重量
											}
								}
						}
				}
				DataRank(u16Left1_TemporaryWeight,u16countMax);    //左轮重数据排序
				DataRank(u16Right1_TemporaryWeight,u16countMax);  //右轮重数据排序
				
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
				if(USER_SystemFlag.A.Flag_Online == 1)         //联网 给主控发
				{
							if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
							{
										USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
										USER_LEDFlag.A.LED_14 = 0;  				//数码管指示灯是否改变 1变  0不变
										FunW5500_SendLED(0,Left_DynamicWeight1[DynamicWeight_Count],Right_DynamicWeight1[DynamicWeight_Count]);//上传轮重
							}
				}
				DynamicWeight_Count ++;
				if(DynamicWeight_Count < DynamicWeight_Number)
				{
						 con_TranMA[4] = DynamicWeight_Count + 0x31;
// 						 if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
// 						 {
// 									W5500_Send_Data(0,con_TranMA);   //检测开始上传检测第1轴
// 						 }
						 if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						 {
									W5500_Send_Data(2,con_TranMA);   //检测开始上传检测第1轴
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
						#ifdef _DEBUG //调试用函数段
						for(i=0;i<DynamicWeight_Number;i++)
						{
								printf("\r\n%u L=%u,R=%u\r\n",i,Left_DynamicWeight1[i],Right_DynamicWeight1[i]);
						}
						#endif													
						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						{									
									W5500_Send_Data(2,con_TranT0);   //正常检测结束
									if(USER_SystemFlag.A.Flag_AutoD == 1)
									{
											USER_SystemFlag.A.Flag_AutoD = 0;
											FunW5500_SendDW(2);				//发送当前轮重
									}
									if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
									{								
												if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
												{
														USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
												}
												USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
												USER_LEDFlag.A.LED_15 = 1;  				      //数码管数据是否改变 1变  0不变
												USER_LEDFlag.A.LED_14 = 1;  				      //数码管指示灯是否改变 1变  0不变
												USER_LEDFlag.A.LED_Weight = 1;  			//重量指示灯
												USER_LEDFlag.A.LED_Brake = 0;  			  //制动指示灯
												FunW5500_SendLED(0,Left_DynamicWeight1[DynamicWeight_Count-1],Right_DynamicWeight1[DynamicWeight_Count-1]);	
									}	
						}		
						else
						{	
									if(USER_SystemFlag.A.Flag_Standalone == 1) //单机联网 给数码管发
									{
												W5500_Send_Data(0,con_TranT0);   //正常检测结束									
									}
						}
					  System_State = ReadData_State; //上边倒计时时间结束也有这条状态切换
				}
		}		
}

/*******************************************************************************
* 函数名  : Fun_MotorWeightTest
* 描述    : 摩托称重测试函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void Fun_MotorWeightTest(void)
{
		uint8_t Countdown_time;//倒计时初始值
		uint16_t  u16Left0_TemporaryWeight[10] = {0},
								   u16Right0_TemporaryWeight[10] = {0};//左右轮重1ms模拟量转换结果存储 临时变量

		uint8_t u8Count10=0;       //10个点计数
		uint8_t u8Count101=0;     //10个点计数   在不到位中使用 用来显示实时值使用
		uint16_t u16Count300=0; //300个点存储计数
		uint16_t u16countMax=0; //计数最大存储点数量
		uint16_t Voltage_Relative;//相对电压
		float u32Left_SumWeight=0.0,
					 u32Right_SumWeight=0.0;//重量求和临时变量

		u16countMax = 0;//初始化
		W5500_Send_Data(0,con_TranML);//上传MW 静态轮重检测
		Left_DynamicWeight = 0;
		Right_DynamicWeight = 0;  //动态显示值清零
		Motor_StaticWeight = 0; //初始化摩重结果
		Countdown_time = SysParameter.Par0_WeightTestTime + 1; //倒计时时间
		USER_SystemFlag.A.Flag_1s = 1;//首次调用上传倒计时第一个数使用
		while(System_State == StaticWeight_Motor_State)
		{
				W5500_Receive_Deal(); //网络通信查询处理
				if(IO_GetInStatus(IN_Weight_ID) == 1)
				{
						u8Count101 = 0;//不到位中采集模拟量使用
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
							
								u16Left0_TemporaryWeight[u8Count10] = ADC_Convert(Left_Weight_ID);   //左重转换结果
								u32Left_SumWeight += u16Left0_TemporaryWeight[u8Count10];
							
								u16Right0_TemporaryWeight[u8Count10] = ADC_Convert(Right_Wight_ID); //右重转换结果
								u32Right_SumWeight += u16Right0_TemporaryWeight[u8Count10];
							
								u8Count10 ++;
								if(u8Count10 >= 10)
								{
										u16countMax ++;
										/* 左轮重计算 */
										u32Left_SumWeight /= 10;
// 										u32Left_SumWeight += 0.5;
										u32Left_SumWeight = u32Left_SumWeight*VOLTAGE; //转换成电压值
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
										/* 右轮重计算 */
										u32Right_SumWeight /= 10;//取平均值
// 										u32Right_SumWeight += 0.5;
										u32Right_SumWeight = u32Right_SumWeight*VOLTAGE;//求电压值		
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
										u32Right_SumWeight = 0; //求和变量清零 临时变量															
								}
						}
				}
				else
				{
						//实时显示使用
						if(USER_SystemFlag.A.Flag_1ms == 1)
						{
								USER_SystemFlag.A.Flag_1ms = 0;
							
								u16Left0_TemporaryWeight[u8Count101] = ADC_Convert(Left_Weight_ID);   //左重转换结果
								u32Left_SumWeight += u16Left0_TemporaryWeight[u8Count101];
							
								u16Right0_TemporaryWeight[u8Count101] = ADC_Convert(Right_Wight_ID); //右重转换结果
								u32Right_SumWeight += u16Right0_TemporaryWeight[u8Count101];
							
								u8Count101 ++;
								if(u8Count101 >= 10)
								{
										/* 左轮重计算 */
										u32Left_SumWeight /= 10;
// 										u32Left_SumWeight += 0.5;
										u32Left_SumWeight = u32Left_SumWeight*VOLTAGE; //转换成电压值
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
										/* 右轮重计算 */
										u32Right_SumWeight /= 10;//取平均值
// 										u32Right_SumWeight += 0.5;
										u32Right_SumWeight = u32Right_SumWeight*VOLTAGE;//求电压值	
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
										u32Right_SumWeight = 0; //求和变量清零 临时变量															
								}
						}
						if(USER_SystemFlag.A.Flag_500ms == 1)
						{
								USER_SystemFlag.A.Flag_500ms = 0;
								W5500_Send_Data(0,con_TranT4); //回传车辆不到位信号								
						}						
						Countdown_time = SysParameter.Par0_WeightTestTime + 1; //倒计时时间
						TIM2_1ms = 0;//定时计数清零
						USER_SystemFlag.A.Flag_1s = 1;//首次调用上传倒计时第一个数使用
						u8Count10 = 0;
						u16Count300 = 0;
						u16countMax = 0;//初始化
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
		W5500_Send_Data(0,con_TranT0); //正常检测结束
		System_State = Default_State; //上边倒计时时间结束也有这条状态切换
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
	 if(Parameter_Buffer[24] != 0x55)//出厂编号
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
			 W5500_Parameter[11] = 3;     //主控板IP
 		 
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
						if(1)//默认轮重台
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
						fl32ADSum_Channen2 = 0.0;
		uint16_t Voltage_Relative;									 //相对电压�

		Fun_Zero(Left_Weight_ID,0);//判断重量是否调零
		Fun_Zero(Right_Wight_ID,0);//判断重量是否调零
		while(System_State == Default_State)
		{
				W5500_Receive_Deal(); //网络通信查询处理
// 				if(SysParameter.Par12_MachineType == 0) //轮重台
				if(1)//默认轮重台
				{							
							if(USER_SystemFlag.A.Flag_500ms == 1)
							{
										USER_SystemFlag.A.Flag_500ms = 0;
										/*左重*/
										fl32ADSum_Channen1 = ADShow_Display(Left_Weight_ID);
										fl32ADSum_Channen1 = fl32ADSum_Channen1*VOLTAGE; //转换成电压值
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
										/*右重*/
										fl32ADSum_Channen2 = ADShow_Display(Right_Wight_ID);
										fl32ADSum_Channen2 = fl32ADSum_Channen2*VOLTAGE; //转换成电压值
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
										if(USER_SystemFlag.A.Flag_Online == 1) //和主控联网 上传内容
									 {
// 												FunW5500_SendLED(2,Left_DynamicWeight,Right_DynamicWeight);	
												if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
												{
														if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
														{
																USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
														}
														USER_LEDFlag.A.LED_Test = 0;  					//检测指示灯
														USER_LEDFlag.A.LED_15 = 1;  				//数码管数据是否改变 1变  0不变
														USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
														USER_LEDFlag.A.LED_Weight = 1;  			//重量指示灯
														USER_LEDFlag.A.LED_Brake = 0;  			  //制动指示灯
														FunW5500_SendLED(0,Left_DynamicWeight,Right_DynamicWeight);	
												}
									 }
									 else
									 {
												if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
												{
														FunW5500_SendGW(0,Left_DynamicWeight,Right_DynamicWeight);	
												}
									 }
							}				
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
					/* 左重计算 */
					fl32Left_SumBrake = ADShow_Samp(Left_Weight_ID);
					fl32Left_SumBrake = fl32Left_SumBrake*VOLTAGE; //转换成电压值
					if(fl32Left_SumBrake > Zero_LeftWeight)
					{
							Voltage_Relative = fl32Left_SumBrake - Zero_LeftWeight;
							Left_DynamicBrake1 = Fun_VoltageConvertValue(Left_Weight_ID,Voltage_Relative);
					}
					else 
					{
							Left_DynamicBrake1 = 0;
					}
					/* 右重计算 */
					fl32Right_SumBrake = ADShow_Samp(Right_Wight_ID);//取平均值
					fl32Right_SumBrake = fl32Right_SumBrake*VOLTAGE;//求电压值		
					if(fl32Right_SumBrake > Zero_RightWight)
					{
							Voltage_Relative = fl32Right_SumBrake - Zero_RightWight;
							Right_DynamicBrake1 = Fun_VoltageConvertValue(Right_Wight_ID,Voltage_Relative);
					}
					else 
					{
							Right_DynamicBrake1 = 0;
					}
					Tx_Buffer[0] = 13;  //帧长度 低位
					Tx_Buffer[1] = 0;    //帧长度 高位
					Tx_Buffer[2] = 'C'; 
					Tx_Buffer[3] = 'X'; 
					Tx_Buffer[4] = 2;
					Tx_Buffer[5] = 1;
					Tx_Buffer[6] = 0;
					//模拟量
					u16Data.u16_data = Left_DynamicBrake1;
					Tx_Buffer[7] = Left_Weight_ID; //左重
					Tx_Buffer[8] = u16Data.u8_data[0];
					Tx_Buffer[9] = u16Data.u8_data[1];		
					u16Data.u16_data = Right_DynamicBrake1;
					Tx_Buffer[10] = Right_Wight_ID; //右重
					Tx_Buffer[11] = u16Data.u8_data[0];
					Tx_Buffer[12] = u16Data.u8_data[1];
					//开入量
					Tx_Buffer[13] = IN_Weight_ID;
					Tx_Buffer[14] = IO_GetInStatus(IN_Weight_ID)+0x30;
					W5500_Send_Data(s,Tx_Buffer);				//发送当前检测结果
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
						case 18://右动态平均零值 daN 0-30
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
// 													break;
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
								case 17://左动态平均零值 daN 0-30
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
								case 18://右动态平均零值 daN 0-30
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
								case 17://左动态平均零值 daN 0-30
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
								case 18://右动态平均零值 daN 0-30
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
		 W5500_Parameter[11] = 3;     //主控板IP
	 
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
