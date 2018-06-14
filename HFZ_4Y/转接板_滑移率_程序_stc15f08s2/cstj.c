/*************************************************
日  期：20160720（V1.0）
修改人：相旭
内  容：三点滤波，趋势判断
**************************************************
日  期：20160926（V1.1）还没下发到车间
修改人：相旭
内  容：增加标准速度采集值上传，方便工位机检测采集数据是否正常。
*************************************************
日  期：201601114（V1.2）
修改人：相旭
内  容：板子修改，增加成4个指示灯，
		LD1、LD2指示左右脉冲数据故障（亮为正常，灭为故障）
		LD3、LD4指示左右硬停机软停机先后（亮为硬停机，灭为软停机）
**************************************************/


#include "STC15F2K60S2.h"
#include "stdio.h"
#include "absacc.h"
#include "intrins.h"

#define  High   1   //高电平
#define  Low    0   //低电平

#define	 uint   unsigned int
#define	 ulong  unsigned long 
#define	 uchar  unsigned char

#define FOSC 11059200L          	//系统频率
#define BAUD 115200             	//串口波特率
#define High 1   //不停机
#define Low 0    //停机

sbit SWL_in  = P3^2;      //左第三滚速度脉冲信号
sbit SWL_out = P3^5;      //左电机停机信号	  0停机   2^6
sbit SWR_in  = P3^3;      //右第三滚速度脉冲信号
sbit SWR_out = P3^4;      //右电机停机信号	  0停机	  P2^7
sbit SW_cai  = P1^3;      //上位机启动测速停机板信号   原来是1.3

//sbit SW_Led  = P1^4;      //工作状态
//sbit SW_MOL  = P2^0;      //左灯  LD2
//sbit SW_MOR  = P2^1;      //右灯  LD3

#define Bright 0   //指示灯亮
#define Dark 1    //指示灯熄灭
sbit LED_L_GZ  = P5^5;      //左第三滚筒脉冲数据错误指示灯  LD1
sbit LED_R_GZ  = P2^0;      //左第三滚筒脉冲数据错误指示灯  LD2
sbit LED_L_Stop  = P1^5;        //左边硬停机软停机指示   LD4
sbit LED_R_Stop  = P2^1;        //右边硬停机软停机指示   LD3	   P1^4;  20161206

sbit MC_Flg6_24= P1^4;     //6/24脉冲选择位	  P1^5;20161206
sbit HY_tx4 = P1^0;       //滑移率通过跳线设置I/O口 3脚								   
sbit HY_tx3= P1^1;		  //4脚
sbit HY_tx2= P1^2;		  //5脚
sbit HY_tx1= P2^5;		  //28脚

sbit Lstop=P3^6;		  //左边软停机输入
sbit Rstop=P3^7;		  //右边软停机输入

uchar Counter_70ms=0;   //Led发光二极管 闪烁用
float Hy_sd=0;          //滑移率设定
#define Hy_sd20 1.25	//20%		 1/0.8
#define Hy_sd22 1.28	//22%
#define Hy_sd25 1.33	//25%
#define Hy_sd28 1.39	//28%
#define Hy_sd30 1.43	//30%
#define Hy_sd32 1.47	//32%
#define Hy_sd34 1.52	//34%
#define Hy_sd35 1.54	//35%
#define Hy_sd38 1.61	//38%
#define Hy_sd40 1.67	//40%

#define Hy_sd10 1.12	//10% 滑移率趋势判断停机条件限值

uint Left_t0,Left_t1,Left_t2,Left_linshi,Left_Old,Left_New;
uint Right_t0,Right_t1,Right_t2,Right_linshi,Right_Old,Right_New;  //连续三点比较下标值
uchar ucLeft_T0,ucRight_T1;		//实时检测定时器计数值 超过溢出

#define Overtime_Flg6  0x9D//0x4F	 40000 超时判断24齿时20000  6孔时40000
#define Overtime_Flg24 0x4F
#define Index_Flg24 24     //存储数据标号 6孔的为6	24齿的为24
#define Index_Flg6 6
#define OverConstant_24 20000
#define OverConstant_6  39000
uchar uc_Overtime_Flg;//6/24选择时使用的超时变量
uchar uc_Index_Flg;   //6/24选择时使用的存储数据下标变量
uint  ui_OverConstant;

uint ui_Average_L3,ui_Average_R3; //3点点平滑使用的基准值
ulong uli_All_L,uli_All_R;  //24点基准值相加和

unsigned char bdata Flag ;               //系统标志,flag为可位寻址对象

sbit  Watch_Flag=  Flag^0;        //用于灯屏提示后，进入记录m1程序的循环
sbit  Wal_Flag  =  Flag^1;        //左停机标志   ,只是为了提高代码运行效率，若果没有该标志，停机后继续扫描滑移率，判断停车
sbit  War_Flag  =  Flag^2;        //右停机标志
sbit  INT0_Flag =  Flag^3;        //左脉冲下降沿到来触发中断标志
sbit  INT1_Flag = Flag^4;         //右脉冲下降沿到来触发中断标志  
sbit  IE_Flag = Flag^5;           //避免重复开外部中断  
sbit  Lstop_Flag = Flag^6;        //左硬停机软停机有效判断标志	1 有效 0无效
sbit  Rstop_Flag = Flag^7 ;       //右硬停机软停机有效判断标志


uint  LineL,LineR;	//记录24个脉冲数组下标变量	
//unsigned  char  Mj=0;				//6/24脉冲选择
//

union						 //联合体用于存放第三滚速度值 右脉冲时间
{									 
    uint  i_Data;
    uchar b_Data[2];
}uint_m2_Right;

union						 //联合体用于存放推移时间 左脉冲时间
{									 
    uint  t_Data;
    uchar c_Data[2];
}uint_m1_Left;

uint Left1_buf[24]={0};//左存标准速度使用
uint Left2_buf[24]={0};//左存实时检测速度使用
uint idata Right1_buf[24]={0};//右存标准速度使用
uint idata Right2_buf[24]={0};//右存实时检测速度使用
uchar ucLeft_T0_Count,ucRight_T1_Count;//判断异常停机

//uint Left_Plus[336]={0};
//uint Right_Plus[336]={0};

void SendData(unsigned char dat);
void fn_Set_Hyl(void);	//滑移率设置
void fn_Compare24(void);//24齿判断停机算法

void main( void ) 
{  	   

	uint uc_i;//清零使用
   //定时器、外部中断初始化 
    TMOD = 0x11; 	  //0001 0001定时器工作在方式1 ，为16位的定时器/计数器
   //设置串口波特率	
	PCON = 0x00;  
    SCON = 0x50;                 //串口工作在方式1，8位可变波特率
	T2L =(65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H =(65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2	 不分频速度是传统8051的12倍
    AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
   
    SWL_out=Low; //停机
	SWR_out=Low; //停机  待机状态无法启动，上位机必须有启动信号
	//指示灯初始化均点亮	有故障指示灯熄灭
	LED_L_GZ=Bright;
	LED_R_GZ=Bright;
	LED_L_Stop=Dark;	//软停机默认灯灭
	LED_R_Stop=Dark;    //软停机默认灯灭
	IE_Flag=0;
	Watch_Flag=0;          //实时检测和取标准速度状态标志 1 代表实时检测  0取标准速度检测
	Wal_Flag  =  0;          //左停机标志 
	War_Flag  =  0;          //右停机标志
	INT0_Flag =  0;          //左脉冲下降沿到来触发中断标志
	INT1_Flag = 0;           //右脉冲下降沿到来触发中断标志 
	fn_Set_Hyl();
    while(1)
    {		
		TI=1;	 //为printf使用
		while((IE_Flag==0)&&(SW_cai==1))//待机状态
		{
			Watch_Flag=1;
			IE_Flag=0; 	//待机状态 IE_Flag清零 
			IE=0x00;	//关闭所有中断
			INT_CLKO=0x00;//关闭外部中断2 3 只能下降沿中断	  							
		}		
        while(SW_cai==0) // 取标准速度状态
        {	
            if(IE_Flag==0)
            { 
				INT_CLKO=0x30;//开外部中断2、3
				EA=1;//开总中断
				for(uc_i=0;uc_i<24;uc_i++)
				{
					Left1_buf[uc_i]=1;
					Left2_buf[uc_i]=1;
					Right1_buf[uc_i]=1;	  //保证分母不为0
					Right2_buf[uc_i]=1;	
				}
				SWL_out = High; //待机状态不停机
				SWR_out = High; //不停机
				//指示灯均点亮	测试时有故障指示灯熄灭
				LED_L_GZ=Bright;
				LED_R_GZ=Bright;
				LED_L_Stop=Dark;
				LED_R_Stop=Dark;  //软停机 默认灭
				Wal_Flag=0;     		    //左停机标志 
				War_Flag=0;		            //右停机标志
				INT0_Flag=0;          //左脉冲下降沿到来触发中断标志
				INT1_Flag=0;           //右脉冲下降沿到来触发中断标志 	
                IE_Flag=1;	  //保证打开一次就行
				Watch_Flag=1; //取标准速度使用，在外部中断中使用判断存储位置
				Lstop_Flag=0; //左边硬停机软停机标志清零
				Rstop_Flag=0; //右边硬停机软停机标志清零
				LineL=0;
				LineR=0;	  //计数下标清0
                TH0=0X01;					     //清0定时器T0，T1，开启T0，T1
                TL0=0X01;
                TH1=0X01;					     //清0定时器T0，T1，开启T0，T1
                TL1=0X01;
                TR1=1;    //开定时器1
				TR0=1; 	  //开定时器0
				IT0=1;    //外部中断0下降沿中断
				IT1=1;    //外部中断1下降沿中断
              //  IE=0X85;  //进入测试以后，开外部中断0、1
				IE=0X8F;  //进入取标准速度状态，开总中断  定时器0中断 定时器1中断 外部中断0、1
            } 
			while(SW_cai==0)
			{
				Watch_Flag=1; //取标准速度使用，在外部中断中使用判断存储位置
				ucLeft_T0=TH0;
				ucRight_T1=TH1;
				if(ucLeft_T0>uc_Overtime_Flg)
				{
					Left1_buf[LineL++]=ui_OverConstant;
					if(LineL>=uc_Index_Flg)
					{
						LineL=0;
					}	
				}
				if(ucRight_T1>uc_Overtime_Flg)
				{
					Right1_buf[LineR++]=ui_OverConstant;
					if(LineR>=uc_Index_Flg)
					{
						LineR=0;
					}
				}
			}	
        } 
		while((IE_Flag==1)&&(SW_cai==1)) //正常测试状态
		{
			Watch_Flag=0; //实时检测状态标志清0	
			if(uc_Index_Flg==Index_Flg24)
			{
				//3点平滑处理
				uli_All_L=0;
				uli_All_R=0;
				for(uc_i=0;uc_i<24;uc_i++)
				{
					uli_All_L+=Left1_buf[uc_i];
					if((Left1_buf[uc_i]>10000)||(Left1_buf[uc_i]<6000))	  //大于9500时左边灯亮或者小于6000
					{
						//大于9500时左边灯灭或者小于6000
						LED_L_GZ=Dark;	
					}
					uli_All_R+=Right1_buf[uc_i];	  //保证分母不为0
					if((Right1_buf[uc_i]>10000)||(Right1_buf[uc_i]<6000))
					{
				 		//大于9500时右边灯灭或者小于6000
						LED_R_GZ=Dark;
					}	
				}
				ui_Average_L3=uli_All_L/8;
				ui_Average_R3=uli_All_R/8;
				if(ui_Average_L3>30000)
				{
					 Wal_Flag=1;//左电机停机标志置位
					 EX0=0; //关闭外部中断0
					 TR0=0; //定时器0停止计数
					 ET0=0; //关定时器0中断
					 SWL_out = Low; //左电机停机
					 //LED_L_Stop=Bright; 	//软硬指示灯 硬停机指示灯亮
				}
				if(ui_Average_R3>30000)
				{
					 War_Flag=1;//右电机停机标志置位
					 EX1=0; //关闭外部中断1
					 TR1=0; //定时器1停止计数
					 ET1=0; //关定时器1中断
					 SWR_out=Low; //右电机停
					 //LED_R_Stop=Bright;  //软硬指示灯 硬停机指示灯亮
				}
//				for(uc_i=0;uc_i<24;uc_i++)
//				{
//					printf("Left1_buf%d=%u\n",uc_i,Left1_buf[uc_i]);
//					printf("Right1_buf%d=%u\n",uc_i,Right1_buf[uc_i]);
//				}
//				printf("ui_Average_L3=%u\n",ui_Average_L3);
//				printf("ui_Average_R3=%u\n",ui_Average_R3);				
			} 
			if((Lstop_Flag==1)&&(Wal_Flag==1))
			{
				 LED_L_Stop=Bright;  //软硬指示灯 硬停机
			}
			if((Rstop_Flag==1)&&(War_Flag==1))
			{
				 LED_R_Stop=Bright;  //软硬指示灯 硬停机
			}
			while(Watch_Flag==0)
			{
				if((Lstop_Flag==1)&&(Wal_Flag==0))	//左边硬停机软停机判断
				{
					if(Lstop==1)   //左边软停机起作用
					{
						Wal_Flag=1;	
						LED_L_Stop=Dark;  //软停机
						EX0=0; //关闭外部中断0
						TR0=0; //定时器0停止计数
						ET0=0; //关定时器0中断
						SWL_out = Low; //左电机停机		
					}	
				}
				if((Rstop_Flag==1)&&(War_Flag==0))	//右边硬停机软停机判断
				{
					if(Rstop==1)   //右边软停机起作用
					{
						War_Flag=1;	
						LED_R_Stop=Dark;  //软停机
						EX1=0; //关闭外部中断1
						TR1=0; //定时器1停止计数
						ET1=0; //关定时器1中断
						SWR_out=Low; //右电机停		
					}	
				}	
				fn_Compare24();	
				if((Wal_Flag==1)&&(War_Flag==1))
				{
					Watch_Flag=1;//0实时检测状态  1退出实时检测状态
					IE_Flag=0;	 //0检测结束进入待机状态
				    Wal_Flag=0;  //左停机标志 
					War_Flag=0;	 //右停机标志
					IE=0x00;	//关闭所有中断
					INT_CLKO=0x00;//关闭外部中断2 3	
				}
			}
		}
    } 
}

//定时器0的中断函数
void Timer0_Interrupt( void ) interrupt 1    //
{	
    TF0 = 0;
   	if(Watch_Flag==1)//标准速度
	{
		//Left1_buf[LineL++]=60000;
	}
	else //实测速度
	{
		Left2_buf[LineL++]=65500;
	}
	if(LineL>=uc_Index_Flg)
	{
		LineL=0;
	}
}

void Timer1_Interrupt( void ) interrupt 3    //
{	
    TF1 = 0;
	if(Watch_Flag==1)
	{
		//Right1_buf[LineR++]=60000;
	}
	else
	{
		Right2_buf[LineR++]=65500;
	}
	if(LineR>=uc_Index_Flg)
	{
		LineR=0;
	}
}

//外部中断1，右脉冲 m2 的中断函数  外部脉冲下降沿触发，计数器T0从0开始计数   
void INT1_Interrupt( void ) interrupt 2
{
    EX1=0;
//	ET1=1;//开定时器1中断
    uint_m2_Right.b_Data[0] = TH1;
    uint_m2_Right.b_Data[1] = TL1;
	if(Watch_Flag==1)//取标准速度状态
	{
		Right1_buf[LineR++]=uint_m2_Right.i_Data; 
		
	}
	else	//实时检测判断状态
	{
		Right2_buf[LineR++]=uint_m2_Right.i_Data;
		INT1_Flag=1;//触发中断一次 实时检测比较一次
	}
	if(LineR>=uc_Index_Flg)
	{
		LineR=0;
	}
    TH1=0x01;
    TL1=0x01;
    EX1=1;  
}

void INT0_Interrupt( void ) interrupt 0	//左脉冲 m1
{
    EX0=0;
//	ET0=1;//开定时器0中断
    uint_m1_Left.c_Data[0] = TH0;
    uint_m1_Left.c_Data[1] = TL0;
	if(Watch_Flag==1)//标准速度
	{
		Left1_buf[LineL++]=uint_m1_Left.t_Data;
		
	}
	else //实测速度
	{
		Left2_buf[LineL++]=uint_m1_Left.t_Data;
		INT0_Flag=1;//触发中断一次 实时检测比较一次
	}
	if(LineL>=uc_Index_Flg)
	{
		LineL=0;
	}	
    TH0=0x01;
    TL0=0x01;
    EX0=1;
}
//外部中断2 左边硬停机软停机判断
void INT2_Interrupt( void ) interrupt 10
{
	if(Lstop==0)
	{
		Lstop_Flag=1;//硬停机软停机标志有效
		INT_CLKO &= 0xEF;//1110 1111禁止外部中断2
	}		
}
//外部中断3 右边硬停机软停机判断
void INT3_Interrupt( void ) interrupt 11
{
	if(Rstop==0)
	{
		Rstop_Flag=1;//硬停机软停机标志有效
		INT_CLKO &= 0xDF;//1101 1111禁止外部中断3
	}
}
void SendData(unsigned char dat)
{
    SBUF=dat;
    while(!TI);
    TI=0;
}

void fn_Set_Hyl(void)	//滑移率设置  6/24齿选择
{
	uchar uc_Hyl_Flag=0; 
	if(HY_tx4==1)
	{
		uc_Hyl_Flag|=0x08;
	}
	else
	{
		uc_Hyl_Flag&=0xF7;
	}
	if(HY_tx3==1)
	{
		uc_Hyl_Flag|=0x04;
	}
	else
	{
		uc_Hyl_Flag&=0xFB;
	}
	if(HY_tx2==1)
	{
		uc_Hyl_Flag|=0x02;
	}
	else
	{
		uc_Hyl_Flag&=0xFD;
	}
	if(HY_tx1==1)
	{
		uc_Hyl_Flag|=0x01;
	}
	else
	{
		uc_Hyl_Flag&=0xFE;
	}
	uc_Overtime_Flg=Overtime_Flg24; //6/24选择时使用的超时变量
	uc_Index_Flg=Index_Flg24;       //6/24选择时使用的存储数据下标变量
	ui_OverConstant=OverConstant_24;
	switch(uc_Hyl_Flag)
	{
		case 0x0E://0000 1110
				  Hy_sd=Hy_sd20;
				  break;
		case 0x0C:
				  Hy_sd=Hy_sd22;
				  break;
		case 0x0D:
				  Hy_sd=Hy_sd25;
				  break;
		case 0x0A:
				  Hy_sd=Hy_sd28;
				  break;
	    case 0x0B:
				  Hy_sd=Hy_sd30;
				  break;
		case 0x06:
				  Hy_sd=Hy_sd32;
				  break;
		case 0x09:
				  Hy_sd=Hy_sd34;
				  break;
		case 0x07:
				  Hy_sd=Hy_sd35;
				  break;
	    case 0x05:
				  Hy_sd=Hy_sd38;
				  break;
		case 0x03:
				  Hy_sd=Hy_sd40;
				  break;
		default:
				  Hy_sd=Hy_sd30;
				  break;				  		
	}
}

void fn_Compare24(void)//24齿判断停机算法
{
	if(Wal_Flag==0)//未触发停机信号
	{		
		if(INT0_Flag==1)//左脉冲判断
		{
			INT0_Flag=0;
			switch(LineL)  //左电机停机判断
			{
				case 0:
					   Left_t0=21;
					   Left_t1=22;
					   Left_t2=23; //当前值
					   break;
			    case 1:
					   Left_t0=22;
					   Left_t1=23;
					   Left_t2=0; //当前值
					   break;
				case 2:
					   Left_t0=23;
					   Left_t1=0;
					   Left_t2=1; //当前值
					   break;
				default:
					   Left_t0=LineL-3;
					   Left_t1=LineL-2;
				       Left_t2=LineL-1; //当前值
					   break;	
			}
			Left_linshi=Left2_buf[Left_t2]+Left2_buf[Left_t1]+Left2_buf[Left_t0];
			if(Left_linshi>ui_Average_L3)
			{
				if(((float)Left_linshi/(float)ui_Average_L3)>=Hy_sd) //连续三点达到滑移率则停机 左
				{
						Wal_Flag=1;//左电机停机标志置位
						EX0=0; //关闭外部中断0
						TR0=0; //定时器0停止计数
						ET0=0; //关定时器0中断
						SWL_out = Low; //左电机停机												
				}
			}
			//3点趋势判断
			if(((float)Left2_buf[Left_t2]/(float)Left1_buf[Left_t2])>=Hy_sd)
			{
				if(((float)Left2_buf[Left_t1]/(float)Left1_buf[Left_t1])>((float)Left2_buf[Left_t0]/(float)Left1_buf[Left_t0]))
				{
					if(((float)Left2_buf[Left_t1]/(float)Left1_buf[Left_t1])>=Hy_sd10)
					{
						if(((float)Left2_buf[Left_t0]/(float)Left1_buf[Left_t0])>=Hy_sd10)
						{
							Wal_Flag=1;//左电机停机标志置位
							EX0=0; //关闭外部中断0
							TR0=0; //定时器0停止计数
							ET0=0; //关定时器0中断
							SWL_out = Low; //左电机停机
						}
					}
				}	
			}
		}
		else
		{
			uint_m1_Left.c_Data[0] = TH0;
			uint_m1_Left.c_Data[1] = TL0;
			Left2_buf[LineL]=uint_m1_Left.t_Data;
			switch(LineL)  //左电机停机判断
			{
				case 0:
					   Left_t0=21;
					   Left_t1=22;
					   Left_t2=23; //当前值
					   break;
			    case 1:
					   Left_t0=22;
					   Left_t1=23;
					   Left_t2=0; //当前值
					   break;
				case 2:
					   Left_t0=23;
					   Left_t1=0;
					   Left_t2=1; //当前值
					   break;
				default:
					   Left_t0=LineL-3;
					   Left_t1=LineL-2;
				       Left_t2=LineL-1; //当前值
					   break;	
			}
			Left_linshi=Left2_buf[Left_t2]+Left2_buf[Left_t1]+Left2_buf[LineL];
			if(Left_linshi>ui_Average_L3)
			{
				if(((float)Left_linshi/(float)ui_Average_L3)>=Hy_sd) //连续三点达到滑移率则停机 左
				{
						Wal_Flag=1;//左电机停机标志置位
						EX0=0; //关闭外部中断0
						TR0=0; //定时器0停止计数
						ET0=0; //关定时器0中断
						SWL_out = Low; //左电机停机				
				}
			}
		}
		if((Lstop_Flag==1)&&(Wal_Flag==1))
		{
			LED_L_Stop=Bright;//硬停机先  LD3亮
		}
	}
	if(War_Flag==0)//未触发停机信号
	{
		if(INT1_Flag==1)//右脉冲判断
		{
			INT1_Flag=0;
			switch(LineR)  //右电机停机判断
			{
				
			    case 0:
					   Right_t0=21;
					   Right_t1=22;
					   Right_t2=23; //当前值
					   break;
			    case 1:
					   Right_t0=22;
					   Right_t1=23;
					   Right_t2=0; //当前值
					   break;
				case 2:
					   Right_t0=23;
					   Right_t1=0;
					   Right_t2=1; //当前值
					   break;
				default:
					   Right_t0=LineR-3;
					   Right_t1=LineR-2;
				       Right_t2=LineR-1; //当前值
					   break;	
			}
			Right_linshi=Right2_buf[Right_t2]+Right2_buf[Right_t1]+Right2_buf[Right_t0];
			if(Right_linshi>ui_Average_R3)
			{
				if(((float)Right_linshi/(float)ui_Average_R3)>=Hy_sd) //连续三点达到滑移率则停机 左
				{
						War_Flag=1;//右电机停机标志置位
						EX1=0; //关闭外部中断1
						TR1=0; //定时器1停止计数
						ET1=0; //关定时器1中断
						SWR_out=Low; //右电机停							
				}
			}
			//3点趋势判断
			if(((float)Right2_buf[Right_t2]/(float)Right1_buf[Right_t2])>=Hy_sd)
			{
				if(((float)Right2_buf[Right_t1]/(float)Right1_buf[Right_t1])>((float)Right2_buf[Right_t0]/(float)Right1_buf[Right_t0]))
				{
					if(((float)Right2_buf[Right_t1]/(float)Right1_buf[Right_t1])>=Hy_sd10)
					{
						if(((float)Right2_buf[Right_t0]/(float)Right1_buf[Right_t0])>=Hy_sd10)
						{
							War_Flag=1;//右电机停机标志置位
							EX1=0; //关闭外部中断1
							TR1=0; //定时器1停止计数
							ET1=0; //关定时器1中断
							SWR_out=Low; //右电机停
						}
					}
				}	
			}
		}
		else
		{
			uint_m2_Right.b_Data[0] = TH1;
			uint_m2_Right.b_Data[1] = TL1;
			Right2_buf[LineR]=uint_m2_Right.i_Data;
			switch(LineR)  //右电机停机判断
			{
				
			    case 0:
					   Right_t0=21;
					   Right_t1=22;
					   Right_t2=23; //当前值
					   break;
			    case 1:
					   Right_t0=22;
					   Right_t1=23;
					   Right_t2=0; //当前值
					   break;
				case 2:
					   Right_t0=23;
					   Right_t1=0;
					   Right_t2=1; //当前值
					   break;
				default:
					   Right_t0=LineR-3;
					   Right_t1=LineR-2;
				       Right_t2=LineR-1; //当前值
					   break;	
			}
			Right_linshi=Right2_buf[Right_t2]+Right2_buf[Right_t1]+Right2_buf[LineR];
			if(Right_linshi>ui_Average_R3)
			{
				if(((float)Right_linshi/(float)ui_Average_R3)>=Hy_sd)
				{
						War_Flag=1;//右电机停机标志置位
						EX1=0; //关闭外部中断1
						TR1=0; //定时器1停止计数
						ET1=0; //关定时器1中断
						SWR_out=Low; //右电机停机						
				}
			}	
		}
		if((Rstop_Flag==1)&&(War_Flag==1))
		{
			LED_R_Stop=Bright;//硬停机先  LD4亮
		}
	}
}
