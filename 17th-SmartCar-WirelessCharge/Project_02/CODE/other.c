#include "headfile.h"



#define SWITCH1 P11_9
#define SWITCH2 P11_6
#define SWITCH3 P11_3
#define SWITCH4 P11_2
#define SWITCH5 P13_3
#define SWITCH6 P13_2





uint8 Ring_Flag;

uint8 switch1=1,switch2=1,switch3=1,switch4=1,switch5=1,switch6=1;



void Beep_init(void)
{
    gpio_init(P00_12, GPO, 0, PUSHPULL);
}

void Beep_Ring(int dir)
{
//    if(dir==1)  //长报告 500 500
//    {
//        gpio_set(P00_12,1);
//        systick_delay_ms(STM0,500);
//        gpio_set(P00_12,0);
//    }
//    if(dir==2)  //急促报告 100 100
//    {
//        gpio_set(P00_12,1);
//        systick_delay_ms(STM0,100);
//        gpio_set(P00_12,0);
//    }
//    if(dir==3)   //丢线提示等 100 1000
//    {
//        gpio_set(P00_12,1);
//        systick_delay_ms(STM0,100);
//        gpio_set(P00_12,0);
//        systick_delay_ms(STM0,1000);
//    }
//    systick_delay_ms(STM0,200);

    if(Element_Processing_Flag==1)
    {
        gpio_set(P00_12,1);
    }
    else
    {
        gpio_set(P00_12,0);
    }
//


//    if(Element_Processing_Flag==0)
//    {
//        gpio_set(P00_12,0);
//    }
}


void SWITCH_init(void)
{
    gpio_init(SWITCH1, GPO, 1, PUSHPULL);
    gpio_init(SWITCH2, GPO, 1, PUSHPULL);
    gpio_init(SWITCH3, GPO, 1, PUSHPULL);
    gpio_init(SWITCH4, GPO, 1, PUSHPULL);
    gpio_init(SWITCH5, GPO, 1, PUSHPULL);
    gpio_init(SWITCH6, GPO, 1, PUSHPULL);


}
//only单个闭合
void SWITCH(void)
{
    switch1=gpio_get(SWITCH1);
    switch2=gpio_get(SWITCH2);
    switch3=gpio_get(SWITCH3);
    switch4=gpio_get(SWITCH4);
    switch5=gpio_get(SWITCH5);
    switch6=gpio_get(SWITCH6);

     if(switch1==0&&switch2==1&&switch3==1&&switch4==1&&switch5==1)
     {
         MODE_SELECT=1;
         lcd_showstr(0, 0, "S1");
     }

     if(switch1==1&&switch2==0&&switch3==1&&switch4==1&&switch5==1)
     {
         MODE_SELECT=2;
         lcd_showstr(0, 0, "S2");
     }

     if(switch1==1&&switch2==1&&switch3==0&&switch4==1&&switch5==1)
     {
         MODE_SELECT=3;
         lcd_showstr(0, 0, "S3");
     }

     if(switch1==1&&switch2==1&&switch3==1&&switch4==0&&switch5==1)
     {
         MODE_SELECT=4;
         lcd_showstr(0, 0, "S4");
     }
     if(switch6==0)
     {
         Back_Cnt=0;//关闭P字检测
         lcd_showstr(20, 0, "CLOSE P");

     }
}


void Show_data(void)
{
    lcd_showfloat(0,7,BatVolt,2,2);    //电容电压 12vmax
    lcd_showfloat(80,7,TestVolt,4,1);  //检测板电压
    lcd_showfloat(0,6,angle_yaw,4,1);  //陀螺仪角度


//    lcd_showfloat(0,4,Back_Cross_Flag,1,0);

    lcd_showfloat(80,1,Zuo_Correct_Flag,1,0);
    lcd_showfloat(100,1,You_Correct_Flag,1,0);
    lcd_showfloat(120,1,Round_Statue,1,0);

    lcd_showfloat(0,1,Stop_Flag,1,0);


//    lcd_showfloat(0,1,ImageSide[90][0],3,0);
//    lcd_showfloat(0,2,ImageSide[100][0],3,0);
//    lcd_showfloat(0,3,ImageSide[110][0],3,0);
//
//    lcd_showfloat(50,1,ImageSide[90][1],3,0);
//    lcd_showfloat(50,2,ImageSide[100][1],3,0);
//    lcd_showfloat(50,3,ImageSide[110][1],3,0);

//    lcd_showfloat(50,6,S_zuo_duty,1,3);
//    lcd_showfloat(90,6,S_you_duty,1,3);
//        lcd_showfloat(90,6,Out_Garage_Flag,1,0);

//    lcd_showfloat(50,3,zuo_real_speed,3,1);
//    lcd_showfloat(90,3,you_real_speed,3,1);


}


