/****************************************************************************
* 文件名:    w5500_app.c
* 文件历史:
* 版本号:    V1.0
* 日期:      2017.09.29    
* 作者:      qgk
* 说明:      W5500应用部分
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
#include "stm32f10x.h"
#include "W5500.h"
#include "led.h"
#include "stm32f10x.h"
#include "sys_delay.h"
#include "common.h"
#include "stm32f10x_flash.h"
char cMaster_Address=0x03;                                              //制动设备地址
unsigned char ucW5500_Test_LRC[2];                                 //校验存储区
unsigned char ucFlash_Rx_Buffer[2048];                             //数据缓存区
unsigned char  ucZB_Version[]={'B',2,3,4,5,6,7,8,9,10,11,12,13};   //版本号
unsigned  char *ucWip,*ucFip;                                      //Flash数据擦写缓存
u16 wUp_grade=0;                                                   //升级阶段
u16 wNumber_Data,wFlash_Page;                                      //数据块号，Flash页数
uint32_t uFlashDestination = ApplicationAddress;                   //应用程序地址
uint16_t wPageSize = PAGE_SIZE;                      //Flash页大小 
u16 wLED_Time_Mark=0;                                //LED定时标志
u16 wSunPackage;                                     //要传输的数据块数量
uint32_t FflashDestination = ApplicationAddress;     //擦写地址
FLASH_Status WFLASHStatus = FLASH_COMPLETE;          //Flash状态变量

   /*应答变量定义*/
#pragma pack(1)
typedef union
{
	  unsigned char 	ucACK_Data[6];
    struct 
    {
        unsigned char ucOpc[1];          //功能码      
			  u16  uError;                     //错误码
        unsigned char ucNull[2];	         //空留用
        unsigned char ucZero;	             //0
    }
		stACK_Data;
}
unACK_TypeDef;
#pragma pack()
/*应答变量重定义*/
unACK_TypeDef unACK_Answer;
/*******************************************************************************
* 函数名  : FLASH_Read_Word
* 描  述  : 读取相应地址内的数据
* 输  入  : address 数据地址
* 输  出  : 无
* 返回值  : 相应地址内存储的数据
* 说  明  : 
*******************************************************************************/
uint32_t FLASH_Read_Word(uint32_t address)	
{
    return(*(__IO uint32_t*)address);
}
/*******************************************************************************
* 函数名  : Load_Net_Parameters
* 描  述  : 装载网络参数
* 输  入  : 无
* 输  出  : 无
* 返回值  : 无
* 说  明  : 网关、掩码、物理地址、本机IP地址、端口号
*******************************************************************************/
void Load_Net_Parameters(void)
{
		unsigned char 	ucW5500_Id[6];
		#pragma pack(1)
  union 
  {
   unsigned char 	ucW5500_Id[12];
   struct 
   {
   u32 wW5500_Id[3];

   }
   stW5500_Id;
   }uW5500_Id;			
	#pragma pack() 
	uint8_t W5500_Parameter[27];    //网络IP参数变量
	 //IP参数读取
	SPI_FLASH_BufferRead(W5500_Parameter,20,27);
	 
				uW5500_Id.stW5500_Id.wW5500_Id[0] = *(__IO u32*)(0X1FFFF7E8); 
        uW5500_Id.stW5500_Id.wW5500_Id[1] = *(__IO u32 *)(0X1FFFF7EC); 
        uW5500_Id.stW5500_Id.wW5500_Id[2] = *(__IO u32 *)(0X1FFFF7F0); 
			
			  ucW5500_Id[0]=(unsigned char)((uW5500_Id.stW5500_Id.wW5500_Id[1]&0x0000FF00)>>8);
			  if(ucW5500_Id[0]%2)
				{
		  	ucW5500_Id[0]=ucW5500_Id[0]-1;	
				}
			  ucW5500_Id[1]=(unsigned char)(uW5500_Id.stW5500_Id.wW5500_Id[1]&0x000000FF);
			  ucW5500_Id[2]=(unsigned char)((uW5500_Id.stW5500_Id.wW5500_Id[0]&0xFF000000)>>24);
			  ucW5500_Id[3]=(unsigned char)((uW5500_Id.stW5500_Id.wW5500_Id[0]&0x00FF0000)>>16);
			  ucW5500_Id[4]=(unsigned char)((uW5500_Id.stW5500_Id.wW5500_Id[0]&0x0000FF00)>>8);
			  ucW5500_Id[5]=(unsigned char)(uW5500_Id.stW5500_Id.wW5500_Id[0]&0x000000FF);
				
				Phy_Addr[0]=ucW5500_Id[0];    //加载物理地址
        Phy_Addr[1]=ucW5500_Id[1];
        Phy_Addr[2]=ucW5500_Id[2];
        Phy_Addr[3]=ucW5500_Id[3];
        Phy_Addr[4]=ucW5500_Id[4];
        Phy_Addr[5]=ucW5500_Id[5];

        Gateway_IP[0]= W5500_Parameter[0]; //加载网关参数
        Gateway_IP[1]= W5500_Parameter[1];
        Gateway_IP[2]= W5500_Parameter[2];
        Gateway_IP[3]= W5500_Parameter[3];
        
        Sub_Mask[0]=W5500_Parameter[4];     //加载子网掩码
        Sub_Mask[1]=W5500_Parameter[5];
        Sub_Mask[2]=W5500_Parameter[6];
        Sub_Mask[3]=W5500_Parameter[7];


//         Phy_Addr[0]=0x00;    //加载物理地址
//         Phy_Addr[1]=0x0C;
//         Phy_Addr[2]=0x29;
//         Phy_Addr[3]=0x7D;
//         Phy_Addr[4]=0x64;
//         Phy_Addr[5]=0x01;
        
				IP_Addr[0]=W5500_Parameter[8];//加载本机IP地址   称重 3
				IP_Addr[1]=W5500_Parameter[9];
				IP_Addr[2]=W5500_Parameter[10];
				IP_Addr[3]=W5500_Parameter[11];
        
        S0_Port[0]=W5500_Parameter[13];   //加载端口0的端口号50000 
        S0_Port[1]=W5500_Parameter[12];
   
        S0_Mode=TCP_SERVER;//加载端口0的工作模式,TCP客户端模式
}

/*******************************************************************************
* 函数名  : W5500_Initialization
* 描  述  : W5500初始化
* 输  入  : 无
* 输  出  : 无
* 返回值  : 无
* 说  明  : 无
*******************************************************************************/
void W5500_Initialization(void)
{
    W5500_Init();		  //初始化W5500寄存器函数
    Detect_Gateway();	//检查网关服务器 
    Socket_Init(0);		//指定Socket(0~7)初始化,初始化端口0
    
}
/*******************************************************************************
* 函数名  : W5500_LRC
* 描  述  : 校验位获取
* 输  入  : *ucData:数据首地址；wLength：数据长度
* 输  出  : 无
* 返回值  : 校验值
* 说  明  : 无�
*******************************************************************************/
 unsigned char W5500_LRC(unsigned char*ucData,unsigned short wLength)
{
    unsigned char uchLRC=0;
    int i;
    for(i=1;i<wLength;i++)
    {
        uchLRC+=ucData[i];
    }
    return uchLRC;
}
/*******************************************************************************
* 函数名  : W5500_Return_Data
* 描  述  : 回传数据
* 输  入  : ucOrder：命令码；*uc_Data_Buffer：回传数据首地址；wSize：数据长度
* 输  出  : 无
* 返回值  : 无
*******************************************************************************/
void W5500_Return_Data(unsigned char ucOrder, unsigned char *uc_Data_Buffer, unsigned short wSize)
{
    unsigned short wSize2,wSize3,wi,wj;
    char cW5500_LRC;
    unsigned char  uc_W5500_Buffer[180];      
    uc_W5500_Buffer[0]=0x02;                
    uc_W5500_Buffer[1]=cMaster_Address|0X80;    
    wSize2=wSize+1;
    wSize3=wSize+3;
    uc_W5500_Buffer[2]=wSize2&0XFF;
    uc_W5500_Buffer[3]=(wSize2>>8)&0XFF;		 
    uc_W5500_Buffer[4]=ucOrder;
    wj=5;
    if(wSize!=0)
    {
        for(wi=0;wi<wSize;wi++,wj++)
        {
            uc_W5500_Buffer[wj]=(*uc_Data_Buffer++);
        }
    }
    wSize3=wSize+5; 
    cW5500_LRC=W5500_LRC(uc_W5500_Buffer,wSize3); 
    uc_W5500_Buffer[wSize+5]=cW5500_LRC;
    wSize2=wSize+6;	 
    uc_W5500_Buffer[wSize2]=0X03;
    wSize2=wSize+7;
    memcpy(W5500_Tx_Buffer, uc_W5500_Buffer, wSize2);
    Write_SOCK_Data_Buffer(0, uc_W5500_Buffer,wSize2);
}


/******************************************************************************
* 函数名  : Process_Socket_Data
* 描  述  : 指令解析
* 输  入  : s:端口号
* 输  出  : 无
* 返回值  : 无
* 说  明  : 升级命令、升级请求、写请求解析。
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
    u16 wNumber_Write;
	  uint32_t *uData_Write;
    unsigned short wSize;
    unsigned char  ucOrder;
    uint32_t *uData_Read=((uint32_t*)(&ucFlash_Rx_Buffer[0]));
    wSize=Read_SOCK_Data_Buffer(s, W5500_Rx_Buffer);
    ucW5500_Test_LRC[0]=W5500_Rx_Buffer[wSize-2];                           
    ucW5500_Test_LRC[1]=W5500_LRC(W5500_Rx_Buffer,wSize-2);   
    if((ucW5500_Test_LRC[0]!=ucW5500_Test_LRC[1])||(W5500_Rx_Buffer[0]!=2)||(W5500_Rx_Buffer[wSize-1]!=3)||(W5500_Rx_Buffer[1]!=cMaster_Address))           //LRC位判断
    {
        W5500_Return_Data('Z', ucZB_Version, 0); 
    }
    else
    {
        ucOrder=W5500_Rx_Buffer[4];
			 if((ucOrder=='B')||(ucOrder=='W'))
			 {
        switch(ucOrder)
        {
            case'B':                                                         //获取版本信息指令                                                               
            {
               W5500_Return_Data('B',ucZB_Version,13);
            }
            break;
            case'W':                                                         //升级命令
            {
                ucOrder=W5500_Rx_Buffer[5];
                switch(ucOrder)
                {
									case'G':                                                 //重新升级命请求
                    {
											      FLASH_Lock();
                            unACK_Answer.ucACK_Data[0]='G';
                            W5500_Return_Data('W',unACK_Answer.ucACK_Data,1);
											      TimeDelay_ms(10);//延时10ms
                            NVIC_SystemReset();
                    }
                    break;
									case'W':                                                 //升级命请求
                    {
                        if(wUp_grade==2)
                        {
                            unACK_Answer.ucACK_Data[0]='W';
													  wSunPackage=W5500_Rx_Buffer[7];                                                 //要传输的数据块数量
                            wSunPackage<<=8;
												    wSunPackage+=W5500_Rx_Buffer[6];
													  wSunPackage-=1;       //下标从0开始，所以减1
													  wUp_grade=4;
                            W5500_Return_Data('W',unACK_Answer.ucACK_Data,1);
                        }
                        else             //E5流程错误
												{
											  unACK_Answer.stACK_Data.ucOpc[0]='E';     
                        unACK_Answer.stACK_Data.uError=5;
                        unACK_Answer.stACK_Data.ucNull[0]=0;
                        unACK_Answer.stACK_Data.ucNull[1]=0;
                        unACK_Answer.stACK_Data.ucZero=0;
                        W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                        }
                    }
                    break;
                    
                    case'D':                                                 //写数据
                    {
                        
                        if(wUp_grade>=4)                                      //步骤判断
                        {
                            if(wUp_grade==4)
                            {
															  /*Flash 解锁*/
                                FLASH_Unlock();
															  FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);
                                uData_Write=uData_Read;
                                wUp_grade=5;
                             }
				
                            unACK_Answer.ucACK_Data[1]=W5500_Rx_Buffer[6];
                            unACK_Answer.ucACK_Data[2]=W5500_Rx_Buffer[7];
													 /******************************************************************
                             ********************块号异常 E4 **********************************
                            ******************************************************************/	
                            if(unACK_Answer.stACK_Data.uError!=wNumber_Data)      //与存储的写入块号对比
                            {
                                unACK_Answer.stACK_Data.ucOpc[0]='E';
                                unACK_Answer.stACK_Data.uError=4;
                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                unACK_Answer.stACK_Data.ucZero=0;
                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
															  return;
                            }
                            else
                            {
													         if((wSize<522)||(wSunPackage==unACK_Answer.stACK_Data.uError))      //数据量小于512
                                    {
														        if(wSunPackage!=unACK_Answer.stACK_Data.uError)  //不为最后一块数据
																	    {
																	   	unACK_Answer.stACK_Data.ucOpc[0]='E';
                                      unACK_Answer.stACK_Data.uError=2;                //长度异常
                                      unACK_Answer.stACK_Data.ucNull[0]=0;
                                      unACK_Answer.stACK_Data.ucNull[1]=0;
                                      unACK_Answer.stACK_Data.ucZero=0;
                                      W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
																	    wUp_grade=7;
																			return;
                                      }
                                     else
                                     {	
                                        ucWip=(&W5500_Rx_Buffer[8]);	    //需要拷贝数据的首地址
                                        ucFip=((&ucFlash_Rx_Buffer[0])+(wNumber_Data%4)*512);	    //计算缓存区的地址
                                        memcpy(ucFip,ucWip,(wSize-10)); 			
                                        while(FLASH_GetFlagStatus(FLASH_FLAG_BSY)==SET)
                                        {
                                            ;
                                        }
                                        WFLASHStatus = FLASH_ErasePage(uFlashDestination + (wPageSize * wFlash_Page));
                                        if(WFLASHStatus!=FLASH_COMPLETE)
																				{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                                 //E8擦除异常
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																					      return;	
                                         }
                                        uData_Write=uData_Read;
                                        for (wNumber_Write = 0; wNumber_Write <((wSize-10)+wNumber_Data%4*512); wNumber_Write += 4)
                                        {
                                            WFLASHStatus=FLASH_ProgramWord(FflashDestination, *uData_Write);	
                                            if(WFLASHStatus!=FLASH_COMPLETE)
																						{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=7;                                 //E7写入异常
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																							  return;
                                               } 
                                            /******************************************************************
                                                              E3  校对异常
                                            ******************************************************************/	
                                            if(FLASH_Read_Word(FflashDestination)!=*uData_Write)
                                            {
                                                unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=3;
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																							  return;
                                            }	
                                            FflashDestination+=4;
                                            uData_Write+=1;	
                                          } 
																					
																				WFLASHStatus = FLASH_ErasePage(2048*255);      //清标志位，位置内容自己定义。
                                        if(WFLASHStatus!=FLASH_COMPLETE)
																				{
																								unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                         //E8擦除异常
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																							  return;	
                                         }			
																					
																					
																					
																					
																		wUp_grade=6;				
                                    unACK_Answer.ucACK_Data[0]='D';
                                    unACK_Answer.ucACK_Data[1]=	W5500_Rx_Buffer[6];
                                    unACK_Answer.ucACK_Data[2]=	W5500_Rx_Buffer[7];
                                    W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
																	  return;	
                                     }	
                                }	
                                else if(wSize==522)   
                                { 
                                    ucWip=(&W5500_Rx_Buffer[8]);	
                                    ucFip=(&ucFlash_Rx_Buffer[0]+(wNumber_Data%4)*512);	
                                    memcpy(ucFip,ucWip, 512); 	
                                    wNumber_Data+=1;        //接收到的数据包次数
                                    if(wNumber_Data/4>wFlash_Page)         
                                    {
                                        WFLASHStatus = FLASH_ErasePage(uFlashDestination + (wPageSize * wFlash_Page));
                                        if(WFLASHStatus != FLASH_COMPLETE)
																				{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                                 //E8擦除异常
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																					      return;	
																				}
                                        wFlash_Page+=1;        //Flash页
                                        /* Flash写入*/	
                                        uData_Write=uData_Read;
                                        for (wNumber_Write = 0; wNumber_Write <wPageSize ; wNumber_Write += 4)
                                        { 
                                            WFLASHStatus=FLASH_ProgramWord(FflashDestination, *uData_Write);
                                            if(WFLASHStatus!=FLASH_COMPLETE)
																						{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=7;                                 //E7写入异常
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																							  return;	
                                               } 
                                            /******************************************************************
                                                                    读写异常
                                            ******************************************************************/		
                                            if(FLASH_Read_Word(FflashDestination)!=*uData_Write)
                                            {
                                                unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=3;
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
																							  wUp_grade=7;
																							  return;	
                                            }								 
                                            FflashDestination+=4;
                                            uData_Write+=1;	
                                        }						
                                    }
                                    unACK_Answer.ucACK_Data[0]='D';
                                    unACK_Answer.ucACK_Data[1]=	W5500_Rx_Buffer[6];
                                    unACK_Answer.ucACK_Data[2]=	W5500_Rx_Buffer[7];
                                    W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);
                                }
                                /******************************************************************
                                                           长度异常
                                ******************************************************************/	
                                else              //数据超出长度
                                {
                                    unACK_Answer.stACK_Data.ucOpc[0]='E';
                                    unACK_Answer.stACK_Data.uError=2;
                                    unACK_Answer.stACK_Data.ucNull[0]=0;
                                    unACK_Answer.stACK_Data.ucNull[1]=0;
                                    unACK_Answer.stACK_Data.ucZero=0;
                                    W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
																	  wUp_grade=7;
																	  return;
                                }

                            }
                        }
                        else      //升级步骤错误
												{
												unACK_Answer.stACK_Data.ucOpc[0]='E';     
                        unACK_Answer.stACK_Data.uError=5;
                        unACK_Answer.stACK_Data.ucNull[0]=0;
                        unACK_Answer.stACK_Data.ucNull[1]=0;
                        unACK_Answer.stACK_Data.ucZero=0;
                        W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                        }	
                    }
                    break;
                    default:
                    break;
                }
            }
            break;
            default:
            break;
        }
			  }
				else
				{
          unACK_Answer.stACK_Data.ucOpc[0]='E';
          unACK_Answer.stACK_Data.uError=1;                   //升级程序下无法执行指令回复
          unACK_Answer.stACK_Data.ucNull[0]=0;
          unACK_Answer.stACK_Data.ucNull[1]=0;
          unACK_Answer.stACK_Data.ucZero=0;
          W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);		

       }
    }
}
/*******************************************************************************
* 函数名  : W5500_DataHandling
* 描  述  : W5500运行函数
* 输  入  : 无
* 输  出  : 无
* 返回值  : 无
* 说  明  : W5500启动，数据处理
*******************************************************************************/
void W5500_DataHandling(void)
{
    if((ucW5500_InitCTR==0)||(ucW5500_InitCTR==1))
    {
        W5500_Hardware_Reset();
    }
    if(ucW5500_InitCTR==2)
    {
        W5500_Initialization();		
        ucW5500_InitCTR=3;
    }
    if(ucW5500_InitCTR==3)
    {
        W5500_Socket_Set();          //W5500端口初始化配置
        
        if(W5500_Interrupt)          //处理W5500中断
        {
            W5500_Interrupt_Process();//W5500中断处理程序框架
        }
        if((S0_Data & S_RECEIVE) == S_RECEIVE)  //如果Socket0接收到数据
        {
            S0_Data&=~S_RECEIVE;
            Process_Socket_Data(0);              //W5500数据处理
        }
    }
    if(wUp_grade==1)
    {
        if(FLASH_Read_Word(2048 * 255)==0X77777777)   //判断升级标志   自己定义
        {
            wUp_grade=2;
        }
        else
        wUp_grade=6;
    }

}

