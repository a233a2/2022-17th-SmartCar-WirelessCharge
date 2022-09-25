#include "headfile.h"


//������ȫ�ֱ���
sint32 Encoder1,Encoder2;

sint32 zuo_real_speed_add;
sint32 you_real_speed_add;

/*
 * ��������ʼ��
 */
void Encoder_Init (void)
{
    /*��������ʼ��*/
    gpt12_init(GPT12_T4, GPT12_T4INA_P02_8, GPT12_T4EUDA_P00_9);
    gpt12_init(GPT12_T5, GPT12_T5INB_P10_3, GPT12_T5EUDB_P10_1);    //������� ��ȷ
}


/*
 * �Ŷ�ʱ������
 *5ms
 */
void Encoder_get_speed(void)
{
    Encoder1 =  gpt12_get(GPT12_T4);   //��ת���� ������ֵ  1024�� 5ms
    Encoder2 = -gpt12_get(GPT12_T5);   //��Ҫʵ����Ȧ����

    gpt12_clear(GPT12_T4);
    gpt12_clear(GPT12_T5);  //���������ݲɼ���ɺ��������

    zuo_real_speed = Encoder1;
    you_real_speed = Encoder2;

    zuo_real_speed_add+=zuo_real_speed;
    you_real_speed_add+=you_real_speed;

}

