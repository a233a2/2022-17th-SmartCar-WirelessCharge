#include"headfile.h"

/**
 * @brief �����������˲�
 */
float gx, gy, gz;  //���ٶ�
//float gz;
//float ax, ay, az;  //���ٶ�
float gz_zero=0.100; //�Ƕ����Ư��
float gx_zero=0; //�Ƕ����Ư��
float gy_zero=0; //�Ƕ����Ư��

float Target_Z_angle; //Ŀ��Ƕ�ֵ  Ԫ���ж����

void angle_filter(void)
{
    gx = 0;
    gy = 0;
    gz = 0;
    for (int i = 0; i < 10; i++)
    {
        get_icm20602_gyro_spi();    //���ٶȻ��
        gz += icm_gyro_z;
    }
    gz = gz/10.0 - gz_zero;

}

/**
 * @brief ���ٶȻ��ֻ�ȡ�Ƕ�
 * �ڶ�ʱ���е��ã�����2ms
 * ��ʼ������������־λ����
 */
float angle_pitch, angle_yaw, angle_roll;

int pitch_flag=0, yaw_flag=0, roll_flag=0; //��ʼ���ֱ�־
void angle_get(void)
{
    angle_filter(); //��̬��������Ϣ��ȡ
    angle_yaw += gz * 0.00012480f;
}


