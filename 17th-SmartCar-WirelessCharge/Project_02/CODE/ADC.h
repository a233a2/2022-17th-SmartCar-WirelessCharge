

extern float BatVolt;
extern float TestVolt;
extern float Last_TestVolt;
extern uint8 Stop_Flag;   //停车标志位 标志位置1时候关闭转向环的速度调节
extern uint8 Charge_Finish_Flag;  //充电完成后标志位 Stop_Flag=1 即停车时置0 充满置1
extern float Charge_Top;  //充电峰值电压
extern uint8 Sit_Flag;   //停车标志位 标志位置1时候关闭转向环的速度调节
//extern float Charge_point;

extern uint8 Charge_Flag;

extern float Charge_point;


void ADC_Init(void);


void Charge_stop(void);

void Stop_speed1(void);
void Stop_speed2(void);
void Stop_speed3(void);
void Stop_speed4(void);
