/****************************************************************************
* 文件名: AD7663_app.c
* 文件历史:
* 版本号:    V1.0
* 日期:      2017.09.15    
* 作者:      xx
* 说明:     AD采集循环队列形式模板
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
#include "ad7663_app.h"		    //AD7663 队列采集

/**循环队列使用变量**/
uint16_t AD_SampBuff[MaxCabItem][DLLength];  //各个通道模拟量采集缓存区
uint16_t AD_Ind[MaxCabItem];                             //各个通道当前位置指针索引
uint32_t AD_SumDisplay[MaxCabItem];  							//各个通道模拟量采集和 用于数码管显示
uint32_t AD_SumAdjust[MaxCabItem];  								//各个通道模拟量采集和  用于标定数据上传
uint32_t AD_SumSamp[MaxCabItem];  								//各个通道模拟量采集和  用于有效数据采集


/*******************************************************************************
* @函数名称	：TIM4_NVIC_Config
* @函数说明 ：定时器4配置 中断配置
* @输入参数 ：无
* @输出参数 ：无
* @返回参数 ：无
*******************************************************************************/
void TIM4_NVIC_Config(void)
{
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);				//使能Timer2时钟
		TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;														//7200 - 1;//设置分频系数 7200分频
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数  0.1ms计数一次
		TIM_TimeBaseStructure.TIM_Period = Tim4_ms*10 - 1;																		//计数时间 1ms
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//时钟分割
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;													//设置计数开始值
		TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
		TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE ); 				                //使能TIM2指定的中断
    TIM_Cmd(TIM4, ENABLE); 																												//使能TIMx外设
	
// 		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);              
// 	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Set);				      //设置中断优先级组为1，优先组(可设0～4位) 
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;				         //设置中断向量号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM4_IRQChannelPreemptionPriority;	 //设置抢先优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM4_IRQChannelSubPriority;			                     //设置响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				                                                             //使能NVIC
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* @函数名称	：InitADList
* @函数说明 ： 初始化各个通道变量值
* @输入参数 ：chm:通道号  InitValue:AD模拟量采集值（0-65535）
* @输出参数 ：无
* @返回参数 ：无
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
* @函数名称	：InitADScreen
* @函数说明 ： 初始化所有通道
* @输入参数 ：无
* @输出参数 ：无
* @返回参数 ：无
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
* @函数名称	：ADGet_Cycle_Queue
* @函数说明 ： 获取队列中指定索引元素值函数
* @输入参数 ：chm:通道号     Index:指针变量
* @输出参数 ：无
* @返回参数 ：无
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
* @函数名称	：ADGet_Data
* @函数说明 ： 定时采集AD变量  定时中断调用
* @输入参数 ：chm:通道号  
* @输出参数 ：无
* @返回参数 ：无
*******************************************************************************/
void ADGet_Data(uint8_t chm)
{
		uint8_t chm1;
		switch(chm)
		{
				case Left_Weight_ID:
																						chm1 = 0;	//对应模拟通道1通道
																						break;
				case Right_Wight_ID:
																						chm1 = 1;	//对应模拟通道2通道
																						break;
				case Left_Brake_ID:
																						chm1 = 2;	//对应模拟通道3通道
																						break;
				case Right_Brake_ID:
																						chm1 = 3;	//对应模拟通道4通道
																						break;
				case All_Weight_ID:
																						chm1 = 4;	//对应模拟通道5通道
																						break;
				case FootPlat_ID:
																						chm1 = 5;	//对应模拟通道6通道
																						break;
				case Left_Move_ID:
																						chm1 = 6;	//对应模拟通道7通道
																						break;
				case Right_Move_ID:
																						chm1 = 7;	//对应模拟通道8通道
																						break;
				default:
																						printf("\r\nError:The ADGet_Data channel is false!!!\r\n");
																						chm1 = 0; //默认出错时对应模拟通道1 
																						break;				
		}
		AD_SampBuff[chm1][AD_Ind[chm1]] = ADC_Convert(chm);  //采集模拟量  ADC_Convert根据自己的模板修改通道号
		/* 累加和 */
		AD_SumDisplay[chm1] += ADGet_Cycle_Queue(chm1,AD_Ind[chm1]) - ADGet_Cycle_Queue(chm1,AD_Ind[chm1]-Window_Display);
		AD_SumAdjust[chm1] += ADGet_Cycle_Queue(chm1,AD_Ind[chm1]) - ADGet_Cycle_Queue(chm1,AD_Ind[chm1]-Window_Adjust);
		AD_SumSamp[chm1] += ADGet_Cycle_Queue(chm1,AD_Ind[chm1]) - ADGet_Cycle_Queue(chm1,AD_Ind[chm1]-Window_Samp);
		AD_Ind[chm1] ++;  //指针加1
		if(AD_Ind[chm1] >= DLLength)  //指针循环
		{
				AD_Ind[chm1] = 0;
		}
}

/*******************************************************************************
* @函数名称	：ADShow_Display
* @函数说明 ： 上传显示值
* @输入参数 ：chm:通道号  
* @输出参数 ：无
* @返回参数 ：无
*******************************************************************************/
uint16_t ADShow_Display(uint8_t chm)
{
		float temp;
		uint16_t temp1;
		switch(chm)
		{
				case Left_Weight_ID:
																						chm = 0;	//对应模拟通道1通道
																						break;
				case Right_Wight_ID:
																						chm = 1;	//对应模拟通道2通道
																						break;
				case Left_Brake_ID:
																						chm = 2;	//对应模拟通道3通道
																						break;
				case Right_Brake_ID:
																						chm = 3;	//对应模拟通道4通道
																						break;
				case All_Weight_ID:
																						chm = 4;	//对应模拟通道5通道
																						break;
				case FootPlat_ID:
																						chm = 5;	//对应模拟通道6通道
																						break;
				case Left_Move_ID:
																						chm = 6;	//对应模拟通道7通道
																						break;
				case Right_Move_ID:
																						chm = 7;	//对应模拟通道8通道
																						break;
				default:
																						printf("\r\nError:The ADGet_Data channel is false!!!\r\n");
																						chm = 0; //默认出错时对应模拟通道1 
																						break;				
		}
	  temp = (float)AD_SumDisplay[chm] / Window_Display;
		temp1 = temp + 0.5;
		return(temp1);
}
/*******************************************************************************
* @函数名称	：ADShow_Adjust
* @函数说明 ： 上传标定值
* @输入参数 ：chm:通道号  
* @输出参数 ：无
* @返回参数 ：无
*******************************************************************************/
uint16_t ADShow_Adjust(uint8_t chm)
{
		float temp;
		uint16_t temp1;
		switch(chm)
		{
				case Left_Weight_ID:
																						chm = 0;	//对应模拟通道1通道
																						break;
				case Right_Wight_ID:
																						chm = 1;	//对应模拟通道2通道
																						break;
				case Left_Brake_ID:
																						chm = 2;	//对应模拟通道3通道
																						break;
				case Right_Brake_ID:
																						chm = 3;	//对应模拟通道4通道
																						break;
				case All_Weight_ID:
																						chm = 4;	//对应模拟通道5通道
																						break;
				case FootPlat_ID:
																						chm = 5;	//对应模拟通道6通道
																						break;
				case Left_Move_ID:
																						chm = 6;	//对应模拟通道7通道
																						break;
				case Right_Move_ID:
																						chm = 7;	//对应模拟通道8通道
																						break;
				default:
																						printf("\r\nError:The ADGet_Data channel is false!!!\r\n");
																						chm = 0; //默认出错时对应模拟通道1 
																						break;				
		}
	  temp = (float)AD_SumAdjust[chm] / Window_Adjust;
		temp1 = temp + 0.5;		
		return(temp1);
}
/*******************************************************************************
* @函数名称	：ADShow_Samp
* @函数说明 ： 实际采样有效值
* @输入参数 ：chm:通道号  
* @输出参数 ：无
* @返回参数 ：无
*******************************************************************************/
uint16_t ADShow_Samp(uint8_t chm)
{
		float temp;
		uint16_t temp1;
		switch(chm)
		{
				case Left_Weight_ID:
																						chm = 0;	//对应模拟通道1通道
																						break;
				case Right_Wight_ID:
																						chm = 1;	//对应模拟通道2通道
																						break;
				case Left_Brake_ID:
																						chm = 2;	//对应模拟通道3通道
																						break;
				case Right_Brake_ID:
																						chm = 3;	//对应模拟通道4通道
																						break;
				case All_Weight_ID:
																						chm = 4;	//对应模拟通道5通道
																						break;
				case FootPlat_ID:
																						chm = 5;	//对应模拟通道6通道
																						break;
				case Left_Move_ID:
																						chm = 6;	//对应模拟通道7通道
																						break;
				case Right_Move_ID:
																						chm = 7;	//对应模拟通道8通道
																						break;
				default:
																						printf("\r\nError:The ADGet_Data channel is false!!!\r\n");
																						chm = 0; //默认出错时对应模拟通道1 
																						break;				
		}
	  temp = (float)AD_SumSamp[chm] / Window_Samp;
		temp1 = temp + 0.5;
		return(temp1);
}
/*******************************************************************************
* @函数名称	：TIM4_ISR
* @函数说明 ：定时器中断服务函数，定时采集通道模拟量存放在队列中
* @输入参数 ：无
* @输出参数 ：无
* @返回参数 ：无
*******************************************************************************/
void TIM4_ISR(void)
{
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
		{
				ADGet_Data(Left_Brake_ID);
				ADGet_Data(Right_Brake_ID);         //根据自己的需要添加循环采集的通道数量
				ADGet_Data(All_Weight_ID);
				TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		}
}




