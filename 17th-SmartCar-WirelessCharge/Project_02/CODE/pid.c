#include "headfile.h"
//ת��PID����
float dir_kp,dir_kd;
float dir_err,dir_last_err,mid_err,side_err;
float just_mid_err;
float head_err;  //����ƫ�� �����ƫ��  �����ƫ�� ������ǰ�ж�Ԫ��
float dir_err_integration;
//ת������ٶȲ�
float Speed_dif;   //���ҵ��PWM �Լ�ת���ٶȲ�

//�ǶȻ�PID����
float ang_kp,ang_kd;
float ang_err,ang_last_err;  //����ƫ�� �����ƫ��  �����ƫ�� ������ǰ�ж�Ԫ��
//ת������ٶȲ�
float ang_speed_dif;   //���ҵ��PWM �Լ�ת���ٶȲ�

sint32 zuo_set_speed;   //��ʵ�� ͨ�����ñ������ֵ�������ٶ�
sint32 you_set_speed;

//���ٶ���
sint32 zuo_real_speed;  //��������ȡ�ٶ�
double S_zuo_duty=0;   //�����趨�ٶ�     //���� 0.5 284����(����ʱ)

//���ٶ���
sint32 you_real_speed;
double S_you_duty=0;

//�ٶȻ� v2.0
sint32 real_speed,real_speed_last;  //������  set_speed��̬����

double set_duty=0;
double speed_out=0,  speed_out_increment;
//�ٶȻ�PID����
int    speed_err,  speed_err1, speed_err2;
double speed_kp,  speed_ki;


//�������������־λ
uint8 Small_Turn_Flag=0; //С�� ֱ�� ��־λ
uint8 Big_Turn_Flag=0;
uint8 Direct_Flag=0;

uint8 Mid_aim=78;

//���߻�ȡ��ʽѡ�� 0Ϊ�������� 1Ϊ�� 2Ϊ��  3Ϊ���� 4Ϊ���� 5ֱ�ӿ��� �������ͣ���õ�  ͣ��ʱ��������ԭ��
uint8 Mid_Get_Type=0;

void Mid_Get(void)
{
    switch (Mid_Get_Type)
    {
        case 0:  //����
        {
            mid_err = (Mid_aim-ImageMid[40])*0.33+(Mid_aim-ImageMid[60])*0.33+(Mid_aim-ImageMid[80])*0.33; //���ߵ�ƫ��  ǰհ��1mԶ?   ���滻б�ʴ��������
            break;
        }
        case 1:  //����߻�ȡ����  ����߼���ĳֵ
        {
            mid_err = (Mid_aim-(ImageSide[40][0]+32))*0.33+(Mid_aim-(ImageSide[60][0]+38))*0.33+(Mid_aim-(ImageSide[80][0]+46))*0.33;
            break;
        }
        case 2: //�ұ��߻�ȡ����  �ұ��߼�ȥĳֵ  115 124 133
        {
            mid_err = (Mid_aim-(ImageSide[40][1]-35))*0.33+(Mid_aim-(ImageSide[60][1]-44))*0.33+(Mid_aim-(ImageSide[80][1]-53))*0.33;   //��ͳһ��ȥ��5+3 Ҳ����������ͷ���˵����󻷵���ô���� ��ת������
            break;
        }

        case 3: //����߻�ȡ����  ����߼���ĳֵ   ����  33 28 25
        {
            mid_err = (Mid_aim-(ImageSide[90][0]+47))*0.33+(Mid_aim-(ImageSide[100][0]+52))*0.33+(Mid_aim-(ImageSide[110][0]+55))*0.33;
            break;
        }
        case 4: //�ұ��߻�ȡ����  �ұ��߼�ȥĳֵ   ����  134 138 142
        {
            mid_err = (Mid_aim-(ImageSide[90][1]-54))*0.33+(Mid_aim-(ImageSide[100][1]-58))*0.33+(Mid_aim-(ImageSide[110][1]-62))*0.33;
            break;
        }

        case 5: //ֱ�ӿ��� ������ת��ѭ�� ת�����Ϊ0  Ӧ������ֱ��
        {
            mid_err=0;
        }
    }
}





/*
     �������ڵ���̬����λ�û���PD����

     λ�û��ֶ�ʽPID
     �������ٶȶ�̬����

     1.ֱ������� ��Ҫ���� 2.�����ֱ�� ��Ҫ����
    ֻ�Ƕ�̬����ת�򻷵�PD����

(1)��e�ϴ�ʱ��Ϊʹϵͳ���нϺõĸ������ܣ�Ӧȡ�ϴ��Kp ���С��Kd��ͬʱΪ����ϵͳ��Ӧ���ֽϴ�ĳ�����Ӧ�Ի������ü������ƣ�ͨ��ȡKi=0��
(2)��e�����еȴ�Сʱ��Ϊʹϵͳ��Ӧ���н�С�ĳ�����KpӦ ȡ��СЩ������������£�Kd��ȡֵ��ϵͳ��Ӧ��Ӱ��ϴ�Ki��ȡֵҪ�ʵ���
(3)��e��Сʱ��Ϊʹϵͳ���нϺõ��ȶ����ܣ�Kp��Ki��Ӧ ȡ�ô�Щ��ͬʱΪ����ϵͳ���趨ֵ���������񵴣�Kdֵ��ѡ�����|ec|ֵ�ϴ�ʱ��Kdȡ��Сֵ��ͨ��KdΪ�еȴ�С��

 */
void Position_PD_control(int dir)
{
if(dir==1)   //�����ٶ�
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


   // 60 40 35 ����    70 60 40 ת�������� Сϸ�� Сֱ������
    if(Direct_Flag==1)  //ֱ��
    {
        Target_speed=60;
        dir_kp=0.00190;
        dir_kd=0.03000;
    }
    if(Small_Turn_Flag==1)  //С���
    {
        Target_speed=60;
        dir_kp = 0.00200;
        dir_kd = 0.04000;
    }
    if(Big_Turn_Flag==1)  //�����
    {
        Target_speed=65;
        dir_kp = 0.00250;
        dir_kd = 0.05000;
    }
    //����ר���ٶȲ���
    if(Round_Road_Flag!=0)
    {
        Target_speed=58;
        dir_kp = 0.00300;
        dir_kd = 0.06000;
    }
}


        if(dir==2)  //���������� �ȶ��ٶ�
        {
            if(abs(head_err)<=10&&abs(dir_err)<6&&abs(dir_last_err)<6)  //ֱ��
             {
                 Big_Turn_Flag=0;
                 Small_Turn_Flag=0;
                 Direct_Flag=1;
             }
         //    else(head_err>=10&&dir_err>=6&&dir_last_err>=6) //С�� ֱ��   ��⵽ͷƫ�����10
             else  //��ֱ�߼�С���
             {
                 Direct_Flag=0;
                 Big_Turn_Flag=0;
                 Small_Turn_Flag=1;
             }
             if(abs(dir_err)>=10&&abs(dir_last_err)>=10) //����
             {
                 Direct_Flag=0;
                 Small_Turn_Flag=0;
                 Big_Turn_Flag=1;
             }

             // 60 40 35 ����    70 60 40 ת�������� Сϸ�� Сֱ������
              if(Direct_Flag==1)  //ֱ��
              {
                  Target_speed=75;
                  dir_kp=0.002200;
                  dir_kd=0.05000;
              }
              if(Small_Turn_Flag==1)  //С���
              {
                  Target_speed=65;
                  dir_kp = 0.00350;
                  dir_kd = 0.06000;
              }
              if(Big_Turn_Flag==1)  //�����
              {
                  Target_speed=60;
                  dir_kp = 0.00350;
                  dir_kd = 0.06000;
              }
              //����ר���ٶȲ���
              if(Round_Road_Flag!=0)
              {
                  Target_speed=60;
                  dir_kp = 0.00280;
                  dir_kd = 0.06000;
              }
        }

        if(dir==3)  //�����������ٶ�  50������
        {
            if(abs(head_err)<=10&&abs(dir_err)<6&&abs(dir_last_err)<6)  //ֱ��
            {
                Big_Turn_Flag=0;
                Small_Turn_Flag=0;
                Direct_Flag=1;
            }
        //    else(head_err>=10&&dir_err>=6&&dir_last_err>=6) //С�� ֱ��   ��⵽ͷƫ�����10
            else  //��ֱ�߼�С���
            {
                Direct_Flag=0;
                Big_Turn_Flag=0;
                Small_Turn_Flag=1;
            }
            if(abs(dir_err)>=10&&abs(dir_last_err)>=10) //����
            {
                Direct_Flag=0;
                Small_Turn_Flag=0;
                Big_Turn_Flag=1;
            }

            // 60 40 35 ����    70 60 40 ת�������� Сϸ�� Сֱ������
             if(Direct_Flag==1)  //ֱ��
             {
                 Target_speed=75;
                 dir_kp=0.002800;
                 dir_kd=0.05000;
             }
             if(Small_Turn_Flag==1)  //С���
             {
                 Target_speed=75;
                 dir_kp = 0.00400;
                 dir_kd = 0.06000;
             }
             if(Big_Turn_Flag==1)  //�����
             {
                 Target_speed=62;
                 dir_kp = 0.00400;
                 dir_kd = 0.06000;
             }
             //����ר���ٶȲ���
             if(Round_Road_Flag!=0)
             {
                 Target_speed=60;
                 dir_kp = 0.00300;
                 dir_kd = 0.06000;
             }
        }


        if(dir==4)  //�����������ٶ�  50������
        {
            if(abs(head_err)<=10&&abs(dir_err)<6&&abs(dir_last_err)<6)  //ֱ��
            {
                Big_Turn_Flag=0;
                Small_Turn_Flag=0;
                Direct_Flag=1;
            }
        //    else(head_err>=10&&dir_err>=6&&dir_last_err>=6) //С�� ֱ��   ��⵽ͷƫ�����10
            else  //��ֱ�߼�С���
            {
                Direct_Flag=0;
                Big_Turn_Flag=0;
                Small_Turn_Flag=1;
            }
            if(abs(dir_err)>=10&&abs(dir_last_err)>=10) //����
            {
                Direct_Flag=0;
                Small_Turn_Flag=0;
                Big_Turn_Flag=1;
            }

            // 60 40 35 ����    70 60 40 ת�������� Сϸ�� Сֱ������
             if(Direct_Flag==1)  //ֱ��
             {
                 Target_speed=78;
                 dir_kp=0.002800;
                 dir_kd=0.05000;
             }
             if(Small_Turn_Flag==1)  //С���
             {
                 Target_speed=75;
                 dir_kp = 0.00420;
                 dir_kd = 0.06000;
             }
             if(Big_Turn_Flag==1)  //�����
             {
                 Target_speed=60;
                 dir_kp = 0.00400;
                 dir_kd = 0.06000;
             }
             //����ר���ٶȲ���
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
//    Target_speed=45;            /////30   ���� ///
    dir_kp = 0.00400;   //����600??
    dir_kd = 0.09000;
}
    //S��ר���ٶȲ���

    //ʮ�ֻػ�ר���ٶȲ���

    //180�������ٶȲ���
}




/*
 * λ�û��ջ�����   λ��ʽPD����   ���� ERROR  ʹ��ǰȷ��ERR  ת��
 *  D�����������������𵴣���Ȼ����һ��ǰ�ᣬ����D�ķ��Ÿ�����
 */
double Position_control(void)  //��P��D���ٶȺ�ƫ�������
{
    just_mid_err  = (Mid_aim-ImageMid[40])*0.33+(Mid_aim-ImageMid[60])*0.33+(Mid_aim-ImageMid[80])*0.33; //���ߵ�ƫ��

    Mid_Get();
    head_err = (78-ImageMid[1])*0.25+(78-ImageMid[2])*0.25+(78-ImageMid[3])*0.25+(78-ImageMid[4])*0.25;
    side_err = 0;  //����ƫ��

    dir_err = mid_err;
    //printf("ERR : %lf",mid_err);
    //����ƫ��

    dir_err = dir_err*0.900+dir_last_err*0.100;  //����ͨ�˲� ��ֹ����ͻ��

//    if(dir_err>0)
//    {
//        dir_err = dir_err*0.4;
//    }

    Speed_dif = dir_kp*dir_err + dir_kd*(dir_err-dir_last_err);
    //printf("DIF : %lf",Speed_dif);
    dir_last_err=dir_err;

    if(Speed_dif>=0.85)Speed_dif=0.85;     //����޷�
    if(Speed_dif<=-0.85)Speed_dif=-0.85;

    return Speed_dif;
}

/*
 * �ٶȻ� �׶ο��� ����ʵ���ٶȵ����ٶȻ�ǿ��
 * Target_speed ��real_speed
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
        if(Round_Statue>=3)  //�����ٶȴ��� ����λ������ȼ�
        {
//            Charge_point=7.00;   //������������ֵ����8.00v  ����10vʹ��������
            Sit_Flag=0;
            speed_kp=0.0060;
            speed_ki=0.00015;
        }

//        if(Round_Statue==5&&Round_Cnt==2)//�����ٶȴ��� ����λ������ȼ�
//        {
//            Target_speed=60;   //90
//        }
//        if(Round_Statue==5&&Round_Cnt==0)//�����ٶȴ��� ����λ������ȼ�
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
  //�뻷����� ����������� �뻷������ �������Ѹĳ����ֵ����
        if(Round_Statue>=3)//�����ٶȴ��� ����λ������ȼ�
        {
            Sit_Flag=0;
            speed_kp=0.0060;
            speed_ki=0.0004;
        }

//        if(Round_Statue==5&&Round_Cnt==2)//�����ٶȴ��� ����λ������ȼ�
//        {
//            Target_speed=75;   //90
//        }
//        if(Round_Statue==5&&Round_Cnt==0)//�����ٶȴ��� ����λ������ȼ�
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
        if(Round_Statue>=3)//�����ٶȴ��� ����λ������ȼ�
        {
            speed_kp=0.0060;
            speed_ki=0.0003;
        }
//        if(Round_Statue==5&&Round_Cnt==2)//�����ٶȴ��� ����λ������ȼ�
//        {
//            Target_speed=80;   //90
//        }
//        if(Round_Statue==5&&Round_Cnt==0)//�����ٶȴ��� ����λ������ȼ�
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
        if(Round_Statue>=3)//�����ٶȴ��� ����λ������ȼ�
        {
            speed_kp=0.0060;
            speed_ki=0.0003;
        }
        //����������

        if(Round_Statue==5&&Round_Cnt==2)//�����ٶȴ��� ����λ������ȼ�
        {
            Target_speed=75;   //90
        }
        if(Round_Statue==5&&Round_Cnt==0)//�����ٶȴ��� ����λ������ȼ�
        {
            Target_speed=75;   //90
        }
}


//����Ԫ���ٶȴ��� ������ȼ�
        if(Out_Garage_Flag==1)
        {
            Target_speed = 35;//�����  30 Ϊ����ٶ�
            speed_kp=0.0060;
            speed_ki=0.0015;
        }
        if(Zebra_Flag==2)
        {
            Target_speed = 40;//�����  30 Ϊ����ٶ�
            speed_kp=0.0060;
            speed_ki=0.0015;
        }
}
}

//��λ�ٶȵ���
//void Speed_Target_Control(void)
//{
//    switch(MODE_SELECT)
//    {
//        case 1:
//        {
//            Target_speed=58;
//            //����ר���ٶȲ���
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
//            //����ר���ٶȲ���
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
//            //����ר���ٶȲ���
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
//            //����ר���ٶȲ���
//            if(Round_Road_Flag!=0)
//            {
//                Target_speed=30;
//            }
//        }
//    }
//}

/*
 *�ٶȻ�  2.0
 *ͨ�����ұ�������ֵ��Ӧ��ʵ�ٶ�
 *�Ӽ��ٶ�ͨ���������ҵ��PWM��ͬ�Ӽ�
 *�ϵ��ݰ��ٶȻ��Լ�ת�򻷵���  ����ʵ�ֶ�̬�ٶȻ����� ��Ϊ���ʱ�����ȥ...�տ������Կ���
 */
double Speed_Control(void)
{
//      speed_kp=0.0030;
//      speed_ki=0.0030;  //�ٶȻ����� ����������  ��ȡ0.0001


       zuo_real_speed = zuo_real_speed_add/8;//�ٶ�ƽ���˲�����ĸΪ�������ڶ��ڲɼ����ڵı���
       you_real_speed = you_real_speed_add/8;

       you_real_speed_add = 0;//���ֹ���
       zuo_real_speed_add = 0;

       real_speed_last = real_speed;
       real_speed = (sint32)((zuo_real_speed+you_real_speed)/2);//�����ֵ�ƽ��ֵΪ���������ٶȣ��ȳ���
       real_speed = 0.9*real_speed + 0.1*real_speed_last;//��ͨ�˲�
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
// * ������ �ǶȻ�����
// * �ǶȻ��� ת�򻷹�
// * λ��ʽPD����
// *
// * ��һ�� ������������
//
// */
//double Angle_control(void)
//{
//    ang_kp=0.0001;  //��Ӧ������  ������Ҫ�ټӴ�   0.001  0.150
//    ang_kd=0.0015;
//
//    ang_err = Target_Z_angle - angle_yaw;
//    /* ����Сʱȡ�����(��������) */
//    if(abs(ang_err)<=10)
//    {
//        ang_err=0;
//    }
//
//    ang_last_err=ang_err;
//
//    ang_speed_dif = ang_kp*ang_err + ang_kd*(ang_err-ang_last_err);
//
//    if(ang_speed_dif>=0.5)ang_speed_dif=0.5;     //�ǶȻ��޷����  �������ı�800
//    if(ang_speed_dif<=-0.5)ang_speed_dif=-0.5;
//
//    return ang_speed_dif;
//}
