#include "log.h"
#include "driver_flash.h"
#include "co_printf.h"
#include <string.h>

#define log_info_save_base_address    (300-4)*1024


#define log_write_base_adderss  300*1024

#define    FLASH_PAGE_SIZE    (0x1000)    // 4k  �˴����ɱ�
#define    LOG_FLASH_SIZE      200*1024

uint8_t   _log_write_base_adderss[8] = {0x00,0X04,0Xb0,0X00,0x00,0x00,0x00,0x00};  //300k
uint8_t   log_address_buffer[4];
volatile  uint32_t log_address_index;  //��־������ַ
uint8_t   overflow_count_buff[4];  //�������
uint32_t  overflow_count;

/* char������תint */
int char_turn_int(uint8_t* bytes) {
    return ((int)bytes[0] << 24) | 
           ((int)bytes[1] << 16) | 
           ((int)bytes[2] << 8)  | 
           bytes[3];
}


/* int������תchar */
void int_turn_char_param(int num, uint8_t* outBytes) {
    outBytes[0] = (num >> 24) & 0xFF;
    outBytes[1] = (num >> 16) & 0xFF;
    outBytes[2] = (num >> 8) & 0xFF;
    outBytes[3] = num & 0xFF;
}


//�����־��ַ
void get_log_address(uint8_t is_first)
{
	if(is_first) //log��һ��д�� ��ʼ��
	{
	    //����
       flash_erase(log_info_save_base_address, FLASH_PAGE_SIZE);	  
	    //д��
	   flash_write(log_info_save_base_address, sizeof(_log_write_base_adderss), _log_write_base_adderss);
	  	
	    //��ȡ��ַ����
	   flash_read(log_info_save_base_address,sizeof(log_address_buffer),log_address_buffer);
	  
	   log_address_index = char_turn_int(log_address_buffer);
			
	   co_printf("\r\n first write log base address success: 0x%08x \r\n",log_address_index);
		 	   			
       //��ȡ��־�������		
	   flash_read(log_info_save_base_address+4,sizeof(overflow_count_buff),overflow_count_buff);
		
	   overflow_count = char_turn_int(overflow_count_buff);
	  
	   co_printf("\r\n first write overflow count: %08d \r\n",overflow_count);
	   
	}else{ //�ǵ�һ��,��ȡҪд�ĵ�ַ
	
	    //��ȡ��Ҫд��log �� ��ַ
	    flash_read(log_info_save_base_address,sizeof(log_address_buffer),log_address_buffer);
		
	    co_printf("\r\n read log_address_index 0x%08x\r\n",log_address_index);
		
		//��ȡ�������		
		flash_read(log_info_save_base_address+4,sizeof(overflow_count_buff),overflow_count_buff);
		
		overflow_count = char_turn_int(overflow_count_buff); //char תint 
		
		co_printf("\r\n  read overflow count: %08d \r\n",overflow_count);
	}
}

/* ��־���� */
typedef enum log_state{
	
	SYNC_TIM = 1,
	BASAL_RATE = 2,
	NORMAL_LARGE_DOSE = 3,
	SQUARE_WAVE_LARGE_DOSE = 4,
	DOUBLE_WAVE_LARGE_DOSE = 5,
	CONNECT_TIM = 6,
	DIS_CONNECT_TIM = 7

}log_state;

// ����page size��4096�ֽ�
#define PAGE_SIZE 4096
#define PAGE_MASK (PAGE_SIZE - 1)  // 4096 - 1 = 0xFFF


// ���������ݸ����ĵ�ַ����ҳ����׵�ַ
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

uint8_t  log_page_buffer[4096]; //4kһ�� page
#define LOG_SIZE 1024

//д��־
void wright_log(log_state e_log_state)
{
	   uint8_t *logbuffer;
       uint8_t save_address[4];
	
	  if (log_address_index % 4096 != 0) {  // �������׸�page ��Ҫȡ��ԭ�������ݱ���
		#ifdef LOG_APP_DEBUG
		co_printf("\r\n  not page base address \r\n");
		#endif
		  
        uint32_t page_base_address = get_page_start_address(log_address_index); //��ȡҳ���׵�ַ
        uint32_t log_number_address = log_address_index % 4096; 
		  
//      uint32_t log_number = log_number_address / LOG_SIZE; //��ȡ��page�ĵڼ���log

		if(e_log_state == SYNC_TIM){  logbuffer = log__buff(SYNC_TIM); }
		
		else if(e_log_state == BASAL_RATE){  logbuffer = log__buff(BASAL_RATE);}
			
		else if(e_log_state == NORMAL_LARGE_DOSE){  logbuffer = log__buff(NORMAL_LARGE_DOSE);}
				
		else if(e_log_state == SQUARE_WAVE_LARGE_DOSE){  logbuffer = log__buff(SQUARE_WAVE_LARGE_DOSE);}
			 
		else if(e_log_state == DOUBLE_WAVE_LARGE_DOSE){  logbuffer = log__buff(DOUBLE_WAVE_LARGE_DOSE);}
				
		else if(e_log_state == CONNECT_TIM){  logbuffer = log__buff(CONNECT_TIM);}
		 
		else if(e_log_state == DIS_CONNECT_TIM){  logbuffer = log__buff(DIS_CONNECT_TIM);}
			 	
		//����һ��page���ݵ�log_buffer����
		flash_read(page_base_address,sizeof(log_page_buffer),log_page_buffer);
			
		//����������ݿ���������
        memcpy((uint8_t *)log_page_buffer+log_number_address, logbuffer, LOG_SIZE);
		
		//��page����
        flash_erase(page_base_address, FLASH_PAGE_SIZE);
		
		//�������е�����д��
        flash_write(page_base_address, sizeof(log_page_buffer), log_page_buffer);	
		
        //��ջ���
        memset(log_page_buffer,0,sizeof(log_page_buffer));		
		
    } else { 
		
		
	}
	
	


}







