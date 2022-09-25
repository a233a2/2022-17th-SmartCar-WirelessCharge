#include"headfile.h"

//电容电压 实时
float BatVolt;

//检测板电压值
float TestVolt;

uint8 Max_Pass_Flag=0;  //过监测最高点标志位

uint8 Charge_Flag=0; //充电标志位 判断是否该停车充电的标志位 决定条件为当前电量

uint8 Stop_Flag=1;   //停车标志位 标志位置1时候关闭转向环的速度调节  开机置1

sint32 Min_Stop_Point=1750; //停车阈值 这个值一般随着速度变化而变化 应该也与目前电量有点关系

uint8 Charge_Finish_Flag=0;  //充电完成后标志位 Stop_Flag=1 即停车时置0 充满置1


uint8 Bat_ABS=0;  //电容电压截至挡位 5个挡位

float Charge_Top=12.50;  //充电峰值电压

float Charge_point=8.90;

uint8 Speed_Gear=0;  //根据real_speed所得的速度档

//平时置1 充满电置0 代表不再sit
uint8 Sit_Flag=0;//充电停车标志位 因为分段减速停车  在到达MaxTestVlot点时为了防止不往前行驶错过充电而设置此标志位 速度锁定限制标志位
//此标志位就起一个充满电不再执行停车速度控制程序的功能


//速度分级函数
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
    if(BatVolt<=6.00)     //6v以下 停车提前停
    {
        Min_Stop_Point=1800;
    }
    if(BatVolt>6.00&&BatVolt<=7.00)   //6-7.5 较为提前
    {
        Min_Stop_Point=1950;
    }
    if(BatVolt>7.00)  //7.50 以上 晚点
    {
        Min_Stop_Point=2100;
    }
}

/*
 * 电容电压初始化
 */
void ADC_Init(void)
{
    adc_init(ADC_1, ADC1_CH9_A25);//V_out_adc      检测电压
    adc_init(ADC_2, ADC2_CH3_A35);//V_out_adc    电容电压BatVolt
}





//停车速度控制程序
//进入条件1为 Charge_Flag=0 即电量下降到9.00伏
//进入条件2为 Sit_Flag=0
void Stop_Test_Process(void)
{
    switch (MODE_SELECT)   //速度控制程序 根据速度挡位分段处理
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
 * 停车充电程序
 * 下一代调节为PID控制
 * 停车 充电思路为 先进行减速再进行速度置0停车
 * 在电量达到标准后
 */
void Charge_stop(void)//停一次接着就冲出去了 停车效果不好
{
//    Speed_Sort_Gear();
    Stop_Bat_Gear();
//    Stop_Test_Process();
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=Min_Stop_Point&&TestVolt<=3600)
        {
            Sit_Flag=1;  //不再执行这些语句 速度坐死
            Stop_Flag=1;   //正在停车中 满足停车条件 当stop_flag=1 转向环节的速度调节关闭
        }
    }

    if(Sit_Flag==1)  //速度控制最高优先级
    {
        Target_speed=0;  //继续速度赋值0 直到充满电
    }

        if(BatVolt>=Charge_Top)
        {
            Stop_Flag=0;
            Sit_Flag=0;             //解除Sit_Flag限制  此时当电量下到 规定  以下时 进行停车检测程序  sit_flag作用就是不充满电不然跑
            Charge_Flag=1;         //已完成充电标志位  该判断值为最大充电量
        }

//        if(Round_Statue>=3)
//        {
//            Charge_point=6.00;        //拉低 出环岛不充电
//        }else if(Round_Statue<3)
//        {
//            Charge_point=9.00;          //该充电了  保证他出环岛不充电而入环岛充电
//        }

        if(BatVolt<=Charge_point)Charge_Flag=0;

}
//bug 充电标志位在移除充电点时候不清除 直到12.00伏才清除

//速度1 停车动态阈值
void Stop_speed1(void)
{
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=2000&&TestVolt<=3500)
        {
            Sit_Flag=1;  //不再执行这些语句 速度坐死
            Stop_Flag=1;   //正在停车中 满足停车条件 当stop_flag=1 转向环节的速度调节关闭
        }
    }
}

//速度2 停车动态阈值 基本很稳
void Stop_speed2(void)
{
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=2100&&TestVolt<=3500)
        {
            Sit_Flag=1;  //不再执行这些语句 速度坐死
            Stop_Flag=1;   //正在停车中 满足停车条件 当stop_flag=1 转向环节的速度调节关闭
        }
//        else Stop_Flag=0;
    }
}


//速度3 停车动态阈值
void Stop_speed3(void)
{
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=1800&&TestVolt<=3500)
        {
            Sit_Flag=1;  //不再执行这些语句 速度坐死
            Stop_Flag=1;   //正在停车中 满足停车条件 当stop_flag=1 转向环节的速度调节关闭
        }
//        else Stop_Flag=0;
    }
}
//速度3 停车动态阈值
void Stop_speed4(void)
{
    if(Charge_Flag==0&&Sit_Flag==0)
    {
        if(TestVolt>=1700&&TestVolt<=3000)
        {
            Sit_Flag=1;  //不再执行这些语句 速度坐死
//            Target_speed=0;  //减速准备停车
            Stop_Flag=1;   //正在停车中 满足停车条件 当stop_flag=1 转向环节的速度调节关闭
        }
//        else Stop_Flag=0;
    }
}

//停车停好至少提升5s左右!

//停车新
//停车根据电量和速度来确定 我认为应该主要根据电量来决定
//电量高时 停车应延后 2100
//电量低时 停车应提前 1700


//充电处
// 出库   第一个三岔口   右环岛  左环岛  三岔出口  5处充电
 //     剩余电量8.00v左右   6.5v    5.0v    6.50v
