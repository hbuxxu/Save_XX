/****************************************************************************
* 文件名: W5500_APP.c
* 文件历史:
* 版本号    V1.0
* 日期      2016.09.29    
* 作者      qgk
* 说明      
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
#include "W5500.h"	
#include "W5500_APP.h"
unsigned char ucW5500_InitCTR=0;
FLASH_Status WFLASHStatus = FLASH_COMPLETE;

// static uint8_t u8flash_buffer[21];//标定系数  参数临时存储变量
static	_ADCab _ADCabVariable;  //曲线拟合变量
/*******************************************************************************
* 函数名  : IDtoMAC
* 描述    : 读取STM32F10X ID后六位作为MAC地址
* 输入    : *ulp 指向MAC地址数组的指针
* 输出    : 无
* 返回值  : 无
* 说明    : 若Phy_Addr[]为存储MAC的数组，使用IDtoMAC(Phy_Addr)调用
*******************************************************************************/
void IDtoMAC(u8 *ulp)	
{
	u32 ultmp1,ultmp2;

    //读取产品ID
	ultmp1=*(__IO u32*)(0x1FFFF7EC);
	ultmp2=*(__IO u32*)(0x1FFFF7F0);

	*ulp++ = (u8)(((ultmp1 & 0x00FF0000)>>16)&0xfe);	//高字节最低位为零
	*ulp++ = (u8) ((ultmp1 & 0xFF000000)>>24);
	*ulp++ = (u8)  (ultmp2 & 0x000000FF);
	*ulp++ = (u8) ((ultmp2 & 0x0000FF00)>>8);
	*ulp++ = (u8) ((ultmp2 & 0x00FF0000)>>16);
	*ulp =   (u8) ((ultmp2 & 0xFF000000)>>24);
}

/*******************************************************************************
* 函数名  : Load_Net_Parameters
* 描述    : 装载网络参数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
void Load_Net_Parameters(void)
{
		Gateway_IP[0] = W5500_Parameter[0];//加载网关参数
		Gateway_IP[1] = W5500_Parameter[1];
		Gateway_IP[2] = W5500_Parameter[2];
		Gateway_IP[3] = W5500_Parameter[3];
		Sub_Mask[0]=W5500_Parameter[4];//加载子网掩码
		Sub_Mask[1]=W5500_Parameter[5];
		Sub_Mask[2]=W5500_Parameter[6];
		Sub_Mask[3]=W5500_Parameter[7];			
		IDtoMAC(Phy_Addr);  //加载物理地址
		IP_Addr[0]=W5500_Parameter[8];//加载本机IP地址   称重 3
		IP_Addr[1]=W5500_Parameter[9];
		IP_Addr[2]=W5500_Parameter[10];
		IP_Addr[3]=W5500_Parameter[11];
// 		IP_Addr[0]=10;//加载本机IP地址   称重 3
// 		IP_Addr[1]=14;
// 		IP_Addr[2]=16;
// 		IP_Addr[3]=150;
		S0_Port[0] = 0x13;//加载端口0的端口号5000
		S0_Port[1] = 0x88;
		S0_DIP[0]=192;//加载端口0的目的IP地址
		S0_DIP[1]=168;
		S0_DIP[2]=148;
		S0_DIP[3]=57;			
		S0_DPort[0] = 0x17;//加载端口0的目的端口号6000
		S0_DPort[1] = 0x70;
		S0_Mode=TCP_SERVER;//加载端口0的工作模式,TCP客户端模式	
		
			//===UDP通信参数
		S1_Port[0] = W5500_Parameter[25];   //接口1的本地端口号25031
		S1_Port[1] = W5500_Parameter[24];	
		//=UDP广播目的IP地址与端口号
		UDP_DIPR[0]=W5500_Parameter[20];	//UDP(广播)模式,目的主机IP地址  
		UDP_DIPR[1]=W5500_Parameter[21];
		UDP_DIPR[2]=W5500_Parameter[22];
		UDP_DIPR[3]=W5500_Parameter[23];
		UDP_DPORT[0]=W5500_Parameter[25];//目的端口号60000  25031
		UDP_DPORT[1]=W5500_Parameter[24];
		S1_Mode=UDP_MODE;//				
		
		S2_Port[0] = W5500_Parameter[13];//端口2的端口号25032  与主控连接
		S2_Port[1] = W5500_Parameter[12];				
		S2_Mode=TCP_SERVER;//加载端口2的工作模式,TCP客户端模式				
}
/*******************************************************************************
* 函数名  : W5500_Initialization
* 描述    : W5500初始货配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//初始化W5500寄存器函数
	Detect_Gateway();	//检查网关服务器 
	Socket_Init(0);		//指定Socket(0~7)初始化,初始化端口0  //服务端  和数码管相连
	Socket_Init(1);		//指定Socket(0~7)初始化,初始化端口1
	Socket_Init(2);		//指定Socket(0~7)初始化,初始化端口2  //服务端  和主控相连
}
/*******************************************************************************
* 函数名  : W5500_Hardware_Reset
* 描述    : 硬件复位W5500
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : W5500的复位引脚保持低电平至少500us以上,才能重起W5500
*******************************************************************************/
void W5500_Hardware_Reset(void)
{
	if(ucW5500_InitCTR==0)
  {
			GPIO_ResetBits(W5500_RST_PORT, W5500_RST);//复位引脚拉低
			Delay_ms(50);
			GPIO_SetBits(W5500_RST_PORT, W5500_RST);//复位引脚拉高
			Delay_ms(200);
			ucW5500_InitCTR=1;
  }
  if(ucW5500_InitCTR==1)
  {
			while(((Read_W5500_1Byte(PHYCFGR)&LINK)==1)&&(ucW5500_InitCTR==1))		
			{
						ucW5500_InitCTR=2;
			}
  }
}
/*******************************************************************************
* 函数名  : W5500_Send_Data
* 描述    : W5500发送协议数据数据
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void W5500_Send_Data(uint8_t s,uint8_t *Data)
{
		uint16_t i,u16_size=0;
		uint8_t u8_Lrc=0;
		u16_size = *(Data+1)<<8 | *Data; //帧长
		u16_size +=  6;     												//总长=帧长+剩余字节长度
		u8Send_Buffer[0] = CON_START;
// 		if(SysParameter.Par12_MachineType == 0) //轮重
// 		{
// 				 u8Send_Buffer[1] = WEIGHT_ADDRESS | 0x80;
// 		}
// 		else
// 		{
// 				 u8Send_Buffer[1] = BRAKE_ADDRESS | 0x80;
// 		}
		u8Send_Buffer[1] = CON_ADDRESS | 0x80;
		u8_Lrc = u8Send_Buffer[1];
		for(i=2;i<(u16_size-2);i++)
		{
				u8Send_Buffer[i] = *Data++;
				u8_Lrc += u8Send_Buffer[i];
		}
		u8Send_Buffer[u16_size-2] = u8_Lrc;
		u8Send_Buffer[u16_size-1] = CON_END;
		Write_SOCK_Data_Buffer(s,u8Send_Buffer,u16_size);
}

uint32_t FLASH_Read_Word(uint32_t address)	
{
    return(*(__IO uint32_t*)address);
}	

/*******************************************************************************
* 函数名  : Process_Socket_Data
* 描述    : W5500接收并发送接收到的数据
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 本过程先调用S_rx_process()从W5500的端口接收数据缓冲区读取数据,
*			然后将读取的数据从Rx_Buffer拷贝到Temp_Buffer缓冲区进行处理。
*			处理完毕，将数据从Temp_Buffer拷贝到Tx_Buffer缓冲区。调用S_tx_process()
*			发送数据。
*******************************************************************************/
void W5500_Socket_Data(SOCKET s)
{
	uint16_t u16_size;
	uint16_t i;  //for循环计数使用
	uint8_t u8read_flg;//读标定系数标志
	uint8_t u8flash_buffer[21];//标定系数  参数临时存储变量
	/* 帧长低位		帧长高位   命令   校验*/
	uint8_t u8_Command,u8_Lrc=0;
	uint16_t u16_Length;
// 	uint8_t u8Par_ID;//参数读写时ID临时变量
	uint8_t Remainder;  //余数
  uint16_t Quotient;      //商
	int8_t cCheck_Result;//加密校验
	
// 	_ADCab _ADCabVariable;  //曲线拟合变量
	
	u16_size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
//	for(i=0;i<u16_size;i++)
//	{
//			printf("%x ",Rx_Buffer[i]);
//	}
//	printf("\r\n");
	if(((Rx_Buffer[0] == CON_START) && (Rx_Buffer[u16_size-1] == CON_END) && (Rx_Buffer[4] == 'F')) == 1)
	{
			SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
				for(i=0;i<24;i++) //把所有数据存储的临时变量Parameter_Buffer
				{
						 con_TranF[i+5] = Parameter_Buffer[i];
				}	
			 W5500_Send_Data(s,con_TranF);       //回传设备信息
	}
	else
	{
				if(((Rx_Buffer[0] == CON_START) && (Rx_Buffer[u16_size-1] == CON_END) && (Rx_Buffer[1] == CON_ADDRESS)) != 1)
				{
							/*	起始位、结束位、地址位任意一个不正确，错误应答Z	*/
							W5500_Send_Data(s,con_TranZACK);
				}
				else
				{
						for(i=1;i<(u16_size-2);i++)
						{
								u8_Lrc += Rx_Buffer[i];
						}
						u16_Length = (Rx_Buffer[2] | (Rx_Buffer[3] <<8)) + 6;
						if((Rx_Buffer[u16_size-2] != u8_Lrc) || (u16_Length != u16_size))
						{
								/*	奇偶校验不正确，错误应答Z	*/
								W5500_Send_Data(s,con_TranZACK);
						}
						else  /* 起始位、结束位、校验位和地址码正确后进行命令分析 */
						{
								u8_Command = Rx_Buffer[4];
								if(s==0)
								{
										Network0_StopFlg = 0;
////										W5500_Send_Data(s,con_TranO);	//心跳
								}
								else if(s==2)
								{
										Network1_StopFlg = 0;
								}
								if(1) //轮重台
								{
										if(s==0)
										{
													if(USER_SystemFlag.A.Flag_Sucess1 == 0)  //数码管握手加密
                          {
                              switch(u8_Command)
															{
																	case 'O'://心跳指令
																	{
																								if(s==0)
																								{
																										Network0_StopFlg = 0;
                                                    W5500_Send_Data(s,con_TranO);	//心跳
																								}
																								else if(s==2)
																								{
																										Network1_StopFlg = 0;
																								}
																								break;
																	}
																	case 'H':
																	{
																								if(Rx_Buffer[5]  == 'A')
																								{
																										Get_System_Ciphertext(cID_Key);    //获取要加密字符
																										Tx_Buffer[0] = 12;  //帧长度 低位
																										Tx_Buffer[1] = 0;  //帧长度 高位
																										Tx_Buffer[2] = 'H'; 
																										Tx_Buffer[3] = 'A'; 
																										memcpy(&Tx_Buffer[4],cID_Key,10);
																										W5500_Send_Data(s,Tx_Buffer);//回传
																								}
																								if(Rx_Buffer[5]  == 'E')
																								{
																										if(Encryption_Cnt <= ENCRYPTION_MAX)
																										{
																												cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
																												if(cCheck_Result == 0)  //加密正确
																												{
																														Encryption_Cnt = 0;
																														W5500_Send_Data(s,con_TranHET);//回传成功
																														USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                            USER_SystemFlag.A.Flag_Standalone = 1;  //和数码管联网成功
																												}
																												else
																												{
																														Encryption_Cnt ++;																											
																														W5500_Send_Data(s,con_TranHEF);//回传失败
																												}
																										}
																										else
																										{
																												W5500_Send_Data(s,con_TranHX1);//回传加密超次数
																										}
																										
																								}
																								break;
																	 }
																	default :
																								W5500_Send_Data(s,con_TranHX0);//回传指令错误
																								break;
															}
                          }
                          else
                          {
                              if(WorkState_Flg == Default_State_Flg)   //待机状态
                              {
                                    switch(u8_Command)
                                    {															
                                        case 'O'://心跳指令
                                        {
                                                      if(s==0)
                                                      {
                                                          Network0_StopFlg = 0;
                                                          W5500_Send_Data(s,con_TranO);	//心跳
                                                      }
                                                      else if(s==2)
                                                      {
                                                          Network1_StopFlg = 0;
                                                      }
                                                      
                                                      break;
                                        }
                                        case 'H':
                                        {
                                                      if(Rx_Buffer[5]  == 'A')
                                                      {
                                                          Get_System_Ciphertext(cID_Key);    //获取要加密字符
                                                          Tx_Buffer[0] = 12;  //帧长度 低位
                                                          Tx_Buffer[1] = 0;  //帧长度 高位
                                                          Tx_Buffer[2] = 'H'; 
                                                          Tx_Buffer[3] = 'A'; 
                                                          memcpy(&Tx_Buffer[4],cID_Key,10);
                                                          W5500_Send_Data(s,Tx_Buffer);//回传
                                                      }
                                                      if(Rx_Buffer[5]  == 'E')
                                                      {
                                                          if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                          {
                                                              cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
                                                              if(cCheck_Result == 0)  //加密正确
                                                              {
                                                                  Encryption_Cnt = 0;
                                                                  W5500_Send_Data(s,con_TranHET);//回传成功
                                                                  USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                  USER_SystemFlag.A.Flag_Standalone = 1;  //和数码管联网成功
                                                              }
                                                              else
                                                              {
                                                                  Encryption_Cnt ++;																											
                                                                  W5500_Send_Data(s,con_TranHEF);//回传失败
                                                              }
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranHX1);//回传加密超次数
                                                          }
                                                          
                                                      }
                                                      break;
                                         }
                                        case 'S'://取状态
                                        {
                                                      switch(WorkState_Flg)
                                                      {
                                                            case Default_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'S';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case Adjust_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'J';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case SignalTest_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'C';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case StaticWeight_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'T';
                                                                                                con_TranS[4] = '7';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }										
                                                            case StaticWeight_Motor_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'T';
                                                                                                con_TranS[4] = '8';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }	
                                                            default :
                                                                        W5500_Send_Data(s,con_TranXACK);				//当前命令无效
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }
                                        case 'U'://强制收尾指令 回到待机状态
                                        {
                                                      W5500_Send_Data(s,con_TranAACK);				//正确应答
                                                      Left_Machine = IO_CLOSE;
                                                      Right_Machine = IO_CLOSE;
                                                      Machine_Switch = IO_CLOSE;
                                                      Speed_Control = IO_CLOSE;	
                                                      Pump_Before = IO_CLOSE;
                                                      Pump_Behind = IO_CLOSE;
                                                      Left_Up = IO_CLOSE;
                                                      Right_Up = IO_CLOSE;
                                                      Left_Down = IO_OPEN;
                                                      Right_Down = IO_OPEN;
                                                      Clamp_All = IO_CLOSE;
                                                      Protect_Overflow = IO_CLOSE; 
                                                      Hydraumatic_Machine = IO_CLOSE;
                                                      System_State = Default_State;
                                                      Delay_ms(20);//延时 保证上位机接收到指令
                                                      WorkState_Flg = Default_State_Flg;  //待机状态
                                                      W5500_Send_Data(s,con_TranU);				//结束
                                                      return;
                                        }
                                        case 'R'://强制复位指令 回到待机状态
                                        {
                                                      Left_Machine = IO_CLOSE;
                                                      Right_Machine = IO_CLOSE;
                                                      Machine_Switch = IO_CLOSE;
                                                      Speed_Control = IO_CLOSE;														
                                                      Pump_Before = IO_CLOSE;
                                                      Pump_Behind = IO_CLOSE;
                                                      Left_Up = IO_CLOSE;
                                                      Right_Up = IO_CLOSE;
                                                      Left_Down = IO_OPEN;
                                                      Right_Down = IO_OPEN;
                                                      Clamp_All = IO_CLOSE;
                                                      Protect_Overflow = IO_CLOSE; 
                                                      Hydraumatic_Machine = IO_CLOSE;
                                                      System_State = Default_State;
                                                      Delay_ms(20);//延时 保证上位机接收到指令
                                                      WorkState_Flg = Default_State_Flg;  //待机状tai
                                                      W5500_Send_Data(s,con_TranAACK);				//正确应答
                                                      return;
                                        }
                                        case 'I'://初始化指令
                                        {
                                                      if(Rx_Buffer[5]  == 'W')
                                                      {
                                                          W5500_Send_Data(s,con_TranAACK);				//正确应答
                                                          for(i=0;i<6;i++) //缓存初始化
                                                          {
                                                              Left_DynamicWeight1[i] = 0;
                                                              Right_DynamicWeight1[i] = 0;
                                                          }
                                                          Left_StaticWeight = 0;				//左轮静态轴重 
                                                          Right_StaticWeight = 0;  		//右轮静态轴重
                                                          Motor_StaticWeight = 0; 		//摩托静态轴重 左台架
                                                          Delay_ms(20);//延时 保证上位机接收到指令												
                                                          W5500_Send_Data(s,con_TranI);				//正确应答																
                                                      }																			
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);				//当前命令无效
                                                      }
                                                      break;
                                        }												
                                        case 'B'://版本信息
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//当前版本信息
                                                      break;
                                        }
                                        case 'C'://信号测试界面
                                        {
                                                      if(Rx_Buffer[5] == 0)
                                                      {
                                                            if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
                                                            {
                                                                Signal_TransferTime = Rx_Buffer[6] - 0x30;
                                                                System_State = SignalTest_State;
                                                                WorkState_Flg = SignalTest_State_Flg;    //信号测试状态
                                                                W5500_Send_Data(s,con_TranAACK);//正确应答
                                                            }
                                                            else
                                                            {
                                                                W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                            }
                                                      }
                                                      else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //标定状态
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //标定状态
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//当前命令无效
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
                                                                                                    break;	
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == 'E')
                                                      {
                                                          System_State = Default_State;
                                                          WorkState_Flg = Default_State_Flg;    //待机状态
                                                          W5500_Send_Data(s,con_TranAACK);//正确应答
                                                      }
                                                      else if(Rx_Buffer[5] == 'X')
                                                      {
                                                          USER_SystemFlag.A.Flag_TestC1 = 1;
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                      }
                                                      break;
                                        }
                                        case 'D'://取测试结果
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'W'://静态轮重结果
                                                          {																												
                                                                        FunW5500_SendDW(s);				//发送当前轮重
                                                                        break;
                                                          }
                                                          case 'A'://动态轴荷结果
                                                          {																												
                                                                        break;
                                                          }
                                                          case 'L'://发送摩托车重量结果
                                                          {
                                                                        Tx_Buffer[0] = 8;    //帧长度 低位
                                                                        Tx_Buffer[1] = 0;    //帧长度 高位
                                                                        Tx_Buffer[2] = 'D'; 
                                                                        Tx_Buffer[3] = 'L'; 
                                                                        /* 摩托称重 */
                                                                        Tx_Buffer[4] = Motor_StaticWeight/10000; 
                                                                        if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
                                                                        else Tx_Buffer[4] = 0x20;
                                                                        Tx_Buffer[5] = Motor_StaticWeight%10000/1000; 
                                                                        if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
                                                                        else Tx_Buffer[5] = 0x20;
                                                                        Tx_Buffer[6] = Motor_StaticWeight%1000/100; 
                                                                        if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
                                                                        else Tx_Buffer[6] = 0x20;
                                                                        Tx_Buffer[7] = Motor_StaticWeight%100/10; 
                                                                        if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
                                                                        else Tx_Buffer[7] = 0x20;
                                                                        Tx_Buffer[8] = Motor_StaticWeight%10 + 0x30; 
                                                                        Tx_Buffer[9] = 0x20; 
                                                                        W5500_Send_Data(s,Tx_Buffer);				//发送当前摩托重量
                                                                        break;
                                                          }																																																																																																									
                                                          default:
                                                                        W5500_Send_Data(s,con_TranXACK); //当前指令无效
                                                                        break;
                                                      }
                                                      break;
                                        }
                                        case 'G'://取实时数据
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'W':
                                                          {
                                                                         if(SysParameter.Par6_Weight == 2)
                                                                         {
                                                                             Quotient = Left_DynamicWeight / 2;
                                                                             Remainder = Left_DynamicWeight % 2;
                                                                             if(Remainder > 0)
                                                                             {
                                                                                Left_DynamicWeight = (Quotient +1) * 2;																										 
                                                                             }
                                                                             Quotient = Right_DynamicWeight / 2;
                                                                             Remainder = Right_DynamicWeight % 2;
                                                                             if(Remainder > 0)
                                                                             {
                                                                                Right_DynamicWeight = (Quotient +1) * 2;																										 
                                                                             }
                                                                         }
                                                                         else if(SysParameter.Par6_Weight == 5)
                                                                         {
                                                                            Quotient = Left_DynamicWeight / 5;
                                                                             Remainder = Left_DynamicWeight % 5;
                                                                             if(Remainder < 3)
                                                                             {
                                                                                Left_DynamicWeight = Quotient * 5;																										 
                                                                             }
                                                                             else
                                                                             {
                                                                                Left_DynamicWeight = (Quotient +1) * 5;
                                                                             }
                                                                             Quotient = Right_DynamicWeight / 5;
                                                                             Remainder = Right_DynamicWeight % 5;
                                                                             if(Remainder < 3)
                                                                             {
                                                                                Right_DynamicWeight = Quotient * 5;																										 
                                                                             }
                                                                             else
                                                                             {
                                                                                Right_DynamicWeight = (Quotient +1) * 5;
                                                                             }
                                                                         }
                                                                        FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //上传动态显示轮重
                                                                        break;
                                                          }																																																																																																										
                                                          default :
                                                                       W5500_Send_Data(s,con_TranXACK); //当前指令无效
                                                                       break;
                                                      }
                                                      break;
                                        }
                                        case 'A': //标定状态
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'C'://进入标定状态
                                                                        {				
                                                                            System_State = Adjust_State;
                                                                            WorkState_Flg = Adjust_State_Flg;    //标定状态
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:
                                                                                                                    AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                                    break;																																
                                                                                default:
                                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                                    W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                                                                                    AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
                                                                                                                    break;	
                                                                            }
                                                                            break;
                                                                        }
                                                          case 'E'://退出标定状态
                                                                        {
                                                                            System_State = Default_State;
                                                                            WorkState_Flg = Default_State_Flg;    //待机状态
                                                                            W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                            break;
                                                                        }
                                                          case 'W'://保存标定系数
                                                                        {
                                                                            for(i=0;i<18;i++)
                                                                            {
                                                                                u8flash_buffer[i] = Rx_Buffer[i+7];																			
                                                                            }
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:			
                                                                                                                  CabItemIndex = 0;
                                                                                                                  _ADCabVariable.NbofVar = u8flash_buffer[0];
                                                                                                                  _ADCabVariable.CabPointNum = u8flash_buffer[1];
                                                                                                                  _ADCabVariable.Vlt[0] = (u8flash_buffer[3]<<8)|u8flash_buffer[2];
                                                                                                                  _ADCabVariable.Val[0] = (u8flash_buffer[5]<<8)|u8flash_buffer[4];
                                                                                                                  _ADCabVariable.Vlt[1] = (u8flash_buffer[7]<<8)|u8flash_buffer[6];
                                                                                                                  _ADCabVariable.Val[1] = (u8flash_buffer[9]<<8)|u8flash_buffer[8];
                                                                                                                  _ADCabVariable.Vlt[2] = (u8flash_buffer[11]<<8)|u8flash_buffer[10];
                                                                                                                  _ADCabVariable.Val[2] = (u8flash_buffer[13]<<8)|u8flash_buffer[12];
                                                                                                                  _ADCabVariable.Vlt[3] = (u8flash_buffer[15]<<8)|u8flash_buffer[14];
                                                                                                                  _ADCabVariable.Val[3] = (u8flash_buffer[17]<<8)|u8flash_buffer[16];
                                                                                                                  CabBuf[0] = &_ADCabVariable;
                                                                                                                  Cabrate_Computer();  //计算标定系数
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//擦出扇区
                                                                                                                  SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//保存系数																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//擦出扇区
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//保存标定点数
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                                  break;
                                                                                case Right_Wight_ID:
                                                                                                                    CabItemIndex = 1;
                                                                                                                  _ADCabVariable.NbofVar = u8flash_buffer[0];
                                                                                                                  _ADCabVariable.CabPointNum = u8flash_buffer[1];
                                                                                                                  _ADCabVariable.Vlt[0] = (u8flash_buffer[3]<<8)|u8flash_buffer[2];
                                                                                                                  _ADCabVariable.Val[0] = (u8flash_buffer[5]<<8)|u8flash_buffer[4];
                                                                                                                  _ADCabVariable.Vlt[1] = (u8flash_buffer[7]<<8)|u8flash_buffer[6];
                                                                                                                  _ADCabVariable.Val[1] = (u8flash_buffer[9]<<8)|u8flash_buffer[8];
                                                                                                                  _ADCabVariable.Vlt[2] = (u8flash_buffer[11]<<8)|u8flash_buffer[10];
                                                                                                                  _ADCabVariable.Val[2] = (u8flash_buffer[13]<<8)|u8flash_buffer[12];
                                                                                                                  _ADCabVariable.Vlt[3] = (u8flash_buffer[15]<<8)|u8flash_buffer[14];
                                                                                                                  _ADCabVariable.Val[3] = (u8flash_buffer[17]<<8)|u8flash_buffer[16];
                                                                                                                  CabBuf[1] = &_ADCabVariable;
                                                                                                                  Cabrate_Computer();  //计算标定系数
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//擦出扇区
                                                                                                                  SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//保存系数																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//擦出扇区
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//保存标定点数
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                                  break;																																
                                                                                default:
                                                                                                                    printf("\r\nError:The command channel ID is false!!!xie\r\n");
                                                                                                                    W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                                                                                    break;
                                                                            }
                                                                            break;
                                                                        }
                                                          case 'R'://读标定系数
                                                                        {
                                                                            u8read_flg = 1;//读标定系数成功
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
                                                                                                                    break;																																
                                                                                default:
                                                                                                                    u8read_flg = 0;//读标定系数失败
                                                                                                                    printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
                                                                                                                    break;
                                                                            }			
                                                                            if(u8read_flg == 1)//读标定系数成功
                                                                            {
                                                                                Tx_Buffer[0] = 21;//帧长度 低位
                                                                                Tx_Buffer[1] = 0;  //帧长度 高位
                                                                                Tx_Buffer[2] = 'A'; 
                                                                                Tx_Buffer[3] = 'R'; 
                                                                                Tx_Buffer[4] = Rx_Buffer[6]; 
                                                                                for(i=0;i<18;i++)
                                                                                {
                                                                                    Tx_Buffer[i+5] = u8flash_buffer[i];
                                                                                }
                                                                                W5500_Send_Data(s,Tx_Buffer);
                                                                            }
                                                                            else
                                                                            {
                                                                                W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                                            }
                                                                            break;
                                                                        }
                                                           default :
                                                                       W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                                       break;
                                                      }
                                                      break;
                                        }
                                        case 'Y': //调零
                                        {
                                                      if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                      {
                                                          Fun_Zero(Rx_Buffer[5],1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //强制清零
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                      }
                                                      break;
                                        }
                                        case 'E'://参数读写
                                        {
                                                      if(Rx_Buffer[5] == '1')
                                                      {
                                                            if(Rx_Buffer[6] == 'W')
                                                            {																							
                                                                for(i=0;i<(Rx_Buffer[7]*3+1);i++) //把所有数据存储的临时变量u8flash_buffer
                                                                {
                                                                     Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                                }								
                                                                if(Fun_DealParameter()==1)
                                                                {
                                                                    SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//擦出扇区
                                                                    SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(Rx_Buffer[7]*3+1));//保存系数		
                                                                    W5500_Send_Data(s,con_TranE1W);//正确应答
                                                                }
                                                                else
                                                                {
                                                                    W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行
                                                                    printf("\r\nError:Write Parameter false!!!E1W\r\n");
                                                                }
                                                            }
                                                            else if(Rx_Buffer[6] == 'R')
                                                            {												
                                                                  SPI_FLASH_BufferRead(Parameter_Buffer,ParameterAll_Flash_Address,100);
                                                                  Tx_Buffer[0] = Parameter_Buffer[0]*3+4;  //帧长度 低位
                                                                  Tx_Buffer[1] = 0;  //帧长度 高位
                                                                  Tx_Buffer[2] = 'E'; 
                                                                  Tx_Buffer[3] = '1'; 
                                                                  Tx_Buffer[4] = 'R';
                                                                  for(i=0;i<(Parameter_Buffer[0]*3+1);i++)
                                                                  {
                                                                      Tx_Buffer[i+5] = Parameter_Buffer[i];
                                                                  }
                                                                  W5500_Send_Data(s,Tx_Buffer);
                                                            }
                                                            else
                                                            {
                                                                W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行	
                                                                printf("\r\nError:The command E1 is false!!!E1\r\n");
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == '2')
                                                      {
                                                          if(Rx_Buffer[6] == 'W')
                                                          {
                                                              for(i=0;i<24;i++) //把所有数据存储的临时变量Parameter_Buffer
                                                              {
                                                                   Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                              }	
                                                              SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//擦出扇区
                                                              SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,24);//保存系数		
                                                              W5500_Send_Data(s,con_TranE2W);//正确应答
                                                          }
                                                          else if(Rx_Buffer[6] == 'R')
                                                          {
                                                              SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
                                                              Tx_Buffer[0] = 27;  //帧长度 低位
                                                              Tx_Buffer[1] = 0;    //帧长度 高位
                                                              Tx_Buffer[2] = 'E'; 
                                                              Tx_Buffer[3] = '2'; 
                                                              Tx_Buffer[4] = 'R';
                                                              for(i=0;i<24;i++)
                                                              {
                                                                  Tx_Buffer[i+5] = Parameter_Buffer[i];
                                                              }
                                                              W5500_Send_Data(s,Tx_Buffer);
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行	
                                                              printf("\r\nError:The command E2 is false!!!E2\r\n");
                                                          }
                                                      }
                                                      else if(Rx_Buffer[5] == 'I')
                                                      {
                                                          if(Rx_Buffer[6] == 'W')
                                                          {
                                                              for(i=0;i<26;i++) //把所有数据存储的临时变量Parameter_Buffer
                                                              {
                                                                   Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                              }	
                                                              SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//擦出扇区
                                                              SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterIP_Flash_Address,26);//保存系数		
                                                              W5500_Send_Data(s,con_TranEIW);//正确应答
                                                          }
                                                          else if(Rx_Buffer[6] == 'R')
                                                          {
                                                              SPI_FLASH_BufferRead(Parameter_Buffer,ParameterIP_Flash_Address,26);
                                                              Tx_Buffer[0] = 29;  //帧长度 低位
                                                              Tx_Buffer[1] = 0;    //帧长度 高位
                                                              Tx_Buffer[2] = 'E'; 
                                                              Tx_Buffer[3] = 'I'; 
                                                              Tx_Buffer[4] = 'R';
                                                              for(i=0;i<26;i++)
                                                              {
                                                                  Tx_Buffer[i+5] = Parameter_Buffer[i];
                                                              }
                                                              W5500_Send_Data(s,Tx_Buffer);
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行	
                                                              printf("\r\nError:The command EII is false!!!EI\r\n");
                                                          }
                                                      }
                                                      else
                                                      {
                                                           W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行	
                                                           printf("\r\nError:The command E is false!!!E\r\n");
                                                      }
                                                      break;
                                        }
                                        case 'T':
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {																																																		
                                                          case 'W'://静态轮重检测
                                                          {																												 																												 
                                                                         if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
                                                                         {
                                                                             DynamicWeight_Number = Rx_Buffer[6] - 0x30;
                                                                             System_State = DynamicWeight_State; //系统进入动态称重
                                                                             W5500_Send_Data(s,con_TranAACK);  //正确应答	
                                                                             Delay_ms(20);//延时 保证上位机接收到指令
                                                                              WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else if(Rx_Buffer[6] == 0x31)
                                                                         {
                                                                            DynamicWeight_Number = 1;
                                                                            System_State = StaticWeight_State; //系统进入静态称重
                                                                             W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                             Delay_ms(20);//延时 保证上位机接收到指令
                                                                             WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else
                                                                         {
                                                                              W5500_Send_Data(s,con_TranXACK);	//无效帧指令
                                                                         }
                                                                         break;
                                                          }
                                                          case 'A'://动态态轮重检测
                                                          {
                                                                         if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
                                                                         {
                                                                             DynamicWeight_Number = Rx_Buffer[6] - 0x30;
                                                                             System_State = DynamicWeight_State; //系统进入动态称重
                                                                             W5500_Send_Data(s,con_TranAACK);  //正确应答	
                                                                             Delay_ms(20);//延时 保证上位机接收到指令
                                                                              WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else
                                                                         {
                                                                              W5500_Send_Data(s,con_TranXACK);	//无效帧指令
                                                                         }
                                                                         break;
                                                          }
                                                          case 'L':  //摩重检测
                                                          {																							 
                                                                         System_State = StaticWeight_Motor_State; //系统进入摩托称重
                                                                         W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                         Delay_ms(20);//延时 保证上位机接收到指令
                                                                         WorkState_Flg = StaticWeight_Motor_State_Flg;
                                                                         break;
                                                          }																																																															
                                                      }
                                                      break;
                                        }
                                        default :
                                                      W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行																								 
                                                      printf("\r\nError:The command is false!!!\r\n");
                                                      break;
                                    }
                              }
                              else if(WorkState_Flg == Adjust_State_Flg)   //标定状态
                              {
                                    switch(u8_Command)
                                    {
                                        case 'O'://心跳指令
                                        {
                                                      if(s==0)
                                                      {
                                                          Network0_StopFlg = 0;
                                                          W5500_Send_Data(s,con_TranO);	//心跳
                                                      }
                                                      else if(s==2)
                                                      {
                                                          Network1_StopFlg = 0;
                                                      }
                                                      break;
                                        }
                                         case 'H':
                                        {
                                                      if(Rx_Buffer[5]  == 'A')
                                                      {
                                                          Get_System_Ciphertext(cID_Key);    //获取要加密字符
                                                          Tx_Buffer[0] = 12;  //帧长度 低位
                                                          Tx_Buffer[1] = 0;  //帧长度 高位
                                                          Tx_Buffer[2] = 'H'; 
                                                          Tx_Buffer[3] = 'A'; 
                                                          memcpy(&Tx_Buffer[4],cID_Key,10);
                                                          W5500_Send_Data(s,Tx_Buffer);//回传
                                                      }
                                                      if(Rx_Buffer[5]  == 'E')
                                                      {
                                                          if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                          {
                                                              cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
                                                              if(cCheck_Result == 0)  //加密正确
                                                              {
                                                                  Encryption_Cnt = 0;
                                                                  W5500_Send_Data(s,con_TranHET);//回传成功
                                                                  USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                  USER_SystemFlag.A.Flag_Standalone = 1;  //和数码管联网成功
                                                              }
                                                              else
                                                              {
                                                                  Encryption_Cnt ++;																											
                                                                  W5500_Send_Data(s,con_TranHEF);//回传失败
                                                              }
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranHX1);//回传加密超次数
                                                          }
                                                          
                                                      }
                                                      break;
                                         }
                                        case 'S'://取状态
                                        {
                                                      switch(WorkState_Flg)
                                                      {
                                                            case Default_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'S';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case Adjust_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'J';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case SignalTest_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'C';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case StaticWeight_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'T';
                                                                                                con_TranS[4] = '7';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }										
                                                            case StaticWeight_Motor_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'T';
                                                                                                con_TranS[4] = '8';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }	
                                                            default :
                                                                        W5500_Send_Data(s,con_TranXACK);				//当前命令无效
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }											
                                        case 'R'://强制复位指令 回到待机状态
                                        {
                                                      Left_Machine = IO_CLOSE;
                                                      Right_Machine = IO_CLOSE;
                                                      Machine_Switch = IO_CLOSE;
                                                      Speed_Control = IO_CLOSE;														
                                                      Pump_Before = IO_CLOSE;
                                                      Pump_Behind = IO_CLOSE;
                                                      Left_Up = IO_CLOSE;
                                                      Right_Up = IO_CLOSE;
                                                      Left_Down = IO_OPEN;
                                                      Right_Down = IO_OPEN;
                                                      Clamp_All = IO_CLOSE;
                                                      Protect_Overflow = IO_CLOSE; 
                                                      Hydraumatic_Machine = IO_CLOSE;
                                                      System_State = Default_State;
                                                      Delay_ms(20);//延时 保证上位机接收到指令
                                                      WorkState_Flg = Default_State_Flg;  //待机状�
                                                      W5500_Send_Data(s,con_TranAACK);				//正确应答
                                                      return;
                                        }																								
                                        case 'B'://版本信息
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//当前版本信息
                                                      break;
                                        }																																
                                        case 'C'://信号测试界面
                                        {
                                                      if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //标定状态
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //标定状态
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//当前命令无效
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
                                                                                                    break;	
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == 'E')  //退出
                                                      {
                                                          System_State = Default_State;
                                                          WorkState_Flg = Default_State_Flg;    //待机状态
                                                          W5500_Send_Data(s,con_TranAACK);//正确应答
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                      }
                                                      break;
                                        }
                                        case 'A': //标定状态
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'C'://进入标定状态
                                                                        {				
                                                                            System_State = Adjust_State;
                                                                            WorkState_Flg = Adjust_State_Flg;    //标定状态
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:
                                                                                                                    AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                                    break;																																
                                                                                default:
                                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                                    W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                                                                                    AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
                                                                                                                    break;	
                                                                            }
                                                                            break;
                                                                        }
                                                          case 'E'://退出标定状态
                                                                        {
                                                                            System_State = Default_State;
                                                                            WorkState_Flg = Default_State_Flg;    //待机状态
                                                                            W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                            break;
                                                                        }
                                                          case 'W'://保存标定系数
                                                                        {
                                                                            for(i=0;i<18;i++)
                                                                            {
                                                                                u8flash_buffer[i] = Rx_Buffer[i+7];																			
                                                                            }
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:			
                                                                                                                  CabItemIndex = 0;
                                                                                                                  _ADCabVariable.NbofVar = u8flash_buffer[0];
                                                                                                                  _ADCabVariable.CabPointNum = u8flash_buffer[1];
                                                                                                                  _ADCabVariable.Vlt[0] = (u8flash_buffer[3]<<8)|u8flash_buffer[2];
                                                                                                                  _ADCabVariable.Val[0] = (u8flash_buffer[5]<<8)|u8flash_buffer[4];
                                                                                                                  _ADCabVariable.Vlt[1] = (u8flash_buffer[7]<<8)|u8flash_buffer[6];
                                                                                                                  _ADCabVariable.Val[1] = (u8flash_buffer[9]<<8)|u8flash_buffer[8];
                                                                                                                  _ADCabVariable.Vlt[2] = (u8flash_buffer[11]<<8)|u8flash_buffer[10];
                                                                                                                  _ADCabVariable.Val[2] = (u8flash_buffer[13]<<8)|u8flash_buffer[12];
                                                                                                                  _ADCabVariable.Vlt[3] = (u8flash_buffer[15]<<8)|u8flash_buffer[14];
                                                                                                                  _ADCabVariable.Val[3] = (u8flash_buffer[17]<<8)|u8flash_buffer[16];
                                                                                                                  CabBuf[0] = &_ADCabVariable;
                                                                                                                  Cabrate_Computer();  //计算标定系数
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//擦出扇区
                                                                                                                  SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//保存系数																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//擦出扇区
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//保存标定点数
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                                  break;
                                                                                case Right_Wight_ID:
                                                                                                                    CabItemIndex = 1;
                                                                                                                  _ADCabVariable.NbofVar = u8flash_buffer[0];
                                                                                                                  _ADCabVariable.CabPointNum = u8flash_buffer[1];
                                                                                                                  _ADCabVariable.Vlt[0] = (u8flash_buffer[3]<<8)|u8flash_buffer[2];
                                                                                                                  _ADCabVariable.Val[0] = (u8flash_buffer[5]<<8)|u8flash_buffer[4];
                                                                                                                  _ADCabVariable.Vlt[1] = (u8flash_buffer[7]<<8)|u8flash_buffer[6];
                                                                                                                  _ADCabVariable.Val[1] = (u8flash_buffer[9]<<8)|u8flash_buffer[8];
                                                                                                                  _ADCabVariable.Vlt[2] = (u8flash_buffer[11]<<8)|u8flash_buffer[10];
                                                                                                                  _ADCabVariable.Val[2] = (u8flash_buffer[13]<<8)|u8flash_buffer[12];
                                                                                                                  _ADCabVariable.Vlt[3] = (u8flash_buffer[15]<<8)|u8flash_buffer[14];
                                                                                                                  _ADCabVariable.Val[3] = (u8flash_buffer[17]<<8)|u8flash_buffer[16];
                                                                                                                  CabBuf[1] = &_ADCabVariable;
                                                                                                                  Cabrate_Computer();  //计算标定系数
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//擦出扇区
                                                                                                                  SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//保存系数																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//擦出扇区
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//保存标定点数
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                                  break;																																
                                                                                default:
                                                                                                                    printf("\r\nError:The command channel ID is false!!!xie\r\n");
                                                                                                                    W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                                                                                    break;
                                                                            }
                                                                            break;
                                                                        }
                                                          case 'R'://读标定系数
                                                                        {
                                                                            u8read_flg = 1;//读标定系数成功
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
                                                                                                                    break;																																
                                                                                default:
                                                                                                                    u8read_flg = 0;//读标定系数失败
                                                                                                                    printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
                                                                                                                    break;
                                                                            }			
                                                                            if(u8read_flg == 1)//读标定系数成功
                                                                            {
                                                                                Tx_Buffer[0] = 21;//帧长度 低位
                                                                                Tx_Buffer[1] = 0;  //帧长度 高位
                                                                                Tx_Buffer[2] = 'A'; 
                                                                                Tx_Buffer[3] = 'R'; 
                                                                                Tx_Buffer[4] = Rx_Buffer[6]; 
                                                                                for(i=0;i<18;i++)
                                                                                {
                                                                                    Tx_Buffer[i+5] = u8flash_buffer[i];
                                                                                }
                                                                                W5500_Send_Data(s,Tx_Buffer);
                                                                            }
                                                                            else
                                                                            {
                                                                                W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                                            }
                                                                            break;
                                                                        }
                                                           default :
                                                                       W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                                       break;
                                                      }
                                                      break;
                                        }
                                        case 'Y': //调零
                                        {
                                                      if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                      {
                                                          Fun_Zero(Rx_Buffer[5],1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //强制清零
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                      }
                                                      break;
                                        }																					
                                        default :
                                                      W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行																								 
                                                      printf("\r\nError:The command is false!!!\r\n");
                                                      break;
                                    }
                              }
                              else if(WorkState_Flg == SignalTest_State_Flg)  //信号测试状态
                              {
                                  switch(u8_Command)
                                  {
                                        case 'O'://心跳指令
                                        {
                                                      if(s==0)
                                                      {
                                                          Network0_StopFlg = 0;
                                                          W5500_Send_Data(s,con_TranO);	//心跳
                                                      }
                                                      else if(s==2)
                                                      {
                                                          Network1_StopFlg = 0;
                                                      }
                                                      break;
                                        }
                                         case 'H':
                                        {
                                                      if(Rx_Buffer[5]  == 'A')
                                                      {
                                                          Get_System_Ciphertext(cID_Key);    //获取要加密字符
                                                          Tx_Buffer[0] = 12;  //帧长度 低位
                                                          Tx_Buffer[1] = 0;  //帧长度 高位
                                                          Tx_Buffer[2] = 'H'; 
                                                          Tx_Buffer[3] = 'A'; 
                                                          memcpy(&Tx_Buffer[4],cID_Key,10);
                                                          W5500_Send_Data(s,Tx_Buffer);//回传
                                                      }
                                                      if(Rx_Buffer[5]  == 'E')
                                                      {
                                                          if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                          {
                                                              cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
                                                              if(cCheck_Result == 0)  //加密正确
                                                              {
                                                                  Encryption_Cnt = 0;
                                                                  W5500_Send_Data(s,con_TranHET);//回传成功
                                                                  USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                  USER_SystemFlag.A.Flag_Standalone = 1;  //和数码管联网成功
                                                              }
                                                              else
                                                              {
                                                                  Encryption_Cnt ++;																											
                                                                  W5500_Send_Data(s,con_TranHEF);//回传失败
                                                              }
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranHX1);//回传加密超次数
                                                          }
                                                          
                                                      }
                                                      break;
                                         }
                                        case 'S'://取状态
                                        {
                                                      switch(WorkState_Flg)
                                                      {
                                                            case Default_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'S';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case Adjust_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'J';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case SignalTest_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'C';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case StaticWeight_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'T';
                                                                                                con_TranS[4] = '7';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }										
                                                            case StaticWeight_Motor_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'T';
                                                                                                con_TranS[4] = '8';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }	
                                                            default :
                                                                        W5500_Send_Data(s,con_TranXACK);				//当前命令无效
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }												
                                        case 'R'://强制复位指令 回到待机状态
                                        {
                                                      Left_Machine = IO_CLOSE;
                                                      Right_Machine = IO_CLOSE;
                                                      Machine_Switch = IO_CLOSE;
                                                      Speed_Control = IO_CLOSE;														
                                                      Pump_Before = IO_CLOSE;
                                                      Pump_Behind = IO_CLOSE;
                                                      Left_Up = IO_CLOSE;
                                                      Right_Up = IO_CLOSE;
                                                      Left_Down = IO_OPEN;
                                                      Right_Down = IO_OPEN;
                                                      Clamp_All = IO_CLOSE;
                                                      Protect_Overflow = IO_CLOSE; 
                                                      Hydraumatic_Machine = IO_CLOSE;
                                                      System_State = Default_State;
                                                      Delay_ms(20);//延时 保证上位机接收到指令
                                                      WorkState_Flg = Default_State_Flg;  //待机状�
                                                      W5500_Send_Data(s,con_TranAACK);				//正确应答
                                                      return;
                                        }																			
                                        case 'B'://版本信息
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//当前版本信息
                                                      break;
                                        }
                                        case 'C'://信号测试界面
                                        {
                                                      if(Rx_Buffer[5] == 0)
                                                      {
                                                            if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
                                                            {
                                                                Signal_TransferTime = Rx_Buffer[6] - 0x30;
                                                                System_State = SignalTest_State;
                                                                WorkState_Flg = SignalTest_State_Flg;    //信号测试状态
                                                                W5500_Send_Data(s,con_TranAACK);//正确应答
                                                            }
                                                            else
                                                            {
                                                                W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                            }
                                                      }
                                                      else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //标定状态
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //标定状态
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
                                                                                                    W5500_Send_Data(s,con_TranAACK);//正确应答
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//当前命令无效
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
                                                                                                    break;	
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == 'E')
                                                      {
                                                          System_State = Default_State;
                                                          WorkState_Flg = Default_State_Flg;    //待机状态
                                                          W5500_Send_Data(s,con_TranAACK);//正确应答
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                      }
                                                      break;
                                        }
                                        case 'Y': //调零
                                        {
                                                      if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                      {
                                                          Fun_Zero(Rx_Buffer[5],1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //强制清零
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//当前命令无效
                                                      }
                                                      break;
                                        }																								
                                        default :
                                                      W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行																								 
                                                      printf("\r\nError:The command is false!!!\r\n");
                                                      break;
                                    }
                              }
                              else   //检测状态
                              {
                                    switch(u8_Command)
                                    {
                                        case 'O'://心跳指令
                                        {
                                                      if(s==0)
                                                      {
                                                          Network0_StopFlg = 0;
                                                          W5500_Send_Data(s,con_TranO);	//心跳
                                                      }
                                                      else if(s==2)
                                                      {
                                                          Network1_StopFlg = 0;
                                                      }
                                                      break;
                                        }
                                         case 'H':
                                        {
                                                      if(Rx_Buffer[5]  == 'A')
                                                      {
                                                          Get_System_Ciphertext(cID_Key);    //获取要加密字符
                                                          Tx_Buffer[0] = 12;  //帧长度 低位
                                                          Tx_Buffer[1] = 0;  //帧长度 高位
                                                          Tx_Buffer[2] = 'H'; 
                                                          Tx_Buffer[3] = 'A'; 
                                                          memcpy(&Tx_Buffer[4],cID_Key,10);
                                                          W5500_Send_Data(s,Tx_Buffer);//回传
                                                      }
                                                      if(Rx_Buffer[5]  == 'E')
                                                      {
                                                          if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                          {
                                                              cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
                                                              if(cCheck_Result == 0)  //加密正确
                                                              {
                                                                  Encryption_Cnt = 0;
                                                                  W5500_Send_Data(s,con_TranHET);//回传成功
                                                                  USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                  USER_SystemFlag.A.Flag_Standalone = 1;  //和数码管联网成功
                                                              }
                                                              else
                                                              {
                                                                  Encryption_Cnt ++;																											
                                                                  W5500_Send_Data(s,con_TranHEF);//回传失败
                                                              }
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranHX1);//回传加密超次数
                                                          }
                                                          
                                                      }
                                                      break;
                                         }
                                        case 'S'://取状态
                                        {
                                                      switch(WorkState_Flg)
                                                      {
                                                            case Default_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'S';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case Adjust_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'J';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case SignalTest_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'C';
                                                                                                con_TranS[4] = '2';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }
                                                            case StaticWeight_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'T';
                                                                                                con_TranS[4] = '7';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }										
                                                            case StaticWeight_Motor_State_Flg:
                                                            {
                                                                                                con_TranS[3] = 'T';
                                                                                                con_TranS[4] = '8';
                                                                                                W5500_Send_Data(s,con_TranS);		
                                                                                                break;
                                                            }	
                                                            default :
                                                                        W5500_Send_Data(s,con_TranXACK);				//当前命令无效
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }
                                        case 'U'://强制收尾指令 回到待机状态
                                        {
                                                      W5500_Send_Data(s,con_TranAACK);				//正确应答
                                                      Left_Machine = IO_CLOSE;
                                                      Right_Machine = IO_CLOSE;
                                                      Machine_Switch = IO_CLOSE;
                                                      Speed_Control = IO_CLOSE;	
                                                      Pump_Before = IO_CLOSE;
                                                      Pump_Behind = IO_CLOSE;
                                                      Left_Up = IO_CLOSE;
                                                      Right_Up = IO_CLOSE;
                                                      Left_Down = IO_OPEN;
                                                      Right_Down = IO_OPEN;
                                                      Clamp_All = IO_CLOSE;
                                                      Protect_Overflow = IO_CLOSE; 
                                                      Hydraumatic_Machine = IO_CLOSE;
                                                      System_State = Default_State;
                                                      Delay_ms(20);//延时 保证上位机接收到指令
                                                      WorkState_Flg = Default_State_Flg;  //待机状态
                                                      W5500_Send_Data(s,con_TranU);				//结束
                                                      return;
                                        }
                                        case 'R'://强制复位指令 回到待机状态
                                        {
                                                      Left_Machine = IO_CLOSE;
                                                      Right_Machine = IO_CLOSE;
                                                      Machine_Switch = IO_CLOSE;
                                                      Speed_Control = IO_CLOSE;														
                                                      Pump_Before = IO_CLOSE;
                                                      Pump_Behind = IO_CLOSE;
                                                      Left_Up = IO_CLOSE;
                                                      Right_Up = IO_CLOSE;
                                                      Left_Down = IO_OPEN;
                                                      Right_Down = IO_OPEN;
                                                      Clamp_All = IO_CLOSE;
                                                      Protect_Overflow = IO_CLOSE; 
                                                      Hydraumatic_Machine = IO_CLOSE;
                                                      System_State = Default_State;
                                                      Delay_ms(20);//延时 保证上位机接收到指令
                                                      WorkState_Flg = Default_State_Flg;  //待机状�
                                                      W5500_Send_Data(s,con_TranAACK);				//正确应答
                                                      return;
                                        }																						
                                        case 'B'://版本信息
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//当前版本信息
                                                      break;
                                        }											
                                        case 'G'://取实时数据
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'W':
                                                          {
                                                                         if(SysParameter.Par6_Weight == 2)
                                                                         {
                                                                             Quotient = Left_DynamicWeight / 2;
                                                                             Remainder = Left_DynamicWeight % 2;
                                                                             if(Remainder > 0)
                                                                             {
                                                                                Left_DynamicWeight = (Quotient +1) * 2;																										 
                                                                             }
                                                                             Quotient = Right_DynamicWeight / 2;
                                                                             Remainder = Right_DynamicWeight % 2;
                                                                             if(Remainder > 0)
                                                                             {
                                                                                Right_DynamicWeight = (Quotient +1) * 2;																										 
                                                                             }
                                                                         }
                                                                         else if(SysParameter.Par6_Weight == 5)
                                                                         {
                                                                            Quotient = Left_DynamicWeight / 5;
                                                                             Remainder = Left_DynamicWeight % 5;
                                                                             if(Remainder < 3)
                                                                             {
                                                                                Left_DynamicWeight = Quotient * 5;																										 
                                                                             }
                                                                             else
                                                                             {
                                                                                Left_DynamicWeight = (Quotient +1) * 5;
                                                                             }
                                                                             Quotient = Right_DynamicWeight / 5;
                                                                             Remainder = Right_DynamicWeight % 5;
                                                                             if(Remainder < 3)
                                                                             {
                                                                                Right_DynamicWeight = Quotient * 5;																										 
                                                                             }
                                                                             else
                                                                             {
                                                                                Right_DynamicWeight = (Quotient +1) * 5;
                                                                             }
                                                                         }																																																						
                                                                         FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //上传动态显示轮重
                                                                        break;
                                                          }																																																																																																										
                                                          default :
                                                                       W5500_Send_Data(s,con_TranXACK); //当前指令无效
                                                                       break;
                                                      }
                                                      break;
                                        }																						
                                        case 'D'://取测试结果
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'W'://静态轮重结果
                                                          {
                                                                        FunW5500_SendDW(s);
                                                                        break;
                                                          }
                                                          case 'A'://动态轴荷结果
                                                          {
                                                                        break;
                                                          }
                                                          case 'L'://发送摩托车重量结果
                                                          {
                                                                        Tx_Buffer[0] = 8;    //帧长度 低位
                                                                        Tx_Buffer[1] = 0;    //帧长度 高位
                                                                        Tx_Buffer[2] = 'D'; 
                                                                        Tx_Buffer[3] = 'L'; 
                                                                        /* 摩托称重 */
                                                                        Tx_Buffer[4] = Motor_StaticWeight/10000; 
                                                                        if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
                                                                        else Tx_Buffer[4] = 0x20;
                                                                        Tx_Buffer[5] = Motor_StaticWeight%10000/1000; 
                                                                        if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
                                                                        else Tx_Buffer[5] = 0x20;
                                                                        Tx_Buffer[6] = Motor_StaticWeight%1000/100; 
                                                                        if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
                                                                        else Tx_Buffer[6] = 0x20;
                                                                        Tx_Buffer[7] = Motor_StaticWeight%100/10; 
                                                                        if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
                                                                        else Tx_Buffer[7] = 0x20;
                                                                        Tx_Buffer[8] = Motor_StaticWeight%10 + 0x30; 
                                                                        Tx_Buffer[9] = 0x20; 
                                                                        W5500_Send_Data(s,Tx_Buffer);				//发送当前摩托重量
                                                                        break;
                                                          }																																																																																																									
                                                          default:
                                                                        W5500_Send_Data(s,con_TranXACK); //当前指令无效
                                                                        break;
                                                      }
                                                      break;
                                        }
                                        default :
                                                      W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行																								 
                                                      printf("\r\nError:The command is false!!!\r\n");
                                                      break;
                                    }
                              }
                          }
										}
										else if(s==2)
										{
												 if(USER_SystemFlag.A.Flag_Sucess == 0) //握手加密
												 {
															switch(u8_Command)
															{
																	case 'O'://心跳指令
																	{
																								if(s==0)
																								{
																										Network0_StopFlg = 0;
																								}
																								else if(s==2)
																								{
																										Network1_StopFlg = 0;
																								}
																								break;
																	}
																	case 'H':
																	{
																								if(Rx_Buffer[5]  == 'A')
																								{
																										Get_System_Ciphertext(cID_Key);    //获取要加密字符
																										Tx_Buffer[0] = 12;  //帧长度 低位
																										Tx_Buffer[1] = 0;  //帧长度 高位
																										Tx_Buffer[2] = 'H'; 
																										Tx_Buffer[3] = 'A'; 
																										memcpy(&Tx_Buffer[4],cID_Key,10);
																										W5500_Send_Data(s,Tx_Buffer);//回传
																								}
																								if(Rx_Buffer[5]  == 'E')
																								{
																										if(Encryption_Cnt <= ENCRYPTION_MAX)
																										{
																												cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
																												if(cCheck_Result == 0)  //加密正确
																												{
																														Encryption_Cnt = 0;
																														W5500_Send_Data(s,con_TranHET);//回传成功
																														USER_SystemFlag.A.Flag_Sucess = 1;
                                                            USER_SystemFlag.A.Flag_Online = 1;  //和主控联网成功
                                                            USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
																												}
																												else
																												{
																														Encryption_Cnt ++;																											
																														W5500_Send_Data(s,con_TranHEF);//回传失败
																												}
																										}
																										else
																										{
																												W5500_Send_Data(s,con_TranHX1);//回传加密超次数
																										}
																										
																								}
																								break;
																	 }
																	default :
																								W5500_Send_Data(s,con_TranHX0);//回传指令错误
																								break;
															}
												 }
												 else
												 {
															if(WorkState_Flg == Default_State_Flg)   //待机状态
															{
																		switch(u8_Command)
																		{				
																				case 'V'://自检
																				{
																										if(Rx_Buffer[5]  == 'R')
																										{
																												SystemErr_W5500Send();
																										}
																										else
																										{
																												SystemErr_Test();
																										}
																										break;
																				}
																				case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //获取要加密字符
																													Tx_Buffer[0] = 12;  //帧长度 低位
																													Tx_Buffer[1] = 0;  //帧长度 高位
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//回传
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
																															if(cCheck_Result == 0)  //加密正确
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//回传成功
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //和主控联网成功
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//回传失败
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//回传加密超次数
																													}
																													
																											}
																											break;
																				 }
																				case'W':                                                                  
																				{
																						if(Rx_Buffer[5]=='G')                                             //
																						{
																								FLASH_Unlock();
																								FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);
																								WFLASHStatus = FLASH_ErasePage(2048 * 255);
																								if(WFLASHStatus!=FLASH_COMPLETE)                                    //???????:???E0
																								{
																										 FLASH_Lock();
																										 W5500_Send_Data(s,con_TranWE);				//回传异常错误
																										 return;	
																								}
																								WFLASHStatus=FLASH_ProgramWord(2048 * 255, 0X77777777);
																								if(WFLASHStatus!=FLASH_COMPLETE)                                  //???????:???E0
																								{
																										FLASH_Lock();
																										W5500_Send_Data(s,con_TranWE);				//回传异常错误
																										return;	
																								}
																								if(FLASH_Read_Word(2048 * 255)==0X77777777)
																								{
																										FLASH_Lock();
																										W5500_Send_Data(s,con_TranWG);				//回传升级应答	
																										Delay_ms(10);                                                //10ms??	
																										NVIC_SystemReset();
																								}
																								else                                                                //
																								{
																											FLASH_Lock();
																											W5500_Send_Data(s,con_TranWE);			//回传异常错误
																								 }
																						}		
																						break;
																				}																
																				case 'O'://心跳指令
																				{
																											if(s==0)
																											{
																													Network0_StopFlg = 0;
																											}
																											else if(s==2)
																											{
																													Network1_StopFlg = 0;
																											}
																											break;
																				}
																				case 'S'://取状态
																				{
																											switch(WorkState_Flg)
																											{
																														case Default_State_Flg:
																														{
																																																con_TranS[3] = 'S';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Adjust_State_Flg:
																														{
																																																con_TranS[3] = 'J';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case SignalTest_State_Flg:
																														{
																																																con_TranS[3] = 'C';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '7';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}										
																														case StaticWeight_Motor_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '8';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}	
																														default :
																																				W5500_Send_Data(s,con_TranXACK);				//当前命令无效
																																				break;
																														
																											}
																											break;
																				}
																				case 'U'://强制收尾指令 回到待机状态
																				{
																											W5500_Send_Data(s,con_TranAACK);				//正确应答
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;	
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(20);//延时 保证上位机接收到指令
																											WorkState_Flg = Default_State_Flg;  //待机状态
																											W5500_Send_Data(s,con_TranU);				//结束
																											return;
																				}
																				case 'R'://强制复位指令 回到待机状态
																				{
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;														
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(10);//延时 保证上位机接收到指令
																											WorkState_Flg = Default_State_Flg;  //待机状tai
																											W5500_Send_Data(s,con_TranAACK);				//正确应答
																											Delay_ms(10);//延时 保证上位机接收到指令
																											W5500_Send_Data(s,con_TranR);				//复位完成
																											return;
																				}
																				case 'I'://初始化指令
																				{
																											if(Rx_Buffer[5]  == 'W')
																											{
																													W5500_Send_Data(s,con_TranAACK);				//正确应答
																													for(i=0;i<6;i++) //缓存初始化
																													{
																															Left_DynamicWeight1[i] = 0;
																															Right_DynamicWeight1[i] = 0;
																													}
																													Left_StaticWeight = 0;				//左轮静态轴重 
																													Right_StaticWeight = 0;  		//右轮静态轴重
																													Motor_StaticWeight = 0; 		//摩托静态轴重 左台架
																													Delay_ms(20);//延时 保证上位机接收到指令												
																													W5500_Send_Data(s,con_TranI);				//正确应答																
																											}																			
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);				//当前命令无效
																											}
																											break;
																				}												
																				case 'B'://版本信息
																				{
																											W5500_Send_Data(s,con_TranB);				//当前版本信息
																											break;
																				}
																				case 'C'://信号测试界面
																				{
																											if(Rx_Buffer[5] == 0)
																											{
																														if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
																														{
																																Signal_TransferTime = Rx_Buffer[6] - 0x30;
																																System_State = SignalTest_State;
																																WorkState_Flg = SignalTest_State_Flg;    //信号测试状态
																																W5500_Send_Data(s,con_TranAACK);//正确应答
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);//当前命令无效
																														}
																											}
																											else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //标定状态
																																																		AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
																																																		W5500_Send_Data(s,con_TranAACK);//正确应答
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //标定状态
																																																		AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
																																																		W5500_Send_Data(s,con_TranAACK);//正确应答
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//当前命令无效
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
																																																		break;	
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //待机状态
																													W5500_Send_Data(s,con_TranAACK);//正确应答
																											}
																											else if(Rx_Buffer[5] == 'X')
																											{
																													USER_SystemFlag.A.Flag_TestC1 = 1;
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//当前命令无效
																											}
																											break;
																				}
																				case 'D'://取测试结果
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'W'://静态轮重结果
																													{																												
																																				FunW5500_SendDW(s);				//发送当前轮重
																																				break;
																													}
																													case 'A'://动态轴荷结果
																													{																												
																																				break;
																													}
																													case 'L'://发送摩托车重量结果
																													{
																																				Tx_Buffer[0] = 8;    //帧长度 低位
																																				Tx_Buffer[1] = 0;    //帧长度 高位
																																				Tx_Buffer[2] = 'D'; 
																																				Tx_Buffer[3] = 'L'; 
																																				/* 摩托称重 */
																																				Tx_Buffer[4] = Motor_StaticWeight/10000; 
																																				if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
																																				else Tx_Buffer[4] = 0x20;
																																				Tx_Buffer[5] = Motor_StaticWeight%10000/1000; 
																																				if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
																																				else Tx_Buffer[5] = 0x20;
																																				Tx_Buffer[6] = Motor_StaticWeight%1000/100; 
																																				if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
																																				else Tx_Buffer[6] = 0x20;
																																				Tx_Buffer[7] = Motor_StaticWeight%100/10; 
																																				if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
																																				else Tx_Buffer[7] = 0x20;
																																				Tx_Buffer[8] = Motor_StaticWeight%10 + 0x30; 
																																				Tx_Buffer[9] = 0x20; 
																																				W5500_Send_Data(s,Tx_Buffer);				//发送当前摩托重量
																																				break;
																													}																																																																																																									
																													default:
																																				W5500_Send_Data(s,con_TranXACK); //当前指令无效
																																				break;
																											}
																											break;
																				}
																				case 'G'://取实时数据
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'W':
																													{
																																				 if(SysParameter.Par6_Weight == 2)
																																				 {
																																						 Quotient = Left_DynamicWeight / 2;
																																						 Remainder = Left_DynamicWeight % 2;
																																						 if(Remainder > 0)
																																						 {
																																								Left_DynamicWeight = (Quotient +1) * 2;																										 
																																						 }
																																						 Quotient = Right_DynamicWeight / 2;
																																						 Remainder = Right_DynamicWeight % 2;
																																						 if(Remainder > 0)
																																						 {
																																								Right_DynamicWeight = (Quotient +1) * 2;																										 
																																						 }
																																				 }
																																				 else if(SysParameter.Par6_Weight == 5)
																																				 {
																																						Quotient = Left_DynamicWeight / 5;
																																						 Remainder = Left_DynamicWeight % 5;
																																						 if(Remainder < 3)
																																						 {
																																								Left_DynamicWeight = Quotient * 5;																										 
																																						 }
																																						 else
																																						 {
																																								Left_DynamicWeight = (Quotient +1) * 5;
																																						 }
																																						 Quotient = Right_DynamicWeight / 5;
																																						 Remainder = Right_DynamicWeight % 5;
																																						 if(Remainder < 3)
																																						 {
																																								Right_DynamicWeight = Quotient * 5;																										 
																																						 }
																																						 else
																																						 {
																																								Right_DynamicWeight = (Quotient +1) * 5;
																																						 }
																																				 }
																																				FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //上传动态显示轮重
																																				break;
																													}																																																																																																										
																													default :
																																			 W5500_Send_Data(s,con_TranXACK); //当前指令无效
																																			 break;
																											}
																											break;
																				}
																				case 'A': //标定状态
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'C'://进入标定状态
																																				{				
																																						System_State = Adjust_State;
																																						WorkState_Flg = Adjust_State_Flg;    //标定状态
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:
																																																										AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
																																																										W5500_Send_Data(s,con_TranAACK);//正确应答
																																																										break;
																																								case Right_Wight_ID:
																																																										AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
																																																										W5500_Send_Data(s,con_TranAACK);//正确应答
																																																										break;																																
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//当前命令无效
																																																										AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
																																																										break;	
																																						}
																																						break;
																																				}
																													case 'E'://退出标定状态
																																				{
																																						System_State = Default_State;
																																						WorkState_Flg = Default_State_Flg;    //待机状态
																																						W5500_Send_Data(s,con_TranAACK);//正确应答
																																						break;
																																				}
																													case 'W'://保存标定系数
																																				{
																																						for(i=0;i<18;i++)
																																						{
																																								u8flash_buffer[i] = Rx_Buffer[i+7];																			
																																						}
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:			
																																																									CabItemIndex = 0;
																																																									_ADCabVariable.NbofVar = u8flash_buffer[0];
																																																									_ADCabVariable.CabPointNum = u8flash_buffer[1];
																																																									_ADCabVariable.Vlt[0] = (u8flash_buffer[3]<<8)|u8flash_buffer[2];
																																																									_ADCabVariable.Val[0] = (u8flash_buffer[5]<<8)|u8flash_buffer[4];
																																																									_ADCabVariable.Vlt[1] = (u8flash_buffer[7]<<8)|u8flash_buffer[6];
																																																									_ADCabVariable.Val[1] = (u8flash_buffer[9]<<8)|u8flash_buffer[8];
																																																									_ADCabVariable.Vlt[2] = (u8flash_buffer[11]<<8)|u8flash_buffer[10];
																																																									_ADCabVariable.Val[2] = (u8flash_buffer[13]<<8)|u8flash_buffer[12];
																																																									_ADCabVariable.Vlt[3] = (u8flash_buffer[15]<<8)|u8flash_buffer[14];
																																																									_ADCabVariable.Val[3] = (u8flash_buffer[17]<<8)|u8flash_buffer[16];
																																																									CabBuf[0] = &_ADCabVariable;
																																																									Cabrate_Computer();  //计算标定系数
																																																									Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//擦出扇区
																																																									SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//保存系数																																																
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//擦出扇区
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//保存标定点数
																																																									W5500_Send_Data(s,con_TranAACK);//正确应答
																																																									break;
																																								case Right_Wight_ID:
																																																										CabItemIndex = 1;
																																																									_ADCabVariable.NbofVar = u8flash_buffer[0];
																																																									_ADCabVariable.CabPointNum = u8flash_buffer[1];
																																																									_ADCabVariable.Vlt[0] = (u8flash_buffer[3]<<8)|u8flash_buffer[2];
																																																									_ADCabVariable.Val[0] = (u8flash_buffer[5]<<8)|u8flash_buffer[4];
																																																									_ADCabVariable.Vlt[1] = (u8flash_buffer[7]<<8)|u8flash_buffer[6];
																																																									_ADCabVariable.Val[1] = (u8flash_buffer[9]<<8)|u8flash_buffer[8];
																																																									_ADCabVariable.Vlt[2] = (u8flash_buffer[11]<<8)|u8flash_buffer[10];
																																																									_ADCabVariable.Val[2] = (u8flash_buffer[13]<<8)|u8flash_buffer[12];
																																																									_ADCabVariable.Vlt[3] = (u8flash_buffer[15]<<8)|u8flash_buffer[14];
																																																									_ADCabVariable.Val[3] = (u8flash_buffer[17]<<8)|u8flash_buffer[16];
																																																									CabBuf[1] = &_ADCabVariable;
																																																									Cabrate_Computer();  //计算标定系数
																																																									Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//擦出扇区
																																																									SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//保存系数																																																
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//擦出扇区
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//保存标定点数
																																																									W5500_Send_Data(s,con_TranAACK);//正确应答
																																																									break;																																
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!xie\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//当前命令无效
																																																										break;
																																						}
																																						break;
																																				}
																													case 'R'://读标定系数
																																				{
																																						u8read_flg = 1;//读标定系数成功
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
																																																										break;
																																								case Right_Wight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
																																																										break;																																
																																								default:
																																																										u8read_flg = 0;//读标定系数失败
																																																										printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
																																																										break;
																																						}			
																																						if(u8read_flg == 1)//读标定系数成功
																																						{
																																								Tx_Buffer[0] = 21;//帧长度 低位
																																								Tx_Buffer[1] = 0;  //帧长度 高位
																																								Tx_Buffer[2] = 'A'; 
																																								Tx_Buffer[3] = 'R'; 
																																								Tx_Buffer[4] = Rx_Buffer[6]; 
																																								for(i=0;i<18;i++)
																																								{
																																										Tx_Buffer[i+5] = u8flash_buffer[i];
																																								}
																																								W5500_Send_Data(s,Tx_Buffer);
																																						}
																																						else
																																						{
																																								W5500_Send_Data(s,con_TranXACK);//当前命令无效
																																						}
																																						break;
																																				}
																													 default :
																																			 W5500_Send_Data(s,con_TranXACK);//当前命令无效
																																			 break;
																											}
																											break;
																				}
																				case 'Y': //调零
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//当前命令有效
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //强制清零
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//当前命令无效
																											}
																											break;
																				}
																				case 'E'://参数读写
																				{
																											if(Rx_Buffer[5] == '1')
																											{
																														if(Rx_Buffer[6] == 'W')
																														{																							
																																for(i=0;i<(Rx_Buffer[7]*3+1);i++) //把所有数据存储的临时变量u8flash_buffer
																																{
																																		 Parameter_Buffer[i] = Rx_Buffer[i+7];
																																}								
																																if(Fun_DealParameter()==1)
																																{
																																		SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//擦出扇区
																																		SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(Rx_Buffer[7]*3+1));//保存系数		
																																		W5500_Send_Data(s,con_TranE1W);//正确应答
																																}
																																else
																																{
																																		W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行
																																		printf("\r\nError:Write Parameter false!!!E1W\r\n");
																																}
																														}
																														else if(Rx_Buffer[6] == 'R')
																														{												
																																	SPI_FLASH_BufferRead(Parameter_Buffer,ParameterAll_Flash_Address,100);
																																	Tx_Buffer[0] = Parameter_Buffer[0]*3+4;  //帧长度 低位
																																	Tx_Buffer[1] = 0;  //帧长度 高位
																																	Tx_Buffer[2] = 'E'; 
																																	Tx_Buffer[3] = '1'; 
																																	Tx_Buffer[4] = 'R';
																																	for(i=0;i<(Parameter_Buffer[0]*3+1);i++)
																																	{
																																			Tx_Buffer[i+5] = Parameter_Buffer[i];
																																	}
																																	W5500_Send_Data(s,Tx_Buffer);
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行	
																																printf("\r\nError:The command E1 is false!!!E1\r\n");
																														}
																											}
																											else if(Rx_Buffer[5] == '2')
																											{
																													if(Rx_Buffer[6] == 'W')
																													{
																															for(i=0;i<24;i++) //把所有数据存储的临时变量Parameter_Buffer
																															{
																																	 Parameter_Buffer[i] = Rx_Buffer[i+7];
																															}	
																															SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//擦出扇区
																															SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,24);//保存系数		
																															W5500_Send_Data(s,con_TranE2W);//正确应答
																													}
																													else if(Rx_Buffer[6] == 'R')
																													{
																															SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
																															Tx_Buffer[0] = 27;  //帧长度 低位
																															Tx_Buffer[1] = 0;    //帧长度 高位
																															Tx_Buffer[2] = 'E'; 
																															Tx_Buffer[3] = '2'; 
																															Tx_Buffer[4] = 'R';
																															for(i=0;i<24;i++)
																															{
																																	Tx_Buffer[i+5] = Parameter_Buffer[i];
																															}
																															W5500_Send_Data(s,Tx_Buffer);
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行	
																															printf("\r\nError:The command E2 is false!!!E2\r\n");
																													}
																											}
																											else if(Rx_Buffer[5] == 'I')
																											{
																														if(Rx_Buffer[6] == 'W')
																														{
																																for(i=0;i<26;i++) //把所有数据存储的临时变量Parameter_Buffer
																																{
																																		 W5500_Parameter[i] = Rx_Buffer[i+7];
																																}	
																																W5500_Parameter[26] = 0x55;
																																SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//擦出扇区
																																SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//保存系数		
																																W5500_Send_Data(s,con_TranEIW);//正确应答
																																ucW5500_InitCTR = 0;
																																S0_State = 0;
																																S1_State = 0;
																																S2_State = 0;																														    
																																Load_Net_Parameters();
																														}
																														else if(Rx_Buffer[6] == 'R')
																														{
																																SPI_FLASH_BufferRead(Parameter_Buffer,ParameterIP_Flash_Address,26);
																																Tx_Buffer[0] = 29;  //帧长度 低位
																																Tx_Buffer[1] = 0;    //帧长度 高位
																																Tx_Buffer[2] = 'E'; 
																																Tx_Buffer[3] = 'I'; 
																																Tx_Buffer[4] = 'R';
																																for(i=0;i<26;i++)
																																{
																																		Tx_Buffer[i+5] = Parameter_Buffer[i];
																																}
																																W5500_Send_Data(s,Tx_Buffer);
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行	
																																printf("\r\nError:The command EII is false!!!EI\r\n");
																														}
																											}
																											else
																											{
																													 W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行	
																													 printf("\r\nError:The command E is false!!!E\r\n");
																											}
																											break;
																				}
																				case 'T':
																				{
																											switch(Rx_Buffer[5])
																											{																																																		
																													case 'W'://静态轮重检测
																													{																												 																												 
																																				 if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //系统进入动态称重
																																								  W5500_Send_Data(s,con_TranAACK);  //正确应答	
																																								  Delay_ms(20);//延时 保证上位机接收到指令
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //不主动上传结果
																																									USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //系统进入动态称重
																																								  W5500_Send_Data(s,con_TranAACK);  //正确应答	
																																								  Delay_ms(20);//延时 保证上位机接收到指令
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //主动上传结果
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //不主动上传结果
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//指令无效
																																							}																																					 																																					 
																																				 }
																																				 else if(Rx_Buffer[6] == 0x31)
																																				 {
																																						 if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //系统进入静态称重
																																									W5500_Send_Data(s,con_TranAACK);//正确应答
																																									Delay_ms(20);//延时 保证上位机接收到指令
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //不主动上传结果
																																									USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //系统进入静态称重
																																									W5500_Send_Data(s,con_TranAACK);//正确应答
																																									Delay_ms(20);//延时 保证上位机接收到指令
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //主动上传结果
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //不主动上传结果
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//指令无效
																																							}																																						 
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//无效帧指令
																																				 }
																																				 if(USER_SystemFlag.A.Flag_Online == 1) //和主控联网 上传内容
																																				 {
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//数码管数据是否改变 1变  0不变
																																									USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																																									USER_LEDFlag.A.LED_Weight = 1;  			//重量指示灯
																																									USER_LEDFlag.A.LED_Brake = 0;  			  //制动指示灯
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																													case 'A'://动态态轮重检测
																													{
																																				 if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
																																				 {
																																						 DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																						 System_State = DynamicWeight_State; //系统进入动态称重
																																						 W5500_Send_Data(s,con_TranAACK);  //正确应答	
																																						 Delay_ms(20);//延时 保证上位机接收到指令
																																							WorkState_Flg = StaticWeight_State_Flg;																																						
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//无效帧指令
																																				 }
																																				 break;
																													}
																													case 'L':  //摩重检测
																													{																							 
																																				 System_State = StaticWeight_Motor_State; //系统进入摩托称重
																																				 W5500_Send_Data(s,con_TranAACK);//正确应答
																																				 Delay_ms(20);//延时 保证上位机接收到指令
																																				 WorkState_Flg = StaticWeight_Motor_State_Flg;
																																				 break;
																													}																																																															
																											}
																											break;
																				}
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															}
															else if(WorkState_Flg == Adjust_State_Flg)   //标定状态
															{
																		switch(u8_Command)
																		{
																				case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //获取要加密字符
																													Tx_Buffer[0] = 12;  //帧长度 低位
																													Tx_Buffer[1] = 0;  //帧长度 高位
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//回传
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
																															if(cCheck_Result == 0)  //加密正确
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//回传成功
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //和主控联网成功
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//回传失败
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//回传加密超次数
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://自检
																				{
																										if(Rx_Buffer[5]  == 'R')
																										{
																												SystemErr_W5500Send();
																										}
																										else
																										{
																												SystemErr_Test();
																										}
																										break;
																				}
																				case 'O'://心跳指令
																				{
																											if(s==0)
																											{
																													Network0_StopFlg = 0;
																											}
																											else if(s==2)
																											{
																													Network1_StopFlg = 0;
																											}
																											break;
																				}
																				case 'S'://取状态
																				{
																											switch(WorkState_Flg)
																											{
																														case Default_State_Flg:
																														{
																																																con_TranS[3] = 'S';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Adjust_State_Flg:
																														{
																																																con_TranS[3] = 'J';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case SignalTest_State_Flg:
																														{
																																																con_TranS[3] = 'C';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '7';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}										
																														case StaticWeight_Motor_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '8';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}	
																														default :
																																				W5500_Send_Data(s,con_TranXACK);				//当前命令无效
																																				break;
																														
																											}
																											break;
																				}											
																				case 'R'://强制复位指令 回到待机状态
																				{
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;														
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(10);//延时 保证上位机接收到指令
																											WorkState_Flg = Default_State_Flg;  //待机状tai
																											W5500_Send_Data(s,con_TranAACK);				//正确应答
																											Delay_ms(10);//延时 保证上位机接收到指令
																											W5500_Send_Data(s,con_TranR);				//复位完成
																											return;
																				}																								
																				case 'B'://版本信息
																				{
																											W5500_Send_Data(s,con_TranB);				//当前版本信息
																											break;
																				}																																
																				case 'C'://信号测试界面
																				{
																											if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //标定状态
																																																		AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
																																																		W5500_Send_Data(s,con_TranAACK);//正确应答
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //标定状态
																																																		AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
																																																		W5500_Send_Data(s,con_TranAACK);//正确应答
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//当前命令无效
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
																																																		break;	
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')  //退出
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //待机状态
																													W5500_Send_Data(s,con_TranAACK);//正确应答
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//当前命令无效
																											}
																											break;
																				}
																				case 'A': //标定状态
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'C'://进入标定状态
																																				{				
																																						System_State = Adjust_State;
																																						WorkState_Flg = Adjust_State_Flg;    //标定状态
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:
																																																										AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
																																																										W5500_Send_Data(s,con_TranAACK);//正确应答
																																																										break;
																																								case Right_Wight_ID:
																																																										AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
																																																										W5500_Send_Data(s,con_TranAACK);//正确应答
																																																										break;																																
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//当前命令无效
																																																										AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
																																																										break;	
																																						}
																																						break;
																																				}
																													case 'E'://退出标定状态
																																				{
																																						System_State = Default_State;
																																						WorkState_Flg = Default_State_Flg;    //待机状态
																																						W5500_Send_Data(s,con_TranAACK);//正确应答
																																						break;
																																				}
																													case 'W'://保存标定系数
																																				{
																																						for(i=0;i<18;i++)
																																						{
																																								u8flash_buffer[i] = Rx_Buffer[i+7];																			
																																						}
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:			
																																																									CabItemIndex = 0;
																																																									_ADCabVariable.NbofVar = u8flash_buffer[0];
																																																									_ADCabVariable.CabPointNum = u8flash_buffer[1];
																																																									_ADCabVariable.Vlt[0] = (u8flash_buffer[3]<<8)|u8flash_buffer[2];
																																																									_ADCabVariable.Val[0] = (u8flash_buffer[5]<<8)|u8flash_buffer[4];
																																																									_ADCabVariable.Vlt[1] = (u8flash_buffer[7]<<8)|u8flash_buffer[6];
																																																									_ADCabVariable.Val[1] = (u8flash_buffer[9]<<8)|u8flash_buffer[8];
																																																									_ADCabVariable.Vlt[2] = (u8flash_buffer[11]<<8)|u8flash_buffer[10];
																																																									_ADCabVariable.Val[2] = (u8flash_buffer[13]<<8)|u8flash_buffer[12];
																																																									_ADCabVariable.Vlt[3] = (u8flash_buffer[15]<<8)|u8flash_buffer[14];
																																																									_ADCabVariable.Val[3] = (u8flash_buffer[17]<<8)|u8flash_buffer[16];
																																																									CabBuf[0] = &_ADCabVariable;
																																																									Cabrate_Computer();  //计算标定系数
																																																									Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//擦出扇区
																																																									SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//保存系数																																																
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//擦出扇区
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//保存标定点数
																																																									W5500_Send_Data(s,con_TranAACK);//正确应答
																																																									break;
																																								case Right_Wight_ID:
																																																										CabItemIndex = 1;
																																																									_ADCabVariable.NbofVar = u8flash_buffer[0];
																																																									_ADCabVariable.CabPointNum = u8flash_buffer[1];
																																																									_ADCabVariable.Vlt[0] = (u8flash_buffer[3]<<8)|u8flash_buffer[2];
																																																									_ADCabVariable.Val[0] = (u8flash_buffer[5]<<8)|u8flash_buffer[4];
																																																									_ADCabVariable.Vlt[1] = (u8flash_buffer[7]<<8)|u8flash_buffer[6];
																																																									_ADCabVariable.Val[1] = (u8flash_buffer[9]<<8)|u8flash_buffer[8];
																																																									_ADCabVariable.Vlt[2] = (u8flash_buffer[11]<<8)|u8flash_buffer[10];
																																																									_ADCabVariable.Val[2] = (u8flash_buffer[13]<<8)|u8flash_buffer[12];
																																																									_ADCabVariable.Vlt[3] = (u8flash_buffer[15]<<8)|u8flash_buffer[14];
																																																									_ADCabVariable.Val[3] = (u8flash_buffer[17]<<8)|u8flash_buffer[16];
																																																									CabBuf[1] = &_ADCabVariable;
																																																									Cabrate_Computer();  //计算标定系数
																																																									Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//擦出扇区
																																																									SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//保存系数																																																
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//擦出扇区
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//保存标定点数
																																																									W5500_Send_Data(s,con_TranAACK);//正确应答
																																																									break;																																
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!xie\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//当前命令无效
																																																										break;
																																						}
																																						break;
																																				}
																													case 'R'://读标定系数
																																				{
																																						u8read_flg = 1;//读标定系数成功
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
																																																										break;
																																								case Right_Wight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
																																																										break;																																
																																								default:
																																																										u8read_flg = 0;//读标定系数失败
																																																										printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
																																																										break;
																																						}			
																																						if(u8read_flg == 1)//读标定系数成功
																																						{
																																								Tx_Buffer[0] = 21;//帧长度 低位
																																								Tx_Buffer[1] = 0;  //帧长度 高位
																																								Tx_Buffer[2] = 'A'; 
																																								Tx_Buffer[3] = 'R'; 
																																								Tx_Buffer[4] = Rx_Buffer[6]; 
																																								for(i=0;i<18;i++)
																																								{
																																										Tx_Buffer[i+5] = u8flash_buffer[i];
																																								}
																																								W5500_Send_Data(s,Tx_Buffer);
																																						}
																																						else
																																						{
																																								W5500_Send_Data(s,con_TranXACK);//当前命令无效
																																						}
																																						break;
																																				}
																													 default :
																																			 W5500_Send_Data(s,con_TranXACK);//当前命令无效
																																			 break;
																											}
																											break;
																				}
																				case 'Y': //调零
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<0x09))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//当前命令有效
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //强制清零
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//当前命令无效
																											}
																											break;
																				}																					
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															}
															else if(WorkState_Flg == SignalTest_State_Flg)  //信号测试状态
															{
																	switch(u8_Command)
																	{			
																			  case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //获取要加密字符
																													Tx_Buffer[0] = 12;  //帧长度 低位
																													Tx_Buffer[1] = 0;  //帧长度 高位
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//回传
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
																															if(cCheck_Result == 0)  //加密正确
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//回传成功
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //和主控联网成功
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//回传失败
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//回传加密超次数
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://自检
																				{
																										if(Rx_Buffer[5]  == 'R')
																										{
																												SystemErr_W5500Send();
																										}
																										else
																										{
																												SystemErr_Test();
																										}
																										break;
																				}
																				case 'O'://心跳指令
																				{
																											if(s==0)
																											{
																													Network0_StopFlg = 0;
																											}
																											else if(s==2)
																											{
																													Network1_StopFlg = 0;
																											}
																											break;
																				}
																				case 'S'://取状态
																				{
																											switch(WorkState_Flg)
																											{
																														case Default_State_Flg:
																														{
																																																con_TranS[3] = 'S';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Adjust_State_Flg:
																														{
																																																con_TranS[3] = 'J';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case SignalTest_State_Flg:
																														{
																																																con_TranS[3] = 'C';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '7';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}										
																														case StaticWeight_Motor_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '8';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}	
																														default :
																																				W5500_Send_Data(s,con_TranXACK);				//当前命令无效
																																				break;
																														
																											}
																											break;
																				}												
																				case 'R'://强制复位指令 回到待机状态
																				{
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;														
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(10);//延时 保证上位机接收到指令
																											WorkState_Flg = Default_State_Flg;  //待机状tai
																											W5500_Send_Data(s,con_TranAACK);				//正确应答
																											Delay_ms(10);//延时 保证上位机接收到指令
																											W5500_Send_Data(s,con_TranR);				//复位完成
																											return;
																				}																			
																				case 'B'://版本信息
																				{
																											W5500_Send_Data(s,con_TranB);				//当前版本信息
																											break;
																				}
																				case 'C'://信号测试界面
																				{
																											if(Rx_Buffer[5] == 0)
																											{
																														if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
																														{
																																Signal_TransferTime = Rx_Buffer[6] - 0x30;
																																System_State = SignalTest_State;
																																WorkState_Flg = SignalTest_State_Flg;    //信号测试状态
																																W5500_Send_Data(s,con_TranAACK);//正确应答
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);//当前命令无效
																														}
																											}
																											else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //标定状态
																																																		AD_Channel_ID = Left_Weight_ID;	//对应模拟通道1通道
																																																		W5500_Send_Data(s,con_TranAACK);//正确应答
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //标定状态
																																																		AD_Channel_ID = Right_Wight_ID;	//对应模拟通道2通道
																																																		W5500_Send_Data(s,con_TranAACK);//正确应答
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//当前命令无效
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//默认出错时对应模拟通道1 
																																																		break;	
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //待机状态
																													W5500_Send_Data(s,con_TranAACK);//正确应答
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//当前命令无效
																											}
																											break;
																				}
																				case 'Y': //调零
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//当前命令有效
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //强制清零
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//当前命令有效
                                                      }
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//当前命令无效
																											}
																											break;
																				}																								
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															}
															else   //检测状态
															{
																		switch(u8_Command)
																		{
																				case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //获取要加密字符
																													Tx_Buffer[0] = 12;  //帧长度 低位
																													Tx_Buffer[1] = 0;  //帧长度 高位
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//回传
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES解密并校验
																															if(cCheck_Result == 0)  //加密正确
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//回传成功
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //和主控联网成功
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//回传失败
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//回传加密超次数
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://自检
																				{
																										if(Rx_Buffer[5]  == 'R')
																										{
																												SystemErr_W5500Send();
																										}
																										else
																										{
																												SystemErr_Test();
																										}
																										break;
																				}
																				case 'O'://心跳指令
																				{
																											if(s==0)
																											{
																													Network0_StopFlg = 0;
																											}
																											else if(s==2)
																											{
																													Network1_StopFlg = 0;
																											}
																											break;
																				}
																				case 'S'://取状态
																				{
																											switch(WorkState_Flg)
																											{
																														case Default_State_Flg:
																														{
																																																con_TranS[3] = 'S';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Adjust_State_Flg:
																														{
																																																con_TranS[3] = 'J';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case SignalTest_State_Flg:
																														{
																																																con_TranS[3] = 'C';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '7';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}										
																														case StaticWeight_Motor_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '8';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}	
																														default :
																																				W5500_Send_Data(s,con_TranXACK);				//当前命令无效
																																				break;
																														
																											}
																											break;
																				}
																				case 'U'://强制收尾指令 回到待机状态
																				{
																											W5500_Send_Data(s,con_TranAACK);				//正确应答
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;	
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(20);//延时 保证上位机接收到指令
																											WorkState_Flg = Default_State_Flg;  //待机状态
																											W5500_Send_Data(s,con_TranU);				//结束
																											return;
																				}
																				case 'R'://强制复位指令 回到待机状态
																				{
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;														
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(10);//延时 保证上位机接收到指令
																											WorkState_Flg = Default_State_Flg;  //待机状tai
																											W5500_Send_Data(s,con_TranAACK);				//正确应答
																											Delay_ms(10);//延时 保证上位机接收到指令
																											W5500_Send_Data(s,con_TranR);				//复位完成
																											return;
																				}																						
																				case 'B'://版本信息
																				{
																											W5500_Send_Data(s,con_TranB);				//当前版本信息
																											break;
																				}											
																				case 'G'://取实时数据
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'W':
																													{
																																				 if(SysParameter.Par6_Weight == 2)
																																				 {
																																						 Quotient = Left_DynamicWeight / 2;
																																						 Remainder = Left_DynamicWeight % 2;
																																						 if(Remainder > 0)
																																						 {
																																								Left_DynamicWeight = (Quotient +1) * 2;																										 
																																						 }
																																						 Quotient = Right_DynamicWeight / 2;
																																						 Remainder = Right_DynamicWeight % 2;
																																						 if(Remainder > 0)
																																						 {
																																								Right_DynamicWeight = (Quotient +1) * 2;																										 
																																						 }
																																				 }
																																				 else if(SysParameter.Par6_Weight == 5)
																																				 {
																																						Quotient = Left_DynamicWeight / 5;
																																						 Remainder = Left_DynamicWeight % 5;
																																						 if(Remainder < 3)
																																						 {
																																								Left_DynamicWeight = Quotient * 5;																										 
																																						 }
																																						 else
																																						 {
																																								Left_DynamicWeight = (Quotient +1) * 5;
																																						 }
																																						 Quotient = Right_DynamicWeight / 5;
																																						 Remainder = Right_DynamicWeight % 5;
																																						 if(Remainder < 3)
																																						 {
																																								Right_DynamicWeight = Quotient * 5;																										 
																																						 }
																																						 else
																																						 {
																																								Right_DynamicWeight = (Quotient +1) * 5;
																																						 }
																																				 }																																																						
																																				 FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //上传动态显示轮重
																																				break;
																													}																																																																																																										
																													default :
																																			 W5500_Send_Data(s,con_TranXACK); //当前指令无效
																																			 break;
																											}
																											break;
																				}																						
																				case 'C'://信号测试界面
																				{																											
																											if(Rx_Buffer[5] == 'X')
																											{
																													USER_SystemFlag.A.Flag_TestC1 = 1;
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//当前命令无效
																											}
																											break;
																				}
																				case 'T':
																				{
																											switch(Rx_Buffer[5])
																											{																																																		
																													case 'W'://静态轮重检测
																													{																												 																												 
																																				 if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //系统进入动态称重
																																								  W5500_Send_Data(s,con_TranAACK);  //正确应答	
																																								  Delay_ms(20);//延时 保证上位机接收到指令
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //不主动上传结果
																																									USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //系统进入动态称重
																																								  W5500_Send_Data(s,con_TranAACK);  //正确应答	
																																								  Delay_ms(20);//延时 保证上位机接收到指令
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //主动上传结果
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //不主动上传结果
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//指令无效
																																							}																																					 																																					 
																																				 }
																																				 else if(Rx_Buffer[6] == 0x31)
																																				 {
																																						 if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //系统进入静态称重
																																									W5500_Send_Data(s,con_TranAACK);//正确应答
																																									Delay_ms(20);//延时 保证上位机接收到指令
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //不主动上传结果
																																									USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //系统进入静态称重
																																									W5500_Send_Data(s,con_TranAACK);//正确应答
																																									Delay_ms(20);//延时 保证上位机接收到指令
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//检测指示灯
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //主动上传结果
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //不主动上传结果
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//指令无效
																																							}																																						 
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//无效帧指令
																																				 }
																																				 if(USER_SystemFlag.A.Flag_Online == 1) //和主控联网 上传内容
																																				 {
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //握手加密
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//联网指示灯
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//数码管数据是否改变 1变  0不变
																																									USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变
																																									USER_LEDFlag.A.LED_Weight = 1;  			//重量指示灯
																																									USER_LEDFlag.A.LED_Brake = 0;  			  //制动指示灯
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																													case 'A'://动态态轮重检测
																													{
																																				 if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
																																				 {
																																						 DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																						 System_State = DynamicWeight_State; //系统进入动态称重
																																						 W5500_Send_Data(s,con_TranAACK);  //正确应答	
																																						 Delay_ms(20);//延时 保证上位机接收到指令
																																							WorkState_Flg = StaticWeight_State_Flg;
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//无效帧指令
																																				 }
																																				 break;
																													}
																													case 'L':  //摩重检测
																													{																							 
																																				 System_State = StaticWeight_Motor_State; //系统进入摩托称重
																																				 W5500_Send_Data(s,con_TranAACK);//正确应答
																																				 Delay_ms(20);//延时 保证上位机接收到指令
																																				 WorkState_Flg = StaticWeight_Motor_State_Flg;
																																				 break;
																													}																																																															
																											}
																											break;
																				}
																				case 'D'://取测试结果
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'W'://静态轮重结果
																													{
																																				FunW5500_SendDW(s);
																																				break;
																													}
																													case 'A'://动态轴荷结果
																													{
																																				break;
																													}
																													case 'L'://发送摩托车重量结果
																													{
																																				Tx_Buffer[0] = 8;    //帧长度 低位
																																				Tx_Buffer[1] = 0;    //帧长度 高位
																																				Tx_Buffer[2] = 'D'; 
																																				Tx_Buffer[3] = 'L'; 
																																				/* 摩托称重 */
																																				Tx_Buffer[4] = Motor_StaticWeight/10000; 
																																				if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
																																				else Tx_Buffer[4] = 0x20;
																																				Tx_Buffer[5] = Motor_StaticWeight%10000/1000; 
																																				if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
																																				else Tx_Buffer[5] = 0x20;
																																				Tx_Buffer[6] = Motor_StaticWeight%1000/100; 
																																				if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
																																				else Tx_Buffer[6] = 0x20;
																																				Tx_Buffer[7] = Motor_StaticWeight%100/10; 
																																				if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
																																				else Tx_Buffer[7] = 0x20;
																																				Tx_Buffer[8] = Motor_StaticWeight%10 + 0x30; 
																																				Tx_Buffer[9] = 0x20; 
																																				W5500_Send_Data(s,Tx_Buffer);				//发送当前摩托重量
																																				break;
																													}																																																																																																									
																													default:
																																				W5500_Send_Data(s,con_TranXACK); //当前指令无效
																																				break;
																											}
																											break;
																				}
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//当前指令无法执行																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															}
												 }
										}
								}					
						}
				 }
   }
}
/*******************************************************************************
* 函数名  : W5500_Receive_Deal
* 描述    : W5500测试程序
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 本模板在引用时此部分可不做修改
*******************************************************************************/
void W5500_Receive_Deal(void)
{
	  //加密文件主循环调用
		AES_Handler();
    IWDG_ReloadCounter();   //看门狗清零
    if((ucW5500_InitCTR==0)||(ucW5500_InitCTR==1))
    {
        W5500_Hardware_Reset();
    }
    else if(ucW5500_InitCTR==2)
    {
        W5500_Initialization();		
        ucW5500_InitCTR=3;
    }
    else if(ucW5500_InitCTR==3)
    {
        W5500_Socket_Set();                                    		//W5500端口初始化配置 main        
        if(W5500_Interrupt)                                       		 	//处理W5500中断		 main
        {
            W5500_Interrupt_Process();                      		//W5500中断处理程序框架
        }
        if((S0_Data & S_RECEIVE) == S_RECEIVE)    //如果Socket0接收到数据  main
        {
            S0_Data&=~S_RECEIVE;
            W5500_Socket_Data(0);                              //W5500数据处理
// 						W5500_Send_Data(0,con_TranAACK);
        }
				if((S2_Data & S_RECEIVE) == S_RECEIVE)    //如果Socket2接收到数据  main
        {
            S2_Data&=~S_RECEIVE;
            W5500_Socket_Data(2);                              //W5500数据处理
// 						W5500_Send_Data(0,con_TranAACK);
        }
    }
}

/*******************************************************************************
* 函数名  : W5500_Init_Config
* 描述    : W5500初始配置 供主函数调用
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 需要提前配置好定时器2 用到了延时函数  定时器2在AD7663中配置
*******************************************************************************/
void W5500_Init_Config(void)
{
		W5500_SPI_Configuration();		//W5500 SPI初始化配置(STM32 SPI1)
		W5500_GPIO_Configuration();	//W5500 GPIO初始化配置	
		W5500_Initialization();		//W5500初始化配置	
		Load_Net_Parameters();
		TIM_Cmd(TIM3, ENABLE); 		//使能TIMx外设  UDP使用
}
/*******************************************************************************
* 函数名  : W5500_Reset
* 描述    : W5500强制恢复出厂设置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 需要提前配置好定时器2 用到了延时函数  定时器2在AD7663中配置
*******************************************************************************/
void W5500_Reset(void)
{
		ucW5500_InitCTR = 0;
		S0_State = 0;
		S1_State = 0;
		S2_State = 0;
// 		W5500_Initialization();		//W5500初始化配置	
		if(USER_SystemFlag.A.Flag_Online == 1) //和主控联网 上传内容
		{									
				USER_SystemFlag.A.Flag_Online = 0;
				USER_LEDFlag.A.LED_Online = 0;   //联网指示灯
				if(USER_SystemFlag.A.Flag_Standalone == 1) //和数码管联网联网 上传内容
				{
						USER_LEDFlag.A.LED_15 = 0;  				//数码管数据是否改变 1变  0不变
						USER_LEDFlag.A.LED_14 = 1;  				//数码管指示灯是否改变 1变  0不变						
						FunW5500_SendLED(0,0,0);            //上传给显示板
				}
		}
		W5500_ParameterReset();
		Load_Net_Parameters();
}
