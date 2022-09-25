#include"headfile.h"

#define L1 P33_13
#define L2 P33_12
#define L3 P33_11


#define LA P33_10
#define LB P33_9
#define LC P33_8
#define LD P33_7
#define LE P33_6

void LED_Init(void)
{
        //��ʼ��LED��������
        gpio_init(L1, GPO,1, PUSHPULL);
        gpio_init(L2, GPO,1, PUSHPULL);
        gpio_init(L3, GPO,1, PUSHPULL);
        gpio_init(LA, GPO,0, PUSHPULL);
        gpio_init(LB, GPO,0, PUSHPULL);
        gpio_init(LC, GPO,0, PUSHPULL);
        gpio_init(LD, GPO,0, PUSHPULL);
        gpio_init(LE, GPO,0, PUSHPULL);
}


void Clear_LED(void)
{
    //�ر�����LED��ʾ
    gpio_set(L1,1);
    gpio_set(L2,1);
    gpio_set(L3,1);
    systick_delay_us(STM0,1);
    gpio_set(LA,0);
    gpio_set(LB,0);
    gpio_set(LC,0);
    gpio_set(LD,0);
    gpio_set(LE,0);
}


uint8 temp=0,temp1=0;       //�������
uint8 num=0;                //ˢ�¼ƴ�

//�Ŷ�ʱ��4  1ms  ����   cpu0��while(1)�� ��ʱ1ms
void Show_Voltage(void)
{
    temp=(uint8)BatVolt;     //��ȡ��Դ��ѹ
    num++;                          //ˢ�¼ƴ�
    if(num>14) num=0;

    Clear_LED();

    if(num < temp)        //������Ҫ������LED��
    {
        temp1 = num/3;
        switch(temp1){                          //����
//            case 0:  gpio_set(LA,1);break;
//            case 1:  gpio_set(LB,1);break;
//            case 2:  gpio_set(LC,1);break;
//            case 3:  gpio_set(LD,1);break;
//            case 4:  gpio_set(LE,1);break;

            case 0:  gpio_set(LE,1);break;
            case 1:  gpio_set(LD,1);break;
            case 2:  gpio_set(LC,1);break;
            case 3:  gpio_set(LB,1);break;
            case 4:  gpio_set(LA,1);break;
            default:break;
        }
        temp1 = num%3;
        switch(temp1){                          //����
            case 0:  gpio_set(L3,0);break;
            case 1:  gpio_set(L2,0);break;
            case 2:  gpio_set(L1,0);break;
            default:break;
        }
    }
}
