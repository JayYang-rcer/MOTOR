#include "user_can_init.h"
#include "can.h"
#include "stm32f4xx.h"
#include "gpio.h"
#include "stm32f4xx_hal_can.h"


uint8_t   RxData[8];
CAN_TxHeaderTypeDef			TxHeader;
CAN_RxHeaderTypeDef 		RxHeader;


M3508_REAL_INFO      M3508_CHASSIS_MOTOR_REAL_INFO[4];


void User_Open_Can(void)
{
	CAN_FilterTypeDef Filter;
	
	Filter.FilterActivation = ENABLE;
    Filter.FilterMode = CAN_FILTERMODE_IDMASK;
    Filter.FilterScale = CAN_FILTERSCALE_32BIT;
    Filter.FilterIdHigh = 0x0000;
    Filter.FilterIdLow = 0x0000;
    Filter.FilterMaskIdHigh = 0x0000;
    Filter.FilterMaskIdLow = 0x0000;
    Filter.FilterBank = 0;
    Filter.FilterFIFOAssignment = CAN_RX_FIFO0;
	
	if (HAL_CAN_ConfigFilter(&hcan1, &Filter) != HAL_OK)
	{
			/* Filter configuration Error */
			Error_Handler();
	}
		
	if (HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		/* Start Error */
		Error_Handler();
	}
		
		/*##-4- Activate CAN RX notification #######################################*/
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
	{
		/* Start Error */
		Error_Handler();
	}
	
	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		/* Start Error */
		Error_Handler();
	}
	
	TxHeader.ExtId=0x0000;        //��չ��ʶ��(29λ)
	TxHeader.IDE=CAN_ID_EXT;    //ʹ�ñ�׼֡
	TxHeader.RTR=CAN_RTR_DATA;  //����֡
	TxHeader.DLC=8; 
	TxHeader.TransmitGlobalTime = DISABLE;
}



//hal��CAN�ص�����,���յ������
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN1_RX0_message;
	
	//��ȡ������ݣ�&msg�а������յ���can��ID��Ϣ (RxData ���������״̬��Ϣ)
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&CAN1_RX0_message, RxData);
	
	//���µ������
	get_motor_measure(&CAN1_RX0_message, RxData);
	
}


//������ݶ�ȡ
void get_motor_measure(CAN_RxHeaderTypeDef *msg, uint8_t Data[8])                                    
{                                                                                         
    switch(msg -> StdId)  // ����׼ID
	{
		case CHASSIS_M3508_M1_ID:
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[0].ANGLE   = (uint16_t)((Data[0] << 8) |  Data[1]);  // ת�ӻ�е�Ƕ�
			M3508_CHASSIS_MOTOR_REAL_INFO[0].RPM     = (uint16_t)((Data[2] << 8) |  Data[3]);  // ʵ��ת��ת��
			M3508_CHASSIS_MOTOR_REAL_INFO[0].CURRENT = (uint16_t)((Data[4] << 8) |  Data[5]);  // ʵ��ת�ص���
		}; break;
		
		case CHASSIS_M3508_M2_ID:
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[1].ANGLE   = (uint16_t)((Data[0] << 8) |  Data[1]);  // ת�ӻ�е�Ƕ�
			M3508_CHASSIS_MOTOR_REAL_INFO[1].RPM     = (uint16_t)((Data[2] << 8) |  Data[3]);  // ʵ��ת��ת��
			M3508_CHASSIS_MOTOR_REAL_INFO[1].CURRENT = (uint16_t)((Data[4] << 8) |  Data[5]);  // ʵ��ת�ص���
		}; break;
		
		case CHASSIS_M3508_M3_ID:
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[2].ANGLE   = (uint16_t)((Data[0] << 8) |  Data[1]);  // ת�ӻ�е�Ƕ�
			M3508_CHASSIS_MOTOR_REAL_INFO[2].RPM     = (uint16_t)((Data[2] << 8) |  Data[3]);  // ʵ��ת��ת��
			M3508_CHASSIS_MOTOR_REAL_INFO[2].CURRENT = (uint16_t)((Data[4] << 8) |  Data[5]);  // ʵ��ת�ص���
		}; break;
		
		case CHASSIS_M3508_M4_ID:
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[3].ANGLE   = (uint16_t)((Data[0] << 8) |  Data[1]);  // ת�ӻ�е�Ƕ�
			M3508_CHASSIS_MOTOR_REAL_INFO[3].RPM     = (uint16_t)((Data[2] << 8) |  Data[3]);  // ʵ��ת��ת��
			M3508_CHASSIS_MOTOR_REAL_INFO[3].CURRENT = (uint16_t)((Data[4] << 8) |  Data[5]);  // ʵ��ת�ص���
		}; break;
		
		default: break;
	}            
}


void M3508_Send_Currents(void)
{
	uint8_t TxData[8];
	uint32_t Send_Mail_Box;
	
	//���ÿ��ƶ�
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 0x08;
	
	//�����ٲöκ����ݶ�
	TxHeader.StdId = CAN_CHASSIS_ALL_ID;
	
	TxData[0] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[0].TARGET_CURRENT >> 8);
	TxData[1] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[0].TARGET_CURRENT;
	TxData[2] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[1].TARGET_CURRENT >> 8);
	TxData[3] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[1].TARGET_CURRENT;
	TxData[4] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[2].TARGET_CURRENT >> 8);
	TxData[5] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[2].TARGET_CURRENT;
	TxData[6] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[3].TARGET_CURRENT >> 8);
	TxData[7] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[3].TARGET_CURRENT;
	
	HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &Send_Mail_Box);
}


void M3508_Send_Currents_Reset(void)
{
	uint8_t TxData[8];
	uint32_t Send_Mail_Box;
	TxHeader.StdId = 0x700;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 0x08;
	
	TxData[0] = 0 >> 8;
	TxData[1] = 0;
	TxData[2] = 0 >> 8;
	TxData[3] = 0;
	TxData[4] = 0 >> 8;
	TxData[5] = 0;
	TxData[6] = 0 >> 8;
	TxData[7] = 0;
	
	HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &Send_Mail_Box);
}



