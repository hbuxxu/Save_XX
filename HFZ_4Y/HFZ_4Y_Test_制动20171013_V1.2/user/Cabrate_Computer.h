/***********************************************************************************************************
*�ļ�����Cabrate_Computer.h
*�ļ���ʷ����
*�汾�ţ�VER1.0
*���ڣ�2017.6.23
*���ߣ��۴���
*˵������������㷨������λ��ͬ��C�汾
***********************************************************************************************************/

#ifndef _CABRATE_COMPUTER_
#define _CABRATE_COMPUTER_

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f10x.h" 
//=���ݽṹ
#define  MaxCabPoint   7     //������
typedef struct
{
    float Vlt[MaxCabPoint];    //��ѹֵ
    float Val[MaxCabPoint];    //��׼��ֵ
    float Cof[MaxCabPoint];    //�궨ϵ��
    int   CabPointNum;//��Ч�궨����  
    int   NbofVar;//��ϴ���
    int   Chm;//ͨ����
    int   MinValue;//��Сֵ   
    int   MaxValue;//���ֵ
}_ADCab;
extern    uint8_t  CabItemIndex;//�궨ͨ����
extern    _ADCab*  CabBuf[16];

extern  uint8_t  Cabrate_Computer(void);//�궨ϵ������
	
#ifdef __cplusplus
}
#endif

#endif 

