#ifndef __LOG_H
#define __LOG_H

#include "stdint.h"

/* 
  ��־��Ϣ�������ȵ��رõ�ʱ�䲻׼ȷ��������֮��app����������ʱ��ͬ��ָ�У׼��ǰϵͳʱ�䣬
  ֮��appÿ����Сʱ������������ͬ��ʱ����Ϣ�Ա�֤ϵͳʱ���׼ȷ��ͬ�����֮��app�����Ͽ�����
  
  
  
  
*/

/* ��־��Ϣ������ʱ�䣬�Ͽ�����ʱ�䣬�����еĲ��� */


#define CONNECT_TIM_BUFFER_SIZE  1024   

typedef struct log_informaton{
	
	/***************************����ʱ��***********************************/
	
	//����ʱ��  
	uint16_t connect_tim_mon[CONNECT_TIM_BUFFER_SIZE];   
	uint16_t connect_tim_day[CONNECT_TIM_BUFFER_SIZE];
	uint16_t connect_tim_hh[CONNECT_TIM_BUFFER_SIZE];
	uint16_t connect_tim_min[CONNECT_TIM_BUFFER_SIZE];
	
	//����ʱ�� �洢����
	uint32_t log_connect_tim_number; 
	
	//����ʱ�� �洢�����Ƿ���
	uint8_t is_log_connect_tim_number_full;
	
	/********************************************************************/
	
	/***************************�Ͽ�����ʱ��*****************************/
	
	//�Ͽ�����ʱ��
	uint16_t dis_connect_tim_mon[CONNECT_TIM_BUFFER_SIZE];
	uint16_t dis_connect_tim_day[CONNECT_TIM_BUFFER_SIZE];
	uint16_t dis_connect_tim_hh[CONNECT_TIM_BUFFER_SIZE];
	uint16_t dis_connect_tim_min[CONNECT_TIM_BUFFER_SIZE];
	
	//�Ͽ�����ʱ�� �洢����
	uint32_t log_disconnect_tim_number; 
	
	//�Ͽ�����ʱ�� �洢�����Ƿ���
	uint8_t is_log_disconnect_tim_number_full;
	
	/********************************************************************/
	
	//ʱ��ͬ��
	
	
	
	//���û�����
	
	
    //���ô����-����ģʽ�����
	
	
	//���ô����-����ģʽ�����
	
		 
	//���ô����-˫��ģʽ�����
		
}log_informaton;













#endif  



