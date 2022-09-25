#include "headfile.h"
//转向环PID参数
float dir_kp,dir_kd;
float dir_err,dir_last_err,mid_err,side_err;
float just_mid_err;
float head_err;  //中线偏差 侧边线偏差  最高线偏差 用于提前判断元素
float dir_err_integration;
//转向环输出速度差
float Speed_dif;   //左右电机PWM 以及转向速度差

//角度环PID参数
float ang_kp,ang_kd;
float ang_err,ang_last_err;  //中线偏差 侧边线偏差  最高线偏差 用于提前判断元素
//转向环输出速度差
float ang_speed_dif;   //左右电机PWM 以及转向速度差

sint32 zuo_set_speed;   //真实的 通过设置编码输出值来控制速度
sint32 you_set_speed;

//左速度用
sint32 zuo_real_speed;  //编码器获取速度
double S_zuo_duty=0;   //程序设定速度     //测试 0.5 284左右(空载时)

//右速度用
sint32 you_real_speed;
double S_you_duty=0;

//速度环 v2.0
sint32 real_speed,real_speed_last;  //输出误差  set_speed动态调整

double set_duty=0;
double speed_out=0,  speed_out_increment;
//速度环PID参数
int    speed_err,  speed_err1, speed_err2;
double speed_kp,  speed_ki;


//赛道初步跑完标志位
uint8 Small_Turn_Flag=0; //小弯 直弯 标志位
uint8 Big_Turn_Flag=0;
uint8 Direct_Flag=0;

uint8 Mid_aim=78;

//中线获取方式选择 0为左右正常 1为左 2为右  3为下左 4为下右 5直接开摆 车库或者停车用到  停车时车发抖的原因？
uint8 Mid_Get_Type=0;

void Mid_Get(void)
{
    switch (Mid_Get_Type)
    {
        case 0:  //中线
        {
            mid_err = (Mid_aim-ImageMid[40])*0.33+(Mid_aim-ImageMid[60])*0.33+(Mid_aim-ImageMid[80])*0.33; //中线的偏差  前瞻有1m远?   后面换斜率代替误差跑
            break;
        }
        case 1:  //左边线获取中线  左边线加上某值
        {
            mid_err = (Mid_aim-(ImageSide[40][0]+32))*0.33+(Mid_aim-(ImageSide[60][0]+38))*0.33+(Mid_aim-(ImageSide[80][0]+46))*0.33;
            break;
        }
        case 2: //右边线获取中线  右边线减去某值  115 124 133
        {
            mid_err = (Mid_aim-(ImageSide[40][1]-35))*0.33+(Mid_aim-(ImageSide[60][1]-44))*0.33+(Mid_aim-(ImageSide[80][1]-53))*0.33;   //都统一减去了5+3 也可能是摄像头歪了导致左环岛那么抽象 左转很严重
            break;
        }

        case 3: //左边线获取中线  左边线加上某值   靠下  33 28 25
        {
            mid_err = (Mid_aim-(ImageSide[90][0]+47))*0.33+(Mid_aim-(ImageSide[100][0]+52))*0.33+(Mid_aim-(ImageSide[110][0]+55))*0.33;
            break;
        }
        case 4: //右边线获取中线  右边线减去某值   靠下  134 138 142
        {
            mid_err = (Mid_aim-(ImageSide[90][1]-54))*0.33+(Mid_aim-(ImageSide[100][1]-58))*0.33+(Mid_aim-(ImageSide[110][1]-62))*0.33;
            break;
        }

        case 5: //直接开摆 不进行转向环循迹 转向环输出为0  应该是走直线
        {
            mid_err=0;
        }
    }
}





/*
     根据现在的误差动态调整位置环的PD参数

     位置环分段式PID
     加入了速度动态控制

     1.直道入弯道 需要减速 2.弯道入直道 需要加速
    只是动态调整转向环的PD参数

(1)当e较大时，为使系统具有较好的跟踪性能，应取较大的Kp 与较小的Kd，同时为避免系统响应出现较大的超调，应对积分作用加以限制，通常取Ki=0。
(2)当e处于中等大小时，为使系统响应具有较小的超调，Kp应 取得小些。在这种情况下，Kd的取值对系统响应的影响较大，Ki的取值要适当。
(3)当e较小时，为使系统具有较好的稳定性能，Kp与Ki均应 取得大些，同时为避免系统在设定值附近出现振荡，Kd值的选择根据|ec|值较大时，Kd取较小值，通常Kd为中等大小。

 */
void Position_PD_control(int dir)
{
if(dir==1)   //测试速度
{
    if(abs(dir_err)<8&&abs(dir_last_err)<8)
    {
        Big_Turn_Flag=0;
        Small_Turn_Flag=0;
        Direct_Flag=1;
    }
    if(abs(dir_err)>=8&&abs(dir_last_err)>=8)
    {
        Direct_Flag=0;
        Big_Turn_Flag=0;
        Small_Turn_Flag=1;
    }
    if(abs(dir_err)>=12&&abs(dir_last_err)>=12)
    {
        Direct_Flag=0;
        Small_Turn_Flag=0;
        Big_Turn_Flag=1;
    }


   // 60 40 35 很稳    70 60 40 转不过来弯 小细节 小直道致命
    if(Direct_Flag==1)  //直道
    {
        Target_speed=60;
        dir_kp=0.00190;
        dir_kd=0.03000;
    }
    if(Small_Turn_Flag==1)  //小弯道
    {
        Target_speed=60;
        dir_kp = 0.00200;
        dir_kd = 0.04000;
    }
    if(Big_Turn_Flag==1)  //大弯道
    {
        Target_speed=65;
        dir_kp = 0.00250;
        dir_kd = 0.05000;
    }
    //环岛专用速度参数
    if(Round_Road_Flag!=0)
    {
        Target_speed=58;
        dir_kp = 0.00300;
        dir_kd = 0.06000;
    }
}


        if(dir==2)  //比赛用慢速 稳定速度
        {
            if(abs(head_err)<=10&&abs(dir_err)<6&&abs(dir_last_err)<6)  //直道
             {
                 Big_Turn_Flag=0;
                 Small_Turn_Flag=0;
                 Direct_Flag=1;
             }
         //    else(head_err>=10&&dir_err>=6&&dir_last_err>=6) //小弯 直角   检测到头偏差大于10
             else  //非直线即小弯道
             {
                 Direct_Flag=0;
                 Big_Turn_Flag=0;
                 Small_Turn_Flag=1;
             }
             if(abs(dir_err)>=10&&abs(dir_last_err)>=10) //大弯
             {
                 Direct_Flag=0;
                 Small_Turn_Flag=0;
                 Big_Turn_Flag=1;
             }

             // 60 40 35 很稳    70 60 40 转不过来弯 小细节 小直道致命
              if(Direct_Flag==1)  //直道
              {
                  Target_speed=75;
                  dir_kp=0.002200;
                  dir_kd=0.05000;
              }
              if(Small_Turn_Flag==1)  //小弯道
              {
                  Target_speed=65;
                  dir_kp = 0.00350;
                  dir_kd = 0.06000;
              }
              if(Big_Turn_Flag==1)  //大弯道
              {
                  Target_speed=60;
                  dir_kp = 0.00350;
                  dir_kd = 0.06000;
              }
              //环岛专用速度参数
              if(Round_Road_Flag!=0)
              {
                  Target_speed=60;
                  dir_kp = 0.00280;
                  dir_kd = 0.06000;
              }
        }

        if(dir==3)  //比赛用正常速度  50秒左右
        {
            if(abs(head_err)<=10&&abs(dir_err)<6&&abs(dir_last_err)<6)  //直道
            {
                Big_Turn_Flag=0;
                Small_Turn_Flag=0;
                Direct_Flag=1;
            }
        //    else(head_err>=10&&dir_err>=6&&dir_last_err>=6) //小弯 直角   检测到头偏差大于10
            else  //非直线即小弯道
            {
                Direct_Flag=0;
                Big_Turn_Flag=0;
                Small_Turn_Flag=1;
            }
            if(abs(dir_err)>=10&&abs(dir_last_err)>=10) //大弯
            {
                Direct_Flag=0;
                Small_Turn_Flag=0;
                Big_Turn_Flag=1;
            }

            // 60 40 35 很稳    70 60 40 转不过来弯 小细节 小直道致命
             if(Direct_Flag==1)  //直道
             {
                 Target_speed=75;
                 dir_kp=0.002800;
                 dir_kd=0.05000;
             }
             if(Small_Turn_Flag==1)  //小弯道
             {
                 Target_speed=75;
                 dir_kp = 0.00400;
                 dir_kd = 0.06000;
             }
             if(Big_Turn_Flag==1)  //大弯道
             {
                 Target_speed=62;
                 dir_kp = 0.00400;
                 dir_kd = 0.06000;
             }
             //环岛专用速度参数
             if(Round_Road_Flag!=0)
             {
                 Target_speed=60;
                 dir_kp = 0.00300;
                 dir_kd = 0.06000;
             }
        }


        if(dir==4)  //比赛用正常速度  50秒左右
        {
            if(abs(head_err)<=10&&abs(dir_err)<6&&abs(dir_last_err)<6)  //直道
            {
                Big_Turn_Flag=0;
                Small_Turn_Flag=0;
                Direct_Flag=1;
            }
        //    else(head_err>=10&&dir_err>=6&&dir_last_err>=6) //小弯 直角   检测到头偏差大于10
            else  //非直线即小弯道
            {
                Direct_Flag=0;
                Big_Turn_Flag=0;
                Small_Turn_Flag=1;
            }
            if(abs(dir_err)>=10&&abs(dir_last_err)>=10) //大弯
            {
                Direct_Flag=0;
                Small_Turn_Flag=0;
                Big_Turn_Flag=1;
            }

            // 60 40 35 很稳    70 60 40 转不过来弯 小细节 小直道致命
             if(Direct_Flag==1)  //直道
             {
                 Target_speed=78;
                 dir_kp=0.002800;
                 dir_kd=0.05000;
             }
             if(Small_Turn_Flag==1)  //小弯道
             {
                 Target_speed=75;
                 dir_kp = 0.00420;
                 dir_kd = 0.06000;
             }
             if(Big_Turn_Flag==1)  //大弯道
             {
                 Target_speed=60;
                 dir_kp = 0.00400;
                 dir_kd = 0.06000;
             }
             //环岛专用速度参数
             if(Round_Road_Flag!=0)
             {
                 Target_speed=70;
                 dir_kp = 0.00350;
                 dir_kd = 0.06000;
             }
        }


if(Out_Garage_Flag==1)
{
    dir_kp = 0.00250;
    dir_kd = 0.04000;
}
if(Zebra_Flag==2)
{
//    Target_speed=45;            /////30   润滑油 ///
    dir_kp = 0.00400;   //三档600??
    dir_kd = 0.09000;
}
    //S弯专用速度参数

    //十字回环专用速度参数

    //180度弯用速度参数
}




/*
 * 位置环闭环控制   位置式PD控制   参数 ERROR  使用前确定ERR  转向环
 *  D的主流作用是抑制震荡，当然这有一个前提，就是D的符号给对了
 */
double Position_control(void)  //将P和D与速度和偏差相关联
{
    just_mid_err  = (Mid_aim-ImageMid[40])*0.33+(Mid_aim-ImageMid[60])*0.33+(Mid_aim-ImageMid[80])*0.33; //中线的偏差

    Mid_Get();
    head_err = (78-ImageMid[1])*0.25+(78-ImageMid[2])*0.25+(78-ImageMid[3])*0.25+(78-ImageMid[4])*0.25;
    side_err = 0;  //边线偏差

    dir_err = mid_err;
    //printf("ERR : %lf",mid_err);
    //横向偏差

    dir_err = dir_err*0.900+dir_last_err*0.100;  //误差低通滤波 防止中线突变

//    if(dir_err>0)
//    {
//        dir_err = dir_err*0.4;
//    }

    Speed_dif = dir_kp*dir_err + dir_kd*(dir_err-dir_last_err);
    //printf("DIF : %lf",Speed_dif);
    dir_last_err=dir_err;

    if(Speed_dif>=0.85)Speed_dif=0.85;     //输出限幅
    if(Speed_dif<=-0.85)Speed_dif=-0.85;

    return Speed_dif;
}

/*
 * 速度环 阶段控制 根据实际速度调节速度环强度
 * Target_speed 与real_speed
 *
 * 0.0025MAXXXX   0.0008T MINNNN
 */
void Incremental_PI_control(void)
{
switch(MODE_SELECT)
{
    case 1:
    {
        Judge_Stright_Amount=18;
        if(TestVolt>=800)
        {
            speed_kp=0.0060;
            speed_ki=0.0020;
        }else if(TestVolt<=800)
        {
            speed_kp=0.0060;
            speed_ki=0.0006;
        }

        if(Charge_Flag==1)
        {
            speed_kp=0.0060;
            speed_ki=0.0006;
        }
        if(Round_Statue>=3)  //环岛速度处理 单挡位最高优先级
        {
//            Charge_point=7.00;   //遇到环岛把阈值拉到8.00v  开机10v使入三岔充电
            Sit_Flag=0;
            speed_kp=0.0060;
            speed_ki=0.00015;
        }

//        if(Round_Statue==5&&Round_Cnt==2)//环岛速度处理 单挡位最高优先级
//        {
//            Target_speed=60;   //90
//        }
//        if(Round_Statue==5&&Round_Cnt==0)//环岛速度处理 单挡位最高优先级
//        {
//            Target_speed=60;   //90
//        }

        break;
    }
    case 2:
    {
        Judge_Stright_Amount=20;
        if(TestVolt>=1000)
        {
//            Target_speed=50;
            speed_kp=0.0060;
            speed_ki=0.0020;
        }else if(TestVolt<=1000)
        {
            speed_kp=0.0060;
            speed_ki=0.0006;
        }

        if(Charge_Flag==1)
        {
            speed_kp=0.0060;
            speed_ki=0.0004;
        }
  //入环岛充电 出环岛不充电 入环岛不改 出环岛把改充电阈值降低
        if(Round_Statue>=3)//环岛速度处理 单挡位最高优先级
        {
            Sit_Flag=0;
            speed_kp=0.0060;
            speed_ki=0.0004;
        }

//        if(Round_Statue==5&&Round_Cnt==2)//环岛速度处理 单挡位最高优先级
//        {
//            Target_speed=75;   //90
//        }
//        if(Round_Statue==5&&Round_Cnt==0)//环岛速度处理 单挡位最高优先级
//        {
//            Target_speed=75;   //90
//        }

        break;
    }
    case 3:
    {
        Judge_Stright_Amount=20;
        if(TestVolt>=800)
        {
            speed_kp=0.0060;
            speed_ki=0.0023;
        }else if(TestVolt<=800)
        {
            speed_kp=0.0060;
            speed_ki=0.0003;
        }
        if(Charge_Flag==1)
        {
            speed_kp=0.0060;
            speed_ki=0.0003;
        }
        if(Round_Statue>=3)//环岛速度处理 单挡位最高优先级
        {
            speed_kp=0.0060;
            speed_ki=0.0003;
        }
//        if(Round_Statue==5&&Round_Cnt==2)//环岛速度处理 单挡位最高优先级
//        {
//            Target_speed=80;   //90
//        }
//        if(Round_Statue==5&&Round_Cnt==0)//环岛速度处理 单挡位最高优先级
//        {
//            Target_speed=80;   //90
//        }
        break;
    }
    case 4:
    {
        Judge_Stright_Amount=20;
        if(TestVolt>=800)
        {
            speed_kp=0.0060;
            speed_ki=0.0020;
        }else if(TestVolt<=800)
        {
            speed_kp=0.0060;
            speed_ki=0.0003;
        }
        if(Charge_Flag==1)
        {
            speed_kp=0.0060;
            speed_ki=0.0003;
        }
        if(Round_Statue>=3)//环岛速度处理 单挡位最高优先级
        {
            speed_kp=0.0060;
            speed_ki=0.0003;
        }
        //出环岛加速

        if(Round_Statue==5&&Round_Cnt==2)//环岛速度处理 单挡位最高优先级
        {
            Target_speed=75;   //90
        }
        if(Round_Statue==5&&Round_Cnt==0)//环岛速度处理 单挡位最高优先级
        {
            Target_speed=75;   //90
        }
}


//特殊元素速度处理 最高优先级
        if(Out_Garage_Flag==1)
        {
            Target_speed = 35;//软出库  30 为最佳速度
            speed_kp=0.0060;
            speed_ki=0.0015;
        }
        if(Zebra_Flag==2)
        {
            Target_speed = 40;//软出库  30 为最佳速度
            speed_kp=0.0060;
            speed_ki=0.0015;
        }
}
}

//挡位速度调整
//void Speed_Target_Control(void)
//{
//    switch(MODE_SELECT)
//    {
//        case 1:
//        {
//            Target_speed=58;
//            //环岛专用速度参数
//            if(Round_Road_Flag!=0)
//            {
//                Target_speed=50;
//            }
//            break;
//        }
//        case 2:
//        {
//            Target_speed=70;
//            if(TestVolt>=1000&&BatVolt>=10.80)
//            {
//                Target_speed=50;
//            }
//            //环岛专用速度参数
//            if(Round_Road_Flag!=0)
//            {
//                Target_speed=60;
//            }
//            break;
//        }
//        case 3:
//        {
//            Target_speed=80;
//            if(TestVolt>=1000&&BatVolt>=10.80)
//            {
//                Target_speed=60;
//            }
//            //环岛专用速度参数
//            if(Round_Road_Flag!=0)
//            {
//                Target_speed=70;
//            }
//            break;
//        }
//        case 4:
//        {
//            Target_speed=40;
//            if(TestVolt>=1000&&BatVolt>=10.80)
//            {
//                Target_speed=30;
//            }
//            //环岛专用速度参数
//            if(Round_Road_Flag!=0)
//            {
//                Target_speed=30;
//            }
//        }
//    }
//}

/*
 *速度环  2.0
 *通过左右编码器数值反应真实速度
 *加减速度通过控制左右电机PWM共同加减
 *上电容把速度环以及转向环调软  后期实现动态速度环参数 因为充电时候会冲出去...刚开机难以控制
 */
double Speed_Control(void)
{
//      speed_kp=0.0030;
//      speed_ki=0.0030;  //速度环很稳 单比例控制  软取0.0001


       zuo_real_speed = zuo_real_speed_add/8;//速度平均滤波，分母为控制周期对于采集周期的倍数
       you_real_speed = you_real_speed_add/8;

       you_real_speed_add = 0;//积分归零
       zuo_real_speed_add = 0;

       real_speed_last = real_speed;
       real_speed = (sint32)((zuo_real_speed+you_real_speed)/2);//左右轮的平均值为车轴中心速度，既车速
       real_speed = 0.9*real_speed + 0.1*real_speed_last;//低通滤波
       //printf("REAL SPEED = %d ", real_speed);


      speed_err2 = speed_err1;
      speed_err1 = speed_err;
      speed_err = Target_speed - real_speed;

//      Incremental_PI_control();


      speed_out_increment=
                              speed_kp*(speed_err-speed_err1)
                              +speed_ki*speed_err;
//                              +speed_ki*(speed_err-2*speed_err2+speed_err2);

      speed_out += speed_out_increment;

    if(speed_out>=0.85)speed_out=0.85;
    if(speed_out<=-0.85)speed_out=-0.85;
    //printf("SPEED OUT = %lf ", speed_out);
    return speed_out;
}


///*
// * 陀螺仪 角度环控制
// * 角度环开 转向环关
// * 位置式PD控制
// *
// * 下一步 不用这玩意了
//
// */
//double Angle_control(void)
//{
//    ang_kp=0.0001;  //反应不够快  根据需要再加大   0.001  0.150
//    ang_kd=0.0015;
//
//    ang_err = Target_Z_angle - angle_yaw;
//    /* 误差较小时取消误差(积分死区) */
//    if(abs(ang_err)<=10)
//    {
//        ang_err=0;
//    }
//
//    ang_last_err=ang_err;
//
//    ang_speed_dif = ang_kp*ang_err + ang_kd*(ang_err-ang_last_err);
//
//    if(ang_speed_dif>=0.5)ang_speed_dif=0.5;     //角度环限幅输出  限制最大改变800
//    if(ang_speed_dif<=-0.5)ang_speed_dif=-0.5;
//
//    return ang_speed_dif;
//}
