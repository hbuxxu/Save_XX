/****************************************************************************
* �ļ���: SystemErr.c
* �ļ���ʷ:
* �汾��    V1.0
* ����      2017.10.31   
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
#include "SystemErr.h"
#include <string.h>
/*******************************************************************************
�����붨��
*******************************************************************************/


    unSystemErr_TypeDef  un11_Err[Pass_11_Number];        // ����ͨ�������붨��
    unSystemErr_TypeDef  un12_Err[Pass_12_Number];        // ����ͨ�������붨��
    unSystemErr_TypeDef  un13_Err[Pass_13_Number];        // A/D�����붨��
    unSystemErr_TypeDef  un14_Err[Pass_14_Number];        // D/A�����붨��
    unSystemErr_TypeDef  un15_Err[Pass_15_Number];        // �����������붨��
    unSystemErr_TypeDef  un16_Err[Pass_16_Number];        // ���ڹ����붨��
    unSystemErr_TypeDef  un17_Err[Pass_17_Number];        // ����ͨѶ�����붨��
    unSystemErr_TypeDef  un18_Err[Pass_18_Number];        // ���̿��ƹ����붨��
    unSystemErr_TypeDef  un19_Err[Pass_19_Number];        // ���ݴ�������붨��
    unSystemErr_TypeDef  un20_Err[Pass_20_Number];        // ���ݴ洢�����붨��
    unSystemErr_TypeDef  un21_Err[Pass_21_Number];        // ������������붨��

/*******************************************************************************
�������־λ����
*******************************************************************************/
st11_ErrFlag_TypeDef st11_ErrFlag;
st13_ErrFlag_TypeDef st13_ErrFlag;
st17_ErrFlag_TypeDef st17_ErrFlag;
// st11_ErrFlag_TypeDef   st11_ErrFlag[Pass_11_Number];   // ����ͨ�����ϱ�־λ����
// st12_ErrFlag_TypeDef   st12_ErrFlag[Pass_12_Number];   // ����ͨ�����ϱ�־λ����
// st13_ErrFlag_TypeDef   st13_ErrFlag[Pass_13_Number];   // A/Dͨ�����ϱ�־λ����
// st14_ErrFlag_TypeDef   st14_ErrFlag[Pass_14_Number];   // D/Aͨ�����ϱ�־λ����
// st15_ErrFlag_TypeDef   st15_ErrFlag[Pass_15_Number];   // ������ͨ�����ϱ�־λ����
// st16_ErrFlag_TypeDef   st16_ErrFlag[Pass_16_Number];   // ���ڹ��ϱ�־λ����
// st17_ErrFlag_TypeDef   st17_ErrFlag[Pass_17_Number];   // ����ͨѶ���ϱ�־λ����
// st18_ErrFlag_TypeDef   st18_ErrFlag[Pass_18_Number];   // ���̿��ƹ��ϱ�־λ����
// st19_ErrFlag_TypeDef   st19_ErrFlag[Pass_19_Number];   // ���ݴ�����ϱ�־λ����
// st20_ErrFlag_TypeDef   st20_ErrFlag[Pass_20_Number];   // ���ݴ洢���ϱ�־λ����
// st21_ErrFlag_TypeDef   st21_ErrFlag[Pass_21_Number];   // ����������ϱ�־λ����
/*******************************************************************************
�����͹��ϵ�ͨ������
*******************************************************************************/		

//  unsigned char   Pass_11_Number_Set[Pass_11_Number];      //�������ͨ����������
//  unsigned char   Pass_12_Number_Set[Pass_12_Number];      //��������ͨ����������
//  unsigned char   Pass_13_Number_Set[Pass_13_Number];      //A/D����ͨ����������
//  unsigned char   Pass_14_Number_Set[Pass_14_Number];      //D/A����ͨ����������
//  unsigned char   Pass_15_Number_Set[Pass_15_Number];      //����������ͨ����������
//  unsigned char   Pass_16_Number_Set[Pass_16_Number];      //���ڹ���ͨ����������
//  unsigned char   Pass_17_Number_Set[Pass_17_Number];      //����ͨѶ������������
//  unsigned char   Pass_18_Number_Set[Pass_18_Number];      //���̿��ƹ�����������
//  unsigned char   Pass_19_Number_Set[Pass_19_Number];      //���ݴ��������������
//  unsigned char   Pass_20_Number_Set[Pass_20_Number];      //���ݴ洢������������
//  unsigned char   Pass_21_Number_Set[Pass_21_Number];      //�������������������



/*******************************************************************************
* ������  : SystemPass_Init
* ����    : ͨ���ų�ʼ�����������ͨ����ӦӲ���ӿڵĳ�ʼ����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
	/***********************������Ҫ���******************************/
void SystemPass_Init(void)
{ 
//    Pass_11_Number_Set[0]=1;      //�������ͨ��������������
// 	 Pass_11_Number_Set[1]=2;      //�������ͨ��������������
// 	 Pass_11_Number_Set[2]=3;      //�������ͨ��������������
//    Pass_12_Number_Set[3]=4;      //��������ͨ��������������
//    Pass_12_Number_Set[4]=5;      //��������ͨ��������������
//    Pass_13_Number_Set[5]=6;      //A/D����ͨ��������������
//    Pass_13_Number_Set[6]=7;      //A/D����ͨ��������������
//    Pass_13_Number_Set[7]=8;      //A/D����ͨ��������������
//    Pass_13_Number_Set[8]=9;      //A/D����ͨ��������������
}
/*******************************************************************************
* ������  : SystemErr_Init
* ����    : �������ʼ��
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void SystemErr_Init(void)
{ 
    u8 uci;
    /****************************����ͨ�����ϴ����ʼ��***************************/	
		#ifdef USE_11
		un11_Err[0].stSystemErr.ucPass=IN_Weight_ID;
		st11_ErrFlag = es11_Err_N;
    for(uci=0;uci<Pass_11_Number;uci++)
    {
        un11_Err[uci].stSystemErr.ucErr_Code=11;
				un11_Err[uci].stSystemErr.ucErr_State=st11_ErrFlag;
    }	
		#endif
    /*******************************����ͨ�����ϴ����ʼ��***************************/		
	  #ifdef USE_12
		#endif
    /***************************A/D���ϴ����ʼ��****************************/	
		#ifdef USE_13
		un13_Err[0].stSystemErr.ucPass=Left_Weight_ID;
		un13_Err[1].stSystemErr.ucPass=Right_Wight_ID;
		st13_ErrFlag = es13_Err_N;       //�޹���
		for(uci=0;uci<Pass_13_Number;uci++)
    {
        un13_Err[uci].stSystemErr.ucErr_Code=13;
				un13_Err[uci].stSystemErr.ucErr_State=st13_ErrFlag;
    }	  
		#endif
    /****************************D/A���ϴ����ʼ��****************************/	
		#ifdef USE_14
		#endif
    /***************************���������ϴ����ʼ��***************************/	
		#ifdef USE_15
		#endif
    /***************************���ڹ��ϴ����ʼ��****************************/	
		#ifdef USE_16
		#endif    
    /***************************����ͨѶ���ϴ����ʼ��****************************/			
		#ifdef USE_17
		un17_Err[0].stSystemErr.ucPass=ERR_Net_ID;
		st17_ErrFlag = es17_Err_N;       //�޹���
		for(uci=0;uci<Pass_17_Number;uci++)
    {
        un17_Err[uci].stSystemErr.ucErr_Code=17;
				un17_Err[uci].stSystemErr.ucErr_State=st17_ErrFlag;
    }	 
		#endif          
    /***********************���̿��ƹ��ϴ����ʼ��****************************/		
		#ifdef USE_18
		#endif
    /***************************���ݴ�����ϴ����ʼ��****************************/	
		#ifdef USE_19
		#endif    		
    /***************************���ݴ洢���ϴ����ʼ��****************************/	
 		#ifdef USE_20
		#endif   
    /***************************�������ϴ����ʼ��****************************/	
    
}

/*******************************************************************************
* ������  : SystemErr_W5500Send
* ����    : �����뷢�ͺ��� �ϴ���λ��
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void SystemErr_W5500Send(void)
{
		uint8_t u8Err_Num=0;
		uint8_t uci;
	  Tx_Buffer[2] = 'V';  //ָ����
// 		Tx_Buffer[3] = '';    //������
  /****************************����ͨ�����ϴ����ʼ��***************************/	
		#ifdef USE_11
    for(uci=0;uci<Pass_11_Number;uci++)
    {
				if(un11_Err[uci].stSystemErr.ucErr_State != es11_Err_N)
				{							
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un11_Err[uci].stSystemErr.ucErr_Code;  //���ϴ���
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un11_Err[uci].stSystemErr.ucPass;  //����ͨ����
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un11_Err[uci].stSystemErr.ucErr_State;  //��������
							u8Err_Num ++;
				}				
    }	
		#endif
		/***************************A/D���ϴ����ʼ��****************************/	
		#ifdef USE_13
    for(uci=0;uci<Pass_13_Number;uci++)
    {
				if(un13_Err[uci].stSystemErr.ucErr_State != es13_Err_N)
				{							
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un13_Err[uci].stSystemErr.ucErr_Code;  //���ϴ���
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un13_Err[uci].stSystemErr.ucPass;  //����ͨ����
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un13_Err[uci].stSystemErr.ucErr_State;  //��������
							u8Err_Num ++;
				}				
    }	
		#endif
		/***************************����ͨѶ���ϴ����ʼ��****************************/			
		#ifdef USE_17
		for(uci=0;uci<Pass_17_Number;uci++)
    {
				if(un17_Err[uci].stSystemErr.ucErr_State != es17_Err_N)
				{							
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un18_Err[uci].stSystemErr.ucErr_Code;  //���ϴ���
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un18_Err[uci].stSystemErr.ucPass;  //����ͨ����
							Tx_Buffer[uci+(u8Err_Num*3+4)] = un18_Err[uci].stSystemErr.ucErr_State;  //��������
							u8Err_Num ++;
				}				
    }	
		#endif      
		Tx_Buffer[3] = u8Err_Num;    //������
		Tx_Buffer[0] = u8Err_Num*3+2;//֡����
		Tx_Buffer[1] = u8Err_Num*3+2;//֡����
		SPI_FLASH_SectorErase_A(ParameterERR_Flash_Address);		//��������
		SPI_FLASH_SectorWrite(Tx_Buffer,ParameterERR_Flash_Address,(u8Err_Num*3+4));//����ϵ��
		if((USER_SystemFlag.A.Flag_Online == 1)&&(USER_SystemFlag.A.Flag_Sucess == 1))  //���� �����ط�
		{
					W5500_Send_Data(2,Tx_Buffer);				//���͵�ǰ������
		}	
}
/*******************************************************************************
* ������  : SystemErr_Test
* ����    : �Լ�
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void SystemErr_Test(void)
{
		if(IO_GetInStatus(IN_Weight_ID) == 1)  //�ڵ����
		{
				un11_Err[0].stSystemErr.ucErr_State = es11_Err_1;
		}
		else
		{
				un11_Err[0].stSystemErr.ucErr_State = es11_Err_N;
		}
		if(Zero_LeftWeight > Zero_Max)//��������ѹ����Χ
		{
				un13_Err[0].stSystemErr.ucErr_State=es13_Err_0;
		}
		if(Zero_RightWight > Zero_Max)//��������ѹ����Χ
		{
				un13_Err[1].stSystemErr.ucErr_State=es13_Err_0;
		}
		SystemErr_W5500Send();
}
