#ifndef TIMER_RTC_H
#define TIMER_RTC_H

#include "stdint.h"
#include "cjson_process.h"

typedef struct clock_param
{
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t week;
    uint16_t hour;
    uint16_t min;
    uint16_t sec;
} clock_param_t;


extern clock_param_t clock_env;

void demo_rtc_start_timer(void);
void get_current_date(uint16_t *year, uint16_t *month, uint16_t *day);
void clock_hdl(void);
void update_tim(sync_tim * p_sync_tim);

#endif

