#ifndef  PID_h
#define  PID_h

extern double S_zuo_duty;   //�ٶȻ����PWM�������
extern double S_you_duty;

extern sint32 zuo_set_speed;   //��ʵ��  ͨ��  ���� ������ ���ֵ �� �����ٶ�
extern sint32 you_set_speed;

extern sint32 zuo_real_speed; //�����������ٶ� Encoder_get�����õ��ٶ�ֵ
extern sint32 you_real_speed;



extern sint32 real_speed;

extern float dir_err;   //��ֵ �Ŷ�ʱ���������������ٶ�
extern float mid_err;   //ѭ���õ���� ����Ϊ�������������
extern float head_err;
extern float ang_err;
extern float ang_speed_dif;   //���ҵ��PWM �ǶȻ�PD�����ĵ���ٶȲ�
extern float just_mid_err; //�̶�������� �����ж�P�ػ���Ԫ���ж�����

extern float Speed_dif;   //�з������Ͱ�  ת�����
extern int    speed_err;
extern double  speed_out;    //ͬ�ٶ��ٶȻ����

extern uint8 Mid_Get_Type;  //���߻�ȡ��ʽ

double Position_control(void);

void Position_PD_control(int dir);

void Incremental_PI_control(void);

double Speed_Control(void);

//��λ�ٶȵ���
void Speed_Target_Control(void);

void Mid_Get(void);
//double Angle_control(void);




#endif

