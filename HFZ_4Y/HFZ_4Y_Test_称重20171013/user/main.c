#include "stm32f10x.h"

int main(void)
{			
		__enable_irq();  //Ϊ��������������
		IO_GpioConfig();  //IO���� ��ʼ��
		IWDG_Configuration();  //���Ź�����
		Usart_Init();
		ADC_7663_Init();
		SPI_FLASH_Init();		//W25q64�洢оƬ����
		Get_Parameter();     //��������������֮ǰ
		W5500_Init_Config();//������ADC_7663_Init()֮��	 
// 		SysParameter.DynamoStart_Flag = 0;		
		Left_Machine = IO_CLOSE;
		Right_Machine = IO_CLOSE;
		Machine_Switch = IO_CLOSE;
		Left_Up = IO_CLOSE;  			//��̨�ܾٹر�
		Left_Down = IO_CLOSE;   //��̨�ܽ��ر�
		Hydraumatic_Machine = IO_CLOSE;
	  //ϵͳ��־λ��0
		USER_SystemFlag.Init = 0;
		USER_LEDFlag.Init = 0;
		//ָʾ�Ʊ�־λ
		USER_LEDFlag.A.LED_Online = 0;  		  //����ָʾ��
		USER_LEDFlag.A.LED_Test = 0;  			  //���ָʾ��
		USER_LEDFlag.A.LED_Weight = 1;  			//����ָʾ��
		USER_LEDFlag.A.LED_Brake = 0; 			  //�ƶ�ָʾ��
	  WorkState_Flg = Default_State_Flg;//��ʼ��  ��������״̬
		InitADScreen();
		TIM4_NVIC_Config();
		SystemErr_Init();  //���ϴ����ʼ��
		SystemErr_Test();
		TIM2_count_Delay=0;
    USER_SystemFlag.A.Flag_Sucess1 = 1;
    IWDG_Configuration();  //���Ź�����
		while(TIM2_count_Delay < 12000)
		{
// 			 W5500_Receive_Deal();
       IWDG_ReloadCounter();   //���Ź�����
		}
		Fun_Zero(Left_Weight_ID,1);//����
		Fun_Zero(Right_Wight_ID,1);//����
// 	  Fun_Zero(Left_Brake_ID,1);//����
// 		Fun_Zero(Right_Brake_ID,1);//����	
// 		Fun_Zero(All_Weight_ID,1);//����
    while(1)
    {								
					W5500_Receive_Deal();
					switch(System_State)
					{
							/* �궨 */
							case Adjust_State:
																												Fun_Adjust();
																												break;
							/* �źŲ��� */
							case SignalTest_State:
																												Fun_SignalTest();
																												break;
							/* ��̬���� */
							case StaticWeight_State:   
																												Fun_StaticWeightTest();							
																												break;
							/* ��̬���� */
							case DynamicWeight_State:   
																												Fun_DynamicWeightTest();							
																												break;
							/* Ħ�г����� */
							case StaticWeight_Motor_State:
																												Fun_MotorWeightTest();							
																												break;
							/* ��ȡ����״̬ */
							case ReadData_State:
																												Fun_ReadData();
																												break;
							/* Ĭ�ϴ���״̬ */
							case Default_State:
																												Fun_Stable();
																												break;
							default :
													 System_State = Default_State;
													 break;
					}					
    }
}
