#include "headfile.h"
#pragma section all "cpu1_dsram"
//���������#pragma section all restore���֮���ȫ�ֱ���������CPU1��RAM��
void core1_main(void)
{
	disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    //�û��ڴ˴����ø��ֳ�ʼ��������

    lcd_init(); //��ʼ��IPS��Ļ
    systick_delay_ms(STM0,2);

    lcd_display_chinese(30,50,16,chinese_test[0],6,RED);

    systick_delay_ms(STM0,20);
    lcd_clear(WHITE);
    Beep_init();
    systick_delay_ms(STM0,2);
    Beep_Ring(1);

//    seekfree_wireless_init();
//    systick_delay_ms(STM0,25);

	//�ȴ����к��ĳ�ʼ�����
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    while (TRUE)
    {
        if(mt9v03x_finish_flag==1)   //�ɼ�һ�� 4.3ms   ���� �����ٶ��������������й�
        {
             Out_Garage2();  //����ע��  �ҳ��⽫��ͷ����ƫ�� ����� ����ͷ����ƫ�� �ǺǺ�
             //Element_Zebra();
             image_process1();   //OTSU����ֵ ��ֵ��ͼ��  �����˲�
//             Pixle_Show();   //�ʺ���ʾ
             seek_line();    //�ұ���
             Element();
//             lcd_displayimage032(Pixle[0],MT9V03X_W2,MT9V03X_H2);
//             lcd_displayimage032(use_image[0],MT9V03X_W2,MT9V03X_H2);
//            lcd_displayimage032(mt9v03x_image[0],MT9V03X_W2,MT9V03X_H2);
             Show_data();
             Element_show();
             SWITCH();
             mt9v03x_finish_flag=0;  //ͼ��ʹ����Ϻ�����
//             lcd_clear(WHITE);
        }
    }
}

#pragma section all restore
