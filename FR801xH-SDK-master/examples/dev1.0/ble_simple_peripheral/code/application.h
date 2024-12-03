#ifndef __APPLICATION
#define __APPLICATION

#include "stdint.h"
#include "cjson_process.h"
#include "time_rtc.h"



//����ʱ���
typedef struct weak_up_tim{
   uint16_t hh;
   uint16_t min;
   uint16_t s;
}weak_up_tim;

//�洢����ʱ���
extern weak_up_tim  wake_up_times[];

//ָ��Ϸ��Լ��
void instruct_legal_check_process(void);

/* ��û�����ʱ���� */
uint32_t get_weak_up_tim_interval_s(void);

/* �������л���ʱ�䲢��ӡ */
void calculate_wake_up_times(  basal_rate_information * p_basal_rate_info,
                          	   uint32_t weak_up_tim_interval_s ,
							   int max_size 
						    ) ;
							   
							   
void normal_large_dose_mode_start(normal_large_dose *p_n_large_dose);
							   
void sw_is_motor_start(clock_param_t *p_clock_env, uint32_t weak_up_tim_interval_s, square_wave_large_dose * p_s_w_large_dose);

void sw_motor_start_process(void);

#endif  

