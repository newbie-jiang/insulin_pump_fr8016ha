#ifndef __CJSON_PROCESS_H
#define __CJSON_PROCESS_H




#include "stdint.h"
/***************************设定基础率***********************************/ 

/* 指令包序号 用于区分是哪一条指令 */
typedef struct pack_num{
	
  uint8_t cjson_instruct_num;

}pack_num;

/* 基础率参数 */
 typedef struct basal_rate_information{
	
	char  basal_rate_num; //基础率段数
	float basal_rate_speed;//基础率速度
	
	uint16_t basal_rate_start_tim_hh;  //起始时间hh
	uint16_t basal_rate_start_tim_min; //起始时间min
	
	uint16_t basal_rate_end_tim_hh;    //结束时间hh
	uint16_t basal_rate_end_tim_min;   //结束时间min
	
}basal_rate_information;
  

//同步时间参数
 typedef struct sync_tim{
	 
	 uint16_t sync_tim_year;
	 uint16_t sync_tim_month;
	 uint16_t sync_tim_day;
	 
	 uint16_t sync_tim_hh;
	 uint16_t sync_tim_min;
	 uint16_t sync_tim_s;
}sync_tim;
 

//大剂量设定参数
typedef struct large_dose{
  	float large_dose_liquid; //大剂量液量
	float large_dose_speed;  //大剂量速度
}large_dose_information;

//常规大剂量 (立马输注)
typedef struct normal_large_dose{	
	
   float large_dose_liquid; //大剂量液量

}normal_large_dose;


//方波大剂量 (匀速输注)
typedef struct square_wave_large_dose{
	
	 float large_dose_liquid; //大剂量液量
	 uint16_t tim_hh;    //大剂量时间 hh
	 uint16_t tim_min;   //大剂量时间 min

}square_wave_large_dose;


//双波大剂量 （按照百分比立马输注 + 匀速）
typedef struct double_wave_large_dose{
	
	 float all_liquid;  //总的大剂量
	
	 uint8_t proportion;           //输注比例
	
	 uint16_t tim_hh;     //第二段方波时间 hh
	 uint16_t tim_min;    //第二段方波时间 min
	
}double_wave_large_dose;


 extern  basal_rate_information     rate_info;        //基础率
 extern  sync_tim                   current_time;     //时间同步
 extern  large_dose_information     large_dose_info;
 extern  pack_num                   s_pack_num;
 extern  basal_rate_information     s_rate_info[48];  /* 存储基础率参数，最小半个小时一段 */

 extern  normal_large_dose          n_large_dose;     /* 常规大剂量 */
 extern  square_wave_large_dose     s_w_large_dose;   /* 方波大剂量 */
 extern  double_wave_large_dose     d_w_large_dose;   /* 双波大剂量 */


//基础率设定
void basal_rate_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on);
void ack_basal_rate_cjson_process(basal_rate_information *p_basal_rate_info, char *output_buffer, uint32_t buffer_size);

//获得基础率有多少段
uint32_t get_max_basal_rate_num(void);

//同步时间
void sync_tim_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on);
void ack_sync_tim_cjson_process(sync_tim * p_sync_tim,char *output_buffer, uint32_t buffer_size);

//大剂量设定
void large_dose_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on);
void ack_large_dose_cjson_process(large_dose_information * p_large_dose_info,char *output_buffer, uint32_t buffer_size);


//正常模式大剂量 数据解析
void normal_large_dose_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on);

//方波模式大剂量 数据解析
void square_wave_large_dose_process( uint8_t *data, uint32_t len, uint8_t dbg_on);

//双波模式大剂量 数据解析
void double_wave_large_dose_process( uint8_t *data, uint32_t len, uint8_t dbg_on);




#endif
