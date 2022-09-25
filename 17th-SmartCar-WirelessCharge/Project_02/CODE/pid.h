#ifndef  PID_h
#define  PID_h

extern double S_zuo_duty;   //速度环电机PWM控制输出
extern double S_you_duty;

extern sint32 zuo_set_speed;   //真实的  通过  设置 编码器 输出值 来 控制速度
extern sint32 you_set_speed;

extern sint32 zuo_real_speed; //编码器测量速度 Encoder_get函数得到速度值
extern sint32 you_real_speed;



extern sint32 real_speed;

extern float dir_err;   //差值 放定时器里面用来控制速度
extern float mid_err;   //循迹用的误差 可能为左右中三种情况
extern float head_err;
extern float ang_err;
extern float ang_speed_dif;   //左右电机PWM 角度环PD生产的电机速度差
extern float just_mid_err; //固定中线误差 用来判断P回环和元素判断有用

extern float Speed_dif;   //有符号类型啊  转向环输出
extern int    speed_err;
extern double  speed_out;    //同速度速度环输出

extern uint8 Mid_Get_Type;  //中线获取方式

double Position_control(void);

void Position_PD_control(int dir);

void Incremental_PI_control(void);

double Speed_Control(void);

//挡位速度调整
void Speed_Target_Control(void);

void Mid_Get(void);
//double Angle_control(void);




#endif

