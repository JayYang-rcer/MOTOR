#ifndef __MOVE__H
#define __MOVE__H
#include <stm32f4xx.h>

#define COS60 0.500000f
#define COS30 0.866025f
#define COS45 0.707106f
#define V_REAL    0.128f/60                      //���ӵ����ٶ�
#define  PI              3.1415926f    //PI��ֵ
#define WHEEL_R              0.152f/2                 //���Ӱ뾶 
#define RM_transition_MS     (PI*WHEEL_R)/570.0f      //ת�����ٶȵ�ת��  
#define MS_transition_RM     570.0f/(PI*WHEEL_R)      //�ٶ���ת�ٵ�ת��                 


extern int MOVE_OK ;

//�����˵������ݽṹ��
typedef struct ROBOT_SPEED
{
 	float World_VX;
 	float World_VY;
 	float World_W;
 	float Robot_VX;
 	float Robot_VY;
} ROBOT_SPEED;


typedef struct ROBOT_CHASSIS
{
	ROBOT_SPEED SPEED;
  float Motor_RPM[4];           //4�����ӵ�ת��
} ROBOT_CHASSIS;

//extern  ROBOT_CHASSIS  ROBOT_CHASSI;

//������·������ʱ�����
typedef struct TIMES
{
	uint16_t TIME_LAST;           //�ϴε�ʱ��
	uint16_t TIME_pass;           //������ʱ��   
} TIMES;

typedef struct PATH_FOLLOW
{
	TIMES  Times;            //ʱ��ṹ��
	uint8_t     COUNT;            //����
} PATH_FOLLOW;


typedef struct ROBO_VELOCITY_PLANNING
{
	float up_a;         //���ٵļ��ٶ�
	float dn_a;
	float Vstart;       // ��λ��RPM ����ֵ
	float Vmax;
	float Vend;	
	float q1;           // ��·��
	float t;            // ʱ��t
	float t1;           //����ʱ��T1
	float t2;           //����ʱ��T2
}ROBO_VELOCITY_PLANNING;

extern ROBO_VELOCITY_PLANNING  v_planing;

void Robot_Wheels_RPM_calculate(void);
void Robot_Wheels_Adjust(void);
void Motor_PID_init (void);

#endif
