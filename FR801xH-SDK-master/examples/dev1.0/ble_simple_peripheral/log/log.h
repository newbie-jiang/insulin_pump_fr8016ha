#ifndef __LOG_H
#define __LOG_H

#include "stdint.h"

/* 
  日志信息，由于胰岛素泵的时间不准确，在连接之后app会主动发送时间同步指令，校准当前系统时间，
  之后app每隔半小时主动连接蓝牙同步时间信息以保证系统时间的准确，同步完成之后app主动断卡蓝牙
  
  
  
  
*/

/* 日志信息，连接时间，断开连接时间，连接中的操作 */


#define CONNECT_TIM_BUFFER_SIZE  1024   

typedef struct log_informaton{
	
	/***************************连接时间***********************************/
	
	//连接时间  
	uint16_t connect_tim_mon[CONNECT_TIM_BUFFER_SIZE];   
	uint16_t connect_tim_day[CONNECT_TIM_BUFFER_SIZE];
	uint16_t connect_tim_hh[CONNECT_TIM_BUFFER_SIZE];
	uint16_t connect_tim_min[CONNECT_TIM_BUFFER_SIZE];
	
	//连接时间 存储计数
	uint32_t log_connect_tim_number; 
	
	//连接时间 存储计数是否满
	uint8_t is_log_connect_tim_number_full;
	
	/********************************************************************/
	
	/***************************断开连接时间*****************************/
	
	//断开连接时间
	uint16_t dis_connect_tim_mon[CONNECT_TIM_BUFFER_SIZE];
	uint16_t dis_connect_tim_day[CONNECT_TIM_BUFFER_SIZE];
	uint16_t dis_connect_tim_hh[CONNECT_TIM_BUFFER_SIZE];
	uint16_t dis_connect_tim_min[CONNECT_TIM_BUFFER_SIZE];
	
	//断开连接时间 存储计数
	uint32_t log_disconnect_tim_number; 
	
	//断开连接时间 存储计数是否满
	uint8_t is_log_disconnect_tim_number_full;
	
	/********************************************************************/
	
	//时间同步
	
	
	
	//设置基础率
	
	
    //设置大剂量-正常模式大剂量
	
	
	//设置大剂量-方波模式大剂量
	
		 
	//设置大剂量-双波模式大剂量
		
}log_informaton;













#endif  



