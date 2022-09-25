
#include "isr_config.h"
#include "isr.h"
#include "headfile.h"

sint32 speed_get_flag = 0;
sint32 speed_control_flag = 0;

sint32 Target_speed;  //目标速度

sint32 Ecoder_get_flag=0;   //编码器采集标志位
sint32 Gryo_get_flag=0;

uint8 MODE_SELECT;   //速度挡


//PIT中断函数
//角度环 5ms定时器
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
	enableInterrupts();//开启中断嵌套
    PIT_CLEAR_FLAG(CCU6_0, PIT_CH0);
    speed_control_flag++;
    speed_get_flag++;

    if(speed_control_flag==1)  //转向环5ms标志位
    {
        speed_control_flag=0;


        if(Stop_Flag==0)
        {
//            Speed_Target_Control();//期望速度值动态调整
            Position_PD_control(MODE_SELECT);//高速状态下控制弯道减速加强控制精度
        }


        Position_control();
        if(Speed_dif > 0)
        {
            S_zuo_duty -= Speed_dif/2;  //减左轮速度
            S_you_duty += Speed_dif/2;   //加右轮速度
        }
        if(Speed_dif <= 0)
        {
            S_zuo_duty += (-Speed_dif)/2;  //加左轮速度
            S_you_duty -= (-Speed_dif)/2;   //减右轮速度
        }
        Loss_line_Protect();

        MotorControl(S_zuo_duty,S_you_duty);  //电机控制
    }

    if(speed_get_flag==8)  //40ms速度环  速度恢复矫正与控制
    {
        speed_get_flag=0;

        Incremental_PI_control();//速度环参数调整  特殊元素控制
        Charge_stop();   //停车调整  调整优先级最高

        //速度环
        Speed_Control();
        S_zuo_duty=speed_out;
        S_you_duty=speed_out;
    }
}


/*
//编码器采样专用定时器:频率5ms 角度获取 2ms
 *
 */
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
	enableInterrupts();//开启中断嵌套
	PIT_CLEAR_FLAG(CCU6_0, PIT_CH1);

	Show_Voltage();

	Gryo_get_flag++;
    Ecoder_get_flag++;

    if(Angle_CNT_Flag==0)ang_err=6666;
    if(Gryo_get_flag>=2 && Angle_CNT_Flag==1)
    {
        angle_get();
        ang_err = Target_Z_angle-angle_yaw;
        if(ang_err<=0)ang_err=-ang_err;
        Gryo_get_flag=0;
    }
    if(Ecoder_get_flag>=5)
    {
        Ecoder_get_flag=0;
        Encoder_get_speed();
    }
}

/*
 * 蜂鸣器以及ADC采集  100ms定时器
 */
IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
	enableInterrupts();//开启中断嵌套
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH0);

    Beep_Ring(Ring_Flag);
    Ring_Flag=0;

    BatVolt=(adc_mean_filter(ADC_1, ADC1_CH9_A25, ADC_12BIT, 50)*1.000-1235)/100-0.150;  //0-12v对应  电容电压   //30

    TestVolt=adc_mean_filter(ADC_2, ADC2_CH3_A35, ADC_12BIT, 50)*1.000;  //0-3v对应  检测版 电压 MAX到2700多
}



/*
        灯板
    1ms定时器
 */
IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY)
{
	enableInterrupts();//开启中断嵌套
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH1);


}




IFX_INTERRUPT(eru_ch0_ch4_isr, 0, ERU_CH0_CH4_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
	if(GET_GPIO_FLAG(ERU_CH0_REQ4_P10_7))//通道0中断
	{
		CLEAR_GPIO_FLAG(ERU_CH0_REQ4_P10_7);
	}

	if(GET_GPIO_FLAG(ERU_CH4_REQ13_P15_5))//通道4中断
	{
		CLEAR_GPIO_FLAG(ERU_CH4_REQ13_P15_5);
	}
}

IFX_INTERRUPT(eru_ch1_ch5_isr, 0, ERU_CH1_CH5_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
	if(GET_GPIO_FLAG(ERU_CH1_REQ5_P10_8))//通道1中断
	{
		CLEAR_GPIO_FLAG(ERU_CH1_REQ5_P10_8);
	}

	if(GET_GPIO_FLAG(ERU_CH5_REQ1_P15_8))//通道5中断
	{
		CLEAR_GPIO_FLAG(ERU_CH5_REQ1_P15_8);
	}
}

//由于摄像头pclk引脚默认占用了 2通道，用于触发DMA，因此这里不再定义中断函数
//IFX_INTERRUPT(eru_ch2_ch6_isr, 0, ERU_CH2_CH6_INT_PRIO)
//{
//	enableInterrupts();//开启中断嵌套
//	if(GET_GPIO_FLAG(ERU_CH2_REQ7_P00_4))//通道2中断
//	{
//		CLEAR_GPIO_FLAG(ERU_CH2_REQ7_P00_4);
//
//	}
//	if(GET_GPIO_FLAG(ERU_CH6_REQ9_P20_0))//通道6中断
//	{
//		CLEAR_GPIO_FLAG(ERU_CH6_REQ9_P20_0);
//
//	}
//}






IFX_INTERRUPT(eru_ch3_ch7_isr, 0, ERU_CH3_CH7_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
	if(GET_GPIO_FLAG(ERU_CH3_REQ6_P02_0))//通道3中断
	{
		CLEAR_GPIO_FLAG(ERU_CH3_REQ6_P02_0);
		if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_vsync();
		else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_vsync();
		else if	(CAMERA_BIN       == camera_type)	ov7725_vsync();

	}
	if(GET_GPIO_FLAG(ERU_CH7_REQ16_P15_1))//通道7中断
	{
		CLEAR_GPIO_FLAG(ERU_CH7_REQ16_P15_1);
	}
}



IFX_INTERRUPT(dma_ch5_isr, 0, ERU_DMA_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套

	if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_dma();
//	else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_dma();
//	else if	(CAMERA_BIN       == camera_type)	ov7725_dma();
}


//串口中断函数  示例
IFX_INTERRUPT(uart0_tx_isr, 0, UART0_TX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart0_handle);
}
IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart0_handle);
}
IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart0_handle);
}

//串口1默认连接到摄像头配置串口
IFX_INTERRUPT(uart1_tx_isr, 0, UART1_TX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart1_handle);
}
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart1_handle);
    if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_uart_callback();
    else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_callback();
}
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart1_handle);
}


//串口2默认连接到无线转串口模块
IFX_INTERRUPT(uart2_tx_isr, 0, UART2_TX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart2_handle);
}
IFX_INTERRUPT(uart2_rx_isr, 0, UART2_RX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart2_handle);
    switch(wireless_type)
    {
    	case WIRELESS_SI24R1:
    	{
    		wireless_uart_callback();
    	}break;

    	case WIRELESS_CH9141:
		{
		    bluetooth_ch9141_uart_callback();
		}break;
    	default:break;
    }

}
IFX_INTERRUPT(uart2_er_isr, 0, UART2_ER_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart2_handle);
}



IFX_INTERRUPT(uart3_tx_isr, 0, UART3_TX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart3_handle);
}
IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart3_handle);
}
IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart3_handle);
}
