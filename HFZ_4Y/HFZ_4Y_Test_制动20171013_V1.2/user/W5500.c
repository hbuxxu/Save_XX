/****************************************************************************
* �ļ���: W5500.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2016.09.29    
* ����:      qgk
* ˵��:      ��������ģ��
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


/***************----- ��������������� -----***************/
unsigned char Gateway_IP[4];//����IP��ַ 
unsigned char Sub_Mask[4];	//�������� 
unsigned char Phy_Addr[6];	//�����ַ(MAC) 
unsigned char IP_Addr[4];	//����IP��ַ 

unsigned char S0_Port[2];	//�˿�0�Ķ˿ں�(5000) 
unsigned char S0_DIP[4];	//�˿�0Ŀ��IP��ַ 
unsigned char S0_DPort[2];	//�˿�0Ŀ�Ķ˿ں�(6000) 

unsigned char S1_Port[2];   //�ӿ�1�ı��ض˿ں�60000   UDP������

unsigned char UDP_DIPR[4];	//UDP(�㲥)ģʽ,Ŀ������IP��ַ
unsigned char UDP_DPORT[2];	//UDP(�㲥)ģʽ,Ŀ�������˿ں�

unsigned char S2_Port[2];	//�˿�2�Ķ˿ں�(25032) 

/***************----- �˿ڵ�����ģʽ -----***************/
unsigned char S0_Mode =3;	//�˿�0������ģʽ,0:TCP������ģʽ,1:TCP�ͻ���ģʽ,2:UDP(�㲥)ģʽ
#define TCP_SERVER	0x00	//TCP������ģʽ
#define TCP_CLIENT	0x01	//TCP�ͻ���ģʽ 
#define UDP_MODE	0x02	//UDP(�㲥)ģʽ 

/***************----- �˿ڵ�����״̬ -----***************/
unsigned char S0_State =0;	//�˿�0״̬��¼,1:�˿���ɳ�ʼ��,2�˿��������(����������������) 
#define S_INIT		0x01	//�˿���ɳ�ʼ�� 
#define S_CONN		0x02	//�˿��������,���������������� 

/***************----- �˿��շ����ݵ�״̬ -----***************/
unsigned char S0_Data;		//�˿�0���պͷ������ݵ�״̬,1:�˿ڽ��յ�����,2:�˿ڷ���������� 
#define S_RECEIVE	 0x01	//�˿ڽ��յ�һ�����ݰ� 
#define S_TRANSMITOK 0x02	//�˿ڷ���һ�����ݰ���� 

unsigned char S1_Mode =3;	//�˿�1������ģʽ,0:TCP������ģʽ,1:TCP�ͻ���ģʽ,2:UDP(�㲥)ģʽ
/***************----- �˿ڵ�����״̬ -----***************/
unsigned char S1_State =0;	//�˿�1״̬��¼,1:�˿���ɳ�ʼ��,2�˿��������(����������������) 
/***************----- �˿��շ����ݵ�״̬ -----***************/
unsigned char S1_Data;		//�˿�1���պͷ������ݵ�״̬,1:�˿ڽ��յ�����,2:�˿ڷ���������� 

unsigned char S2_Mode =3;	//�˿�2������ģʽ,0:TCP������ģʽ,1:TCP�ͻ���ģʽ,2:UDP(�㲥)ģʽ
/***************----- �˿ڵ�����״̬ -----***************/
unsigned char S2_State =0;	//�˿�2״̬��¼,1:�˿���ɳ�ʼ��,2�˿��������(����������������) 
/***************----- �˿��շ����ݵ�״̬ -----***************/
unsigned char S2_Data;		//�˿�2���պͷ������ݵ�״̬,1:�˿ڽ��յ�����,2:�˿ڷ���������� 

//����ʹ��
unsigned char S3_Port[2];	//�˿�3�Ķ˿ں�(6666) ����ʹ�ö˿� 
unsigned char S3_Mode =3;	//�˿�3������ģʽ,0:TCP������ģʽ,1:TCP�ͻ���ģʽ,2:UDP(�㲥)ģʽ
/***************----- �˿ڵ�����״̬ -----***************/
unsigned char S3_State =0;	//�˿�3״̬��¼,1:�˿���ɳ�ʼ��,2�˿��������(����������������) 
/***************----- �˿��շ����ݵ�״̬ -----***************/
unsigned char S3_Data;		//�˿�3���պͷ������ݵ�״̬,1:�˿ڽ��յ�����,2:�˿ڷ���������� 


/***************----- �˿����ݻ����� -----***************/
// unsigned char Rx_Buffer[2048];	//�˿ڽ������ݻ����� 
// unsigned char Tx_Buffer[2048];	//�˿ڷ������ݻ����� 
unsigned char W5500_Interrupt;	//W5500�жϱ�־(0:���ж�,1:���ж�)
/*******************************************************************************
* ������  : W5500_GPIO_Configuration
* ����    : W5500 GPIO��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_GPIO_Configuration(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		EXTI_InitTypeDef  EXTI_InitStructure;	
		NVIC_InitTypeDef NVIC_InitStructure;						//����NVIC��ʼ���ṹ��

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		/* W5500_RST���ų�ʼ������(PC4) */
		GPIO_InitStructure.GPIO_Pin  = W5500_RST;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);
		GPIO_ResetBits(W5500_RST_PORT, W5500_RST);
		
		/* W5500_INT���ų�ʼ������(PC5) */	
		GPIO_InitStructure.GPIO_Pin = W5500_INT;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);
	
	  /* ϵͳ��λ���ų�ʼ������(PE1) */	
		GPIO_InitStructure.GPIO_Pin = SYSTEM_REST;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(SYSTEM_REST_PORT, &GPIO_InitStructure);
			
		/* Connect EXTI Line9 to PA9    �ж���������  */
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
		/* PA9 as W5500 interrupt input */
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI9_5_IRQnChannelPreemptionPriority;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI9_5_IRQnChannelSubPriority;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* ������  : W5500_SPI_Configuration
* ����    : W5500 SPI��ʼ������(STM32 SPI1)
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void  W5500_SPI_Configuration(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	SPI_InitTypeDef   	SPI_InitStructure;	   

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);	

	/* ��ʼ��SCK��MISO��MOSI���� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

	/* ��ʼ��CS���� */
	GPIO_InitStructure.GPIO_Pin = W5500_SCS;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(W5500_SCS_PORT, &GPIO_InitStructure);
	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS);

	/* ��ʼ������STM32 SPI1 */
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;	//SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;							//����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;						//SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;							//ʱ�����յ�
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;							//���ݲ����ڵ�1��ʱ����
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;								//NSS���ⲿ�ܽŹ���
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;	//������Ԥ��ƵֵΪ2
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;					//���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial=7;								//CRC����ʽΪ7
	SPI_Init(SPI1,&SPI_InitStructure);									//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPI1�Ĵ���
	SPI_Cmd(SPI1,ENABLE);	//STM32ʹ��SPI1
}

/*******************************************************************************
* ������  : EXTI9_5_ISR
* ����    : �ж���5�жϷ�����(W5500 INT�����жϷ�����)
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void EXTI9_5_ISR(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
			EXTI_ClearITPendingBit(EXTI_Line5);
			W5500_Interrupt=1;
	}
}



/*******************************************************************************
* ������  : SPI1_Send_Byte
* ����    : SPI1����1���ֽ�����
* ����    : dat:�����͵�����
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void SPI1_Send_Byte(unsigned char dat)
{
	SPI_I2S_SendData(SPI1,dat);//д1���ֽ�����
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//�ȴ����ݼĴ�����
}

/*******************************************************************************
* ������  : SPI1_Send_Short
* ����    : SPI1����2���ֽ�����(16λ)
* ����    : dat:�����͵�16λ����
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void SPI1_Send_Short(unsigned short dat)
{
	SPI1_Send_Byte(dat/256);//д���ݸ�λ
	SPI1_Send_Byte(dat);	//д���ݵ�λ
}

/*******************************************************************************
* ������  : Write_W5500_1Byte
* ����    : ͨ��SPI1��ָ����ַ�Ĵ���д1���ֽ�����
* ����    : reg:16λ�Ĵ�����ַ,dat:��д�������
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_1Byte(unsigned short reg, unsigned char dat)
{
	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ

	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM1|RWB_WRITE|COMMON_R);//ͨ��SPI1д�����ֽ�,1���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���
	SPI1_Send_Byte(dat);//д1���ֽ�����

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  : Write_W5500_2Byte
* ����    : ͨ��SPI1��ָ����ַ�Ĵ���д2���ֽ�����
* ����    : reg:16λ�Ĵ�����ַ,dat:16λ��д�������(2���ֽ�)
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_2Byte(unsigned short reg, unsigned short dat)
{
	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ
		
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM2|RWB_WRITE|COMMON_R);//ͨ��SPI1д�����ֽ�,2���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���
	SPI1_Send_Short(dat);//д16λ����

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  : Write_W5500_nByte
* ����    : ͨ��SPI1��ָ����ַ�Ĵ���дn���ֽ�����
* ����    : reg:16λ�Ĵ�����ַ,*dat_ptr:��д�����ݻ�����ָ��,size:��д������ݳ���
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_nByte(unsigned short reg, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short i;

	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ	
		
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(VDM|RWB_WRITE|COMMON_R);//ͨ��SPI1д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���

	for(i=0;i<size;i++)//ѭ������������size���ֽ�����д��W5500
	{
		SPI1_Send_Byte(*dat_ptr++);//дһ���ֽ�����
	}

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  : Write_W5500_SOCK_1Byte
* ����    : ͨ��SPI1��ָ���˿ڼĴ���д1���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ,dat:��д�������
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat)
{
	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ	
		
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM1|RWB_WRITE|(s*0x20+0x08));//ͨ��SPI1д�����ֽ�,1���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���
	SPI1_Send_Byte(dat);//д1���ֽ�����

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  : Write_W5500_SOCK_2Byte
* ����    : ͨ��SPI1��ָ���˿ڼĴ���д2���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ,dat:16λ��д�������(2���ֽ�)
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat)
{
	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ
			
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM2|RWB_WRITE|(s*0x20+0x08));//ͨ��SPI1д�����ֽ�,2���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���
	SPI1_Send_Short(dat);//д16λ����

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  : Write_W5500_SOCK_4Byte
* ����    : ͨ��SPI1��ָ���˿ڼĴ���д4���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ,*dat_ptr:��д���4���ֽڻ�����ָ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr)
{
	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ
			
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM4|RWB_WRITE|(s*0x20+0x08));//ͨ��SPI1д�����ֽ�,4���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���

	SPI1_Send_Byte(*dat_ptr++);//д��1���ֽ�����
	SPI1_Send_Byte(*dat_ptr++);//д��2���ֽ�����
	SPI1_Send_Byte(*dat_ptr++);//д��3���ֽ�����
	SPI1_Send_Byte(*dat_ptr++);//д��4���ֽ�����

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ
}

/*******************************************************************************
* ������  : Read_W5500_1Byte
* ����    : ��W5500ָ����ַ�Ĵ�����1���ֽ�����
* ����    : reg:16λ�Ĵ�����ַ
* ���    : ��
* ����ֵ  : ��ȡ���Ĵ�����1���ֽ�����
* ˵��    : ��
*******************************************************************************/
unsigned char Read_W5500_1Byte(unsigned short reg)
{
	unsigned char i;

	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ
			
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM1|RWB_READ|COMMON_R);//ͨ��SPI1д�����ֽ�,1���ֽ����ݳ���,������,ѡ��ͨ�üĴ���

	i=SPI_I2S_ReceiveData(SPI1);
	SPI1_Send_Byte(0x00);//����һ��������
	i=SPI_I2S_ReceiveData(SPI1);//��ȡ1���ֽ�����

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�ߵ�ƽ
	return i;//���ض�ȡ���ļĴ�������
}

/*******************************************************************************
* ������  : Read_W5500_SOCK_1Byte
* ����    : ��W5500ָ���˿ڼĴ�����1���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ
* ���    : ��
* ����ֵ  : ��ȡ���Ĵ�����1���ֽ�����
* ˵��    : ��
*******************************************************************************/
unsigned char Read_W5500_SOCK_1Byte(SOCKET s, unsigned short reg)
{
	unsigned char i;

	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ
			
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM1|RWB_READ|(s*0x20+0x08));//ͨ��SPI1д�����ֽ�,1���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���

	i=SPI_I2S_ReceiveData(SPI1);
	SPI1_Send_Byte(0x00);//����һ��������
	i=SPI_I2S_ReceiveData(SPI1);//��ȡ1���ֽ�����

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�ߵ�ƽ
	return i;//���ض�ȡ���ļĴ�������
}

/*******************************************************************************
* ������  : Read_W5500_SOCK_2Byte
* ����    : ��W5500ָ���˿ڼĴ�����2���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ
* ���    : ��
* ����ֵ  : ��ȡ���Ĵ�����2���ֽ�����(16λ)
* ˵��    : ��
*******************************************************************************/
unsigned short Read_W5500_SOCK_2Byte(SOCKET s, unsigned short reg)
{
	unsigned short i;

	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ
			
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM2|RWB_READ|(s*0x20+0x08));//ͨ��SPI1д�����ֽ�,2���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���

	i=SPI_I2S_ReceiveData(SPI1);
	SPI1_Send_Byte(0x00);//����һ��������
	i=SPI_I2S_ReceiveData(SPI1);//��ȡ��λ����
	SPI1_Send_Byte(0x00);//����һ��������
	i*=256;
	i+=SPI_I2S_ReceiveData(SPI1);//��ȡ��λ����

	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�ߵ�ƽ
	return i;//���ض�ȡ���ļĴ�������
}

/*******************************************************************************
* ������  : Read_SOCK_Data_Buffer
* ����    : ��W5500�������ݻ������ж�ȡ����
* ����    : s:�˿ں�,*dat_ptr:���ݱ��滺����ָ��
* ���    : ��
* ����ֵ  : ��ȡ�������ݳ���,rx_size���ֽ�
* ˵��    : ��
*******************************************************************************/
unsigned short Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr)
{
	unsigned short rx_size;
	unsigned short offset, offset1;
	unsigned short i;
	unsigned char j;

	rx_size=Read_W5500_SOCK_2Byte(s,Sn_RX_RSR);
	if(rx_size==0) return 0;//û���յ������򷵻�
	if(rx_size>1460) rx_size=1460;

	offset=Read_W5500_SOCK_2Byte(s,Sn_RX_RD);
	offset1=offset;
	offset&=(S_RX_SIZE-1);//����ʵ�ʵ������ַ

	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ

	SPI1_Send_Short(offset);//д16λ��ַ
	SPI1_Send_Byte(VDM|RWB_READ|(s*0x20+0x18));//д�����ֽ�,N���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���
	j=SPI_I2S_ReceiveData(SPI1);
	
	if((offset+rx_size)<S_RX_SIZE)//�������ַδ����W5500���ջ������Ĵ���������ַ
	{
		for(i=0;i<rx_size;i++)//ѭ����ȡrx_size���ֽ�����
		{
			SPI1_Send_Byte(0x00);//����һ��������
			j=SPI_I2S_ReceiveData(SPI1);//��ȡ1���ֽ�����
			*dat_ptr=j;//����ȡ�������ݱ��浽���ݱ��滺����
			dat_ptr++;//���ݱ��滺����ָ���ַ����1
		}
	}
	else//�������ַ����W5500���ջ������Ĵ���������ַ
	{
		offset=S_RX_SIZE-offset;
		for(i=0;i<offset;i++)//ѭ����ȡ��ǰoffset���ֽ�����
		{
			SPI1_Send_Byte(0x00);//����һ��������
			j=SPI_I2S_ReceiveData(SPI1);//��ȡ1���ֽ�����
			*dat_ptr=j;//����ȡ�������ݱ��浽���ݱ��滺����
			dat_ptr++;//���ݱ��滺����ָ���ַ����1
		}
		GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ

		GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ

		SPI1_Send_Short(0x00);//д16λ��ַ
		SPI1_Send_Byte(VDM|RWB_READ|(s*0x20+0x18));//д�����ֽ�,N���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���
		j=SPI_I2S_ReceiveData(SPI1);

		for(;i<rx_size;i++)//ѭ����ȡ��rx_size-offset���ֽ�����
		{
			SPI1_Send_Byte(0x00);//����һ��������
			j=SPI_I2S_ReceiveData(SPI1);//��ȡ1���ֽ�����
			*dat_ptr=j;//����ȡ�������ݱ��浽���ݱ��滺����
			dat_ptr++;//���ݱ��滺����ָ���ַ����1
		}
	}
	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ

	offset1+=rx_size;//����ʵ�������ַ,���´ζ�ȡ���յ������ݵ���ʼ��ַ
	Write_W5500_SOCK_2Byte(s, Sn_RX_RD, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, RECV);//����������������
	return rx_size;//���ؽ��յ����ݵĳ���
}

/*******************************************************************************
* ������  : Write_SOCK_Data_Buffer
* ����    : ������д��W5500�����ݷ��ͻ�����
* ����    : s:�˿ں�,*dat_ptr:���ݱ��滺����ָ��,size:��д�����ݵĳ���
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short offset,offset1;
	unsigned short i;

	//�����UDPģʽ,�����ڴ�����Ŀ��������IP�Ͷ˿ں�
	if((Read_W5500_SOCK_1Byte(s,Sn_MR)&0x0f) != SOCK_UDP)//���Socket��ʧ��
	{		
		Write_W5500_SOCK_4Byte(s, Sn_DIPR, UDP_DIPR);//����Ŀ������IP  		
		Write_W5500_SOCK_2Byte(s, Sn_DPORTR, UDP_DPORT[0]*256+UDP_DPORT[1]);//����Ŀ�������˿ں�				
	}

	offset=Read_W5500_SOCK_2Byte(s,Sn_TX_WR);
	offset1=offset;
	offset&=(S_TX_SIZE-1);//����ʵ�ʵ������ַ

	GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ

	SPI1_Send_Short(offset);//д16λ��ַ
	SPI1_Send_Byte(VDM|RWB_WRITE|(s*0x20+0x10));//д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���

	if((offset+size)<S_TX_SIZE)//�������ַδ����W5500���ͻ������Ĵ���������ַ
	{
		for(i=0;i<size;i++)//ѭ��д��size���ֽ�����
		{
			SPI1_Send_Byte(*dat_ptr++);//д��һ���ֽڵ�����		
		}
	}
	else//�������ַ����W5500���ͻ������Ĵ���������ַ
	{
		offset=S_TX_SIZE-offset;
		for(i=0;i<offset;i++)//ѭ��д��ǰoffset���ֽ�����
		{
			SPI1_Send_Byte(*dat_ptr++);//д��һ���ֽڵ�����
		}
		GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ

		GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);//��W5500��SCSΪ�͵�ƽ

		SPI1_Send_Short(0x00);//д16λ��ַ
		SPI1_Send_Byte(VDM|RWB_WRITE|(s*0x20+0x10));//д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���

		for(;i<size;i++)//ѭ��д��size-offset���ֽ�����
		{
			SPI1_Send_Byte(*dat_ptr++);//д��һ���ֽڵ�����
		}
	}
	GPIO_SetBits(W5500_SCS_PORT, W5500_SCS); //��W5500��SCSΪ�ߵ�ƽ

	offset1+=size;//����ʵ�������ַ,���´�д���������ݵ��������ݻ���������ʼ��ַ
	Write_W5500_SOCK_2Byte(s, Sn_TX_WR, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, SEND);//����������������				
}




/*******************************************************************************
* ������  : Detect_Gateway
* ����    : ������ط�����
* ����    : ��
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ��
*******************************************************************************/
unsigned char Detect_Gateway(void)
{
	unsigned char ip_adde[4];
	ip_adde[0]=IP_Addr[0]+1;
	ip_adde[1]=IP_Addr[1]+1;
	ip_adde[2]=IP_Addr[2]+1;
	ip_adde[3]=IP_Addr[3]+1;

	//������ؼ���ȡ���ص������ַ
	Write_W5500_SOCK_4Byte(0,Sn_DIPR,ip_adde);//��Ŀ�ĵ�ַ�Ĵ���д���뱾��IP��ͬ��IPֵ
	Write_W5500_SOCK_1Byte(0,Sn_MR,MR_TCP);//����socketΪTCPģʽ
	Write_W5500_SOCK_1Byte(0,Sn_CR,OPEN);//��Socket	
	Delay_ms(5);//��ʱ5ms 	
	
	if(Read_W5500_SOCK_1Byte(0,Sn_SR) != SOCK_INIT)//���socket��ʧ��
	{
		Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);//�򿪲��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}

	Write_W5500_SOCK_1Byte(0,Sn_CR,CONNECT);//����SocketΪConnectģʽ						

	do
	{
		u8 j=0;
		j=Read_W5500_SOCK_1Byte(0,Sn_IR);//��ȡSocket0�жϱ�־�Ĵ���
		if(j!=0)
		Write_W5500_SOCK_1Byte(0,Sn_IR,j);
		Delay_ms(5);//��ʱ5ms 
		if((j&IR_TIMEOUT) == IR_TIMEOUT)
		{
			return FALSE;	
		}
		else if(Read_W5500_SOCK_1Byte(0,Sn_DHAR) != 0xff)
		{
			Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);//�ر�Socket
			return TRUE;							
		}
	}while(1);
}

/*******************************************************************************
* ������  : Socket_Init
* ����    : ָ��Socket(0~7)��ʼ��
* ����    : s:����ʼ���Ķ˿�
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Socket_Init(SOCKET s)
{
	//���÷�Ƭ���ȣ��ο�W5500�����ֲᣬ��ֵ���Բ��޸�	
	Write_W5500_SOCK_2Byte(0, Sn_MSSR, 1460);//����Ƭ�ֽ���=1460(0x5b4)
	Write_W5500_SOCK_2Byte(2, Sn_MSSR, 1460);//����Ƭ�ֽ���=1460(0x5b4)
  Write_W5500_SOCK_2Byte(3, Sn_MSSR, 1460);//����Ƭ�ֽ���=1460(0x5b4)
	//����ָ���˿�
	switch(s)
	{
		case 0:
			//���ö˿�0�Ķ˿ں�
			Write_W5500_SOCK_2Byte(0, Sn_PORT, S0_Port[0]*256+S0_Port[1]);
			//���ö˿�0Ŀ��(Զ��)�˿ں�
			Write_W5500_SOCK_2Byte(0, Sn_DPORTR, S0_DPort[0]*256+S0_DPort[1]);
			//���ö˿�0Ŀ��(Զ��)IP��ַ
			Write_W5500_SOCK_4Byte(0, Sn_DIPR, S0_DIP);						
			break;

		case 1:
			//���ö˿�1ΪUDPģʽ
		//���ö˿�1��Դ�˿ں�
			Write_W5500_SOCK_2Byte(1, Sn_PORT, S1_Port[0]*256+S1_Port[1]);
			//���ö˿�1Ŀ��(Զ��)�˿ں�                                       
			Write_W5500_SOCK_2Byte(1, Sn_DPORTR, UDP_DPORT[0]*256+UDP_DPORT[1]);
			//���ö˿�1Ŀ��(Զ��)IP��ַ
			Write_W5500_SOCK_4Byte(1, Sn_DIPR, UDP_DIPR);			
			break;

		case 2:
			//���ö˿�2�Ķ˿ں�
			Write_W5500_SOCK_2Byte(2, Sn_PORT, S2_Port[0]*256+S2_Port[1]);
			//���ö˿�2Ŀ��(Զ��)�˿ں�
			Write_W5500_SOCK_2Byte(2, Sn_DPORTR, S0_DPort[0]*256+S0_DPort[1]);
			//���ö˿�2Ŀ��(Զ��)IP��ַ
			Write_W5500_SOCK_4Byte(2, Sn_DIPR, S0_DIP);
			break;

		case 3:
      //���ö˿�3�Ķ˿ں�
			Write_W5500_SOCK_2Byte(3, Sn_PORT, S3_Port[0]*256+S3_Port[1]);
			//���ö˿�3Ŀ��(Զ��)�˿ں�
			Write_W5500_SOCK_2Byte(3, Sn_DPORTR, S0_DPort[0]*256+S0_DPort[1]);
			//���ö˿�3Ŀ��(Զ��)IP��ַ
			Write_W5500_SOCK_4Byte(3, Sn_DIPR, S0_DIP);
			break;

		case 4:
			break;

		case 5:
			break;

		case 6:
			break;

		case 7:
			break;

		default:
			break;
	}
}

/*******************************************************************************
* ������  : Socket_Connect
* ����    : ����ָ��Socket(0~7)Ϊ�ͻ�����Զ�̷���������
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ������Socket�����ڿͻ���ģʽʱ,���øó���,��Զ�̷�������������
*			����������Ӻ���ֳ�ʱ�жϣ��������������ʧ��,��Ҫ���µ��øó�������
*			�ó���ÿ����һ��,�������������һ������
*******************************************************************************/
unsigned char Socket_Connect(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);//����socketΪTCPģʽ
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);//��Socket
	Delay_ms(5);//��ʱ5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)//���socket��ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//�򿪲��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}
	Write_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);//����SocketΪConnectģʽ
	return TRUE;//����TRUE,���óɹ�
}

/*******************************************************************************
* ������  : Socket_Listen
* ����    : ����ָ��Socket(0~7)��Ϊ�������ȴ�Զ������������
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ������Socket�����ڷ�����ģʽʱ,���øó���,�ȵ�Զ������������
*			�ó���ֻ����һ��,��ʹW5500����Ϊ������ģʽ
*******************************************************************************/
unsigned char Socket_Listen(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);//����socketΪTCPģʽ 
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);//��Socket	
	Delay_ms(5);//��ʱ5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)//���socket��ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//�򿪲��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}	
	Write_W5500_SOCK_1Byte(s,Sn_CR,LISTEN);//����SocketΪ����ģʽ	
	Delay_ms(5);//��ʱ5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_LISTEN)//���socket����ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//���ò��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}

	return TRUE;

	//���������Socket�Ĵ򿪺�������������,����Զ�̿ͻ����Ƿ�������������,����Ҫ�ȴ�Socket�жϣ�
	//���ж�Socket�������Ƿ�ɹ����ο�W5500�����ֲ��Socket�ж�״̬
	//�ڷ���������ģʽ����Ҫ����Ŀ��IP��Ŀ�Ķ˿ں�
}

/*******************************************************************************
* ������  : Socket_UDP
* ����    : ����ָ��Socket(0~7)ΪUDPģʽ
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ���Socket������UDPģʽ,���øó���,��UDPģʽ��,Socketͨ�Ų���Ҫ��������
*			�ó���ֻ����һ�Σ���ʹW5500����ΪUDPģʽ
*******************************************************************************/
unsigned char Socket_UDP(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_UDP);//����SocketΪUDPģʽ*/
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);//��Socket*/
	Delay_ms(5);//��ʱ5ms
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_UDP)//���Socket��ʧ��
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);//�򿪲��ɹ�,�ر�Socket
		return FALSE;//����FALSE(0x00)
	}
	else
		return TRUE;

	//���������Socket�Ĵ򿪺�UDPģʽ����,������ģʽ��������Ҫ��Զ��������������
	//��ΪSocket����Ҫ��������,�����ڷ�������ǰ����������Ŀ������IP��Ŀ��Socket�Ķ˿ں�
	//���Ŀ������IP��Ŀ��Socket�Ķ˿ں��ǹ̶���,�����й�����û�иı�,��ôҲ��������������
}

/*******************************************************************************
* ������  : W5500_Interrupt_Process
* ����    : W5500�жϴ��������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Interrupt_Process(void)
{
  unsigned char i,j;
  IntDispose:
	W5500_Interrupt=0;//�����жϱ�־
	i = Read_W5500_1Byte(IR);//��ȡ�жϱ�־�Ĵ���
	Write_W5500_1Byte(IR, (i&0xf0));//��д����жϱ�־

	if((i & CONFLICT) == CONFLICT)//IP��ַ��ͻ�쳣����
	{
		 //�Լ���Ӵ���
	}

	if((i & UNREACH) == UNREACH)//UDPģʽ�µ�ַ�޷������쳣����
	{
		//�Լ���Ӵ���
	}

	i=Read_W5500_1Byte(SIR);//��ȡ�˿��жϱ�־�Ĵ���	
	if((i & S0_INT) == S0_INT)//Socket0�¼����� 
	{
		j=Read_W5500_SOCK_1Byte(0,Sn_IR);//��ȡSocket0�жϱ�־�Ĵ���
		Write_W5500_SOCK_1Byte(0,Sn_IR,j);
		if(j&IR_CON)//��TCPģʽ��,Socket0�ɹ����� 
		{
				S0_State|=S_CONN;//��������״̬0x02,�˿�������ӣ�����������������
				USER_SystemFlag.A.Flag_Standalone = 1;  //������������ɹ�
//				if(USER_SystemFlag.A.Flag_Sucess1 == 1)//���ּ��ܳɹ�
//        {
//           USER_SystemFlag.A.Flag_Standalone = 1;  //������������ɹ�
//        }
//        else
//        {
//            USER_SystemFlag.A.Flag_Standalone = 0;  //������������ɹ�
//        }
		}
		if(j&IR_DISCON)//��TCPģʽ��Socket�Ͽ����Ӵ���
		{
				Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
				Socket_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
				S0_State=0;//��������״̬0x00,�˿�����ʧ��
				USER_SystemFlag.A.Flag_Standalone = 0;  //�����������ʧ��
		}
		if(j&IR_SEND_OK)//Socket0���ݷ������,�����ٴ�����S_tx_process()������������ 
		{
			S0_Data|=S_TRANSMITOK;//�˿ڷ���һ�����ݰ���� 
		}
		if(j&IR_RECV)//Socket���յ�����,��������S_rx_process()���� 
		{
			S0_Data|=S_RECEIVE;//�˿ڽ��յ�һ�����ݰ�
		}
		if(j&IR_TIMEOUT)//Socket���ӻ����ݴ��䳬ʱ���� 
		{
				Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);// �رն˿�,�ȴ����´����� 
				S0_State=0;//��������״̬0x00,�˿�����ʧ��
				USER_SystemFlag.A.Flag_Standalone = 0;  //�����������ʧ��
		}
	}
	
	if((i & S1_INT) == S1_INT)//Socket1�¼����� 
	{
		j=Read_W5500_SOCK_1Byte(1,Sn_IR);//��ȡSocket1�жϱ�־�Ĵ���
		Write_W5500_SOCK_1Byte(1,Sn_IR,j);
		if(j&IR_CON)//��TCPģʽ��,Socket1�ɹ����� 
		{
			S1_State|=S_CONN;//��������״̬0x02,�˿�������ӣ�����������������
		}
		if(j&IR_DISCON)//��TCPģʽ��Socket�Ͽ����Ӵ���
		{
			Write_W5500_SOCK_1Byte(1,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
			Socket_Init(1);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
			S1_State=0;//��������״̬0x00,�˿�����ʧ��
		}
		if(j&IR_SEND_OK)//Socket0���ݷ������,�����ٴ�����S_tx_process()������������ 
		{
			S1_Data|=S_TRANSMITOK;//�˿ڷ���һ�����ݰ���� 
		}
		if(j&IR_RECV)//Socket���յ�����,��������S_rx_process()���� 
		{
			S1_Data|=S_RECEIVE;//�˿ڽ��յ�һ�����ݰ�
		}
		if(j&IR_TIMEOUT)//Socket���ӻ����ݴ��䳬ʱ���� 
		{
			Write_W5500_SOCK_1Byte(1,Sn_CR,CLOSE);// �رն˿�,�ȴ����´����� 
			S1_State=0;//��������״̬0x00,�˿�����ʧ��
		}
	}
	if((i & S2_INT) == S2_INT)//Socket2�¼����� 
	{
		j=Read_W5500_SOCK_1Byte(2,Sn_IR);//��ȡSocket2�жϱ�־�Ĵ���
		Write_W5500_SOCK_1Byte(2,Sn_IR,j);
		if(j&IR_CON)//��TCPģʽ��,Socket0�ɹ����� 
		{
				S2_State|=S_CONN;//��������״̬0x02,�˿�������ӣ�����������������
// 				USER_SystemFlag.A.Flag_Online = 1;  //�����������ɹ�
				if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
				{
						USER_SystemFlag.A.Flag_Online = 1;  //�����������ɹ�
            USER_LEDFlag.A.LED_Online = 1;  				//����ָʾ��
				}				
        else
        {
            USER_SystemFlag.A.Flag_Online = 0;  //����������ʧ��
            USER_LEDFlag.A.LED_Online = 0;  				//����ָʾ��
        }
		}
		if(j&IR_DISCON)//��TCPģʽ��Socket�Ͽ����Ӵ���
		{
        USER_SystemFlag.A.Flag_Online = 0;  //����������ʧ��
				USER_LEDFlag.A.LED_Online = 0;  				//����ָʾ��
        USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
        USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
        USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
        USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
        if(USER_SystemFlag.A.Flag_Standalone == 1)
        {
            if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
            {
                FunW5500_SendLED(0,0,0);
            }   
        }
				Write_W5500_SOCK_1Byte(2,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
				Socket_Init(2);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
				S2_State=0;//��������״̬0x00,�˿�����ʧ��
		}
		if(j&IR_SEND_OK)//Socket0���ݷ������,�����ٴ�����S_tx_process()������������ 
		{
			S2_Data|=S_TRANSMITOK;//�˿ڷ���һ�����ݰ���� 
		}
		if(j&IR_RECV)//Socket���յ�����,��������S_rx_process()���� 
		{
			S2_Data|=S_RECEIVE;//�˿ڽ��յ�һ�����ݰ�
		}
		if(j&IR_TIMEOUT)//Socket���ӻ����ݴ��䳬ʱ���� 
		{
        USER_SystemFlag.A.Flag_Online = 0;  //����������ʧ��
				USER_LEDFlag.A.LED_Online = 0;  				//����ָʾ��
        USER_LEDFlag.A.LED_15 = 0;  				//����������Ƿ�ı� 1��  0����
        USER_LEDFlag.A.LED_14 = 1;  				//�����ָʾ���Ƿ�ı� 1��  0����
        USER_LEDFlag.A.LED_Weight = 0;  			//����ָʾ��
        USER_LEDFlag.A.LED_Brake = 1;  			  //�ƶ�ָʾ��
        if(USER_SystemFlag.A.Flag_Standalone == 1)
        {
            if(USER_SystemFlag.A.Flag_Sucess == 1) //���ּ���
            {
                FunW5500_SendLED(0,0,0);
            }   	
        }
				Write_W5500_SOCK_1Byte(2,Sn_CR,CLOSE);// �رն˿�,�ȴ����´����� 
				S2_State=0;//��������״̬0x00,�˿�����ʧ��
		}
	}
  if((i & S3_INT) == S3_INT)//Socket3�¼����� 
	{
		j=Read_W5500_SOCK_1Byte(3,Sn_IR);//��ȡSocket3�жϱ�־�Ĵ���
		Write_W5500_SOCK_1Byte(3,Sn_IR,j);
		if(j&IR_CON)//��TCPģʽ��,Socket0�ɹ����� 
		{
				S3_State|=S_CONN;//��������״̬0x02,�˿�������ӣ�����������������		
		}
		if(j&IR_DISCON)//��TCPģʽ��Socket�Ͽ����Ӵ���
		{
				Write_W5500_SOCK_1Byte(3,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
				Socket_Init(3);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�
        S3_State=0;//��������״̬0x00,�˿�����ʧ��
		}
		if(j&IR_SEND_OK)//Socket0���ݷ������,�����ٴ�����S_tx_process()������������ 
		{
			S3_Data|=S_TRANSMITOK;//�˿ڷ���һ�����ݰ���� 
		}
		if(j&IR_RECV)//Socket���յ�����,��������S_rx_process()���� 
		{
			S3_Data|=S_RECEIVE;//�˿ڽ��յ�һ�����ݰ�
		}
		if(j&IR_TIMEOUT)//Socket���ӻ����ݴ��䳬ʱ���� 
		{
				Write_W5500_SOCK_1Byte(3,Sn_CR,CLOSE);// �رն˿�,�ȴ����´����� 
				S3_State=0;//��������״̬0x00,�˿�����ʧ��
		}
	}
	if(Read_W5500_1Byte(SIR) != 0) 
		goto IntDispose;
}



/*******************************************************************************
* ������  : W5500_Init
* ����    : ��ʼ��W5500�Ĵ�������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ʹ��W5500֮ǰ���ȶ�W5500��ʼ��
*******************************************************************************/
void W5500_Init(void)
{
	u8 i=0;

	Write_W5500_1Byte(MR, RST);//�����λW5500,��1��Ч,��λ���Զ���0
	Delay_ms(10);//��ʱ10ms,�Լ�����ú���

	//��������(Gateway)��IP��ַ,Gateway_IPΪ4�ֽ�unsigned char����,�Լ����� 
	//ʹ�����ؿ���ʹͨ��ͻ�������ľ��ޣ�ͨ�����ؿ��Է��ʵ��������������Internet
	Write_W5500_nByte(GAR, Gateway_IP, 4);
			
	//������������(MASK)ֵ,SUB_MASKΪ4�ֽ�unsigned char����,�Լ�����
	//��������������������
	Write_W5500_nByte(SUBR,Sub_Mask,4);		
	
	//���������ַ,PHY_ADDRΪ6�ֽ�unsigned char����,�Լ�����,����Ψһ��ʶ�����豸�������ֵַ
	//�õ�ֵַ��Ҫ��IEEE���룬����OUI�Ĺ涨��ǰ3���ֽ�Ϊ���̴��룬�������ֽ�Ϊ��Ʒ���
	//����Լ����������ַ��ע���һ���ֽڱ���Ϊż��
	Write_W5500_nByte(SHAR,Phy_Addr,6);		

	//���ñ�����IP��ַ,IP_ADDRΪ4�ֽ�unsigned char����,�Լ�����
	//ע�⣬����IP�����뱾��IP����ͬһ�����������򱾻����޷��ҵ�����
	Write_W5500_nByte(SIPR,IP_Addr,4);		
	
	//���÷��ͻ������ͽ��ջ������Ĵ�С���ο�W5500�����ֲ�
	for(i=0;i<8;i++)
	{
		Write_W5500_SOCK_1Byte(i,Sn_RXBUF_SIZE, 0x02);//Socket Rx memory size=2k
//		Write_W5500_SOCK_1Byte(i,Sn_TXBUF_SIZE, 0x04);//Socket Tx mempry size=4k  ��������4k
	}
	for(i=0;i<4;i++)
	{
		Write_W5500_SOCK_1Byte(i,Sn_TXBUF_SIZE, 0x04);//Socket Tx mempry size=4k  ��������4k
	}
	for(i=4;i<8;i++)
	{
		Write_W5500_SOCK_1Byte(i,Sn_TXBUF_SIZE, 0x00);//Socket Tx mempry size=0k  ��������0k
	}
	
	//��������ʱ�䣬Ĭ��Ϊ2000(200ms) 
	//ÿһ��λ��ֵΪ100΢��,��ʼ��ʱֵ��Ϊ2000(0x07D0),����200����
	Write_W5500_2Byte(W5500_RTR, 0x07d0);

	//�������Դ�����Ĭ��Ϊ8�� 
	//����ط��Ĵ��������趨ֵ,�������ʱ�ж�(��صĶ˿��жϼĴ����е�Sn_IR ��ʱλ(TIMEOUT)�á�1��)
	Write_W5500_1Byte(W5500_RCR,8);

	//�����жϣ��ο�W5500�����ֲ�ȷ���Լ���Ҫ���ж�����
	//IMR_CONFLICT��IP��ַ��ͻ�쳣�ж�,IMR_UNREACH��UDPͨ��ʱ����ַ�޷�������쳣�ж�
	//������Socket�¼��жϣ�������Ҫ���
	Write_W5500_1Byte(W5500_IMR,IM_IR7 | IM_IR6);
	Write_W5500_1Byte(SIMR,S0_IMR|S2_IMR|S3_IMR);
	Write_W5500_SOCK_1Byte(0, Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);
	Write_W5500_SOCK_1Byte(0,Sn_KPALVTR,3);
	
	//���ö˿�2TCP�ж�
  // 	Write_W5500_1Byte(SIMR,S2_IMR);
	Write_W5500_SOCK_1Byte(2, Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);
	Write_W5500_SOCK_1Byte(2,Sn_KPALVTR,3);
	
	//	Write_W5500_1Byte(SIMR,S1_IMR);//�����ӿ�S1�ж�
	Write_W5500_SOCK_1Byte(1, Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);//����socket������ɣ���ʱ���������ݵ��жϡ�
	
  //���ö˿�3 TCP�ж�
  // 	Write_W5500_1Byte(SIMR,S3_IMR);
	Write_W5500_SOCK_1Byte(3, Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);
	Write_W5500_SOCK_1Byte(3,Sn_KPALVTR,3);
}

/*******************************************************************************
* ������  : W5500_Socket_Set
* ����    : W5500�˿ڳ�ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �ֱ�����4���˿�,���ݶ˿ڹ���ģʽ,���˿�����TCP��������TCP�ͻ��˻�UDPģʽ.
*			�Ӷ˿�״̬�ֽ�Socket_State�����ж϶˿ڵĹ������
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//�˿�0��ʼ������
	{
		if(S0_Mode==TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
		}
	}
	
	if(S1_State==0)//�ӿ�1��ʼ������                  �����������socket1.
	{
		if(S1_Mode==TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(1)==TRUE)
				S1_State=S_INIT;
			else
				S1_State=0;
		}
		
		else if(S1_Mode==TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(1)==TRUE)
				S1_State=S_INIT;
			else
				S1_State=0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(1)==TRUE)
				S1_State=S_INIT|S_CONN;
			else
				S1_State=0;
		}
	}
	if(S2_State==0)//�˿�2��ʼ������
	{
		if(S2_Mode==TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(2)==TRUE)
				S2_State=S_INIT;
			else
				S2_State=0;
		}
		else if(S2_Mode==TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(2)==TRUE)
				S2_State=S_INIT;
			else
				S2_State=0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(2)==TRUE)
				S2_State=S_INIT|S_CONN;
			else
				S2_State=0;
		}	
	}  
  if(S3_State==0)//�˿�3��ʼ������
	{
		if(S3_Mode==TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(3)==TRUE)
				S3_State=S_INIT;
			else
				S3_State=0;
		}
		else if(S3_Mode==TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(3)==TRUE)
				S3_State=S_INIT;
			else
				S3_State=0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(3)==TRUE)
				S3_State=S_INIT|S_CONN;
			else
				S3_State=0;
		}	
	}
}

