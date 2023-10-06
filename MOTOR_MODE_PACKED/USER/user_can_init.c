#include "main.h"



uint8_t   RxData[8];
CAN_TxHeaderTypeDef			TxHeader;
CAN_RxHeaderTypeDef 		RxHeader;



/**
 * @brief 配置CAN滤波器
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
	
	TxHeader.ExtId=0x0000;        //扩展标识符(29位)
	TxHeader.IDE=CAN_ID_EXT;    //使用标准帧
	TxHeader.RTR=CAN_RTR_DATA;  //数据帧
	TxHeader.DLC=8; 
	TxHeader.TransmitGlobalTime = DISABLE;
}



/**
 * @brief hal库CAN回调函数,接收电机数据
*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN1_RX0_message;
	
	//获取电机数据，&msg中包含接收到的can的ID信息 (RxData 包含电机的状态信息)
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&CAN1_RX0_message, RxData);
	
	//更新电机数据
	get_motor_measure(&CAN1_RX0_message, RxData);
	
	//RM电机进行角度积分
	for(uint16_t m=0;m<8;m++)
		RM_MOTOR_Angle_Integral(&MOTO_REAL_INFO[m]);
}



