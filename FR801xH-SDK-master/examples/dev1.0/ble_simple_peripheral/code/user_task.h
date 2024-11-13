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

#endif  // _USER_TASK_H

