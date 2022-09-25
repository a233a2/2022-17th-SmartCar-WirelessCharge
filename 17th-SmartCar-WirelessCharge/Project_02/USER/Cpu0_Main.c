#include "headfile.h"
#pragma section all "cpu0_dsram"
//���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��
//����TCϵ��Ĭ���ǲ�֧���ж�Ƕ�׵ģ�ϣ��֧���ж�Ƕ����Ҫ���ж���ʹ��enableInterrupts();�������ж�Ƕ��
//�򵥵�˵ʵ���Ͻ����жϺ�TCϵ�е�Ӳ���Զ�������disableInterrupts();���ܾ���Ӧ�κε��жϣ������Ҫ�����Լ��ֶ�����enableInterrupts();�������жϵ���Ӧ��

int core0_main(void)
{
    disableInterrupts();
    get_clk();//��ȡʱ��Ƶ��  ��ر���

    icm20602_init_spi();
    systick_delay_ms(STM0,2);

    LED_Init();      //�ư��ʼ��
    systick_delay_ms(STM0,2);

    MotorInit();
    systick_delay_ms(STM0,2);

    mt9v03x_init(); //��ʼ������ͷ
    systick_delay_ms(STM0,2);

    Encoder_Init();
    systick_delay_ms(STM0,2);

    SWITCH_init();
    systick_delay_ms(STM0,2);

    ADC_Init();
    systick_delay_ms(STM0,20);//����25�����˳�ʼ��??


    //��ʱ���ж�
    pit_interrupt_ms(CCU6_0, PIT_CH0, 5);//5ms 40ms�ٶȻ�  5msת��

    pit_interrupt_ms(CCU6_0, PIT_CH1, 1);//����������ר�ö�ʱ��:Ƶ��5ms �ǶȻ�ȡ 2ms

    pit_interrupt_ms(CCU6_1, PIT_CH0, 50);//������50  �Լ� ADC�ɼ�

//    pit_interrupt_ms(CCU6_1, PIT_CH1, 1);  //�ư�����

    //�ȴ����к��ĳ�ʼ�����
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();
    while (TRUE)
    {
//        MotorControl(0.1,0.1);
//        if(Round_Cnt==0&&Round_Statue==5)   //���󻷵�ʱ��Ŀ�����ѹ
//        {
//            Charge_Top=10.00;   //����
//        }



    }
}
#pragma section all restore






