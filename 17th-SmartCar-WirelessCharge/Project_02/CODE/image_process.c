#include "headfile.h"
#include"Platform_Types.h"
#include"math.h"
#include"stdlib.h"
//https://zhuanlan.zhihu.com/p/391797246
//参考上海大学SJTU

//摄像头动态前瞻
uint8 camear_foward=60;
//上边线
uint8 up_line;

 // 前瞻 左线
uint8 zuo_line;
// 前瞻 右线
uint8 you_line;
// 前瞻 中线
uint8 middle_line;
uint8 middle_line1;
uint8 low_line;

float Line_slope;
//图像阈值  OTSU用
uint8 image_threshold;

//边线数组
uint8 ImageSide[MT9V03X_H2][2];  //0左边线 1右边线
uint8 ImageMid[MT9V03X_H2];  //中线
uint8 Road_width[MT9V03X_H2];

//环岛标志位
uint8 Round_Road_Flag = 0;   //1左环岛  2右环岛
uint8 Round_Statue=0;    //1遇到 2进入 3出环岛 4离开环岛
//出环岛 延时
uint8 Round_Cnt_Boring=0;
uint8 Round_Cnt=1;  //等于0时不再判断环岛   1时候判断右环岛 2时候判断左环岛  左环岛判断完成后不再进行环岛判断 相当于背赛道了

//uint8 Round_cnt=0; //每次判断为环岛就+1 等于2 时再不判断环岛 避免出三岔误判

//Loss_line 丢线标志位
uint8 Loss_line_Flag=0;
uint8 Loss_line=0;
//十字标志位
uint8 Cross_Road_Flag = 0;
////P回环帧数检测统计数  无元素且连续10帧数为偏移时候判断为回环 之后切线循迹 究其根源还是赛道太简单了
//sint32 Back_Road_FPS_CNT1=0;
//sint32 Back_Road_FPS_CNT2=0;
//斑马线标志位
uint8 Zebra_Flag=0;
uint8 Zebra_cnt=0;
uint8 Zebra_Statue=0;  //斑马线状态标志位 发现斑马线 先延时后打脚

//三叉标志位
uint8 Three_Fork_Flag = 0;

//三叉2号标志位  三叉运行一次后记为1
uint8 Three_Fork_Cnt = 1;
uint32 Three_Fork_Cnt_Pro = 0;

uint8 Three_Statue=0;//三岔状态标志位



//回环标志位
uint8 Back_Cross_Flag=0;

//出回环标志位
uint8 Back_Out_Flag=0;
uint8 Back_Out_Boring=0;

//回环状态
uint8 Back_Cross_Statue=0;
uint8 Back_Cnt=2;  //1时候左转 2时候右转   第一个回环不处理了 只处理第二个回环

//补线类型 放Fix_Line里
uint8 Fix_Line_Flag=0;  //1左 2右

//元素正在处理标志位 元素处理时关元素判断
uint8 Element_Processing_Flag=0;

//目标角度改变标志位  达到目标角度进行下一步操作或者退出操作
uint8 Angle_CNT_Flag=0;

//左右边线正常与否标志位 1正常 0不正常
uint8 Zuo_Correct_Flag=1;
uint8 You_Correct_Flag=1;

//结束标志 1 寻左线入库 2 寻右线入库
uint8 End_Flag=0;

//左右数组丢线时的值
uint8 Zuo_Hind=3;
uint8 You_Hind=157;

//出库计时器和出库标志位
uint8 Out_Garage_FPS=1;
uint8 Out_Garage_Flag=1;
//出库状态标志位
uint8 Out_Garage_statue=1;


uint8 Eat_Line_Flag=0;   //腐蚀滤波标志位 1为开 0为关

uint8 Judge_Stright_Amount=25;//直线判断量 以25/50符合要求为判断条件  速度1 25   速度2 20 速度3 20



//完全结束标志位
//uint8 Finish_Flag=0;
/*
  *边线寻找
  *普通算法
 * v1.1
 */
void seek_line(void)
{
    uint8 h=0,w;          //middle_seed每次寻线完成后更新 使下次寻线中心起始点为上一次中点   h:高度遍历  w:宽度遍历
    uint8 zuo_seed=0,you_seed=0,middle_seed=MT9V03X_W2/2;
    for(h=MT9V03X_H2-1;h>0;h--)  //遍历高度
    {
        for(w=middle_seed;w>0+3;w--) //从中线--向左寻找 宽度遍历
        {
            if(use_image[h][w-2]==0&&use_image[h][w-1]==0&&use_image[h][w]==255)
            {
                break;
            }
        }
            zuo_seed=w;   //左边界线

        for(w=middle_seed;w<MT9V03X_W2-3;w++)    //从中线++向右寻找  宽度遍历
        {
            if(use_image[h][w+2]==0&&use_image[h][w+1]==0&&use_image[h][w]==255)   //补线时补两条线
            {
                break;
            }
        }
           you_seed=w;          //边界线
           middle_seed=(zuo_seed+you_seed)/2;           //中线



           up_line=0;
           if(h>1&&use_image[h-1][middle_seed]==0)       //防止上边线封锁 影响走线
           {
               low_line=h;
               for(int k=0;k<MT9V03X_W2;k++)    //画线封锁
               {
                     low_line=h;
//                   lcd_drawpoint(k,h,YELLOW);
                     Pixle[h][k]=0;
               }
               up_line = h;
               break;       //上边检测不到 停止遍历
           }
           if(h==camear_foward)  //储存 前瞻处 左右中点坐标
           {
               zuo_line=zuo_seed;
               you_line=you_seed;
               middle_line=middle_seed;
           }

//        lcd_drawpoint(zuo_seed,h,RED);
//        lcd_drawpoint(you_seed,h,RED);
//        lcd_drawpoint(middle_seed,h,GREEN);

           Pixle[h][zuo_seed]=0;    //边线图像
           Pixle[h][you_seed]=0;
           Pixle[h][middle_seed]=0;

           Road_width[h]=you_seed-zuo_seed;  //赛道宽度

           ImageSide[h-1][0]=zuo_seed;  //边线记录到数组
           ImageSide[h-1][1]=you_seed;
           ImageMid[h-1]=middle_seed;

    }
}



/*调试用
 *生成边线图-简约
 */
void Pixle_Show(void)
{
    uint8 i,j;
    for(i=0;i<MT9V03X_H2;i++)
    {
        for(j=0;j<MT9V03X_W2;j++)
        {
            Pixle[i][j]=255;   //初始化全白
        }
    }
//    seek_line();
//    lcd_displayimage032(Pixle[0],MT9V03X_W2,MT9V03X_H2);
    //seekfree_sendimg_03x(UART_3,Pixle[0],MT9V03X_W2,MT9V03X_H2);
}



/*
 * OTSU计算动态二值化阈值
 *
 *100-200
 */
uint8 otsuThreshold_Pro(uint8 *image, uint16 width, uint16 height)  //阈值实时计算
{
   float pixelPro[256]={0.0};
   float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
   int pixelCount[256]={0};
   int i, j, pixelSum = width * height;//pixelSum =列*行=宽*高=面积
   uint8 threshold = 0;
   //统计灰度级中每个像素在整幅图像中的个数
   for (i = 0; i < height; i=i+2)//这里就他妈是把每个像素点的值作为pixelCount的下角标
   {
      for (j = 0; j < width; j=j+2)
        {
            pixelCount[(int)image[i * width + j]]++;  //将像素值作为计数数组的下标，记录每个像素值出现的概率
        }
   }

   for (i = 0; i < 256; i=i+2)
    {
      pixelPro[i] = (float)pixelCount[i] / pixelSum;//上面自增的玩意除以面积，从而取得每个像素值出现的概率
    }
   //         0        256
       for (i = 100; i < 201; i=i+3)     // i作为阈值  运算速度为之前53倍
    {
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
          //         256
        for (j = 0; j < 256; j=j+3)
        {
            if (j <= i)   //背景部分
            {
                w0 += pixelPro[j];//j <= i的概率之和
                u0tmp += j * pixelPro[j];////j <= i的j与概率之积
            }
            else   //前景部分
            {
                w1 += pixelPro[j];//j > i的概率之和
                u1tmp += j * pixelPro[j];//j > i的j与概率之积
            }
        }
        u0 = u0tmp / w0;//（j <= i的j与概率之积）除以（j <= i的概率之和）
        u1 = u1tmp / w1;//（j > i的j与概率之积）除以（j > i的概率之和）
        u = u0tmp + u1tmp;//（j <= i的j与概率之积）加（j > i的j与概率之积）
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);//
        if (deltaTmp > deltaMax)//多次循环取最大阈值
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
  * 图像二值化    遍历
 * v1.0
 */
void Binaryzation(uint8 *image,uint16 width, uint16 height,uint8 image_threshold) //二值化
{
   int i=0;
   for(i=0;i<height*width;i++)
   {
       if(image[i]>=image_threshold) image[i]=255;
       else image[i]=0;
   }
}

/*
 * 图像裁剪
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
//  @brief 以下限幅保护部分
//  Sample usage:
//----------------------------------
int32 range_protect(uint8 duty, uint8 min, uint8 max)//限幅保护
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
//  @brief 以下为腐蚀滤波算法
//  Sample usage:  腐蚀滤波是将干扰滤除，并统计白黑跳变点数量，用于
//                 起跑线检测，本滤波可将白点之后10个以内的黑点滤掉，
//                 黑点超过10个，则认为不是干扰，需寻找另一个白点，
//                 之后重复之前的过程，滤波同时记录了白黑跳变点的个数
//
//          发车时 出车库使用此功能 滤掉 斑马线
//
//----------------------------------
uint8 Test_Jump;
uint8 Corrode_Filter(uint8 i, uint8 *image, uint8 Left_Min, uint8 Right_Max)//1,159，i是行数，date
{
     uint8 j;
     uint8 White_Flag = 0;
     uint8 Jump_Count = 0;  // 跳变点计数
     Test_Jump = 0;

     if(Right_Max>MT9V03X_W2-1)Right_Max=MT9V03X_W2-1;
     if(Right_Max<1)Right_Max=1;

     for (j = Left_Min; j <= Right_Max; j++)    // 从左向右扫描，方向不影响结果
    {
        if (!White_Flag)    // 先查找白点，只滤黑点，不滤白点
        {
            if (image[i*MT9V03X_W2 + j]==255)   //返回值为1，检测到白点，0为黑点
            {
                White_Flag = 1; // 开始找黑点
            }
        }
        else
        {
            if (image[i*MT9V03X_W2 + j]==0)  // 检测到黑点
            {
                Jump_Count++;   // 视为一次跳变，将跳变累计

                Test_Jump = Jump_Count;

                if (image[i*MT9V03X_W2 + j+1]==0 && j+1 <= Right_Max)    // 连续两个黑点
                {
                    if (image[i*MT9V03X_W2 + j+2]==0 && j+2 <= Right_Max)    // 连续三个黑点
                    {
                        if (image[i*MT9V03X_W2 + j+3]==0 && j+3 <= Right_Max)    // 连续四个黑点
                        {
                            if (image[i*MT9V03X_W2 + j+4]==0 && j+4 <= Right_Max)    // 连续五个黑点
                            {
                                if (image[i*MT9V03X_W2 + j+5]==0 && j+5 <= Right_Max)    // 连续六个黑点
                                {
                                    if (image[i*MT9V03X_W2 + j+6]==0 && j+6 <= Right_Max)    // 连续七个黑点
                                    {
                                        if (image[i*MT9V03X_W2 + j+7]==0 && j+7 <= Right_Max)    // 连续八个黑点
                                        {
                                            if (image[i*MT9V03X_W2 + j+8]==0 && j+8 <= Right_Max)    // 连续九个黑点
                                            {
                                                if (image[i*MT9V03X_W2 + j+9]==0 && j+9 <= Right_Max)    // 连续十个黑点
                                                {
                                                    if (image[i*MT9V03X_W2 + j+10]==0 && j+10 <= Right_Max)  // 连续11个黑点
                                                    {
                                                        White_Flag = 0; // 认为不是干扰，不做任何处理，下次搜索白点
                                                        j += 10;
                                                    }
                                                    else if (j+10 <= Right_Max)
                                                    {
                                                        image[i*MT9V03X_W2 + j] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+1] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+2] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+3] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+4] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+5] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+6] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+7] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+8] = 255; // 仅有连续10个黑点，滤除掉
                                                        image[i*MT9V03X_W2+j+9] = 255; // 仅有连续10个黑点，滤除掉

                                                        j += 10;
                                                    }
                                                    else
                                                    {
                                                        j += 10;
                                                    }
                                                }
                                                else if (j+9 <= Right_Max)
                                                {
                                                    image[i*MT9V03X_W2 + j] = 255; // 仅有连续九个黑点，滤除掉
                                                    image[i*MT9V03X_W2+j+1] = 255; // 仅有连续九个黑点，滤除掉
                                                    image[i*MT9V03X_W2+j+2] = 255; // 仅有连续九个黑点，滤除掉
                                                    image[i*MT9V03X_W2+j+3] = 255; // 仅有连续九个黑点，滤除掉
                                                    image[i*MT9V03X_W2+j+4] = 255; // 仅有连续九个黑点，滤除掉
                                                    image[i*MT9V03X_W2+j+5] = 255; // 仅有连续九个黑点，滤除掉
                                                    image[i*MT9V03X_W2+j+6] = 255; // 仅有连续九个黑点，滤除掉
                                                    image[i*MT9V03X_W2+j+7] = 255; // 仅有连续九个黑点，滤除掉
                                                    image[i*MT9V03X_W2+j+8] = 255; // 仅有连续九个黑点，滤除掉

                                                    j += 9;
                                                }
                                                else
                                                {
                                                    j += 9;
                                                }
                                            }
                                            else if (j+8 <= Right_Max)
                                            {
                                                image[i*MT9V03X_W2 + j] = 255; // 仅有连续八个黑点，滤除掉
                                                image[i*MT9V03X_W2+j+1] = 255; // 仅有连续八个黑点，滤除掉
                                                image[i*MT9V03X_W2+j+2] = 255; // 仅有连续八个黑点，滤除掉
                                                image[i*MT9V03X_W2+j+3] = 255; // 仅有连续八个黑点，滤除掉
                                                image[i*MT9V03X_W2+j+4] = 255; // 仅有连续八个黑点，滤除掉
                                                image[i*MT9V03X_W2+j+5] = 255; // 仅有连续八个黑点，滤除掉
                                                image[i*MT9V03X_W2+j+6] = 255; // 仅有连续八个黑点，滤除掉
                                                image[i*MT9V03X_W2+j+7] = 255; // 仅有连续八个黑点，滤除掉

                                                j += 8;
                                            }
                                            else
                                            {
                                                j += 8;
                                            }
                                        }
                                        else if (j+7 <= Right_Max)
                                        {
                                            image[i*MT9V03X_W2 + j] = 255; // 仅有连续七个黑点，滤除掉
                                            image[i*MT9V03X_W2+j+1] = 255; // 仅有连续七个黑点，滤除掉
                                            image[i*MT9V03X_W2+j+2] = 255; // 仅有连续七个黑点，滤除掉
                                            image[i*MT9V03X_W2+j+3] = 255; // 仅有连续七个黑点，滤除掉
                                            image[i*MT9V03X_W2+j+4] = 255; // 仅有连续七个黑点，滤除掉
                                            image[i*MT9V03X_W2+j+5] = 255; // 仅有连续七个黑点，滤除掉
                                            image[i*MT9V03X_W2+j+6] = 255; // 仅有连续七个黑点，滤除掉

                                            j += 7;
                                        }
                                        else
                                        {
                                            j += 7;
                                        }
                                    }
                                    else if (j+6 <= Right_Max)
                                    {
                                        image[i*MT9V03X_W2 + j] = 255; // 仅有连续六个黑点，滤除掉
                                        image[i*MT9V03X_W2+j+1] = 255; // 仅有连续六个黑点，滤除掉
                                        image[i*MT9V03X_W2+j+2] = 255; // 仅有连续六个黑点，滤除掉
                                        image[i*MT9V03X_W2+j+3] = 255; // 仅有连续六个黑点，滤除掉
                                        image[i*MT9V03X_W2+j+4] = 255; // 仅有连续六个黑点，滤除掉
                                        image[i*MT9V03X_W2+j+5] = 255; // 仅有连续六个黑点，滤除掉

                                        j += 6;
                                    }
                                    else
                                    {
                                        j += 6;
                                    }
                                }
                                else if (j+5 <= Right_Max)
                                {
                                    image[i*MT9V03X_W2 + j] = 255; // 仅有连续五个黑点，滤除掉
                                    image[i*MT9V03X_W2+j+1] = 255; // 仅有连续五个黑点，滤除掉
                                    image[i*MT9V03X_W2+j+2] = 255; // 仅有连续五个黑点，滤除掉
                                    image[i*MT9V03X_W2+j+3] = 255; // 仅有连续五个黑点，滤除掉
                                    image[i*MT9V03X_W2+j+4] = 255; // 仅有连续五个黑点，滤除掉

                                    j += 5;
                                }
                                else
                                {
                                    j += 5;
                                }
                            }
                            else if (j+4 <= Right_Max)
                            {
                                image[i*MT9V03X_W2 + j] = 255; // 仅有连续四个黑点，滤除掉
                                image[i*MT9V03X_W2+j+1] = 255; // 仅有连续四个黑点，滤除掉
                                image[i*MT9V03X_W2+j+2] = 255; // 仅有连续四个黑点，滤除掉
                                image[i*MT9V03X_W2+j+3] = 255; // 仅有连续四个黑点，滤除掉

                                j += 4;
                            }
                            else
                            {
                                j += 4;
                            }
                        }
                        else if (j+3 <= Right_Max)
                        {
                            image[i*MT9V03X_W2 + j] = 255; // 仅有连续三个黑点，滤除掉
                            image[i*MT9V03X_W2+j+1] = 255; // 仅有连续三个黑点，滤除掉
                            image[i*MT9V03X_W2+j+2] = 255; // 仅有连续三个黑点，滤除掉

                            j += 3;
                        }
                        else
                        {
                            j += 3;
                        }
                    }
                    else if (j+2 <= Right_Max)
                    {
                        image[i*MT9V03X_W2 + j] = 255; // 仅有连续两个黑点，滤除掉
                        image[i*MT9V03X_W2+j+1] = 255; // 仅有连续两个黑点，滤除掉

                        j += 2;
                    }
                    else
                    {
                        j += 2;
                    }
                }
                else if (j+1 <= Right_Max)
                {
                    image[i*MT9V03X_W2 + j] = 255; // 有一个黑点，滤除掉

                    j += 1;
                }
                else
                {
                    j += 1;
                }
            }
        }
    }
  return Jump_Count;    // 返回跳变点计数
}


/*
 * 全图 腐蚀滤波
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
  *         除单一的噪点
  *         检测黑点边缘全白或者 白点边缘全黑  符合则同化
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
 * 1剪切图像
 * 2动态计算二值化阈值
 * 3二值化
 * 4腐蚀图像滤波修复10黑点   起跑检测用
  *    使用图像大小 160*120
  *
 */
void image_process1(void)
{
    Get_use_image();

    image_threshold = otsuThreshold_Pro(use_image[0],MT9V03X_W2,MT9V03X_H2);  //OTSU计算动态阈值

    Binaryzation(use_image[0],MT9V03X_W2,MT9V03X_H2,image_threshold);  //图像二值化

//    OnePoint_fix();

    if(Eat_Line_Flag==1)fix_image();  //腐蚀滤波  处理


//    fix_image();  //腐蚀滤波  处理

}
//// 前瞻 左线
//uint8 zuo_line;
//// 前瞻 右线
//uint8 you_line;
//// 前瞻 中线
//uint8 middle_line;
////图像阈值  OTSU用
//extern uint8 image_threshold;
////边线数组
//uint8 ImageSide[MT9V03X_H2][2];  //0左边线 1右边线
//uint8 ImageMid[MT9V03X_H2];  //中线

/****************************元素处理****************************/

/*
 * 十字判别
 *v1.0
 *注意斜入十字
 */
void Element_Cross(void)
{

}

//十字处理
void Element_Cross_Process(void)
{

}
/*
 * P回环判断
 *判断完后执行回环下一步判断 判断认定为出回环则补线左右转
 * 可能与 车库环岛误判  后期加斑马线限制条件  限制住
 *
 * 先边线异 最下面不丢 往上丢一段 再不丢 再丢一小段 并且Head_err也偏
 * 作者 sjy  2022 - 7 - 14
 */
void Element_BackCross(void)
{
    uint8 i;
    uint8 cnt1=0,cnt2=0;   //左右的单点标志位
    int loss_start_point=0; //丢线起始点
    int loss_point1=0; //


    uint8 Back_Type_Maybe=0; //为1 左回环判断  为2右回环判断 为0不判断 节省时间

    if(Zuo_Correct_Flag==0&&You_Correct_Flag==1)  //可能为左回环
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
                case 1:   //左  检测上下两段丢线
                {
                    for(i=110;i>=50;i--)  //先向上找 找丢线起始点
                    {
                        if(ImageSide[i][0]>=(Zuo_Hind+3)&&ImageSide[i-1][0]<=(Zuo_Hind+3)&&ImageSide[i-2][0]<=(Zuo_Hind+3)) //丢 不丢 不丢
                        {
                            cnt1++;
                            loss_start_point=i;
                            break;
                        }
                    }
                    if(cnt1!=0)
                    {
                        for(i=(loss_start_point-5);i>=(loss_start_point-60);i--)  //丢线起始点向上移5像素 开始循迹 找丢线结束点
                        {
                            if(ImageSide[i][0]<=(Zuo_Hind+3)&&ImageSide[i-1][0]>=(Zuo_Hind+3)&&ImageSide[i-2][0]>=(Zuo_Hind+3))//丢 丢 不丢 点
                            {
                                cnt1++;
                                loss_point1=i;
                                break;
                            }
                        }
                    }
                    if(cnt1==2)
                    {
                        for(i=(loss_point1-5);i>=0;i--)  //再次向上5像素 向上
                        {
                            if(ImageSide[i][0]<=(Zuo_Hind+3))cnt2++;  //丢
                            if(cnt2>=0)break;
                        }
                        if(cnt2==0)
                        {
                            Back_Cross_Flag=1;  //左回环
                            Element_Processing_Flag=1;
                            Back_Cross_Statue=1;
                        }
                    }
                    break;
                }
                case 2:   //右回环判断 右回环实验室不反光 使用上下两点法判断 因为摄像头高的原因回环可以看到两段丢线点
                {
                    for(i=110;i>=50;i--)  //先向上找 找丢线起始点
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
                        for(i=(loss_start_point-5);i>=(loss_start_point-60);i--)  //丢线起始点向上移10像素 开始循迹 找丢线结束点
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
                            if(cnt2>=0)break;    //向上检测到丢线点 就确定为 两段丢线
                        }
                        if(cnt2==0)
                        {
                            Back_Cross_Flag=2;  //左回环
                            Element_Processing_Flag=1;
                            Back_Cross_Statue=1;
                        }
                    }
                }
            }
        }
}


//回环判断2
//通过下边线多行丢线以及此时中线偏差 来判断P回环
//作者 孙继一 2022-07-15-16:36
void Element_BackCross2(void)
{
 uint8 i;
 uint8 Down_Loss_Cnt=0;  //下边线丢线行数 计数器
 uint8 Back_Maybe=0;  //1 左回环 2 右回环
 int Mid_001;  //中线误差
 uint8 Back_Flag=0;
 Mid_001 = (78-ImageMid[30])*0.25+(78-ImageMid[40])*0.25+(78-ImageMid[50])*0.25+(78-ImageMid[60])*0.25;

 if(TestVolt<=700&&TestVolt>=400)   //左右都不单调时判断 选择性删除
 {
     if(ImageSide[117][0]==Zuo_Hind)  //左边线下方丢线
     {
         Back_Maybe=1;  //可能为左回环
     }
     else if(ImageSide[117][1]==You_Hind)  //右边线下方丢线
     {
         Back_Maybe=2;  //可能为左回环
     }
     if(Back_Maybe!=0)
     {
         Down_Loss_Cnt=0;
         switch (Back_Maybe)
         {
             case 1: //左回环进一步判断
             {
                 for(i=118;i>=10;i-=2)  //下往上循迹  巡点54次
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
                 if(Mid_001>=0&&Back_Flag==1)  //左P
                 {
                     Back_Cross_Flag=1;
                     Element_Processing_Flag=1;
                     Back_Cross_Statue=1;      //确定为左回环
                 }
                 break;
             }
             case 2:  //右回环进一步判断
             {
                 for(i=118;i>=10;i-=2)  //下往上循迹  巡点54次
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
                 if(Mid_001<=0&&Back_Flag==1)  //右P
                 {
                     Back_Cross_Flag=2;
                     Element_Processing_Flag=1;
                     Back_Cross_Statue=1;            //确定为右回环
                 }
             }
         }
     }


 }
}


//回环判断3.0
//判断左右丢线且前方看到很少
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

    if(use_image[20][20]==0&&use_image[20][80]==0&&use_image[20][140]==0&&low_line>=30)  //上方三黑
    {
        Black_Correct=1;
    }
    else Black_Correct=0;

    if(Black_Correct==1&&TestVolt>=300&&TestVolt<=500)  //上方丢线行数超过40 进入进一步判断  三岔处有误判 故小于等于800
    {
        for(i=110;i>=10;i-=2) //从底部向上循迹  87
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
                Element_Processing_Flag=1;  //关元素判断
//                break;
            }
            else if(cnt1>=30&&cnt2>=5)
            {
                Back_Cross_Flag=1;
                Element_Processing_Flag=1;  //关元素判断
//                break;
            }
            else if(cnt1>=5&&cnt2>=30)
            {
                Back_Cross_Flag=1;
                Element_Processing_Flag=1;  //关元素判断
//                break;
            }
        }
    }
}

//回环判断4.0
//判断左右丢线且前方看到很少
void Element_BackCross4(void)
{
    uint8 i;
    uint8 cnt1=0,cnt2=0;
    uint8 BINGO_FLAG=0;
    uint8 Black_Correct=0;
    if(use_image[20][20]==0&&use_image[20][80]==0&&use_image[20][140]==0&&low_line>=20)  //且丢线大于某值
    {
        Black_Correct=1;
    }
    else Black_Correct=0;

    if(Black_Correct==1)  //上方丢线行数超过40 进入进一步判断  三岔处有误判 故小于等于800
    {
        for(i=117;i>=15;i-=2) //从底部向上循迹  87
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
            for(i=117;i>=15;i-=2) //从底部向上循迹  87
            {
                if(ImageSide[i][0]==Zuo_Hind)
                {
                    cnt2++;
                }
                if(cnt2>=20)
                {
                    Back_Cross_Flag=1;
                    Element_Processing_Flag=1;  //关元素判断
                    break;
                }
            }
        }
    }
}

//第一个回环左转 第二个右转
void Backcross_process2(void)
{
    switch (Back_Cnt)
    {
        case 1:  //第一个回环右转
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=-60.00;
                Angle_CNT_Flag=1;
                Fix_Line_Flag=4;  //右转补线
                Mid_Get_Type=0;
            }
            if(Angle_CNT_Flag==1)
            {
                if(ang_err<=10.00&&Angle_CNT_Flag==1)    //结束
                {
                    Back_Cnt++;
                    Angle_CNT_Flag=0;
                    Fix_Line_Flag=0;  //补线关闭  .
                    Element_Processing_Flag=0;   //重启元素判断
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
                Fix_Line_Flag=3;  //左转补线
            }
            if(Angle_CNT_Flag==1)
            {
                if(ang_err<=10.00&&Angle_CNT_Flag==1)    //结束
                {
                    Back_Cnt=0;
                    Angle_CNT_Flag=0;
                    Fix_Line_Flag=0;  //补线关闭  .
                    Element_Processing_Flag=0;   //重启元素判断
                    Back_Cross_Flag=0;
                }
            }
        }
    }

}

//P字处理

//搞个跟环岛一样的的状态处理机
void Backcross_process(void)
{
    switch (Back_Cross_Flag)
    {
        case 1: //左回环
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

//                        Mid_Get_Type=2;//右转
                        Fix_Line_Flag=4;  //右转补线
                    }
                    if(Angle_CNT_Flag==1)
                    {
                        if(ang_err<=10.00&&Angle_CNT_Flag==1)    //结束
                        {
                            Angle_CNT_Flag=0;
                            Back_Cross_Statue=2;

//                            Mid_Get_Type=0;//右转
                            Fix_Line_Flag=0;  //右转补线  .
                            Element_Processing_Flag=0;   //重启元素判断
                            Back_Cross_Flag=0;
                        }
                    }
                }
            }
            break;
        }
        case 2:  //右回环
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

//                        Mid_Get_Type=1;//左转
                        Fix_Line_Flag=3;  //左转补线
                    }
                    if(Angle_CNT_Flag==1)
                    {
                        if(ang_err<=10.00&&Angle_CNT_Flag==1)    //结束
                        {
                            Angle_CNT_Flag=0;
                            Back_Cross_Statue=0;

//                            Mid_Get_Type=0;
                            Fix_Line_Flag=0;
                            Element_Processing_Flag=0;   //重启元素判断
                            Back_Cross_Flag=0;
                        }
                    }
                }
            }

        }


    }

}


/*
 * 判断边线是否单调 正常
    dir 0 左边线判断 dir 1 右边线   3 左右都判断 3时返回    0为都不单调 1 左不单调 2 右不单调 3都单调
    返回1则单调 0 不单调 有突变 不是直线特征
 */
void Analyse_line_straight(uint8 dir)  //返回0则不单调 存在突变 返回1则边线单调
{
    uint8 i;
    uint8 cnt=0;
    int slope1=0,slope2=0,slope3=0;
    int slope_dec = 0;

    switch (dir)
    {
        case 0:
        {
            for(i=110;i>=10;i-=2)  //计数60 MAX (120-lowline)/2
            {
                if(i==100)
                slope1 = ImageSide[i-10][0] - ImageSide[i+10][0];
                if(i==60)
                slope2 = ImageSide[i-10][0] - ImageSide[i+10][0];
                if(i==20)
                slope3 = ImageSide[i-10][0] - ImageSide[i+10][0];



                if(ImageSide[i][0] == Zuo_Hind)   //遇到靠边情况不计数
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
                        Zuo_Correct_Flag=1;   //正确的点大于50 则认为整条线正确
                        break;
                    }
                }
                else if( cnt< Judge_Stright_Amount )
                {
                    Zuo_Correct_Flag=0;   //不单调
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

                if(ImageSide[i][1] == You_Hind)   //遇到靠边情况不计数
                {
                    continue;
                }
                if(ImageSide[i][1]<ImageSide[i+5][1] && ImageSide[i][1]>ImageSide[i-5][1] ) //与预置正常线比较
                {
                    cnt++;
                }
                if( cnt>= Judge_Stright_Amount )
                {
                    slope_dec = (abs(slope1-slope2) + abs(slope1-slope3) + abs(slope2-slope3))/3;
                    if(slope_dec<=5)
                    {
                        You_Correct_Flag=1;   //单调
                        break;
                    }
                }
                else if( cnt< Judge_Stright_Amount )
                {
                    You_Correct_Flag=0;//不单调 有突变可能
                }
            }
        }
    }
}
//环岛程序根据速度调整
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


//带充电检测的环岛执行程序
//因为环岛一进就偏 一偏就充不进去了
void Round_Processer_Charge1(void)
{
    switch (Round_Road_Flag)
    {
        case 1:         //左环岛  可以了 别动了//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            switch (Round_Statue)
            {
                case 1:  //左环岛先寻右线保证直线行驶
                {
                    Mid_Get_Type=4;  //先巡右线保证直线行驶   这时候寻右边线 改为模式4 寻靠下面的右边线
                    if(Stop_Flag==1) //停车之后
                    {
                        Round_Statue=2;
                    }
                    break;
                }
                case 2:  //角度计数开  该打角进环岛了  打角进环岛 左环岛依靠左边线进环岛 到角度附近换循右边线 因为此时左边线内环线 已经是看不到了
                {
                    Mid_Get_Type=3;  //检测到弧线就寻弧线 寻左弧线进环岛
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=60.00;  //1设定期望角度值
                        Angle_CNT_Flag=1;  //开角度计数器
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度  25度误差允许
                    {
                        Round_Statue=3;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 3:
                {
                    Mid_Get_Type=4;   //进环岛后换循右边线 保证稳定性    2改4 下右边线  左环岛不稳定可能为上方丢线导致分量为0 与常规不符合
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=130.00;  //1设定期望角度值
                        Angle_CNT_Flag=1;  //开角度计数器
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
                        Round_Statue=4;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 4:   //出左环岛寻左下弧线
                {
                    Mid_Get_Type=3;
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=120.00;  //1设定期望角度
                        Angle_CNT_Flag=1;  //开角度计数器
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
                        Round_Statue=5;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 5:   //出环时候换下右线 寻直线 找充电点
                {
                    Mid_Get_Type=2;   //换右线 出环保持稳定 此时左线是个直线
                    Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判
                    if(Round_Cnt_Boring==30)  //环岛大结局  出环也是在充电 可以大点
                    {
                        Round_Cnt_Boring=0;
                        Round_Statue=0;
                        Round_Road_Flag=0;
                        Element_Processing_Flag=0;   //重启元素判断
                        Mid_Get_Type=0;
                    }
                }
            }
            break;
        }
        case 2:         //右环岛//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            switch (Round_Statue)
            {
                case 1:  //右环岛先寻左线保证直线行驶
                {
                    Mid_Get_Type=1;  //先巡左线保证直线行驶   这时候寻左下边线 改为模式3 寻靠下面的左边线
                    if(Stop_Flag==1) //停车之后
                    {
                        Round_Statue=2;
                    }
                    break;
                }
                case 2:  //角度计数开  该打角进环岛了 此时切换线路到左环岛的弧线
                {
//                    Fix_Line_Flag=2;
                    Mid_Get_Type=4;  //检测到弧线就寻弧线
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=-50.00;  //1设定期望角度值
//                        Target_Z_angle = angle_yaw - 60.00;  //角度统计90度
                        Angle_CNT_Flag=1;  //开角度计数器
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
//                        Fix_Line_Flag=0;
                        Round_Statue=3;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 3:
                {
                    Mid_Get_Type=3;  //环内 换寻左线
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=-200.00;  //1设定期望角度值
//                         Target_Z_angle = angle_yaw - 200.00;  //角度统计200度
                         Angle_CNT_Flag=1;  //开角度计数器
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
                        Round_Statue=4;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 4:   //换下弧线
                {
                    Mid_Get_Type=2;
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=-45.00;  //1设定期望角度值
                        Angle_CNT_Flag=1;  //开角度计数器
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
                        Round_Statue=5;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 5:   //出环  换下左线
                {
                    Mid_Get_Type=1;
                    Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判
                    if(Round_Cnt_Boring==30)  //环岛大结局
                    {
                        Round_Cnt_Boring=0;
                        Round_Statue=0;
                        Round_Road_Flag=0;
                        Element_Processing_Flag=0;   //重启元素判断
                        Mid_Get_Type=0;
                    }
                }
            }
        }

    }
}
//带充电检测的环岛执行程序
//因为环岛一进就偏 一偏就充不进去了
void Round_Processer_Charge2(void)
{
    switch (Round_Road_Flag)
    {
        case 1:         //左环岛  可以了 别动了//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            switch (Round_Statue)
            {
                case 1:  //左环岛先寻右线保证直线行驶
                {
                    Mid_Get_Type=4;  //先巡右线保证直线行驶   这时候寻右边线 改为模式4 寻靠下面的右边线
                    if(Stop_Flag==1) //停车之后
                    {
                        Round_Statue=2;
                    }
                    break;
                }
                case 2:  //角度计数开  该打角进环岛了  打角进环岛 左环岛依靠左边线进环岛 到角度附近换循右边线 因为此时左边线内环线 已经是看不到了
                {
                    Mid_Get_Type=3;  //检测到弧线就寻弧线 寻左弧线进环岛
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=60.00;  //1设定期望角度值
                        Angle_CNT_Flag=1;  //开角度计数器
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度  25度误差允许
                    {
                        Round_Statue=3;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 3:
                {
                    Mid_Get_Type=4;   //进环岛后换循右边线 保证稳定性    2改4 下右边线  左环岛不稳定可能为上方丢线导致分量为0 与常规不符合
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=190.00;  //1设定期望角度值
                        Angle_CNT_Flag=1;  //开角度计数器
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
                        Round_Statue=4;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 4:   //出左环岛寻左下弧线
                {
                    Mid_Get_Type=1;
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=60.00;  //1设定期望角度
                        Angle_CNT_Flag=1;  //开角度计数器
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
                        Round_Statue=5;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 5:   //出环时候换下右线 寻直线 找充电点
                {
                    Mid_Get_Type=2;   //换右线 出环保持稳定 此时左线是个直线
                    Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判
                    if(Round_Cnt_Boring==30)  //环岛大结局  出环也是在充电 可以大点
                    {
                        Round_Cnt_Boring=0;
                        Round_Statue=0;
                        Round_Road_Flag=0;
                        Element_Processing_Flag=0;   //重启元素判断
                        Mid_Get_Type=0;
                    }
                }
            }
            break;
        }
        case 2:         //右环岛//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        {
            switch (Round_Statue)
            {
                case 1:  //右环岛先寻左线保证直线行驶
                {
                    Mid_Get_Type=1;  //先巡左线保证直线行驶   这时候寻左下边线 改为模式3 寻靠下面的左边线
                    if(Stop_Flag==1) //停车之后
                    {
                        Round_Statue=2;
                    }
                    break;
                }
                case 2:  //角度计数开  该打角进环岛了 此时切换线路到左环岛的弧线
                {
//                    Fix_Line_Flag=2;
                    Mid_Get_Type=4;  //检测到弧线就寻弧线
                    if(Angle_CNT_Flag==0)
                    {
                        angle_yaw=0;
                        Target_Z_angle=-50.00;  //1设定期望角度值
//                        Target_Z_angle = angle_yaw - 60.00;  //角度统计90度
                        Angle_CNT_Flag=1;  //开角度计数器
                    }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
//                        Fix_Line_Flag=0;
                        Round_Statue=3;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 3:
                {
                    Mid_Get_Type=3;  //环内 换寻左线
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=-200.00;  //1设定期望角度值
//                         Target_Z_angle = angle_yaw - 200.00;  //角度统计200度
                         Angle_CNT_Flag=1;  //开角度计数器
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
                        Round_Statue=4;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 4:   //换下弧线
                {
                    Mid_Get_Type=2;
                    if(Angle_CNT_Flag==0)
                     {
                        angle_yaw=0;
                        Target_Z_angle=-70.00;  //1设定期望角度值
                        Angle_CNT_Flag=1;  //开角度计数器
                     }
                    if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                    {
                        Round_Statue=5;
                        Angle_CNT_Flag=0;
                    }
                    break;
                }
                case 5:   //出环  换下左线
                {
                    Mid_Get_Type=1;
                    Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判
                    if(Round_Cnt_Boring==30)  //环岛大结局
                    {
                        Round_Cnt_Boring=0;
                        Round_Statue=0;
                        Round_Road_Flag=0;
                        Element_Processing_Flag=0;   //重启元素判断
                        Mid_Get_Type=0;
                    }
                }
            }
        }

    }
}//带充电检测的环岛执行程序
//因为环岛一进就偏 一偏就充不进去了  速度三 环岛速度70
void Round_Processer_Charge3(void)
{
switch (Round_Road_Flag)
{
    case 1:         //左环岛  可以了 别动了//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        switch (Round_Statue)
        {
            case 1:  //左环岛先寻右线保证直线行驶
            {
                Mid_Get_Type=2;  //先巡右线保证直线行驶   这时候寻右边线 改为模式4 寻靠下面的右边线
                if(Stop_Flag==1) //停车之后
                {
                    Round_Statue=2;
                }
                break;
            }
            case 2:  //角度计数开  该打角进环岛了  打角进环岛 左环岛依靠左边线进环岛 到角度附近换循右边线 因为此时左边线内环线 已经是看不到了
            {
                Mid_Get_Type=3;  //检测到弧线就寻弧线 寻左弧线进环岛
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=60.00;  //1设定期望角度值
                    Angle_CNT_Flag=1;  //开角度计数器
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度  25度误差允许
                {
                    Round_Statue=3;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 3:
            {
                Mid_Get_Type=4;   //进环岛后换循右边线 保证稳定性    2改4 下右边线  左环岛不稳定可能为上方丢线导致分量为0 与常规不符合
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=180.00;  //1设定期望角度值
                    Angle_CNT_Flag=1;  //开角度计数器
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
                    Round_Statue=4;
                    Angle_CNT_Flag=0;


                    Charge_point=6.00;        //拉低 出环岛不充电


                }
                break;
            }
            case 4:   //出左环岛寻左下弧线
            {
                Mid_Get_Type=1;
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=70.00;  //1设定期望角度
                    Angle_CNT_Flag=1;  //开角度计数器
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
                    Round_Statue=5;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 5:   //出环时候换下右线 寻直线 找充电点
            {


                Mid_Get_Type=2;   //换右线 出环保持稳定 此时左线是个直线
                Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判
                if(Round_Cnt_Boring==35)  //环岛大结局  出环也是在充电 可以大点
                {
                    Round_Cnt_Boring=0;
                    Round_Statue=0;
                    Round_Road_Flag=0;
                    Element_Processing_Flag=0;   //重启元素判断
                    Mid_Get_Type=0;


                    Charge_Top=8.50;   //出左环岛把充电最高电压拉低 让最后一个出三叉不必冲太多
                    Charge_point=6.00;        //拉低 出环岛不充电


                }
            }
        }
        break;
    }
    case 2:         //右环岛//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        switch (Round_Statue)
        {
            case 1:  //右环岛先寻左线保证直线行驶
            {
                Mid_Get_Type=1;  //先巡左线保证直线行驶   这时候寻左下边线 改为模式3 寻靠下面的左边线
                if(Stop_Flag==1) //停车之后
                {
                    Round_Statue=2;
                }
                break;
            }
            case 2:  //角度计数开  该打角进环岛了 此时切换线路到左环岛的弧线
            {
//                    Fix_Line_Flag=2;
                Mid_Get_Type=4;  //检测到弧线就寻弧线  速度高情况下换打脚更大的线
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=-60.00;  //1设定期望角度值
//                        Target_Z_angle = angle_yaw - 60.00;  //角度统计90度
                    Angle_CNT_Flag=1;  //开角度计数器
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
//                        Fix_Line_Flag=0;
                    Round_Statue=3;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 3:
            {
                Mid_Get_Type=3;  //环内 换寻左线
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=-180.00;  //1设定期望角度值
//                         Target_Z_angle = angle_yaw - 200.00;  //角度统计200度
                     Angle_CNT_Flag=1;  //开角度计数器
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
                    Round_Statue=4;
                    Angle_CNT_Flag=0;




                    Charge_point=5.00;        //拉低 出环岛不充电  6.00伏再冲




                }
                break;
            }
            case 4:   //换下弧线
            {
                Mid_Get_Type=2;
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=-70.00;  //1设定期望角度值
                    Angle_CNT_Flag=1;  //开角度计数器
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
                    Round_Statue=5;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 5:   //出环  换下左线
            {


                Mid_Get_Type=1;
                Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判
                if(Round_Cnt_Boring==35)  //环岛大结局
                {
                    Round_Cnt_Boring=0;
                    Round_Statue=0;
                    Round_Road_Flag=0;
                    Element_Processing_Flag=0;   //重启元素判断
                    Mid_Get_Type=0;

                    Charge_point=9.00;        //  出右环岛后修正 充电下限电压

                }
            }
        }
    }

}
}
//带充电检测的环岛执行程序
//因为环岛一进就偏 一偏就充不进去了  速度三 环岛速度70
void Round_Processer_Charge4(void)
{
switch (Round_Road_Flag)
{
    case 1:         //左环岛  可以了 别动了//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        switch (Round_Statue)
        {
            case 1:  //左环岛先寻右线保证直线行驶
            {
                Mid_Get_Type=2;  //先巡右线保证直线行驶   这时候寻右边线 改为模式4 寻靠下面的右边线
                if(Stop_Flag==1) //停车之后
                {
                    Round_Statue=2;
                }
                break;
            }
            case 2:  //角度计数开  该打角进环岛了  打角进环岛 左环岛依靠左边线进环岛 到角度附近换循右边线 因为此时左边线内环线 已经是看不到了
            {
                Mid_Get_Type=3;  //检测到弧线就寻弧线 寻左弧线进环岛
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=60.00;  //1设定期望角度值
                    Angle_CNT_Flag=1;  //开角度计数器
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度  25度误差允许
                {
                    Round_Statue=3;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 3:
            {
                Mid_Get_Type=4;   //进环岛后换循右边线 保证稳定性    2改4 下右边线  左环岛不稳定可能为上方丢线导致分量为0 与常规不符合
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=180.00;  //1设定期望角度值
                    Angle_CNT_Flag=1;  //开角度计数器
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
                    Round_Statue=4;
                    Angle_CNT_Flag=0;


                    Charge_point=6.00;        //拉低 出环岛不充电


                }
                break;
            }
            case 4:   //出左环岛寻左下弧线
            {
                Mid_Get_Type=1;
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=70.00;  //1设定期望角度
                    Angle_CNT_Flag=1;  //开角度计数器
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
                    Round_Statue=5;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 5:   //出环时候换下右线 寻直线 找充电点
            {


                Mid_Get_Type=2;   //换右线 出环保持稳定 此时左线是个直线
                Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判
                if(Round_Cnt_Boring==35)  //环岛大结局  出环也是在充电 可以大点
                {
                    Round_Cnt_Boring=0;
                    Round_Statue=0;
                    Round_Road_Flag=0;
                    Element_Processing_Flag=0;   //重启元素判断
                    Mid_Get_Type=0;


                    Charge_Top=8.50;   //出左环岛把充电最高电压拉低 让最后一个出三叉不必冲太多
                    Charge_point=5.00;        //拉低 出环岛不充电


                }
            }
        }
        break;
    }
    case 2:         //右环岛//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        switch (Round_Statue)
        {
            case 1:  //右环岛先寻左线保证直线行驶
            {
                Mid_Get_Type=1;  //先巡左线保证直线行驶   这时候寻左下边线 改为模式3 寻靠下面的左边线
                if(Stop_Flag==1) //停车之后
                {
                    Round_Statue=2;
                }
                break;
            }
            case 2:  //角度计数开  该打角进环岛了 此时切换线路到左环岛的弧线
            {
//                    Fix_Line_Flag=2;
                Mid_Get_Type=4;  //检测到弧线就寻弧线  速度高情况下换打脚更大的线
                if(Angle_CNT_Flag==0)
                {
                    angle_yaw=0;
                    Target_Z_angle=-60.00;  //1设定期望角度值
//                        Target_Z_angle = angle_yaw - 60.00;  //角度统计90度
                    Angle_CNT_Flag=1;  //开角度计数器
                }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
//                        Fix_Line_Flag=0;
                    Round_Statue=3;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 3:
            {
                Mid_Get_Type=3;  //环内 换寻左线
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=-180.00;  //1设定期望角度值
//                         Target_Z_angle = angle_yaw - 200.00;  //角度统计200度
                     Angle_CNT_Flag=1;  //开角度计数器
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
                    Round_Statue=4;
                    Angle_CNT_Flag=0;




                    Charge_point=5.00;        //拉低 出环岛不充电  6.00伏再冲




                }
                break;
            }
            case 4:   //换下弧线
            {
                Mid_Get_Type=2;
                if(Angle_CNT_Flag==0)
                 {
                    angle_yaw=0;
                    Target_Z_angle=-70.00;  //1设定期望角度值
                    Angle_CNT_Flag=1;  //开角度计数器
                 }
                if(ang_err<=10.00&&Angle_CNT_Flag==1)   //到达设定角度
                {
                    Round_Statue=5;
                    Angle_CNT_Flag=0;
                }
                break;
            }
            case 5:   //出环  换下左线
            {


                Mid_Get_Type=1;
                Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判
                if(Round_Cnt_Boring==35)  //环岛大结局
                {
                    Round_Cnt_Boring=0;
                    Round_Statue=0;
                    Round_Road_Flag=0;
                    Element_Processing_Flag=0;   //重启元素判断
                    Mid_Get_Type=0;

                    Charge_point=9.00;        //  出右环岛后修正 充电下限电压

                }
            }
        }
    }

}
}

//带充电检测的环岛执行程序
//因为环岛一进就偏 一偏就充不进去了
/*
 *    环岛回环一起判断 先检测边线正常与否标志
 *    两种情况符合 则进行下一步 判断环岛与回环特征
 *    仅处理两个情况
 *    下一步具体元素判断时候 仅判断中线特征即可 不需判断边线
 *
 *    正常  为 严格正常 笔直赛道格式 若都不正常 或者都正常则环岛 回环直接置0 在一定程度上节省了计算量
 */
void Element_Round_and_Backcross(void)
{
    if(Zuo_Correct_Flag==0&&You_Correct_Flag==1&&Round_Cnt==2)   //左不正常 右正常 可能为左环岛
    {
        Element_Round_Pro(0);
    }
    if(Zuo_Correct_Flag==1&&You_Correct_Flag==0&&Round_Cnt==1)   //左正常 右不正常 可能为右环岛
    {
        Element_Round_Pro(1);
    }

}



/*
 * 环岛判别02
 *
 * 作者sjy
检测中线的弧度
因为此时 可以判断到 左右边线差异点

  DIR 0 左环岛判断 DIR 1 右环岛判断

   声明:此函数为void Element_Round_and_Backcross(void)的内部调用函数

   检测弧线 5个连续增量 5个连续减量
 */
void Element_Round_Pro(int DIR)
{
    uint8 i;
    uint8 rise=0,fall=0;    //上升 下降 计数器  设右偏为RISE 左偏为FALL
    uint8 hind_cnt=0,Hind_Flag=0;   //丢线统计 与 丢线情况符合标志位
    switch(DIR)
    {
        case 0:   //左环岛
        {
            //60以下的从50往下找
            for(i=50;i<=117;i++)
            {
                if(ImageSide[i][0]!=Zuo_Hind&&ImageSide[i+1][0]==Zuo_Hind&&ImageSide[i+2][0]==Zuo_Hind)
                {
                    hind_cnt=1;
                    break;
                }
            }
            if(hind_cnt==1)   //改动
            {
                for(i=70;i>=5;i--)    //在找上面的丢线点的时候会出现6 导致左环岛判断不了  使用小于等于(Zuo_Hind+3)替代等于丢线点  使用大于等于替代不丢线 出现6的原因未知...
                {
                    if(ImageSide[i][0]>=(Zuo_Hind+3)&&ImageSide[i-1][0]<=(Zuo_Hind+3)&&ImageSide[i-2][0]<=(Zuo_Hind+3))   //不丢 丢
                    {
                        Hind_Flag=1;
                        break;
                    }

                }
            }

                if(Hind_Flag==1)  //如果有出现 丢线 不丢 再丢的情况 进入下一步判断弧线
                {
                    for(i=100;i>=10;i--)  //向上循迹 先连续减 后连续增    右环岛 RISE大 fall小
                    {
                        if(ImageSide[i][0]==Zuo_Hind || ImageSide[i][0]==0)  //丢线或者上边线截至
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
                              Element_Processing_Flag=1;   //关元素判断
                              Round_Statue=1;
                              Round_Cnt=0;  //置0不再判断环岛
                              break;
                        }
                    }

                }
                break;
        }

        case 1: //右环岛
        {
            //60以下的从60往下找
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
                    if(ImageSide[i][1]!=You_Hind&&ImageSide[i-1][1]==You_Hind&&ImageSide[i-2][1]==You_Hind)   //不丢 丢
                    {
                        Hind_Flag=1;
                        break;
                    }
                }
            }

                if(Hind_Flag==1)  //如果有出现 丢线 不丢 再丢的情况 进入下一步判断弧线
                {
                    for(i=100;i>=10;i--)  //向上循迹 先连续减 后连续增    右环岛 RISE大 fall小
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
                              Element_Processing_Flag=1;   //关元素判断
                              Round_Statue=1;
                              Round_Cnt=2;   //判断完右环岛 接下来只判断左环岛
                              break;
                        }
                    }
                }
            }
        }
}


/*
 * 三岔路判断
 *
 * 后期利用上三岔上方的倒三角进行判断
 */
void Element_Three_Pro(void)
{
    uint8 i;
    uint8 Zcnt=0,Ycnt=0;
    if(use_image[10][80]==0 &&Zuo_Correct_Flag==0&&You_Correct_Flag==0)//上点出现黑点  左右线都不正常
    {
        for(i=100;i>=40;i--)
        {
            //左边线寻找
            if(ImageSide[i-1][0]<ImageSide[i][0])
            {
                Zcnt++;
            }
            //右边线寻找
            if(ImageSide[i-1][1]>ImageSide[i][1])
            {
                Ycnt++;
            }

                if(Zcnt>=15&&Ycnt>=5)  //车身偏正对前左岔路  左斜入三岔
                {
                    Three_Fork_Flag=1;
                    Element_Processing_Flag=1;
                    Three_Statue=1;  //三岔状态位置1
                    break;
                }
                else if(Zcnt>=5&&Ycnt>=15)  //车身偏正对前右岔路  右斜入三岔
                {
                    Three_Fork_Flag=1;
                    Element_Processing_Flag=1;
                    Three_Statue=1;  //三岔状态位置1
                    break;
                }
                else if(Zcnt>=13&&Ycnt>=13)   //正入三岔
                {
                    Three_Fork_Flag=1;
                    Element_Processing_Flag=1;
                    Three_Statue=1;  //三岔状态位置1
                    break;
                }
        }
        Zcnt=Ycnt=0;//清零
    }
}


/*
 * 三岔路补线  2022/5/9
 *
 * sjy
 *
 * 两点确定一条直线，确定斜率，就确定两点中的每一个点，然后让这条直线上所有白点变成黑点，在重新扫线，就可以了。
 */
void Three_Processor(void)
{
        if(Three_Fork_Cnt==1||Three_Fork_Cnt==2)
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;  //1设定期望角度值
                Angle_CNT_Flag=1;

            }
            if(Angle_CNT_Flag==1)
            {
                Mid_Get_Type=1;   //换线过三岔
//                Fix_Line_Flag=3;   //补线过三岔
                if(ang_err<=47.00&&Angle_CNT_Flag==1)   //角度达到目标值后 清标志位
                {
                    Angle_CNT_Flag=0;   //1关闭角度计数器
                    Three_Fork_Flag=0;  //2清三岔标志位
                    Three_Fork_Cnt++;  //4三岔计数器++
                    Element_Processing_Flag=0;   //重启元素判断
                    Fix_Line_Flag=0;   //3关闭补线
                    Mid_Get_Type=0;
                }
            }
        }
        if(Three_Fork_Cnt==3||Three_Fork_Cnt==4||Three_Fork_Cnt==5)
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;  //1设定期望角度值
                Angle_CNT_Flag=1;
            }
            if(Angle_CNT_Flag==1)
            {
                Mid_Get_Type=1;
//                Fix_Line_Flag=3;   //补线类型 三岔右转
                if(ang_err<=47.00&&Angle_CNT_Flag==1)
                {
                    Angle_CNT_Flag=0;   //1关闭角度计数器
                    Three_Fork_Flag=0;  //2清三岔标志位
                    Three_Fork_Cnt++;  //4三岔计数器++
                    if(Three_Fork_Cnt==6) Three_Fork_Cnt=1;  //为4清零
                    Element_Processing_Flag=0;   //重启元素判断
                    Mid_Get_Type=0;
                    Fix_Line_Flag=0;   //3关闭补线

                }
            }
        }
}

//双三岔统一处理
//此版本引入了三岔状态字 Three_Statue
void Three_Processor2(void)
{
    uint8 i;
    uint8 Zcnt=0,Ycnt=0;
    switch(Three_Statue)
    {
        case 1:  //先处理第一个三岔口
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;  //1设定期望角度值
                Angle_CNT_Flag=1;

            }
            if(Angle_CNT_Flag==1)
            {
                Mid_Get_Type=1;   //换线过三岔
                if(ang_err<=40.00&&Angle_CNT_Flag==1)   //角度达到目标值后 清标志位
                {
                    Angle_CNT_Flag=0;   //1关闭角度计数器
                    Fix_Line_Flag=0;   //3关闭补线
                    Mid_Get_Type=0;
                    Three_Statue=2;
                    Element_Processing_Flag=1;   //关掉元素判断
                }
            }
            break;
        }
        case 2:
        {
            Round_Cnt_Boring++;
            Element_Processing_Flag=1;   //关掉元素判断

            if(Round_Cnt_Boring>=110)     //进三岔延时 避免坡道处误判
            {
                Round_Cnt_Boring=0;
                Three_Statue=3;
            }
            break;
        }

        case 3:  //检测第二个三岔路
        {
            if(use_image[10][80]==0 &&Zuo_Correct_Flag==0&&You_Correct_Flag==0)//上点出现黑点  左右线都不正常
            {
                for(i=100;i>=40;i--)
                {
                    //左边线寻找
                    if(ImageSide[i-1][0]<ImageSide[i][0])
                    {
                        Zcnt++;
                    }
                    //右边线寻找
                    if(ImageSide[i-1][1]>ImageSide[i][1])
                    {
                        Ycnt++;
                    }

                        if(Zcnt>=13&&Ycnt>=5)  //车身偏正对前左岔路  左斜入三岔
                        {
                            Three_Statue=4;  //三岔状态位置1
                            Element_Processing_Flag=1;   //关掉元素判断

                            break;
                        }
                        else if(Zcnt>=5&&Ycnt>=13)  //车身偏正对前右岔路  右斜入三岔
                        {
                            Three_Statue=4;  //三岔状态位置1
                            Element_Processing_Flag=1;   //关掉元素判断

                            break;
                        }
                        else if(Zcnt>=12&&Ycnt>=12)   //正入三岔
                        {
                            Three_Statue=4;  //三岔状态位置1
                            Element_Processing_Flag=1;   //关掉元素判断

                            break;
                        }
                }
                Zcnt=Ycnt=0;//清零
            }
            break;
        }
        case 4:  //处理第二个三岔口
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=60.00;  //1设定期望角度值
                Angle_CNT_Flag=1;
            }
            if(Angle_CNT_Flag==1)
            {
                Mid_Get_Type=1;   //换线过三岔
                if(ang_err<=40.00&&Angle_CNT_Flag==1)   //角度达到目标值后 清标志位
                {
                    Angle_CNT_Flag=0;   //1关闭角度计数器

                    Three_Fork_Flag=0;  //2清三岔标志位
                    Element_Processing_Flag=0;   //重启元素判断

                    Fix_Line_Flag=0;
                    Mid_Get_Type=0;

                    Three_Statue=0;
                }
            }
        }
    }
}

//斑马线元素判断
//  Zebra_Flag==1 左车库  2==右车库
//车运行规则 先右出库先跑回环 开机默认打开腐蚀滤波
//遇见三岔或者环岛之后 将腐蚀滤波关闭 检测右车库
//检测到右车库 执行右入库
void Element_Zebra(void)
{
    uint8 i;
    uint8 cnt=0;

    if(Zuo_Correct_Flag==1&&You_Correct_Flag==0)  //右车库-----执行入库
    {
            for(i=20;i<=140;i++)
            {
//                if(use_image[40][i]==255&&use_image[40][i+1]==0) //白 黑  分界点
//                {
//                    cnt++;
//                }
//                if(use_image[40][i]==255&&use_image[40][i+1]==0) //白 黑  分界点
//                {
//                    cnt++;
//                }


                if(use_image[40][i]==255&&use_image[40][i+1]==0) //白 黑  分界点
                {
                    cnt++;
                }
                if(use_image[40][i]==255&&use_image[40][i+1]==0) //白 黑  分界点
                {
                    cnt++;
                }
                if(cnt>=7)
                {
                    Zebra_Flag=2;   //确定为斑马线  伪造的斑马线 此时应 开启服饰滤波延时20帧数
                    Element_Processing_Flag=1;
                    Zebra_Statue=1; //斑马线状态置1
                    break;
                }
            }
    }
//屏蔽检测左车库
//
//    if(Zuo_Correct_Flag==0&&You_Correct_Flag==1)    //左出库下对应只有这一种情况下 判断为斑马线 则换线入库
//    {
//            for(i=10;i<=150;i++)
//            {
//                if(use_image[20][i]==0&&use_image[20][i+1]==255) //黑 白 分界点
//                {
//                    cnt++;
//                }
//                if(use_image[20][i]==255&&use_image[20][i+1]==0) //白 黑  分界点
//                {
//                    cnt++;
//                }
//                if(cnt>=8)
//                {
//                    Zebra_Flag=1;   //确定为斑马线
//                    Element_Processing_Flag=1;
//                    break;
//                }
//            }
//    }
}




//斑马线元素处理
void Element_Zebra_Process(void)
{
    switch (Zebra_Flag)
    {
//        case 1:   //发现左车库 直接入库
//        {
//            Mid_Get_Type=1;
//            Round_Cnt_Boring++;  //之后啥也不干 防止下一步误判

//            break;
//        }
        case 2:  //发现右车库  逻辑执行入库
        {
            switch (Zebra_Statue)
            {
                case 1:
                {
                    Round_Cnt_Boring++;
                    if(Round_Cnt_Boring>=0)  //关屏幕18 可以了  因为关掉了屏幕 提高了速度 尝试5
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
                        Target_Z_angle=-90.00;  //1设定期望角度值
                        Angle_CNT_Flag=1;
                        Eat_Line_Flag=1;  //此时开启腐蚀滤波使其完美入库
                    }
                    if(Angle_CNT_Flag==1)
                    {
                        Fix_Line_Flag=4;
                        Mid_Get_Type=0;    //右入库
                        if(ang_err<=10.00 && Angle_CNT_Flag==1)   //角度达到目标值后 清标志位
                        {
                            Angle_CNT_Flag=0;   //1关闭角度计数器
                            Element_Processing_Flag=1;   //关元素判断
                            Fix_Line_Flag=0;

                            Mid_Get_Type=5;             //开摆
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

////出库计时器和出库标志位
////uint8 Out_Garage_FPS=0;
////uint8 Out_Garage_Flag=1;
////出库程序 延时出库
////开机先充电 充电过程可以移动车模 充满直接走 但是电压不得大于15 13灯亮之前开车走
//void Out_Garage(void)
//{
//    if(Out_Garage_Flag==1)
//    {
//        if(Stop_Flag==1) //充电中
//        {
//            Element_Processing_Flag=1;
//            Mid_Get_Type=5;  //开摆
//        }
//        if(Stop_Flag==0) //充满电 再出库   开机是stop_flag=1; 之后一旦等于0即执行出库程序
//        {
//            Mid_Get_Type=5;  //右出库
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
//                    Target_Z_angle=-80.00;  //1设定期望角度值   只巡线出库此值改为30
//                    Angle_CNT_Flag=1;
//
//                    Element_Processing_Flag=1;
//                    Eat_Line_Flag=1;  //出库开腐蚀滤波
//                    Mid_Get_Type=0;  //寻右边线
//                    Fix_Line_Flag=4;//右出库
//                }
//                if(Angle_CNT_Flag==1)
//                {
//                    if(ang_err<=45.00&&Angle_CNT_Flag==1)   //角度达到目标值后 清标志位
//                    {
//                        Angle_CNT_Flag=0;   //1关闭角度计数器
//                        Element_Processing_Flag=0;   //重启元素判断
//                        Mid_Get_Type=0;
//                        Fix_Line_Flag=0;
//                        Out_Garage_Flag=0; //出库结束 标志位清零 Out_Garage函数弃用
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
//    if(Stop_Flag==1) //充电中
//    {
//        Element_Processing_Flag=1;
//        Mid_Get_Type=5;  //开摆
//    }
if(Out_Garage_Flag==1)
{
    switch (Out_Garage_statue)
    {
        case 1:   //等待充电完成
        {
            Target_speed=0;
            Mid_Get_Type=5;  //直线行驶
            Element_Processing_Flag=1;

            if(Stop_Flag==0)
            {
                Mid_Get_Type=5;  //直线行驶
                Out_Garage_statue=2;
            }
            break;
        }
        case 2:   //充电完成后延时 直线行驶5FPS
        {
            Mid_Get_Type=5;  //直线行驶

            Out_Garage_FPS++;
            if(Out_Garage_FPS>=5)    //关掉屏幕显示后此值调大 关前10  关后  17
            {
                Out_Garage_FPS=0;
                Out_Garage_statue=3;
            }
            break;
        }
        case 3:  //计角度出库
        {
            if(Angle_CNT_Flag==0)
            {
                angle_yaw=0;
                Target_Z_angle=-75.00;  //1设定期望角度值   只巡线出库此值改为30
                Angle_CNT_Flag=1;

                Element_Processing_Flag=1;
                Eat_Line_Flag=1;  //出库开腐蚀滤波
                Mid_Get_Type=0;  //寻右边线
                Fix_Line_Flag=4;//右出库
            }
            if(Angle_CNT_Flag==1)
            {
                if(ang_err<=40.00&&Angle_CNT_Flag==1)   //角度达到目标值后 清标志位
                {
                    Angle_CNT_Flag=0;   //1关闭角度计数器
                    Element_Processing_Flag=0;   //重启元素判断
                    Mid_Get_Type=0;
                    Fix_Line_Flag=0;
                    Out_Garage_Flag=0; //出库结束 标志位清零 Out_Garage函数弃用
                    Out_Garage_statue=0;
                }
            }

        }

    }
}
}




////摄像头动态前瞻
//uint8 camear_foward=80;
//
// // 前瞻 左线
//uint8 zuo_line;
//// 前瞻 右线
//uint8 you_line;
//// 前瞻 中线
//uint8 middle_line;
////图像阈值  OTSU用
//extern uint8 image_threshold;
////边线数组
//uint8 ImageSide[MT9V03X_H2][2];  //0左边线 1右边线
//uint8 ImageMid[MT9V03X_H2];  //中线
//
////环岛标志位
//uint8 Round_Road_Flag = 0;   //1左环岛  2右环岛
//
////十字标志位
//uint8 Cross_Road_Flag = 0;   //入十字后向左转圈则下次出十字时向左转出十字  向右转圈则出下次十字时向右转出
//
////三叉标志位
//uint8 Three_Fork_Flag = 0;
//
////三叉2号标志位  三叉运行一次后记为1
//uint8 Three_Fork_Cnt = 0;

//无元素车体修正
void No_element_fix(void)
{
    if(Three_Fork_Flag==0&&Round_Road_Flag==0&&Back_Cross_Flag==0)    //巡线选择 那条线直线就寻哪条线
    {
        if(Zuo_Correct_Flag==0&&You_Correct_Flag==1)   //左不正常 右正常 可能为左环岛
        {
            Mid_Get_Type=2;
        }
        if(Zuo_Correct_Flag==1&&You_Correct_Flag==0)   //左正常 右不正常 可能为右环岛
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

//元素判断   状态反馈
void Element_Judge(void)
{
    Analyse_line_straight(0);  //左右边线判断
    Analyse_line_straight(1);
    if(Element_Processing_Flag==0)  //没有元素正在处理 则进行元素判断  Element_Processing_Flag在确定为此元素时就为1 就关闭了判断元素  处理完置0  元素标志位也置0
    {
            Element_Round_and_Backcross();    //元素判断
            if(Round_Road_Flag==0)
            {
                if(Back_Cnt!=0)
                Element_BackCross3();  //有问题  开关关闭

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



//背赛道  元素判断 17届山东省
//出库元素顺序 P T T R P R T T Z  回环 三岔 三岔 环岛 回环 环岛 三岔 三岔 车库
void Element_Order_Memory()
{

}



//元素处理
void Element_Do(void)
{
    if(Back_Cross_Flag!=0)
    {
        Ring_Flag=1;//蜂鸣器报告
        Backcross_process2();
    }
    else if(Back_Cross_Flag==0)
    {
        if(Round_Road_Flag!=0)
        {
            Ring_Flag=1;   //蜂鸣器报告
            Round_Process_Select();
        }
        else if(Round_Road_Flag==0)
        {
            if(Three_Fork_Flag!=0)
            {
                Eat_Line_Flag=0;  //判断到环岛把腐蚀滤波关掉 以便检测入库 在出库之后一直开着腐蚀滤波 直到遇见第一个 环岛/三岔
                Ring_Flag=1;   //蜂鸣器报告
                Three_Processor();
//                Three_Processor2();
            }
            else if(Three_Fork_Flag==0)
            {
                if(Zebra_Flag!=0)
                {
                    Ring_Flag=1;   //蜂鸣器报告
                    Element_Zebra_Process();
                }
            }
        }
    }
}

void Element(void)
{
    Element_Judge();
    Element_Do();    //开陀螺仪卡
    Mid_Line_Fix(Fix_Line_Flag);   //Element_Do();给信号 让此函数补线  补线完成后  清除 Fix_Line_Flag
}

//中线修复
/* 根据DATA不同从data.c调用数据进行修复
 * 1 三岔 左转
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
        case 1:    //左转  三岔
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
        case 2:    //右转  三岔
        {
            for(i=30;i<=90;i++)
            {
                ImageMid[i]=(ImageSide[i][1]+Three_Fork_you[i-30])/2;
                Pixle[i][ImageMid[i]]=0;  //显示修复后的中线
                Pixle[i][ImageMid[i]+1]=0;
                Pixle[i][ImageMid[i]+2]=0;
            }
            break;
        }
        case 3:    //急速左转
        {
            for(i=30;i<=90;i++)
            {
                ImageMid[i]=(ImageSide[i][0]+Back_Round_zuo[i-30])/2;
                Pixle[i][ImageMid[i]]=0;  //显示修复后的中线
                Pixle[i][ImageMid[i]+1]=0;
                Pixle[i][ImageMid[i]+2]=0;
            }
            break;
        }
        case 4:    //急速右转
        {
            for(i=30;i<=90;i++)
            {
                ImageMid[i]=(ImageSide[i][1]+Back_Round_you[i-30])/2;
                Pixle[i][ImageMid[i]]=0;  //显示修复后的中线
                Pixle[i][ImageMid[i]+1]=0;
                Pixle[i][ImageMid[i]+2]=0;
            }
            break;
        }
    }
}




///*
// * 最小二乘法多项式直线拟合                                                 0------------------------160
// * 斜率计算                                                                             |                                |
// * 未实际测量与当前ERR计算方式误差                                      |           图像              |
// * 待上位机实验                                                                       |                                |
// *                                                  120-----------120*160
// *
// */
//int MidSlope_get(int startline, int endline)
//{
//    //y = A + B x ;最小二乘法多项式直线拟合，就是根据给定的点，用计算的方法求出最佳的 a(斜率) 和 b（截距）。显然，关键是如何求出最佳的 a 和 b。
//      int i=0,SumX=0,SumY=0,SumLines = 0;
//      float SumUp=0,SumDown=0,avrX=0,avrY=0,B,A;   //B:斜率   A:复制计算的截距
//      SumLines=endline-startline;   // startline 为开始行， //endline 结束行 //SumLines
//
//      for(i=startline;i<endline;i++)
//      {
//        SumX+=i;
//        SumY+=ImageMid[i];    //这里Middle_black为存放中线的数组
//      }
//      avrX=SumX/SumLines;     //X的平均值
//      avrY=SumY/SumLines;     //Y的平均值
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
//        A=(SumY-B*SumX)/SumLines;  //截距
//        return B;  //返回斜率
//}


void Element_show(void)
{
    if(Round_Road_Flag!=0)   //环岛处理 分左右
    {
        if(Round_Road_Flag==1)
        lcd_showstr(40, 1, "R Z");
        if(Round_Road_Flag==2)
        lcd_showstr(40, 1, "R Y");
    }
    else if(Round_Road_Flag==0)
    {
        if(Three_Fork_Flag==1)  //三岔处理
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

