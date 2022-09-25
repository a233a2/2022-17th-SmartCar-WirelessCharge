#include "headfile.h"
#include"Platform_Types.h"
#include"math.h"
#include"stdlib.h"
//https://zhuanlan.zhihu.com/p/391797246
//�ο��Ϻ���ѧSJTU

//����ͷ��̬ǰհ
uint8 camear_foward=60;
//�ϱ���
uint8 up_line;

 // ǰհ ����
uint8 zuo_line;
// ǰհ ����
uint8 you_line;
// ǰհ ����
uint8 middle_line;
uint8 middle_line1;
uint8 low_line;

float Line_slope;
//ͼ����ֵ  OTSU��
uint8 image_threshold;

//��������
uint8 ImageSide[MT9V03X_H2][2];  //0����� 1�ұ���
uint8 ImageMid[MT9V03X_H2];  //����
uint8 Road_width[MT9V03X_H2];

//������־λ
uint8 Round_Road_Flag = 0;   //1�󻷵�  2�һ���
uint8 Round_Statue=0;    //1���� 2���� 3������ 4�뿪����
//������ ��ʱ
uint8 Round_Cnt_Boring=0;
uint8 Round_Cnt=1;  //����0ʱ�����жϻ���   1ʱ���ж��һ��� 2ʱ���ж��󻷵�  �󻷵��ж���ɺ��ٽ��л����ж� �൱�ڱ�������

//uint8 Round_cnt=0; //ÿ���ж�Ϊ������+1 ����2 ʱ�ٲ��жϻ��� �������������

//Loss_line ���߱�־λ
uint8 Loss_line_Flag=0;
uint8 Loss_line=0;
//ʮ�ֱ�־λ
uint8 Cross_Road_Flag = 0;
////P�ػ�֡�����ͳ����  ��Ԫ��������10֡��Ϊƫ��ʱ���ж�Ϊ�ػ� ֮������ѭ�� �����Դ��������̫����
//sint32 Back_Road_FPS_CNT1=0;
//sint32 Back_Road_FPS_CNT2=0;
//�����߱�־λ
uint8 Zebra_Flag=0;
uint8 Zebra_cnt=0;
uint8 Zebra_Statue=0;  //������״̬��־λ ���ְ����� ����ʱ����

//�����־λ
uint8 Three_Fork_Flag = 0;

//����2�ű�־λ  ��������һ�κ��Ϊ1
uint8 Three_Fork_Cnt = 1;
uint32 Three_Fork_Cnt_Pro = 0;

uint8 Three_Statue=0;//����״̬��־λ



//�ػ���־λ
uint8 Back_Cross_Flag=0;

//���ػ���־λ
uint8 Back_Out_Flag=0;
uint8 Back_Out_Boring=0;

//�ػ�״̬
uint8 Back_Cross_Statue=0;
uint8 Back_Cnt=2;  //1ʱ����ת 2ʱ����ת   ��һ���ػ��������� ֻ����ڶ����ػ�

//�������� ��Fix_Line��
uint8 Fix_Line_Flag=0;  //1�� 2��

//Ԫ�����ڴ����־λ Ԫ�ش���ʱ��Ԫ���ж�
uint8 Element_Processing_Flag=0;

//Ŀ��Ƕȸı��־λ  �ﵽĿ��ǶȽ�����һ�����������˳�����
uint8 Angle_CNT_Flag=0;

//���ұ�����������־λ 1���� 0������
uint8 Zuo_Correct_Flag=1;
uint8 You_Correct_Flag=1;

//������־ 1 Ѱ������� 2 Ѱ�������
uint8 End_Flag=0;

//�������鶪��ʱ��ֵ
uint8 Zuo_Hind=3;
uint8 You_Hind=157;

//�����ʱ���ͳ����־λ
uint8 Out_Garage_FPS=1;
uint8 Out_Garage_Flag=1;
//����״̬��־λ
uint8 Out_Garage_statue=1;


uint8 Eat_Line_Flag=0;   //��ʴ�˲���־λ 1Ϊ�� 0Ϊ��

uint8 Judge_Stright_Amount=25;//ֱ���ж��� ��25/50����Ҫ��Ϊ�ж�����  �ٶ�1 25   �ٶ�2 20 �ٶ�3 20



//��ȫ������־λ
//uint8 Finish_Flag=0;
/*
  *����Ѱ��
  *��ͨ�㷨
 * v1.1
 */
void seek_line(void)
{
    uint8 h=0,w;          //middle_seedÿ��Ѱ����ɺ���� ʹ�´�Ѱ��������ʼ��Ϊ��һ���е�   h:�߶ȱ���  w:��ȱ���
    uint8 zuo_seed=0,you_seed=0,middle_seed=MT9V03X_W2/2;
    for(h=MT9V03X_H2-1;h>0;h--)  //�����߶�
    {
        for(w=middle_seed;w>0+3;w--) //������--����Ѱ�� ��ȱ���
        {
            if(use_image[h][w-2]==0&&use_image[h][w-1]==0&&use_image[h][w]==255)
            {
                break;
            }
        }
            zuo_seed=w;   //��߽���

        for(w=middle_seed;w<MT9V03X_W2-3;w++)    //������++����Ѱ��  ��ȱ���
        {
            if(use_image[h][w+2]==0&&use_image[h][w+1]==0&&use_image[h][w]==255)   //����ʱ��������
            {
                break;
            }
        }
           you_seed=w;          //�߽���
           middle_seed=(zuo_seed+you_seed)/2;           //����



           up_line=0;
           if(h>1&&use_image[h-1][middle_seed]==0)       //��ֹ�ϱ��߷��� Ӱ������
           {
               low_line=h;
               for(int k=0;k<MT9V03X_W2;k++)    //���߷���
               {
                     low_line=h;
//                   lcd_drawpoint(k,h,YELLOW);
                     Pixle[h][k]=0;
               }
               up_line = h;
               break;       //�ϱ߼�ⲻ�� ֹͣ����
           }
           if(h==camear_foward)  //���� ǰհ�� �����е�����
           {
               zuo_line=zuo_seed;
               you_line=you_seed;
               middle_line=middle_seed;
           }

//        lcd_drawpoint(zuo_seed,h,RED);
//        lcd_drawpoint(you_seed,h,RED);
//        lcd_drawpoint(middle_seed,h,GREEN);

           Pixle[h][zuo_seed]=0;    //����ͼ��
           Pixle[h][you_seed]=0;
           Pixle[h][middle_seed]=0;

           Road_width[h]=you_seed-zuo_seed;  //�������

           ImageSide[h-1][0]=zuo_seed;  //���߼�¼������
           ImageSide[h-1][1]=you_seed;
           ImageMid[h-1]=middle_seed;

    }
}



/*������
 *���ɱ���ͼ-��Լ
 */
void Pixle_Show(void)
{
    uint8 i,j;
    for(i=0;i<MT9V03X_H2;i++)
    {
        for(j=0;j<MT9V03X_W2;j++)
        {
            Pixle[i][j]=255;   //��ʼ��ȫ��
        }
    }
//    seek_line();
//    lcd_displayimage032(Pixle[0],MT9V03X_W2,MT9V03X_H2);
    //seekfree_sendimg_03x(UART_3,Pixle[0],MT9V03X_W2,MT9V03X_H2);
}



/*
 * OTSU���㶯̬��ֵ����ֵ
 *
 *100-200
 */
uint8 otsuThreshold_Pro(uint8 *image, uint16 width, uint16 height)  //��ֵʵʱ����
{
   float pixelPro[256]={0.0};
   float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
   int pixelCount[256]={0};
   int i, j, pixelSum = width * height;//pixelSum =��*��=��*��=���
   uint8 threshold = 0;
   //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
   for (i = 0; i < height; i=i+2)//����������ǰ�ÿ�����ص��ֵ��ΪpixelCount���½Ǳ�
   {
      for (j = 0; j < width; j=j+2)
        {
            pixelCount[(int)image[i * width + j]]++;  //������ֵ��Ϊ����������±꣬��¼ÿ������ֵ���ֵĸ���
        }
   }

   for (i = 0; i < 256; i=i+2)
    {
      pixelPro[i] = (float)pixelCount[i] / pixelSum;//�����������������������Ӷ�ȡ��ÿ������ֵ���ֵĸ���
    }
   //         0        256
       for (i = 100; i < 201; i=i+3)     // i��Ϊ��ֵ  �����ٶ�Ϊ֮ǰ53��
    {
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
          //         256
        for (j = 0; j < 256; j=j+3)
        {
            if (j <= i)   //��������
            {
                w0 += pixelPro[j];//j <= i�ĸ���֮��
                u0tmp += j * pixelPro[j];////j <= i��j�����֮��
            }
            else   //ǰ������
            {
                w1 += pixelPro[j];//j > i�ĸ���֮��
                u1tmp += j * pixelPro[j];//j > i��j�����֮��
            }
        }
        u0 = u0tmp / w0;//��j <= i��j�����֮�������ԣ�j <= i�ĸ���֮�ͣ�
        u1 = u1tmp / w1;//��j > i��j�����֮�������ԣ�j > i�ĸ���֮�ͣ�
        u = u0tmp + u1tmp;//��j <= i��j�����֮�����ӣ�j > i��j�����֮����
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);//
        if (deltaTmp > deltaMax)//���ѭ��ȡ�����ֵ
        {
            deltaMax = deltaTmp;
            threshold = i;
        }
    }
   if(threshold>200)
     threshold = 200;
   if(threshold<100)
     threshold = 100;

   return threshold;
}


/*
  * ͼ���ֵ��    ����
 * v1.0
 */
void Binaryzation(uint8 *image,uint16 width, uint16 height,uint8 image_threshold) //��ֵ��
{
   int i=0;
   for(i=0;i<height*width;i++)
   {
       if(image[i]>=image_threshold) image[i]=255;
       else image[i]=0;
   }
}

/*
 * ͼ��ü�
 * 188*120->160*120
 *
 */
void Get_use_image(void)   //mt9v03x_image[Mid_line[i+4]][i]
{
    int i=0,j=0;
    for(i=0;i<MT9V03X_H2;i++)
    {
        for(j=0;j<MT9V03X_W2;j++)
        {
            use_image[i][j]=mt9v03x_image[i][j];
        }
    }
}


//----------------------------------
//  @brief �����޷���������
//  Sample usage:
//----------------------------------
int32 range_protect(uint8 duty, uint8 min, uint8 max)//�޷�����
{
    if (duty >= max)
    {
        return max;
    }
    if (duty <= min)
    {
        return min;
    }
    else
    {
        return duty;
    }
}

//----------------------------------
//  @brief ����Ϊ��ʴ�˲��㷨
//  Sample usage:  ��ʴ�˲��ǽ������˳�����ͳ�ư׺����������������
//                 �����߼�⣬���˲��ɽ��׵�֮��10�����ڵĺڵ��˵���
//                 �ڵ㳬��10��������Ϊ���Ǹ��ţ���Ѱ����һ���׵㣬
//                 ֮���ظ�֮ǰ�Ĺ��̣��˲�ͬʱ��¼�˰׺������ĸ���
//
//          ����ʱ ������ʹ�ô˹��� �˵� ������
//
//----------------------------------
uint8 Test_Jump;
uint8 Corrode_Filter(uint8 i, uint8 *image, uint8 Left_Min, uint8 Right_Max)//1,159��i��������date
{
     uint8 j;
     uint8 White_Flag = 0;
     uint8 Jump_Count = 0;  // ��������
     Test_Jump = 0;

     if(Right_Max>MT9V03X_W2-1)Right_Max=MT9V03X_W2-1;
     if(Right_Max<1)Right_Max=1;

     for (j = Left_Min; j <= Right_Max; j++)    // ��������ɨ�裬����Ӱ����
    {
        if (!White_Flag)    // �Ȳ��Ұ׵㣬ֻ�˺ڵ㣬���˰׵�
        {
            if (image[i*MT9V03X_W2 + j]==255)   //����ֵΪ1����⵽�׵㣬0Ϊ�ڵ�
            {
                White_Flag = 1; // ��ʼ�Һڵ�
            }
        }
        else
        {
            if (image[i*MT9V03X_W2 + j]==0)  // ��⵽�ڵ�
            {
                Jump_Count++;   // ��Ϊһ�����䣬�������ۼ�

                Test_Jump = Jump_Count;

                if (image[i*MT9V03X_W2 + j+1]==0 && j+1 <= Right_Max)    // ���������ڵ�
                {
                    if (image[i*MT9V03X_W2 + j+2]==0 && j+2 <= Right_Max)    // ���������ڵ�
                    {
                        if (image[i*MT9V03X_W2 + j+3]==0 && j+3 <= Right_Max)    // �����ĸ��ڵ�
                        {
                            if (image[i*MT9V03X_W2 + j+4]==0 && j+4 <= Right_Max)    // ��������ڵ�
                            {
                                if (image[i*MT9V03X_W2 + j+5]==0 && j+5 <= Right_Max)    // ���������ڵ�
                                {
                                    if (image[i*MT9V03X_W2 + j+6]==0 && j+6 <= Right_Max)    // �����߸��ڵ�
                                    {
                                        if (image[i*MT9V03X_W2 + j+7]==0 && j+7 <= Right_Max)    // �����˸��ڵ�
                                        {
                                            if (image[i*MT9V03X_W2 + j+8]==0 && j+8 <= Right_Max)    // �����Ÿ��ڵ�
                                            {
                                                if (image[i*MT9V03X_W2 + j+9]==0 && j+9 <= Right_Max)    // ����ʮ���ڵ�
                                                {
                                                    if (image[i*MT9V03X_W2 + j+10]==0 && j+10 <= Right_Max)  // ����11���ڵ�
                                                    {
                                                        White_Flag = 0; // ��Ϊ���Ǹ��ţ������κδ����´������׵�
                                                        j += 10;
                                                    }
                                                    else if (j+10 <= Right_Max)
                                                    {
                                                        image[i*MT9V03X_W2 + j] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+1] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+2] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+3] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+4] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+5] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+6] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+7] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+8] = 255; // ��������10���ڵ㣬�˳���
                                                        image[i*MT9V03X_W2+j+9] = 255; // ��������10���ڵ㣬�˳���

                                                        j += 10;
                                                    }
                                                    else
                                                    {
                                                        j += 10;
                                                    }
                                                }
                                                else if (j+9 <= Right_Max)
                                                {
                                                    image[i*MT9V03X_W2 + j] = 255; // ���������Ÿ��ڵ㣬�˳���
                                                    image[i*MT9V03X_W2+j+1] = 255; // ���������Ÿ��ڵ㣬�˳���
                                                    image[i*MT9V03X_W2+j+2] = 255; // ���������Ÿ��ڵ㣬�˳���
                                                    image[i*MT9V03X_W2+j+3] = 255; // ���������Ÿ��ڵ㣬�˳���
                                                    image[i*MT9V03X_W2+j+4] = 255; // ���������Ÿ��ڵ㣬�˳���
                                                    image[i*MT9V03X_W2+j+5] = 255; // ���������Ÿ��ڵ㣬�˳���
                                                    image[i*MT9V03X_W2+j+6] = 255; // ���������Ÿ��ڵ㣬�˳���
                                                    image[i*MT9V03X_W2+j+7] = 255; // ���������Ÿ��ڵ㣬�˳���
                                                    image[i*MT9V03X_W2+j+8] = 255; // ���������Ÿ��ڵ㣬�˳���

                                                    j += 9;
                                                }
                                                else
                                                {
                                                    j += 9;
                                                }
                                            }
                                            else if (j+8 <= Right_Max)
                                            {
                                                image[i*MT9V03X_W2 + j] = 255; // ���������˸��ڵ㣬�˳���
                                                image[i*MT9V03X_W2+j+1] = 255; // ���������˸��ڵ㣬�˳���
                                                image[i*MT9V03X_W2+j+2] = 255; // ���������˸��ڵ㣬�˳���
                                                image[i*MT9V03X_W2+j+3] = 255; // ���������˸��ڵ㣬�˳���
                                                image[i*MT9V03X_W2+j+4] = 255; // ���������˸��ڵ㣬�˳���
                                                image[i*MT9V03X_W2+j+5] = 255; // ���������˸��ڵ㣬�˳���
                                                image[i*MT9V03X_W2+j+6] = 255; // ���������˸��ڵ㣬�˳���
                                                image[i*MT9V03X_W2+j+7] = 255; // ���������˸��ڵ㣬�˳���

                                                j += 8;
                                            }
                                            else
                                            {
                                                j += 8;
                                            }
                                        }
                                        else if (j+7 <= Right_Max)
                                        {
                                            image[i*MT9V03X_W2 + j] = 255; // ���������߸��ڵ㣬�˳���
                                            image[i*MT9V03X_W2+j+1] = 255; // ���������߸��ڵ㣬�˳���
                                            image[i*MT9V03X_W2+j+2] = 255; // ���������߸��ڵ㣬�˳���
                                            image[i*MT9V03X_W2+j+3] = 255; // ���������߸��ڵ㣬�˳���
                                            image[i*MT9V03X_W2+j+4] = 255; // ���������߸��ڵ㣬�˳���
                                            image[i*MT9V03X_W2+j+5] = 255; // ���������߸��ڵ㣬�˳���
                                            image[i*MT9V03X_W2+j+6] = 255; // ���������߸��ڵ㣬�˳���

                                            j += 7;
                                        }
                                        else
                                        {
                                            j += 7;
                                        }
                                    }
                                    else if (j+6 <= Right_Max)
                                    {
                                        image[i*MT9V03X_W2 + j] = 255; // �������������ڵ㣬�˳���
                                        image[i*MT9V03X_W2+j+1] = 255; // �������������ڵ㣬�˳���
                                        image[i*MT9V03X_W2+j+2] = 255; // �������������ڵ㣬�˳���
                                        image[i*MT9V03X_W2+j+3] = 255; // �������������ڵ㣬�˳���
                                        image[i*MT9V03X_W2+j+4] = 255; // �������������ڵ㣬�˳���
                                        image[i*MT9V03X_W2+j+5] = 255; // �������������ڵ㣬�˳���

                                        j += 6;
                                    }
                                    else
                                    {
                                        j += 6;
                                    }
                                }
                                else if (j+5 <= Right_Max)
                                {
                                    image[i*MT9V03X_W2 + j] = 255; // ������������ڵ㣬�˳���
                                    image[i*MT9V03X_W2+j+1] = 255; // ������������ڵ㣬�˳���
                                    image[i*MT9V03X_W2+j+2] = 255; // ������������ڵ㣬�˳���
                                    image[i*MT9V03X_W2+j+3] = 255; // ������������ڵ㣬�˳���
                                    image[i*MT9V03X_W2+j+4] = 255; // ������������ڵ㣬�˳���

                                    j += 5;
                                }
                                else
                                {
                                    j += 5;
                                }
                            }
                            else if (j+4 <= Right_Max)
                            {
                                image[i*MT9V03X_W2 + j] = 255; // ���������ĸ��ڵ㣬�˳���
                                image[i*MT9V03X_W2+j+1] = 255; // ���������ĸ��ڵ㣬�˳���
                                image[i*MT9V03X_W2+j+2] = 255; // ���������ĸ��ڵ㣬�˳���
                                image[i*MT9V03X_W2+j+3] = 255; // ���������ĸ��ڵ㣬�˳���

                                j += 4;
                            }
                            else
                            {
                                j += 4;
                            }
                        }
                        else if (j+3 <= Right_Max)
                        {
                            image[i*MT9V03X_W2 + j] = 255; // �������������ڵ㣬�˳���
                            image[i*MT9V03X_W2+j+1] = 255; // �������������ڵ㣬�˳���
                            image[i*MT9V03X_W2+j+2] = 255; // �������������ڵ㣬�˳���

                            j += 3;
                        }
                        else
                        {
                            j += 3;
                        }
                    }
                    else if (j+2 <= Right_Max)
                    {
                        image[i*MT9V03X_W2 + j] = 255; // �������������ڵ㣬�˳���
                        image[i*MT9V03X_W2+j+1] = 255; // �������������ڵ㣬�˳���

                        j += 2;
                    }
                    else
                    {
                        j += 2;
                    }
                }
                else if (j+1 <= Right_Max)
                {
                    image[i*MT9V03X_W2 + j] = 255; // ��һ���ڵ㣬�˳���

                    j += 1;
                }
                else
                {
                    j += 1;
                }
            }
        }
    }
  return Jump_Count;    // ������������
}


/*
 * ȫͼ ��ʴ�˲�
 *
 */
void fix_image(void)
{
    uint8 i;
    for(i=0;i<MT9V03X_H2;i++)
    {
        Corrode_Filter(i,use_image[0],1,MT9V03X_W2-1);
    }
}

/*!
  *         ����һ�����
  *         ���ڵ��Եȫ�׻��� �׵��Եȫ��  ������ͬ��
  */
void OnePoint_fix(void)
{
    //uint8 temp = 0;
    for(int i = 1; i < MT9V03X_H2 - 1; i++)
    {
        for(int j = 1; j < MT9V03X_W2 - 1; j++)
        {
          if(use_image[i][j]==255&&use_image[i-1][j-1]==0&&use_image[i-1][j]==0&&use_image[i-1][j+1]==0&&use_image[i][j-1]==0&&use_image[i][j+1]==0&&use_image[i+1][j-1]==0&&use_image[i+1][j]==0&&use_image[i+1][j+1]==0)
          {
              use_image[i][j]=0;
          }
          if(use_image[i][j]==0&&use_image[i-1][j-1]==255&&use_image[i-1][j]==255&&use_image[i-1][j+1]==255&&use_image[i][j-1]==255&&use_image[i][j+1]==255&&use_image[i+1][j-1]==255&&use_image[i+1][j]==255&&use_image[i+1][j+1]==255)
          {
              use_image[i][j]=0;
          }
        }
    }
}
/*
 * 1����ͼ��
 * 2��̬�����ֵ����ֵ
 * 3��ֵ��
 * 4��ʴͼ���˲��޸�10�ڵ�   ���ܼ����
  *    ʹ��ͼ���С 160*120
  *
 */
void image_process1(void)
{
    Get_use_image();

    image_threshold = otsuThreshold_Pro(use_image[0],MT9V03X_W2,MT9V03X_H2);  //OTSU���㶯̬��ֵ

    Binaryzation(use_image[0],MT9V03X_W2,MT9V03X_H2,image_threshold);  //ͼ���ֵ��

//    OnePoint_fix();

    if(Eat_Line_Flag==1)fix_image();  //��ʴ�˲�  ����


//    fix_image();  //��ʴ�˲�  ����

}
//// ǰհ ����
//uint8 zuo_line;
//// ǰհ ����
//uint8 you_line;
//// ǰհ ����
//uint8 middle_line;
////ͼ����ֵ  OTSU��
//extern uint8 image_threshold;
////��������
//uint8 ImageSide[MT9V03X_H2][2];  //0����� 1�ұ���
//uint8 ImageMid[MT9V03X_H2];  //����

/****************************Ԫ�ش���****************************/

/*
 * ʮ���б�
 *v1.0
 *ע��б��ʮ��
 */
void Element_Cross(void)
{

}

//ʮ�ִ���
void Element_Cross_Process(void)
{

}
/*
 * P�ػ��ж�
 *�ж����ִ�лػ���һ���ж� �ж��϶�Ϊ���ػ���������ת
 * ������ ���⻷������  ���ڼӰ�������������  ����ס
 *
 * �ȱ����� �����治�� ���϶�һ�� �ٲ��� �ٶ�һС�� ����Head_errҲƫ
 * ���� sjy  2022 - 7 - 14
 */
void Element_BackCross(void)
{
    uint8 i;
    uint8 cnt1=0,cnt2=0;   //���ҵĵ����־λ
    int loss_start_point=0; //������ʼ��
    int loss_point1=0; //


    uint8 Back_Type_Maybe=0; //Ϊ1 ��ػ��ж�  Ϊ2�һػ��ж� Ϊ0���ж� ��ʡʱ��

    if(Zuo_Correct_Flag==0&&You_Correct_Flag==1)  //����Ϊ��ػ�
    {
        Back_Type_Maybe=1;
    }
    else if(Zuo_Correct_Flag==1&&You_Correct_Flag==0)
    {
        Back_Type_Maybe=2;
    }
    else Back_Type_Maybe=0;


        if(Back_Type_Maybe!=0)
        {
            switch (Back_Type_Maybe)
            {
                case 1:   //��  ����������ζ���
                {
                    for(i=110;i>=50;i--)  //�������� �Ҷ�����ʼ��
                    {
                        if(ImageSide[i][0]>=(Zuo_Hind+3)&&ImageSide[i-1][0]<=(Zuo_Hind+3)&&ImageSide[i-2][0]<=(Zuo_Hind+3)) //�� ���� ����
                        {
                            cnt1++;
                            loss_start_point=i;
                            break;
                        }
                    }
                    if(cnt1!=0)
                    {
                        for(i=(loss_start_point-5);i>=(loss_start_point-60);i--)  //������ʼ��������5���� ��ʼѭ�� �Ҷ��߽�����
                        {
                            if(ImageSide[i][0]<=(Zuo_Hind+3)&&ImageSide[i-1][0]>=(Zuo_Hind+3)&&ImageSide[i-2][0]>=(Zuo_Hind+3))//�� �� ���� ��
                            {
                                cnt1++;
                                loss_point1=i;
                                break;
                            }
                        }
                    }
                    if(cnt1==2)
                    {
                        for(i=(loss_point1-5);i>=0;i--)  //�ٴ�����5���� ����
                        {
                            if(ImageSide[i][0]<=(Zuo_Hind+3))cnt2++;  //��
                            if(cnt2>=0)break;
                        }
                        if(cnt2==0)
                        {
                            Back_Cross_Flag=1;  //��ػ�
                            Element_Processing_Flag=1;
                            Back_Cross_Statue=1;
                        }
                    }
                    break;
                }
                case 2:   //�һػ��ж� �һػ�ʵ���Ҳ����� ʹ���������㷨�ж� ��Ϊ����ͷ�ߵ�ԭ��ػ����Կ������ζ��ߵ�
                {
                    for(i=110;i>=50;i--)  //�������� �Ҷ�����ʼ��
                    {
                        if(ImageSide[i][1]!=You_Hind&&ImageSide[i-1][1]==You_Hind&&ImageSide[i-2][1]==You_Hind-1)
                        {
                            cnt1++;
                            loss_start_point=i;
                            break;
                        }
                    }
                    if(cnt1!=0)
                    {
                        for(i=(loss_start_point-5);i>=(loss_start_point-60);i--)  //������ʼ��������10���� ��ʼѭ�� �Ҷ��߽�����
                        {
                            if(ImageSide[i][1]>=(You_Hind-1)&&ImageSide[i-1][1]<=(You_Hind-1)&&ImageSide[i-2][1]<=(You_Hind-1))
                            {
                                cnt1++;
                                loss_point1=i;
                                break;
                            }
                        }
                    }
                    if(cnt1==2)
                    {
                        for(i=(loss_point1-5);i>=0;i--)
                        {
                            if(ImageSide[i][1]>=(You_Hind-1))cnt2++;
                            if(cnt2>=0)break;    //���ϼ�⵽���ߵ� ��ȷ��Ϊ ���ζ���
                        }
                        if(cnt2==0)
                        {
                            Back_Cross_Flag=2;  //��ػ�
                            Element_Processing_Flag=1;
                            Back_Cross_Statue=1;
                        }
                    }
                }
            }
        }
}


//�ػ��ж�2
//ͨ���±��߶��ж����Լ���ʱ����ƫ�� ���ж�P�ػ�
//���� ���һ 2022-07-15-16:36
void Element_BackCross2(void)
{
 uint8 i;
 uint8 Down_Loss_Cnt=0;  //�±��߶������� ������
 uint8 Back_Maybe=0;  //1 ��ػ� 2 �һػ�
 int Mid_001;  //�������
 uint8 Back_Flag=0;
 Mid_001 = (78-ImageMid[30])*0.25+(78-ImageMid[40])*0.25+(78-ImageMid[50])*0.25+(78-ImageMid[60])*0.25;

 if(TestVolt<=700&&TestVolt>=400)   //���Ҷ�������ʱ�ж� ѡ����ɾ��
 {
     if(ImageSide[117][0]==Zuo_Hind)  //������·�����
     {
         Back_Maybe=1;  //����Ϊ��ػ�
     }
     else if(ImageSide[117][1]==You_Hind)  //�ұ����·�����
     {
         Back_Maybe=2;  //����Ϊ��ػ�
     }
     if(Back_Maybe!=0)
     {
         Down_Loss_Cnt=0;
         switch (Back_Maybe)
         {
             case 1: //��ػ���һ���ж�
             {
                 for(i=118;i>=10;i-=2)  //������ѭ��  Ѳ��54��
                 {
                     if(ImageSide[i][0]==Zuo_Hind)
                     {
                         Down_Loss_Cnt++;
                     }
                     if(Down_Loss_Cnt>=12)
                     {
                         Back_Flag=1;
                         break;
                     }
                 }
                 if(Mid_001>=0&&Back_Flag==1)  //��P
                 {
                     Back_Cross_Flag=1;
                     Element_Processing_Flag=1;
                     Back_Cross_Statue=1;      //ȷ��Ϊ��ػ�
                 }
                 break;
             }
             case 2:  //�һػ���һ���ж�
             {
                 for(i=118;i>=10;i-=2)  //������ѭ��  Ѳ��54��
                 {
                     if(ImageSide[i][1]==You_Hind)
                     {
                         Down_Loss_Cnt++;
                     }
                     if(Down_Loss_Cnt>=12)
                     {
                         Back_Flag=1;
                         break;
                     }
                 }
                 if(Mid_001<=0&&Back_Flag==1)  //��P
                 {
                     Back_Cross_Flag=2;
                     Element_Processing_Flag=1;
                     Back_Cross_Statue=1;            //ȷ��Ϊ�һػ�
                 }
             }
         }
     }


 }
}


//�ػ��ж�3.0
//�ж����Ҷ�����ǰ����������
void Element_BackCross3(void)
{
    uint8 i;
    uint8 cnt1=0,cnt2=0;
    uint8 cnt=0;
    uint8 Black_Correct=0;

//    for(i=10;i<=150;i+=2)  //140
//    {
//        if(use_image[10][i]==0)
//        {
//            cnt++;
//        }
//        if(cnt>=45)
//        {
//            Black_Correct=1;
//            break;
//        }
//    }

    if(use_image[20][20]==0&&use_image[20][80]==0&&use_image[20][140]==0&&low_line>=30)  //�Ϸ�����
    {
        Black_Correct=1;
    }
    else Black_Correct=0;

    if(Black_Correct==1&&TestVolt>=300&&TestVolt<=500)  //�Ϸ�������������40 �����һ���ж�  ���������� ��С�ڵ���800
    {
        for(i=110;i>=10;i-=2) //�ӵײ�����ѭ��  87
        {
            if(ImageSide[i][0]==Zuo_Hind)
            {
                cnt1++;
            }
            if(ImageSide[i][1]==You_Hind)
            {
                cnt2++;
            }
            if(cnt1>=15&&cnt2>=15)
            {
                Back_Cross_Flag=1;
                Element_Processing_Flag=1;  //��Ԫ���ж�
//                break;
            }
            else if(cnt1>=30&&cnt2>=5)
            {
                Back_Cross_Flag=1;
                Element_Processing_Flag=1;  //��Ԫ���ж�
//                break;
            }
            else if(cnt1>=5&&cnt2>=30)
            {
                Back_Cross_Flag=1;
                Element_Processing_Flag=1;  //��Ԫ���ж�
//                break;
            }
        }
    }
}

//�ػ��ж�4.0
//�ж����Ҷ�����ǰ����������
void Element_BackCross4(void)
{
    uint8 i;
    uint8 cnt1=0,cnt2=0;
    uint8 BINGO_FLAG=0;
    uint8 Black_Correct=0;
    if(use_image[20][20]==0&&use_image[20][80]==0&&use_image[20][140]==0&&low_line>=20)  //�Ҷ��ߴ���ĳֵ
    {
        Black_Correct=1;
    }
    else Black_Correct=0;

    if(Black_Correct==1)  //�Ϸ�������������40 �����һ���ж�  ���������� ��С�ڵ���800
    {
        for(i=117;i>=15;i-=2) //�ӵײ�����ѭ��  87
        {
            if(ImageSide[i][0]==Zuo_Hind)
            {
                cnt1++;
            }
            if(cnt1>=20)
            {
                BINGO_FLAG=1;
                break;
            }
        }
        if(BINGO_FLAG==1)
        {
            for(i=117;i>=15;i-=2) //�ӵײ�����ѭ��  87
            {
                if(ImageSide[i][0]==Zuo_Hind)
                {
                    cnt2++;
                }
                if(cnt2>=20)
                {
                    Back_Cross_Flag=1;
                    Element_Processing_Flag=1;  //��Ԫ���ж�
                    break;
                }
            }
        }
    }
}

//��һ���ػ���ת �ڶ�����ת
void Backcross_process2(void)
{
    switch (Back_Cnt)
    {
        case 1:  //��һ���ػ���ת
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=-60.00;
                Angle_CNT_Flag=1;
                Fix_Line_Flag=4;  //��ת����
                Mid_Get_Type=0;
            }
            if(Angle_CNT_Flag==1)
            {
                if(ang_err<=10.00&&Angle_CNT_Flag==1)    //����
                {
                    Back_Cnt++;
                    Angle_CNT_Flag=0;
                    Fix_Line_Flag=0;  //���߹ر�  .
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Back_Cross_Flag=0;
                    Mid_Get_Type=0;
                }
            }
            break;
        }
        case 2:
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;
                Angle_CNT_Flag=1;
                Fix_Line_Flag=3;  //��ת����
            }
            if(Angle_CNT_Flag==1)
            {
                if(ang_err<=10.00&&Angle_CNT_Flag==1)    //����
                {
                    Back_Cnt=0;
                    Angle_CNT_Flag=0;
                    Fix_Line_Flag=0;  //���߹ر�  .
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Back_Cross_Flag=0;
                }
            }
        }
    }

}

//P�ִ���

//���������һ���ĵ�״̬�����
void Backcross_process(void)
{
    switch (Back_Cross_Flag)
    {
        case 1: //��ػ�
        {
            switch (Back_Cross_Statue)
            {
                case 1:
                {
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=260.00;
                        Angle_CNT_Flag=1;
//                        Mid_Get_Type=2;
                    }
                    if(Angle_CNT_Flag==1)
                    {
                        if(ang_err<=10.00&&Angle_CNT_Flag==1)
                        {
                            Angle_CNT_Flag=0;
                            Back_Cross_Statue=2;

                        }
                    }
                    break;
                }
                case 2:
                {
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=-60.00;
                        Angle_CNT_Flag=1;

//                        Mid_Get_Type=2;//��ת
                        Fix_Line_Flag=4;  //��ת����
                    }
                    if(Angle_CNT_Flag==1)
                    {
                        if(ang_err<=10.00&&Angle_CNT_Flag==1)    //����
                        {
                            Angle_CNT_Flag=0;
                            Back_Cross_Statue=2;

//                            Mid_Get_Type=0;//��ת
                            Fix_Line_Flag=0;  //��ת����  .
                            Element_Processing_Flag=0;   //����Ԫ���ж�
                            Back_Cross_Flag=0;
                        }
                    }
                }
            }
            break;
        }
        case 2:  //�һػ�
        {
            switch (Back_Cross_Statue)
            {
                case 1:
                {
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=-250.00;
                        Angle_CNT_Flag=1;
                    }
                    if(Angle_CNT_Flag==1)
                    {
                        if(ang_err<=10.00&&Angle_CNT_Flag==1)
                        {
                            Angle_CNT_Flag=0;
                            Back_Cross_Statue=2;
                        }
                    }
                    break;
                }
                case 2:
                {
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=80.00;
                        Angle_CNT_Flag=1;

//                        Mid_Get_Type=1;//��ת
                        Fix_Line_Flag=3;  //��ת����
                    }
                    if(Angle_CNT_Flag==1)
                    {
                        if(ang_err<=10.00&&Angle_CNT_Flag==1)    //����
                        {
                            Angle_CNT_Flag=0;
                            Back_Cross_Statue=0;

//                            Mid_Get_Type=0;
                            Fix_Line_Flag=0;
                            Element_Processing_Flag=0;   //����Ԫ���ж�
                            Back_Cross_Flag=0;
                        }
                    }
                }
            }

        }


    }

}


/*
 * �жϱ����Ƿ񵥵� ����
    dir 0 ������ж� dir 1 �ұ���   3 ���Ҷ��ж� 3ʱ����    0Ϊ�������� 1 �󲻵��� 2 �Ҳ����� 3������
    ����1�򵥵� 0 ������ ��ͻ�� ����ֱ������
 */
void Analyse_line_straight(uint8 dir)  //����0�򲻵��� ����ͻ�� ����1����ߵ���
{
    uint8 i;
    uint8 cnt=0;
    int slope1=0,slope2=0,slope3=0;
    int slope_dec = 0;

    switch (dir)
    {
        case 0:
        {
            for(i=110;i>=10;i-=2)  //����60 MAX (120-lowline)/2
            {
                if(i==100)
                slope1 = ImageSide[i-10][0] - ImageSide[i+10][0];
                if(i==60)
                slope2 = ImageSide[i-10][0] - ImageSide[i+10][0];
                if(i==20)
                slope3 = ImageSide[i-10][0] - ImageSide[i+10][0];



                if(ImageSide[i][0] == Zuo_Hind)   //�����������������
                {
                    continue;
                }
                if(ImageSide[i][0]>ImageSide[i+5][0] && ImageSide[i][0]<ImageSide[i-5][0])
                {
                    cnt++;
                }

                if( cnt>= Judge_Stright_Amount )   //   35/50
                {
                    slope_dec = (abs(slope1-slope2) + abs(slope1-slope3) + abs(slope2-slope3))/3;
                    if(slope_dec<=5)
                    {
                        Zuo_Correct_Flag=1;   //��ȷ�ĵ����50 ����Ϊ��������ȷ
                        break;
                    }
                }
                else if( cnt< Judge_Stright_Amount )
                {
                    Zuo_Correct_Flag=0;   //������
                }
            }
            break;
        }

        case 1:
        {
            for(i=110;i>=10;i-=2)//50
            {
                if(i==100)
                slope1 = ImageSide[i+10][1] - ImageSide[i-10][1];
                if(i==60)
                slope2 = ImageSide[i+10][1] - ImageSide[i-10][1];
                if(i==20)
                slope3 = ImageSide[i+10][1] - ImageSide[i-10][1];

                if(ImageSide[i][1] == You_Hind)   //�����������������
                {
                    continue;
                }
                if(ImageSide[i][1]<ImageSide[i+5][1] && ImageSide[i][1]>ImageSide[i-5][1] ) //��Ԥ�������߱Ƚ�
                {
                    cnt++;
                }
                if( cnt>= Judge_Stright_Amount )
                {
                    slope_dec = (abs(slope1-slope2) + abs(slope1-slope3) + abs(slope2-slope3))/3;
                    if(slope_dec<=5)
                    {
                        You_Correct_Flag=1;   //����
                        break;
                    }
                }
                else if( cnt< Judge_Stright_Amount )
                {
                    You_Correct_Flag=0;//������ ��ͻ�����
                }
            }
        }
    }
}
//������������ٶȵ���
void Round_Process_Select(void)
{
    switch (MODE_SELECT)
    {
        case 1:
        {
            Round_Processer_Charge1();
            break;
        }
        case 2:
        {
            Round_Processer_Charge2();
            break;
        }
        case 3:
        {
            Round_Processer_Charge3();
            break;
        }
        case 4:
        {
            Round_Processer_Charge4();
        }
    }
}


//�������Ļ���ִ�г���
//��Ϊ����һ����ƫ һƫ�ͳ䲻��ȥ��
void Round_Processer_Charge1(void)
{
    switch (Round_Road_Flag)
    {
        case 1:         //�󻷵�  ������ ����//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            switch (Round_Statue)
            {
                case 1:  //�󻷵���Ѱ���߱�ֱ֤����ʻ
                {
                    Mid_Get_Type=4;  //��Ѳ���߱�ֱ֤����ʻ   ��ʱ��Ѱ�ұ��� ��Ϊģʽ4 Ѱ��������ұ���
                    if(Stop_Flag==1) //ͣ��֮��
                    {
                        Round_Statue=2;
                    }
                    break;
                }
                case 2:  //�Ƕȼ�����  �ô�ǽ�������  ��ǽ����� �󻷵���������߽����� ���Ƕȸ�����ѭ�ұ��� ��Ϊ��ʱ������ڻ��� �Ѿ��ǿ�������
                {
                    Mid_Get_Type=3;  //��⵽���߾�Ѱ���� Ѱ���߽�����
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=60.00;  //1�趨�����Ƕ�ֵ
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�  25���������
                    {
                        Round_Statue=3;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 3:
                {
                    Mid_Get_Type=4;   //��������ѭ�ұ��� ��֤�ȶ���    2��4 ���ұ���  �󻷵����ȶ�����Ϊ�Ϸ����ߵ��·���Ϊ0 �볣�治����
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=130.00;  //1�趨�����Ƕ�ֵ
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
                        Round_Statue=4;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 4:   //���󻷵�Ѱ���»���
                {
                    Mid_Get_Type=3;
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=120.00;  //1�趨�����Ƕ�
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
                        Round_Statue=5;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 5:   //����ʱ�������� Ѱֱ�� �ҳ���
                {
                    Mid_Get_Type=2;   //������ ���������ȶ� ��ʱ�����Ǹ�ֱ��
                    Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������
                    if(Round_Cnt_Boring==30)  //��������  ����Ҳ���ڳ�� ���Դ��
                    {
                        Round_Cnt_Boring=0;
                        Round_Statue=0;
                        Round_Road_Flag=0;
                        Element_Processing_Flag=0;   //����Ԫ���ж�
                        Mid_Get_Type=0;
                    }
                }
            }
            break;
        }
        case 2:         //�һ���//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            switch (Round_Statue)
            {
                case 1:  //�һ�����Ѱ���߱�ֱ֤����ʻ
                {
                    Mid_Get_Type=1;  //��Ѳ���߱�ֱ֤����ʻ   ��ʱ��Ѱ���±��� ��Ϊģʽ3 Ѱ������������
                    if(Stop_Flag==1) //ͣ��֮��
                    {
                        Round_Statue=2;
                    }
                    break;
                }
                case 2:  //�Ƕȼ�����  �ô�ǽ������� ��ʱ�л���·���󻷵��Ļ���
                {
//                    Fix_Line_Flag=2;
                    Mid_Get_Type=4;  //��⵽���߾�Ѱ����
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=-50.00;  //1�趨�����Ƕ�ֵ
//                        Target_Z_angle = angle_yaw - 60.00;  //�Ƕ�ͳ��90��
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
//                        Fix_Line_Flag=0;
                        Round_Statue=3;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 3:
                {
                    Mid_Get_Type=3;  //���� ��Ѱ����
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=-200.00;  //1�趨�����Ƕ�ֵ
//                         Target_Z_angle = angle_yaw - 200.00;  //�Ƕ�ͳ��200��
                         Angle_CNT_Flag=1;  //���Ƕȼ�����
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
                        Round_Statue=4;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 4:   //���»���
                {
                    Mid_Get_Type=2;
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=-45.00;  //1�趨�����Ƕ�ֵ
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
                        Round_Statue=5;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 5:   //����  ��������
                {
                    Mid_Get_Type=1;
                    Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������
                    if(Round_Cnt_Boring==30)  //��������
                    {
                        Round_Cnt_Boring=0;
                        Round_Statue=0;
                        Round_Road_Flag=0;
                        Element_Processing_Flag=0;   //����Ԫ���ж�
                        Mid_Get_Type=0;
                    }
                }
            }
        }

    }
}
//�������Ļ���ִ�г���
//��Ϊ����һ����ƫ һƫ�ͳ䲻��ȥ��
void Round_Processer_Charge2(void)
{
    switch (Round_Road_Flag)
    {
        case 1:         //�󻷵�  ������ ����//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            switch (Round_Statue)
            {
                case 1:  //�󻷵���Ѱ���߱�ֱ֤����ʻ
                {
                    Mid_Get_Type=4;  //��Ѳ���߱�ֱ֤����ʻ   ��ʱ��Ѱ�ұ��� ��Ϊģʽ4 Ѱ��������ұ���
                    if(Stop_Flag==1) //ͣ��֮��
                    {
                        Round_Statue=2;
                    }
                    break;
                }
                case 2:  //�Ƕȼ�����  �ô�ǽ�������  ��ǽ����� �󻷵���������߽����� ���Ƕȸ�����ѭ�ұ��� ��Ϊ��ʱ������ڻ��� �Ѿ��ǿ�������
                {
                    Mid_Get_Type=3;  //��⵽���߾�Ѱ���� Ѱ���߽�����
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=60.00;  //1�趨�����Ƕ�ֵ
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�  25���������
                    {
                        Round_Statue=3;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 3:
                {
                    Mid_Get_Type=4;   //��������ѭ�ұ��� ��֤�ȶ���    2��4 ���ұ���  �󻷵����ȶ�����Ϊ�Ϸ����ߵ��·���Ϊ0 �볣�治����
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=190.00;  //1�趨�����Ƕ�ֵ
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
                        Round_Statue=4;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 4:   //���󻷵�Ѱ���»���
                {
                    Mid_Get_Type=1;
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=60.00;  //1�趨�����Ƕ�
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
                        Round_Statue=5;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 5:   //����ʱ�������� Ѱֱ�� �ҳ���
                {
                    Mid_Get_Type=2;   //������ ���������ȶ� ��ʱ�����Ǹ�ֱ��
                    Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������
                    if(Round_Cnt_Boring==30)  //��������  ����Ҳ���ڳ�� ���Դ��
                    {
                        Round_Cnt_Boring=0;
                        Round_Statue=0;
                        Round_Road_Flag=0;
                        Element_Processing_Flag=0;   //����Ԫ���ж�
                        Mid_Get_Type=0;
                    }
                }
            }
            break;
        }
        case 2:         //�һ���//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            switch (Round_Statue)
            {
                case 1:  //�һ�����Ѱ���߱�ֱ֤����ʻ
                {
                    Mid_Get_Type=1;  //��Ѳ���߱�ֱ֤����ʻ   ��ʱ��Ѱ���±��� ��Ϊģʽ3 Ѱ������������
                    if(Stop_Flag==1) //ͣ��֮��
                    {
                        Round_Statue=2;
                    }
                    break;
                }
                case 2:  //�Ƕȼ�����  �ô�ǽ������� ��ʱ�л���·���󻷵��Ļ���
                {
//                    Fix_Line_Flag=2;
                    Mid_Get_Type=4;  //��⵽���߾�Ѱ����
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=-50.00;  //1�趨�����Ƕ�ֵ
//                        Target_Z_angle = angle_yaw - 60.00;  //�Ƕ�ͳ��90��
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
//                        Fix_Line_Flag=0;
                        Round_Statue=3;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 3:
                {
                    Mid_Get_Type=3;  //���� ��Ѱ����
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=-200.00;  //1�趨�����Ƕ�ֵ
//                         Target_Z_angle = angle_yaw - 200.00;  //�Ƕ�ͳ��200��
                         Angle_CNT_Flag=1;  //���Ƕȼ�����
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
                        Round_Statue=4;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 4:   //���»���
                {
                    Mid_Get_Type=2;
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=-70.00;  //1�趨�����Ƕ�ֵ
                        Angle_CNT_Flag=1;  //���Ƕȼ�����
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                    {
                        Round_Statue=5;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 5:   //����  ��������
                {
                    Mid_Get_Type=1;
                    Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������
                    if(Round_Cnt_Boring==30)  //��������
                    {
                        Round_Cnt_Boring=0;
                        Round_Statue=0;
                        Round_Road_Flag=0;
                        Element_Processing_Flag=0;   //����Ԫ���ж�
                        Mid_Get_Type=0;
                    }
                }
            }
        }

    }
}//�������Ļ���ִ�г���
//��Ϊ����һ����ƫ һƫ�ͳ䲻��ȥ��  �ٶ��� �����ٶ�70
void Round_Processer_Charge3(void)
{
switch (Round_Road_Flag)
{
    case 1:         //�󻷵�  ������ ����//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        switch (Round_Statue)
        {
            case 1:  //�󻷵���Ѱ���߱�ֱ֤����ʻ
            {
                Mid_Get_Type=2;  //��Ѳ���߱�ֱ֤����ʻ   ��ʱ��Ѱ�ұ��� ��Ϊģʽ4 Ѱ��������ұ���
                if(Stop_Flag==1) //ͣ��֮��
                {
                    Round_Statue=2;
                }
                break;
            }
            case 2:  //�Ƕȼ�����  �ô�ǽ�������  ��ǽ����� �󻷵���������߽����� ���Ƕȸ�����ѭ�ұ��� ��Ϊ��ʱ������ڻ��� �Ѿ��ǿ�������
            {
                Mid_Get_Type=3;  //��⵽���߾�Ѱ���� Ѱ���߽�����
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=60.00;  //1�趨�����Ƕ�ֵ
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�  25���������
                {
                    Round_Statue=3;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 3:
            {
                Mid_Get_Type=4;   //��������ѭ�ұ��� ��֤�ȶ���    2��4 ���ұ���  �󻷵����ȶ�����Ϊ�Ϸ����ߵ��·���Ϊ0 �볣�治����
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=180.00;  //1�趨�����Ƕ�ֵ
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
                    Round_Statue=4;
                    Angle_CNT_Flag=0;


                    Charge_point=6.00;        //���� �����������


                }
                break;
            }
            case 4:   //���󻷵�Ѱ���»���
            {
                Mid_Get_Type=1;
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=70.00;  //1�趨�����Ƕ�
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
                    Round_Statue=5;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 5:   //����ʱ�������� Ѱֱ�� �ҳ���
            {


                Mid_Get_Type=2;   //������ ���������ȶ� ��ʱ�����Ǹ�ֱ��
                Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������
                if(Round_Cnt_Boring==35)  //��������  ����Ҳ���ڳ�� ���Դ��
                {
                    Round_Cnt_Boring=0;
                    Round_Statue=0;
                    Round_Road_Flag=0;
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Mid_Get_Type=0;


                    Charge_Top=8.50;   //���󻷵��ѳ����ߵ�ѹ���� �����һ�������治�س�̫��
                    Charge_point=6.00;        //���� �����������


                }
            }
        }
        break;
    }
    case 2:         //�һ���//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        switch (Round_Statue)
        {
            case 1:  //�һ�����Ѱ���߱�ֱ֤����ʻ
            {
                Mid_Get_Type=1;  //��Ѳ���߱�ֱ֤����ʻ   ��ʱ��Ѱ���±��� ��Ϊģʽ3 Ѱ������������
                if(Stop_Flag==1) //ͣ��֮��
                {
                    Round_Statue=2;
                }
                break;
            }
            case 2:  //�Ƕȼ�����  �ô�ǽ������� ��ʱ�л���·���󻷵��Ļ���
            {
//                    Fix_Line_Flag=2;
                Mid_Get_Type=4;  //��⵽���߾�Ѱ����  �ٶȸ�����»���Ÿ������
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=-60.00;  //1�趨�����Ƕ�ֵ
//                        Target_Z_angle = angle_yaw - 60.00;  //�Ƕ�ͳ��90��
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
//                        Fix_Line_Flag=0;
                    Round_Statue=3;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 3:
            {
                Mid_Get_Type=3;  //���� ��Ѱ����
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=-180.00;  //1�趨�����Ƕ�ֵ
//                         Target_Z_angle = angle_yaw - 200.00;  //�Ƕ�ͳ��200��
                     Angle_CNT_Flag=1;  //���Ƕȼ�����
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
                    Round_Statue=4;
                    Angle_CNT_Flag=0;




                    Charge_point=5.00;        //���� �����������  6.00���ٳ�




                }
                break;
            }
            case 4:   //���»���
            {
                Mid_Get_Type=2;
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=-70.00;  //1�趨�����Ƕ�ֵ
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
                    Round_Statue=5;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 5:   //����  ��������
            {


                Mid_Get_Type=1;
                Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������
                if(Round_Cnt_Boring==35)  //��������
                {
                    Round_Cnt_Boring=0;
                    Round_Statue=0;
                    Round_Road_Flag=0;
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Mid_Get_Type=0;

                    Charge_point=9.00;        //  ���һ��������� ������޵�ѹ

                }
            }
        }
    }

}
}
//�������Ļ���ִ�г���
//��Ϊ����һ����ƫ һƫ�ͳ䲻��ȥ��  �ٶ��� �����ٶ�70
void Round_Processer_Charge4(void)
{
switch (Round_Road_Flag)
{
    case 1:         //�󻷵�  ������ ����//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        switch (Round_Statue)
        {
            case 1:  //�󻷵���Ѱ���߱�ֱ֤����ʻ
            {
                Mid_Get_Type=2;  //��Ѳ���߱�ֱ֤����ʻ   ��ʱ��Ѱ�ұ��� ��Ϊģʽ4 Ѱ��������ұ���
                if(Stop_Flag==1) //ͣ��֮��
                {
                    Round_Statue=2;
                }
                break;
            }
            case 2:  //�Ƕȼ�����  �ô�ǽ�������  ��ǽ����� �󻷵���������߽����� ���Ƕȸ�����ѭ�ұ��� ��Ϊ��ʱ������ڻ��� �Ѿ��ǿ�������
            {
                Mid_Get_Type=3;  //��⵽���߾�Ѱ���� Ѱ���߽�����
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=60.00;  //1�趨�����Ƕ�ֵ
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�  25���������
                {
                    Round_Statue=3;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 3:
            {
                Mid_Get_Type=4;   //��������ѭ�ұ��� ��֤�ȶ���    2��4 ���ұ���  �󻷵����ȶ�����Ϊ�Ϸ����ߵ��·���Ϊ0 �볣�治����
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=180.00;  //1�趨�����Ƕ�ֵ
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
                    Round_Statue=4;
                    Angle_CNT_Flag=0;


                    Charge_point=6.00;        //���� �����������


                }
                break;
            }
            case 4:   //���󻷵�Ѱ���»���
            {
                Mid_Get_Type=1;
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=70.00;  //1�趨�����Ƕ�
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
                    Round_Statue=5;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 5:   //����ʱ�������� Ѱֱ�� �ҳ���
            {


                Mid_Get_Type=2;   //������ ���������ȶ� ��ʱ�����Ǹ�ֱ��
                Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������
                if(Round_Cnt_Boring==35)  //��������  ����Ҳ���ڳ�� ���Դ��
                {
                    Round_Cnt_Boring=0;
                    Round_Statue=0;
                    Round_Road_Flag=0;
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Mid_Get_Type=0;


                    Charge_Top=8.50;   //���󻷵��ѳ����ߵ�ѹ���� �����һ�������治�س�̫��
                    Charge_point=5.00;        //���� �����������


                }
            }
        }
        break;
    }
    case 2:         //�һ���//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        switch (Round_Statue)
        {
            case 1:  //�һ�����Ѱ���߱�ֱ֤����ʻ
            {
                Mid_Get_Type=1;  //��Ѳ���߱�ֱ֤����ʻ   ��ʱ��Ѱ���±��� ��Ϊģʽ3 Ѱ������������
                if(Stop_Flag==1) //ͣ��֮��
                {
                    Round_Statue=2;
                }
                break;
            }
            case 2:  //�Ƕȼ�����  �ô�ǽ������� ��ʱ�л���·���󻷵��Ļ���
            {
//                    Fix_Line_Flag=2;
                Mid_Get_Type=4;  //��⵽���߾�Ѱ����  �ٶȸ�����»���Ÿ������
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=-60.00;  //1�趨�����Ƕ�ֵ
//                        Target_Z_angle = angle_yaw - 60.00;  //�Ƕ�ͳ��90��
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
//                        Fix_Line_Flag=0;
                    Round_Statue=3;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 3:
            {
                Mid_Get_Type=3;  //���� ��Ѱ����
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=-180.00;  //1�趨�����Ƕ�ֵ
//                         Target_Z_angle = angle_yaw - 200.00;  //�Ƕ�ͳ��200��
                     Angle_CNT_Flag=1;  //���Ƕȼ�����
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
                    Round_Statue=4;
                    Angle_CNT_Flag=0;




                    Charge_point=5.00;        //���� �����������  6.00���ٳ�




                }
                break;
            }
            case 4:   //���»���
            {
                Mid_Get_Type=2;
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=-70.00;  //1�趨�����Ƕ�ֵ
                    Angle_CNT_Flag=1;  //���Ƕȼ�����
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //�����趨�Ƕ�
                {
                    Round_Statue=5;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 5:   //����  ��������
            {


                Mid_Get_Type=1;
                Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������
                if(Round_Cnt_Boring==35)  //��������
                {
                    Round_Cnt_Boring=0;
                    Round_Statue=0;
                    Round_Road_Flag=0;
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Mid_Get_Type=0;

                    Charge_point=9.00;        //  ���һ��������� ������޵�ѹ

                }
            }
        }
    }

}
}

//�������Ļ���ִ�г���
//��Ϊ����һ����ƫ һƫ�ͳ䲻��ȥ��
/*
 *    �����ػ�һ���ж� �ȼ�������������־
 *    ����������� �������һ�� �жϻ�����ػ�����
 *    �������������
 *    ��һ������Ԫ���ж�ʱ�� ���ж������������� �����жϱ���
 *
 *    ����  Ϊ �ϸ����� ��ֱ������ʽ ���������� ���߶������򻷵� �ػ�ֱ����0 ��һ���̶��Ͻ�ʡ�˼�����
 */
void Element_Round_and_Backcross(void)
{
    if(Zuo_Correct_Flag==0&&You_Correct_Flag==1&&Round_Cnt==2)   //������ ������ ����Ϊ�󻷵�
    {
        Element_Round_Pro(0);
    }
    if(Zuo_Correct_Flag==1&&You_Correct_Flag==0&&Round_Cnt==1)   //������ �Ҳ����� ����Ϊ�һ���
    {
        Element_Round_Pro(1);
    }

}



/*
 * �����б�02
 *
 * ����sjy
������ߵĻ���
��Ϊ��ʱ �����жϵ� ���ұ��߲����

  DIR 0 �󻷵��ж� DIR 1 �һ����ж�

   ����:�˺���Ϊvoid Element_Round_and_Backcross(void)���ڲ����ú���

   ��⻡�� 5���������� 5����������
 */
void Element_Round_Pro(int DIR)
{
    uint8 i;
    uint8 rise=0,fall=0;    //���� �½� ������  ����ƫΪRISE ��ƫΪFALL
    uint8 hind_cnt=0,Hind_Flag=0;   //����ͳ�� �� ����������ϱ�־λ
    switch(DIR)
    {
        case 0:   //�󻷵�
        {
            //60���µĴ�50������
            for(i=50;i<=117;i++)
            {
                if(ImageSide[i][0]!=Zuo_Hind&&ImageSide[i+1][0]==Zuo_Hind&&ImageSide[i+2][0]==Zuo_Hind)
                {
                    hind_cnt=1;
                    break;
                }
            }
            if(hind_cnt==1)   //�Ķ�
            {
                for(i=70;i>=5;i--)    //��������Ķ��ߵ��ʱ������6 �����󻷵��жϲ���  ʹ��С�ڵ���(Zuo_Hind+3)������ڶ��ߵ�  ʹ�ô��ڵ������������ ����6��ԭ��δ֪...
                {
                    if(ImageSide[i][0]>=(Zuo_Hind+3)&&ImageSide[i-1][0]<=(Zuo_Hind+3)&&ImageSide[i-2][0]<=(Zuo_Hind+3))   //���� ��
                    {
                        Hind_Flag=1;
                        break;
                    }

                }
            }

                if(Hind_Flag==1)  //����г��� ���� ���� �ٶ������ ������һ���жϻ���
                {
                    for(i=100;i>=10;i--)  //����ѭ�� �������� ��������    �һ��� RISE�� fallС
                    {
                        if(ImageSide[i][0]==Zuo_Hind || ImageSide[i][0]==0)  //���߻����ϱ��߽���
                        {
                            continue;
                        }
                        if(ImageSide[i][0]<ImageSide[i+1][0]&&ImageSide[i][0]>ImageSide[i-1][0])
                        {
                            fall++;
                        }
                        if(ImageSide[i][0]>ImageSide[i+1][0]&&ImageSide[i][0]<ImageSide[i-1][0])
                        {
                            rise++;
                        }
                        if(rise>=16&&fall<=13&&fall>=6)
                        {
                              Round_Road_Flag=1;
                              Element_Processing_Flag=1;   //��Ԫ���ж�
                              Round_Statue=1;
                              Round_Cnt=0;  //��0�����жϻ���
                              break;
                        }
                    }

                }
                break;
        }

        case 1: //�һ���
        {
            //60���µĴ�60������
            for(i=60;i<=117;i++)
            {
                if(ImageSide[i][1]!=You_Hind&&ImageSide[i+1][1]==You_Hind&&ImageSide[i+2][1]==You_Hind)
                {
                    hind_cnt=1;
                    break;
                }
            }
            if(hind_cnt==1)
            {
                for(i=70;i>=0;i--)
                {
                    if(ImageSide[i][1]!=You_Hind&&ImageSide[i-1][1]==You_Hind&&ImageSide[i-2][1]==You_Hind)   //���� ��
                    {
                        Hind_Flag=1;
                        break;
                    }
                }
            }

                if(Hind_Flag==1)  //����г��� ���� ���� �ٶ������ ������һ���жϻ���
                {
                    for(i=100;i>=10;i--)  //����ѭ�� �������� ��������    �һ��� RISE�� fallС
                    {
                        if(ImageSide[i][1]==You_Hind || ImageSide[i][1]==0)
                        {
                            continue;
                        }
                        if(ImageSide[i][1]<ImageSide[i+1][1]&&ImageSide[i][1]>ImageSide[i-1][1])
                        {
                            rise++;
                        }
                        if(ImageSide[i][1]>ImageSide[i+1][1]&&ImageSide[i][1]<ImageSide[i-1][1])
                        {
                            fall++;
                        }
                        if(rise>=16&&fall<=13&&fall>=6)
                        {
                              Round_Road_Flag=2;
                              Element_Processing_Flag=1;   //��Ԫ���ж�
                              Round_Statue=1;
                              Round_Cnt=2;   //�ж����һ��� ������ֻ�ж��󻷵�
                              break;
                        }
                    }
                }
            }
        }
}


/*
 * ����·�ж�
 *
 * ���������������Ϸ��ĵ����ǽ����ж�
 */
void Element_Three_Pro(void)
{
    uint8 i;
    uint8 Zcnt=0,Ycnt=0;
    if(use_image[10][80]==0 &&Zuo_Correct_Flag==0&&You_Correct_Flag==0)//�ϵ���ֺڵ�  �����߶�������
    {
        for(i=100;i>=40;i--)
        {
            //�����Ѱ��
            if(ImageSide[i-1][0]<ImageSide[i][0])
            {
                Zcnt++;
            }
            //�ұ���Ѱ��
            if(ImageSide[i-1][1]>ImageSide[i][1])
            {
                Ycnt++;
            }

                if(Zcnt>=15&&Ycnt>=5)  //����ƫ����ǰ���·  ��б������
                {
                    Three_Fork_Flag=1;
                    Element_Processing_Flag=1;
                    Three_Statue=1;  //����״̬λ��1
                    break;
                }
                else if(Zcnt>=5&&Ycnt>=15)  //����ƫ����ǰ�Ҳ�·  ��б������
                {
                    Three_Fork_Flag=1;
                    Element_Processing_Flag=1;
                    Three_Statue=1;  //����״̬λ��1
                    break;
                }
                else if(Zcnt>=13&&Ycnt>=13)   //��������
                {
                    Three_Fork_Flag=1;
                    Element_Processing_Flag=1;
                    Three_Statue=1;  //����״̬λ��1
                    break;
                }
        }
        Zcnt=Ycnt=0;//����
    }
}


/*
 * ����·����  2022/5/9
 *
 * sjy
 *
 * ����ȷ��һ��ֱ�ߣ�ȷ��б�ʣ���ȷ�������е�ÿһ���㣬Ȼ��������ֱ�������а׵��ɺڵ㣬������ɨ�ߣ��Ϳ����ˡ�
 */
void Three_Processor(void)
{
        if(Three_Fork_Cnt==1||Three_Fork_Cnt==2)
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;  //1�趨�����Ƕ�ֵ
                Angle_CNT_Flag=1;

            }
            if(Angle_CNT_Flag==1)
            {
                Mid_Get_Type=1;   //���߹�����
//                Fix_Line_Flag=3;   //���߹�����
                if(ang_err<=47.00&&Angle_CNT_Flag==1)   //�ǶȴﵽĿ��ֵ�� ���־λ
                {
                    Angle_CNT_Flag=0;   //1�رսǶȼ�����
                    Three_Fork_Flag=0;  //2�������־λ
                    Three_Fork_Cnt++;  //4���������++
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Fix_Line_Flag=0;   //3�رղ���
                    Mid_Get_Type=0;
                }
            }
        }
        if(Three_Fork_Cnt==3||Three_Fork_Cnt==4||Three_Fork_Cnt==5)
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;  //1�趨�����Ƕ�ֵ
                Angle_CNT_Flag=1;
            }
            if(Angle_CNT_Flag==1)
            {
                Mid_Get_Type=1;
//                Fix_Line_Flag=3;   //�������� ������ת
                if(ang_err<=47.00&&Angle_CNT_Flag==1)
                {
                    Angle_CNT_Flag=0;   //1�رսǶȼ�����
                    Three_Fork_Flag=0;  //2�������־λ
                    Three_Fork_Cnt++;  //4���������++
                    if(Three_Fork_Cnt==6) Three_Fork_Cnt=1;  //Ϊ4����
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Mid_Get_Type=0;
                    Fix_Line_Flag=0;   //3�رղ���

                }
            }
        }
}

//˫����ͳһ����
//�˰汾����������״̬�� Three_Statue
void Three_Processor2(void)
{
    uint8 i;
    uint8 Zcnt=0,Ycnt=0;
    switch(Three_Statue)
    {
        case 1:  //�ȴ����һ�������
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;  //1�趨�����Ƕ�ֵ
                Angle_CNT_Flag=1;

            }
            if(Angle_CNT_Flag==1)
            {
                Mid_Get_Type=1;   //���߹�����
                if(ang_err<=40.00&&Angle_CNT_Flag==1)   //�ǶȴﵽĿ��ֵ�� ���־λ
                {
                    Angle_CNT_Flag=0;   //1�رսǶȼ�����
                    Fix_Line_Flag=0;   //3�رղ���
                    Mid_Get_Type=0;
                    Three_Statue=2;
                    Element_Processing_Flag=1;   //�ص�Ԫ���ж�
                }
            }
            break;
        }
        case 2:
        {
            Round_Cnt_Boring++;
            Element_Processing_Flag=1;   //�ص�Ԫ���ж�

            if(Round_Cnt_Boring>=110)     //��������ʱ �����µ�������
            {
                Round_Cnt_Boring=0;
                Three_Statue=3;
            }
            break;
        }

        case 3:  //���ڶ�������·
        {
            if(use_image[10][80]==0 &&Zuo_Correct_Flag==0&&You_Correct_Flag==0)//�ϵ���ֺڵ�  �����߶�������
            {
                for(i=100;i>=40;i--)
                {
                    //�����Ѱ��
                    if(ImageSide[i-1][0]<ImageSide[i][0])
                    {
                        Zcnt++;
                    }
                    //�ұ���Ѱ��
                    if(ImageSide[i-1][1]>ImageSide[i][1])
                    {
                        Ycnt++;
                    }

                        if(Zcnt>=13&&Ycnt>=5)  //����ƫ����ǰ���·  ��б������
                        {
                            Three_Statue=4;  //����״̬λ��1
                            Element_Processing_Flag=1;   //�ص�Ԫ���ж�

                            break;
                        }
                        else if(Zcnt>=5&&Ycnt>=13)  //����ƫ����ǰ�Ҳ�·  ��б������
                        {
                            Three_Statue=4;  //����״̬λ��1
                            Element_Processing_Flag=1;   //�ص�Ԫ���ж�

                            break;
                        }
                        else if(Zcnt>=12&&Ycnt>=12)   //��������
                        {
                            Three_Statue=4;  //����״̬λ��1
                            Element_Processing_Flag=1;   //�ص�Ԫ���ж�

                            break;
                        }
                }
                Zcnt=Ycnt=0;//����
            }
            break;
        }
        case 4:  //����ڶ��������
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;  //1�趨�����Ƕ�ֵ
                Angle_CNT_Flag=1;
            }
            if(Angle_CNT_Flag==1)
            {
                Mid_Get_Type=1;   //���߹�����
                if(ang_err<=40.00&&Angle_CNT_Flag==1)   //�ǶȴﵽĿ��ֵ�� ���־λ
                {
                    Angle_CNT_Flag=0;   //1�رսǶȼ�����

                    Three_Fork_Flag=0;  //2�������־λ
                    Element_Processing_Flag=0;   //����Ԫ���ж�

                    Fix_Line_Flag=0;
                    Mid_Get_Type=0;

                    Three_Statue=0;
                }
            }
        }
    }
}

//������Ԫ���ж�
//  Zebra_Flag==1 �󳵿�  2==�ҳ���
//�����й��� ���ҳ������ܻػ� ����Ĭ�ϴ򿪸�ʴ�˲�
//����������߻���֮�� ����ʴ�˲��ر� ����ҳ���
//��⵽�ҳ��� ִ�������
void Element_Zebra(void)
{
    uint8 i;
    uint8 cnt=0;

    if(Zuo_Correct_Flag==1&&You_Correct_Flag==0)  //�ҳ���-----ִ�����
    {
            for(i=20;i<=140;i++)
            {
//                if(use_image[40][i]==255&&use_image[40][i+1]==0) //�� ��  �ֽ��
//                {
//                    cnt++;
//                }
//                if(use_image[40][i]==255&&use_image[40][i+1]==0) //�� ��  �ֽ��
//                {
//                    cnt++;
//                }


                if(use_image[40][i]==255&&use_image[40][i+1]==0) //�� ��  �ֽ��
                {
                    cnt++;
                }
                if(use_image[40][i]==255&&use_image[40][i+1]==0) //�� ��  �ֽ��
                {
                    cnt++;
                }
                if(cnt>=7)
                {
                    Zebra_Flag=2;   //ȷ��Ϊ������  α��İ����� ��ʱӦ ���������˲���ʱ20֡��
                    Element_Processing_Flag=1;
                    Zebra_Statue=1; //������״̬��1
                    break;
                }
            }
    }
//���μ���󳵿�
//
//    if(Zuo_Correct_Flag==0&&You_Correct_Flag==1)    //������¶�Ӧֻ����һ������� �ж�Ϊ������ �������
//    {
//            for(i=10;i<=150;i++)
//            {
//                if(use_image[20][i]==0&&use_image[20][i+1]==255) //�� �� �ֽ��
//                {
//                    cnt++;
//                }
//                if(use_image[20][i]==255&&use_image[20][i+1]==0) //�� ��  �ֽ��
//                {
//                    cnt++;
//                }
//                if(cnt>=8)
//                {
//                    Zebra_Flag=1;   //ȷ��Ϊ������
//                    Element_Processing_Flag=1;
//                    break;
//                }
//            }
//    }
}




//������Ԫ�ش���
void Element_Zebra_Process(void)
{
    switch (Zebra_Flag)
    {
//        case 1:   //�����󳵿� ֱ�����
//        {
//            Mid_Get_Type=1;
//            Round_Cnt_Boring++;  //֮��ɶҲ���� ��ֹ��һ������

//            break;
//        }
        case 2:  //�����ҳ���  �߼�ִ�����
        {
            switch (Zebra_Statue)
            {
                case 1:
                {
                    Round_Cnt_Boring++;
                    if(Round_Cnt_Boring>=0)  //����Ļ18 ������  ��Ϊ�ص�����Ļ ������ٶ� ����5
                    {
                        Round_Cnt_Boring=0;
                        Zebra_Statue=2;
                    }
                    break;
                }
                case 2:
                {
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=-90.00;  //1�趨�����Ƕ�ֵ
                        Angle_CNT_Flag=1;
                        Eat_Line_Flag=1;  //��ʱ������ʴ�˲�ʹ���������
                    }
                    if(Angle_CNT_Flag==1)
                    {
                        Fix_Line_Flag=4;
                        Mid_Get_Type=0;    //�����
                        if(ang_err<=10.00 && Angle_CNT_Flag==1)   //�ǶȴﵽĿ��ֵ�� ���־λ
                        {
                            Angle_CNT_Flag=0;   //1�رսǶȼ�����
                            Element_Processing_Flag=1;   //��Ԫ���ж�
                            Fix_Line_Flag=0;

                            Mid_Get_Type=5;             //����
                            Stop_Flag=1;
                            Zebra_Statue=0;
                            while(1)
                            {
                                Sit_Flag=1;
                                Charge_Top=100.00;
                            }
                        }
                    }
                }
            }


        }
}

}

////�����ʱ���ͳ����־λ
////uint8 Out_Garage_FPS=0;
////uint8 Out_Garage_Flag=1;
////������� ��ʱ����
////�����ȳ�� �����̿����ƶ���ģ ����ֱ���� ���ǵ�ѹ���ô���15 13����֮ǰ������
//void Out_Garage(void)
//{
//    if(Out_Garage_Flag==1)
//    {
//        if(Stop_Flag==1) //�����
//        {
//            Element_Processing_Flag=1;
//            Mid_Get_Type=5;  //����
//        }
//        if(Stop_Flag==0) //������ �ٳ���   ������stop_flag=1; ֮��һ������0��ִ�г������
//        {
//            Mid_Get_Type=5;  //�ҳ���
//            Out_Garage_FPS++;
//            if(Out_Garage_FPS>=5)
//            {
//                Out_Garage_FPS=0;
//            }
//            if(Out_Garage_FPS==0)
//            {
//                if(Angle_CNT_Flag==0)
//                {
//                    angle_yaw=0;
//                    Target_Z_angle=-80.00;  //1�趨�����Ƕ�ֵ   ֻѲ�߳����ֵ��Ϊ30
//                    Angle_CNT_Flag=1;
//
//                    Element_Processing_Flag=1;
//                    Eat_Line_Flag=1;  //���⿪��ʴ�˲�
//                    Mid_Get_Type=0;  //Ѱ�ұ���
//                    Fix_Line_Flag=4;//�ҳ���
//                }
//                if(Angle_CNT_Flag==1)
//                {
//                    if(ang_err<=45.00&&Angle_CNT_Flag==1)   //�ǶȴﵽĿ��ֵ�� ���־λ
//                    {
//                        Angle_CNT_Flag=0;   //1�رսǶȼ�����
//                        Element_Processing_Flag=0;   //����Ԫ���ж�
//                        Mid_Get_Type=0;
//                        Fix_Line_Flag=0;
//                        Out_Garage_Flag=0; //������� ��־λ���� Out_Garage��������
//                    }
//                }
//            }
//        }
//    }
//}
//    }
//    }
//}

void Out_Garage2(void)
{
//    if(Stop_Flag==1) //�����
//    {
//        Element_Processing_Flag=1;
//        Mid_Get_Type=5;  //����
//    }
if(Out_Garage_Flag==1)
{
    switch (Out_Garage_statue)
    {
        case 1:   //�ȴ�������
        {
            Target_speed=0;
            Mid_Get_Type=5;  //ֱ����ʻ
            Element_Processing_Flag=1;

            if(Stop_Flag==0)
            {
                Mid_Get_Type=5;  //ֱ����ʻ
                Out_Garage_statue=2;
            }
            break;
        }
        case 2:   //�����ɺ���ʱ ֱ����ʻ5FPS
        {
            Mid_Get_Type=5;  //ֱ����ʻ

            Out_Garage_FPS++;
            if(Out_Garage_FPS>=5)    //�ص���Ļ��ʾ���ֵ���� ��ǰ10  �غ�  17
            {
                Out_Garage_FPS=0;
                Out_Garage_statue=3;
            }
            break;
        }
        case 3:  //�ƽǶȳ���
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=-75.00;  //1�趨�����Ƕ�ֵ   ֻѲ�߳����ֵ��Ϊ30
                Angle_CNT_Flag=1;

                Element_Processing_Flag=1;
                Eat_Line_Flag=1;  //���⿪��ʴ�˲�
                Mid_Get_Type=0;  //Ѱ�ұ���
                Fix_Line_Flag=4;//�ҳ���
            }
            if(Angle_CNT_Flag==1)
            {
                if(ang_err<=40.00&&Angle_CNT_Flag==1)   //�ǶȴﵽĿ��ֵ�� ���־λ
                {
                    Angle_CNT_Flag=0;   //1�رսǶȼ�����
                    Element_Processing_Flag=0;   //����Ԫ���ж�
                    Mid_Get_Type=0;
                    Fix_Line_Flag=0;
                    Out_Garage_Flag=0; //������� ��־λ���� Out_Garage��������
                    Out_Garage_statue=0;
                }
            }

        }

    }
}
}




////����ͷ��̬ǰհ
//uint8 camear_foward=80;
//
// // ǰհ ����
//uint8 zuo_line;
//// ǰհ ����
//uint8 you_line;
//// ǰհ ����
//uint8 middle_line;
////ͼ����ֵ  OTSU��
//extern uint8 image_threshold;
////��������
//uint8 ImageSide[MT9V03X_H2][2];  //0����� 1�ұ���
//uint8 ImageMid[MT9V03X_H2];  //����
//
////������־λ
//uint8 Round_Road_Flag = 0;   //1�󻷵�  2�һ���
//
////ʮ�ֱ�־λ
//uint8 Cross_Road_Flag = 0;   //��ʮ�ֺ�����תȦ���´γ�ʮ��ʱ����ת��ʮ��  ����תȦ����´�ʮ��ʱ����ת��
//
////�����־λ
//uint8 Three_Fork_Flag = 0;
//
////����2�ű�־λ  ��������һ�κ��Ϊ1
//uint8 Three_Fork_Cnt = 0;

//��Ԫ�س�������
void No_element_fix(void)
{
    if(Three_Fork_Flag==0&&Round_Road_Flag==0&&Back_Cross_Flag==0)    //Ѳ��ѡ�� ������ֱ�߾�Ѱ������
    {
        if(Zuo_Correct_Flag==0&&You_Correct_Flag==1)   //������ ������ ����Ϊ�󻷵�
        {
            Mid_Get_Type=2;
        }
        if(Zuo_Correct_Flag==1&&You_Correct_Flag==0)   //������ �Ҳ����� ����Ϊ�һ���
        {
            Mid_Get_Type=1;
        }
        else if(Zuo_Correct_Flag==0&&You_Correct_Flag==0)
        {
            Mid_Get_Type=0;
        }
        else if(Zuo_Correct_Flag==1&&You_Correct_Flag==1)
        {
            Mid_Get_Type=0;
        }
    }
}

//Ԫ���ж�   ״̬����
void Element_Judge(void)
{
    Analyse_line_straight(0);  //���ұ����ж�
    Analyse_line_straight(1);
    if(Element_Processing_Flag==0)  //û��Ԫ�����ڴ��� �����Ԫ���ж�  Element_Processing_Flag��ȷ��Ϊ��Ԫ��ʱ��Ϊ1 �͹ر����ж�Ԫ��  ��������0  Ԫ�ر�־λҲ��0
    {
            Element_Round_and_Backcross();    //Ԫ���ж�
            if(Round_Road_Flag==0)
            {
                if(Back_Cnt!=0)
                Element_BackCross3();  //������  ���عر�

                if(Back_Cross_Flag==0)
                {
                    Element_Three_Pro();  //Round_about_ProPlus();  //Element_Three_Pro(); // Element_BackCross();
                    if(Three_Fork_Flag== 0)
                    {
                        Element_Zebra();
                    }
                }
            }
            No_element_fix();
    }
}



//������  Ԫ���ж� 17��ɽ��ʡ
//����Ԫ��˳�� P T T R P R T T Z  �ػ� ���� ���� ���� �ػ� ���� ���� ���� ����
void Element_Order_Memory()
{

}



//Ԫ�ش���
void Element_Do(void)
{
    if(Back_Cross_Flag!=0)
    {
        Ring_Flag=1;//����������
        Backcross_process2();
    }
    else if(Back_Cross_Flag==0)
    {
        if(Round_Road_Flag!=0)
        {
            Ring_Flag=1;   //����������
            Round_Process_Select();
        }
        else if(Round_Road_Flag==0)
        {
            if(Three_Fork_Flag!=0)
            {
                Eat_Line_Flag=0;  //�жϵ������Ѹ�ʴ�˲��ص� �Ա������ �ڳ���֮��һֱ���Ÿ�ʴ�˲� ֱ��������һ�� ����/����
                Ring_Flag=1;   //����������
                Three_Processor();
//                Three_Processor2();
            }
            else if(Three_Fork_Flag==0)
            {
                if(Zebra_Flag!=0)
                {
                    Ring_Flag=1;   //����������
                    Element_Zebra_Process();
                }
            }
        }
    }
}

void Element(void)
{
    Element_Judge();
    Element_Do();    //�������ǿ�
    Mid_Line_Fix(Fix_Line_Flag);   //Element_Do();���ź� �ô˺�������  ������ɺ�  ��� Fix_Line_Flag
}

//�����޸�
/* ����DATA��ͬ��data.c�������ݽ����޸�
 * 1 ���� ��ת
 */
void Mid_Line_Fix(uint8 DATA)
{
    uint8 i;
    switch (DATA)
    {
        case 0:
        {
            break;
        }
        case 1:    //��ת  ����
        {
            for(i=30;i<=90;i++)
            {
                ImageMid[i]=(ImageSide[i][0]+Three_Fork_zuo[i-30])/2;
                Pixle[i][ImageMid[i]]=0;
                Pixle[i][ImageMid[i]+1]=0;
                Pixle[i][ImageMid[i]+2]=0;
            }
            break;
        }
        case 2:    //��ת  ����
        {
            for(i=30;i<=90;i++)
            {
                ImageMid[i]=(ImageSide[i][1]+Three_Fork_you[i-30])/2;
                Pixle[i][ImageMid[i]]=0;  //��ʾ�޸��������
                Pixle[i][ImageMid[i]+1]=0;
                Pixle[i][ImageMid[i]+2]=0;
            }
            break;
        }
        case 3:    //������ת
        {
            for(i=30;i<=90;i++)
            {
                ImageMid[i]=(ImageSide[i][0]+Back_Round_zuo[i-30])/2;
                Pixle[i][ImageMid[i]]=0;  //��ʾ�޸��������
                Pixle[i][ImageMid[i]+1]=0;
                Pixle[i][ImageMid[i]+2]=0;
            }
            break;
        }
        case 4:    //������ת
        {
            for(i=30;i<=90;i++)
            {
                ImageMid[i]=(ImageSide[i][1]+Back_Round_you[i-30])/2;
                Pixle[i][ImageMid[i]]=0;  //��ʾ�޸��������
                Pixle[i][ImageMid[i]+1]=0;
                Pixle[i][ImageMid[i]+2]=0;
            }
            break;
        }
    }
}




///*
// * ��С���˷�����ʽֱ�����                                                 0------------------------160
// * б�ʼ���                                                                             |                                |
// * δʵ�ʲ����뵱ǰERR���㷽ʽ���                                      |           ͼ��              |
// * ����λ��ʵ��                                                                       |                                |
// *                                                  120-----------120*160
// *
// */
//int MidSlope_get(int startline, int endline)
//{
//    //y = A + B x ;��С���˷�����ʽֱ����ϣ����Ǹ��ݸ����ĵ㣬�ü���ķ��������ѵ� a(б��) �� b���ؾࣩ����Ȼ���ؼ�����������ѵ� a �� b��
//      int i=0,SumX=0,SumY=0,SumLines = 0;
//      float SumUp=0,SumDown=0,avrX=0,avrY=0,B,A;   //B:б��   A:���Ƽ���Ľؾ�
//      SumLines=endline-startline;   // startline Ϊ��ʼ�У� //endline ������ //SumLines
//
//      for(i=startline;i<endline;i++)
//      {
//        SumX+=i;
//        SumY+=ImageMid[i];    //����Middle_blackΪ������ߵ�����
//      }
//      avrX=SumX/SumLines;     //X��ƽ��ֵ
//      avrY=SumY/SumLines;     //Y��ƽ��ֵ
//      SumUp=0;
//      SumDown=0;
//      for(i=startline;i<endline;i++)
//      {
//        SumUp+=(ImageMid[i]-avrY)*(i-avrX);
//        SumDown+=(i-avrX)*(i-avrX);
//      }
//      if(SumDown==0)
//        B=0;
//      else
//        B=(int)(SumUp/SumDown);
//        A=(SumY-B*SumX)/SumLines;  //�ؾ�
//        return B;  //����б��
//}


void Element_show(void)
{
    if(Round_Road_Flag!=0)   //�������� ������
    {
        if(Round_Road_Flag==1)
        lcd_showstr(40, 1, "R Z");
        if(Round_Road_Flag==2)
        lcd_showstr(40, 1, "R Y");
    }
    else if(Round_Road_Flag==0)
    {
        if(Three_Fork_Flag==1)  //������
        {
            lcd_showstr(40, 1, "T");
        }
        else if(Three_Fork_Flag==0)
        {
            if(Back_Cross_Flag!=0)
            {
                lcd_showstr(40, 1, "P");
            }
            else if(Back_Cross_Flag==0)
            {
                if(Zebra_Flag!=0)
                {
                    lcd_showstr(40, 1, "Z");
                }
                else if(Out_Garage_Flag!=0)
                {
                    lcd_showstr(40, 1, "OUT G");
                }
            }

        }
    }
}

