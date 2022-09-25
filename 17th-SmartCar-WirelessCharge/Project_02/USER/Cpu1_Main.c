#include "headfile.h"
#pragma section all "cpu1_dsram"
//将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中
void core1_main(void)
{
	disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    //用户在此处调用各种初始化函数等

    lcd_init(); //初始化IPS屏幕
    systick_delay_ms(STM0,2);

    lcd_display_chinese(30,50,16,chinese_test[0],6,RED);

    systick_delay_ms(STM0,20);
    lcd_clear(WHITE);
    Beep_init();
    systick_delay_ms(STM0,2);
    Beep_Ring(1);

//    seekfree_wireless_init();
//    systick_delay_ms(STM0,25);

	//等待所有核心初始化完毕
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    while (TRUE)
    {
        if(mt9v03x_finish_flag==1)   //采集一次 4.3ms   别动了 计算速度与过环正常与否有关
        {
             Out_Garage2();  //出库注意  右出库将车头往左偏移 左出库 将车头往右偏移 呵呵呵
             //Element_Zebra();
             image_process1();   //OTSU求阈值 二值化图像  整体滤波
//             Pixle_Show();   //适合显示
             seek_line();    //找边线
             Element();
//             lcd_displayimage032(Pixle[0],MT9V03X_W2,MT9V03X_H2);
//             lcd_displayimage032(use_image[0],MT9V03X_W2,MT9V03X_H2);
//            lcd_displayimage032(mt9v03x_image[0],MT9V03X_W2,MT9V03X_H2);
             Show_data();
             Element_show();
             SWITCH();
             mt9v03x_finish_flag=0;  //图像使用完毕后清零
//             lcd_clear(WHITE);
        }
    }
}

#pragma section all restore
