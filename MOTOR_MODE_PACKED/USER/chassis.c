#include "main.h"


volatile  ROBOT_CHASSIS  ROBOT_CHASSI = {0};


/**
 * @brief 地盘解算，四全向轮
 * @param NULL
 * @return NULL
*/
void Robot_Wheels_RPM_calculate(void)
{
	ROBOT_CHASSI.Motor_Target_RPM[0] = -ROBOT_CHASSI.SPEED.Robot_VY*COS45 + ROBOT_CHASSI.SPEED.Robot_VX*COS45 + ROBOT_CHASSI.SPEED.World_W*MS_transition_RM;
	ROBOT_CHASSI.Motor_Target_RPM[1] = -ROBOT_CHASSI.SPEED.Robot_VY*COS45 - ROBOT_CHASSI.SPEED.Robot_VX*COS45 + ROBOT_CHASSI.SPEED.World_W*MS_transition_RM;
	ROBOT_CHASSI.Motor_Target_RPM[2] =  ROBOT_CHASSI.SPEED.Robot_VY*COS45 - ROBOT_CHASSI.SPEED.Robot_VX*COS45 + ROBOT_CHASSI.SPEED.World_W*MS_transition_RM;
	ROBOT_CHASSI.Motor_Target_RPM[3] =  ROBOT_CHASSI.SPEED.Robot_VY*COS45 + ROBOT_CHASSI.SPEED.Robot_VX*COS45 + ROBOT_CHASSI.SPEED.World_W*MS_transition_RM;
}


/**
 * @brief 控制底盘运动
 * @param NULL
 * @return NULL
*/
void Robot_Wheels_Adjust(void)
{

	Speed_Control(&MOTO_REAL_INFO[1],-3000);
	Speed_Control(&MOTO_REAL_INFO[2],3000);

	Motor_Control();
}


