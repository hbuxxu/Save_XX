/****************************************************************************
* �ļ���:    w5500_app.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2017.09.29    
* ����:      qgk
* ˵��:      W5500Ӧ�ò���
*****************************************************************************/
/*=============================================================================
*******�޸���־*******
1����    �ڣ�
��    �㣺
�� �� �ˣ�
������ݣ�
�汾��:
2����    �ڣ�
��    �㣺
�� �� �ˣ�
������ݣ�
�汾��:
================================================================================*/
#include "stm32f10x.h"
#include "W5500.h"
#include "led.h"
#include "stm32f10x.h"
#include "sys_delay.h"
#include "common.h"
#include "stm32f10x_flash.h"
char cMaster_Address=0x02;                                              //�����豸��ַ
unsigned char ucW5500_Test_LRC[2];                                 //У��洢��
unsigned char ucFlash_Rx_Buffer[2048];                             //���ݻ�����
unsigned char  ucZB_Version[]={'B',2,3,4,5,6,7,8,9,10,11,12,13};   //�汾��
unsigned  char *ucWip,*ucFip;                                      //Flash���ݲ�д����
u16 wUp_grade=0;                                                   //�����׶�
u16 wNumber_Data,wFlash_Page;                                      //���ݿ�ţ�Flashҳ��
uint32_t uFlashDestination = ApplicationAddress;                   //Ӧ�ó����ַ
uint16_t wPageSize = PAGE_SIZE;                      //Flashҳ��С 
u16 wLED_Time_Mark=0;                                //LED��ʱ��־
u16 wSunPackage;                                     //Ҫ��������ݿ�����
uint32_t FflashDestination = ApplicationAddress;     //��д��ַ
FLASH_Status WFLASHStatus = FLASH_COMPLETE;          //Flash״̬����

   /*Ӧ���������*/
#pragma pack(1)
typedef union
{
	  unsigned char 	ucACK_Data[6];
    struct 
    {
        unsigned char ucOpc[1];          //������      
			  u16  uError;                     //������
        unsigned char ucNull[2];	         //������
        unsigned char ucZero;	             //0
    }
		stACK_Data;
}
unACK_TypeDef;
#pragma pack()
/*Ӧ������ض���*/
unACK_TypeDef unACK_Answer;
/*******************************************************************************
* ������  : FLASH_Read_Word
* ��  ��  : ��ȡ��Ӧ��ַ�ڵ�����
* ��  ��  : address ���ݵ�ַ
* ��  ��  : ��
* ����ֵ  : ��Ӧ��ַ�ڴ洢������
* ˵  ��  : 
*******************************************************************************/
uint32_t FLASH_Read_Word(uint32_t address)	
{
    return(*(__IO uint32_t*)address);
}
/*******************************************************************************
* ������  : Load_Net_Parameters
* ��  ��  : װ���������
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : ��
* ˵  ��  : ���ء����롢�����ַ������IP��ַ���˿ں�
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
	uint8_t W5500_Parameter[27];    //����IP��������
	 //IP������ȡ
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
				
				Phy_Addr[0]=ucW5500_Id[0];    //���������ַ
        Phy_Addr[1]=ucW5500_Id[1];
        Phy_Addr[2]=ucW5500_Id[2];
        Phy_Addr[3]=ucW5500_Id[3];
        Phy_Addr[4]=ucW5500_Id[4];
        Phy_Addr[5]=ucW5500_Id[5];

        Gateway_IP[0]= W5500_Parameter[0]; //�������ز���
        Gateway_IP[1]= W5500_Parameter[1];
        Gateway_IP[2]= W5500_Parameter[2];
        Gateway_IP[3]= W5500_Parameter[3];
        
        Sub_Mask[0]=W5500_Parameter[4];     //������������
        Sub_Mask[1]=W5500_Parameter[5];
        Sub_Mask[2]=W5500_Parameter[6];
        Sub_Mask[3]=W5500_Parameter[7];


//         Phy_Addr[0]=0x00;    //���������ַ
//         Phy_Addr[1]=0x0C;
//         Phy_Addr[2]=0x29;
//         Phy_Addr[3]=0x7D;
//         Phy_Addr[4]=0x64;
//         Phy_Addr[5]=0x01;
        
				IP_Addr[0]=W5500_Parameter[8];//���ر���IP��ַ   ���� 3
				IP_Addr[1]=W5500_Parameter[9];
				IP_Addr[2]=W5500_Parameter[10];
				IP_Addr[3]=W5500_Parameter[11];
        
        S0_Port[0]=W5500_Parameter[13];   //���ض˿�0�Ķ˿ں�50000 
        S0_Port[1]=W5500_Parameter[12];
   
        S0_Mode=TCP_SERVER;//���ض˿�0�Ĺ���ģʽ,TCP�ͻ���ģʽ
}

/*******************************************************************************
* ������  : W5500_Initialization
* ��  ��  : W5500��ʼ��
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : ��
* ˵  ��  : ��
*******************************************************************************/
void W5500_Initialization(void)
{
    W5500_Init();		  //��ʼ��W5500�Ĵ�������
    Detect_Gateway();	//������ط����� 
    Socket_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
    
}
/*******************************************************************************
* ������  : W5500_LRC
* ��  ��  : У��λ��ȡ
* ��  ��  : *ucData:�����׵�ַ��wLength�����ݳ���
* ��  ��  : ��
* ����ֵ  : У��ֵ
* ˵  ��  : �ޣ
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
* ������  : W5500_Return_Data
* ��  ��  : �ش�����
* ��  ��  : ucOrder�������룻*uc_Data_Buffer���ش������׵�ַ��wSize�����ݳ���
* ��  ��  : ��
* ����ֵ  : ��
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
* ������  : Process_Socket_Data
* ��  ��  : ָ�����
* ��  ��  : s:�˿ں�
* ��  ��  : ��
* ����ֵ  : ��
* ˵  ��  : ���������������д���������
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
    if((ucW5500_Test_LRC[0]!=ucW5500_Test_LRC[1])||(W5500_Rx_Buffer[0]!=2)||(W5500_Rx_Buffer[wSize-1]!=3)||(W5500_Rx_Buffer[1]!=cMaster_Address))           //LRCλ�ж�
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
            case'B':                                                         //��ȡ�汾��Ϣָ��                                                               
            {
               W5500_Return_Data('B',ucZB_Version,13);
            }
            break;
            case'W':                                                         //��������
            {
                ucOrder=W5500_Rx_Buffer[5];
                switch(ucOrder)
                {
									case'G':                                                 //��������������
                    {
											      FLASH_Lock();
                            unACK_Answer.ucACK_Data[0]='G';
                            W5500_Return_Data('W',unACK_Answer.ucACK_Data,1);
											      TimeDelay_ms(10);//��ʱ10ms
                            NVIC_SystemReset();
                    }
                    break;
									case'W':                                                 //����������
                    {
                        if(wUp_grade==2)
                        {
                            unACK_Answer.ucACK_Data[0]='W';
													  wSunPackage=W5500_Rx_Buffer[7];                                                 //Ҫ��������ݿ�����
                            wSunPackage<<=8;
												    wSunPackage+=W5500_Rx_Buffer[6];
													  wSunPackage-=1;       //�±��0��ʼ�����Լ�1
													  wUp_grade=4;
                            W5500_Return_Data('W',unACK_Answer.ucACK_Data,1);
                        }
                        else             //E5���̴���
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
                    
                    case'D':                                                 //д����
                    {
                        
                        if(wUp_grade>=4)                                      //�����ж�
                        {
                            if(wUp_grade==4)
                            {
															  /*Flash ����*/
                                FLASH_Unlock();
															  FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);
                                uData_Write=uData_Read;
                                wUp_grade=5;
                             }
				
                            unACK_Answer.ucACK_Data[1]=W5500_Rx_Buffer[6];
                            unACK_Answer.ucACK_Data[2]=W5500_Rx_Buffer[7];
													 /******************************************************************
                             ********************����쳣 E4 **********************************
                            ******************************************************************/	
                            if(unACK_Answer.stACK_Data.uError!=wNumber_Data)      //��洢��д���ŶԱ�
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
													         if((wSize<522)||(wSunPackage==unACK_Answer.stACK_Data.uError))      //������С��512
                                    {
														        if(wSunPackage!=unACK_Answer.stACK_Data.uError)  //��Ϊ���һ������
																	    {
																	   	unACK_Answer.stACK_Data.ucOpc[0]='E';
                                      unACK_Answer.stACK_Data.uError=2;                //�����쳣
                                      unACK_Answer.stACK_Data.ucNull[0]=0;
                                      unACK_Answer.stACK_Data.ucNull[1]=0;
                                      unACK_Answer.stACK_Data.ucZero=0;
                                      W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
																	    wUp_grade=7;
																			return;
                                      }
                                     else
                                     {	
                                        ucWip=(&W5500_Rx_Buffer[8]);	    //��Ҫ�������ݵ��׵�ַ
                                        ucFip=((&ucFlash_Rx_Buffer[0])+(wNumber_Data%4)*512);	    //���㻺�����ĵ�ַ
                                        memcpy(ucFip,ucWip,(wSize-10)); 			
                                        while(FLASH_GetFlagStatus(FLASH_FLAG_BSY)==SET)
                                        {
                                            ;
                                        }
                                        WFLASHStatus = FLASH_ErasePage(uFlashDestination + (wPageSize * wFlash_Page));
                                        if(WFLASHStatus!=FLASH_COMPLETE)
																				{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                                 //E8�����쳣
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
                                                unACK_Answer.stACK_Data.uError=7;                                 //E7д���쳣
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																							  return;
                                               } 
                                            /******************************************************************
                                                              E3  У���쳣
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
																					
																				WFLASHStatus = FLASH_ErasePage(2048*255);      //���־λ��λ�������Լ����塣
                                        if(WFLASHStatus!=FLASH_COMPLETE)
																				{
																								unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                         //E8�����쳣
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
                                    wNumber_Data+=1;        //���յ������ݰ�����
                                    if(wNumber_Data/4>wFlash_Page)         
                                    {
                                        WFLASHStatus = FLASH_ErasePage(uFlashDestination + (wPageSize * wFlash_Page));
                                        if(WFLASHStatus != FLASH_COMPLETE)
																				{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                                 //E8�����쳣
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																					      return;	
																				}
                                        wFlash_Page+=1;        //Flashҳ
                                        /* Flashд��*/	
                                        uData_Write=uData_Read;
                                        for (wNumber_Write = 0; wNumber_Write <wPageSize ; wNumber_Write += 4)
                                        { 
                                            WFLASHStatus=FLASH_ProgramWord(FflashDestination, *uData_Write);
                                            if(WFLASHStatus!=FLASH_COMPLETE)
																						{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=7;                                 //E7д���쳣
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																							  return;	
                                               } 
                                            /******************************************************************
                                                                    ��д�쳣
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
                                                           �����쳣
                                ******************************************************************/	
                                else              //���ݳ�������
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
                        else      //�����������
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
          unACK_Answer.stACK_Data.uError=1;                   //�����������޷�ִ��ָ��ظ�
          unACK_Answer.stACK_Data.ucNull[0]=0;
          unACK_Answer.stACK_Data.ucNull[1]=0;
          unACK_Answer.stACK_Data.ucZero=0;
          W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);		

       }
    }
}
/*******************************************************************************
* ������  : W5500_DataHandling
* ��  ��  : W5500���к���
* ��  ��  : ��
* ��  ��  : ��
* ����ֵ  : ��
* ˵  ��  : W5500���������ݴ���
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
        W5500_Socket_Set();          //W5500�˿ڳ�ʼ������
        
        if(W5500_Interrupt)          //����W5500�ж�
        {
            W5500_Interrupt_Process();//W5500�жϴ��������
        }
        if((S0_Data & S_RECEIVE) == S_RECEIVE)  //���Socket0���յ�����
        {
            S0_Data&=~S_RECEIVE;
            Process_Socket_Data(0);              //W5500���ݴ���
        }
    }
    if(wUp_grade==1)
    {
        if(FLASH_Read_Word(2048 * 255)==0X77777777)   //�ж�������־   �Լ�����
        {
            wUp_grade=2;
        }
        else
        wUp_grade=6;
    }

}

