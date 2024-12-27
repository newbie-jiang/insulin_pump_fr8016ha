#include "log.h"
#include "driver_flash.h"
#include "co_printf.h"
#include <string.h>

#define log_info_save_base_address    (300-4)*1024


#define log_write_base_adderss  300*1024

#define    FLASH_PAGE_SIZE    (0x1000)    // 4k  此处不可变
#define    LOG_FLASH_SIZE      200*1024

uint8_t   _log_write_base_adderss[8] = {0x00,0X04,0Xb0,0X00,0x00,0x00,0x00,0x00};  //300k
uint8_t   log_address_buffer[4];
volatile  uint32_t log_address_index;  //日志索引地址
uint8_t   overflow_count_buff[4];  //溢出计数
uint32_t  overflow_count;

/* char型数据转int */
int char_turn_int(uint8_t* bytes) {
    return ((int)bytes[0] << 24) | 
           ((int)bytes[1] << 16) | 
           ((int)bytes[2] << 8)  | 
           bytes[3];
}


/* int型数据转char */
void int_turn_char_param(int num, uint8_t* outBytes) {
    outBytes[0] = (num >> 24) & 0xFF;
    outBytes[1] = (num >> 16) & 0xFF;
    outBytes[2] = (num >> 8) & 0xFF;
    outBytes[3] = num & 0xFF;
}


//获得日志地址
void get_log_address(uint8_t is_first)
{
	if(is_first) //log第一次写入 初始化
	{
	    //擦除
       flash_erase(log_info_save_base_address, FLASH_PAGE_SIZE);	  
	    //写入
	   flash_write(log_info_save_base_address, sizeof(_log_write_base_adderss), _log_write_base_adderss);
	  	
	    //读取地址索引
	   flash_read(log_info_save_base_address,sizeof(log_address_buffer),log_address_buffer);
	  
	   log_address_index = char_turn_int(log_address_buffer);
			
	   co_printf("\r\n first write log base address success: 0x%08x \r\n",log_address_index);
		 	   			
       //读取日志溢出计数		
	   flash_read(log_info_save_base_address+4,sizeof(overflow_count_buff),overflow_count_buff);
		
	   overflow_count = char_turn_int(overflow_count_buff);
	  
	   co_printf("\r\n first write overflow count: %08d \r\n",overflow_count);
	   
	}else{ //非第一次,获取要写的地址
	
	    //获取需要写入log 的 地址
	    flash_read(log_info_save_base_address,sizeof(log_address_buffer),log_address_buffer);
		
	    co_printf("\r\n read log_address_index 0x%08x\r\n",log_address_index);
		
		//获取溢出次数		
		flash_read(log_info_save_base_address+4,sizeof(overflow_count_buff),overflow_count_buff);
		
		overflow_count = char_turn_int(overflow_count_buff); //char 转int 
		
		co_printf("\r\n  read overflow count: %08d \r\n",overflow_count);
	}
}

/* 日志定义 */
typedef enum log_state{
	
	SYNC_TIM = 1,
	BASAL_RATE = 2,
	NORMAL_LARGE_DOSE = 3,
	SQUARE_WAVE_LARGE_DOSE = 4,
	DOUBLE_WAVE_LARGE_DOSE = 5,
	CONNECT_TIM = 6,
	DIS_CONNECT_TIM = 7

}log_state;

// 假设page size是4096字节
#define PAGE_SIZE 4096
#define PAGE_MASK (PAGE_SIZE - 1)  // 4096 - 1 = 0xFFF


// 函数：根据给定的地址计算页面的首地址
uint32_t get_page_start_address(uint32_t address) {
    return address & ~PAGE_MASK;
}

 uint8_t send_buffer[1024];

 uint8_t * log__buff(enum log_state e_log_state)
{
	
	if(e_log_state == SYNC_TIM)
	{
	
	  	
	 
	
	}else if(e_log_state == BASAL_RATE)
	{
		
	
	
	}else if(e_log_state == NORMAL_LARGE_DOSE)
	{
		
	
	
	}else if(e_log_state == SQUARE_WAVE_LARGE_DOSE)
	{
		
	
	}else if(e_log_state == DOUBLE_WAVE_LARGE_DOSE)
	{
	
	
	}else if(e_log_state == CONNECT_TIM)
	{
		
	
	}else if(e_log_state == DIS_CONNECT_TIM)
	{
	
	
	}
		
}

uint8_t  log_page_buffer[4096]; //4k一个 page
#define LOG_SIZE 1024

//写日志
void wright_log(log_state e_log_state)
{
	   uint8_t *logbuffer;
       uint8_t save_address[4];
	
	  if (log_address_index % 4096 != 0) {  // 不满足首个page 需要取出原来的数据保存
		#ifdef LOG_APP_DEBUG
		co_printf("\r\n  not page base address \r\n");
		#endif
		  
        uint32_t page_base_address = get_page_start_address(log_address_index); //获取页面首地址
        uint32_t log_number_address = log_address_index % 4096; 
		  
//      uint32_t log_number = log_number_address / LOG_SIZE; //获取是page的第几个log

		if(e_log_state == SYNC_TIM){  logbuffer = log__buff(SYNC_TIM); }
		
		else if(e_log_state == BASAL_RATE){  logbuffer = log__buff(BASAL_RATE);}
			
		else if(e_log_state == NORMAL_LARGE_DOSE){  logbuffer = log__buff(NORMAL_LARGE_DOSE);}
				
		else if(e_log_state == SQUARE_WAVE_LARGE_DOSE){  logbuffer = log__buff(SQUARE_WAVE_LARGE_DOSE);}
			 
		else if(e_log_state == DOUBLE_WAVE_LARGE_DOSE){  logbuffer = log__buff(DOUBLE_WAVE_LARGE_DOSE);}
				
		else if(e_log_state == CONNECT_TIM){  logbuffer = log__buff(CONNECT_TIM);}
		 
		else if(e_log_state == DIS_CONNECT_TIM){  logbuffer = log__buff(DIS_CONNECT_TIM);}
			 	
		//读出一个page数据到log_buffer保存
		flash_read(page_base_address,sizeof(log_page_buffer),log_page_buffer);
			
		//将新添加数据拷贝至缓存
        memcpy((uint8_t *)log_page_buffer+log_number_address, logbuffer, LOG_SIZE);
		
		//将page擦除
        flash_erase(page_base_address, FLASH_PAGE_SIZE);
		
		//将缓存中的数据写入
        flash_write(page_base_address, sizeof(log_page_buffer), log_page_buffer);	
		
        //清空缓存
        memset(log_page_buffer,0,sizeof(log_page_buffer));		
		
    } else { 
		
		
	}
	
	


}







