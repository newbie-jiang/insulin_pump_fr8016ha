/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

/*
 * INCLUDES
 */
#include <stdint.h>

#include "os_task.h"
#include "os_msg_q.h"
#include "os_mem.h"

#include "co_printf.h"
#include "user_task.h"
#include "button.h"
#include "driver_gpio.h"
#include "lcd.h"
#include "adpcm_ms.h"
#include "speaker.h"
#include "decoder.h"
#include "ble_simple_peripheral.h"
#include "sys_utils.h"
#include "driver_pmu.h"
#include "driver_pwm.h"

extern uint32_t speed_set_flag;

void motor_task_fun(void *arg)
{	
    gpio_set_pin_value(GPIO_PORT_D,GPIO_BIT_5,0);
    if(speed_set_flag==1){
      co_delay_100us(50); //5ms
    }else if(speed_set_flag==2){
      co_delay_100us(500);//50ms
    }else if(speed_set_flag ==3){
      co_delay_100us(5000);//500ms 
    }
	
    gpio_set_pin_value(GPIO_PORT_D,GPIO_BIT_5,1);
	co_delay_100us(10000);
}



void beep_task_fun(void *arg)
{ 	
	  static int buzzer_pwm_started = 0; // ��� pwm �Ƿ��Ѿ�����

    if (0 == gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_5)) // ��������
    {
        if (!buzzer_pwm_started)
        {
            pwm_start(PWM_CHANNEL_4); // ���� buzzer pwm
            buzzer_pwm_started = 1;   // ��� pwm �Ѿ�����
        }
    }
    else // �����ɿ�
    {
        if (buzzer_pwm_started)
        {
            pwm_stop(PWM_CHANNEL_4); // �ر� buzzer pwm
            buzzer_pwm_started = 0;   // ���ñ��
        }
    }  
}


extern float get_vbat_adc_val(void);

/* electric quantity detection */ 
void electric_quantity_task_fun(void *arg)
{
	static  int  vol_val;
	
    vol_val = get_vbat_adc_val();
	
    co_printf("vol_val = %dmV \r\n",vol_val);	
}




static uint32_t tick; 
void key_scan_task_fun(void *arg)
{
	if (0 == gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_5)) // ��������
    {	
		tick++; //100ms ��һ��
        if (0 == gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_5)) // ������������
        {
			if(tick >= 20 ){
			  // ִ�йػ�����
              pmu_set_led2_value(1);     // �ر� LED
              gpio_set_pin_value(GPIO_PORT_D, GPIO_BIT_4, 0);  //�ػ�
			  pwm_stop(PWM_CHANNEL_4); // �ر� buzzer pwm			
			}                       
        }else{
			
		 tick =0;
			
		}  
    }else{
	  tick = 0;
	}
	
//	co_printf("key_scan_task_fun tick  =  %d\r\n" , tick);
   
}



void led_task_fun(void *arg)
{
	 pmu_set_led2_value(0); 	//����led	
     co_delay_100us(5000);//500ms 
     pmu_set_led2_value(1); 	//�ر�led	
     co_delay_100us(5000);//500ms 	
}


