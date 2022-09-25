#ifndef _gyro_h
#define _gyro_h

extern float gx, gy, gz;
extern float ax, ay, az;

extern float gx_zero, gy_zero, gz_zero;

extern float angle_pitch, angle_yaw, angle_roll;

extern int pitch_flag, yaw_flag, roll_flag; //开始积分标志

extern float Target_Z_angle;

void angle_get(void);

void angle_filter(void);

#endif
