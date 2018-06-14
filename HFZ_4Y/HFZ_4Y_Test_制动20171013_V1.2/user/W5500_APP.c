/****************************************************************************
* �ļ���: W5500_APP.c
* �ļ���ʷ:
* �汾��    V1.0
* ����      2016.09.29    
* ����      qgk
* ˵��      
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
#include "W5500.h"	
#include "W5500_APP.h"
unsigned char ucW5500_InitCTR=0;
FLASH_Status WFLASHStatus = FLASH_COMPLETE;

// static uint8_t u8flash_buffer[21];//�궨ϵ��  ������ʱ�洢����
static	_ADCab _ADCabVariable;  //������ϱ���
/*******************************************************************************
* ������  : IDtoMAC
* ����    : ��ȡSTM32F10X ID����λ��ΪMAC��ַ
* ����    : *ulp ָ��MAC��ַ�����ָ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��Phy_Addr[]Ϊ�洢MAC�����飬ʹ��IDtoMAC(Phy_Addr)����
*******************************************************************************/
void IDtoMAC(u8 *ulp)	
{
	u32 ultmp1,ultmp2;

    //��ȡ��ƷID
	ultmp1=*(__IO u32*)(0x1FFFF7EC);
	ultmp2=*(__IO u32*)(0x1FFFF7F0);

	*ulp++ = (u8)(((ultmp1 & 0x00FF0000)>>16)&0xfe);	//���ֽ����λΪ��
	*ulp++ = (u8) ((ultmp1 & 0xFF000000)>>24);
	*ulp++ = (u8)  (ultmp2 & 0x000000FF);
	*ulp++ = (u8) ((ultmp2 & 0x0000FF00)>>8);
	*ulp++ = (u8) ((ultmp2 & 0x00FF0000)>>16);
	*ulp =   (u8) ((ultmp2 & 0xFF000000)>>24);
}

/*******************************************************************************
* ������  : Load_Net_Parameters
* ����    : װ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
void Load_Net_Parameters(void)
{
	  Gateway_IP[0] = W5500_Parameter[0];//�������ز���
		Gateway_IP[1] = W5500_Parameter[1];
		Gateway_IP[2] = W5500_Parameter[2];
		Gateway_IP[3] = W5500_Parameter[3];
		Sub_Mask[0]=W5500_Parameter[4];//������������
		Sub_Mask[1]=W5500_Parameter[5];
		Sub_Mask[2]=W5500_Parameter[6];
		Sub_Mask[3]=W5500_Parameter[7];			
		IDtoMAC(Phy_Addr);  //���������ַ
		IP_Addr[0]=W5500_Parameter[8];//���ر���IP��ַ   ���� 3
		IP_Addr[1]=W5500_Parameter[9];
		IP_Addr[2]=W5500_Parameter[10];
		IP_Addr[3]=W5500_Parameter[11];
		S0_Port[0] = 0x13;//���ض˿�0�Ķ˿ں�5000
		S0_Port[1] = 0x88;
		S0_DIP[0]=192;//���ض˿�0��Ŀ��IP��ַ
		S0_DIP[1]=168;
		S0_DIP[2]=148;
		S0_DIP[3]=57;			
		S0_DPort[0] = 0x17;//���ض˿�0��Ŀ�Ķ˿ں�6000
		S0_DPort[1] = 0x70;
		S0_Mode=TCP_SERVER;//���ض˿�0�Ĺ���ģʽ,TCP�ͻ���ģʽ	    
		
			//===UDPͨ�Ų���
		S1_Port[0] = W5500_Parameter[25];   //�ӿ�1�ı��ض˿ں�25031
		S1_Port[1] = W5500_Parameter[24];	
		//=UDP�㲥Ŀ��IP��ַ��˿ں�
		UDP_DIPR[0]=W5500_Parameter[20];	//UDP(�㲥)ģʽ,Ŀ������IP��ַ  
		UDP_DIPR[1]=W5500_Parameter[21];
		UDP_DIPR[2]=W5500_Parameter[22];
		UDP_DIPR[3]=W5500_Parameter[23];
		UDP_DPORT[0]=W5500_Parameter[25];//Ŀ�Ķ˿ں�60000  25031
		UDP_DPORT[1]=W5500_Parameter[24];
		S1_Mode=UDP_MODE;//				
		
		S2_Port[0] = W5500_Parameter[13];//�˿�2�Ķ˿ں�25032  ����������
		S2_Port[1] = W5500_Parameter[12];				
		S2_Mode=TCP_SERVER;//���ض˿�2�Ĺ���ģʽ,TCP�ͻ���ģʽ
    
    //����ʹ�ö˿�3
    S3_Port[0] = 0x1A;//�˿�3�Ķ˿ں�6666  ����������
		S3_Port[1] = 0x0A;				
		S3_Mode=TCP_SERVER;//���ض˿�2�Ĺ���ģʽ,TCP�ͻ���ģʽ
}
/*******************************************************************************
* ������  : W5500_Initialization
* ����    : W5500��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//��ʼ��W5500�Ĵ�������
	Detect_Gateway();	//������ط����� 
	Socket_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0  //�����  �����������
	Socket_Init(1);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�1
	Socket_Init(2);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�2  //�����  ����������
  Socket_Init(3);   //����ʹ��
}
/*******************************************************************************
* ������  : W5500_Hardware_Reset
* ����    : Ӳ����λW5500
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : W5500�ĸ�λ���ű��ֵ͵�ƽ����500us����,��������W5500
*******************************************************************************/
void W5500_Hardware_Reset(void)
{
	if(ucW5500_InitCTR==0)
  {
			GPIO_ResetBits(W5500_RST_PORT, W5500_RST);//��λ��������
			Delay_ms(50);
			GPIO_SetBits(W5500_RST_PORT, W5500_RST);//��λ��������
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
* ������  : W5500_Send_Data
* ����    : W5500����Э����������
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : 
*******************************************************************************/
void W5500_Send_Data(uint8_t s,uint8_t *Data)
{
		uint16_t i,u16_size=0;
// 		uint8_t u8Send_Buffer[BUFFER_SIZE] = {3,0,'0','0','0'};
		uint8_t u8_Lrc=0;
		u16_size = *(Data+1)<<8 | *Data; //֡��
		u16_size +=  6;     												//�ܳ�=֡��+ʣ���ֽڳ���
		u8Send_Buffer[0] = CON_START;
// 		if(SysParameter.Par12_MachineType == 0) //����
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
    //�˿�3����ʹ��
    if(s==0)
    {
        Write_SOCK_Data_Buffer(3,u8Send_Buffer,u16_size);
    }
// 		for(i=0;i<u16_size;i++)
// 		{
// 				printf("%x ",u8Send_Buffer[i]);
// 		}
// 		printf("\r\n");
}

uint32_t FLASH_Read_Word(uint32_t address)	
{
    return(*(__IO uint32_t*)address);
}	

/*******************************************************************************
* ������  : Process_Socket_Data
* ����    : W5500���ղ����ͽ��յ�������
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : �������ȵ���S_rx_process()��W5500�Ķ˿ڽ������ݻ�������ȡ����,
*			Ȼ�󽫶�ȡ�����ݴ�Rx_Buffer������Temp_Buffer���������д���
*			������ϣ������ݴ�Temp_Buffer������Tx_Buffer������������S_tx_process()
*			�������ݡ�
*******************************************************************************/
void W5500_Socket_Data(SOCKET s)
{
	uint16_t u16_size;
	uint16_t i;  //forѭ������ʹ��
	uint8_t u8read_flg;//���궨ϵ����־
	uint8_t u8flash_buffer[21];//�궨ϵ��  ������ʱ�洢����
	/* ֡����λ		֡����λ   ����   У��*/
	uint8_t u8_Command,u8_Lrc=0;
	uint16_t u16_Length;
	uint8_t Remainder;  //����
  uint16_t Quotient;      //��
	int8_t cCheck_Result;//����У��
	
// 	_ADCab _ADCabVariable;  //������ϱ���
	
	u16_size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
//	for(i=0;i<u16_size;i++)
//	{
//			printf("%x ",Rx_Buffer[i]);
//	}
//	printf("\r\n");
	if(((Rx_Buffer[0] == CON_START) && (Rx_Buffer[u16_size-1] == CON_END) && (Rx_Buffer[4] == 'F')) == 1)
	{
			 SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
				for(i=0;i<24;i++) //���������ݴ洢����ʱ����Parameter_Buffer
				{
						 con_TranF[i+5] = Parameter_Buffer[i];
				}	
			  W5500_Send_Data(s,con_TranF);       //�ش��豸��Ϣ
	}
	else
	{
      Write_SOCK_Data_Buffer(3,Rx_Buffer,u16_size);
			if(((Rx_Buffer[0] == CON_START) && (Rx_Buffer[u16_size-1] == CON_END) && (Rx_Buffer[1] == CON_ADDRESS)) != 1)
			{
						/*	��ʼλ������λ����ַλ����һ������ȷ������Ӧ��Z	*/
						W5500_Send_Data(s,con_TranZACK);
			}
			else
			{
					for(i=1;i<(u16_size-2);i++)
					{
							u8_Lrc += Rx_Buffer[i];
					}
					u16_Length = (Rx_Buffer[2] | (Rx_Buffer[3] <<8)) + 6;                //����ռ������
          if(u16_Length != u16_size) //������Ȳ���  ���ǳ���������ָ��ռ��
          {
             if(Rx_Buffer[4] == 'O')//����
             {
                  for(i=0;i<(u16_size-7);i++)
                  {
                      Rx_Buffer[i] = Rx_Buffer[i+7];
                  }
                  u16_size = u16_size-7;
                  u8_Lrc = 0;
                  for(i=1;i<(u16_size-2);i++)
                  {
                      u8_Lrc += Rx_Buffer[i];
                  }
                  u16_Length = (Rx_Buffer[2] | (Rx_Buffer[3] <<8)) + 6;             
             }
             else
             {
                  u16_size = u16_size-7;
                  u8_Lrc = 0;
                  for(i=1;i<(u16_size-2);i++)
                  {
                      u8_Lrc += Rx_Buffer[i];
                  }
                  u16_Length = (Rx_Buffer[2] | (Rx_Buffer[3] <<8)) + 6; 
             }
          }
					if((Rx_Buffer[u16_size-2] != u8_Lrc) || (u16_Length != u16_size))
					{
							/*	��żУ�鲻��ȷ������Ӧ��Z	*/
							W5500_Send_Data(s,con_TranZACK);
					}
					else  /* ��ʼλ������λ��У��λ�͵�ַ����ȷ������������ */
					{
							u8_Command = Rx_Buffer[4];
							if(s==0)
							{
									Network0_StopFlg = 0;
//									W5500_Send_Data(s,con_TranO);	//����
							}
							else if(s==2)
							{
									Network1_StopFlg = 0;
							}
// 							if(SysParameter.Par12_MachineType == 0) //����̨
// 							{															
// 							}
// 							else  //�ƶ�̨
							{
									 if(s == 0)
									 {
										if(USER_SystemFlag.A.Flag_Sucess1 == 0)  //��������ּ���
										{
											switch(u8_Command)
															{
																	case 'O'://����ָ��
																	{
																								if(s==0)
																								{
																										Network0_StopFlg = 0;
																										W5500_Send_Data(s,con_TranO);	//����
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
																										Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
																										Tx_Buffer[0] = 12;  //֡���� ��λ
																										Tx_Buffer[1] = 0;  //֡���� ��λ
																										Tx_Buffer[2] = 'H'; 
																										Tx_Buffer[3] = 'A'; 
																										memcpy(&Tx_Buffer[4],cID_Key,10);
																										W5500_Send_Data(s,Tx_Buffer);//�ش�
																								}
																								if(Rx_Buffer[5]  == 'E')
																								{
																										if(Encryption_Cnt <= ENCRYPTION_MAX)
																										{
																												cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
																												if(cCheck_Result == 0)  //������ȷ
																												{
																														Encryption_Cnt = 0;
																														W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
																														USER_SystemFlag.A.Flag_Sucess1 = 1;
																														USER_SystemFlag.A.Flag_Standalone = 1;  //������������ɹ�
																												}
																												else
																												{
																														Encryption_Cnt ++;																											
																														W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
																												}
																										}
																										else
																										{
																												W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
																										}
																										
																								}
																								break;
																	 }
																	default :
																								W5500_Send_Data(s,con_TranHX0);//�ش�ָ�����
																								break;
															}
                         }
                         else
                         {
                             if(WorkState_Flg == Default_State_Flg)  //����״̬
                             {
                                  switch(u8_Command)
                                  {
                                      case 'H':
                                      {
                                                    if(Rx_Buffer[5]  == 'A')
                                                    {
                                                        Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
                                                        Tx_Buffer[0] = 12;  //֡���� ��λ
                                                        Tx_Buffer[1] = 0;  //֡���� ��λ
                                                        Tx_Buffer[2] = 'H'; 
                                                        Tx_Buffer[3] = 'A'; 
                                                        memcpy(&Tx_Buffer[4],cID_Key,10);
                                                        W5500_Send_Data(s,Tx_Buffer);//�ش�
                                                    }
                                                    if(Rx_Buffer[5]  == 'E')
                                                    {
                                                        if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                        {
                                                            cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
                                                            if(cCheck_Result == 0)  //������ȷ
                                                            {
                                                                Encryption_Cnt = 0;
                                                                W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
                                                                USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                USER_SystemFlag.A.Flag_Standalone = 1;  //������������ɹ�
                                                            }
                                                            else
                                                            {
                                                                Encryption_Cnt ++;																											
                                                                W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
                                                            }
                                                        }
                                                        else
                                                        {
                                                            W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
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
                                                   W5500_Send_Data(s,con_TranWE);				//�ش��쳣����
                                                   return;	
                                              }
                                              WFLASHStatus=FLASH_ProgramWord(2048 * 255, 0X77777777);
                                              if(WFLASHStatus!=FLASH_COMPLETE)                                  //???????:???E0
                                              {
                                                  FLASH_Lock();
                                                  W5500_Send_Data(s,con_TranWE);				//�ش��쳣����
                                                  return;	
                                              }
                                              if(FLASH_Read_Word(2048 * 255)==0X77777777)
                                              {
                                                  FLASH_Lock();
                                                  W5500_Send_Data(s,con_TranWG);				//�ش�����Ӧ��	
                                                  Delay_ms(10);                                                //10ms??	
                                                  NVIC_SystemReset();
                                              }
                                              else                                                                //
                                              {
                                                    FLASH_Lock();
                                                    W5500_Send_Data(s,con_TranWE);			//�ش��쳣����
                                               }
                                          }		
                                          break;
                                      }	
                                      case 'O'://����ָ��
                                      {
                                                    if(s==0)
                                                    {
                                                        Network0_StopFlg = 0;
                                                        W5500_Send_Data(s,con_TranO);	//����
                                                    }
                                                    else if(s==2)
                                                    {
                                                        Network1_StopFlg = 0;
                                                    }
                                                    break;
                                      }
                                      case 'S'://ȡ״̬
                                      {
                                                    switch(WorkState_Flg)
                                                    {
                                                          case Default_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'S';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case Adjust_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'J';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case SignalTest_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'C';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case StaticWeight_None_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case StaticWeight_Full_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '2';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case Retardant_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '3';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_None_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '4';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_Full_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '5';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_Motor_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '6';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          default :
                                                                      W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
                                                                      break;
                                                          
                                                    }
                                                    break;
                                      }
                                      case 'U'://ǿ����βָ�� �ص�����״̬
                                      {
                                                    W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                    Left_Machine = IO_CLOSE;
                                                    Right_Machine = IO_CLOSE;
                                                    Machine_Switch = IO_CLOSE;
                                                    Speed_Control = IO_CLOSE;	
                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                    USER_SystemFlag.A.Flag_ST = 0;
                                        
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
                                                    Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                    WorkState_Flg = Default_State_Flg;  //����״̬
                                                    W5500_Send_Data(s,con_TranU);				//����
                                                    return;
                                      }
                                      case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
                                      {
                                                    TIM2_count = 0;//40S�жϹرյ�ŷ���Դ
                                                    Pump_UpDown = IO_CLOSE; //������
                                                    USER_LEDFlag.A.LED_Down = 0;         //������
                                                    USER_LEDFlag.A.LED_Up = 1;           //������
                                                    Left_Machine = IO_CLOSE;
                                                    Right_Machine = IO_CLOSE;
                                                    Machine_Switch = IO_CLOSE;
                                                    Speed_Control = IO_CLOSE;			
                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                    Pump_Before = IO_CLOSE;
                                                    Pump_Behind = IO_CLOSE;
                                                    Left_Up = IO_CLOSE;
                                                    Right_Up = IO_CLOSE;
                                                    Left_Down = IO_OPEN;
                                                    Right_Down = IO_OPEN;
                                                    USER_LEDFlag.A.LED_TJUp = 0;         //̨�ܾ�
                                                    USER_LEDFlag.A.LED_TJDown = 1;       //̨�ܽ�
                                        
                                                    Clamp_All = IO_CLOSE;
                                                    Protect_Overflow = IO_CLOSE; 
                                                    Hydraumatic_Machine = IO_CLOSE;
                                                    System_State = Default_State;
                                                    Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
                                                    WorkState_Flg = Default_State_Flg;  //����״̬
                                                    W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                    return;
                                      }
                                      case 'I'://��ʼ��ָ��
                                      {																		
                                                    if(Rx_Buffer[5]  == 'B')
                                                    {
                                                        W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                        for(i=0;i<802;i++)
                                                        {
                                                            u16Left_Data[i] = 0;
                                                            u16Right_Data[i] = 0;
                                                        }
              // 																					for(i=0;i<2010;i++)
              // 																					{
              // 																							u8Tran_CurrentZ[i] = 0;
              // 																					}
                                                        for(i=0;i<3250;i++)
                                                        {
                                                            u8Tran_CurrentZ[i] = 0;
                                                            u8Tran_CurrentC[i] = 0;
                                                            u8Tran_CurrentJ[i] = 0;
                                                            u8Tran_CurrentN[i] = 0;
                                                        }
                                                        Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                        W5500_Send_Data(s,con_TranI);				//��ȷӦ��
                                                    }
                                                    else
                                                    {
                                                        W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
                                                    }
                                                    break;
                                      }
                                      case 'N'://�������ƺ�֪ͨ
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {
                                                        case '0'://ǰ�н��н�
                                                        {
                                                                    Pump_Before = IO_OPEN;	
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '1'://ǰ�н��ɿ�
                                                        {
                                                                    Pump_Before = IO_CLOSE;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '2'://��н��н�
                                                        {
                                                                    Pump_Behind = IO_OPEN;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '3'://��н��ɿ�
                                                        {
                                                                    Pump_Behind = IO_CLOSE;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '4'://���׾�
                                                        {
                                                                    Pump_UpDown = IO_CLOSE;
                                                                    USER_LEDFlag.A.LED_Up = 1;  				//������ָʾ��
                                                                    USER_LEDFlag.A.LED_Down = 0;    //������ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '5'://���׽�
                                                        {
                                                                    Pump_UpDown = IO_OPEN;
                                                                    USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
                                                                    USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '6'://̨�ܾ�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Left_Up = IO_OPEN;
                                                                    Right_Up = IO_OPEN;
                                                                    Left_Down = IO_CLOSE;
                                                                    Right_Down = IO_CLOSE;
                                                                    Protect_Overflow = IO_OPEN;			
                                                                    USER_LEDFlag.A.LED_TJUp = 1;  				//̨�ܾ�ָʾ��
                                                                    USER_LEDFlag.A.LED_TJDown = 0;    //̨�ܽ�ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '7'://̨�ܽ�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Left_Up = IO_CLOSE;
                                                                    Right_Up = IO_CLOSE;
                                                                    Left_Down = IO_OPEN;
                                                                    Right_Down = IO_OPEN;
                                                                    Protect_Overflow = IO_OPEN;
                                                                    USER_LEDFlag.A.LED_TJUp = 0;  				//̨�ܾ�ָʾ��
                                                                    USER_LEDFlag.A.LED_TJDown = 1;    //̨�ܽ�ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'I'://̨�ܼн�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Clamp_All = IO_OPEN;
                                                                    Protect_Overflow = IO_OPEN;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '8'://�����ƶ�������
                                                        {																											
                                                                    if(Rx_Buffer[2] == 26)//֡��26
                                                                    {
                                                                        Left_BrakeSum = 0;
                                                                        Right_BrakeSum = 0;
                                                                        Left_BrakeSub = 0;
                                                                        Right_BrakeSub = 0;
                                                                        if(Rx_Buffer[6] > 0x30) Left_BrakeSum += (Rx_Buffer[6]-0x30)*10000;
                                                                        if(Rx_Buffer[7] > 0x30) Left_BrakeSum += (Rx_Buffer[7]-0x30)*1000;
                                                                        if(Rx_Buffer[8] > 0x30) Left_BrakeSum += (Rx_Buffer[8]-0x30)*100;
                                                                        if(Rx_Buffer[9] > 0x30) Left_BrakeSum += (Rx_Buffer[9]-0x30)*10;
                                                                        if(Rx_Buffer[10] > 0x30) Left_BrakeSum += Rx_Buffer[10]-0x30;
                                                                      
                                                                        if(Rx_Buffer[12] > 0x30) Right_BrakeSum += (Rx_Buffer[12]-0x30)*10000;
                                                                        if(Rx_Buffer[13] > 0x30) Right_BrakeSum += (Rx_Buffer[13]-0x30)*1000;
                                                                        if(Rx_Buffer[14] > 0x30) Right_BrakeSum += (Rx_Buffer[14]-0x30)*100;
                                                                        if(Rx_Buffer[15] > 0x30) Right_BrakeSum += (Rx_Buffer[15]-0x30)*10;
                                                                        if(Rx_Buffer[16] > 0x30) Right_BrakeSum += Rx_Buffer[16]-0x30;
                                                                      
                                                                        if(Rx_Buffer[18] > 0x30) Left_BrakeSub += (Rx_Buffer[18]-0x30)*10000;
                                                                        if(Rx_Buffer[19] > 0x30) Left_BrakeSub += (Rx_Buffer[19]-0x30)*1000;
                                                                        if(Rx_Buffer[20] > 0x30) Left_BrakeSub += (Rx_Buffer[20]-0x30)*100;
                                                                        if(Rx_Buffer[21] > 0x30) Left_BrakeSub += (Rx_Buffer[21]-0x30)*10;
                                                                        if(Rx_Buffer[22] > 0x30) Left_BrakeSub += Rx_Buffer[22]-0x30;
                                                                        
                                                                        if(Rx_Buffer[24] > 0x30) Right_BrakeSub += (Rx_Buffer[24]-0x30)*10000;
                                                                        if(Rx_Buffer[25] > 0x30) Right_BrakeSub += (Rx_Buffer[25]-0x30)*1000;
                                                                        if(Rx_Buffer[26] > 0x30) Right_BrakeSub += (Rx_Buffer[26]-0x30)*100;
                                                                        if(Rx_Buffer[27] > 0x30) Right_BrakeSub += (Rx_Buffer[27]-0x30)*10;
                                                                        if(Rx_Buffer[28] > 0x30) Right_BrakeSub += Rx_Buffer[28]-0x30;
                                                                        W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                        if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
                                                                        {
                                                                              if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                                                                              {
                                                                                    USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                                                                                    USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
                                                                                    FunW5500_SendLED(0,Left_BrakeSum,Right_BrakeSum);//�ϴ������ƶ���
                                                                              }
                                                                        }
                                                                     }
                                                                     else
                                                                     {
                                                                         W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
                                                                     }
                                                                    break;
                                                        }
                                                        case '9'://���������� 
                                                        {
                                                                    if(Rx_Buffer[2] == 14)//֡��14
                                                                    {
                                                                        Left_BrakeAve = 0;
                                                                        Right_BrakeAve = 0;
                                                                      
                                                                        if(Rx_Buffer[6] > 0x30) Left_BrakeAve += (Rx_Buffer[6]-0x30)*10000;
                                                                        if(Rx_Buffer[7] > 0x30) Left_BrakeAve += (Rx_Buffer[7]-0x30)*1000;
                                                                        if(Rx_Buffer[8] > 0x30) Left_BrakeAve += (Rx_Buffer[8]-0x30)*100;
                                                                        if(Rx_Buffer[9] > 0x30) Left_BrakeAve += (Rx_Buffer[9]-0x30)*10;
                                                                        if(Rx_Buffer[10] > 0x30) Left_BrakeAve += Rx_Buffer[10]-0x30;
                                                                      
                                                                        if(Rx_Buffer[12] > 0x30) Right_BrakeAve += (Rx_Buffer[12]-0x30)*10000;
                                                                        if(Rx_Buffer[13] > 0x30) Right_BrakeAve += (Rx_Buffer[13]-0x30)*1000;
                                                                        if(Rx_Buffer[14] > 0x30) Right_BrakeAve += (Rx_Buffer[14]-0x30)*100;
                                                                        if(Rx_Buffer[15] > 0x30) Right_BrakeAve += (Rx_Buffer[15]-0x30)*10;
                                                                        if(Rx_Buffer[16] > 0x30) Right_BrakeAve += Rx_Buffer[16]-0x30;																												
                                                                        W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                        if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
                                                                        {
                                                                              if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                                                                              {
                                                                                    USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                                                                                    USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
                                                                                    FunW5500_SendLED(0,Left_BrakeAve,Right_BrakeAve);//�ϴ�������
                                                                              }
                                                                        }
                                                                     }
                                                                     else
                                                                     {
                                                                         W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
                                                                     }
                                                                    break;
                                                        }
                                                        case 'A':
                                                        {
                                                                      if(Rx_Buffer[2] == 26)//֡��26
                                                                    {
                                                                        Left_BrakeSumJJ = 0;
                                                                        Right_BrakeSumJJ = 0;
                                                                        Left_BrakeSubJJ = 0;
                                                                        Right_BrakeSubJJ = 0;
                                                                        if(Rx_Buffer[6] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[6]-0x30)*10000;
                                                                        if(Rx_Buffer[7] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[7]-0x30)*1000;
                                                                        if(Rx_Buffer[8] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[8]-0x30)*100;
                                                                        if(Rx_Buffer[9] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[9]-0x30)*10;
                                                                        if(Rx_Buffer[10] > 0x30) Left_BrakeSumJJ += Rx_Buffer[10]-0x30;
                                                                      
                                                                        if(Rx_Buffer[12] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[12]-0x30)*10000;
                                                                        if(Rx_Buffer[13] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[13]-0x30)*1000;
                                                                        if(Rx_Buffer[14] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[14]-0x30)*100;
                                                                        if(Rx_Buffer[15] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[15]-0x30)*10;
                                                                        if(Rx_Buffer[16] > 0x30) Right_BrakeSumJJ += Rx_Buffer[16]-0x30;
                                                                      
                                                                        if(Rx_Buffer[18] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[18]-0x30)*10000;
                                                                        if(Rx_Buffer[19] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[19]-0x30)*1000;
                                                                        if(Rx_Buffer[20] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[20]-0x30)*100;
                                                                        if(Rx_Buffer[21] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[21]-0x30)*10;
                                                                        if(Rx_Buffer[22] > 0x30) Left_BrakeSubJJ += Rx_Buffer[22]-0x30;
                                                                        
                                                                        if(Rx_Buffer[24] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[24]-0x30)*10000;
                                                                        if(Rx_Buffer[25] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[25]-0x30)*1000;
                                                                        if(Rx_Buffer[26] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[26]-0x30)*100;
                                                                        if(Rx_Buffer[27] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[27]-0x30)*10;
                                                                        if(Rx_Buffer[28] > 0x30) Right_BrakeSubJJ += Rx_Buffer[28]-0x30;
                                                                        W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                        if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
                                                                        {
                                                                              if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                                                                              {
                                                                                    USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                                                                                    USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
                                                                                    FunW5500_SendLED(0,Left_BrakeSumJJ,Right_BrakeSumJJ);//�ϴ������ƶ���
                                                                              }
                                                                        }
                                                                     }
                                                                     else
                                                                     {
                                                                         W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
                                                                     }
                                                                    break;
                                                        }
                                                        case 'B'://��������
                                                        {
                                                                    Speed_Control = IO_OPEN;
                                                                    Machine_Switch = IO_CLOSE;
                                                                    Left_Machine = IO_OPEN;
                                                                    USER_SystemFlag.A.Flag_Ldynamo = 1;
                                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 1;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'C'://����ֹͣ
                                                        {
                                                                    Speed_Control = IO_CLOSE;
                                                                    Left_Machine = IO_CLOSE;
                                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'D'://�ҵ������
                                                        {
                                                                    Speed_Control = IO_OPEN;
                                                                    Machine_Switch = IO_CLOSE;
                                                                    Right_Machine = IO_OPEN;
                                                                    USER_SystemFlag.A.Flag_Rdynamo = 1;
                                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 1;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'E'://�ҵ��ֹͣ
                                                        {
                                                                    Speed_Control = IO_CLOSE;
                                                                    Right_Machine = IO_CLOSE;
                                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'F'://�������л�
                                                        {
                                                                    Machine_Switch = IO_OPEN;
                                                                    USER_SystemFlag.A.Flag_Switch = 1;
                        // 																						SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'G'://ȫ��ֹͣ
                                                        {
                                                                    Speed_Control = IO_CLOSE;
                                                                    Left_Machine = IO_CLOSE;
                                                                    Right_Machine = IO_CLOSE;
                                                                    Machine_Switch = IO_CLOSE;
                                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }				
                                                        case 'J'://N J  ��������
                                                        {
                                                                    if(Rx_Buffer[2] == 14)//֡��14
                                                                    {
                                                                        Left_StaticWeight = 0;
                                                                        Right_StaticWeight = 0;
                                                                        if(Rx_Buffer[6] > 0x30) Left_StaticWeight += (Rx_Buffer[6]-0x30)*10000;
                                                                        if(Rx_Buffer[7] > 0x30) Left_StaticWeight += (Rx_Buffer[7]-0x30)*1000;
                                                                        if(Rx_Buffer[8] > 0x30) Left_StaticWeight += (Rx_Buffer[8]-0x30)*100;
                                                                        if(Rx_Buffer[9] > 0x30) Left_StaticWeight += (Rx_Buffer[9]-0x30)*10;
                                                                        if(Rx_Buffer[10] > 0x30) Left_StaticWeight += Rx_Buffer[10]-0x30;
                                                                        if(Rx_Buffer[12] > 0x30) Right_StaticWeight += (Rx_Buffer[12]-0x30)*10000;
                                                                        if(Rx_Buffer[13] > 0x30) Right_StaticWeight += (Rx_Buffer[13]-0x30)*1000;
                                                                        if(Rx_Buffer[14] > 0x30) Right_StaticWeight += (Rx_Buffer[14]-0x30)*100;
                                                                        if(Rx_Buffer[15] > 0x30) Right_StaticWeight += (Rx_Buffer[15]-0x30)*10;
                                                                        if(Rx_Buffer[16] > 0x30) Right_StaticWeight += Rx_Buffer[16]-0x30;
                                                                        if(Left_StaticWeight<200)
                                                                        {
                                                                            Left_StaticWeight = 200;																															
                                                                            W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
                                                                        }
                                                                        if(Right_StaticWeight<200)
                                                                        {
                                                                            Right_StaticWeight = 200;
                                                                            W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
                                                                        }
              // 																													System_State = DynamoStart_State;
                                                                        W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                                        Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                    }
                                                                    else
                                                                    {
                                                                        W5500_Send_Data(s,con_TranXACK);				//������Ч
                                                                    }
                                                                    break;
                                                        }			
                                                        default:
                                                                   W5500_Send_Data(s,con_TranXACK);				//������Ч
                                                                   break;
                                                    }
                                                    break;
                                      }
                                      case 'B'://�汾��Ϣ
                                      {
                                                    W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
                                                    break;
                                      }
                                      case 'C'://�źŲ��Խ���
                                      {
                                                    if(Rx_Buffer[5] == 0)
                                                    {
                                                          if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
                                                          {
                                                              Signal_TransferTime = Rx_Buffer[6] - 0x30;
                                                              System_State = SignalTest_State;
                                                              WorkState_Flg = SignalTest_State_Flg;    //�źŲ���״̬
                                                              W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                          }
                                                    }
                                                    else if(Rx_Buffer[5] == 'E')
                                                    {
                                                        System_State = Default_State;
                                                        WorkState_Flg = Default_State_Flg;    //����״̬
                                                        W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                    }
                                                    else if(Rx_Buffer[5] == 'X')
                                                    {
                                                        USER_SystemFlag.A.Flag_TestC1 = 1;
                                                    }
                                                    else
                                                    {
                                                        switch(Rx_Buffer[5])
                                                        {																																
                                                            case Left_Brake_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Right_Brake_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case All_Weight_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case FootPlat_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Left_Move_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Right_Move_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            default:
                                                                                                printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
              // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
                                                                                                break;	
                                                        }
                                                    }
                                                    break;
                                      }
                                      case 'D'://ȡ���Խ��
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {																																																																																																					
                                                        case 'F'://�����ƶ�̨��ɽ��
                                                        {
                                                                      if(Rx_Buffer[6] == '1')
                                                                      {
                                                                          FunW5500_SendDF(s,1,AllNone_StaticWeight);
                                                                      }
                                                                      else if(Rx_Buffer[6] == '2')
                                                                      {																													
                                                                          FunW5500_SendDF(s,2,AllFull_StaticWeight);
                                                                      }
                                                                      else
                                                                      {
                                                                          W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
                                                                      }
                                                                      break;
                                                        }
                                                        case 'T'://����̤���������
                                                        {
                                                                      break;
                                                        }
                                                        case 'D'://���ͼ���� DD
                                                        {
                                                                      FunW5500_SendDC(s);				//���͵�ǰ�����
                                                                      break;
                                                        }	
                                                        case 'C'://����̤���������
                                                        {
                                                                      W5500_Send_Data(s,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
    // 																																	FunW5500_SendDC(s);				//���͵�ǰ�����
                                                                      break;
                                                        }																					
                                                        case 'B'://�����ƶ���������
                                                        {
                                                                      
    // 																																	W5500_Send_Data(s,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
                                                                      break;
                                                        }
                                                        case 'J'://���ͼ����ƶ���������
                                                        {
                                                                      
                                                                      W5500_Send_Data(s,u8Tran_CurrentJ);				//���͵�ǰ�����ƶ���������
                                                                      break;
                                                        }
                                                        case 'Z'://�������͹�������
                                                        {
                                                                      
                                                                      W5500_Send_Data(s,u8Tran_CurrentZ);				//���͵�ǰ���͹�������
                                                                      break;
                                                        }
                                                        default:
                                                                      W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
                                                                      break;
                                                    }
                                                    break;
                                      }
                                      case 'G'://ȡʵʱ����
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {																					
                                                        case 'B':
                                                        {
                                                                      Tx_Buffer[0] = 14;  //֡���� ��λ
                                                                      Tx_Buffer[1] = 0;    //֡���� ��λ
                                                                      Tx_Buffer[2] = 'G'; 
                                                                      Tx_Buffer[3] = 'B'; 
                                                                      /* ���� */
                                                                      Tx_Buffer[4] = Left_DynamicBrake/10000; 
                                                                      if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
                                                                      else Tx_Buffer[4] = 0x20;
                                                                      Tx_Buffer[5] = Left_DynamicBrake%10000/1000; 
                                                                      if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
                                                                      else Tx_Buffer[5] = 0x20;
                                                                      Tx_Buffer[6] = Left_DynamicBrake%1000/100; 
                                                                      if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
                                                                      else Tx_Buffer[6] = 0x20;
                                                                      Tx_Buffer[7] = Left_DynamicBrake%100/10; 
                                                                      if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
                                                                      else Tx_Buffer[7] = 0x20;
                                                                      Tx_Buffer[8] = Left_DynamicBrake%10 + 0x30; 
                                                                      Tx_Buffer[9] = 0x20; 
                                                                      /* ���� */
                                                                      Tx_Buffer[10] = Right_DynamicBrake/10000; 
                                                                      if(Tx_Buffer[10] > 0) Tx_Buffer[10] += 0x30;
                                                                      else Tx_Buffer[10] = 0x20;
                                                                      Tx_Buffer[11] = Right_DynamicBrake%10000/1000; 
                                                                      if(Tx_Buffer[11] > 0) Tx_Buffer[11] += 0x30;
                                                                      else Tx_Buffer[11] = 0x20;
                                                                      Tx_Buffer[12] = Right_DynamicBrake%1000/100; 
                                                                      if(Tx_Buffer[12] > 0) Tx_Buffer[12] += 0x30;
                                                                      else Tx_Buffer[12] = 0x20;
                                                                      Tx_Buffer[13] = Right_DynamicBrake%100/10; 
                                                                      if(Tx_Buffer[13] > 0) Tx_Buffer[13] += 0x30;
                                                                      else Tx_Buffer[13] = 0x20;
                                                                      Tx_Buffer[14] = Right_DynamicBrake%10 + 0x30; 
                                                                      Tx_Buffer[15] = 0x20; 
                                                                      W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ�ƶ�
                                                                      break;
                                                        }																																								
                                                        case 'T':
                                                        {
                                                                      Tx_Buffer[0] = 8;    //֡���� ��λ
                                                                      Tx_Buffer[1] = 0;    //֡���� ��λ
                                                                      Tx_Buffer[2] = 'G'; 
                                                                      Tx_Buffer[3] = 'T'; 
                                                                      /* ̤���� */
                                                                      Tx_Buffer[4] = DynamicFootPlat/10000; 
                                                                      if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
                                                                      else Tx_Buffer[4] = 0x20;
                                                                      Tx_Buffer[5] = DynamicFootPlat%10000/1000; 
                                                                      if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
                                                                      else Tx_Buffer[5] = 0x20;
                                                                      Tx_Buffer[6] = DynamicFootPlat%1000/100; 
                                                                      if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
                                                                      else Tx_Buffer[6] = 0x20;
                                                                      Tx_Buffer[7] = DynamicFootPlat%100/10; 
                                                                      if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
                                                                      else Tx_Buffer[7] = 0x20;
                                                                      Tx_Buffer[8] = DynamicFootPlat%10 + 0x30; 
                                                                      Tx_Buffer[9] = 0x20; 
                                                                      W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ̤����
                                                                      break;
                                                        }						
                                                        case 'F':
                                                        {
                                                                      Tx_Buffer[0] = 8;    //֡���� ��λ
                                                                      Tx_Buffer[1] = 0;    //֡���� ��λ
                                                                      Tx_Buffer[2] = 'G'; 
                                                                      Tx_Buffer[3] = 'F'; 
                                                                      if(SysParameter.Par6_Weight == 2)
                                                                       {
                                                                           Quotient = All_DynamicWeight / 2;
                                                                           Remainder = All_DynamicWeight % 2;
                                                                           if(Remainder > 0)
                                                                           {
                                                                              All_DynamicWeight = (Quotient +1) * 2;																										 
                                                                           }																									
                                                                       }
                                                                       else if(SysParameter.Par6_Weight == 5)
                                                                       {
                                                                          Quotient = All_DynamicWeight / 5;
                                                                           Remainder = All_DynamicWeight % 5;
                                                                           if(Remainder < 3)
                                                                           {
                                                                              All_DynamicWeight = Quotient * 5;																										 
                                                                           }
                                                                           else
                                                                           {
                                                                              All_DynamicWeight = (Quotient +1) * 5;
                                                                           }																									
                                                                       }
                                                                      /* ���ϳ��� */
                                                                      Tx_Buffer[4] = All_DynamicWeight/10000; 
                                                                      if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
                                                                      else Tx_Buffer[4] = 0x20;
                                                                      Tx_Buffer[5] = All_DynamicWeight%10000/1000; 
                                                                      if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
                                                                      else Tx_Buffer[5] = 0x20;
                                                                      Tx_Buffer[6] = All_DynamicWeight%1000/100; 
                                                                      if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
                                                                      else Tx_Buffer[6] = 0x20;
                                                                      Tx_Buffer[7] = All_DynamicWeight%100/10; 
                                                                      if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
                                                                      else Tx_Buffer[7] = 0x20;
                                                                      Tx_Buffer[8] = All_DynamicWeight%10 + 0x30; 
                                                                      Tx_Buffer[9] = 0x20; 
                                                                      W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ��������
                                                                      break;
                                                        }	
                                                        default :
                                                                     W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
                                                                     break;
                                                    }
                                                    break;
                                      }
                                      case 'A': //�궨״̬
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {
                                                        case 'C'://����궨״̬
                                                                      {				
                                                                          System_State = Adjust_State;
                                                                          WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                          switch(Rx_Buffer[6])
                                                                          {																																
                                                                              case Left_Brake_ID:
                                                                                                                  AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Right_Brake_ID:
                                                                                                                  AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case All_Weight_ID:
                                                                                                                  AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case FootPlat_ID:
                                                                                                                  AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Left_Move_ID:
                                                                                                                  AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Right_Move_ID:
                                                                                                                  AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              default:
                                                                                                                  printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                                  W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                                                                                  AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
                                                                                                                  break;	
                                                                          }
                                                                          break;
                                                                      }
                                                        case 'E'://�˳��궨״̬
                                                                      {
                                                                          System_State = Default_State;
                                                                          WorkState_Flg = Default_State_Flg;    //����״̬
                                                                          W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                          break;
                                                                      }
                                                        case 'W'://����궨ϵ��
                                                                      {
                                                                          for(i=0;i<18;i++)
                                                                          {
                                                                              u8flash_buffer[i] = Rx_Buffer[i+7];																			
                                                                          }
                                                                          switch(Rx_Buffer[6])
                                                                          {																																
                                                                              case Left_Brake_ID:
                                                                                                                  CabItemIndex = 2;
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
                                                                                                                CabBuf[2] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(Left_Brake_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(Left_Brake_par.Load_Flash,Left_Brake_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(Left_Brake_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,Left_Brake_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��																																
                                                                                                                  break;
                                                                              case Right_Brake_ID:
                                                                                                                  CabItemIndex = 3;
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
                                                                                                                CabBuf[3] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(Right_Brake_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(Right_Brake_par.Load_Flash,Right_Brake_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(Right_Brake_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,Right_Brake_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��																																																																																	
              // 																																																		printf("NB=%u,C0=%f,c1=%f,C2=%f,c3=%f",Right_Brake_par.Load_Parameter.Parameter_NB,
              // 																																																																					Right_Brake_par.Load_Parameter.Parameter_C0.float_data,
              // 																																																																					Right_Brake_par.Load_Parameter.Parameter_C1.float_data,
              // 																																																																					Right_Brake_par.Load_Parameter.Parameter_C2.float_data,
              // 																																																																					Right_Brake_par.Load_Parameter.Parameter_C3.float_data);
                                                                                                                  break;
                                                                              case All_Weight_ID:
                                                                                                                  CabItemIndex = 4;
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
                                                                                                                CabBuf[4] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                All_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                All_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                All_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                All_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                All_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(All_Weight_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(All_Weight_par.Load_Flash,All_Weight_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(All_Weight_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,All_Weight_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case FootPlat_ID:
                                                                                                                  CabItemIndex = 5;
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
                                                                                                                CabBuf[5] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                FootPlat_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                FootPlat_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                FootPlat_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                FootPlat_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                FootPlat_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(FootPlat_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(FootPlat_par.Load_Flash,FootPlat_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(FootPlat_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,FootPlat_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Left_Move_ID:
                                                                                                                  CabItemIndex = 6;
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
                                                                                                                CabBuf[6] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                Left_Move_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                Left_Move_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                Left_Move_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                Left_Move_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                Left_Move_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(Left_Move_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(Left_Move_par.Load_Flash,Left_Move_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(Left_Move_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,Left_Move_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Right_Move_ID:
                                                                                                                    CabItemIndex = 7;
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
                                                                                                                CabBuf[7] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                Right_Move_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                Right_Move_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                Right_Move_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                Right_Move_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                Right_Move_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(Right_Move_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(Right_Move_par.Load_Flash,Right_Move_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(Right_Move_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,Right_Move_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                break;
                                                                              default:
                                                                                                                  printf("\r\nError:The command channel ID is false!!!xie\r\n");
                                                                                                                  W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                                                                                  break;
                                                                          }
                                                                          break;
                                                                      }
                                                        case 'R'://���궨ϵ��
                                                                      {
                                                                          u8read_flg = 1;//���궨ϵ���ɹ�
                                                                          switch(Rx_Buffer[6])
                                                                          {																																
                                                                              case Left_Brake_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,Left_Brake_Flash_Address1,18);
                                                                                                                  break;
                                                                              case Right_Brake_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,Right_Brake_Flash_Address1,18);
                                                                                                                  break;
                                                                              case All_Weight_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,All_Weight_Flash_Address1,18);
                                                                                                                  break;
                                                                              case FootPlat_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,FootPlat_Flash_Address1,18);
                                                                                                                  break;
                                                                              case Left_Move_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,Left_Move_Flash_Address1,18);
                                                                                                                  break;
                                                                              case Right_Move_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,Right_Move_Flash_Address1,18);
                                                                                                                  break;
                                                                              default:
                                                                                                                  u8read_flg = 0;//���궨ϵ��ʧ��
                                                                                                                  printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
                                                                                                                  break;
                                                                          }			
                                                                          if(u8read_flg == 1)//���궨ϵ���ɹ�
                                                                          {
                                                                              Tx_Buffer[0] = 21;//֡���� ��λ
                                                                              Tx_Buffer[1] = 0;  //֡���� ��λ
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
                                                                              W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                                          }
                                                                          break;
                                                                      }
                                                         default :
                                                                     W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                                     break;
                                                    }
                                                    break;
                                      }
                                      case 'Y': //����  
                                      {
                                                    if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                    {
                                                        Fun_Zero(Rx_Buffer[5],1);
                                                        W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                    }
                                                    else if(Rx_Buffer[5] == AD_Clear_ID) //ǿ������
                                                    {
                                                        Fun_Zero(Left_Brake_ID,1);
                                                        Fun_Zero(Right_Brake_ID,1);
                                                        Fun_Zero(All_Weight_ID,1);
                                                        W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                    }
                                                    else
                                                    {
                                                        W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                    }
                                                    break;
                                      }
                                      case 'E'://������д
                                      {
                                                    if(Rx_Buffer[5] == '1')
                                                    {
                                                          if(Rx_Buffer[6] == 'W')
                                                          {																							
                                                              for(i=0;i<(Rx_Buffer[7]*3+1);i++) //���������ݴ洢����ʱ����u8flash_buffer
                                                              {
                                                                   Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                              }								
                                                              if(Fun_DealParameter()==1)
                                                              {
                                                                  SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//��������
                                                                  SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(Rx_Buffer[7]*3+1));//����ϵ��		
                                                                  W5500_Send_Data(s,con_TranE1W);//��ȷӦ��
                                                              }
                                                              else
                                                              {
                                                                  W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��
                                                                  printf("\r\nError:Write Parameter false!!!E1W\r\n");
                                                              }
                                                          }
                                                          else if(Rx_Buffer[6] == 'R')
                                                          {												
                                                                SPI_FLASH_BufferRead(Parameter_Buffer,ParameterAll_Flash_Address,100);
                                                                Tx_Buffer[0] = Parameter_Buffer[0]*3+4;  //֡���� ��λ
                                                                Tx_Buffer[1] = 0;  //֡���� ��λ
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
                                                              W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��	
                                                              printf("\r\nError:The command E1 is false!!!E1\r\n");
                                                          }
                                                    }
                                                    else if(Rx_Buffer[5] == '2')
                                                    {
                                                        if(Rx_Buffer[6] == 'W')
                                                        {
                                                            for(i=0;i<24;i++) //���������ݴ洢����ʱ����Parameter_Buffer
                                                            {
                                                                 Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                            }	
                                                            SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//��������
                                                            SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,24);//����ϵ��		
                                                            W5500_Send_Data(s,con_TranE2W);//��ȷӦ��
                                                        }
                                                        else if(Rx_Buffer[6] == 'R')
                                                        {
                                                            SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
                                                            Tx_Buffer[0] = 27;  //֡���� ��λ
                                                            Tx_Buffer[1] = 0;    //֡���� ��λ
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
                                                            W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��	
                                                            printf("\r\nError:The command E2 is false!!!E2\r\n");
                                                        }
                                                    }
                                                    else if(Rx_Buffer[5] == 'I')
                                                    {
                                                          if(Rx_Buffer[6] == 'W')
                                                          {
                                                              for(i=0;i<26;i++) //���������ݴ洢����ʱ����Parameter_Buffer
                                                              {
                                                                   W5500_Parameter[i] = Rx_Buffer[i+7];
                                                              }	
                                                              W5500_Parameter[26] = 0x55;
                                                              SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//��������
                                                              SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//����ϵ��		
                                                              W5500_Send_Data(s,con_TranEIW);//��ȷӦ��
                                                          }
                                                          else if(Rx_Buffer[6] == 'R')
                                                          {
                                                              SPI_FLASH_BufferRead(Parameter_Buffer,ParameterIP_Flash_Address,26);
                                                              Tx_Buffer[0] = 29;  //֡���� ��λ
                                                              Tx_Buffer[1] = 0;    //֡���� ��λ
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
                                                              W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��	
                                                              printf("\r\nError:The command EII is false!!!EI\r\n");
                                                          }
                                                    }
                                                    else
                                                    {
                                                         W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��	
                                                         printf("\r\nError:The command E is false!!!E\r\n");
                                                    }
                                                    break;
                                      }
                                      case 'T':
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {																
                                                        case 'B': //�����ƶ�����
                                                        {
                                                                      System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
                                                                      W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                      Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                      WorkState_Flg = BrakeTest_None_State_Flg;
                                                                      break;
                                                        }
                                                        case 'J': //�����ƶ�����
                                                        {
                                                                      System_State = BrakeTest_Full_State; //ϵͳ��������ƶ�����
                                                                       W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                       Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                       WorkState_Flg = BrakeTest_Full_State_Flg;
                                                                      break;
                                                        }																											
                                                        case 'Z':  //���������
                                                        {
                                                                       if((Rx_Buffer[6] == 0x31) || (Rx_Buffer[6] == 0x32))
                                                                       {
                                                                           System_State = Retardant_State; 					//ϵͳ�������Ͳ���
                                                                           SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
                                                                           W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
                                                                           Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                          WorkState_Flg = Retardant_State_Flg;
                                                                       }
                                                                       else
                                                                       {	
                                                                            W5500_Send_Data(s,con_TranXACK);	//��Чָ֡��
                                                                       }
                                                                       break;
                                                        }
                                                        case 'D':  //�������
                                                        {
                                                                      if(Rx_Buffer[2] == 14)//֡��14
                                                                      {
                                                                          Left_StaticWeight = 0;
                                                                          Right_StaticWeight = 0;
                        // 																									if(Rx_Buffer[6] > 0x30) Left_StaticWeight += (Rx_Buffer[6]-0x30)*10000;
                                                                          if(Rx_Buffer[7] > 0x30) Left_StaticWeight += (Rx_Buffer[7]-0x30)*1000;
                                                                          if(Rx_Buffer[8] > 0x30) Left_StaticWeight += (Rx_Buffer[8]-0x30)*100;
                                                                          if(Rx_Buffer[9] > 0x30) Left_StaticWeight += (Rx_Buffer[9]-0x30)*10;
                                                                          if(Rx_Buffer[10] > 0x30) Left_StaticWeight += Rx_Buffer[10]-0x30;
                        // 																									if(Rx_Buffer[12] > 0x30) Right_StaticWeight += (Rx_Buffer[12]-0x30)*10000;
                                                                          if(Rx_Buffer[13] > 0x30) Right_StaticWeight += (Rx_Buffer[13]-0x30)*1000;
                                                                          if(Rx_Buffer[14] > 0x30) Right_StaticWeight += (Rx_Buffer[14]-0x30)*100;
                                                                          if(Rx_Buffer[15] > 0x30) Right_StaticWeight += (Rx_Buffer[15]-0x30)*10;
                                                                          if(Rx_Buffer[16] > 0x30) Right_StaticWeight += Rx_Buffer[16]-0x30;
                                                                          if(Left_StaticWeight<200)
                                                                          {
                                                                              Left_StaticWeight = 200;
                                                                              W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
                                                                          }
                                                                          if(Right_StaticWeight<200)
                                                                          {
                                                                              Right_StaticWeight = 200;
                                                                              W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
                                                                          }
                                                                          System_State = DynamoStart_State;
                                                                          W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                                          Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                      }
                                                                      else
                                                                      {
                                                                          W5500_Send_Data(s,con_TranXACK);				//������Ч
                                                                      }
                                                                      break;
                                                        }
                                                        case 'F':		//��ɲ���
                                                        {	
                                                                       if(Rx_Buffer[6] == '1') //���ظ��ϳ���
                                                                       {
                                                                          System_State = StaticWeight_None_State;   //ϵͳ������ظ��ϳ���
                                                                          W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                          WorkState_Flg = StaticWeight_None_State_Flg;
                                                                       }
                                                                       else if(Rx_Buffer[6] == '2')//���ؾ���
                                                                       {
                                                                          System_State = StaticWeight_Full_State;  //ϵͳ������ؾ���״̬
                                                                          W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                          WorkState_Flg = StaticWeight_Full_State_Flg;
                                                                       }
                                                                       else
                                                                       {
                                                                          W5500_Send_Data(s,con_TranXACK);//ָ����Ч
                                                                       }																
                                                                       Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                       break;
                                                        }
                                                    }
                                                    break;
                                      }
                                      default :
                                                    W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��																								 
                                                    printf("\r\nError:The command is false!!!\r\n");
                                                    break;
                                  }
                             }	
                             else if(WorkState_Flg == Adjust_State_Flg)  //�궨״̬
                             {
                                  switch(u8_Command)
                                  {
                                      case 'H':
                                      {
                                                    if(Rx_Buffer[5]  == 'A')
                                                    {
                                                        Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
                                                        Tx_Buffer[0] = 12;  //֡���� ��λ
                                                        Tx_Buffer[1] = 0;  //֡���� ��λ
                                                        Tx_Buffer[2] = 'H'; 
                                                        Tx_Buffer[3] = 'A'; 
                                                        memcpy(&Tx_Buffer[4],cID_Key,10);
                                                        W5500_Send_Data(s,Tx_Buffer);//�ش�
                                                    }
                                                    if(Rx_Buffer[5]  == 'E')
                                                    {
                                                        if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                        {
                                                            cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
                                                            if(cCheck_Result == 0)  //������ȷ
                                                            {
                                                                Encryption_Cnt = 0;
                                                                W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
                                                                USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                USER_SystemFlag.A.Flag_Standalone = 1;  //������������ɹ�
                                                            }
                                                            else
                                                            {
                                                                Encryption_Cnt ++;																											
                                                                W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
                                                            }
                                                        }
                                                        else
                                                        {
                                                            W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
                                                        }
                                                        
                                                    }
                                                    break;
                                      }
                                      case 'O'://����ָ��
                                      {
                                                    if(s==0)
                                                    {
                                                        Network0_StopFlg = 0;
                                                        W5500_Send_Data(s,con_TranO);	//����
                                                    }
                                                    else if(s==2)
                                                    {
                                                        Network1_StopFlg = 0;
                                                    }
                                                    break;
                                      }
                                      case 'S'://ȡ״̬
                                      {
                                                    switch(WorkState_Flg)
                                                    {
                                                          case Default_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'S';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case Adjust_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'J';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case SignalTest_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'C';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case StaticWeight_None_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case StaticWeight_Full_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '2';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case Retardant_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '3';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_None_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '4';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_Full_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '5';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_Motor_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '6';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          default :
                                                                      W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
                                                                      break;
                                                          
                                                    }
                                                    break;
                                      }												
                                      case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
                                      {
                                                    TIM2_count = 0;//40S�жϹرյ�ŷ���Դ
                                                    Pump_UpDown = IO_CLOSE; //������
                                                    USER_LEDFlag.A.LED_Down = 0;         //������
                                                    USER_LEDFlag.A.LED_Up = 1;           //������
                                                    Left_Machine = IO_CLOSE;
                                                    Right_Machine = IO_CLOSE;
                                                    Machine_Switch = IO_CLOSE;
                                                    Speed_Control = IO_CLOSE;			
                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                    Pump_Before = IO_CLOSE;
                                                    Pump_Behind = IO_CLOSE;
                                                    Left_Up = IO_CLOSE;
                                                    Right_Up = IO_CLOSE;
                                                    Left_Down = IO_OPEN;
                                                    Right_Down = IO_OPEN;
                                                    USER_LEDFlag.A.LED_TJUp = 0;         //̨�ܾ�
                                                    USER_LEDFlag.A.LED_TJDown = 1;       //̨�ܽ�
                                        
                                                    Clamp_All = IO_CLOSE;
                                                    Protect_Overflow = IO_CLOSE; 
                                                    Hydraumatic_Machine = IO_CLOSE;
                                                    System_State = Default_State;
                                                    Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
                                                    WorkState_Flg = Default_State_Flg;  //����״̬
                                                    W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                    return;
                                      }										
                                      case 'B'://�汾��Ϣ
                                      {
                                                    W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
                                                    break;
                                      }					
                                      case 'C'://�źŲ��Խ���
                                      {
                                                    if(Rx_Buffer[5] == 0)
                                                    {
                                                          if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
                                                          {
                                                              Signal_TransferTime = Rx_Buffer[6] - 0x30;
                                                              System_State = SignalTest_State;
                                                              WorkState_Flg = SignalTest_State_Flg;    //�źŲ���״̬
                                                              W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                          }
                                                    }
                                                    else if(Rx_Buffer[5] == 'E')
                                                    {
                                                        System_State = Default_State;
                                                        WorkState_Flg = Default_State_Flg;    //����״̬
                                                        W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                    }
                                                    else
                                                    {
                                                        switch(Rx_Buffer[5])
                                                        {																																
                                                            case Left_Brake_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Right_Brake_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case All_Weight_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case FootPlat_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Left_Move_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Right_Move_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            default:
                                                                                                printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
              // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
                                                                                                break;	
                                                        }
                                                    }
                                                    break;
                                      }
                                      case 'A': //�궨״̬
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {
                                                        case 'C'://����궨״̬
                                                                      {				
                                                                          System_State = Adjust_State;
                                                                          WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                          switch(Rx_Buffer[6])
                                                                          {																																
                                                                              case Left_Brake_ID:
                                                                                                                  AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Right_Brake_ID:
                                                                                                                  AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case All_Weight_ID:
                                                                                                                  AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case FootPlat_ID:
                                                                                                                  AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Left_Move_ID:
                                                                                                                  AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Right_Move_ID:
                                                                                                                  AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              default:
                                                                                                                  printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                                  W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                                                                                  AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
                                                                                                                  break;	
                                                                          }
                                                                          break;
                                                                      }
                                                        case 'E'://�˳��궨״̬
                                                                      {
                                                                          System_State = Default_State;
                                                                          WorkState_Flg = Default_State_Flg;    //����״̬
                                                                          W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                          break;
                                                                      }
                                                        case 'W'://����궨ϵ��
                                                                      {
                                                                          for(i=0;i<18;i++)
                                                                          {
                                                                              u8flash_buffer[i] = Rx_Buffer[i+7];																			
                                                                          }
                                                                          switch(Rx_Buffer[6])
                                                                          {																																
                                                                              case Left_Brake_ID:
                                                                                                                  CabItemIndex = 2;
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
                                                                                                                CabBuf[2] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                Left_Brake_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(Left_Brake_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(Left_Brake_par.Load_Flash,Left_Brake_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(Left_Brake_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,Left_Brake_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��																																
                                                                                                                  break;
                                                                              case Right_Brake_ID:
                                                                                                                  CabItemIndex = 3;
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
                                                                                                                CabBuf[3] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                Right_Brake_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(Right_Brake_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(Right_Brake_par.Load_Flash,Right_Brake_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(Right_Brake_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,Right_Brake_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��																																																																																	
              // 																																																		printf("NB=%u,C0=%f,c1=%f,C2=%f,c3=%f",Right_Brake_par.Load_Parameter.Parameter_NB,
              // 																																																																					Right_Brake_par.Load_Parameter.Parameter_C0.float_data,
              // 																																																																					Right_Brake_par.Load_Parameter.Parameter_C1.float_data,
              // 																																																																					Right_Brake_par.Load_Parameter.Parameter_C2.float_data,
              // 																																																																					Right_Brake_par.Load_Parameter.Parameter_C3.float_data);
                                                                                                                  break;
                                                                              case All_Weight_ID:
                                                                                                                  CabItemIndex = 4;
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
                                                                                                                CabBuf[4] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                All_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                All_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                All_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                All_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                All_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(All_Weight_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(All_Weight_par.Load_Flash,All_Weight_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(All_Weight_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,All_Weight_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case FootPlat_ID:
                                                                                                                  CabItemIndex = 5;
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
                                                                                                                CabBuf[5] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                FootPlat_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                FootPlat_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                FootPlat_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                FootPlat_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                FootPlat_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(FootPlat_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(FootPlat_par.Load_Flash,FootPlat_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(FootPlat_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,FootPlat_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Left_Move_ID:
                                                                                                                  CabItemIndex = 6;
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
                                                                                                                CabBuf[6] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                Left_Move_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                Left_Move_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                Left_Move_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                Left_Move_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                Left_Move_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(Left_Move_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(Left_Move_par.Load_Flash,Left_Move_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(Left_Move_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,Left_Move_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                  break;
                                                                              case Right_Move_ID:
                                                                                                                    CabItemIndex = 7;
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
                                                                                                                CabBuf[7] = &_ADCabVariable;
                                                                                                                Cabrate_Computer();  //����궨ϵ��
                                                                                                                Right_Move_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                Right_Move_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                Right_Move_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                Right_Move_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                Right_Move_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                
                                                                                                                SPI_FLASH_SectorErase_A(Right_Move_Flash_Address);		//��������
                                                                                                                SPI_FLASH_SectorWrite(Right_Move_par.Load_Flash,Right_Move_Flash_Address,17);//����ϵ��																																																
                                                                                                                SPI_FLASH_SectorErase_A(Right_Move_Flash_Address1);		//��������
                                                                                                                SPI_FLASH_SectorWrite(u8flash_buffer,Right_Move_Flash_Address1,18);//����궨����
                                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                break;
                                                                              default:
                                                                                                                  printf("\r\nError:The command channel ID is false!!!xie\r\n");
                                                                                                                  W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                                                                                  break;
                                                                          }
                                                                          break;
                                                                      }
                                                        case 'R'://���궨ϵ��
                                                                      {
                                                                          u8read_flg = 1;//���궨ϵ���ɹ�
                                                                          switch(Rx_Buffer[6])
                                                                          {																																
                                                                              case Left_Brake_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,Left_Brake_Flash_Address1,18);
                                                                                                                  break;
                                                                              case Right_Brake_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,Right_Brake_Flash_Address1,18);
                                                                                                                  break;
                                                                              case All_Weight_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,All_Weight_Flash_Address1,18);
                                                                                                                  break;
                                                                              case FootPlat_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,FootPlat_Flash_Address1,18);
                                                                                                                  break;
                                                                              case Left_Move_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,Left_Move_Flash_Address1,18);
                                                                                                                  break;
                                                                              case Right_Move_ID:
                                                                                                                  SPI_FLASH_BufferRead(u8flash_buffer,Right_Move_Flash_Address1,18);
                                                                                                                  break;
                                                                              default:
                                                                                                                  u8read_flg = 0;//���궨ϵ��ʧ��
                                                                                                                  printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
                                                                                                                  break;
                                                                          }			
                                                                          if(u8read_flg == 1)//���궨ϵ���ɹ�
                                                                          {
                                                                              Tx_Buffer[0] = 21;//֡���� ��λ
                                                                              Tx_Buffer[1] = 0;  //֡���� ��λ
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
                                                                              W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                                          }
                                                                          break;
                                                                      }
                                                         default :
                                                                     W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                                     break;
                                                    }
                                                    break;
                                      }
                                      case 'Y': //����
                                      {
                                                    if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                    {
                                                        Fun_Zero(Rx_Buffer[5],1);
                                                        W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                    }
                                                    else if(Rx_Buffer[5] == AD_Clear_ID) //ǿ������
                                                    {
                                                        Fun_Zero(Left_Brake_ID,1);
                                                        Fun_Zero(Right_Brake_ID,1);
                                                        Fun_Zero(All_Weight_ID,1);
                                                        W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                    }
                                                    else
                                                    {
                                                        W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                    }
                                                    break;
                                      }
                                      default :
                                                    W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��																								 
                                                    printf("\r\nError:The command is false!!!\r\n");
                                                    break;
                                  }
                             }
                             else if(WorkState_Flg == SignalTest_State_Flg) //�źŲ���״̬
                             {
                                  switch(u8_Command)
                                  {
                                      case 'H':
                                      {
                                                    if(Rx_Buffer[5]  == 'A')
                                                    {
                                                        Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
                                                        Tx_Buffer[0] = 12;  //֡���� ��λ
                                                        Tx_Buffer[1] = 0;  //֡���� ��λ
                                                        Tx_Buffer[2] = 'H'; 
                                                        Tx_Buffer[3] = 'A'; 
                                                        memcpy(&Tx_Buffer[4],cID_Key,10);
                                                        W5500_Send_Data(s,Tx_Buffer);//�ش�
                                                    }
                                                    if(Rx_Buffer[5]  == 'E')
                                                    {
                                                        if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                        {
                                                            cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
                                                            if(cCheck_Result == 0)  //������ȷ
                                                            {
                                                                Encryption_Cnt = 0;
                                                                W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
                                                                USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                USER_SystemFlag.A.Flag_Standalone = 1;  //������������ɹ�
                                                            }
                                                            else
                                                            {
                                                                Encryption_Cnt ++;																											
                                                                W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
                                                            }
                                                        }
                                                        else
                                                        {
                                                            W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
                                                        }
                                                        
                                                    }
                                                    break;
                                      }
                                      case 'O'://����ָ��
                                      {
                                                    if(s==0)
                                                    {
                                                        Network0_StopFlg = 0;
                                                        W5500_Send_Data(s,con_TranO);	//����
                                                    }
                                                    else if(s==2)
                                                    {
                                                        Network1_StopFlg = 0;
                                                    }
                                                    break;
                                      }
                                      case 'S'://ȡ״̬
                                      {
                                                    switch(WorkState_Flg)
                                                    {
                                                          case Default_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'S';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case Adjust_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'J';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case SignalTest_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'C';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case StaticWeight_None_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case StaticWeight_Full_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '2';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case Retardant_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '3';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_None_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '4';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_Full_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '5';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_Motor_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '6';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          default :
                                                                      W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
                                                                      break;
                                                          
                                                    }
                                                    break;
                                      }												
                                      case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
                                      {
                                                    TIM2_count = 0;//40S�жϹرյ�ŷ���Դ
                                                    Pump_UpDown = IO_CLOSE; //������
                                                    USER_LEDFlag.A.LED_Down = 0;         //������
                                                    USER_LEDFlag.A.LED_Up = 1;           //������
                                                    Left_Machine = IO_CLOSE;
                                                    Right_Machine = IO_CLOSE;
                                                    Machine_Switch = IO_CLOSE;
                                                    Speed_Control = IO_CLOSE;			
                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                    Pump_Before = IO_CLOSE;
                                                    Pump_Behind = IO_CLOSE;
                                                    Left_Up = IO_CLOSE;
                                                    Right_Up = IO_CLOSE;
                                                    Left_Down = IO_OPEN;
                                                    Right_Down = IO_OPEN;
                                                    USER_LEDFlag.A.LED_TJUp = 0;         //̨�ܾ�
                                                    USER_LEDFlag.A.LED_TJDown = 1;       //̨�ܽ�
                                        
                                                    Clamp_All = IO_CLOSE;
                                                    Protect_Overflow = IO_CLOSE; 
                                                    Hydraumatic_Machine = IO_CLOSE;
                                                    System_State = Default_State;
                                                    Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
                                                    WorkState_Flg = Default_State_Flg;  //����״̬
                                                    W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                    return;
                                      }												
                                      case 'N'://�������ƺ�֪ͨ
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {
                                                        case '0'://ǰ�н��н�
                                                        {
                                                                    Pump_Before = IO_OPEN;	
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '1'://ǰ�н��ɿ�
                                                        {
                                                                    Pump_Before = IO_CLOSE;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '2'://��н��н�
                                                        {
                                                                    Pump_Behind = IO_OPEN;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '3'://��н��ɿ�
                                                        {
                                                                    Pump_Behind = IO_CLOSE;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '4'://���׾�
                                                        {
                                                                    Pump_UpDown = IO_CLOSE;
                                                                    USER_LEDFlag.A.LED_Up = 1;  				//������ָʾ��
                                                                    USER_LEDFlag.A.LED_Down = 0;    //������ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '5'://���׽�
                                                        {
                                                                    Pump_UpDown = IO_OPEN;
                                                                    USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
                                                                    USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '6'://̨�ܾ�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Left_Up = IO_OPEN;
                                                                    Right_Up = IO_OPEN;
                                                                    Left_Down = IO_CLOSE;
                                                                    Right_Down = IO_CLOSE;
                                                                    Protect_Overflow = IO_OPEN;
                                                                    USER_LEDFlag.A.LED_TJUp = 1;  				//̨�ܾ�ָʾ��
                                                                    USER_LEDFlag.A.LED_TJDown = 0;    //̨�ܽ�ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '7'://̨�ܽ�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Left_Up = IO_CLOSE;
                                                                    Right_Up = IO_CLOSE;
                                                                    Left_Down = IO_OPEN;
                                                                    Right_Down = IO_OPEN;
                                                                    Protect_Overflow = IO_OPEN;
                                                                    USER_LEDFlag.A.LED_TJUp = 0;  				//̨�ܾ�ָʾ��
                                                                    USER_LEDFlag.A.LED_TJDown = 1;    //̨�ܽ�ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'I'://̨�ܼн�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Clamp_All = IO_OPEN;
                                                                    Protect_Overflow = IO_OPEN;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '8'://�����ƶ���  Э��ʱ��
                                                        {
                                                                    
                                                                    break;
                                                        }
                                                        case '9'://���������� �ƶ���  �ƶ���ƽ����
                                                        {
                                                                    
                                                                    break;
                                                        }
                                                        case 'A':
                                                        {
                                                                    
                                                                    break;
                                                        }
                                                        case 'B'://��������
                                                        {
                                                                    Speed_Control = IO_OPEN;
                                                                    Machine_Switch = IO_CLOSE;
                                                                    Left_Machine = IO_OPEN;
                                                                    USER_SystemFlag.A.Flag_Ldynamo = 1;
                                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 1;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'C'://����ֹͣ
                                                        {
                                                                    Speed_Control = IO_CLOSE;
                                                                    Left_Machine = IO_CLOSE;
                                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'D'://�ҵ������
                                                        {
                                                                    Speed_Control = IO_OPEN;
                                                                    Machine_Switch = IO_CLOSE;
                                                                    Right_Machine = IO_OPEN;
                                                                    USER_SystemFlag.A.Flag_Rdynamo = 1;
                                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 1;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'E'://�ҵ��ֹͣ
                                                        {
                                                                    Speed_Control = IO_CLOSE;
                                                                    Right_Machine = IO_CLOSE;
                                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'F'://�������л�
                                                        {
                                                                    Machine_Switch = IO_OPEN;
                                                                    USER_SystemFlag.A.Flag_Switch = 1;
                        // 																						SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'G'://ȫ��ֹͣ
                                                        {
                                                                    Left_Machine = IO_CLOSE;
                                                                    Right_Machine = IO_CLOSE;
                                                                    Machine_Switch = IO_CLOSE;
                                                                    Speed_Control = IO_CLOSE;
                                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                                    SysParameter.DynamoStart_Flag = 0;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }																
                                                        default:
                                                                   W5500_Send_Data(s,con_TranXACK);				//������Ч
                                                                   break;
                                                    }
                                                    break;
                                      }
                                      case 'B'://�汾��Ϣ
                                      {
                                                    W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
                                                    break;
                                      }
                                      case 'C'://�źŲ��Խ���
                                      {
                                                    if(Rx_Buffer[5] == 'B')
                                                    {
                                                          if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
                                                          {
                                                              Signal_TransferTime = Rx_Buffer[6] - 0x30;
                                                              System_State = SignalTest_State;
                                                              WorkState_Flg = SignalTest_State_Flg;    //�źŲ���״̬
                                                              W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                          }
                                                    }
                                                    else if(Rx_Buffer[5] == 'E')
                                                    {
                                                        System_State = Default_State;
                                                        WorkState_Flg = Default_State_Flg;    //����״̬
                                                        W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                    }
                                                    else
                                                    {
                                                        switch(Rx_Buffer[5])
                                                        {																																
                                                            case Left_Brake_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Right_Brake_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case All_Weight_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case FootPlat_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Left_Move_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            case Right_Move_ID:
                                                                                                System_State = Adjust_State;
                                                                                                WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
                                                                                                W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                break;
                                                            default:
                                                                                                printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
              // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
                                                                                                break;	
                                                        }
                                                    }
                                                    break;
                                      }											
                                      case 'Y': //����
                                      {
                                                    if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                    {
                                                        Fun_Zero(Rx_Buffer[5],1);
                                                        W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                    }
                                                    else if(Rx_Buffer[5] == AD_Clear_ID) //ǿ������
                                                    {
                                                        Fun_Zero(Left_Brake_ID,1);
                                                        Fun_Zero(Right_Brake_ID,1);
                                                        Fun_Zero(All_Weight_ID,1);
                                                        W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                    }
                                                    else
                                                    {
                                                        W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                    }
                                                    break;
                                      }
                                      default :
                                                    W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��																								 
                                                    printf("\r\nError:The command is false!!!\r\n");
                                                    break;
                                  }
                             }
                             else  //���״̬
                             {
                                  switch(u8_Command)
                                   {
                                      case 'H':
                                      {
                                                    if(Rx_Buffer[5]  == 'A')
                                                    {
                                                        Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
                                                        Tx_Buffer[0] = 12;  //֡���� ��λ
                                                        Tx_Buffer[1] = 0;  //֡���� ��λ
                                                        Tx_Buffer[2] = 'H'; 
                                                        Tx_Buffer[3] = 'A'; 
                                                        memcpy(&Tx_Buffer[4],cID_Key,10);
                                                        W5500_Send_Data(s,Tx_Buffer);//�ش�
                                                    }
                                                    if(Rx_Buffer[5]  == 'E')
                                                    {
                                                        if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                        {
                                                            cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
                                                            if(cCheck_Result == 0)  //������ȷ
                                                            {
                                                                Encryption_Cnt = 0;
                                                                W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
                                                                USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                USER_SystemFlag.A.Flag_Standalone = 1;  //������������ɹ�
                                                            }
                                                            else
                                                            {
                                                                Encryption_Cnt ++;																											
                                                                W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
                                                            }
                                                        }
                                                        else
                                                        {
                                                            W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
                                                        }
                                                        
                                                    }
                                                    break;
                                      }
                                      case 'O'://����ָ��
                                      {
                                                    if(s==0)
                                                    {
                                                        Network0_StopFlg = 0;
                                                        W5500_Send_Data(s,con_TranO);	//����
                                                    }
                                                    else if(s==2)
                                                    {
                                                        Network1_StopFlg = 0;
                                                    }
                                                    break;
                                      }
                                      case 'S'://ȡ״̬
                                      {
                                                    switch(WorkState_Flg)
                                                    {
                                                          case Default_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'S';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case Adjust_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'J';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case SignalTest_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'C';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case StaticWeight_None_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '1';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case StaticWeight_Full_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '2';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case Retardant_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '3';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_None_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '4';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_Full_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '5';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          case BrakeTest_Motor_State_Flg:
                                                          {
                                                                                              con_TranS[3] = 'T';
                                                                                              con_TranS[4] = '6';
                                                                                              W5500_Send_Data(s,con_TranS);		
                                                                                              break;
                                                          }
                                                          default :
                                                                      W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
                                                                      break;
                                                          
                                                    }
                                                    break;
                                      }
                                      case 'U'://ǿ����βָ�� �ص�����״̬
                                      {
                                                    W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                    Left_Machine = IO_CLOSE;
                                                    Right_Machine = IO_CLOSE;
                                                    Machine_Switch = IO_CLOSE;
                                                    Speed_Control = IO_CLOSE;	
                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                    USER_SystemFlag.A.Flag_ST = 0;
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
                                                    Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                    WorkState_Flg = Default_State_Flg;  //����״̬
                                                    W5500_Send_Data(s,con_TranU);				//����
                                                    return;
                                      }
                                      case 'I'://��ʼ��ָ��
                                      {																		
                                                    if(Rx_Buffer[5]  == 'B')
                                                    {
                                                        W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                        for(i=0;i<802;i++)
                                                        {
                                                            u16Left_Data[i] = 0;
                                                            u16Right_Data[i] = 0;
                                                        }
                                                        for(i=0;i<3250;i++)
                                                        {
                                                            u8Tran_CurrentZ[i] = 0;
                                                            u8Tran_CurrentC[i] = 0;
                                                            u8Tran_CurrentJ[i] = 0;
                                                            u8Tran_CurrentN[i] = 0;
                                                        }
                                                        Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                        W5500_Send_Data(s,con_TranI);				//��ȷӦ��
                                                    }
                                                    else
                                                    {
                                                        W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
                                                    }
                                                    break;
                                      }
                                      case 'N'://�������ƺ�֪ͨ
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {
                                                        case '0'://ǰ�н��н�
                                                        {
                                                                    Pump_Before = IO_OPEN;	
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '1'://ǰ�н��ɿ�
                                                        {
                                                                    Pump_Before = IO_CLOSE;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '2'://��н��н�
                                                        {
                                                                    Pump_Behind = IO_OPEN;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '3'://��н��ɿ�
                                                        {
                                                                    Pump_Behind = IO_CLOSE;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '4'://���׾�
                                                        {
                                                                    Pump_UpDown = IO_CLOSE;
                                                                    USER_LEDFlag.A.LED_Up = 1;  				//������ָʾ��
                                                                    USER_LEDFlag.A.LED_Down = 0;    //������ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '5'://���׽�
                                                        {
                                                                    Pump_UpDown = IO_OPEN;
                                                                    USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
                                                                    USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '6'://̨�ܾ�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Left_Up = IO_OPEN;
                                                                    Right_Up = IO_OPEN;
                                                                    Left_Down = IO_CLOSE;
                                                                    Right_Down = IO_CLOSE;
                                                                    Protect_Overflow = IO_OPEN;
                                                                    USER_LEDFlag.A.LED_TJUp = 1;  				//̨�ܾ�ָʾ��
                                                                    USER_LEDFlag.A.LED_TJDown = 0;    //̨�ܽ�ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case '7'://̨�ܽ�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Left_Up = IO_CLOSE;
                                                                    Right_Up = IO_CLOSE;
                                                                    Left_Down = IO_OPEN;
                                                                    Right_Down = IO_OPEN;
                                                                    Protect_Overflow = IO_OPEN;
                                                                    USER_LEDFlag.A.LED_TJUp = 0;  				//̨�ܾ�ָʾ��
                                                                    USER_LEDFlag.A.LED_TJDown = 1;    //̨�ܽ�ָʾ��
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }
                                                        case 'I'://̨�ܼн�
                                                        {
                                                                    TIM2_count = 0;
                                                                    Hydraumatic_Machine = IO_OPEN;
                                                                    Clamp_All = IO_OPEN;
                                                                    Protect_Overflow = IO_OPEN;
                                                                    W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                    break;
                                                        }			
                                                        case '8'://�����ƶ�������
                                                        {																											
                                                                    if(Rx_Buffer[2] == 26)//֡��26
                                                                    {
                                                                        Left_BrakeSum = 0;
                                                                        Right_BrakeSum = 0;
                                                                        Left_BrakeSub = 0;
                                                                        Right_BrakeSub = 0;
                                                                        if(Rx_Buffer[6] > 0x30) Left_BrakeSum += (Rx_Buffer[6]-0x30)*10000;
                                                                        if(Rx_Buffer[7] > 0x30) Left_BrakeSum += (Rx_Buffer[7]-0x30)*1000;
                                                                        if(Rx_Buffer[8] > 0x30) Left_BrakeSum += (Rx_Buffer[8]-0x30)*100;
                                                                        if(Rx_Buffer[9] > 0x30) Left_BrakeSum += (Rx_Buffer[9]-0x30)*10;
                                                                        if(Rx_Buffer[10] > 0x30) Left_BrakeSum += Rx_Buffer[10]-0x30;
                                                                      
                                                                        if(Rx_Buffer[12] > 0x30) Right_BrakeSum += (Rx_Buffer[12]-0x30)*10000;
                                                                        if(Rx_Buffer[13] > 0x30) Right_BrakeSum += (Rx_Buffer[13]-0x30)*1000;
                                                                        if(Rx_Buffer[14] > 0x30) Right_BrakeSum += (Rx_Buffer[14]-0x30)*100;
                                                                        if(Rx_Buffer[15] > 0x30) Right_BrakeSum += (Rx_Buffer[15]-0x30)*10;
                                                                        if(Rx_Buffer[16] > 0x30) Right_BrakeSum += Rx_Buffer[16]-0x30;
                                                                      
                                                                        if(Rx_Buffer[18] > 0x30) Left_BrakeSub += (Rx_Buffer[18]-0x30)*10000;
                                                                        if(Rx_Buffer[19] > 0x30) Left_BrakeSub += (Rx_Buffer[19]-0x30)*1000;
                                                                        if(Rx_Buffer[20] > 0x30) Left_BrakeSub += (Rx_Buffer[20]-0x30)*100;
                                                                        if(Rx_Buffer[21] > 0x30) Left_BrakeSub += (Rx_Buffer[21]-0x30)*10;
                                                                        if(Rx_Buffer[22] > 0x30) Left_BrakeSub += Rx_Buffer[22]-0x30;
                                                                        
                                                                        if(Rx_Buffer[24] > 0x30) Right_BrakeSub += (Rx_Buffer[24]-0x30)*10000;
                                                                        if(Rx_Buffer[25] > 0x30) Right_BrakeSub += (Rx_Buffer[25]-0x30)*1000;
                                                                        if(Rx_Buffer[26] > 0x30) Right_BrakeSub += (Rx_Buffer[26]-0x30)*100;
                                                                        if(Rx_Buffer[27] > 0x30) Right_BrakeSub += (Rx_Buffer[27]-0x30)*10;
                                                                        if(Rx_Buffer[28] > 0x30) Right_BrakeSub += Rx_Buffer[28]-0x30;
                                                                        W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                        if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
                                                                        {
                                                                              if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                                                                              {
                                                                                    USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                                                                                    USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
                                                                                    FunW5500_SendLED(0,Left_BrakeSum,Right_BrakeSum);//�ϴ������ƶ���
                                                                              }
                                                                        }
                                                                     }
                                                                     else
                                                                     {
                                                                         W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
                                                                     }
                                                                    break;
                                                        }
                                                        case '9'://���������� 
                                                        {
                                                                    if(Rx_Buffer[2] == 14)//֡��14
                                                                    {
                                                                        Left_BrakeAve = 0;
                                                                        Right_BrakeAve = 0;
                                                                      
                                                                        if(Rx_Buffer[6] > 0x30) Left_BrakeAve += (Rx_Buffer[6]-0x30)*10000;
                                                                        if(Rx_Buffer[7] > 0x30) Left_BrakeAve += (Rx_Buffer[7]-0x30)*1000;
                                                                        if(Rx_Buffer[8] > 0x30) Left_BrakeAve += (Rx_Buffer[8]-0x30)*100;
                                                                        if(Rx_Buffer[9] > 0x30) Left_BrakeAve += (Rx_Buffer[9]-0x30)*10;
                                                                        if(Rx_Buffer[10] > 0x30) Left_BrakeAve += Rx_Buffer[10]-0x30;
                                                                      
                                                                        if(Rx_Buffer[12] > 0x30) Right_BrakeAve += (Rx_Buffer[12]-0x30)*10000;
                                                                        if(Rx_Buffer[13] > 0x30) Right_BrakeAve += (Rx_Buffer[13]-0x30)*1000;
                                                                        if(Rx_Buffer[14] > 0x30) Right_BrakeAve += (Rx_Buffer[14]-0x30)*100;
                                                                        if(Rx_Buffer[15] > 0x30) Right_BrakeAve += (Rx_Buffer[15]-0x30)*10;
                                                                        if(Rx_Buffer[16] > 0x30) Right_BrakeAve += Rx_Buffer[16]-0x30;																												
                                                                        W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                        if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
                                                                        {
                                                                              if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                                                                              {
                                                                                    USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                                                                                    USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
                                                                                    FunW5500_SendLED(0,Left_BrakeAve,Right_BrakeAve);//�ϴ�������
                                                                              }
                                                                        }
                                                                     }
                                                                     else
                                                                     {
                                                                         W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
                                                                     }
                                                                    break;
                                                        }
                                                        case 'A':
                                                        {
                                                                      if(Rx_Buffer[2] == 26)//֡��26
                                                                    {
                                                                        Left_BrakeSumJJ = 0;
                                                                        Right_BrakeSumJJ = 0;
                                                                        Left_BrakeSubJJ = 0;
                                                                        Right_BrakeSubJJ = 0;
                                                                        if(Rx_Buffer[6] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[6]-0x30)*10000;
                                                                        if(Rx_Buffer[7] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[7]-0x30)*1000;
                                                                        if(Rx_Buffer[8] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[8]-0x30)*100;
                                                                        if(Rx_Buffer[9] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[9]-0x30)*10;
                                                                        if(Rx_Buffer[10] > 0x30) Left_BrakeSumJJ += Rx_Buffer[10]-0x30;
                                                                      
                                                                        if(Rx_Buffer[12] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[12]-0x30)*10000;
                                                                        if(Rx_Buffer[13] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[13]-0x30)*1000;
                                                                        if(Rx_Buffer[14] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[14]-0x30)*100;
                                                                        if(Rx_Buffer[15] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[15]-0x30)*10;
                                                                        if(Rx_Buffer[16] > 0x30) Right_BrakeSumJJ += Rx_Buffer[16]-0x30;
                                                                      
                                                                        if(Rx_Buffer[18] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[18]-0x30)*10000;
                                                                        if(Rx_Buffer[19] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[19]-0x30)*1000;
                                                                        if(Rx_Buffer[20] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[20]-0x30)*100;
                                                                        if(Rx_Buffer[21] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[21]-0x30)*10;
                                                                        if(Rx_Buffer[22] > 0x30) Left_BrakeSubJJ += Rx_Buffer[22]-0x30;
                                                                        
                                                                        if(Rx_Buffer[24] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[24]-0x30)*10000;
                                                                        if(Rx_Buffer[25] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[25]-0x30)*1000;
                                                                        if(Rx_Buffer[26] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[26]-0x30)*100;
                                                                        if(Rx_Buffer[27] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[27]-0x30)*10;
                                                                        if(Rx_Buffer[28] > 0x30) Right_BrakeSubJJ += Rx_Buffer[28]-0x30;
                                                                        W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
                                                                        if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
                                                                        {
                                                                              if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
                                                                              {
                                                                                    USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
                                                                                    USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
                                                                                    FunW5500_SendLED(0,Left_BrakeSumJJ,Right_BrakeSumJJ);//�ϴ������ƶ���
                                                                              }
                                                                        }
                                                                     }
                                                                     else
                                                                     {
                                                                         W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
                                                                     }
                                                                    break;
                                                        }
                                                        case 'J'://N J  ��������
                                                        {
                                                                    if(Rx_Buffer[2] == 14)//֡��14
                                                                    {
                                                                        Left_StaticWeight = 0;
                                                                        Right_StaticWeight = 0;
                                                                        if(Rx_Buffer[6] > 0x30) Left_StaticWeight += (Rx_Buffer[6]-0x30)*10000;
                                                                        if(Rx_Buffer[7] > 0x30) Left_StaticWeight += (Rx_Buffer[7]-0x30)*1000;
                                                                        if(Rx_Buffer[8] > 0x30) Left_StaticWeight += (Rx_Buffer[8]-0x30)*100;
                                                                        if(Rx_Buffer[9] > 0x30) Left_StaticWeight += (Rx_Buffer[9]-0x30)*10;
                                                                        if(Rx_Buffer[10] > 0x30) Left_StaticWeight += Rx_Buffer[10]-0x30;
                                                                        if(Rx_Buffer[12] > 0x30) Right_StaticWeight += (Rx_Buffer[12]-0x30)*10000;
                                                                        if(Rx_Buffer[13] > 0x30) Right_StaticWeight += (Rx_Buffer[13]-0x30)*1000;
                                                                        if(Rx_Buffer[14] > 0x30) Right_StaticWeight += (Rx_Buffer[14]-0x30)*100;
                                                                        if(Rx_Buffer[15] > 0x30) Right_StaticWeight += (Rx_Buffer[15]-0x30)*10;
                                                                        if(Rx_Buffer[16] > 0x30) Right_StaticWeight += Rx_Buffer[16]-0x30;
                                                                        if(Left_StaticWeight<200)
                                                                        {
                                                                            Left_StaticWeight = 200;																															
                                                                            W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
                                                                        }
                                                                        if(Right_StaticWeight<200)
                                                                        {
                                                                            Right_StaticWeight = 200;
                                                                            W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
                                                                        }
              // 																													System_State = DynamoStart_State;
                                                                        W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                                        Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                    }
                                                                    else
                                                                    {
                                                                        W5500_Send_Data(s,con_TranXACK);				//������Ч
                                                                    }
                                                                    break;
                                                        }			
                                                        default:
                                                                   W5500_Send_Data(s,con_TranXACK);				//������Ч
                                                                   break;
                                                    }																			
                                                    break;
                                      }
                                      case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
                                      {
                                                    TIM2_count = 0;//40S�жϹرյ�ŷ���Դ
                                                    Pump_UpDown = IO_CLOSE; //������
                                                    USER_LEDFlag.A.LED_Down = 0;         //������
                                                    USER_LEDFlag.A.LED_Up = 1;           //������
                                                    Left_Machine = IO_CLOSE;
                                                    Right_Machine = IO_CLOSE;
                                                    Machine_Switch = IO_CLOSE;
                                                    Speed_Control = IO_CLOSE;			
                                                    USER_SystemFlag.A.Flag_Ldynamo = 0;
                                                    USER_SystemFlag.A.Flag_Rdynamo = 0;
                                                    USER_SystemFlag.A.Flag_Switch = 0;
                                                    USER_SystemFlag.A.Flag_ST = 0;
                                                    Pump_Before = IO_CLOSE;
                                                    Pump_Behind = IO_CLOSE;
                                                    Left_Up = IO_CLOSE;
                                                    Right_Up = IO_CLOSE;
                                                    Left_Down = IO_OPEN;
                                                    Right_Down = IO_OPEN;
                                                    USER_LEDFlag.A.LED_TJUp = 0;         //̨�ܾ�
                                                    USER_LEDFlag.A.LED_TJDown = 1;       //̨�ܽ�
                                        
                                                    Clamp_All = IO_CLOSE;
                                                    Protect_Overflow = IO_CLOSE; 
                                                    Hydraumatic_Machine = IO_CLOSE;
                                                    System_State = Default_State;
                                                    Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
                                                    WorkState_Flg = Default_State_Flg;  //����״̬
                                                    W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                    return;
                                      }																
                                      case 'B'://�汾��Ϣ
                                      {
                                                    W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
                                                    break;
                                      }											
                                      case 'D'://ȡ���Խ��
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {																																																																																																					
                                                        case 'F'://�����ƶ�̨��ɽ��
                                                        {
                                                                      if(Rx_Buffer[6] == '1')
                                                                      {
                                                                          FunW5500_SendDF(s,1,AllNone_StaticWeight);																											
                                                                      }
                                                                      else if(Rx_Buffer[6] == '2')
                                                                      {																														
                                                                          FunW5500_SendDF(s,2,AllFull_StaticWeight);
                                                                      }
                                                                      else
                                                                      {
                                                                          W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
                                                                      }
                                                                      break;
                                                        }
                                                        case 'T'://����̤���������
                                                        {
                                                                      break;
                                                        }	
                                                        case 'D'://���ͼ���� DD
                                                        {
                                                                      FunW5500_SendDC(s);				//���͵�ǰ�����
                                                                      break;
                                                        }	
                                                        case 'C'://����̤���������
                                                        {
                                                                      W5500_Send_Data(s,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
    // 																																	FunW5500_SendDC(s);				//���͵�ǰ�����
                                                                      break;
                                                        }			
                                                        case 'B'://�����ƶ���������
                                                        {
                                                                      
    // 																																	W5500_Send_Data(s,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
                                                                      break;
                                                        }
                                                        case 'J'://���ͼ����ƶ���������
                                                        {
                                                                      
                                                                      W5500_Send_Data(s,u8Tran_CurrentJ);				//���͵�ǰ�����ƶ���������
                                                                      break;
                                                        }
                                                        case 'Z'://�������͹�������
                                                        {
                                                                      
                                                                      W5500_Send_Data(s,u8Tran_CurrentZ);				//���͵�ǰ���͹�������
                                                                      break;
                                                        }
                                                        default:
                                                                      W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
                                                                      break;
                                                    }
                                                    break;
                                      }
                                      case 'G'://ȡʵʱ����
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {																					
                                                        case 'B':
                                                        {
                                                                      Tx_Buffer[0] = 14;  //֡���� ��λ
                                                                      Tx_Buffer[1] = 0;    //֡���� ��λ
                                                                      Tx_Buffer[2] = 'G'; 
                                                                      Tx_Buffer[3] = 'B'; 
                                                                      /* ���� */
                                                                      Tx_Buffer[4] = Left_DynamicBrake/10000; 
                                                                      if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
                                                                      else Tx_Buffer[4] = 0x20;
                                                                      Tx_Buffer[5] = Left_DynamicBrake%10000/1000; 
                                                                      if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
                                                                      else Tx_Buffer[5] = 0x20;
                                                                      Tx_Buffer[6] = Left_DynamicBrake%1000/100; 
                                                                      if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
                                                                      else Tx_Buffer[6] = 0x20;
                                                                      Tx_Buffer[7] = Left_DynamicBrake%100/10; 
                                                                      if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
                                                                      else Tx_Buffer[7] = 0x20;
                                                                      Tx_Buffer[8] = Left_DynamicBrake%10 + 0x30; 
                                                                      Tx_Buffer[9] = 0x20; 
                                                                      /* ���� */
                                                                      Tx_Buffer[10] = Right_DynamicBrake/10000; 
                                                                      if(Tx_Buffer[10] > 0) Tx_Buffer[10] += 0x30;
                                                                      else Tx_Buffer[10] = 0x20;
                                                                      Tx_Buffer[11] = Right_DynamicBrake%10000/1000; 
                                                                      if(Tx_Buffer[11] > 0) Tx_Buffer[11] += 0x30;
                                                                      else Tx_Buffer[11] = 0x20;
                                                                      Tx_Buffer[12] = Right_DynamicBrake%1000/100; 
                                                                      if(Tx_Buffer[12] > 0) Tx_Buffer[12] += 0x30;
                                                                      else Tx_Buffer[12] = 0x20;
                                                                      Tx_Buffer[13] = Right_DynamicBrake%100/10; 
                                                                      if(Tx_Buffer[13] > 0) Tx_Buffer[13] += 0x30;
                                                                      else Tx_Buffer[13] = 0x20;
                                                                      Tx_Buffer[14] = Right_DynamicBrake%10 + 0x30; 
                                                                      Tx_Buffer[15] = 0x20; 
                                                                      W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ�ƶ�
                                                                      break;
                                                        }																																								
                                                        case 'T':
                                                        {
                                                                      Tx_Buffer[0] = 8;    //֡���� ��λ
                                                                      Tx_Buffer[1] = 0;    //֡���� ��λ
                                                                      Tx_Buffer[2] = 'G'; 
                                                                      Tx_Buffer[3] = 'T'; 
                                                                      /* ̤���� */
                                                                      Tx_Buffer[4] = DynamicFootPlat/10000; 
                                                                      if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
                                                                      else Tx_Buffer[4] = 0x20;
                                                                      Tx_Buffer[5] = DynamicFootPlat%10000/1000; 
                                                                      if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
                                                                      else Tx_Buffer[5] = 0x20;
                                                                      Tx_Buffer[6] = DynamicFootPlat%1000/100; 
                                                                      if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
                                                                      else Tx_Buffer[6] = 0x20;
                                                                      Tx_Buffer[7] = DynamicFootPlat%100/10; 
                                                                      if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
                                                                      else Tx_Buffer[7] = 0x20;
                                                                      Tx_Buffer[8] = DynamicFootPlat%10 + 0x30; 
                                                                      Tx_Buffer[9] = 0x20; 
                                                                      W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ̤����
                                                                      break;
                                                        }						
                                                        case 'F':
                                                        {
                                                                      Tx_Buffer[0] = 8;    //֡���� ��λ
                                                                      Tx_Buffer[1] = 0;    //֡���� ��λ
                                                                      Tx_Buffer[2] = 'G'; 
                                                                      Tx_Buffer[3] = 'F'; 
                                                                      if(SysParameter.Par6_Weight == 2)
                                                                       {
                                                                           Quotient = All_DynamicWeight / 2;
                                                                           Remainder = All_DynamicWeight % 2;
                                                                           if(Remainder > 0)
                                                                           {
                                                                              All_DynamicWeight = (Quotient +1) * 2;																										 
                                                                           }																									
                                                                       }
                                                                       else if(SysParameter.Par6_Weight == 5)
                                                                       {
                                                                          Quotient = All_DynamicWeight / 5;
                                                                           Remainder = All_DynamicWeight % 5;
                                                                           if(Remainder < 3)
                                                                           {
                                                                              All_DynamicWeight = Quotient * 5;																										 
                                                                           }
                                                                           else
                                                                           {
                                                                              All_DynamicWeight = (Quotient +1) * 5;
                                                                           }																									
                                                                       }
                                                                      /* ���ϳ��� */
                                                                      Tx_Buffer[4] = All_DynamicWeight/10000; 
                                                                      if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
                                                                      else Tx_Buffer[4] = 0x20;
                                                                      Tx_Buffer[5] = All_DynamicWeight%10000/1000; 
                                                                      if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
                                                                      else Tx_Buffer[5] = 0x20;
                                                                      Tx_Buffer[6] = All_DynamicWeight%1000/100; 
                                                                      if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
                                                                      else Tx_Buffer[6] = 0x20;
                                                                      Tx_Buffer[7] = All_DynamicWeight%100/10; 
                                                                      if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
                                                                      else Tx_Buffer[7] = 0x20;
                                                                      Tx_Buffer[8] = All_DynamicWeight%10 + 0x30; 
                                                                      Tx_Buffer[9] = 0x20; 
                                                                      W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ��������
                                                                      break;
                                                        }	
                                                        default :
                                                                     W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
                                                                     break;
                                                    }
                                                    break;
                                      }
                                      case 'T':
                                      {
                                                    switch(Rx_Buffer[5])
                                                    {																
                                                        case 'B': //�����ƶ�����
                                                        {
                                                                      System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
                                                                      W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                      Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                      WorkState_Flg = BrakeTest_None_State_Flg;
                                                                      break;
                                                        }
                                                        case 'J': //�����ƶ�����
                                                        {
                                                                      System_State = BrakeTest_Full_State; //ϵͳ��������ƶ�����
                                                                       W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                       Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                       WorkState_Flg = BrakeTest_Full_State_Flg;
                                                                      break;
                                                        }																											
                                                        case 'Z':  //���������
                                                        {
                                                                       if((Rx_Buffer[6] == 0x31) || (Rx_Buffer[6] == 0x32))
                                                                       {
                                                                           System_State = Retardant_State; 					//ϵͳ�������Ͳ���
                                                                           SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
                                                                           W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
                                                                           Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                          WorkState_Flg = Retardant_State_Flg;
                                                                       }
                                                                       else
                                                                       {	
                                                                            W5500_Send_Data(s,con_TranXACK);	//��Чָ֡��
                                                                       }
                                                                       break;
                                                        }
                                                        case 'D':  //�������
                                                        {
                                                                      if(Rx_Buffer[2] == 14)//֡��14
                                                                      {
                                                                          Left_StaticWeight = 0;
                                                                          Right_StaticWeight = 0;
                        // 																									if(Rx_Buffer[6] > 0x30) Left_StaticWeight += (Rx_Buffer[6]-0x30)*10000;
                                                                          if(Rx_Buffer[7] > 0x30) Left_StaticWeight += (Rx_Buffer[7]-0x30)*1000;
                                                                          if(Rx_Buffer[8] > 0x30) Left_StaticWeight += (Rx_Buffer[8]-0x30)*100;
                                                                          if(Rx_Buffer[9] > 0x30) Left_StaticWeight += (Rx_Buffer[9]-0x30)*10;
                                                                          if(Rx_Buffer[10] > 0x30) Left_StaticWeight += Rx_Buffer[10]-0x30;
                        // 																									if(Rx_Buffer[12] > 0x30) Right_StaticWeight += (Rx_Buffer[12]-0x30)*10000;
                                                                          if(Rx_Buffer[13] > 0x30) Right_StaticWeight += (Rx_Buffer[13]-0x30)*1000;
                                                                          if(Rx_Buffer[14] > 0x30) Right_StaticWeight += (Rx_Buffer[14]-0x30)*100;
                                                                          if(Rx_Buffer[15] > 0x30) Right_StaticWeight += (Rx_Buffer[15]-0x30)*10;
                                                                          if(Rx_Buffer[16] > 0x30) Right_StaticWeight += Rx_Buffer[16]-0x30;
                                                                          if(Left_StaticWeight<200)
                                                                          {
                                                                              Left_StaticWeight = 200;
                                                                              W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
                                                                          }
                                                                          if(Right_StaticWeight<200)
                                                                          {
                                                                              Right_StaticWeight = 200;
                                                                              W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
                                                                          }
                                                                          System_State = DynamoStart_State;
                                                                          W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                                          Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                      }
                                                                      else
                                                                      {
                                                                          W5500_Send_Data(s,con_TranXACK);				//������Ч
                                                                      }
                                                                      break;
                                                        }
                                                        case 'F':		//��ɲ���
                                                        {	
                                                                       if(Rx_Buffer[6] == '1') //���ظ��ϳ���
                                                                       {
                                                                          System_State = StaticWeight_None_State;   //ϵͳ������ظ��ϳ���
                                                                          W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                          WorkState_Flg = StaticWeight_None_State_Flg;
                                                                       }
                                                                       else if(Rx_Buffer[6] == '2')//���ؾ���
                                                                       {
                                                                          System_State = StaticWeight_Full_State;  //ϵͳ������ؾ���״̬
                                                                          W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                          WorkState_Flg = StaticWeight_Full_State_Flg;
                                                                       }
                                                                       else
                                                                       {
                                                                          W5500_Send_Data(s,con_TranXACK);//ָ����Ч
                                                                       }																
                                                                       Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                       break;
                                                        }
                                                    }
                                                    break;
                                      }
                                      default :
                                                    W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��																								 
                                                    printf("\r\nError:The command is false!!!\r\n");
                                                    break;
                                  }
                             }
                         }
									 }
									 else
									 {
												 if(USER_SystemFlag.A.Flag_Sucess == 0) //���ּ���
												 {
															switch(u8_Command)
															{																	
																	case 'O'://����ָ��
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
																										Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
																										Tx_Buffer[0] = 12;  //֡���� ��λ
																										Tx_Buffer[1] = 0;  //֡���� ��λ
																										Tx_Buffer[2] = 'H'; 
																										Tx_Buffer[3] = 'A'; 
																										memcpy(&Tx_Buffer[4],cID_Key,10);
																										W5500_Send_Data(s,Tx_Buffer);//�ش�
																								}
																								if(Rx_Buffer[5]  == 'E')
																								{
																										if(Encryption_Cnt <= ENCRYPTION_MAX)
																										{
																												cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
																												if(cCheck_Result == 0)  //������ȷ
																												{
																														Encryption_Cnt = 0;
																														W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
																														USER_SystemFlag.A.Flag_Sucess = 1;
                                                            USER_SystemFlag.A.Flag_Online = 1;  //�����������ɹ�
                                                            USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																												}
																												else
																												{
																														Encryption_Cnt ++;																											
																														W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
																												}
																										}
																										else
																										{
																												W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
																										}
																										
																								}
																								break;
																	 }
																	default :
																								W5500_Send_Data(s,con_TranHX0);//�ش�ָ�����
																								break;
															}
												 }
												 else
												 {
															 if(WorkState_Flg == Default_State_Flg)  //����״̬
															 {
																		switch(u8_Command)
																		{						
																				case 'V'://�Լ�
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
																													Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
																													Tx_Buffer[0] = 12;  //֡���� ��λ
																													Tx_Buffer[1] = 0;  //֡���� ��λ
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//�ش�
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
																															if(cCheck_Result == 0)  //������ȷ
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //�����������ɹ�
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
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
																										 W5500_Send_Data(s,con_TranWE);				//�ش��쳣����
																										 return;	
																								}
																								WFLASHStatus=FLASH_ProgramWord(2048 * 255, 0X77777777);
																								if(WFLASHStatus!=FLASH_COMPLETE)                                  //???????:???E0
																								{
																										FLASH_Lock();
																										W5500_Send_Data(s,con_TranWE);				//�ش��쳣����
																										return;	
																								}
																								if(FLASH_Read_Word(2048 * 255)==0X77777777)
																								{
																										FLASH_Lock();
																										W5500_Send_Data(s,con_TranWG);				//�ش�����Ӧ��	
																										Delay_ms(10);                                                //10ms??	
																										NVIC_SystemReset();
																								}
																								else                                                                //
																								{
																											FLASH_Lock();
																											W5500_Send_Data(s,con_TranWE);			//�ش��쳣����
																								 }
																						}		
																						break;
																				}	
																				case 'O'://����ָ��
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
																				case 'S'://ȡ״̬
																				{
																											switch(WorkState_Flg)
																											{
																														case Default_State_Flg:
																														{
																																																con_TranS[3] = 'S';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Adjust_State_Flg:
																														{
																																																con_TranS[3] = 'J';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case SignalTest_State_Flg:
																														{
																																																con_TranS[3] = 'C';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_None_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_Full_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Retardant_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '3';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_None_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '4';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_Full_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '5';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_Motor_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '6';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														default :
																																				W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
																																				break;
																														
																											}
																											break;
																				}
																				case 'U'://ǿ����βָ�� �ص�����״̬
																				{
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;	
																											USER_SystemFlag.A.Flag_Ldynamo = 0;
																											USER_SystemFlag.A.Flag_Rdynamo = 0;
																											USER_SystemFlag.A.Flag_Switch = 0;
																											USER_SystemFlag.A.Flag_ST = 0;
																					
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
																											Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״̬
																											W5500_Send_Data(s,con_TranU);				//����
																											return;
																				}
																				case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
																				{
                                                      TIM2_count = 0;//40S�жϹرյ�ŷ���Դ
                                                      Pump_UpDown = IO_CLOSE; //������
                                                      USER_LEDFlag.A.LED_Down = 0;         //������
                                                      USER_LEDFlag.A.LED_Up = 1;           //������
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;			
																											USER_SystemFlag.A.Flag_Ldynamo = 0;
																											USER_SystemFlag.A.Flag_Rdynamo = 0;
																											USER_SystemFlag.A.Flag_Switch = 0;
																											USER_SystemFlag.A.Flag_ST = 0;
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
                                                      USER_LEDFlag.A.LED_TJUp = 0;         //̨�ܾ�
                                                      USER_LEDFlag.A.LED_TJDown = 1;       //̨�ܽ�
                                          
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״̬
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											W5500_Send_Data(s,con_TranR);				   //��λ���
																											return;
																				}
																				case 'I'://��ʼ��ָ��
																				{																		
																											if(Rx_Buffer[5]  == 'B')
																											{
																													W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																													Pump_UpDown = IO_OPEN;
																													USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																													USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																													for(i=0;i<802;i++)
																													{
																															u16Left_Data[i] = 0;
																															u16Right_Data[i] = 0;
																													}
								// 																					for(i=0;i<2010;i++)
								// 																					{
								// 																							u8Tran_CurrentZ[i] = 0;
								// 																					}
																													for(i=0;i<3250;i++)
																													{
																															u8Tran_CurrentZ[i] = 0;
																															u8Tran_CurrentC[i] = 0;
																															u8Tran_CurrentJ[i] = 0;
																															u8Tran_CurrentN[i] = 0;
																													}
																													Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																													W5500_Send_Data(s,con_TranI);				//��ȷӦ��
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
																											}
																											break;
																				}
																				case 'N'://�������ƺ�֪ͨ
																				{
																											switch(Rx_Buffer[5])
																											{
																													case '0'://ǰ�н��н�
																													{
																																			Pump_Before = IO_OPEN;	
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '1'://ǰ�н��ɿ�
																													{
																																			Pump_Before = IO_CLOSE;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '2'://��н��н�
																													{
																																			Pump_Behind = IO_OPEN;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '3'://��н��ɿ�
																													{
																																			Pump_Behind = IO_CLOSE;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '4'://���׾�
																													{
																																			Pump_UpDown = IO_CLOSE;
																																			USER_LEDFlag.A.LED_Up = 1;  				//������ָʾ��
																																			USER_LEDFlag.A.LED_Down = 0;    //������ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '5'://���׽�
																													{
																																			Pump_UpDown = IO_OPEN;
																																			USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																			USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '6'://̨�ܾ�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Left_Up = IO_OPEN;
																																			Right_Up = IO_OPEN;
																																			Left_Down = IO_CLOSE;
																																			Right_Down = IO_CLOSE;
																																			Protect_Overflow = IO_OPEN;			
																																			USER_LEDFlag.A.LED_TJUp = 1;  				//̨�ܾ�ָʾ��
																																			USER_LEDFlag.A.LED_TJDown = 0;    //̨�ܽ�ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '7'://̨�ܽ�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Left_Up = IO_CLOSE;
																																			Right_Up = IO_CLOSE;
																																			Left_Down = IO_OPEN;
																																			Right_Down = IO_OPEN;
																																			Protect_Overflow = IO_OPEN;
																																			USER_LEDFlag.A.LED_TJUp = 0;  				//̨�ܾ�ָʾ��
																																			USER_LEDFlag.A.LED_TJDown = 1;    //̨�ܽ�ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'I'://̨�ܼн�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Clamp_All = IO_OPEN;
																																			Protect_Overflow = IO_OPEN;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '8'://�����ƶ�������
																													{																											
																																			if(Rx_Buffer[2] == 26)//֡��26
																																			{
																																					Left_BrakeSum = 0;
																																					Right_BrakeSum = 0;
																																					Left_BrakeSub = 0;
																																					Right_BrakeSub = 0;
																																					if(Rx_Buffer[6] > 0x30) Left_BrakeSum += (Rx_Buffer[6]-0x30)*10000;
																																					if(Rx_Buffer[7] > 0x30) Left_BrakeSum += (Rx_Buffer[7]-0x30)*1000;
																																					if(Rx_Buffer[8] > 0x30) Left_BrakeSum += (Rx_Buffer[8]-0x30)*100;
																																					if(Rx_Buffer[9] > 0x30) Left_BrakeSum += (Rx_Buffer[9]-0x30)*10;
																																					if(Rx_Buffer[10] > 0x30) Left_BrakeSum += Rx_Buffer[10]-0x30;
																																				
																																					if(Rx_Buffer[12] > 0x30) Right_BrakeSum += (Rx_Buffer[12]-0x30)*10000;
																																					if(Rx_Buffer[13] > 0x30) Right_BrakeSum += (Rx_Buffer[13]-0x30)*1000;
																																					if(Rx_Buffer[14] > 0x30) Right_BrakeSum += (Rx_Buffer[14]-0x30)*100;
																																					if(Rx_Buffer[15] > 0x30) Right_BrakeSum += (Rx_Buffer[15]-0x30)*10;
																																					if(Rx_Buffer[16] > 0x30) Right_BrakeSum += Rx_Buffer[16]-0x30;
																																				
																																					if(Rx_Buffer[18] > 0x30) Left_BrakeSub += (Rx_Buffer[18]-0x30)*10000;
																																					if(Rx_Buffer[19] > 0x30) Left_BrakeSub += (Rx_Buffer[19]-0x30)*1000;
																																					if(Rx_Buffer[20] > 0x30) Left_BrakeSub += (Rx_Buffer[20]-0x30)*100;
																																					if(Rx_Buffer[21] > 0x30) Left_BrakeSub += (Rx_Buffer[21]-0x30)*10;
																																					if(Rx_Buffer[22] > 0x30) Left_BrakeSub += Rx_Buffer[22]-0x30;
																																					
																																					if(Rx_Buffer[24] > 0x30) Right_BrakeSub += (Rx_Buffer[24]-0x30)*10000;
																																					if(Rx_Buffer[25] > 0x30) Right_BrakeSub += (Rx_Buffer[25]-0x30)*1000;
																																					if(Rx_Buffer[26] > 0x30) Right_BrakeSub += (Rx_Buffer[26]-0x30)*100;
																																					if(Rx_Buffer[27] > 0x30) Right_BrakeSub += (Rx_Buffer[27]-0x30)*10;
																																					if(Rx_Buffer[28] > 0x30) Right_BrakeSub += Rx_Buffer[28]-0x30;
																																					W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																					if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
																																					{
																																								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
																																								{
																																											USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																																											USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																											FunW5500_SendLED(0,Left_BrakeSum,Right_BrakeSum);//�ϴ������ƶ���
																																								}
																																					}
																																			 }
																																			 else
																																			 {
																																					 W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
																																			 }
																																			break;
																													}
																													case '9'://���������� 
																													{
																																			if(Rx_Buffer[2] == 14)//֡��14
																																			{
																																					Left_BrakeAve = 0;
																																					Right_BrakeAve = 0;
																																				
																																					if(Rx_Buffer[6] > 0x30) Left_BrakeAve += (Rx_Buffer[6]-0x30)*10000;
																																					if(Rx_Buffer[7] > 0x30) Left_BrakeAve += (Rx_Buffer[7]-0x30)*1000;
																																					if(Rx_Buffer[8] > 0x30) Left_BrakeAve += (Rx_Buffer[8]-0x30)*100;
																																					if(Rx_Buffer[9] > 0x30) Left_BrakeAve += (Rx_Buffer[9]-0x30)*10;
																																					if(Rx_Buffer[10] > 0x30) Left_BrakeAve += Rx_Buffer[10]-0x30;
																																				
																																					if(Rx_Buffer[12] > 0x30) Right_BrakeAve += (Rx_Buffer[12]-0x30)*10000;
																																					if(Rx_Buffer[13] > 0x30) Right_BrakeAve += (Rx_Buffer[13]-0x30)*1000;
																																					if(Rx_Buffer[14] > 0x30) Right_BrakeAve += (Rx_Buffer[14]-0x30)*100;
																																					if(Rx_Buffer[15] > 0x30) Right_BrakeAve += (Rx_Buffer[15]-0x30)*10;
																																					if(Rx_Buffer[16] > 0x30) Right_BrakeAve += Rx_Buffer[16]-0x30;																												
																																					W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																					if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
																																					{
																																								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
																																								{
																																											USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																																											USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																											FunW5500_SendLED(0,Left_BrakeAve,Right_BrakeAve);//�ϴ�������
																																								}
																																					}
																																			 }
																																			 else
																																			 {
																																					 W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
																																			 }
																																			break;
																													}
																													case 'A':
																													{
																																				if(Rx_Buffer[2] == 26)//֡��26
																																			{
																																					Left_BrakeSumJJ = 0;
																																					Right_BrakeSumJJ = 0;
																																					Left_BrakeSubJJ = 0;
																																					Right_BrakeSubJJ = 0;
																																					if(Rx_Buffer[6] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[6]-0x30)*10000;
																																					if(Rx_Buffer[7] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[7]-0x30)*1000;
																																					if(Rx_Buffer[8] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[8]-0x30)*100;
																																					if(Rx_Buffer[9] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[9]-0x30)*10;
																																					if(Rx_Buffer[10] > 0x30) Left_BrakeSumJJ += Rx_Buffer[10]-0x30;
																																				
																																					if(Rx_Buffer[12] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[12]-0x30)*10000;
																																					if(Rx_Buffer[13] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[13]-0x30)*1000;
																																					if(Rx_Buffer[14] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[14]-0x30)*100;
																																					if(Rx_Buffer[15] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[15]-0x30)*10;
																																					if(Rx_Buffer[16] > 0x30) Right_BrakeSumJJ += Rx_Buffer[16]-0x30;
																																				
																																					if(Rx_Buffer[18] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[18]-0x30)*10000;
																																					if(Rx_Buffer[19] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[19]-0x30)*1000;
																																					if(Rx_Buffer[20] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[20]-0x30)*100;
																																					if(Rx_Buffer[21] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[21]-0x30)*10;
																																					if(Rx_Buffer[22] > 0x30) Left_BrakeSubJJ += Rx_Buffer[22]-0x30;
																																					
																																					if(Rx_Buffer[24] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[24]-0x30)*10000;
																																					if(Rx_Buffer[25] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[25]-0x30)*1000;
																																					if(Rx_Buffer[26] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[26]-0x30)*100;
																																					if(Rx_Buffer[27] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[27]-0x30)*10;
																																					if(Rx_Buffer[28] > 0x30) Right_BrakeSubJJ += Rx_Buffer[28]-0x30;
																																					W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																					if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
																																					{
																																								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
																																								{
																																											USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																																											USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																											FunW5500_SendLED(0,Left_BrakeSumJJ,Right_BrakeSumJJ);//�ϴ������ƶ���
																																								}
																																					}
																																			 }
																																			 else
																																			 {
																																					 W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
																																			 }
																																			break;
																													}
																													case 'B'://��������
																													{
																																			Speed_Control = IO_OPEN;
																																			Machine_Switch = IO_CLOSE;
																																			Left_Machine = IO_OPEN;
																																			USER_SystemFlag.A.Flag_Ldynamo = 1;
																																			USER_SystemFlag.A.Flag_Switch = 0;
																																			USER_SystemFlag.A.Flag_ST = 1;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'C'://����ֹͣ
																													{
																																			Speed_Control = IO_CLOSE;
																																			Left_Machine = IO_CLOSE;
																																			USER_SystemFlag.A.Flag_Ldynamo = 0;
																																			USER_SystemFlag.A.Flag_ST = 0;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'D'://�ҵ������
																													{
																																			Speed_Control = IO_OPEN;
																																			Machine_Switch = IO_CLOSE;
																																			Right_Machine = IO_OPEN;
																																			USER_SystemFlag.A.Flag_Rdynamo = 1;
																																			USER_SystemFlag.A.Flag_Switch = 0;
																																			USER_SystemFlag.A.Flag_ST = 1;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'E'://�ҵ��ֹͣ
																													{
																																			Speed_Control = IO_CLOSE;
																																			Right_Machine = IO_CLOSE;
																																			USER_SystemFlag.A.Flag_Rdynamo = 0;
																																			USER_SystemFlag.A.Flag_ST = 0;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'F'://�������л�
																													{
																																			Machine_Switch = IO_OPEN;
																																			USER_SystemFlag.A.Flag_Switch = 1;
													// 																						SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'G'://ȫ��ֹͣ
																													{
																																			Speed_Control = IO_CLOSE;
																																			Left_Machine = IO_CLOSE;
																																			Right_Machine = IO_CLOSE;
																																			Machine_Switch = IO_CLOSE;
																																			USER_SystemFlag.A.Flag_Ldynamo = 0;
																																			USER_SystemFlag.A.Flag_Rdynamo = 0;
																																			USER_SystemFlag.A.Flag_Switch = 0;
																																			USER_SystemFlag.A.Flag_ST = 0;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}				
																													case 'J'://N J  ��������
																													{
																																			if(Rx_Buffer[2] == 14)//֡��14
																																			{
																																					Left_StaticWeight = 0;
																																					Right_StaticWeight = 0;
																																					if(Rx_Buffer[6] > 0x30) Left_StaticWeight += (Rx_Buffer[6]-0x30)*10000;
																																					if(Rx_Buffer[7] > 0x30) Left_StaticWeight += (Rx_Buffer[7]-0x30)*1000;
																																					if(Rx_Buffer[8] > 0x30) Left_StaticWeight += (Rx_Buffer[8]-0x30)*100;
																																					if(Rx_Buffer[9] > 0x30) Left_StaticWeight += (Rx_Buffer[9]-0x30)*10;
																																					if(Rx_Buffer[10] > 0x30) Left_StaticWeight += Rx_Buffer[10]-0x30;
																																					if(Rx_Buffer[12] > 0x30) Right_StaticWeight += (Rx_Buffer[12]-0x30)*10000;
																																					if(Rx_Buffer[13] > 0x30) Right_StaticWeight += (Rx_Buffer[13]-0x30)*1000;
																																					if(Rx_Buffer[14] > 0x30) Right_StaticWeight += (Rx_Buffer[14]-0x30)*100;
																																					if(Rx_Buffer[15] > 0x30) Right_StaticWeight += (Rx_Buffer[15]-0x30)*10;
																																					if(Rx_Buffer[16] > 0x30) Right_StaticWeight += Rx_Buffer[16]-0x30;
																																					if(Left_StaticWeight<200)
																																					{
																																							Left_StaticWeight = 200;																															
																																							W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
																																					}
																																					if(Right_StaticWeight<200)
																																					{
																																							Right_StaticWeight = 200;
																																							W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
																																					}
								// 																													System_State = DynamoStart_State;
																																					W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																																					Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																			}
																																			else
																																			{
																																					W5500_Send_Data(s,con_TranXACK);				//������Ч
																																			}
																																			break;
																													}			
																													default:
																																		 W5500_Send_Data(s,con_TranXACK);				//������Ч
																																		 break;
																											}
																											break;
																				}
																				case 'B'://�汾��Ϣ
																				{
																											W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
																											break;
																				}
																				case 'C'://�źŲ��Խ���
																				{
																											if(Rx_Buffer[5] == 0)
																											{
																														if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
																														{
																																Signal_TransferTime = Rx_Buffer[6] - 0x30;
																																System_State = SignalTest_State;
																																WorkState_Flg = SignalTest_State_Flg;    //�źŲ���״̬
																																W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //����״̬
																													W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																											}
																											else if(Rx_Buffer[5] == 'X')
																											{
																													USER_SystemFlag.A.Flag_TestC1 = 1;
																											}
																											else
																											{
																													switch(Rx_Buffer[5])
																													{																																
																															case Left_Brake_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Right_Brake_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case All_Weight_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case FootPlat_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Left_Move_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Right_Move_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															default:
																																																	printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																	W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																																																	break;	
																													}
																											}
																											break;
																				}
																				case 'D'://ȡ���Խ��
																				{
																											switch(Rx_Buffer[5])
																											{																																																																																																					
																													case 'F'://�����ƶ�̨��ɽ��
																													{
																																				if(Rx_Buffer[6] == '1')
																																				{
																																						FunW5500_SendDF(s,1,AllNone_StaticWeight);
																																				}
																																				else if(Rx_Buffer[6] == '2')
																																				{																													
																																						FunW5500_SendDF(s,2,AllFull_StaticWeight);
																																				}
																																				else
																																				{
																																						W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
																																				}
																																				break;
																													}
																													case 'T'://����̤���������
																													{
																																				break;
																													}
																													case 'C'://����̤���������
																													{
																																				W5500_Send_Data(s,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
// 																																				FunW5500_SendDC(s);				//���͵�ǰ�����
																																				break;
																													}						
																													case 'D'://���ͼ����
																													{
																																				FunW5500_SendDC(s);				//���͵�ǰ�����
																																				break;
																													}	
																													case 'B'://�����ƶ���������
																													{
																																				
																																				
																																				break;
																													}
																													case 'J'://���ͼ����ƶ���������
																													{
																																				
																																				W5500_Send_Data(s,u8Tran_CurrentJ);				//���͵�ǰ�����ƶ���������
																																				break;
																													}
																													case 'Z'://�������͹�������
																													{
																																				
																																				W5500_Send_Data(s,u8Tran_CurrentZ);				//���͵�ǰ���͹�������
																																				break;
																													}
																													case 'L'://����ͣ�����ͺ�ͣ��ʱ��
																													{
																																				Tx_Buffer[0] = 8;  //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'D'; 
																																				Tx_Buffer[3] = 'L'; 
																																				Tx_Buffer[4] = Left_StopFlag; 
																																				Tx_Buffer[5] = Right_StopFlag;
																																				Tx_Buffer[6] = Left_StopTime.u8_data[0]; 
																																				Tx_Buffer[7] = Left_StopTime.u8_data[1]; 
																																				Tx_Buffer[8] = Right_StopTime.u8_data[0]; 
																																				Tx_Buffer[9] = Right_StopTime.u8_data[1]; 																																																										
																																				W5500_Send_Data(s,Tx_Buffer);				//
																																				break;
																													}
																													default:
																																				W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
																																				break;
																											}
																											break;
																				}
																				case 'G'://ȡʵʱ����
																				{
																											switch(Rx_Buffer[5])
																											{																					
																													case 'B':
																													{
																																				Tx_Buffer[0] = 14;  //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'G'; 
																																				Tx_Buffer[3] = 'B'; 
																																				/* ���� */
																																				Tx_Buffer[4] = Left_DynamicBrake/10000; 
																																				if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
																																				else Tx_Buffer[4] = 0x20;
																																				Tx_Buffer[5] = Left_DynamicBrake%10000/1000; 
																																				if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
																																				else Tx_Buffer[5] = 0x20;
																																				Tx_Buffer[6] = Left_DynamicBrake%1000/100; 
																																				if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
																																				else Tx_Buffer[6] = 0x20;
																																				Tx_Buffer[7] = Left_DynamicBrake%100/10; 
																																				if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
																																				else Tx_Buffer[7] = 0x20;
																																				Tx_Buffer[8] = Left_DynamicBrake%10 + 0x30; 
																																				Tx_Buffer[9] = 0x20; 
																																				/* ���� */
																																				Tx_Buffer[10] = Right_DynamicBrake/10000; 
																																				if(Tx_Buffer[10] > 0) Tx_Buffer[10] += 0x30;
																																				else Tx_Buffer[10] = 0x20;
																																				Tx_Buffer[11] = Right_DynamicBrake%10000/1000; 
																																				if(Tx_Buffer[11] > 0) Tx_Buffer[11] += 0x30;
																																				else Tx_Buffer[11] = 0x20;
																																				Tx_Buffer[12] = Right_DynamicBrake%1000/100; 
																																				if(Tx_Buffer[12] > 0) Tx_Buffer[12] += 0x30;
																																				else Tx_Buffer[12] = 0x20;
																																				Tx_Buffer[13] = Right_DynamicBrake%100/10; 
																																				if(Tx_Buffer[13] > 0) Tx_Buffer[13] += 0x30;
																																				else Tx_Buffer[13] = 0x20;
																																				Tx_Buffer[14] = Right_DynamicBrake%10 + 0x30; 
																																				Tx_Buffer[15] = 0x20; 
																																				W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ�ƶ�
																																				break;
																													}																																								
																													case 'T':
																													{
																																				Tx_Buffer[0] = 8;    //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'G'; 
																																				Tx_Buffer[3] = 'T'; 
																																				/* ̤���� */
																																				Tx_Buffer[4] = DynamicFootPlat/10000; 
																																				if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
																																				else Tx_Buffer[4] = 0x20;
																																				Tx_Buffer[5] = DynamicFootPlat%10000/1000; 
																																				if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
																																				else Tx_Buffer[5] = 0x20;
																																				Tx_Buffer[6] = DynamicFootPlat%1000/100; 
																																				if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
																																				else Tx_Buffer[6] = 0x20;
																																				Tx_Buffer[7] = DynamicFootPlat%100/10; 
																																				if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
																																				else Tx_Buffer[7] = 0x20;
																																				Tx_Buffer[8] = DynamicFootPlat%10 + 0x30; 
																																				Tx_Buffer[9] = 0x20; 
																																				W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ̤����
																																				break;
																													}						
																													case 'F':
																													{
																																				Tx_Buffer[0] = 8;    //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'G'; 
																																				Tx_Buffer[3] = 'F'; 
																																				if(SysParameter.Par6_Weight == 2)
																																				 {
																																						 Quotient = All_DynamicWeight / 2;
																																						 Remainder = All_DynamicWeight % 2;
																																						 if(Remainder > 0)
																																						 {
																																								All_DynamicWeight = (Quotient +1) * 2;																										 
																																						 }																									
																																				 }
																																				 else if(SysParameter.Par6_Weight == 5)
																																				 {
																																						Quotient = All_DynamicWeight / 5;
																																						 Remainder = All_DynamicWeight % 5;
																																						 if(Remainder < 3)
																																						 {
																																								All_DynamicWeight = Quotient * 5;																										 
																																						 }
																																						 else
																																						 {
																																								All_DynamicWeight = (Quotient +1) * 5;
																																						 }																									
																																				 }
																																				/* ���ϳ��� */
																																				Tx_Buffer[4] = All_DynamicWeight/10000; 
																																				if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
																																				else Tx_Buffer[4] = 0x20;
																																				Tx_Buffer[5] = All_DynamicWeight%10000/1000; 
																																				if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
																																				else Tx_Buffer[5] = 0x20;
																																				Tx_Buffer[6] = All_DynamicWeight%1000/100; 
																																				if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
																																				else Tx_Buffer[6] = 0x20;
																																				Tx_Buffer[7] = All_DynamicWeight%100/10; 
																																				if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
																																				else Tx_Buffer[7] = 0x20;
																																				Tx_Buffer[8] = All_DynamicWeight%10 + 0x30; 
																																				Tx_Buffer[9] = 0x20; 
																																				W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ��������
																																				break;
																													}	
																													default :
																																			 W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
																																			 break;
																											}
																											break;
																				}
																				case 'A': //�궨״̬
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'C'://����궨״̬
																																				{				
																																						System_State = Adjust_State;
																																						WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																						switch(Rx_Buffer[6])
																																						{																																
																																								case Left_Brake_ID:
																																																										AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Right_Brake_ID:
																																																										AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case All_Weight_ID:
																																																										AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case FootPlat_ID:
																																																										AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Left_Move_ID:
																																																										AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Right_Move_ID:
																																																										AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																																																										break;	
																																						}
																																						break;
																																				}
																													case 'E'://�˳��궨״̬
																																				{
																																						System_State = Default_State;
																																						WorkState_Flg = Default_State_Flg;    //����״̬
																																						W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						break;
																																				}
																													case 'W'://����궨ϵ��
																																				{
																																						for(i=0;i<18;i++)
																																						{
																																								u8flash_buffer[i] = Rx_Buffer[i+7];																			
																																						}
																																						switch(Rx_Buffer[6])
																																						{																																
																																								case Left_Brake_ID:
																																																										CabItemIndex = 2;
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
																																																									CabBuf[2] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Left_Brake_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Brake_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Brake_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Brake_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Brake_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Brake_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Left_Brake_par.Load_Flash,Left_Brake_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Left_Brake_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Brake_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��																																
																																																										break;
																																								case Right_Brake_ID:
																																																										CabItemIndex = 3;
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
																																																									CabBuf[3] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Right_Brake_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Brake_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Brake_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Brake_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Brake_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Brake_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Right_Brake_par.Load_Flash,Right_Brake_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Right_Brake_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Brake_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��																																																																																	
								// 																																																		printf("NB=%u,C0=%f,c1=%f,C2=%f,c3=%f",Right_Brake_par.Load_Parameter.Parameter_NB,
								// 																																																																					Right_Brake_par.Load_Parameter.Parameter_C0.float_data,
								// 																																																																					Right_Brake_par.Load_Parameter.Parameter_C1.float_data,
								// 																																																																					Right_Brake_par.Load_Parameter.Parameter_C2.float_data,
								// 																																																																					Right_Brake_par.Load_Parameter.Parameter_C3.float_data);
																																																										break;
																																								case All_Weight_ID:
																																																										CabItemIndex = 4;
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
																																																									CabBuf[4] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									All_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									All_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									All_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									All_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									All_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(All_Weight_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(All_Weight_par.Load_Flash,All_Weight_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(All_Weight_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,All_Weight_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case FootPlat_ID:
																																																										CabItemIndex = 5;
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
																																																									CabBuf[5] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									FootPlat_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									FootPlat_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									FootPlat_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									FootPlat_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									FootPlat_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(FootPlat_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(FootPlat_par.Load_Flash,FootPlat_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(FootPlat_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,FootPlat_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Left_Move_ID:
																																																										CabItemIndex = 6;
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
																																																									CabBuf[6] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Left_Move_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Move_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Move_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Move_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Move_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Move_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Left_Move_par.Load_Flash,Left_Move_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Left_Move_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Move_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Right_Move_ID:
																																																											CabItemIndex = 7;
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
																																																									CabBuf[7] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Right_Move_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Move_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Move_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Move_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Move_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Move_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Right_Move_par.Load_Flash,Right_Move_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Right_Move_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Move_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																									break;
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!xie\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																																																										break;
																																						}
																																						break;
																																				}
																													case 'R'://���궨ϵ��
																																				{
																																						u8read_flg = 1;//���궨ϵ���ɹ�
																																						switch(Rx_Buffer[6])
																																						{																																
																																								case Left_Brake_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Brake_Flash_Address1,18);
																																																										break;
																																								case Right_Brake_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Brake_Flash_Address1,18);
																																																										break;
																																								case All_Weight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,All_Weight_Flash_Address1,18);
																																																										break;
																																								case FootPlat_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,FootPlat_Flash_Address1,18);
																																																										break;
																																								case Left_Move_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Move_Flash_Address1,18);
																																																										break;
																																								case Right_Move_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Move_Flash_Address1,18);
																																																										break;
																																								default:
																																																										u8read_flg = 0;//���궨ϵ��ʧ��
																																																										printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
																																																										break;
																																						}			
																																						if(u8read_flg == 1)//���궨ϵ���ɹ�
																																						{
																																								Tx_Buffer[0] = 21;//֡���� ��λ
																																								Tx_Buffer[1] = 0;  //֡���� ��λ
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
																																								W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																																						}
																																						break;
																																				}
																													 default :
																																			 W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																																			 break;
																											}
																											break;
																				}
																				case 'Y': //����  
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //ǿ������
                                                      {
                                                          Fun_Zero(Left_Brake_ID,1);
                                                          Fun_Zero(Right_Brake_ID,1);
                                                          Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                      }
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																											}
																											break;
																				}
																				case 'E'://������д
																				{
																											if(Rx_Buffer[5] == '1')
																											{
																														if(Rx_Buffer[6] == 'W')
																														{																							
																																for(i=0;i<(Rx_Buffer[7]*3+1);i++) //���������ݴ洢����ʱ����u8flash_buffer
																																{
																																		 Parameter_Buffer[i] = Rx_Buffer[i+7];
																																}								
																																if(Fun_DealParameter()==1)
																																{
																																		SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//��������
																																		SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(Rx_Buffer[7]*3+1));//����ϵ��		
																																		W5500_Send_Data(s,con_TranE1W);//��ȷӦ��
																																}
																																else
																																{
																																		W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��
																																		printf("\r\nError:Write Parameter false!!!E1W\r\n");
																																}
																														}
																														else if(Rx_Buffer[6] == 'R')
																														{												
																																	SPI_FLASH_BufferRead(Parameter_Buffer,ParameterAll_Flash_Address,100);
																																	Tx_Buffer[0] = Parameter_Buffer[0]*3+4;  //֡���� ��λ
																																	Tx_Buffer[1] = 0;  //֡���� ��λ
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
																																W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��	
																																printf("\r\nError:The command E1 is false!!!E1\r\n");
																														}
																											}
																											else if(Rx_Buffer[5] == '2')
																											{
																													if(Rx_Buffer[6] == 'W')
																													{
																															for(i=0;i<24;i++) //���������ݴ洢����ʱ����Parameter_Buffer
																															{
																																	 Parameter_Buffer[i] = Rx_Buffer[i+7];
																															}	
																															Parameter_Buffer[24] = 0x55;
																															SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//��������
																															SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,25);//����ϵ��		
																															W5500_Send_Data(s,con_TranE2W);//��ȷӦ��
																													}
																													else if(Rx_Buffer[6] == 'R')
																													{
																															SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
																															Tx_Buffer[0] = 27;  //֡���� ��λ
																															Tx_Buffer[1] = 0;    //֡���� ��λ
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
																															W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��	
																															printf("\r\nError:The command E2 is false!!!E2\r\n");
																													}
																											}
																											else if(Rx_Buffer[5] == 'I') //IP�洢
																											{
																														if(Rx_Buffer[6] == 'W')
																														{
																																for(i=0;i<26;i++) //���������ݴ洢����ʱ����Parameter_Buffer
																																{
																																		 W5500_Parameter[i] = Rx_Buffer[i+7];
																																}	
																																W5500_Parameter[26] = 0x55;
																																SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//��������
																																SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//����ϵ��		
																																W5500_Send_Data(s,con_TranEIW);//��ȷӦ��
																																ucW5500_InitCTR = 0;
																																S0_State = 0;
																																S1_State = 0;
																																S2_State = 0;																														    
																																Load_Net_Parameters();
																														}
																														else if(Rx_Buffer[6] == 'R')
																														{
																																SPI_FLASH_BufferRead(Parameter_Buffer,ParameterIP_Flash_Address,26);
																																Tx_Buffer[0] = 29;  //֡���� ��λ
																																Tx_Buffer[1] = 0;    //֡���� ��λ
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
																																W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��	
																																printf("\r\nError:The command EII is false!!!EI\r\n");
																														}
																											}
																											else
																											{
																													 W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��	
																													 printf("\r\nError:The command E is false!!!E\r\n");
																											}
																											break;
																				}
																				case 'T':
																				{
																											switch(Rx_Buffer[5])
																											{																
																													case 'B': //�����ƶ�����
																													{
																																				if(Rx_Buffer[2] == 2)       //ֻ��TB ������ ��D
																																				{
																																						
																																						 System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_None_State_Flg;
																																						 Left_StaticWeight = StaticWeight_Init;
																																						 Right_StaticWeight = StaticWeight_Init;  //Ĭ�Ͼ�̬����
																																						 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else if(Rx_Buffer[2] == 3)   //ֻ��TB ������ ��D
																																				{
																																						System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_None_State_Flg;
																																						 Left_StaticWeight = StaticWeight_Init;
																																						 Right_StaticWeight = StaticWeight_Init;  //Ĭ�Ͼ�̬����
																																					   if(Rx_Buffer[6] == 'D')
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																						 }
																																						 else
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 }
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else if(Rx_Buffer[2] == 6)      //ֻ��TB ������ ��D
																																				{
																																						 System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_None_State_Flg;
																																						 Left_StaticWeight = (Rx_Buffer[7]<<8)|Rx_Buffer[6];
																																						 Right_StaticWeight = (Rx_Buffer[9]<<8)|Rx_Buffer[8];  //Ĭ�Ͼ�̬����
																																						 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else if(Rx_Buffer[2] == 7)     //ֻ��TB ������ ��D
																																				{
																																						 System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_None_State_Flg;
																																						 Left_StaticWeight = (Rx_Buffer[7]<<8)|Rx_Buffer[6];
																																						 Right_StaticWeight = (Rx_Buffer[9]<<8)|Rx_Buffer[8];  //Ĭ�Ͼ�̬����
																																					   if(Rx_Buffer[10] == 'D')
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																						 }
																																						 else
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 }
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else
																																				{
																																						 W5500_Send_Data(s,con_TranXACK);//��Ч����
																																				}					
																																				if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
																																				{
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				}																																				
																																				break;
																													}
																													case 'J': //�����ƶ�����
																													{
																																				if(Rx_Buffer[2] == 2)       //ֻ��TB ������ ��D
																																				{																																							
																																						 System_State = BrakeTest_Full_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_Full_State_Flg;
																																						 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else if(Rx_Buffer[2] == 3)   //ֻ��TB ������ ��D
																																				{
																																						 
																																						 System_State = BrakeTest_Full_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_Full_State_Flg;
																																						 if(Rx_Buffer[6] == 'D')
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																						 }
																																						 else
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 }
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}																																					
																																				else
																																				{
																																						 W5500_Send_Data(s,con_TranXACK);//��Ч����
																																				}								
																																				if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
																																				{
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				}
																																				break;
																													}																											
																													case 'Z':  //���������
																													{
																																				 if((Rx_Buffer[6] == 0x31) || (Rx_Buffer[6] == 0x32))
																																				 {
																																						  if(Rx_Buffer[2] == 3)       //ֻ��TZ ������ ��D
																																							{
																																									 System_State = Retardant_State; 					//ϵͳ�������Ͳ���
																																									 SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
																																									 W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
																																									 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									 WorkState_Flg = Retardant_State_Flg;
																																									 Left_StaticWeight = StaticWeight_Init;
																																									 Right_StaticWeight = StaticWeight_Init;  //Ĭ�Ͼ�̬����
																																									 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 4)   //ֻ��TZ ������ ��D
																																							{
																																									 System_State = Retardant_State; 					//ϵͳ�������Ͳ���
																																									 SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
																																									 W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
																																									 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									 WorkState_Flg = Retardant_State_Flg;
																																									 Left_StaticWeight = StaticWeight_Init;
																																									 Right_StaticWeight = StaticWeight_Init;  //Ĭ�Ͼ�̬����
																																									 if(Rx_Buffer[7] == 'D')
																																									 {
																																												USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									 }
																																									 else
																																									 {
																																												USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									 }
																																									 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 7)      //ֻ��TZ ������ ��D
																																							{
																																									 System_State = Retardant_State; 					//ϵͳ�������Ͳ���
																																									 SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
																																									 W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
																																									 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									 WorkState_Flg = Retardant_State_Flg;
																																									 Left_StaticWeight = (Rx_Buffer[8]<<8)|Rx_Buffer[7];
																																									 Right_StaticWeight = (Rx_Buffer[10]<<8)|Rx_Buffer[9];  //Ĭ�Ͼ�̬����
																																									 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 8)     //ֻ��TZ ������ ��D
																																							{
																																									 System_State = Retardant_State; 					//ϵͳ�������Ͳ���
																																									 SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
																																									 W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
																																									 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									 WorkState_Flg = Retardant_State_Flg;
																																									 Left_StaticWeight = (Rx_Buffer[8]<<8)|Rx_Buffer[7];
																																									 Right_StaticWeight = (Rx_Buffer[10]<<8)|Rx_Buffer[9];  //Ĭ�Ͼ�̬����
																																									 if(Rx_Buffer[11] == 'D')
																																									 {
																																												USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									 }
																																									 else
																																									 {
																																												USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									 }
																																									 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else
																																							{
																																									 W5500_Send_Data(s,con_TranXACK);//��Ч����
																																							}																																								 
																																				 }
																																				 else
																																				 {	
																																							W5500_Send_Data(s,con_TranXACK);	//��Чָ֡��
																																				 }
																																				 if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
																																				 {
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																													case 'D':  //�������
																													{
																																				if(Rx_Buffer[2] == 14)//֡��14
																																				{
																																						Left_StaticWeight = 0;
																																						Right_StaticWeight = 0;
													// 																									if(Rx_Buffer[6] > 0x30) Left_StaticWeight += (Rx_Buffer[6]-0x30)*10000;
																																						if(Rx_Buffer[7] > 0x30) Left_StaticWeight += (Rx_Buffer[7]-0x30)*1000;
																																						if(Rx_Buffer[8] > 0x30) Left_StaticWeight += (Rx_Buffer[8]-0x30)*100;
																																						if(Rx_Buffer[9] > 0x30) Left_StaticWeight += (Rx_Buffer[9]-0x30)*10;
																																						if(Rx_Buffer[10] > 0x30) Left_StaticWeight += Rx_Buffer[10]-0x30;
													// 																									if(Rx_Buffer[12] > 0x30) Right_StaticWeight += (Rx_Buffer[12]-0x30)*10000;
																																						if(Rx_Buffer[13] > 0x30) Right_StaticWeight += (Rx_Buffer[13]-0x30)*1000;
																																						if(Rx_Buffer[14] > 0x30) Right_StaticWeight += (Rx_Buffer[14]-0x30)*100;
																																						if(Rx_Buffer[15] > 0x30) Right_StaticWeight += (Rx_Buffer[15]-0x30)*10;
																																						if(Rx_Buffer[16] > 0x30) Right_StaticWeight += Rx_Buffer[16]-0x30;
																																						if(Left_StaticWeight<200)
																																						{
																																								Left_StaticWeight = 200;
																																								W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
																																						}
																																						if(Right_StaticWeight<200)
																																						{
																																								Right_StaticWeight = 200;
																																								W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
																																						}
																																						System_State = DynamoStart_State;
																																						W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																																						Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																				}
																																				else
																																				{
																																						W5500_Send_Data(s,con_TranXACK);				//������Ч
																																				}
																																				break;
																													}
																													case 'F':		//��ɲ���
																													{	
																																				 if(Rx_Buffer[6] == '1') //���ظ��ϳ���
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									System_State = StaticWeight_None_State;   //ϵͳ������ظ��ϳ���
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									WorkState_Flg = StaticWeight_None_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(USER_LEDFlag.A.LED_Down != 1)
																																									{
																																											Pump_UpDown = IO_OPEN;
																																											USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																											USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																											Delay_ms(2000);
																																									}   
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									System_State = StaticWeight_None_State;   //ϵͳ������ظ��ϳ���
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									WorkState_Flg = StaticWeight_None_State_Flg;
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									}
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(USER_LEDFlag.A.LED_Down != 1)
																																									{
																																											Pump_UpDown = IO_OPEN;
																																											USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																											USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																											Delay_ms(2000);
																																									}  
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//ָ����Ч
																																							}
																																				 }
																																				 else if(Rx_Buffer[6] == '2')//���ؾ���
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									System_State = StaticWeight_Full_State;  //ϵͳ������ؾ���״̬
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									WorkState_Flg = StaticWeight_Full_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(USER_LEDFlag.A.LED_Down != 1)
																																									{
																																											Pump_UpDown = IO_OPEN;
																																											USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																											USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																											Delay_ms(2000);
																																									}  
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									System_State = StaticWeight_Full_State;  //ϵͳ������ؾ���״̬
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									WorkState_Flg = StaticWeight_Full_State_Flg;
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									}
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(USER_LEDFlag.A.LED_Down != 1)
																																									{
																																											Pump_UpDown = IO_OPEN;
																																											USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																											USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																											Delay_ms(2000);
																																									}  
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//ָ����Ч
																																							}																																						
																																				 }
																																				 else
																																				 {
																																						W5500_Send_Data(s,con_TranXACK);//ָ����Ч
																																				 }																
																																				 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																				 if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
																																				 {
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																											}
																											break;
																				}
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															 }	
															 else if(WorkState_Flg == Adjust_State_Flg)  //�궨״̬
															 {
																		switch(u8_Command)
																		{	
																				case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
																													Tx_Buffer[0] = 12;  //֡���� ��λ
																													Tx_Buffer[1] = 0;  //֡���� ��λ
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//�ش�
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
																															if(cCheck_Result == 0)  //������ȷ
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //�����������ɹ�
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://�Լ�
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
																				case 'O'://����ָ��
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
																				case 'S'://ȡ״̬
																				{
																											switch(WorkState_Flg)
																											{
																														case Default_State_Flg:
																														{
																																																con_TranS[3] = 'S';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Adjust_State_Flg:
																														{
																																																con_TranS[3] = 'J';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case SignalTest_State_Flg:
																														{
																																																con_TranS[3] = 'C';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_None_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_Full_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Retardant_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '3';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_None_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '4';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_Full_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '5';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_Motor_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '6';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														default :
																																				W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
																																				break;
																														
																											}
																											break;
																				}												
																				case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
																				{
                                                      TIM2_count = 0;//40S�жϹرյ�ŷ���Դ
                                                      Pump_UpDown = IO_CLOSE; //������
                                                      USER_LEDFlag.A.LED_Down = 0;         //������
                                                      USER_LEDFlag.A.LED_Up = 1;           //������
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;			
																											USER_SystemFlag.A.Flag_Ldynamo = 0;
																											USER_SystemFlag.A.Flag_Rdynamo = 0;
																											USER_SystemFlag.A.Flag_Switch = 0;
																											USER_SystemFlag.A.Flag_ST = 0;													
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
                                                      USER_LEDFlag.A.LED_TJUp = 0;         //̨�ܾ�
                                                      USER_LEDFlag.A.LED_TJDown = 1;       //̨�ܽ�
                                          
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״̬
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											W5500_Send_Data(s,con_TranR);				   //��λ���
																											return;
																				}										
																				case 'B'://�汾��Ϣ
																				{
																											W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
																											break;
																				}					
																				case 'C'://�źŲ��Խ���
																				{
																											if(Rx_Buffer[5] == 0)
																											{
																														if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
																														{
																																Signal_TransferTime = Rx_Buffer[6] - 0x30;
																																System_State = SignalTest_State;
																																WorkState_Flg = SignalTest_State_Flg;    //�źŲ���״̬
																																W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //����״̬
																													W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																											}
																											else
																											{
																													switch(Rx_Buffer[5])
																													{																																
																															case Left_Brake_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Right_Brake_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case All_Weight_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case FootPlat_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Left_Move_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Right_Move_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															default:
																																																	printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																	W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																																																	break;	
																													}
																											}
																											break;
																				}
																				case 'A': //�궨״̬
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'C'://����궨״̬
																																				{				
																																						System_State = Adjust_State;
																																						WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																						switch(Rx_Buffer[6])
																																						{																																
																																								case Left_Brake_ID:
																																																										AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Right_Brake_ID:
																																																										AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case All_Weight_ID:
																																																										AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case FootPlat_ID:
																																																										AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Left_Move_ID:
																																																										AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Right_Move_ID:
																																																										AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																																																										break;	
																																						}
																																						break;
																																				}
																													case 'E'://�˳��궨״̬
																																				{
																																						System_State = Default_State;
																																						WorkState_Flg = Default_State_Flg;    //����״̬
																																						W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						break;
																																				}
																													case 'W'://����궨ϵ��
																																				{
																																						for(i=0;i<18;i++)
																																						{
																																								u8flash_buffer[i] = Rx_Buffer[i+7];																			
																																						}
																																						switch(Rx_Buffer[6])
																																						{																																
																																								case Left_Brake_ID:
																																																										CabItemIndex = 2;
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
																																																									CabBuf[2] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Left_Brake_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Brake_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Brake_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Brake_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Brake_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Brake_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Left_Brake_par.Load_Flash,Left_Brake_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Left_Brake_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Brake_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��																																
																																																										break;
																																								case Right_Brake_ID:
																																																										CabItemIndex = 3;
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
																																																									CabBuf[3] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Right_Brake_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Brake_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Brake_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Brake_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Brake_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Brake_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Right_Brake_par.Load_Flash,Right_Brake_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Right_Brake_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Brake_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��																																																																																	
								// 																																																		printf("NB=%u,C0=%f,c1=%f,C2=%f,c3=%f",Right_Brake_par.Load_Parameter.Parameter_NB,
								// 																																																																					Right_Brake_par.Load_Parameter.Parameter_C0.float_data,
								// 																																																																					Right_Brake_par.Load_Parameter.Parameter_C1.float_data,
								// 																																																																					Right_Brake_par.Load_Parameter.Parameter_C2.float_data,
								// 																																																																					Right_Brake_par.Load_Parameter.Parameter_C3.float_data);
																																																										break;
																																								case All_Weight_ID:
																																																										CabItemIndex = 4;
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
																																																									CabBuf[4] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									All_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									All_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									All_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									All_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									All_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(All_Weight_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(All_Weight_par.Load_Flash,All_Weight_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(All_Weight_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,All_Weight_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case FootPlat_ID:
																																																										CabItemIndex = 5;
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
																																																									CabBuf[5] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									FootPlat_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									FootPlat_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									FootPlat_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									FootPlat_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									FootPlat_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(FootPlat_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(FootPlat_par.Load_Flash,FootPlat_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(FootPlat_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,FootPlat_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Left_Move_ID:
																																																										CabItemIndex = 6;
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
																																																									CabBuf[6] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Left_Move_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Move_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Move_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Move_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Move_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Move_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Left_Move_par.Load_Flash,Left_Move_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Left_Move_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Move_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Right_Move_ID:
																																																											CabItemIndex = 7;
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
																																																									CabBuf[7] = &_ADCabVariable;
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Right_Move_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Move_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Move_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Move_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Move_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Move_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Right_Move_par.Load_Flash,Right_Move_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Right_Move_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Move_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																									break;
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!xie\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																																																										break;
																																						}
																																						break;
																																				}
																													case 'R'://���궨ϵ��
																																				{
																																						u8read_flg = 1;//���궨ϵ���ɹ�
																																						switch(Rx_Buffer[6])
																																						{																																
																																								case Left_Brake_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Brake_Flash_Address1,18);
																																																										break;
																																								case Right_Brake_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Brake_Flash_Address1,18);
																																																										break;
																																								case All_Weight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,All_Weight_Flash_Address1,18);
																																																										break;
																																								case FootPlat_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,FootPlat_Flash_Address1,18);
																																																										break;
																																								case Left_Move_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Move_Flash_Address1,18);
																																																										break;
																																								case Right_Move_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Move_Flash_Address1,18);
																																																										break;
																																								default:
																																																										u8read_flg = 0;//���궨ϵ��ʧ��
																																																										printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
																																																										break;
																																						}			
																																						if(u8read_flg == 1)//���궨ϵ���ɹ�
																																						{
																																								Tx_Buffer[0] = 21;//֡���� ��λ
																																								Tx_Buffer[1] = 0;  //֡���� ��λ
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
																																								W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																																						}
																																						break;
																																				}
																													 default :
																																			 W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																																			 break;
																											}
																											break;
																				}
																				case 'Y': //����
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //ǿ������
                                                      {
                                                          Fun_Zero(Left_Brake_ID,1);
                                                          Fun_Zero(Right_Brake_ID,1);
                                                          Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                      }
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																											}
																											break;
																				}
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															 }
															 else if(WorkState_Flg == SignalTest_State_Flg) //�źŲ���״̬
															 {
																		switch(u8_Command)
																		{		
																				case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
																													Tx_Buffer[0] = 12;  //֡���� ��λ
																													Tx_Buffer[1] = 0;  //֡���� ��λ
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//�ش�
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
																															if(cCheck_Result == 0)  //������ȷ
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //�����������ɹ�
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://�Լ�
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
																				case 'O'://����ָ��
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
																				case 'S'://ȡ״̬
																				{
																											switch(WorkState_Flg)
																											{
																														case Default_State_Flg:
																														{
																																																con_TranS[3] = 'S';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Adjust_State_Flg:
																														{
																																																con_TranS[3] = 'J';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case SignalTest_State_Flg:
																														{
																																																con_TranS[3] = 'C';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_None_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_Full_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Retardant_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '3';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_None_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '4';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_Full_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '5';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_Motor_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '6';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														default :
																																				W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
																																				break;
																														
																											}
																											break;
																				}												
																				case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
																				{
                                                      TIM2_count = 0;//40S�жϹرյ�ŷ���Դ
                                                      Pump_UpDown = IO_CLOSE; //������
                                                      USER_LEDFlag.A.LED_Down = 0;         //������
                                                      USER_LEDFlag.A.LED_Up = 1;           //������
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;			
																											USER_SystemFlag.A.Flag_Ldynamo = 0;
																											USER_SystemFlag.A.Flag_Rdynamo = 0;
																											USER_SystemFlag.A.Flag_Switch = 0;
																											USER_SystemFlag.A.Flag_ST = 0;
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
                                                      USER_LEDFlag.A.LED_TJUp = 0;         //̨�ܾ�
                                                      USER_LEDFlag.A.LED_TJDown = 1;       //̨�ܽ�
                                          
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״̬
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											W5500_Send_Data(s,con_TranR);				   //��λ���
																											return;
																				}												
																				case 'N'://�������ƺ�֪ͨ
																				{
																											switch(Rx_Buffer[5])
																											{
																													case '0'://ǰ�н��н�
																													{
																																			Pump_Before = IO_OPEN;	
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '1'://ǰ�н��ɿ�
																													{
																																			Pump_Before = IO_CLOSE;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '2'://��н��н�
																													{
																																			Pump_Behind = IO_OPEN;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '3'://��н��ɿ�
																													{
																																			Pump_Behind = IO_CLOSE;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '4'://���׾�
																													{
																																			Pump_UpDown = IO_CLOSE;
																																			USER_LEDFlag.A.LED_Up = 1;  				//������ָʾ��
																																			USER_LEDFlag.A.LED_Down = 0;    //������ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '5'://���׽�
																													{
																																			Pump_UpDown = IO_OPEN;
																																			USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																			USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '6'://̨�ܾ�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Left_Up = IO_OPEN;
																																			Right_Up = IO_OPEN;
																																			Left_Down = IO_CLOSE;
																																			Right_Down = IO_CLOSE;
																																			Protect_Overflow = IO_OPEN;
																																			USER_LEDFlag.A.LED_TJUp = 1;  				//̨�ܾ�ָʾ��
																																			USER_LEDFlag.A.LED_TJDown = 0;    //̨�ܽ�ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '7'://̨�ܽ�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Left_Up = IO_CLOSE;
																																			Right_Up = IO_CLOSE;
																																			Left_Down = IO_OPEN;
																																			Right_Down = IO_OPEN;
																																			Protect_Overflow = IO_OPEN;
																																			USER_LEDFlag.A.LED_TJUp = 0;  				//̨�ܾ�ָʾ��
																																			USER_LEDFlag.A.LED_TJDown = 1;    //̨�ܽ�ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'I'://̨�ܼн�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Clamp_All = IO_OPEN;
																																			Protect_Overflow = IO_OPEN;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '8'://�����ƶ���  Э��ʱ��
																													{
																																			
																																			break;
																													}
																													case '9'://���������� �ƶ���  �ƶ���ƽ����
																													{
																																			
																																			break;
																													}
																													case 'A':
																													{
																																			
																																			break;
																													}
																													case 'B'://��������
																													{
																																			Speed_Control = IO_OPEN;
																																			Machine_Switch = IO_CLOSE;
																																			Left_Machine = IO_OPEN;
																																			USER_SystemFlag.A.Flag_Ldynamo = 1;
																																			USER_SystemFlag.A.Flag_Switch = 0;
																																			USER_SystemFlag.A.Flag_ST = 1;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'C'://����ֹͣ
																													{
																																			Speed_Control = IO_CLOSE;
																																			Left_Machine = IO_CLOSE;
																																			USER_SystemFlag.A.Flag_Ldynamo = 0;
																																			USER_SystemFlag.A.Flag_ST = 0;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'D'://�ҵ������
																													{
																																			Speed_Control = IO_OPEN;
																																			Machine_Switch = IO_CLOSE;
																																			Right_Machine = IO_OPEN;
																																			USER_SystemFlag.A.Flag_Rdynamo = 1;
																																			USER_SystemFlag.A.Flag_Switch = 0;
																																			USER_SystemFlag.A.Flag_ST = 1;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'E'://�ҵ��ֹͣ
																													{
																																			Speed_Control = IO_CLOSE;
																																			Right_Machine = IO_CLOSE;
																																			USER_SystemFlag.A.Flag_Rdynamo = 0;
																																			USER_SystemFlag.A.Flag_ST = 0;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'F'://�������л�
																													{
																																			Machine_Switch = IO_OPEN;
																																			USER_SystemFlag.A.Flag_Switch = 1;
													// 																						SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'G'://ȫ��ֹͣ
																													{
																																			Left_Machine = IO_CLOSE;
																																			Right_Machine = IO_CLOSE;
																																			Machine_Switch = IO_CLOSE;
																																			Speed_Control = IO_CLOSE;
																																			USER_SystemFlag.A.Flag_Ldynamo = 0;
																																			USER_SystemFlag.A.Flag_Rdynamo = 0;
																																			USER_SystemFlag.A.Flag_Switch = 0;
																																			USER_SystemFlag.A.Flag_ST = 0;
																																			SysParameter.DynamoStart_Flag = 0;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}																
																													default:
																																		 W5500_Send_Data(s,con_TranXACK);				//������Ч
																																		 break;
																											}
																											break;
																				}
																				case 'B'://�汾��Ϣ
																				{
																											W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
																											break;
																				}
																				case 'C'://�źŲ��Խ���
																				{
																											if(Rx_Buffer[5] == 'B')
																											{
																														if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
																														{
																																Signal_TransferTime = Rx_Buffer[6] - 0x30;
																																System_State = SignalTest_State;
																																WorkState_Flg = SignalTest_State_Flg;    //�źŲ���״̬
																																W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //����״̬
																													W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																											}
																											else
																											{
																													switch(Rx_Buffer[5])
																													{																																
																															case Left_Brake_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Left_Brake_ID;	//��Ӧģ��ͨ��3ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Right_Brake_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Right_Brake_ID;	//��Ӧģ��ͨ��4ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case All_Weight_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = All_Weight_ID;	//��Ӧģ��ͨ��5ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case FootPlat_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = FootPlat_ID;	//��Ӧģ��ͨ��6ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Left_Move_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Left_Move_ID;	//��Ӧģ��ͨ��7ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															case Right_Move_ID:
																																																	System_State = Adjust_State;
																																																	WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																	AD_Channel_ID = Right_Move_ID;	//��Ӧģ��ͨ��8ͨ��
																																																	W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																	break;
																															default:
																																																	printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																	W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																																																	break;	
																													}
																											}
																											break;
																				}											
																				case 'Y': //����
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //ǿ������
                                                      {
                                                          Fun_Zero(Left_Brake_ID,1);
                                                          Fun_Zero(Right_Brake_ID,1);
                                                          Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
                                                      }

																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																											}
																											break;
																				}
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															 }
															 else  //���״̬
															 {
																		switch(u8_Command)
																		{				
																				case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //��ȡҪ�����ַ�
																													Tx_Buffer[0] = 12;  //֡���� ��λ
																													Tx_Buffer[1] = 0;  //֡���� ��λ
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//�ش�
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES���ܲ�У��
																															if(cCheck_Result == 0)  //������ȷ
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//�ش��ɹ�
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //�����������ɹ�
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//�ش�ʧ��
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//�ش����ܳ�����
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://�Լ�
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
																				case 'O'://����ָ��
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
																				case 'S'://ȡ״̬
																				{
																											switch(WorkState_Flg)
																											{
																														case Default_State_Flg:
																														{
																																																con_TranS[3] = 'S';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Adjust_State_Flg:
																														{
																																																con_TranS[3] = 'J';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case SignalTest_State_Flg:
																														{
																																																con_TranS[3] = 'C';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_None_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '1';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case StaticWeight_Full_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '2';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case Retardant_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '3';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_None_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '4';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_Full_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '5';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														case BrakeTest_Motor_State_Flg:
																														{
																																																con_TranS[3] = 'T';
																																																con_TranS[4] = '6';
																																																W5500_Send_Data(s,con_TranS);		
																																																break;
																														}
																														default :
																																				W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
																																				break;
																														
																											}
																											break;
																				}
																				case 'U'://ǿ����βָ�� �ص�����״̬
																				{
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;	
																											USER_SystemFlag.A.Flag_Ldynamo = 0;
																											USER_SystemFlag.A.Flag_Rdynamo = 0;
																											USER_SystemFlag.A.Flag_Switch = 0;
																											USER_SystemFlag.A.Flag_ST = 0;
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
																											Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״̬
																											W5500_Send_Data(s,con_TranU);				//����
																											return;
																				}
																				case 'I'://��ʼ��ָ��
																				{																		
																											if(Rx_Buffer[5]  == 'B')
																											{
																													W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																													for(i=0;i<802;i++)
																													{
																															u16Left_Data[i] = 0;
																															u16Right_Data[i] = 0;
																													}
																													for(i=0;i<3250;i++)
																													{
																															u8Tran_CurrentZ[i] = 0;
																															u8Tran_CurrentC[i] = 0;
																															u8Tran_CurrentJ[i] = 0;
																															u8Tran_CurrentN[i] = 0;
																													}
																													Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																													W5500_Send_Data(s,con_TranI);				//��ȷӦ��
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
																											}
																											break;
																				}
																				case 'N'://�������ƺ�֪ͨ
																				{
																											switch(Rx_Buffer[5])
																											{
																													case '0'://ǰ�н��н�
																													{
																																			Pump_Before = IO_OPEN;	
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '1'://ǰ�н��ɿ�
																													{
																																			Pump_Before = IO_CLOSE;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '2'://��н��н�
																													{
																																			Pump_Behind = IO_OPEN;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '3'://��н��ɿ�
																													{
																																			Pump_Behind = IO_CLOSE;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '4'://���׾�
																													{
																																			Pump_UpDown = IO_CLOSE;
																																			USER_LEDFlag.A.LED_Up = 1;  				//������ָʾ��
																																			USER_LEDFlag.A.LED_Down = 0;    //������ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '5'://���׽�
																													{
																																			Pump_UpDown = IO_OPEN;
																																			USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																			USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '6'://̨�ܾ�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Left_Up = IO_OPEN;
																																			Right_Up = IO_OPEN;
																																			Left_Down = IO_CLOSE;
																																			Right_Down = IO_CLOSE;
																																			Protect_Overflow = IO_OPEN;
																																			USER_LEDFlag.A.LED_TJUp = 1;  				//̨�ܾ�ָʾ��
																																			USER_LEDFlag.A.LED_TJDown = 0;    //̨�ܽ�ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case '7'://̨�ܽ�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Left_Up = IO_CLOSE;
																																			Right_Up = IO_CLOSE;
																																			Left_Down = IO_OPEN;
																																			Right_Down = IO_OPEN;
																																			Protect_Overflow = IO_OPEN;
																																			USER_LEDFlag.A.LED_TJUp = 0;  				//̨�ܾ�ָʾ��
																																			USER_LEDFlag.A.LED_TJDown = 1;    //̨�ܽ�ָʾ��
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}
																													case 'I'://̨�ܼн�
																													{
																																			TIM2_count = 0;
																																			Hydraumatic_Machine = IO_OPEN;
																																			Clamp_All = IO_OPEN;
																																			Protect_Overflow = IO_OPEN;
																																			W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																			break;
																													}			
																													case '8'://�����ƶ�������
																													{																											
																																			if(Rx_Buffer[2] == 26)//֡��26
																																			{
																																					Left_BrakeSum = 0;
																																					Right_BrakeSum = 0;
																																					Left_BrakeSub = 0;
																																					Right_BrakeSub = 0;
																																					if(Rx_Buffer[6] > 0x30) Left_BrakeSum += (Rx_Buffer[6]-0x30)*10000;
																																					if(Rx_Buffer[7] > 0x30) Left_BrakeSum += (Rx_Buffer[7]-0x30)*1000;
																																					if(Rx_Buffer[8] > 0x30) Left_BrakeSum += (Rx_Buffer[8]-0x30)*100;
																																					if(Rx_Buffer[9] > 0x30) Left_BrakeSum += (Rx_Buffer[9]-0x30)*10;
																																					if(Rx_Buffer[10] > 0x30) Left_BrakeSum += Rx_Buffer[10]-0x30;
																																				
																																					if(Rx_Buffer[12] > 0x30) Right_BrakeSum += (Rx_Buffer[12]-0x30)*10000;
																																					if(Rx_Buffer[13] > 0x30) Right_BrakeSum += (Rx_Buffer[13]-0x30)*1000;
																																					if(Rx_Buffer[14] > 0x30) Right_BrakeSum += (Rx_Buffer[14]-0x30)*100;
																																					if(Rx_Buffer[15] > 0x30) Right_BrakeSum += (Rx_Buffer[15]-0x30)*10;
																																					if(Rx_Buffer[16] > 0x30) Right_BrakeSum += Rx_Buffer[16]-0x30;
																																				
																																					if(Rx_Buffer[18] > 0x30) Left_BrakeSub += (Rx_Buffer[18]-0x30)*10000;
																																					if(Rx_Buffer[19] > 0x30) Left_BrakeSub += (Rx_Buffer[19]-0x30)*1000;
																																					if(Rx_Buffer[20] > 0x30) Left_BrakeSub += (Rx_Buffer[20]-0x30)*100;
																																					if(Rx_Buffer[21] > 0x30) Left_BrakeSub += (Rx_Buffer[21]-0x30)*10;
																																					if(Rx_Buffer[22] > 0x30) Left_BrakeSub += Rx_Buffer[22]-0x30;
																																					
																																					if(Rx_Buffer[24] > 0x30) Right_BrakeSub += (Rx_Buffer[24]-0x30)*10000;
																																					if(Rx_Buffer[25] > 0x30) Right_BrakeSub += (Rx_Buffer[25]-0x30)*1000;
																																					if(Rx_Buffer[26] > 0x30) Right_BrakeSub += (Rx_Buffer[26]-0x30)*100;
																																					if(Rx_Buffer[27] > 0x30) Right_BrakeSub += (Rx_Buffer[27]-0x30)*10;
																																					if(Rx_Buffer[28] > 0x30) Right_BrakeSub += Rx_Buffer[28]-0x30;
																																					W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																					if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
																																					{
																																								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
																																								{
																																											USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																																											USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																											FunW5500_SendLED(0,Left_BrakeSum,Right_BrakeSum);//�ϴ������ƶ���
																																								}
																																					}
																																			 }
																																			 else
																																			 {
																																					 W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
																																			 }
																																			break;
																													}
																													case '9'://���������� 
																													{
																																			if(Rx_Buffer[2] == 14)//֡��14
																																			{
																																					Left_BrakeAve = 0;
																																					Right_BrakeAve = 0;
																																				
																																					if(Rx_Buffer[6] > 0x30) Left_BrakeAve += (Rx_Buffer[6]-0x30)*10000;
																																					if(Rx_Buffer[7] > 0x30) Left_BrakeAve += (Rx_Buffer[7]-0x30)*1000;
																																					if(Rx_Buffer[8] > 0x30) Left_BrakeAve += (Rx_Buffer[8]-0x30)*100;
																																					if(Rx_Buffer[9] > 0x30) Left_BrakeAve += (Rx_Buffer[9]-0x30)*10;
																																					if(Rx_Buffer[10] > 0x30) Left_BrakeAve += Rx_Buffer[10]-0x30;
																																				
																																					if(Rx_Buffer[12] > 0x30) Right_BrakeAve += (Rx_Buffer[12]-0x30)*10000;
																																					if(Rx_Buffer[13] > 0x30) Right_BrakeAve += (Rx_Buffer[13]-0x30)*1000;
																																					if(Rx_Buffer[14] > 0x30) Right_BrakeAve += (Rx_Buffer[14]-0x30)*100;
																																					if(Rx_Buffer[15] > 0x30) Right_BrakeAve += (Rx_Buffer[15]-0x30)*10;
																																					if(Rx_Buffer[16] > 0x30) Right_BrakeAve += Rx_Buffer[16]-0x30;																												
																																					W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																					if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
																																					{
																																								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
																																								{
																																											USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																																											USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																											FunW5500_SendLED(0,Left_BrakeAve,Right_BrakeAve);//�ϴ�������
																																								}
																																					}
																																			 }
																																			 else
																																			 {
																																					 W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
																																			 }
																																			break;
																													}
																													case 'A':
																													{
																																				if(Rx_Buffer[2] == 26)//֡��26
																																			 {
																																					Left_BrakeSumJJ = 0;
																																					Right_BrakeSumJJ = 0;
																																					Left_BrakeSubJJ = 0;
																																					Right_BrakeSubJJ = 0;
																																					if(Rx_Buffer[6] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[6]-0x30)*10000;
																																					if(Rx_Buffer[7] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[7]-0x30)*1000;
																																					if(Rx_Buffer[8] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[8]-0x30)*100;
																																					if(Rx_Buffer[9] > 0x30) Left_BrakeSumJJ += (Rx_Buffer[9]-0x30)*10;
																																					if(Rx_Buffer[10] > 0x30) Left_BrakeSumJJ += Rx_Buffer[10]-0x30;
																																				
																																					if(Rx_Buffer[12] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[12]-0x30)*10000;
																																					if(Rx_Buffer[13] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[13]-0x30)*1000;
																																					if(Rx_Buffer[14] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[14]-0x30)*100;
																																					if(Rx_Buffer[15] > 0x30) Right_BrakeSumJJ += (Rx_Buffer[15]-0x30)*10;
																																					if(Rx_Buffer[16] > 0x30) Right_BrakeSumJJ += Rx_Buffer[16]-0x30;
																																				
																																					if(Rx_Buffer[18] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[18]-0x30)*10000;
																																					if(Rx_Buffer[19] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[19]-0x30)*1000;
																																					if(Rx_Buffer[20] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[20]-0x30)*100;
																																					if(Rx_Buffer[21] > 0x30) Left_BrakeSubJJ += (Rx_Buffer[21]-0x30)*10;
																																					if(Rx_Buffer[22] > 0x30) Left_BrakeSubJJ += Rx_Buffer[22]-0x30;
																																					
																																					if(Rx_Buffer[24] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[24]-0x30)*10000;
																																					if(Rx_Buffer[25] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[25]-0x30)*1000;
																																					if(Rx_Buffer[26] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[26]-0x30)*100;
																																					if(Rx_Buffer[27] > 0x30) Right_BrakeSubJJ += (Rx_Buffer[27]-0x30)*10;
																																					if(Rx_Buffer[28] > 0x30) Right_BrakeSubJJ += Rx_Buffer[28]-0x30;
																																					W5500_Send_Data(s,con_TranAACK); //��ȷӦ��
																																					if(USER_SystemFlag.A.Flag_Online == 1)         //���� �����ط�
																																					{
																																								if(USER_SystemFlag.A.Flag_Standalone == 1) //�������� ������ܷ�
																																								{
																																											USER_LEDFlag.A.LED_15 = 1;  				//����������Ƿ�ı� 1��  0����
																																											USER_LEDFlag.A.LED_14 = 0;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																											FunW5500_SendLED(0,Left_BrakeSumJJ,Right_BrakeSumJJ);//�ϴ������ƶ���
																																								}
																																					}
																																			 }
																																			 else
																																			 {
																																					 W5500_Send_Data(s,con_TranXACK); //��ЧӦ��
																																			 }
																																			break;
																													}
																													case 'J'://N J  ��������
																													{
																																			if(Rx_Buffer[2] == 14)//֡��14
																																			{
																																					Left_StaticWeight = 0;
																																					Right_StaticWeight = 0;
																																					if(Rx_Buffer[6] > 0x30) Left_StaticWeight += (Rx_Buffer[6]-0x30)*10000;
																																					if(Rx_Buffer[7] > 0x30) Left_StaticWeight += (Rx_Buffer[7]-0x30)*1000;
																																					if(Rx_Buffer[8] > 0x30) Left_StaticWeight += (Rx_Buffer[8]-0x30)*100;
																																					if(Rx_Buffer[9] > 0x30) Left_StaticWeight += (Rx_Buffer[9]-0x30)*10;
																																					if(Rx_Buffer[10] > 0x30) Left_StaticWeight += Rx_Buffer[10]-0x30;
																																					if(Rx_Buffer[12] > 0x30) Right_StaticWeight += (Rx_Buffer[12]-0x30)*10000;
																																					if(Rx_Buffer[13] > 0x30) Right_StaticWeight += (Rx_Buffer[13]-0x30)*1000;
																																					if(Rx_Buffer[14] > 0x30) Right_StaticWeight += (Rx_Buffer[14]-0x30)*100;
																																					if(Rx_Buffer[15] > 0x30) Right_StaticWeight += (Rx_Buffer[15]-0x30)*10;
																																					if(Rx_Buffer[16] > 0x30) Right_StaticWeight += Rx_Buffer[16]-0x30;
																																					if(Left_StaticWeight<200)
																																					{
																																							Left_StaticWeight = 200;																															
																																							W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
																																					}
																																					if(Right_StaticWeight<200)
																																					{
																																							Right_StaticWeight = 200;
																																							W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
																																					}
								// 																													System_State = DynamoStart_State;
																																					W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																																					Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																			}
																																			else
																																			{
																																					W5500_Send_Data(s,con_TranXACK);				//������Ч
																																			}
																																			break;
																													}			
																													default:
																																		 W5500_Send_Data(s,con_TranXACK);				//������Ч
																																		 break;
																											}																			
																											break;
																				}
																				case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
																				{
                                                      TIM2_count = 0;//40S�жϹرյ�ŷ���Դ
                                                      Pump_UpDown = IO_CLOSE; //������
                                                      USER_LEDFlag.A.LED_Down = 0;         //������
                                                      USER_LEDFlag.A.LED_Up = 1;           //������
																											Left_Machine = IO_CLOSE;
																											Right_Machine = IO_CLOSE;
																											Machine_Switch = IO_CLOSE;
																											Speed_Control = IO_CLOSE;		
																											USER_SystemFlag.A.Flag_Ldynamo = 0;
																											USER_SystemFlag.A.Flag_Rdynamo = 0;
																											USER_SystemFlag.A.Flag_Switch = 0;
																											USER_SystemFlag.A.Flag_ST = 0;
																											Pump_Before = IO_CLOSE;
																											Pump_Behind = IO_CLOSE;
																											Left_Up = IO_CLOSE;
																											Right_Up = IO_CLOSE;
																											Left_Down = IO_OPEN;
																											Right_Down = IO_OPEN;
                                                      USER_LEDFlag.A.LED_TJUp = 0;         //̨�ܾ�
                                                      USER_LEDFlag.A.LED_TJDown = 1;       //̨�ܽ�
																											Clamp_All = IO_CLOSE;
																											Protect_Overflow = IO_CLOSE; 
																											Hydraumatic_Machine = IO_CLOSE;
																											System_State = Default_State;
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״̬
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											W5500_Send_Data(s,con_TranR);				   //��λ���
																											return;
																				}																
																				case 'B'://�汾��Ϣ
																				{
																											W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
																											break;
																				}											
																				case 'D'://ȡ���Խ��
																				{
																											switch(Rx_Buffer[5])
																											{																																																																																																					
																													case 'F'://�����ƶ�̨��ɽ��
																													{
																																				if(Rx_Buffer[6] == '1')
																																				{
																																						FunW5500_SendDF(s,1,AllNone_StaticWeight);																											
																																				}
																																				else if(Rx_Buffer[6] == '2')
																																				{																														
																																						FunW5500_SendDF(s,2,AllFull_StaticWeight);
																																				}
																																				else
																																				{
																																						W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
																																				}
																																				break;
																													}
																													case 'T'://����̤���������
																													{
																																				break;
																													}
																													case 'C'://���Ϳ����ƶ���������
																													{
// 																																				FunW5500_SendDC(s);				//���͵�ǰ�����
																																				W5500_Send_Data(s,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
																																				break;
																													}	
																													case 'D'://���ͼ����
																													{
																																				FunW5500_SendDC(s);				//���͵�ǰ�����
																																				break;
																													}	
																													case 'B'://�����ƶ���������
																													{																																				
// 																																				W5500_Send_Data(s,u8Tran_CurrentC);				//���͵�ǰ�����ƶ���������
																																				break;
																													}
																													case 'J'://���ͼ����ƶ���������
																													{
																																				
																																				W5500_Send_Data(s,u8Tran_CurrentJ);				//���͵�ǰ�����ƶ���������
																																				break;
																													}
																													case 'Z'://�������͹�������
																													{
																																				
																																				W5500_Send_Data(s,u8Tran_CurrentZ);				//���͵�ǰ���͹�������
																																				break;
																													}
																													case 'L'://����ͣ�����ͺ�ͣ��ʱ��
																													{
																																				Tx_Buffer[0] = 8;  //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'D'; 
																																				Tx_Buffer[3] = 'L'; 
																																				Tx_Buffer[4] = Left_StopFlag; 
																																				Tx_Buffer[5] = Right_StopFlag;
																																				Tx_Buffer[6] = Left_StopTime.u8_data[0]; 
																																				Tx_Buffer[7] = Left_StopTime.u8_data[1]; 
																																				Tx_Buffer[8] = Right_StopTime.u8_data[0]; 
																																				Tx_Buffer[9] = Right_StopTime.u8_data[1]; 																																																										
																																				W5500_Send_Data(s,Tx_Buffer);				//
																																				break;
																													}
																													default:
																																				W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
																																				break;
																											}
																											break;
																				}
																				case 'G'://ȡʵʱ����
																				{
																											switch(Rx_Buffer[5])
																											{																					
																													case 'B':
																													{
																																				Tx_Buffer[0] = 14;  //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'G'; 
																																				Tx_Buffer[3] = 'B'; 
																																				/* ���� */
																																				Tx_Buffer[4] = Left_DynamicBrake/10000; 
																																				if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
																																				else Tx_Buffer[4] = 0x20;
																																				Tx_Buffer[5] = Left_DynamicBrake%10000/1000; 
																																				if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
																																				else Tx_Buffer[5] = 0x20;
																																				Tx_Buffer[6] = Left_DynamicBrake%1000/100; 
																																				if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
																																				else Tx_Buffer[6] = 0x20;
																																				Tx_Buffer[7] = Left_DynamicBrake%100/10; 
																																				if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
																																				else Tx_Buffer[7] = 0x20;
																																				Tx_Buffer[8] = Left_DynamicBrake%10 + 0x30; 
																																				Tx_Buffer[9] = 0x20; 
																																				/* ���� */
																																				Tx_Buffer[10] = Right_DynamicBrake/10000; 
																																				if(Tx_Buffer[10] > 0) Tx_Buffer[10] += 0x30;
																																				else Tx_Buffer[10] = 0x20;
																																				Tx_Buffer[11] = Right_DynamicBrake%10000/1000; 
																																				if(Tx_Buffer[11] > 0) Tx_Buffer[11] += 0x30;
																																				else Tx_Buffer[11] = 0x20;
																																				Tx_Buffer[12] = Right_DynamicBrake%1000/100; 
																																				if(Tx_Buffer[12] > 0) Tx_Buffer[12] += 0x30;
																																				else Tx_Buffer[12] = 0x20;
																																				Tx_Buffer[13] = Right_DynamicBrake%100/10; 
																																				if(Tx_Buffer[13] > 0) Tx_Buffer[13] += 0x30;
																																				else Tx_Buffer[13] = 0x20;
																																				Tx_Buffer[14] = Right_DynamicBrake%10 + 0x30; 
																																				Tx_Buffer[15] = 0x20; 
																																				W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ�ƶ�
																																				break;
																													}																																								
																													case 'T':
																													{
																																				Tx_Buffer[0] = 8;    //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'G'; 
																																				Tx_Buffer[3] = 'T'; 
																																				/* ̤���� */
																																				Tx_Buffer[4] = DynamicFootPlat/10000; 
																																				if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
																																				else Tx_Buffer[4] = 0x20;
																																				Tx_Buffer[5] = DynamicFootPlat%10000/1000; 
																																				if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
																																				else Tx_Buffer[5] = 0x20;
																																				Tx_Buffer[6] = DynamicFootPlat%1000/100; 
																																				if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
																																				else Tx_Buffer[6] = 0x20;
																																				Tx_Buffer[7] = DynamicFootPlat%100/10; 
																																				if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
																																				else Tx_Buffer[7] = 0x20;
																																				Tx_Buffer[8] = DynamicFootPlat%10 + 0x30; 
																																				Tx_Buffer[9] = 0x20; 
																																				W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ̤����
																																				break;
																													}						
																													case 'F':
																													{
																																				Tx_Buffer[0] = 8;    //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'G'; 
																																				Tx_Buffer[3] = 'F'; 
																																				if(SysParameter.Par6_Weight == 2)
																																				 {
																																						 Quotient = All_DynamicWeight / 2;
																																						 Remainder = All_DynamicWeight % 2;
																																						 if(Remainder > 0)
																																						 {
																																								All_DynamicWeight = (Quotient +1) * 2;																										 
																																						 }																									
																																				 }
																																				 else if(SysParameter.Par6_Weight == 5)
																																				 {
																																						Quotient = All_DynamicWeight / 5;
																																						 Remainder = All_DynamicWeight % 5;
																																						 if(Remainder < 3)
																																						 {
																																								All_DynamicWeight = Quotient * 5;																										 
																																						 }
																																						 else
																																						 {
																																								All_DynamicWeight = (Quotient +1) * 5;
																																						 }																									
																																				 }
																																				/* ���ϳ��� */
																																				Tx_Buffer[4] = All_DynamicWeight/10000; 
																																				if(Tx_Buffer[4] > 0) Tx_Buffer[4] += 0x30;
																																				else Tx_Buffer[4] = 0x20;
																																				Tx_Buffer[5] = All_DynamicWeight%10000/1000; 
																																				if(Tx_Buffer[5] > 0) Tx_Buffer[5] += 0x30;
																																				else Tx_Buffer[5] = 0x20;
																																				Tx_Buffer[6] = All_DynamicWeight%1000/100; 
																																				if(Tx_Buffer[6] > 0) Tx_Buffer[6] += 0x30;
																																				else Tx_Buffer[6] = 0x20;
																																				Tx_Buffer[7] = All_DynamicWeight%100/10; 
																																				if(Tx_Buffer[7] > 0) Tx_Buffer[7] += 0x30;
																																				else Tx_Buffer[7] = 0x20;
																																				Tx_Buffer[8] = All_DynamicWeight%10 + 0x30; 
																																				Tx_Buffer[9] = 0x20; 
																																				W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰ��������
																																				break;
																													}	
																													default :
																																			 W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
																																			 break;
																											}
																											break;
																				}
																				case 'C'://�źŲ�
																				{																											
																											if(Rx_Buffer[5] == 'X')
																											{
																													USER_SystemFlag.A.Flag_TestC1 = 1;
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																											}
																											break;
																				}
																				case 'T':
																				{
																											switch(Rx_Buffer[5])
																											{																
																													case 'B': //�����ƶ�����
																													{
																																				if(Rx_Buffer[2] == 2)       //ֻ��TB ������ ��D
																																				{
																																						
																																						 System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_None_State_Flg;
																																						 Left_StaticWeight = StaticWeight_Init;
																																						 Right_StaticWeight = StaticWeight_Init;  //Ĭ�Ͼ�̬����
																																						 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else if(Rx_Buffer[2] == 3)   //ֻ��TB ������ ��D
																																				{
																																						System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_None_State_Flg;
																																						 Left_StaticWeight = StaticWeight_Init;
																																						 Right_StaticWeight = StaticWeight_Init;  //Ĭ�Ͼ�̬����
																																					   if(Rx_Buffer[6] == 'D')
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																						 }
																																						 else
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 }
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else if(Rx_Buffer[2] == 6)      //ֻ��TB ������ ��D
																																				{
																																						 System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_None_State_Flg;
																																						 Left_StaticWeight = (Rx_Buffer[7]<<8)|Rx_Buffer[6];
																																						 Right_StaticWeight = (Rx_Buffer[9]<<8)|Rx_Buffer[8];  //Ĭ�Ͼ�̬����
																																						 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else if(Rx_Buffer[2] == 7)     //ֻ��TB ������ ��D
																																				{
																																						 System_State = BrakeTest_None_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_None_State_Flg;
																																						 Left_StaticWeight = (Rx_Buffer[7]<<8)|Rx_Buffer[6];
																																						 Right_StaticWeight = (Rx_Buffer[9]<<8)|Rx_Buffer[8];  //Ĭ�Ͼ�̬����
																																					   if(Rx_Buffer[10] == 'D')
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																						 }
																																						 else
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 }
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else
																																				{
																																						 W5500_Send_Data(s,con_TranXACK);//��Ч����
																																				}					
																																				if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
																																				{
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				}																																				
																																				break;
																													}
																													case 'J': //�����ƶ�����
																													{
																																				if(Rx_Buffer[2] == 2)       //ֻ��TB ������ ��D
																																				{																																							
																																						 System_State = BrakeTest_Full_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_Full_State_Flg;
																																						 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}
																																				else if(Rx_Buffer[2] == 3)   //ֻ��TB ������ ��D
																																				{
																																						 
																																						 System_State = BrakeTest_Full_State; //ϵͳ��������ƶ�����
																																						 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																						 WorkState_Flg = BrakeTest_Full_State_Flg;
																																						 if(Rx_Buffer[6] == 'D')
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																						 }
																																						 else
																																						 {
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																						 }
																																						 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																				}																																					
																																				else
																																				{
																																						 W5500_Send_Data(s,con_TranXACK);//��Ч����
																																				}								
																																				if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
																																				{
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				}
																																				break;
																													}																											
																													case 'Z':  //���������
																													{
																																				 if((Rx_Buffer[6] == 0x31) || (Rx_Buffer[6] == 0x32))
																																				 {
																																						  if(Rx_Buffer[2] == 3)       //ֻ��TZ ������ ��D
																																							{
																																									 System_State = Retardant_State; 					//ϵͳ�������Ͳ���
																																									 SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
																																									 W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
																																									 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									 WorkState_Flg = Retardant_State_Flg;
																																									 Left_StaticWeight = StaticWeight_Init;
																																									 Right_StaticWeight = StaticWeight_Init;  //Ĭ�Ͼ�̬����
																																									 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 4)   //ֻ��TZ ������ ��D
																																							{
																																									 System_State = Retardant_State; 					//ϵͳ�������Ͳ���
																																									 SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
																																									 W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
																																									 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									 WorkState_Flg = Retardant_State_Flg;
																																									 Left_StaticWeight = StaticWeight_Init;
																																									 Right_StaticWeight = StaticWeight_Init;  //Ĭ�Ͼ�̬����
																																									 if(Rx_Buffer[7] == 'D')
																																									 {
																																												USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									 }
																																									 else
																																									 {
																																												USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									 }
																																									 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 7)      //ֻ��TZ ������ ��D
																																							{
																																									 System_State = Retardant_State; 					//ϵͳ�������Ͳ���
																																									 SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
																																									 W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
																																									 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									 WorkState_Flg = Retardant_State_Flg;
																																									 Left_StaticWeight = (Rx_Buffer[8]<<8)|Rx_Buffer[7];
																																									 Right_StaticWeight = (Rx_Buffer[10]<<8)|Rx_Buffer[9];  //Ĭ�Ͼ�̬����
																																									 USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 8)     //ֻ��TZ ������ ��D
																																							{
																																									 System_State = Retardant_State; 					//ϵͳ�������Ͳ���
																																									 SysParameter.Retardant_Flag = Rx_Buffer[6] - 0x30;
																																									 W5500_Send_Data(s,con_TranAACK);	//��ȷӦ��
																																									 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									 WorkState_Flg = Retardant_State_Flg;
																																									 Left_StaticWeight = (Rx_Buffer[8]<<8)|Rx_Buffer[7];
																																									 Right_StaticWeight = (Rx_Buffer[10]<<8)|Rx_Buffer[9];  //Ĭ�Ͼ�̬����
																																									 if(Rx_Buffer[11] == 'D')
																																									 {
																																												USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									 }
																																									 else
																																									 {
																																												USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									 }
																																									 USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else
																																							{
																																									 W5500_Send_Data(s,con_TranXACK);//��Ч����
																																							}																																								 
																																				 }
																																				 else
																																				 {	
																																							W5500_Send_Data(s,con_TranXACK);	//��Чָ֡��
																																				 }
																																				 if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
																																				 {
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																													case 'D':  //�������
																													{
																																				if(Rx_Buffer[2] == 14)//֡��14
																																				{
																																						Left_StaticWeight = 0;
																																						Right_StaticWeight = 0;
													// 																									if(Rx_Buffer[6] > 0x30) Left_StaticWeight += (Rx_Buffer[6]-0x30)*10000;
																																						if(Rx_Buffer[7] > 0x30) Left_StaticWeight += (Rx_Buffer[7]-0x30)*1000;
																																						if(Rx_Buffer[8] > 0x30) Left_StaticWeight += (Rx_Buffer[8]-0x30)*100;
																																						if(Rx_Buffer[9] > 0x30) Left_StaticWeight += (Rx_Buffer[9]-0x30)*10;
																																						if(Rx_Buffer[10] > 0x30) Left_StaticWeight += Rx_Buffer[10]-0x30;
													// 																									if(Rx_Buffer[12] > 0x30) Right_StaticWeight += (Rx_Buffer[12]-0x30)*10000;
																																						if(Rx_Buffer[13] > 0x30) Right_StaticWeight += (Rx_Buffer[13]-0x30)*1000;
																																						if(Rx_Buffer[14] > 0x30) Right_StaticWeight += (Rx_Buffer[14]-0x30)*100;
																																						if(Rx_Buffer[15] > 0x30) Right_StaticWeight += (Rx_Buffer[15]-0x30)*10;
																																						if(Rx_Buffer[16] > 0x30) Right_StaticWeight += Rx_Buffer[16]-0x30;
																																						if(Left_StaticWeight<200)
																																						{
																																								Left_StaticWeight = 200;
																																								W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
																																						}
																																						if(Right_StaticWeight<200)
																																						{
																																								Right_StaticWeight = 200;
																																								W5500_Send_Data(s,con_TranT9);//����̫Сʹ��Ĭ������
																																						}
																																						System_State = DynamoStart_State;
																																						W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																																						Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																				}
																																				else
																																				{
																																						W5500_Send_Data(s,con_TranXACK);				//������Ч
																																				}
																																				break;
																													}
																													case 'F':		//��ɲ���
																													{	
																																				 if(Rx_Buffer[6] == '1') //���ظ��ϳ���
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									System_State = StaticWeight_None_State;   //ϵͳ������ظ��ϳ���
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									WorkState_Flg = StaticWeight_None_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(USER_LEDFlag.A.LED_Down != 1)
																																									{
																																											Pump_UpDown = IO_OPEN;
																																											USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																											USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																											Delay_ms(2000);
																																									}   
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									System_State = StaticWeight_None_State;   //ϵͳ������ظ��ϳ���
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									WorkState_Flg = StaticWeight_None_State_Flg;
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									}
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(USER_LEDFlag.A.LED_Down != 1)
																																									{
																																											Pump_UpDown = IO_OPEN;
																																											USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																											USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																											Delay_ms(2000);
																																									}  
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//ָ����Ч
																																							}
																																				 }
																																				 else if(Rx_Buffer[6] == '2')//���ؾ���
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									System_State = StaticWeight_Full_State;  //ϵͳ������ؾ���״̬
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									WorkState_Flg = StaticWeight_Full_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(USER_LEDFlag.A.LED_Down != 1)
																																									{
																																											Pump_UpDown = IO_OPEN;
																																											USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																											USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																											Delay_ms(2000);
																																									}  
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									System_State = StaticWeight_Full_State;  //ϵͳ������ؾ���״̬
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									WorkState_Flg = StaticWeight_Full_State_Flg;
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									}
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(USER_LEDFlag.A.LED_Down != 1)
																																									{
																																											Pump_UpDown = IO_OPEN;
																																											USER_LEDFlag.A.LED_Up = 0;  				//������ָʾ��
																																											USER_LEDFlag.A.LED_Down = 1;    //������ָʾ��
																																											Delay_ms(2000);
																																									}  
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//ָ����Ч
																																							}																																						
																																				 }
																																				 else
																																				 {
																																						W5500_Send_Data(s,con_TranXACK);//ָ����Ч
																																				 }																
																																				 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																				 if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
																																				 {
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
																																									USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																											}
																											break;
																				}
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//��ǰָ���޷�ִ��																								 
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
* ������  : W5500_Receive_Deal
* ����    : W5500���Գ���
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ģ��������ʱ�˲��ֿɲ����޸�
*******************************************************************************/
void W5500_Receive_Deal(void)
{
		//�����ļ���ѭ������
		AES_Handler();
		IWDG_ReloadCounter();   //���Ź�����
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
        W5500_Socket_Set();                                    		//W5500�˿ڳ�ʼ������ main        
        if(W5500_Interrupt)                                       		 	//����W5500�ж�		 main
        {
            W5500_Interrupt_Process();                      		//W5500�жϴ��������
        }
        if((S0_Data & S_RECEIVE) == S_RECEIVE)    //���Socket0���յ�����  main
        {
            S0_Data&=~S_RECEIVE;
            W5500_Socket_Data(0);                              //W5500���ݴ���
// 						W5500_Send_Data(0,con_TranAACK);
        }
				if((S2_Data & S_RECEIVE) == S_RECEIVE)    //���Socket2���յ�����  main
        {
            S2_Data&=~S_RECEIVE;
            W5500_Socket_Data(2);                              //W5500���ݴ���
// 						W5500_Send_Data(0,con_TranAACK);
        }
    }
}

/*******************************************************************************
* ������  : W5500_Init_Config
* ����    : W5500��ʼ���� ������������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��Ҫ��ǰ���úö�ʱ��2 �õ�����ʱ����  ��ʱ��2��AD7663������
*******************************************************************************/
void W5500_Init_Config(void)
{
		W5500_SPI_Configuration();		//W5500 SPI��ʼ������(STM32 SPI1)
		W5500_GPIO_Configuration();	//W5500 GPIO��ʼ������	
		W5500_Initialization();		//W5500��ʼ������	
		Load_Net_Parameters();
		TIM_Cmd(TIM3, ENABLE); 		//ʹ��TIMx����  UDPʹ��
}
/*******************************************************************************
* ������  : W5500_Reset
* ����    : W5500ǿ�ƻָ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��Ҫ��ǰ���úö�ʱ��2 �õ�����ʱ����  ��ʱ��2��AD7663������
*******************************************************************************/
void W5500_Reset(void)
{
		ucW5500_InitCTR = 0;
		S0_State = 0;
		S1_State = 0;
		S2_State = 0;
    S3_State = 0;
// 		W5500_Initialization();		//W5500��ʼ������	
	  if(USER_SystemFlag.A.Flag_Online == 1) //���������� �ϴ�����
		{									
				USER_SystemFlag.A.Flag_Online = 0;
				USER_LEDFlag.A.LED_Online = 0;   //����ָʾ��
				if(USER_SystemFlag.A.Flag_Standalone == 1) //��������������� �ϴ�����
				{
						USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
						USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����						
						FunW5500_SendLED(0,0,0);            //�ϴ�����ʾ��
				}
		}
		W5500_ParameterReset();
		Load_Net_Parameters();
}


