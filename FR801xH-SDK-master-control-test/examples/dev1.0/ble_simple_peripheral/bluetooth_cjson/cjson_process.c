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

  /* 基础率数据指令  
   {
     "basal_rate":   
    {
     "basal_rate_num": "2",   //基础率段数，第几段
     "speed": "8.5",          //基础率速度
     "basal_rate_tim": "01:30-20:30" //基础率 起始时间 - 结束时间
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

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收的 JSON 数据
    co_printf("Received JSON: %s\n", buffer);

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 创建 basal_rate_information 结构体实例
  

    // 提取 "basal_rate" 对象
    cJSON *basal_rate = cJSON_GetObjectItemCaseSensitive(root, "basal_rate");
    if (basal_rate == NULL) {
        co_printf("basal_rate object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
		
	 s_pack_num.cjson_instruct_num = 1;
	
	}

    // 提取 "basal_rate_num"
    cJSON *basal_rate_num = cJSON_GetObjectItemCaseSensitive(basal_rate, "basal_rate_num");
    if (cJSON_IsString(basal_rate_num) && (basal_rate_num->valuestring != NULL)) {
        rate_info.basal_rate_num = atoi(basal_rate_num->valuestring);
    }

    // 提取 "speed"
    cJSON *speed = cJSON_GetObjectItemCaseSensitive(basal_rate, "speed");
    if (cJSON_IsString(speed) && (speed->valuestring != NULL)) {
        rate_info.basal_rate_speed = atof(speed->valuestring);
    }

    // 提取 "basal_rate_tim"
    cJSON *basal_rate_tim = cJSON_GetObjectItemCaseSensitive(basal_rate, "basal_rate_tim");
    if (cJSON_IsString(basal_rate_tim) && (basal_rate_tim->valuestring != NULL)) {
        // 分割时间段
        char start_time[6] = {0};
        char end_time[6] = {0};
        sscanf(basal_rate_tim->valuestring, "%5[^-]-%5s", start_time, end_time);

        // 分解起始时间
        sscanf(start_time, "%2hhd:%2hhd", &rate_info.basal_rate_start_tim_hh, &rate_info.basal_rate_start_tim_min);

        // 分解结束时间
        sscanf(end_time, "%2hhd:%2hhd", &rate_info.basal_rate_end_tim_hh, &rate_info.basal_rate_end_tim_min);
    }

    // 打印解析结果
    co_printf("Parsed basal_rate_information:\n");
    co_printf("  basal_rate_num: %d\n", rate_info.basal_rate_num);

    // 由于 co_printf 无法打印浮点数，这里扩大十倍打印
//    co_printf("  (float)basal_rate_speed * 10 : %d\n", (uint32_t)(rate_info.basal_rate_speed * 10));
	printf("basal_rate_speed : %f\n",rate_info.basal_rate_speed);
    co_printf("  Start time: %02d:%02d\n", rate_info.basal_rate_start_tim_hh, rate_info.basal_rate_start_tim_min);
    co_printf("  End time: %02d:%02d\n", rate_info.basal_rate_end_tim_hh, rate_info.basal_rate_end_tim_min);

    // 清理 JSON 内存
    cJSON_Delete(root);
}
   
  
  /************************************同步时间***********************************/  
/*
{
  "sync_tim": {      
    "now_tim": "12:20:30"
  }
}   
*/

void sync_tim_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on) {
	
    if (len == 0 || data == NULL) return;

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收到的 JSON 数据
    co_printf("Received JSON: %s\n", buffer);

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 创建 sync_tim 结构体实例
    

    // 提取 "sync_tim" 对象
    cJSON *sync_tim_obj = cJSON_GetObjectItemCaseSensitive(root, "sync_tim");
    if (sync_tim_obj == NULL) {
        co_printf("sync_tim object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
	
	 s_pack_num.cjson_instruct_num = 2;
		
	}

    // 提取 "now_tim"
    cJSON *now_tim = cJSON_GetObjectItemCaseSensitive(sync_tim_obj, "now_tim");
    if (cJSON_IsString(now_tim) && (now_tim->valuestring != NULL)) {
        // 分解时间信息
        sscanf(now_tim->valuestring, "%2hhd:%2hhd:%2hhd", 
               &current_time.sync_tim_hh, 
               &current_time.sync_tim_min, 
               &current_time.sync_tim_s);
    } else {
        co_printf("now_tim not found or invalid format!\n");
        cJSON_Delete(root);
        return;
    }

    // 打印解析结果
    co_printf("Parsed sync_tim:\n");
    co_printf("  Hours: %02d\n", current_time.sync_tim_hh);
    co_printf("  Minutes: %02d\n", current_time.sync_tim_min);
    co_printf("  Seconds: %02d\n", current_time.sync_tim_s);

    // 清理 JSON 内存
    cJSON_Delete(root);
}


//大剂量设定
//typedef struct large_dose{
//  	char large_dose_liquid; //大剂量液量
//	char large_dose_speed;  //大剂量速度
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

/* 大剂量设置 */
void large_dose_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on)
{
	 if (len == 0 || dbg_on == 0) return;

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收的 JSON 数据
    co_printf("Received JSON: %s\n", buffer);

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 提取 "large_dose" 对象
    cJSON *large_dose = cJSON_GetObjectItemCaseSensitive(root, "large_dose");
    if (large_dose == NULL) {
        co_printf("large_dose object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
	
	  s_pack_num.cjson_instruct_num = 3;
	}

    // 提取 "large_dose_liquid"
    cJSON *large_dose_liquid = cJSON_GetObjectItemCaseSensitive(large_dose, "large_dose_liquid");
    if (cJSON_IsString(large_dose_liquid) && (large_dose_liquid->valuestring != NULL)) {
        large_dose_info.large_dose_liquid = atof(large_dose_liquid->valuestring);
    }

    // 提取 "large_dose_speed"
    cJSON *large_dose_speed = cJSON_GetObjectItemCaseSensitive(large_dose, "large_dose_speed");
    if (cJSON_IsString(large_dose_speed) && (large_dose_speed->valuestring != NULL)) {
        large_dose_info.large_dose_speed = atof(large_dose_speed->valuestring);
    }

    // 打印解析结果
    co_printf("Parsed large_dose_information:\n");
	
	printf("  large_dose_liquid scaled=: %f\r\n", large_dose_info.large_dose_liquid);
    printf("  large_dose_speed scaled: %f\r\n", large_dose_info.large_dose_speed);

    // 清理 JSON 内存
    cJSON_Delete(root);
}


/*********************************ACK*************************************/

//基础率ACK
void ack_basal_rate_cjson_process(basal_rate_information *p_basal_rate_info, char *output_buffer, uint32_t buffer_size)
{	
    if (p_basal_rate_info == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // 创建 JSON 根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // 创建 "basal_rate" 对象
    cJSON *basal_rate = cJSON_CreateObject();
    if (basal_rate == NULL) {
        co_printf("Failed to create basal_rate object\n");
        cJSON_Delete(root);
        return;
    }

    // 添加 basal_rate_num
    cJSON_AddStringToObject(basal_rate, "basal_rate_num", 
                            (char[3]){p_basal_rate_info->basal_rate_num + '0', '\0'});
    
    // 添加 speed
    char speed_str[16];
    snprintf(speed_str, sizeof(speed_str), "%.1f", p_basal_rate_info->basal_rate_speed); // 正常处理速度
    cJSON_AddStringToObject(basal_rate, "speed", speed_str);

    // 添加 basal_rate_tim
    char time_range[32];
    snprintf(time_range, sizeof(time_range), 
             "%02d:%02d-%02d:%02d", 
             p_basal_rate_info->basal_rate_start_tim_hh, 
             p_basal_rate_info->basal_rate_start_tim_min, 
             p_basal_rate_info->basal_rate_end_tim_hh, 
             p_basal_rate_info->basal_rate_end_tim_min);
    cJSON_AddStringToObject(basal_rate, "basal_rate_tim", time_range);

    // 将 "basal_rate" 添加到根对象
    cJSON_AddItemToObject(root, "basal_rate", basal_rate);

    // 将 JSON 对象转换为字符串
    char *json_data = cJSON_PrintUnformatted(root);
    if (json_data == NULL) {
        co_printf("Failed to print JSON data\n");
        cJSON_Delete(root);
        return;
    }

    // 检查缓冲区大小，确保不发生溢出
    if (strlen(json_data) < buffer_size) {
        strncpy(output_buffer, json_data, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // 确保字符串以 NULL 结尾
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }

    // 清理 JSON 字符串和对象内存
    cJSON_free(json_data);
    cJSON_Delete(root);
}



void ack_sync_tim_cjson_process(sync_tim *p_sync_tim, char *output_buffer, uint32_t buffer_size) {
    if (p_sync_tim == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // 创建 JSON 根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // 创建 "sync_tim" 对象
    cJSON *sync_tim_obj = cJSON_CreateObject();
    if (sync_tim_obj == NULL) {
        co_printf("Failed to create sync_tim object\n");
        cJSON_Delete(root);
        return;
    }

    // 将结构体数据转换为字符串格式 " hh:mm:ss "
    char now_tim[10] = {0};
    snprintf(now_tim, sizeof(now_tim), "%02d:%02d:%02d",
             p_sync_tim->sync_tim_hh,
             p_sync_tim->sync_tim_min,
             p_sync_tim->sync_tim_s);

    // 添加字段到 JSON 对象
    cJSON_AddStringToObject(sync_tim_obj, "now_tim", now_tim);
    cJSON_AddItemToObject(root, "sync_tim", sync_tim_obj);

    // 转换为 JSON 字符串
    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        co_printf("Failed to create JSON string\n");
        cJSON_Delete(root);
        return;
    }

    // 检查缓冲区大小，确保不会溢出
    if (strlen(json_str) < buffer_size) {
        strncpy(output_buffer, json_str, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // 确保字符串以 NULL 结尾
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }

    // 清理 JSON 字符串和对象内存
    cJSON_free(json_str);
    cJSON_Delete(root);
}

/* 大剂量ack */

void ack_large_dose_cjson_process(large_dose_information *p_large_dose_info, char *output_buffer, uint32_t buffer_size) {
    if (p_large_dose_info == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // 创建 JSON 根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // 创建 "large_dose" 对象
    cJSON *large_dose = cJSON_CreateObject();
    if (large_dose == NULL) {
        co_printf("Failed to create large_dose object\n");
        cJSON_Delete(root);
        return;
    }

    // 添加 "large_dose_liquid"
    char liquid_str[16] = {0};
    snprintf(liquid_str, sizeof(liquid_str), "%.1f", p_large_dose_info->large_dose_liquid);
    cJSON_AddStringToObject(large_dose, "large_dose_liquid", liquid_str);

    // 添加 "large_dose_speed"
    char speed_str[16] = {0};
    snprintf(speed_str, sizeof(speed_str), "%.1f", p_large_dose_info->large_dose_speed);
    cJSON_AddStringToObject(large_dose, "large_dose_speed", speed_str);

    // 将 "large_dose" 添加到根对象
    cJSON_AddItemToObject(root, "large_dose", large_dose);

    // 转换为 JSON 字符串
    char *json_data = cJSON_PrintUnformatted(root);
    if (json_data == NULL) {
        co_printf("Failed to create JSON string\n");
        cJSON_Delete(root);
        return;
    }

    // 检查缓冲区大小，确保不发生溢出
    if (strlen(json_data) < buffer_size) {
        strncpy(output_buffer, json_data, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // 确保字符串以 NULL 结尾
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }

    // 清理 JSON 字符串和对象内存
    cJSON_free(json_data);
    cJSON_Delete(root);
}













