/****************************************************************************
* �ļ���: AD7663.h
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2016.12.09    
* ����:      XX
* ˵��:   ȫ�ֱ������塢����
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
#ifndef __USER_DATA_H
#define __USER_DATA_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif
/* ����ȫ���ж����ȼ� */
#define 		NVIC_PriorityGroup_Set 			NVIC_PriorityGroup_1
	 
#define    TIM2_IRQChannelPreemptionPriority   					 1   //�����������ȼ� �����ж���һ���ж�������
#define    TIM2_IRQChannelSubPriority   											 0	 	//������Ӧ���ȼ�
	 
#define    TIM3_IRQChannelPreemptionPriority   					 1   //�����������ȼ� �����ж���һ���ж�������
#define    TIM3_IRQChannelSubPriority   											 2	 	//������Ӧ���ȼ�	 
	 
#define    EXTI15_10_IRQnChannelPreemptionPriority   0   //�����������ȼ�   ADת������ж�
#define    EXTI15_10_IRQnChannelSubPriority   						0	 	//������Ӧ���ȼ�	 
	 
#define    EXTI9_5_IRQnChannelPreemptionPriority   		1   //�����������ȼ�    W5500������������ж�
#define    EXTI9_5_IRQnChannelSubPriority   								1	 	//������Ӧ���ȼ�
	 	 
/* ����ȫ�ָ�λ�������� */	 
extern uint8_t Reset_Flag;   //��λ��������	 
#define	 RESET_MAX   10    //5S
	 
/* ����ȫ������Ͽ����� */
extern uint8_t Network0_StopFlg;       //����Ͽ���ʶ����  ��������
extern uint8_t Network1_StopFlg;       //����Ͽ���ʶ����  ���������
#define	 NETWORK_MAX   3              //2*3S

/* ����ȫ�ּ��ܱ��� */
extern uint8_t cID_Key[32];            //Keyֵ  �����ַ�
extern uint8_t Encryption_Cnt;        //���ܴ���
#define	 ENCRYPTION_MAX   10   //�����ܴ���	 
	 
/* ����ȫ�ָ������״̬��־ */	 
extern uint8_t System_State;   //ϵͳ���״̬��־	 
#define  Default_State																0     //Ĭ��״̬
#define  Adjust_State         												1		 //�궨״̬
#define  StaticWeight_State      								2		 //��̬���س���
#define  DynamicWeight_State      						3		 //��̬���س���

#define  StaticWeight_Motor_State      			4		 //Ħ�г���̬���س��� 
#define  StaticWeight_None_State      			5		 //���ظ���̨��̬���� 
#define  StaticWeight_Full_State      			    6		 //���ظ���̨��̬���� ���ؾ���
#define  SignalTest_State                          7     //�źŲ���״̬��ʶ
#define  DynamoStart_State                      8     //���������ʶ
#define  Retardant_State                           9     //���Ͳ��Ա�ʶ
#define  BrakeTest_None_State                10   //�����ƶ����Ա�ʶ
#define  BrakeTest_Full_State                   11   //�����ƶ����Ա�ʶ
#define  ReadData_State                           12   //��ȡ����Ա��ʶ

/* ����ȫ�ֹ���ģʽ ��Ӧ */	 	 
extern	uint8_t WorkState_Flg;              //����״̬��־ ��Ϊ����״̬  �궨״̬  �źŲ���״̬  ���״̬

#define  Default_State_Flg														    0    //Ĭ��״̬
#define  Adjust_State_Flg	         									    1		 //�궨״̬
#define  SignalTest_State_Flg	      						      2		 //�źŲ���״̬

#define  StaticWeight_State_Flg	      							3		 //���ؼ��״̬
#define  StaticWeight_Motor_State_Flg      			4		 //Ħ�г���̬���س��� ״̬

#define  StaticWeight_None_State_Flg      			5		 //���ظ���̨��̬���� ״̬
#define  StaticWeight_Full_State_Flg      			  6		 //���ظ���̨��̬���� ���ؾ���״̬
// #define  DynamoStart_State_Flg                     8     //�������״̬
#define  Retardant_State_Flg                          7     //���Ͳ���״̬
#define  BrakeTest_None_State_Flg               8   //�����ƶ�����״̬
#define  BrakeTest_Full_State_Flg                  9   //�����ƶ�����״̬
#define  BrakeTest_Motor_State_Flg              10   //Ħ���ƶ�����״̬
	 
/* ����ģ����ͨ��ID��ʶ */	  
extern uint8_t AD_Channel_ID; //ADת��ͨ��ID  �궨ʱʹ��
#define Left_Weight_ID     0x05  	//������
#define Right_Wight_ID			0x06		//������
#define Left_Brake_ID			  0x07		//���ƶ�
#define Right_Brake_ID     0x08		//���ƶ�
#define All_Weight_ID        0x2B		//���ϳ����ź�
#define FootPlat_ID           0x09		//̤�����ź�
#define Left_Move_ID				44		    //���
#define Right_Move_ID			45		    //�Ҹ�

#define AD_Clear_ID      0xFF       //����ͨ������ID

/* ���忪��ͨ��ID��ʶ */
#define IN_Weight_ID     		0x05  	//���ص�λ�źſ���ID  ��λ�źŵ�λ1  ����λΪ0
#define IN_Brake_ID					0x06		//�ƶ���λ�źſ���ID
#define IN_Raise_ID			  	0x25		//������λ�źſ���ID
#define IN_Protect_ID     	0x15		//�ǰ屣���źſ���ID
#define IN_LeftStop_ID      0x09		//��ͣ���źſ���ID
#define IN_RightStop_ID    0x0A		//��ͣ���źſ���ID




/* �����洢��W25Q64�׵�ַ һ��2048������ */	
/* ���������׵�ַ */
// #define Parameter0_Flash_Address       0     //��0����
// #define Parameter1_Flash_Address       1     //��1����
// #define Parameter2_Flash_Address       2     //��2����
// #define Parameter3_Flash_Address       3     //��3����
// #define Parameter4_Flash_Address       4     //��4����
// #define Parameter5_Flash_Address       5     //��5����
// #define Parameter6_Flash_Address       6     //��6����
// #define Parameter7_Flash_Address       7     //��7����
// #define Parameter8_Flash_Address       8     //��8����
// #define Parameter9_Flash_Address       9     //��9����
// #define Parameter10_Flash_Address     10     //��10����
// #define Parameter11_Flash_Address     11     //��11����
// #define Parameter12_Flash_Address     12     //��12����
// #define Parameter13_Flash_Address     13     //��13����
// #define Parameter14_Flash_Address     14     //��14����
#define ParameterAll_Flash_Address     0     //��14����
#define Parameter15_Flash_Address    15     //��15����  �������

#define ParameterIP_Flash_Address     20     //��20����

#define ParameterERR_Flash_Address     1     //��1����  ���ϴ���洢����
/* ģ����ͨ��ϵ�������ַ */
#define Left_Weight_Flash_Address       21     //��21����
#define Right_Wight_Flash_Address				22		 //��22����
#define Left_Brake_Flash_Address					23
#define Right_Brake_Flash_Address				24
#define All_Weight_Flash_Address					25
#define FootPlat_Flash_Address							26
#define Left_Move_Flash_Address					27
#define Right_Move_Flash_Address				28
/* ģ����ͨ��ϵ���궨���ϴ���ַ */
#define Left_Weight_Flash_Address1       29     //��29����
#define Right_Wight_Flash_Address1				30		 //��30����
#define Left_Brake_Flash_Address1					31
#define Right_Brake_Flash_Address1				32
#define All_Weight_Flash_Address1					33
#define FootPlat_Flash_Address1							34
#define Left_Move_Flash_Address1					35
#define Right_Move_Flash_Address1				36


/* ģ����ͨ������ֵ��ֵ */
#define Left_Weight_Limit       9999       //Kg
#define Right_Wight_Limit			  9999		   //Kg
#define Left_Brake_Limit				50000		 // N
#define Right_Brake_Limit				50000		 // N
#define All_Weight_Limit				20000		 // Kg
#define FootPlat_Limit					9999			 // N
#define Left_Move_Limit					999				 //mm
#define Right_Move_Limit				999

/* ��ͣ��ͣ������ */
#define con_LDN20   30  					// ����20%��60%�ڼ�ͣ������
#define con_LDN60   10  					// ����60%�����ڼ�ͣ������
#define con_LDNStop 10  					// ����ͣ����������� Ӳͣ��
// #define con_LDNHuan 20  			// ����60%�����ڼ仺��ͣ������
// #define con_LDNJi   10  				// ����60%�����ڼ伱��ͣ������


#define con_RDN20   30  					// �ҵ��20%��60%�ڼ�ͣ������
#define con_RDN60   10  					// �ҵ��60%�����ڼ�ͣ������
#define con_RDNStop 10  				// �ҵ��ͣ�����������  Ӳͣ��
// #define con_RDNHuan 20  			// �ҵ��60%�����ڼ仺��ͣ������
// #define con_RDNJi   10  				// �ҵ��60%�����ڼ伱��ͣ������

/***************----- �˿����ݻ����� -----***************/
#define   BUFFER_SIZE       3300   //���ڷ���  ���ջ�������С  ��  ���߻�������С

extern uint8_t Parameter_Buffer[100];	//�������ݴ������� 	

extern uint8_t Rx_Buffer[500];	//�˿ڽ������ݻ����� 
extern uint8_t Tx_Buffer[500];	//�˿ڷ������ݻ����� 	

extern uint8_t u8Send_Buffer[BUFFER_SIZE]; //���ڷ��ͺ�������
	



typedef union 
{
	struct
	{
		u8 Flag_1ms      								:1;		 		// 1msһ��
		u8 Flag_10ms										:1;		 		// 10mSһ��ѭ��
		u8 Flag_100ms  								:1;		 		// 100mS��־
		u8 Flag_500ms   								:1;     	  //500ms  ʵ��330ms
		u8 Flag_1s         								:1;		 		// 1s����ʱʹ��
		u8 Flag_UDP      								:1;	   		// UDP�㲥ʱ����
		u8 Flag_OverTime_Enable  :1;					//���ڽ������ݳ�ʱ��־ʹ��
		u8 Flag_Online  										:1;          //�����ػ�����������־ 1����   0����
		u8 Flag_Standalone							:1;          //������������ɹ���־ 1����   0ʧ��
		u8 Flag_TestC        							:1;          //��λ�������������ݱ�־    ��ʾ�������п����ϴ�����
		u8 Flag_TestC1        						:1;          //��λ�������������ݱ�־1  ���յ���λ��ָ�����1 ��ʾ���μ������ϴ��������
		u8 Flag_Ldynamo        				:1;          //����
		u8 Flag_Rdynamo        				:1;          //�ҵ��
		u8 Flag_Switch       							:1;          //������
		u8 Flag_ST        									:1;          //ͣ��������
		u8 Flag_Sucess                   :1;      //���ּ��ܳɹ���־
		u8 Flag_AutoD                      :1;      //������Ƿ������ϴ�
    u8 Flag_Sucess1                    :1;      //��������ּ��ܳɹ��Ƿ�
	}A;
	u32 Init;                       //��ʼ��ʹ��
} _unionFlag;

extern _unionFlag USER_SystemFlag;						   //	ϵͳ��־λ1
	
typedef union 
{
	struct
	{
		u8 LED_TJDown      :1;		 		// ̨�ܽ�    
		u8 LED_TJUp				  :1;		 		// ̨�ܾ�
		u8 LED_Down  				:1;		 		// ������
		u8 LED_Up   						:1;     	  //������
		u8 LED_Block         	:1;		 		//����
		u8 LED_PlugMinus   :1;	   		//��/��
		u8 LED_Brake  				 :1;				//�ƶ�
		u8 LED_Weight  			 :1;        //����
		u8 LED_Test						 :1;          //���
		u8 LED_Online					 :1;          //����
		u8 LED_10               :1;         //����
		u8 LED_11               :1;         //����
		u8 LED_12               :1;         //����
		u8 LED_13               :1;         //����
		u8 LED_14               :1;         //�����ָʾ���Ƿ�ı�   1�ı� 0����
		u8 LED_15               :1;         //��������������Ƿ�ı� 1�ı�  0����
	}A;
	u16 Init;                       //��ʼ��ʹ��
} _unionFlagLED;

extern _unionFlagLED USER_LEDFlag;						   //	ϵͳָʾ�Ʊ�־

typedef union 
{
		uint8_t u8_data[4];
		float float_data;
} _union_float_u8_type; //������  float��ת����u8

typedef union 
{
		uint8_t u8_data[2];
		uint16_t  u16_data;
} _union_u16_u8_type; //������  u16��ת����u8

#pragma pack(1)  //�洢��ʽ����
typedef union  
{
    struct 
    {				
			  uint8_t                        Parameter_NB; //��ϴ���			
				_union_float_u8_type Parameter_C0; //����궨ϵ��4�ֽ�
			  _union_float_u8_type Parameter_C1; //��һ�궨ϵ��4�ֽ�
				_union_float_u8_type Parameter_C2; //��һ�궨ϵ��4�ֽ�
				_union_float_u8_type Parameter_C3; //��һ�궨ϵ��4�ֽ�
    } 
		Load_Parameter;
		uint8_t 	Load_Flash[17];  //flash��������
} _unionType1; //

/*  */
typedef   struct 
{				
		uint8_t Retardant_Flag;            //���Ͳ�������Ƿ���ֹͣ��־
		uint8_t DynamoStart_Flag;				//���������־  ����ʱ�Ƿ�Ӳͣ���ж�
		uint8_t Par0_WeightTestTime; //��̬���ص���ʱ			s
		uint8_t Par1_RollerFlag; 				  //������Ͳ���� ��Ч��Ч��1��Ч  0��Ч
		uint8_t Par2_FootPlatFlag; 			//̤��������
		uint8_t Par3_BrakeReadyTime;//�ƶ�����׼��ʱ�� s
		uint8_t Par4_ZuzhiTestTime; 		 //���Ͳ���ʱ��	s
		uint8_t Par5_BrakeTestTime;		 //�ƶ�����ʱ�� s
		uint8_t Par6_Weight;		 						 //����̨�ֶ�ֵ 1  2  5
		uint16_t Par7_LeftAverZero;      //��̬ƽ����ֵ daN
		uint16_t Par8_RightAverZero;    //�Ҷ�̬ƽ����ֵ daN
		uint8_t Par9_LeftMaxZero;       //��̬�����ֵ daN
		uint8_t Par10_RightMaxZero;   //�Ҷ�̬�����ֵ daN 
		uint16_t Par11_MaxWeight;      //����������ֵ  *10Kg
		uint8_t Par12_MachineType;    //��е̨������ 0:����̨  1:��ͨ�ƶ�̨��  2:��������    3:Һѹ����  4��Ħ�г�
		uint16_t Par13_UpHeight;    		 //̨�ܾ����߶� mm
		uint8_t Par14_UpTime;    			   //̨�ܾ���ʱ�� s
																										 //Par20 IP����������W5500.h
} _structParameter; 			//�����ṹ��
#pragma pack()	

/* ������ģ����ͨ��ϵ��*/		
extern _unionType1  Left_Weight_par,  //����ϵ�� 1ͨ��
																 Right_Wight_par,  //����ϵ�� 2ͨ��
																 Left_Brake_par,    //����ϵ�� 3ͨ��
																 Right_Brake_par,  //����ϵ�� 4ͨ��
																 All_Weight_par,     //��������ϵ��  5ͨ��
																 FootPlat_par,        //̤����ͨ��  6ͨ��
																 Left_Move_par,    //������߶�   7ͨ��
																 Right_Move_par;  //�Ҿ����߶�   8ͨ��

extern _structParameter SysParameter;    //ϵͳ��������						

/* ����������ѹ */			
#define Zero_Max 1000  //����ѹ���ֵ  ����֮����㲻����
extern uint16_t  Zero_LeftWeight,
												 Zero_RightWight,
												 Zero_LeftBrake,
												 Zero_RightBrake,
												 Zero_AllWeight,
												 Zero_FootPlat,
												 Zero_LeftMove,
												 Zero_RightMove;
/* �źŲ����ϴ�Ƶ�ʱ��� */								 
extern vu8 Signal_TransferTime;	

extern vu16  TIM2_1ms;  //��ʱ��ms����  ������Ե���ʱʱʹ��
extern vu32 TIM2_count; //��ʱ��2������ʱ��������������1ms��ʱ��
extern vu32 TIM2_count_Delay; //��ʱ��2������ʱ��������������1ms��ʱ��
extern vu16 TIM2_count1;
extern vu16 TIM2_count2;
extern vu16 TIM2_count3;
extern vu16 TIM2_count4;


/* ���������� �ƶ�*/
#define StaticWeight_Init   200   //Ĭ�Ͼ�̬����  �������ʹ�� 


extern  uint16_t	u16Left1_TemporaryWeight[300],
													u16Right1_TemporaryWeight[300]; //��������10����ƽ����洢λ��  ��ʱ����
													
extern uint16_t  Left_DynamicWeight,   //��̬���� ��̬��ʾʹ��
													Right_DynamicWeight, //�Ҷ�̬���� ��̬��ʾʹ��
													All_DynamicWeight, 		//��̬����̨���� ��̬��ʾʹ��
													DynamicFootPlat, 				//��̬̤���� ��̬��ʾʹ��
													Left_DynamicBrake, 		//��̬�ƶ� ��̬��ʾʹ��
													Right_DynamicBrake; 	//�Ҷ�̬�ƶ� ��̬��ʾʹ��																				
													
													
extern uint8_t  DynamicWeight_Number;//��̬���������� ���6��																							
								
extern uint16_t 	 Left_DynamicWeight1[6],   //��̬�������
													 Right_DynamicWeight1[6]; //�Ҷ�̬�������								
extern  uint16_t   Left_StaticWeight,				//���־�̬���� 
									 Right_StaticWeight,  		//���־�̬����
									 Motor_StaticWeight, 		//Ħ�о�̬���� ��̨��
									 AllNone_StaticWeight, 	//���ظ���̨����
									 AllFull_StaticWeight,	  //���ظ���̨����
									 Left_BrakeSum,           //���ƶ������ֵ  �����ƶ�
									 Right_BrakeSum,         //���ƶ������ֵ  �����ƶ�
									 Left_BrakeSub,           //���Ʋ����ֵ  �����ƶ�
									 Right_BrakeSub,         //���Ʋ����ֵ  �����ƶ�
									 Left_BrakeSumJJ,       //���ƶ������ֵ  �����ƶ�
									 Right_BrakeSumJJ,     //���ƶ������ֵ  �����ƶ�
									 Left_BrakeSubJJ,        //���Ʋ����ֵ  �����ƶ�
									 Right_BrakeSubJJ,      //���Ʋ����ֵ  �����ƶ�
									 Left_BrakeAve,           //������
									 Right_BrakeAve;         //������

//���Ҷ�̬��ֵ								
extern uint16_t u16Left_BrakeZero, //��̬��ֵ
				        u16Right_BrakeZero;//�Ҷ�̬��ֵ

//ͣ��������ͣ������
extern _union_u16_u8_type	Left_StopTime,Right_StopTime;//����ͣ��ʱ��
extern uint8_t Left_StopFlag,Right_StopFlag;					 //����ͣ�����ͱ�־ ��1��Ӳͣ��  ��0����ͣ��
	
extern uint16_t u16Protect_Limit;   					//������������ֵ		
													 
extern uint16_t u16Left_Data[802],    						//���ƶ���������
												u16Right_Data[802];    					//���ƶ���������
extern uint8_t u8Tran_CurrentZ[3250],				//�����ϴ�����
											u8Tran_CurrentC[3250],      	//�����ƶ�����
											u8Tran_CurrentJ[3250],       	//�����ƶ�����
											u8Tran_CurrentN[3250];       	//̤��������					

/***************----- IP�������� -----***************/
extern uint8_t W5500_Parameter[27];       //����IP��������				���һλ�洢�Ƿ��һ���޸�IP  0x55��ʾ�����IP				
													 
/*************���ڷ��ͱ���**********************/ 
extern  uint8_t con_TranB[19] ;   // �汾��ϢVE01020320170224
extern  uint8_t con_TranAACK[3];   // ��ȷӦ��
extern  uint8_t con_TranZACK[3];   // ����Ӧ��
extern  uint8_t con_TranXACK[3];   // ��ǰ�����޷�ִ��
extern  uint8_t con_TranKACK[6];   //��������ش�


extern  uint8_t con_TranU[3];          // ����
extern  uint8_t con_TranI[3];           // ��ʼ�����
extern  uint8_t con_TranR[3];         // ��λ���

extern uint8_t con_TranMI[8]; 			// �ϴ�M I  UDP�㲥
extern  uint8_t con_TranMW[5];  // �ϴ�M W��̬���ؼ��
extern  uint8_t con_TranMA[5];  // �ϴ�M A��̬���ؼ��
extern  uint8_t con_TranMF1[5]; // �ϴ�M F 1���ϳ��ؼ��
extern  uint8_t con_TranMF2[5]; // �ϴ�M F 2���ؾ������
extern  uint8_t con_TranML[4];   // // �ϴ�M L Ħ�г����ؼ�� 
extern  uint8_t con_TranMM[4];  // �ϴ�M M Ħ�г��ƶ����
extern uint8_t con_TranMD[4];   // �ϴ�M D�����������
extern uint8_t con_TranMZ[4];   // �ϴ�M Z���Ͳ�������
extern uint8_t con_TranMB[4];   // �ϴ�M B�����ƶ���������
extern uint8_t con_TranMJ[4];   // �ϴ�M J�����ƶ���������
extern uint8_t con_TranM2[4];   // �ϴ�M 2��

extern  uint8_t con_TranT0[4];    //  ����������
extern  uint8_t con_TranT1[4];    //�������ʹ� ����ʧ��
extern  uint8_t con_TranT2[4];    //�������ʹ� ����ʧ��
extern  uint8_t con_TranT3[4];    // ���ؾ�����ʱ ����λ
extern  uint8_t con_TranT4[4];    //  ��������λ
extern  uint8_t con_TranT7[4];    //���������쳣
extern  uint8_t con_TranT8[4];    //���������쳣
extern  uint8_t con_TranT9[4];    //����ֵ̫С
extern  uint8_t con_TranTD[4];    //����������
extern  uint8_t con_TranTF[4];    //���ؾ������

extern  uint8_t con_TranS[5];      //״̬�ش�ָ��

extern  uint8_t con_TranE1W[5];      //����1���óɹ��ش�ָ��
extern  uint8_t con_TranE2W[5];      //����2���óɹ��ش�ָ��
extern  uint8_t con_TranEIW[5];       //����IP���óɹ��ش�ָ��
extern uint8_t con_TranHX0[5];    		 //����ָ�����
extern uint8_t con_TranHX1[5];    		 //���ܳ�����
extern uint8_t con_TranHEF[5];    		 //����ʧ��
extern uint8_t con_TranHET[5];    		 //���ܳɹ�

extern uint8_t con_TranWG[4];       //��������ش�
extern uint8_t con_TranWE[6];       //�����쳣WE00

extern uint8_t con_TranF[29];         //Fָ���豸��Ϣ
extern uint8_t con_TranO[3];          //����ָ��

#ifdef __cplusplus
}
#endif
#endif /* __AD7663_H */

