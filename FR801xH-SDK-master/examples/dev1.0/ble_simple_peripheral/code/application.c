#include "application.h"
#include  "co_printf.h"
#include  "time_rtc.h"


//参数检查
#define  MAX_TIM_HH    23
#define  MIN_TIM_HH    0

#define  MAX_TIM_MIN   59
#define  MIN_TIM_MIN   0

#define  MAX_TIM_S     59
#define  MIN_TIM_S     0


//基础率计算运行时间间隔
uint32_t basal_rate_calculate_wake_up_time(basal_rate_information * p_basal_rate_info)
{
	static uint16_t start_hh,start_min;
	static uint16_t end_hh,end_min;	
	static float basal_rate_speed;   //基础率速度
	char basal_rate_num; //基础率段数
	
	uint32_t weak_up_tim_interval_s; //电机运行时间间隔 
		
    start_hh  =  p_basal_rate_info->basal_rate_start_tim_hh;
 	start_min =  p_basal_rate_info->basal_rate_start_tim_min;
	end_hh    =  p_basal_rate_info->basal_rate_end_tim_hh;
    end_min   =  p_basal_rate_info->basal_rate_end_tim_min;	
	basal_rate_speed = p_basal_rate_info->basal_rate_speed;
	basal_rate_num   = p_basal_rate_info->basal_rate_num;
	
	/*
	  计算公式推导
	  基础率 = x   tim_s = 运行时间间隔    tim_s = 3600 /  (x / 0.1087)步    
	  0.1087(一步的量) -> 停止1s，运动20ms，运动一圈用时  2min | 运动一圈的步数为120/1.02 = 117步|一步的量 12.72(电机转动一圈的量) / 117 = 0.1087U
	*/
	
	//计算唤醒时间间隔
	weak_up_tim_interval_s = (uint32_t)(3600 / (basal_rate_speed / 0.1087));
	
	//计算唤醒时间(该数据应该在ram中保持，以便rtc定时唤醒)
	
	
	return weak_up_tim_interval_s;
}

/* 获得基础率时间间隔 */
uint32_t get_weak_up_tim_interval_s(void)
{
  co_printf("weak_up_tim_interval_s = %d s \r\n",basal_rate_calculate_wake_up_time(&rate_info));
  return basal_rate_calculate_wake_up_time(&rate_info);
}


// 计算唤醒时间并存储到结构体数组
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
    // 转换起始和结束时间为秒（以分钟为单位）
    uint32_t start_time_s = start_hh * 3600 + start_min * 60;
    uint32_t end_time_s = end_hh * 3600 + end_min * 60;
    uint32_t current_time_s = start_time_s;

    // 处理跨天的情况
    uint32_t day_seconds = 24 * 3600; // 一天的总秒数
    if (end_time_s < start_time_s) {
        end_time_s += day_seconds; // 跨天处理，将结束时间调整为第二天的秒数
    }

    // 按间隔计算唤醒时间
    while (current_time_s <= end_time_s && count < max_size) {
        // 转换当前时间为 hh:mm:ss
//        uint32_t seconds_of_day = current_time_s % day_seconds; // 当前时间的当天秒数
//        wake_up_times[count].hh = seconds_of_day / 3600;
//        wake_up_times[count].min = (seconds_of_day % 3600) / 60;
//        wake_up_times[count].s = seconds_of_day % 60;
        count++;
   
        // 增加时间
        current_time_s += weak_up_tim_interval_s;
    }
		
	/* 遍历唤醒时间 */
	#if 1
//	co_printf("printf weak up tim\r\n");
//	for(int i=0 ; i<count; i++)
//	{
//	  co_printf("wakeup_time[%d]---%02d:%02d:%02d\r\n",i,wake_up_times[i].hh,wake_up_times[i].min,wake_up_times[i].s);	
//	}
	#endif
	
	co_printf("calculate_wake_up_times success\r\n");
	
//    return count; // 返回生成的唤醒时间数量
	printf(" count is %d\r\n",count);
}



//基础率指令合法性检查  参数限制
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


//时间同步指令合法性检查  参数限制
int sync_tim_Instruction_legal_check(void)
{


   return 0;
}


//大剂量指令合法性检查 参数限制
int large_doseInstruction_legal_check(void)
{
	
	


  return 0;
}




// pack_num  s_pack_num = {0};

//指令处理  检查是哪一条指令
void instruct_legal_check_process(void)
{		
	switch(s_pack_num.cjson_instruct_num)
	{
		case 1:
			if(!basal_rate_Instruction_legal_check(&rate_info)) //基础率数据合法性检查通过
			{
			  calculate_wake_up_times(&rate_info,get_weak_up_tim_interval_s(),6100); //计算唤醒时间
			}			
			break;
			
		case 2:
			break;
		
		    sync_tim_Instruction_legal_check();    //时间同步数据合法性检查
				
		case 3:
			
		    break;
		
		    large_doseInstruction_legal_check();    //大剂量数据合法性检查
		
		default:
			break;
	}
}







//#define    NORMAL_DEBUG
//#define    SW_DEBUG
#define    DW_DEBUG

extern void motor_low_powre_start(uint32_t delay_ms);

//正常大剂量模式启动   设置后立马启动，蓝牙处于连接状态
void normal_large_dose_mode_start(normal_large_dose *p_n_large_dose)
{
	//获取剂量
	float get_liquid  =  p_n_large_dose->large_dose_liquid;
	
	#ifdef NORMAL_DEBUG
	printf("get_liquid is %f\r\n",get_liquid);
	#endif
		
	/*
	  0.1087(一步的量 20ms)
	*/
	
	//依据剂量计算运行时间ms  (get_liquid/0.1087) 一共要运行多少步
	uint32_t run_ms = (get_liquid/0.1087) * 20; 
		
	//运行电机
	motor_low_powre_start(run_ms*10);		
}



 uint32_t sw_last_motor_start_time_s = 0;

//方波电机运行
void sw_is_motor_start(clock_param_t *p_clock_env, uint32_t weak_up_tim_interval_s, square_wave_large_dose * p_s_w_large_dose)
{
    //计算当前时间对应的秒数
    uint32_t  current_time_s = p_clock_env->hour * 3600 + 
                              p_clock_env->min * 60 + 
                              p_clock_env->sec;
    // 处理跨天情况
    uint32_t day_seconds = 24 * 3600;
		
	uint32_t get_run_tim  =  p_s_w_large_dose->tim_hh*3600 + p_s_w_large_dose->tim_min*60;
	
    // 使用 static 保证只初始化一次
    static uint32_t get_start_tim = 0;
    static uint32_t get_end_tim = 0;

    // 判断是否为第一次初始化
    if (get_start_tim == 0 && get_end_tim == 0) {
        get_start_tim = current_time_s;    
    }
	
	get_end_tim = get_start_tim+get_run_tim;
	
	#ifdef SW_DEBUG
	co_printf("get_start_tim = %d\r\n",get_start_tim);
	co_printf("get_end_tim = %d\r\n",get_end_tim);
	#endif
				
    if (current_time_s < sw_last_motor_start_time_s) {
        // 跨天：重置 last_motor_start_time_s
        sw_last_motor_start_time_s = 0;
    }
	
	 #ifdef SW_DEBUG
	 co_printf("now  tim_s:%d\r\n",current_time_s);
	 co_printf("last tim_s:%d\r\n",sw_last_motor_start_time_s);
	 #endif
	
	//限制运行时间
	if(current_time_s>=get_start_tim&&current_time_s<=get_end_tim)
	{		
		    // 检查是否需要启动电机  标准启动时间 正负1s，因为唤醒时间为3s，  
       if ((current_time_s - sw_last_motor_start_time_s) >= (weak_up_tim_interval_s)&&(current_time_s - sw_last_motor_start_time_s) <= (weak_up_tim_interval_s+2)) {
        // 启动电机
        motor_low_powre_start(200); // 200 表示启动时间，单位需根据电机设计定义

        // 更新上次启动时间
        sw_last_motor_start_time_s = current_time_s;
         
		#ifdef SW_DEBUG
        // 打印日志信息
        co_printf(".....sw.......Motor started at %02d:%02d:%02d............s\r\n", 
                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);
		#endif
    }
   else{
//        // 打印日志，表示未满足启动条件
//        co_printf("Motor not started, current time: %02d:%02d:%02d\r\n", 
//                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);
       }
	
     }else{
	 #ifdef SW_DEBUG
     co_printf("not run tim......\r\n");
     #endif		 
	   
   }
}



//方波大剂量模式启动
uint32_t square_wave_large_dose_mode_start(square_wave_large_dose  * p_s_w_large_dose)
{
	//获取剂量
	float get_liquid  =  p_s_w_large_dose->large_dose_liquid;
	#ifdef SW_DEBUG
	printf("get_liquid is %f\r\n",get_liquid);
	#endif
	
    //获取运行时间
	uint16_t tim_hh = p_s_w_large_dose->tim_hh;      //大剂量时间 hh
	uint16_t tim_min = p_s_w_large_dose->tim_min;    //大剂量时间 min
	
	//计算运行时间 s
	float all_time = tim_hh + tim_min/60.0;
		
	//速度 = 液量/时间
	float square_wave_speed = get_liquid / all_time;
	
	#ifdef SW_DEBUG
	printf("square_wave_speed is %fu/h \r\n",square_wave_speed);
	#endif
	
	//计算唤醒时间间隔
	uint32_t weak_up_tim_interval_s = (uint32_t)(3600 / (square_wave_speed / 0.1087));
   	
	//返回运行时间间隔
	return weak_up_tim_interval_s;	
}




void sw_motor_start_process(void)
{
	//计算运行时间间隔
	uint32_t weak_up_tim_interval_s = square_wave_large_dose_mode_start(&s_w_large_dose);
	#ifdef SW_DEBUG
	co_printf("weak_up_tim_interval_s is %d\r\n",weak_up_tim_interval_s);
	#endif
	//方波大剂量依据时间启动
    sw_is_motor_start(&clock_env,weak_up_tim_interval_s,&s_w_large_dose);		
}


typedef struct dw_information{
	
	float now_liquid;
	uint32_t dw_weak_up_tim_interval_s;

}dw_information;



//双波大剂量运行,返回立马输注量和运行时间间隔
dw_information double_wave_large_dose_mode_start(double_wave_large_dose * p_d_w_large_dose)
{	
	dw_information dw_info = {0};
	
	//所有输注量
    float _all_liquid = p_d_w_large_dose->all_liquid;
	
	//立马输注量
	float now_liquid = _all_liquid* p_d_w_large_dose->proportion/100;
  	dw_info.now_liquid = now_liquid;
	//方波量 
	float sw_liquid = _all_liquid - now_liquid;
		
	//方波运行时间
	uint16_t tim_hh = p_d_w_large_dose->tim_hh;      //大剂量时间 hh
	uint16_t tim_min = p_d_w_large_dose->tim_min;    //大剂量时间 min
	
	//计算运行时间 h
	float all_time = tim_hh + tim_min/60.0;
	
 	//速度 = 液量/时间
	float double_wave_speed = sw_liquid / all_time;
	
	#ifdef DW_DEBUG
	printf("double_wave_speed is %fu/h \r\n",double_wave_speed);
	#endif
	//计算唤醒时间间隔
	uint32_t weak_up_tim_interval_s = (uint32_t)(3600 / (double_wave_speed / 0.1087));
	
	dw_info.dw_weak_up_tim_interval_s = weak_up_tim_interval_s;
   	
	//返回运行时间间隔
	return dw_info;	
}




 uint32_t dw_last_motor_start_time_s = 0;
//方波电机运行
void dw_is_motor_start(clock_param_t *p_clock_env, uint32_t weak_up_tim_interval_s, double_wave_large_dose * p_d_w_large_dose)
{
    //计算当前时间对应的秒数
    uint32_t  current_time_s = p_clock_env->hour * 3600 + 
                              p_clock_env->min * 60 + 
                              p_clock_env->sec;
    // 处理跨天情况
    uint32_t day_seconds = 24 * 3600;
		
	uint32_t get_run_tim  =  p_d_w_large_dose->tim_hh*3600 + p_d_w_large_dose->tim_min*60;
	
    // 使用 static 保证只初始化一次
    static uint32_t get_start_tim = 0;
    static uint32_t get_end_tim = 0;

    // 判断是否为第一次初始化
    if (get_start_tim == 0 && get_end_tim == 0) {
        get_start_tim = current_time_s;    
    }
	
	get_end_tim = get_start_tim+get_run_tim;
	
	#ifdef DW_DEBUG
	co_printf("get_start_tim = %d\r\n",get_start_tim);
	co_printf("get_end_tim = %d\r\n",get_end_tim);
	#endif
				
    if (current_time_s < dw_last_motor_start_time_s) {
        // 跨天：重置 last_motor_start_time_s
        dw_last_motor_start_time_s = 0;
    }
	 #ifdef DW_DEBUG
	 co_printf("now  tim_s:%d\r\n",current_time_s);
	 co_printf("last tim_s:%d\r\n",dw_last_motor_start_time_s);
	 #endif
	
	//限制运行时间
	if(current_time_s>=get_start_tim&&current_time_s<=get_end_tim)
	{		
		    // 检查是否需要启动电机  标准启动时间 正负1s，因为唤醒时间为3s，  
       if ((current_time_s - dw_last_motor_start_time_s) >= (weak_up_tim_interval_s)&&(current_time_s - dw_last_motor_start_time_s) <= (weak_up_tim_interval_s+2)) {
       
		// 启动电机
        motor_low_powre_start(200); // 200 表示启动时间，单位需根据电机设计定义

        // 更新上次启动时间
        dw_last_motor_start_time_s = current_time_s;
		   

		#ifdef DW_DEBUG
        // 打印日志信息
        co_printf(".....dw.......Motor started at %02d:%02d:%02d............s\r\n", 
                  p_clock_env->hour, p_clock_env->min, p_clock_env->sec);		   
		#endif		   
    }
   else{
//        // 打印日志，表示未满足启动条件
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
	//获取立马输注剂量
	float get_liquid  =  dw_info.now_liquid;
	
	#ifdef DW_DEBUG
	printf("get_liquid is %f\r\n",get_liquid);
	#endif	
	/*
	  0.1087(一步的量 20ms)	
	*/	
	//依据剂量计算运行时间ms  (get_liquid/0.1087) 一共要运行多少步
	uint32_t run_ms = (get_liquid/0.1087) * 20; 
		
	//运行电机
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
	
	   //方波大剂量依据时间启动
       dw_is_motor_start(&clock_env,weak_up_tim_interval_s,&d_w_large_dose);	
	}		
}  





















