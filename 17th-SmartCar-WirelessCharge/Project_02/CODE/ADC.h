

extern float BatVolt;
extern float TestVolt;
extern float Last_TestVolt;
extern uint8 Stop_Flag;   //ͣ����־λ ��־λ��1ʱ��ر�ת�򻷵��ٶȵ���
extern uint8 Charge_Finish_Flag;  //�����ɺ��־λ Stop_Flag=1 ��ͣ��ʱ��0 ������1
extern float Charge_Top;  //����ֵ��ѹ
extern uint8 Sit_Flag;   //ͣ����־λ ��־λ��1ʱ��ر�ת�򻷵��ٶȵ���
//extern float Charge_point;

extern uint8 Charge_Flag;

extern float Charge_point;


void ADC_Init(void);


void Charge_stop(void);

void Stop_speed1(void);
void Stop_speed2(void);
void Stop_speed3(void);
void Stop_speed4(void);
