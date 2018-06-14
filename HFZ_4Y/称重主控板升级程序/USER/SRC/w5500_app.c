/****************************************************************************
* ÎÄ¼þÃû:    w5500_app.c
* ÎÄ¼þÀúÊ·:
* °æ±¾ºÅ:    V1.0
* ÈÕÆÚ:      2017.09.29    
* ×÷Õß:      qgk
* ËµÃ÷:      W5500Ó¦ÓÃ²¿·Ö
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
#include "stm32f10x.h"
#include "W5500.h"
#include "led.h"
#include "stm32f10x.h"
#include "sys_delay.h"
#include "common.h"
#include "stm32f10x_flash.h"
char cMaster_Address=0x02;                                              //³ÆÖØÉè±¸µØÖ·
unsigned char ucW5500_Test_LRC[2];                                 //Ð£Ñé´æ´¢Çø
unsigned char ucFlash_Rx_Buffer[2048];                             //Êý¾Ý»º´æÇø
unsigned char  ucZB_Version[]={'B',2,3,4,5,6,7,8,9,10,11,12,13};   //°æ±¾ºÅ
unsigned  char *ucWip,*ucFip;                                      //FlashÊý¾Ý²ÁÐ´»º´æ
u16 wUp_grade=0;                                                   //Éý¼¶½×¶Î
u16 wNumber_Data,wFlash_Page;                                      //Êý¾Ý¿éºÅ£¬FlashÒ³Êý
uint32_t uFlashDestination = ApplicationAddress;                   //Ó¦ÓÃ³ÌÐòµØÖ·
uint16_t wPageSize = PAGE_SIZE;                      //FlashÒ³´óÐ¡ 
u16 wLED_Time_Mark=0;                                //LED¶¨Ê±±êÖ¾
u16 wSunPackage;                                     //Òª´«ÊäµÄÊý¾Ý¿éÊýÁ¿
uint32_t FflashDestination = ApplicationAddress;     //²ÁÐ´µØÖ·
FLASH_Status WFLASHStatus = FLASH_COMPLETE;          //Flash×´Ì¬±äÁ¿

   /*Ó¦´ð±äÁ¿¶¨Òå*/
#pragma pack(1)
typedef union
{
	  unsigned char 	ucACK_Data[6];
    struct 
    {
        unsigned char ucOpc[1];          //¹¦ÄÜÂë      
			  u16  uError;                     //´íÎóÂë
        unsigned char ucNull[2];	         //¿ÕÁôÓÃ
        unsigned char ucZero;	             //0
    }
		stACK_Data;
}
unACK_TypeDef;
#pragma pack()
/*Ó¦´ð±äÁ¿ÖØ¶¨Òå*/
unACK_TypeDef unACK_Answer;
/*******************************************************************************
* º¯ÊýÃû  : FLASH_Read_Word
* Ãè  Êö  : ¶ÁÈ¡ÏàÓ¦µØÖ·ÄÚµÄÊý¾Ý
* Êä  Èë  : address Êý¾ÝµØÖ·
* Êä  ³ö  : ÎÞ
* ·µ»ØÖµ  : ÏàÓ¦µØÖ·ÄÚ´æ´¢µÄÊý¾Ý
* Ëµ  Ã÷  : 
*******************************************************************************/
uint32_t FLASH_Read_Word(uint32_t address)	
{
    return(*(__IO uint32_t*)address);
}
/*******************************************************************************
* º¯ÊýÃû  : Load_Net_Parameters
* Ãè  Êö  : ×°ÔØÍøÂç²ÎÊý
* Êä  Èë  : ÎÞ
* Êä  ³ö  : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* Ëµ  Ã÷  : Íø¹Ø¡¢ÑÚÂë¡¢ÎïÀíµØÖ·¡¢±¾»úIPµØÖ·¡¢¶Ë¿ÚºÅ
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
	uint8_t W5500_Parameter[27];    //ÍøÂçIP²ÎÊý±äÁ¿
	 //IP²ÎÊý¶ÁÈ¡
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
				
				Phy_Addr[0]=ucW5500_Id[0];    //¼ÓÔØÎïÀíµØÖ·
        Phy_Addr[1]=ucW5500_Id[1];
        Phy_Addr[2]=ucW5500_Id[2];
        Phy_Addr[3]=ucW5500_Id[3];
        Phy_Addr[4]=ucW5500_Id[4];
        Phy_Addr[5]=ucW5500_Id[5];

        Gateway_IP[0]= W5500_Parameter[0]; //¼ÓÔØÍø¹Ø²ÎÊý
        Gateway_IP[1]= W5500_Parameter[1];
        Gateway_IP[2]= W5500_Parameter[2];
        Gateway_IP[3]= W5500_Parameter[3];
        
        Sub_Mask[0]=W5500_Parameter[4];     //¼ÓÔØ×ÓÍøÑÚÂë
        Sub_Mask[1]=W5500_Parameter[5];
        Sub_Mask[2]=W5500_Parameter[6];
        Sub_Mask[3]=W5500_Parameter[7];


//         Phy_Addr[0]=0x00;    //¼ÓÔØÎïÀíµØÖ·
//         Phy_Addr[1]=0x0C;
//         Phy_Addr[2]=0x29;
//         Phy_Addr[3]=0x7D;
//         Phy_Addr[4]=0x64;
//         Phy_Addr[5]=0x01;
        
				IP_Addr[0]=W5500_Parameter[8];//¼ÓÔØ±¾»úIPµØÖ·   ³ÆÖØ 3
				IP_Addr[1]=W5500_Parameter[9];
				IP_Addr[2]=W5500_Parameter[10];
				IP_Addr[3]=W5500_Parameter[11];
        
        S0_Port[0]=W5500_Parameter[13];   //¼ÓÔØ¶Ë¿Ú0µÄ¶Ë¿ÚºÅ50000 
        S0_Port[1]=W5500_Parameter[12];
   
        S0_Mode=TCP_SERVER;//¼ÓÔØ¶Ë¿Ú0µÄ¹¤×÷Ä£Ê½,TCP¿Í»§¶ËÄ£Ê½
}

/*******************************************************************************
* º¯ÊýÃû  : W5500_Initialization
* Ãè  Êö  : W5500³õÊ¼»¯
* Êä  Èë  : ÎÞ
* Êä  ³ö  : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* Ëµ  Ã÷  : ÎÞ
*******************************************************************************/
void W5500_Initialization(void)
{
    W5500_Init();		  //³õÊ¼»¯W5500¼Ä´æÆ÷º¯Êý
    Detect_Gateway();	//¼ì²éÍø¹Ø·þÎñÆ÷ 
    Socket_Init(0);		//Ö¸¶¨Socket(0~7)³õÊ¼»¯,³õÊ¼»¯¶Ë¿Ú0
    
}
/*******************************************************************************
* º¯ÊýÃû  : W5500_LRC
* Ãè  Êö  : Ð£ÑéÎ»»ñÈ¡
* Êä  Èë  : *ucData:Êý¾ÝÊ×µØÖ·£»wLength£ºÊý¾Ý³¤¶È
* Êä  ³ö  : ÎÞ
* ·µ»ØÖµ  : Ð£ÑéÖµ
* Ëµ  Ã÷  : ÎÞ£
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
* º¯ÊýÃû  : W5500_Return_Data
* Ãè  Êö  : »Ø´«Êý¾Ý
* Êä  Èë  : ucOrder£ºÃüÁîÂë£»*uc_Data_Buffer£º»Ø´«Êý¾ÝÊ×µØÖ·£»wSize£ºÊý¾Ý³¤¶È
* Êä  ³ö  : ÎÞ
* ·µ»ØÖµ  : ÎÞ
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
* º¯ÊýÃû  : Process_Socket_Data
* Ãè  Êö  : Ö¸Áî½âÎö
* Êä  Èë  : s:¶Ë¿ÚºÅ
* Êä  ³ö  : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* Ëµ  Ã÷  : Éý¼¶ÃüÁî¡¢Éý¼¶ÇëÇó¡¢Ð´ÇëÇó½âÎö¡£
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
    if((ucW5500_Test_LRC[0]!=ucW5500_Test_LRC[1])||(W5500_Rx_Buffer[0]!=2)||(W5500_Rx_Buffer[wSize-1]!=3)||(W5500_Rx_Buffer[1]!=cMaster_Address))           //LRCÎ»ÅÐ¶Ï
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
            case'B':                                                         //»ñÈ¡°æ±¾ÐÅÏ¢Ö¸Áî                                                               
            {
               W5500_Return_Data('B',ucZB_Version,13);
            }
            break;
            case'W':                                                         //Éý¼¶ÃüÁî
            {
                ucOrder=W5500_Rx_Buffer[5];
                switch(ucOrder)
                {
									case'G':                                                 //ÖØÐÂÉý¼¶ÃüÇëÇó
                    {
											      FLASH_Lock();
                            unACK_Answer.ucACK_Data[0]='G';
                            W5500_Return_Data('W',unACK_Answer.ucACK_Data,1);
											      TimeDelay_ms(10);//ÑÓÊ±10ms
                            NVIC_SystemReset();
                    }
                    break;
									case'W':                                                 //Éý¼¶ÃüÇëÇó
                    {
                        if(wUp_grade==2)
                        {
                            unACK_Answer.ucACK_Data[0]='W';
													  wSunPackage=W5500_Rx_Buffer[7];                                                 //Òª´«ÊäµÄÊý¾Ý¿éÊýÁ¿
                            wSunPackage<<=8;
												    wSunPackage+=W5500_Rx_Buffer[6];
													  wSunPackage-=1;       //ÏÂ±ê´Ó0¿ªÊ¼£¬ËùÒÔ¼õ1
													  wUp_grade=4;
                            W5500_Return_Data('W',unACK_Answer.ucACK_Data,1);
                        }
                        else             //E5Á÷³Ì´íÎó
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
                    
                    case'D':                                                 //Ð´Êý¾Ý
                    {
                        
                        if(wUp_grade>=4)                                      //²½ÖèÅÐ¶Ï
                        {
                            if(wUp_grade==4)
                            {
															  /*Flash ½âËø*/
                                FLASH_Unlock();
															  FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);
                                uData_Write=uData_Read;
                                wUp_grade=5;
                             }
				
                            unACK_Answer.ucACK_Data[1]=W5500_Rx_Buffer[6];
                            unACK_Answer.ucACK_Data[2]=W5500_Rx_Buffer[7];
													 /******************************************************************
                             ********************¿éºÅÒì³£ E4 **********************************
                            ******************************************************************/	
                            if(unACK_Answer.stACK_Data.uError!=wNumber_Data)      //Óë´æ´¢µÄÐ´Èë¿éºÅ¶Ô±È
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
													         if((wSize<522)||(wSunPackage==unACK_Answer.stACK_Data.uError))      //Êý¾ÝÁ¿Ð¡ÓÚ512
                                    {
														        if(wSunPackage!=unACK_Answer.stACK_Data.uError)  //²»Îª×îºóÒ»¿éÊý¾Ý
																	    {
																	   	unACK_Answer.stACK_Data.ucOpc[0]='E';
                                      unACK_Answer.stACK_Data.uError=2;                //³¤¶ÈÒì³£
                                      unACK_Answer.stACK_Data.ucNull[0]=0;
                                      unACK_Answer.stACK_Data.ucNull[1]=0;
                                      unACK_Answer.stACK_Data.ucZero=0;
                                      W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
																	    wUp_grade=7;
																			return;
                                      }
                                     else
                                     {	
                                        ucWip=(&W5500_Rx_Buffer[8]);	    //ÐèÒª¿½±´Êý¾ÝµÄÊ×µØÖ·
                                        ucFip=((&ucFlash_Rx_Buffer[0])+(wNumber_Data%4)*512);	    //¼ÆËã»º´æÇøµÄµØÖ·
                                        memcpy(ucFip,ucWip,(wSize-10)); 			
                                        while(FLASH_GetFlagStatus(FLASH_FLAG_BSY)==SET)
                                        {
                                            ;
                                        }
                                        WFLASHStatus = FLASH_ErasePage(uFlashDestination + (wPageSize * wFlash_Page));
                                        if(WFLASHStatus!=FLASH_COMPLETE)
																				{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                                 //E8²Á³ýÒì³£
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
                                                unACK_Answer.stACK_Data.uError=7;                                 //E7Ð´ÈëÒì³£
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																							  return;
                                               } 
                                            /******************************************************************
                                                              E3  Ð£¶ÔÒì³£
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
																					
																				WFLASHStatus = FLASH_ErasePage(2048*255);      //Çå±êÖ¾Î»£¬Î»ÖÃÄÚÈÝ×Ô¼º¶¨Òå¡£
                                        if(WFLASHStatus!=FLASH_COMPLETE)
																				{
																								unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                         //E8²Á³ýÒì³£
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
                                    wNumber_Data+=1;        //½ÓÊÕµ½µÄÊý¾Ý°ü´ÎÊý
                                    if(wNumber_Data/4>wFlash_Page)         
                                    {
                                        WFLASHStatus = FLASH_ErasePage(uFlashDestination + (wPageSize * wFlash_Page));
                                        if(WFLASHStatus != FLASH_COMPLETE)
																				{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=8;                                 //E8²Á³ýÒì³£
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																					      return;	
																				}
                                        wFlash_Page+=1;        //FlashÒ³
                                        /* FlashÐ´Èë*/	
                                        uData_Write=uData_Read;
                                        for (wNumber_Write = 0; wNumber_Write <wPageSize ; wNumber_Write += 4)
                                        { 
                                            WFLASHStatus=FLASH_ProgramWord(FflashDestination, *uData_Write);
                                            if(WFLASHStatus!=FLASH_COMPLETE)
																						{
																							  unACK_Answer.stACK_Data.ucOpc[0]='E';
                                                unACK_Answer.stACK_Data.uError=7;                                 //E7Ð´ÈëÒì³£
                                                unACK_Answer.stACK_Data.ucNull[0]=0;
                                                unACK_Answer.stACK_Data.ucNull[1]=0;
                                                unACK_Answer.stACK_Data.ucZero=0;
                                                W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);	
                                                wUp_grade=7;
																							  return;	
                                               } 
                                            /******************************************************************
                                                                    ¶ÁÐ´Òì³£
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
                                                           ³¤¶ÈÒì³£
                                ******************************************************************/	
                                else              //Êý¾Ý³¬³ö³¤¶È
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
                        else      //Éý¼¶²½Öè´íÎó
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
          unACK_Answer.stACK_Data.uError=1;                   //Éý¼¶³ÌÐòÏÂÎÞ·¨Ö´ÐÐÖ¸Áî»Ø¸´
          unACK_Answer.stACK_Data.ucNull[0]=0;
          unACK_Answer.stACK_Data.ucNull[1]=0;
          unACK_Answer.stACK_Data.ucZero=0;
          W5500_Return_Data('W',unACK_Answer.ucACK_Data,3);		

       }
    }
}
/*******************************************************************************
* º¯ÊýÃû  : W5500_DataHandling
* Ãè  Êö  : W5500ÔËÐÐº¯Êý
* Êä  Èë  : ÎÞ
* Êä  ³ö  : ÎÞ
* ·µ»ØÖµ  : ÎÞ
* Ëµ  Ã÷  : W5500Æô¶¯£¬Êý¾Ý´¦Àí
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
        W5500_Socket_Set();          //W5500¶Ë¿Ú³õÊ¼»¯ÅäÖÃ
        
        if(W5500_Interrupt)          //´¦ÀíW5500ÖÐ¶Ï
        {
            W5500_Interrupt_Process();//W5500ÖÐ¶Ï´¦Àí³ÌÐò¿ò¼Ü
        }
        if((S0_Data & S_RECEIVE) == S_RECEIVE)  //Èç¹ûSocket0½ÓÊÕµ½Êý¾Ý
        {
            S0_Data&=~S_RECEIVE;
            Process_Socket_Data(0);              //W5500Êý¾Ý´¦Àí
        }
    }
    if(wUp_grade==1)
    {
        if(FLASH_Read_Word(2048 * 255)==0X77777777)   //ÅÐ¶ÏÉý¼¶±êÖ¾   ×Ô¼º¶¨Òå
        {
            wUp_grade=2;
        }
        else
        wUp_grade=6;
    }

}

