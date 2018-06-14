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
// 		IP_Addr[0]=10;//���ر���IP��ַ   ���� 3
// 		IP_Addr[1]=14;
// 		IP_Addr[2]=16;
// 		IP_Addr[3]=150;
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
// 	uint8_t u8Par_ID;//������дʱID��ʱ����
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
						u16_Length = (Rx_Buffer[2] | (Rx_Buffer[3] <<8)) + 6;
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
////										W5500_Send_Data(s,con_TranO);	//����
								}
								else if(s==2)
								{
										Network1_StopFlg = 0;
								}
								if(1) //����̨
								{
										if(s==0)
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
                              if(WorkState_Flg == Default_State_Flg)   //����״̬
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
                                        case 'S'://ȡ״̬
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
                                                      Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                      WorkState_Flg = Default_State_Flg;  //����״tai
                                                      W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                      return;
                                        }
                                        case 'I'://��ʼ��ָ��
                                        {
                                                      if(Rx_Buffer[5]  == 'W')
                                                      {
                                                          W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                          for(i=0;i<6;i++) //�����ʼ��
                                                          {
                                                              Left_DynamicWeight1[i] = 0;
                                                              Right_DynamicWeight1[i] = 0;
                                                          }
                                                          Left_StaticWeight = 0;				//���־�̬���� 
                                                          Right_StaticWeight = 0;  		//���־�̬����
                                                          Motor_StaticWeight = 0; 		//Ħ�о�̬���� ��̨��
                                                          Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��												
                                                          W5500_Send_Data(s,con_TranI);				//��ȷӦ��																
                                                      }																			
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
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
                                                      else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
                                                                                                    W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
                                                                                                    W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
                                                                                                    break;	
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
                                                          W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                                                      }
                                                      break;
                                        }
                                        case 'D'://ȡ���Խ��
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'W'://��̬���ؽ��
                                                          {																												
                                                                        FunW5500_SendDW(s);				//���͵�ǰ����
                                                                        break;
                                                          }
                                                          case 'A'://��̬��ɽ��
                                                          {																												
                                                                        break;
                                                          }
                                                          case 'L'://����Ħ�г��������
                                                          {
                                                                        Tx_Buffer[0] = 8;    //֡���� ��λ
                                                                        Tx_Buffer[1] = 0;    //֡���� ��λ
                                                                        Tx_Buffer[2] = 'D'; 
                                                                        Tx_Buffer[3] = 'L'; 
                                                                        /* Ħ�г��� */
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
                                                                        W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰĦ������
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
                                                                        FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //�ϴ���̬��ʾ����
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
                                                                                case Left_Weight_ID:
                                                                                                                    AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
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
                                                                                                                  Cabrate_Computer();  //����궨ϵ��
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//��������
                                                                                                                  SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//����ϵ��																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//��������
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//����궨����
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
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
                                                                                                                  Cabrate_Computer();  //����궨ϵ��
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//��������
                                                                                                                  SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//����ϵ��																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//��������
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//����궨����
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
                                                                                case Left_Weight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
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
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
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
                                                                   Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                              }	
                                                              SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//��������
                                                              SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterIP_Flash_Address,26);//����ϵ��		
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
                                                          case 'W'://��̬���ؼ��
                                                          {																												 																												 
                                                                         if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
                                                                         {
                                                                             DynamicWeight_Number = Rx_Buffer[6] - 0x30;
                                                                             System_State = DynamicWeight_State; //ϵͳ���붯̬����
                                                                             W5500_Send_Data(s,con_TranAACK);  //��ȷӦ��	
                                                                             Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                              WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else if(Rx_Buffer[6] == 0x31)
                                                                         {
                                                                            DynamicWeight_Number = 1;
                                                                            System_State = StaticWeight_State; //ϵͳ���뾲̬����
                                                                             W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                             Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                             WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else
                                                                         {
                                                                              W5500_Send_Data(s,con_TranXACK);	//��Чָ֡��
                                                                         }
                                                                         break;
                                                          }
                                                          case 'A'://��̬̬���ؼ��
                                                          {
                                                                         if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
                                                                         {
                                                                             DynamicWeight_Number = Rx_Buffer[6] - 0x30;
                                                                             System_State = DynamicWeight_State; //ϵͳ���붯̬����
                                                                             W5500_Send_Data(s,con_TranAACK);  //��ȷӦ��	
                                                                             Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                              WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else
                                                                         {
                                                                              W5500_Send_Data(s,con_TranXACK);	//��Чָ֡��
                                                                         }
                                                                         break;
                                                          }
                                                          case 'L':  //Ħ�ؼ��
                                                          {																							 
                                                                         System_State = StaticWeight_Motor_State; //ϵͳ����Ħ�г���
                                                                         W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                         Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                                         WorkState_Flg = StaticWeight_Motor_State_Flg;
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
                              else if(WorkState_Flg == Adjust_State_Flg)   //�궨״̬
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
                                        case 'S'://ȡ״̬
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
                                                                        W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }											
                                        case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
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
                                                      Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                      WorkState_Flg = Default_State_Flg;  //����״�
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
                                                      if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
                                                                                                    W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
                                                                                                    W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
                                                                                                    break;	
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == 'E')  //�˳�
                                                      {
                                                          System_State = Default_State;
                                                          WorkState_Flg = Default_State_Flg;    //����״̬
                                                          W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
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
                                                                                case Left_Weight_ID:
                                                                                                                    AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
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
                                                                                                                  Cabrate_Computer();  //����궨ϵ��
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//��������
                                                                                                                  SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//����ϵ��																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//��������
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//����궨����
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
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
                                                                                                                  Cabrate_Computer();  //����궨ϵ��
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//��������
                                                                                                                  SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//����ϵ��																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//��������
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//����궨����
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
                                                                                case Left_Weight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
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
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
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
                              else if(WorkState_Flg == SignalTest_State_Flg)  //�źŲ���״̬
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
                                        case 'S'://ȡ״̬
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
                                                                        W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }												
                                        case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
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
                                                      Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                      WorkState_Flg = Default_State_Flg;  //����״�
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
                                                      else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
                                                                                                    W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //�궨״̬
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
                                                                                                    W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
                                                                                                    break;	
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
                                                          W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
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
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
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
                              else   //���״̬
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
                                        case 'S'://ȡ״̬
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
                                                      Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
                                                      WorkState_Flg = Default_State_Flg;  //����״�
                                                      W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
                                                      return;
                                        }																						
                                        case 'B'://�汾��Ϣ
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
                                                      break;
                                        }											
                                        case 'G'://ȡʵʱ����
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
                                                                         FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //�ϴ���̬��ʾ����
                                                                        break;
                                                          }																																																																																																										
                                                          default :
                                                                       W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
                                                                       break;
                                                      }
                                                      break;
                                        }																						
                                        case 'D'://ȡ���Խ��
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'W'://��̬���ؽ��
                                                          {
                                                                        FunW5500_SendDW(s);
                                                                        break;
                                                          }
                                                          case 'A'://��̬��ɽ��
                                                          {
                                                                        break;
                                                          }
                                                          case 'L'://����Ħ�г��������
                                                          {
                                                                        Tx_Buffer[0] = 8;    //֡���� ��λ
                                                                        Tx_Buffer[1] = 0;    //֡���� ��λ
                                                                        Tx_Buffer[2] = 'D'; 
                                                                        Tx_Buffer[3] = 'L'; 
                                                                        /* Ħ�г��� */
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
                                                                        W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰĦ������
                                                                        break;
                                                          }																																																																																																									
                                                          default:
                                                                        W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
                                                                        break;
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
										else if(s==2)
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
															if(WorkState_Flg == Default_State_Flg)   //����״̬
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
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״tai
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											W5500_Send_Data(s,con_TranR);				//��λ���
																											return;
																				}
																				case 'I'://��ʼ��ָ��
																				{
																											if(Rx_Buffer[5]  == 'W')
																											{
																													W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																													for(i=0;i<6;i++) //�����ʼ��
																													{
																															Left_DynamicWeight1[i] = 0;
																															Right_DynamicWeight1[i] = 0;
																													}
																													Left_StaticWeight = 0;				//���־�̬���� 
																													Right_StaticWeight = 0;  		//���־�̬����
																													Motor_StaticWeight = 0; 		//Ħ�о�̬���� ��̨��
																													Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��												
																													W5500_Send_Data(s,con_TranI);				//��ȷӦ��																
																											}																			
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
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
																											else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																		AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
																																																		W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																		AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
																																																		W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																																																		break;	
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
																													W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
																											}
																											break;
																				}
																				case 'D'://ȡ���Խ��
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'W'://��̬���ؽ��
																													{																												
																																				FunW5500_SendDW(s);				//���͵�ǰ����
																																				break;
																													}
																													case 'A'://��̬��ɽ��
																													{																												
																																				break;
																													}
																													case 'L'://����Ħ�г��������
																													{
																																				Tx_Buffer[0] = 8;    //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'D'; 
																																				Tx_Buffer[3] = 'L'; 
																																				/* Ħ�г��� */
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
																																				W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰĦ������
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
																																				FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //�ϴ���̬��ʾ����
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
																																								case Left_Weight_ID:
																																																										AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Right_Wight_ID:
																																																										AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
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
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
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
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//����궨����
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
																																								case Left_Weight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
																																																										break;
																																								case Right_Wight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
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
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
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
																													case 'W'://��̬���ؼ��
																													{																												 																												 
																																				 if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //ϵͳ���붯̬����
																																								  W5500_Send_Data(s,con_TranAACK);  //��ȷӦ��	
																																								  Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //ϵͳ���붯̬����
																																								  W5500_Send_Data(s,con_TranAACK);  //��ȷӦ��	
																																								  Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//ָ����Ч
																																							}																																					 																																					 
																																				 }
																																				 else if(Rx_Buffer[6] == 0x31)
																																				 {
																																						 if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //ϵͳ���뾲̬����
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //ϵͳ���뾲̬����
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//ָ����Ч
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
																																									USER_LEDFlag.A.LED_Weight = 1;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 0;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																													case 'A'://��̬̬���ؼ��
																													{
																																				 if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
																																				 {
																																						 DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																						 System_State = DynamicWeight_State; //ϵͳ���붯̬����
																																						 W5500_Send_Data(s,con_TranAACK);  //��ȷӦ��	
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																							WorkState_Flg = StaticWeight_State_Flg;																																						
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//��Чָ֡��
																																				 }
																																				 break;
																													}
																													case 'L':  //Ħ�ؼ��
																													{																							 
																																				 System_State = StaticWeight_Motor_State; //ϵͳ����Ħ�г���
																																				 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																				 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																				 WorkState_Flg = StaticWeight_Motor_State_Flg;
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
															else if(WorkState_Flg == Adjust_State_Flg)   //�궨״̬
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
																																				W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
																																				break;
																														
																											}
																											break;
																				}											
																				case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
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
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״tai
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											W5500_Send_Data(s,con_TranR);				//��λ���
																											return;
																				}																								
																				case 'B'://�汾��Ϣ
																				{
																											W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
																											break;
																				}																																
																				case 'C'://�źŲ��Խ���
																				{
																											if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																		AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
																																																		W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																		AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
																																																		W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																																																		break;	
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')  //�˳�
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //����״̬
																													W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
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
																																								case Left_Weight_ID:
																																																										AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
																																																										W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																										break;
																																								case Right_Wight_ID:
																																																										AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
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
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//����궨����
																																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
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
																																																									Cabrate_Computer();  //����궨ϵ��
																																																									Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//��������
																																																									SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//����ϵ��																																																
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//��������
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//����궨����
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
																																								case Left_Weight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
																																																										break;
																																								case Right_Wight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
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
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<0x09))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//��ǰ������Ч
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //ǿ������
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
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
															else if(WorkState_Flg == SignalTest_State_Flg)  //�źŲ���״̬
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
																																				W5500_Send_Data(s,con_TranXACK);				//��ǰ������Ч
																																				break;
																														
																											}
																											break;
																				}												
																				case 'R'://ǿ�Ƹ�λָ�� �ص�����״̬
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
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״tai
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											W5500_Send_Data(s,con_TranR);				//��λ���
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
																											else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																		AD_Channel_ID = Left_Weight_ID;	//��Ӧģ��ͨ��1ͨ��
																																																		W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //�궨״̬
																																																		AD_Channel_ID = Right_Wight_ID;	//��Ӧģ��ͨ��2ͨ��
																																																		W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ĭ�ϳ���ʱ��Ӧģ��ͨ��1 
																																																		break;	
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
																													W5500_Send_Data(s,con_TranXACK);//��ǰ������Ч
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
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
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
															else   //���״̬
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
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											WorkState_Flg = Default_State_Flg;  //����״tai
																											W5500_Send_Data(s,con_TranAACK);				//��ȷӦ��
																											Delay_ms(10);//��ʱ ��֤��λ�����յ�ָ��
																											W5500_Send_Data(s,con_TranR);				//��λ���
																											return;
																				}																						
																				case 'B'://�汾��Ϣ
																				{
																											W5500_Send_Data(s,con_TranB);				//��ǰ�汾��Ϣ
																											break;
																				}											
																				case 'G'://ȡʵʱ����
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
																																				 FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //�ϴ���̬��ʾ����
																																				break;
																													}																																																																																																										
																													default :
																																			 W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
																																			 break;
																											}
																											break;
																				}																						
																				case 'C'://�źŲ��Խ���
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
																													case 'W'://��̬���ؼ��
																													{																												 																												 
																																				 if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //ϵͳ���붯̬����
																																								  W5500_Send_Data(s,con_TranAACK);  //��ȷӦ��	
																																								  Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //ϵͳ���붯̬����
																																								  W5500_Send_Data(s,con_TranAACK);  //��ȷӦ��	
																																								  Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//ָ����Ч
																																							}																																					 																																					 
																																				 }
																																				 else if(Rx_Buffer[6] == 0x31)
																																				 {
																																						 if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //ϵͳ���뾲̬����
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //ϵͳ���뾲̬����
																																									W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																									Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//���ָʾ��
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //�����ϴ����
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //�������ϴ����
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//ָ����Ч
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
																																									USER_LEDFlag.A.LED_Weight = 1;  			//����ָʾ��
																																									USER_LEDFlag.A.LED_Brake = 0;  			  //�ƶ�ָʾ��
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																													case 'A'://��̬̬���ؼ��
																													{
																																				 if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
																																				 {
																																						 DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																						 System_State = DynamicWeight_State; //ϵͳ���붯̬����
																																						 W5500_Send_Data(s,con_TranAACK);  //��ȷӦ��	
																																						 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																							WorkState_Flg = StaticWeight_State_Flg;
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//��Чָ֡��
																																				 }
																																				 break;
																													}
																													case 'L':  //Ħ�ؼ��
																													{																							 
																																				 System_State = StaticWeight_Motor_State; //ϵͳ����Ħ�г���
																																				 W5500_Send_Data(s,con_TranAACK);//��ȷӦ��
																																				 Delay_ms(20);//��ʱ ��֤��λ�����յ�ָ��
																																				 WorkState_Flg = StaticWeight_Motor_State_Flg;
																																				 break;
																													}																																																															
																											}
																											break;
																				}
																				case 'D'://ȡ���Խ��
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'W'://��̬���ؽ��
																													{
																																				FunW5500_SendDW(s);
																																				break;
																													}
																													case 'A'://��̬��ɽ��
																													{
																																				break;
																													}
																													case 'L'://����Ħ�г��������
																													{
																																				Tx_Buffer[0] = 8;    //֡���� ��λ
																																				Tx_Buffer[1] = 0;    //֡���� ��λ
																																				Tx_Buffer[2] = 'D'; 
																																				Tx_Buffer[3] = 'L'; 
																																				/* Ħ�г��� */
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
																																				W5500_Send_Data(s,Tx_Buffer);				//���͵�ǰĦ������
																																				break;
																													}																																																																																																									
																													default:
																																				W5500_Send_Data(s,con_TranXACK); //��ǰָ����Ч
																																				break;
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
