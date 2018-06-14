/****************************************************************************
 * �ļ���: SystemErr.h
 * �ļ���ʷ:
 * �汾��    V1.0
 * ����      2017.10.31
 * ����      qgk 
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
#ifndef	_qSystemErr_H_
#define	_qSystemErr_H_
#include "stm32f10x.h"
/**********************************************************
��������ѡ����ע�͵����õ����͡�
***********************************************************/
#define USE_11      //�������ѡ��
// #define USE_12      //��������ѡ��
#define USE_13      //A/D����ѡ��
// #define USE_14      //D/A����ѡ��
// #define USE_15      //����������ѡ��
// #define USE_16      //���ڹ���
#define USE_17      //����ͨѶ����
#define USE_18      //���̿��ƴ���
// #define USE_19      //���ݴ������
// #define USE_20      //���ݴ洢����
// #define USE_21      //��������
// #define USE_22      //����Ԥ��1
// #define USE_23      //����Ԥ��2
/**********************************************************
����ͨ���Ÿ������ã�ֻ���޸����ù���ͨ���Ÿ��������е�
ͨ���Ÿ�����������Ϊ0��
***********************************************************/
#define Pass_11_Number 4      //�������ͨ������������     0�ƶ���λ  1������λ  2��ͣ��  3��ͣ��
#define Pass_12_Number 1      //��������ͨ������������     
#define Pass_13_Number 3      //A/D����ͨ������������       0���ƶ�    1���ƶ�    2���ϳ���
#define Pass_14_Number 1      //D/A����ͨ������������
#define Pass_15_Number 1      //����������ͨ������������
#define Pass_16_Number 1      //���ڹ���ͨ������������
#define Pass_17_Number 1      //����ͨѶ����ͨ������������
#define Pass_18_Number 1      //���̿��ƹ���ͨ������������
#define Pass_19_Number 1      //���ݴ������ͨ������������
#define Pass_20_Number 1      //���ݴ洢����ͨ������������
#define Pass_21_Number 1      //�����������ͨ������������
/**********************************************************
�����ⲿ����
***********************************************************/
// extern unsigned char   Pass_11_Number_Set[Pass_11_Number];      //�������ͨ����������
// extern unsigned char   Pass_12_Number_Set[Pass_12_Number];      //��������ͨ����������
// extern unsigned char   Pass_13_Number_Set[Pass_13_Number];      //A/D����ͨ����������
// extern unsigned char   Pass_14_Number_Set[Pass_14_Number];      //D/A����ͨ����������
// extern unsigned char   Pass_15_Number_Set[Pass_15_Number];      //����������ͨ����������
// extern unsigned char   Pass_16_Number_Set[Pass_16_Number];      //���ڹ���ͨ����������
// extern unsigned char   Pass_17_Number_Set[Pass_17_Number];      //����ͨѶ������������
// extern unsigned char   Pass_18_Number_Set[Pass_18_Number];      //���̿��ƹ�����������
// extern unsigned char   Pass_19_Number_Set[Pass_19_Number];      //���ݴ��������������
// extern unsigned char   Pass_20_Number_Set[Pass_20_Number];      //���ݴ洢������������
// extern unsigned char   Pass_21_Number_Set[Pass_21_Number];      //�������������������


/**********************************************************
���������Ͷ���
***********************************************************/

typedef union  
{
    unsigned char 	ucSystemErr[3];
    struct 
    {
        unsigned char ucErr_Code;     //���ϴ���
        unsigned char ucPass;         //����ͨ��
        unsigned char ucErr_State;    //����״̬
    }
    stSystemErr;
}
unSystemErr_TypeDef;	


/**********************************************************
���ϱ�־λ����
***********************************************************/
/********************������ϱ�־λ����********************/
typedef enum 
{
      es11_Err_0,      //����״̬����
      es11_Err_1,      //����״̬��ͨ
			es11_Err_N       //�޹���
}
st11_ErrFlag_TypeDef;
extern st11_ErrFlag_TypeDef st11_ErrFlag;

/********************�������ϱ�־λ����********************/		
typedef struct 
{
    ErrorStatus  es12_Err_0;      //����״̬����
    ErrorStatus  es12_Err_1;      //����״̬��ͨ
		
}
st12_ErrFlag_TypeDef;
/********************A/D���ϱ�־λ����********************/		
typedef enum 
{
      es13_Err_0,      //����״̬����Ư
      es13_Err_1,      //����״̬��������
      es13_Err_2,      //����״̬����С���
      es13_Err_3,      //����״̬���ź�����
      es13_Err_4,      //����״̬���궨ϵ��
			es13_Err_N       //�޹���
}
st13_ErrFlag_TypeDef;
extern st13_ErrFlag_TypeDef st13_ErrFlag;
/********************D/A���ϱ�־λ����********************/		
typedef struct 
{
    ErrorStatus  es14_Err_0;      //����״̬����Ư
    ErrorStatus  es14_Err_1;      //����״̬��������
    ErrorStatus  es14_Err_2;      //����״̬����С���
    ErrorStatus  es14_Err_3;      //����״̬���ź�����
    ErrorStatus  es14_Err_4;      //����״̬���궨ϵ��
}
st14_ErrFlag_TypeDef;
/******************���������ϱ�־λ���Ͷ���********************/		
typedef struct 
{
    ErrorStatus  es15_Err_0;      //������
    ErrorStatus  es15_Err_1;      //������
}
st15_ErrFlag_TypeDef;

/******************���ڹ��ϱ�־λ���Ͷ���********************/		
typedef struct 
{
    ErrorStatus  es16_Err_0;      //����ʧ��
    ErrorStatus  es16_Err_1;      //�������
}
st16_ErrFlag_TypeDef;
/******************����ͨѶ���ϱ�־λ���Ͷ���********************/		
#define ERR_Net_ID 0
typedef enum 
{
    es17_Err_0,     //����ʧ��
    es17_Err_1,      //�������
		es17_Err_N       //�޹���
}
st17_ErrFlag_TypeDef;
extern st17_ErrFlag_TypeDef st17_ErrFlag;

/******************���̿��ƹ��ϱ�־λ���Ͷ���********************/		
#define ERR_Test_ID 0
typedef enum 
{
    es18_Err_0,      //�����ж�
		es18_Err_1,      //������Ͳ�쳣  �����������쳣��������10��
		es18_Err_N,      //�޹���
}
st18_ErrFlag_TypeDef;
extern st18_ErrFlag_TypeDef st18_ErrFlag;

/******************���ݴ�����ϱ�־λ���Ͷ���********************/		
typedef struct 
{
    ErrorStatus  es19_Err_0;      //��������
    ErrorStatus  es19_Err_1;      //����Խ��
}
st19_ErrFlag_TypeDef;

/******************���ݴ洢���ϱ�־λ���Ͷ���********************/		
typedef struct 
{
    ErrorStatus  es20_Err_0;      //��������
    ErrorStatus  es20_Err_1;      //����Խ��
    ErrorStatus  es20_Err_2;      //����ʧ��
    ErrorStatus  es20_Err_3;      //�ռ����
}
st20_ErrFlag_TypeDef;

/******************�������ϱ�־λ���Ͷ���********************/		
typedef struct 
{
    ErrorStatus  es21_Err_0;      //��������
}
st21_ErrFlag_TypeDef;


extern    unSystemErr_TypeDef  un11_Err[Pass_11_Number];        // ����ͨ�������붨��
extern    unSystemErr_TypeDef  un12_Err[Pass_12_Number];        // ����ͨ�������붨��
extern    unSystemErr_TypeDef  un13_Err[Pass_13_Number];        // A/D�����붨��
extern    unSystemErr_TypeDef  un14_Err[Pass_14_Number];        // D/A�����붨��
extern    unSystemErr_TypeDef  un15_Err[Pass_15_Number];        // �����������붨��
extern    unSystemErr_TypeDef  un16_Err[Pass_16_Number];        // ���ڹ����붨��
extern    unSystemErr_TypeDef  un17_Err[Pass_17_Number];        // ����ͨѶ�����붨��
extern    unSystemErr_TypeDef  un18_Err[Pass_18_Number];        // ���̿��ƹ����붨��
extern    unSystemErr_TypeDef  un19_Err[Pass_19_Number];        // ���ݴ�������붨��
extern    unSystemErr_TypeDef  un20_Err[Pass_20_Number];        // ���ݴ洢�����붨��
extern    unSystemErr_TypeDef  un21_Err[Pass_21_Number];        // ������������붨��

// /*******************************************************************************
// �������־λ����
// *******************************************************************************/

// extern st11_ErrFlag_TypeDef   st11_ErrFlag[Pass_11_Number];   // ����ͨ�����ϱ�־λ����
// extern st12_ErrFlag_TypeDef   st12_ErrFlag[Pass_12_Number];   // ����ͨ�����ϱ�־λ����
// extern st13_ErrFlag_TypeDef   st13_ErrFlag[Pass_13_Number];   // A/Dͨ�����ϱ�־λ����
// extern st14_ErrFlag_TypeDef   st14_ErrFlag[Pass_14_Number];   // D/Aͨ�����ϱ�־λ����
// extern st15_ErrFlag_TypeDef   st15_ErrFlag[Pass_15_Number];   // ������ͨ�����ϱ�־λ����
// extern st16_ErrFlag_TypeDef   st16_ErrFlag[Pass_16_Number];   // ���ڹ��ϱ�־λ����
// extern st17_ErrFlag_TypeDef   st17_ErrFlag[Pass_17_Number];   // ����ͨѶ���ϱ�־λ����
// extern st18_ErrFlag_TypeDef   st18_ErrFlag[Pass_18_Number];   // ���̿��ƹ��ϱ�־λ����
// extern st19_ErrFlag_TypeDef   st19_ErrFlag[Pass_19_Number];   // ���ݴ�����ϱ�־λ����
// extern st20_ErrFlag_TypeDef   st20_ErrFlag[Pass_20_Number];   // ���ݴ洢���ϱ�־λ����
// extern st21_ErrFlag_TypeDef   st21_ErrFlag[Pass_21_Number];   // ����������ϱ�־λ����

// extern    void SystemErrTest(unsigned char *ucErrCode,unsigned short  *size);
extern    void SystemErr_Init(void);
extern    void SystemPass_Init(void);
extern void SystemErr_W5500Send(void);
extern void SystemErr_Test(void);
#endif
