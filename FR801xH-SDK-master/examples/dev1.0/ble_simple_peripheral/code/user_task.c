
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

#include "cJSON.h"
#include "driver_rtc.h"
#include "application.h"
//#define demo  


extern uint32_t speed_set_flag;

void motor_task_fun(void *arg)
{	
	uint32_t weak_up_tim_interval_s = get_weak_up_tim_interval_s();  //基础率计算时间间隔
	
//	calculate_wake_up_times(&rate_info,weak_up_tim_interval_s,7100); //计算所有唤醒时间并打印
	
    //依据时间间隔启动1次电机
	is_motor_start(&clock_env,weak_up_tim_interval_s,&rate_info);
	
//    //依据时间启动电机测试
//    if(clock_env.hour==12&&clock_env.min==00&&(clock_env.sec>=10&&clock_env.sec<=30))
//	{
//      motor_low_powre_start(200);	
//	}
		
//	adc_task_fun(NULL);
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



/*************************** 低电量检测报警 ********************************/
uint8_t is_lopowre;

void lopowre_callback(void)
{
	static int buzzer_pwm_started = 0; // 标记 buzzer pwm 是否已经启动
	
	 if(is_lopowre==1) //低电量报警
	 {
	  	  if (!buzzer_pwm_started)
        {
            pwm_start(PWM_CHANNEL_4); // 开启 buzzer pwm
            buzzer_pwm_started = 1;   // 标记 buzzer pwm 已经启动
        }		
	
	 }else{
		  if (buzzer_pwm_started)
        {
            pwm_stop(PWM_CHANNEL_4); // 关闭 buzzer pwm
            buzzer_pwm_started = 0;  // 重置标记
        }		
     }
}



extern float get_vbat_adc_val(void);

/* electric quantity detection */ 
void electric_quantity_task_fun(void *arg)
{
	static  int  vol_val;
	
    vol_val = get_vbat_adc_val();
		
	if(vol_val<2500){ 
		
		is_lopowre =1;	
	    lopowre_callback();	
		
	}else if(vol_val>2600){
		
		is_lopowre = 0;
	    lopowre_callback();		
	}	
  co_printf("vol_val = %dmV \r\n",vol_val);		
}


/*****************************************************************************/



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



/***************** 移动平滑滤波 *****************/

// 定义数据窗口大小
#define WINDOW_SIZE 6

// 定义一个数组来存储历史数据
static int adc_history[WINDOW_SIZE] = {0};  
static int index = 0;  // 当前数据插入位置
static int sum = 0;    // 存储窗口数据的累积和

//// 滑动平均滤波函数
//int moving_average_filter(int new_adc_value) {
//    // 更新历史数据数组，移除最旧的数据并添加新采样数据
//    sum -= adc_history[index];         // 从和中减去最旧的数值
//    adc_history[index] = new_adc_value; // 更新当前索引的历史数据
//    sum += new_adc_value;               // 将新值加到和中

//    // 更新索引，确保索引循环
//    index = (index + 1) % WINDOW_SIZE;

//    // 计算并返回平均值
//    return sum / WINDOW_SIZE;
//}
/************************************************/



/*  中值滤波  */
// 用于存储最近 WINDOW_SIZE 个 ADC 值的数组
//static int adc_history[WINDOW_SIZE] = {0};

// 用于记录历史数据的索引
static int current_index = 0;

// 中值滤波函数
int median_filter(int new_adc_value) {
    // 将新采样的数据存入历史数据数组
    adc_history[current_index] = new_adc_value;

    // 更新索引，确保索引循环
    current_index = (current_index + 1) % WINDOW_SIZE;

    // 将当前数组进行排序
    int sorted_history[WINDOW_SIZE];
    for (int i = 0; i < WINDOW_SIZE; i++) {
        sorted_history[i] = adc_history[i];
    }

    // 使用简单的冒泡排序（也可以使用更高效的排序算法）
    for (int i = 0; i < WINDOW_SIZE - 1; i++) {
        for (int j = i + 1; j < WINDOW_SIZE; j++) {
            if (sorted_history[i] > sorted_history[j]) {
                int temp = sorted_history[i];
                sorted_history[i] = sorted_history[j];
                sorted_history[j] = temp;
            }
        }
    }
    // 计算并返回中值（中间位置的数值）
    return sorted_history[WINDOW_SIZE / 2];  // 中位数是排序后数组的中间元素
}



//// 用于存储最近5个ADC值的数组
//static int adc__history[HISTORY_SIZE] = {0};

//// 用于记录历史数据的索引
//static int history_index = 0;

//// 每秒钟调用的采集回调函数
//void adc_manage_callback(int adc_val)
//{
//    // 将新采集的数据存入环形缓冲区
//    adc__history[history_index] = adc_val;

//    // 更新历史数据索引，循环更新
//    history_index = (history_index + 1) % HISTORY_SIZE;

//    // 判断是否已经收集到5个数据点
//    if (history_index == 0) {
//        // 数据已满，开始比较最新数据与最旧数据的大小
//        int data1 = adc__history[0];   // 最早的数据
//        int data5 = adc__history[4];   // 最新的数据

//        // 如果数据5 大于或等于 数据1，打印"stop"  运行时候数据应该依次减小
//        if (data5 >= data1) {
//            printf(" motor block\r\n");
//        }
//    }
//}


/********************************** 电位器检测 ******************************************/

// 电位器阻塞报警
void potentiometer_block(uint8_t block_state)
{
	static int buzzer_pwm_started = 0; // 标记 buzzer pwm 是否已经启动

	if(block_state) // block -> buzzer on
	{
		  if (!buzzer_pwm_started)
        {
            pwm_start(PWM_CHANNEL_4); // 开启 buzzer pwm
            buzzer_pwm_started = 1;   // 标记 buzzer pwm 已经启动
        }		
	}else{    // buzzer off
		
		 if (buzzer_pwm_started)
        {
            pwm_stop(PWM_CHANNEL_4); // 关闭 buzzer pwm
            buzzer_pwm_started = 0;  // 重置标记
        }	
	}      
}


#define HISTORY_SIZE 6

// 用于存储最近5个ADC值的数组
static int adc__history[HISTORY_SIZE] = {0};

// 用于记录历史数据的索引
static int history_index = 0;

// 用于记录有效数据的数量
static int data_count = 0;

uint8_t is_potentiometer;

// 每秒钟调用的采集回调函数
void adc_manage_callback(int adc_val)
{
    // 将新采集的数据存入环形缓冲区
    adc__history[history_index] = adc_val;

    // 更新历史数据索引，循环更新
    history_index = (history_index + 1) % HISTORY_SIZE;

    // 如果缓存中的数据已经有5个，进行比较
    if (data_count == HISTORY_SIZE) {
        int data1 = adc__history[history_index]; // 最旧的数据
        int data5 = adc__history[(history_index + HISTORY_SIZE - 1) % HISTORY_SIZE]; // 最新的数据

        // 如果数据5 大于或等于 数据1，打印"motor block"
        if (data5 >= data1) {
			is_potentiometer = 1;
//            printf("motor block\r\n");
//			potentiometer_block(is_potentiometer); //电位器阻塞
        }else{
			
		    is_potentiometer = 0;	
//            potentiometer_block(is_potentiometer); //电位器阻塞			
		}
    } else {
           // 如果数据量还没达到5个，则增加数据量计数
           data_count++;
    }
}


//检测行程，检测阻塞

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
//  co_printf("pd6 adc val: %d\r\n",result_pd6);	
	int smoothing_result = median_filter(result_pd6); //中值滤波	
	//打印时间函数
	
//	co_printf("adc smoothing val: %d\r\n",smoothing_result);	
		
//	co_printf("pd6 adc vol: %d mv\r\n",(result_pd6 *33*100/1024)); // 电压 计算中有小数会造成数据错误
	adc_manage_callback(smoothing_result);
				
//	 struct adc_cfg_t cfg_pd7;
//	 uint16_t result_pd7;
//	 float  vol_pd7;
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
////    vol_pd7 = (float)result_pd7 * 3.3/1023;
////	co_printf("pd7 vol val: %f\r\n",vol_pd7);	

}
/******************************************************************************************/



/**********************************Cjson发送数据***********************************************/
void send_message_stop(void)
{
	
	infusion_information infusion_inf = {
		.states = "stop",
		.remainder_drug = 100.0,
		.have_finished_drug = 200.0,
		.basal_rate = 1
	};	
	
	
	// 创建根对象
    cJSON *root = cJSON_CreateObject();

    // 创建 "running" 子对象
    cJSON *stop = cJSON_CreateObject();
	
	 cJSON_AddItemToObject(root, "stop", stop);

    // 添加数据到 "running"
    cJSON_AddStringToObject(stop, "states", infusion_inf.states);

    // 将浮点数转为格式化字符串，确保整数部分两位，小数部分一位
    char remainder_str[6];
    char finished_str[6];
	char basal_rate_str[6]; 
	  
    snprintf(remainder_str, sizeof(remainder_str), "%d", infusion_inf.remainder_drug);
    snprintf(finished_str, sizeof(finished_str), "%d", infusion_inf.have_finished_drug);
    snprintf(basal_rate_str, sizeof(basal_rate_str), "%d", infusion_inf.basal_rate);  
	  
    cJSON_AddStringToObject(stop, "remainder drug", remainder_str);
    cJSON_AddStringToObject(stop, "have finished drug", finished_str);
    cJSON_AddStringToObject(stop, "basal rate", basal_rate_str);

    // 将 JSON 转为字符串
    char *json_string = cJSON_Print(root);
    if (json_string != NULL) {
        
        co_printf("JSON data to send: %s\n", json_string);
    }

    // 清理内存
    cJSON_Delete(root);
    os_free(json_string);  
}


void send_message_run(void)	
{
	infusion_information infusion_inf = {
		.states = "run",
		.remainder_drug = 200.0,
		.have_finished_drug = 100.0,
		.basal_rate = 1
	};	
		
	// 创建根对象
    cJSON *root = cJSON_CreateObject();

    // 创建 "running" 子对象
    cJSON *running = cJSON_CreateObject();
	
	 cJSON_AddItemToObject(root, "running", running);

    // 添加数据到 "running"
    cJSON_AddStringToObject(running, "states", infusion_inf.states);

    // 将浮点数转为格式化字符串，确保整数部分两位，小数部分一位
    char remainder_str[6];
    char finished_str[6];
	char basal_rate_str[6]; 
	  
    snprintf(remainder_str, sizeof(remainder_str), "%d", infusion_inf.remainder_drug);
    snprintf(finished_str, sizeof(finished_str), "%d", infusion_inf.have_finished_drug);
    snprintf(basal_rate_str, sizeof(basal_rate_str), "%d", infusion_inf.basal_rate);  
	  
    cJSON_AddStringToObject(running, "remainder drug", remainder_str);
    cJSON_AddStringToObject(running, "have finished drug", finished_str);
    cJSON_AddStringToObject(running, "basal rate", basal_rate_str);

    // 将 JSON 转为字符串
    char *json_string = cJSON_Print(root);
    if (json_string != NULL) {
        
        co_printf("JSON data to send: %s\n", json_string);
    }

    // 清理内存
    cJSON_Delete(root);
    os_free(json_string);  
	
}

/*************************************Cjson接收数据*****************************************/


//typedef struct basal_rate_information{
//	
//	char basal_rate_num; //基础率段数
//	float basal_rate_speed;//基础率速度
//	
//	char basal_rate_start_tim_hh;
//	char basal_rate_start_tim_min;
//	char basal_rate_start_tim_s;
//	
//	char basal_rate_end_tim_hh;
//	char basal_rate_end_tim_min;
//	char basal_rate_end_tim_s;
//	
//}basal_rate_information;

/*
   "basal_rate":
  {
    "basal_rate_num":"x" //基础率数量(当前是第几段)
    "speed":"xx.x" //基础率速度
    "basal_rate_tim":"xx:xx:xx-xx:xx:xx"  
  }

 "basal_rate":
  {
    "basal_rate_num":"1" 
    "speed":"5.0" 
    "basal_rate_tim":"00:00:00-24:00:00"  
  }
*/


/*******************************************************************************************/
void rtc_tim_task_fun (void *arg)
{	    
	  clock_hdl();
	  co_printf("rtc_tim_task_fun: ");
	  co_printf("%04d-%02d-%02d ", clock_env.year, clock_env.month, clock_env.day);
	  co_printf("%02d:%02d:%02d \r\n", clock_env.hour, clock_env.min, clock_env.sec);
}

/*
   fun   :   获取睡眠时间
   para1 ：  basal_rate:基础率
   return：  睡眠时间 

   0.1087计算结果如下：

   停止1s，运动20ms，运动一圈用时  2min

   运动一圈的步数为120/1.02 = 117步

   12.72（一圈的量） / 117 = 0.1087U（一步的量）

*/
uint32_t get_sleep_tim_s(float basal_rate)
{
	return (3600/(basal_rate/0.1087));
}


extern os_timer_t instruct_check_task;

extern void stop_task(void);
extern void start_task(void);

//单次任务
void instruct_check_task_fun(void * arg)
{		
	instruct_legal_check_process(); //处理当前的数据	
}

