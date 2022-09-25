#include "headfile.h"
#pragma section all "cpu0_dsram"
//将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中
//对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用enableInterrupts();来开启中断嵌套
//简单点说实际上进入中断后TC系列的硬件自动调用了disableInterrupts();来拒绝响应任何的中断，因此需要我们自己手动调用enableInterrupts();来开启中断的响应。

int core0_main(void)
{
    disableInterrupts();
    get_clk();//获取时钟频率  务必保留

    icm20602_init_spi();
    systick_delay_ms(STM0,2);

    LED_Init();      //灯板初始化
    systick_delay_ms(STM0,2);

    MotorInit();
    systick_delay_ms(STM0,2);

    mt9v03x_init(); //初始化摄像头
    systick_delay_ms(STM0,2);

    Encoder_Init();
    systick_delay_ms(STM0,2);

    SWITCH_init();
    systick_delay_ms(STM0,2);

    ADC_Init();
    systick_delay_ms(STM0,20);//不是25过不了初始化??


    //定时器中断
    pit_interrupt_ms(CCU6_0, PIT_CH0, 5);//5ms 40ms速度环  5ms转向环

    pit_interrupt_ms(CCU6_0, PIT_CH1, 1);//编码器采样专用定时器:频率5ms 角度获取 2ms

    pit_interrupt_ms(CCU6_1, PIT_CH0, 50);//蜂鸣器50  以及 ADC采集

//    pit_interrupt_ms(CCU6_1, PIT_CH1, 1);  //灯板驱动

    //等待所有核心初始化完毕
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();
    while (TRUE)
    {
//        MotorControl(0.1,0.1);
//        if(Round_Cnt==0&&Round_Statue==5)   //出左环岛时改目标充电电压
//        {
//            Charge_Top=10.00;   //有用
//        }



    }
}
#pragma section all restore






