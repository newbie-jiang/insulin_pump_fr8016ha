#include "application.h"
//#include  "cjson_process.h"
#include  "co_printf.h"


//weak_up_tim  wake_up_times[3000] = {0};




//�������
#define  MAX_TIM_HH    23
#define  MIN_TIM_HH    0

#define  MAX_TIM_MIN   59
#define  MIN_TIM_MIN   0

#define  MAX_TIM_S     59
#define  MIN_TIM_S     0


//�����ʼ��㻽��ʱ����
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













