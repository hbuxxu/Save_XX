/****************************************************************************
* �ļ���: user_data.c
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2017.02.09    
* ����:      xx
* ˵��:   ȫ�ֱ�������Ͳ�������
*****************************************************************************/
/*=============================================================================
*******�޸���־*******
1����    �ڣ�
��    �㣺
�� �� �ˣ�
������ݣ�
�汾��:
================================================================================*/
#include "user_data.h"

/* ȫ�ֱ��� */

uint8_t System_State;               //ϵͳ���״̬��־
_unionFlag USER_SystemFlag; //��ʱ��������־λ
_unionFlagLED USER_LEDFlag;						   //	ϵͳָʾ�Ʊ�־

uint8_t WorkState_Flg;              //����״̬��־ ��Ϊ����״̬  �궨״̬  �źŲ���״̬  ���״̬

uint8_t Reset_Flag=0;   							//��λ��������

uint8_t Network0_StopFlg=0;       //����Ͽ���ʶ����  ��������
uint8_t Network1_StopFlg=0;       //����Ͽ���ʶ����  ���������

uint8_t cID_Key[32] = {0};            //Keyֵ  �����ַ�
uint8_t Encryption_Cnt = 0;          //���ܴ���

/* �궨״̬ѡ��ת��ͨ������ */
uint8_t AD_Channel_ID; //ADת��ͨ��ID

/* Flash����  ϵ��ʹ�ñ��� */
_unionType1 Left_Weight_par,  //����ϵ�� 0ͨ��    17�ֽ�����
										 Right_Wight_par,  //����ϵ�� 1ͨ��
										 Left_Brake_par,    //����ϵ�� 2ͨ��
										 Right_Brake_par,  //����ϵ�� 3ͨ��
										 All_Weight_par,     //��������ϵ��  4ͨ��
										 FootPlat_par,        //̤����ͨ��  5ͨ��
										 Left_Move_par,    //������߶�   6ͨ��
										 Right_Move_par;  //�Ҿ����߶�   7ͨ��
										 
_structParameter SysParameter;    //ϵͳ��������									 
										 
/* ����������ѹ */										 
uint16_t   Zero_LeftWeight,
							 Zero_RightWight,
							 Zero_LeftBrake,
							 Zero_RightBrake,
							 Zero_AllWeight,
							 Zero_FootPlat,
							 Zero_LeftMove,
							 Zero_RightMove;
							 
/* �źŲ����ϴ�Ƶ�ʱ��� */								 
vu8 Signal_TransferTime;							 
							 
/* ��ʱ��2��ʱ���� */		
vu16  TIM2_1ms;  //��ʱ��ms����  ������Ե���ʱʱʹ��
vu32 TIM2_count; //��ʱ��2������ʱ1ms��ʱ   ȫ��ʹ�á�
vu32 TIM2_count_Delay; //��ʱ��2������ʱ����ʹ�á�
vu16 TIM2_count1 = 0;
vu16 TIM2_count2 = 0;
vu16 TIM2_count3 = 0;
vu16 TIM2_count4 = 0;

/* ���������� �ƶ�*/
 uint16_t	u16Left1_TemporaryWeight[300] = {0},
							u16Right1_TemporaryWeight[300] = {0}; //��������10����ƽ����洢λ��  ��ʱ����
 uint16_t  Left_DynamicWeight,   //��̬���� ��̬��ʾʹ��
								Right_DynamicWeight, //�Ҷ�̬���� ��̬��ʾʹ��								
								All_DynamicWeight, 		//��̬����̨���� ��̬��ʾʹ��
								DynamicFootPlat, 				//��̬̤���� ��̬��ʾʹ��
							  Left_DynamicBrake, 		//��̬�ƶ� ��̬��ʾʹ��
							  Right_DynamicBrake; 	//�Ҷ�̬�ƶ� ��̬��ʾʹ��
							
							
 uint8_t  DynamicWeight_Number;//��̬���������� ���6��				

 uint16_t 	 Left_DynamicWeight1[6],   //��̬�������
								 Right_DynamicWeight1[6]; //�Ҷ�̬�������
 uint16_t   Left_StaticWeight,				//���־�̬���� ���ֵ
							  Right_StaticWeight,  		//���־�̬���� ���ֵ
							  Motor_StaticWeight, 		//Ħ�о�̬���� ��̨��
							  AllNone_StaticWeight, //���ظ���̨����
								AllFull_StaticWeight;	  //���ظ���̨����
							
uint16_t u16Protect_Limit;           //������������ֵ								
							
uint16_t u16Left_Data[802],    	//���ƶ���������
							u16Right_Data[802];    //���ƶ���������
uint8_t u8Tran_CurrentZ[2010],				//�����ϴ�����
					  u8Tran_CurrentC[3250],      //�����ƶ�����
						u8Tran_CurrentJ[3250],       //�����ƶ�����
						u8Tran_CurrentN[3250];       //̤��������



/***************----- �˿����ݻ����� -----***************/
uint8_t Rx_Buffer[500];	//�˿ڽ������ݻ����� 
uint8_t Tx_Buffer[500];	//�˿ڷ������ݻ����� 	

uint8_t Parameter_Buffer[100];	//�������ݴ������� 	

uint8_t u8Send_Buffer[BUFFER_SIZE];

/***************----- IP�������� -----***************/
uint8_t W5500_Parameter[27];    //����IP��������
									 
/*************���ڷ��ͱ���**********************/ 
	
 uint8_t  con_TranB[19]  = { 0x11,0x00,'B','V','E','0','1','0','2','0','3','2','0','1','7','0','2','2','4'};  // �汾��ϢVE01020320170224
	
 uint8_t con_TranAACK[3]  = { 0x01, 0x00, 'A' };   			// ��ȷӦ��
 uint8_t con_TranZACK[3]  = { 0x01, 0x00, 'Z' };   			// ����Ӧ��
 uint8_t con_TranXACK[3]  = { 0x01, 0x00, 'X' };   			// ��ǰ�����޷�ִ��

 uint8_t con_TranU[3]  = { 0x01, 0x00, 'U' };   			     // ����
 uint8_t con_TranI[3]  = { 0x01, 0x00, 'I' };   			         // ��ʼ�����
 uint8_t con_TranR[3]  = { 0x01, 0x00, 'R' };   			     // ��λ���
 
 
 uint8_t con_TranMI[8] = { 0x02, 0x00, 'M', 'I'}; 			    // �ϴ�M I  UDP�㲥
 uint8_t con_TranMW[5] = { 0x03, 0x00, 'M', 'W','1'}; 			// �ϴ�M W��̬���ؼ��
 uint8_t con_TranMA[5] = { 0x03, 0x00, 'M', 'W', '1'}; 	// �ϴ�M A��̬���ؼ��
 uint8_t con_TranMF1[5] = { 0x03, 0x00, 'M', 'F', '1'}; // �ϴ�M F 1���ϳ��ؼ��
 uint8_t con_TranMF2[5] = { 0x03, 0x00, 'M', 'F', '2'}; // �ϴ�M F 2���ؾ������
 uint8_t con_TranML[4] = { 0x02, 0x00, 'M', 'L'}; 				// �ϴ�M L Ħ�г����ؼ�� 
 uint8_t con_TranMM[4] = { 0x02, 0x00, 'M', 'M'}; 			// �ϴ�M M Ħ�г��ƶ����
 uint8_t con_TranMD[4] = { 0x02, 0x00, 'M', 'D'}; 				// �ϴ���������������
 uint8_t con_TranMZ[4] = { 0x02, 0x00, 'M', 'Z'}; 				// �ϴ��������Ͳ�������
 uint8_t con_TranMB[4] = { 0x02, 0x00, 'M', 'B'}; 				// �ϴ���������ƶ����
 uint8_t con_TranMJ[4] = { 0x02, 0x00, 'M', 'J'}; 				// �ϴ���������ƶ����
 
 
 uint8_t con_TranT0[4] = { 0x02, 0x00, 'T', '0'};     			//���������� 
 uint8_t con_TranT1[4] = { 0x02, 0x00, 'T', '1'};     			//�������ʹ� ����ʧ��
 uint8_t con_TranT2[4] = { 0x02, 0x00, 'T', '2'};     			//�������ʹ� ����ʧ��
 uint8_t con_TranT3[4] = { 0x02, 0x00, 'T', '3'};     			//���ؾ�����ʱ δ��λ
 uint8_t con_TranT4[4] = { 0x02, 0x00, 'T', '4'};     			//��������λ������ 
	
 uint8_t con_TranT7[4] = { 0x02, 0x00, 'T', '7'};     			//���������쳣
 uint8_t con_TranT8[4] = { 0x02, 0x00, 'T', '8'};    			 	//���������쳣
 uint8_t con_TranT9[4] = { 0x02, 0x00, 'T', '9'};     			//����ֵ̫С
 uint8_t con_TranTD[4] = { 0x02, 0x00, 'T', 'D'};     			//����������
 uint8_t con_TranTF[4] = { 0x02, 0x00, 'T', 'F'};     			//���ؾ������
 
 uint8_t con_TranS[5] = { 0x03, 0x00, 'S', 'S','1'};     	//״̬�ش�
 
 uint8_t con_TranWG[4] = { 0x02, 0x00, 'W', 'G'};       //��������ش�
 uint8_t con_TranWE[6] = { 0x04, 0x00, 'W', 'E',0x00,0x00};       //�����쳣WE00
 
 uint8_t con_TranE1W[5] = { 0x03, 0x00, 'E', '1','W'};  //����1���óɹ�
 uint8_t con_TranE2W[5] = { 0x03, 0x00, 'E', '2','W'};  //����2���óɹ�
 uint8_t con_TranEIW[5] = { 0x03, 0x00, 'E', 'I','W'};    //����IP���óɹ�
 uint8_t con_TranHX0[5] = { 0x03, 0x00, 'H', 'X','0'};    //����ָ�����
 uint8_t con_TranHX1[5] = { 0x03, 0x00, 'H', 'X','1'};    //���ܳ�����
 uint8_t con_TranHEF[5] = { 0x03, 0x00, 'H', 'E','F'};    //����ʧ��
 uint8_t con_TranHET[5] = { 0x03, 0x00, 'H', 'E','T'};    //���ܳɹ�
 uint8_t con_TranF[29] = { 29, 0x00, 'F', 102,102,0};    //Fָ���豸��Ϣ
 uint8_t con_TranO[3] = { 0x01, 0x00, 'O'};              //����ָ��
 
 