#ifndef __APPLICATION
#define __APPLICATION

#include "stdint.h"

//唤醒时间点
typedef struct weak_up_tim{
   uint16_t hh;
   uint16_t min;
   uint16_t s;
}weak_up_tim;

//存储唤醒时间点
extern weak_up_tim  wake_up_times[];


//指令合法性检查
void instruct_legal_check_process(void);


#endif  

