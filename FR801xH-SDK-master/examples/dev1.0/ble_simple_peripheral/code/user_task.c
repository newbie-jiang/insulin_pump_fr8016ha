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
  	
	
	
	
	
}


extern float get_vbat_adc_val(void);

/* electric quantity detection */ 
void electric_quantity_task_fun(void *arg)
{
	static  int  vol_val;
    vol_val = get_vbat_adc_val();
	
    co_printf("vol_val = %dmV \r\n",vol_val);	
}




