/****************************************************************************
* 文件名: SystemErr.c
* 文件历史:
* 版本号    V1.0
* 日期      2017.10.31   
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
#include "SystemErr.h"
#include <string.h>
/*******************************************************************************
故障码定义
*******************************************************************************/


    unSystemErr_TypeDef  un11_Err[Pass_11_Number];        // 开入通道故障码定义
    unSystemErr_TypeDef  un12_Err[Pass_12_Number];        // 开出通道故障码定义
    unSystemErr_TypeDef  un13_Err[Pass_13_Number];        // A/D故障码定义
    unSystemErr_TypeDef  un14_Err[Pass_14_Number];        // D/A故障码定义
    unSystemErr_TypeDef  un15_Err[Pass_15_Number];        // 脉冲量故障码定义
    unSystemErr_TypeDef  un16_Err[Pass_16_Number];        // 串口故障码定义
    unSystemErr_TypeDef  un17_Err[Pass_17_Number];        // 网络通讯故障码定义
    unSystemErr_TypeDef  un18_Err[Pass_18_Number];        // 流程控制故障码定义
    unSystemErr_TypeDef  un19_Err[Pass_19_Number];        // 数据处理故障码定义
    unSystemErr_TypeDef  un20_Err[Pass_20_Number];        // 数据存储故障码定义
    unSystemErr_TypeDef  un21_Err[Pass_21_Number];        // 参数错误故障码定义

/*******************************************************************************
故障码标志位定义
*******************************************************************************/
st11_ErrFlag_TypeDef st11_ErrFlag;
st13_ErrFlag_TypeDef st13_ErrFlag;
st17_ErrFlag_TypeDef st17_ErrFlag;
// st11_ErrFlag_TypeDef   st11_ErrFlag[Pass_11_Number];   // 开入通道故障标志位定义
// st12_ErrFlag_TypeDef   st12_ErrFlag[Pass_12_Number];   // 开出通道故障标志位定义
// st13_ErrFlag_TypeDef   st13_ErrFlag[Pass_13_Number];   // A/D通道故障标志位定义
// st14_ErrFlag_TypeDef   st14_ErrFlag[Pass_14_Number];   // D/A通道故障标志位定义
// st15_ErrFlag_TypeDef   st15_ErrFlag[Pass_15_Number];   // 脉冲量通道故障标志位定义
// st16_ErrFlag_TypeDef   st16_ErrFlag[Pass_16_Number];   // 串口故障标志位定义
// st17_ErrFlag_TypeDef   st17_ErrFlag[Pass_17_Number];   // 网络通讯故障标志位定义
// st18_ErrFlag_TypeDef   st18_ErrFlag[Pass_18_Number];   // 流程控制故障标志位定义
// st19_ErrFlag_TypeDef   st19_ErrFlag[Pass_19_Number];   // 数据处理故障标志位定义
// st20_ErrFlag_TypeDef   st20_ErrFlag[Pass_20_Number];   // 数据存储故障标志位定义
// st21_ErrFlag_TypeDef   st21_ErrFlag[Pass_21_Number];   // 参数错误故障标志位定义
/*******************************************************************************
各类型故障的通道索引
*******************************************************************************/		

//  unsigned char   Pass_11_Number_Set[Pass_11_Number];      //开入故障通道索引数组
//  unsigned char   Pass_12_Number_Set[Pass_12_Number];      //开出故障通道索引数组
//  unsigned char   Pass_13_Number_Set[Pass_13_Number];      //A/D故障通道索引数组
//  unsigned char   Pass_14_Number_Set[Pass_14_Number];      //D/A故障通道索引数组
//  unsigned char   Pass_15_Number_Set[Pass_15_Number];      //脉冲量故障通道索引数组
//  unsigned char   Pass_16_Number_Set[Pass_16_Number];      //串口故障通道索引数组
//  unsigned char   Pass_17_Number_Set[Pass_17_Number];      //网络通讯故障索引数组
//  unsigned char   Pass_18_Number_Set[Pass_18_Number];      //流程控制故障索引数组
//  unsigned char   Pass_19_Number_Set[Pass_19_Number];      //数据处理故障索引数组
//  unsigned char   Pass_20_Number_Set[Pass_20_Number];      //数据存储故障索引数组
//  unsigned char   Pass_21_Number_Set[Pass_21_Number];      //参数错误故障索引数组



/*******************************************************************************
* 函数名  : SystemPass_Init
* 描述    : 通道号初始化：完成所有通道对应硬件接口的初始化。
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
	/***********************根据需要添加******************************/
void SystemPass_Init(void)
{ 
//    Pass_11_Number_Set[0]=1;      //开入故障通道索引数组设置
// 	 Pass_11_Number_Set[1]=2;      //开入故障通道索引数组设置
// 	 Pass_11_Number_Set[2]=3;      //开入故障通道索引数组设置
//    Pass_12_Number_Set[3]=4;      //开出故障通道索引数组设置
//    Pass_12_Number_Set[4]=5;      //开出故障通道索引数组设置
//    Pass_13_Number_Set[5]=6;      //A/D故障通道索引数组设置
//    Pass_13_Number_Set[6]=7;      //A/D故障通道索引数组设置
//    Pass_13_Number_Set[7]=8;      //A/D故障通道索引数组设置
//    Pass_13_Number_Set[8]=9;      //A/D故障通道索引数组设置
}
/*******************************************************************************
* 函数名  : SystemErr_Init
* 描述    : 故障码初始化
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void SystemErr_Init(void)
{ 
    u8 uci;
    /****************************开入通道故障代码初始化***************************/	
		#ifdef USE_11
		un11_Err[0].stSystemErr.ucPass=IN_Weight_ID;
		st11_ErrFlag = es11_Err_N;
    for(uci=0;uci<Pass_11_Number;uci++)
    {
        un11_Err[uci].stSystemErr.ucErr_Code=11;
				un11_Err[uci].stSystemErr.ucErr_State=st11_ErrFlag;
    }	
		#endif
    /*******************************开出通道故障代码初始化***************************/		
	  #ifdef USE_12
		#endif
    /***************************A/D故障代码初始化****************************/	
		#ifdef USE_13
		un13_Err[0].stSystemErr.ucPass=Left_Weight_ID;
		un13_Err[1].stSystemErr.ucPass=Right_Wight_ID;
		st13_ErrFlag = es13_Err_N;       //无故障
		for(uci=0;uci<Pass_13_Number;uci++)
    {
        un13_Err[uci].stSystemErr.ucErr_Code=13;
				un13_Err[uci].stSystemErr.ucErr_State=st13_ErrFlag;
    }	  
		#endif
    /****************************D/A故障代码初始化****************************/	
		#ifdef USE_14
		#endif
    /***************************脉冲量故障代码初始化***************************/	
		#ifdef USE_15
		#endif
    /***************************串口故障代码初始化****************************/	
		#ifdef USE_16
		#endif    
    /***************************网络通讯故障代码初始化****************************/			
		#ifdef USE_17
		un17_Err[0].stSystemErr.ucPass=ERR_Net_ID;
		st17_ErrFlag = es17_Err_N;       //无故障
		for(uci=0;uci<Pass_17_Number;uci++)
    {
        un17_Err[uci].stSystemErr.ucErr_Code=17;
				un17_Err[uci].stSystemErr.ucErr_State=st17_ErrFlag;
    }	 
		#endif          
    /***********************流程控制故障代码初始化****************************/		
		#ifdef USE_18
		#endif
    /***************************数据处理故障代码初始化****************************/	
		#ifdef USE_19
		#endif    		
    /***************************数据存储故障代码初始化****************************/	
 		#ifdef USE_20
		#endif   
    /***************************参数故障代码初始化****************************/	
    
}

/*******************************************************************************
* 函数名  : SystemErr_W5500Send
* 描述    : 故障码发送函数 上传上位机
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void SystemErr_W5500Send(void)
{
		uint8_t u8Err_Num=0;
		uint8_t uci;
	  Tx_Buffer[2] = 'V';  //指令码
// 		Tx_Buffer[3] = '';    //故障数
  /****************************开入通道故障代码初始化***************************/	
		#ifdef USE_11
    for(uci=0;uci<Pass_11_Number;uci++)
    {
				if(un11_Err[uci].stSystemErr.ucErr_State != es11_Err_N)
				{							
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un11_Err[uci].stSystemErr.ucErr_Code;  //故障代码
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un11_Err[uci].stSystemErr.ucPass;  //故障通道号
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un11_Err[uci].stSystemErr.ucErr_State;  //故障类型
							u8Err_Num ++;
				}				
    }	
		#endif
		/***************************A/D故障代码初始化****************************/	
		#ifdef USE_13
    for(uci=0;uci<Pass_13_Number;uci++)
    {
				if(un13_Err[uci].stSystemErr.ucErr_State != es13_Err_N)
				{							
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un13_Err[uci].stSystemErr.ucErr_Code;  //故障代码
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un13_Err[uci].stSystemErr.ucPass;  //故障通道号
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un13_Err[uci].stSystemErr.ucErr_State;  //故障类型
							u8Err_Num ++;
				}				
    }	
		#endif
		/***************************网络通讯故障代码初始化****************************/			
		#ifdef USE_17
		for(uci=0;uci<Pass_17_Number;uci++)
    {
				if(un17_Err[uci].stSystemErr.ucErr_State != es17_Err_N)
				{							
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un18_Err[uci].stSystemErr.ucErr_Code;  //故障代码
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un18_Err[uci].stSystemErr.ucPass;  //故障通道号
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un18_Err[uci].stSystemErr.ucErr_State;  //故障类型
							u8Err_Num ++;
				}				
    }	
		#endif      
		Tx_Buffer[3] = u8Err_Num;    //故障数
		Tx_Buffer[0] = u8Err_Num*3+2;//帧长度
		Tx_Buffer[1] = u8Err_Num*3+2;//帧长度
		SPI_FLASH_SectorErase_A(ParameterERR_Flash_Address);		//擦出扇区
		SPI_FLASH_SectorWrite(Tx_Buffer,ParameterERR_Flash_Address,(u8Err_Num*3+4));//保存系数
		if((USER_SystemFlag.A.Flag_Online == 1)&&(USER_SystemFlag.A.Flag_Sucess == 1))  //联网 给主控发
		{
					W5500_Send_Data(2,Tx_Buffer);				//发送当前错误码
		}	
}
/*******************************************************************************
* 函数名  : SystemErr_Test
* 描述    : 自检
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void SystemErr_Test(void)
{
		if(IO_GetInStatus(IN_Weight_ID) == 1)  //遮挡光电
		{
				un11_Err[0].stSystemErr.ucErr_State = es11_Err_1;
		}
		else
		{
				un11_Err[0].stSystemErr.ucErr_State = es11_Err_N;
		}
		if(Zero_LeftWeight > Zero_Max)//左重零点电压超范围
		{
				un13_Err[0].stSystemErr.ucErr_State=es13_Err_0;
		}
		if(Zero_RightWight > Zero_Max)//右重零点电压超范围
		{
				un13_Err[1].stSystemErr.ucErr_State=es13_Err_0;
		}
		SystemErr_W5500Send();
}
