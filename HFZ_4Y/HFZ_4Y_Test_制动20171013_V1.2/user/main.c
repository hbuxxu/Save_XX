#include "stm32f10x.h"

int main(void)
{			
		__enable_irq();  //为升级程序做处理
		IO_GpioConfig();  //IO配置 初始化
		Usart_Init();
		ADC_7663_Init();
		SPI_FLASH_Init();		//W25q64存储芯片配置
		Get_Parameter();     //必须在网络配置之前
		W5500_Init_Config();//必须在ADC_7663_Init()之后	  
// 		SysParameter.DynamoStart_Flag = 0;		
		Left_Machine = IO_CLOSE;
		Right_Machine = IO_CLOSE;
		Machine_Switch = IO_CLOSE;
		Left_Up = IO_CLOSE;  			//左台架举关闭
		Left_Down = IO_CLOSE;   //左台架降关闭
		Hydraumatic_Machine = IO_CLOSE;
		
	    WorkState_Flg = Default_State_Flg;//初始化  待机工作状态
		//系统标志位清0
		USER_SystemFlag.Init = 0;
		USER_LEDFlag.Init = 0;
    //指示灯标志位
		USER_LEDFlag.A.LED_Online = 0;  				//联网指示灯
		USER_LEDFlag.A.LED_Test = 0;  					//检测指示灯
		USER_LEDFlag.A.LED_Weight = 1;  			//重量指示灯
		USER_LEDFlag.A.LED_Brake = 0; 					//制动指示灯
		USER_LEDFlag.A.LED_PlugMinus = 0;   //和/差指示灯
		USER_LEDFlag.A.LED_Block = 0;  					//阻滞指示灯
		USER_LEDFlag.A.LED_Up = 1;  							//出车举指示灯
		USER_LEDFlag.A.LED_Down = 0;  				//出车降指示灯
		USER_LEDFlag.A.LED_TJUp = 0;  					//台架举指示灯
		USER_LEDFlag.A.LED_TJDown = 1;  		//台架降指示灯
		InitADScreen();
		TIM4_NVIC_Config();
//		SystemErr_Init();  //故障代码初始化
//		SystemErr_Test(); //开机自检
		TIM2_count_Delay=0;
        USER_SystemFlag.A.Flag_Sucess1 = 1;
		IWDG_Configuration();  //看门狗配置
		while(TIM2_count_Delay < 8000)
		{
			 W5500_Receive_Deal();
			 IWDG_ReloadCounter();   //看门狗清零
		}
		Fun_Zero(Left_Weight_ID,1);//调零
		Fun_Zero(Right_Wight_ID,1);//调零
	    Fun_Zero(Left_Brake_ID,1);//调零
		Fun_Zero(Right_Brake_ID,1);//调零	
		Fun_Zero(All_Weight_ID,1);//调零
//		SystemErr_Init();  //故障代码初始化
//		SystemErr_Test(); //开机自检
    while(1)
    {								
					W5500_Receive_Deal();
// 					Left_Machine = IO_CLOSE;
// 					Right_Machine = IO_CLOSE;
// 					Machine_Switch = IO_CLOSE;
// 					System_State = StaticWeight_State;
					switch(System_State)
					{
							/* 标定 */
							case Adjust_State:
																												Fun_Adjust();
																												break;
							/* 信号测试 */
							case SignalTest_State:
																												Fun_SignalTest();
																												break;
// 							/* 静态轮重 */
// 							case StaticWeight_State:   
// 																												Fun_StaticWeightTest();							
// 																												break;
// 							/* 动态轮重 */
// 							case DynamicWeight_State:   
// 																												Fun_DynamicWeightTest();							
// 																												break;
// 							/* 摩托车称重 */
// 							case StaticWeight_Motor_State:
// 																												Fun_MotorWeightTest();							
// 																												break;
							/* 空载复合重量 */
							case StaticWeight_None_State:  
																												Fun_AllWeightTest();							
																												break;
								/* 加载举升控制 */
							case StaticWeight_Full_State:  
																												Fun_AllWeightTestJJ();							
																												break;
							/* 阻滞测试 */
							case Retardant_State:
																												Fun_TestZZ();
																												break;							
							/* 空载制动测试 */
							case BrakeTest_None_State:
																												Fun_TestCC();
																												break;
							/* 加载制动测试 */
							case BrakeTest_Full_State:
																												Fun_TestJJ();
																												break;
							/* 默认待机状态 */
							case Default_State:
																												Fun_Stable();
																												break;
							/* 读取数据状态 */
							case ReadData_State:
																												Fun_ReadData();
																												break;
							default :
													 System_State = Default_State;
													 break;
					}					
    }
}
