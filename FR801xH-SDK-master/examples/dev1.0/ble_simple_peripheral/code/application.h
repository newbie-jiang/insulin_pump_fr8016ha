#ifndef __APPLICATION
#define __APPLICATION

#include "stdint.h"

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


#endif  

