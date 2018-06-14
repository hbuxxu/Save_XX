/*************************************************
��  �ڣ�20160720��V1.0��
�޸��ˣ�����
��  �ݣ������˲��������ж�
**************************************************
��  �ڣ�20160926��V1.1����û�·�������
�޸��ˣ�����
��  �ݣ����ӱ�׼�ٶȲɼ�ֵ�ϴ������㹤λ�����ɼ������Ƿ�������
*************************************************
��  �ڣ�201601114��V1.2��
�޸��ˣ�����
��  �ݣ������޸ģ����ӳ�4��ָʾ�ƣ�
		LD1��LD2ָʾ�����������ݹ��ϣ���Ϊ��������Ϊ���ϣ�
		LD3��LD4ָʾ����Ӳͣ����ͣ���Ⱥ���ΪӲͣ������Ϊ��ͣ����
**************************************************/


#include "STC15F2K60S2.h"
#include "stdio.h"
#include "absacc.h"
#include "intrins.h"

#define  High   1   //�ߵ�ƽ
#define  Low    0   //�͵�ƽ

#define	 uint   unsigned int
#define	 ulong  unsigned long 
#define	 uchar  unsigned char

#define FOSC 11059200L          	//ϵͳƵ��
#define BAUD 115200             	//���ڲ�����
#define High 1   //��ͣ��
#define Low 0    //ͣ��

sbit SWL_in  = P3^2;      //��������ٶ������ź�
sbit SWL_out = P3^5;      //����ͣ���ź�	  0ͣ��   2^6
sbit SWR_in  = P3^3;      //�ҵ������ٶ������ź�
sbit SWR_out = P3^4;      //�ҵ��ͣ���ź�	  0ͣ��	  P2^7
sbit SW_cai  = P1^3;      //��λ����������ͣ�����ź�   ԭ����1.3

//sbit SW_Led  = P1^4;      //����״̬
//sbit SW_MOL  = P2^0;      //���  LD2
//sbit SW_MOR  = P2^1;      //�ҵ�  LD3

#define Bright 0   //ָʾ����
#define Dark 1    //ָʾ��Ϩ��
sbit LED_L_GZ  = P5^5;      //�������Ͳ�������ݴ���ָʾ��  LD1
sbit LED_R_GZ  = P2^0;      //�������Ͳ�������ݴ���ָʾ��  LD2
sbit LED_L_Stop  = P1^5;        //���Ӳͣ����ͣ��ָʾ   LD4
sbit LED_R_Stop  = P2^1;        //�ұ�Ӳͣ����ͣ��ָʾ   LD3	   P1^4;  20161206

sbit MC_Flg6_24= P1^4;     //6/24����ѡ��λ	  P1^5;20161206
sbit HY_tx4 = P1^0;       //������ͨ����������I/O�� 3��								   
sbit HY_tx3= P1^1;		  //4��
sbit HY_tx2= P1^2;		  //5��
sbit HY_tx1= P2^5;		  //28��

sbit Lstop=P3^6;		  //�����ͣ������
sbit Rstop=P3^7;		  //�ұ���ͣ������

uchar Counter_70ms=0;   //Led��������� ��˸��
float Hy_sd=0;          //�������趨
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

#define Hy_sd10 1.12	//10% �����������ж�ͣ��������ֵ

uint Left_t0,Left_t1,Left_t2,Left_linshi,Left_Old,Left_New;
uint Right_t0,Right_t1,Right_t2,Right_linshi,Right_Old,Right_New;  //��������Ƚ��±�ֵ
uchar ucLeft_T0,ucRight_T1;		//ʵʱ��ⶨʱ������ֵ �������

#define Overtime_Flg6  0x9D//0x4F	 40000 ��ʱ�ж�24��ʱ20000  6��ʱ40000
#define Overtime_Flg24 0x4F
#define Index_Flg24 24     //�洢���ݱ�� 6�׵�Ϊ6	24�ݵ�Ϊ24
#define Index_Flg6 6
#define OverConstant_24 20000
#define OverConstant_6  39000
uchar uc_Overtime_Flg;//6/24ѡ��ʱʹ�õĳ�ʱ����
uchar uc_Index_Flg;   //6/24ѡ��ʱʹ�õĴ洢�����±����
uint  ui_OverConstant;

uint ui_Average_L3,ui_Average_R3; //3���ƽ��ʹ�õĻ�׼ֵ
ulong uli_All_L,uli_All_R;  //24���׼ֵ��Ӻ�

unsigned char bdata Flag ;               //ϵͳ��־,flagΪ��λѰַ����

sbit  Watch_Flag=  Flag^0;        //���ڵ�����ʾ�󣬽����¼m1�����ѭ��
sbit  Wal_Flag  =  Flag^1;        //��ͣ����־   ,ֻ��Ϊ����ߴ�������Ч�ʣ�����û�иñ�־��ͣ�������ɨ�軬���ʣ��ж�ͣ��
sbit  War_Flag  =  Flag^2;        //��ͣ����־
sbit  INT0_Flag =  Flag^3;        //�������½��ص��������жϱ�־
sbit  INT1_Flag = Flag^4;         //�������½��ص��������жϱ�־  
sbit  IE_Flag = Flag^5;           //�����ظ����ⲿ�ж�  
sbit  Lstop_Flag = Flag^6;        //��Ӳͣ����ͣ����Ч�жϱ�־	1 ��Ч 0��Ч
sbit  Rstop_Flag = Flag^7 ;       //��Ӳͣ����ͣ����Ч�жϱ�־


uint  LineL,LineR;	//��¼24�����������±����	
//unsigned  char  Mj=0;				//6/24����ѡ��
//

union						 //���������ڴ�ŵ������ٶ�ֵ ������ʱ��
{									 
    uint  i_Data;
    uchar b_Data[2];
}uint_m2_Right;

union						 //���������ڴ������ʱ�� ������ʱ��
{									 
    uint  t_Data;
    uchar c_Data[2];
}uint_m1_Left;

uint Left1_buf[24]={0};//����׼�ٶ�ʹ��
uint Left2_buf[24]={0};//���ʵʱ����ٶ�ʹ��
uint idata Right1_buf[24]={0};//�Ҵ��׼�ٶ�ʹ��
uint idata Right2_buf[24]={0};//�Ҵ�ʵʱ����ٶ�ʹ��
uchar ucLeft_T0_Count,ucRight_T1_Count;//�ж��쳣ͣ��

//uint Left_Plus[336]={0};
//uint Right_Plus[336]={0};

void SendData(unsigned char dat);
void fn_Set_Hyl(void);	//����������
void fn_Compare24(void);//24���ж�ͣ���㷨

void main( void ) 
{  	   

	uint uc_i;//����ʹ��
   //��ʱ�����ⲿ�жϳ�ʼ�� 
    TMOD = 0x11; 	  //0001 0001��ʱ�������ڷ�ʽ1 ��Ϊ16λ�Ķ�ʱ��/������
   //���ô��ڲ�����	
	PCON = 0x00;  
    SCON = 0x50;                 //���ڹ����ڷ�ʽ1��8λ�ɱ䲨����
	T2L =(65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    T2H =(65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2	 ����Ƶ�ٶ��Ǵ�ͳ8051��12��
    AUXR |= 0x01;               //ѡ��ʱ��2Ϊ����1�Ĳ����ʷ�����
   
    SWL_out=Low; //ͣ��
	SWR_out=Low; //ͣ��  ����״̬�޷���������λ�������������ź�
	//ָʾ�Ƴ�ʼ��������	�й���ָʾ��Ϩ��
	LED_L_GZ=Bright;
	LED_R_GZ=Bright;
	LED_L_Stop=Dark;	//��ͣ��Ĭ�ϵ���
	LED_R_Stop=Dark;    //��ͣ��Ĭ�ϵ���
	IE_Flag=0;
	Watch_Flag=0;          //ʵʱ����ȡ��׼�ٶ�״̬��־ 1 ����ʵʱ���  0ȡ��׼�ٶȼ��
	Wal_Flag  =  0;          //��ͣ����־ 
	War_Flag  =  0;          //��ͣ����־
	INT0_Flag =  0;          //�������½��ص��������жϱ�־
	INT1_Flag = 0;           //�������½��ص��������жϱ�־ 
	fn_Set_Hyl();
    while(1)
    {		
		TI=1;	 //Ϊprintfʹ��
		while((IE_Flag==0)&&(SW_cai==1))//����״̬
		{
			Watch_Flag=1;
			IE_Flag=0; 	//����״̬ IE_Flag���� 
			IE=0x00;	//�ر������ж�
			INT_CLKO=0x00;//�ر��ⲿ�ж�2 3 ֻ���½����ж�	  							
		}		
        while(SW_cai==0) // ȡ��׼�ٶ�״̬
        {	
            if(IE_Flag==0)
            { 
				INT_CLKO=0x30;//���ⲿ�ж�2��3
				EA=1;//�����ж�
				for(uc_i=0;uc_i<24;uc_i++)
				{
					Left1_buf[uc_i]=1;
					Left2_buf[uc_i]=1;
					Right1_buf[uc_i]=1;	  //��֤��ĸ��Ϊ0
					Right2_buf[uc_i]=1;	
				}
				SWL_out = High; //����״̬��ͣ��
				SWR_out = High; //��ͣ��
				//ָʾ�ƾ�����	����ʱ�й���ָʾ��Ϩ��
				LED_L_GZ=Bright;
				LED_R_GZ=Bright;
				LED_L_Stop=Dark;
				LED_R_Stop=Dark;  //��ͣ�� Ĭ����
				Wal_Flag=0;     		    //��ͣ����־ 
				War_Flag=0;		            //��ͣ����־
				INT0_Flag=0;          //�������½��ص��������жϱ�־
				INT1_Flag=0;           //�������½��ص��������жϱ�־ 	
                IE_Flag=1;	  //��֤��һ�ξ���
				Watch_Flag=1; //ȡ��׼�ٶ�ʹ�ã����ⲿ�ж���ʹ���жϴ洢λ��
				Lstop_Flag=0; //���Ӳͣ����ͣ����־����
				Rstop_Flag=0; //�ұ�Ӳͣ����ͣ����־����
				LineL=0;
				LineR=0;	  //�����±���0
                TH0=0X01;					     //��0��ʱ��T0��T1������T0��T1
                TL0=0X01;
                TH1=0X01;					     //��0��ʱ��T0��T1������T0��T1
                TL1=0X01;
                TR1=1;    //����ʱ��1
				TR0=1; 	  //����ʱ��0
				IT0=1;    //�ⲿ�ж�0�½����ж�
				IT1=1;    //�ⲿ�ж�1�½����ж�
              //  IE=0X85;  //��������Ժ󣬿��ⲿ�ж�0��1
				IE=0X8F;  //����ȡ��׼�ٶ�״̬�������ж�  ��ʱ��0�ж� ��ʱ��1�ж� �ⲿ�ж�0��1
            } 
			while(SW_cai==0)
			{
				Watch_Flag=1; //ȡ��׼�ٶ�ʹ�ã����ⲿ�ж���ʹ���жϴ洢λ��
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
		while((IE_Flag==1)&&(SW_cai==1)) //��������״̬
		{
			Watch_Flag=0; //ʵʱ���״̬��־��0	
			if(uc_Index_Flg==Index_Flg24)
			{
				//3��ƽ������
				uli_All_L=0;
				uli_All_R=0;
				for(uc_i=0;uc_i<24;uc_i++)
				{
					uli_All_L+=Left1_buf[uc_i];
					if((Left1_buf[uc_i]>10000)||(Left1_buf[uc_i]<6000))	  //����9500ʱ��ߵ�������С��6000
					{
						//����9500ʱ��ߵ������С��6000
						LED_L_GZ=Dark;	
					}
					uli_All_R+=Right1_buf[uc_i];	  //��֤��ĸ��Ϊ0
					if((Right1_buf[uc_i]>10000)||(Right1_buf[uc_i]<6000))
					{
				 		//����9500ʱ�ұߵ������С��6000
						LED_R_GZ=Dark;
					}	
				}
				ui_Average_L3=uli_All_L/8;
				ui_Average_R3=uli_All_R/8;
				if(ui_Average_L3>30000)
				{
					 Wal_Flag=1;//����ͣ����־��λ
					 EX0=0; //�ر��ⲿ�ж�0
					 TR0=0; //��ʱ��0ֹͣ����
					 ET0=0; //�ض�ʱ��0�ж�
					 SWL_out = Low; //����ͣ��
					 //LED_L_Stop=Bright; 	//��Ӳָʾ�� Ӳͣ��ָʾ����
				}
				if(ui_Average_R3>30000)
				{
					 War_Flag=1;//�ҵ��ͣ����־��λ
					 EX1=0; //�ر��ⲿ�ж�1
					 TR1=0; //��ʱ��1ֹͣ����
					 ET1=0; //�ض�ʱ��1�ж�
					 SWR_out=Low; //�ҵ��ͣ
					 //LED_R_Stop=Bright;  //��Ӳָʾ�� Ӳͣ��ָʾ����
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
				 LED_L_Stop=Bright;  //��Ӳָʾ�� Ӳͣ��
			}
			if((Rstop_Flag==1)&&(War_Flag==1))
			{
				 LED_R_Stop=Bright;  //��Ӳָʾ�� Ӳͣ��
			}
			while(Watch_Flag==0)
			{
				if((Lstop_Flag==1)&&(Wal_Flag==0))	//���Ӳͣ����ͣ���ж�
				{
					if(Lstop==1)   //�����ͣ��������
					{
						Wal_Flag=1;	
						LED_L_Stop=Dark;  //��ͣ��
						EX0=0; //�ر��ⲿ�ж�0
						TR0=0; //��ʱ��0ֹͣ����
						ET0=0; //�ض�ʱ��0�ж�
						SWL_out = Low; //����ͣ��		
					}	
				}
				if((Rstop_Flag==1)&&(War_Flag==0))	//�ұ�Ӳͣ����ͣ���ж�
				{
					if(Rstop==1)   //�ұ���ͣ��������
					{
						War_Flag=1;	
						LED_R_Stop=Dark;  //��ͣ��
						EX1=0; //�ر��ⲿ�ж�1
						TR1=0; //��ʱ��1ֹͣ����
						ET1=0; //�ض�ʱ��1�ж�
						SWR_out=Low; //�ҵ��ͣ		
					}	
				}	
				fn_Compare24();	
				if((Wal_Flag==1)&&(War_Flag==1))
				{
					Watch_Flag=1;//0ʵʱ���״̬  1�˳�ʵʱ���״̬
					IE_Flag=0;	 //0�������������״̬
				    Wal_Flag=0;  //��ͣ����־ 
					War_Flag=0;	 //��ͣ����־
					IE=0x00;	//�ر������ж�
					INT_CLKO=0x00;//�ر��ⲿ�ж�2 3	
				}
			}
		}
    } 
}

//��ʱ��0���жϺ���
void Timer0_Interrupt( void ) interrupt 1    //
{	
    TF0 = 0;
   	if(Watch_Flag==1)//��׼�ٶ�
	{
		//Left1_buf[LineL++]=60000;
	}
	else //ʵ���ٶ�
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

//�ⲿ�ж�1�������� m2 ���жϺ���  �ⲿ�����½��ش�����������T0��0��ʼ����   
void INT1_Interrupt( void ) interrupt 2
{
    EX1=0;
//	ET1=1;//����ʱ��1�ж�
    uint_m2_Right.b_Data[0] = TH1;
    uint_m2_Right.b_Data[1] = TL1;
	if(Watch_Flag==1)//ȡ��׼�ٶ�״̬
	{
		Right1_buf[LineR++]=uint_m2_Right.i_Data; 
		
	}
	else	//ʵʱ����ж�״̬
	{
		Right2_buf[LineR++]=uint_m2_Right.i_Data;
		INT1_Flag=1;//�����ж�һ�� ʵʱ���Ƚ�һ��
	}
	if(LineR>=uc_Index_Flg)
	{
		LineR=0;
	}
    TH1=0x01;
    TL1=0x01;
    EX1=1;  
}

void INT0_Interrupt( void ) interrupt 0	//������ m1
{
    EX0=0;
//	ET0=1;//����ʱ��0�ж�
    uint_m1_Left.c_Data[0] = TH0;
    uint_m1_Left.c_Data[1] = TL0;
	if(Watch_Flag==1)//��׼�ٶ�
	{
		Left1_buf[LineL++]=uint_m1_Left.t_Data;
		
	}
	else //ʵ���ٶ�
	{
		Left2_buf[LineL++]=uint_m1_Left.t_Data;
		INT0_Flag=1;//�����ж�һ�� ʵʱ���Ƚ�һ��
	}
	if(LineL>=uc_Index_Flg)
	{
		LineL=0;
	}	
    TH0=0x01;
    TL0=0x01;
    EX0=1;
}
//�ⲿ�ж�2 ���Ӳͣ����ͣ���ж�
void INT2_Interrupt( void ) interrupt 10
{
	if(Lstop==0)
	{
		Lstop_Flag=1;//Ӳͣ����ͣ����־��Ч
		INT_CLKO &= 0xEF;//1110 1111��ֹ�ⲿ�ж�2
	}		
}
//�ⲿ�ж�3 �ұ�Ӳͣ����ͣ���ж�
void INT3_Interrupt( void ) interrupt 11
{
	if(Rstop==0)
	{
		Rstop_Flag=1;//Ӳͣ����ͣ����־��Ч
		INT_CLKO &= 0xDF;//1101 1111��ֹ�ⲿ�ж�3
	}
}
void SendData(unsigned char dat)
{
    SBUF=dat;
    while(!TI);
    TI=0;
}

void fn_Set_Hyl(void)	//����������  6/24��ѡ��
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
	uc_Overtime_Flg=Overtime_Flg24; //6/24ѡ��ʱʹ�õĳ�ʱ����
	uc_Index_Flg=Index_Flg24;       //6/24ѡ��ʱʹ�õĴ洢�����±����
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

void fn_Compare24(void)//24���ж�ͣ���㷨
{
	if(Wal_Flag==0)//δ����ͣ���ź�
	{		
		if(INT0_Flag==1)//�������ж�
		{
			INT0_Flag=0;
			switch(LineL)  //����ͣ���ж�
			{
				case 0:
					   Left_t0=21;
					   Left_t1=22;
					   Left_t2=23; //��ǰֵ
					   break;
			    case 1:
					   Left_t0=22;
					   Left_t1=23;
					   Left_t2=0; //��ǰֵ
					   break;
				case 2:
					   Left_t0=23;
					   Left_t1=0;
					   Left_t2=1; //��ǰֵ
					   break;
				default:
					   Left_t0=LineL-3;
					   Left_t1=LineL-2;
				       Left_t2=LineL-1; //��ǰֵ
					   break;	
			}
			Left_linshi=Left2_buf[Left_t2]+Left2_buf[Left_t1]+Left2_buf[Left_t0];
			if(Left_linshi>ui_Average_L3)
			{
				if(((float)Left_linshi/(float)ui_Average_L3)>=Hy_sd) //��������ﵽ��������ͣ�� ��
				{
						Wal_Flag=1;//����ͣ����־��λ
						EX0=0; //�ر��ⲿ�ж�0
						TR0=0; //��ʱ��0ֹͣ����
						ET0=0; //�ض�ʱ��0�ж�
						SWL_out = Low; //����ͣ��												
				}
			}
			//3�������ж�
			if(((float)Left2_buf[Left_t2]/(float)Left1_buf[Left_t2])>=Hy_sd)
			{
				if(((float)Left2_buf[Left_t1]/(float)Left1_buf[Left_t1])>((float)Left2_buf[Left_t0]/(float)Left1_buf[Left_t0]))
				{
					if(((float)Left2_buf[Left_t1]/(float)Left1_buf[Left_t1])>=Hy_sd10)
					{
						if(((float)Left2_buf[Left_t0]/(float)Left1_buf[Left_t0])>=Hy_sd10)
						{
							Wal_Flag=1;//����ͣ����־��λ
							EX0=0; //�ر��ⲿ�ж�0
							TR0=0; //��ʱ��0ֹͣ����
							ET0=0; //�ض�ʱ��0�ж�
							SWL_out = Low; //����ͣ��
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
			switch(LineL)  //����ͣ���ж�
			{
				case 0:
					   Left_t0=21;
					   Left_t1=22;
					   Left_t2=23; //��ǰֵ
					   break;
			    case 1:
					   Left_t0=22;
					   Left_t1=23;
					   Left_t2=0; //��ǰֵ
					   break;
				case 2:
					   Left_t0=23;
					   Left_t1=0;
					   Left_t2=1; //��ǰֵ
					   break;
				default:
					   Left_t0=LineL-3;
					   Left_t1=LineL-2;
				       Left_t2=LineL-1; //��ǰֵ
					   break;	
			}
			Left_linshi=Left2_buf[Left_t2]+Left2_buf[Left_t1]+Left2_buf[LineL];
			if(Left_linshi>ui_Average_L3)
			{
				if(((float)Left_linshi/(float)ui_Average_L3)>=Hy_sd) //��������ﵽ��������ͣ�� ��
				{
						Wal_Flag=1;//����ͣ����־��λ
						EX0=0; //�ر��ⲿ�ж�0
						TR0=0; //��ʱ��0ֹͣ����
						ET0=0; //�ض�ʱ��0�ж�
						SWL_out = Low; //����ͣ��				
				}
			}
		}
		if((Lstop_Flag==1)&&(Wal_Flag==1))
		{
			LED_L_Stop=Bright;//Ӳͣ����  LD3��
		}
	}
	if(War_Flag==0)//δ����ͣ���ź�
	{
		if(INT1_Flag==1)//�������ж�
		{
			INT1_Flag=0;
			switch(LineR)  //�ҵ��ͣ���ж�
			{
				
			    case 0:
					   Right_t0=21;
					   Right_t1=22;
					   Right_t2=23; //��ǰֵ
					   break;
			    case 1:
					   Right_t0=22;
					   Right_t1=23;
					   Right_t2=0; //��ǰֵ
					   break;
				case 2:
					   Right_t0=23;
					   Right_t1=0;
					   Right_t2=1; //��ǰֵ
					   break;
				default:
					   Right_t0=LineR-3;
					   Right_t1=LineR-2;
				       Right_t2=LineR-1; //��ǰֵ
					   break;	
			}
			Right_linshi=Right2_buf[Right_t2]+Right2_buf[Right_t1]+Right2_buf[Right_t0];
			if(Right_linshi>ui_Average_R3)
			{
				if(((float)Right_linshi/(float)ui_Average_R3)>=Hy_sd) //��������ﵽ��������ͣ�� ��
				{
						War_Flag=1;//�ҵ��ͣ����־��λ
						EX1=0; //�ر��ⲿ�ж�1
						TR1=0; //��ʱ��1ֹͣ����
						ET1=0; //�ض�ʱ��1�ж�
						SWR_out=Low; //�ҵ��ͣ							
				}
			}
			//3�������ж�
			if(((float)Right2_buf[Right_t2]/(float)Right1_buf[Right_t2])>=Hy_sd)
			{
				if(((float)Right2_buf[Right_t1]/(float)Right1_buf[Right_t1])>((float)Right2_buf[Right_t0]/(float)Right1_buf[Right_t0]))
				{
					if(((float)Right2_buf[Right_t1]/(float)Right1_buf[Right_t1])>=Hy_sd10)
					{
						if(((float)Right2_buf[Right_t0]/(float)Right1_buf[Right_t0])>=Hy_sd10)
						{
							War_Flag=1;//�ҵ��ͣ����־��λ
							EX1=0; //�ر��ⲿ�ж�1
							TR1=0; //��ʱ��1ֹͣ����
							ET1=0; //�ض�ʱ��1�ж�
							SWR_out=Low; //�ҵ��ͣ
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
			switch(LineR)  //�ҵ��ͣ���ж�
			{
				
			    case 0:
					   Right_t0=21;
					   Right_t1=22;
					   Right_t2=23; //��ǰֵ
					   break;
			    case 1:
					   Right_t0=22;
					   Right_t1=23;
					   Right_t2=0; //��ǰֵ
					   break;
				case 2:
					   Right_t0=23;
					   Right_t1=0;
					   Right_t2=1; //��ǰֵ
					   break;
				default:
					   Right_t0=LineR-3;
					   Right_t1=LineR-2;
				       Right_t2=LineR-1; //��ǰֵ
					   break;	
			}
			Right_linshi=Right2_buf[Right_t2]+Right2_buf[Right_t1]+Right2_buf[LineR];
			if(Right_linshi>ui_Average_R3)
			{
				if(((float)Right_linshi/(float)ui_Average_R3)>=Hy_sd)
				{
						War_Flag=1;//�ҵ��ͣ����־��λ
						EX1=0; //�ر��ⲿ�ж�1
						TR1=0; //��ʱ��1ֹͣ����
						ET1=0; //�ض�ʱ��1�ж�
						SWR_out=Low; //�ҵ��ͣ��						
				}
			}	
		}
		if((Rstop_Flag==1)&&(War_Flag==1))
		{
			LED_R_Stop=Bright;//Ӳͣ����  LD4��
		}
	}
}
