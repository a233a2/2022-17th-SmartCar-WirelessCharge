
#include "isr_config.h"
#include "isr.h"
#include "headfile.h"

sint32 speed_get_flag = 0;
sint32 speed_control_flag = 0;

sint32 Target_speed;  //Ŀ���ٶ�

sint32 Ecoder_get_flag=0;   //�������ɼ���־λ
sint32 Gryo_get_flag=0;

uint8 MODE_SELECT;   //�ٶȵ�


//PIT�жϺ���
//�ǶȻ� 5ms��ʱ��
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
	enableInterrupts();//�����ж�Ƕ��
    PIT_CLEAR_FLAG(CCU6_0, PIT_CH0);
    speed_control_flag++;
    speed_get_flag++;

    if(speed_control_flag==1)  //ת��5ms��־λ
    {
        speed_control_flag=0;


        if(Stop_Flag==0)
        {
//            Speed_Target_Control();//�����ٶ�ֵ��̬����
            Position_PD_control(MODE_SELECT);//����״̬�¿���������ټ�ǿ���ƾ���
        }


        Position_control();
        if(Speed_dif > 0)
        {
            S_zuo_duty -= Speed_dif/2;  //�������ٶ�
            S_you_duty += Speed_dif/2;   //�������ٶ�
        }
        if(Speed_dif <= 0)
        {
            S_zuo_duty += (-Speed_dif)/2;  //�������ٶ�
            S_you_duty -= (-Speed_dif)/2;   //�������ٶ�
        }
        Loss_line_Protect();

        MotorControl(S_zuo_duty,S_you_duty);  //�������
    }

    if(speed_get_flag==8)  //40ms�ٶȻ�  �ٶȻָ����������
    {
        speed_get_flag=0;

        Incremental_PI_control();//�ٶȻ���������  ����Ԫ�ؿ���
        Charge_stop();   //ͣ������  �������ȼ����

        //�ٶȻ�
        Speed_Control();
        S_zuo_duty=speed_out;
        S_you_duty=speed_out;
    }
}


/*
//����������ר�ö�ʱ��:Ƶ��5ms �ǶȻ�ȡ 2ms
 *
 */
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
	enableInterrupts();//�����ж�Ƕ��
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
 * �������Լ�ADC�ɼ�  100ms��ʱ��
 */
IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
	enableInterrupts();//�����ж�Ƕ��
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH0);

    Beep_Ring(Ring_Flag);
    Ring_Flag=0;

    BatVolt=(adc_mean_filter(ADC_1, ADC1_CH9_A25, ADC_12BIT, 50)*1.000-1235)/100-0.150;  //0-12v��Ӧ  ���ݵ�ѹ   //30

    TestVolt=adc_mean_filter(ADC_2, ADC2_CH3_A35, ADC_12BIT, 50)*1.000;  //0-3v��Ӧ  ���� ��ѹ MAX��2700��
}



/*
        �ư�
    1ms��ʱ��
 */
IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY)
{
	enableInterrupts();//�����ж�Ƕ��
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH1);


}




IFX_INTERRUPT(eru_ch0_ch4_isr, 0, ERU_CH0_CH4_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
	if(GET_GPIO_FLAG(ERU_CH0_REQ4_P10_7))//ͨ��0�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH0_REQ4_P10_7);
	}

	if(GET_GPIO_FLAG(ERU_CH4_REQ13_P15_5))//ͨ��4�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH4_REQ13_P15_5);
	}
}

IFX_INTERRUPT(eru_ch1_ch5_isr, 0, ERU_CH1_CH5_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
	if(GET_GPIO_FLAG(ERU_CH1_REQ5_P10_8))//ͨ��1�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH1_REQ5_P10_8);
	}

	if(GET_GPIO_FLAG(ERU_CH5_REQ1_P15_8))//ͨ��5�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH5_REQ1_P15_8);
	}
}

//��������ͷpclk����Ĭ��ռ���� 2ͨ�������ڴ���DMA��������ﲻ�ٶ����жϺ���
//IFX_INTERRUPT(eru_ch2_ch6_isr, 0, ERU_CH2_CH6_INT_PRIO)
//{
//	enableInterrupts();//�����ж�Ƕ��
//	if(GET_GPIO_FLAG(ERU_CH2_REQ7_P00_4))//ͨ��2�ж�
//	{
//		CLEAR_GPIO_FLAG(ERU_CH2_REQ7_P00_4);
//
//	}
//	if(GET_GPIO_FLAG(ERU_CH6_REQ9_P20_0))//ͨ��6�ж�
//	{
//		CLEAR_GPIO_FLAG(ERU_CH6_REQ9_P20_0);
//
//	}
//}






IFX_INTERRUPT(eru_ch3_ch7_isr, 0, ERU_CH3_CH7_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
	if(GET_GPIO_FLAG(ERU_CH3_REQ6_P02_0))//ͨ��3�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH3_REQ6_P02_0);
		if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_vsync();
		else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_vsync();
		else if	(CAMERA_BIN       == camera_type)	ov7725_vsync();

	}
	if(GET_GPIO_FLAG(ERU_CH7_REQ16_P15_1))//ͨ��7�ж�
	{
		CLEAR_GPIO_FLAG(ERU_CH7_REQ16_P15_1);
	}
}



IFX_INTERRUPT(dma_ch5_isr, 0, ERU_DMA_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��

	if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_dma();
//	else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_dma();
//	else if	(CAMERA_BIN       == camera_type)	ov7725_dma();
}


//�����жϺ���  ʾ��
IFX_INTERRUPT(uart0_tx_isr, 0, UART0_TX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart0_handle);
}
IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart0_handle);
}
IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart0_handle);
}

//����1Ĭ�����ӵ�����ͷ���ô���
IFX_INTERRUPT(uart1_tx_isr, 0, UART1_TX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart1_handle);
}
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart1_handle);
    if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_uart_callback();
    else if (CAMERA_BIN_UART  == camera_type)	ov7725_uart_callback();
}
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart1_handle);
}


//����2Ĭ�����ӵ�����ת����ģ��
IFX_INTERRUPT(uart2_tx_isr, 0, UART2_TX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart2_handle);
}
IFX_INTERRUPT(uart2_rx_isr, 0, UART2_RX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
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
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart2_handle);
}



IFX_INTERRUPT(uart3_tx_isr, 0, UART3_TX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrTransmit(&uart3_handle);
}
IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrReceive(&uart3_handle);
}
IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO)
{
	enableInterrupts();//�����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart3_handle);
}
