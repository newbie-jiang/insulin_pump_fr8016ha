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
#include "driver_adc.h "



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
	  static int buzzer_pwm_started = 0; // 标记 pwm 是否已经启动

    if (0 == gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_5)) // 按键按下
    {
        if (!buzzer_pwm_started)
        {
            pwm_start(PWM_CHANNEL_4); // 开启 buzzer pwm
            buzzer_pwm_started = 1;   // 标记 pwm 已经启动
        }
    }
    else // 按键松开
    {
        if (buzzer_pwm_started)
        {
            pwm_stop(PWM_CHANNEL_4); // 关闭 buzzer pwm
            buzzer_pwm_started = 0;   // 重置标记
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
	if (0 == gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_5)) // 按键按下
    {	
		tick++; //100ms 加一次
        if (0 == gpio_get_pin_value(GPIO_PORT_A, GPIO_BIT_5)) // 按键持续按下
        {
			if(tick >= 20 ){
			  // 执行关机操作
              pmu_set_led2_value(1);     // 关闭 LED
              gpio_set_pin_value(GPIO_PORT_D, GPIO_BIT_4, 0);  //关机
			  pwm_stop(PWM_CHANNEL_4); // 关闭 buzzer pwm			
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
	 pmu_set_led2_value(0); 	//开启led	
     co_delay_100us(5000);//500ms 
     pmu_set_led2_value(1); 	//关闭led	
     co_delay_100us(5000);//500ms 	
}


//PD6 PD7 ADC   10bit adc
void adc_task_fun(void *arg)
{
	struct adc_cfg_t cfg_pd6;
    uint16_t  result_pd6;
	float  vol_pd6;
	
    memset((void*)&cfg_pd6, 0, sizeof(cfg_pd6));
    cfg_pd6.src = ADC_TRANS_SOURCE_PAD;
    cfg_pd6.ref_sel = ADC_REFERENCE_AVDD;
    cfg_pd6.channels = 0x04;  
    cfg_pd6.route.pad_to_sample = 1;
    cfg_pd6.clk_sel = ADC_SAMPLE_CLK_24M_DIV13;
    cfg_pd6.clk_div = 0x3f;
    adc_init(&cfg_pd6);
    adc_enable(NULL, NULL, 0);
    adc_get_result(ADC_TRANS_SOURCE_PAD, 0x04, &result_pd6);
    adc_disable();
    co_printf("pd6 adc val: %d\r\n",result_pd6);	
	co_printf("pd6 adc vol: %d mv\r\n",(result_pd6 *33*100/1024)); // 计算中有小数会造成数据错误
	
//	vol_pd6 = (float)result_pd6 * 3.3/1023;
//	co_printf("pd6 vol val: %f\r\n",vol_pd6);	
	
			
//	struct adc_cfg_t cfg_pd7;
//	uint16_t result_pd7;
//	float  vol_pd7;
//	
//    memset((void*)&cfg_pd7, 0, sizeof(cfg_pd7));
//    cfg_pd7.src = ADC_TRANS_SOURCE_PAD;
//    cfg_pd7.ref_sel = ADC_REFERENCE_AVDD;
//    cfg_pd7.channels = 0x04;  //
//    cfg_pd7.route.pad_to_sample = 1;
//    cfg_pd7.clk_sel = ADC_SAMPLE_CLK_24M_DIV13;
//    cfg_pd7.clk_div = 0x3f;
//    adc_init(&cfg_pd7);
//    adc_enable(NULL, NULL, 0);
//    adc_get_result(ADC_TRANS_SOURCE_PAD, 0x04, (uint16_t *)&result_pd7);
//    adc_disable();
//    co_printf("pd7 adc val: %d\r\n",result_pd7);	
////	vol_pd7 = (float)result_pd7 * 3.3/1023;
////	co_printf("pd7 vol val: %f\r\n",vol_pd7);	
}


