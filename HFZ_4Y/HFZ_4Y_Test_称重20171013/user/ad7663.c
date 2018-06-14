/****************************************************************************
* 文件名: AD7663.c
* 文件历史:
* 版本号:    V1.0
* 日期:      2017.02.09    
* 作者:      xx
* 说明:     
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
#include "ad7663.h"

uint8_t  ADC_FinishFlg;  //转换完成标志 TURE
// static uint16_t TIM2_count; //定时器2用作延时函数变量计数，1ms定时。
// static uint16_t TIM2_count1 = 0;
// static uint16_t TIM2_count2 = 0;
// static uint16_t TIM2_count3 = 0;
/*******************************************************************************
* @函数名称	：ADC_7663_Gpioinit   
* @函数说明 ：AD转换引脚初始化
* @输入参数 ：无
* @输出参数 ：无
* @返回参数 ：无
*******************************************************************************/
void ADC_7663_Gpioinit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;	
	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_All;   //AD采集数据端口  PF0-15
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
    
		AD7663_nCS = 0;			//CS=0  器件使能
		AD7663_CVT = 1;			//CNVST=1
		AD7663_nRD = 1;			//RD=1
		AD7663_Byte = 0;			//BYTE=0 
}
/*******************************************************************************
* 函数名  : ADC_7663_NVIC_Config
* 描述    : AD采集外部中断配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
/*============================================================================*/
void ADC_7663_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);				//使能Timer2时钟
		TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;														//7200 - 1;//设置分频系数 7200分频
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数  0.1ms计数一次
		TIM_TimeBaseStructure.TIM_Period = 10 - 1;																		//计数时间 1ms
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//时钟分割
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;													//设置计数开始值
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); 				                //使能TIM2指定的中断
    TIM_Cmd(TIM2, ENABLE); 																												//使能TIMx外设
		//定时器3配置  用于UDP广播
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);				//使能Timer3时钟
		TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;														//7200 - 1;//设置分频系数 7200分频
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数  0.1ms计数一次
		TIM_TimeBaseStructure.TIM_Period = 10 - 1;																		//计数时间 1ms
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//时钟分割
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;													//设置计数开始值
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE ); 				                //使能TIM3指定的中断
    TIM_Cmd(TIM3, DISABLE); 																												//使能TIMx外设
		
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8010000);
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Set);				           //设置中断优先级组为1，优先组(可设0～4位)
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				         //设置中断向量号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM2_IRQChannelPreemptionPriority;	 //设置抢先优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM2_IRQChannelSubPriority;			         //设置响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				     //使能NVIC
    NVIC_Init(&NVIC_InitStructure);
		
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				         //设置中断向量号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_IRQChannelPreemptionPriority;	 //设置抢先优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM3_IRQChannelSubPriority;			         //设置响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				     //使能NVIC
    NVIC_Init(&NVIC_InitStructure);
	
	  EXTI_ClearITPendingBit(EXTI_Line10);                                     	//AD转换完成中断
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI15_10_IRQnChannelPreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI15_10_IRQnChannelSubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* @函数名称	：ADC_Convert
* @函数说明 ：AD转换开始
* @输入参数 ：u8Channel：通道选择1-8
* @输出参数 ：无
* @返回参数 ：转换结果 
*******************************************************************************/
uint16_t  ADC_Convert(uint8_t u8Channel)
{	
// 		uint16_t u16Con_Channel[8] = {0x1000,0x3000,0xB000,0x9000,0xF000,0xD000,0x5000,0x7000};	//0-7通道																								
		uint16_t u16ADC_Data;  //AD转换结果
		uint16_t u16Channel,j,u16Channel_ID;
		switch(u8Channel)
		{
				case Left_Weight_ID:
																						u16Channel_ID = 0x1000;	//对应模拟通道1通道
																						break;
				case Right_Wight_ID:
																						u16Channel_ID = 0x3000;	//对应模拟通道2通道
																						break;
				case Left_Brake_ID:
																						u16Channel_ID = 0xB000;	//对应模拟通道3通道
																						break;
				case Right_Brake_ID:
																						u16Channel_ID = 0x9000;	//对应模拟通道4通道
																						break;
				case All_Weight_ID:
																						u16Channel_ID = 0xF000;	//对应模拟通道5通道
																						break;
				case FootPlat_ID:
																						u16Channel_ID = 0xD000;	//对应模拟通道6通道
																						break;
				case Left_Move_ID:
																						u16Channel_ID = 0x5000;	//对应模拟通道7通道
																						break;
				case Right_Move_ID:
																						u16Channel_ID = 0x7000;	//对应模拟通道8通道
																						break;
				default:
																						printf("\r\nError:The AD channel is false!!!\r\n");
																						u16Channel_ID = 0x1000; //默认出错时对应模拟通道1 
																						break;				
		}
		
	  /* 设置通道号 */
		u16Channel = GPIO_ReadOutputData(GPIOE);
    u16Channel &= 0x0FFF;	
    u16Channel |= u16Channel_ID;
    GPIO_Write(GPIOE, u16Channel);
		for( j = 0; j < 300; j++ ) ;	//延时保证通道切换成功
    /* 置位转换是否完成标志位 */
		ADC_FinishFlg = 0;
		/*--启动转换--*/  
    AD7663_CVT = 1;        //CNVST=1     下降沿启动转换
    AD7663_nRD = 1;        //RD=1
    AD7663_CVT = 0;      // CNVST=0 启动转换	 
    AD7663_nRD = 0;      // RD=0 可以读取数据
		while(ADC_FinishFlg == 0) ;//判断是否转换完成
		u16ADC_Data = GPIO_ReadInputData(GPIOF);
		AD7663_nRD = 1;
		AD7663_CVT = 1;
		return u16ADC_Data;
}

/*******************************************************************************
* 函数名  : EXTI15_10_ISR
* 描述    : AD采集外部中断处理函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
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
* 函数名  : TIM2_ISR
* 描述    : 定时器2中断服务函数
* 输入    : 无
* 输出    : 无   
* 返回    : 无 
* 说明    : 无
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
							TIM2_count_Delay = 0;   //延时函数使用的变量											
					}
					if(TIM2_count > 100000) //超时之后清零
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
											TIM2_count = 0;   //延时函数使用的变量											
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
* 函数名  : TIM3_ISR
* 描述    : 定时器3中断服务函数 判断光电信号
* 输入    : 无
* 输出    : 无   
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void TIM3_ISR(void)
{
		static uint16_t TIM3_count1 = 0;
		static uint16_t TIM3_count2 = 0;
		static uint16_t TIM3_count3 = 0;
		static uint16_t TIM3_count4 = 0;
		static uint8_t u8Before_Status = 0;//IO口上次状态
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
						if(USER_SystemFlag.A.Flag_Online == 1)//和主控联网成功
						{
								Network1_StopFlg ++;
								if(Network1_StopFlg >= NETWORK_MAX)
								{
										Network1_StopFlg = 0;
										USER_SystemFlag.A.Flag_Online = 0;
										USER_LEDFlag.A.LED_Online = 0;  				//联网指示灯
										if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
										{
												USER_LEDFlag.A.LED_15 = 0;  				//数码管数据是否改变 1变  0不变
												USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
												FunW5500_SendLED(0,0,0);            //上传给显示板
										}
										Write_W5500_SOCK_1Byte(2,Sn_CR,CLOSE);//关闭端口,等待重新打开连接 
										Socket_Init(2);		//指定Socket(0~7)初始化,初始化端口0
										S2_State=0;//网络连接状态0x00,端口连接失败
								}
						}
            if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网成功
						{
								Network0_StopFlg ++;
								if(Network0_StopFlg >= NETWORK_MAX)
								{
										Network0_StopFlg = 0;
										USER_SystemFlag.A.Flag_Standalone = 0;										
										Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);//关闭端口,等待重新打开连接 
										Socket_Init(0);		//指定Socket(0~7)初始化,初始化端口0
										S0_State=0;//网络连接状态0x00,端口连接失败
								}
						}
				}
				if(TIM3_count3 >= 20)
				{
						TIM3_count3 = 0;
						if(USER_SystemFlag.A.Flag_Online == 1)  //联网 给主控发
						{
								FunW5500_TestC(2);
						}
				}
				if(TIM3_count2 >= 7) //7ms判断一次光电信号是否有变化
				{
						TIM3_count2 = 0;
// 						if(SysParameter.Par12_MachineType == 0)//轮重台
						if(1)//轮重台
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
				if(TIM3_count1 >= 500)   //强制复位判断
				{
						TIM3_count1 = 0;
						if(GPIO_ReadInputDataBit(SYSTEM_REST_PORT,SYSTEM_REST) == Bit_RESET) //复位按键
						{
								Reset_Flag ++;
								if(Reset_Flag>=RESET_MAX)
								{
										//复位操作
										W5500_Reset();
										Reset_Flag = 0;
								}
						}
						else
						{
								Reset_Flag = 0;
						}
// 						USER_SystemFlag.A.Flag_UDP = 1;
// 						if(SysParameter.Par12_MachineType == 0)//轮重台
						if(1)
						{
									con_TranMI[0] = 4; //帧长
									con_TranMI[4] = IN_Weight_ID;
									con_TranMI[5] = IO_GetInStatus(IN_Weight_ID) + 0x30;
									W5500_Send_Data(1,con_TranMI);
						}
						else
						{
								 con_TranMI[0] = 4; //帧长
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
* 函数名  : Delay
* 描述    : 延时函数(ms)
* 输入    : d:延时系数，单位为毫秒  最大2000
* 输出    : 无
* 返回    : 无 
* 说明    : 延时是利用Timer2定时器产生的1毫秒的计数来实现的
*******************************************************************************/
void Delay_ms(uint16_t time_ms)
{
	TIM2_count_Delay=0; 
	while(TIM2_count_Delay < time_ms)
	{
			IWDG_ReloadCounter();   //看门狗清零
	}
}

/*******************************************************************************
* 函数名  : ADC_7663_Init
* 描述    : 初始化配置AD7663
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 供主函数调用配置AD7663
*******************************************************************************/
void ADC_7663_Init(void)
{
		ADC_7663_Gpioinit();
		ADC_7663_NVIC_Config();	
}
