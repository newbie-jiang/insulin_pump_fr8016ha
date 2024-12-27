#include "application.h"
#include  "co_printf.h"
#include  "time_rtc.h"


//�������
#define  MAX_TIM_HH    23
#define  MIN_TIM_HH    0

#define  MAX_TIM_MIN   59
#define  MIN_TIM_MIN   0

#define  MAX_TIM_S     59
#define  MIN_TIM_S     0


//�����ʼ�������ʱ����
uint32_t basal_rate_calculate_wake_up_time(basal_rate_information * p_basal_rate_info)
{
	static uint16_t start_hh,start_min;
	static uint16_t end_hh,end_min;	
	static float basal_rate_speed;   //�������ٶ�
	char basal_rate_num; //�����ʶ���
	
	uint32_t weak_up_tim_interval_s; //�������ʱ���� 
		
    start_hh  =  p_basal_rate_info->basal_rate_start_tim_hh;
 	start_min =  p_basal_rate_info->basal_rate_start_tim_min;
	end_hh    =  p_basal_rate_info->basal_rate_end_tim_hh;
    end_min   =  p_basal_rate_info->basal_rate_end_tim_min;	
	basal_rate_speed = p_basal_rate_info->basal_rate_speed;
	basal_rate_num   = p_basal_rate_info->basal_rate_num;
	
	/*
	  ���㹫ʽ�Ƶ�
	  ������ = x   tim_s = ����ʱ����    tim_s = 3600 /  (x / 0.1087)��    
	  0.1087(һ������) -> ֹͣ1s���˶�20ms���˶�һȦ��ʱ  2min | �˶�һȦ�Ĳ���Ϊ120/1.02 = 117��|һ������ 12.72(���ת��һȦ����) / 117 = 0.1087U
	*/
	
	//���㻽��ʱ����
	weak_up_tim_interval_s = (uint32_t)(3600 / (basal_rate_speed / 0.1087));
	
	//���㻽��ʱ��(������Ӧ����ram�б��֣��Ա�rtc��ʱ����)
	
	
	return weak_up_tim_interval_s;
}

/* ��û�����ʱ���� */
uint32_t get_weak_up_tim_interval_s(void)
{
  co_printf("weak_up_tim_interval_s = %d s \r\n",basal_rate_calculate_wake_up_time(&rate_info));
  return basal_rate_calculate_wake_up_time(&rate_info);
}


// ���㻽��ʱ�䲢�洢���ṹ������
void calculate_wake_up_times(  basal_rate_information * p_basal_rate_info,
                          	   uint32_t weak_up_tim_interval_s ,
							   int max_size 
						     )   
{		
	static uint16_t start_hh,start_min;
	static uint16_t end_hh,end_min;	
			
    start_hh  =  p_basal_rate_info->basal_rate_start_tim_hh;
 	start_min =  p_basal_rate_info->basal_rate_start_tim_min;
	end_hh    =  p_basal_rate_info->basal_rate_end_tim_hh;
    end_min   =  p_basal_rate_info->basal_rate_end_tim_min;	
	
    int count = 0;
    // ת����ʼ�ͽ���ʱ��Ϊ�루�Է���Ϊ��λ��
    uint32_t start_time_s = start_hh * 3600 + start_min * 60;
    uint32_t end_time_s = end_hh * 3600 + end_min * 60;
    uint32_t current_time_s = start_time_s;

    // �����������
    uint32_t day_seconds = 24 * 3600; // һ���������
    if (end_time_s < start_time_s) {
        end_time_s += day_seconds; // ���촦��������ʱ�����Ϊ�ڶ��������
    }

    // ��������㻽��ʱ��
    while (current_time_s <= end_time_s && count < max_size) {
        // ת����ǰʱ��Ϊ hh:mm:ss
//        uint32_t seconds_of_day = current_time_s % day_seconds; // ��ǰʱ��ĵ�������
//        wake_up_times[count].hh = seconds_of_day / 3600;
//        wake_up_times[count].min = (seconds_of_day % 3600) / 60;
//        wake_up_times[count].s = seconds_of_day % 60;
        count++;
   
        // ����ʱ��
        current_time_s += weak_up_tim_interval_s;
    }
		
	/* ��������ʱ�� */
	#if 1
//	co_printf("printf weak up tim\r\n");
//	for(int i=0 ; i<count; i++)
//	{
//	  co_printf("wakeup_time[%d]---%02d:%02d:%02d\r\n",i,wake_up_times[i].hh,wake_up_times[i].min,wake_up_times[i].s);	
//	}
	#endif
	
	co_printf("calculate_wake_up_times success\r\n");
	
//    return count; // �������ɵĻ���ʱ������
	printf(" count is %d\r\n",count);
}



//������ָ��Ϸ��Լ��  ��������
int basal_rate_Instruction_legal_check(basal_rate_information * p_basal_rate_info)
{
	static uint16_t start_hh,start_min;
	static uint16_t end_hh,end_min;	
			
    start_hh  =  p_basal_rate_info->basal_rate_start_tim_hh;
 	start_min =  p_basal_rate_info->basal_rate_start_tim_min;
	end_hh    =  p_basal_rate_info->basal_rate_end_tim_hh;
    end_min   =  p_basal_rate_info->basal_rate_end_tim_min;	
	
	if(start_hh>MAX_TIM_HH||end_hh>MAX_TIM_HH)
	{
      co_printf("basal_rate_Instruction_legal_check err 1");		
	  return -1;
	}else if(start_hh<MIN_TIM_HH||end_hh<MIN_TIM_HH)
	{
	  co_printf("basal_rate_Instruction_legal_check err 2");	
	  return -1;
	}else if(start_min>MAX_TIM_MIN||end_min>MAX_TIM_MIN)
	{
	  co_printf("basal_rate_Instruction_legal_check err 3");	
	  return -1;
	}else if(start_min<MIN_TIM_MIN||end_hh<MIN_TIM_MIN){
		
	  co_printf("basal_rate_Instruction_legal_check err 4");	
	  return -1;	
	}else{
	
	 co_printf("basal_rate_Instruction_legal_check success \r\n");
	}
	
	return 0;
}


//ʱ��ͬ��ָ��Ϸ��Լ��  ��������
int sync_tim_Instruction_legal_check(void)
{


   return 0;
}


//�����ָ��Ϸ��Լ�� ��������
int large_doseInstruction_legal_check(void)
{
	
	


  return 0;
}




// pack_num  s_pack_num = {0};

//ָ���  �������һ��ָ��
void instruct_legal_check_process(void)
{		
	switch(s_pack_num.cjson_instruct_num)
	{
		case 1:
			if(!basal_rate_Instruction_legal_check(&rate_info)) //���������ݺϷ��Լ��ͨ��
			{
			  calculate_wake_up_times(&rate_info,get_weak_up_tim_interval_s(),6100); //���㻽��ʱ��
			}			
			break;
			
		case 2:
			break;
		
		    sync_tim_Instruction_legal_check();    //ʱ��ͬ�����ݺϷ��Լ��
				
		case 3:
			
		    break;
		
		    large_doseInstruction_legal_check();    //��������ݺϷ��Լ��
		
		default:
			break;
	}
}







//#define    NORMAL_DEBUG
//#define    SW_DEBUG
#define    DW_DEBUG

extern void motor_low_powre_start(uint32_t delay_ms);

//���������ģʽ����   ���ú�����������������������״̬
void normal_large_dose_mode_start(normal_large_dose *p_n_large_dose)
{
	//��ȡ����
	float get_liquid  =  p_n_large_dose->large_dose_liquid;
	
	#ifdef NORMAL_DEBUG
	printf("get_liquid is %f\r\n",get_liquid);
	#endif
		
	/*
	  0.1087(һ������ 20ms)
	*/
	
	//���ݼ�����������ʱ��ms  (get_liquid/0.1087) һ��Ҫ���ж��ٲ�
	uint32_t run_ms = (get_liquid/0.1087) * 20; 
		
	//���е��
	motor_low_powre_start(run_ms*10);		
}



 uint32_t sw_last_motor_start_time_s = 0;

//�����������
void sw_is_motor_start(clock_param_t *p_clock_env, uint32_t weak_up_tim_interval_s, square_wave_large_dose * p_s_w_large_dose)
{
    //���㵱ǰʱ���Ӧ������
    uint32_t  current_time_s = p_clock_env->hour * 3600 + 
                              p_clock_env->min * 60 + 
                              p_clock_env->sec;
    // ����������
    uint32_t day_seconds = 24 * 3600;
		
	uint32_t get_run_tim  =  p_s_w_large_dose->tim_hh*3600 + p_s_w_large_dose->tim_min*60;
	
    // ʹ�� static ��ֻ֤��ʼ��һ��
    static uint32_t get_start_tim = 0;
    static uint32_t get_end_tim = 0;

    // �ж��Ƿ�Ϊ��һ�γ�ʼ��
    if (get_start_tim == 0 && get_end_tim == 0) {
        get_start_tim = current_time_s;    
    }
	
	get_end_tim = get_start_tim+get_run_tim;
	
	#ifdef SW_DEBUG
	co_printf("get_start_tim = %d\r\n",get_start_tim);
	co_printf("get_end_tim = %d\r\n",get_end_tim);
	#endif
				
    if (current_time_s < sw_last_motor_start_time_s) {
        // ���죺���� last_motor_start_time_s
        sw_last_motor_start_time_s = 0;
    }
	
	 #ifdef SW_DEBUG
	 co_printf("now  tim_s:%d\r\n",current_time_s);
	 co_printf("last tim_s:%d\r\n",sw_last_motor_start_time_s);
	 #endif
	
	//��������ʱ��
	if(current_time_s>=get_start_tim&&current_time_s<=get_end_tim)
	{		
		    // ����Ƿ���Ҫ�������  ��׼����ʱ�� ����1s����Ϊ����ʱ��Ϊ3s��  
       if ((current_time_s - sw_last_motor_start_time_s) >= (weak_up_tim_interval_s)&&(current_time_s - sw_last_motor_start_time_s) <= (weak_up_tim_interval_s+2)) {
        // �������
        motor_low_powre_start(200); // 200 ��ʾ����ʱ�䣬��λ����ݵ����ƶ���

        // �����ϴ�����ʱ��
        sw_last_motor_start_time_s = current_time_s;
         
		#ifdef SW_DEBUG
        // ��ӡ��־��Ϣ
        co_printf(".....sw.......Motor started at %02d:%02d:%02d............s\r\n", 
                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);
		#endif
    }
   else{
//        // ��ӡ��־����ʾδ������������
//        co_printf("Motor not started, current time: %02d:%02d:%02d\r\n", 
//                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);
       }
	
     }else{
	 #ifdef SW_DEBUG
     co_printf("not run tim......\r\n");
     #endif		 
	   
   }
}



//���������ģʽ����
uint32_t square_wave_large_dose_mode_start(square_wave_large_dose  * p_s_w_large_dose)
{
	//��ȡ����
	float get_liquid  =  p_s_w_large_dose->large_dose_liquid;
	#ifdef SW_DEBUG
	printf("get_liquid is %f\r\n",get_liquid);
	#endif
	
    //��ȡ����ʱ��
	uint16_t tim_hh = p_s_w_large_dose->tim_hh;      //�����ʱ�� hh
	uint16_t tim_min = p_s_w_large_dose->tim_min;    //�����ʱ�� min
	
	//��������ʱ�� s
	float all_time = tim_hh + tim_min/60.0;
		
	//�ٶ� = Һ��/ʱ��
	float square_wave_speed = get_liquid / all_time;
	
	#ifdef SW_DEBUG
	printf("square_wave_speed is %fu/h \r\n",square_wave_speed);
	#endif
	
	//���㻽��ʱ����
	uint32_t weak_up_tim_interval_s = (uint32_t)(3600 / (square_wave_speed / 0.1087));
   	
	//��������ʱ����
	return weak_up_tim_interval_s;	
}




void sw_motor_start_process(void)
{
	//��������ʱ����
	uint32_t weak_up_tim_interval_s = square_wave_large_dose_mode_start(&s_w_large_dose);
	#ifdef SW_DEBUG
	co_printf("weak_up_tim_interval_s is %d\r\n",weak_up_tim_interval_s);
	#endif
	//�������������ʱ������
    sw_is_motor_start(&clock_env,weak_up_tim_interval_s,&s_w_large_dose);		
}


typedef struct dw_information{
	
	float now_liquid;
	uint32_t dw_weak_up_tim_interval_s;

}dw_information;



//˫�����������,����������ע��������ʱ����
dw_information double_wave_large_dose_mode_start(double_wave_large_dose * p_d_w_large_dose)
{	
	dw_information dw_info = {0};
	
	//������ע��
    float _all_liquid = p_d_w_large_dose->all_liquid;
	
	//������ע��
	float now_liquid = _all_liquid* p_d_w_large_dose->proportion/100;
  	dw_info.now_liquid = now_liquid;
	//������ 
	float sw_liquid = _all_liquid - now_liquid;
		
	//��������ʱ��
	uint16_t tim_hh = p_d_w_large_dose->tim_hh;      //�����ʱ�� hh
	uint16_t tim_min = p_d_w_large_dose->tim_min;    //�����ʱ�� min
	
	//��������ʱ�� h
	float all_time = tim_hh + tim_min/60.0;
	
 	//�ٶ� = Һ��/ʱ��
	float double_wave_speed = sw_liquid / all_time;
	
	#ifdef DW_DEBUG
	printf("double_wave_speed is %fu/h \r\n",double_wave_speed);
	#endif
	//���㻽��ʱ����
	uint32_t weak_up_tim_interval_s = (uint32_t)(3600 / (double_wave_speed / 0.1087));
	
	dw_info.dw_weak_up_tim_interval_s = weak_up_tim_interval_s;
   	
	//��������ʱ����
	return dw_info;	
}




 uint32_t dw_last_motor_start_time_s = 0;
//�����������
void dw_is_motor_start(clock_param_t *p_clock_env, uint32_t weak_up_tim_interval_s, double_wave_large_dose * p_d_w_large_dose)
{
    //���㵱ǰʱ���Ӧ������
    uint32_t  current_time_s = p_clock_env->hour * 3600 + 
                              p_clock_env->min * 60 + 
                              p_clock_env->sec;
    // ����������
    uint32_t day_seconds = 24 * 3600;
		
	uint32_t get_run_tim  =  p_d_w_large_dose->tim_hh*3600 + p_d_w_large_dose->tim_min*60;
	
    // ʹ�� static ��ֻ֤��ʼ��һ��
    static uint32_t get_start_tim = 0;
    static uint32_t get_end_tim = 0;

    // �ж��Ƿ�Ϊ��һ�γ�ʼ��
    if (get_start_tim == 0 && get_end_tim == 0) {
        get_start_tim = current_time_s;    
    }
	
	get_end_tim = get_start_tim+get_run_tim;
	
	#ifdef DW_DEBUG
	co_printf("get_start_tim = %d\r\n",get_start_tim);
	co_printf("get_end_tim = %d\r\n",get_end_tim);
	#endif
				
    if (current_time_s < dw_last_motor_start_time_s) {
        // ���죺���� last_motor_start_time_s
        dw_last_motor_start_time_s = 0;
    }
	 #ifdef DW_DEBUG
	 co_printf("now  tim_s:%d\r\n",current_time_s);
	 co_printf("last tim_s:%d\r\n",dw_last_motor_start_time_s);
	 #endif
	
	//��������ʱ��
	if(current_time_s>=get_start_tim&&current_time_s<=get_end_tim)
	{		
		    // ����Ƿ���Ҫ�������  ��׼����ʱ�� ����1s����Ϊ����ʱ��Ϊ3s��  
       if ((current_time_s - dw_last_motor_start_time_s) >= (weak_up_tim_interval_s)&&(current_time_s - dw_last_motor_start_time_s) <= (weak_up_tim_interval_s+2)) {
       
		// �������
        motor_low_powre_start(200); // 200 ��ʾ����ʱ�䣬��λ����ݵ����ƶ���

        // �����ϴ�����ʱ��
        dw_last_motor_start_time_s = current_time_s;
		   

		#ifdef DW_DEBUG
        // ��ӡ��־��Ϣ
        co_printf(".....dw.......Motor started at %02d:%02d:%02d............s\r\n", 
                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);		   
		#endif		   
    }
   else{
//        // ��ӡ��־����ʾδ������������
//        co_printf("Motor not started, current time: %02d:%02d:%02d\r\n", 
//                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);
       }
	
     }else{
		 
	 #ifdef DW_DEBUG
     co_printf("not run tim......\r\n");  
	 #endif
		 
   }
}

uint8_t dw_first_run_over;

void dw_now_start(void)
{	
	dw_information dw_info = {0};
	
	dw_info = double_wave_large_dose_mode_start(&d_w_large_dose);
	//��ȡ������ע����
	float get_liquid  =  dw_info.now_liquid;
	
	#ifdef DW_DEBUG
	printf("get_liquid is %f\r\n",get_liquid);
	#endif	
	/*
	  0.1087(һ������ 20ms)	
	*/	
	//���ݼ�����������ʱ��ms  (get_liquid/0.1087) һ��Ҫ���ж��ٲ�
	uint32_t run_ms = (get_liquid/0.1087) * 20; 
		
	//���е��
	motor_low_powre_start(run_ms*10);	
	
	dw_first_run_over = 1;
}


void dw_second_sw_start_process(void)
{
	dw_information dw_info = {0};
	
	dw_info = double_wave_large_dose_mode_start(&d_w_large_dose);
	
	if(dw_first_run_over)
	{
	   uint32_t weak_up_tim_interval_s = dw_info.dw_weak_up_tim_interval_s;

       co_printf("dw weak_up_tim_interval_s is %d\r\n",weak_up_tim_interval_s);
	
	   //�������������ʱ������
       dw_is_motor_start(&clock_env,weak_up_tim_interval_s,&d_w_large_dose);	
	}		
}  





















