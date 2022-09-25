#include "headfile.h"

//BTN7960
#define MOTOR1_P          ATOM1_CH5_P21_7   //P21_7
#define MOTOR1_N          ATOM0_CH2_P21_4

#define MOTOR2_P          ATOM0_CH1_P21_3
#define MOTOR2_N          ATOM0_CH3_P21_5

////DRV8701E
//#define MOTOR1_P          ATOM2_CH2_P33_6   //PWM
//#define MOTOR1_N          ATOM2_CH3_P33_7   //01
//#define MOTOR2_P          ATOM2_CH4_P33_8   //PWM
//#define MOTOR2_N          ATOM0_CH1_P33_9   //01


#define MOTOR_FREQUENCY          17*1000


void MotorInit (void)
{

    gpio_init(P21_7, GPO, 0, PUSHPULL);
    gpio_init(P21_4, GPO, 0, PUSHPULL);
    gpio_init(P21_3, GPO, 0, PUSHPULL);
    gpio_init(P21_5, GPO, 0, PUSHPULL);

    gtm_pwm_init(MOTOR1_P, MOTOR_FREQUENCY, 0);  //�����PWMʹ��17K�������Ծ�������PWM���ŵ���Ųɼ���
    gtm_pwm_init(MOTOR1_N, MOTOR_FREQUENCY, 0);
    gtm_pwm_init(MOTOR2_P, MOTOR_FREQUENCY, 0);
    gtm_pwm_init(MOTOR2_N, MOTOR_FREQUENCY, 0);
}

/*
 * ���߱���1.0
 * �޶��߷��س���
 * Protect_Flag=1 ���� else
 */
void Loss_line_Protect(void)
{
    if(low_line>=117&&Out_Garage_Flag==0)   //���߱��� v1.0&&Out_Garage_Flag==0
    {
//        Ring_Flag=3;
        S_zuo_duty=0;
        S_you_duty=0;
//        Target_speed=0;
    }else
        {
            //�����߱���
        }
}


/*
 *BTN7960
 *  ���PWMʵʱ����  From LQ
 *��ڲ��� ���� PWM DUTY ռ�ձ� 0% 0-10000 100%
 *
 *������ֵ  ֧������ת
 */
void MotorControl(double motor1, double motor2)//���� �ٶ�ֵ���ɿ��Ƶ��ת�� ����ٶ� 1 ��С�ٶ� 0
{
    static sint32 motor11,motor22;  //ǿ������ת��

    if(motor1>=0.85)motor1=0.85;
    if(motor1<=-0.85)motor1=-0.85;

    if(motor2>=0.85)motor2=0.85;
    if(motor2<=-0.85)motor2=-0.85;

    motor11=motor1*10000;
    motor22=motor2*10000;

    if (motor11 >= 0)
    {
        pwm_duty(MOTOR1_P,motor11);
        pwm_duty(MOTOR1_N, 0);
    }
    else if(motor11 < 0)    //��ת
    {
        pwm_duty(MOTOR1_P, 0);
        pwm_duty(MOTOR1_N,abs(motor11));
    }
    if (motor22 >= 0)
    {
        pwm_duty(MOTOR2_P,motor22);
        pwm_duty(MOTOR2_N, 0);
    }
    else if(motor22 < 0)
    {
        pwm_duty(MOTOR2_P, 0);
        pwm_duty(MOTOR2_N, abs(motor22));
    }
}

/*
 * DRV8701
 */
void MotorControl_DRV(double motor1, double motor2)//���� �ٶ�ֵ���ɿ��Ƶ��ת�� ����ٶ� 1 ��С�ٶ� 0
{
    static sint32 motor11,motor22;  //ǿ������ת��

    if(motor1>=0.80)motor1=0.80;
    if(motor1<=-0.80)motor1=-0.80;

    if(motor2>=0.80)motor2=0.80;
    if(motor2<=-0.80)motor2=-0.80;

    motor11=motor1*10000;
    motor22=motor2*10000;

    if (motor1 >= 0)
    {
        pwm_duty(MOTOR1_P,motor11);
        pwm_duty(MOTOR1_N, 0);
    }
    else if(motor1 < 0)    //��ת
    {
        pwm_duty(MOTOR1_P, abs(motor11));
        pwm_duty(MOTOR1_N,10000);
    }
    if (motor2 >= 0)
    {
        pwm_duty(MOTOR2_P,motor22);
        pwm_duty(MOTOR2_N, 0);
    }
    else if(motor2 < 0)
    {
        pwm_duty(MOTOR2_P, abs(motor22));
        pwm_duty(MOTOR2_N, 10000);
    }
}

void Motor_test(void)
{
    pwm_duty(MOTOR1_P, 0);
    pwm_duty(MOTOR1_N,2000);
    pwm_duty(MOTOR2_P, 0);
    pwm_duty(MOTOR2_N, 2000);
}


