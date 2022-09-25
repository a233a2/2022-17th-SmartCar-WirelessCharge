#include"headfile.h"

/**
 * @brief 陀螺仪数据滤波
 */
float gx, gy, gz;  //角速度
//float gz;
//float ax, ay, az;  //加速度
float gz_zero=0.100; //角度零点漂移
float gx_zero=0; //角度零点漂移
float gy_zero=0; //角度零点漂移

float Target_Z_angle; //目标角度值  元素判断输出

void angle_filter(void)
{
    gx = 0;
    gy = 0;
    gz = 0;
    for (int i = 0; i < 10; i++)
    {
        get_icm20602_gyro_spi();    //角速度获得
        gz += icm_gyro_z;
    }
    gz = gz/10.0 - gz_zero;

}

/**
 * @brief 角速度积分获取角度
 * 在定时器中调用，周期2ms
 * 开始积分由三个标志位控制
 */
float angle_pitch, angle_yaw, angle_roll;

int pitch_flag=0, yaw_flag=0, roll_flag=0; //开始积分标志
void angle_get(void)
{
    angle_filter(); //姿态传感器信息获取
    angle_yaw += gz * 0.00012480f;
}


