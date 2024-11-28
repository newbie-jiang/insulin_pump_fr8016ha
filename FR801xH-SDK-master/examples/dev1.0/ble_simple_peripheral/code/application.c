#include "application.h"
//#include  "cjson_process.h"
#include  "co_printf.h"


//weak_up_tim  wake_up_times[3000] = {0};




//参数检查
#define  MAX_TIM_HH    23
#define  MIN_TIM_HH    0

#define  MAX_TIM_MIN   59
#define  MIN_TIM_MIN   0

#define  MAX_TIM_S     59
#define  MIN_TIM_S     0


//基础率计算唤醒时间间隔
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













