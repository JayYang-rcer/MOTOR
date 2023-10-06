#ifndef  __CHASSIS_H
#define  __CHASSIS_H

void Speed_Calculate(void);
void Robot_Wheels_Adjust(void);

#define COS60 0.500000f
#define COS30 0.866025f
#define COS45 0.707106f
#define V_REAL    0.128f/60                      //���ӵ����ٶ�
#define  PI              3.1415926f    //PI��ֵ
#define WHEEL_R              0.152f/2                 //���Ӱ뾶 
#define RM_transition_MS     (PI*WHEEL_R)/570.0f      //ת�����ٶȵ�ת��  
#define MS_transition_RM     570.0f/(PI*WHEEL_R)      //�ٶ���ת�ٵ�ת��    


//�����˵������ݽṹ��
typedef struct ROBOT_SPEED
{
 	float World_W;
 	float Robot_VX;
 	float Robot_VY;
} ROBOT_SPEED;


typedef struct ROBOT_CHASSIS
{
	ROBOT_SPEED SPEED;
	float Motor_Target_RPM[4];           //4�����ӵ�Ŀ��ת��
} ROBOT_CHASSIS;

#endif

