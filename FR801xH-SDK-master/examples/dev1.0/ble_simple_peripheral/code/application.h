#ifndef __APPLICATION
#define __APPLICATION

#include "stdint.h"
#include "cjson_process.h"


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


#endif  

