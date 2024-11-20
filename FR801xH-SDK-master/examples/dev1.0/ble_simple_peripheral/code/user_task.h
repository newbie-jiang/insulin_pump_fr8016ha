/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

/*
 * INCLUDES
 */
 
#ifndef _USER_TASK_H
#define _USER_TASK_H

#include "stdint.h"
#include "time_rtc.h"

enum user_event_t {
    USER_EVT_AT_COMMAND,
    USER_EVT_BUTTON,
};

enum user_audio_event_t {
    USER_EVT_MIC,			//Mic
    DECODER_EVENT_PREPARE,
    DECODER_EVENT_NEXT_FRAME,
    DECODER_EVENT_STOP,
    DECODER_EVENT_WAIT_END_TO,
};





void motor_task_fun(void *arg);
void beep_task_fun(void *arg);
void electric_quantity_task_fun(void *arg);
void key_scan_task_fun(void *arg);
void led_task_fun(void *arg);
void adc_task_fun(void *arg);
void rtc_tim_task_fun (void *arg);


enum infusion_state{
	stop = 0,
	run
};

typedef struct infusion_information{
	
	char * states;               //状态
	uint32_t remainder_drug;     //剩余药量
	uint32_t have_finished_drug; //已入药量
	uint32_t basal_rate;         //基础率

}infusion_information;


void send_message(enum infusion_state e_infusion_status);

void send_message_stop(void);
void send_message_run(void);


#endif  // _USER_TASK_H

