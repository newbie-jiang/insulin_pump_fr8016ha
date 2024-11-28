
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
	uint32_t weak_up_tim_interval_s = get_weak_up_tim_interval_s();  //�����ʼ���ʱ����
	
//	calculate_wake_up_times(&rate_info,weak_up_tim_interval_s,7100); //�������л���ʱ�䲢��ӡ
	
    //����ʱ��������1�ε��
	is_motor_start(&clock_env,weak_up_tim_interval_s,&rate_info);
	
//    //����ʱ�������������
//    if(clock_env.hour==12&&clock_env.min==00&&(clock_env.sec>=10&&clock_env.sec<=30))
//	{
//      motor_low_powre_start(200);	
//	}
		
//	adc_task_fun(NULL);
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



/*************************** �͵�����ⱨ�� ********************************/
uint8_t is_lopowre;

void lopowre_callback(void)
{
	static int buzzer_pwm_started = 0; // ��� buzzer pwm �Ƿ��Ѿ�����
	
	 if(is_lopowre==1) //�͵�������
	 {
	  	  if (!buzzer_pwm_started)
        {
            pwm_start(PWM_CHANNEL_4); // ���� buzzer pwm
            buzzer_pwm_started = 1;   // ��� buzzer pwm �Ѿ�����
        }		
	
	 }else{
		  if (buzzer_pwm_started)
        {
            pwm_stop(PWM_CHANNEL_4); // �ر� buzzer pwm
            buzzer_pwm_started = 0;  // ���ñ��
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



/***************** �ƶ�ƽ���˲� *****************/

// �������ݴ��ڴ�С
#define WINDOW_SIZE 6

// ����һ���������洢��ʷ����
static int adc_history[WINDOW_SIZE] = {0};  
static int index = 0;  // ��ǰ���ݲ���λ��
static int sum = 0;    // �洢�������ݵ��ۻ���

//// ����ƽ���˲�����
//int moving_average_filter(int new_adc_value) {
//    // ������ʷ�������飬�Ƴ���ɵ����ݲ�����²�������
//    sum -= adc_history[index];         // �Ӻ��м�ȥ��ɵ���ֵ
//    adc_history[index] = new_adc_value; // ���µ�ǰ��������ʷ����
//    sum += new_adc_value;               // ����ֵ�ӵ�����

//    // ����������ȷ������ѭ��
//    index = (index + 1) % WINDOW_SIZE;

//    // ���㲢����ƽ��ֵ
//    return sum / WINDOW_SIZE;
//}
/************************************************/



/*  ��ֵ�˲�  */
// ���ڴ洢��� WINDOW_SIZE �� ADC ֵ������
//static int adc_history[WINDOW_SIZE] = {0};

// ���ڼ�¼��ʷ���ݵ�����
static int current_index = 0;

// ��ֵ�˲�����
int median_filter(int new_adc_value) {
    // ���²��������ݴ�����ʷ��������
    adc_history[current_index] = new_adc_value;

    // ����������ȷ������ѭ��
    current_index = (current_index + 1) % WINDOW_SIZE;

    // ����ǰ�����������
    int sorted_history[WINDOW_SIZE];
    for (int i = 0; i < WINDOW_SIZE; i++) {
        sorted_history[i] = adc_history[i];
    }

    // ʹ�ü򵥵�ð������Ҳ����ʹ�ø���Ч�������㷨��
    for (int i = 0; i < WINDOW_SIZE - 1; i++) {
        for (int j = i + 1; j < WINDOW_SIZE; j++) {
            if (sorted_history[i] > sorted_history[j]) {
                int temp = sorted_history[i];
                sorted_history[i] = sorted_history[j];
                sorted_history[j] = temp;
            }
        }
    }
    // ���㲢������ֵ���м�λ�õ���ֵ��
    return sorted_history[WINDOW_SIZE / 2];  // ��λ���������������м�Ԫ��
}



//// ���ڴ洢���5��ADCֵ������
//static int adc__history[HISTORY_SIZE] = {0};

//// ���ڼ�¼��ʷ���ݵ�����
//static int history_index = 0;

//// ÿ���ӵ��õĲɼ��ص�����
//void adc_manage_callback(int adc_val)
//{
//    // ���²ɼ������ݴ��뻷�λ�����
//    adc__history[history_index] = adc_val;

//    // ������ʷ����������ѭ������
//    history_index = (history_index + 1) % HISTORY_SIZE;

//    // �ж��Ƿ��Ѿ��ռ���5�����ݵ�
//    if (history_index == 0) {
//        // ������������ʼ�Ƚ�����������������ݵĴ�С
//        int data1 = adc__history[0];   // ���������
//        int data5 = adc__history[4];   // ���µ�����

//        // �������5 ���ڻ���� ����1����ӡ"stop"  ����ʱ������Ӧ�����μ�С
//        if (data5 >= data1) {
//            printf(" motor block\r\n");
//        }
//    }
//}


/********************************** ��λ����� ******************************************/

// ��λ����������
void potentiometer_block(uint8_t block_state)
{
	static int buzzer_pwm_started = 0; // ��� buzzer pwm �Ƿ��Ѿ�����

	if(block_state) // block -> buzzer on
	{
		  if (!buzzer_pwm_started)
        {
            pwm_start(PWM_CHANNEL_4); // ���� buzzer pwm
            buzzer_pwm_started = 1;   // ��� buzzer pwm �Ѿ�����
        }		
	}else{    // buzzer off
		
		 if (buzzer_pwm_started)
        {
            pwm_stop(PWM_CHANNEL_4); // �ر� buzzer pwm
            buzzer_pwm_started = 0;  // ���ñ��
        }	
	}      
}


#define HISTORY_SIZE 6

// ���ڴ洢���5��ADCֵ������
static int adc__history[HISTORY_SIZE] = {0};

// ���ڼ�¼��ʷ���ݵ�����
static int history_index = 0;

// ���ڼ�¼��Ч���ݵ�����
static int data_count = 0;

uint8_t is_potentiometer;

// ÿ���ӵ��õĲɼ��ص�����
void adc_manage_callback(int adc_val)
{
    // ���²ɼ������ݴ��뻷�λ�����
    adc__history[history_index] = adc_val;

    // ������ʷ����������ѭ������
    history_index = (history_index + 1) % HISTORY_SIZE;

    // ��������е������Ѿ���5�������бȽ�
    if (data_count == HISTORY_SIZE) {
        int data1 = adc__history[history_index]; // ��ɵ�����
        int data5 = adc__history[(history_index + HISTORY_SIZE - 1) % HISTORY_SIZE]; // ���µ�����

        // �������5 ���ڻ���� ����1����ӡ"motor block"
        if (data5 >= data1) {
			is_potentiometer = 1;
//            printf("motor block\r\n");
//			potentiometer_block(is_potentiometer); //��λ������
        }else{
			
		    is_potentiometer = 0;	
//            potentiometer_block(is_potentiometer); //��λ������			
		}
    } else {
           // �����������û�ﵽ5��������������������
           data_count++;
    }
}


//����г̣��������

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
	int smoothing_result = median_filter(result_pd6); //��ֵ�˲�	
	//��ӡʱ�亯��
	
//	co_printf("adc smoothing val: %d\r\n",smoothing_result);	
		
//	co_printf("pd6 adc vol: %d mv\r\n",(result_pd6 *33*100/1024)); // ��ѹ ��������С����������ݴ���
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



/**********************************Cjson��������***********************************************/
void send_message_stop(void)
{
	
	infusion_information infusion_inf = {
		.states = "stop",
		.remainder_drug = 100.0,
		.have_finished_drug = 200.0,
		.basal_rate = 1
	};	
	
	
	// ����������
    cJSON *root = cJSON_CreateObject();

    // ���� "running" �Ӷ���
    cJSON *stop = cJSON_CreateObject();
	
	 cJSON_AddItemToObject(root, "stop", stop);

    // ������ݵ� "running"
    cJSON_AddStringToObject(stop, "states", infusion_inf.states);

    // ��������תΪ��ʽ���ַ�����ȷ������������λ��С������һλ
    char remainder_str[6];
    char finished_str[6];
	char basal_rate_str[6]; 
	  
    snprintf(remainder_str, sizeof(remainder_str), "%d", infusion_inf.remainder_drug);
    snprintf(finished_str, sizeof(finished_str), "%d", infusion_inf.have_finished_drug);
    snprintf(basal_rate_str, sizeof(basal_rate_str), "%d", infusion_inf.basal_rate);  
	  
    cJSON_AddStringToObject(stop, "remainder drug", remainder_str);
    cJSON_AddStringToObject(stop, "have finished drug", finished_str);
    cJSON_AddStringToObject(stop, "basal rate", basal_rate_str);

    // �� JSON תΪ�ַ���
    char *json_string = cJSON_Print(root);
    if (json_string != NULL) {
        
        co_printf("JSON data to send: %s\n", json_string);
    }

    // �����ڴ�
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
		
	// ����������
    cJSON *root = cJSON_CreateObject();

    // ���� "running" �Ӷ���
    cJSON *running = cJSON_CreateObject();
	
	 cJSON_AddItemToObject(root, "running", running);

    // ������ݵ� "running"
    cJSON_AddStringToObject(running, "states", infusion_inf.states);

    // ��������תΪ��ʽ���ַ�����ȷ������������λ��С������һλ
    char remainder_str[6];
    char finished_str[6];
	char basal_rate_str[6]; 
	  
    snprintf(remainder_str, sizeof(remainder_str), "%d", infusion_inf.remainder_drug);
    snprintf(finished_str, sizeof(finished_str), "%d", infusion_inf.have_finished_drug);
    snprintf(basal_rate_str, sizeof(basal_rate_str), "%d", infusion_inf.basal_rate);  
	  
    cJSON_AddStringToObject(running, "remainder drug", remainder_str);
    cJSON_AddStringToObject(running, "have finished drug", finished_str);
    cJSON_AddStringToObject(running, "basal rate", basal_rate_str);

    // �� JSON תΪ�ַ���
    char *json_string = cJSON_Print(root);
    if (json_string != NULL) {
        
        co_printf("JSON data to send: %s\n", json_string);
    }

    // �����ڴ�
    cJSON_Delete(root);
    os_free(json_string);  
	
}

/*************************************Cjson��������*****************************************/


//typedef struct basal_rate_information{
//	
//	char basal_rate_num; //�����ʶ���
//	float basal_rate_speed;//�������ٶ�
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
    "basal_rate_num":"x" //����������(��ǰ�ǵڼ���)
    "speed":"xx.x" //�������ٶ�
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
   fun   :   ��ȡ˯��ʱ��
   para1 ��  basal_rate:������
   return��  ˯��ʱ�� 

   0.1087���������£�

   ֹͣ1s���˶�20ms���˶�һȦ��ʱ  2min

   �˶�һȦ�Ĳ���Ϊ120/1.02 = 117��

   12.72��һȦ������ / 117 = 0.1087U��һ��������

*/
uint32_t get_sleep_tim_s(float basal_rate)
{
	return (3600/(basal_rate/0.1087));
}


extern os_timer_t instruct_check_task;

extern void stop_task(void);
extern void start_task(void);

//��������
void instruct_check_task_fun(void * arg)
{		
	instruct_legal_check_process(); //����ǰ������	
}

