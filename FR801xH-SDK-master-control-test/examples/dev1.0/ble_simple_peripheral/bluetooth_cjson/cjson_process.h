
#include "stdint.h"
/***************************�趨������***********************************/ 

typedef struct pack_num{
	
  uint8_t cjson_instruct_num;

}pack_num;

 
  typedef struct basal_rate_information{
	
	char  basal_rate_num; //�����ʶ���
	float basal_rate_speed;//�������ٶ� ����ʮ��
	
	char basal_rate_start_tim_hh;  //��ʼʱ��hh
	char basal_rate_start_tim_min; //��ʼʱ��min
	
	char basal_rate_end_tim_hh;    //����ʱ��hh
	char basal_rate_end_tim_min;   //����ʱ��min
	
}basal_rate_information;
  

//ͬ��ʱ��
 typedef struct sync_tim{
	 char sync_tim_hh;
	 char sync_tim_min;
	 char sync_tim_s;
}sync_tim;
 

//������趨
typedef struct large_dose{
  	float large_dose_liquid; //�����Һ��
	float large_dose_speed;  //������ٶ�
}large_dose_information;


 extern  basal_rate_information     rate_info;
 extern  sync_tim                   current_time;
 extern  large_dose_information     large_dose_info;
 extern  pack_num                   s_pack_num;

//�������趨
void basal_rate_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on);
void ack_basal_rate_cjson_process(basal_rate_information *p_basal_rate_info, char *output_buffer, uint32_t buffer_size);

//ͬ��ʱ��
void sync_tim_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on);
void ack_sync_tim_cjson_process(sync_tim * p_sync_tim,char *output_buffer, uint32_t buffer_size);

//������趨
void large_dose_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on);
void ack_large_dose_cjson_process(large_dose_information * p_large_dose_info,char *output_buffer, uint32_t buffer_size);


void ack_process(void);

