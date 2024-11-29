/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

/*
 * INCLUDE FILES
 */
#include <stdio.h>
#include <string.h>

#include "gap_api.h"
#include "gatt_api.h"

#include "os_timer.h"
#include "os_mem.h"
#include "sys_utils.h"
#include "button.h"
#include "jump_table.h"

#include "user_task.h"

#include "driver_plf.h"
#include "driver_system.h"
#include "driver_i2s.h"
#include "driver_pmu.h"
#include "driver_uart.h"
#include "driver_rtc.h"
#include "driver_efuse.h"
#include "driver_flash.h"
#include "flash_usage_config.h"

#include "ble_simple_peripheral.h"
#include "simple_gatt_service.h"
#include "application.h"

const struct jump_table_version_t _jump_table_version __attribute__((section("jump_table_3"))) = 
{
    .firmware_version = 0x00000000,
};

const struct jump_table_image_t _jump_table_image __attribute__((section("jump_table_1"))) =
{
    .image_type = IMAGE_TYPE_APP,
    .image_size = 0x19000,      
};


//pmu_gpio 中断函数
__attribute__((section("ram_code"))) void pmu_gpio_isr_ram(void)
{
    uint32_t gpio_value = ool_read32(PMU_REG_GPIOA_V);
	
	if(gpio_value)
	{
	 co_printf("PA0 ");
	}
    
    button_toggle_detected(gpio_value);
	
    ool_write32(PMU_REG_PORTA_LAST, gpio_value);
	
	
}


void user_custom_parameters(void)
{
    struct chip_unique_id_t id_data;

    efuse_get_chip_unique_id(&id_data);
    __jump_table.addr.addr[0] = 0xBD;
    __jump_table.addr.addr[1] = 0xAD;
    __jump_table.addr.addr[2] = 0xD0;
    __jump_table.addr.addr[3] = 0xF0;
    __jump_table.addr.addr[4] = 0x17;
    __jump_table.addr.addr[5] = 0x20;
    
    id_data.unique_id[5] |= 0xc0; // random addr->static addr type:the top two bit must be 1.
    memcpy(__jump_table.addr.addr,id_data.unique_id,6);
    __jump_table.system_clk = SYSTEM_SYS_CLK_48M;
    jump_table_set_static_keys_store_offset(JUMP_TABLE_STATIC_KEY_OFFSET);
    ble_set_addr_type(BLE_ADDR_TYPE_PUBLIC);
    retry_handshake();
}

/* 进入睡眠前执行函数 */
__attribute__((section("ram_code"))) void user_entry_before_sleep_imp(void)
{
	uart_putc_noint_no_wait(UART1, 's');
		
    rtc_init();//初始化rtc
	
	rtc_alarm(RTC_A,5000);//启动RTCB定时器 5s	
		
	pmu_set_led2_value(1); 	//关闭led	
	
	if(gap_get_connect_status(0))
	{
	 rtc_disalarm(RTC_A); //停止rtcA 定时器   rtcA定时器启动一次 开启蓝牙连接
				
	 system_sleep_disable(); //检测到处于连接状态时关闭睡眠模式	 
	}
}



void motor_low_powre_start(uint32_t delay_ms)
{
	//低功耗  io初始化
	pmu_set_pin_to_PMU(GPIO_PORT_D,GPIO_BIT_5);	
	pmu_set_pin_dir(GPIO_PORT_D,BIT(5), GPIO_DIR_OUT); //输出低电平 
	
	co_delay_100us(delay_ms);//运动时间
	
	pmu_set_pin_dir(GPIO_PORT_D,BIT(5), GPIO_DIR_IN);  //输入浮空,保证不漏电
}

     uint32_t last_motor_start_time_s = 0;


//获得当前时间运行的基础率段
uint32_t get_now_basal_rate_info(clock_param_t *p_clock_env)
{
	 uint32_t max_basal_rate_num;
	
	 uint32_t get_start_tim [48] = {0};
	 uint32_t get_end_tim   [48] = {0};
	
	 //计算当前时间对应的秒数
     uint32_t  current_time_s = p_clock_env->hour * 3600 + 
                              p_clock_env->min * 60 + 
                              p_clock_env->sec;
     // 处理跨天情况
     uint32_t day_seconds = 24 * 3600;
					
    if (current_time_s < last_motor_start_time_s) {
        // 跨天：重置 last_motor_start_time_s
        last_motor_start_time_s = 0;
    }
		
	//获得最大基础率段，最大基础率段为0时则不启动电机
	max_basal_rate_num = get_max_basal_rate_num();
	
	if(max_basal_rate_num == 0)
	{
		printf(" max_basal_rate_num == 0 ");
	 
	    return -1;
	}
		 
	 //依据时间遍历基础率信息，获取当前时间处于第几段
	 for(int i=0;i<max_basal_rate_num;i++)
	 {
        get_start_tim[i]  =  s_rate_info[i].basal_rate_start_tim_hh*3600 + s_rate_info[i].basal_rate_start_tim_min*60;
	    get_end_tim[i]    =  s_rate_info[i].basal_rate_end_tim_hh*3600 + s_rate_info[i].basal_rate_end_tim_min*60;
		printf("get_start_tim[%d] = %d\r\n",i,get_start_tim[i]);
	    printf("get_end_tim[%d] = %d\r\n",i,get_end_tim[i]);
		 
	    if(current_time_s>=get_start_tim[i]&&current_time_s<=get_end_tim[i])
		{
			printf("get_now_basal_rate_info return: %d\r\n",(i+1));
		     //返回当前时间是运行的第几段	从1开始
		    return (i+1);
		} 
	 }

	 return -1;
}




void is_motor_start(clock_param_t *p_clock_env, uint32_t weak_up_tim_interval_s, basal_rate_information * p_rate_info)
{
    //计算当前时间对应的秒数
    uint32_t  current_time_s = p_clock_env->hour * 3600 + 
                              p_clock_env->min * 60 + 
                              p_clock_env->sec;
    // 处理跨天情况
    uint32_t day_seconds = 24 * 3600;
		
	uint32_t get_start_tim  =  p_rate_info->basal_rate_start_tim_hh*3600 + p_rate_info->basal_rate_start_tim_min*60;
	uint32_t get_end_tim    =  p_rate_info->basal_rate_end_tim_hh*3600 + p_rate_info->basal_rate_end_tim_min*60;
	
	
	co_printf("get_start_tim = %d\r\n",get_start_tim);
	co_printf("get_end_tim = %d\r\n",get_end_tim);
				
    if (current_time_s < last_motor_start_time_s) {
        // 跨天：重置 last_motor_start_time_s
        last_motor_start_time_s = 0;
    }
	
	 co_printf("now  tim_s:%d\r\n",current_time_s);
	 co_printf("last tim_s:%d\r\n",last_motor_start_time_s);
	
	//限制运行时间
	if(current_time_s>=get_start_tim&&current_time_s<=get_end_tim)
	{		
		    // 检查是否需要启动电机  标准启动时间 正负1s，因为唤醒时间为3s，  
       if ((current_time_s - last_motor_start_time_s) >= (weak_up_tim_interval_s)&&(current_time_s - last_motor_start_time_s) <= (weak_up_tim_interval_s+2)) {
        // 启动电机
        motor_low_powre_start(200); // 200 表示启动时间，单位需根据电机设计定义

        // 更新上次启动时间
        last_motor_start_time_s = current_time_s;

        // 打印日志信息
        co_printf("............Motor started at %02d:%02d:%02d............s\r\n", 
                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);
       }
      else {
//        // 打印日志，表示未满足启动条件
//        co_printf("Motor not started, current time: %02d:%02d:%02d\r\n", 
//                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);
       }
	
   }else{
   
     co_printf("not run tim......\r\n");
   
   }
}



void motor_start_process(void)
{
	uint32_t now_basal_rate_info = get_now_basal_rate_info(&clock_env); 
	printf("now_basal_rate_info is %d \r\n",now_basal_rate_info);
	
	uint32_t weak_up_tim_interval_s = basal_rate_calculate_wake_up_time(&s_rate_info[now_basal_rate_info-1]);
	printf("weak_up_tim %d \r\n",weak_up_tim_interval_s);
	
    is_motor_start(&clock_env,weak_up_tim_interval_s,&s_rate_info[now_basal_rate_info-1]);
}



 /* 出睡眠时执行函数 */
__attribute__((section("ram_code"))) void user_entry_after_sleep_imp(void)
{
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
	
    uart_init(UART1, BAUD_RATE_115200); 
	
    NVIC_EnableIRQ(UART1_IRQn);	
		
	uart_putc_noint_no_wait(UART1, 'w');
	
    NVIC_EnableIRQ(PMU_IRQn);
	
	pmu_set_led2_value(0); 	//开启led		
	
    //时间上增加3s
	clock_hdl();
    clock_hdl();
	clock_hdl();
	co_printf("\r\n%04d-%02d-%02d ", clock_env.year, clock_env.month, clock_env.day);
	co_printf("%02d:%02d:%02d \r\n", clock_env.hour, clock_env.min, clock_env.sec);			
	    
	motor_start_process();
}

/*********************************************************************
 * @fn      user_entry_before_ble_init
 *
 * @brief   Code to be executed before BLE stack to be initialized.
 *          Power mode configurations, PMU part driver interrupt enable, MCU 
 *          peripherals init, etc. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void user_entry_before_ble_init(void)
{    
     /* set system power supply in BUCK mode */
    pmu_set_sys_power_mode(PMU_SYS_POW_BUCK);
#ifdef FLASH_PROTECT
    flash_protect_enable(1);
#endif
    pmu_enable_irq(PMU_ISR_BIT_ACOK
                   | PMU_ISR_BIT_ACOFF
                   | PMU_ISR_BIT_ONKEY_PO
                   | PMU_ISR_BIT_OTP
                   | PMU_ISR_BIT_LVD
                   | PMU_ISR_BIT_BAT
                   | PMU_ISR_BIT_ONKEY_HIGH);
    NVIC_EnableIRQ(PMU_IRQn);
    
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    uart_init(UART1, BAUD_RATE_115200);   

    system_set_port_mux(GPIO_PORT_D,GPIO_BIT_4,PORTD4_FUNC_D4);
  
    gpio_set_dir(GPIO_PORT_D, GPIO_BIT_4, GPIO_DIR_OUT);
	
    gpio_set_pin_value(GPIO_PORT_D,GPIO_BIT_4,1);	
}


#define DISABLE_SLEEP 0

/*********************************************************************
 * @fn      user_entry_after_ble_init
 *
 * @brief   Main entrancy of user application. This function is called after BLE stack
 *          is initialized, and all the application code will be executed from here.
 *          In that case, application layer initializtion can be startd here. 
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void user_entry_after_ble_init(void)
{ 		
#if 0
//    system_sleep_disable();		//disable sleep 
#else
    if(__jump_table.system_option & SYSTEM_OPTION_SLEEP_ENABLE)  //if sleep is enalbed, delay 3s for JLINK 
    {
        co_printf("\r\na");
        co_delay_100us(10000);       
        co_printf("\r\nb");
        co_delay_100us(10000);
        co_printf("\r\nc");
        co_delay_100us(10000);
        co_printf("\r\nd");
    }
#endif
	
    pmu_set_pin_pull(GPIO_PORT_D, (1<<GPIO_BIT_4)|(1<<GPIO_BIT_5), true);
    pmu_port_wakeup_func_set(GPIO_PD4|GPIO_PD5);
	
}





void rtc_isr_ram(uint8_t rtc_idx)
{
    if(rtc_idx == RTC_A)
    {
      //唤醒		
	  system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_UART1_RXD);
      system_set_port_mux(GPIO_PORT_A, GPIO_BIT_3, PORTA3_FUNC_UART1_TXD);
    
      uart_init(UART1, BAUD_RATE_115200);
		
	  co_printf("\r\n rtc weak up \r\n");
		
	  ble_init();
		
	  //关闭rtc
	  
//    system_sleep_disable();
				
//	  co_delay_100us(20000);
		
//	  system_sleep_enable();
    }
	
    if(rtc_idx == RTC_B)
    {		
//	  clock_hdl();
//	
//	  co_printf("%04d-%02d-%02d ", clock_env.year, clock_env.month, clock_env.day);
//	  co_printf("%02d:%02d:%02d \r\n", clock_env.hour, clock_env.min, clock_env.sec);    
    }
}





