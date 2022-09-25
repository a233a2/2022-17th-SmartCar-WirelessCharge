#include"headfile.h"

//���ݵ�ѹ ʵʱ
float BatVolt;

//�����ѹֵ
float TestVolt;

uint8 Max_Pass_Flag=0;  //�������ߵ��־λ

uint8 Charge_Flag=0; //����־λ �ж��Ƿ��ͣ�����ı�־λ ��������Ϊ��ǰ����

uint8 Stop_Flag=1;   //ͣ����־λ ��־λ��1ʱ��ر�ת�򻷵��ٶȵ���  ������1

sint32 Min_Stop_Point=1750; //ͣ����ֵ ���ֵһ�������ٶȱ仯���仯 Ӧ��Ҳ��Ŀǰ�����е��ϵ

uint8 Charge_Finish_Flag=0;  //�����ɺ��־λ Stop_Flag=1 ��ͣ��ʱ��0 ������1


uint8 Bat_ABS=0;  //���ݵ�ѹ������λ 5����λ

float Charge_Top=12.50;  //����ֵ��ѹ

float Charge_point=8.90;

uint8 Speed_Gear=0;  //����real_speed���õ��ٶȵ�

//ƽʱ��1 ��������0 ������sit
uint8 Sit_Flag=0;//���ͣ����־λ ��Ϊ�ֶμ���ͣ��  �ڵ���MaxTestVlot��ʱΪ�˷�ֹ����ǰ��ʻ����������ô˱�־λ �ٶ��������Ʊ�־λ
//�˱�־λ����һ�������粻��ִ��ͣ���ٶȿ��Ƴ���Ĺ���


//�ٶȷּ�����
void Speed_Sort_Gear(void)
{
    if(real_speed<=40)
    {
        Min_Stop_Point=2200;
    }
    if(real_speed>=40&&real_speed<=60)
    {
        Min_Stop_Point=2100;
    }
    if(real_speed>=60&&real_speed<=80)
    {
        Min_Stop_Point=2000;
    }
}


void Stop_Bat_Gear(void)
{
    if(BatVolt<=6.00)     //6v���� ͣ����ǰͣ
    {
        Min_Stop_Point=1800;
    }
    if(BatVolt>6.00&&BatVolt<=7.00)   //6-7.5 ��Ϊ��ǰ
    {
        Min_Stop_Point=1950;
    }
    if(BatVolt>7.00)  //7.50 ���� ���
    {
        Min_Stop_Point=2100;
    }
}

/*
 * ���ݵ�ѹ��ʼ��
 */
void ADC_Init(void)
{
    adc_init(ADC_1, ADC1_CH9_A25);//V_out_adc      ����ѹ
    adc_init(ADC_2, ADC2_CH3_A35);//V_out_adc    ���ݵ�ѹBatVolt
}





//ͣ���ٶȿ��Ƴ���
//��������1Ϊ Charge_Flag=0 �������½���9.00��
//��������2Ϊ Sit_Flag=0
void Stop_Test_Process(void)
{
    switch (MODE_SELECT)   //�ٶȿ��Ƴ��� �����ٶȵ�λ�ֶδ���
    {
        case 1:
        {
            Stop_speed1();
            break;
        }
        case 2:
        {
            Stop_speed2();
            break;
        }
        case 3:
        {
            Stop_speed3();
            break;
        }
        case 4:
        {
            Stop_speed4();
        }
    }
}

/*
 * ͣ��������
 * ��һ������ΪPID����
 * ͣ�� ���˼·Ϊ �Ƚ��м����ٽ����ٶ���0ͣ��
 * �ڵ����ﵽ��׼��
 */
void Charge_stop(void)//ͣһ�ν��žͳ��ȥ�� ͣ��Ч������
{
//    Speed_Sort_Gear();
    Stop_Bat_Gear();
//    Stop_Test_Process();
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=Min_Stop_Point&&TestVolt<=3600)
        {
            Sit_Flag=1;  //����ִ����Щ��� �ٶ�����
            Stop_Flag=1;   //����ͣ���� ����ͣ������ ��stop_flag=1 ת�򻷽ڵ��ٶȵ��ڹر�
        }
    }

    if(Sit_Flag==1)  //�ٶȿ���������ȼ�
    {
        Target_speed=0;  //�����ٶȸ�ֵ0 ֱ��������
    }

        if(BatVolt>=Charge_Top)
        {
            Stop_Flag=0;
            Sit_Flag=0;             //���Sit_Flag����  ��ʱ�������µ� �涨  ����ʱ ����ͣ��������  sit_flag���þ��ǲ������粻Ȼ��
            Charge_Flag=1;         //����ɳ���־λ  ���ж�ֵΪ�������
        }

//        if(Round_Statue>=3)
//        {
//            Charge_point=6.00;        //���� �����������
//        }else if(Round_Statue<3)
//        {
//            Charge_point=9.00;          //�ó����  ��֤���������������뻷�����
//        }

        if(BatVolt<=Charge_point)Charge_Flag=0;

}
//bug ����־λ���Ƴ�����ʱ����� ֱ��12.00�������

//�ٶ�1 ͣ����̬��ֵ
void Stop_speed1(void)
{
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=2000&&TestVolt<=3500)
        {
            Sit_Flag=1;  //����ִ����Щ��� �ٶ�����
            Stop_Flag=1;   //����ͣ���� ����ͣ������ ��stop_flag=1 ת�򻷽ڵ��ٶȵ��ڹر�
        }
    }
}

//�ٶ�2 ͣ����̬��ֵ ��������
void Stop_speed2(void)
{
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=2100&&TestVolt<=3500)
        {
            Sit_Flag=1;  //����ִ����Щ��� �ٶ�����
            Stop_Flag=1;   //����ͣ���� ����ͣ������ ��stop_flag=1 ת�򻷽ڵ��ٶȵ��ڹر�
        }
//        else Stop_Flag=0;
    }
}


//�ٶ�3 ͣ����̬��ֵ
void Stop_speed3(void)
{
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=1800&&TestVolt<=3500)
        {
            Sit_Flag=1;  //����ִ����Щ��� �ٶ�����
            Stop_Flag=1;   //����ͣ���� ����ͣ������ ��stop_flag=1 ת�򻷽ڵ��ٶȵ��ڹر�
        }
//        else Stop_Flag=0;
    }
}
//�ٶ�3 ͣ����̬��ֵ
void Stop_speed4(void)
{
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=1700&&TestVolt<=3000)
        {
            Sit_Flag=1;  //����ִ����Щ��� �ٶ�����
//            Target_speed=0;  //����׼��ͣ��
            Stop_Flag=1;   //����ͣ���� ����ͣ������ ��stop_flag=1 ת�򻷽ڵ��ٶȵ��ڹر�
        }
//        else Stop_Flag=0;
    }
}

//ͣ��ͣ����������5s����!

//ͣ����
//ͣ�����ݵ������ٶ���ȷ�� ����ΪӦ����Ҫ���ݵ���������
//������ʱ ͣ��Ӧ�Ӻ� 2100
//������ʱ ͣ��Ӧ��ǰ 1700


//��紦
// ����   ��һ�������   �һ���  �󻷵�  �������  5�����
 //     ʣ�����8.00v����   6.5v    5.0v    6.50v
