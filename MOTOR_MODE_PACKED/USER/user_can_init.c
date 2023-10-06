#include "main.h"



uint8_t   RxData[8];
CAN_TxHeaderTypeDef			TxHeader;
CAN_RxHeaderTypeDef 		RxHeader;



/**
 * @brief ����CAN�˲���
*/
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



/**
 * @brief hal��CAN�ص�����,���յ������
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN1_RX0_message;
	
	//��ȡ������ݣ�&msg�а������յ���can��ID��Ϣ (RxData ���������״̬��Ϣ)
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&CAN1_RX0_message, RxData);
	
	//���µ������
	get_motor_measure(&CAN1_RX0_message, RxData);
	
	//RM������нǶȻ���
	for(uint16_t m=0;m<8;m++)
		RM_MOTOR_Angle_Integral(&MOTO_REAL_INFO[m]);
}



