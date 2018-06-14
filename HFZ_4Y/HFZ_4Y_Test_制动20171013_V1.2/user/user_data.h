/****************************************************************************
* 文件名: AD7663.h
* 文件历史:
* 版本号:    V1.0
* 日期:      2016.12.09    
* 作者:      XX
* 说明:   全局变量定义、参数
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
#ifndef __USER_DATA_H
#define __USER_DATA_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif
/* 定义全局中断优先级 */
#define 		NVIC_PriorityGroup_Set 			NVIC_PriorityGroup_1
	 
#define    TIM2_IRQChannelPreemptionPriority   					 1   //设置抢先优先级 根据中断组一共有多少设置
#define    TIM2_IRQChannelSubPriority   											 0	 	//设置响应优先级
	 
#define    TIM3_IRQChannelPreemptionPriority   					 1   //设置抢先优先级 根据中断组一共有多少设置
#define    TIM3_IRQChannelSubPriority   											 2	 	//设置响应优先级	 
	 
#define    EXTI15_10_IRQnChannelPreemptionPriority   0   //设置抢先优先级   AD转换完成中断
#define    EXTI15_10_IRQnChannelSubPriority   						0	 	//设置响应优先级	 
	 
#define    EXTI9_5_IRQnChannelPreemptionPriority   		1   //设置抢先优先级    W5500接收数据完成中断
#define    EXTI9_5_IRQnChannelSubPriority   								1	 	//设置响应优先级
	 	 
/* 定义全局复位按键变量 */	 
extern uint8_t Reset_Flag;   //复位计数变量	 
#define	 RESET_MAX   10    //5S
	 
/* 定义全局网络断开变量 */
extern uint8_t Network0_StopFlg;       //网络断开标识变量  主控心跳
extern uint8_t Network1_StopFlg;       //网络断开标识变量  数码管心跳
#define	 NETWORK_MAX   3              //2*3S

/* 定义全局加密变量 */
extern uint8_t cID_Key[32];            //Key值  加密字符
extern uint8_t Encryption_Cnt;        //加密次数
#define	 ENCRYPTION_MAX   10   //最大加密次数	 
	 
/* 定义全局各个检测状态标志 */	 
extern uint8_t System_State;   //系统检测状态标志	 
#define  Default_State																0     //默认状态
#define  Adjust_State         												1		 //标定状态
#define  StaticWeight_State      								2		 //静态轮重称重
#define  DynamicWeight_State      						3		 //动态轮重称重

#define  StaticWeight_Motor_State      			4		 //摩托车静态轮重称重 
#define  StaticWeight_None_State      			5		 //空载复合台静态称重 
#define  StaticWeight_Full_State      			    6		 //加载复合台静态称重 加载举升
#define  SignalTest_State                          7     //信号测试状态标识
#define  DynamoStart_State                      8     //电机启动标识
#define  Retardant_State                           9     //阻滞测试标识
#define  BrakeTest_None_State                10   //空载制动测试标识
#define  BrakeTest_Full_State                   11   //加载制动测试标识
#define  ReadData_State                           12   //读取数据员标识

/* 定义全局工作模式 响应 */	 	 
extern	uint8_t WorkState_Flg;              //工作状态标志 分为待机状态  标定状态  信号测试状态  检测状态

#define  Default_State_Flg														    0    //默认状态
#define  Adjust_State_Flg	         									    1		 //标定状态
#define  SignalTest_State_Flg	      						      2		 //信号测试状态

#define  StaticWeight_State_Flg	      							3		 //轮重检测状态
#define  StaticWeight_Motor_State_Flg      			4		 //摩托车静态轮重称重 状态

#define  StaticWeight_None_State_Flg      			5		 //空载复合台静态称重 状态
#define  StaticWeight_Full_State_Flg      			  6		 //加载复合台静态称重 加载举升状态
// #define  DynamoStart_State_Flg                     8     //电机启动状态
#define  Retardant_State_Flg                          7     //阻滞测试状态
#define  BrakeTest_None_State_Flg               8   //空载制动测试状态
#define  BrakeTest_Full_State_Flg                  9   //加载制动测试状态
#define  BrakeTest_Motor_State_Flg              10   //摩托制动测试状态
	 
/* 定义模拟量通道ID标识 */	  
extern uint8_t AD_Channel_ID; //AD转换通道ID  标定时使用
#define Left_Weight_ID     0x05  	//左轮重
#define Right_Wight_ID			0x06		//右轮重
#define Left_Brake_ID			  0x07		//左制动
#define Right_Brake_ID     0x08		//右制动
#define All_Weight_ID        0x2B		//复合称重信号
#define FootPlat_ID           0x09		//踏板力信号
#define Left_Move_ID				44		    //左高
#define Right_Move_ID			45		    //右高

#define AD_Clear_ID      0xFF       //所有通道清零ID

/* 定义开入通道ID标识 */
#define IN_Weight_ID     		0x05  	//轮重到位信号开入ID  到位信号到位1  不到位为0
#define IN_Brake_ID					0x06		//制动到位信号开入ID
#define IN_Raise_ID			  	0x25		//举升到位信号开入ID
#define IN_Protect_ID     	0x15		//盖板保护信号开入ID
#define IN_LeftStop_ID      0x09		//左停机信号开入ID
#define IN_RightStop_ID    0x0A		//右停机信号开入ID




/* 参数存储在W25Q64首地址 一共2048个扇区 */	
/* 参数保存首地址 */
// #define Parameter0_Flash_Address       0     //第0扇区
// #define Parameter1_Flash_Address       1     //第1扇区
// #define Parameter2_Flash_Address       2     //第2扇区
// #define Parameter3_Flash_Address       3     //第3扇区
// #define Parameter4_Flash_Address       4     //第4扇区
// #define Parameter5_Flash_Address       5     //第5扇区
// #define Parameter6_Flash_Address       6     //第6扇区
// #define Parameter7_Flash_Address       7     //第7扇区
// #define Parameter8_Flash_Address       8     //第8扇区
// #define Parameter9_Flash_Address       9     //第9扇区
// #define Parameter10_Flash_Address     10     //第10扇区
// #define Parameter11_Flash_Address     11     //第11扇区
// #define Parameter12_Flash_Address     12     //第12扇区
// #define Parameter13_Flash_Address     13     //第13扇区
// #define Parameter14_Flash_Address     14     //第14扇区
#define ParameterAll_Flash_Address     0     //第14扇区
#define Parameter15_Flash_Address    15     //第15扇区  出厂编号

#define ParameterIP_Flash_Address     20     //第20扇区

#define ParameterERR_Flash_Address     1     //第1扇区  故障代码存储扇区
/* 模拟量通道系数保存地址 */
#define Left_Weight_Flash_Address       21     //第21扇区
#define Right_Wight_Flash_Address				22		 //第22扇区
#define Left_Brake_Flash_Address					23
#define Right_Brake_Flash_Address				24
#define All_Weight_Flash_Address					25
#define FootPlat_Flash_Address							26
#define Left_Move_Flash_Address					27
#define Right_Move_Flash_Address				28
/* 模拟量通道系数标定点上传地址 */
#define Left_Weight_Flash_Address1       29     //第29扇区
#define Right_Wight_Flash_Address1				30		 //第30扇区
#define Left_Brake_Flash_Address1					31
#define Right_Brake_Flash_Address1				32
#define All_Weight_Flash_Address1					33
#define FootPlat_Flash_Address1							34
#define Left_Move_Flash_Address1					35
#define Right_Move_Flash_Address1				36


/* 模拟量通道测量值阀值 */
#define Left_Weight_Limit       9999       //Kg
#define Right_Wight_Limit			  9999		   //Kg
#define Left_Brake_Limit				50000		 // N
#define Right_Brake_Limit				50000		 // N
#define All_Weight_Limit				20000		 // Kg
#define FootPlat_Limit					9999			 // N
#define Left_Move_Limit					999				 //mm
#define Right_Move_Limit				999

/* 软停机停机点数 */
#define con_LDN20   30  					// 左电机20%到60%期间停机点数
#define con_LDN60   10  					// 左电机60%以上期间停机点数
#define con_LDNStop 10  					// 左电机停机后采样点数 硬停机
// #define con_LDNHuan 20  			// 左电机60%以上期间缓踩停机点数
// #define con_LDNJi   10  				// 左电机60%以上期间急踩停机点数


#define con_RDN20   30  					// 右电机20%到60%期间停机点数
#define con_RDN60   10  					// 右电机60%以上期间停机点数
#define con_RDNStop 10  				// 右电机停机后采样点数  硬停机
// #define con_RDNHuan 20  			// 右电机60%以上期间缓踩停机点数
// #define con_RDNJi   10  				// 右电机60%以上期间急踩停机点数

/***************----- 端口数据缓冲区 -----***************/
#define   BUFFER_SIZE       3300   //网口发送  接收缓冲区大小  和  曲线缓冲区大小

extern uint8_t Parameter_Buffer[100];	//参数数据处理缓冲区 	

extern uint8_t Rx_Buffer[500];	//端口接收数据缓冲区 
extern uint8_t Tx_Buffer[500];	//端口发送数据缓冲区 	

extern uint8_t u8Send_Buffer[BUFFER_SIZE]; //网口发送函数变量
	



typedef union 
{
	struct
	{
		u8 Flag_1ms      								:1;		 		// 1ms一次
		u8 Flag_10ms										:1;		 		// 10mS一次循环
		u8 Flag_100ms  								:1;		 		// 100mS标志
		u8 Flag_500ms   								:1;     	  //500ms  实际330ms
		u8 Flag_1s         								:1;		 		// 1s倒计时使用
		u8 Flag_UDP      								:1;	   		// UDP广播时间间隔
		u8 Flag_OverTime_Enable  :1;					//串口接收数据超时标志使能
		u8 Flag_Online  										:1;          //和主控机联网单机标志 1联网   0单机
		u8 Flag_Standalone							:1;          //和数码管联网成功标志 1联网   0失败
		u8 Flag_TestC        							:1;          //上位机测试曲线数据标志    表示检测过程中可以上传数据
		u8 Flag_TestC1        						:1;          //上位机测试曲线数据标志1  接收到上位机指令后置1 表示本次检测过程上传检测数据
		u8 Flag_Ldynamo        				:1;          //左电机
		u8 Flag_Rdynamo        				:1;          //右电机
		u8 Flag_Switch       							:1;          //星三角
		u8 Flag_ST        									:1;          //停机板启动
		u8 Flag_Sucess                   :1;      //握手加密成功标志
		u8 Flag_AutoD                      :1;      //检测结果是否主动上传
    u8 Flag_Sucess1                    :1;      //数码管握手加密成功是否
	}A;
	u32 Init;                       //初始化使用
} _unionFlag;

extern _unionFlag USER_SystemFlag;						   //	系统标志位1
	
typedef union 
{
	struct
	{
		u8 LED_TJDown      :1;		 		// 台架降    
		u8 LED_TJUp				  :1;		 		// 台架举
		u8 LED_Down  				:1;		 		// 出车降
		u8 LED_Up   						:1;     	  //出车举
		u8 LED_Block         	:1;		 		//阻滞
		u8 LED_PlugMinus   :1;	   		//和/差
		u8 LED_Brake  				 :1;				//制动
		u8 LED_Weight  			 :1;        //重量
		u8 LED_Test						 :1;          //检测
		u8 LED_Online					 :1;          //联网
		u8 LED_10               :1;         //保留
		u8 LED_11               :1;         //保留
		u8 LED_12               :1;         //保留
		u8 LED_13               :1;         //保留
		u8 LED_14               :1;         //数码管指示灯是否改变   1改变 0不变
		u8 LED_15               :1;         //数码管左右数据是否改变 1改变  0不变
	}A;
	u16 Init;                       //初始化使用
} _unionFlagLED;

extern _unionFlagLED USER_LEDFlag;						   //	系统指示灯标志

typedef union 
{
		uint8_t u8_data[4];
		float float_data;
} _union_float_u8_type; //共用体  float型转换成u8

typedef union 
{
		uint8_t u8_data[2];
		uint16_t  u16_data;
} _union_u16_u8_type; //共用体  u16型转换成u8

#pragma pack(1)  //存储格式对齐
typedef union  
{
    struct 
    {				
			  uint8_t                        Parameter_NB; //拟合次数			
				_union_float_u8_type Parameter_C0; //第零标定系数4字节
			  _union_float_u8_type Parameter_C1; //第一标定系数4字节
				_union_float_u8_type Parameter_C2; //第一标定系数4字节
				_union_float_u8_type Parameter_C3; //第一标定系数4字节
    } 
		Load_Parameter;
		uint8_t 	Load_Flash[17];  //flash保存数据
} _unionType1; //

/*  */
typedef   struct 
{				
		uint8_t Retardant_Flag;            //阻滞测试完成是否电机停止标志
		uint8_t DynamoStart_Flag;				//电机启动标志  待机时是否硬停机判断
		uint8_t Par0_WeightTestTime; //静态称重倒计时			s
		uint8_t Par1_RollerFlag; 				  //第三滚筒有无 有效无效，1有效  0无效
		uint8_t Par2_FootPlatFlag; 			//踏板力有无
		uint8_t Par3_BrakeReadyTime;//制动倒计准备时间 s
		uint8_t Par4_ZuzhiTestTime; 		 //阻滞测试时间	s
		uint8_t Par5_BrakeTestTime;		 //制动测试时间 s
		uint8_t Par6_Weight;		 						 //称重台分度值 1  2  5
		uint16_t Par7_LeftAverZero;      //左动态平均零值 daN
		uint16_t Par8_RightAverZero;    //右动态平均零值 daN
		uint8_t Par9_LeftMaxZero;       //左动态最大零值 daN
		uint8_t Par10_RightMaxZero;   //右动态最大零值 daN 
		uint16_t Par11_MaxWeight;      //加载重量阀值  *10Kg
		uint8_t Par12_MachineType;    //机械台架类型 0:轮重台  1:普通制动台架  2:气动加载    3:液压加载  4：摩托车
		uint16_t Par13_UpHeight;    		 //台架举升高度 mm
		uint8_t Par14_UpTime;    			   //台架举升时间 s
																										 //Par20 IP参数定义在W5500.h
} _structParameter; 			//参数结构体
#pragma pack()	

/* 变量：模拟量通道系数*/		
extern _unionType1  Left_Weight_par,  //左重系数 1通道
																 Right_Wight_par,  //右重系数 2通道
																 Left_Brake_par,    //左制系数 3通道
																 Right_Brake_par,  //右制系数 4通道
																 All_Weight_par,     //复合重量系数  5通道
																 FootPlat_par,        //踏板力通道  6通道
																 Left_Move_par,    //左举升高度   7通道
																 Right_Move_par;  //右举升高度   8通道

extern _structParameter SysParameter;    //系统参数变量						

/* 变量：零点电压 */			
#define Zero_Max 1000  //零点电压最大值  超过之后零点不正常
extern uint16_t  Zero_LeftWeight,
												 Zero_RightWight,
												 Zero_LeftBrake,
												 Zero_RightBrake,
												 Zero_AllWeight,
												 Zero_FootPlat,
												 Zero_LeftMove,
												 Zero_RightMove;
/* 信号测试上传频率变量 */								 
extern vu8 Signal_TransferTime;	

extern vu16  TIM2_1ms;  //定时器ms计数  用与测试倒计时时使用
extern vu32 TIM2_count; //定时器2用作延时函数变量计数，1ms定时。
extern vu32 TIM2_count_Delay; //定时器2用作延时函数变量计数，1ms定时。
extern vu16 TIM2_count1;
extern vu16 TIM2_count2;
extern vu16 TIM2_count3;
extern vu16 TIM2_count4;


/* 变量：重量 制动*/
#define StaticWeight_Init   200   //默认静态轮重  电机启动使用 


extern  uint16_t	u16Left1_TemporaryWeight[300],
													u16Right1_TemporaryWeight[300]; //左右轮重10数据平均后存储位置  临时变量
													
extern uint16_t  Left_DynamicWeight,   //左动态轮重 动态显示使用
													Right_DynamicWeight, //右动态轮重 动态显示使用
													All_DynamicWeight, 		//动态复合台称重 动态显示使用
													DynamicFootPlat, 				//动态踏板力 动态显示使用
													Left_DynamicBrake, 		//左动态制动 动态显示使用
													Right_DynamicBrake; 	//右动态制动 动态显示使用																				
													
													
extern uint8_t  DynamicWeight_Number;//动态检测轴的数量 最多6轴																							
								
extern uint16_t 	 Left_DynamicWeight1[6],   //左动态检测轮重
													 Right_DynamicWeight1[6]; //右动态检测轮重								
extern  uint16_t   Left_StaticWeight,				//左轮静态轴重 
									 Right_StaticWeight,  		//右轮静态轴重
									 Motor_StaticWeight, 		//摩托静态轴重 左台架
									 AllNone_StaticWeight, 	//空载复合台重量
									 AllFull_StaticWeight,	  //加载复合台重量
									 Left_BrakeSum,           //左制动力最大值  空载制动
									 Right_BrakeSum,         //右制动力最大值  空载制动
									 Left_BrakeSub,           //左制差最大值  空载制动
									 Right_BrakeSub,         //右制差最大值  空载制动
									 Left_BrakeSumJJ,       //左制动力最大值  加载制动
									 Right_BrakeSumJJ,     //右制动力最大值  加载制动
									 Left_BrakeSubJJ,        //左制差最大值  加载制动
									 Right_BrakeSubJJ,      //右制差最大值  加载制动
									 Left_BrakeAve,           //左阻滞
									 Right_BrakeAve;         //右阻滞

//左右动态零值								
extern uint16_t u16Left_BrakeZero, //左动态零值
				        u16Right_BrakeZero;//右动态零值

//停机点数和停机类型
extern _union_u16_u8_type	Left_StopTime,Right_StopTime;//左右停机时刻
extern uint8_t Left_StopFlag,Right_StopFlag;					 //左右停机类型标志 ‘1’硬停机  ‘0’软停机
	
extern uint16_t u16Protect_Limit;   					//阻滞力保护阀值		
													 
extern uint16_t u16Left_Data[802],    						//左制动过程数据
												u16Right_Data[802];    					//右制动过程数据
extern uint8_t u8Tran_CurrentZ[3250],				//阻滞上传数据
											u8Tran_CurrentC[3250],      	//空载制动数据
											u8Tran_CurrentJ[3250],       	//加载制动数据
											u8Tran_CurrentN[3250];       	//踏板力数据					

/***************----- IP参数设置 -----***************/
extern uint8_t W5500_Parameter[27];       //网络IP参数变量				最后一位存储是否第一次修改IP  0x55表示保存过IP				
													 
/*************网口发送变量**********************/ 
extern  uint8_t con_TranB[19] ;   // 版本信息VE01020320170224
extern  uint8_t con_TranAACK[3];   // 正确应答
extern  uint8_t con_TranZACK[3];   // 错误应答
extern  uint8_t con_TranXACK[3];   // 当前命令无法执行
extern  uint8_t con_TranKACK[6];   //升级请求回传


extern  uint8_t con_TranU[3];          // 结束
extern  uint8_t con_TranI[3];           // 初始化完成
extern  uint8_t con_TranR[3];         // 复位完成

extern uint8_t con_TranMI[8]; 			// 上传M I  UDP广播
extern  uint8_t con_TranMW[5];  // 上传M W静态轮重检测
extern  uint8_t con_TranMA[5];  // 上传M A动态轮重检测
extern  uint8_t con_TranMF1[5]; // 上传M F 1复合称重检测
extern  uint8_t con_TranMF2[5]; // 上传M F 2加载举升检测
extern  uint8_t con_TranML[4];   // // 上传M L 摩托车称重检测 
extern  uint8_t con_TranMM[4];  // 上传M M 摩托车制动检测
extern uint8_t con_TranMD[4];   // 上传M D电机启动流程
extern uint8_t con_TranMZ[4];   // 上传M Z阻滞测试流程
extern uint8_t con_TranMB[4];   // 上传M B空载制动测试流程
extern uint8_t con_TranMJ[4];   // 上传M J加载制动测试流程
extern uint8_t con_TranM2[4];   // 上传M 2踩

extern  uint8_t con_TranT0[4];    //  正常检测结束
extern  uint8_t con_TranT1[4];    //左轮阻滞大 测试失败
extern  uint8_t con_TranT2[4];    //右轮阻滞大 测试失败
extern  uint8_t con_TranT3[4];    // 加载举升超时 不到位
extern  uint8_t con_TranT4[4];    //  车辆不到位
extern  uint8_t con_TranT7[4];    //左轮脉冲异常
extern  uint8_t con_TranT8[4];    //右轮脉冲异常
extern  uint8_t con_TranT9[4];    //轮重值太小
extern  uint8_t con_TranTD[4];    //电机启动完成
extern  uint8_t con_TranTF[4];    //加载举升完成

extern  uint8_t con_TranS[5];      //状态回传指令

extern  uint8_t con_TranE1W[5];      //参数1设置成功回传指令
extern  uint8_t con_TranE2W[5];      //参数2设置成功回传指令
extern  uint8_t con_TranEIW[5];       //参数IP设置成功回传指令
extern uint8_t con_TranHX0[5];    		 //加密指令错误
extern uint8_t con_TranHX1[5];    		 //加密超次数
extern uint8_t con_TranHEF[5];    		 //加密失败
extern uint8_t con_TranHET[5];    		 //加密成功

extern uint8_t con_TranWG[4];       //升级请求回传
extern uint8_t con_TranWE[6];       //升级异常WE00

extern uint8_t con_TranF[29];         //F指令设备信息
extern uint8_t con_TranO[3];          //心跳指令

#ifdef __cplusplus
}
#endif
#endif /* __AD7663_H */

