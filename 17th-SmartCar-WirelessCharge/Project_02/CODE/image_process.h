
#ifndef  _search_line_h
#define  _search_line_h



//�ұ���
void seek_line(void);

//�򻯱�����ʾ�õ�����
void Pixle_Show(void);

//OTSU
uint8 otsuThreshold_Pro(uint8 *image, uint16 width, uint16 height);

//��ֵ��
void Binaryzation(uint8 *image,uint16 width, uint16 height,uint8 image_threshold);

//����
void Get_use_image(void);

void fix_image(void);

//��ʴ�˲�
uint8 Corrode_Filter(uint8 i, uint8 *image, uint8 Left_Min, uint8 Right_Max);

int32 range_protect(uint8 duty, uint8 min, uint8 max);

//����
void image_process1(void);

//ȥ����һ��/�ڵ�
void OnePoint_fix(void);

//ʮ���ж�
void Element_Cross(void);

//P�ػ�����
void Element_BackCross(void);
void Element_BackCross2(void);

void Backcross_process(void);

//�ж��߶��Ƿ񵥵�
void Analyse_line_straight(uint8 dir);  //����0�򲻵��� ����ͻ�� ����1����ߵ���

//��������
void Round_Processer(void);  //ʹ����� Round_Flag ��Ϊ0

void Round_Processer_Charge1(void); //1���ٶ�ʹ��
void Round_Processer_Charge2(void); //2���ٶ�ʹ��
void Round_Processer_Charge3(void); //3���ٶ�ʹ��
void Round_Processer_Charge4(void); //4���ٶ�ʹ��


void Round_Process_Select(void);
//�����ж�
void Element_Round_Pro(int DIR);


//�����ж�
void Element_Three_Pro(void);
//���洦��
void Three_Processor(void);

//�����߼��
void Element_Zebra(void);

//������Ԫ�ش���
void Element_Zebra_Process(void);



//����Ԫ�ش�����
void Element_Judge(void);
void Element_Do(void);
void Element(void);

//�����޸�  ������ɺ� ���� �޸�����
void Mid_Line_Fix(uint8 DATA);

//Ԫ��չʾ ������
void Element_show(void);

//������� ���???
void Out_Garage(void);
void Out_Garage2(void);


extern uint8 camear_foward;
extern uint8 up_line;  //����
extern uint8 zuo_line; // ǰհ ����
extern uint8 you_line; // ǰհ ����
extern uint8 middle_line; // ǰհ ����
extern uint8 middle_line1; // ǰհ ����
//��������
extern uint8 ImageSide[MT9V03X_H2][2];  //1����� 2�ұ���
extern uint8 UpdowmSide[2][MT9V03X_W2];  //���±��� ?
extern uint8 ImageMid[MT9V03X_H2];  //����

extern uint8 Road_width[MT9V03X_H2];  //�������

extern uint8 low_line;  //�϶������� ��Ҫ����

//������־λ
extern uint8 Round_Road_Flag;
//1���� 2���� 3������ 4�뿪����
extern uint8 Round_Statue;
//���� ��ʱ
extern uint8 Round_Cnt_Boring;
extern uint8 Round_Cnt;
//ʮ�ֻػ���־λ
extern uint8 Cross_Road_Flag;

//�����־λ
extern uint8 Three_Fork_Flag;
extern uint8 Three_Fork_Cnt;


//�����߱�־λ
extern uint8 Zebra_Flag;
extern uint8 Zebra_cnt;


//Loss_line ���߱�־λ
extern uint8 Loss_line_Flag;
extern uint8 Loss_line;

//���� ѡ����
extern uint8 Fix_Line_Flag;

//Ԫ�����ڴ����־λ Ԫ�ش���ʱ��Ԫ���ж�
extern uint8 Element_Processing_Flag;

//Ŀ��Ƕȸı��־λ  �ﵽĿ��ǶȽ�����һ�����������˳�����  ���� 60��    ���� 60�ؽ���  260 ������  360 �س���
extern uint8 Angle_CNT_Flag;

//�ػ���־λ
extern uint8 Back_Cross_Flag;

//P�ػ�֡�����ͳ����  ��Ԫ��������10֡��Ϊƫ��ʱ���ж�Ϊ�ػ� ֮������ѭ�� �����Դ��������̫����
extern sint32 Back_Road_FPS_CNT1; //��
extern sint32 Back_Road_FPS_CNT2; //��

//���ػ���־λ
extern uint8 Back_Out_Flag;
//���ػ�������
extern uint8 Back_Out_Boring;
extern uint8 Back_Cnt;  //�ػ�������

//���ұ�����������־λ 1���� 0������
extern uint8 Zuo_Correct_Flag;
extern uint8 You_Correct_Flag;

//�������鶪��ʱ��ֵ
extern uint8 Zuo_Hind;
extern uint8 You_Hind;

//��ʴ�˲���־λ 1Ϊ�� 0Ϊ��
extern uint8 Eat_Line_Flag;

//�����־λ
extern uint8 Out_Garage_Flag;

//extern uint8 Finish_Flag;
extern uint8 Judge_Stright_Amount;//ֱ���ж��� ��25/50����Ҫ��Ϊ�ж�����  �ٶ�1 25   �ٶ�2 20 �ٶ�3 20
#endif
