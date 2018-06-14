/****************************************************************************
* ÎÄ¼þÃû: W5500_APP.c
* ÎÄ¼þÀúÊ·:
* °æ±¾ºÅ    V1.0
* ÈÕÆÚ      2016.09.29    
* ×÷Õß      qgk
* ËµÃ÷      
*****************************************************************************/
/*=============================================================================
*******ÐÞ¸ÄÈÕÖ¾*******
1¡¢ÈÕ    ÆÚ£º
µØ    µã£º
ÐÞ ¸Ä ÈË£º
Ìí¼ÓÄÚÈÝ£º
°æ±¾ºÅ:
2¡¢ÈÕ    ÆÚ£º
µØ    µã£º
ÐÞ ¸Ä ÈË£º
Ìí¼ÓÄÚÈÝ£º
°æ±¾ºÅ:
================================================================================*/
#include "W5500.h"	
#include "W5500_APP.h"
unsigned char ucW5500_InitCTR=0;
FLASH_Status WFLASHStatus = FLASH_COMPLETE;

// static uint8_t u8flash_buffer[21];//±ê¶¨ÏµÊý  ²ÎÊýÁÙÊ±´æ´¢±äÁ¿
static	_ADCab _ADCabVariable;  //ÇúÏßÄâºÏ±äÁ¿
/*******************************************************************************
* º¯ÊýÃû  : IDtoMAC
* ÃèÊö    : ¶ÁÈ¡STM32F10X IDºóÁùÎ»×÷ÎªMACµØÖ·
* ÊäÈë    : *ulp Ö¸ÏòMACµØÖ·Êý×éµÄÖ¸Õë
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : ÈôPhy_Addr[]Îª´æ´¢MACµÄÊý×é£¬Ê¹ÓÃIDtoMAC(Phy_Addr)µ÷ÓÃ
*******************************************************************************/
void IDtoMAC(u8 *ulp)	
{
	u32 ultmp1,ultmp2;

    //¶ÁÈ¡²úÆ·ID
	ultmp1=*(__IO u32*)(0x1FFFF7EC);
	ultmp2=*(__IO u32*)(0x1FFFF7F0);

	*ulp++ = (u8)(((ultmp1 & 0x00FF0000)>>16)&0xfe);	//¸ß×Ö½Ú×îµÍÎ»ÎªÁã
	*ulp++ = (u8) ((ultmp1 & 0xFF000000)>>24);
	*ulp++ = (u8)  (ultmp2 & 0x000000FF);
	*ulp++ = (u8) ((ultmp2 & 0x0000FF00)>>8);
	*ulp++ = (u8) ((ultmp2 & 0x00FF0000)>>16);
	*ulp =   (u8) ((ultmp2 & 0xFF000000)>>24);
}

/*******************************************************************************
* º¯ÊýÃû  : Load_Net_Parameters
* ÃèÊö    : ×°ÔØÍøÂç²ÎÊý
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : Íø¹Ø¡¢ÑÚÂë¡¢ÎïÀíµØÖ·¡¢±¾»úIPµØÖ·¡¢¶Ë¿ÚºÅ¡¢Ä¿µÄIPµØÖ·¡¢Ä¿µÄ¶Ë¿ÚºÅ¡¢¶Ë¿Ú¹¤×÷Ä£Ê½
*******************************************************************************/
void Load_Net_Parameters(void)
{
		Gateway_IP[0] = W5500_Parameter[0];//¼ÓÔØÍø¹Ø²ÎÊý
		Gateway_IP[1] = W5500_Parameter[1];
		Gateway_IP[2] = W5500_Parameter[2];
		Gateway_IP[3] = W5500_Parameter[3];
		Sub_Mask[0]=W5500_Parameter[4];//¼ÓÔØ×ÓÍøÑÚÂë
		Sub_Mask[1]=W5500_Parameter[5];
		Sub_Mask[2]=W5500_Parameter[6];
		Sub_Mask[3]=W5500_Parameter[7];			
		IDtoMAC(Phy_Addr);  //¼ÓÔØÎïÀíµØÖ·
		IP_Addr[0]=W5500_Parameter[8];//¼ÓÔØ±¾»úIPµØÖ·   ³ÆÖØ 3
		IP_Addr[1]=W5500_Parameter[9];
		IP_Addr[2]=W5500_Parameter[10];
		IP_Addr[3]=W5500_Parameter[11];
// 		IP_Addr[0]=10;//¼ÓÔØ±¾»úIPµØÖ·   ³ÆÖØ 3
// 		IP_Addr[1]=14;
// 		IP_Addr[2]=16;
// 		IP_Addr[3]=150;
		S0_Port[0] = 0x13;//¼ÓÔØ¶Ë¿Ú0µÄ¶Ë¿ÚºÅ5000
		S0_Port[1] = 0x88;
		S0_DIP[0]=192;//¼ÓÔØ¶Ë¿Ú0µÄÄ¿µÄIPµØÖ·
		S0_DIP[1]=168;
		S0_DIP[2]=148;
		S0_DIP[3]=57;			
		S0_DPort[0] = 0x17;//¼ÓÔØ¶Ë¿Ú0µÄÄ¿µÄ¶Ë¿ÚºÅ6000
		S0_DPort[1] = 0x70;
		S0_Mode=TCP_SERVER;//¼ÓÔØ¶Ë¿Ú0µÄ¹¤×÷Ä£Ê½,TCP¿Í»§¶ËÄ£Ê½	
		
			//===UDPÍ¨ÐÅ²ÎÊý
		S1_Port[0] = W5500_Parameter[25];   //½Ó¿Ú1µÄ±¾µØ¶Ë¿ÚºÅ25031
		S1_Port[1] = W5500_Parameter[24];	
		//=UDP¹ã²¥Ä¿µÄIPµØÖ·Óë¶Ë¿ÚºÅ
		UDP_DIPR[0]=W5500_Parameter[20];	//UDP(¹ã²¥)Ä£Ê½,Ä¿µÄÖ÷»úIPµØÖ·  
		UDP_DIPR[1]=W5500_Parameter[21];
		UDP_DIPR[2]=W5500_Parameter[22];
		UDP_DIPR[3]=W5500_Parameter[23];
		UDP_DPORT[0]=W5500_Parameter[25];//Ä¿µÄ¶Ë¿ÚºÅ60000  25031
		UDP_DPORT[1]=W5500_Parameter[24];
		S1_Mode=UDP_MODE;//				
		
		S2_Port[0] = W5500_Parameter[13];//¶Ë¿Ú2µÄ¶Ë¿ÚºÅ25032  ÓëÖ÷¿ØÁ¬½Ó
		S2_Port[1] = W5500_Parameter[12];				
		S2_Mode=TCP_SERVER;//¼ÓÔØ¶Ë¿Ú2µÄ¹¤×÷Ä£Ê½,TCP¿Í»§¶ËÄ£Ê½				
}
/*******************************************************************************
* º¯ÊýÃû  : W5500_Initialization
* ÃèÊö    : W5500³õÊ¼»õÅäÖÃ
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : ÎÞ
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//³õÊ¼»¯W5500¼Ä´æÆ÷º¯Êý
	Detect_Gateway();	//¼ì²éÍø¹Ø·þÎñÆ÷ 
	Socket_Init(0);		//Ö¸¶¨Socket(0~7)³õÊ¼»¯,³õÊ¼»¯¶Ë¿Ú0  //·þÎñ¶Ë  ºÍÊýÂë¹ÜÏàÁ¬
	Socket_Init(1);		//Ö¸¶¨Socket(0~7)³õÊ¼»¯,³õÊ¼»¯¶Ë¿Ú1
	Socket_Init(2);		//Ö¸¶¨Socket(0~7)³õÊ¼»¯,³õÊ¼»¯¶Ë¿Ú2  //·þÎñ¶Ë  ºÍÖ÷¿ØÏàÁ¬
}
/*******************************************************************************
* º¯ÊýÃû  : W5500_Hardware_Reset
* ÃèÊö    : Ó²¼þ¸´Î»W5500
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : W5500µÄ¸´Î»Òý½Å±£³ÖµÍµçÆ½ÖÁÉÙ500usÒÔÉÏ,²ÅÄÜÖØÆðW5500
*******************************************************************************/
void W5500_Hardware_Reset(void)
{
	if(ucW5500_InitCTR==0)
  {
			GPIO_ResetBits(W5500_RST_PORT, W5500_RST);//¸´Î»Òý½ÅÀ­µÍ
			Delay_ms(50);
			GPIO_SetBits(W5500_RST_PORT, W5500_RST);//¸´Î»Òý½ÅÀ­¸ß
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
* º¯ÊýÃû  : W5500_Send_Data
* ÃèÊö    : W5500·¢ËÍÐ­ÒéÊý¾ÝÊý¾Ý
* ÊäÈë    : s:¶Ë¿ÚºÅ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : 
*******************************************************************************/
void W5500_Send_Data(uint8_t s,uint8_t *Data)
{
		uint16_t i,u16_size=0;
		uint8_t u8_Lrc=0;
		u16_size = *(Data+1)<<8 | *Data; //Ö¡³¤
		u16_size +=  6;     												//×Ü³¤=Ö¡³¤+Ê£Óà×Ö½Ú³¤¶È
		u8Send_Buffer[0] = CON_START;
// 		if(SysParameter.Par12_MachineType == 0) //ÂÖÖØ
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
* º¯ÊýÃû  : Process_Socket_Data
* ÃèÊö    : W5500½ÓÊÕ²¢·¢ËÍ½ÓÊÕµ½µÄÊý¾Ý
* ÊäÈë    : s:¶Ë¿ÚºÅ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : ±¾¹ý³ÌÏÈµ÷ÓÃS_rx_process()´ÓW5500µÄ¶Ë¿Ú½ÓÊÕÊý¾Ý»º³åÇø¶ÁÈ¡Êý¾Ý,
*			È»ºó½«¶ÁÈ¡µÄÊý¾Ý´ÓRx_Buffer¿½±´µ½Temp_Buffer»º³åÇø½øÐÐ´¦Àí¡£
*			´¦ÀíÍê±Ï£¬½«Êý¾Ý´ÓTemp_Buffer¿½±´µ½Tx_Buffer»º³åÇø¡£µ÷ÓÃS_tx_process()
*			·¢ËÍÊý¾Ý¡£
*******************************************************************************/
void W5500_Socket_Data(SOCKET s)
{
	uint16_t u16_size;
	uint16_t i;  //forÑ­»·¼ÆÊýÊ¹ÓÃ
	uint8_t u8read_flg;//¶Á±ê¶¨ÏµÊý±êÖ¾
	uint8_t u8flash_buffer[21];//±ê¶¨ÏµÊý  ²ÎÊýÁÙÊ±´æ´¢±äÁ¿
	/* Ö¡³¤µÍÎ»		Ö¡³¤¸ßÎ»   ÃüÁî   Ð£Ñé*/
	uint8_t u8_Command,u8_Lrc=0;
	uint16_t u16_Length;
// 	uint8_t u8Par_ID;//²ÎÊý¶ÁÐ´Ê±IDÁÙÊ±±äÁ¿
	uint8_t Remainder;  //ÓàÊý
  uint16_t Quotient;      //ÉÌ
	int8_t cCheck_Result;//¼ÓÃÜÐ£Ñé
	
// 	_ADCab _ADCabVariable;  //ÇúÏßÄâºÏ±äÁ¿
	
	u16_size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
//	for(i=0;i<u16_size;i++)
//	{
//			printf("%x ",Rx_Buffer[i]);
//	}
//	printf("\r\n");
	if(((Rx_Buffer[0] == CON_START) && (Rx_Buffer[u16_size-1] == CON_END) && (Rx_Buffer[4] == 'F')) == 1)
	{
			SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
				for(i=0;i<24;i++) //°ÑËùÓÐÊý¾Ý´æ´¢µÄÁÙÊ±±äÁ¿Parameter_Buffer
				{
						 con_TranF[i+5] = Parameter_Buffer[i];
				}	
			 W5500_Send_Data(s,con_TranF);       //»Ø´«Éè±¸ÐÅÏ¢
	}
	else
	{
				if(((Rx_Buffer[0] == CON_START) && (Rx_Buffer[u16_size-1] == CON_END) && (Rx_Buffer[1] == CON_ADDRESS)) != 1)
				{
							/*	ÆðÊ¼Î»¡¢½áÊøÎ»¡¢µØÖ·Î»ÈÎÒâÒ»¸ö²»ÕýÈ·£¬´íÎóÓ¦´ðZ	*/
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
								/*	ÆæÅ¼Ð£Ñé²»ÕýÈ·£¬´íÎóÓ¦´ðZ	*/
								W5500_Send_Data(s,con_TranZACK);
						}
						else  /* ÆðÊ¼Î»¡¢½áÊøÎ»¡¢Ð£ÑéÎ»ºÍµØÖ·ÂëÕýÈ·ºó½øÐÐÃüÁî·ÖÎö */
						{
								u8_Command = Rx_Buffer[4];
								if(s==0)
								{
										Network0_StopFlg = 0;
////										W5500_Send_Data(s,con_TranO);	//ÐÄÌø
								}
								else if(s==2)
								{
										Network1_StopFlg = 0;
								}
								if(1) //ÂÖÖØÌ¨
								{
										if(s==0)
										{
													if(USER_SystemFlag.A.Flag_Sucess1 == 0)  //ÊýÂë¹ÜÎÕÊÖ¼ÓÃÜ
                          {
                              switch(u8_Command)
															{
																	case 'O'://ÐÄÌøÖ¸Áî
																	{
																								if(s==0)
																								{
																										Network0_StopFlg = 0;
                                                    W5500_Send_Data(s,con_TranO);	//ÐÄÌø
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
																										Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
																										Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
																										Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
																										Tx_Buffer[2] = 'H'; 
																										Tx_Buffer[3] = 'A'; 
																										memcpy(&Tx_Buffer[4],cID_Key,10);
																										W5500_Send_Data(s,Tx_Buffer);//»Ø´«
																								}
																								if(Rx_Buffer[5]  == 'E')
																								{
																										if(Encryption_Cnt <= ENCRYPTION_MAX)
																										{
																												cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
																												if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
																												{
																														Encryption_Cnt = 0;
																														W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
																														USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                            USER_SystemFlag.A.Flag_Standalone = 1;  //ºÍÊýÂë¹ÜÁªÍø³É¹¦
																												}
																												else
																												{
																														Encryption_Cnt ++;																											
																														W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
																												}
																										}
																										else
																										{
																												W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
																										}
																										
																								}
																								break;
																	 }
																	default :
																								W5500_Send_Data(s,con_TranHX0);//»Ø´«Ö¸Áî´íÎó
																								break;
															}
                          }
                          else
                          {
                              if(WorkState_Flg == Default_State_Flg)   //´ý»ú×´Ì¬
                              {
                                    switch(u8_Command)
                                    {															
                                        case 'O'://ÐÄÌøÖ¸Áî
                                        {
                                                      if(s==0)
                                                      {
                                                          Network0_StopFlg = 0;
                                                          W5500_Send_Data(s,con_TranO);	//ÐÄÌø
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
                                                          Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
                                                          Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
                                                          Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
                                                          Tx_Buffer[2] = 'H'; 
                                                          Tx_Buffer[3] = 'A'; 
                                                          memcpy(&Tx_Buffer[4],cID_Key,10);
                                                          W5500_Send_Data(s,Tx_Buffer);//»Ø´«
                                                      }
                                                      if(Rx_Buffer[5]  == 'E')
                                                      {
                                                          if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                          {
                                                              cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
                                                              if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
                                                              {
                                                                  Encryption_Cnt = 0;
                                                                  W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
                                                                  USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                  USER_SystemFlag.A.Flag_Standalone = 1;  //ºÍÊýÂë¹ÜÁªÍø³É¹¦
                                                              }
                                                              else
                                                              {
                                                                  Encryption_Cnt ++;																											
                                                                  W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
                                                              }
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
                                                          }
                                                          
                                                      }
                                                      break;
                                         }
                                        case 'S'://È¡×´Ì¬
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
                                                                        W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }
                                        case 'U'://Ç¿ÖÆÊÕÎ²Ö¸Áî »Øµ½´ý»ú×´Ì¬
                                        {
                                                      W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
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
                                                      Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                      WorkState_Flg = Default_State_Flg;  //´ý»ú×´Ì¬
                                                      W5500_Send_Data(s,con_TranU);				//½áÊø
                                                      return;
                                        }
                                        case 'R'://Ç¿ÖÆ¸´Î»Ö¸Áî »Øµ½´ý»ú×´Ì¬
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
                                                      Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                      WorkState_Flg = Default_State_Flg;  //´ý»ú×´tai
                                                      W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
                                                      return;
                                        }
                                        case 'I'://³õÊ¼»¯Ö¸Áî
                                        {
                                                      if(Rx_Buffer[5]  == 'W')
                                                      {
                                                          W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
                                                          for(i=0;i<6;i++) //»º´æ³õÊ¼»¯
                                                          {
                                                              Left_DynamicWeight1[i] = 0;
                                                              Right_DynamicWeight1[i] = 0;
                                                          }
                                                          Left_StaticWeight = 0;				//×óÂÖ¾²Ì¬ÖáÖØ 
                                                          Right_StaticWeight = 0;  		//ÓÒÂÖ¾²Ì¬ÖáÖØ
                                                          Motor_StaticWeight = 0; 		//Ä¦ÍÐ¾²Ì¬ÖáÖØ ×óÌ¨¼Ü
                                                          Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî												
                                                          W5500_Send_Data(s,con_TranI);				//ÕýÈ·Ó¦´ð																
                                                      }																			
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
                                                      }
                                                      break;
                                        }												
                                        case 'B'://°æ±¾ÐÅÏ¢
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//µ±Ç°°æ±¾ÐÅÏ¢
                                                      break;
                                        }
                                        case 'C'://ÐÅºÅ²âÊÔ½çÃæ
                                        {
                                                      if(Rx_Buffer[5] == 0)
                                                      {
                                                            if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
                                                            {
                                                                Signal_TransferTime = Rx_Buffer[6] - 0x30;
                                                                System_State = SignalTest_State;
                                                                WorkState_Flg = SignalTest_State_Flg;    //ÐÅºÅ²âÊÔ×´Ì¬
                                                                W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                            }
                                                            else
                                                            {
                                                                W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                            }
                                                      }
                                                      else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
                                                                                                    break;	
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == 'E')
                                                      {
                                                          System_State = Default_State;
                                                          WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
                                                          W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                      }
                                                      else if(Rx_Buffer[5] == 'X')
                                                      {
                                                          USER_SystemFlag.A.Flag_TestC1 = 1;
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                      }
                                                      break;
                                        }
                                        case 'D'://È¡²âÊÔ½á¹û
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'W'://¾²Ì¬ÂÖÖØ½á¹û
                                                          {																												
                                                                        FunW5500_SendDW(s);				//·¢ËÍµ±Ç°ÂÖÖØ
                                                                        break;
                                                          }
                                                          case 'A'://¶¯Ì¬ÖáºÉ½á¹û
                                                          {																												
                                                                        break;
                                                          }
                                                          case 'L'://·¢ËÍÄ¦ÍÐ³µÖØÁ¿½á¹û
                                                          {
                                                                        Tx_Buffer[0] = 8;    //Ö¡³¤¶È µÍÎ»
                                                                        Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
                                                                        Tx_Buffer[2] = 'D'; 
                                                                        Tx_Buffer[3] = 'L'; 
                                                                        /* Ä¦ÍÐ³ÆÖØ */
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
                                                                        W5500_Send_Data(s,Tx_Buffer);				//·¢ËÍµ±Ç°Ä¦ÍÐÖØÁ¿
                                                                        break;
                                                          }																																																																																																									
                                                          default:
                                                                        W5500_Send_Data(s,con_TranXACK); //µ±Ç°Ö¸ÁîÎÞÐ§
                                                                        break;
                                                      }
                                                      break;
                                        }
                                        case 'G'://È¡ÊµÊ±Êý¾Ý
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
                                                                        FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //ÉÏ´«¶¯Ì¬ÏÔÊ¾ÂÖÖØ
                                                                        break;
                                                          }																																																																																																										
                                                          default :
                                                                       W5500_Send_Data(s,con_TranXACK); //µ±Ç°Ö¸ÁîÎÞÐ§
                                                                       break;
                                                      }
                                                      break;
                                        }
                                        case 'A': //±ê¶¨×´Ì¬
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'C'://½øÈë±ê¶¨×´Ì¬
                                                                        {				
                                                                            System_State = Adjust_State;
                                                                            WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:
                                                                                                                    AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                                    break;																																
                                                                                default:
                                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                                    W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                                                                                    AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
                                                                                                                    break;	
                                                                            }
                                                                            break;
                                                                        }
                                                          case 'E'://ÍË³ö±ê¶¨×´Ì¬
                                                                        {
                                                                            System_State = Default_State;
                                                                            WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
                                                                            W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                            break;
                                                                        }
                                                          case 'W'://±£´æ±ê¶¨ÏµÊý
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
                                                                                                                  Cabrate_Computer();  //¼ÆËã±ê¶¨ÏµÊý
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//²Á³öÉÈÇø
                                                                                                                  SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//±£´æÏµÊý																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//²Á³öÉÈÇø
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//±£´æ±ê¶¨µãÊý
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
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
                                                                                                                  Cabrate_Computer();  //¼ÆËã±ê¶¨ÏµÊý
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//²Á³öÉÈÇø
                                                                                                                  SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//±£´æÏµÊý																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//²Á³öÉÈÇø
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//±£´æ±ê¶¨µãÊý
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                                  break;																																
                                                                                default:
                                                                                                                    printf("\r\nError:The command channel ID is false!!!xie\r\n");
                                                                                                                    W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                                                                                    break;
                                                                            }
                                                                            break;
                                                                        }
                                                          case 'R'://¶Á±ê¶¨ÏµÊý
                                                                        {
                                                                            u8read_flg = 1;//¶Á±ê¶¨ÏµÊý³É¹¦
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
                                                                                                                    break;																																
                                                                                default:
                                                                                                                    u8read_flg = 0;//¶Á±ê¶¨ÏµÊýÊ§°Ü
                                                                                                                    printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
                                                                                                                    break;
                                                                            }			
                                                                            if(u8read_flg == 1)//¶Á±ê¶¨ÏµÊý³É¹¦
                                                                            {
                                                                                Tx_Buffer[0] = 21;//Ö¡³¤¶È µÍÎ»
                                                                                Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
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
                                                                                W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                                            }
                                                                            break;
                                                                        }
                                                           default :
                                                                       W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                                       break;
                                                      }
                                                      break;
                                        }
                                        case 'Y': //µ÷Áã
                                        {
                                                      if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                      {
                                                          Fun_Zero(Rx_Buffer[5],1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //Ç¿ÖÆÇåÁã
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                      }
                                                      break;
                                        }
                                        case 'E'://²ÎÊý¶ÁÐ´
                                        {
                                                      if(Rx_Buffer[5] == '1')
                                                      {
                                                            if(Rx_Buffer[6] == 'W')
                                                            {																							
                                                                for(i=0;i<(Rx_Buffer[7]*3+1);i++) //°ÑËùÓÐÊý¾Ý´æ´¢µÄÁÙÊ±±äÁ¿u8flash_buffer
                                                                {
                                                                     Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                                }								
                                                                if(Fun_DealParameter()==1)
                                                                {
                                                                    SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//²Á³öÉÈÇø
                                                                    SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(Rx_Buffer[7]*3+1));//±£´æÏµÊý		
                                                                    W5500_Send_Data(s,con_TranE1W);//ÕýÈ·Ó¦´ð
                                                                }
                                                                else
                                                                {
                                                                    W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ
                                                                    printf("\r\nError:Write Parameter false!!!E1W\r\n");
                                                                }
                                                            }
                                                            else if(Rx_Buffer[6] == 'R')
                                                            {												
                                                                  SPI_FLASH_BufferRead(Parameter_Buffer,ParameterAll_Flash_Address,100);
                                                                  Tx_Buffer[0] = Parameter_Buffer[0]*3+4;  //Ö¡³¤¶È µÍÎ»
                                                                  Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
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
                                                                W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ	
                                                                printf("\r\nError:The command E1 is false!!!E1\r\n");
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == '2')
                                                      {
                                                          if(Rx_Buffer[6] == 'W')
                                                          {
                                                              for(i=0;i<24;i++) //°ÑËùÓÐÊý¾Ý´æ´¢µÄÁÙÊ±±äÁ¿Parameter_Buffer
                                                              {
                                                                   Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                              }	
                                                              SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//²Á³öÉÈÇø
                                                              SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,24);//±£´æÏµÊý		
                                                              W5500_Send_Data(s,con_TranE2W);//ÕýÈ·Ó¦´ð
                                                          }
                                                          else if(Rx_Buffer[6] == 'R')
                                                          {
                                                              SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
                                                              Tx_Buffer[0] = 27;  //Ö¡³¤¶È µÍÎ»
                                                              Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
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
                                                              W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ	
                                                              printf("\r\nError:The command E2 is false!!!E2\r\n");
                                                          }
                                                      }
                                                      else if(Rx_Buffer[5] == 'I')
                                                      {
                                                          if(Rx_Buffer[6] == 'W')
                                                          {
                                                              for(i=0;i<26;i++) //°ÑËùÓÐÊý¾Ý´æ´¢µÄÁÙÊ±±äÁ¿Parameter_Buffer
                                                              {
                                                                   Parameter_Buffer[i] = Rx_Buffer[i+7];
                                                              }	
                                                              SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//²Á³öÉÈÇø
                                                              SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterIP_Flash_Address,26);//±£´æÏµÊý		
                                                              W5500_Send_Data(s,con_TranEIW);//ÕýÈ·Ó¦´ð
                                                          }
                                                          else if(Rx_Buffer[6] == 'R')
                                                          {
                                                              SPI_FLASH_BufferRead(Parameter_Buffer,ParameterIP_Flash_Address,26);
                                                              Tx_Buffer[0] = 29;  //Ö¡³¤¶È µÍÎ»
                                                              Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
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
                                                              W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ	
                                                              printf("\r\nError:The command EII is false!!!EI\r\n");
                                                          }
                                                      }
                                                      else
                                                      {
                                                           W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ	
                                                           printf("\r\nError:The command E is false!!!E\r\n");
                                                      }
                                                      break;
                                        }
                                        case 'T':
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {																																																		
                                                          case 'W'://¾²Ì¬ÂÖÖØ¼ì²â
                                                          {																												 																												 
                                                                         if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
                                                                         {
                                                                             DynamicWeight_Number = Rx_Buffer[6] - 0x30;
                                                                             System_State = DynamicWeight_State; //ÏµÍ³½øÈë¶¯Ì¬³ÆÖØ
                                                                             W5500_Send_Data(s,con_TranAACK);  //ÕýÈ·Ó¦´ð	
                                                                             Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                                              WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else if(Rx_Buffer[6] == 0x31)
                                                                         {
                                                                            DynamicWeight_Number = 1;
                                                                            System_State = StaticWeight_State; //ÏµÍ³½øÈë¾²Ì¬³ÆÖØ
                                                                             W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                             Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                                             WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else
                                                                         {
                                                                              W5500_Send_Data(s,con_TranXACK);	//ÎÞÐ§Ö¡Ö¸Áî
                                                                         }
                                                                         break;
                                                          }
                                                          case 'A'://¶¯Ì¬Ì¬ÂÖÖØ¼ì²â
                                                          {
                                                                         if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
                                                                         {
                                                                             DynamicWeight_Number = Rx_Buffer[6] - 0x30;
                                                                             System_State = DynamicWeight_State; //ÏµÍ³½øÈë¶¯Ì¬³ÆÖØ
                                                                             W5500_Send_Data(s,con_TranAACK);  //ÕýÈ·Ó¦´ð	
                                                                             Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                                              WorkState_Flg = StaticWeight_State_Flg;
                                                                         }
                                                                         else
                                                                         {
                                                                              W5500_Send_Data(s,con_TranXACK);	//ÎÞÐ§Ö¡Ö¸Áî
                                                                         }
                                                                         break;
                                                          }
                                                          case 'L':  //Ä¦ÖØ¼ì²â
                                                          {																							 
                                                                         System_State = StaticWeight_Motor_State; //ÏµÍ³½øÈëÄ¦ÍÐ³ÆÖØ
                                                                         W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                         Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                                         WorkState_Flg = StaticWeight_Motor_State_Flg;
                                                                         break;
                                                          }																																																															
                                                      }
                                                      break;
                                        }
                                        default :
                                                      W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ																								 
                                                      printf("\r\nError:The command is false!!!\r\n");
                                                      break;
                                    }
                              }
                              else if(WorkState_Flg == Adjust_State_Flg)   //±ê¶¨×´Ì¬
                              {
                                    switch(u8_Command)
                                    {
                                        case 'O'://ÐÄÌøÖ¸Áî
                                        {
                                                      if(s==0)
                                                      {
                                                          Network0_StopFlg = 0;
                                                          W5500_Send_Data(s,con_TranO);	//ÐÄÌø
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
                                                          Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
                                                          Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
                                                          Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
                                                          Tx_Buffer[2] = 'H'; 
                                                          Tx_Buffer[3] = 'A'; 
                                                          memcpy(&Tx_Buffer[4],cID_Key,10);
                                                          W5500_Send_Data(s,Tx_Buffer);//»Ø´«
                                                      }
                                                      if(Rx_Buffer[5]  == 'E')
                                                      {
                                                          if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                          {
                                                              cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
                                                              if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
                                                              {
                                                                  Encryption_Cnt = 0;
                                                                  W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
                                                                  USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                  USER_SystemFlag.A.Flag_Standalone = 1;  //ºÍÊýÂë¹ÜÁªÍø³É¹¦
                                                              }
                                                              else
                                                              {
                                                                  Encryption_Cnt ++;																											
                                                                  W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
                                                              }
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
                                                          }
                                                          
                                                      }
                                                      break;
                                         }
                                        case 'S'://È¡×´Ì¬
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
                                                                        W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }											
                                        case 'R'://Ç¿ÖÆ¸´Î»Ö¸Áî »Øµ½´ý»ú×´Ì¬
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
                                                      Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                      WorkState_Flg = Default_State_Flg;  //´ý»ú×´Ì
                                                      W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
                                                      return;
                                        }																								
                                        case 'B'://°æ±¾ÐÅÏ¢
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//µ±Ç°°æ±¾ÐÅÏ¢
                                                      break;
                                        }																																
                                        case 'C'://ÐÅºÅ²âÊÔ½çÃæ
                                        {
                                                      if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
                                                                                                    break;	
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == 'E')  //ÍË³ö
                                                      {
                                                          System_State = Default_State;
                                                          WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
                                                          W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                      }
                                                      break;
                                        }
                                        case 'A': //±ê¶¨×´Ì¬
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'C'://½øÈë±ê¶¨×´Ì¬
                                                                        {				
                                                                            System_State = Adjust_State;
                                                                            WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:
                                                                                                                    AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
                                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                                    break;																																
                                                                                default:
                                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                                    W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                                                                                    AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
                                                                                                                    break;	
                                                                            }
                                                                            break;
                                                                        }
                                                          case 'E'://ÍË³ö±ê¶¨×´Ì¬
                                                                        {
                                                                            System_State = Default_State;
                                                                            WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
                                                                            W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                            break;
                                                                        }
                                                          case 'W'://±£´æ±ê¶¨ÏµÊý
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
                                                                                                                  Cabrate_Computer();  //¼ÆËã±ê¶¨ÏµÊý
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//²Á³öÉÈÇø
                                                                                                                  SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//±£´æÏµÊý																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//²Á³öÉÈÇø
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//±£´æ±ê¶¨µãÊý
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
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
                                                                                                                  Cabrate_Computer();  //¼ÆËã±ê¶¨ÏµÊý
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
                                                                                                                  Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
                                                                                                                                                                                  
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//²Á³öÉÈÇø
                                                                                                                  SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//±£´æÏµÊý																																																
                                                                                                                  SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//²Á³öÉÈÇø
                                                                                                                  SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//±£´æ±ê¶¨µãÊý
                                                                                                                  W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                                  break;																																
                                                                                default:
                                                                                                                    printf("\r\nError:The command channel ID is false!!!xie\r\n");
                                                                                                                    W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                                                                                    break;
                                                                            }
                                                                            break;
                                                                        }
                                                          case 'R'://¶Á±ê¶¨ÏµÊý
                                                                        {
                                                                            u8read_flg = 1;//¶Á±ê¶¨ÏµÊý³É¹¦
                                                                            switch(Rx_Buffer[6])
                                                                            {
                                                                                case Left_Weight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
                                                                                                                    break;
                                                                                case Right_Wight_ID:
                                                                                                                    SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
                                                                                                                    break;																																
                                                                                default:
                                                                                                                    u8read_flg = 0;//¶Á±ê¶¨ÏµÊýÊ§°Ü
                                                                                                                    printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
                                                                                                                    break;
                                                                            }			
                                                                            if(u8read_flg == 1)//¶Á±ê¶¨ÏµÊý³É¹¦
                                                                            {
                                                                                Tx_Buffer[0] = 21;//Ö¡³¤¶È µÍÎ»
                                                                                Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
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
                                                                                W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                                            }
                                                                            break;
                                                                        }
                                                           default :
                                                                       W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                                       break;
                                                      }
                                                      break;
                                        }
                                        case 'Y': //µ÷Áã
                                        {
                                                      if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                      {
                                                          Fun_Zero(Rx_Buffer[5],1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //Ç¿ÖÆÇåÁã
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                      }
                                                      break;
                                        }																					
                                        default :
                                                      W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ																								 
                                                      printf("\r\nError:The command is false!!!\r\n");
                                                      break;
                                    }
                              }
                              else if(WorkState_Flg == SignalTest_State_Flg)  //ÐÅºÅ²âÊÔ×´Ì¬
                              {
                                  switch(u8_Command)
                                  {
                                        case 'O'://ÐÄÌøÖ¸Áî
                                        {
                                                      if(s==0)
                                                      {
                                                          Network0_StopFlg = 0;
                                                          W5500_Send_Data(s,con_TranO);	//ÐÄÌø
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
                                                          Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
                                                          Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
                                                          Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
                                                          Tx_Buffer[2] = 'H'; 
                                                          Tx_Buffer[3] = 'A'; 
                                                          memcpy(&Tx_Buffer[4],cID_Key,10);
                                                          W5500_Send_Data(s,Tx_Buffer);//»Ø´«
                                                      }
                                                      if(Rx_Buffer[5]  == 'E')
                                                      {
                                                          if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                          {
                                                              cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
                                                              if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
                                                              {
                                                                  Encryption_Cnt = 0;
                                                                  W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
                                                                  USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                  USER_SystemFlag.A.Flag_Standalone = 1;  //ºÍÊýÂë¹ÜÁªÍø³É¹¦
                                                              }
                                                              else
                                                              {
                                                                  Encryption_Cnt ++;																											
                                                                  W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
                                                              }
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
                                                          }
                                                          
                                                      }
                                                      break;
                                         }
                                        case 'S'://È¡×´Ì¬
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
                                                                        W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }												
                                        case 'R'://Ç¿ÖÆ¸´Î»Ö¸Áî »Øµ½´ý»ú×´Ì¬
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
                                                      Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                      WorkState_Flg = Default_State_Flg;  //´ý»ú×´Ì
                                                      W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
                                                      return;
                                        }																			
                                        case 'B'://°æ±¾ÐÅÏ¢
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//µ±Ç°°æ±¾ÐÅÏ¢
                                                      break;
                                        }
                                        case 'C'://ÐÅºÅ²âÊÔ½çÃæ
                                        {
                                                      if(Rx_Buffer[5] == 0)
                                                      {
                                                            if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
                                                            {
                                                                Signal_TransferTime = Rx_Buffer[6] - 0x30;
                                                                System_State = SignalTest_State;
                                                                WorkState_Flg = SignalTest_State_Flg;    //ÐÅºÅ²âÊÔ×´Ì¬
                                                                W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                            }
                                                            else
                                                            {
                                                                W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                            }
                                                      }
                                                      else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
                                                      {																						
                                                            switch(Rx_Buffer[5])
                                                            {
                                                                case Left_Weight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
                                                                                                    AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                    break;
                                                                case Right_Wight_ID:
                                                                                                    System_State = Adjust_State;
                                                                                                    WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
                                                                                                    AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
                                                                                                    W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                                                                    break;																																
                                                                default:
                                                                                                    printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
                                                                                                    W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                // 																																										AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
                                                                                                    break;	
                                                            }
                                                      }
                                                      else if(Rx_Buffer[5] == 'E')
                                                      {
                                                          System_State = Default_State;
                                                          WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
                                                          W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                      }
                                                      break;
                                        }
                                        case 'Y': //µ÷Áã
                                        {
                                                      if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
                                                      {
                                                          Fun_Zero(Rx_Buffer[5],1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //Ç¿ÖÆÇåÁã
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
                                                      else
                                                      {
                                                          W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
                                                      }
                                                      break;
                                        }																								
                                        default :
                                                      W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ																								 
                                                      printf("\r\nError:The command is false!!!\r\n");
                                                      break;
                                    }
                              }
                              else   //¼ì²â×´Ì¬
                              {
                                    switch(u8_Command)
                                    {
                                        case 'O'://ÐÄÌøÖ¸Áî
                                        {
                                                      if(s==0)
                                                      {
                                                          Network0_StopFlg = 0;
                                                          W5500_Send_Data(s,con_TranO);	//ÐÄÌø
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
                                                          Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
                                                          Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
                                                          Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
                                                          Tx_Buffer[2] = 'H'; 
                                                          Tx_Buffer[3] = 'A'; 
                                                          memcpy(&Tx_Buffer[4],cID_Key,10);
                                                          W5500_Send_Data(s,Tx_Buffer);//»Ø´«
                                                      }
                                                      if(Rx_Buffer[5]  == 'E')
                                                      {
                                                          if(Encryption_Cnt <= ENCRYPTION_MAX)
                                                          {
                                                              cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
                                                              if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
                                                              {
                                                                  Encryption_Cnt = 0;
                                                                  W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
                                                                  USER_SystemFlag.A.Flag_Sucess1 = 1;
                                                                  USER_SystemFlag.A.Flag_Standalone = 1;  //ºÍÊýÂë¹ÜÁªÍø³É¹¦
                                                              }
                                                              else
                                                              {
                                                                  Encryption_Cnt ++;																											
                                                                  W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
                                                              }
                                                          }
                                                          else
                                                          {
                                                              W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
                                                          }
                                                          
                                                      }
                                                      break;
                                         }
                                        case 'S'://È¡×´Ì¬
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
                                                                        W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
                                                                        break;
                                                            
                                                      }
                                                      break;
                                        }
                                        case 'U'://Ç¿ÖÆÊÕÎ²Ö¸Áî »Øµ½´ý»ú×´Ì¬
                                        {
                                                      W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
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
                                                      Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                      WorkState_Flg = Default_State_Flg;  //´ý»ú×´Ì¬
                                                      W5500_Send_Data(s,con_TranU);				//½áÊø
                                                      return;
                                        }
                                        case 'R'://Ç¿ÖÆ¸´Î»Ö¸Áî »Øµ½´ý»ú×´Ì¬
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
                                                      Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
                                                      WorkState_Flg = Default_State_Flg;  //´ý»ú×´Ì
                                                      W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
                                                      return;
                                        }																						
                                        case 'B'://°æ±¾ÐÅÏ¢
                                        {
                                                      W5500_Send_Data(s,con_TranB);				//µ±Ç°°æ±¾ÐÅÏ¢
                                                      break;
                                        }											
                                        case 'G'://È¡ÊµÊ±Êý¾Ý
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
                                                                         FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //ÉÏ´«¶¯Ì¬ÏÔÊ¾ÂÖÖØ
                                                                        break;
                                                          }																																																																																																										
                                                          default :
                                                                       W5500_Send_Data(s,con_TranXACK); //µ±Ç°Ö¸ÁîÎÞÐ§
                                                                       break;
                                                      }
                                                      break;
                                        }																						
                                        case 'D'://È¡²âÊÔ½á¹û
                                        {
                                                      switch(Rx_Buffer[5])
                                                      {
                                                          case 'W'://¾²Ì¬ÂÖÖØ½á¹û
                                                          {
                                                                        FunW5500_SendDW(s);
                                                                        break;
                                                          }
                                                          case 'A'://¶¯Ì¬ÖáºÉ½á¹û
                                                          {
                                                                        break;
                                                          }
                                                          case 'L'://·¢ËÍÄ¦ÍÐ³µÖØÁ¿½á¹û
                                                          {
                                                                        Tx_Buffer[0] = 8;    //Ö¡³¤¶È µÍÎ»
                                                                        Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
                                                                        Tx_Buffer[2] = 'D'; 
                                                                        Tx_Buffer[3] = 'L'; 
                                                                        /* Ä¦ÍÐ³ÆÖØ */
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
                                                                        W5500_Send_Data(s,Tx_Buffer);				//·¢ËÍµ±Ç°Ä¦ÍÐÖØÁ¿
                                                                        break;
                                                          }																																																																																																									
                                                          default:
                                                                        W5500_Send_Data(s,con_TranXACK); //µ±Ç°Ö¸ÁîÎÞÐ§
                                                                        break;
                                                      }
                                                      break;
                                        }
                                        default :
                                                      W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ																								 
                                                      printf("\r\nError:The command is false!!!\r\n");
                                                      break;
                                    }
                              }
                          }
										}
										else if(s==2)
										{
												 if(USER_SystemFlag.A.Flag_Sucess == 0) //ÎÕÊÖ¼ÓÃÜ
												 {
															switch(u8_Command)
															{
																	case 'O'://ÐÄÌøÖ¸Áî
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
																										Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
																										Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
																										Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
																										Tx_Buffer[2] = 'H'; 
																										Tx_Buffer[3] = 'A'; 
																										memcpy(&Tx_Buffer[4],cID_Key,10);
																										W5500_Send_Data(s,Tx_Buffer);//»Ø´«
																								}
																								if(Rx_Buffer[5]  == 'E')
																								{
																										if(Encryption_Cnt <= ENCRYPTION_MAX)
																										{
																												cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
																												if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
																												{
																														Encryption_Cnt = 0;
																														W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
																														USER_SystemFlag.A.Flag_Sucess = 1;
                                                            USER_SystemFlag.A.Flag_Online = 1;  //ºÍÖ÷¿ØÁªÍø³É¹¦
                                                            USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
																												}
																												else
																												{
																														Encryption_Cnt ++;																											
																														W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
																												}
																										}
																										else
																										{
																												W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
																										}
																										
																								}
																								break;
																	 }
																	default :
																								W5500_Send_Data(s,con_TranHX0);//»Ø´«Ö¸Áî´íÎó
																								break;
															}
												 }
												 else
												 {
															if(WorkState_Flg == Default_State_Flg)   //´ý»ú×´Ì¬
															{
																		switch(u8_Command)
																		{				
																				case 'V'://×Ô¼ì
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
																													Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
																													Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
																													Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//»Ø´«
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
																															if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //ºÍÖ÷¿ØÁªÍø³É¹¦
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
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
																										 W5500_Send_Data(s,con_TranWE);				//»Ø´«Òì³£´íÎó
																										 return;	
																								}
																								WFLASHStatus=FLASH_ProgramWord(2048 * 255, 0X77777777);
																								if(WFLASHStatus!=FLASH_COMPLETE)                                  //???????:???E0
																								{
																										FLASH_Lock();
																										W5500_Send_Data(s,con_TranWE);				//»Ø´«Òì³£´íÎó
																										return;	
																								}
																								if(FLASH_Read_Word(2048 * 255)==0X77777777)
																								{
																										FLASH_Lock();
																										W5500_Send_Data(s,con_TranWG);				//»Ø´«Éý¼¶Ó¦´ð	
																										Delay_ms(10);                                                //10ms??	
																										NVIC_SystemReset();
																								}
																								else                                                                //
																								{
																											FLASH_Lock();
																											W5500_Send_Data(s,con_TranWE);			//»Ø´«Òì³£´íÎó
																								 }
																						}		
																						break;
																				}																
																				case 'O'://ÐÄÌøÖ¸Áî
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
																				case 'S'://È¡×´Ì¬
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
																																				W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
																																				break;
																														
																											}
																											break;
																				}
																				case 'U'://Ç¿ÖÆÊÕÎ²Ö¸Áî »Øµ½´ý»ú×´Ì¬
																				{
																											W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
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
																											Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											WorkState_Flg = Default_State_Flg;  //´ý»ú×´Ì¬
																											W5500_Send_Data(s,con_TranU);				//½áÊø
																											return;
																				}
																				case 'R'://Ç¿ÖÆ¸´Î»Ö¸Áî »Øµ½´ý»ú×´Ì¬
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
																											Delay_ms(10);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											WorkState_Flg = Default_State_Flg;  //´ý»ú×´tai
																											W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
																											Delay_ms(10);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											W5500_Send_Data(s,con_TranR);				//¸´Î»Íê³É
																											return;
																				}
																				case 'I'://³õÊ¼»¯Ö¸Áî
																				{
																											if(Rx_Buffer[5]  == 'W')
																											{
																													W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
																													for(i=0;i<6;i++) //»º´æ³õÊ¼»¯
																													{
																															Left_DynamicWeight1[i] = 0;
																															Right_DynamicWeight1[i] = 0;
																													}
																													Left_StaticWeight = 0;				//×óÂÖ¾²Ì¬ÖáÖØ 
																													Right_StaticWeight = 0;  		//ÓÒÂÖ¾²Ì¬ÖáÖØ
																													Motor_StaticWeight = 0; 		//Ä¦ÍÐ¾²Ì¬ÖáÖØ ×óÌ¨¼Ü
																													Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî												
																													W5500_Send_Data(s,con_TranI);				//ÕýÈ·Ó¦´ð																
																											}																			
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
																											}
																											break;
																				}												
																				case 'B'://°æ±¾ÐÅÏ¢
																				{
																											W5500_Send_Data(s,con_TranB);				//µ±Ç°°æ±¾ÐÅÏ¢
																											break;
																				}
																				case 'C'://ÐÅºÅ²âÊÔ½çÃæ
																				{
																											if(Rx_Buffer[5] == 0)
																											{
																														if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
																														{
																																Signal_TransferTime = Rx_Buffer[6] - 0x30;
																																System_State = SignalTest_State;
																																WorkState_Flg = SignalTest_State_Flg;    //ÐÅºÅ²âÊÔ×´Ì¬
																																W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																														}
																											}
																											else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
																																																		AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
																																																		W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
																																																		AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
																																																		W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
																																																		break;	
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
																													W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																											}
																											else if(Rx_Buffer[5] == 'X')
																											{
																													USER_SystemFlag.A.Flag_TestC1 = 1;
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																											}
																											break;
																				}
																				case 'D'://È¡²âÊÔ½á¹û
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'W'://¾²Ì¬ÂÖÖØ½á¹û
																													{																												
																																				FunW5500_SendDW(s);				//·¢ËÍµ±Ç°ÂÖÖØ
																																				break;
																													}
																													case 'A'://¶¯Ì¬ÖáºÉ½á¹û
																													{																												
																																				break;
																													}
																													case 'L'://·¢ËÍÄ¦ÍÐ³µÖØÁ¿½á¹û
																													{
																																				Tx_Buffer[0] = 8;    //Ö¡³¤¶È µÍÎ»
																																				Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
																																				Tx_Buffer[2] = 'D'; 
																																				Tx_Buffer[3] = 'L'; 
																																				/* Ä¦ÍÐ³ÆÖØ */
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
																																				W5500_Send_Data(s,Tx_Buffer);				//·¢ËÍµ±Ç°Ä¦ÍÐÖØÁ¿
																																				break;
																													}																																																																																																									
																													default:
																																				W5500_Send_Data(s,con_TranXACK); //µ±Ç°Ö¸ÁîÎÞÐ§
																																				break;
																											}
																											break;
																				}
																				case 'G'://È¡ÊµÊ±Êý¾Ý
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
																																				FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //ÉÏ´«¶¯Ì¬ÏÔÊ¾ÂÖÖØ
																																				break;
																													}																																																																																																										
																													default :
																																			 W5500_Send_Data(s,con_TranXACK); //µ±Ç°Ö¸ÁîÎÞÐ§
																																			 break;
																											}
																											break;
																				}
																				case 'A': //±ê¶¨×´Ì¬
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'C'://½øÈë±ê¶¨×´Ì¬
																																				{				
																																						System_State = Adjust_State;
																																						WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:
																																																										AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
																																																										W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																										break;
																																								case Right_Wight_ID:
																																																										AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
																																																										W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																										break;																																
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																																																										AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
																																																										break;	
																																						}
																																						break;
																																				}
																													case 'E'://ÍË³ö±ê¶¨×´Ì¬
																																				{
																																						System_State = Default_State;
																																						WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
																																						W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																						break;
																																				}
																													case 'W'://±£´æ±ê¶¨ÏµÊý
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
																																																									Cabrate_Computer();  //¼ÆËã±ê¶¨ÏµÊý
																																																									Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//²Á³öÉÈÇø
																																																									SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//±£´æÏµÊý																																																
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//²Á³öÉÈÇø
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//±£´æ±ê¶¨µãÊý
																																																									W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
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
																																																									Cabrate_Computer();  //¼ÆËã±ê¶¨ÏµÊý
																																																									Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//²Á³öÉÈÇø
																																																									SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//±£´æÏµÊý																																																
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//²Á³öÉÈÇø
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//±£´æ±ê¶¨µãÊý
																																																									W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																									break;																																
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!xie\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																																																										break;
																																						}
																																						break;
																																				}
																													case 'R'://¶Á±ê¶¨ÏµÊý
																																				{
																																						u8read_flg = 1;//¶Á±ê¶¨ÏµÊý³É¹¦
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
																																																										break;
																																								case Right_Wight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
																																																										break;																																
																																								default:
																																																										u8read_flg = 0;//¶Á±ê¶¨ÏµÊýÊ§°Ü
																																																										printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
																																																										break;
																																						}			
																																						if(u8read_flg == 1)//¶Á±ê¶¨ÏµÊý³É¹¦
																																						{
																																								Tx_Buffer[0] = 21;//Ö¡³¤¶È µÍÎ»
																																								Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
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
																																								W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																																						}
																																						break;
																																				}
																													 default :
																																			 W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																																			 break;
																											}
																											break;
																				}
																				case 'Y': //µ÷Áã
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //Ç¿ÖÆÇåÁã
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																											}
																											break;
																				}
																				case 'E'://²ÎÊý¶ÁÐ´
																				{
																											if(Rx_Buffer[5] == '1')
																											{
																														if(Rx_Buffer[6] == 'W')
																														{																							
																																for(i=0;i<(Rx_Buffer[7]*3+1);i++) //°ÑËùÓÐÊý¾Ý´æ´¢µÄÁÙÊ±±äÁ¿u8flash_buffer
																																{
																																		 Parameter_Buffer[i] = Rx_Buffer[i+7];
																																}								
																																if(Fun_DealParameter()==1)
																																{
																																		SPI_FLASH_SectorErase_A(ParameterAll_Flash_Address);		//²Á³öÉÈÇø
																																		SPI_FLASH_SectorWrite(Parameter_Buffer,ParameterAll_Flash_Address,(Rx_Buffer[7]*3+1));//±£´æÏµÊý		
																																		W5500_Send_Data(s,con_TranE1W);//ÕýÈ·Ó¦´ð
																																}
																																else
																																{
																																		W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ
																																		printf("\r\nError:Write Parameter false!!!E1W\r\n");
																																}
																														}
																														else if(Rx_Buffer[6] == 'R')
																														{												
																																	SPI_FLASH_BufferRead(Parameter_Buffer,ParameterAll_Flash_Address,100);
																																	Tx_Buffer[0] = Parameter_Buffer[0]*3+4;  //Ö¡³¤¶È µÍÎ»
																																	Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
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
																																W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ	
																																printf("\r\nError:The command E1 is false!!!E1\r\n");
																														}
																											}
																											else if(Rx_Buffer[5] == '2')
																											{
																													if(Rx_Buffer[6] == 'W')
																													{
																															for(i=0;i<24;i++) //°ÑËùÓÐÊý¾Ý´æ´¢µÄÁÙÊ±±äÁ¿Parameter_Buffer
																															{
																																	 Parameter_Buffer[i] = Rx_Buffer[i+7];
																															}	
																															SPI_FLASH_SectorErase_A(Parameter15_Flash_Address);		//²Á³öÉÈÇø
																															SPI_FLASH_SectorWrite(Parameter_Buffer,Parameter15_Flash_Address,24);//±£´æÏµÊý		
																															W5500_Send_Data(s,con_TranE2W);//ÕýÈ·Ó¦´ð
																													}
																													else if(Rx_Buffer[6] == 'R')
																													{
																															SPI_FLASH_BufferRead(Parameter_Buffer,Parameter15_Flash_Address,24);
																															Tx_Buffer[0] = 27;  //Ö¡³¤¶È µÍÎ»
																															Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
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
																															W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ	
																															printf("\r\nError:The command E2 is false!!!E2\r\n");
																													}
																											}
																											else if(Rx_Buffer[5] == 'I')
																											{
																														if(Rx_Buffer[6] == 'W')
																														{
																																for(i=0;i<26;i++) //°ÑËùÓÐÊý¾Ý´æ´¢µÄÁÙÊ±±äÁ¿Parameter_Buffer
																																{
																																		 W5500_Parameter[i] = Rx_Buffer[i+7];
																																}	
																																W5500_Parameter[26] = 0x55;
																																SPI_FLASH_SectorErase_A(ParameterIP_Flash_Address);		//²Á³öÉÈÇø
																																SPI_FLASH_SectorWrite(W5500_Parameter,ParameterIP_Flash_Address,27);//±£´æÏµÊý		
																																W5500_Send_Data(s,con_TranEIW);//ÕýÈ·Ó¦´ð
																																ucW5500_InitCTR = 0;
																																S0_State = 0;
																																S1_State = 0;
																																S2_State = 0;																														    
																																Load_Net_Parameters();
																														}
																														else if(Rx_Buffer[6] == 'R')
																														{
																																SPI_FLASH_BufferRead(Parameter_Buffer,ParameterIP_Flash_Address,26);
																																Tx_Buffer[0] = 29;  //Ö¡³¤¶È µÍÎ»
																																Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
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
																																W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ	
																																printf("\r\nError:The command EII is false!!!EI\r\n");
																														}
																											}
																											else
																											{
																													 W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ	
																													 printf("\r\nError:The command E is false!!!E\r\n");
																											}
																											break;
																				}
																				case 'T':
																				{
																											switch(Rx_Buffer[5])
																											{																																																		
																													case 'W'://¾²Ì¬ÂÖÖØ¼ì²â
																													{																												 																												 
																																				 if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //ÏµÍ³½øÈë¶¯Ì¬³ÆÖØ
																																								  W5500_Send_Data(s,con_TranAACK);  //ÕýÈ·Ó¦´ð	
																																								  Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //²»Ö÷¶¯ÉÏ´«½á¹û
																																									USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //ÏµÍ³½øÈë¶¯Ì¬³ÆÖØ
																																								  W5500_Send_Data(s,con_TranAACK);  //ÕýÈ·Ó¦´ð	
																																								  Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //Ö÷¶¯ÉÏ´«½á¹û
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //²»Ö÷¶¯ÉÏ´«½á¹û
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//Ö¸ÁîÎÞÐ§
																																							}																																					 																																					 
																																				 }
																																				 else if(Rx_Buffer[6] == 0x31)
																																				 {
																																						 if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //ÏµÍ³½øÈë¾²Ì¬³ÆÖØ
																																									W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																									Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //²»Ö÷¶¯ÉÏ´«½á¹û
																																									USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //ÏµÍ³½øÈë¾²Ì¬³ÆÖØ
																																									W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																									Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //Ö÷¶¯ÉÏ´«½á¹û
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //²»Ö÷¶¯ÉÏ´«½á¹û
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//Ö¸ÁîÎÞÐ§
																																							}																																						 
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//ÎÞÐ§Ö¡Ö¸Áî
																																				 }
																																				 if(USER_SystemFlag.A.Flag_Online == 1) //ºÍÖ÷¿ØÁªÍø ÉÏ´«ÄÚÈÝ
																																				 {
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍÊýÂë¹ÜÁªÍøÁªÍø ÉÏ´«ÄÚÈÝ
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //ÎÕÊÖ¼ÓÃÜ
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//ÊýÂë¹ÜÊý¾ÝÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
																																									USER_LEDFlag.A.LED_14 = 1;  				//ÊýÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
																																									USER_LEDFlag.A.LED_Weight = 1;  			//ÖØÁ¿Ö¸Ê¾µÆ
																																									USER_LEDFlag.A.LED_Brake = 0;  			  //ÖÆ¶¯Ö¸Ê¾µÆ
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																													case 'A'://¶¯Ì¬Ì¬ÂÖÖØ¼ì²â
																													{
																																				 if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
																																				 {
																																						 DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																						 System_State = DynamicWeight_State; //ÏµÍ³½øÈë¶¯Ì¬³ÆÖØ
																																						 W5500_Send_Data(s,con_TranAACK);  //ÕýÈ·Ó¦´ð	
																																						 Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																							WorkState_Flg = StaticWeight_State_Flg;																																						
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//ÎÞÐ§Ö¡Ö¸Áî
																																				 }
																																				 break;
																													}
																													case 'L':  //Ä¦ÖØ¼ì²â
																													{																							 
																																				 System_State = StaticWeight_Motor_State; //ÏµÍ³½øÈëÄ¦ÍÐ³ÆÖØ
																																				 W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																				 Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																				 WorkState_Flg = StaticWeight_Motor_State_Flg;
																																				 break;
																													}																																																															
																											}
																											break;
																				}
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															}
															else if(WorkState_Flg == Adjust_State_Flg)   //±ê¶¨×´Ì¬
															{
																		switch(u8_Command)
																		{
																				case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
																													Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
																													Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//»Ø´«
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
																															if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //ºÍÖ÷¿ØÁªÍø³É¹¦
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://×Ô¼ì
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
																				case 'O'://ÐÄÌøÖ¸Áî
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
																				case 'S'://È¡×´Ì¬
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
																																				W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
																																				break;
																														
																											}
																											break;
																				}											
																				case 'R'://Ç¿ÖÆ¸´Î»Ö¸Áî »Øµ½´ý»ú×´Ì¬
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
																											Delay_ms(10);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											WorkState_Flg = Default_State_Flg;  //´ý»ú×´tai
																											W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
																											Delay_ms(10);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											W5500_Send_Data(s,con_TranR);				//¸´Î»Íê³É
																											return;
																				}																								
																				case 'B'://°æ±¾ÐÅÏ¢
																				{
																											W5500_Send_Data(s,con_TranB);				//µ±Ç°°æ±¾ÐÅÏ¢
																											break;
																				}																																
																				case 'C'://ÐÅºÅ²âÊÔ½çÃæ
																				{
																											if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
																																																		AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
																																																		W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
																																																		AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
																																																		W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
																																																		break;	
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')  //ÍË³ö
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
																													W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																											}
																											break;
																				}
																				case 'A': //±ê¶¨×´Ì¬
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'C'://½øÈë±ê¶¨×´Ì¬
																																				{				
																																						System_State = Adjust_State;
																																						WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:
																																																										AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
																																																										W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																										break;
																																								case Right_Wight_ID:
																																																										AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
																																																										W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																										break;																																
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																																																										AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
																																																										break;	
																																						}
																																						break;
																																				}
																													case 'E'://ÍË³ö±ê¶¨×´Ì¬
																																				{
																																						System_State = Default_State;
																																						WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
																																						W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																						break;
																																				}
																													case 'W'://±£´æ±ê¶¨ÏµÊý
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
																																																									Cabrate_Computer();  //¼ÆËã±ê¶¨ÏµÊý
																																																									Left_Weight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Left_Weight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Left_Weight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Left_Weight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Left_Weight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address);		//²Á³öÉÈÇø
																																																									SPI_FLASH_SectorWrite(Left_Weight_par.Load_Flash,Left_Weight_Flash_Address,17);//±£´æÏµÊý																																																
																																																									SPI_FLASH_SectorErase_A(Left_Weight_Flash_Address1);		//²Á³öÉÈÇø
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Left_Weight_Flash_Address1,18);//±£´æ±ê¶¨µãÊý
																																																									W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
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
																																																									Cabrate_Computer();  //¼ÆËã±ê¶¨ÏµÊý
																																																									Right_Wight_par.Load_Parameter.Parameter_NB = _ADCabVariable.NbofVar;
																																																									Right_Wight_par.Load_Parameter.Parameter_C0.float_data = _ADCabVariable.Cof[0];
																																																									Right_Wight_par.Load_Parameter.Parameter_C1.float_data = _ADCabVariable.Cof[1];
																																																									Right_Wight_par.Load_Parameter.Parameter_C2.float_data = _ADCabVariable.Cof[2];
																																																									Right_Wight_par.Load_Parameter.Parameter_C3.float_data = _ADCabVariable.Cof[3];
																																																																																									
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address);		//²Á³öÉÈÇø
																																																									SPI_FLASH_SectorWrite(Right_Wight_par.Load_Flash,Right_Wight_Flash_Address,17);//±£´æÏµÊý																																																
																																																									SPI_FLASH_SectorErase_A(Right_Wight_Flash_Address1);		//²Á³öÉÈÇø
																																																									SPI_FLASH_SectorWrite(u8flash_buffer,Right_Wight_Flash_Address1,18);//±£´æ±ê¶¨µãÊý
																																																									W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																									break;																																
																																								default:
																																																										printf("\r\nError:The command channel ID is false!!!xie\r\n");
																																																										W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																																																										break;
																																						}
																																						break;
																																				}
																													case 'R'://¶Á±ê¶¨ÏµÊý
																																				{
																																						u8read_flg = 1;//¶Á±ê¶¨ÏµÊý³É¹¦
																																						switch(Rx_Buffer[6])
																																						{
																																								case Left_Weight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Left_Weight_Flash_Address1,18);
																																																										break;
																																								case Right_Wight_ID:
																																																										SPI_FLASH_BufferRead(u8flash_buffer,Right_Wight_Flash_Address1,18);
																																																										break;																																
																																								default:
																																																										u8read_flg = 0;//¶Á±ê¶¨ÏµÊýÊ§°Ü
																																																										printf("\r\nError:The command channel ID is false!!!du\r\n");																																													
																																																										break;
																																						}			
																																						if(u8read_flg == 1)//¶Á±ê¶¨ÏµÊý³É¹¦
																																						{
																																								Tx_Buffer[0] = 21;//Ö¡³¤¶È µÍÎ»
																																								Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
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
																																								W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																																						}
																																						break;
																																				}
																													 default :
																																			 W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																																			 break;
																											}
																											break;
																				}
																				case 'Y': //µ÷Áã
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<0x09))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //Ç¿ÖÆÇåÁã
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																											}
																											break;
																				}																					
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															}
															else if(WorkState_Flg == SignalTest_State_Flg)  //ÐÅºÅ²âÊÔ×´Ì¬
															{
																	switch(u8_Command)
																	{			
																			  case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
																													Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
																													Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//»Ø´«
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
																															if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //ºÍÖ÷¿ØÁªÍø³É¹¦
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://×Ô¼ì
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
																				case 'O'://ÐÄÌøÖ¸Áî
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
																				case 'S'://È¡×´Ì¬
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
																																				W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
																																				break;
																														
																											}
																											break;
																				}												
																				case 'R'://Ç¿ÖÆ¸´Î»Ö¸Áî »Øµ½´ý»ú×´Ì¬
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
																											Delay_ms(10);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											WorkState_Flg = Default_State_Flg;  //´ý»ú×´tai
																											W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
																											Delay_ms(10);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											W5500_Send_Data(s,con_TranR);				//¸´Î»Íê³É
																											return;
																				}																			
																				case 'B'://°æ±¾ÐÅÏ¢
																				{
																											W5500_Send_Data(s,con_TranB);				//µ±Ç°°æ±¾ÐÅÏ¢
																											break;
																				}
																				case 'C'://ÐÅºÅ²âÊÔ½çÃæ
																				{
																											if(Rx_Buffer[5] == 0)
																											{
																														if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x3A))
																														{
																																Signal_TransferTime = Rx_Buffer[6] - 0x30;
																																System_State = SignalTest_State;
																																WorkState_Flg = SignalTest_State_Flg;    //ÐÅºÅ²âÊÔ×´Ì¬
																																W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																														}
																														else
																														{
																																W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																														}
																											}
																											else if((Rx_Buffer[5] > 0)&&(Rx_Buffer[5] < 9))
																											{																						
																														switch(Rx_Buffer[5])
																														{
																																case Left_Weight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
																																																		AD_Channel_ID = Left_Weight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ1Í¨µÀ
																																																		W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																		break;
																																case Right_Wight_ID:
																																																		System_State = Adjust_State;
																																																		WorkState_Flg = Adjust_State_Flg;    //±ê¶¨×´Ì¬
																																																		AD_Channel_ID = Right_Wight_ID;	//¶ÔÓ¦Ä£ÄâÍ¨µÀ2Í¨µÀ
																																																		W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																																		break;																																
																																default:
																																																		printf("\r\nError:The command channel ID is false!!!biaoding\r\n");
																																																		W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
								// 																																										AD_Channel_ID = Left_Weight_ID; 	//Ä¬ÈÏ³ö´íÊ±¶ÔÓ¦Ä£ÄâÍ¨µÀ1 
																																																		break;	
																														}
																											}
																											else if(Rx_Buffer[5] == 'E')
																											{
																													System_State = Default_State;
																													WorkState_Flg = Default_State_Flg;    //´ý»ú×´Ì¬
																													W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																											}
																											break;
																				}
																				case 'Y': //µ÷Áã
																				{
																											if((Rx_Buffer[5]>0x00) && (Rx_Buffer[5]<50))
																											{
																													Fun_Zero(Rx_Buffer[5],1);
																													W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
																											}
                                                      else if(Rx_Buffer[5] == AD_Clear_ID) //Ç¿ÖÆÇåÁã
                                                      {
                                                          Fun_Zero(Left_Weight_ID,1);
                                                          Fun_Zero(Right_Wight_ID,1);
//                                                           Fun_Zero(All_Weight_ID,1);
                                                          W5500_Send_Data(s,con_TranAACK);//µ±Ç°ÃüÁîÓÐÐ§
                                                      }
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																											}
																											break;
																				}																								
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ																								 
																											printf("\r\nError:The command is false!!!\r\n");
																											break;
																		}
															}
															else   //¼ì²â×´Ì¬
															{
																		switch(u8_Command)
																		{
																				case 'H':
																				{
																											if(Rx_Buffer[5]  == 'A')
																											{
																													Get_System_Ciphertext(cID_Key);    //»ñÈ¡Òª¼ÓÃÜ×Ö·û
																													Tx_Buffer[0] = 12;  //Ö¡³¤¶È µÍÎ»
																													Tx_Buffer[1] = 0;  //Ö¡³¤¶È ¸ßÎ»
																													Tx_Buffer[2] = 'H'; 
																													Tx_Buffer[3] = 'A'; 
																													memcpy(&Tx_Buffer[4],cID_Key,10);
																													W5500_Send_Data(s,Tx_Buffer);//»Ø´«
																											}
																											if(Rx_Buffer[5]  == 'E')
																											{
																													if(Encryption_Cnt <= ENCRYPTION_MAX)
																													{
																															cCheck_Result = De_AES(&Rx_Buffer[6],0x31,cID_Key);   //AES½âÃÜ²¢Ð£Ñé
																															if(cCheck_Result == 0)  //¼ÓÃÜÕýÈ·
																															{
																																	Encryption_Cnt = 0;
																																	W5500_Send_Data(s,con_TranHET);//»Ø´«³É¹¦
																																	USER_SystemFlag.A.Flag_Sucess = 1;
                                                                  USER_SystemFlag.A.Flag_Online = 1;  //ºÍÖ÷¿ØÁªÍø³É¹¦
                                                                  USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
																															}
																															else
																															{
																																	Encryption_Cnt ++;																											
																																	W5500_Send_Data(s,con_TranHEF);//»Ø´«Ê§°Ü
																															}
																													}
																													else
																													{
																															W5500_Send_Data(s,con_TranHX1);//»Ø´«¼ÓÃÜ³¬´ÎÊý
																													}
																													
																											}
																											break;
																				 }
																				case 'V'://×Ô¼ì
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
																				case 'O'://ÐÄÌøÖ¸Áî
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
																				case 'S'://È¡×´Ì¬
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
																																				W5500_Send_Data(s,con_TranXACK);				//µ±Ç°ÃüÁîÎÞÐ§
																																				break;
																														
																											}
																											break;
																				}
																				case 'U'://Ç¿ÖÆÊÕÎ²Ö¸Áî »Øµ½´ý»ú×´Ì¬
																				{
																											W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
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
																											Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											WorkState_Flg = Default_State_Flg;  //´ý»ú×´Ì¬
																											W5500_Send_Data(s,con_TranU);				//½áÊø
																											return;
																				}
																				case 'R'://Ç¿ÖÆ¸´Î»Ö¸Áî »Øµ½´ý»ú×´Ì¬
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
																											Delay_ms(10);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											WorkState_Flg = Default_State_Flg;  //´ý»ú×´tai
																											W5500_Send_Data(s,con_TranAACK);				//ÕýÈ·Ó¦´ð
																											Delay_ms(10);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																											W5500_Send_Data(s,con_TranR);				//¸´Î»Íê³É
																											return;
																				}																						
																				case 'B'://°æ±¾ÐÅÏ¢
																				{
																											W5500_Send_Data(s,con_TranB);				//µ±Ç°°æ±¾ÐÅÏ¢
																											break;
																				}											
																				case 'G'://È¡ÊµÊ±Êý¾Ý
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
																																				 FunW5500_SendGW(s,Left_DynamicWeight,Right_DynamicWeight); //ÉÏ´«¶¯Ì¬ÏÔÊ¾ÂÖÖØ
																																				break;
																													}																																																																																																										
																													default :
																																			 W5500_Send_Data(s,con_TranXACK); //µ±Ç°Ö¸ÁîÎÞÐ§
																																			 break;
																											}
																											break;
																				}																						
																				case 'C'://ÐÅºÅ²âÊÔ½çÃæ
																				{																											
																											if(Rx_Buffer[5] == 'X')
																											{
																													USER_SystemFlag.A.Flag_TestC1 = 1;
																											}
																											else
																											{
																													W5500_Send_Data(s,con_TranXACK);//µ±Ç°ÃüÁîÎÞÐ§
																											}
																											break;
																				}
																				case 'T':
																				{
																											switch(Rx_Buffer[5])
																											{																																																		
																													case 'W'://¾²Ì¬ÂÖÖØ¼ì²â
																													{																												 																												 
																																				 if((Rx_Buffer[6]>0x31) && (Rx_Buffer[6]<0x37))
																																				 {
																																							if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //ÏµÍ³½øÈë¶¯Ì¬³ÆÖØ
																																								  W5500_Send_Data(s,con_TranAACK);  //ÕýÈ·Ó¦´ð	
																																								  Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //²»Ö÷¶¯ÉÏ´«½á¹û
																																									USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																								  System_State = DynamicWeight_State; //ÏµÍ³½øÈë¶¯Ì¬³ÆÖØ
																																								  W5500_Send_Data(s,con_TranAACK);  //ÕýÈ·Ó¦´ð	
																																								  Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //Ö÷¶¯ÉÏ´«½á¹û
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //²»Ö÷¶¯ÉÏ´«½á¹û
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//Ö¸ÁîÎÞÐ§
																																							}																																					 																																					 
																																				 }
																																				 else if(Rx_Buffer[6] == 0x31)
																																				 {
																																						 if(Rx_Buffer[2] == 3)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //ÏµÍ³½øÈë¾²Ì¬³ÆÖØ
																																									W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																									Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_SystemFlag.A.Flag_AutoD = 0;    //²»Ö÷¶¯ÉÏ´«½á¹û
																																									USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
																																							}
																																							else if(Rx_Buffer[2] == 4)
																																							{
																																									DynamicWeight_Number = 1;
																																									System_State = StaticWeight_State; //ÏµÍ³½øÈë¾²Ì¬³ÆÖØ
																																									W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																									Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																									WorkState_Flg = StaticWeight_State_Flg;
																																									USER_LEDFlag.A.LED_Test = 1;  					//¼ì²âÖ¸Ê¾µÆ
																																									if(Rx_Buffer[7] == 'D')
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 1;    //Ö÷¶¯ÉÏ´«½á¹û
																																									}
																																									else
																																									{
																																											USER_SystemFlag.A.Flag_AutoD = 0;    //²»Ö÷¶¯ÉÏ´«½á¹û
																																									}
																																							}
																																							else
																																							{
																																									W5500_Send_Data(s,con_TranXACK);//Ö¸ÁîÎÞÐ§
																																							}																																						 
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//ÎÞÐ§Ö¡Ö¸Áî
																																				 }
																																				 if(USER_SystemFlag.A.Flag_Online == 1) //ºÍÖ÷¿ØÁªÍø ÉÏ´«ÄÚÈÝ
																																				 {
																																							if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍÊýÂë¹ÜÁªÍøÁªÍø ÉÏ´«ÄÚÈÝ
																																							{
																																									if(USER_SystemFlag.A.Flag_Sucess == 1) //ÎÕÊÖ¼ÓÃÜ
																																									{
																																											USER_LEDFlag.A.LED_Online = 1;  				//ÁªÍøÖ¸Ê¾µÆ
																																									}
																																									USER_LEDFlag.A.LED_15 = 0;  				//ÊýÂë¹ÜÊý¾ÝÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
																																									USER_LEDFlag.A.LED_14 = 1;  				//ÊýÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
																																									USER_LEDFlag.A.LED_Weight = 1;  			//ÖØÁ¿Ö¸Ê¾µÆ
																																									USER_LEDFlag.A.LED_Brake = 0;  			  //ÖÆ¶¯Ö¸Ê¾µÆ
																																									FunW5500_SendLED(0,0,0);	
																																							}
																																				 }
																																				 break;
																													}
																													case 'A'://¶¯Ì¬Ì¬ÂÖÖØ¼ì²â
																													{
																																				 if((Rx_Buffer[6]>0x30) && (Rx_Buffer[6]<0x37))
																																				 {
																																						 DynamicWeight_Number = Rx_Buffer[6] - 0x30;
																																						 System_State = DynamicWeight_State; //ÏµÍ³½øÈë¶¯Ì¬³ÆÖØ
																																						 W5500_Send_Data(s,con_TranAACK);  //ÕýÈ·Ó¦´ð	
																																						 Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																							WorkState_Flg = StaticWeight_State_Flg;
																																				 }
																																				 else
																																				 {
																																							W5500_Send_Data(s,con_TranXACK);	//ÎÞÐ§Ö¡Ö¸Áî
																																				 }
																																				 break;
																													}
																													case 'L':  //Ä¦ÖØ¼ì²â
																													{																							 
																																				 System_State = StaticWeight_Motor_State; //ÏµÍ³½øÈëÄ¦ÍÐ³ÆÖØ
																																				 W5500_Send_Data(s,con_TranAACK);//ÕýÈ·Ó¦´ð
																																				 Delay_ms(20);//ÑÓÊ± ±£Ö¤ÉÏÎ»»ú½ÓÊÕµ½Ö¸Áî
																																				 WorkState_Flg = StaticWeight_Motor_State_Flg;
																																				 break;
																													}																																																															
																											}
																											break;
																				}
																				case 'D'://È¡²âÊÔ½á¹û
																				{
																											switch(Rx_Buffer[5])
																											{
																													case 'W'://¾²Ì¬ÂÖÖØ½á¹û
																													{
																																				FunW5500_SendDW(s);
																																				break;
																													}
																													case 'A'://¶¯Ì¬ÖáºÉ½á¹û
																													{
																																				break;
																													}
																													case 'L'://·¢ËÍÄ¦ÍÐ³µÖØÁ¿½á¹û
																													{
																																				Tx_Buffer[0] = 8;    //Ö¡³¤¶È µÍÎ»
																																				Tx_Buffer[1] = 0;    //Ö¡³¤¶È ¸ßÎ»
																																				Tx_Buffer[2] = 'D'; 
																																				Tx_Buffer[3] = 'L'; 
																																				/* Ä¦ÍÐ³ÆÖØ */
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
																																				W5500_Send_Data(s,Tx_Buffer);				//·¢ËÍµ±Ç°Ä¦ÍÐÖØÁ¿
																																				break;
																													}																																																																																																									
																													default:
																																				W5500_Send_Data(s,con_TranXACK); //µ±Ç°Ö¸ÁîÎÞÐ§
																																				break;
																											}
																											break;
																				}
																				default :
																											W5500_Send_Data(s,con_TranXACK);				//µ±Ç°Ö¸ÁîÎÞ·¨Ö´ÐÐ																								 
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
* º¯ÊýÃû  : W5500_Receive_Deal
* ÃèÊö    : W5500²âÊÔ³ÌÐò
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : ±¾Ä£°åÔÚÒýÓÃÊ±´Ë²¿·Ö¿É²»×öÐÞ¸Ä
*******************************************************************************/
void W5500_Receive_Deal(void)
{
	  //¼ÓÃÜÎÄ¼þÖ÷Ñ­»·µ÷ÓÃ
		AES_Handler();
    IWDG_ReloadCounter();   //¿´ÃÅ¹·ÇåÁã
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
        W5500_Socket_Set();                                    		//W5500¶Ë¿Ú³õÊ¼»¯ÅäÖÃ main        
        if(W5500_Interrupt)                                       		 	//´¦ÀíW5500ÖÐ¶Ï		 main
        {
            W5500_Interrupt_Process();                      		//W5500ÖÐ¶Ï´¦Àí³ÌÐò¿ò¼Ü
        }
        if((S0_Data & S_RECEIVE) == S_RECEIVE)    //Èç¹ûSocket0½ÓÊÕµ½Êý¾Ý  main
        {
            S0_Data&=~S_RECEIVE;
            W5500_Socket_Data(0);                              //W5500Êý¾Ý´¦Àí
// 						W5500_Send_Data(0,con_TranAACK);
        }
				if((S2_Data & S_RECEIVE) == S_RECEIVE)    //Èç¹ûSocket2½ÓÊÕµ½Êý¾Ý  main
        {
            S2_Data&=~S_RECEIVE;
            W5500_Socket_Data(2);                              //W5500Êý¾Ý´¦Àí
// 						W5500_Send_Data(0,con_TranAACK);
        }
    }
}

/*******************************************************************************
* º¯ÊýÃû  : W5500_Init_Config
* ÃèÊö    : W5500³õÊ¼ÅäÖÃ ¹©Ö÷º¯Êýµ÷ÓÃ
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : ÐèÒªÌáÇ°ÅäÖÃºÃ¶¨Ê±Æ÷2 ÓÃµ½ÁËÑÓÊ±º¯Êý  ¶¨Ê±Æ÷2ÔÚAD7663ÖÐÅäÖÃ
*******************************************************************************/
void W5500_Init_Config(void)
{
		W5500_SPI_Configuration();		//W5500 SPI³õÊ¼»¯ÅäÖÃ(STM32 SPI1)
		W5500_GPIO_Configuration();	//W5500 GPIO³õÊ¼»¯ÅäÖÃ	
		W5500_Initialization();		//W5500³õÊ¼»¯ÅäÖÃ	
		Load_Net_Parameters();
		TIM_Cmd(TIM3, ENABLE); 		//Ê¹ÄÜTIMxÍâÉè  UDPÊ¹ÓÃ
}
/*******************************************************************************
* º¯ÊýÃû  : W5500_Reset
* ÃèÊö    : W5500Ç¿ÖÆ»Ö¸´³ö³§ÉèÖÃ
* ÊäÈë    : ÎÞ
* Êä³ö    : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* ËµÃ÷    : ÐèÒªÌáÇ°ÅäÖÃºÃ¶¨Ê±Æ÷2 ÓÃµ½ÁËÑÓÊ±º¯Êý  ¶¨Ê±Æ÷2ÔÚAD7663ÖÐÅäÖÃ
*******************************************************************************/
void W5500_Reset(void)
{
		ucW5500_InitCTR = 0;
		S0_State = 0;
		S1_State = 0;
		S2_State = 0;
// 		W5500_Initialization();		//W5500³õÊ¼»¯ÅäÖÃ	
		if(USER_SystemFlag.A.Flag_Online == 1) //ºÍÖ÷¿ØÁªÍø ÉÏ´«ÄÚÈÝ
		{									
				USER_SystemFlag.A.Flag_Online = 0;
				USER_LEDFlag.A.LED_Online = 0;   //ÁªÍøÖ¸Ê¾µÆ
				if(USER_SystemFlag.A.Flag_Standalone == 1) //ºÍÊýÂë¹ÜÁªÍøÁªÍø ÉÏ´«ÄÚÈÝ
				{
						USER_LEDFlag.A.LED_15 = 0;  				//ÊýÂë¹ÜÊý¾ÝÊÇ·ñ¸Ä±ä 1±ä  0²»±ä
						USER_LEDFlag.A.LED_14 = 1;  				//ÊýÂë¹ÜÖ¸Ê¾µÆÊÇ·ñ¸Ä±ä 1±ä  0²»±ä						
						FunW5500_SendLED(0,0,0);            //ÉÏ´«¸øÏÔÊ¾°å
				}
		}
		W5500_ParameterReset();
		Load_Net_Parameters();
}
