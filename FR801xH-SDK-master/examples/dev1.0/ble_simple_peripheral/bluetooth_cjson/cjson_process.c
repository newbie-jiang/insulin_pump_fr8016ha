#include  "cjson_process.h"
#include  "Cjson.h"
#include  "stdint.h"
#include  "co_printf.h"
#include  <stdlib.h>
#include  <stdio.h>

  basal_rate_information     rate_info        = {0};
  sync_tim                   current_time     = {0};
  large_dose_information     large_dose_info  = {0};
  pack_num                   s_pack_num       = {0};

  /* ����������ָ��  
   {
     "basal_rate":   
    {
     "basal_rate_num": "2",   //�����ʶ������ڼ���
     "speed": "8.5",          //�������ٶ�
     "basal_rate_tim": "01:30-20:30" //������ ��ʼʱ�� - ����ʱ��
    }
   }  

   {
     "basal_rate":   
    {
     "basal_rate_num": "2",
     "speed": "8.5",          
     "basal_rate_tim": "01:30-20:30" 
    }
   }      
   */
  
void basal_rate_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on) {
	
    if (len == 0 || dbg_on == 0) return;

    // �����յ�����תΪ�ַ���
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // ȷ���ַ����� null ��β

    // ��ӡ���յ� JSON ����
    co_printf("Received JSON: %s\n", buffer);

    // ���� JSON ����
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // ���� basal_rate_information �ṹ��ʵ��
  

    // ��ȡ "basal_rate" ����
    cJSON *basal_rate = cJSON_GetObjectItemCaseSensitive(root, "basal_rate");
    if (basal_rate == NULL) {
        co_printf("basal_rate object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
		
	 s_pack_num.cjson_instruct_num = 1;
	
	}

    // ��ȡ "basal_rate_num"
    cJSON *basal_rate_num = cJSON_GetObjectItemCaseSensitive(basal_rate, "basal_rate_num");
    if (cJSON_IsString(basal_rate_num) && (basal_rate_num->valuestring != NULL)) {
        rate_info.basal_rate_num = atoi(basal_rate_num->valuestring);
    }

    // ��ȡ "speed"
    cJSON *speed = cJSON_GetObjectItemCaseSensitive(basal_rate, "speed");
    if (cJSON_IsString(speed) && (speed->valuestring != NULL)) {
        rate_info.basal_rate_speed = atof(speed->valuestring);
    }

    // ��ȡ "basal_rate_tim"
    cJSON *basal_rate_tim = cJSON_GetObjectItemCaseSensitive(basal_rate, "basal_rate_tim");
    if (cJSON_IsString(basal_rate_tim) && (basal_rate_tim->valuestring != NULL)) {
        // �ָ�ʱ���
        char start_time[6] = {0};
        char end_time[6] = {0};
        sscanf(basal_rate_tim->valuestring, "%5[^-]-%5s", start_time, end_time);

        // �ֽ���ʼʱ��
        sscanf(start_time, "%2hhd:%2hhd", &rate_info.basal_rate_start_tim_hh, &rate_info.basal_rate_start_tim_min);

        // �ֽ����ʱ��
        sscanf(end_time, "%2hhd:%2hhd", &rate_info.basal_rate_end_tim_hh, &rate_info.basal_rate_end_tim_min);
    }

    // ��ӡ�������
    co_printf("Parsed basal_rate_information:\n");
    co_printf("  basal_rate_num: %d\n", rate_info.basal_rate_num);

    // ���� co_printf �޷���ӡ����������������ʮ����ӡ
//    co_printf("  (float)basal_rate_speed * 10 : %d\n", (uint32_t)(rate_info.basal_rate_speed * 10));
	printf("basal_rate_speed : %f\n",rate_info.basal_rate_speed);
    co_printf("  Start time: %02d:%02d\n", rate_info.basal_rate_start_tim_hh, rate_info.basal_rate_start_tim_min);
    co_printf("  End time: %02d:%02d\n", rate_info.basal_rate_end_tim_hh, rate_info.basal_rate_end_tim_min);

    // ���� JSON �ڴ�
    cJSON_Delete(root);
}
   
  
  /************************************ͬ��ʱ��***********************************/  
/*
{
  "sync_tim": {      
    "now_tim": "12:20:30"
  }
}   
*/

void sync_tim_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on) {
	
    if (len == 0 || data == NULL) return;

    // �����յ�����תΪ�ַ���
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // ȷ���ַ����� null ��β

    // ��ӡ���յ��� JSON ����
    co_printf("Received JSON: %s\n", buffer);

    // ���� JSON ����
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // ���� sync_tim �ṹ��ʵ��
    

    // ��ȡ "sync_tim" ����
    cJSON *sync_tim_obj = cJSON_GetObjectItemCaseSensitive(root, "sync_tim");
    if (sync_tim_obj == NULL) {
        co_printf("sync_tim object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
	
	 s_pack_num.cjson_instruct_num = 2;
		
	}

    // ��ȡ "now_tim"
    cJSON *now_tim = cJSON_GetObjectItemCaseSensitive(sync_tim_obj, "now_tim");
    if (cJSON_IsString(now_tim) && (now_tim->valuestring != NULL)) {
        // �ֽ�ʱ����Ϣ
        sscanf(now_tim->valuestring, "%2hhd:%2hhd:%2hhd", 
               &current_time.sync_tim_hh, 
               &current_time.sync_tim_min, 
               &current_time.sync_tim_s);
    } else {
        co_printf("now_tim not found or invalid format!\n");
        cJSON_Delete(root);
        return;
    }

    // ��ӡ�������
    co_printf("Parsed sync_tim:\n");
    co_printf("  Hours: %02d\n", current_time.sync_tim_hh);
    co_printf("  Minutes: %02d\n", current_time.sync_tim_min);
    co_printf("  Seconds: %02d\n", current_time.sync_tim_s);

    // ���� JSON �ڴ�
    cJSON_Delete(root);
}


//������趨
//typedef struct large_dose{
//  	char large_dose_liquid; //�����Һ��
//	char large_dose_speed;  //������ٶ�
//}large_dose;
/*
 
  {
   "large_dose": 
   {      
     "large_dose_liquid": "xxx.x",
     "large_dose_speed": "xx.x"
   }
  }

  {
   "large_dose": 
   {      
     "large_dose_liquid": "002.5",
     "large_dose_speed": "02.5"
   }
  }

*/

/* ��������� */
void large_dose_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on)
{
	 if (len == 0 || dbg_on == 0) return;

    // �����յ�����תΪ�ַ���
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // ȷ���ַ����� null ��β

    // ��ӡ���յ� JSON ����
    co_printf("Received JSON: %s\n", buffer);

    // ���� JSON ����
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // ��ȡ "large_dose" ����
    cJSON *large_dose = cJSON_GetObjectItemCaseSensitive(root, "large_dose");
    if (large_dose == NULL) {
        co_printf("large_dose object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
	
	  s_pack_num.cjson_instruct_num = 3;
	}

    // ��ȡ "large_dose_liquid"
    cJSON *large_dose_liquid = cJSON_GetObjectItemCaseSensitive(large_dose, "large_dose_liquid");
    if (cJSON_IsString(large_dose_liquid) && (large_dose_liquid->valuestring != NULL)) {
        large_dose_info.large_dose_liquid = atof(large_dose_liquid->valuestring);
    }

    // ��ȡ "large_dose_speed"
    cJSON *large_dose_speed = cJSON_GetObjectItemCaseSensitive(large_dose, "large_dose_speed");
    if (cJSON_IsString(large_dose_speed) && (large_dose_speed->valuestring != NULL)) {
        large_dose_info.large_dose_speed = atof(large_dose_speed->valuestring);
    }

    // ��ӡ�������
    co_printf("Parsed large_dose_information:\n");
	
	printf("  large_dose_liquid scaled=: %f\r\n", large_dose_info.large_dose_liquid);
    printf("  large_dose_speed scaled: %f\r\n", large_dose_info.large_dose_speed);

    // ���� JSON �ڴ�
    cJSON_Delete(root);
}


/*********************************ACK*************************************/

//������ACK
void ack_basal_rate_cjson_process(basal_rate_information *p_basal_rate_info, char *output_buffer, uint32_t buffer_size)
{	
    if (p_basal_rate_info == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // ���� JSON ������
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // ���� "basal_rate" ����
    cJSON *basal_rate = cJSON_CreateObject();
    if (basal_rate == NULL) {
        co_printf("Failed to create basal_rate object\n");
        cJSON_Delete(root);
        return;
    }

    // ��� basal_rate_num
    cJSON_AddStringToObject(basal_rate, "basal_rate_num", 
                            (char[3]){p_basal_rate_info->basal_rate_num + '0', '\0'});
    
    // ��� speed
    char speed_str[16];
    snprintf(speed_str, sizeof(speed_str), "%.1f", p_basal_rate_info->basal_rate_speed); // ���������ٶ�
    cJSON_AddStringToObject(basal_rate, "speed", speed_str);

    // ��� basal_rate_tim
    char time_range[32];
    snprintf(time_range, sizeof(time_range), 
             "%02d:%02d-%02d:%02d", 
             p_basal_rate_info->basal_rate_start_tim_hh, 
             p_basal_rate_info->basal_rate_start_tim_min, 
             p_basal_rate_info->basal_rate_end_tim_hh, 
             p_basal_rate_info->basal_rate_end_tim_min);
    cJSON_AddStringToObject(basal_rate, "basal_rate_tim", time_range);

    // �� "basal_rate" ��ӵ�������
    cJSON_AddItemToObject(root, "basal_rate", basal_rate);

    // �� JSON ����ת��Ϊ�ַ���
    char *json_data = cJSON_PrintUnformatted(root);
    if (json_data == NULL) {
        co_printf("Failed to print JSON data\n");
        cJSON_Delete(root);
        return;
    }

    // ��黺������С��ȷ�����������
    if (strlen(json_data) < buffer_size) {
        strncpy(output_buffer, json_data, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // ȷ���ַ����� NULL ��β
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }

    // ���� JSON �ַ����Ͷ����ڴ�
    cJSON_free(json_data);
    cJSON_Delete(root);
}



void ack_sync_tim_cjson_process(sync_tim *p_sync_tim, char *output_buffer, uint32_t buffer_size) {
    if (p_sync_tim == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // ���� JSON ������
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // ���� "sync_tim" ����
    cJSON *sync_tim_obj = cJSON_CreateObject();
    if (sync_tim_obj == NULL) {
        co_printf("Failed to create sync_tim object\n");
        cJSON_Delete(root);
        return;
    }

    // ���ṹ������ת��Ϊ�ַ�����ʽ " hh:mm:ss "
    char now_tim[10] = {0};
    snprintf(now_tim, sizeof(now_tim), "%02d:%02d:%02d",
             p_sync_tim->sync_tim_hh,
             p_sync_tim->sync_tim_min,
             p_sync_tim->sync_tim_s);

    // ����ֶε� JSON ����
    cJSON_AddStringToObject(sync_tim_obj, "now_tim", now_tim);
    cJSON_AddItemToObject(root, "sync_tim", sync_tim_obj);

    // ת��Ϊ JSON �ַ���
    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        co_printf("Failed to create JSON string\n");
        cJSON_Delete(root);
        return;
    }

    // ��黺������С��ȷ���������
    if (strlen(json_str) < buffer_size) {
        strncpy(output_buffer, json_str, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // ȷ���ַ����� NULL ��β
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }

    // ���� JSON �ַ����Ͷ����ڴ�
    cJSON_free(json_str);
    cJSON_Delete(root);
}

/* �����ack */

void ack_large_dose_cjson_process(large_dose_information *p_large_dose_info, char *output_buffer, uint32_t buffer_size) {
    if (p_large_dose_info == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // ���� JSON ������
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // ���� "large_dose" ����
    cJSON *large_dose = cJSON_CreateObject();
    if (large_dose == NULL) {
        co_printf("Failed to create large_dose object\n");
        cJSON_Delete(root);
        return;
    }

    // ��� "large_dose_liquid"
    char liquid_str[16] = {0};
    snprintf(liquid_str, sizeof(liquid_str), "%.1f", p_large_dose_info->large_dose_liquid);
    cJSON_AddStringToObject(large_dose, "large_dose_liquid", liquid_str);

    // ��� "large_dose_speed"
    char speed_str[16] = {0};
    snprintf(speed_str, sizeof(speed_str), "%.1f", p_large_dose_info->large_dose_speed);
    cJSON_AddStringToObject(large_dose, "large_dose_speed", speed_str);

    // �� "large_dose" ��ӵ�������
    cJSON_AddItemToObject(root, "large_dose", large_dose);

    // ת��Ϊ JSON �ַ���
    char *json_data = cJSON_PrintUnformatted(root);
    if (json_data == NULL) {
        co_printf("Failed to create JSON string\n");
        cJSON_Delete(root);
        return;
    }

    // ��黺������С��ȷ�����������
    if (strlen(json_data) < buffer_size) {
        strncpy(output_buffer, json_data, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // ȷ���ַ����� NULL ��β
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }

    // ���� JSON �ַ����Ͷ����ڴ�
    cJSON_free(json_data);
    cJSON_Delete(root);
}













