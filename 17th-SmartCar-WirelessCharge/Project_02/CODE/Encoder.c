#include "headfile.h"


//编码器全局变量
sint32 Encoder1,Encoder2;

sint32 zuo_real_speed_add;
sint32 you_real_speed_add;

/*
 * 编码器初始化
 */
void Encoder_Init (void)
{
    /*编码器初始化*/
    gpt12_init(GPT12_T4, GPT12_T4INA_P02_8, GPT12_T4EUDA_P00_9);
    gpt12_init(GPT12_T5, GPT12_T5INB_P10_3, GPT12_T5EUDB_P10_1);    //左编码器 正确
}


/*
 * 放定时器里面
 *5ms
 */
void Encoder_get_speed(void)
{
    Encoder1 =  gpt12_get(GPT12_T4);   //空转测试 加正负值  1024线 5ms
    Encoder2 = -gpt12_get(GPT12_T5);   //需要实际跑圈测试

    gpt12_clear(GPT12_T4);
    gpt12_clear(GPT12_T5);  //编码器数据采集完成后务必清零

    zuo_real_speed = Encoder1;
    you_real_speed = Encoder2;

    zuo_real_speed_add+=zuo_real_speed;
    you_real_speed_add+=you_real_speed;

}

