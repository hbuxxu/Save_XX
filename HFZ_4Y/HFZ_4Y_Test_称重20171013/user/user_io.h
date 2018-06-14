/****************************************************************************
* �ļ���: USER_IO.h
* �ļ���ʷ:
* �汾��:    V1.0
* ����:      2016.12.09    
* ����:     XX
* ˵��:   ���뿪������
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
#ifndef __USER_IO_H
#define __USER_IO_H

#include "stm32f10x.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define IO_OPEN 0
#define IO_CLOSE 1	 
	 
#define Left_Up                 PGout(3)//   PDout(15)     //��̨�ܾ���  KO1
#define Left_Down               PGout(4)//PGout(2)       //��̨���½�  KO2
#define Right_Up                  PGout(3)       //��̨�ܾ���
#define Right_Down             PGout(4)       //��̨���½�	 
#define Clamp_All                 PGout(5)      //̨�ܼн�
#define Protect_Overflow     PGout(6)      //Һѹ���������ŷ�����  
#define Pump_UpDown         PGout(7)     //�����ƶ��������ٽ����� OPEN:�½�   COLSE:����

#define Pump_Before           PGout(8)     //Ħ�г� ǰ�н�  
#define Pump_Behind           PCout(6)  	  //Ħ�г� ��н�
	 
#define Left_Machine           PCout(8)     //����
#define Right_Machine         PCout(9)     //�ҵ��
#define Machine_Switch       PCout(7)     //�������л�
#define Speed_Control         PAout(8)     //�ƶ�̨���ٰ�����ź�st

#define Hydraumatic_Machine        PEout(2)     //Һѹ�������

#define LED14 PDout(0) 
#define LED15 PCout(11)
#define LED17 PCout(12)
#define LED18 PCout(10)
	 
extern void IO_GpioConfig(void);
#ifdef __cplusplus
}
#endif
#endif /* __USER_IO_H */
