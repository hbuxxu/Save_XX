/****************************************************************************
* 文件名: user_data.c
* 文件历史:
* 版本号:    V1.0
* 日期:      2017.02.09    
* 作者:      xx
* 说明:   全局变量定义和参数设置
*****************************************************************************/
/*=============================================================================
*******修改日志*******
1、日    期：
地    点：
修 改 人：
添加内容：
版本号:
================================================================================*/
#include "user_data.h"

/* 全局变量 */

uint8_t System_State;               //系统检测状态标志
_unionFlag USER_SystemFlag; //定时器各个标志位
_unionFlagLED USER_LEDFlag;						   //	系统指示灯标志

uint8_t WorkState_Flg;              //工作状态标志 分为待机状态  标定状态  信号测试状态  检测状态

uint8_t Reset_Flag=0;   							//复位计数变量

uint8_t Network0_StopFlg=0;       //网络断开标识变量  主控心跳
uint8_t Network1_StopFlg=0;       //网络断开标识变量  数码管心跳

uint8_t cID_Key[32] = {0};            //Key值  加密字符
uint8_t Encryption_Cnt = 0;          //加密次数

/* 标定状态选择转换通道变量 */
uint8_t AD_Channel_ID; //AD转换通道ID

/* Flash保存  系数使用变量 */
_unionType1 Left_Weight_par,  //左重系数 0通道    17字节数据
										 Right_Wight_par,  //右重系数 1通道
										 Left_Brake_par,    //左制系数 2通道
										 Right_Brake_par,  //右制系数 3通道
										 All_Weight_par,     //复合重量系数  4通道
										 FootPlat_par,        //踏板力通道  5通道
										 Left_Move_par,    //左举升高度   6通道
										 Right_Move_par;  //右举升高度   7通道
										 
_structParameter SysParameter;    //系统参数变量									 
										 
/* 变量：零点电压 */										 
uint16_t   Zero_LeftWeight,
							 Zero_RightWight,
							 Zero_LeftBrake,
							 Zero_RightBrake,
							 Zero_AllWeight,
							 Zero_FootPlat,
							 Zero_LeftMove,
							 Zero_RightMove;
							 
/* 信号测试上传频率变量 */								 
vu8 Signal_TransferTime;							 
							 
/* 定时器2定时变量 */		
vu16  TIM2_1ms;  //定时器ms计数  用与测试倒计时时使用
vu32 TIM2_count; //定时器2用作延时1ms定时   全局使用。
vu32 TIM2_count_Delay; //定时器2用作延时函数使用。
vu16 TIM2_count1 = 0;
vu16 TIM2_count2 = 0;
vu16 TIM2_count3 = 0;
vu16 TIM2_count4 = 0;

/* 变量：重量 制动*/
 uint16_t	u16Left1_TemporaryWeight[300] = {0},
							u16Right1_TemporaryWeight[300] = {0}; //左右轮重10数据平均后存储位置  临时变量
 uint16_t  Left_DynamicWeight,   //左动态轮重 动态显示使用
								Right_DynamicWeight, //右动态轮重 动态显示使用								
								All_DynamicWeight, 		//动态复合台称重 动态显示使用
								DynamicFootPlat, 				//动态踏板力 动态显示使用
							  Left_DynamicBrake, 		//左动态制动 动态显示使用
							  Right_DynamicBrake; 	//右动态制动 动态显示使用
							
							
 uint8_t  DynamicWeight_Number;//动态检测轴的数量 最多6轴				

 uint16_t 	 Left_DynamicWeight1[6],   //左动态检测轮重
								 Right_DynamicWeight1[6]; //右动态检测轮重
 uint16_t   Left_StaticWeight,				//左轮静态轴重 结果值
							  Right_StaticWeight,  		//右轮静态轴重 结果值
							  Motor_StaticWeight, 		//摩托静态轴重 左台架
							  AllNone_StaticWeight, //空载复合台重量
								AllFull_StaticWeight;	  //加载复合台重量
							
uint16_t u16Protect_Limit;           //阻滞力保护阀值								
							
uint16_t u16Left_Data[802],    	//左制动过程数据
							u16Right_Data[802];    //右制动过程数据
uint8_t u8Tran_CurrentZ[2010],				//阻滞上传数据
					  u8Tran_CurrentC[3250],      //空载制动数据
						u8Tran_CurrentJ[3250],       //加载制动数据
						u8Tran_CurrentN[3250];       //踏板力数据



/***************----- 端口数据缓冲区 -----***************/
uint8_t Rx_Buffer[500];	//端口接收数据缓冲区 
uint8_t Tx_Buffer[500];	//端口发送数据缓冲区 	

uint8_t Parameter_Buffer[100];	//参数数据处理缓冲区 	

uint8_t u8Send_Buffer[BUFFER_SIZE];

/***************----- IP参数设置 -----***************/
uint8_t W5500_Parameter[27];    //网络IP参数变量
									 
/*************网口发送变量**********************/ 
	
 uint8_t  con_TranB[19]  = { 0x11,0x00,'B','V','E','0','1','0','2','0','3','2','0','1','7','0','2','2','4'};  // 版本信息VE01020320170224
	
 uint8_t con_TranAACK[3]  = { 0x01, 0x00, 'A' };   			// 正确应答
 uint8_t con_TranZACK[3]  = { 0x01, 0x00, 'Z' };   			// 错误应答
 uint8_t con_TranXACK[3]  = { 0x01, 0x00, 'X' };   			// 当前命令无法执行

 uint8_t con_TranU[3]  = { 0x01, 0x00, 'U' };   			     // 结束
 uint8_t con_TranI[3]  = { 0x01, 0x00, 'I' };   			         // 初始化完成
 uint8_t con_TranR[3]  = { 0x01, 0x00, 'R' };   			     // 复位完成
 
 
 uint8_t con_TranMI[8] = { 0x02, 0x00, 'M', 'I'}; 			    // 上传M I  UDP广播
 uint8_t con_TranMW[5] = { 0x03, 0x00, 'M', 'W','1'}; 			// 上传M W静态轮重检测
 uint8_t con_TranMA[5] = { 0x03, 0x00, 'M', 'W', '1'}; 	// 上传M A动态轮重检测
 uint8_t con_TranMF1[5] = { 0x03, 0x00, 'M', 'F', '1'}; // 上传M F 1复合称重检测
 uint8_t con_TranMF2[5] = { 0x03, 0x00, 'M', 'F', '2'}; // 上传M F 2加载举升检测
 uint8_t con_TranML[4] = { 0x02, 0x00, 'M', 'L'}; 				// 上传M L 摩托车称重检测 
 uint8_t con_TranMM[4] = { 0x02, 0x00, 'M', 'M'}; 			// 上传M M 摩托车制动检测
 uint8_t con_TranMD[4] = { 0x02, 0x00, 'M', 'D'}; 				// 上传进入电机启动流程
 uint8_t con_TranMZ[4] = { 0x02, 0x00, 'M', 'Z'}; 				// 上传进入阻滞测试流程
 uint8_t con_TranMB[4] = { 0x02, 0x00, 'M', 'B'}; 				// 上传进入空载制动检测
 uint8_t con_TranMJ[4] = { 0x02, 0x00, 'M', 'J'}; 				// 上传进入加载制动检测
 
 
 uint8_t con_TranT0[4] = { 0x02, 0x00, 'T', '0'};     			//正常检测结束 
 uint8_t con_TranT1[4] = { 0x02, 0x00, 'T', '1'};     			//左轮阻滞大 测试失败
 uint8_t con_TranT2[4] = { 0x02, 0x00, 'T', '2'};     			//右轮阻滞大 测试失败
 uint8_t con_TranT3[4] = { 0x02, 0x00, 'T', '3'};     			//加载举升超时 未到位
 uint8_t con_TranT4[4] = { 0x02, 0x00, 'T', '4'};     			//车辆不到位错误码 
	
 uint8_t con_TranT7[4] = { 0x02, 0x00, 'T', '7'};     			//左轮脉冲异常
 uint8_t con_TranT8[4] = { 0x02, 0x00, 'T', '8'};    			 	//右轮脉冲异常
 uint8_t con_TranT9[4] = { 0x02, 0x00, 'T', '9'};     			//轮重值太小
 uint8_t con_TranTD[4] = { 0x02, 0x00, 'T', 'D'};     			//电机启动完成
 uint8_t con_TranTF[4] = { 0x02, 0x00, 'T', 'F'};     			//加载举升完成
 
 uint8_t con_TranS[5] = { 0x03, 0x00, 'S', 'S','1'};     	//状态回传
 
 uint8_t con_TranWG[4] = { 0x02, 0x00, 'W', 'G'};       //升级请求回传
 uint8_t con_TranWE[6] = { 0x04, 0x00, 'W', 'E',0x00,0x00};       //升级异常WE00
 
 uint8_t con_TranE1W[5] = { 0x03, 0x00, 'E', '1','W'};  //参数1设置成功
 uint8_t con_TranE2W[5] = { 0x03, 0x00, 'E', '2','W'};  //参数2设置成功
 uint8_t con_TranEIW[5] = { 0x03, 0x00, 'E', 'I','W'};    //参数IP设置成功
 uint8_t con_TranHX0[5] = { 0x03, 0x00, 'H', 'X','0'};    //加密指令错误
 uint8_t con_TranHX1[5] = { 0x03, 0x00, 'H', 'X','1'};    //加密超次数
 uint8_t con_TranHEF[5] = { 0x03, 0x00, 'H', 'E','F'};    //加密失败
 uint8_t con_TranHET[5] = { 0x03, 0x00, 'H', 'E','T'};    //加密成功
 uint8_t con_TranF[29] = { 29, 0x00, 'F', 102,102,0};    //F指令设备信息
 uint8_t con_TranO[3] = { 0x01, 0x00, 'O'};              //心跳指令
 
 