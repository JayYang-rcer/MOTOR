#include "config.h"

/**
 * @brief 电机类型选择,请跳转到config.c中配置
 * @param 电机类型
 * @return NULL
*/
void Motor_Type_Init(void)
{
	MOTO_REAL_INFO[0].Motor_Type = M_2006;
	MOTO_REAL_INFO[1].Motor_Type = M_3508;
	MOTO_REAL_INFO[2].Motor_Type = M_3508;
    MOTO_REAL_INFO[3].Motor_Type = M_3508;
}


/**
 * @brief PID初始化函数,请跳转到config.c中配置
*/
void Pid_Init_All(void)
{
    PID_Parameter_Init(&MOTOR_PID_RPM[0],12.0,4.0,0.1 , 5000 ,10 , 0 , 500);	//0x201号电机 M2006
	PID_Parameter_Init(&MOTOR_PID_RPM[1],12.0,4.0,0.1 , 5000 ,10 , 0 , 500);	//0x202号电机 M3508
	PID_Parameter_Init(&MOTOR_PID_RPM[2],12.0,4.0,0.1 , 5000 ,10 , 0 , 500);	//0x203号电机 M3508
	PID_Parameter_Init(&MOTOR_PID_RPM[3],12.0,4.0,0.1 , 5000 ,10 , 0 , 500);	//0x204号电机 M3508

    PID_Parameter_Init(&MOTOR_PID_POS[0], 10.2, 0, 3, 7000, 0.05, 7000, 500);   //位置环PID
    PID_Parameter_Init(&MOTOR_PID_POS[1], 10.2, 0, 3, 7000, 0.05, 7000, 500);
    PID_Parameter_Init(&MOTOR_PID_POS[2], 10.2, 0, 3, 7000, 0.05, 7000, 500);
    PID_Parameter_Init(&MOTOR_PID_POS[3], 10.2, 0, 3, 7000, 0.05, 7000, 500);
}
