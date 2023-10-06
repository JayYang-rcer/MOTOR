#include "main.h"


MOTOR_REAL_INFO     MOTO_REAL_INFO[8];
PID_Data MOTOR_PID_RPM[8];
PID_Data MOTOR_PID_POS[8];	//位置pid信息



/**
 * @brief  电机数据读取
 * @param  can数据接收结构体
 * @param  can数据接收数组
 * @return none
*/
void get_motor_measure(CAN_RxHeaderTypeDef *msg, uint8_t Data[8])                                    
{                                                                                         
    switch(msg -> StdId)  // 检测标准ID
	{
		case CHASSIS_M3508_M1_ID:
		{ 
			MOTO_REAL_INFO[0].ANGLE   = (uint16_t)((Data[0] << 8) |  Data[1]);  // 转子机械角度
			MOTO_REAL_INFO[0].RPM     = (uint16_t)((Data[2] << 8) |  Data[3]);  // 实际转子转速
			MOTO_REAL_INFO[0].CURRENT = (uint16_t)((Data[4] << 8) |  Data[5]);  // 实际转矩电流
		}; break;
		
		case CHASSIS_M3508_M2_ID:
		{ 
			MOTO_REAL_INFO[1].ANGLE   = (uint16_t)((Data[0] << 8) |  Data[1]);  // 转子机械角度
			MOTO_REAL_INFO[1].RPM     = (uint16_t)((Data[2] << 8) |  Data[3]);  // 实际转子转速
			MOTO_REAL_INFO[1].CURRENT = (uint16_t)((Data[4] << 8) |  Data[5]);  // 实际转矩电流
		}; break;
		
		case CHASSIS_M3508_M3_ID:
		{ 
			MOTO_REAL_INFO[2].ANGLE   = (uint16_t)((Data[0] << 8) |  Data[1]);  // 转子机械角度
			MOTO_REAL_INFO[2].RPM     = (uint16_t)((Data[2] << 8) |  Data[3]);  // 实际转子转速 
			
			MOTO_REAL_INFO[2].CURRENT = (uint16_t)((Data[4] << 8) |  Data[5]);  // 实际转矩电流
		}; break;
		
		case CHASSIS_M3508_M4_ID:
		{ 
			MOTO_REAL_INFO[3].ANGLE   = (uint16_t)((Data[0] << 8) |  Data[1]);  // 转子机械角度
			MOTO_REAL_INFO[3].RPM     = (uint16_t)((Data[2] << 8) |  Data[3]);  // 实际转子转速
			MOTO_REAL_INFO[3].CURRENT = (uint16_t)((Data[4] << 8) |  Data[5]);  // 实际转矩电流
		}; break;
		
		default: break;
	}            
}



/**
 * @brief 发送电机数据
 * @param NULL
 * @return NULL
*/
void M3508_Send_Currents(void)
{
	uint8_t TxData[8];
	uint32_t Send_Mail_Box;
	
	//配置控制端
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 0x08;
	
	//配置仲裁段和数据段
	TxHeader.StdId = CAN_CHASSIS_ALL_ID;//0x200
	
	TxData[0] = (uint8_t)(MOTO_REAL_INFO[0].TARGET_CURRENT >> 8);//0x201
	TxData[1] = (uint8_t) MOTO_REAL_INFO[0].TARGET_CURRENT;

	TxData[2] = (uint8_t)(MOTO_REAL_INFO[1].TARGET_CURRENT >> 8);//0x202
	TxData[3] = (uint8_t) MOTO_REAL_INFO[1].TARGET_CURRENT;

	TxData[4] = (uint8_t)(MOTO_REAL_INFO[2].TARGET_CURRENT >> 8);//0x203
	TxData[5] = (uint8_t) MOTO_REAL_INFO[2].TARGET_CURRENT;

	TxData[6] = (uint8_t)(MOTO_REAL_INFO[3].TARGET_CURRENT >> 8);//0x204
	TxData[7] = (uint8_t) MOTO_REAL_INFO[3].TARGET_CURRENT;
	
	HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &Send_Mail_Box);
}



/**
 * @brief 电机控制模式
 * @param NULL
 * @return NULL
*/
void Motor_Control(void)
{
	for(int i=0; i < 8; i++)
	{
		//判断是否有选择电机类型，若无直接退出
		if (MOTO_REAL_INFO[i].Motor_Type == NONE)	
			break;
		
		//电机模式选择
		switch (MOTO_REAL_INFO[i].Motor_Mode)
		{
			case SPEED_CONTROL_MODE: //速度模式
			{
				PID_Incremental_PID_Calculation(&MOTOR_PID_RPM[i],MOTO_REAL_INFO[i].TARGET_RPM,MOTO_REAL_INFO[i].RPM);//速度环
				break;
			}

			case VELOCITY_PLANNING_MODE: //梯形模式
			{
				Velocity_Planning_MODE(&MOTO_REAL_INFO[i]);					
				PID_Incremental_PID_Calculation(&MOTOR_PID_RPM[i],MOTO_REAL_INFO[i].TARGET_RPM,MOTO_REAL_INFO[i].RPM);
				break;
			}

			case CURRENT_MODE: //电流模式(直接赋电流值)
			{
				break;
			}

			case POSITION_CONTROL_MODE://位置模式
			{
				PID_Position_Calculate(&MOTOR_PID_POS[i],MOTO_REAL_INFO[i].TARGET_POS, MOTO_REAL_INFO[i].REAL_ANGLE);//位置环
				PID_Incremental_PID_Calculation(&MOTOR_PID_RPM[i], MOTOR_PID_POS[i].Output, MOTO_REAL_INFO[i].RPM);//速度环
				break;
			}

			case SPEED_TARQUE_CONTROL_MODE://速度转矩模式
			{
				PID_Incremental_PID_Calculation(&MOTOR_PID_RPM[i],MOTO_REAL_INFO[i].Velocity_Tarque.Target_Vel,MOTO_REAL_INFO[i].RPM);	//速度环
				MOTOR_PID_RPM[i].Output = Max_Value_Limit(MOTOR_PID_RPM[i].Output,MOTO_REAL_INFO[i].Velocity_Tarque.TARGET_TORQUE);	//限制转矩模式时电流值
				
				//判断是否夹取
				//flag = 1夹取成功
				if(fabsf(MOTO_REAL_INFO[i].RPM) <=10)
				{
					MOTO_REAL_INFO[i].Velocity_Tarque.Cnt++;
				}
				else
				{
					MOTO_REAL_INFO[i].Velocity_Tarque.Cnt = 0;
				}

				if(MOTO_REAL_INFO[i].Velocity_Tarque.Cnt >= 10)
				{
					MOTO_REAL_INFO[i].Velocity_Tarque.Cnt=0;
					MOTO_REAL_INFO[i].Velocity_Tarque.Flag = 1;
					MOTO_REAL_INFO[i].Motor_Mode= SPEED_CONTROL_MODE;
					MOTO_REAL_INFO[i].TARGET_RPM = 0;
				}
				break;
			}

			case MOTO_OFF://电机关闭
			{
				MOTO_REAL_INFO[i].TARGET_CURRENT = 0.0f;//电流赋值
				break;
			}

			case HOMEING_MODE://说实在，和速度转矩没什么差别。直接用速度转矩即可
			{
				Homeing_Mode(&MOTO_REAL_INFO[i]);	//调用校准模式
				PID_Incremental_PID_Calculation(&MOTOR_PID_RPM[i], MOTO_REAL_INFO[i].TARGET_RPM, MOTO_REAL_INFO[i].RPM); 	//速度环
				MOTOR_PID_RPM[i].Output = Max_Value_Limit(MOTOR_PID_RPM[i].Output,MOTO_REAL_INFO[i].HomingMode.TARGET_TORQUE);	//限制校准模式电流
			}

			case POSITION_TORQUE_MODE://位置转矩模式
			{
				PID_Position_Calculate(&MOTOR_PID_POS[i],MOTO_REAL_INFO[i].Position_Tarque.Pos, MOTO_REAL_INFO[i].REAL_ANGLE);//位置环
				PID_Incremental_PID_Calculation(&MOTOR_PID_RPM[i], MOTOR_PID_POS[i].Output, MOTO_REAL_INFO[i].RPM);//速度环
				MOTOR_PID_RPM[i].Output = Max_Value_Limit(MOTOR_PID_RPM[i].Output,MOTO_REAL_INFO[i].Position_Tarque.TARGET_TORQUE);//限制转矩模式时电流值

				//判断是否到达目标位置
				if(fabsf(MOTO_REAL_INFO[i].RPM) <=10)
				{		
					MOTO_REAL_INFO[i].Position_Tarque.Cnt++;
				}
				else
				{
					MOTO_REAL_INFO[i].Position_Tarque.Cnt = 0;
				}
		
				if(MOTO_REAL_INFO[i].Position_Tarque.Cnt>=50)//50ms
				{
					MOTO_REAL_INFO[i].Position_Tarque.Cnt = 0;
					MOTO_REAL_INFO[i].Position_Tarque.Flag = 1;
				}
				break;
			}

			default: break;
		}
	}

	//电机转动参数
	for(int i = 0; i < 8; i++)
	{
		if(MOTO_REAL_INFO[i].Motor_Mode == CURRENT_MODE)//防止选择该模式却无法判断
		{
			
		}//电流模式下的特殊情况
		else
		{
			if (MOTO_REAL_INFO[i].Motor_Type == M_3508)
			{
				/* code */
				MOTO_REAL_INFO[i].TARGET_CURRENT = MOTOR_PID_RPM[i].Output*16384.0f/20000.0f;	//M3508单位毫安
			}
			else if (MOTO_REAL_INFO[i].Motor_Type == M_2006)
			{
				/* code */
				MOTO_REAL_INFO[i].TARGET_CURRENT = MOTOR_PID_RPM[i].Output*10000.0f/10000.0f;  	//M2006单位毫安
			}
			else
			{
				MOTO_REAL_INFO[i].TARGET_CURRENT = 0.0f;
			}
		}
	}

	//使能电机，发送电流数据
	M3508_Send_Currents();
}



/**
 * @brief 最值限制函数
 * @param 传入值
 * @param 限制值(最值) 
 * @return 输出值
*/
float Max_Value_Limit(float Value, float Limit)
{
	if(Value > Limit) Value = Limit;
	if(Value < -Limit) Value = -Limit;

	return Value;
}



/**
  * @brief  Homing mode 回零模式
  * @param  电机结构体
  * @return NULL
 */
void Homeing_Mode(MOTOR_REAL_INFO* RM_MOTOR)
{
	int Sign_Vel = 1.0f;
	RM_MOTOR->HomingMode.flag = 0;

	if (RM_MOTOR->HomingMode.Vel >= 0)
	{
		Sign_Vel = -1.0f;
	}

	//转速赋值
	RM_MOTOR->TARGET_RPM = RM_MOTOR->HomingMode.Vel;
	
	if(fabsf(RM_MOTOR->RPM) <= 30)
	{
		RM_MOTOR->HomingMode.cnt++;
	}
	else
	{
		RM_MOTOR->HomingMode.cnt = 0;
	}
	
	if(RM_MOTOR->HomingMode.cnt >= 50) //500ms
	{
		//清除输出
		RM_MOTOR->HomingMode.cnt = 0;
		RM_MOTOR->REAL_ANGLE=0.0f;	
		RM_MOTOR->HomingMode.flag=1;
		RM_MOTOR->Motor_Mode = SPEED_CONTROL_MODE;
		RM_MOTOR->TARGET_RPM = 0;
	}
}



/**
 * @brief 梯度速度规划
 * @param M电机结构体
 * @return NULL
*/
void Velocity_Planning_MODE(MOTOR_REAL_INFO *M3508_MOTOR)	
{
	//公式：(V1^2 - V2^2) = 2ax
	//static int cnt;//记时用
	float Ssu;   //总路程
	float Sac;   //加速路程
	float Sde;   //减速路程
	float Sco;   //匀速路程
	float Aac;   //加速加速度
	float Ade;   //减速加速度
	float S;     //当前路程
	
	// 如果所配数据有误，则不执行速度规划		
	if((M3508_MOTOR->Velocity_Planning.Rac > 1) || (M3508_MOTOR->Velocity_Planning.Rac < 0) ||		//加速路程的比例
		 (M3508_MOTOR->Velocity_Planning.Rde > 1) || (M3508_MOTOR->Velocity_Planning.Rde < 0) ||	//减速路程的比例
		 (M3508_MOTOR->Velocity_Planning.Vmax < M3508_MOTOR->Velocity_Planning.Vstart) )			//最大的速度<开始的速度 
	{
		M3508_MOTOR->TARGET_RPM = 0;  // 令夹爪不运动
		return;
	}
	// 匀速模式
	if(M3508_MOTOR->Velocity_Planning.Pstart == M3508_MOTOR->Velocity_Planning.Pend)	//开始位置=结束位置
	{
		M3508_MOTOR->TARGET_RPM = M3508_MOTOR->Velocity_Planning.Vmax;	//最大的速度
		return;
	}
	
	// 计算一些变量
	Ssu = ABS(M3508_MOTOR->Velocity_Planning.Pend - M3508_MOTOR->Velocity_Planning.Pstart); 	//总路程   
	Sac = Ssu * M3508_MOTOR->Velocity_Planning.Rac;												//加速路程 =	总路程 * 加速路程的比例
	Sde = Ssu * M3508_MOTOR->Velocity_Planning.Rde;												//减速路程 =	总路程 * 减速路程的比例
	Sco = Ssu - Sac - Sde;																		//匀速路程 = 总路程 - 加速路程 - 减速路程
	Aac = (M3508_MOTOR->Velocity_Planning.Vmax * M3508_MOTOR->Velocity_Planning.Vmax - M3508_MOTOR->Velocity_Planning.Vstart * M3508_MOTOR->Velocity_Planning.Vstart) / (2.0f * Sac);	//加速加速度 (最大的速度*最大的速度 - 开始的速度 *开始的速度 ) / (2.0f * 加速路程)
	Ade = (M3508_MOTOR->Velocity_Planning.Vend * M3508_MOTOR->Velocity_Planning.Vend -   M3508_MOTOR->Velocity_Planning.Vmax *   M3508_MOTOR->Velocity_Planning.Vmax) / (2.0f * Sde);	//减速加速度 (末速度*末速度 - 最大速度 * 最大速度 ) / (2.0f * 加速路程)
	
	// 过滤异常情况
	if(((M3508_MOTOR->Velocity_Planning.Pend > M3508_MOTOR->Velocity_Planning.Pstart) && (M3508_MOTOR->REAL_ANGLE < M3508_MOTOR->Velocity_Planning.Pstart)) ||		//[(结束位置 > 开始位置) && (处理过的真实角度pos <开始位置)]	||
		 ((M3508_MOTOR->Velocity_Planning.Pend < M3508_MOTOR->Velocity_Planning.Pstart) && (M3508_MOTOR->REAL_ANGLE > M3508_MOTOR->Velocity_Planning.Pstart)))		//	[(结束位置 < 开始位置) && (处理过的真实角度pos >开始位置)]
	{
		M3508_MOTOR->TARGET_RPM = M3508_MOTOR->Velocity_Planning.Vstart;	//TARGET_RPM = 开始的速度
	}
	else if(((M3508_MOTOR->Velocity_Planning.Pend > M3508_MOTOR->Velocity_Planning.Pstart) && (M3508_MOTOR->REAL_ANGLE > M3508_MOTOR->Velocity_Planning.Pend)) ||	//[(结束位置 > 开始位置) && (处理过的真实角度pos > 结束位置)]	||
		      ((M3508_MOTOR->Velocity_Planning.Pend < M3508_MOTOR->Velocity_Planning.Pstart) && (M3508_MOTOR->REAL_ANGLE < M3508_MOTOR->Velocity_Planning.Pend)))	//	[(结束位置 < 开始位置) && (处理过的真实角度pos < 结束位置)]
	{
		M3508_MOTOR->TARGET_RPM = M3508_MOTOR->Velocity_Planning.Vstart;	//TARGET_RPM = 末尾的速度
	}
	else
	{
		S = ABS(M3508_MOTOR->REAL_ANGLE - M3508_MOTOR->Velocity_Planning.Pstart);      //开始位置
		
		// 规划RPM
		if     (S < Sac)       M3508_MOTOR->TARGET_RPM = sqrt(2.0f * Aac * S + M3508_MOTOR->Velocity_Planning.Vstart * M3508_MOTOR->Velocity_Planning.Vstart);               // 加速阶段
		else if(S < (Sac+Sco)) M3508_MOTOR->TARGET_RPM = M3508_MOTOR->Velocity_Planning.Vmax;                                                        // 匀速阶段
		else                   M3508_MOTOR->TARGET_RPM = sqrt(M3508_MOTOR->Velocity_Planning.Vend * M3508_MOTOR->Velocity_Planning.Vend - 2.0f * Ade * ABS(Ssu - S));  // 减速阶段
	}
	 
	// 分配合适的正负号
	if(M3508_MOTOR->Velocity_Planning.Pend < M3508_MOTOR->Velocity_Planning.Pstart) M3508_MOTOR->TARGET_RPM = -M3508_MOTOR->TARGET_RPM;
	//判断是否完成
	if((fabsf(M3508_MOTOR->REAL_ANGLE - M3508_MOTOR->Velocity_Planning.Pend)) < 3)
	{
		M3508_MOTOR->Velocity_Planning.flag = 1;//设置标志位		
		M3508_MOTOR->TARGET_RPM=0;
	}
		
						
	if((fabsf(M3508_MOTOR->REAL_ANGLE - M3508_MOTOR->Velocity_Planning.Pend)) > 3)
	{
		M3508_MOTOR->Velocity_Planning.flag = 0;
	}
}



/**
  * @brief  位置控制(新位置环程序)
  * @param  target_pos目标位置
  * @return 
*/
float Position_Control(MOTOR_REAL_INFO *MOTO_REAL_INFO,float target_pos)
{
	MOTO_REAL_INFO->Motor_Mode = POSITION_CONTROL_MODE;
	MOTO_REAL_INFO->TARGET_POS = target_pos;
	if(ABS(MOTO_REAL_INFO->TARGET_POS-target_pos)<1)
		return 1;	
	else
		return 0;
}



/**
  * @brief  
	* @param 电机结构体
	* @param  target_torque目标转矩，用电流表示
	* @param target_pos目标位置
	* @retval none
  */
void Pos_Torque_Control(MOTOR_REAL_INFO *MOTO_REAL_INFO, uint16_t Target_Torque, float Target_Pos)
{
 	MOTO_REAL_INFO->Motor_Mode = POSITION_TORQUE_MODE;
	MOTO_REAL_INFO->Position_Tarque.Pos = Target_Pos;
	MOTO_REAL_INFO->Position_Tarque.TARGET_TORQUE = Target_Torque;
}



/**
 * @brief 速度模式
 * @param 目标转速
 * @return NULL
*/
void Speed_Control(MOTOR_REAL_INFO *RM_MOTOR, int16_t Target_RPM)
{
	RM_MOTOR->Motor_Mode = SPEED_CONTROL_MODE;
	RM_MOTOR->TARGET_RPM = Target_RPM;
}



/**
  * @brief  速度转矩控制函数,假如你要改变电机的转向，那么直接改变Target_Vel的值即可
  * @param  target_torque目标转矩,用电流表示（正数类型）
  * @param target_vel目标位置（有正负，代表转向）
  * @retval none
*/
void Vel_Torque_Control(MOTOR_REAL_INFO *MOTO_REAL_INFO, uint16_t Target_Torque, float Target_Vel)
{	
	MOTO_REAL_INFO->Motor_Mode = SPEED_TARQUE_CONTROL_MODE;
	MOTO_REAL_INFO->Velocity_Tarque.Target_Vel = Target_Vel;
	MOTO_REAL_INFO->Velocity_Tarque.TARGET_TORQUE = Target_Torque;
}



/**
  * @brief  设置速度规划的参数，开启速度规划控制
  * @param  
  * @param float Pstart;        //开始位置
  * @param float Pend;          //结束位置
  * @param float Vstart;        //开始的速度  单位：RPM 绝对值
  * @param float Vmax;          //最大的速度
  * @param float Vend;          //末尾的速度
  * @param float Rac;           //加速路程的比例
  * @param float Rde;           //减速路程的比例
  * @retval NULL
  */
void Velocity_Planning_setpos(MOTOR_REAL_INFO *M3508_MOTOR,float Pstart,float Pend,float Vstart,float Vmax,float Vend,float Rac,float Rde)
{
	M3508_MOTOR->Motor_Mode = VELOCITY_PLANNING_MODE;//配置模式
	M3508_MOTOR->Velocity_Planning.Pstart = Pstart;
	M3508_MOTOR->Velocity_Planning.Pend = Pend;
	M3508_MOTOR->Velocity_Planning.Vstart = Vstart;
	M3508_MOTOR->Velocity_Planning.Vmax = Vmax;
	M3508_MOTOR->Velocity_Planning.Vend = Vend;
	M3508_MOTOR->Velocity_Planning.Rac = Rac;
	M3508_MOTOR->Velocity_Planning.Rde = Rde;
	M3508_MOTOR->Velocity_Planning.flag = 0;
}



/**
 * @brief M3508角度积分
 * @param 电机结构体
 * @return NULL
*/

void RM_MOTOR_Angle_Integral(MOTOR_REAL_INFO* RM_MOTOR)
{
	static float Delta_Pos = 0;
	float Deceleration_P = 0;

	//记录第一次进入时的数据
	if(!RM_MOTOR->FIRST_ANGLE_INTEGRAL_FLAG)
	{
		RM_MOTOR->LAST_ANGLE = RM_MOTOR->ANGLE;
		RM_MOTOR->FIRST_ANGLE_INTEGRAL_FLAG = 1;
		return;
	}

	switch (RM_MOTOR->Motor_Type)
	{
		case M_3508:
			Deceleration_P = 19.0f;
			break;

		case M_2006:
			Deceleration_P = 36.0f;
			break;
		
		default:
			break;
	}	
	
	//计算角度变化
	if (RM_MOTOR->RPM >= 0)
	{
		/* code */
		if(RM_MOTOR->ANGLE < RM_MOTOR->LAST_ANGLE)
		{
			if (ABS(8191 + RM_MOTOR->ANGLE - RM_MOTOR->LAST_ANGLE < 1250))
			{
				/* code */
				Delta_Pos = ((float)(8191 + RM_MOTOR->ANGLE - RM_MOTOR->LAST_ANGLE)/8191.0f) * 360.0f;
				Delta_Pos = Delta_Pos / Deceleration_P;	//减速比
			}
		}
		else
		{
			Delta_Pos = ((float)(RM_MOTOR->ANGLE - RM_MOTOR->LAST_ANGLE) / 8191.0f) * 360.0f;
			Delta_Pos = Delta_Pos / Deceleration_P;	//减速比
		}

		//平通滤波
		if(Delta_Pos > 0)
		{
			RM_MOTOR->REAL_ANGLE += Delta_Pos;  // 积分	
		}
	}
	else
	{
		if(RM_MOTOR->ANGLE > RM_MOTOR->LAST_ANGLE)
		{
			if(ABS(8191 - RM_MOTOR->ANGLE + RM_MOTOR->LAST_ANGLE) < 1250)  // 利用两次CAN接收时间电机最大转动角度进行滤波			
			{
				Delta_Pos = ((float)(8191 - RM_MOTOR->ANGLE + RM_MOTOR->LAST_ANGLE) / 8191.0f) * 360.0f;
				Delta_Pos = Delta_Pos /Deceleration_P;	//减速比
			}
		}	
		else
		{
			Delta_Pos = ((float)(RM_MOTOR->ANGLE - RM_MOTOR->LAST_ANGLE) / 8191.0f) * 360.0f;
			Delta_Pos = Delta_Pos / Deceleration_P;	//减速比
		}
		
		// 滤波
		if(Delta_Pos < 0) 
			RM_MOTOR->REAL_ANGLE += Delta_Pos;  // 积分
	}

	// 存储角度值 
	RM_MOTOR->LAST_ANGLE = RM_MOTOR->ANGLE;
}


